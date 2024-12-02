import {
  closeAudioStream,
  loadSound,
  loopSounds,
  openAudioStream,
  pauseAudioStream,
  playSounds,
  seekSoundsTo,
  setSoundsVolume,
  setupAudioStream,
} from '../module';
import { Player } from './Player';

export class AudioManager {
  public static shared = new AudioManager();

  private constructor() {}

  public setupAudioStream(
    sampleRate: number = 44100,
    channelCount: number = 2
  ) {
    setupAudioStream(sampleRate, channelCount);
  }

  public openAudioStream(): void {
    openAudioStream();
  }

  public pauseAudioStream(): void {
    pauseAudioStream();
  }

  public closeAudioStream(): void {
    closeAudioStream();
  }

  public async loadSound(asset: number) {
    const id = await loadSound(asset);
    return id ? new Player(id) : null;
  }

  public loopSounds(args: ReadonlyArray<[Player, boolean]>): void {
    loopSounds(args.map(([player, loop]) => [player.id, loop]));
  }

  public playSounds(args: ReadonlyArray<[Player, boolean]>): void {
    playSounds(args.map(([player, loop]) => [player.id, loop]));
  }

  public seekSoundsTo(args: ReadonlyArray<[Player, number]>): void {
    seekSoundsTo(args.map(([player, timeInMs]) => [player.id, timeInMs]));
  }

  public setSoundsVolume(args: ReadonlyArray<[Player, number]>): void {
    setSoundsVolume(args.map(([player, volume]) => [player.id, volume]));
  }
}
