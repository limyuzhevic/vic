#pragma once

#include "Action.hpp"

namespace nlm {

/**
 * @file Observation.hpp
 * 
 * Sensory observation system for NLM
 * 
 * This file provides the sensory observation system for NLM, implementing:
 * - Sensory input observation
 * - Observation validation
 * - Observation cloning
 * - Sensory observation processing
 * - Observation management
 * 
 * The sensory observation system in NLM:
 * - **Observation**: Sensory input observation
 * - **Observation processing**: Processes sensory observation
 * - **Observation validation**: Validates sensory observation
 * - **Observation cloning**: Clones sensory observation
 * - **Sensory observation**: Manages sensory observation
 * 
 * Key features:
 * - **Observation**: Sensory input observation
 * - **Observation processing**: Processes sensory observation
 * - **Observation validation**: Validates sensory observation
 * - **Observation cloning**: Clones sensory observation
 * - **Sensory observation**: Manages sensory observation
 * 
 * Observation characteristics in NLM:
 * - **Observation**: Sensory input observation
 * - **Observation processing**: Processes sensory observation
 * - **Observation validation**: Validates sensory observation
 * - **Observation cloning**: Clones sensory observation
 * - **Sensory observation**: Manages sensory observation
 * 
 * @note Critical for sensory observation
 * @note Used extensively in observation systems
 * @note Essential for observation management
 * @see Action.hpp - For action handling
 * @see Observation.cpp - For implementation details
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */

/**
 * @class Observation
 * 
 * Neural implementation of sensory observation.
 * 
 * This class implements the sensory observation system, providing
 * the foundation for all sensory observation in NLM. It is essential for:
 * - **Observation**: Sensory input observation
 * - **Observation processing**: Processes sensory observation
 * - **Observation validation**: Validates sensory observation
 * - **Observation cloning**: Clones sensory observation
 * - **Sensory observation**: Manages sensory observation
 * 
 * Observation characteristics in NLM:
 * - **Observation**: Sensory input observation
 * - **Observation processing**: Processes sensory observation
 * - **Observation validation**: Validates sensory observation
 * - **Observation cloning**: Clones sensory observation
 * - **Sensory observation**: Manages sensory observation
 * 
 * Key components:
 * - **Sensory input**: Provides sensory input
 * - **Observation access**: Accesses sensory observation
 * - **Observation validation**: Validates sensory observation
 * - **Observation cloning**: Clones sensory observation
 * - **Sensory observation management**: Manages sensory observation
 * 
 * @note Critical for sensory observation
 * @see Action.hpp - For action handling
 * @see SensoryInput.hpp - For sensory input
 * @see Observation.cpp - For implementation details
 * 
 * @author NLM Development Team
 */
class Observation {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a default observation instance with default parameters.
     * The observation is initialized with default values for
     * all observation fields.
     */
    Observation();
    
    /**
     * @brief Parameterized constructor
     * 
     * Creates an observation instance with specified sensory input.
     * The observation is initialized with specified sensory input.
     */
    Observation(const SensoryInput& input);
    
    /**
     * @brief Destructor
     * 
     * Cleans up observation resources and performs any necessary
     * cleanup operations. The observation is left in a valid but
     * non-operational state after destruction.
     */
    ~Observation();
    
    /**
     * @brief Get the sensory input
     * 
     * Returns the sensory input. This is used for:
     * - Sensory observation
     * - Sensory access
     * - Sensory processing
     * - Sensory management
     */
    const SensoryInput* getSensoryInput() const;
    
    /**
     * @brief Check if observation is valid
     * 
     * Checks if the observation is valid. This is used for:
     * - Observation validation
     * - Observation checking
     * - Observation processing
     * - Observation control
     */
    bool isValid() const;
    
    /**
     * @brief Clone
     * 
     * Clones the observation. This is used for:
     * - Observation cloning
     * - Observation duplication
     * - Observation copying
     * - Observation replication
     */
    std::unique_ptr<Observation> clone() const;
    
private:
    /** Sensory input for observation */
    std::unique_ptr<SensoryInput> sensoryInput_;
};

} // namespace nlm
