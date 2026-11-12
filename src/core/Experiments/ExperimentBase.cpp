// Experiment Base Classes Implementation
// Provides common functionality for all NLM experiments

#include "ExperimentBase.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Validation/Validation.hpp"

namespace nlm {
    // Implementations of ExperimentBase methods would go here
    // Most of the functionality is header-only for simplicity
    
    // Additional utility functions could be implemented here
    
    // Example implementation of a common experiment runner template
    template<typename TExperiment>
    class ExperimentRunner {
    public:
        static int run(int argc, char** argv) {
            TExperiment experiment;
            try {
                experiment.setupCommonInfrastructure();
                return experiment.run(argc, argv);
            } catch (const ConfigurationException& e) {
                NLM_LOG_ERROR("Configuration error: " + std::string(e.what()));
                return 1;
            } catch (const ExecutionException& e) {
                NLM_LOG_ERROR("Execution error: " + std::string(e.what()));
                return 2;
            } catch (const ValidationException& e) {
                NLM_LOG_ERROR("Validation error: " + std::string(e.what()));
                return 3;
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Unexpected error: " + std::string(e.what()));
                return 4;
            }
        }
    };
    
    // Helper function to create experiment results
    inline ExperimentResult createExperimentResult(const std::string& name) {
        return ExperimentResult(name);
    }
}

// Additional utility functions implementation
namespace nlm {
    namespace ExperimentUtils {
        
        // Enhanced configuration loader with more sophisticated parsing
        bool advancedLoadConfig(std::shared_ptr<Config> config, const std::string& filename, 
                              bool mergeWithDefaults = true) {
            if (!config) return false;
            
            // Load from file
            bool fileLoaded = config->loadFromFile(filename);
            if (!fileLoaded) {
                NLM_LOG_WARNING("Failed to load config from: " + filename);
                if (!mergeWithDefaults) return false;
            }
            
            // Apply phase-specific defaults if needed
            if (mergeWithDefaults) {
                int phase = 2;  // Default phase
                ConfigurationUtils::applyDefaultsForPhase(config, phase);
            }
            
            return true;
        }
        
        // Validate configuration with comprehensive checking
        bool validateConfiguration(std::shared_ptr<Config> config, int phase = 2) {
            if (!config) {
                NLM_LOG_ERROR("Cannot validate null configuration");
                return false;
            }
            
            auto errorHandler = ErrorHandler();
            
            // Phase-specific validation
            switch (phase) {
                case 2:
                    if (config->has("neuron_count")) {
                        auto neuronCountOpt = config->get<int64_t>("neuron_count");
                        if (neuronCountOpt && (*neuronCountOpt <= 0 || *neuronCountOpt > 10000)) {
                            errorHandler.recordError(ErrorCode::InvalidArgument, 
                                                   "Invalid neuron count: " + std::to_string(*neuronCountOpt), 
                                                   "ConfigurationUtils::validateConfiguration");
                        }
                    }
                    break;
                    
                case 3:
                    // Add Phase 3 specific validation
                    break;
                    
                case 4:
                    // Add Phase 4 specific validation
                    break;
                    
                case 5:
                    // Add Phase 5 specific validation
                    break;
                    
                case 6:
                    // Add Phase 6 specific validation
                    if (config->has("neuron_count")) {
                        auto neuronCountOpt = config->get<int64_t>("neuron_count");
                        if (neuronCountOpt && (*neuronCountOpt <= 0)) {
                            errorHandler.recordError(ErrorCode::InvalidArgument,
                                                   "Neuron count must be positive: " + std::to_string(*neuronCountOpt),
                                                   "ConfigurationUtils::validateConfiguration");
                        }
                    }
                    break;
            }
            
            if (errorHandler.hasErrors()) {
                errorHandler.logErrors("ConfigurationUtils");
                return false;
            }
            
            return true;
        }
    }
}