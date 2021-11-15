/* This is a near complete C wrapper for our WebRTC Runtime!
*  When? When you need to utilize WebRTC natively with a complex tech stack. Avoid the mess.
*  Where? Anywhere you can link against a C header.
*  How? Easy. Download this header along with our precompiled Dynamic Link Library (DLL).
*  Bonus. We provide access to pre-compiled builds of WebRTC for Windows.
*  Special notes: Most Ptrs are stored as 64bit integer.
*  This provides a fairly easy marashaling/interoping between programs and languages.
*  Author: Vince McMullin
*  Email: vincem@genxp.com
*  Company: GenXP
*/

#include <stdint.h>

#define GXP_EXPORT extern "C" __declspec(dllexport)

//============== callback reference only

/*GXP_EXPORT typedef void _stdcall DelegateDebugLog(const char* str);

GXP_EXPORT typedef void _stdcall DelegateCreateSDSuccess(intptr_t ptr, int type, const char* sdp);

GXP_EXPORT typedef void _stdcall DelegateCollectStats(intptr_t ptr, intptr_t reportPtr);

GXP_EXPORT typedef void _stdcall DelegateCreateGetStats(intptr_t ptr, int type, const char* sdp);

GXP_EXPORT typedef void _stdcall DelegateCreateSDFailure(intptr_t ptr, int type, const char* message);

GXP_EXPORT typedef void _stdcall DelegateNativePeerConnectionSetSessionDescSuccess(intptr_t ptr);

GXP_EXPORT typedef void _stdcall DelegateNativePeerConnectionSetSessionDescFailure(intptr_t ptr, int type, const char* message);

GXP_EXPORT typedef void _stdcall DelegateNativeOnIceConnectionChange(intptr_t ptr, int state);

GXP_EXPORT typedef void _stdcall DelegateNativeOnConnectionStateChange(intptr_t ptr, int state);

GXP_EXPORT typedef void _stdcall DelegateNativeOnIceGatheringChange(intptr_t ptr, int state);

GXP_EXPORT typedef void _stdcall DelegateNativeOnIceCandidate(intptr_t ptr, const char* candidate, const char* sdpMid, int sdpMlineIndex);

GXP_EXPORT typedef void _stdcall DelegateNativeOnNegotiationNeeded(intptr_t ptr);

GXP_EXPORT typedef void _stdcall DelegateNativeOnTrack(intptr_t ptr, int transceiver);

GXP_EXPORT typedef void _stdcall DelegateNativeOnRemoveTrack(intptr_t ptr, intptr_t receiver);

GXP_EXPORT typedef void _stdcall DelegateNativeOnDataChannel(intptr_t ptr, intptr_t ptrChannel);

GXP_EXPORT typedef void _stdcall DelegateNativeOnMessage(intptr_t ptr, unsigned int* bytes, int size);

GXP_EXPORT typedef void _stdcall DelegateNativeOnOpen(intptr_t ptr);

GXP_EXPORT typedef void _stdcall DelegateNativeOnClose(intptr_t ptr);

GXP_EXPORT typedef void _stdcall DelegateNativeMediaStreamOnAddTrack(intptr_t stream, intptr_t track);

GXP_EXPORT typedef void _stdcall DelegateNativeMediaStreamOnRemoveTrack(intptr_t stream, intptr_t track);

GXP_EXPORT typedef void _stdcall DelegateIceCandidate(intptr_t obj);

GXP_EXPORT typedef void _stdcall DelegateSetSessionDescSuccess(intptr_t context, intptr_t obj);

GXP_EXPORT typedef void _stdcall DelegateSetSessionDescFailure(intptr_t context, intptr_t obj);

GXP_EXPORT typedef void _stdcall DelegateAudioReceive(intptr_t track, float* audioData, int size, int sampleRate, int numOfChannels, int numOfFrames);

GXP_EXPORT typedef void _stdcall DelegateOnDataChannel(intptr_t obj);

GXP_EXPORT typedef void _stdcall DelegateOnRenegotiationNeeded(intptr_t obj);

GXP_EXPORT typedef void _stdcall DelegateOnTrack(intptr_t obj);

GXP_EXPORT typedef void _stdcall DelegateOnRemoveTrack(intptr_t obj);

GXP_EXPORT typedef void _stdcall DelegateOnMessage(intptr_t dataChannelObj);

GXP_EXPORT typedef void _stdcall DelegateOnOpen(intptr_t dataChannelObj);

GXP_EXPORT typedef void _stdcall DelegateOnClose(intptr_t dataChannelObj); */

