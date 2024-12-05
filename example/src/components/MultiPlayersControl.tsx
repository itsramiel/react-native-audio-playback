import { StyleSheet, View } from 'react-native';
import Slider from '@react-native-community/slider';
import { AudioManager, type Player } from 'react-native-audio-playback';

import { Button } from './Button';
import { Section } from './Section';

interface MultiPlayersControlProps {
  players: Array<{ title: string; player: Player; selected: boolean }>;
  onPlayerPress: (player: Player) => void;
}

export function MultiPlayersControl({
  players,
  onPlayerPress,
}: MultiPlayersControlProps) {
  const selectedPlayers = players.filter((p) => p.selected);

  function onPlay() {
    AudioManager.shared.playSounds(
      selectedPlayers.map((p) => [p.player, true] as const)
    );
  }

  function onPause() {
    AudioManager.shared.playSounds(
      selectedPlayers.map((p) => [p.player, false] as const)
    );
  }

  function onSeekTo() {
    AudioManager.shared.seekSoundsTo(
      selectedPlayers.map((p) => [p.player, 0] as const)
    );
  }

  function onLoop() {
    AudioManager.shared.loopSounds(
      selectedPlayers.map((p) => [p.player, true] as const)
    );
  }

  function onUnloop() {
    AudioManager.shared.loopSounds(
      selectedPlayers.map((p) => [p.player, false] as const)
    );
  }

  function onVolumeChange(value: number) {
    AudioManager.shared.setSoundsVolume(
      selectedPlayers.map((p) => [p.player, value] as const)
    );
  }

  return (
    <Section title="Multi Players Control">
      <View style={styles.playersSelectionContainer}>
        {players.map((playerObject) => (
          <Button
            key={playerObject.player.id}
            title={`${playerObject.title} ${playerObject.selected ? '✅' : '❌'}`}
            onPress={() => onPlayerPress(playerObject.player)}
          />
        ))}
      </View>
      <Button title="Play" onPress={onPlay} />
      <Button title="Pause" onPress={onPause} />
      <Button title="Seek To Start" onPress={onSeekTo} />
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
