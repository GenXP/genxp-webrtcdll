#include "../WebRTC.h"
#include "WebRTCPlugin.h"
#include "Context.h"

#include "GXPAudioEncoderFactory.h"
#include "GXPAudioDecoderFactory.h"

#include "AudioTrackSinkAdapter.h"
#include "DummyVideoEncoder.h"
#include "SetSessionDescriptionObserver.h"
#include "GXPAudioTrackSource.h"

#include "../json.hpp"


using namespace ::webrtc;
using json = nlohmann::json;

namespace genxp
{
namespace webrtc
{

    ContextManager ContextManager::s_instance;

    Context* ContextManager::GetContext(int uid) const
    {
        auto it = s_instance.m_contexts.find(uid);
        if (it != s_instance.m_contexts.end()) {
            return it->second.get();
        }
        return nullptr;
    }

    Context* ContextManager::CreateContext(int uid, GXPEncoderType encoderType, bool forTest)
    {
        auto it = s_instance.m_contexts.find(uid);
        if (it != s_instance.m_contexts.end()) {
            DebugLog("Using already created context with ID %d", uid);
            return nullptr;
        }
        auto ctx = new Context(uid, encoderType, forTest);
        s_instance.m_contexts[uid].reset(ctx);
        return ctx;
    }

    void ContextManager::SetCurContext(Context* context)
    {
        curContext = context;
    }

    bool ContextManager::Exists(Context *context)
    {
        for(auto it = s_instance.m_contexts.begin(); it != s_instance.m_contexts.end(); ++it)
        {
            if(it->second.get() == context)
                return true;
        }
        return false;
    }

    void ContextManager::DestroyContext(int uid)
    {
        auto it = s_instance.m_contexts.find(uid);
        if (it != s_instance.m_contexts.end())
        {
            s_instance.m_contexts.erase(it);
            DebugLog("Unregistered context with ID %d", uid);
        }
    }

    ContextManager::~ContextManager()
    {
        if (m_contexts.size()) {
            DebugWarning("%lu remaining context(s) registered", m_contexts.size());
        }
        m_contexts.clear();
    }

#pragma region open an encode session
    uint32_t Context::s_encoderId = 0;
    uint32_t Context::GenerateUniqueId() { return s_encoderId++; }
#pragma endregion 

    bool Convert(const std::string& str, webrtc::PeerConnectionInterface::RTCConfiguration& config)
    {
        config = webrtc::PeerConnectionInterface::RTCConfiguration{};

        json configJson = json::parse(str.begin(), str.end());
        if (!configJson)
            return false;

        json iceServersJson = configJson["iceServers"];
        if (!iceServersJson)
            return false;

        //Parse ice servers cleanly in c++

        for (auto iceServerJson : iceServersJson)
        {
                webrtc::PeerConnectionInterface::IceServer iceServer;
                for (auto url : iceServerJson["urls"])
                {
                    iceServer.urls.push_back(url);
                }
                if (!iceServerJson["username"].is_null())
                {
                    iceServer.username = iceServerJson["username"];
                }
                if (!iceServerJson["credential"].is_null())
                {
                    iceServer.password = iceServerJson["credential"];
                }
                config.servers.push_back(iceServer);
        }


        json iceTransportPolicy = configJson["iceTransportPolicy"];
        if(iceTransportPolicy["hasValue"].is_boolean())
        {
            config.type = static_cast<PeerConnectionInterface::IceTransportsType>(iceTransportPolicy["value"]);
        }

        json enableDtlsSrtp = configJson["enableDtlsSrtp"];
        if (enableDtlsSrtp["hasValue"])
        {
            config.enable_dtls_srtp = enableDtlsSrtp["value"];
        }

        json iceCandidatePoolSize = configJson["iceCandidatePoolSize"];
        if (iceCandidatePoolSize["hasValue"])
        {
            config.ice_candidate_pool_size = iceCandidatePoolSize["value"];
        }


        json bundlePolicy = configJson["bundlePolicy"];
        if (bundlePolicy["hasValue"])
        {
            config.bundle_policy = static_cast<PeerConnectionInterface::BundlePolicy>(bundlePolicy["value"]);
        }

        config.sdp_semantics = webrtc::SdpSemantics::kUnifiedPlan;
        config.enable_implicit_rollback = true;
        return true;
    }
#pragma warning(push)
#pragma warning(disable: 4715)
    webrtc::SdpType ConvertSdpType(RTCSdpType type)
    {
        switch (type)
        {
        case RTCSdpType::Offer:
            return webrtc::SdpType::kOffer;
        case RTCSdpType::PrAnswer:
            return webrtc::SdpType::kPrAnswer;
        case RTCSdpType::Answer:
            return webrtc::SdpType::kAnswer;
        case RTCSdpType::Rollback:
            return webrtc::SdpType::kRollback;
        }
        throw std::invalid_argument("Unknown RTCSdpType");
    }

