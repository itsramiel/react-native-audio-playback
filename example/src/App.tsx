import { useEffect, useState } from 'react';
import {
  StyleSheet,
  View,
  type PressableProps,
  Pressable,
  Text,
  type ViewStyle,
  ScrollView,
  SafeAreaView,
} from 'react-native';
import { Player, AudioManager } from 'react-native-audio-playback';

const audioManager = new AudioManager(44100, 2);
audioManager.openAudioStream();

export default function App() {
  const [players, setPlayers] = useState<
    Array<{ player: Player; title: string }>
  >([]);

  useEffect(() => {
    (async () => {
      const players = Array<{ player: Player; title: string }>();
      const player1 = await audioManager.loadSound(
        require('./assets/bamboo.mp3')
      );
      if (player1) {
        players.push({ player: player1, title: 'Bamboo' });
      }
      const player2 = await audioManager.loadSound(
        require('./assets/swords.mp3')
      );
      if (player2) {
        players.push({ player: player2, title: 'Swords' });
      }
      const player3 = await audioManager.loadSound(
        require('./assets/coins.mp3')
      );
      if (player3) {
        players.push({ player: player3, title: 'Coins' });
      }

      const player4 = await audioManager.loadSound(require('./assets/axe.mp3'));

      if (player4) {
        players.push({ player: player4, title: 'Axe' });
      }

      setPlayers(players);
    })();
  }, []);

  return (
    <SafeAreaView style={styles.screen}>
      <ScrollView contentContainerStyle={styles.contentContainer}>
        {players.map((player) => (
          <PlayerControls
            key={player.player.id}
            title={player.title}
            player={player.player}
          />
        ))}
        <PlayersControls
          players={players
            .filter((p) => p.title !== 'Axe')
            .map((player) => player.player)}
        />
      </ScrollView>
    </SafeAreaView>
  );
}

interface PlayersControlsProps {
  players: Array<Player>;
}

function PlayersControls({ players }: PlayersControlsProps) {
  return (
    <View style={styles.controlsContainer}>
      <Text style={styles.controlsTitle}>All players</Text>
      <View style={styles.controlsInnerContainer}>
        <Button
          title="Play all"
          onPress={() =>
            audioManager.playSounds(players.map((player) => [player, true]))
          }
        />
        <Button
          title="Pause all"
          onPress={() =>
            audioManager.playSounds(
              players.map((player) => [player, false] as const)
            )
          }
        />
        <Button
          title="Loop all"
          onPress={() =>
            audioManager.loopSounds(
              players.map((player) => [player, true] as const)
            )
          }
        />
        <Button
          title="Unloop all"
          onPress={() =>
            audioManager.loopSounds(
              players.map((player) => [player, false] as const)
            )
          }
        />
      </View>
    </View>
  );
}

interface PlayerControlsProps {
  title: string;
  player: Player;
}

function PlayerControls({ title, player }: PlayerControlsProps) {
  return (
    <View style={styles.controlsContainer}>
      <Text style={styles.controlsTitle}>{title}</Text>
      <View style={styles.controlsInnerContainer}>
        <Button title="Play" onPress={() => player.playSound()} />
        <Button title="Pause" onPress={() => player.pauseSound()} />
        <Button title="Seek To 5 secs" onPress={() => player.seekTo(5000)} />
        <Button title="loop" onPress={() => player.loopSound(true)} />
        <Button title="unloop" onPress={() => player.loopSound(false)} />
      </View>
    </View>
  );
}

interface ButtonProps extends PressableProps {
  title: string;
  style?: ViewStyle;
}

function Button({ title, ...props }: ButtonProps) {
  return (
    <Pressable
      {...props}
      style={({ pressed }) => [
        styles.buttonContainer,
        pressed ? styles.pressedButtonContainer : undefined,
        props.style,
      ]}
    >
      <Text style={styles.buttonText}>{title}</Text>
    </Pressable>
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
  controlsContainer: {
    gap: 8,
    borderRadius: 4,
    borderWidth: 2,
    borderColor: 'black',
    padding: 8,
  },
  controlsTitle: {
    fontSize: 16,
    fontWeight: 'bold',
  },
  controlsInnerContainer: {
    flexDirection: 'row',
    flexWrap: 'wrap',
    gap: 8,
  },
  buttonContainer: {
    padding: 8,
    borderRadius: 4,
    backgroundColor: 'blue',
  },
  pressedButtonContainer: {
    backgroundColor: 'teal',
  },
  buttonText: {
    color: 'white',
    fontSize: 16,
  },
});
