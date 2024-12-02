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
  const res = AudioPlayback.setupAudioStream(sampleRate, channelCount);
  if (res.error) {
    throw new Error(res.error);
  }
}

export function openAudioStream(): void {
  const res = AudioPlayback.openAudioStream();
  if (res.error) {
    throw new Error(res.error);
  }
}

export function pauseAudioStream(): void {
  const res = AudioPlayback.pauseAudioStream();
  if (res.error) {
    throw new Error(res.error);
  }
}

export function closeAudioStream(): void {
  const res = AudioPlayback.closeAudioStream();
  if (res.error) {
    throw new Error(res.error);
  }
}

export function playSounds(arg: Array<[string, boolean]>): void {
  AudioPlayback.playSounds(arg);
}

export function seekSoundsTo(arg: Array<[string, number]>): void {
  AudioPlayback.seekSoundsTo(arg);
}

export function loopSounds(arg: Array<[string, boolean]>): void {
  AudioPlayback.loopSounds(arg);
}

export function setSoundsVolume(arg: Array<[string, number]>): void {
  for (const [_, volume] of arg) {
    if (volume < 0 || volume > 1) {
      throw new Error('Volume must be between 0 and 1');
    }
  }

  AudioPlayback.setSoundsVolume(arg);
}

export async function loadSound(requiredAsset: number): Promise<string> {
  const res = await AudioPlayback.loadSound(
    Image.resolveAssetSource(requiredAsset).uri
  );
  if (res.error) {
    throw new Error(res.error);
  } else if (typeof res.id !== 'string') {
    throw new Error(
      'An unknown error occurred while loading the audio file. Please create an issue with a reproducible'
    );
  }
  return res.id;
}

export function unloadSound(playerId: string) {
  const res = AudioPlayback.unloadSound(playerId);
  if (res.error) {
    throw new Error(res.error);
  }
}
