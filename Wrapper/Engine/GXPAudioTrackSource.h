#pragma once

#include <mutex>

using namespace ::webrtc;

namespace genxp
{
namespace webrtc
{
    class GXPAudioTrackSource : public LocalAudioSource
    {
    public:
        static rtc::scoped_refptr<GXPAudioTrackSource> Create();
        static rtc::scoped_refptr<GXPAudioTrackSource> Create(const cricket::AudioOptions& audio_options);

        const cricket::AudioOptions options() const override { return _options; }
        void AddSink(AudioTrackSinkInterface* sink) override;
        void RemoveSink(AudioTrackSinkInterface* sink) override;

        void OnData(const float* pAudioData, int nSampleRate, size_t nNumChannels, size_t nNumFrames);

    protected:
        GXPAudioTrackSource();
        GXPAudioTrackSource(const cricket::AudioOptions& audio_options);

        ~GXPAudioTrackSource() override;

    private:
        std::vector<int16_t> _convertedAudioData;
        std::vector <AudioTrackSinkInterface*> _arrSink;
        std::mutex _mutex;
        cricket::AudioOptions _options;
    };
} // end namespace webrtc
} // end namespace genxp
