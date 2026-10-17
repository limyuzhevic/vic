#pragma once

/**
 * Experiment Execution Framework for NLM
 * 
 * Provides a modular framework for running experiments with proper
 * separation of concerns, error handling, and extensibility.
 */

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../environment/Environment.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>

namespace nlm {

/**
 * Experiment configuration base
 */
struct ExperimentConfig {
    std::string name;
    uint64_t seed;
    SimulationStep maxSteps;
    bool enabled;
    
    ExperimentConfig() : seed(42), maxSteps(1000), enabled(true) {}
    ExperimentConfig(const std::string& name) : name(name), seed(42), maxSteps(1000), enabled(true) {}
};

/**
 * Experiment result base
 */
struct ExperimentResult {
    std::string experimentName;
    bool success;
    std::string message;
    SimulationStep stepsCompleted;
    double executionTime;
    
    ExperimentResult() : success(false), stepsCompleted(0), executionTime(0.0) {}
    ExperimentResult(const std::string& name) : experimentName(name), success(false), stepsCompleted(0), executionTime(0.0) {}
};

/**
 * Abstract base class for all experiments
 */
class ExperimentBase {
public:
    virtual ~ExperimentBase() = default;
    
    /**
     * Get experiment name
     */
    virtual std::string getName() const = 0;
    
    /**
     * Get experiment description
     */
    virtual std::string getDescription() const = 0;
    
    /**
     * Get experiment configuration
     */
    virtual ExperimentConfig getConfig() const = 0;
    
    /**
     * Set experiment configuration
     */
    virtual void setConfig(const ExperimentConfig& config) = 0;
    
    /**
     * Run the experiment
     */
    virtual ExperimentResult run(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> environment) = 0;
    
    /**
     * Get last result
     */
    virtual const ExperimentResult& getLastResult() const = 0;
    
    /**
     * Check if experiment is valid
     */
    virtual bool isValid() const = 0;
};

/**
 * Factory for creating experiments
 */
class ExperimentFactory {
public:
    static ExperimentFactory& getInstance() {
        static ExperimentFactory instance;
        return instance;
    }
    
    /**
     * Register an experiment factory function
     */
    template<typename T>
    void registerExperiment(const std::string& name) {
        experimentFactories_[name] = []() -> std::unique_ptr<ExperimentBase> {
            return std::make_unique<T>();
        };
    }
    
    /**
     * Create an experiment by name
     */
    std::unique_ptr<ExperimentBase> createExperiment(const std::string& name) {
        auto it = experimentFactories_.find(name);
        if (it != experimentFactories_.end()) {
            return it->second();
        }
        return nullptr;
    }
    
    /**
     * Get list of registered experiment names
     */
    std::vector<std::string> getRegisteredExperiments() const {
        std::vector<std::string> names;
        for (const auto& pair : experimentFactories_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    /**
     * Check if experiment is registered
     */
    bool hasExperiment(const std::string& name) const {
        return experimentFactories_.find(name) != experimentFactories_.end();
    }

private:
    ExperimentFactory() = default;
    
    std::map<std::string, std::function<std::unique_ptr<ExperimentBase>()>> experimentFactories_;
};

/**
 * Experiment manager for running multiple experiments
 */
class ExperimentManager {
public:
    ExperimentManager() = default;
    ~ExperimentManager() = default;
    
    /**
     * Run a single experiment
     */
    ExperimentResult runExperiment(
        const std::string& experimentName,
        std::shared_ptr<Brain> brain,
        std::shared_ptr<Environment> environment
    ) {
        auto experiment = ExperimentFactory::getInstance().createExperiment(experimentName);
        if (!experiment) {
            return ExperimentResult(experimentName + " not found");
        }
        
        try {
            return experiment->run(brain, environment);
        } catch (const std::exception& e) {
            ExperimentResult result(experimentName);
            result.success = false;
            result.message = std::string("Exception: ") + e.what();
            return result;
        }
    }
    
    /**
     * Run multiple experiments
     */
    std::vector<ExperimentResult> runExperiments(
        const std::vector<std::string>& experimentNames,
        std::shared_ptr<Brain> brainTemplate,
        std::shared_ptr<Environment> environmentTemplate
    ) {
        std::vector<ExperimentResult> results;
        
        for (const auto& name : experimentNames) {
            // Create fresh brain and environment for each experiment
            auto brain = std::make_shared<Brain>(brainTemplate->getConfig());
            brain->initialize();
            
            // In a real implementation, we would also create a fresh environment
            // For now, we'll use the template environment
            
            results.push_back(runExperiment(name, brain, environmentTemplate));
        }
        
        return results;
    }
    
    /**
     * Get all registered experiment names
     */
    std::vector<std::string> getExperimentNames() const {
        return ExperimentFactory::getInstance().getRegisteredExperiments();
    }
    
    /**
     * Check if experiment is registered
     */
    bool hasExperiment(const std::string& name) const {
        return ExperimentFactory::getInstance().hasExperiment(name);
    }
};

/**
 * Experiment base implementation template
 */
template<typename Derived>
class ExperimentBaseImpl : public ExperimentBase {
protected:
    ExperimentConfig config_;
    ExperimentResult lastResult_;
    
public:
    std::string getName() const override {
        return config_.name.empty() ? Derived::getName() : config_.name;
    }
    
    std::string getDescription() const override {
        return Derived::getDescription();
    }
    
    ExperimentConfig getConfig() const override {
        return config_;
    }
    
    void setConfig(const ExperimentConfig& config) override {
        config_ = config;
    }
    
    bool isValid() const override {
        return !getName().empty() && config_.enabled;
    }
    
    const ExperimentResult& getLastResult() const override {
        return lastResult_;
    }
};