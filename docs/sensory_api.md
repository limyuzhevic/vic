# NLM Sensory Systems API

## Overview

The Sensory Systems module implements the perception and processing of external and internal information for the NLM artificial brain. It bridges the gap between the simulated environment and the neural brain, providing the sensory inputs that drive perception and behavior.

## Key Features

- **Multimodal input**: Visual, auditory, tactile, and other sensory modalities
- **Event-driven processing**: Efficient handling of sensory events and updates
- **Feature extraction**: Conversion of raw sensory data to meaningful features
- **Neural encoding**: Transformation of sensory information to neural representations
- **Attention modulation**: Allocation of attention based on stimulus salience
- **Integration**: Combination of multiple sensory inputs
- **Context processing**: Consideration of environmental and behavioral context
- **Adaptive processing**: Modification based on experience and learning
- **Noise handling**: Robust processing despite sensor limitations
- **Calibration**: Adjustment of sensory sensitivity and accuracy

## Core Classes

### SensoryBase Class

Abstract base class for all sensory systems.

```cpp
class SensoryBase {
public:
    virtual ~SensoryBase() = default;
    
    // Sensory capabilities
    virtual void initialize() = 0;
    virtual void update(double timestep) = 0;
    virtual void reset() = 0;
    
    // State management
    virtual void saveState() = 0;
    virtual void restoreState() = 0;
    
    // Configuration
    virtual void setParameters(const SensoryParameters& params) = 0;
    virtual SensoryParameters getParameters() const = 0;
    
    // Statistics
    virtual double getSensoryLoad() const = 0;
    virtual size_t getProcessingTime() const = 0;
};
```

### SensoryInput Class

Core sensory input representation.

```cpp
class SensoryInput {
public:
    // Construction and lifecycle
    SensoryInput();
    explicit SensoryInput(const std::vector<float>& data);
    ~SensoryInput();
    
    // Input properties
    const std::vector<float>& getData() const;
    void setData(const std::vector<float>& data);
    size_t getDataSize() const;
    
    // Input metadata
    SensoryModality getModality() const;
    void setModality(SensoryModality modality);
    Timestamp getTimestamp() const;
    void setTimestamp(Timestamp timestamp);
    
    // Processing
    void normalize();
    void applyGain(float gain);
    void applyFilter(const std::vector<float>& filter);
    
    // Transformation
    std::vector<float> extractFeatures() const;
    std::vector<float> computeDerivatives() const;
    std::vector<float> applyTransform(const Transformation& transform);
    
    // State management
    void reset();
    void saveState();
    void restoreState();
    
    // Statistics
    float getMaxValue() const;
    float getMinValue() const;
    float getMeanValue() const;
    float getStandardDeviation() const;
};
```

### Vision Class

Visual sensory system implementation.

```cpp
class Vision : public SensoryBase {
public:
    // Construction and lifecycle
    Vision();
    explicit Vision(const VisionParameters& params);
    ~Vision() override;
    
    // Core sensory functions
    void initialize() override;
    void update(double timestep) override;
    void reset() override;
    
    // Visual processing
    void processVisualInput(const VisualInput& input);
    std::vector<float> detectEdges(const std::vector<float>& image);
    std::vector<float> detectFeatures(const std::vector<float>& image);
    std::vector<float> computeVisualRemainder(const std::vector<float>& image);
    
    // Feature extraction
    std::vector<float> extractSIFTFeatures(const std::vector<float>& image);
    std::vector<float> extractHOGFeatures(const std::vector<float>& image);
    std::vector<float> extractColorFeatures(const std::vector<float>& image);
    
    // Object recognition
    bool recognizeObject(const std::vector<float>& image, ObjectId& objectId);
    float recognizeScene(const std::vector<float>& image);
    std::vector<ObjectId> detectObjects(const std::vector<float>& image);
    
    // State management
    void saveState() override;
    void restoreState() override;
    
    // Configuration
    void setParameters(const VisionParameters& params) override;
    VisionParameters getParameters() const override;
    
    // Statistics
    double getSensoryLoad() const override;
    size_t getProcessingTime() const override;
    
    // Visual-specific statistics
    float getVisualAcuity() const;
    float getContrastSensitivity() const;
    size_t getFeatureCount() const;
    
    // Activity tracking
    void recordVisualProcessing(const std::vector<float>& image);
    float getAverageResponseTime() const;
    size_t getProcessingCount() const;
};
```

### Audio Class

Auditory sensory system implementation.

