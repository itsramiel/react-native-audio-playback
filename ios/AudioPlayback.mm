#import "AudioPlayback.h"

#import "react_native_audio_playback-Swift.h"

@implementation AudioPlayback {
  AudioPlaybackImpl *moduleImpl;
}

- (instancetype) init {
  self = [super init];
  if (self) {
    moduleImpl = [AudioPlaybackImpl new];
  }
  return self;
}

RCT_EXPORT_MODULE()

RCT_EXPORT_METHOD(setupAudioStream:(double)sampleRate channelCount:(double)channelCount) {
  [moduleImpl setupAudioStreamWithSampleRate:sampleRate channelCount:channelCount];
}

RCT_EXPORT_METHOD(openAudioStream) {
  [moduleImpl openAudioStream];
}

RCT_EXPORT_METHOD(loadSound:(NSString *)uri resolve:(RCTPromiseResolveBlock)resolve reject:(RCTPromiseRejectBlock)reject) {
  [moduleImpl loadSoundWithUri:uri completion:^(NSString * _Nullable soundId) {
    resolve(soundId);
  }];
}

RCT_EXPORT_METHOD(unloadSound:(NSString *)id) {
  [moduleImpl unloadSoundWithId:id];
}

RCT_EXPORT_METHOD(loopSounds:(NSArray *)arg) {
  [moduleImpl loopSoundsWithArg:arg];
}


RCT_EXPORT_METHOD(playSounds:(NSArray *)arg) {
  [moduleImpl playSoundsWithArg:arg];
}


RCT_EXPORT_METHOD(seekSoundsTo:(NSArray *)arg) {
  [moduleImpl seekSoundsToArg:arg];
}


// Don't compile this code when we build for the old architecture.
#ifdef RCT_NEW_ARCH_ENABLED
- (std::shared_ptr<facebook::react::TurboModule>)getTurboModule:
    (const facebook::react::ObjCTurboModule::InitParams &)params
{
    return std::make_shared<facebook::react::NativeAudioPlaybackSpecJSI>(params);
}
#endif

@end
