#pragma once

#include <vector>
#include <string>
#include <memory>

namespace nlm {

// Base class for all sensory input
// 
// This is the abstract base class for all sensory modalities in the NLM system.
// It defines the interface for sensory data that can be injected into the brain.
// Each concrete sensory class (Vision, Audio, InternalSignals) implements
// this interface to provide specific types of sensory information.
// 
// Sensory input represents external or internal stimuli that drive neural activity.
// In NLM, sensory data is converted to neural current and injected into
// sensory neurons, providing the foundation for perception and learning.
// 
// Key characteristics:
// - All sensory input has a timestamp indicating when it was received
// - Data is stored as floating-point vectors for neural processing
// - Each modality has its own dimensionality and format
// - Sensory input can be cloned for parallel processing
// 
// The sensory system is fundamental to the brain's interaction with the world
// and its internal state, forming the basis for perception, learning, and action.
// 
// Example usage:
//   // Create vision sensory input
//   auto vision = std::make_unique<Vision>(16, 16);
//   // Fill with visual data
//   vision->setData(someData);
//   // Get timestamp
//   double timestamp = vision->getTimestamp();
//   
//   // Create internal signals
//   auto internal = std::make_unique<InternalSignals>();
//   internal->addSignal(0.5f);  // Homeostatic signal
//   
//   // Inject into brain
//   brain->receiveSensoryInput(*vision);

class SensoryInput {
public:
    //! @brief Constructor
    //! 
    //! Initializes sensory input with default timestamp (current time).
    //! The actual timestamp should be set when the sensory data is received.

    SensoryInput();
    
    //! @brief Destructor
    //! 
    //! Virtual destructor ensures proper cleanup in derived classes.

    virtual ~SensoryInput() = default;
    
    //! @brief Get input type
    //! 
    //! Returns a human-readable string describing the sensory modality.
    //! 
    //! Returns:
    //!   const char* String literal (e.g., "Vision", "Audio", "Internal")
    //! 
    //! Example:
    //!   SensoryInput* input = ...;
    //!   std::cout << input->getType() << std::endl;
    //!   // Output: "Vision"

    virtual const char* getType() const = 0;

    //! @brief Get raw data as vector
    //! 
    //! Returns the raw sensory data as a vector of floating-point values.
    //! The data format and interpretation depend on the sensory modality.
    //! 
    //! Returns:
    //!   const std::vector<float>& Reference to the data vector
    //!   
    //! @note For Vision: grayscale intensity values 0.0-1.0, row-major
    //! @note For Audio: audio samples, depends on sample rate
    //! @note For Internal: homeostatic signals, typically 4-6 values
    //!
    //! Example:
    //!   auto& data = vision->getData();
    //!   for (float value : data) {
    //!       std::cout << value << " ";
    //!   }

    virtual const std::vector<float>& getData() const = 0;

    //! @brief Get dimensionality
    //! 
    //! Returns the number of elements in the sensory data vector.
    //! This determines how much data is injected into sensory neurons.
    //! 
    //! Returns:
    //!   size_t Number of data elements (e.g., 256 for 16x16 vision)
    //!   
    //! @note For Vision(16,16): returns 256
    //! @note For Audio(44100, 1): returns 44100
    //! @note For InternalSignals(4): returns 4
    //!
    //! Example:
    //!   size_t dims = vision->getDimensions();
    //!   std::cout << "Vision dimensions: " << dims << std::endl;

    virtual size_t getDimensions() const = 0;

    //! @brief Get timestamp
    //! 
    //! Returns the timestamp when this sensory data was received or generated.
    //! 
    //! Returns:
    //!   double Timestamp in seconds since epoch or simulation time
    //!   
    //! Example:
    //!   double timestamp = input->getTimestamp();
    //!   std::cout << "Received at time: " << timestamp << std::endl;

    double getTimestamp() const;

    //! @brief Set timestamp
    //! 
    //! Updates the timestamp of the sensory data.
    //! 
    //! Parameters:
    //!   timestamp: New timestamp value
    //!   
    //! Example:
    //!   input->setTimestamp(12.5);  // Set to 12.5 seconds

    void setTimestamp(double timestamp);

    //! @brief Clone
    //! 
    //! Creates a deep copy of this sensory input.
    //! 
    //! Returns:
    //!   std::unique_ptr<SensoryInput> Copy of this sensory input
    //!   
    //! Example:
    //!   auto copy = original->clone();
    //!   // Use copy in another context

