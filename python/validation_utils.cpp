// Enhanced validation utilities for NLM Python bindings
#ifndef NLM_VALIDATION_UTILS_H
#define NLM_VALIDATION_UTILS_H

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <memory>
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>
#include "../src/core/Config/Config.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/core/Types/Types.hpp"

namespace py = pybind11;
namespace nlm {

// Validation result structure
struct ValidationResult {
    bool is_valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : is_valid(true) {}
    
    void add_error(const std::string& error) {
        errors.push_back(error);
        is_valid = false;
    }
    
    void add_warning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    std::string get_summary() const {
        std::string summary = "Validation " + std::string(is_valid ? "PASSED" : "FAILED");
        if (!errors.empty()) {
            summary += " with " + std::to_string(errors.size()) + " error(s).";
        }
        if (!warnings.empty()) {
            summary += " with " + std::to_string(warnings.size()) + " warning(s).";
        }
        return summary;
    }
};

// Configuration validation utilities
class ConfigValidator {
public:
    static ValidationResult validate(const Config& config) {
        ValidationResult result;
        
        // Validate required keys
        auto keys = config.getKeys();
        std::vector<std::string> required_keys = {
            "brain.neuron_count",
            "brain.region_count",
            "brain.connection_probability",
            "brain.v_thresh",
            "brain.v_rest",
            "brain.v_reset",
            "brain.tau_mem",
            "brain.tau_ref"
        };
        
        for (const auto& required : required_keys) {
            if (!config.has(required)) {
                result.add_error("Missing required configuration key: " + required);
            }
        }
        
        // Validate value ranges
        if (config.has("brain.neuron_count")) {
            auto neuron_count = config.get<int64_t>("brain.neuron_count");
            if (neuron_count.has_value()) {
                if (neuron_count.value() <= 0) {
                    result.add_error("brain.neuron_count must be positive");
                } else if (neuron_count.value() > 1000000) {
                    result.add_warning("brain.neuron_count is very large, may impact performance");
                }
            }
        }
        
        if (config.has("brain.region_count")) {
            auto region_count = config.get<int64_t>("brain.region_count");
            if (region_count.has_value()) {
                if (region_count.value() <= 0) {
                    result.add_error("brain.region_count must be positive");
                }
            }
        }
        
        if (config.has("brain.connection_probability")) {
            auto conn_prob = config.get<double>("brain.connection_probability");
            if (conn_prob.has_value()) {
                if (conn_prob.value() < 0.0 || conn_prob.value() > 1.0) {
                    result.add_error("brain.connection_probability must be between 0.0 and 1.0");
                }
            }
        }
        
        if (config.has("brain.v_thresh")) {
            auto v_thresh = config.get<double>("brain.v_thresh");
            if (v_thresh.has_value()) {
                // V threshold should typically be more negative than resting potential
                double v_rest = config.get<double>("brain.v_rest").value_or(-70.0);
                if (v_thresh.value() >= v_rest) {
                    result.add_error("brain.v_thresh should be more negative than brain.v_rest");
                }
            }
        }
        
        // Validate plasticity settings
        if (config.has("plasticity.stdp.enable")) {
            auto stdp_enable = config.get<bool>("plasticity.stdp.enable");
            if (stdp_enable.has_value()) {
                // STDP is generally required for meaningful learning
                if (!stdp_enable.value()) {
                    result.add_warning("STDP is disabled, brain may not learn effectively");
                }
            }
        }
        
        if (config.has("plasticity.hebbian.enable")) {
            auto hebbian_enable = config.get<bool>("plasticity.hebbian.enable");
            if (hebbian_enable.has_value()) {
                // Hebbian learning is generally required for pattern recognition
                if (!hebbian_enable.value()) {
                    result.add_warning("Hebbian learning is disabled, pattern recognition may be impaired");
                }
            }
        }
        
        // Validate neuromodulation settings
        if (config.has("neuromod.dopamine.scale")) {
            auto dopamine_scale = config.get<double>("neuromod.dopamine.scale");
            if (dopamine_scale.has_value()) {
                if (dopamine_scale.value() < 0.0) {
                    result.add_error("neuromod.dopamine.scale must be non-negative");
                }
            }
        }
        
        return result;
    }
    