//============== Functions

GXP_EXPORT intptr_t _stdcall ContextCreateMediaStream(intptr_t context, const char* streamId);

GXP_EXPORT void _stdcall ContextStopMediaStreamTrack(intptr_t context, intptr_t track);

GXP_EXPORT intptr_t _stdcall ContextCreateAudioTrackSource(intptr_t context);

GXP_EXPORT intptr_t _stdcall ContextCreateAudioTrack(intptr_t context, const char* label, intptr_t source);

GXP_EXPORT void _stdcall ContextAddRefPtr(intptr_t context, intptr_t ptr);

GXP_EXPORT void _stdcall ContextDeleteRefPtr(intptr_t context, intptr_t ptr);

GXP_EXPORT bool _stdcall MediaStreamAddTrack(intptr_t stream, intptr_t track);

GXP_EXPORT bool _stdcall MediaStreamRemoveTrack(intptr_t stream, intptr_t track);

GXP_EXPORT char* _stdcall MediaStreamGetID(intptr_t stream);

GXP_EXPORT void* _stdcall MediaStreamGetAudioTracks(intptr_t stream, size_t* length);

GXP_EXPORT intptr_t _stdcall MediaStreamTrackGetKind(intptr_t track);

GXP_EXPORT intptr_t _stdcall MediaStreamTrackGetReadyState(intptr_t track);

GXP_EXPORT char* _stdcall MediaStreamTrackGetID(intptr_t track);

GXP_EXPORT bool _stdcall MediaStreamTrackGetEnabled(intptr_t track);

GXP_EXPORT void _stdcall MediaStreamTrackSetEnabled(intptr_t track, bool enabled);

GXP_EXPORT void _stdcall RegisterDebugLog(intptr_t func, bool enableNativeLog, int loggingSeverity);

GXP_EXPORT intptr_t _stdcall ContextCreate(int32_t uid, int encoderType, bool forTest);

GXP_EXPORT void _stdcall ContextDestroy(int32_t uid);

GXP_EXPORT intptr_t _stdcall ContextCreatePeerConnection(intptr_t context);

GXP_EXPORT intptr_t _stdcall ContextCreatePeerConnectionWithConfig(intptr_t context, const char* conf);

GXP_EXPORT void _stdcall ContextDeletePeerConnection(intptr_t context, intptr_t obj);

GXP_EXPORT void _stdcall PeerConnectionClose(intptr_t obj);

GXP_EXPORT void _stdcall PeerConnectionRestartIce(intptr_t obj);

GXP_EXPORT intptr_t _stdcall PeerConnectionAddTrack(
    intptr_t obj, intptr_t track, const char* streamId, intptr_t sender);

GXP_EXPORT void* _stdcall PeerConnectionAddTransceiver(
    intptr_t context, intptr_t obj, intptr_t track);

GXP_EXPORT void* _stdcall PeerConnectionAddTransceiverWithInit(
    intptr_t context, intptr_t obj, intptr_t track, intptr_t init);

GXP_EXPORT void* _stdcall PeerConnectionAddTransceiverWithType(
    intptr_t context, intptr_t obj, int type);

GXP_EXPORT void* _stdcall PeerConnectionAddTransceiverWithTypeAndInit(
    intptr_t context, intptr_t obj, int type, intptr_t init);

GXP_EXPORT intptr_t _stdcall PeerConnectionRemoveTrack(intptr_t obj, intptr_t sender);

