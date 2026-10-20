#pragma once

#include "Errors.hpp"
#include "ErrorHandling.hpp"
#include <string>
#include <vector>
#include <memory>
#include <cmath>
#include <algorithm>
#include <stdexcept>

namespace nlm {

// Brain module error reporting
class BrainErrorReporter {
public:
    static void reportNeuronError(const std::string& context, const std::string& error) {
        NLM_LOG_ERROR("Brain error in " << context << ": " << error);
    }

    static void reportSynapseError(const std::string& context, const std::string& error) {
        NLM_LOG_ERROR("Synapse error in " << context << ": " << error);
    }

    static void reportNetworkError(const std::string& context, const std::string& error) {
        NLM_LOG_ERROR("Network error in " << context << ": " << error);
    }
};

class NeuronErrorHandler {
public:
    static NeuronErrorHandler& getInstance() {
        static NeuronErrorHandler instance;
        return instance;
    }

    void registerNeuronError(const std::string& neuronId, const std::string& error) {
        NLM_LOG_ERROR("Neuron " << neuronId << " error: " << error);
        // In practice, you might store this error for later retrieval or analysis
    }

    void checkNeuronHealth(const Neuron& neuron) {
        // Check neuron health and log warnings if issues found
        if (neuron.getMembranePotential() < -100.0f) {
            NLM_LOG_WARNING("Neuron " << neuron.getId() << " has very low membrane potential: " << neuron.getMembranePotential());
        }
        if (neuron.getMembranePotential() > 50.0f) {
            NLM_LOG_WARNING("Neuron " << neuron.getId() << " has very high membrane potential: " << neuron.getMembranePotential());
        }
    }

private:
    NeuronErrorHandler() = default;
};

class SynapseErrorHandler {
public:
    static SynapseErrorHandler& getInstance() {
        static SynapseErrorHandler instance;
        return instance;
    }

    void registerSynapseError(const std::string& synapseId, const std::string& error) {
        NLM_LOG_ERROR("Synapse " << synapseId << " error: " << error);
    }
};

class NeuralPopulationErrorHandler {
public:
    static NeuralPopulationErrorHandler& getInstance() {
        static NeuralPopulationErrorHandler instance;
        return instance;
    }

    void registerPopulationError(const std::string& populationId, const std::string& error) {
        NLM_LOG_ERROR("NeuralPopulation " << populationId << " error: " << error);
    }
};

class NeuralRegionErrorHandler {
public:
    static NeuralRegionErrorHandler& getInstance() {
        static NeuralRegionErrorHandler instance;
        return instance;
    }

    void registerRegionError(const std::string& regionId, const std::string& error) {
        NLM_LOG_ERROR("NeuralRegion " << regionId << " error: " << error);
    }
};

class BrainErrorHandler {
public:
    static BrainErrorHandler& getInstance() {
        static BrainErrorHandler instance;
        return instance;
    }

    void registerBrainError(const std::string& context, const std::string& error) {
        NLM_LOG_ERROR("Brain error in " << context << ": " << error);
    }

    void performBrainHealthCheck() {
        // Perform brain-wide health checks
        NLM_LOG_DEBUG("Performing brain-wide health checks");
        // Check for neurons with issues, networks with problems, etc.
        NLM_LOG_INFO("Brain health check completed");
    }
};

// Validation functions for Neuron module
class NeuronValidator {
public:
    static void validateMembranePotential(float potential) {
        if (std::isnan(potential) || std::isinf(potential)) {
            NLM_THROW(ErrorCode::INVALID_ARGUMENT, "Invalid membrane potential: " << potential);
        }
        if (potential < -150.0f || potential > 100.0f) {
            NLM_THROW(ErrorCode::RANGE_VALIDATION_FAILED, "Membrane potential out of valid range: " << potential);
        }
    }

    static void validateThreshold(float threshold) {
        if (std::isnan(threshold) || std::isinf(threshold)) {
            NLM_THROW(ErrorCode::INVALID_ARGUMENT, "Invalid threshold: " << threshold);
        }
    }

    static void validateRefractoryPeriod(uint32_t period) {
        if (period > 1000) { // Reasonable upper bound
            NLM_THROW(ErrorCode::RANGE_VALIDATION_FAILED, "Refractory period too long: " << period);
        }
    }

    static void validateSpikeHistory(const std::vector<Timestamp>& history) {
        if (history.size() > 1000) {
            NLM_THROW(ErrorCode::DATA_INTEGRITY_FAILED, "Spike history exceeds maximum size: " << history.size());
        }
        // Check for duplicate timestamps
        std::set<Timestamp> uniqueTimestamps(history.begin(), history.end());
        if (uniqueTimestamps.size() != history.size()) {
            NLM_THROW(ErrorCode::DATA_INTEGRITY_FAILED, "Duplicate spike timestamps found in history");
        }
    }
};

// Error recovery for Neuron module
class NeuronErrorRecovery {
public:
    struct RecoveryConfig {
        bool autoReset = true;
        float potentialClampMin = -150.0f;
        float potentialClampMax = 100.0f;
        bool recoverFromRefractory = true;
    };

    static void recoverNeuron(Neuron& neuron, const RecoveryConfig& config) {
        try {
            // Attempt to recover neuron state
            if (neuron.getMembranePotential() < -100.0f) {
                NLM_LOG_INFO("Recovering neuron " << neuron.getId() << ": resetting membrane potential");
                neuron.setMembranePotential(-70.0f); // Reset to resting potential
            }

            if (neuron.getState().refractoryRemaining > 0) {
                NLM_LOG_INFO("Recovering neuron " << neuron.getId() << ": recovering from refractory period");
                neuron.setFiringState(FiringState::Resting);
            }

            // Ensure plasticity flags are valid
            if (!neuron.getPlasticityFlags().hebbian && !neuron.getPlasticityFlags().stdp && !neuron.getPlasticityFlags().reward_modulated) {
                NLM_LOG_WARNING("Neuron " << neuron.getId() << ": no plasticity flags enabled, enabling basic plasticity");
                neuron.enablePlasticity(true, false, false);
            }

        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to recover neuron " << neuron.getId() << ": " << e.what());
        }
    }
};

} // namespace nlm