    static ValidationResult validate_all(const std::vector<std::shared_ptr<Config>>& configs) {
        ValidationResult result;
        
        for (size_t i = 0; i < configs.size(); ++i) {
            auto validation = validate(*configs[i]);
            if (!validation.is_valid) {
                result.add_error("Config " + std::to_string(i) + ": " + validation.get_summary());
                result.errors.insert(result.errors.end(), validation.errors.begin(), validation.errors.end());
                result.warnings.insert(result.warnings.end(), validation.warnings.begin(), validation.warnings.end());
            }
        }
        
        return result;
    }
};

// Brain validation utilities
class BrainValidator {
public:
    static ValidationResult validate(const Brain& brain) {
        ValidationResult result;
        
        // Check if brain is initialized
        if (!brain.getTotalNeuronCount()) {
            result.add_error("Brain appears to be uninitialized");
            return result;
        }
        
        // Validate neuron counts
        int64_t neuron_count = brain.getTotalNeuronCount();
        if (neuron_count <= 0) {
            result.add_error("Invalid neuron count: " + std::to_string(neuron_count));
        } else if (neuron_count > 1000000) {
            result.add_warning("Very large neuron count may impact performance");
        }
        
        // Validate synapse count
        int64_t synapse_count = brain.getTotalSynapseCount();
        if (synapse_count < 0) {
            result.add_error("Invalid synapse count: " + std::to_string(synapse_count));
        }
        
        // Validate firing neuron count
        int64_t firing_count = brain.getFiringNeuronCount();
        if (firing_count < 0) {
            result.add_error("Invalid firing neuron count: " + std::to_string(firing_count));
        } else if (firing_count > neuron_count) {
            result.add_error("Firing neuron count cannot exceed total neuron count");
        }
        
        // Validate average firing rate
        float avg_firing_rate = brain.getAverageFiringRate();
        if (avg_firing_rate < 0.0) {
            result.add_error("Invalid average firing rate: " + std::to_string(avg_firing_rate));
        } else if (avg_firing_rate > 1000.0) {
            result.add_warning("Very high average firing rate, may indicate instability");
        }
        
        // Validate excitation/inhibition ratio
        float ei_ratio = brain.getExcitationInhibitionRatio();
        if (ei_ratio < 0.0) {
            result.add_error("Invalid excitation/inhibition ratio: " + std::to_string(ei_ratio));
        } else if (ei_ratio > 10.0) {
            result.add_warning("Very high E/I ratio, may indicate imbalance");
        } else if (ei_ratio < 0.1) {
            result.add_warning("Very low E/I ratio, may indicate imbalance");
        }
        
        // Validate total spike count
        int64_t total_spikes = brain.getTotalSpikeCount();
        if (total_spikes < 0) {
            result.add_error("Invalid total spike count: " + std::to_string(total_spikes));
        }
        
        // Validate developmental stage
        DevelopmentalStage stage = brain.getDevelopmentalStage();
        if (static_cast<int>(stage) < 0 || static_cast<int>(stage) >= 5) {
            result.add_error("Invalid developmental stage");
        }
        
        // Check region consistency
        int64_t region_count = brain.getRegionCount();
        if (region_count < 0) {
            result.add_error("Invalid region count: " + std::to_string(region_count));
        }
        
        auto region_ids = brain.getRegionIds();
        for (const auto& rid : region_ids) {
            const auto* region = brain.getRegion(rid);
            if (!region) {
                result.add_error("Region ID " + std::to_string(rid.index()) + " not found");
            }
        }
        
        return result;
    }
};

// AgentBrain validation utilities
class AgentBrainValidator {
public:
    static ValidationResult validate(const AgentBrain& agent) {
        ValidationResult result;
        
        // Check if agent brain has valid underlying brain
        auto brain = agent.getBrain();
        if (!brain) {
            result.add_error("AgentBrain has no valid underlying brain");
            return result;
        }
        
        // Validate brain state
        auto brain_validation = BrainValidator::validate(*brain);
        if (!brain_validation.is_valid) {
            result.add_error("Underlying brain is invalid: " + brain_validation.get_summary());
            result.errors.insert(result.errors.end(), brain_validation.errors.begin(), brain_validation.errors.end());
            result.warnings.insert(result.warnings.end(), brain_validation.warnings.begin(), brain_validation.warnings.end());
        }
        
        // Validate agent-specific settings
        if (agent.isRewardModulationEnabled() && agent.isCuriosityEnabled()) {
            result.add_warning("Both reward modulation and curiosity are enabled; may compete for influence");
        }
        
        if (agent.isDevelopmentEnabled() && agent.isStructuralPlasticityEnabled()) {
            result.add_warning("Both development and structural plasticity are enabled; may compete for resources");
        }
        
        // Validate neuromodulation levels
        float neuromod_level = agent.getNeuromodulationLevel();
        if (neuromod_level < 0.0 || neuromod_level > 1.0) {
            result.add_error("Invalid neuromodulation level: " + std::to_string(neuromod_level));
        }
        
        float curiosity_level = agent.getCuriosityLevel();
        if (curiosity_level < 0.0 || curiosity_level > 1.0) {
            result.add_error("Invalid curiosity level: " + std::to_string(curiosity_level));
        }
        
        float novelty_level = agent.getNoveltyLevel();
        if (novelty_level < 0.0 || novelty_level > 1.0) {
            result.add_error("Invalid novelty level: " + std::to_string(novelty_level));
        }
        
        float prediction_error = agent.getPredictionError();
        if (prediction_error < 0.0) {
            result.add_error("Invalid prediction error: " + std::to_string(prediction_error));
        }
        
        return result;
    }
};

// World validation utilities
class WorldValidator {
public:
    static ValidationResult validate(const SimpleWorld& world) {
        ValidationResult result;
        
        // Validate world dimensions
        int64_t width = world.getWidth();
        int64_t height = world.getHeight();
        
        if (width <= 0) {
            result.add_error("World width must be positive");
        } else if (width > 1000) {
            result.add_warning("Very large world width may impact performance");
        }
        
        if (height <= 0) {
            result.add_error("World height must be positive");
        } else if (height > 1000) {
            result.add_warning("Very large world height may impact performance");
        }
        
        // Validate vision dimensions
        int64_t vision_width = world.getVisionWidth();
        int64_t vision_height = world.getVisionHeight();
        
        if (vision_width <= 0) {
            result.add_error("World vision width must be positive");
        }
        
        if (vision_height <= 0) {
            result.add_error("World vision height must be positive");
        }
        
        // Check if vision dimensions fit within world dimensions
        if (vision_width > width || vision_height > height) {
            result.add_error("Vision dimensions cannot exceed world dimensions");
        }
        
        // Validate agent position
        auto body = world.getAgentBody();
        if (!body.is_alive()) {
            result.add_warning("Agent is not alive");
        }
        
        float x = body.x;
        float y = body.y;
        
        if (x < 0.0 || x >= static_cast<float>(width)) {
            result.add_error("Agent X position is out of bounds");
        }
        
        if (y < 0.0 || y >= static_cast<float>(height)) {
            result.add_error("Agent Y position is out of bounds");
        }
        
        // Validate energy levels
        float max_energy = world.getMaxEnergy();
        float energy_decay = world.getEnergyDecayRate();
        
        if (max_energy <= 0.0) {
            result.add_error("World max energy must be positive");
        }
        
        if (energy_decay < 0.0) {
            result.add_error("World energy decay rate must be non-negative");
        }
        
        // Validate random seed if set
        int64_t seed = world.getRandomSeed();
        if (seed != 0) {
            // Non-zero seed is valid, but warn about deterministic behavior
            result.add_warning("World has fixed random seed; simulation will be deterministic");
        }
        
        return result;
    }
};

// SensoryPercept validation utilities
class SensoryPerceptValidator {
public:
    static ValidationResult validate(const SensoryPercept& percept) {
        ValidationResult result;
        
        // Check if percept contains any data
        auto signals = percept.getAllSignals();
        if (signals.empty()) {
            result.add_error("SensoryPercept contains no signals");
            return result;
        }
        
        // Validate signal values
        for (size_t i = 0; i < signals.size(); ++i) {
            float value = signals[i];
            if (std::isnan(value) || std::isinf(value)) {
                result.add_error("Signal " + std::to_string(i) + " has invalid value");
            } else if (std::abs(value) > 1e6) {
                result.add_warning("Signal " + std::to_string(i) + " has very large magnitude");
            }
        }
        
        // Check consistency between modalities
        auto vision = percept.getVision();
        if (vision) {
            auto vision_data = vision->getData();
            if (!vision_data.empty() && (vision_data.size() % 3 != 0)) {
                result.add_error("Vision data size is not divisible by 3 (RGB)");
            }
        }
        
        return result;
    }
};

// Comprehensive validation utilities
class ValidationUtils {
public:
    static ValidationResult validate_brain_system(
        const Brain& brain,
        const AgentBrain& agent,
        const SimpleWorld& world,
        const SensoryPercept& percept
    ) {
        ValidationResult result;
        
        // Validate each component
        auto brain_validation = BrainValidator::validate(brain);
        if (!brain_validation.is_valid) {
            result.add_error("Brain validation failed: " + brain_validation.get_summary());
            result.errors.insert(result.errors.end(), brain_validation.errors.begin(), brain_validation.errors.end());
            result.warnings.insert(result.warnings.end(), brain_validation.warnings.begin(), brain_validation.warnings.end());
        }
        
        auto agent_validation = AgentBrainValidator::validate(agent);
        if (!agent_validation.is_valid) {
            result.add_error("Agent validation failed: " + agent_validation.get_summary());
            result.errors.insert(result.errors.end(), agent_validation.errors.begin(), agent_validation.errors.end());
            result.warnings.insert(result.warnings.end(), agent_validation.warnings.begin(), agent_validation.warnings.end());
        }
        
        auto world_validation = WorldValidator::validate(world);
        if (!world_validation.is_valid) {
            result.add_error("World validation failed: " + world_validation.get_summary());
            result.errors.insert(result.errors.end(), world_validation.errors.begin(), world_validation.errors.end());
            result.warnings.insert(result.warnings.end(), world_validation.warnings.begin(), world_validation.warnings.end());
        }
        
        auto percept_validation = SensoryPerceptValidator::validate(percept);
        if (!percept_validation.is_valid) {
            result.add_error("SensoryPercept validation failed: " + percept_validation.get_summary());
            result.errors.insert(result.errors.end(), percept_validation.errors.begin(), percept_validation.errors.end());
            result.warnings.insert(result.warnings.end(), percept_validation.warnings.begin(), percept_validation.warnings.end());
        }
        
        return result;
    }
    
    static ValidationResult validate_config_system(
        const std::vector<std::shared_ptr<Config>>& configs
    ) {
        return ConfigValidator::validate_all(configs);
    }
    
    static std::string validation_result_to_string(const ValidationResult& result) {
        std::string output = "Validation Results:\n";
        output += "Status: " + std::string(result.is_valid ? "VALID" : "INVALID") + "\n";
        
        if (!result.errors.empty()) {
            output += "\nErrors:\n";
            for (const auto& error : result.errors) {
                output += "  - " + error + "\n";
            }
        }
        
        if (!result.warnings.empty()) {
            output += "\nWarnings:\n";
            for (const auto& warning : result.warnings) {
                output += "  - " + warning + "\n";
            }
        }
        
        output += "\nSummary: " + result.get_summary();
        return output;
    }
};

} // namespace nlm

#endif // NLM_VALIDATION_UTILS_H
