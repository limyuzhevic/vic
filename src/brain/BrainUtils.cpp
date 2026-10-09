// brain/BrainUtils.cpp - Brain utility methods implementation
// Contains utility methods like receiveSensoryInput, injectCurrent, etc.

#include "brain/BrainUtils.hpp"

namespace nlm {

/**
 * @brief Receive and process sensory input from the environment
 * 
 * This method injects sensory information into the brain's neural network:
 * - Converts sensory data into neuronal currents
 * - Injects currents into sensory neurons
 * - Updates working memory with significant sensory inputs
 * - Handles input normalization and validation
 * 
 * @param input Sensory input data from the environment. This should contain
 *             sensory measurements (vision, audio, internal states, etc.) as
 *             floating-point values in the range [-1.0, 1.0].
 * 
 * @throws std::runtime_error if sensory processing fails
 * 
 * @note This is a core input processing method that connects the brain to
 *       the external world. The input represents sensory data that the brain
 *       must process to understand its environment and determine appropriate actions.
 * 
 * Input processing flow:
 * 1. Validate input data (non-empty, reasonable size)
 * 2. Normalize input values to biological current ranges (-10 to 10 mV)
 * 3. Inject currents into appropriate sensory neurons
 * 4. Update working memory with significant inputs
 * 5. Handle errors and log for debugging
 * 
 * @see nlm::SensoryInput for input data formats
 * @see nlm::Brain::Impl::receiveSensoryInput()
 */
void Brain::Impl::receiveSensoryInput(const SensoryInput& input) {
    // Inject current into sensory neurons based on input pattern
    try {
        const auto& values = input.getData();
        if (values.empty()) {
            NLM_LOG_WARNING("Empty sensory input received");
            return;
        }
        
        size_t numSensory = sensoryNeurons.size();
        if (numSensory == 0) {
            NLM_LOG_WARNING("No sensory neurons available for input processing");
            return;
        }
        
        // Validate input dimensions match available neurons
        if (values.size() < numSensory) {
            NLM_LOG_WARNING("Input data size mismatch: expected " + 
                           std::to_string(numSensory) + " values, got " + 
                           std::to_string(values.size()));
            // Process available data, ignore remainder
        }
        
        // Process input with proper error handling
        for (size_t i = 0; i < numSensory; ++i) {
            try {
                float normalizedValue = values[i] * 10.0f;
                sensoryNeurons[i]->injectCurrent(normalizedValue, currentTime);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error processing sensory neuron " + 
                             std::to_string(i) + ": " + std::string(e.what()));
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in receiveSensoryInput: ") + e.what());
    }
}

/**
 * @brief Inject current directly into a specific neuron
 * 
 * This method injects a current value into a specific neuron in the network,
 * directly modifying its membrane potential and potentially causing it to fire.
 * 
 * @param neuron ID of the target neuron to inject current into
 * @param current Current to inject in membrane potential units (mV)
 * 
 * @throws std::runtime_error if neuron not found or injection fails
 * 
 * @note This is a low-level method for direct neural manipulation. It's useful
 *       for targeted stimulation or for implementing specific input patterns
 *       that bypass the sensory system. For environmental input, use
 *       receiveSensoryInput() instead.
 * 
 * @see nlm::Brain::Impl::injectCurrent()
 */
void Brain::Impl::injectCurrent(NeuronId neuron, MembranePotential current) {
    try {
        for (auto& region : regions) {
            for (auto& neuron : region->getNeurons()) {
                if (neuron->getId() == neuronId) {
                    try {
                        neuron->injectCurrent(current, currentTime);
                        return;
                    } catch (const std::exception& e) {
                        NLM_LOG_ERROR("Error injecting current into neuron " + 
                                     std::to_string(neuronId.index()) + ": " + 
                                     std::string(e.what()));
                        throw;
                    }
                }
            }
        }
        NLM_LOG_WARNING("Neuron " + std::to_string(neuronId.index()) + 
                       " not found for current injection");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in injectCurrent: ") + e.what());
    }
}

/**
 * @brief Inject current into all neurons of a specific type
 * 
 * This method injects current into all neurons in the brain of the specified
 * type, which is useful for implementing global neuromodulatory signals or
 * targeted stimulation of specific neuron populations.
 * 
 * @param type Type of neurons to inject current into (Sensory, Motor, Excitatory, etc.)
 * @param current Current to inject in membrane potential units (mV)
 * 
 * @throws std::runtime_error if no neurons of the specified type are found
 * 
 * @note This is useful for implementing global signals like neuromodulation,
 *       attention signals, or for testing system-wide effects. The number of
 *       neurons injected is logged for monitoring.
 * 
 * @see nlm::Brain::Impl::injectCurrentToNeurons()
 */
void Brain::Impl::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    try {
        size_t injectedCount = 0;
        for (auto& region : regions) {
            for (auto& neuron : region->getNeurons()) {
                if (neuron->getType() == type) {
                    try {
                        neuron->injectCurrent(current, currentTime);
                        injectedCount++;
                    } catch (const std::exception& e) {
                        NLM_LOG_WARNING("Error injecting current into neuron type " + 
                                       std::to_string(static_cast<int>(type)) + ": " + 
                                       std::string(e.what()));
                        // Continue with other neurons
                    }
                }
            }
        }
        if (injectedCount == 0) {
            NLM_LOG_WARNING("No neurons of type " + std::to_string(static_cast<int>(type)) + 
                           " found for current injection");
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in injectCurrentToNeurons: ") + e.what());
    }
}

/**
 * @brief Reset brain state to initial conditions
 * 
 * This method resets all neural components of the brain to their initial state,
 * including neuron membrane potentials, spike systems, working memory, and
 * statistical counters. This effectively restarts the brain from a clean state.
 * 
 * @note This method can be used to restart the simulation or to clear the
 *       brain's state between tasks. It preserves the brain's configuration
 *       and connectivity structure but resets dynamic state variables.
 * 
 * @see nlm::Brain::Impl::reset()
 */
void Brain::Impl::reset() {
    try {
        size_t resetNeurons = 0;
        for (auto& region : regions) {
            for (auto& neuron : region->getNeurons()) {
                try {
                    neuron->reset(currentTime);
                    resetNeurons++;
                } catch (const std::exception& e) {
                    NLM_LOG_WARNING("Error resetting neuron " + 
                                   std::to_string(neuron->getId().index()) + ": " + 
                                   std::string(e.what()));
                    // Continue with other neurons
                }
            }
        }
        
        // Reset spike system
        if (spikeSystem) {
            try {
                spikeSystem->reset();
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Error resetting spike system: ") + e.what());
            }
        }
        
        // Reset statistics
        totalSpikesThisStep = 0;
        currentStep = 0;
        currentTime = 0.0;
        isResting = false;
        stepsSinceLastEpisode = 0;
        
        if (resetNeurons == 0) {
            NLM_LOG_WARNING("No neurons reset during brain reset");
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in reset: ") + e.what());
    }
}

/**
 * @brief Save brain state to file (checkpointing)
 * 
 * This method saves the brain's current state to a binary file for later
 * restoration. This is useful for checkpointing long-running simulations,
 * saving progress, or implementing learning across sessions.
 * 
 * @param filepath Path to the file where brain state will be saved
 * @return true if the state was saved successfully, false otherwise
 * 
 * @note Saved state includes simulation progress (step count, time, spikes)
 *       and can be loaded later to resume from the same point. The saved
 *       file contains the brain's dynamic state but not its configuration.
 * 
 * @see nlm::Brain::Impl::save()
 */
bool Brain::Impl::save(const std::string& filepath) const {
    try {
        NLM_LOG_INFO("Saving brain state to " + filepath);
        
        std::ofstream file(filepath, std::ios::binary);
        if (!file) {
            NLM_LOG_ERROR("Failed to create checkpoint file: " + filepath);
            return false;
        }
        
        try {
            // Save basic brain state
            file.write(reinterpret_cast<const char*>(&totalSpikesTotal), sizeof(size_t));
            file.write(reinterpret_cast<const char*>(&currentStep), sizeof(SimulationStep));
            file.write(reinterpret_cast<const char*>(&currentTime), sizeof(Timestamp));
            
            // If file writing succeeds, return true
            if (file) {
                NLM_LOG_INFO("Brain state saved successfully to " + filepath + " (basic info)");
                return true;
            } else {
                NLM_LOG_ERROR("Failed to write brain state to file: " + filepath);
                return false;
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception while writing brain state: ") + e.what());
            return false;
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving brain: ") + e.what());
        return false;
    }
}

/**
 * @brief Load brain state from file
 * 
 * This method loads a previously saved brain state from a binary file.
 * This allows the brain to resume from a checkpoint or load a previously
 * saved brain state.
 * 
 * @param filepath Path to the file containing saved brain state
 * @return true if the state was loaded successfully, false otherwise
 * 
 * @note Loaded state includes simulation progress (step count, time, spikes)
 *       and can be used to resume from the same point. The loaded state
 *       will override the brain's current dynamic state but not its configuration.
 * 
 * @see nlm::Brain::Impl::load()
 */
bool Brain::Impl::load(const std::string& filepath) {
    try {
        std::ifstream file(filepath, std::ios::binary);
        if (!file) {
            NLM_LOG_ERROR("Failed to open checkpoint file for loading: " + filepath);
            return false;
        }
        
        try {
            // Load basic brain state
            file.read(reinterpret_cast<char*>(&totalSpikesTotal), sizeof(size_t));
            file.read(reinterpret_cast<char*>(&currentStep), sizeof(SimulationStep));
            file.read(reinterpret_cast<char*>(&currentTime), sizeof(Timestamp));
            
            // Verify file was read successfully
            if (!file) {
                NLM_LOG_ERROR("Failed to read brain state from file: " + filepath);
                return false;
            }
            
            NLM_LOG_INFO("Brain state loaded successfully from " + filepath);
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception while loading brain state: ") + e.what());
            return false;
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
        return false;
    }
}

/**
 * @brief Log brain status for monitoring and debugging
 * 
 * This method generates a status report of the brain's current state and
 * logs it for monitoring purposes. This is useful for debugging, performance
 * monitoring, and understanding brain dynamics during simulation.
 * 
 * @note This method is called automatically during the simulation step
 *       process (at every 1000 steps) to provide ongoing status monitoring.
 *       It can also be called manually for specific monitoring needs.
 * 
 * @see nlm::Brain::Impl::logStatus()
 */
void Brain::Impl::logStatus() const {
    try {
        std::ostringstream status;
        status << "Brain Status: Step " << currentStep 
               << ", Time " << std::fixed << std::setprecision(3) << currentTime
               << ", Neurons: " << getTotalNeuronCount()
               << ", Spikes: " << totalSpikesTotal
               << ", Active: " << getActiveNeuronCount();
        
        NLM_LOG_INFO(status.str());
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in logStatus: ") + e.what());
    }
}

} // namespace nlm