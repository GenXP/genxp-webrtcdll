#pragma once

#include "WebRTCAPI.h"

#include "../WebRTC.h"
#include "WebRTCPlugin.h"
#include "PeerConnectionObject.h"
#include "SetSessionDescriptionObserver.h"
#include "GXPLogStream.h"
#include "Context.h"
#include "GXPAudioTrackSource.h"
#include <combaseapi.h>

#pragma warning( disable : 4244 )

namespace genxp
{
    namespace webrtc
    {
        template<class T>
        T** ConvertPtrArrayFromRefPtrArray(
            std::vector<rtc::scoped_refptr<T>> vec, size_t* length)
        {
            *length = vec.size();
            const auto buf = CoTaskMemAlloc(sizeof(T*) * vec.size());
            const auto ret = static_cast<T**>(buf);
            std::copy(vec.begin(), vec.end(), ret);
            return ret;
        }

        template<typename T>
        T* ConvertArray(std::vector<T> vec, size_t* length)
        {
            *length = vec.size();
            size_t size = sizeof(T*) * vec.size();
            auto dst = CoTaskMemAlloc(size);
            auto src = vec.data();
            std::memcpy(dst, src, size);
            return static_cast<T*>(dst);
        }

        template<typename T>
        struct MarshallArray
        {
            int32_t length;
            T* values;

            T& operator[](intptr_t i) const
            {
                return values[i];
            }

            template<typename U>
            MarshallArray& operator=(const std::vector<U>& src)
            {
                length = static_cast<uint32_t>(src.size());
                values = static_cast<T*>(CoTaskMemAlloc(sizeof(T) * src.size()));

                for (size_t i = 0; i < src.size(); i++)
                {
                    values[i] = src[i];
                }
                return *this;
            }
        };

        template<typename T, typename U>
        void ConvertArray(const MarshallArray<T>& src, std::vector<U>& dst)
        {
            dst.resize(src.length);
            for (size_t i = 0; i < dst.size(); i++)
            {
                dst[i] = src.values[i];
            }
        }

        template<typename T>
        struct Optional
        {
            bool hasValue;
            T value;

            template<typename U>
            Optional& operator=(const absl::optional<U>& src)
            {
                hasValue = src.has_value();
                if (hasValue)
                {
                    value = static_cast<T>(src.value());
                }
                else
                {
                    value = T();
                }
                return *this;
            }

            explicit operator const absl::optional<T>& () const
            {
                absl::optional<T> dst = absl::nullopt;
                if (hasValue)
                    dst = value;
                return dst;
            }

            const T& value_or(const T& v) const
            {
                return hasValue ? value : v;
            }
        };

        template<typename T>
        absl::optional<T> ConvertOptional(const Optional<T>& value)
        {
            absl::optional<T> dst = absl::nullopt;
            if (value.hasValue)
            {
                dst = value.value;
            }
            return dst;
        }

        std::string ConvertSdp(const std::map<std::string, std::string>& map)
        {
            std::string str = "";
            for (const auto& pair : map)
            {
                if (!str.empty())
                {
                    str += ";";
                }
                str += pair.first + "=" + pair.second;
            }
            return str;
        }

        std::vector<std::string> Split(const std::string& str, const std::string& delimiter)
        {
            std::vector<std::string> dst;
            std::string s = str;
            size_t pos = 0;
            while (true)
            {
                pos = s.find(delimiter);
                intptr_t length = pos;
                if (pos == std::string::npos)
                    length = str.length();
                if (length == 0)
                    break;
                dst.push_back(s.substr(0, length));
                if (pos == std::string::npos)
                    break;
                s.erase(0, pos + delimiter.length());
            }
            return dst;
        }

        std::tuple<cricket::MediaType, std::string> ConvertMimeType(const std::string& mimeType)
        {
            const std::vector<std::string> vec = Split(mimeType, "/");
            const std::string kind = vec[0];
            const std::string name = vec[1];
            cricket::MediaType mediaType;
            if (kind == "video")
            {
                mediaType = cricket::MEDIA_TYPE_VIDEO;
            }
            else if (kind == "audio")
            {
                mediaType = cricket::MEDIA_TYPE_AUDIO;
            }
            return { mediaType, name };
        }

        std::map<std::string, std::string> ConvertSdp(const std::string& src)
        {
            std::map<std::string, std::string> map;
            std::vector<std::string> vec = Split(src, ";");

            for (const auto& str : vec)
            {
                std::vector<std::string> pair = Split(str, "=");
                map.emplace(pair[0], pair[1]);
            }
            return map;
        }

        ///
        /// avoid compile error for vector<bool>
        /// https://en.cppreference.com/w/cpp/container/vector_bool
        bool* ConvertArray(std::vector<bool> vec, size_t* length)
        {
            *length = vec.size();
            size_t size = sizeof(bool*) * vec.size();
            auto dst = CoTaskMemAlloc(size);
            bool* ret = static_cast<bool*>(dst);
            for (size_t i = 0; i < vec.size(); i++)
            {
                ret[i] = vec[i];
            }
            return ret;
        }

        char* ConvertString(const std::string str)
        {
            const size_t size = str.size();
            char* ret = static_cast<char*>(CoTaskMemAlloc(size + sizeof(char)));
            str.copy(ret, size);
            ret[size] = '\0';
            return ret;
        }
    }
}


using namespace genxp::webrtc;
using namespace ::webrtc;
using DelegateDebugLog = void(*)(const char*);
void debugLog(const char* buf);

GXP_EXPORT intptr_t _stdcall ContextCreateMediaStream(intptr_t context, const char* streamId)
{
    Context* c = (Context*)context;
    MediaStreamInterface* m = c->CreateMediaStream(streamId);
    return (intptr_t)m;
}

GXP_EXPORT void _stdcall ContextStopMediaStreamTrack(intptr_t context, intptr_t track)
{
    Context* c = (Context*)context;
    ::webrtc::MediaStreamTrackInterface* t = (::webrtc::MediaStreamTrackInterface*)track;
    c->StopMediaStreamTrack(t);
}

GXP_EXPORT intptr_t _stdcall ContextCreateAudioTrackSource(intptr_t context)
{
    Context* c = (Context*)context;
    return (intptr_t)c->CreateAudioSource();
}

GXP_EXPORT intptr_t _stdcall ContextCreateAudioTrack(intptr_t context, const char* label, intptr_t source)
{
    Context* c = (Context*)context;
    webrtc::AudioSourceInterface* s = (webrtc::AudioSourceInterface*)source;
    return (intptr_t)c->CreateAudioTrack(label, s);
}

GXP_EXPORT void _stdcall ContextAddRefPtr(intptr_t context, intptr_t ptr)
{
    Context* c = (Context*)context;
    rtc::RefCountInterface* r = (rtc::RefCountInterface*)ptr;
    c->AddRefPtr(r);
}

GXP_EXPORT void _stdcall ContextDeleteRefPtr(intptr_t context, intptr_t ptr)
{
    Context* c = (Context*)context;
    rtc::RefCountInterface* r = (rtc::RefCountInterface*)ptr;
    c->RemoveRefPtr(r);
}

