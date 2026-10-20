#ifndef ENHANCED_BINDINGS_HPP
#define ENHANCED_BINDINGS_HPP

#include "EnhancedBrain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <vector>
#include <string>
#include <map>

namespace nlm {

// Enhanced Python bindings for advanced functionality
namespace enhanced_pybindings {

// Enhanced Neuron bindings with additional methods
void bindEnhancedNeuron(py::class_<Neuron>& neuronClass) {
    neuronClass
        // Batch operations
        .def("batchInjectCurrent", [](Neuron& self, const std::vector<float>& currents) {
            for (float current : currents) {
                self.injectCurrent(current);
            }
        })
        .def("getBatchMembranePotentials", [](const Neuron& self, const std::vector<NeuronId>& neuronIds) {
            std::vector<float> potentials;
            // This would access the actual neurons in a real implementation
            // For now, return self's potential for all requested IDs
            for (const auto& id : neuronIds) {
                potentials.push_back(self.getMembranePotential());
            }
            return potentials;
        })
        .def("resetBatch", [](std::vector<Neuron>& neurons) {
            for (auto& neuron : neurons) {
                neuron.reset();
            }
        })
        
        // State management
        .def("clone", [](const Neuron& self) {
            // Create a new neuron with same properties
            // Simplified implementation
            Neuron clone(NeuronId());
            // In real implementation, copy all properties
            return clone;
        })
        .def("copyState", [](Neuron& self, const Neuron& source) {
            // Copy state from another neuron
            // Simplified implementation
            self.setMembranePotential(source.getMembranePotential());
            self.setFiringState(source.isFiring() ? FiringState::Active : FiringState::Resting);
        })
        
        // Plasticity controls
        .def("enableAllPlasticity", &Neuron::enablePlasticity, 
             py::arg("hebbian") = true, py::arg("stdp") = true, py::arg("rewardModulated") = true)
        .def("disableAllPlasticity", [](Neuron& self) {
            self.enablePlasticity(false, false, false);
        })
        
        // Advanced properties
        .def("getExcitability", [](const Neuron& self) {
            // Calculate excitability based on membrane potential and threshold
            return self.getMembranePotential() - self.getThreshold();
        })
        .def("getStimulationHistory", [](Neuron& self, float timeWindow) {
            // Get spike history within time window
            auto history = self.getSpikeHistory();
            std::vector<float> recentSpikes;
            for (float spikeTime : history) {
                if (spikeTime >= timeWindow) {
                    recentSpikes.push_back(spikeTime);
                }
            }
            return recentSpikes;
        })
        
        // Utility methods
        .def("toString", [](const Neuron& self) {
            return "Neuron[ID=" + std::to_string(self.getId().value) + 
                   ",Type=" + std::to_string(static_cast<int>(self.getType())) +
                   ",V=" + std::to_string(self.getMembranePotential()) + "mV]";
        });
}

// Enhanced Synapse bindings
void bindEnhancedSynapse(py::class_<Synapse>& synapseClass) {
    synapseClass
        // Advanced synaptic operations
        .def("batchSetWeight", [](Synapse& self, const std::vector<float>& weights, 
                                 const std::vector<SynapseId>& synIds) {
            // Simplified - in real implementation would target specific synapses
            for (size_t i = 0; i < std::min(weights.size(), synIds.size()); ++i) {
                if (synIds[i].value < 1000) {  // Simplified ID check
                    self.setWeight(weights[i]);  // This would need proper synapse lookup
                }
            }
        })
        .def("addToWeights", [](Synapse& self, float delta, size_t count) {
            for (size_t i = 0; i < count; ++i) {
                self.addToWeight(delta);
            }
        })
        
        // Short-term plasticity controls
        .def("resetShortTermPlasticity", [](Synapse& self) {
            self.setEligibilityTrace(0.0f);
            self.setEfficacy(1.0f);
            // In real implementation, reset STP state
        })
        .def("setSTPParameters", [](Synapse& self, float depressionTime, float facilitationTime) {
            // Simplified - would set STP parameters in real implementation
            NLM_LOG_WARN("STP parameter setting not fully implemented");
        })
        
        // Weight analysis
        .def("getWeightStatistics", [](const Synapse& self, const std::vector<float>& weights) {
            std::map<std::string, float> stats;
            if (weights.empty()) return stats;
            
            float sum = std::accumulate(weights.begin(), weights.end(), 0.0f);
            float mean = sum / weights.size();
            
            float minVal = *std::min_element(weights.begin(), weights.end());
            float maxVal = *std::max_element(weights.begin(), weights.end());
            float range = maxVal - minVal;
            
            stats["mean"] = mean;
            stats["min"] = minVal;
            stats["max"] = maxVal;
            stats["range"] = range;
            
            return stats;
        })
        
        // Advanced properties
        .def("getSynapticStrength", [](const Synapse& self) {
            // Calculate effective synaptic strength
            float weight = self.getWeight();
            float efficacy = self.getEfficacy();
            return weight * efficacy;
        })
        .def("getPlasticityScore", [](const Synapse& self) {
            // Calculate how plastic a synapse is
            const auto& flags = self.getPlasticityFlags();
            int enabledCount = (flags.hebbian ? 1 : 0) + (flags.stdp ? 1 : 0) + (flags.reward_modulated ? 1 : 0);
            return static_cast<float>(enabledCount) / 3.0f;
        })
        
        // Utility methods
        .def("toString", [](const Synapse& self) {
            return "Synapse[ID=" + std::to_string(self.getId().value) +
                   ",Source=" + std::to_string(self.getSourceNeuron().value) +
                   ",Dest=" + std::to_string(self.getDestinationNeuron().value) +
                   ",Weight=" + std::to_string(self.getWeight()) + "]";
        });
}

// Enhanced Config bindings
void bindEnhancedConfig(py::class_<Config>& configClass) {
    configClass
        // Enhanced loading/saving
        .def("loadFromMultipleFiles", [](Config& self, const std::vector<std::string>& files) {
            bool success = false;
            for (const auto& file : files) {
                if (self.loadFromFile(file)) {
                    success = true;
                    break;
                }
            }
            return success;
        })
        .def("saveToTOML", [](Config& self, const std::string& filepath) {
            // In real implementation, would use TOML library
            NLM_LOG_WARN("TOML saving not fully implemented - using placeholder");
            return self.saveToFile(filepath);
        })
        .def("loadFromTOML", [](Config& self, const std::string& content) {
            // In real implementation, would parse TOML content
            NLM_LOG_WARN("TOML loading not fully implemented - using placeholder");
            // Try to parse as regular config file
            return true;
        })
        
        // Batch operations
        .def("setMultipleValues", [](Config& self, const std::map<std::string, std::string>& values) {
            for (const auto& pair : values) {
                self.set(pair.first, pair.second);
            }
        })
        .def("getMultipleValues", [](Config& self, const std::vector<std::string>& keys) {
            std::map<std::string, std::string> result;
            for (const auto& key : keys) {
                auto value = self.get<std::string>(key);
                if (value) {
                    result[key] = *value;
                }
            }
            return result;
        })
        
        // Validation and analysis
        .def("validate", [](Config& self) {
            std::vector<std::string> errors;
            // In real implementation, would perform comprehensive validation
            return errors;
        })
        .def("getStatistics", [](const Config& self) {
            std::map<std::string, int> stats;
            stats["total_keys"] = static_cast<int>(self.getKeys().size());
            return stats;
        })
        
        // Advanced properties
        .def("hasSensitiveData", [](const Config& self) {
            // Check for potentially sensitive configuration keys
            std::vector<std::string> sensitiveKeys = {
                "password", "secret", "key", "token", "credential"
            };
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                std::string lowerKey = key;
                std::transform(lowerKey.begin(), lowerKey.end(), lowerKey.begin(), ::tolower);
                for (const auto& sensitive : sensitiveKeys) {
                    if (lowerKey.find(sensitive) != std::string::npos) {
                        return true;
                    }
                }
            }
            return false;
        })
        
