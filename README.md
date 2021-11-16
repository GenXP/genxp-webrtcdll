# genxp-webrtcdll
 A fully functional WebRTC Runtime Wrapper with a C/C++ provided WebRTCAPI.h header.
 
 ## What is it?
 This is a fully functional WebRTC C/C++ wrapper. First of all. We aren't the only wrapper out there. Other wrapper examples include:
 - [Microsoft's WinRTC WebRTC Wrapper](https://github.com/microsoft/winrtc/tree/master/WebRtcWrapper)
 - [Unity3D's WebRTC Plugin](https://github.com/Unity-Technologies/com.unity.webrtc)
 - NodeJS WebRTC Wrapper
 - [Wk Webview WebRTC Shim](https://github.com/common-tater/wkwebview-webrtc-shim)

However, to our knowledge we are the **only** C compatible runtime wrapper. We've negotiated the complexity of WebRTC to an easy to use Wrapper.

## API

```
GXP_EXPORT intptr_t _stdcall ContextCreateMediaStream(intptr_t context, const char* streamId);
GXP_EXPORT void _stdcall ContextStopMediaStreamTrack(intptr_t context, intptr_t track);
GXP_EXPORT intptr_t _stdcall ContextCreateAudioTrackSource(intptr_t context);
GXP_EXPORT intptr_t _stdcall ContextCreateAudioTrack(intptr_t context, const char* label, intptr_t source);
GXP_EXPORT void _stdcall ContextAddRefPtr(intptr_t context, intptr_t ptr);
GXP_EXPORT void _stdcall ContextDeleteRefPtr(intptr_t context, intptr_t ptr);
GXP_EXPORT bool _stdcall MediaStreamAddTrack(intptr_t stream, intptr_t track);
```...

## Runtime & Callbacks

## Examples

## Header
Our API is contained to one simple API header ```WebRTCAPI.h``` which provides runtime support to the WebRTC library.


## Other resources
Other additional resources include [awesome-webrtc](https://giters.com/openrtc-io/awesome-webrtc).


