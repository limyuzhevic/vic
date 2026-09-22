#pragma once

#include "SensoryInput.hpp"

namespace nlm {

/**
 * @brief Sensory observation wrapper representing an agent's environmental perception
 * 
 * The Observation class serves as a container for sensory input data, providing a unified
 * interface to access various types of sensory information in the NLM (Neural Logic Machine)
 * agent system. This class encapsulates raw sensor data from multiple modalities including
 * vision, touch, internal state, and proprioception signals.
 * 
 * Observations are critical for agent perception and decision-making, as they represent
 * the external and internal state of the agent's environment. Each observation can contain
 * multiple sensory modalities and is timestamped to track when the sensory data was captured.
 * 
 * @note This class acts as a facade over the SensoryInput hierarchy, providing convenient
 * access to specific sensory data types while maintaining abstraction of the underlying
 * sensor implementation details.
 * 
 * @see SensoryInput
 * @see Vision
 * @see Audio
 * @see InternalSignals
 * @see AgentBody
 * 
 * @example
 * @code
 * // Creating an observation from vision input
 * Vision visionInput(640, 480);
 * visionInput.setData(visionData);
 * Observation obs(visionInput);
 * 
 * // Accessing observation data
 * const float* visionData = obs.getVisionData();
 * int width = obs.getVisionWidth();
 * int height = obs.getVisionHeight();
 * @endcode
 */
class Observation {
public:
    /**
     * @brief Default constructor
     * 
     * Creates an invalid/undefined observation that contains no sensory data.
     * The observation will be invalid until updated with actual sensory input.
     * 
     * @note Use the parameterized constructor or update() method to populate
     * the observation with valid sensory data.
     * 
     * @see update(const SensoryInput&)
     * @see Observation(const SensoryInput&)
     */
    Observation();

    /**
     * @brief Constructs an observation from sensory input
     * 
     * Initializes the observation with the provided sensory input data.
     * This constructor creates a copy of the sensory input data for encapsulation.
     * 
     * @param[in] input The sensory input data to observe
     * 
     * @pre The input parameter must represent valid sensory data
     * @post The observation becomes valid and contains the sensory input
     * 
     * @note This constructor performs a deep copy of the sensory input data
     * to ensure the observation maintains its own independent copy.
     * 
     * @see clone()
     * @see update(const SensoryInput&)
     */
    Observation(const SensoryInput& input);

    /**
     * @brief Destructor
     * 
     * Cleans up the observation's resources, particularly the unique pointer
     * to sensory input data.
     * 
     * @note The destructor is automatically generated ( defaulted ), ensuring
     * proper cleanup of the std::unique_ptr contained in sensoryInput_.
     */
    ~Observation();

    /**
     * @brief Retrieves a constant pointer to the underlying sensory input
     * 
     * Provides read-only access to the sensory input data contained in this observation.
     * This method is ideal for scenarios where the sensory input should not be modified
     * through this observation.
     * 
     * @return const SensoryInput* Pointer to the sensory input data, or nullptr if
     * the observation is invalid
     * 
     * @note Returns a pointer rather than a reference to maintain flexibility
     * for null return values when the observation is invalid.
     * 
     * @see getSensoryInput()
     * @see isValid()
     */
    const SensoryInput* getSensoryInput() const;

    /**
     * @brief Retrieves a reference to the underlying sensory input
     * 
     * Provides access to the sensory input data contained in this observation.
     * Unlike getSensoryInput(), this method throws an exception if the observation
     * is invalid, ensuring defensive programming practices.
     * 
     * @return const SensoryInput& Reference to the sensory input data
     * 
     * @throws std::runtime_error If the observation is invalid
     * 
     * @note This non-constant version provides guaranteed access to valid data
     * and is preferred when the observation has been validated as valid.
     * 
     * @see getSensoryInput() const
     * @see isValid()
     */
    const SensoryInput& getSensoryInput();

    /**
     * @brief Checks if the observation contains valid sensory data
     * 
     * Determines whether this observation has been properly initialized with
     * valid sensory input data.
     * 
     * @return bool true if the observation contains valid sensory data,
     * false otherwise
     * 
     * @note An observation can be invalid if:
     *       - It was default-constructed
     *       - update() was called with nullptr data
     *       - setFromPercept() was called with an invalid percept
     * 
     * @see getSensoryInput()
     * @see getSensoryInput() const
     */
    bool isValid() const;

