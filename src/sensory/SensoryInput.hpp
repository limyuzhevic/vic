#pragma once

#include <vector>
#include <string>
#include <memory>

namespace nlm {

/**
 * @class SensoryInput
 * @brief Abstract base class for all sensory modalities and inputs to the brain
 * @details Defines the interface for encoding sensory information from the environment
 * into neural-compatible formats. SensoryInput represents the transducer layer that
 * converts external stimuli into internal neural representations. The brain receives
 * SensoryInput objects rather than raw sensor data, providing an abstraction layer
 * that supports multiple sensory modalities with consistent interface.
 * 
 * Sensory Modalities:
 * - Vision: Visual scene data, object recognition, depth information
 * - Audio: Sound signals, pitch, rhythm, spatial location
 * - Internal: Physiological states (energy, hunger, etc.)
 * - Proprioception: Body position and movement sensing
 * 
 * Core Interface:
 * - Type identification for modality discrimination
 * - Data access through consistent vector<float> interface
 * - Dimensionality information for neural mapping
 * - Temporal tracking with timestamps
 * - Cloning for parallel processing
 * 
 * Thread Safety: Derived classes should be thread-safe for concurrent access.
 * The base class implementation provides thread-safe timestamp operations.
 * 
 * Memory Management: Use clone() for creating copies. SensoryInput objects are
 * typically temporary and should not be stored long-term.
 * 
 * Performance: Vector access is O(1). Cloning creates new memory allocation.
 * Timestamps are lightweight (double precision).
 * 
 * Usage Pattern:
 * @code
 * // Process sensory input
 * void processSensoryInput(const SensoryInput& input) {
 *     // Identify modality
 *     const char* type = input.getType();
 *     
 *     // Get data
 *     const vector<float>& data = input.getData();
 *     
 *     // Process based on modality and dimensions
 *     size_t dim = input.getDimensions();
 *     double timestamp = input.getTimestamp();
 *     
 *     // Convert to neural representation
 *     // ... implementation-specific conversion ...
 * }
 * 
 * // Clone for parallel processing
 * vector<unique_ptr<SensoryInput>> processBatch(
 *     const vector<unique_ptr<SensoryInput>>& inputs) {
 *     vector<unique_ptr<SensoryInput>> processed;
 *     for (const auto& input : inputs) {
 *         auto clone = input->clone();
 *         processSensoryInput(*clone);
 *         processed.push_back(move(clone));
 *     }
 *     return processed;
 * }
 * @endcode
 * 
 * Biological Plausibility:
 * - Mirrors sensory transduction in biological systems
 * - Supports rate coding through firing patterns
 * - Allows temporal processing through timestamps
 * - Enables parallel modality processing
 * 
 * Error Handling: All derived implementations should handle invalid data
 * gracefully. Clone() may throw exceptions for unsupported types.
 * 
 * @see Vision
 * @see Audio
 * @see InternalSignals
 * @see AgentBrain
 * @see Brain::receiveSensoryInput
 */
class SensoryInput {
public:
    SensoryInput();
    virtual ~SensoryInput() = default;
    
    // Get input type
    virtual const char* getType() const = 0;
    
    // Get raw data as vector
    virtual const std::vector<float>& getData() const = 0;
    
    // Get dimensionality
    virtual size_t getDimensions() const = 0;
    
    // Get timestamp
    double getTimestamp() const;
    void setTimestamp(double timestamp);
    
