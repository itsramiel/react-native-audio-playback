import { useCallback, useState } from 'react';
import { type Player } from 'react-native-audio-playback';

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
    // Todo
  }, []);

  const onPlayerPressed = useCallback((player: Player) => {
    // Todo
  }, []);

  return { playerObjects, loadPlayers, onPlayerPressed };
}