GXP_EXPORT bool _stdcall MediaStreamAddTrack(intptr_t stream, intptr_t track)
{
    MediaStreamInterface* s = (MediaStreamInterface*)stream;
    MediaStreamTrackInterface* t = (MediaStreamTrackInterface*)track;

    if (t->kind() == "audio")
    {
        return s->AddTrack(static_cast<AudioTrackInterface*>(t));
    }

    return false;
}

GXP_EXPORT bool _stdcall MediaStreamRemoveTrack(intptr_t stream, intptr_t track)
{
    MediaStreamInterface* s = (MediaStreamInterface*)stream;
    MediaStreamTrackInterface* t = (MediaStreamTrackInterface*)track;

    if (t->kind() == "audio")
    {
        return s->RemoveTrack(static_cast<AudioTrackInterface*>(t));
    }

    return false;
}

GXP_EXPORT char* _stdcall MediaStreamGetID(intptr_t stream)
{
    MediaStreamInterface* s = (MediaStreamInterface*)stream;
    return ConvertString(s->id());
}

GXP_EXPORT void* _stdcall MediaStreamGetAudioTracks(intptr_t stream, size_t* length)
{
    MediaStreamInterface* s = (MediaStreamInterface*)stream;
    return (void*)ConvertPtrArrayFromRefPtrArray<AudioTrackInterface>(s->GetAudioTracks(), length);
}

GXP_EXPORT intptr_t _stdcall MediaStreamTrackGetKind(intptr_t track)
{
    MediaStreamTrackInterface* t = (MediaStreamTrackInterface*)track;
    const auto kindStr = t->kind();
    if (kindStr == "audio")
    {
        return (intptr_t)TrackKind::Audio;
    }
    else
    {
        return (intptr_t)TrackKind::Video;
    }
}

GXP_EXPORT intptr_t _stdcall MediaStreamTrackGetReadyState(intptr_t track)
{
    MediaStreamTrackInterface* t = (MediaStreamTrackInterface*)track;
    return (intptr_t)t->state();
}

GXP_EXPORT char* _stdcall MediaStreamTrackGetID(intptr_t track)
{
    MediaStreamTrackInterface* t = (MediaStreamTrackInterface*)track;
    return ConvertString(t->id());
}

GXP_EXPORT bool _stdcall MediaStreamTrackGetEnabled(intptr_t track)
{
    MediaStreamTrackInterface* t = (MediaStreamTrackInterface*)track;
    return t->enabled();
}

GXP_EXPORT void _stdcall MediaStreamTrackSetEnabled(intptr_t track, bool enabled)
{
    MediaStreamTrackInterface* t = (MediaStreamTrackInterface*)track;
    t->set_enabled(enabled);
}

GXP_EXPORT void _stdcall RegisterDebugLog(intptr_t func, bool enableNativeLog, int loggingSeverity)
{
    delegateDebugLog = (DelegateDebugLog)func;
    if ((DelegateDebugLog)func != nullptr && enableNativeLog)
    {
        GXPLogStream::AddLogStream((DelegateDebugLog)func, (rtc::LoggingSeverity)loggingSeverity);
    }
    else if ((DelegateDebugLog)func == nullptr)
    {
        GXPLogStream::RemoveLogStream();
    }
}

GXP_EXPORT intptr_t _stdcall ContextCreate(int32_t uid, int encoderType, bool forTest)
{
    auto ctx = ContextManager::GetInstance()->GetContext(uid);
    if (ctx != nullptr)
    {
        DebugLog("Already created context with ID %d", uid);
        return (intptr_t)ctx;
    }
    ctx = ContextManager::GetInstance()->CreateContext(uid, (GXPEncoderType)encoderType, forTest);
    return (intptr_t)ctx;
}

GXP_EXPORT void _stdcall ContextDestroy(int32_t uid)
{
    ContextManager::GetInstance()->DestroyContext(uid);
}

PeerConnectionObject* _ContextCreatePeerConnection(
    Context* context, const PeerConnectionInterface::RTCConfiguration& config)
{
    const auto obj = context->CreatePeerConnection(config);
    if (obj == nullptr)
        return nullptr;
    const auto observer = genxp::webrtc::SetSessionDescriptionObserver::Create(obj);
    context->AddObserver(obj->connection, observer);
    return obj;
}

GXP_EXPORT intptr_t _stdcall ContextCreatePeerConnection(intptr_t context)
{
    Context* c = (Context*)context;
    PeerConnectionInterface::RTCConfiguration config;
    config.sdp_semantics = SdpSemantics::kUnifiedPlan;
    config.enable_implicit_rollback = true;
    return (intptr_t)_ContextCreatePeerConnection(c, config);
}

GXP_EXPORT intptr_t _stdcall ContextCreatePeerConnectionWithConfig(intptr_t context, const char* conf)
{
    Context* c = (Context*)context;
    PeerConnectionInterface::RTCConfiguration config;
    if (!Convert(conf, config))
        return (intptr_t)nullptr;

    config.sdp_semantics = SdpSemantics::kUnifiedPlan;
    config.enable_implicit_rollback = true;
    return (intptr_t)_ContextCreatePeerConnection(c, config);
}

GXP_EXPORT void _stdcall ContextDeletePeerConnection(intptr_t context, intptr_t obj)
{
    Context* c = (Context*)context;
    PeerConnectionObject* o = (PeerConnectionObject*)obj;

    o->Close();
    c->RemoveObserver(o->connection);
    c->DeletePeerConnection(o);
}

GXP_EXPORT void _stdcall PeerConnectionClose(intptr_t obj)
{
    PeerConnectionObject* o = (PeerConnectionObject*)obj;
    o->Close();
}

GXP_EXPORT void _stdcall PeerConnectionRestartIce(intptr_t obj)
{
    PeerConnectionObject* o = (PeerConnectionObject*)obj;
    o->connection->RestartIce();
}

GXP_EXPORT intptr_t _stdcall PeerConnectionAddTrack(
    intptr_t obj, intptr_t track, const char* streamId, intptr_t sender)
{
    PeerConnectionObject* o = (PeerConnectionObject*)obj;
    MediaStreamTrackInterface* t = (MediaStreamTrackInterface*)track;
    RtpSenderInterface** s = (RtpSenderInterface**)sender;
    auto result = o->connection->AddTrack(rtc::scoped_refptr <MediaStreamTrackInterface>(t), { streamId });
    if (result.ok())
    {
        *s = result.value();
    }
    return (intptr_t)result.error().type();

}

GXP_EXPORT void* _stdcall PeerConnectionAddTransceiver(
    intptr_t context, intptr_t obj, intptr_t track)
{
    PeerConnectionObject* o = (PeerConnectionObject*)obj;
    MediaStreamTrackInterface* t = (MediaStreamTrackInterface*)track;
    Context* c = (Context*)context;

    auto result = o->connection->AddTransceiver(t);
    if (!result.ok())
        return nullptr;

    return result.value(); //TODO

}

GXP_EXPORT void* _stdcall PeerConnectionAddTransceiverWithInit(
    intptr_t context, intptr_t obj, intptr_t track, intptr_t init)
{
    PeerConnectionObject* o = (PeerConnectionObject*)obj;
    MediaStreamTrackInterface* t = (MediaStreamTrackInterface*)track;
    RtpTransceiverInit* i = (RtpTransceiverInit*)init;

    auto result = o->connection->AddTransceiver(t, *i);
    if (!result.ok())
        return nullptr;

    return result.value();
}

