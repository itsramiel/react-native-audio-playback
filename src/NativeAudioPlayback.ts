import type { TurboModule } from 'react-native';
import { TurboModuleRegistry } from 'react-native';

export interface Spec extends TurboModule {
  setupAudioStream: (options: {
    sampleRate: number;
    channelCount: number;
    ios: {
      audioSessionCategory: number;
    };
    android: {
      usage: number;
    };
  }) => { error: string | null };
  openAudioStream: () => { error: string | null };
  pauseAudioStream: () => { error: string | null };
  closeAudioStream: () => { error: string | null };
  loopSounds: (arg: Array<[string, boolean]>) => void;
  playSounds: (arg: Array<[string, boolean]>) => void;
  seekSoundsTo: (arg: Array<[string, number]>) => void;
  setSoundsVolume: (arg: Array<[string, number]>) => void;
  unloadSound: (id: string) => void;
  loadSound: (
    uri: string
  ) => Promise<{ id: string | null; error: string | null }>;
  getStreamState: () => number;
}

export default TurboModuleRegistry.getEnforcing<Spec>('AudioPlayback');
