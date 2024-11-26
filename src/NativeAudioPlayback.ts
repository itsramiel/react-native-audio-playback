import type { TurboModule } from 'react-native';
import { TurboModuleRegistry } from 'react-native';

export interface Spec extends TurboModule {
  setupAudioStream: (
    sampleRate: number,
    channelCount: number
  ) => { error: string | null };
  openAudioStream: () => { error: string | null };
  closeAudioStream: () => { error: string | null };
  loopSounds: (arg: Array<[string, boolean]>) => { error: string | null };
  playSounds: (arg: Array<[string, boolean]>) => { error: string | null };
  seekSoundsTo: (arg: Array<[string, number]>) => { error: string | null };
  unloadSound: (id: string) => { error: string | null };
  loadSound: (
    uri: string
  ) => Promise<{ id: string | null; error: string | null }>;
}

export default TurboModuleRegistry.getEnforcing<Spec>('AudioPlayback');
