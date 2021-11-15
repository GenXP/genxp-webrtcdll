#include "../WebRTC.h"

#include "GXPLogStream.h"

namespace genxp {
    namespace webrtc {

    std::unique_ptr<GXPLogStream> GXPLogStream::log_stream;

    void GXPLogStream::OnLogMessage(const std::string& message) {
        if (on_log_message != nullptr)
        {
            on_log_message(message.c_str());
        }
    }

    void GXPLogStream::AddLogStream(DelegateDebugLog callback, rtc::LoggingSeverity loggingSeverity)
    {
        rtc::LogMessage::LogTimestamps(true);
        log_stream.reset(new GXPLogStream(callback));
        rtc::LogMessage::AddLogToStream(log_stream.get(), loggingSeverity);
    }

    void GXPLogStream::RemoveLogStream()
    {
        if (log_stream) {
            rtc::LogMessage::RemoveLogToStream(log_stream.get());
            log_stream.reset();
        }
    }


}
} 

