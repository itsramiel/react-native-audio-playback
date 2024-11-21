import { loopSounds, playSounds, seekSoundsTo, unloadSound } from '../module';

export class Player {
  public readonly id: string;

  constructor(id: string) {
    this.id = id;
  }

  public unloadSound(): void {
    unloadSound(this.id);
  }

  public loopSound(value: boolean): void {
    loopSounds([[this.id, value]]);
  }

  public playSound(): void {
    playSounds([[this.id, true]]);
  }

  public pauseSound(): void {
    playSounds([[this.id, false]]);
  }

  public seekTo(timeInMs: number): void {
    seekSoundsTo([[this.id, timeInMs]]);
  }
}