GXP_EXPORT void* _stdcall PeerConnectionAddTransceiverWithType(
    intptr_t context, intptr_t obj, int type)
{
    PeerConnectionObject* o = (PeerConnectionObject*)obj;
    Context* c = (Context*)context;
    cricket::MediaType t = (cricket::MediaType)type;

    auto result = o->connection->AddTransceiver(t);
    if (!result.ok())
        return nullptr;

    return result.value();
}

GXP_EXPORT void* _stdcall PeerConnectionAddTransceiverWithTypeAndInit(
    intptr_t context, intptr_t obj, int type, intptr_t init)
{
    PeerConnectionObject* o = (PeerConnectionObject*)obj;
    Context* c = (Context*)context;
    cricket::MediaType t = (cricket::MediaType)type;
    RtpTransceiverInit* i = (RtpTransceiverInit*)init;

    auto result = o->connection->AddTransceiver(t, *i);
    if (!result.ok())
        return nullptr;

    return result.value();
}

GXP_EXPORT intptr_t _stdcall PeerConnectionRemoveTrack(intptr_t obj, intptr_t sender)
{
    PeerConnectionObject* o = (PeerConnectionObject*)obj;
    RtpSenderInterface* s = (RtpSenderInterface*)sender;

    webrtc::RTCError error = o->connection->RemoveTrackNew(s);
    return (intptr_t)error.type();
}

GXP_EXPORT intptr_t _stdcall PeerConnectionSetConfiguration(intptr_t obj, const char* conf)
{
    PeerConnectionObject* o = (PeerConnectionObject*)obj;
    return (intptr_t)o->SetConfiguration(std::string(conf));
}

GXP_EXPORT char* _stdcall PeerConnectionGetConfiguration(intptr_t obj)
{
    PeerConnectionObject* o = (PeerConnectionObject*)obj;
    const std::string str = o->GetConfiguration();
    return ConvertString(str);
}

GXP_EXPORT void _stdcall PeerConnectionGetStats(intptr_t obj)
{
    PeerConnectionObject* o = (PeerConnectionObject*)obj;
    o->connection->GetStats(PeerConnectionStatsCollectorCallback::Create(o));
}

GXP_EXPORT void _stdcall PeerConnectionSenderGetStats(intptr_t obj, intptr_t selector)
{
    PeerConnectionObject* o = (PeerConnectionObject*)obj;
    RtpSenderInterface* s = (RtpSenderInterface*)selector;

    o->connection->GetStats(s, PeerConnectionStatsCollectorCallback::Create(o));
}

GXP_EXPORT void _stdcall PeerConnectionReceiverGetStats(intptr_t obj, intptr_t receiver)
{
    PeerConnectionObject* o = (PeerConnectionObject*)obj;
    RtpSenderInterface* r = (RtpSenderInterface*)receiver;

    o->connection->GetStats(r, PeerConnectionStatsCollectorCallback::Create(o));
}

const std::map<std::string, byte> statsTypes =
{
    { "codec", 0 },
    { "inbound-rtp", 1 },
    { "outbound-rtp", 2 },
    { "remote-inbound-rtp", 3 },
    { "remote-outbound-rtp", 4 },
    { "media-source", 5 },
    { "csrc", 6 },
    { "peer-connection", 7 },
    { "data-channel", 8 },
    { "stream", 9 },
    { "track", 10 },
    { "transceiver", 11 },
    { "sender", 12 },
    { "receiver", 13 },
    { "transport", 14 },
    { "sctp-transport", 15 },
    { "candidate-pair", 16 },
    { "local-candidate", 17 },
    { "remote-candidate", 18 },
    { "certificate", 19 },
    { "ice-server", 20 }
};

GXP_EXPORT const intptr_t _stdcall StatsReportGetStatsList(const intptr_t report, size_t* length, unsigned int** types)
{
    const RTCStatsReport* r = (const RTCStatsReport*)report;

    const size_t size = r->size();
    *length = size;
    *types = static_cast<unsigned int*>(CoTaskMemAlloc(sizeof(unsigned int) * size));
    void* buf = CoTaskMemAlloc(sizeof(RTCStats*) * size);
    const RTCStats** ret = static_cast<const RTCStats**>(buf);
    if (size == 0)
    {
        return (intptr_t)ret;
    }
    int i = 0;
    for (const auto& stats : *r)
    {
        ret[i] = &stats;
        (*types)[i] = statsTypes.at(stats.type());
        i++;
    }
    return (intptr_t)ret;
}

GXP_EXPORT void _stdcall ContextDeleteStatsReport(intptr_t context, const intptr_t report)
{
    Context* c = (Context*)context;
    const RTCStatsReport* r = (const RTCStatsReport*)report;

    c->DeleteStatsReport(r);
}

GXP_EXPORT const char* _stdcall StatsGetJson(const intptr_t stats)
{
    const RTCStats* s = (const RTCStats*)stats;
    return ConvertString(s->ToJson());
}

GXP_EXPORT intptr_t _stdcall StatsGetTimestamp(const intptr_t stats)
{
    const RTCStats* s = (const RTCStats*)stats;
    return s->timestamp_us();
}

GXP_EXPORT const char* _stdcall StatsGetId(const intptr_t stats)
{
    const RTCStats* s = (const RTCStats*)stats;
    return ConvertString(s->id());
}

GXP_EXPORT unsigned int _stdcall StatsGetType(const intptr_t stats)
{
    const RTCStats* s = (const RTCStats*)stats;
    return statsTypes.at(s->type());
}

GXP_EXPORT const intptr_t _stdcall StatsGetMembers(const intptr_t stats, size_t* length)
{
    const RTCStats* s = (const RTCStats*)stats;
    return (const intptr_t)ConvertArray(s->Members(), length);
}

GXP_EXPORT bool _stdcall StatsMemberIsDefined(const intptr_t member)
{
    const RTCStatsMemberInterface* m = (const RTCStatsMemberInterface*)member;

    return m->is_defined();
}

GXP_EXPORT const char* _stdcall StatsMemberGetName(const intptr_t member)
{
    const RTCStatsMemberInterface* m = (const RTCStatsMemberInterface*)member;

    return ConvertString(std::string(m->name()));
}

GXP_EXPORT bool _stdcall StatsMemberGetBool(const intptr_t member)
{
    const RTCStatsMemberInterface* m = (const RTCStatsMemberInterface*)member;

    return *m->cast_to<::webrtc::RTCStatsMember<bool>>();
}

GXP_EXPORT intptr_t _stdcall StatsMemberGetInt(const intptr_t member)
{
    const RTCStatsMemberInterface* m = (const RTCStatsMemberInterface*)member;

    return *m->cast_to<::webrtc::RTCStatsMember<int>>();
}

GXP_EXPORT unsigned int _stdcall StatsMemberGetUnsignedInt(const intptr_t member)
{
    const RTCStatsMemberInterface* m = (const RTCStatsMemberInterface*)member;

    return *m->cast_to<::webrtc::RTCStatsMember<unsigned int>>();
}

GXP_EXPORT long long _stdcall StatsMemberGetLong(const intptr_t member)
{
    const RTCStatsMemberInterface* m = (const RTCStatsMemberInterface*)member;

    return *m->cast_to<::webrtc::RTCStatsMember<long long>>();
}

