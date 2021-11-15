#pragma once

#include "../WebRTC.h"
#include "api/audio_codecs/audio_encoder_factory.h"

namespace genxp
{
namespace webrtc
{
    rtc::scoped_refptr<::webrtc::AudioEncoderFactory> CreateAudioEncoderFactory();

} // end namespace webrtc
} // end namespace genxp
