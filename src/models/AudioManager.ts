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

import { AndroidAudioStreamUsage, IosAudioSessionCategory } from '../types';
import { Player } from './Player';

export class AudioManager {
  public static shared = new AudioManager();

  private constructor() {}

  public setupAudioStream(options?: {
    sampleRate?: number;
    channelCount?: number;
    ios?: {
      audioSessionCategory?: IosAudioSessionCategory;
    };
    android?: {
      usage?: AndroidAudioStreamUsage;
    };
  }) {
    const sampleRate = options?.sampleRate ?? 44100;
    const channelCount = options?.channelCount ?? 2;
    const iosAudioSessionCategory =
      options?.ios?.audioSessionCategory ?? IosAudioSessionCategory.Playback;
    const androidUsage =
      options?.android?.usage ?? AndroidAudioStreamUsage.Media;

    setupAudioStream({
      channelCount,
      sampleRate,
      ios: {
        audioSessionCategory: iosAudioSessionCategory,
      },
      android: {
        usage: androidUsage,
      },
    });
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
