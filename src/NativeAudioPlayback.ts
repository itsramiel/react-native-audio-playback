import type { TurboModule } from 'react-native';
import { TurboModuleRegistry } from 'react-native';

export interface Spec extends TurboModule {
  setupAudioStream: (sampleRate: number, channelCount: number) => void;
  openAudioStream: () => void;
  loopSounds: (arg: Array<[string, boolean]>) => void;
  playSounds: (arg: Array<[string, boolean]>) => void;
  seekSoundsTo: (arg: Array<[string, number]>) => void;
  unloadSound: (id: string) => void;
  loadSound: (uri: string) => Promise<string | null>;
}

export default TurboModuleRegistry.getEnforcing<Spec>('AudioPlayback');
