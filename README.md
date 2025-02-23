# react-native-audio-playback

⚡ Highest perf, lowest-latency react native library to play sounds on iOS/Android

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

For iOS, run `pod install` in the `ios` directory.

## Usage

1. Setup an Audio Stream using the singleton `AudioManager`'s `shared` static property and calling its `setupAudioStream`.
   `setupAudioStream` takes in an optional options argument where you can pass in the `sampleRate` and `channelCount` of your audio files, or do not specify them and it will default to `44100` and `2` respectively.

[How do I know what `sampleRate` and `channelCount` I need to pass?](#sample-rates-and-channel-counts)

```ts
import { AudioManager } from 'react-native-audio-playback';

AudioManager.shared.setupAudioStream({ sampleRate: 44100, channelCount: 2 });
```

2. Load in your audio sounds as such:

```ts
AudioManager.shared.loadSound(require('./assets/sound1.wav'));
AudioManager.shared.loadSound(require('./assets/sound2.wav'));
```

3. Open the audio stream from the audo manager:

```ts
AudioManager.shared.openAudioStream();
```

From here you can manipulate the sounds individually:

```ts
player1.loopSound(true); // boolean whether to loop or not
player1.playSound();
player1.pauseSound();
player1.seekTo(1000); // timeInMs
```

or you can manipulate the same property for different sounds at once:

```ts
// multi play
AudioManager.shared.playSounds([
  [player1, true],
  [player2, true],
]);
AudioManager.shared.playSounds([
  [player1, true],
  [player2, false],
]); // you can play a sound while pausing the other

// multi loop
AudioManager.shared.loopSounds([
  [player1, true],
  [player2, true],
]);
```

Unload the sounds when you no longer need them:

```ts
player1.unloadSound();
player2.unloadSound();
```

## API

### AudioManager

The AudioManager is used to setup, open, close the audio stream, create audio sounds, manipulate multiple sounds simultaneously:
The `AudioManager` class is a singleton and can be accessed with its static `shared` property:

```ts
AudioManager.shared.<some-method>
```

#### Methods:

- `setupAudioStream(options?: {
  sampleRate?: number;
  channelCount?: number;
  ios?: {
    audioSessionCategory?: IosAudioSessionCategory;
  };
  android?: {
    usage?: AndroidAudioStreamUsage;
  };
}): void`: sets up the Audio Stream to allow it later be opened.
  Notes:
  1. You shouldn't setup multiple streams simultaneously because you only need one stream. Trying to setup another one will simply fails because there is already one setup.
  2. You can change the ios audio session category using the `audioSessionCategory` option in the `ios` object. Check [apple docs](https://developer.apple.com/documentation/avfaudio/avaudiosession/category-swift.struct#Getting-Standard-Categories) for more info on the different audio session categories.
  3. You can change the android usage using the `usage` option in the `android` object. Check [here](https://github.com/google/oboe/blob/11afdfcd3e1c46dc2ea4b86c83519ebc2d44a1d4/include/oboe/Definitions.h#L316-L377) for the list of options.
- `openAudioStream(): void`: Opens the audio stream to allow audio to be played
  Note: You should have called `setupAudioStream` before calling this method. You can't open a stream that hasn't been setup
- `pauseAudioStream(): void`: Pauses the audio stream (An example of when to use this is when user puts app to background)
  Note: The stream has to be in open state. You cant pause a non open stream
- `closeAudioStream(): void`: Closes the audio stream
  Note: After this, you need to resetup the audio stream and then repon it to play sounds. The loaded sounds are still loaded and you dont have to reload them.
- `loadSound(requiredAsset: number): Player`: Loads a local audio sound and returns a `Player` instance
- `playSounds(args: ReadonlyArray<[Player, boolean]>): void` Plays/pauses multiple sounds
- `loopSounds(args: ReadonlyArray<[Player, boolean]>): void` Loops/unloops multiple sounds
- `seekSoundsTo(args: ReadonlyArray<[Player, number]>): void` Seeks multiple sounds
- `setSoundsVolume(args: ReadonlyArray<[Player, number]>): void` Sets the volume of multiple sounds, volume should be a number between 0 and 1.
- `getStreamState(): StreamState` Returns the current state of the stream.

### Player

The `Player` class is used to manage a single sound created by an `AudioManager`.

#### Methods:

- `playSound(): void`: Plays the sound. If the sound is already playing it does nothing. If the sound is paused, it resumes it.
- `pauseSound(): void`: Pauses the sound
- `seekTo(timeInMs: number): void`: Seeks the sound to a given time in Milliseconds
- `setVolume(volume: number): void`: Sets the volume of the sound, volume should be a number between 0 and 1.
- `unloadSound(): void`: Unloads the audio memory, so the Player is useless after this point.

## Sample Rates and Channel Counts

If you don't know what is a `Sample Rate` or `Channel Count` and seem to be off-put by them! **Don't be**.

While these terms can be intimidating, it is really simple to understand enough to get this library working. One important thing to note is that all of your audio files should be in the same sample rate and channel count.

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
