import { AudioManager } from 'react-native-audio-playback';

import { Button } from './Button';
import { Section } from './Section';

interface StreamControlProps {
  onLoadSounds: () => void;
}

export function StreamControl({ onLoadSounds }: StreamControlProps) {
  function onSetupStream() {
    AudioManager.shared.setupAudioStream();
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

  return (
    <Section title="Audio Manager">
      <Button title="Setup Stream" onPress={onSetupStream} />
      <Button title="Open Stream" onPress={onOpenStream} />
      <Button title="Pause Stream" onPress={onPauseStream} />
      <Button title="Close Stream" onPress={onCloseStream} />
      <Button title="Load Sounds" onPress={onLoadSounds} />
    </Section>
  );
}
