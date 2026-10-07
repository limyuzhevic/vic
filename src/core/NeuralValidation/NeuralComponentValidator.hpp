#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include <memory>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <sstream>
#include "ErrorSystem.hpp"
#include "Validation/NeuralInputValidator.hpp"

namespace nlm {

// Validation system for neural components
class NeuralComponentValidator {
public:
    struct ValidationReport {
        bool valid = true;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        double overallScore = 1.0;
        
        ValidationReport& addError(const std::string& error) {
            errors.push_back(error);
            valid = false;
            overallScore = std::max(0.0, overallScore - 0.2);
            return *this;
        }
        
        ValidationReport& addWarning(const std::string& warning) {
            warnings.push_back(warning);
            overallScore = std::max(0.0, overallScore - 0.1);
            return *this;
        }
        
        std::string getSummary() const {
            std::ostringstream oss;
            oss << "Validation Report:\n";
            oss << "  Status: " << (valid ? "VALID" : "INVALID") << "\n";
            oss << "  Score: " << overallScore << "/1.0\n";
            oss << "  Errors: " << errors.size() << "\n";
            oss << "  Warnings: " << warnings.size() << "\n";
            if (!valid && !errors.empty()) {
                oss << "  Issues:\n";
                for (const auto& err : errors) {
                    oss << "    - " << err << "\n";
                }
            }
            return oss.str();
        }
    };
    
    // Neuron validation
    class NeuronValidator {
    public:
        ValidationReport validate(const Neuron& neuron) const {
            ValidationReport report;
            
            // Check basic neuron properties
            if (std::isnan(neuron.getId().index())) {
                report.addError("Invalid neuron ID");
            }
            
            // Validate membrane potential
            auto potential = neuron.getState().membranePotential;
            if (std::isnan(potential) || std::isinf(potential)) {
                report.addError("Invalid membrane potential: " + std::to_string(potential));
            } else if (potential < -1000.0f || potential > 1000.0f) {
                report.addWarning("Unusual membrane potential: " + std::to_string(potential));
            }
            
            // Validate threshold
            auto threshold = neuron.getState().threshold;
            if (std::isnan(threshold) || std::isinf(threshold)) {
                report.addError("Invalid threshold: " + std::to_string(threshold));
            }
            
            // Validate firing state consistency
            auto firingState = neuron.getState().firingState;
            auto lastSpike = neuron.getState().lastSpikeTime;
            
            if (firingState == FiringState::Refractory) {
                if (lastSpike < 0.0f) {
                    report.addError("Refractory neuron with invalid spike time");
                } else if (lastSpike > neuron.getState().refractoryPeriod) {
                    report.addError("Refractory period exceeded");
                }
            }
            
            return report;
        }
        
        ValidationReport validateConnectivity(const std::vector<Neuron*>& neurons) const {
            ValidationReport report;
            
            // Check for duplicate neuron IDs
            std::unordered_map<NeuronId, size_t> idCount;
            for (const auto* neuron : neurons) {
                idCount[neuron->getId()]++;
            }
            
            for (const auto& pair : idCount) {
                if (pair.second > 1) {
                    report.addError("Duplicate neuron ID: " + std::to_string(pair.first));
                }
            }
            
            return report;
        }
    };
    
    // Synapse validation
    class SynapseValidator {
    public:
        ValidationReport validate(const Synapse& synapse, const Neuron& source,
                                const Neuron& destination) const {
            ValidationReport report;
            
            // Validate synapse properties
            auto weight = synapse.getWeight();
            if (std::isnan(weight) || std::isinf(weight)) {
                report.addError("Invalid synapse weight: " + std::to_string(weight));
            } else if (weight < -10.0f || weight > 10.0f) {
                report.addWarning("Unusual synapse weight: " + std::to_string(weight));
            }
            
            // Validate source and destination neurons
            if (source.getId() == destination.getId()) {
                report.addError("Self-connection detected");
            }
            
            // Validate delay
            auto delay = synapse.getDelay();
            if (delay < 0) {
                report.addError("Negative delay: " + std::to_string(delay));
            }
            
            // Validate eligibility trace
            auto eligibilityTrace = synapse.getEligibilityTrace();
            if (std::isnan(eligibilityTrace) || std::isinf(eligibilityTrace)) {
                report.addError("Invalid eligibility trace: " + std::to_string(eligibilityTrace));
            } else if (eligibilityTrace < 0.0f) {
                report.addError("Negative eligibility trace: " + std::to_string(eligibilityTrace));
            }
            
            return report;
        }
    };
    
