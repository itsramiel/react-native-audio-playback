
#ifdef RCT_NEW_ARCH_ENABLED
#import "RNAudioPlaybackSpec.h"

@interface AudioPlayback : NSObject <NativeAudioPlaybackSpec>
#else
#import <React/RCTBridgeModule.h>

@interface AudioPlayback : NSObject <RCTBridgeModule>
#endif

@end
