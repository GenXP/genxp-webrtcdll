# GenXP Runtime WebRTC DLL
 A fully functional WebRTC Runtime C/C++ Wrapper.
 
 ## Why Runtime / Dynamic DLL?
 TLDR; Where is my static lib? This isn't that library. If you are looking for a static lib you can likely compile our library to support it but we don't recommend it. You are bound to find yourself fighting with symbology collisions from the many libraries that WebRTC ships with and your other projects. You also don't have the priviledge to isolate WebRTC from the rest of your codebase.
 
 WebRTC is a mammoth. Runtime support means you can call and interact with the library on your terms. Our wrapper gives you lots of callback support which is passed directly to the WebRTC library.
 
 ## What is it?
 This is a fully functional WebRTC C/C++ wrapper. First of all. We aren't the only wrapper out there. Other wrapper examples include:
 - [Microsoft's WinRTC WebRTC Wrapper](https://github.com/microsoft/winrtc/tree/master/WebRtcWrapper)
 - [Unity3D's WebRTC Plugin](https://github.com/Unity-Technologies/com.unity.webrtc)
 - NodeJS WebRTC Wrapper
 - [Wk Webview WebRTC Shim](https://github.com/common-tater/wkwebview-webrtc-shim)

However, to our knowledge we are the **only** C compatible runtime wrapper. We've negotiated the complexity of WebRTC to an easy to use Wrapper.

## Windows x64 support storage Ptrs
This wrapper is intended to be used for x64 only. We believe the best way to negotiate most pointers via an int pointer is the best method. Unfortunately, standard int can result in a loss of data with x64 binaries. Therefore all of our storage pointers are ```intptr_t``` or ```int64```.

## Supporting standard types
We tried very hard to avoid complex types. We should be entirely C99 compatabile. Our only include is:
```#include <stdint.h>```

## API
Our API is contained to one simple API header ```WebRTCAPI.h``` which provides runtime support to the WebRTC library. The following subset demonstrates the simplicity of our API.

```c++
...
GXP_EXPORT intptr_t _stdcall ContextCreateMediaStream(intptr_t context, const char* streamId);
GXP_EXPORT void _stdcall ContextStopMediaStreamTrack(intptr_t context, intptr_t track);
GXP_EXPORT intptr_t _stdcall ContextCreateAudioTrackSource(intptr_t context);
GXP_EXPORT intptr_t _stdcall ContextCreateAudioTrack(intptr_t context, const char* label, intptr_t source);
GXP_EXPORT void _stdcall ContextAddRefPtr(intptr_t context, intptr_t ptr);
GXP_EXPORT void _stdcall ContextDeleteRefPtr(intptr_t context, intptr_t ptr);
GXP_EXPORT bool _stdcall MediaStreamAddTrack(intptr_t stream, intptr_t track);
...
```

## Runtime & Callbacks

## Examples


## Other resources
Other additional resources include [awesome-webrtc](https://giters.com/openrtc-io/awesome-webrtc).


