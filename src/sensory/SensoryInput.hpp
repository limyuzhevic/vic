#pragma once

#include <vector>
#include <string>
#include <memory>

namespace nlm {

/**
 * @file SensoryInput.hpp
 * 
 * Sensory input system for NLM
 * 
 * This file provides the sensory input system for NLM, implementing:
 * - Base sensory input class
 * - Vision input (placeholder)
 * - Audio input (placeholder)
 * - Internal signals (来自大脑内部状态)
 * - Sensory input processing
 * - Sensory input cloning
 * 
 * The sensory input system in NLM:
 * - **SensoryInput**: Base class for all sensory input
 * - **Vision**: Vision input (placeholder)
 * - **Audio**: Audio input (placeholder)
 * - **InternalSignals**: Internal signals (来自大脑内部状态)
 * - **Input processing**: Processes sensory input
 * - **Input cloning**: Clones sensory input
 * 
 * Key features:
 * - **Base class**: Base class for all sensory input
 * - **Sensory types**: Different sensory input types
 * - **Input processing**: Processes sensory input
 * - **Input cloning**: Clones sensory input
 * - **Timestamp tracking**: Tracks sensory input timestamps
 * 
 * Sensory input characteristics in NLM:
 * - **Base class**: Base class for all sensory input
 * - **Sensory types**: Different sensory input types
 * - **Input processing**: Processes sensory input
 * - **Input cloning**: Clones sensory input
 * - **Timestamp tracking**: Tracks sensory input timestamps
 * 
 * @note Critical for sensory input processing
 * @note Used extensively in sensory input systems
 * @note Essential for sensory input management
 * @see SensoryInput.cpp - For implementation details
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */

/**
 * @class SensoryInput
 * 
 * Neural implementation of base sensory input class.
 * 
 * This class implements the base sensory input class, providing
 * the foundation for all sensory input types in NLM. It is essential for:
 * - **Base class**: Base class for all sensory input
 * - **Sensory processing**: Processes sensory input
 * - **Input cloning**: Clones sensory input
 * - **Timestamp tracking**: Tracks sensory input timestamps
 * - **Input management**: Manages sensory input
 * 
 * Sensory input characteristics in NLM:
 * - **Base class**: Base class for all sensory input
 * - **Sensory processing**: Processes sensory input
 * - **Input cloning**: Clones sensory input
 * - **Timestamp tracking**: Tracks sensory input timestamps
 * - **Input management**: Manages sensory input
 * 
 * Key components:
 * - **Input type**: Returns input type
 * - **Input data**: Returns input data as vector
 * - **Input dimensions**: Returns input dimensions
 * - **Timestamp**: Returns and sets timestamp
 * - **Cloning**: Clones sensory input
 * 
 * @note Base class for all sensory input types
 * @see Vision - For vision input
 * @see Audio - For audio input
 * @see InternalSignals - For internal signals
 * 
 * @author NLM Development Team
 */
class SensoryInput {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a default sensory input instance with default parameters.
     * The sensory input is initialized with default values for
     * all input fields.
     */
    SensoryInput();
    
    /**
     * @brief Destructor
     * 
     * Cleans up sensory input resources and performs any necessary
     * cleanup operations. The sensory input is left in a valid but
     * non-operational state after destruction.
     */
    virtual ~SensoryInput() = default;
    
    /**
     * @brief Get input type
     * 
     * Returns the input type. This is used for:
     * - Input type identification
     * - Input type management
     * - Input type processing
     * - Input type control
     */
    virtual const char* getType() const = 0;
    
    /**
     * @brief Get raw data as vector
     * 
     * Returns the raw data as a vector. This is used for:
     * - Input data access
     * - Input data management
     * - Input data processing
     * - Input data control
     */
    virtual const std::vector<float>& getData() const = 0;
    
    /**
     * @brief Get dimensionality
     * 
     * Returns the dimensionality. This is used for:
     * - Input dimensionality analysis
     * - Input dimensionality management
     * - Input dimensionality processing
     * - Input dimensionality control
     */
    virtual size_t getDimensions() const = 0;
    
    /**
     * @brief Get timestamp
     * 
     * Returns the timestamp. This is used for:
     * - Timestamp access
     * - Timestamp management
     * - Timestamp processing
     * - Timestamp control
     */
    double getTimestamp() const;
    
    /**
     * @brief Set timestamp
     * 
     * Sets the timestamp. This is used for:
     * - Timestamp setting
     * - Timestamp management
     * - Timestamp processing
     * - Timestamp control
     */
    void setTimestamp(double timestamp);
    
