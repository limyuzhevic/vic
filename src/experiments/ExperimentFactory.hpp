// ExperimentFactory.hpp - Factory for creating experiments
#pragma once

/**
 * ExperimentFactory - Factory for creating experiment instances
 * 
 * This class provides a centralized mechanism for creating
 * experiment objects based on experiment type. It supports
 * dynamic experiment loading and configuration.
 */

#include "../interfaces/BaseExperiment.hpp"
#include <memory>
#include <unordered_map>
#include <string>
#include <functional>

namespace nlm {

/**
 * Factory registration macro for experiments
 * 
 * Usage:
 * REGISTER_EXPERIMENT(Phase3Experiment, ExperimentType::Phase3)
 */
#define REGISTER_EXPERIMENT(ClassName, Type) \
    namespace { \
        class _Register ## ClassName { \
        public: \
            _Register ## ClassName() { \
                ExperimentFactory::registerExperiment( \
                    #ClassName, Type, \
                    [](std::shared_ptr<Config> config) { \
                        return std::make_shared<ClassName>(config); \
                    } \
                ); \
            } \
        }; \
        _Register ## ClassName _instance; \
    }

/**
 * Experiment creation function signature
 */
using ExperimentCreator = std::function<std::shared_ptr<BaseExperiment>(std::shared_ptr<Config>)>;

/**
 * Experiment factory class
 * 
 * Manages experiment creation and registration.
 */
class ExperimentFactory {
public:
    /**
     * Register an experiment class
     * 
     * @param className Name of experiment class
     * @param type Experiment type
     * @param creator Function to create experiment instance
     */
    static void registerExperiment(
        const std::string& className,
        ExperimentType type,
        const ExperimentCreator& creator
    ) {
        experimentCreators_[type] = creator;
        classNames_[type] = className;
    }
    
    /**
     * Create an experiment instance
     * 
     * @param type Experiment type
     * @param config Configuration for the experiment
     * @return Shared pointer to experiment instance
     */
    static std::shared_ptr<BaseExperiment> createExperiment(
        ExperimentType type,
        const std::shared_ptr<Config>& config
    ) {
        auto it = experimentCreators_.find(type);
        if (it == experimentCreators_.end()) {
            throw std::runtime_error("Unknown experiment type: " + std::to_string(static_cast<int>(type)));
        }
        
        try {
            return it->second(config);
        } catch (const std::exception& e) {
            throw std::runtime_error("Failed to create experiment of type " + 
                                   std::to_string(static_cast<int>(type)) + ": " + e.what());
        }
    }
    
    /**
     * Get class name for experiment type
     * 
     * @param type Experiment type
     * @return Class name
     */
    static std::string getClassName(ExperimentType type) {
        auto it = classNames_.find(type);
        if (it == classNames_.end()) {
            return "Unknown";
        }
        return it->second;
    }
    
    /**
     * Get all registered experiment types
     * 
     * @return Vector of all experiment types
     */
    static std::vector<ExperimentType> getRegisteredTypes() {
        std::vector<ExperimentType> types;
        types.reserve(experimentCreators_.size());
        for (const auto& pair : experimentCreators_) {
            types.push_back(pair.first);
        }
        return types;
    }
    
    /**
     * Check if experiment type is registered
     * 
     * @param type Experiment type
     * @return true if type is registered
     */
    static bool isRegistered(ExperimentType type) {
        return experimentCreators_.find(type) != experimentCreators_.end();
    }
    
    /**
     * Clear all registered experiments (for testing)
     */
    static void clear() {
        experimentCreators_.clear();
        classNames_.clear();
    }
    
private:
    static std::unordered_map<ExperimentType, ExperimentCreator> experimentCreators_;
    static std::unordered_map<ExperimentType, std::string> classNames_;
};

// Initialize static members
inline std::unordered_map<ExperimentType, ExperimentCreator> ExperimentFactory::experimentCreators_;
inline std::unordered_map<ExperimentType, std::string> ExperimentFactory::classNames_;

} // namespace nlm
