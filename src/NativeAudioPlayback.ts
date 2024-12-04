import type { TurboModule } from 'react-native';
import { TurboModuleRegistry } from 'react-native';

export interface Spec extends TurboModule {
  setupAudioStream: (
    sampleRate: number,
    channelCount: number
  ) => { error: string | null };
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
}

export default TurboModuleRegistry.getEnforcing<Spec>('AudioPlayback');
