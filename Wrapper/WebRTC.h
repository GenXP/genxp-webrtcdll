#pragma once

#define GXP_EXPORT extern "C" __declspec(dllexport)

#define WEBRTC_WIN 1
#define WEBRTC_APM_DEBUG_DUMP 0

#pragma comment(lib, "secur32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dmoguids.lib")
#pragma comment(lib, "wmcodecdspuuid.lib")
#pragma comment(lib, "msdmo.lib")
#pragma comment(lib, "Strmiids.lib")
#pragma comment(lib,"ws2_32.lib")

#include <condition_variable>
#include <iostream>
#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <stdio.h>

#include <system_wrappers/include/field_trial.h>

#include "api/media_stream_interface.h"
#include "api/peer_connection_interface.h"
#include "api/create_peerconnection_factory.h"
#include "api/audio_codecs/audio_decoder_factory_template.h"
#include "api/audio_codecs/audio_encoder_factory_template.h"
#include "api/audio_codecs/opus/audio_decoder_opus.h"
#include "api/audio_codecs/opus/audio_encoder_opus.h"
#include "api/video_codecs/video_decoder_factory.h"
#include "api/video_codecs/builtin_video_decoder_factory.h"
#include "api/video_codecs/video_encoder_factory.h"
#include "api/video_codecs/builtin_video_encoder_factory.h"
#include "api/video_codecs/video_encoder.h"
#include "api/video_codecs/sdp_video_format.h"
#include "api/video/video_frame.h"
#include "api/video/video_frame_buffer.h"
#include "api/video/video_sink_interface.h"
#include "api/video/i420_buffer.h"
#include "api/video_track_source_proxy_factory.h"

#include "rtc_base/thread.h"
#include "rtc_base/ref_counted_object.h"
#include "rtc_base/logging.h"
#include "rtc_base/checks.h"
#include "rtc_base/ssl_adapter.h"
#include "rtc_base/arraysize.h"
#include "rtc_base/net_helpers.h"
#include "rtc_base/string_utils.h"
#include "rtc_base/physical_socket_server.h"
//#include "rtc_base/signal_thread.h"
#include "rtc_base/third_party/sigslot/sigslot.h"
#include "rtc_base/atomic_ops.h"
#include "rtc_base/async_tcp_socket.h"

#ifdef _WIN32
#include "rtc_base/win32.h"
#include "rtc_base/win32_socket_server.h"
#include "rtc_base/win32_socket_init.h"
#include "rtc_base/win32_socket_server.h"
#endif

#include "media/engine/internal_encoder_factory.h"
#include "media/engine/internal_decoder_factory.h"
//#include "media/base/h264_profile_level_id.h"
#include "media/base/adapted_video_track_source.h"
#include "media/base/media_channel.h"
#include "media/base/video_common.h"
#include "media/base/video_broadcaster.h"

#include "modules/video_capture/video_capture_impl.h"
#include "modules/video_capture/video_capture_factory.h"
#include "modules/audio_device/include/audio_device.h"
#include "modules/audio_device/audio_device_buffer.h"
#include "modules/audio_device/audio_device_generic.h"
#include "modules/audio_processing/include/audio_processing.h"
#include "modules/video_coding/codecs/h264/include/h264.h"
#include "modules/video_coding/codecs/vp8/include/vp8.h"
#include "modules/video_coding/codecs/vp9/include/vp9.h"

#include "common_video/h264/h264_bitstream_parser.h"
#include "common_video/h264/h264_common.h"
#include "common_video/include/bitrate_adjuster.h"

#include "pc/media_stream_observer.h"
#include "pc/local_audio_source.h"


namespace genxp
{
    namespace webrtc
    {

        void LogPrint(const char* fmt, ...);
        void LogPrint(const wchar_t* fmt, ...);
        void checkf(bool result, const char* msg);
        #define DebugLog(...)       LogPrint("webrtc Log: " __VA_ARGS__)
        #define DebugWarning(...)   LogPrint("webrtc Warning: " __VA_ARGS__)
        #define DebugError(...)     LogPrint("webrtc Error: "  __VA_ARGS__)
        #define DebugLogW(...)      LogPrint(L"webrtc Log: " __VA_ARGS__)
        #define DebugWarningW(...)  LogPrint(L"webrtc Warning: " __VA_ARGS__)
        #define DebugErrorW(...)    LogPrint(L"webrtc Error: "  __VA_ARGS__)
        #define NV_RESULT(NvFunction) NvFunction == NV_ENC_SUCCESS

        template<class ... Args>
        std::string StringFormat(const std::string& format, Args ... args)
        {
            size_t size = snprintf(nullptr, 0, format.c_str(), args ...) + 1;
            std::unique_ptr<char[]> buf(new char[size]);
            snprintf(buf.get(), size, format.c_str(), args ...);
            return std::string(buf.get(), buf.get() + size - 1);
        }

        using byte = unsigned char;
        using uint8 = unsigned char;
        using uint16 = unsigned short int;
        using uint32 = unsigned int;
        using uint64 = unsigned long long;
        using int8 = signed char;
        using int16 = signed short int;
        using int32 = signed int;
        using int64 = signed long long;

        const uint32 bufferedFrameNum = 3;

        enum GXPEncoderType
        {
            GXPEncoderSoftware = 0,
            GXPEncoderHardware = 1,
        };

    } // end namespace webrtc
} // end namespace genxp