    /**
     * @brief Creates a deep copy of this observation
     * 
     * Generates a new observation with identical sensory data content.
     * This is useful for preserving the current observation state for
     * comparison, history tracking, or delayed processing.
     * 
     * @return std::unique_ptr<Observation> A new observation object containing
     * a copy of the sensory data
     * 
     * @note This method performs a deep copy of the underlying sensory input,
     * ensuring the cloned observation is completely independent of the original.
     * 
     * @see Observation(const SensoryInput&)
     * @see update(const SensoryInput&)
     */
    std::unique_ptr<Observation> clone() const;

    /**
     * @brief Updates the observation with new sensory input data
     * 
     * Replaces the current sensory input with new data. This allows an observation
     * to be reused with different sensory information, which is more efficient than
     * creating new observation objects.
     * 
     * @param[in] input The new sensory input data to update the observation with
     * 
     * @pre The input parameter must represent valid sensory data
     * @post The observation's sensory input is replaced with the new data
     * 
     * @note This method performs a deep copy of the new sensory input, ensuring
     * the observation maintains its own independent copy of the data.
     * 
     * @see Observation(const SensoryInput&)
     * @see clone()
     */
    void update(const SensoryInput& input);

    /**
     * @brief Provides direct access to vision sensor data
     * 
     * Retrieves pointer to the raw vision data contained in this observation.
     * Vision data represents the agent's camera/sensor view of the environment,
     * typically as a flattened array of pixel values.
     * 
     * @return const float* Pointer to vision data array, or nullptr if no
     * vision input is present in this observation
     * 
     * @note The returned pointer points to internal data and should not be modified.
     * The data length is determined by getVisionWidth() * getVisionHeight().
     * 
     * @see getVisionWidth()
     * @see getVisionHeight()
     * @see getSensoryInput()
     */
    const float* getVisionData() const;

    /**
     * @brief Provides direct access to touch sensor data
     * 
     * Retrieves pointer to the raw touch data contained in this observation.
     * Touch data represents tactile sensor readings from the agent's
     * touch/force sensors, typically as an array of contact forces or pressures.
     * 
     * @return const float* Pointer to touch data array, or nullptr if no
     * touch input is present in this observation
     * 
     * @note The returned pointer points to internal data and should not be modified.
     * The data length is determined by getTouchCount().
     * 
     * @see getTouchCount()
     * @see getSensoryInput()
     */
    const float* getTouchData() const;

    /**
     * @brief Provides direct access to internal sensor data
     * 
     * Retrieves pointer to the raw internal signals data contained in this observation.
     * Internal data represents signals from the agent's internal state, such as
     * neural activity, energy levels, or biochemical signals.
     * 
     * @return const float* Pointer to internal data array, or nullptr if no
     * internal input is present in this observation
     * 
     * @note The returned pointer points to internal data and should not be modified.
     * The data length is determined by getInternalCount().
     * 
     * @see getInternalCount()
     * @see getSensoryInput()
     */
    const float* getInternalData() const;

    /**
     * @brief Provides direct access to proprioception sensor data
     * 
     * Retrieves pointer to the raw proprioception data contained in this observation.
     * Proprioception data represents the agent's internal sensing of body position,
     * movement, and joint angles, essential for coordinated movement.
     * 
     * @return const float* Pointer to proprioception data array, or nullptr if no
     * proprioception input is present in this observation
     * 
     * @note The returned pointer points to internal data and should not be modified.
     * The data length is determined by getProprioceptionCount().
     * 
     * @see getProprioceptionCount()
     * @see getSensoryInput()
     */
    const float* getProprioceptionData() const;

    /**
     * @brief Retrieves the width of the vision sensor data
     * 
     * Returns the horizontal dimension of vision data captured by the agent's
     * camera or visual sensors. This is essential for image processing and
     * computer vision operations.
     * 
     * @return int Width of vision data in pixels, or 0 if no vision input
     * 
     * @note This value is only valid when vision data is present in the observation.
     * It represents the number of columns in the visual field.
     * 
     * @see getVisionHeight()
     * @see getVisionData()
     * @see getSensoryInput()
     */
    int getVisionWidth() const;

    /**
     * @brief Retrieves the height of the vision sensor data
     * 
     * Returns the vertical dimension of vision data captured by the agent's
     * camera or visual sensors. This is essential for image processing and
     * computer vision operations.
     * 
     * @return int Height of vision data in pixels, or 0 if no vision input
     * 
     * @note This value is only valid when vision data is present in the observation.
     * It represents the number of rows in the visual field.
     * 
     * @see getVisionWidth()
     * @see getVisionData()
     * @see getSensoryInput()
     */
    int getVisionHeight() const;