GXP_EXPORT intptr_t _stdcall PeerConnectionSetConfiguration(intptr_t obj, const char* conf);

GXP_EXPORT char* _stdcall PeerConnectionGetConfiguration(intptr_t obj);

GXP_EXPORT void _stdcall PeerConnectionGetStats(intptr_t obj);

GXP_EXPORT void _stdcall PeerConnectionSenderGetStats(intptr_t obj, intptr_t selector);

GXP_EXPORT void _stdcall PeerConnectionReceiverGetStats(intptr_t obj, intptr_t receiver);

GXP_EXPORT const intptr_t _stdcall StatsReportGetStatsList(const intptr_t report, size_t* length, unsigned int** types);

GXP_EXPORT void _stdcall ContextDeleteStatsReport(intptr_t context, const intptr_t report);

GXP_EXPORT const char* _stdcall StatsGetJson(const intptr_t stats);

GXP_EXPORT intptr_t _stdcall StatsGetTimestamp(const intptr_t stats);

GXP_EXPORT const char* _stdcall StatsGetId(const intptr_t stats);

GXP_EXPORT unsigned int _stdcall StatsGetType(const intptr_t stats);

GXP_EXPORT const intptr_t _stdcall StatsGetMembers(const intptr_t stats, size_t* length);

GXP_EXPORT bool _stdcall StatsMemberIsDefined(const intptr_t member);

GXP_EXPORT const char* _stdcall StatsMemberGetName(const intptr_t member);

GXP_EXPORT bool _stdcall StatsMemberGetBool(const intptr_t member);

GXP_EXPORT intptr_t _stdcall StatsMemberGetInt(const intptr_t member);

GXP_EXPORT unsigned int _stdcall StatsMemberGetUnsignedInt(const intptr_t member);

GXP_EXPORT long long _stdcall StatsMemberGetLong(const intptr_t member);

GXP_EXPORT unsigned long long _stdcall StatsMemberGetUnsignedLong(const intptr_t member);

GXP_EXPORT double StatsMemberGetDouble(const intptr_t member);

GXP_EXPORT const char* StatsMemberGetString(const intptr_t member);

GXP_EXPORT bool* StatsMemberGetBoolArray(const intptr_t member, size_t* length);

GXP_EXPORT int32_t* StatsMemberGetIntArray(const intptr_t member, size_t* length);

GXP_EXPORT unsigned int* StatsMemberGetUnsignedIntArray(const intptr_t member, size_t* length);

GXP_EXPORT long long* StatsMemberGetLongArray(const intptr_t member, size_t* length);

GXP_EXPORT unsigned long long* StatsMemberGetUnsignedLongArray(const intptr_t member, size_t* length);

GXP_EXPORT double* StatsMemberGetDoubleArray(const intptr_t member, size_t* length);

GXP_EXPORT const char** StatsMemberGetStringArray(const intptr_t member, size_t* length);

GXP_EXPORT intptr_t _stdcall StatsMemberGetType(const intptr_t member);

GXP_EXPORT intptr_t _stdcall PeerConnectionSetLocalDescription(
    intptr_t context, intptr_t obj, const intptr_t desc, char* error[]);

GXP_EXPORT intptr_t _stdcall PeerConnectionSetLocalDescriptionWithoutDescription(intptr_t context, intptr_t obj, char* error[]);

GXP_EXPORT intptr_t _stdcall PeerConnectionSetRemoteDescription(
    intptr_t context, intptr_t obj, const intptr_t desc, char* error[]);

GXP_EXPORT bool _stdcall PeerConnectionGetLocalDescription(intptr_t obj, intptr_t desc);

GXP_EXPORT bool _stdcall PeerConnectionGetRemoteDescription(intptr_t obj, intptr_t desc);

GXP_EXPORT bool _stdcall PeerConnectionGetPendingLocalDescription(intptr_t obj, intptr_t desc);

GXP_EXPORT bool _stdcall PeerConnectionGetPendingRemoteDescription(intptr_t obj, intptr_t desc);

