#pragma once

#include <string>

#include "WebRTCPlugin.h"


namespace genxp {
namespace webrtc {


    class GXPLogStream : public rtc::LogSink {
    public:
        GXPLogStream(DelegateDebugLog callback) : on_log_message(callback){

        }

        // log format can be defined in this interface
        void OnLogMessage(const std::string& message) override;

        static void AddLogStream(DelegateDebugLog callback, rtc::LoggingSeverity loggingSeverity);
        static void RemoveLogStream();


    private:
        DelegateDebugLog on_log_message;

        static std::unique_ptr<GXPLogStream> log_stream;
        
    };


}
} 

