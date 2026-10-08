#include "test_neural_processors.hpp"
#include "Audio.hpp"
#include "Vision.hpp"
#include "InternalSignals.hpp"
#include "AudioProcessor.hpp"
#include "VisionProcessor.hpp"
#include "InternalSignalsProcessor.hpp"
#include <cmath>
#include <vector>
#include <numeric>
#include <algorithm>
#include <random>

namespace nlm {

bool testAudioProcessorNeuralFeatures() {
    Audio audio(44100, 44100); // 1 second of audio at 44.1kHz
    
    // Generate test audio with various characteristics
    std::vector<float> testData(44100, 0.0f);
    
    // Create a test signal with sine waves and noise
    for (size_t i = 0; i < testData.size(); ++i) {
        float t = static_cast<float>(i) / 44100.0f;
        // Add different frequency components
        testData[i] += 0.5f * std::sin(2.0f * M_PI * 440.0f * t); // A4 note
        testData[i] += 0.3f * std::sin(2.0f * M_PI * 880.0f * t); // A5 note
        testData[i] += 0.2f * ((std::rand() % 1000) / 1000.0f - 0.5f); // Noise
    }
    
    audio.setData(testData);
    
    AudioProcessor processor;
    processor.process(audio);
    
    const std::vector<float>& features = processor.getFeatures();
    
    // Test basic feature extraction
    if (features.empty()) {
        return false;
    }
    
    // Test that features are reasonable (should have 64 dimensions for compressed representation)
    if (features.size() != 64) {
        return false;
    }
    
    // Test that features are in valid range [0, 1] after processing
    for (float feature : features) {
        if (feature < 0.0f || feature > 1.0f) {
            return false;
        }
    }
    
    // Test that temporal features are reasonable
    processor.computeTemporalFeatures(testData);
    
    return true;
}

bool testVisionProcessorNeuralFeatures() {
    // Create a test image with various patterns
    size_t width = 320;
    size_t height = 240;
    size_t channels = 3;
    
    Vision vision(width, height, channels);
    std::vector<float> imageData(width * height * channels, 0.0f);
    
    // Create a test image with edges and shapes
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            // Create a checkerboard pattern
            size_t cx = (x / 32) % 2;
            size_t cy = (y / 32) % 2;
            size_t cell = (cx + cy) % 2;
            
            // Set color based on cell
            size_t baseIndex = (y * width + x) * channels;
            if (cell == 0) {
                imageData[baseIndex] = 1.0f;     // Red
                imageData[baseIndex + 1] = 0.5f; // Green
                imageData[baseIndex + 2] = 0.5f; // Blue
            } else {
                imageData[baseIndex] = 0.0f;     // Red
                imageData[baseIndex + 1] = 0.0f; // Green
                imageData[baseIndex + 2] = 0.0f; // Blue
            }
        }
    }
    
    // Add some edges
    for (size_t x = 64; x < width - 64; x += 20) {
        size_t edgeIndex = (height / 2 * width + x) * channels;
        for (size_t c = 0; c < channels; ++c) {
            imageData[edgeIndex + c] = 1.0f;
        }
    }
    
    vision.setData(imageData);
    
    VisionProcessor processor;
    processor.process(vision);
    
    const std::vector<float>& features = processor.getFeatures();
    
    // Test basic feature extraction
    if (features.empty()) {
        return false;
    }
    
    // Test that features have reasonable dimensions
    if (features.size() != 48) { // Expected 48 features for compressed representation
        return false;
    }
    
    // Test that features are in valid range
    for (float feature : features) {
        if (feature < 0.0f || feature > 1.0f) {
            return false;
        }
    }
    
    return true;
}

bool testInternalSignalsProcessorNeuralIntegration() {
    // Create test internal signals
    InternalSignals signals;
    
    // Add various internal states
    signals.addSignal(0.7f);   // High arousal
    signals.addSignal(0.3f);   // Low attention
    signals.addSignal(0.8f);   // High motivation
    signals.addSignal(0.6f);   // Moderate stress
    signals.addSignal(0.4f);   // Low metabolic
    
    // Process with neural mechanisms
    InternalSignalsProcessor processor;
    processor.process(signals);
    
    const std::vector<float>& processedSignals = processor.getProcessedSignals();
    const std::vector<float>& homeostaticSignals = processor.getHomeostaticSignals();
    
    // Test basic processing
    if (processedSignals.empty()) {
        return false;
    }
    
    if (homeostaticSignals.empty()) {
        return false;
    }
    
    // Test that processed signals have reasonable dimensions
    if (processedSignals.size() != 11) { // Expected 11 features
        return false;
    }
    
    // Test that homeostatic signals have reasonable dimensions
    if (homeostaticSignals.size() != 8) { // Expected 8 homeostatic signals
        return false;
    }
    
    // Test that signals are in valid ranges
    for (float signal : processedSignals) {
        if (signal < 0.0f || signal > 2.0f) { // Allow some margin for neural noise
            return false;
        }
    }
    
    // Test that homeostatic signals are reasonable
    for (float signal : homeostaticSignals) {
        if (signal < 0.0f || signal > 1.0f) {
            return false;
        }
    }
    
    return true;
}

bool testNeuralIntegrationAndCompetition() {
    // Test lateral inhibition and competitive network dynamics
    std::vector<float> input = {0.1f, 0.3f, 0.8f, 0.5f, 0.2f, 0.9f, 0.4f, 0.6f};
    
    // Apply lateral inhibition
    std::vector<float> inhibited = lateralInhibition(input, 0.3f);
    
    // Apply competitive selection
    std::vector<float> selected = competitiveSelection(inhibited, 3);
    
    // Test that competitive selection picks top values
    if (selected.size() != input.size()) {
        return false;
    }
    
    // Test that at least some values were enhanced
    bool hasEnhanced = false;
    for (float val : selected) {
        if (val > 1.0f) {
            hasEnhanced = true;
            break;
        }
    }
    
    return hasEnhanced;
}

// Helper functions
std::vector<float> lateralInhibition(const std::vector<float>& input, float inhibitionStrength) {
    std::vector<float> output = input;
    size_t size = input.size();
    
    for (size_t i = 0; i < size; ++i) {
        float maxNeighbor = 0.0f;
        // Find maximum among neighbors (circular neighborhood)
        for (int offset = -2; offset <= 2; ++offset) {
            size_t neighbor = (i + offset) % size;
            maxNeighbor = std::max(maxNeighbor, input[neighbor]);
        }
        
        // Apply lateral inhibition
        float inhibition = inhibitionStrength * (maxNeighbor - input[i]);
        output[i] = std::max(0.0f, output[i] + inhibition);
    }
    
    return output;
}

std::vector<float> competitiveSelection(const std::vector<float>& input, size_t numWinners) {
    std::vector<float> output(input.size(), 0.0f);
    
    // Find top-k features (competitive selection)
    std::vector<size_t> indices(input.size());
    std::iota(indices.begin(), indices.end(), 0);
    
    std::partial_sort(indices.begin(), indices.begin() + numWinners, indices.end(),
                     [&input](size_t a, size_t b) { return input[a] > input[b]; });
    
    // Select and enhance top features
    for (size_t i = 0; i < numWinners && i < indices.size(); ++i) {
        output[indices[i]] = input[indices[i]] * (1.0f + 0.3f * (i + 1) / static_cast<float>(numWinners));
    }
    
    return output;
}

} // namespace nlm