    RTCSdpType ConvertSdpType(webrtc::SdpType type)
    {
        switch (type)
        {
        case webrtc::SdpType::kOffer:
            return RTCSdpType::Offer;
        case webrtc::SdpType::kPrAnswer:
            return RTCSdpType::PrAnswer;
        case webrtc::SdpType::kAnswer:
            return RTCSdpType::Answer;
        case webrtc::SdpType::kRollback:
            return RTCSdpType::Rollback;
        default:
            throw std::invalid_argument("Unknown SdpType");
        }
    }
#pragma warning(pop)

    Context::Context(int uid, GXPEncoderType encoderType, bool forTest)
        : m_uid(uid)
        , m_encoderType(encoderType)
    {
        // init WebRTC networking and inter-thread communication
        rtc::WinsockInitializer();
        rtc::Win32SocketServer SocketServer;
        rtc::Win32Thread W32Thread(&SocketServer);
        rtc::ThreadManager::Instance()->SetCurrentThread(&W32Thread);

        rtc::InitializeSSL();

        m_workerThread->Invoke<void>(
            RTC_FROM_HERE,
            [this]()
            {
                m_audioDevice = new rtc::RefCountedObject<DummyAudioDevice>();
            });

        rtc::scoped_refptr<AudioEncoderFactory> audioEncoderFactory = CreateAudioEncoderFactory();
        rtc::scoped_refptr<AudioDecoderFactory>  audioDecoderFactory = CreateAudioDecoderFactory();

        m_peerConnectionFactory = CreatePeerConnectionFactory(
                                nullptr,
                                nullptr,
                                nullptr,
                                m_audioDevice,
                                audioEncoderFactory,
                                audioDecoderFactory,
                                nullptr, //video factory
                                nullptr, //video factory
                                nullptr,
                                nullptr);
    }

    Context::~Context()
    {
        {
            std::lock_guard<std::mutex> lock(mutex);

            m_peerConnectionFactory = nullptr;
            m_workerThread->Invoke<void>(
                RTC_FROM_HERE,
                [this]()
                {
                    m_audioDevice = nullptr;
                });
            m_mapClients.clear();

            // check count of refptr to avoid to forget disposing
            RTC_DCHECK_EQ(m_mapRefPtr.size(), 0);

            m_mapRefPtr.clear();
            m_mapMediaStreamObserver.clear();
            m_mapSetSessionDescriptionObserver.clear();
            m_mapDataChannels.clear();

            m_workerThread->Quit();
            m_workerThread.reset();
            m_signalingThread->Quit();
            m_signalingThread.reset();
        }
    }

    GXPEncoderType Context::GetEncoderType() const
    {
        return m_encoderType;
    }

    webrtc::MediaStreamInterface* Context::CreateMediaStream(const std::string& streamId)
    {
        rtc::scoped_refptr<webrtc::MediaStreamInterface> stream =
            m_peerConnectionFactory->CreateLocalMediaStream(streamId);
        AddRefPtr(stream);
        return stream;
    }

    MediaStreamObserver* Context::GetObserver(
        const webrtc::MediaStreamInterface* stream)
    {
        return m_mapMediaStreamObserver[stream].get();
    }

    void Context::StopMediaStreamTrack(webrtc::MediaStreamTrackInterface* track)
    {
    }

    webrtc::AudioSourceInterface* Context::CreateAudioSource()
    {
        //avoid optimization specially for voice
        cricket::AudioOptions audioOptions;
        audioOptions.auto_gain_control = false;
        audioOptions.noise_suppression = false;
        audioOptions.highpass_filter = false;

        const rtc::scoped_refptr<GXPAudioTrackSource> source =
            GXPAudioTrackSource::Create(audioOptions);

        AddRefPtr(source);
        return source;
    }

    AudioTrackInterface* Context::CreateAudioTrack(const std::string& label, webrtc::AudioSourceInterface* source)
    {
        const rtc::scoped_refptr<AudioTrackInterface> track =
            m_peerConnectionFactory->CreateAudioTrack(label, source);

        AddRefPtr(track);
        return track;
    }


