import { StyleSheet, ScrollView, SafeAreaView } from 'react-native';

import { usePlayers } from './hooks';
import { PlayerControl, StreamControl } from './components';
import { MultiPlayersControl } from './components/MultiPlayersControl';

export default function App() {
  const { onPlayerPressed, loadPlayers, playerObjects } = usePlayers();

  return (
    <SafeAreaView style={styles.screen}>
      <ScrollView contentContainerStyle={styles.contentContainer}>
        <StreamControl onLoadSounds={loadPlayers} />
        {playerObjects.map(({ title, player }) => (
          <PlayerControl player={player} title={title} key={player.id} />
        ))}
        <MultiPlayersControl
          players={playerObjects}
          onPlayerPress={onPlayerPressed}
        />
      </ScrollView>
    </SafeAreaView>
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
});