    virtual std::unique_ptr<SensoryInput> clone() const = 0;
    
protected:
    double timestamp_;  // Time when sensory data was received (seconds)
};

// Vision input placeholder
// 
// Represents visual sensory input as a 2D grid of grayscale intensity values.
// Vision is the primary sensory modality for environmental perception.
// The grid has no semantic object labels - only raw intensity values.
// 
// Vision data is used to detect edges, shapes, and patterns for navigation
// and interaction planning. In the NLM system, vision drives the development
// of visual processing circuits in the brain.
// 
// Key properties:
// - Width and height define the visual field resolution
// - Number of channels is typically 1 (grayscale) or 3 (RGB)
// - Data is stored as row-major 1D vector for efficient neural processing
// - Each value is normalized to [0.0, 1.0] range representing intensity
// 
// Vision coordinates map to the brain's visual processing regions and
// influence attention, concept formation, and action planning.
// 
// Example usage:
//   // Create 16x16 grayscale vision input
//   Vision vision(16, 16);
//   
//   // Set visual data (e.g., from camera or simulation)
//   std::vector<float> data(256, 0.0f);
//   // Fill with edge pattern or scene content
//   vision.setData(data);
//   
//   // Access properties
//   size_t width = vision.getWidth();
//   size_t height = vision.getHeight();
//   size_t channels = vision.getChannels();
//   
//   // Use in brain
//   brain->receiveSensoryInput(vision);

class Vision : public SensoryInput {
public:
    //! @brief Default constructor
    //! 
    //! Creates empty vision input with default 16x16 resolution and 1 channel.
    //! 
    //! Example:
    //!   Vision vision;  // 16x16 grayscale

    Vision();
    
    //! @brief Parameterized constructor
    //! 
    //! Creates vision input with specified resolution and channel count.
    //! 
    //! Parameters:
    //!   width: Visual field width in pixels
    //!   height: Visual field height in pixels
    //!   channels: Number of color channels (1=grayscale, 3=RGB)
    //!   
    //! Example:
    //!   Vision vision(32, 24, 1);  // 32x24 grayscale
    //!   Vision vision(640, 480, 3); // 640x480 RGB

    explicit Vision(size_t width, size_t height, size_t channels = 3);
    
    //! @brief Destructor
    //! 
    //! Cleans up vision data resources.

    ~Vision() override;
    
    //! @brief Get input type
    //! 
    //! Returns "Vision" to identify this as visual sensory input.
    //! 
    //! Returns:
    //!   const char* String literal "Vision"

    const char* getType() const override;

    //! @brief Get raw data as vector
    //! 
    //! Returns the vision intensity data as a 1D vector.
    //! 
    //! Returns:
    //!   const std::vector<float>& Reference to intensity values [0.0, 1.0]
    //! 
    //! Example:
    //!   auto& pixels = vision->getData();
    //!   // Access pixel at (x,y): pixels[y * width + x]

    const std::vector<float>& getData() const override;

    //! @brief Get dimensionality
    //! 
    //! Returns total number of intensity values in the vision grid.
    //! 
    //! Returns:
    //!   size_t Number of pixels (width * height * channels)
    //! 
    //! Example:
    //!   size_t dims = vision->getDimensions();  // Returns width*height

    size_t getDimensions() const override;
    
    //! @brief Clone
    //! 
    //! Creates a copy of this vision input.
    //! 
    //! Returns:
    //!   std::unique_ptr<SensoryInput> Copy of this vision input

    std::unique_ptr<SensoryInput> clone() const override;

    //! @brief Set image data
    //! 
    //! Copies vision intensity data into this object.
    //! 
    //! Parameters:
    //!   data: Vector of intensity values [0.0, 1.0]
    //!   size: Number of values in data (for pointer version)
    //! 
    //! Example:
    //!   std::vector<float> data = generateVisionData(16, 16);
    //!   vision->setData(data);
    //!   
    //!   float rawData[100];
    //!   vision->setData(rawData, 100);

    void setData(const std::vector<float>& data);
    void setData(float* data, size_t size);

    //! @brief Get image width
    //! 
    //! Returns:
    //!   size_t Visual field width in pixels
    //! 
    //! Example:
    //!   size_t width = vision->getWidth();
    //!   std::cout << "Width: " << width << " pixels" << std::endl;

    size_t getWidth() const;

    //! @brief Get image height
    //! 
    //! Returns:
    //!   size_t Visual field height in pixels
    //! 
    //! Example:
    //!   size_t height = vision->getHeight();
    //!   std::cout << "Height: " << height << " pixels" << std::endl;

