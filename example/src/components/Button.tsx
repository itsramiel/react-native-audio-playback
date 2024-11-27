import {
  Pressable,
  StyleSheet,
  Text,
  type PressableProps,
  type ViewStyle,
} from 'react-native';

interface ButtonProps extends PressableProps {
  title: string;
  style?: ViewStyle;
}

export function Button({ title, ...props }: ButtonProps) {
  return (
    <Pressable
      {...props}
      style={({ pressed }) => [
        styles.container,
        pressed ? styles.pressedContainer : undefined,
        props.style,
      ]}
    >
      <Text style={styles.buttonText}>{title}</Text>
    </Pressable>
  );
}

const styles = StyleSheet.create({
  container: {
    padding: 8,
    borderRadius: 4,
    backgroundColor: 'blue',
  },
  pressedContainer: {
    backgroundColor: 'teal',
  },
  buttonText: {
    color: 'white',
    fontSize: 16,
  },
});