GXP_EXPORT bool _stdcall PeerConnectionGetCurrentLocalDescription(intptr_t obj, intptr_t desc);

GXP_EXPORT bool _stdcall PeerConnectionGetCurrentRemoteDescription(intptr_t obj, intptr_t desc);

GXP_EXPORT void* _stdcall PeerConnectionGetReceivers(intptr_t context, intptr_t obj, size_t* length);

GXP_EXPORT void* _stdcall PeerConnectionGetSenders(intptr_t context, intptr_t obj, size_t* length);

GXP_EXPORT void* _stdcall PeerConnectionGetTransceivers(intptr_t context, intptr_t obj, size_t* length);

GXP_EXPORT void _stdcall PeerConnectionCreateOffer(intptr_t obj, const intptr_t options);

GXP_EXPORT void _stdcall PeerConnectionCreateAnswer(intptr_t obj, const intptr_t options);

GXP_EXPORT intptr_t _stdcall ContextCreateDataChannel(intptr_t ctx, intptr_t obj, const char* label, const intptr_t options);

GXP_EXPORT void _stdcall ContextDeleteDataChannel(intptr_t ctx, intptr_t channel);

GXP_EXPORT void _stdcall PeerConnectionRegisterIceConnectionChange(intptr_t obj, void *callback);

GXP_EXPORT void _stdcall PeerConnectionRegisterIceGatheringChange(intptr_t obj, void *callback);

GXP_EXPORT void _stdcall PeerConnectionRegisterConnectionStateChange(intptr_t obj, void *callback);

GXP_EXPORT void _stdcall PeerConnectionRegisterOnIceCandidate(intptr_t obj, void *callback);

GXP_EXPORT void _stdcall PeerConnectionRegisterCallbackCollectStats(intptr_t context, void *onGetStats);

GXP_EXPORT void _stdcall PeerConnectionRegisterCallbackCreateSD(intptr_t obj, void *onSuccess, void *onFailure);

GXP_EXPORT void _stdcall PeerConnectionRegisterOnSetSessionDescSuccess(intptr_t context, intptr_t obj, void *onSuccess);

GXP_EXPORT void _stdcall PeerConnectionRegisterOnSetSessionDescFailure(intptr_t context, intptr_t obj, void *onFailure);

GXP_EXPORT bool _stdcall PeerConnectionAddIceCandidate(intptr_t obj, const intptr_t candidate);

GXP_EXPORT intptr_t _stdcall CreateIceCandidate(const intptr_t options, intptr_t candidate);

GXP_EXPORT void _stdcall DeleteIceCandidate(intptr_t candidate);

GXP_EXPORT void _stdcall IceCandidateGetCandidate(const intptr_t candidate, intptr_t dst);

GXP_EXPORT intptr_t _stdcall IceCandidateGetSdpLineIndex(const intptr_t candidate);

GXP_EXPORT const char* _stdcall IceCandidateGetSdp(const intptr_t candidate);

GXP_EXPORT const char* _stdcall IceCandidateGetSdpMid(const intptr_t candidate);

GXP_EXPORT intptr_t _stdcall PeerConnectionState(intptr_t obj);

GXP_EXPORT intptr_t _stdcall PeerConnectionIceConditionState(intptr_t obj);

GXP_EXPORT intptr_t _stdcall PeerConnectionSignalingState(intptr_t obj);

GXP_EXPORT intptr_t _stdcall PeerConnectionIceGatheringState(intptr_t obj);

GXP_EXPORT void _stdcall PeerConnectionRegisterOnDataChannel(intptr_t obj, void *callback);

GXP_EXPORT void _stdcall PeerConnectionRegisterOnRenegotiationNeeded(intptr_t obj, void *callback);

GXP_EXPORT void _stdcall PeerConnectionRegisterOnTrack(intptr_t obj, void *callback);

GXP_EXPORT void _stdcall PeerConnectionRegisterOnRemoveTrack(intptr_t obj, void *callback);