GXP_EXPORT unsigned long long _stdcall StatsMemberGetUnsignedLong(const intptr_t member)
{
    const RTCStatsMemberInterface* m = (const RTCStatsMemberInterface*)member;

    return *m->cast_to<::webrtc::RTCStatsMember<unsigned long long>>();
}

GXP_EXPORT double StatsMemberGetDouble(const intptr_t member)
{
    const RTCStatsMemberInterface* m = (const RTCStatsMemberInterface*)member;

    return *m->cast_to<::webrtc::RTCStatsMember<double>>();
}

GXP_EXPORT const char* StatsMemberGetString(const intptr_t member)
{
    const RTCStatsMemberInterface* m = (const RTCStatsMemberInterface*)member;

    return ConvertString(m->ValueToString());
}

GXP_EXPORT bool* StatsMemberGetBoolArray(const intptr_t member, size_t* length)
{
    const RTCStatsMemberInterface* m = (const RTCStatsMemberInterface*)member;

    return ConvertArray(*m->cast_to<::webrtc::RTCStatsMember<std::vector<bool>>>(), length);
}

GXP_EXPORT int32_t* StatsMemberGetIntArray(const intptr_t member, size_t* length)
{
    const RTCStatsMemberInterface* m = (const RTCStatsMemberInterface*)member;

    return ConvertArray(*m->cast_to<::webrtc::RTCStatsMember<std::vector<int>>>(), length);
}

GXP_EXPORT unsigned int* StatsMemberGetUnsignedIntArray(const intptr_t member, size_t* length)
{
    const RTCStatsMemberInterface* m = (const RTCStatsMemberInterface*)member;

    return ConvertArray(*m->cast_to<::webrtc::RTCStatsMember<std::vector<unsigned int>>>(), length);
}

GXP_EXPORT long long* StatsMemberGetLongArray(const intptr_t member, size_t* length)
{
    const RTCStatsMemberInterface* m = (const RTCStatsMemberInterface*)member;

    return ConvertArray(*m->cast_to<::webrtc::RTCStatsMember<std::vector<long long>>>(), length);
}

GXP_EXPORT unsigned long long* StatsMemberGetUnsignedLongArray(const intptr_t member, size_t* length)
{
    const RTCStatsMemberInterface* m = (const RTCStatsMemberInterface*)member;

    return ConvertArray(*m->cast_to<::webrtc::RTCStatsMember<std::vector<unsigned long long>>>(), length);
}

GXP_EXPORT double* StatsMemberGetDoubleArray(const intptr_t member, size_t* length)
{
    const RTCStatsMemberInterface* m = (const RTCStatsMemberInterface*)member;

    return ConvertArray(*m->cast_to<::webrtc::RTCStatsMember<std::vector<double>>>(), length);
}

GXP_EXPORT const char** StatsMemberGetStringArray(const intptr_t member, size_t* length)
{
    const RTCStatsMemberInterface* m = (const RTCStatsMemberInterface*)member;

    std::vector<std::string> vec = *m->cast_to<::webrtc::RTCStatsMember<std::vector<std::string>>>();
    std::vector<const char*>  vc;
    std::transform(vec.begin(), vec.end(), std::back_inserter(vc), ConvertString);
    return ConvertArray(vc, length);
}

GXP_EXPORT intptr_t _stdcall StatsMemberGetType(const intptr_t member)
{
    const RTCStatsMemberInterface* m = (const RTCStatsMemberInterface*)member;

    return m->type();
}

GXP_EXPORT intptr_t _stdcall PeerConnectionSetLocalDescription(
    intptr_t context, intptr_t obj, const intptr_t desc, char* error[])
{
    Context* c = (Context*)context;
    PeerConnectionObject* o = (PeerConnectionObject*)obj;
    const RTCSessionDescription* d = (const RTCSessionDescription*)desc;

    std::string error_;
    RTCErrorType errorType = o->SetLocalDescription(
        *d, c->GetObserver(o->connection), error_);
    *error = ConvertString(error_);
    return (intptr_t)errorType;
}

GXP_EXPORT intptr_t _stdcall PeerConnectionSetLocalDescriptionWithoutDescription(intptr_t context, intptr_t obj, char* error[])
{
    Context* c = (Context*)context;
    PeerConnectionObject* o = (PeerConnectionObject*)obj;

    std::string error_;
    RTCErrorType errorType = o->SetLocalDescriptionWithoutDescription(c->GetObserver(o->connection), error_);
    *error = ConvertString(error_);
    return (intptr_t)errorType;
}

GXP_EXPORT intptr_t _stdcall PeerConnectionSetRemoteDescription(
    intptr_t context, intptr_t obj, const intptr_t desc, char* error[])
{
    Context* c = (Context*)context;
    PeerConnectionObject* o = (PeerConnectionObject*)obj;
    const RTCSessionDescription* d = (const RTCSessionDescription*)desc;

    std::string error_;
    RTCErrorType errorType = o->SetRemoteDescription(
        *d, c->GetObserver(o->connection), error_);
    *error = ConvertString(error_);
    return (intptr_t)errorType;
}

GXP_EXPORT bool _stdcall PeerConnectionGetLocalDescription(intptr_t obj, intptr_t desc)
{
    PeerConnectionObject* o = (PeerConnectionObject*)obj;
    RTCSessionDescription* d = (RTCSessionDescription*)desc;

    return o->GetSessionDescription(o->connection->local_description(), *d);
}

GXP_EXPORT bool _stdcall PeerConnectionGetRemoteDescription(intptr_t obj, intptr_t desc)
{
    PeerConnectionObject* o = (PeerConnectionObject*)obj;
    RTCSessionDescription* d = (RTCSessionDescription*)desc;

    return o->GetSessionDescription(o->connection->remote_description(), *d);
}

GXP_EXPORT bool _stdcall PeerConnectionGetPendingLocalDescription(intptr_t obj, intptr_t desc)
{
    PeerConnectionObject* o = (PeerConnectionObject*)obj;
    RTCSessionDescription* d = (RTCSessionDescription*)desc;

    return o->GetSessionDescription(o->connection->pending_local_description(), *d);
}

GXP_EXPORT bool _stdcall PeerConnectionGetPendingRemoteDescription(intptr_t obj, intptr_t desc)
{
    PeerConnectionObject* o = (PeerConnectionObject*)obj;
    RTCSessionDescription* d = (RTCSessionDescription*)desc;

    return o->GetSessionDescription(o->connection->pending_remote_description(), *d);

}

GXP_EXPORT bool _stdcall PeerConnectionGetCurrentLocalDescription(intptr_t obj, intptr_t desc)
{
    PeerConnectionObject* o = (PeerConnectionObject*)obj;
    RTCSessionDescription* d = (RTCSessionDescription*)desc;

    return o->GetSessionDescription(o->connection->current_local_description(), *d);
}

GXP_EXPORT bool _stdcall PeerConnectionGetCurrentRemoteDescription(intptr_t obj, intptr_t desc)
{
    PeerConnectionObject* o = (PeerConnectionObject*)obj;
    RTCSessionDescription* d = (RTCSessionDescription*)desc;

    return o->GetSessionDescription(o->connection->current_remote_description(), *d);
}

