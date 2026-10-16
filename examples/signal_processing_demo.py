# Example 4: Signal Processing Demo

import pynlm
import numpy as np

print("=== Signal Processing Demo ===")

# Step 1: Create signals
print("\n1. Creating and processing signals...")

# Create signal processing instance
processor = pynlm.SignalProcessor()

# Generate example signals
vision_signal = np.array([0.1, 0.3, 0.5, 0.7, 0.9, 0.8, 0.6, 0.4, 0.2, 0.0])
touch_signal = np.array([0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0])
internal_signal = np.array([-0.5, 0.0, 0.5, 1.0, 0.5, 0.0, -0.5, -1.0, -0.5, 0.0])

# Process signals
print(f"  Vision signal: mean={np.mean(vision_signal):.3f}, std={np.std(vision_signal):.3f}")

normalized_vision = processor.normalize(vision_signal)
print(f"  Normalized vision: mean={np.mean(normalized_vision):.3f}, std={np.std(normalized_vision):.3f}")

smoothed_vision = processor.smooth(vision_signal, window_size=3)
print(f"  Smoothed vision: first 3 values: {smoothed_vision[:3]}")

# Find peaks
peak_idx, peak_value = processor.find_peak(vision_signal)
print(f"  Vision peak: index={peak_idx}, value={peak_value:.3f}")

# Step 2: Combine and filter signals
print("\n2. Combining and filtering signals...")

combined = processor.combine([vision_signal, touch_signal, internal_signal], weights=[0.5, 0.3, 0.2])
print(f"  Combined signal: mean={np.mean(combined):.3f}")

filtered = processor.filter(combined, method="moving_average", window_size=5)
print(f"  Filtered signal: first 5 values: {filtered[:5]}")

# Step 3: Signal analysis
print("\n3. Signal analysis...")

# Get statistics
vision_stats = processor.get_statistics(vision_signal)
print(f"  Vision statistics: {vision_stats}")

touch_stats = processor.get_statistics(touch_signal)
print(f"  Touch statistics: {touch_stats}")

# Compare signals
comparison = processor.compare_signals({
    'vision': vision_signal,
    'touch': touch_signal,
    'internal': internal_signal
})
print(f"  Signal comparison: max similarity={max(comparison.values()):.3f}")

# Step 4: Advanced processing
print("\n4. Advanced signal processing...")

# Extract features
vision_features = processor.extract_features(vision_signal)
print(f"  Vision features: {vision_features}")

# Detect patterns
patterns = processor.detect_patterns(vision_signal)
print(f"  Detected patterns: {patterns}")

# Classify signals
classification = processor.classify_signal(vision_signal)
print(f"  Signal classification: {classification}")

# Step 5: Real-world example
print("\n5. Real-world signal processing example...")

# Simulate sensory data from a robot
simulated_vision = np.random.normal(0.5, 0.2, 100)
simulated_vision = np.clip(simulated_vision, 0, 1)  # Normalize to [0, 1]

simulated_touch = np.random.binomial(1, 0.3, 100)  # Binary touch sensor
simulated_internal = np.random.normal(0, 0.1, 100)  # Internal signals

# Process simulated data
processed_vision = processor.normalize(simulated_vision)
processed_touch = processor.filter(simulated_touch.astype(float), window_size=3)
processed_internal = processor.detect_anomalies(simulated_internal)

# Analyze results
print(f"  Simulated vision: mean={np.mean(simulated_vision):.3f}, std={np.std(simulated_vision):.3f}")
print(f"  Processed vision: mean={np.mean(processed_vision):.3f}, std={np.std(processed_vision):.3f}")
print(f"  Detected anomalies: {len(processed_internal[processed_internal > 2])}")

# Step 6: Machine learning integration
print("\n6. Machine learning integration...")

from sklearn.preprocessing import StandardScaler
from sklearn.cluster import KMeans
from sklearn.svm import SVC

# Prepare data for ML
ml_processor = processor
features = ml_processor.prepare_ml_data([vision_signal, touch_signal, internal_signal])
target = np.array([0, 1, 0])  # Example target

# Scale features
scaler = StandardScaler()
features_scaled = scaler.fit_transform(features)

# Train classifier
classifier = SVC(kernel='rbf', C=1.0, gamma='scale')
classifier.fit(features_scaled, target)

# Make predictions
prediction = classifier.predict(features_scaled)
print(f"  ML classification result: {prediction}")

# Step 7: Signal compression
print("\n7. Signal compression...")

# Compress signals
compressed_vision = processor.compress(vision_signal, compression_ratio=0.5)
compression_ratio = processor.get_compression_ratio(vision_signal)
print(f"  Compression ratio: {compression_ratio:.3f}")
print(f"  Original size: {len(vision_signal)}, Compressed size: {len(compressed_vision)}")

# Step 8: Signal reconstruction
print("\n8. Signal reconstruction...")

# Reconstruct compressed signal
reconstructed = processor.reconstruct(compressed_vision, original_size=len(vision_signal))

# Calculate reconstruction quality
reconstruction_error = np.mean((vision_signal - reconstructed) ** 2)
print(f"  Reconstruction error: {reconstruction_error:.6f}")

# Step 9: Signal transmission simulation
print("\n9. Signal transmission simulation...")

# Simulate signal transmission with noise
noised_signal = processor.add_noise(vision_signal, noise_level=0.1)

de-noised_signal = processor.denoise(noised_signal, method='wiener')

# Calculate quality metrics
snr = processor.calculate_snr(vision_signal, noised_signal)
print(f"  Signal-to-noise ratio: {snr:.3f} dB")

# Step 10: Signal-based control
print("\n10. Signal-based control...")

# Use signals for decision making
decision = processor.make_decision({
    'vision': processed_vision,
    'touch': processed_touch,
    'internal': processed_internal
})

print(f"  Control decision: {decision}")

# Step 11: Signal visualization
print("\n11. Signal visualization (matplotlib required)...")

import matplotlib.pyplot as plt

# Plot signals
plt.figure(figsize=(12, 8))

# Vision signal
plt.subplot(3, 1, 1)
plt.plot(vision_signal, label='Vision')
plt.plot(normalized_vision, label='Normalized', alpha=0.7)
plt.title('Vision Signal Processing')
plt.legend()
plt.grid(True)

# Touch signal
plt.subplot(3, 1, 2)
plt.plot(touch_signal, label='Touch', marker='o')
plt.plot(processed_touch, label='Processed', alpha=0.7)
plt.title('Touch Signal Processing')
plt.legend()
plt.grid(True)

# Internal signal
plt.subplot(3, 1, 3)
plt.plot(internal_signal, label='Internal')
plt.plot(processed_internal, label='Anomalies', alpha=0.7)
plt.title('Internal Signal Processing')
plt.legend()
plt.grid(True)

plt.tight_layout()
print("  Signal plots generated (saved to file if available)")

# Step 12: Real-time signal processing
print("\n12. Real-time signal processing...")

# Simulate real-time processing
stream_processor = processor

# Process signal stream
stream_data = [vision_signal[i:i+10] for i in range(0, len(vision_signal), 10)]
processed_stream = [stream_processor.process(stream) for stream in stream_data]

# Calculate real-time metrics
stream_metrics = {
    'avg_processing_time': stream_processor.get_average_processing_time(),
    'max_processing_time': stream_processor.get_max_processing_time(),
    'throughput': stream_processor.get_throughput()
}

print(f"  Stream processing metrics: {stream_metrics}")

print("\n=== Signal Processing Demo Complete ===")
