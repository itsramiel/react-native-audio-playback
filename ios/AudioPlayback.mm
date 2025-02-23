#import "AudioPlayback.h"

#if __has_include("react_native_audio_playback-Swift.h")
#import "react_native_audio_playback-Swift.h"
#else
#import "react_native_audio_playback/react_native_audio_playback-Swift.h"
#endif

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

RCT_EXPORT_SYNCHRONOUS_TYPED_METHOD(NSDictionary *, setupAudioStream:(JS::NativeAudioPlayback::SpecSetupAudioStreamOptions &)options) {
  double sampleRate = options.sampleRate();
  double channelCount = options.channelCount();
  double audioSessionCategory = options.ios().audioSessionCategory();


  NSString *error = [moduleImpl setupAudioStreamWithSampleRate:sampleRate channelCount:channelCount audioSessionCategory: audioSessionCategory];

  return @{@"error":error?: [NSNull null]};
}

RCT_EXPORT_SYNCHRONOUS_TYPED_METHOD(NSDictionary *, openAudioStream) {
  NSString *error = [moduleImpl openAudioStream];

  return @{@"error":error?: [NSNull null]};
}

RCT_EXPORT_SYNCHRONOUS_TYPED_METHOD(NSDictionary *, pauseAudioStream) {
  NSString *error = [moduleImpl pauseAudioStream];

  return @{@"error":error?: [NSNull null]};
}

RCT_EXPORT_SYNCHRONOUS_TYPED_METHOD(NSDictionary *, closeAudioStream) {
  NSString *error = [moduleImpl closeAudioStream];

  return @{@"error":error?: [NSNull null]};
}

RCT_EXPORT_METHOD(loadSound:(NSString *)uri resolve:(RCTPromiseResolveBlock)resolve reject:(RCTPromiseRejectBlock)reject) {
  [moduleImpl loadSoundWithUri:uri completion:^(NSString * _Nullable soundId, NSString * _Nullable error) {
    resolve(@{@"error": error?: [NSNull null], @"id": soundId?: [NSNull null]});
  }];
}

RCT_EXPORT_SYNCHRONOUS_TYPED_METHOD(NSDictionary *, unloadSound:(NSString *)id) {
  NSString *error = [moduleImpl unloadSoundWithId:id];

  return @{@"error":error?: [NSNull null]};
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

RCT_EXPORT_METHOD(setSoundsVolume:(NSArray *)arg) {
  [moduleImpl setSoundsVolumeWithArg:arg];
}

RCT_EXPORT_SYNCHRONOUS_TYPED_METHOD(NSNumber *, getStreamState) {
  return @([moduleImpl getAudioStreamState]);
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
