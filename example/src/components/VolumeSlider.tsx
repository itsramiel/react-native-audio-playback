import { StyleSheet, Text, View, type ViewStyle } from 'react-native';
import Slider from '@react-native-community/slider';

interface VolumeSliderProps {
  style?: ViewStyle;
  onChange?: (value: number) => void;
}

export function VolumeSlider({ style, onChange }: VolumeSliderProps) {
  return (
    <View style={style}>
      <Text>Volume</Text>
      <Slider
        onValueChange={onChange}
        style={styles.slider}
        minimumValue={0}
        maximumValue={1}
        step={0.1}
        value={1}
      />
    </View>
  );
}

const styles = StyleSheet.create({
  slider: {
    width: '100%',
  },
});