    /**
     * @brief Clone
     * 
     * Clones the sensory input. This is used for:
     * - Input cloning
     * - Input duplication
     * - Input copying
     * - Input replication
     */
    virtual std::unique_ptr<SensoryInput> clone() const = 0;
    
protected:
    /** Timestamp for sensory input */
    double timestamp_;
};

/**
 * @class Vision
 * 
 * Neural implementation of vision input (placeholder).
 * 
 * This class implements the vision input, providing
 * vision data for NLM. It is essential for:
 * - **Vision input**: Provides vision input
 * - **Vision processing**: Processes vision input
 * - **Vision data**: Provides vision data
 * - **Vision properties**: Manages vision properties
 * 
 * Vision characteristics in NLM:
 * - **Vision input**: Provides vision input
 * - **Vision processing**: Processes vision input
 * - **Vision data**: Provides vision data
 * - **Vision properties**: Manages vision properties
 * 
 * Key components:
 * - **Input type**: Returns input type
 * - **Input data**: Returns input data as vector
 * - **Input dimensions**: Returns input dimensions
 * - **Cloning**: Clones sensory input
 * - **Data setting**: Sets vision data
 * - **Image properties**: Manages image properties
 * 
 * @note Placeholder implementation
 * @see SensoryInput - For base sensory input
 * @see Vision.cpp - For implementation details
 * 
 * @author NLM Development Team
 */
class Vision : public SensoryInput {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a default vision input instance with default parameters.
     * The vision input is initialized with default values for
     * all vision fields.
     */
    Vision();
    
    /**
     * @brief Parameterized constructor
     * 
     * Creates a vision input instance with specified parameters.
     * The vision input is initialized with specified width, height,
     * and channels.
     */
    explicit Vision(size_t width, size_t height, size_t channels = 3);
    
    /**
     * @brief Destructor
     * 
     * Cleans up vision input resources and performs any necessary
     * cleanup operations. The vision input is left in a valid but
     * non-operational state after destruction.
     */
    ~Vision() override;
    
    /**
     * @brief Get input type
     * 
     * Returns the input type. This is used for:
     * - Input type identification
     * - Input type management
     * - Input type processing
     * - Input type control
     */
    const char* getType() const override;
    
    /**
     * @brief Get input data
     * 
     * Returns the input data as a vector. This is used for:
     * - Input data access
     * - Input data management
     * - Input data processing
     * - Input data control
     */
    const std::vector<float>& getData() const override;
    
    /**
     * @brief Get input dimensions
     * 
     * Returns the input dimensions. This is used for:
     * - Input dimensionality analysis
     * - Input dimensionality management
     * - Input dimensionality processing
     * - Input dimensionality control
     */
    size_t getDimensions() const override;
    
    /**
     * @brief Clone
     * 
     * Clones the vision input. This is used for:
     * - Input cloning
     * - Input duplication
     * - Input copying
     * - Input replication
     */
    std::unique_ptr<SensoryInput> clone() const override;
    
    /**
     * @brief Set image data
     * 
     * Sets the image data. This is used for:
     * - Data setting
     * - Image data management
     * - Image data processing
     * - Image data control
     */
    void setData(const std::vector<float>& data);
    
    /**
     * @brief Set image data from pointer
     * 
     * Sets the image data from a pointer. This is used for:
     * - Data setting
     * - Image data management
     * - Image data processing
     * - Image data control
     */
    void setData(float* data, size_t size);
    
    /**
     * @brief Get width
     * 
     * Returns the width. This is used for:
     * - Width access
     * - Width management
     * - Width processing
     * - Width control
     */
    size_t getWidth() const;
    
    /**
     * @brief Get height
     * 
     * Returns the height. This is used for:
     * - Height access
     * - Height management
     * - Height processing
     * - Height control
     */
    size_t getHeight() const;
    
    /**
     * @brief Get channels
     * 
     * Returns the number of channels. This is used for:
     * - Channel access
     * - Channel management
     * - Channel processing
     * - Channel control
     */
    size_t getChannels() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @class Audio
 * 
 * Neural implementation of audio input (placeholder).
 * 
 * This class implements the audio input, providing
 * audio data for NLM. It is essential for:
 * - **Audio input**: Provides audio input
 * - **Audio processing**: Processes audio input
 * - **Audio data**: Provides audio data
 * - **Audio properties**: Manages audio properties
 * 
 * Audio characteristics in NLM:
 * - **Audio input**: Provides audio input
 * - **Audio processing**: Processes audio input
 * - **Audio data**: Provides audio data
 * - **Audio properties**: Manages audio properties
 * 
 * Key components:
 * - **Input type**: Returns input type
 * - **Input data**: Returns input data as vector
 * - **Input dimensions**: Returns input dimensions
 * - **Cloning**: Clones sensory input
 * - **Data setting**: Sets audio data
 * - **Audio properties**: Manages audio properties
 * 
 * @note Placeholder implementation
 * @see SensoryInput - For base sensory input
 * @see Audio.cpp - For implementation details
 * 
 * @author NLM Development Team
 */
class Audio : public SensoryInput {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a default audio input instance with default parameters.
     * The audio input is initialized with default values for
     * all audio fields.
     */
    Audio();
    
    /**
     * @brief Parameterized constructor
     * 
     * Creates an audio input instance with specified parameters.
     * The audio input is initialized with specified sample rate
     * and number of samples.
     */
    explicit Audio(size_t sampleRate, size_t numSamples);
    
