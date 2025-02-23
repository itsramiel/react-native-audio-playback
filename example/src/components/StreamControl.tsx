import {
  AndroidAudioStreamUsage,
  AudioManager,
  StreamState,
} from 'react-native-audio-playback';

import { Button } from './Button';
import { Section } from './Section';
import { Alert } from 'react-native';

interface StreamControlProps {
  onLoadSounds: () => void;
}

export function StreamControl({ onLoadSounds }: StreamControlProps) {
  function onSetupStream() {
    AudioManager.shared.setupAudioStream({
      android: {
        usage: AndroidAudioStreamUsage.Alarm,
      },
    });
  }

  function onOpenStream() {
    AudioManager.shared.openAudioStream();
  }

  function onPauseStream() {
    AudioManager.shared.pauseAudioStream();
  }

  function onCloseStream() {
    AudioManager.shared.closeAudioStream();
  }

  function onGetStreamState() {
    const streamState = AudioManager.shared.getStreamState();
    const streamStateString = (() => {
      switch (streamState) {
        case StreamState.closed:
          return 'closed';
        case StreamState.initialized:
          return 'initialized';
        case StreamState.open:
          return 'open';
        case StreamState.paused:
          return 'paused';
      }
    })();

    Alert.alert('Stream State', streamStateString);
  }

  return (
    <Section title="Audio Manager">
      <Button title="Setup Stream" onPress={onSetupStream} />
      <Button title="Open Stream" onPress={onOpenStream} />
      <Button title="Pause Stream" onPress={onPauseStream} />
      <Button title="Close Stream" onPress={onCloseStream} />
      <Button title="Load Sounds" onPress={onLoadSounds} />
      <Button title="Get Stream State" onPress={onGetStreamState} />
    </Section>
  );
}
