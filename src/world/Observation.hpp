#pragma once

#include "../core/Types/Types.hpp"
#include <vector>

namespace nlm {

// Forward declarations
class AgentBody;
class SensoryPercept;

/**
 * @brief Simplified sensory observation wrapper for compatibility with legacy components
 * 
 * The Observation class provides a simplified interface for handling sensory data in the
 * NLM (Neural Logic Machine) agent system. This implementation is designed for compatibility
 * with existing code that expects basic observation functionality without the full
 * sensory input hierarchy complexity.
 * 
 * This simplified version is typically used when:
 * - Compatibility with older code is required
 * - Minimal memory footprint is preferred
 * - The full sensory modality hierarchy is not needed
 * 
 * @note This is a compatibility wrapper around the more comprehensive Observation class
 * found in src/environment/Observation.hpp. For new development, consider using the
 * environment version which provides better abstraction and more features.
 * 
 * @see Observation (src/environment/Observation.hpp)
 * @see AgentBody
 * @see SensoryPercept
 * 
 * @example
 * @code
 * // Creating a basic observation from raw data
 * std::vector<float> sensorData = {0.1f, 0.5f, 0.9f, 0.2f};
 * Observation obs;
 * obs.setFromPercept(percept);
 * 
 * // Accessing observation data
 * const std::vector<float>& data = obs.getData();
 * size_t dims = obs.getDimensions();
 * @endcode
 */
class Observation {
public:
    /**
     * @brief Default constructor
     * 
     * Creates an empty observation with no sensory data. The observation will have
     * zero dimensions and an empty data vector until populated via setFromPercept().
     * 
     * @note This constructor is useful when you want to create an observation
     * object that will be populated later, or when working with optional observations
     * that may or may not contain data.
     * 
     * @see setFromPercept(const SensoryPercept&)
     * @see ~Observation()
     */
    Observation();

    /**
     * @brief Destructor
     * 
     * Cleans up the observation's resources, specifically the data vector contained
     * in data_. Since this class uses the default destructor, it will automatically
     * free the memory allocated for the vector elements.
     * 
     * @note The destructor is automatically generated (defaulted) to ensure proper
     * cleanup of the std::vector container. No additional action is required.
     */
    ~Observation() = default;

    /**
     * @brief Retrieves the complete sensory data vector
     * 
     * Provides read-only access to the complete sensory data stored in this observation.
     * This method is ideal for scenarios where the entire observation data needs to
     * be processed as a single unit.
     * 
     * @return const std::vector<float>& Reference to the sensory data vector.
     * The returned reference points to the internal data_ member and should not be
     * modified directly. For modification, use setFromPercept() or directly modify
     * the data_ vector through the class interface.
     * 
     * @note The vector may be empty if the observation has not been initialized
     * with valid sensory data. The contents represent raw sensor readings in a
     * flattened format.
     * 
     * @see getDimensions()
     * @see setFromPercept(const SensoryPercept&)
     */
    const std::vector<float>& getData() const;

    /**
     * @brief Retrieves the number of dimensions in the sensory data
     * 
     * Returns the dimensionality (size) of the sensory data vector, which indicates
     * how many sensor readings are contained in this observation.
     * 
     * @return size_t The number of elements in the data vector. Returns 0 if the
     * observation is empty or has not been initialized.
     * 
     * @note The dimensionality is equivalent to the size of the data vector
     * and can be used to validate data consistency or determine memory requirements
     * for processing.
     * 
     * @see getData() const
     * @see setFromPercept(const SensoryPercept&)
     */
    size_t getDimensions() const;

    /**
     * @brief Populates the observation from a sensory percept
     * 
     * Extracts and processes sensory data from a SensoryPercept object, converting
     * it into the observation's internal vector format. This method establishes the
     * observation as valid and contains meaningful sensory information.
     * 
     * @param[in] percept The sensory percept object containing the raw sensor data
     * to be extracted and stored in this observation. The percept should contain
     * valid, processed sensory information from the agent's perception system.
     * 
     * @pre The percept parameter must contain valid sensory data
     * @post The observation becomes valid and data_ is populated with the extracted
     * sensory information. Previous data is cleared.
     * 
     * @note This method is the primary way to populate an observation with real
     * sensory data. It handles the conversion from the percept format to the
     * internal vector format.
     * 
     * @see Observation()
     * @see getData() const
     * @see getDimensions() const
     * @see SensoryPercept
     */
    void setFromPercept(const class SensoryPercept& percept);

private:
    /**
     * @brief The raw sensory data vector
     * 
     * This container stores the flattened sensory data readings from the agent's
     * sensors. The vector contains numerical values representing various sensor
     * modalities in a continuous format.
     * 
     * @note The data vector is populated by setFromPercept() and accessed via
     * getData(). It's designed to be compatible with machine learning algorithms
     * and neural network inputs.
     * 
     * @see getData() const
     * @see getDimensions() const
     * @see setFromPercept(const SensoryPercept&)
     */
    std::vector<float> data_;
};

} // namespace nlm
