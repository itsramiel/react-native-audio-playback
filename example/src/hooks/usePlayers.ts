import { useCallback, useState } from 'react';
import { AudioManager, type Player } from 'react-native-audio-playback';

type TPlayerObject = {
  player: Player;
  title: string;
  selected: boolean;
};

export function usePlayers(): {
  playerObjects: Array<TPlayerObject>;
  loadPlayers: () => void;
  onPlayerPressed: (player: Player) => void;
} {
  const [playerObjects, setPlayerObjects] = useState<Array<TPlayerObject>>([]);

  const loadPlayers = useCallback(async () => {
    try {
      const player1 = await AudioManager.shared.loadSound(
        require('../assets/game-track.mp3')
      );
      const player2 = await AudioManager.shared.loadSound(
        require('../assets/laser-shot.mp3')
      );
      const player3 = await AudioManager.shared.loadSound(
        require('../assets/music-beat.mp3')
      );
      const player4 = await AudioManager.shared.loadSound(
        require('../assets/clap.mp3')
      );

      const playerObjects = [
        { player: player1, title: 'Game Track', selected: false },
        { player: player2, title: 'Laser Shot', selected: false },
        { player: player3, title: 'Music Beat', selected: false },
        { player: player4, title: 'Clap', selected: false },
      ].filter((p) => p.player) as Array<TPlayerObject>;

      setPlayerObjects(playerObjects);
    } catch (e) {
      console.error(e);
    }
  }, []);

  const onPlayerPressed = useCallback((player: Player) => {
    setPlayerObjects((prev) => {
      return prev.map((pObject) => {
        return pObject.player === player
          ? { ...pObject, selected: !pObject.selected }
          : pObject;
      });
    });
  }, []);

  return { playerObjects, loadPlayers, onPlayerPressed };
}