    size_t getHeight() const;

    //! @brief Get channel count
    //! 
    //! Returns:
    //!   size_t Number of color channels (1=grayscale, 3=RGB, etc.)
    //! 
    //! Example:
    //!   size_t channels = vision->getChannels();
    //!   std::cout << "Channels: " << channels << std::endl;

    size_t getChannels() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Audio input placeholder
// 
// Represents auditory sensory input as a sequence of audio samples.
// Audio provides temporal information about sound in the environment,
// which can be used for sound localization, language processing, and
// other audition-based tasks.
// 
// Audio data is typically longer than other sensory modalities and
// contains temporal information. In NLM, audio drives auditory processing
// regions and can influence emotional state, attention, and decision making.
// 
// Key properties:
// - Sample rate defines temporal resolution (e.g., 44100 Hz = CD quality)
// - Number of samples defines duration at given sample rate
// - Data is a 1D vector of audio waveforms
// - Each value is normalized to [-1.0, 1.0] for audio signal range
// 
// Audio processing in the brain includes:
// - Sound localization and direction detection
// - Frequency analysis for pitch and timbre
// - Temporal processing for rhythm and speech
// - Integration with other sensory modalities
// 
// Example usage:
//   // Create audio input (1 second at 44.1kHz)
//   Audio audio(44100, 44100);
//   
//   // Generate or load audio samples
//   std::vector<float> samples = generateAudioSignal();
//   audio->setData(samples);
//   audio->setSampleRate(44100);
//   
//   // Use in brain simulation
//   brain->receiveSensoryInput(audio);

class Audio : public SensoryInput {
public:
    //! @brief Default constructor
    //! 
    //! Creates empty audio input with zero samples and zero sample rate.
    //! 
    //! Example:
    //!   Audio audio;  // Empty audio

    Audio();
    
    //! @brief Parameterized constructor
    //! 
    //! Creates audio input with specified sample rate and number of samples.
    //! 
    //! Parameters:
    //!   sampleRate: Samples per second (e.g., 44100 for CD quality)
    //!   numSamples: Total number of audio samples
    //!   
    //! Example:
    //!   Audio audio(48000, 48000);  // 1 second at 48kHz
    //!   Audio audio(8000, 16000);   // 2 seconds at 8kHz

    explicit Audio(size_t sampleRate, size_t numSamples);
    
    //! @brief Destructor
    //! 
    //! Cleans up audio data resources.

    ~Audio() override;
    
    //! @brief Get input type
    //! 
    //! Returns "Audio" to identify this as auditory sensory input.
    //! 
    //! Returns:
    //!   const char* String literal "Audio"

    const char* getType() const override;

    //! @brief Get raw data as vector
    //! 
    //! Returns the audio waveform data as a 1D vector.
    //! 
    //! Returns:
    //!   const std::vector<float>& Reference to audio samples [-1.0, 1.0]
    //! 
    //! Example:
    //!   auto& samples = audio->getData();
    //!   for (float sample : samples) {
    //!       std::cout << sample << " ";
    //!   }

    const std::vector<float>& getData() const override;

    //! @brief Get dimensionality
    //! 
    //! Returns total number of audio samples.
    //! 
    //! Returns:
    //!   size_t Number of samples (sampleRate * duration in seconds)
    //! 
    //! Example:
    //!   size_t dims = audio->getDimensions();  // Returns numSamples
    //!   size_t duration = dims / sampleRate;   // In seconds

    size_t getDimensions() const override;
    
    //! @brief Clone
    //! 
    //! Creates a copy of this audio input.
    //! 
    //! Returns:
    //!   std::unique_ptr<SensoryInput> Copy of this audio input

    std::unique_ptr<SensoryInput> clone() const override;

    //! @brief Set audio data
    //! 
    //! Copies audio waveform data into this object.
    //! 
    //! Parameters:
    //!   data: Vector of audio samples [-1.0, 1.0]
    //!   
    //! Example:
    //!   std::vector<float> samples = generateAudioSignal(44100, 1.0);
    //!   audio->setData(samples);

    void setData(const std::vector<float>& data);
    
    //! @brief Set sample rate
    //! 
    //! Updates the sample rate (samples per second) for this audio data.
    //! Important for temporal processing and synchronization.
    //! 
    //! Parameters:
    //!   rate: New sample rate in Hz
    //!   
    //! Example:
    //!   audio->setSampleRate(44100);
    //!   // Audio now at 44.1 kHz