GXP_EXPORT void* _stdcall PeerConnectionGetReceivers(intptr_t context, intptr_t obj, size_t* length)
{
    Context* c = (Context*)context;
    PeerConnectionObject* o = (PeerConnectionObject*)obj;

    auto receivers = o->connection->GetReceivers();
    return (void*)ConvertPtrArrayFromRefPtrArray<RtpReceiverInterface>(receivers, length);
}

GXP_EXPORT void* _stdcall PeerConnectionGetSenders(intptr_t context, intptr_t obj, size_t* length)
{
    Context* c = (Context*)context;
    PeerConnectionObject* o = (PeerConnectionObject*)obj;
      
    auto senders = o->connection->GetSenders();
    return (void*)ConvertPtrArrayFromRefPtrArray<RtpSenderInterface>(senders, length);
}

GXP_EXPORT void* _stdcall PeerConnectionGetTransceivers(intptr_t context, intptr_t obj, size_t* length)
{
    Context* c = (Context*)context;
    PeerConnectionObject* o = (PeerConnectionObject*)obj;

    auto transceivers = o->connection->GetTransceivers();
    return (void*)ConvertPtrArrayFromRefPtrArray<RtpTransceiverInterface>(transceivers, length);
}

GXP_EXPORT void _stdcall PeerConnectionCreateOffer(intptr_t obj, const intptr_t options)
{
    const RTCOfferAnswerOptions* t = (const RTCOfferAnswerOptions*)options;
    PeerConnectionObject* o = (PeerConnectionObject*)obj;
    o->CreateOffer(*t);
}

GXP_EXPORT void _stdcall PeerConnectionCreateAnswer(intptr_t obj, const intptr_t options)
{
    const RTCOfferAnswerOptions* t = (const RTCOfferAnswerOptions*)options;
    PeerConnectionObject* o = (PeerConnectionObject*)obj;

    o->CreateAnswer(*t);
}

struct RTCDataChannelInit
{
    Optional<bool> ordered;
    Optional<int32_t> maxRetransmitTime;
    Optional<int32_t> maxRetransmits;
    char* protocol;
    Optional<bool> negotiated;
    Optional<int32_t> id;
};

GXP_EXPORT intptr_t _stdcall ContextCreateDataChannel(intptr_t ctx, intptr_t obj, const char* label, const intptr_t options)
{
    Context* c = (Context*)ctx;
    PeerConnectionObject* o = (PeerConnectionObject*)obj;
    const RTCDataChannelInit* t = (const RTCDataChannelInit*)options;

    DataChannelInit _options;
    _options.ordered = t->ordered.value_or(true);
    _options.maxRetransmitTime = static_cast<absl::optional<int>>(t->maxRetransmitTime);
    _options.maxRetransmits = static_cast<absl::optional<int>>(t->maxRetransmits);
    _options.protocol = t->protocol == nullptr ? "" : t->protocol;
    _options.negotiated = t->negotiated.value_or(false);
    _options.id = t->id.value_or(-1);

    return (intptr_t)c->CreateDataChannel(o, label, _options);
}

GXP_EXPORT void _stdcall ContextDeleteDataChannel(intptr_t ctx, intptr_t channel)
{
    Context* c = (Context*)ctx;
    DataChannelObject* d = (DataChannelObject*)channel;

    c->DeleteDataChannel(d);
}

GXP_EXPORT void _stdcall PeerConnectionRegisterIceConnectionChange(intptr_t obj, void *callback)
{
    PeerConnectionObject* o = (PeerConnectionObject*)obj;
    genxp::webrtc::DelegateOnIceConnectionChange c = (genxp::webrtc::DelegateOnIceConnectionChange&)callback;
    o->RegisterIceConnectionChange(c);
}

GXP_EXPORT void PeerConnectionRegisterIceGatheringChange(intptr_t obj, void *callback)
{
    PeerConnectionObject* o = (PeerConnectionObject*)obj;
    genxp::webrtc::DelegateOnIceGatheringChange c = (genxp::webrtc::DelegateOnIceGatheringChange&)callback;

    o->RegisterIceGatheringChange(c);
}

GXP_EXPORT void PeerConnectionRegisterConnectionStateChange(intptr_t obj, void *callback)
{
    PeerConnectionObject* o = (PeerConnectionObject*)obj;
    genxp::webrtc::DelegateOnConnectionStateChange c = (genxp::webrtc::DelegateOnConnectionStateChange&)callback;

    o->RegisterConnectionStateChange(c);
}


GXP_EXPORT void PeerConnectionRegisterOnIceCandidate(intptr_t obj, void *callback)
{
    PeerConnectionObject* o = (PeerConnectionObject*)obj;
    genxp::webrtc::DelegateIceCandidate c = (genxp::webrtc::DelegateIceCandidate&)callback;

    o->RegisterIceCandidate(c);
}

GXP_EXPORT void PeerConnectionRegisterCallbackCollectStats(intptr_t context, void *onGetStats)
{
    Context* c = (Context*)context;
    genxp::webrtc::DelegateCollectStats s = (genxp::webrtc::DelegateCollectStats&)onGetStats;

    PeerConnectionStatsCollectorCallback::RegisterOnGetStats(s);
}

GXP_EXPORT void _stdcall PeerConnectionRegisterCallbackCreateSD(intptr_t obj, void* onSuccess, void* onFailure)
{
    PeerConnectionObject* o = (PeerConnectionObject*)obj;
    genxp::webrtc::DelegateCreateSDSuccess s = (genxp::webrtc::DelegateCreateSDSuccess&)onSuccess;
    genxp::webrtc::DelegateCreateSDFailure f = (genxp::webrtc::DelegateCreateSDFailure&)onFailure;

    o->RegisterCallbackCreateSD(s, f);
}

GXP_EXPORT void _stdcall PeerConnectionRegisterOnSetSessionDescSuccess(intptr_t context, intptr_t obj, void* onSuccess)
{
    Context* c = (Context*)context;
    PeerConnectionObject* o = (PeerConnectionObject*)obj;
    genxp::webrtc::DelegateSetSessionDescSuccess s = (genxp::webrtc::DelegateSetSessionDescSuccess&)onSuccess;

    c->GetObserver(o->connection)->RegisterDelegateOnSuccess(s);
}

GXP_EXPORT void _stdcall PeerConnectionRegisterOnSetSessionDescFailure(intptr_t context, intptr_t obj, void* onFailure)
{
    Context* c = (Context*)context;
    PeerConnectionObject* o = (PeerConnectionObject*)obj;
    genxp::webrtc::DelegateSetSessionDescFailure f = (genxp::webrtc::DelegateSetSessionDescFailure&)onFailure;

    c->GetObserver(o->connection)->RegisterDelegateOnFailure(f);
}

GXP_EXPORT bool _stdcall PeerConnectionAddIceCandidate(intptr_t obj, const intptr_t candidate)
{
    PeerConnectionObject* o = (PeerConnectionObject*)obj;
    const IceCandidateInterface* c = (const IceCandidateInterface*)candidate;
    return o->connection->AddIceCandidate(c);
}

struct RTCIceCandidateInit
{
    char* candidate;
    char* sdpMid;
    int sdpMLineIndex;
};

struct Candidate
{
    char* candidate;
    int component;
    char* foundation;
    char* ip;
    unsigned short port;
    unsigned int priority;
    char* address;
    char* protocol;
    char* relatedAddress;
    unsigned short relatedPort;
    char* tcpType;
    char* type;
    char* usernameFragment;

