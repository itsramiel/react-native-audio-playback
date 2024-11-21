import { Image, NativeModules, Platform } from 'react-native';
import type { Spec } from './NativeAudioPlayback';

const LINKING_ERROR =
  `The package 'react-native-audio-playback' doesn't seem to be linked. Make sure: \n\n` +
  Platform.select({ ios: "- You have run 'pod install'\n", default: '' }) +
  '- You rebuilt the app after installing the package\n' +
  '- You are not using Expo Go\n';

// @ts-expect-error
const isTurboModuleEnabled = global.__turboModuleProxy != null;

const AudioPlaybackModule = isTurboModuleEnabled
  ? require('./NativeAudioPlayback').default
  : NativeModules.AudioPlayback;

const AudioPlayback: Spec = AudioPlaybackModule
  ? AudioPlaybackModule
  : new Proxy(
      {},
      {
        get() {
          throw new Error(LINKING_ERROR);
        },
      }
    );

export function setupAudioStream(
  sampleRate: number,
  channelCount: number
): void {
  return AudioPlayback.setupAudioStream(sampleRate, channelCount);
}

export function openAudioStream(): void {
  return AudioPlayback.openAudioStream();
}

export function playSounds(arg: Array<[string, boolean]>): void {
  return AudioPlayback.playSounds(arg);
}

export function seekSoundsTo(arg: Array<[string, number]>): void {
  return AudioPlayback.seekSoundsTo(arg);
}

export function loopSounds(arg: Array<[string, boolean]>): void {
  return AudioPlayback.loopSounds(arg);
}

export function loadSound(requiredAsset: number): Promise<string | null> {
  return AudioPlayback.loadSound(Image.resolveAssetSource(requiredAsset).uri);
}

export function unloadSound(playerId: string) {
  AudioPlayback.unloadSound(playerId);
}
