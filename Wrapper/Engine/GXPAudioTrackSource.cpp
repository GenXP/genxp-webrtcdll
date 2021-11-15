#include "../WebRTC.h"
#include "GXPAudioTrackSource.h"

namespace genxp
{
namespace webrtc
{

rtc::scoped_refptr<GXPAudioTrackSource> GXPAudioTrackSource::Create()
{
    rtc::scoped_refptr<GXPAudioTrackSource> source(
        new rtc::RefCountedObject<GXPAudioTrackSource>());
    return source;
}

rtc::scoped_refptr<GXPAudioTrackSource> GXPAudioTrackSource::Create(
    const cricket::AudioOptions& audio_options)
{
    rtc::scoped_refptr<GXPAudioTrackSource> source(
        new rtc::RefCountedObject<GXPAudioTrackSource>(audio_options));
    return source;
}

void GXPAudioTrackSource::AddSink(AudioTrackSinkInterface* sink)
{
    std::lock_guard<std::mutex> lock(_mutex);

    _arrSink.push_back(sink);
}

void GXPAudioTrackSource::RemoveSink(AudioTrackSinkInterface* sink)
{
    std::lock_guard<std::mutex> lock(_mutex);

    auto i= std::find(_arrSink.begin(), _arrSink.end(), sink);
    if (i != _arrSink.end())
        _arrSink.erase(i);
}

void GXPAudioTrackSource::OnData(const float* pAudioData, int nSampleRate, size_t nNumChannels, size_t nNumFrames)
{
    RTC_DCHECK(pAudioData);
    RTC_DCHECK(nSampleRate);
    RTC_DCHECK(nNumChannels);
    RTC_DCHECK(nNumFrames);

    std::lock_guard<std::mutex> lock(_mutex);

    if (_arrSink.empty())
        return;

    for (size_t i = 0; i < nNumFrames; i++)
    {
        _convertedAudioData.push_back(pAudioData[i] >= 0 ? pAudioData[i] * SHRT_MAX : pAudioData[i] * -SHRT_MIN);
    }

    // eg.  80 for 8KHz and 160 for 16kHz
    size_t nNumFramesFor10ms = nSampleRate / 100;
    size_t size = _convertedAudioData.size() / (nNumFramesFor10ms * nNumChannels);
    size_t nBitPerSample = sizeof(int16_t) * 8;

    for(auto sink : _arrSink)
    {
        for (size_t i = 0; i < size; i++)
        {
            sink->OnData(
                &_convertedAudioData.data()[i * nNumFramesFor10ms * nNumChannels],
                nBitPerSample, nSampleRate, nNumChannels, nNumFramesFor10ms);
        }
    }

    // pop processed buffer, remained buffer will be processed the next time.
    _convertedAudioData.erase(
        _convertedAudioData.begin(),
        _convertedAudioData.begin() + nNumFramesFor10ms * nNumChannels * size);
}

GXPAudioTrackSource::GXPAudioTrackSource()
{
}
GXPAudioTrackSource::GXPAudioTrackSource(const cricket::AudioOptions& audio_options)
    : _options(audio_options) {
}

GXPAudioTrackSource::~GXPAudioTrackSource()
{
}

} // end namespace webrtc
} // end namespace genxp