    // Brain validation
    class BrainValidator {
    public:
        ValidationReport validate(const Brain& brain) const {
            ValidationReport report;
            
            // Check basic brain properties
            auto regions = brain.getRegions();
            if (regions.empty()) {
                report.addError("Brain has no regions");
            }
            
            // Validate each region
            for (size_t i = 0; i < regions.size(); ++i) {
                std::string regionPath = "region_" + std::to_string(i);
                
                auto regionReport = validateRegion(*regions[i], regionPath);
                report.errors.insert(report.errors.end(), regionReport.errors.begin(), regionReport.errors.end());
                report.warnings.insert(report.warnings.end(), regionReport.warnings.begin(), regionReport.warnings.end());
                report.overallScore = std::min(report.overallScore, regionReport.overallScore);
            }
            
            return report;
        }
        
    private:
        ValidationReport validateRegion(const NeuralRegion& region, const std::string& path) const {
            ValidationReport report;
            
            // Validate region properties
            auto populations = region.getPopulations();
            if (populations.empty()) {
                report.addError("Region " + path + " has no populations");
            }
            
            for (size_t i = 0; i < populations.size(); ++i) {
                std::string popPath = path + ".population_" + std::to_string(i);
                
                auto popReport = validatePopulation(*populations[i], popPath);
                report.errors.insert(report.errors.end(), popReport.errors.begin(), popReport.errors.end());
                report.warnings.insert(report.warnings.end(), popReport.warnings.begin(), popReport.warnings.end());
                report.overallScore = std::min(report.overallScore, popReport.overallScore);
            }
            
            return report;
        }
        
        ValidationReport validatePopulation(const NeuralPopulation& population,
                                           const std::string& path) const {
            ValidationReport report;
            
            // Validate population properties
            auto neurons = population.getNeurons();
            if (neurons.empty()) {
                report.addError("Population " + path + " has no neurons");
            }
            
            // Validate neuron connectivity
            NeuronValidator neuronValidator;
            auto neuronReport = neuronValidator.validateConnectivity(neurons);
            report.errors.insert(report.errors.end(), neuronReport.errors.begin(), neuronReport.errors.end());
            report.warnings.insert(report.warnings.end(), neuronReport.warnings.begin(), neuronReport.warnings.end());
            report.overallScore = std::min(report.overallScore, neuronReport.overallScore);
            
            return report;
        }
    };
    
    // Configuration validation
    class ConfigValidator {
    public:
        ValidationReport validate(const Config& config) const {
            ValidationReport report;
            
            // Check for critical missing keys
            std::vector<std::string> requiredKeys = {
                "random_seed", "simulation_timestep", "neuron_count", "region_count"
            };
            
            for (const auto& key : requiredKeys) {
                if (!config.has(key)) {
                    report.addError("Missing required configuration key: " + key);
                }
            }
            
            // Validate specific values
            auto seedOpt = config.get<int64_t>("random_seed");
            if (seedOpt) {
                auto rangeReport = validateRange(*seedOpt, 0, 2147483647, "random_seed");
                report.errors.insert(report.errors.end(), rangeReport.errors.begin(), rangeReport.errors.end());
                report.warnings.insert(report.warnings.end(), rangeReport.warnings.begin(), rangeReport.warnings.end());
                report.overallScore = std::min(report.overallScore, rangeReport.overallScore);
            }
            
            auto timestepOpt = config.get<double>("simulation_timestep");
            if (timestepOpt) {
                auto rangeReport = validateRange(*timestepOpt, 0.0, 1.0, "simulation_timestep");
                report.errors.insert(report.errors.end(), rangeReport.errors.begin(), rangeReport.errors.end());
                report.warnings.insert(report.warnings.end(), rangeReport.warnings.begin(), rangeReport.warnings.end());
                report.overallScore = std::min(report.overallScore, rangeReport.overallScore);
            }
            
            auto neuronCountOpt = config.get<int64_t>("neuron_count");
            if (neuronCountOpt) {
                auto rangeReport = validateRange(*neuronCountOpt, 1, 1000000, "neuron_count");
                report.errors.insert(report.errors.end(), rangeReport.errors.begin(), rangeReport.errors.end());
                report.warnings.insert(report.warnings.end(), rangeReport.warnings.begin(), rangeReport.warnings.end());
                report.overallScore = std::min(report.overallScore, rangeReport.overallScore);
            }
            
            auto regionCountOpt = config.get<int64_t>("region_count");
            if (regionCountOpt) {
                auto rangeReport = validateRange(*regionCountOpt, 1, 1000, "region_count");
                report.errors.insert(report.errors.end(), rangeReport.errors.begin(), rangeReport.errors.end());
                report.warnings.insert(report.warnings.end(), rangeReport.warnings.begin(), rangeReport.warnings.end());
                report.overallScore = std::min(report.overallScore, rangeReport.overallScore);
            }
            
            return report;
        }
        
