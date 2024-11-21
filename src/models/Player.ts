import { loopSounds, playSounds, seekSoundsTo, unloadSound } from '../module';

export class Player {
  public readonly id: string;

  constructor(id: string) {
    this.id = id;
  }

  isLoaded(): this is { id: string } {
    return typeof this.id === 'string';
  }

  async unloadSound() {
    unloadSound(this.id);
  }

  loopSound(value: boolean) {
    loopSounds([[this.id, value]]);
  }

  playSound() {
    playSounds([[this.id, true]]);
  }

  pauseSound() {
    playSounds([[this.id, false]]);
  }

  seekTo(timeInMs: number) {
    seekSoundsTo([[this.id, timeInMs]]);
  }
}
