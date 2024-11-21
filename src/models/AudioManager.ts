import {
  loadSound,
  loopSounds,
  openAudioStream,
  playSounds,
  seekSoundsTo,
  setupAudioStream,
} from '../module';
import { Player } from './Player';

export class AudioManager {
  constructor(sampleRate: number = 44100, channelCount: number = 2) {
    setupAudioStream(sampleRate, channelCount);
  }

  public openAudioStream() {
    openAudioStream();
  }

  public async loadSound(asset: string) {
    const id = await loadSound(asset);
    return id ? new Player(id) : null;
  }

  loopSounds(args: Array<[Player, boolean]>) {
    loopSounds(args.map(([player, loop]) => [player.id, loop]));
  }

  playSounds(args: Array<[Player, boolean]>) {
    playSounds(args.map(([player, loop]) => [player.id, loop]));
  }

  seekSoundsTo(args: Array<[Player, number]>) {
    seekSoundsTo(args.map(([player, timeInMs]) => [player.id, timeInMs]));
  }
}