```cpp
class Audio : public SensoryBase {
public:
    // Construction and lifecycle
    Audio();
    explicit Audio(const AudioParameters& params);
    ~Audio() override;
    
    // Core sensory functions
    void initialize() override;
    void update(double timestep) override;
    void reset() override;
    
    // Auditory processing
    void processAudioInput(const AudioInput& input);
    std::vector<float> computeSpectrogram(const std::vector<float>& audio);
    std::vector<float> detectPitch(const std::vector<float>& audio);
    std::vector<float> detectRhythm(const std::vector<float>& audio);
    
    // Feature extraction
    std::vector<float> extractMelFrequencyCepstrum(const std::vector<float>& audio);
    std::vector<float> extractZeroCrossingRate(const std::vector<float>& audio);
    std::vector<float> extractHarmonicFeatures(const std::vector<float>& audio);
    
    // Sound recognition
    bool recognizeSpeech(const std::vector<float>& audio, std::string& transcription);
    float recognizeMusic(const std::vector<float>& audio);
    std::vector<SoundCategory> categorizeSounds(const std::vector<float>& audio);
    
    // State management
    void saveState() override;
    void restoreState() override;
    
    // Configuration
    void setParameters(const AudioParameters& params) override;
    AudioParameters getParameters() const override;
    
    // Statistics
    double getSensoryLoad() const override;
    size_t getProcessingTime() const override;
    
    // Auditory-specific statistics
    float getFrequencyResolution() const;
    float getTemporalResolution() const;
    size_t getAudioFeatureCount() const;
    
    // Activity tracking
    void recordAudioProcessing(const std::vector<float>& audio);
    float getAverageResponseTime() const;
    size_t getProcessingCount() const;
};
```

### InternalSignals Class

Internal sensory system for proprioception and interoception.

```cpp
class InternalSignals : public SensoryBase {
public:
    // Construction and lifecycle
    InternalSignals();
    explicit InternalSignals(const InternalSignalsParameters& params);
    ~InternalSignals() override;
    
    // Core sensory functions
    void initialize() override;
    void update(double timestep) override;
    void reset() override;
    
    // Proprioceptive processing
    void processJointPosition(const std::vector<float>& jointAngles);
    void processMuscleForce(const std::vector<float>& forces);
    void processBodyPosture(const std::vector<float>& posture);
    
    // Interoceptive processing
    void processVisceralState(const std::vector<float>& visceralStates);
    void processThermoregulation(const std::vector<float>& temperatures);
    void processMetabolicState(const std::vector<float>& metabolicData);
    
    // Integration
    void integrateSensoryInputs(const std::vector<SensoryInput>& inputs);
    void computeSensoryConflict(const std::vector<float>& predictions,
                               const std::vector<float>& actuals);
    
    // State management
    void saveState() override;
    void restoreState() override;
    
    // Configuration
    void setParameters(const InternalSignalsParameters& params) override;
    InternalSignalsParameters getParameters() const override;
    
    // Statistics
    double getSensoryLoad() const override;
    size_t getProcessingTime() const override;
    
    // Internal-specific statistics
    float getProprioceptiveAccuracy() const;
    float getInteroceptiveSensitivity() const;
    size_t getInternalFeatureCount() const;
    
    // Activity tracking
    void recordInternalProcessing(const std::vector<float>& signals);
    float getAverageResponseTime() const;
    size_t getProcessingCount() const;
};
```

## Sensory Architecture

### Hierarchical Sensory Organization

Sensory systems operate at multiple levels:

```
+-----------------------------------------------------------+
|                 High-Level Processing                     |
|-----------------------------------------------------------|
|  +------------------+--------------------+            |
|  |   Feature        |   Scene            |            |
|  |   Recognition    |   Understanding    |            |
|  +------------------+--------------------+            |
|  |   Object         |   Auditory         |            |
|  |   Detection      |   Processing       |            |
|  +------------------+--------------------+            |
|  |   Sensory        |   Proprioception   |            |
|  |   Input         |   Interoception    |            |
|  +------------------+--------------------+            |
+-----------------------------------------------------------+
```

### Sensory Processing Pipeline

1. **Data Acquisition**: Raw sensory data collection
2. **Preprocessing**: Filtering, normalization, and enhancement
3. **Feature Extraction**: Conversion of raw data to meaningful features
4. **Pattern Recognition**: Identification of patterns and objects
5. **Integration**: Combination of multiple sensory inputs
6. **Contextual Processing**: Consideration of environmental context
7. **Attention Allocation**: Focus on relevant information
8. **Neural Encoding**: Transformation to neural representations