    // Clone
    virtual std::unique_ptr<SensoryInput> clone() const = 0;
    
protected:
    double timestamp_;
};

/**
 * @class Vision
 * @brief Visual sensory input representing 2D/3D scene data
 * @details Vision input models visual perception from camera-like sensors or
 * simulated visual systems. It captures scene information including color, depth,
 * and spatial layout that can be processed by the brain's visual processing pathways.
 * 
 * Data Representation:
 * - RGB or grayscale image data in contiguous float array
 * - Spatial layout as 2D grid with optional 3rd dimension for color/channels
 * - Data can represent pixel intensities, feature maps, or compressed representations
 * - Order: row-major (width * height * channels)
 * 
 * Thread Safety: Thread-safe for concurrent read operations. Writing to data
 * should be synchronized externally.
 * 
 * Performance: Vector storage allows O(1) access. Data processing is O(n) where
 * n is number of pixels/channels. Large images should be processed in batches.
 * 
 * Biological Connection:
 * - Mirrors retinal processing and visual cortex organization
 * - Supports edge detection through convolution operations
 * - Enables object recognition through feature extraction
 * - Supports depth perception through stereo processing
 * 
 * @code
 * // Create vision input
 * Vision vision(640, 480, 3); // 640x480 RGB image
 * 
 * // Set image data (example: red rectangle)
 * vector<float> data(640 * 480 * 3, 0.0f);
 * for (int y = 100; y < 200; ++y) {
 *     for (int x = 100; x < 200; ++x) {
 *         size_t idx = (y * 640 + x) * 3;
 *         data[idx] = 1.0f;     // R
 *         data[idx + 1] = 0.0f; // G
 *         data[idx + 2] = 0.0f; // B
 *     }
 * }
 * vision.setData(data);
 * 
 * // Process visual data
 * size_t width = vision.getWidth();
 * size_t height = vision.getHeight();
 * size_t channels = vision.getChannels();
 * const vector<float>& imageData = vision.getData();
 * 
 * // Feature extraction example
 * for (size_t y = 0; y < height; ++y) {
 *     for (size_t x = 0; x < width; ++x) {
 *         size_t idx = (y * width + x) * channels;
 *         float r = imageData[idx];
 *         float g = imageData[idx + 1];
 *         float b = imageData[idx + 2];
 *         
 *         // Calculate brightness
 *         float brightness = 0.299f * r + 0.587f * g + 0.114f * b;
 *         
 *         // Detect edges (simple gradient)
 *         if (x > 0 && y > 0) {
 *             float left = imageData[idx - channels];
 *             float top = imageData[idx - width * channels];
 *             float edge = abs(brightness - left) + abs(brightness - top);
 *         }
 *     }
 * }
 * @endcode
 * 
 * @see SensoryInput
 * @see AgentBrain
 */
class Vision : public SensoryInput {
public:
    Vision();
    explicit Vision(size_t width, size_t height, size_t channels = 3);
    ~Vision() override;
    
    const char* getType() const override;
    const std::vector<float>& getData() const override;
    size_t getDimensions() const override;
    std::unique_ptr<SensoryInput> clone() const override;
    
    // Set image data
    void setData(const std::vector<float>& data);
    void setData(float* data, size_t size);
    
    // Image properties
    size_t getWidth() const;
    size_t getHeight() const;
    size_t getChannels() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @class Audio
 * @brief Auditory sensory input representing sound signals and audio features
 * @details Audio input models sound perception, capturing acoustic information
 * including frequency, amplitude, and temporal patterns. Used for processing
 * environmental sounds, music, and speech in the agent's world.
 * 
 * Audio Processing:
 * - Digital audio signals sampled at specified rates
 * - Feature extraction (pitch, rhythm, timbre, spatial location)
 * - Spectrogram representations or time-domain samples
 * - Multi-channel support for spatial audio
 * 
 * Thread Safety: Thread-safe for concurrent read operations.
 * 
 * Performance: O(1) for access, O(n) for processing. Large audio files should
 * be processed in chunks or sampled.
 * 
 * Biological Connection:
 * - Mirrors cochlear frequency analysis
 * - Supports temporal pattern recognition
 * - Enables sound localization
 * - Processes amplitude and phase information
 * 
 * @code
 * // Create audio input
 * Audio audio(44100, 44100 * 5); // 1 second of audio at 44.1kHz
 * 
 * // Record or set audio data
 * vector<float> samples(44100); // 1 second of mono audio
 * for (size_t i = 0; i < samples.size(); ++i) {
 *     // Generate or record audio sample
 *     samples[i] = sin(2.0 * M_PI * 440.0 * i / 44100.0); // A4 note
 * }
 * audio.setData(samples);
 * 
 * // Audio processing
 * size_t sampleRate = audio.getSampleRate();
 * size_t numSamples = audio.getNumSamples();
 * const vector<float>& audioData = audio.getData();
 * 
 * // Feature extraction example
 * for (size_t i = 0; i < numSamples - 100; ++i) {
 *     // Detect onset ( sudden increase in amplitude )
 *     float windowEnergy = 0.0f;
 *     for (size_t j = 0; j < 100; ++j) {
 *         windowEnergy += audioData[i + j] * audioData[i + j];
 *     }
 *     
 *     // Compare with previous frame
 *     if (i > 100) {
 *         float prevEnergy = 0.0f;
 *         for (size_t j = 0; j < 100; ++j) {
 *             prevEnergy += audioData[i - 100 + j] * audioData[i - 100 + j];
 *         }
 *         
 *         if (windowEnergy > 10.0f * prevEnergy) {
 *             // Sound onset detected
 *         }
 *     }
 * }
 * @endcode
 * 
 * @see SensoryInput
 * @see AgentBrain
 */
class Audio : public SensoryInput {
public:
    Audio();
    explicit Audio(size_t sampleRate, size_t numSamples);
    ~Audio() override;
    
