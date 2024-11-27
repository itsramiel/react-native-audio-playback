import { useCallback, useState } from 'react';
import { StyleSheet, View, Text, ScrollView, SafeAreaView } from 'react-native';
import { Player, AudioManager } from 'react-native-audio-playback';
import { Button, VolumeSlider } from './components';

export default function App() {
  const [players, setPlayers] = useState<
    Array<{ player: Player; title: string }>
  >([]);

  const loadSounds = useCallback(async () => {
    const loadedPlayers = Array<{ player: Player; title: string }>();
    try {
      const player1 = await AudioManager.shared.loadSound(
        require('./assets/bamboo.mp3')
      );
      if (player1) {
        loadedPlayers.push({ player: player1, title: 'Bamboo' });
      }
      const player2 = await AudioManager.shared.loadSound(
        require('./assets/swords.mp3')
      );
      if (player2) {
        loadedPlayers.push({ player: player2, title: 'Swords' });
      }
      const player3 = await AudioManager.shared.loadSound(
        require('./assets/coins.mp3')
      );
      if (player3) {
        loadedPlayers.push({ player: player3, title: 'Coins' });
      }

      const player4 = await AudioManager.shared.loadSound(
        require('./assets/axe.mp3')
      );

      if (player4) {
        loadedPlayers.push({ player: player4, title: 'Axe' });
      }

      setPlayers(loadedPlayers);
    } catch (e) {
      console.log(e);
    }
  }, []);

  const playersExceptAxe = players
    .filter((p) => p.title !== 'Axe')
    .map((player) => player.player);

  return (
    <SafeAreaView style={styles.screen}>
      <ScrollView contentContainerStyle={styles.contentContainer}>
        <ButtonsGroup title="Audio Manager">
          <Button
            title="Setup Stream"
            onPress={() => {
              AudioManager.shared.setupAudioStream(44100, 2);
            }}
          />
          <Button
            title="Open Stream"
            onPress={() => {
              AudioManager.shared.openAudioStream();
            }}
          />
          <Button
            title="Close Stream"
            onPress={() => {
              AudioManager.shared.closeAudioStream();
            }}
          />
          <Button title="Load Sounds" onPress={loadSounds} />
        </ButtonsGroup>
        {players.map(({ title, player }) => (
          <ButtonsGroup title={title} key={player.id}>
            <Button title="Play" onPress={() => player.playSound()} />
            <Button title="Pause" onPress={() => player.pauseSound()} />
            <Button
              title="Seek To 5 secs"
              onPress={() => player.seekTo(5000)}
            />
            <Button title="loop" onPress={() => player.loopSound(true)} />
            <Button title="unloop" onPress={() => player.loopSound(false)} />
            <VolumeSlider
              style={styles.volumeSlider}
              onChange={(volume) => player.setVolume(volume)}
            />
          </ButtonsGroup>
        ))}
        <ButtonsGroup title="All players">
          <Button
            title="Play all"
            onPress={() =>
              AudioManager.shared.playSounds(
                playersExceptAxe.map((player) => [player, true])
              )
            }
          />
          <Button
            title="Pause all"
            onPress={() =>
              AudioManager.shared.playSounds(
                playersExceptAxe.map((player) => [player, false] as const)
              )
            }
          />
          <Button
            title="Loop all"
            onPress={() =>
              AudioManager.shared.loopSounds(
                playersExceptAxe.map((player) => [player, true] as const)
              )
            }
          />
          <Button
            title="Unloop all"
            onPress={() =>
              AudioManager.shared.loopSounds(
                playersExceptAxe.map((player) => [player, false] as const)
              )
            }
          />
        </ButtonsGroup>
      </ScrollView>
    </SafeAreaView>
  );
}

interface ButtonsGroupProps {
  title: string;
  children: React.ReactNode;
}

function ButtonsGroup({ title, children }: ButtonsGroupProps) {
  return (
    <View style={styles.buttonsGroupContainer}>
      <Text style={styles.buttonGroupTitle}>{title}</Text>
      <View style={styles.buttonsContainer}>{children}</View>
    </View>
  );
}

const styles = StyleSheet.create({
  screen: {
    flex: 1,
  },
  contentContainer: {
    padding: 16,
    gap: 32,
  },
  buttonsGroupContainer: {
    gap: 8,
    borderRadius: 4,
    borderWidth: 2,
    borderColor: 'black',
    padding: 8,
  },
  buttonGroupTitle: {
    fontSize: 16,
    fontWeight: 'bold',
  },
  buttonsContainer: {
    flexDirection: 'row',
    flexWrap: 'wrap',
    gap: 8,
  },
  volumeSlider: {
    width: '100%',
  },
  buttonInput: {
    width: '100%',
  },
});