## Implementation

### Vision Implementation

```cpp
float Vision::getVisualAcuity() const {
    // Calculate visual acuity based on feature detection
    float totalEdgeStrength = 0.0f;
    float totalContrast = 0.0f;
    
    for (const auto& feature : visualFeatures_) {
        totalEdgeStrength += feature.edgeStrength;
        totalContrast += feature.contrast;
    }
    
    return (totalEdgeStrength + totalContrast) / visualFeatures_.size();
}

void Vision::extractSIFTFeatures(const std::vector<float>& image) {
    // Simplified SIFT feature extraction
    std::vector<float> keypoints;
    std::vector<float> descriptors;
    
    // Detect interest points
    keypoints = findInterestPoints(image);
    
    // Compute descriptors
    descriptors = computeDescriptors(image, keypoints);
    
    // Store features
    visualFeatures_.clear();
    for (size_t i = 0; i < keypoints.size(); ++i) {
        VisualFeature feature;
        feature.location = keypoints[i];
        feature.descriptor = descriptors[i];
        feature.strength = calculateFeatureStrength(image, keypoints[i]);
        visualFeatures_.push_back(feature);
    }
}
```

### Audio Implementation

```cpp
float Audio::getFrequencyResolution() const {
    // Calculate frequency resolution based on FFT size
    if (fftSize_ > 0) {
        return SAMPLE_RATE / static_cast<float>(fftSize_);
    }
    return 0.0f;
}

void Audio::extractMelFrequencyCepstrum(const std::vector<float>& audio) {
    // Pre-emphasis
    std::vector<float> preEmphasized = preEmphasize(audio);
    
    // Framing
    std::vector<std::vector<float>> frames = frameAudio(preEmphasized);
    
    // FFT
    std::vector<std::vector<float>> spectra = computeSpectrogram(frames);
    
    // Mel filter banks
    std::vector<std::vector<float>> melFilters = createMelFilters();
    std::vector<float> melSpectrum = applyMelFilters(spectra, melFilters);
    
    // DCT
    std::vector<float> mfcc = computeMFCC(melSpectrum);
    
    // Store features
    audioFeatures_.clear();
    for (float coefficient : mfcc) {
        AudioFeature feature;
        feature.coefficient = coefficient;
        feature.type = AudioFeatureType::MFCC;
        audioFeatures_.push_back(feature);
    }
}
```

### Internal Signals Implementation

```cpp
void InternalSignals::processJointPosition(const std::vector<float>& jointAngles) {
    // Normalize joint angles
    std::vector<float> normalizedAngles = normalizeAngles(jointAngles);
    
    // Detect movement patterns
    std::vector<MovementPattern> patterns = detectMovementPatterns(normalizedAngles);
    
    // Store proprioceptive information
    proprioceptiveData_.clear();
    for (const auto& pattern : patterns) {
        ProprioceptiveFeature feature;
        feature.angle = pattern.averageAngle;
        feature.velocity = pattern.velocity;
        feature.acceleration = pattern.acceleration;
        feature.confidence = pattern.confidence;
        proprioceptiveData_.push_back(feature);
    }
}
```

## Integration with Other Modules

### Brain Integration

```cpp
class Brain {
public:
    // Sensory system access
    Vision* getVision();
    Audio* getAudio();
    InternalSignals* getInternalSignals();
    
    // Sensory coordination
    void coordinateSensorySystems();
    void integrateSensoryInput(const SensoryInput& input);
    void resolveSensoryConflict(const std::vector<SensoryInput>& inputs);
};
```

### Neural System Integration

```cpp
void Brain::coordinateSensorySystems() {
    // Get sensory systems
    auto vision = getVision();
    auto audio = getAudio();
    auto internalSignals = getInternalSignals();
    
    // Get neural system
    auto neuralSystem = getNeuralSystem();
    
    // Integrate sensory inputs with neural processing
    vision->integrateWithNeuralSystem(neuralSystem);
    audio->integrateWithNeuralSystem(neuralSystem);
    internalSignals->integrateWithNeuralSystem(neuralSystem);
    
    // Coordinate attention allocation
    coordinateAttentionBasedOnSensoryInput(vision, audio, internalSignals);
}
```

### Memory System Integration

