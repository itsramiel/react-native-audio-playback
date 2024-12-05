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
    const player1 = await AudioManager.shared.loadSound(
      require('../assets/bamboo.mp3')
    );

    const player2 = await AudioManager.shared.loadSound(
      require('../assets/swords.mp3')
    );

    const player3 = await AudioManager.shared.loadSound(
      require('../assets/coins.mp3')
    );

    const player4 = await AudioManager.shared.loadSound(
      require('../assets/axe.mp3')
    );

    const playerObjects = [
      { player: player1, title: 'Bamboo', selected: false },
      { player: player2, title: 'Swords', selected: false },
      { player: player3, title: 'Coins', selected: false },
      { player: player4, title: 'Axe', selected: false },
    ].filter((p) => p.player) as Array<TPlayerObject>;

    setPlayerObjects(playerObjects);
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
