#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <cmath>

#include "../src/core/Types/Types.hpp"

namespace py = pybind11;
namespace nlm {

// Custom exception classes for better error handling
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigError : public NLMError {
public:
    explicit ConfigError(const std::string& msg) : NLMError("Configuration Error: " + msg) {}
};

class BrainError : public NLMError {
public:
    explicit BrainError(const std::string& msg) : NLMError("Brain Error: " + msg) {}
};

class WorldError : public NLMError {
public:
    explicit WorldError(const std::string& msg) : NLMError("World Error: " + msg) {}
};

// Type validation and conversion utilities
class TypeValidator {
public:
    static void validateNeuronId(const NeuronId& id) {
        if (id.value == UINT64_MAX) {
            throw NLMError("Invalid neuron ID: " + std::to_string(id.value));
        }
    }
    
    static void validateRange(double value, double min, double max, const std::string& paramName) {
        if (value < min || value > max) {
            throw NLMError("Parameter " + paramName + " must be between " + 
                          std::to_string(min) + " and " + std::to_string(max) + ", got " + std::to_string(value));
        }
    }
    
    static void validateNonNegative(int value, const std::string& paramName) {
        if (value < 0) {
            throw NLMError("Parameter " + paramName + " must be non-negative, got " + std::to_string(value));
        }
    }
    
    static void validatePositive(size_t value, const std::string& paramName) {
        if (value == 0) {
            throw NLMError("Parameter " + paramName + " must be positive, got " + std::to_string(value));
        }
    }
};

// Convenience factory functions and helper methods
namespace factory {
    inline std::shared_ptr<Config> createDefaultConfig() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 100);
        config->set("brain.region_count", 1);
        config->set("brain.synapse_density", 0.1);
        config->set("brain.connection_probability", 0.2);
        config->set("brain.membrane_potential_rest", -65.0);
        config->set("brain.membrane_potential_threshold", -50.0);
        config->set("brain.refractory_period", 5);
        config->set("brain.max_firing_rate", 200.0);
        config->set("brain.synaptic_weight", 0.1);
        return config;
    }
    
    inline std::shared_ptr<Brain> createBrainWithConfig(const std::shared_ptr<Config>& config) {
        if (!config) {
            throw BrainError("Configuration cannot be null");
        }
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }
    
    inline std::shared_ptr<SimpleWorld> createSimpleWorld(int width = 100, int height = 100, 
                                                         int visionWidth = 10, int visionHeight = 10) {
        TypeValidator::validatePositive(width, "width");
        TypeValidator::validatePositive(height, "height");
        TypeValidator::validatePositive(visionWidth, "visionWidth");
        TypeValidator::validatePositive(visionHeight, "visionHeight");
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        return world;
    }
    
    inline std::shared_ptr<AgentBrain> createAgentBrain(const std::shared_ptr<Brain>& brain) {
        if (!brain) {
            throw NLMError("Brain cannot be null");
        }
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        return agentBrain;
    }
}

// Batch operations utilities
class BatchOperations {
public:
    // Batch neuron current injection
    static void batchInjectCurrent(const std::shared_ptr<Brain>& brain,
                                   const std::vector<NeuronId>& neuronIds,
                                   double current) {
        if (!brain) {
            throw BrainError("Brain cannot be null");
        }
        for (const auto& id : neuronIds) {
            TypeValidator::validateNeuronId(id);
            brain->injectCurrent(id, current);
        }
    }
    
    // Batch action production
    static std::vector<Action> batchProduceAction(const std::shared_ptr<Brain>& brain,
                                                  const std::vector<Timestamp>& timestamps) {
        if (!brain) {
            throw BrainError("Brain cannot be null");
        }
        std::vector<Action> actions;
        actions.reserve(timestamps.size());
        for (const auto& time : timestamps) {
            actions.push_back(brain->produceAction());
        }
        return actions;
    }
};

// Neural network visualization utilities
class VisualizationUtils {
public:
    // Get brain connectivity matrix
    static py::array_t<float> getConnectivityMatrix(const std::shared_ptr<Brain>& brain) {
        if (!brain) {
            throw BrainError("Brain cannot be null");
        }
        // Get neuron and synapse counts
        auto neuronCount = brain->getTotalNeuronCount();
        auto synapseCount = brain->getTotalSynapseCount();
        
        // Create connectivity matrix (simplified: adjacency matrix)
        py::array_t<float> matrix({neuronCount, neuronCount});
        auto buf = matrix.request();
        float* data = static_cast<float*>(buf.ptr);
        
        // Fill with dummy values for visualization
        for (size_t i = 0; i < neuronCount * neuronCount; ++i) {
            data[i] = 0.0f;
        }
        
        // Add some connections for visualization
        auto regions = brain->getRegions();
        for (const auto& region : regions) {
            // Add intra-region connections
            size_t regionNeuronCount = region->getNeuronCount();
            for (size_t i = 0; i < regionNeuronCount; ++i) {
                for (size_t j = 0; j < regionNeuronCount; ++j) {
                    if (i != j) {
                        data[i * neuronCount + j] = 0.5f;
                    }
                }
            }
        }
        
        return matrix;
    }
    
    // Get brain activity summary
    static py::dict getActivitySummary(const std::shared_ptr<Brain>& brain) {
        if (!brain) {
            throw BrainError("Brain cannot be null");
        }
        py::dict summary;
        summary["total_neurons"] = brain->getTotalNeuronCount();
        summary["total_synapses"] = brain->getTotalSynapseCount();
        summary["active_neurons"] = brain->getActiveNeuronCount();
        summary["firing_neurons"] = brain->getFiringNeuronCount();
        summary["average_firing_rate"] = brain->getAverageFiringRate();
        summary["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        summary["total_spikes"] = brain->getTotalSpikeCount();
        summary["developmental_stage"] = brain->getDevelopmentalStage();
        return summary;
    }
};

// NumPy integration utilities
namespace numpy {
    // Convert std::vector<float> to numpy array
    static py::array_t<float> to_numpy_array(const std::vector<float>& data) {
        return py::array_t<float>(data.size(), data.data());
    }
    
    // Convert std::vector<std::vector<float>> to numpy array
    static py::array_t<float> to_numpy_matrix(const std::vector<std::vector<float>>& data) {
        if (data.empty()) {
            return py::array_t<float>();
        }
        size_t rows = data.size();
        size_t cols = data[0].size();
        py::array_t<float> matrix({rows, cols});
        auto buf = matrix.request();
        float* matrix_data = static_cast<float*>(buf.ptr);
        
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                matrix_data[i * cols + j] = data[i][j];
            }
        }
        return matrix;
    }
    
    // Get numpy array from Vision sensory input
    static py::array_t<float> visionToNumpy(const Vision& vision) {
        size_t size = vision.getWidth() * vision.getHeight() * vision.getChannels();
        const auto& data = vision.getData();
        return py::array_t<float>(size, data.data());
    }
}

} // namespace nlm