```cpp
void Brain::integrateSensoryInput(const SensoryInput& input) {
    // Get memory systems
    auto workingMemory = getWorkingMemory();
    auto episodicMemory = getEpisodicMemory();
    
    // Store sensory input in working memory
    workingMemory->storeSensoryInput(input);
    
    // Create episodic record
    Episode episode;
    episode.input = input;
    episode.timestamp = getCurrentTime();
    episode.context = getCurrentContext();
    
    // Store in episodic memory
    episodicMemory->encodeExperience(episode);
}
```

## Performance Optimization

### Computational Efficiency

```cpp
// Batch processing of sensory inputs
class SensoryBatchProcessor {
public:
    // Process multiple visual inputs efficiently
    static void processVisualInputs(std::vector<VisualInput>& inputs,
                                   std::function<void(VisualInput&)> processor);
    
    // Process multiple audio inputs efficiently
    static void processAudioInputs(std::vector<AudioInput>& inputs,
                                  std::function<void(AudioInput&)> processor);
    
    // Process internal signals efficiently
    static void processInternalSignals(std::vector<InternalSignal>& signals,
                                       std::function<void(InternalSignal&)> processor);
};
```

### Memory Management

```cpp
// Memory pools for sensory states
class SensoryStatePool {
public:
    static SensoryState* allocate();
    static void deallocate(SensoryState* state);
    static void clear();
    static size_t getUsedCount();
};
```

### Parallel Processing

```cpp
// Thread-safe parallel sensory processing
class ParallelSensoryProcessor {
public:
    static void processSensoryInputs(std::vector<SensoryInput>& inputs,
                                    std::function<void(SensoryInput&)> processor);
    
    static void extractFeatures(const std::vector<SensoryInput>& inputs,
                               std::vector<std::vector<float>>& features);
    
    static void integrateSensoryInputs(const std::vector<SensoryInput>& inputs,
                                      std::vector<float>& integrated);
};
```

## Thread Safety

### Read-Only Operations

```cpp
// Thread-safe read operations
const std::vector<float>& Vision::getData() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return data_;
}

SensoryModality InternalSignals::getModality() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return modality_;
}
```

### Write Operations

```cpp
// Write operations with proper synchronization
void Vision::processVisualInput(const VisualInput& input) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Extract features
    std::vector<float> features = extractFeatures(input);
    
    // Store features
    visualFeatures_ = features;
    
    // Update statistics
    processingCount_++;
    lastProcessingTime_ = getCurrentTime();
}
```

## Error Handling

### Sensory Errors

```cpp
enum class SensoryError {
    Success = 0,
    InvalidInput = 1,
    ProcessingFailed = 2,
    FeatureExtractionFailed = 3,
    RecognitionFailed = 4,
    CalibrationFailed = 5,
    IntegrationFailed = 6
};
```

### Error Handling Pattern

```cpp
void Vision::processVisualInput(const VisualInput& input) {
    try {
        // Validate input
        if (input.image.empty()) {
            throw SensoryError::InvalidInput;
        }
        
        // Check image dimensions
        if (!isValidImageDimensions(input.image)) {
            throw SensoryError::InvalidInput;
        }
        
        // Process input
        std::vector<float> features = extractFeatures(input);
        
        // Validate features
        if (features.empty()) {
            throw SensoryError::FeatureExtractionFailed;
        }
        
        // Store features
        visualFeatures_ = features;
        
    } catch (const SensoryError& e) {
        // Handle error
        std::cerr << "Sensory error: " << static_cast<int>(e) << std::endl;
        throw;
    }
}
```

## Usage Examples

### Basic Vision System

```cpp
#include <nlm/sensory/Vision.hpp>

int main() {
    // Create vision system
    auto vision = std::make_shared<nlm::Vision>();
    
    // Create test image
    std::vector<float> image = {
        0.1f, 0.2f, 0.3f,
        0.4f, 0.5f, 0.6f,
        0.7f, 0.8f, 0.9f
    };
    
    // Create visual input
    nlm::VisualInput visualInput;
    visualInput.image = image;
    visualInput.width = 3;
    visualInput.height = 3;
    
    // Process visual input
    vision->processVisualInput(visualInput);
    
    // Extract features
    std::vector<float> features = vision->extractFeatures(image);
    
    std::cout << "Extracted " << features.size() << " features" << std::endl;
    
    return 0;
}
```

### Audio Processing

