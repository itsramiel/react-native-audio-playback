import { StyleSheet } from 'react-native';
import Slider from '@react-native-community/slider';
import type { Player } from 'react-native-audio-playback';

import { Button } from './Button';
import { Section } from './Section';

interface PlayerControlProps {
  player: Player;
  title: string;
}

export function PlayerControl({ player, title }: PlayerControlProps) {
  function onPlay() {
    // Todo
  }

  function onPause() {
    // Todo
  }

  function onSeekToStart() {
    // Todo
  }

  function onLoop() {
    // Todo
  }

  function onUnloop() {
    // Todo
  }

  function onVolumeChange(value: number) {
    // Todo
  }

  return (
    <Section title={title}>
      <Button title="Play" onPress={onPlay} />
      <Button title="Pause" onPress={onPause} />
      <Button title="Seek To Start" onPress={onSeekToStart} />
      <Button title="loop" onPress={onLoop} />
      <Button title="unloop" onPress={onUnloop} />
      <Slider
        onValueChange={onVolumeChange}
        style={styles.volumeSlider}
        minimumValue={0}
        maximumValue={1}
        step={0.1}
        value={1}
      />
    </Section>
  );
}

const styles = StyleSheet.create({
  volumeSlider: {
    width: '100%',
  },
});