    private:
        ValidationReport validateRange(double value, double min, double max,
                                       const std::string& paramName) const {
            ValidationReport report;
            
            if (value < min) {
                report.addError("Value for " + paramName + " must be >= " + std::to_string(min) +
                              ", got " + std::to_string(value));
            } else if (value > max) {
                report.addError("Value for " + paramName + " must be <= " + std::to_string(max) +
                              ", got " + std::to_string(value));
            }
            
            return report;
        }
    };
    
    // Main validation orchestration
    class ValidationOrchestrator {
    public:
        ValidationOrchestrator() {
            // Register validators
            registerValidator("neuron", std::make_unique<NeuronValidator>());
            registerValidator("synapse", std::make_unique<SynapseValidator>());
            registerValidator("brain", std::make_unique<BrainValidator>());
            registerValidator("config", std::make_unique<ConfigValidator>());
        }
        
        void registerValidator(const std::string& type, std::unique_ptr<ValidationReport> (*validator)()) {
            // Note: This is a simplified registration mechanism
            // In a real implementation, you would use a proper plugin system
        }
        
        ValidationReport validate(const std::string& type, const nlm::Config& config) {
            // Simple validator dispatcher
            if (type == "config") {
                ConfigValidator validator;
                return validator.validate(config);
            }
            
            ValidationReport report;
            report.addError("Unknown validator type: " + type);
            return report;
        }
        
        ValidationReport validate(const std::string& type, const Brain& brain) {
            if (type == "brain") {
                BrainValidator validator;
                return validator.validate(brain);
            }
            
            ValidationReport report;
            report.addError("Unknown validator type: " + type);
            return report;
        }
    };
    
    // Global validation function
    static ValidationReport validateAll(const Config& config, const Brain& brain) {
        ValidationReport overallReport;
        
        // Validate configuration
        ConfigValidator configValidator;
        auto configReport = configValidator.validate(config);
        
        // Validate brain structure
        BrainValidator brainValidator;
        auto brainReport = brainValidator.validate(brain);
        
        // Combine reports
        overallReport.errors.insert(overallReport.errors.end(),
                                   configReport.errors.begin(), configReport.errors.end());
        overallReport.errors.insert(overallReport.errors.end(),
                                   brainReport.errors.begin(), brainReport.errors.end());
        
        overallReport.warnings.insert(overallReport.warnings.end(),
                                     configReport.warnings.begin(), configReport.warnings.end());
        overallReport.warnings.insert(overallReport.warnings.end(),
                                     brainReport.warnings.begin(), brainReport.warnings.end());
        
        overallReport.overallScore = std::min(configReport.overallScore, brainReport.overallScore);
        overallReport.valid = overallReport.errors.empty();
        
        return overallReport;
    }
};

// Convenience functions for easy validation
namespace Validation {
    
    // Validate a single neuron
    static NeuralComponentValidator::ValidationReport validateNeuron(const Neuron& neuron) {
        NeuralComponentValidator::NeuronValidator validator;
        return validator.validate(neuron);
    }
    
    // Validate a synapse
    static NeuralComponentValidator::ValidationReport validateSynapse(const Synapse& synapse,
                                                                      const Neuron& source,
                                                                      const Neuron& destination) {
        NeuralComponentValidator::SynapseValidator validator;
        return validator.validate(synapse, source, destination);
    }
    
    // Validate a brain
    static NeuralComponentValidator::ValidationReport validateBrain(const Brain& brain) {
        NeuralComponentValidator::BrainValidator validator;
        return validator.validate(brain);
    }
    
    // Validate configuration
    static NeuralComponentValidator::ValidationReport validateConfig(const Config& config) {
        NeuralComponentValidator::ConfigValidator validator;
        return validator.validate(config);
    }
    
    // Complete validation of brain and configuration
    static NeuralComponentValidator::ValidationReport validateAll(const Config& config, const Brain& brain) {
        return NeuralComponentValidator::validateAll(config, brain);
    }
}

} // namespace nlm