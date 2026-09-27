#pragma once

#include "ExperimentEnhanced.hpp"
#include "Phase6IntegratedExperiment.hpp"  // We'll need to create this

namespace nlm {

// Factory for Phase6 Integrated Experiments
class Phase6IntegratedExperimentFactory : public ExperimentFactory {
public:
    std::unique_ptr<Experiment> create(const std::string& name) override {
        return std::make_unique<Phase6IntegratedExperiment>(name);
    }
    
    ExperimentType getType() const override {
        return ExperimentTypes::PHASE6_INTEGRATED;
    }
    
    const std::string& getDescription() const override {
        static const std::string desc = "Phase 6: Final Integration experiment - Tests all brain systems together";
        return desc;
    }
    
    bool validateConfig(const std::map<std::string, ExperimentParameter>& config) const override {
        // Validate configuration for Phase6 integrated experiment
        // Check required parameters
        bool hasMaxSteps = config.find("max_steps") != config.end();
        bool hasNeuronCount = config.find("neuron_count") != config.end();
        
        if (!hasMaxSteps || !hasNeuronCount) {
            return false;
        }
        
        // Validate parameter types
        auto it = config.find("max_steps");
        if (it != config.end() && !std::holds_alternative<size_t>(it->second)) {
            return false;
        }
        
        it = config.find("neuron_count");
        if (it != config.end() && !std::holds_alternative<size_t>(it->second)) {
            return false;
        }
        
        return true;
    }
};

// Factory for Scaling Benchmark Experiments
class ScalingBenchmarkExperimentFactory : public ExperimentFactory {
public:
    std::unique_ptr<Experiment> create(const std::string& name) override {
        return std::make_unique<ScalingBenchmarkExperiment>(name);
    }
    
    ExperimentType getType() const override {
        return ExperimentTypes::SCALING_BENCHMARK;
    }
    
    const std::string& getDescription() const override {
        static const std::string desc = "Scaling benchmark experiment - Tests system performance with different configurations";
        return desc;
    }
    
    bool validateConfig(const std::map<std::string, ExperimentParameter>& config) const override {
        // Validate scaling benchmark configuration
        bool hasInitialNeurons = config.find("initial_neurons") != config.end();
        bool hasMaxNeurons = config.find("max_neurons") != config.end();
        
        if (!hasInitialNeurons || !hasMaxNeurons) {
            return false;
        }
        
        return true;
    }
};

// Factory for Ablation Study Experiments
class AblationStudyExperimentFactory : public ExperimentFactory {
public:
    std::unique_ptr<Experiment> create(const std::string& name) override {
        return std::make_unique<AblationStudyExperiment>(name);
    }
    
    ExperimentType getType() const override {
        return ExperimentTypes::ABLATION_STUDY;
    }
    
    const std::string& getDescription() const override {
        static const std::string desc = "Ablation study experiment - Tests importance of different brain components";
        return desc;
    }
    
    bool validateConfig(const std::map<std::string, ExperimentParameter>& config) const override {
        // Validate ablation study configuration
        bool hasComponent = config.find("component_to_ablate") != config.end();
        
        if (!hasComponent) {
            return false;
        }
        
        auto it = config.find("component_to_ablate");
        if (it != config.end() && !std::holds_alternative<std::string>(it->second)) {
            return false;
        }
        
        return true;
    }
};

// Create global catalog instance
ExperimentCatalog& getExperimentCatalog() {
    static ExperimentCatalog catalog;
    
    // Register built-in experiment types
    static bool registered = false;
    if (!registered) {
        catalog.registerFactory(std::make_unique<Phase6IntegratedExperimentFactory>());
        catalog.registerFactory(std::make_unique<ScalingBenchmarkExperimentFactory>());
        catalog.registerFactory(std::make_unique<AblationStudyExperimentFactory>());
        registered = true;
    }
    
    return catalog;
}

} // namespace nlm
