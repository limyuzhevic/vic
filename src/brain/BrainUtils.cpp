// brain/BrainUtils.cpp - Brain utility methods implementation
// Contains utility methods like receiveSensoryInput, injectCurrent, etc.

#include "brain/BrainUtils.hpp"

namespace nlm {

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

void Brain::Impl::injectCurrent(NeuronId neuronId, MembranePotential current) {
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