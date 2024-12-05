import { StyleSheet, View } from 'react-native';
import Slider from '@react-native-community/slider';
import { type Player } from 'react-native-audio-playback';

import { Button } from './Button';
import { Section } from './Section';

interface MultiPlayersControlProps {
  playerObjects: Array<{ title: string; player: Player; selected: boolean }>;
  onPlayerPress: (player: Player) => void;
}

export function MultiPlayersControl({
  playerObjects,
  onPlayerPress,
}: MultiPlayersControlProps) {
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
    <Section title="Multi Players Control">
      <View style={styles.playersSelectionContainer}>
        {playerObjects.map((playerObject) => (
          <Button
            key={playerObject.player.id}
            title={`${playerObject.title} ${playerObject.selected ? '✅' : '❌'}`}
            onPress={() => onPlayerPress(playerObject.player)}
          />
        ))}
      </View>
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
  playersSelectionContainer: {
    flexDirection: 'row',
    flexWrap: 'wrap',
    gap: 8,
    width: '100%',
  },
  volumeSlider: {
    width: '100%',
  },
});