    /**
     * @brief Destructor
     * 
     * Cleans up audio input resources and performs any necessary
     * cleanup operations. The audio input is left in a valid but
     * non-operational state after destruction.
     */
    ~Audio() override;
    
    /**
     * @brief Get input type
     * 
     * Returns the input type. This is used for:
     * - Input type identification
     * - Input type management
     * - Input type processing
     * - Input type control
     */
    const char* getType() const override;
    
    /**
     * @brief Get input data
     * 
     * Returns the input data as a vector. This is used for:
     * - Input data access
     * - Input data management
     * - Input data processing
     * - Input data control
     */
    const std::vector<float>& getData() const override;
    
    /**
     * @brief Get input dimensions
     * 
     * Returns the input dimensions. This is used for:
     * - Input dimensionality analysis
     * - Input dimensionality management
     * - Input dimensionality processing
     * - Input dimensionality control
     */
    size_t getDimensions() const override;
    
    /**
     * @brief Clone
     * 
     * Clones the audio input. This is used for:
     * - Input cloning
     * - Input duplication
     * - Input copying
     * - Input replication
     */
    std::unique_ptr<SensoryInput> clone() const override;
    
    /**
     * @brief Set audio data
     * 
     * Sets the audio data. This is used for:
     * - Data setting
     * - Audio data management
     * - Audio data processing
     * - Audio data control
     */
    void setData(const std::vector<float>& data);
    
    /**
     * @brief Set sample rate
     * 
     * Sets the sample rate. This is used for:
     * - Sample rate setting
     * - Audio sample management
     * - Audio sample processing
     * - Audio sample control
     */
    void setSampleRate(size_t rate);
    
    /**
     * @brief Get sample rate
     * 
     * Returns the sample rate. This is used for:
     * - Sample rate access
     * - Sample rate management
     * - Sample rate processing
     * - Sample rate control
     */
    size_t getSampleRate() const;
    
    /**
     * @brief Get number of samples
     * 
     * Returns the number of samples. This is used for:
     * - Sample count access
     * - Sample count management
     * - Sample count processing
     * - Sample count control
     */
    size_t getNumSamples() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @class InternalSignals
 * 
 * Neural implementation of internal signals (来自大脑内部状态).
 * 
 * This class implements the internal signals, providing
 * internal state data for NLM. It is essential for:
 * - **Internal signals**: Provides internal signals
 * - **Internal processing**: Processes internal signals
 * - **Internal data**: Provides internal data
 * - **Internal properties**: Manages internal properties
 * 
 * Internal signals characteristics in NLM:
 * - **Internal signals**: Provides internal signals
 * - **Internal processing**: Processes internal signals
 * - **Internal data**: Provides internal data
 * - **Internal properties**: Manages internal properties
 * 
 * Key components:
 * - **Input type**: Returns input type
 * - **Input data**: Returns input data as vector
 * - **Input dimensions**: Returns input dimensions
 * - **Cloning**: Clones sensory input
 * - **Signal addition**: Adds signals
 * - **Signal clearing**: Clears signals
 * 
 * @note Internal signals (来自大脑内部状态)
 * @see SensoryInput - For base sensory input
 * @see InternalSignals.cpp - For implementation details
 * 
 * @author NLM Development Team
 */
class InternalSignals : public SensoryInput {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a default internal signals instance with default parameters.
     * The internal signals are initialized with default values for
     * all internal signal fields.
     */
    InternalSignals();
    
    /**
     * @brief Destructor
     * 
     * Cleans up internal signals resources and performs any necessary
     * cleanup operations. The internal signals are left in a valid but
     * non-operational state after destruction.
     */
    ~InternalSignals() override;
    
    /**
     * @brief Get input type
     * 
     * Returns the input type. This is used for:
     * - Input type identification
     * - Input type management
     * - Input type processing
     * - Input type control
     */
    const char* getType() const override;
    
    /**
     * @brief Get input data
     * 
     * Returns the input data as a vector. This is used for:
     * - Input data access
     * - Input data management
     * - Input data processing
     * - Input data control
     */
    const std::vector<float>& getData() const override;
    
    /**
     * @brief Get input dimensions
     * 
     * Returns the input dimensions. This is used for:
     * - Input dimensionality analysis
     * - Input dimensionality management
     * - Input dimensionality processing
     * - Input dimensionality control
     */
    size_t getDimensions() const override;
    
    /**
     * @brief Clone
     * 
     * Clones the internal signals. This is used for:
     * - Input cloning
     * - Input duplication
     * - Input copying
     * - Input replication
     */
    std::unique_ptr<SensoryInput> clone() const override;
    
    /**
     * @brief Add signal
     * 
     * Adds a signal. This is used for:
     * - Signal addition
     * - Signal management
     * - Signal processing
     * - Signal control
     */
    void addSignal(float value);
    
    /**
     * @brief Clear signals
     * 
     * Clears all signals. This is used for:
     * - Signal clearing
     * - Signal management
     * - Signal processing
     * - Signal control
     */
    void clearSignals();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
