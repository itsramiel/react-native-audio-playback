# react-native-audio-playback

A react native library to play single/multi low-latency audio on android/iOS.

For Android, it uses [Google's C++ Oboe](https://github.com/google/oboe)

For iOS, it uses [Core Audio](https://developer.apple.com/library/archive/documentation/MusicAudio/Conceptual/CoreAudioOverview/Introduction/Introduction.html)'s [Audio Unit](https://developer.apple.com/library/archive/documentation/MusicAudio/Conceptual/AudioUnitProgrammingGuide/Introduction/Introduction.html)

## Installation

npm:
```sh
npm install react-native-audio-playback
```

yarn:
```sh
yarn add react-native-audio-playback
```

## Usage

1. Instantiate an `AudioManager` class instance passing in `sampleRate` and `channelCount` to the constructor.

[How do I know what `sampleRate` and `channelCount` I need to pass?](#sample-rates-and-channel-counts)

```ts
import { AudioManager } from 'react-native-audio-playback';

const audioManager = new AudioManager(44100, 2);
```

2. Load in your audio sounds as such:

```ts
const sound1 = audioManager.loadSound(require('./assets/sound1.wav'))
const sound2 = audioManager.loadSound(require('./assets/sound2.wav'))
```

3. Open the audio stream from the audo manager:

Note: You are able to open the audio stream before loading sounds

```ts
audioManager.openAudioStream();
```

From here you can manipulate the sounds individually:

```ts
player1.loopSound(true); // boolean whether to loop or not
player1.playSound();
player1.pauseSound();
player1.seekTo(1000) // timeInMs
```

or you can manipulate the same property for different sounds at once:
```ts
// multi play
audioManager.playSounds([[player1, true], [player2, true]]);
audioManager.playSounds([[player1, true], [player2, false]]); // you can play a sound while pausing the other

// multi loop
audioManager.loopSounds([[player1, true], [player2, true]]);
```

Unload the sounds when you no longer need them:
```ts
player1.unloadSound();
player2.unloadSound();
```

## API

### AudioManager

The AudioManager is used to open/close the audio stream, create audio sounds, manipulate multiple sounds simultaneously:
You create an instance of the AudioManager by calling its constructor and passing in the `sampleRate` and `channelCount`:

```ts
const audioStream = AudioManager(44100, 2)
```

#### Methods:

- `openAudioStream(): void`: Opens the audio stream to allow audio to be played
- `loadSound(requiredAsset: number): Player`: Loads a local audio sound and returns a `Player` instance
- `playSounds(args: Array<[Player, boolean]>): void` Plays/pauses multiple sounds
- `loopSounds(args: Array<[Player, boolean]>): void` Loops/unloops multiple sounds
- `seekSoundsTo(args: Array<[Player, number]>): void` Seeks multiple sounds

### Player
The `Player` class is used to manage a single sound created by an `AudioManager`.

#### Methods:
- `playSound(): void`: Plays the sound. If the sound is already playing it does nothing. If the sound is paused, it resumes it.
- `pauseSound(): void`: Pauses the sound
- `seekTo(timeInMs: number): void`: Seeks the sound to a given time in Milliseconds
- `unloadSound(): void`: Unloads the audio memory, so the Player is useless after this point. 

## Sample Rates and Channel Counts

If you don't know what is a `Sample Rate` or `Channel Count` and seem to be off-put bey them! **Don't be**.


While these terms can be intimidating, it is really simple to understand enough to get this library working. 

### Sample Rate:

You most likely will work with audio files of sample rate of `44100` or `48000`. To find out what sample rate your audio is in, you can use ffmpeg:

```sh
ffprobe -v error -select_streams a:0 -show_entries stream=sample_rate -of default=noprint_wrappers=1:nokey=1 <your-audio-file>.<ext>
```
If your audio files are of different sample rates, you can easily convert them to have them all be the same sample rate. My recommendation is convert the higher sample rates to the lower ones. So convert your 48000s to 44100s like this:
```sh
ffmpeg -i <your-audio-file>.ext -ar 44100 <new-name-for-converted-file>.<ext>
```

### Channel Count:

The most commont channel counts used for mobile is `mono`,1, or `stereo`, 2. In my opinion, most of the times you want to go with 2.

To know how many channels your audio file has:
```sh
ffprobe -v error -select_streams a:0 -show_entries stream=channels -of default=noprint_wrappers=1:nokey=1 <your-audio-file>.<ext>
```
If your audio files are of different channel counts, you can easily convert them to have them all be the same channel count. My recommendation is convert all the audio files with 1 channel to 2 channels like this:
```sh
ffmpeg -i <your-audio-file>.<ext> -ac 2 <new-name-for-converted-file>.<ext>
```



## Contributing

See the [contributing guide](CONTRIBUTING.md) to learn how to contribute to the repository and the development workflow.

## License

MIT

---

Made with [create-react-native-library](https://github.com/callstack/react-native-builder-bob)