    Candidate& operator =(const cricket::Candidate& obj)
    {
        candidate = ConvertString(obj.ToString());
        component = obj.component();
        foundation = ConvertString(obj.foundation());
        ip = ConvertString(obj.address().ipaddr().ToString());
        port = obj.address().port();
        priority = obj.priority();
        address = ConvertString(obj.address().ToString());
        protocol = ConvertString(obj.protocol());
        relatedAddress = ConvertString(obj.related_address().ToString());
        relatedPort = obj.related_address().port();
        tcpType = ConvertString(obj.tcptype());
        type = ConvertString(obj.type());
        usernameFragment = ConvertString(obj.username());
        return *this;
    }
};

GXP_EXPORT intptr_t _stdcall CreateIceCandidate(const intptr_t options, intptr_t candidate)
{
    const RTCIceCandidateInit* o = (const RTCIceCandidateInit*)options;
    IceCandidateInterface** c = (IceCandidateInterface**)candidate;

    SdpParseError error;
    IceCandidateInterface* _candidate = CreateIceCandidate(o->sdpMid, o->sdpMLineIndex, o->candidate, &error);
    if (_candidate == nullptr)
        return (intptr_t)RTCErrorType::INVALID_PARAMETER;
    *c = _candidate;
    return (intptr_t)RTCErrorType::NONE;
}

GXP_EXPORT void _stdcall DeleteIceCandidate(intptr_t candidate)
{
    IceCandidateInterface* c = (IceCandidateInterface*)candidate;

    delete c;
}

GXP_EXPORT void _stdcall IceCandidateGetCandidate(const intptr_t candidate, intptr_t dst)
{
    IceCandidateInterface* c = (IceCandidateInterface*)candidate;
    Candidate* d = (Candidate*)dst;

    *d = c->candidate();
}

GXP_EXPORT intptr_t _stdcall IceCandidateGetSdpLineIndex(const intptr_t candidate)
{
    IceCandidateInterface* c = (IceCandidateInterface*)candidate;

    return c->sdp_mline_index();
}

GXP_EXPORT const char* _stdcall IceCandidateGetSdp(const intptr_t candidate)
{
    IceCandidateInterface* c = (IceCandidateInterface*)candidate;

    std::string str;
    if (!c->ToString(&str))
        return nullptr;
    return ConvertString(str);
}

GXP_EXPORT const char* _stdcall IceCandidateGetSdpMid(const intptr_t candidate)
{
    IceCandidateInterface* c = (IceCandidateInterface*)candidate;

    return ConvertString(c->sdp_mid());
}

GXP_EXPORT intptr_t _stdcall PeerConnectionState(intptr_t obj)
{
    PeerConnectionObject* o = (PeerConnectionObject*)obj;

    return (intptr_t)o->connection->peer_connection_state();
}

GXP_EXPORT intptr_t _stdcall PeerConnectionIceConditionState(intptr_t obj)
{
    PeerConnectionObject* o = (PeerConnectionObject*)obj;

    return (intptr_t)o->connection->ice_connection_state();
}

GXP_EXPORT intptr_t _stdcall PeerConnectionSignalingState(intptr_t obj)
{
    PeerConnectionObject* o = (PeerConnectionObject*)obj;

    return (intptr_t)o->connection->signaling_state();
}

GXP_EXPORT intptr_t _stdcall PeerConnectionIceGatheringState(intptr_t obj)
{
    PeerConnectionObject* o = (PeerConnectionObject*)obj;

    return (intptr_t)o->connection->ice_gathering_state();
}

GXP_EXPORT void _stdcall PeerConnectionRegisterOnDataChannel(intptr_t obj, void *callback)
{
    PeerConnectionObject* o = (PeerConnectionObject*)obj;
    genxp::webrtc::DelegateOnDataChannel d = (genxp::webrtc::DelegateOnDataChannel&)callback;

    o->RegisterOnDataChannel(d);
}

GXP_EXPORT void _stdcall PeerConnectionRegisterOnRenegotiationNeeded(intptr_t obj, void* callback)
{
    PeerConnectionObject* o = (PeerConnectionObject*)obj;
    genxp::webrtc::DelegateOnRenegotiationNeeded d = (genxp::webrtc::DelegateOnRenegotiationNeeded&)callback;

    o->RegisterOnRenegotiationNeeded(d);
}

GXP_EXPORT void _stdcall PeerConnectionRegisterOnTrack(intptr_t obj, void* callback)
{
    PeerConnectionObject* o = (PeerConnectionObject*)obj;
    genxp::webrtc::DelegateOnTrack d = (genxp::webrtc::DelegateOnTrack&)callback;

    o->RegisterOnTrack(d);
}

GXP_EXPORT void _stdcall PeerConnectionRegisterOnRemoveTrack(intptr_t obj, void* callback)
{
    PeerConnectionObject* o = (PeerConnectionObject*)obj;
    genxp::webrtc::DelegateOnRemoveTrack d = (genxp::webrtc::DelegateOnRemoveTrack&)callback;

    o->RegisterOnRemoveTrack(d);
}

GXP_EXPORT bool _stdcall TransceiverGetCurrentDirection(intptr_t transceiver, intptr_t direction)
{
    RtpTransceiverInterface* t = (RtpTransceiverInterface*)transceiver;
    RtpTransceiverDirection* d = (RtpTransceiverDirection*)direction;

    if (t->current_direction().has_value())
    {
        *d = t->current_direction().value();
        return true;
    }
    return false;
}

GXP_EXPORT intptr_t _stdcall TransceiverStop(intptr_t transceiver)
{
    RtpTransceiverInterface* t = (RtpTransceiverInterface*)transceiver;

    auto error = t->StopStandard();
    return (intptr_t)error.type();
}

GXP_EXPORT intptr_t _stdcall TransceiverGetDirection(intptr_t transceiver)
{
    RtpTransceiverInterface* t = (RtpTransceiverInterface*)transceiver;

    return (intptr_t)t->direction();
}

GXP_EXPORT void _stdcall TransceiverSetDirection(intptr_t transceiver, intptr_t direction)
{
    RtpTransceiverInterface* t = (RtpTransceiverInterface*)transceiver;
    RtpTransceiverDirection* d = (RtpTransceiverDirection*)direction;

    t->SetDirection(*d); // Check me
}

struct RTCRtpCodecCapability
{
    char* mimeType;
    Optional<int> clockRate;
    Optional<int> channels;
    char* sdpFmtpLine;

    RTCRtpCodecCapability& operator = (const RtpCodecCapability& obj)
    {
        this->mimeType = ConvertString(obj.mime_type());
        this->clockRate = obj.clock_rate;
        this->channels = obj.num_channels;
        this->sdpFmtpLine = ConvertString(ConvertSdp(obj.parameters));
        return *this;
    }
};