    void Context::RegisterAudioReceiveCallback(
        AudioTrackInterface* track, DelegateAudioReceive callback)
    {
        if (m_mapAudioTrackAndSink.find(track) != m_mapAudioTrackAndSink.end())
        {
            RTC_LOG(LS_WARNING) << "The callback is already registered.";
            return;
        }

        std::unique_ptr<AudioTrackSinkAdapter> sink =
            std::make_unique<AudioTrackSinkAdapter>(track, callback);

        track->AddSink(sink.get());
        m_mapAudioTrackAndSink[track] = std::move(sink);
    }

    void Context::UnregisterAudioReceiveCallback(
        AudioTrackInterface* track)
    {
        if (m_mapAudioTrackAndSink.find(track) == m_mapAudioTrackAndSink.end())
            return;

        AudioTrackSinkInterface* sink = m_mapAudioTrackAndSink[track].get();
        track->RemoveSink(sink);
        m_mapAudioTrackAndSink.erase(track);
    }

    void Context::AddStatsReport(const rtc::scoped_refptr<const webrtc::RTCStatsReport>& report)
    {
        m_listStatsReport.push_back(report);
    }

    void Context::DeleteStatsReport(const webrtc::RTCStatsReport* report)
    {
        auto found = std::find_if(m_listStatsReport.begin(), m_listStatsReport.end(),
             [report](rtc::scoped_refptr<const webrtc::RTCStatsReport> it){ return it.get() == report; });
        m_listStatsReport.erase(found);
	}

    DataChannelObject* Context::CreateDataChannel(PeerConnectionObject* obj, const char* label, const DataChannelInit& options)
    {
        auto channel = obj->connection->CreateDataChannel(label, &options);
        if (channel == nullptr)
            return nullptr;
        auto dataChannelObj = std::make_unique<DataChannelObject>(channel, *obj);
        DataChannelObject* ptr = dataChannelObj.get();
        m_mapDataChannels[ptr] = std::move(dataChannelObj);
        return ptr;
    }

    void Context::AddDataChannel(std::unique_ptr<DataChannelObject> channel) {
        const auto ptr = channel.get();
        m_mapDataChannels[ptr] = std::move(channel);
    }

    void Context::DeleteDataChannel(DataChannelObject* obj)
    {
        if (m_mapDataChannels.count(obj) > 0)
        {
            m_mapDataChannels.erase(obj);
        }
    }

    void Context::AddObserver(const webrtc::PeerConnectionInterface* connection, const rtc::scoped_refptr<SetSessionDescriptionObserver>& observer)
    {
        m_mapSetSessionDescriptionObserver[connection] = observer;
    }

    void Context::RemoveObserver(const webrtc::PeerConnectionInterface* connection)
    {
        m_mapSetSessionDescriptionObserver.erase(connection);
    }

    PeerConnectionObject* Context::CreatePeerConnection(
            const webrtc::PeerConnectionInterface::RTCConfiguration& config)
    {
        rtc::scoped_refptr<PeerConnectionObject> obj =
                new rtc::RefCountedObject<PeerConnectionObject>(*this);
        PeerConnectionDependencies dependencies(obj);
        obj->connection = m_peerConnectionFactory->CreatePeerConnection(
                config, std::move(dependencies));
        if (obj->connection == nullptr)
            return nullptr;
        const PeerConnectionObject* ptr = obj.get();
        m_mapClients[ptr] = std::move(obj);
        return m_mapClients[ptr].get();
    }

    void Context::DeletePeerConnection(PeerConnectionObject *obj)
    {
        m_mapClients.erase(obj);
    }

    SetSessionDescriptionObserver* Context::GetObserver(webrtc::PeerConnectionInterface* connection)
    {
        return m_mapSetSessionDescriptionObserver[connection];
    }

    uint32_t Context::s_rendererId = 0;
    uint32_t Context::GenerateRendererId() { return s_rendererId++; }

    void Context::GetRtpSenderCapabilities(
        cricket::MediaType kind, RtpCapabilities* capabilities) const
    {
        *capabilities = m_peerConnectionFactory->GetRtpSenderCapabilities(kind);
    }

    void Context::GetRtpReceiverCapabilities(
        cricket::MediaType kind, RtpCapabilities* capabilities) const
    {
        *capabilities = m_peerConnectionFactory->GetRtpReceiverCapabilities(kind);
    }

} // end namespace webrtc
} // end namespace genxp