```cpp
#include <nlm/sensory/Audio.hpp>

int main() {
    // Create audio system
    auto audio = std::make_shared<nlm::Audio>();
    
    // Create test audio
    std::vector<float> audioData = {
        0.1f, 0.2f, 0.3f, 0.4f, 0.5f,
        0.6f, 0.7f, 0.8f, 0.9f, 1.0f
    };
    
    // Create audio input
    nlm::AudioInput audioInput;
    audioInput.data = audioData;
    audioInput.sampleRate = 44100;
    audioInput.duration = 0.1f;
    
    // Process audio input
    audio->processAudioInput(audioInput);
    
    // Extract features
    std::vector<float> features = audio->extractFeatures(audioData);
    
    std::cout << "Extracted " << features.size() << " audio features" << std::endl;
    
    return 0;
}
```

### Internal Signals Processing

```cpp
#include <nlm/sensory/InternalSignals.hpp>

int main() {
    // Create internal signals system
    auto internalSignals = std::make_shared<nlm::InternalSignals>();
    
    // Create test joint angles
    std::vector<float> jointAngles = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    
    // Process joint position
    internalSignals->processJointPosition(jointAngles);
    
    // Create test visceral states
    std::vector<float> visceralStates = {0.8f, 0.6f, 0.4f};
    
    // Process visceral states
    internalSignals->processVisceralState(visceralStates);
    
    // Get proprioceptive data
    std::vector<ProprioceptiveFeature> proprioception = 
        internalSignals->getProprioceptiveData();
    
    std::cout << "Processed proprioceptive data for " << proprioception.size() << " joints" << std::endl;
    
    return 0;
}
```

## Testing and Validation

### Unit Testing

```cpp
#include <nlm/sensory/Vision.hpp>
#include <gtest/gtest.h>

TEST(VisionTest, FeatureExtraction) {
    auto vision = std::make_shared<nlm::Vision>();
    
    // Create test image
    std::vector<float> image = {
        0.1f, 0.2f, 0.3f,
        0.4f, 0.5f, 0.6f,
        0.7f, 0.8f, 0.9f
    };
    
    // Extract features
    std::vector<float> features = vision->extractFeatures(image);
    
    EXPECT_FALSE(features.empty());
}

TEST(AudioTest, FeatureExtraction) {
    auto audio = std::make_shared<nlm::Audio>();
    
    // Create test audio
    std::vector<float> audioData = {
        0.1f, 0.2f, 0.3f, 0.4f, 0.5f,
        0.6f, 0.7f, 0.8f, 0.9f, 1.0f
    };
    
    // Extract features
    std::vector<float> features = audio->extractFeatures(audioData);
    
    EXPECT_FALSE(features.empty());
}
```

### Performance Testing

```bash
// Run sensory performance tests
./benchmark --sensory --test vision

// Profile sensory processing
./profiler --target sensory --operation audio_processing
```

## Migration Guide

### From Earlier Versions

Sensory API changes:

- **Version 0.1.0**: Basic sensory input only
- **Version 0.2.0**: Added vision and audio systems
- **Version 0.3.0**: Enhanced internal signals
- **Version 0.4.0**: Added feature extraction and processing
- **Version 0.5.0**: Improved integration and attention mechanisms
- **Version 0.6.0**: Complete sensory integration and performance optimization

### API Changes

- **Sensory systems**: Expanded from basic input to comprehensive multimodal systems
- **Feature extraction**: Changed from simple thresholding to advanced algorithms
- **Integration**: Enhanced integration with neural and memory systems
- **Attention mechanisms**: Added adaptive attention allocation
- **Performance optimization**: Added batch processing and parallel processing
- **Thread safety**: Added proper synchronization for concurrent access
- **Error handling**: Enhanced error handling and validation

## References

1. [Computer Vision](https://en.wikipedia.org/wiki/Computer_vision)
2. [Audio Signal Processing](https://en.wikipedia.org/wiki/Audio_signal_processing)
3. [Proprioception](https://en.wikipedia.org/wiki/Proprioception)
4. [Interoception](https://en.wikipedia.org/wiki/Interoception)
5. [Sensory Integration](https://en.wikipedia.org/wiki/Sensory_integration)

## See Also

- [Brain API](brain_api.md) - Brain integration with sensory systems
- [Memory API](memory_api.md) - Memory system integration
- [Neuromodulation API](neuromodulation_api.md) - Neuromodulation effects on sensory processing
- [Development API](development_api.md) - Development system integration
- [Cognition API](cognition_api.md) - Cognitive system integration
- [Config API](config_api.md) - Configuration for sensory systems
- [Logging API](logging_api.md) - Logging system integration

**Last updated:** October 18, 2026
**Version:** 0.6.0