    /**
     * @brief Retrieves the number of touch sensor readings
     * 
     * Returns the count of touch/force sensor readings in the observation.
     * This indicates how many tactile sensors have detected contact or pressure.
     * 
     * @return int Number of touch sensor readings, or 0 if no touch input
     * 
     * @note This value is only valid when touch data is present in the observation.
     * Each touch sensor typically measures force or pressure at a specific point.
     * 
     * @see getTouchData()
     * @see getSensoryInput()
     */
    int getTouchCount() const;

    /**
     * @brief Retrieves the number of internal sensor readings
     * 
     * Returns the count of internal signal readings in the observation.
     * Internal sensors track the agent's internal state, such as neural activity,
     * energy levels, and biochemical markers.
     * 
     * @return int Number of internal sensor readings, or 0 if no internal input
     * 
     * @note This value is only valid when internal data is present in the observation.
     * These readings represent the agent's physiological state.
     * 
     * @see getInternalData()
     * @see getSensoryInput()
     */
    int getInternalCount() const;

    /**
     * @brief Retrieves the number of proprioception sensor readings
     * 
     * Returns the count of proprioception sensor readings in the observation.
     * Proprioceptive sensors track body position, movement, and joint angles,
     * essential for motor coordination and spatial awareness.
     * 
     * @return int Number of proprioception sensor readings, or 0 if no proprioception input
     * 
     * @note This value is only valid when proprioception data is present in the observation.
     * These readings represent the agent's body awareness and position in space.
     * 
     * @see getProprioceptionData()
     * @see getSensoryInput()
     */
    int getProprioceptionCount() const;

    /**
     * @brief Retrieves the maximum energy level from environmental data
     * 
     * Returns the highest energy level recorded in the agent's environment.
     * This information is crucial for understanding environmental constraints
     * and planning energy-efficient actions.
     * 
     * @return float Maximum energy level in the environment, or 0.0 if no
     * environmental data is present
     * 
     * @note This value is derived from internal sensor readings and represents
     * the most favorable energy condition in the observable environment.
     * 
     * @see getEnergyAt(int, int)
     * @see isHazard(int, int)
     */
    float getMaxEnergy() const;

    /**
     * @brief Checks if a specific location contains a hazard
     * 
     * Determines whether the coordinates (x, y) in the agent's environment
     * contain dangerous or damaging conditions for the agent.
     * 
     * @param[in] x X-coordinate in the environment grid
     * @param[in] y Y-coordinate in the environment grid
     * 
     * @return bool true if a hazard is present at the specified location,
     * false otherwise or if no environmental data is available
     * 
     * @note Hazards may include obstacles, toxic areas, extreme temperatures,
     * or other environmental dangers that could harm the agent.
     * 
     * @see getEnergyAt(int, int)
     * @see getMaxEnergy()
     */
    bool isHazard(int x, int y) const;

    /**
     * @brief Retrieves energy level at a specific environmental location
     * 
     * Returns the energy level available at the specified coordinates in the
     * agent's environment. This is useful for path planning and resource
     * management.
     * 
     * @param[in] x X-coordinate in the environment grid
     * @param[in] y Y-coordinate in the environment grid
     * 
     * @return float Energy level at the specified location, or 0.0 if no
     * environmental data is available or coordinates are out of bounds
     * 
     * @note Energy levels typically range from 0.0 (no energy) to 1.0 (maximum
     * available energy). Values outside this range may indicate error conditions.
     * 
     * @see getMaxEnergy()
     * @see isHazard(int, int)
     */
    float getEnergyAt(int x, int y) const;

private:
    /**
     * @brief The sensory input data contained in this observation
     * 
     * This smart pointer holds the actual sensory input data that this observation
     * represents. It uses a unique_ptr for exclusive ownership and automatic cleanup.
     * 
     * @note The type of sensory data can vary (Vision, Audio, InternalSignals, etc.)
     * depending on what was used to create or update this observation.
     * 
     * @see getSensoryInput()
     * @see getSensoryInput() const
     * @see update(const SensoryInput&)
     */
    std::unique_ptr<SensoryInput> sensoryInput_;
};

} // namespace nlm