GXP_EXPORT intptr_t _stdcall TransceiverSetCodecPreferences(intptr_t transceiver, intptr_t codecs, size_t length)
{
    RtpTransceiverInterface* t = (RtpTransceiverInterface*)transceiver;
    RTCRtpCodecCapability* c = (RTCRtpCodecCapability*)codecs;

    std::vector<RtpCodecCapability> _codecs(length);
    for (int i = 0; i < length; i++)
    {
        std::string mimeType = ConvertString(c[i].mimeType);
        std::tie(_codecs[i].kind, _codecs[i].name) = ConvertMimeType(mimeType);
        _codecs[i].clock_rate = ConvertOptional(c[i].clockRate);
        _codecs[i].num_channels = ConvertOptional(c[i].channels);
        _codecs[i].parameters = ConvertSdp(c[i].sdpFmtpLine);
    }
    auto error = t->SetCodecPreferences(_codecs);
    if (error.type() != RTCErrorType::NONE)
        RTC_LOG(LS_ERROR) << error.message();
    return (intptr_t)error.type();
}

GXP_EXPORT intptr_t _stdcall TransceiverGetReceiver(intptr_t transceiver)
{
    RtpTransceiverInterface* t = (RtpTransceiverInterface*)transceiver;

    return (intptr_t)t->receiver().get();
}

GXP_EXPORT intptr_t _stdcall TransceiverGetSender(intptr_t transceiver)
{
    RtpTransceiverInterface* t = (RtpTransceiverInterface*)transceiver;

    return (intptr_t)t->sender().get();
}

struct RTCRtpEncodingParameters
{
    bool active;
    Optional<uint64_t> maxBitrate;
    Optional<uint64_t> minBitrate;
    Optional<uint32_t> maxFramerate;
    Optional<double> scaleResolutionDownBy;
    char* rid;

    RTCRtpEncodingParameters& operator=(const RtpEncodingParameters& obj)
    {
        active = obj.active;
        maxBitrate = obj.max_bitrate_bps;
        minBitrate = obj.min_bitrate_bps;
        maxFramerate = obj.max_framerate;
        scaleResolutionDownBy = obj.scale_resolution_down_by;
        rid = ConvertString(obj.rid);
        return *this;
    }

    operator RtpEncodingParameters() const
    {
        RtpEncodingParameters dst = {};
        dst.active = active;
        dst.max_bitrate_bps = static_cast<absl::optional<int>>(ConvertOptional(maxBitrate));
        dst.min_bitrate_bps = static_cast<absl::optional<int>>(ConvertOptional(minBitrate));
        dst.max_framerate = static_cast<absl::optional<double>>(ConvertOptional(maxFramerate));
        dst.scale_resolution_down_by = ConvertOptional(scaleResolutionDownBy);
        if (rid != nullptr)
            dst.rid = std::string(rid);
        return dst;
    }
};

struct RTCRtpCodecParameters
{
    int payloadType;
    char* mimeType;
    Optional<unsigned long long> clockRate;
    Optional<unsigned short> channels;
    char* sdpFmtpLine;

    RTCRtpCodecParameters& operator=(const RtpCodecParameters& src)
    {
        payloadType = src.payload_type;
        mimeType = ConvertString(src.mime_type());
        clockRate = src.clock_rate;
        channels = src.num_channels;
        sdpFmtpLine = ConvertString(ConvertSdp(src.parameters));
        return *this;
    }
};

struct RTCRtpExtension
{
    char* uri;
    uint16_t id;
    bool encrypted;

    RTCRtpExtension& operator=(const RtpExtension& src)
    {
        uri = ConvertString(src.uri);
        id = src.id;
        encrypted = src.encrypt;
        return *this;
    }
};

struct RTCRtcpParameters
{
    char* cname;
    bool reducedSize;

    RTCRtcpParameters& operator=(const RtcpParameters& src)
    {
        cname = ConvertString(src.cname);
        reducedSize = src.reduced_size;
        return *this;
    }
};

struct RTCRtpSendParameters
{
    MarshallArray<RTCRtpEncodingParameters> encodings;
    char* transactionId;
    MarshallArray<RTCRtpCodecParameters> codecs;
    MarshallArray<RTCRtpExtension> headerExtensions;
    RTCRtcpParameters rtcp;

    RTCRtpSendParameters& operator=(const RtpParameters& src)
    {
        encodings = src.encodings;
        transactionId = ConvertString(src.transaction_id);
        codecs = src.codecs;
        headerExtensions = src.header_extensions;
        rtcp = src.rtcp;
        return *this;
    }
};

GXP_EXPORT void _stdcall SenderGetParameters(intptr_t sender, intptr_t parameters)
{
    RtpSenderInterface* s = (RtpSenderInterface*)sender;
    RTCRtpSendParameters** p = (RTCRtpSendParameters**)parameters;

    const RtpParameters src = s->GetParameters();
    RTCRtpSendParameters* dst = static_cast<RTCRtpSendParameters*>(CoTaskMemAlloc(sizeof(RTCRtpSendParameters)));
    *dst = src;
    *p = dst;
}

GXP_EXPORT intptr_t _stdcall SenderSetParameters(intptr_t sender, const intptr_t src)
{
    RtpSenderInterface* s = (RtpSenderInterface*)sender;
    RTCRtpSendParameters* p = (RTCRtpSendParameters*)src; // check me

    RtpParameters dst = s->GetParameters();

    for (size_t i = 0; i < dst.encodings.size(); i++)
    {
        dst.encodings[i].active = p->encodings[i].active;
        dst.encodings[i].max_bitrate_bps = static_cast<absl::optional<int>>(ConvertOptional(p->encodings[i].maxBitrate));
        dst.encodings[i].min_bitrate_bps = static_cast<absl::optional<int>>(ConvertOptional(p->encodings[i].minBitrate));
        dst.encodings[i].max_framerate = static_cast<absl::optional<double>>(ConvertOptional(p->encodings[i].maxFramerate));
        dst.encodings[i].scale_resolution_down_by = ConvertOptional(p->encodings[i].scaleResolutionDownBy);
        if (p->encodings[i].rid != nullptr)
            dst.encodings[i].rid = std::string(p->encodings[i].rid);
    }
    const ::webrtc::RTCError error = s->SetParameters(dst);
    return (intptr_t)error.type();
}

struct RTCRtpHeaderExtensionCapability
{
    char* uri;

    RTCRtpHeaderExtensionCapability& operator = (const RtpHeaderExtensionCapability& obj)
    {
        this->uri = ConvertString(obj.uri);
        return *this;
    }
};

struct RTCRtpCapabilities
{
    MarshallArray<RTCRtpCodecCapability> codecs;
    MarshallArray<RTCRtpHeaderExtensionCapability> extensionHeaders;

    RTCRtpCapabilities& operator=(const RtpCapabilities& src)
    {
        codecs = src.codecs;
        extensionHeaders = src.header_extensions;
        return *this;
    }
};

GXP_EXPORT void _stdcall ContextGetSenderCapabilities(
    intptr_t context, intptr_t trackKind, intptr_t parameters)
{
    Context* c = (Context*)context;
    TrackKind t = (TrackKind)trackKind;
    RTCRtpCapabilities** p = (RTCRtpCapabilities**)parameters;

    RtpCapabilities src;
    cricket::MediaType type =
        t == TrackKind::Audio ?
        cricket::MEDIA_TYPE_AUDIO : cricket::MEDIA_TYPE_VIDEO;
    c->GetRtpSenderCapabilities(type, &src);

    RTCRtpCapabilities* dst =
        static_cast<RTCRtpCapabilities*>(CoTaskMemAlloc(sizeof(RTCRtpCapabilities)));
    *dst = src;
    *p = dst;
}

