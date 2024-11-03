import React, { useState } from 'react';
import { StatusBar } from 'expo-status-bar';
import {
  Text,
  View,
  TouchableOpacity,
  Platform,
  StyleSheet,
} from 'react-native';
import { MaterialIcons } from '@expo/vector-icons';
export default function App() {
  // Hardcoded current temperature
  const currentTemp = 25; // in °C
  // State for threshold
  const [threshold, setThreshold] = useState(30);
  // Determine status based on threshold
  const isAboveThreshold = currentTemp > threshold;
  // Handlers to increase and decrease threshold
  const increaseThreshold = () => {
    setThreshold((prev) => (prev < 40 ? prev + 1 : prev));
  };
  const decreaseThreshold = () => {
    setThreshold((prev) => (prev > 15 ? prev - 1 : prev));
  };
  return (
    <>
      <View style={styles.container}>
        <StatusBar style='light' />
        {/* Header */}
        <View style={styles.header}>
          <Text style={styles.headerText}>Temp Control</Text>
          <TouchableOpacity>
            <MaterialIcons name='settings' size={28} color='white' />
          </TouchableOpacity>
        </View>
        {/* Temperature Display */}
        <View style={styles.temperatureContainer}>
          <View style={styles.temperatureBubble}>
            <Text style={styles.temperatureText}>{currentTemp}°C</Text>
          </View>
          <Text style={styles.temperatureLabel}>Current Temperature</Text>
        </View>
        {/* Threshold Adjuster */}
        <View style={styles.thresholdContainer}>
          <Text style={styles.thresholdLabel}>Set Temperature Threshold</Text>
          <View style={styles.adjuster}>
            <TouchableOpacity
              onPress={decreaseThreshold}
              style={styles.adjustButton}
            >
              <MaterialIcons name='remove' size={24} color='white' />
            </TouchableOpacity>
            <Text style={styles.thresholdValue}>{threshold}°C</Text>
            <TouchableOpacity
              onPress={increaseThreshold}
              style={styles.adjustButton}
            >
              <MaterialIcons name='add' size={24} color='white' />
            </TouchableOpacity>
          </View>
        </View>
        {/* Status Indicator */}
        <View style={styles.statusContainer}>
          {isAboveThreshold ? (
            <View style={styles.statusWarning}>
              <MaterialIcons name='error' size={24} color='red' />
              <Text style={styles.statusWarningText}>Above Threshold!</Text>
            </View>
          ) : (
            <View style={styles.statusNormal}>
              <MaterialIcons name='check-circle' size={24} color='green' />
              <Text style={styles.statusNormalText}>Within Threshold</Text>
            </View>
          )}
        </View>
      </View>
    </>
  );
}
const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: 'rgba(255,211,20,0.2)', // Since Tailwind handles background
    padding: 24, // p-6
  },
  header: {
    flexDirection: 'row',
    justifyContent: 'space-between',
    alignItems: 'center',
    marginBottom: 40, // mb-10
  },
  headerText: {
    color: '#ffffff',
    fontSize: 24, // text-3xl
    fontWeight: 'bold',
  },
  temperatureContainer: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
  },
  temperatureBubble: {
    backgroundColor: 'rgba(255, 255, 255, 0.2)', // bg-white bg-opacity-20
    borderRadius: 100, // rounded-full
    padding: 40, // p-10
    alignItems: 'center',
    justifyContent: 'center',
  },
  temperatureText: {
    color: '#ffffff',
    fontSize: 48, // text-6xl
    fontWeight: '600',
  },
  temperatureLabel: {
    color: '#ffffff',
    fontSize: 18, // text-xl
    marginTop: 16, // mt-4
  },
  thresholdContainer: {
    marginBottom: 40, // mb-10
  },
  thresholdLabel: {
    color: '#ffffff',
    fontSize: 18, // text-lg
    marginBottom: 8, // mb-2
    textAlign: 'center',
  },
  adjuster: {
    flexDirection: 'row',
    alignItems: 'center',
    justifyContent: 'center',
  },
  adjustButton: {
    backgroundColor: '#f59e0b', // Tailwind's yellow-500 (similar to fbbf24)
    padding: 10,
    borderRadius: 50,
    marginHorizontal: 20,
  },
  thresholdValue: {
    color: '#ffffff',
    fontSize: 20,
    fontWeight: '500',
  },
  statusContainer: {
    alignItems: 'center',
  },
  statusWarning: {
    flexDirection: 'row',
    alignItems: 'center',
    backgroundColor: 'rgba(239, 68, 68, 0.2)', // bg-red-500 bg-opacity-20
    paddingHorizontal: 16,
    paddingVertical: 8,
    borderRadius: 999, // rounded-full
  },
  statusWarningText: {
    color: '#ef4444', // red-500
    marginLeft: 8,
    fontSize: 16,
  },
  statusNormal: {
    flexDirection: 'row',
    alignItems: 'center',
    backgroundColor: 'rgba(34, 197, 94, 0.2)', // bg-green-500 bg-opacity-20
    paddingHorizontal: 16,
    paddingVertical: 8,
    borderRadius: 999, // rounded-full
  },
  statusNormalText: {
    color: '#22c55e', // green-500
    marginLeft: 8,
    fontSize: 16,
  },
});
