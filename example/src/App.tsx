import { useCallback, useState } from 'react';
import { StyleSheet, View, Text, ScrollView, SafeAreaView } from 'react-native';
import { Player, AudioManager } from 'react-native-audio-playback';
import { Button, VolumeSlider } from './components';

export default function App() {
  const [players, setPlayers] = useState<
    Array<{ player: Player; title: string; grouped: boolean }>
  >([]);

  const loadSounds = useCallback(async () => {
    const loadedPlayers = Array<(typeof players)[number]>();
    try {
      const player1 = await AudioManager.shared.loadSound(
        require('./assets/bamboo.mp3')
      );
      if (player1) {
        loadedPlayers.push({
          player: player1,
          title: 'Bamboo',
          grouped: false,
        });
      }
      const player2 = await AudioManager.shared.loadSound(
        require('./assets/swords.mp3')
      );
      if (player2) {
        loadedPlayers.push({
          player: player2,
          title: 'Swords',
          grouped: false,
        });
      }
      const player3 = await AudioManager.shared.loadSound(
        require('./assets/coins.mp3')
      );
      if (player3) {
        loadedPlayers.push({ player: player3, title: 'Coins', grouped: false });
      }

      const player4 = await AudioManager.shared.loadSound(
        require('./assets/axe.mp3')
      );

      if (player4) {
        loadedPlayers.push({ player: player4, title: 'Axe', grouped: false });
      }

      setPlayers(loadedPlayers);
    } catch (e) {
      console.log(e);
    }
  }, []);

  const groupedPlayers = players
    .filter((p) => p.grouped)
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
            title="Pause Stream"
            onPress={() => {
              AudioManager.shared.pauseAudioStream();
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
        <ButtonsGroup title="Multi Control">
          <View style={styles.groupedButtonsContainer}>
            {players.map(({ title, grouped, player }) => (
              <Button
                key={player.id}
                title={`${title} ${grouped ? '✅' : '❌'}`}
                onPress={() => {
                  setPlayers((prev) =>
                    prev.map((p) =>
                      p.player.id === player.id
                        ? { ...p, grouped: !p.grouped }
                        : p
                    )
                  );
                }}
              />
            ))}
          </View>
          <Button
            title="Play all"
            onPress={() =>
              AudioManager.shared.playSounds(
                groupedPlayers.map((player) => [player, true])
              )
            }
          />
          <Button
            title="Pause all"
            onPress={() =>
              AudioManager.shared.playSounds(
                groupedPlayers.map((player) => [player, false] as const)
              )
            }
          />
          <Button
            title="Loop all"
            onPress={() =>
              AudioManager.shared.loopSounds(
                groupedPlayers.map((player) => [player, true] as const)
              )
            }
          />
          <Button
            title="Unloop all"
            onPress={() =>
              AudioManager.shared.loopSounds(
                groupedPlayers.map((player) => [player, false] as const)
              )
            }
          />
          <VolumeSlider
            style={styles.volumeSlider}
            onChange={(volume) =>
              AudioManager.shared.setSoundsVolume(
                groupedPlayers.map((player) => [player, volume] as const)
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
  groupedButtonsContainer: {
    flexDirection: 'row',
    flexWrap: 'wrap',
    gap: 8,
    marginBottom: 8,
    width: '100%',
  },
  volumeSlider: {
    width: '100%',
  },
  buttonInput: {
    width: '100%',
  },
});