GXP_EXPORT bool _stdcall TransceiverGetCurrentDirection(intptr_t transceiver, intptr_t direction);

GXP_EXPORT intptr_t _stdcall TransceiverStop(intptr_t transceiver);

GXP_EXPORT intptr_t _stdcall TransceiverGetDirection(intptr_t transceiver);

GXP_EXPORT void _stdcall TransceiverSetDirection(intptr_t transceiver, intptr_t direction);

GXP_EXPORT intptr_t _stdcall TransceiverSetCodecPreferences(intptr_t transceiver, intptr_t codecs, size_t length);

GXP_EXPORT intptr_t _stdcall TransceiverGetReceiver(intptr_t transceiver);

GXP_EXPORT intptr_t _stdcall TransceiverGetSender(intptr_t transceiver);

GXP_EXPORT void _stdcall SenderGetParameters(intptr_t sender, intptr_t parameters);

GXP_EXPORT intptr_t _stdcall SenderSetParameters(intptr_t sender, const intptr_t src);

GXP_EXPORT void _stdcall ContextGetSenderCapabilities(
    intptr_t context, intptr_t trackKind, intptr_t parameters);

GXP_EXPORT void _stdcall ContextGetReceiverCapabilities(
    intptr_t context, intptr_t trackKind, intptr_t parameters);

GXP_EXPORT bool _stdcall SenderReplaceTrack(intptr_t sender, intptr_t track);

GXP_EXPORT intptr_t _stdcall SenderGetTrack(intptr_t sender);

GXP_EXPORT intptr_t _stdcall ReceiverGetTrack(intptr_t receiver);

GXP_EXPORT void* _stdcall ReceiverGetStreams(intptr_t receiver, size_t* length);

GXP_EXPORT intptr_t _stdcall DataChannelGetID(intptr_t dataChannelObj);

GXP_EXPORT char* _stdcall DataChannelGetLabel(intptr_t dataChannelObj);

GXP_EXPORT char* _stdcall DataChannelGetProtocol(intptr_t dataChannelObj);

GXP_EXPORT unsigned int _stdcall DataChannelGetMaxRetransmits(intptr_t dataChannelObj);

GXP_EXPORT unsigned int _stdcall DataChannelGetMaxRetransmitTime(intptr_t dataChannelObj);

GXP_EXPORT bool _stdcall DataChannelGetOrdered(intptr_t dataChannelObj);

GXP_EXPORT uint64_t _stdcall DataChannelGetBufferedAmount(intptr_t dataChannelObj);

GXP_EXPORT bool _stdcall DataChannelGetNegotiated(intptr_t dataChannelObj);

GXP_EXPORT intptr_t _stdcall DataChannelGetReadyState(
    intptr_t dataChannelObj);

GXP_EXPORT void _stdcall DataChannelSend(intptr_t dataChannelObj, const char* msg);

GXP_EXPORT void _stdcall DataChannelSendBinary(intptr_t dataChannelObj, const unsigned char* msg, int len);

GXP_EXPORT void _stdcall DataChannelClose(intptr_t dataChannelObj);

GXP_EXPORT void _stdcall DataChannelRegisterOnMessage(intptr_t dataChannelObj, void *callback);

GXP_EXPORT void _stdcall DataChannelRegisterOnOpen(intptr_t dataChannelObj, void *callback);

GXP_EXPORT void _stdcall DataChannelRegisterOnClose(intptr_t dataChannelObj, void *callback);

GXP_EXPORT void _stdcall SetCurrentContext(intptr_t context);

GXP_EXPORT void _stdcall ProcessAudio(
    intptr_t track,
    float* audio_data,
    int sample_rate,
    size_t number_of_channels,
    size_t number_of_frames);

GXP_EXPORT void _stdcall ContextRegisterAudioReceiveCallback(
    intptr_t context, intptr_t track, void *callback);

GXP_EXPORT void _stdcall ContextUnregisterAudioReceiveCallback(
    intptr_t context, intptr_t track);

GXP_EXPORT const char* _stdcall GetWrapperVersion();