GXP_EXPORT void _stdcall ContextGetReceiverCapabilities(
    intptr_t context, intptr_t trackKind, intptr_t parameters)
{
    Context* c = (Context*)context;
    TrackKind t = (TrackKind)trackKind;
    RTCRtpCapabilities** p = (RTCRtpCapabilities**)parameters;

    RtpCapabilities src;
    cricket::MediaType type =
        t == TrackKind::Audio ?
        cricket::MEDIA_TYPE_AUDIO : cricket::MEDIA_TYPE_VIDEO;
    c->GetRtpReceiverCapabilities(type, &src);

    RTCRtpCapabilities* dst =
        static_cast<RTCRtpCapabilities*>(CoTaskMemAlloc(sizeof(RTCRtpCapabilities)));
    *dst = src;
    *p = dst;
}

GXP_EXPORT bool _stdcall SenderReplaceTrack(intptr_t sender, intptr_t track)
{
    RtpSenderInterface* s = (RtpSenderInterface*)sender;
    MediaStreamTrackInterface* t = (MediaStreamTrackInterface*)track;

    return s->SetTrack(t);
}

GXP_EXPORT intptr_t _stdcall SenderGetTrack(intptr_t sender)
{
    RtpSenderInterface* s = (RtpSenderInterface*)sender;

    return (intptr_t)s->track().get();
}

GXP_EXPORT intptr_t _stdcall ReceiverGetTrack(intptr_t receiver)
{
    RtpReceiverInterface* r = (RtpReceiverInterface*)receiver;

    return (intptr_t)r->track().get();
}

GXP_EXPORT void* _stdcall ReceiverGetStreams(intptr_t receiver, size_t* length)
{
    RtpReceiverInterface* r = (RtpReceiverInterface*)receiver;

    return (void*)ConvertPtrArrayFromRefPtrArray<MediaStreamInterface>(r->streams(), length);
}

GXP_EXPORT intptr_t _stdcall DataChannelGetID(intptr_t dataChannelObj)
{
    DataChannelObject* d = (DataChannelObject*)dataChannelObj;
    return d->dataChannel->id();
}

GXP_EXPORT char* _stdcall DataChannelGetLabel(intptr_t dataChannelObj)
{
    DataChannelObject* d = (DataChannelObject*)dataChannelObj;

    return ConvertString(d->dataChannel->label());
}

GXP_EXPORT char* _stdcall DataChannelGetProtocol(intptr_t dataChannelObj)
{
    DataChannelObject* d = (DataChannelObject*)dataChannelObj;

    return ConvertString(d->dataChannel->protocol());
}

GXP_EXPORT unsigned int _stdcall DataChannelGetMaxRetransmits(intptr_t dataChannelObj)
{
    DataChannelObject* d = (DataChannelObject*)dataChannelObj;

    return d->dataChannel->maxRetransmits();
}

GXP_EXPORT unsigned int _stdcall DataChannelGetMaxRetransmitTime(intptr_t dataChannelObj)
{
    DataChannelObject* d = (DataChannelObject*)dataChannelObj;

    return d->dataChannel->maxRetransmitTime();
}

GXP_EXPORT bool _stdcall DataChannelGetOrdered(intptr_t dataChannelObj)
{
    DataChannelObject* d = (DataChannelObject*)dataChannelObj;

    return d->dataChannel->ordered();
}

GXP_EXPORT uint64_t _stdcall DataChannelGetBufferedAmount(intptr_t dataChannelObj)
{
    DataChannelObject* d = (DataChannelObject*)dataChannelObj;

    return d->dataChannel->buffered_amount();
}

GXP_EXPORT bool _stdcall DataChannelGetNegotiated(intptr_t dataChannelObj)
{
    DataChannelObject* d = (DataChannelObject*)dataChannelObj;

    return d->dataChannel->negotiated();
}

GXP_EXPORT intptr_t _stdcall DataChannelGetReadyState(
    intptr_t dataChannelObj)
{
    DataChannelObject* d = (DataChannelObject*)dataChannelObj;

    return d->dataChannel->state();
}

GXP_EXPORT void _stdcall DataChannelSend(intptr_t dataChannelObj, const char* msg)
{
    DataChannelObject* d = (DataChannelObject*)dataChannelObj;

    d->Send(msg);
}

GXP_EXPORT void _stdcall DataChannelSendBinary(intptr_t dataChannelObj, const unsigned char* msg, int len)
{
    DataChannelObject* d = (DataChannelObject*)dataChannelObj;

    d->Send(msg, len);
}

GXP_EXPORT void _stdcall DataChannelClose(intptr_t dataChannelObj)
{
    DataChannelObject* d = (DataChannelObject*)dataChannelObj;

    d->Close();
}

GXP_EXPORT void _stdcall DataChannelRegisterOnMessage(intptr_t dataChannelObj, void* callback)
{
    DataChannelObject* d = (DataChannelObject*)dataChannelObj;
    genxp::webrtc::DelegateOnMessage c = (genxp::webrtc::DelegateOnMessage&)callback;

    d->RegisterOnMessage(c);
}

GXP_EXPORT void _stdcall DataChannelRegisterOnOpen(intptr_t dataChannelObj, void* callback)
{
    DataChannelObject* d = (DataChannelObject*)dataChannelObj;
    genxp::webrtc::DelegateOnOpen c = (genxp::webrtc::DelegateOnOpen&)callback;

    d->RegisterOnOpen(c);
}

GXP_EXPORT void _stdcall DataChannelRegisterOnClose(intptr_t dataChannelObj, void* callback)
{
    DataChannelObject* d = (DataChannelObject*)dataChannelObj;
    genxp::webrtc::DelegateOnClose c = (genxp::webrtc::DelegateOnClose&)callback;

    d->RegisterOnClose(c);
}

GXP_EXPORT void _stdcall SetCurrentContext(intptr_t context)
{
    Context* c = (Context*)context;
    ContextManager::GetInstance()->curContext = c;
}

GXP_EXPORT void _stdcall ProcessAudio(
    intptr_t track,
    float* audio_data,
    int sample_rate,
    size_t number_of_channels,
    size_t number_of_frames)
{
    AudioTrackInterface* t = (AudioTrackInterface*)track;

    GXPAudioTrackSource* source =
        static_cast<GXPAudioTrackSource*>(t->GetSource());

    source->OnData(audio_data,
        sample_rate,
        number_of_channels,
        number_of_frames);
}


GXP_EXPORT void _stdcall ContextRegisterAudioReceiveCallback(
    intptr_t context, intptr_t track, void *callback)
{
    Context* c = (Context*)context;
    AudioTrackInterface* t = (AudioTrackInterface*)track;
    genxp::webrtc::DelegateAudioReceive a = (genxp::webrtc::DelegateAudioReceive&)callback;

    c->RegisterAudioReceiveCallback(t, a);
}

GXP_EXPORT void _stdcall ContextUnregisterAudioReceiveCallback(
    intptr_t context, intptr_t track)
{
    Context* c = (Context*)context;
    AudioTrackInterface* t = (AudioTrackInterface*)track;

    c->UnregisterAudioReceiveCallback(t);
}

GXP_EXPORT const char* _stdcall GetWrapperVersion()
{
    return "GXP WebRTCAPI Wrapper v1.0.0";
}

//===Finished GXP WebRTC API