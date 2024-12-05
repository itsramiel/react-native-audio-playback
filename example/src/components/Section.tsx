import { StyleSheet, Text, View } from 'react-native';

interface SectionProps {
  title: string;
  children: React.ReactNode;
}

export function Section({ title, children }: SectionProps) {
  return (
    <View style={styles.container}>
      <Text style={styles.title}>{title}</Text>
      <View style={styles.children}>{children}</View>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    gap: 8,
    borderRadius: 4,
    borderWidth: 2,
    borderColor: 'black',
    padding: 8,
  },
  title: {
    fontSize: 16,
    fontWeight: 'bold',
  },
  children: {
    flexDirection: 'row',
    flexWrap: 'wrap',
    gap: 8,
  },
});