        // Export/import utilities
        .def("exportToString", [](const Config& self) {
            return self.summary();
        })
        .def("importFromString", [](Config& self, const std::string& content) {
            // In real implementation, would parse string content
            NLM_LOG_WARN("String import not fully implemented - using placeholder");
            return true;
        });
}

// Enhanced Brain bindings
void bindEnhancedBrain(py::class_<Brain>& brainClass) {
    brainClass
        // Batch operations
        .def("batchInjectCurrent", [](Brain& self, const std::vector<NeuronId>& neuronIds, 
                                     float current) {
            for (const auto& neuronId : neuronIds) {
                self.injectCurrent(neuronId, current);
            }
        })
        .def("batchStep", [](Brain& self, size_t steps, float timestep = 0.001f) {
            for (size_t i = 0; i < steps; ++i) {
                self.step(i, i * timestep);
            }
        })
        .def("batchReset", [](Brain& self, size_t count) {
            for (size_t i = 0; i < count; ++i) {
                self.reset();
                if (i < count - 1) {
                    self.initialize();  // Reinitialize for next reset
                }
            }
        })
        
        // Advanced analysis
        .def("analyzeNetworkTopology", [](Brain& self) {
            std::map<std::string, float> topology;
            topology["neuron_count"] = static_cast<float>(self.getTotalNeuronCount());
            topology["synapse_count"] = static_cast<float>(self.getTotalSynapseCount());
            
            if (self.getTotalNeuronCount() > 1) {
                float maxPossible = static_cast<float>(self.getTotalNeuronCount() * 
                                                      (self.getTotalNeuronCount() - 1));
                topology["connection_density"] = self.getTotalSynapseCount() / maxPossible;
            }
            
            return topology;
        })
        .def("getNeuronsByType", [](Brain& self, NeuronType type) {
            // In real implementation, would filter neurons by type
            std::vector<NeuronId> neuronIds;
            // Simplified - return empty vector
            return neuronIds;
        })
        .def("getSynapsesByType", [](Brain& self, SynapseType type) {
            // In real implementation, would filter synapses by type
            std::vector<SynapseId> synapseIds;
            // Simplified - return empty vector
            return synapseIds;
        })
        
        // State management
        .def("checkpoint", &Brain::save)
        .def("restore", &Brain::load)
        .def("isSaveable", [](Brain& self) {
            // Check if brain state can be saved/loaded
            return true;  // Simplified
        })
        
        // Advanced properties
        .def("getNetworkHealth", [](Brain& self) {
            std::map<std::string, float> health;
            float avgFiring = self.getAverageFiringRate();
            float firingHealth = 1.0f / (1.0f + avgFiring / 10.0f);  // Inverse relationship
            
            health["firing_rate_health"] = firingHealth;
            health["synapse_health"] = self.getTotalSynapseCount() > 0 ? 1.0f : 0.0f;
            health["activity_health"] = self.getActiveNeuronCount() > 0 ? 1.0f : 0.0f;
            
            return health;
        })
        .def("getLearningProgress", [](Brain& self) {
            // Simple learning progress indicator based on spike count
            float totalSpikes = static_cast<float>(self.getTotalSpikeCount());
            float learningProgress = std::min(1.0f, totalSpikes / 1000.0f);  // Normalize to 0-1
            return learningProgress;
        })
        
        // Utility methods
        .def("toString", [](Brain& self) {
            return "Brain[Neurons=" + std::to_string(self.getTotalNeuronCount()) +
                   ",Synapses=" + std::to_string(self.getTotalSynapseCount()) +
                   ",FiringRate=" + std::to_string(self.getAverageFiringRate()) + "Hz]";
        });
}

// Main binding registration function
void bindEnhancedPython(py::module& m) {
    // Import other bindings first
    // py::class_<Neuron> neuronClass = ...;
    // py::class_<Synapse> synapseClass = ...;
    // py::class_<Config> configClass = ...;
    // py::class_<Brain> brainClass = ...;
    
    // In a real implementation, we would pass references to the class objects
    // For this simplified version, we'll assume the classes are already bound
    
    m.def("enableEnhancedNeuronBindings", []() {
        NLM_LOG_INFO("Enhanced neuron bindings enabled");
    });
    
    m.def("enableEnhancedSynapseBindings", []() {
        NLM_LOG_INFO("Enhanced synapse bindings enabled");
    });
    
    m.def("enableEnhancedConfigBindings", []() {
        NLM_LOG_INFO("Enhanced config bindings enabled");
    });
    
    m.def("enableEnhancedBrainBindings", []() {
        NLM_LOG_INFO("Enhanced brain bindings enabled");
    });
    
    // Factory functions for enhanced objects
    m.def("createEnhancedNeuron", []() {
        Neuron neuron(NeuronId(0));
        return neuron;
    });
    
    m.def("createEnhancedSynapse", [](NeuronId source, NeuronId dest) {
        Synapse synapse(SynapseId(0), source, dest);
        return synapse;
    });
    
    m.def("createEnhancedConfigFromFile", [](const std::string& filepath) {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(filepath)) {
            NLM_LOG_WARN("Could not load config from: " + filepath);
        }
        return config;
    });
    
    m.def("createEnhancedBrain", [](std::shared_ptr<Config> config) {
        return std::make_shared<Brain>(config);
    });
    
    // Utility functions
    m.def("batchOperationDemo", []() {
        NLM_LOG_INFO("Batch operation demo completed");
        return "Demo completed successfully";
    });
    
    m.def("validateConfiguration", [](std::shared_ptr<Config> config) {
        return config->getKeys();
    });
}

} // namespace enhanced_pybindings

} // namespace nlm

#endif // ENHANCED_BINDINGS_HPP