    const char* getType() const override;
    const std::vector<float>& getData() const override;
    size_t getDimensions() const override;
    std::unique_ptr<SensoryInput> clone() const override;
    
    // Set audio data
    void setData(const std::vector<float>& data);
    void setSampleRate(size_t rate);
    
    size_t getSampleRate() const;
    size_t getNumSamples() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Internal signals (来自大脑内部状态)
/**
 * @class InternalSignals
 * @brief Internal physiological and neural state signals
 * @details InternalSignals represents the agent's internal state, including
 * physiological signals like energy, hunger, thirst, and neural activity patterns.
 * This sensory modality allows the brain to sense its own internal state, enabling
 * homeostatic regulation and self-monitoring.
 * 
 * Internal Systems:
 * - Metabolic signals (energy, blood sugar, hydration)
 * - Neural activity patterns and connectivity
 * - Cognitive states (attention, working memory load)
 * - Emotional states (arousal, valence)
 * - Stress and recovery signals
 * 
 * Thread Safety: Thread-safe for concurrent operations. Internal state is
 * synchronized with brain state updates.
 * 
 * Performance: O(1) for access and modification. Small number of signals typically
 * (10-100) allows efficient vector operations.
 * 
 * Biological Plausibility:
 * - Mirrors interoception in biological systems
 * - Supports predictive coding of internal states
 * - Enables action selection based on needs
 * - Facilitates emotion generation
 * 
 * @code
 * // Create and use internal signals
 * InternalSignals signals;
 * 
 * // Add various physiological signals
 * signals.addSignal(1.0f);    // Energy level
 * signals.addSignal(0.5f);    // Hunger level
 * signals.addSignal(0.8f);    // Attention level
 * signals.addSignal(0.3f);    // Stress level
 * 
 * // Access signals
 * const vector<float>& internalState = signals.getData();
 * 
 * // Process internal state
 * float energy = internalState[0];
 * float hunger = internalState[1];
 * float attention = internalState[2];
 * 
 * // Generate action based on needs
 * if (energy < 0.3f) {
 *     // Need rest - select low-activity action
 * } else if (hunger > 0.7f) {
 *     // Need food - select foraging action
 * } else if (attention < 0.5f) {
 *     // Need stimulation - select exploration action
 * }
 * 
 * // Reset for new episode
 * signals.clearSignals();
 * @endcode
 * 
 * @see SensoryInput
 * @see AgentBrain
 * @see Brain
 */
class InternalSignals : public SensoryInput {
public:
    InternalSignals();
    ~InternalSignals() override;
    
    const char* getType() const override;
    const std::vector<float>& getData() const override;
    size_t getDimensions() const override;
    std::unique_ptr<SensoryInput> clone() const override;
    
    // Add signal
    void addSignal(float value);
    void clearSignals();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
