import { Fragment, useState } from 'react';
import { StyleSheet, View, Button } from 'react-native';
import { Player, AudioManager } from 'react-native-audio-playback';

const audioManager = new AudioManager(44100, 2);
audioManager.openAudioStream();

export default function App() {
  const [player1, setPlayer1] = useState<Player | null>(null);
  const [player2, setPlayer2] = useState<Player | null>(null);

  const players = [player1, player2].filter(Boolean) as Array<Player>;

  return (
    <View style={styles.container}>
      <Button
        title="Play sounds"
        onPress={() => {
          audioManager.playSounds(players.map((player) => [player, true]));
        }}
      />
      <Button
        title="Loop sounds"
        onPress={() => {
          audioManager.loopSounds(players.map((player) => [player, true]));
        }}
      />
      <Button
        title="Seek sounds"
        onPress={() => {
          audioManager.seekSoundsTo([
            [player1!, 5000],
            [player2!, 5000],
          ]);
        }}
      />
      {[
        [player1, setPlayer1, require('./assets/CLAP-output1.mp3')] as const,
        [player2, setPlayer2, require('./assets/FUNKY_HOUSE.wav')] as const,
      ].map(([player, setPlayer, asset], index) => (
        <Fragment key={index}>
          {player ? (
            <>
              <Button
                title={`play sound ${index}`}
                onPress={async () => {
                  player.playSound();
                }}
              />
              <Button
                title={`pause sound ${index}`}
                onPress={() => {
                  player.pauseSound();
                }}
              />
              <Button
                title={'Seek to 5 seconds'}
                onPress={() => {
                  player.seekTo(5000);
                }}
              />
              <Button
                title={`loop sound ${index}`}
                onPress={() => {
                  player.loopSound(true);
                }}
              />
              <Button
                title={`unload sound ${index}`}
                onPress={() => {
                  player.unloadSound();
                  setPlayer(null);
                }}
              />
            </>
          ) : (
            <Button
              title={`load sound ${index}`}
              onPress={async () => {
                audioManager.loadSound(asset).then(setPlayer);
              }}
            />
          )}
        </Fragment>
      ))}
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    alignItems: 'center',
    justifyContent: 'center',
  },
  box: {
    width: 60,
    height: 60,
    marginVertical: 20,
  },
});