    void setSampleRate(size_t rate);

    //! @brief Get sample rate
    //! 
    //! Returns:
    //!   size_t Sample rate in Hz (samples per second)
    //! 
    //! Example:
    //!   size_t sampleRate = audio->getSampleRate();
    //!   std::cout << "Sample rate: " << sampleRate << " Hz" << std::endl;

    size_t getSampleRate() const;

    //! @brief Get number of samples
    //! 
    //! Returns:
    //!   size_t Total number of audio samples
    //! 
    //! Example:
    //!   size_t numSamples = audio->getNumSamples();
    //!   double duration = numSamples / (double)getSampleRate();
    //!   std::cout << "Duration: " << duration << " seconds" << std::endl;

    size_t getNumSamples() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Internal signals (来自大脑内部状态)
// 
// Represents internal state signals from the brain's own systems.
// These signals include homeostatic, neuromodulatory, and metabolic
// information that drive internal regulation and motivation.
// 
// Internal signals are crucial for:
// - Maintaining physiological balance (homeostasis)
// - Generating motivation and drive states
// - Implementing reward prediction and prediction error
// - Regulating plasticity and learning rates
// - Controlling attention and arousal
// 
// Internal signals are generated by specialized brain systems:
// - Dopamine system (reward prediction error)
// - Curiosity system (novelty seeking)
// - Novelty detection (unpredictability)
// - Development system (plasticity modulation)
// - Energy metabolism signals
// 
// Example usage:
//   // Create internal signals
//   InternalSignals internal;
//   
//   // Add homeostatic signals
//   internal->addSignal(0.8f);  // Energy level
//   internal->addSignal(-0.2f); // Stress level
//   internal->addSignal(0.5f);  // Arousal level
//   
//   // Use in brain
//   brain->receiveSensoryInput(internal);

class InternalSignals : public SensoryInput {
public:
    //! @brief Constructor
    //! 
    //! Creates empty internal signals with default capacity for 4 signals.
    //! 
    //! Example:
    //!   InternalSignals internal;  // 4 signal slots

    InternalSignals();
    
    //! @brief Destructor
    //! 
    //! Cleans up internal signal resources.

    ~InternalSignals() override;
    
    //! @brief Get input type
    //! 
    //! Returns "InternalSignals" to identify this as internal state input.
    //! 
    //! Returns:
    //!   const char* String literal "InternalSignals"

    const char* getType() const override;

    //! @brief Get raw data as vector
    //! 
    //! Returns the internal signal data as a 1D vector.
    //! 
    //! Returns:
    //!   const std::vector<float>& Reference to internal signal values
    //! 
    //! Example:
    //!   auto& signals = internal->getData();
    //!   for (float signal : signals) {
    //!       std::cout << signal << " ";
    //!   }

    const std::vector<float>& getData() const override;

    //! @brief Get dimensionality
    //! 
    //! Returns number of internal signals (typically 4: energy, health, etc.).
    //! 
    //! Returns:
    //!   size_t Number of internal signals
    //! 
    //! Example:
    //!   size_t dims = internal->getDimensions();  // Returns 4
    //!   std::cout << "Internal signals: " << dims << std::endl;

    size_t getDimensions() const override;
    
    //! @brief Clone
    //! 
    //! Creates a copy of this internal signals object.
    //! 
    //! Returns:
    //!   std::unique_ptr<SensoryInput> Copy of this internal signals

    std::unique_ptr<SensoryInput> clone() const override;

    //! @brief Add signal
    //! 
    //! Adds an internal signal value to the signals vector.
    //! Internal signals represent various physiological and metabolic states.
    //! 
    //! Parameters:
    //!   value: Signal value (e.g., energy level, stress level)
    //!   
    //! Note:
    //! - Signal values are typically in range [-1.0, 1.0]
    //! - Different signals represent different aspects:
    //!   * Positive: Good state (energy, reward prediction)
    //!   * Negative: Bad state (stress, prediction error)
    //!   * Zero: Baseline/neutral state
    //! 
    //! Example:
    //!   internal->addSignal(0.8f);  // High energy
    //!   internal->addSignal(-0.3f); // Moderate stress
    //!   internal->addSignal(0.1f);  // Neutral arousal

    void addSignal(float value);

    //! @brief Clear all signals
    //! 
    //! Removes all internal signal values.
    //! 
    //! Example:
    //!   internal->clearSignals();  // All signals cleared

    void clearSignals();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
