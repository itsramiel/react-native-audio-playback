import { useState } from 'react';
import { StyleSheet, TextInput, View, type ViewStyle } from 'react-native';
import { Button } from './Button';

interface InputButtonProps {
  title: string;
  onPress: (text: string) => void;
  style?: ViewStyle;
}

export function InputButton({ title, onPress, style }: InputButtonProps) {
  const [text, setText] = useState('');

  return (
    <View style={[styles.container, style]}>
      <Button title={title} onPress={() => onPress(text)} />
      <TextInput style={styles.input} value={text} onChangeText={setText} />
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flexDirection: 'row',
    gap: 16,
    alignItems: 'center',
  },
  input: {
    borderWidth: 2,
    borderColor: 'gray',
    borderRadius: 4,
    padding: 8,
    flex: 1,
  },
});
