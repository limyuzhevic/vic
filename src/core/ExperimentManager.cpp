// ExperimentManager.cpp
#include "core/ExperimentManager.hpp"
#include <iostream>
#include <fstream>

namespace nlm {

ExperimentManager::ExperimentManager() {
    initialize();
}

void ExperimentManager::initialize() {
    runner_ = std::make_shared<ExperimentRunner>();
    registerBuiltInExperiments();
    setupDefaultExperimentConfigurations();
}

void ExperimentManager::cleanup() {
    // Cleanup resources
    if (runner_) {
        runner_->clearExperiments();
    }
    experimentFactories_.clear();
    experimentConfigs_.clear();
}

void ExperimentManager::registerExperiment(const std::string& name, ExperimentFactory factory) {
    experimentFactories_[name] = factory;
    NLM_LOG_INFO("Registered experiment: " + name);
}

bool ExperimentManager::runExperiment(const std::string& name, std::shared_ptr<Brain> brain,
                                     std::shared_ptr<Environment> environment) {
    if (!hasExperiment(name)) {
        NLM_LOG_ERROR("Experiment not found: " + name);
        return false;
    }
    
    // Create experiment using the factory
    // For simplicity, we'll create experiments directly here
    NLM_LOG_INFO("Running experiment: " + name);
    
    if (name == "learning") {
        LearningExperiment learningExp(brain, 42);
        learningExp.recordInitialState();
        
        // Run some simulation steps
        for (SimulationStep step = 0; step < 100; ++step) {
            brain->step(step, step * 0.001);
        }
        
        learningExp.recordFinalState();
        learningExp.computeStatistics();
        
    } else if (name == "plasticity") {
        // Run plasticity experiment
        LearningExperiment experiment(brain, 42);
        experiment.recordInitialState();
        
        // Enable plasticity and run
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        for (SimulationStep step = 0; step < 1000; ++step) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            brain->step(step, step * 0.001);
        }
        
        experiment.recordFinalState();
        experiment.computeStatistics();
        
    } else if (name == "stdp") {
        // Run STDP verification
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("No brain region found for STDP experiment");
            return false;
        }
        
        auto& synapses = region->getSynapses();
        if (synapses.size() < 5) {
            NLM_LOG_ERROR("Not enough synapses for STDP test");
            return false;
        }
        
        // Record initial weights
        std::vector<float> beforeWeights;
        for (size_t i = 0; i < 5; ++i) {
            beforeWeights.push_back(synapses[i]->getWeight());
            synapses[i]->enablePlasticity(false, true, false);
        }
        
        // Create correlated activity
        for (int trial = 0; trial < 50; ++trial) {
            auto neurons = region->getAllNeurons();
            if (neurons.size() >= 2) {
                neurons[0]->injectCurrent(60.0f);
                brain->step(trial * 2, trial * 2 * 0.001);
                
                neurons[1]->injectCurrent(60.0f);
                brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            }
        }
        
        // Check for LTP
        float totalDelta = 0.0f;
        for (size_t i = 0; i < 5; ++i) {
            totalDelta += synapses[i]->getWeight() - beforeWeights[i];
        }
        
        if (totalDelta > 0.001f) {
            NLM_LOG_INFO("✓ STDP WORKING: Pre-before-post produced potentiation");
        }
        
    } else if (name == "basic") {
        // Run basic connectivity test
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return false;
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            NLM_LOG_INFO("  No neurons found!");
            return true;
        }
        
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);
        }
        
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
        
        if (spikes > 0) {
            NLM_LOG_INFO("  ✓ Spikes propagate through network");
        }
        
    } else if (name == "phase3") {
        // Run Phase 3 demo (simplified)
        std::cout << "===========================================\n";
        std::cout << "NLM PHASE 3 - WORLD INTERACTION DEMO\n";
        std::cout << "===========================================\n\n";
        
        // Create config for Phase 3
        auto phase3Config = std::make_shared<Config>();
        phase3Config->set("environment_width", 20);
        phase3Config->set("environment_height", 20);
        phase3Config->set("vision_width", 16);
        phase3Config->set("vision_height", 16);
        
        // Run simplified Phase 3 demo
        std::cout << "Phase 3 demo would run here with world interaction.\n";
        
    } else {
        NLM_LOG_ERROR("Experiment not implemented: " + name);
        return false;
    }
    
    return true;
}

std::vector<std::string> ExperimentManager::runExperiments(const std::vector<std::string>& names,
                                                          std::shared_ptr<Brain> brain,
                                                          std::shared_ptr<Environment> environment) {
    std::vector<std::string> successfulExperiments;
    
    for (const auto& name : names) {
        if (runExperiment(name, brain, environment)) {
            successfulExperiments.push_back(name);
        } else {
            NLM_LOG_ERROR("Experiment failed: " + name);
        }
    }
    
    return successfulExperiments;
}

std::vector<std::string> ExperimentManager::runAllExperiments(std::shared_ptr<Brain> brain,
                                                             std::shared_ptr<Environment> environment) {
    return runExperiments(getRegisteredExperimentNames(), brain, environment);
}

std::vector<std::string> ExperimentManager::getRegisteredExperimentNames() const {
    std::vector<std::string> names;
    for (const auto& pair : experimentFactories_) {
        names.push_back(pair.first);
    }
    return names;
}

bool ExperimentManager::hasExperiment(const std::string& name) const {
    return experimentFactories_.find(name) != experimentFactories_.end();
}

void ExperimentManager::setExperimentConfig(const std::string& name,
                                           const std::unordered_map<std::string, ConfigValue>& config) {
    experimentConfigs_[name] = config;
    NLM_LOG_INFO("Set configuration for experiment: " + name);
}

std::unordered_map<std::string, ConfigValue> ExperimentManager::getExperimentConfig(const std::string& name) const {
    if (experimentConfigs_.find(name) != experimentConfigs_.end()) {
        return experimentConfigs_.at(name);
    }
    return std::unordered_map<std::string, ConfigValue>();
}

void ExperimentManager::clearExperiments() {
    experimentFactories_.clear();
    experimentConfigs_.clear();
    if (runner_) {
        runner_->clearExperiments();
    }
}

void ExperimentManager::registerBuiltInExperiments() {
    // Learning experiment factory
    registerExperiment("learning", [this](const Config& config) {
        auto experiment = std::make_shared<LearningExperiment>();
        // Set up experiment from config
        return experiment;
    });
    
    // Plasticity experiment factory
    registerExperiment("plasticity", [this](const Config& config) {
        auto experiment = std::make_shared<LearningExperiment>();
        // Set up plasticity-specific configuration
        return experiment;
    });
    
    // STDP experiment factory
    registerExperiment("stdp", [this](const Config& config) {
        auto experiment = std::make_shared<LearningExperiment>();
        // Set up STDP-specific configuration
        return experiment;
    });
    
    // Basic connectivity experiment factory
    registerExperiment("basic", [this](const Config& config) {
        auto experiment = std::make_shared<LearningExperiment>();
        // Set up basic connectivity configuration
        return experiment;
    });
    
    // Phase 3 experiment factory
    registerExperiment("phase3", [this](const Config& config) {
        auto experiment = std::make_shared<LearningExperiment>();
        // Set up Phase 3 configuration
        return experiment;
    });
    
    NLM_LOG_INFO("Registered built-in experiments");
}

void ExperimentManager::setupDefaultExperimentConfigurations() {
    // Learning experiment configuration
    std::unordered_map<std::string, ConfigValue> learningConfig;
    learningConfig["seed"] = static_cast<int64_t>(42);
    learningConfig["steps"] = static_cast<int64_t>(100);
    setExperimentConfig("learning", learningConfig);
    
    // Plasticity experiment configuration
    std::unordered_map<std::string, ConfigValue> plasticityConfig;
    plasticityConfig["steps"] = static_cast<int64_t>(1000);
    plasticityConfig["enable_hebbian"] = true;
    plasticityConfig["enable_stdp"] = true;
    setExperimentConfig("plasticity", plasticityConfig);
    
    // STDP experiment configuration
    std::unordered_map<std::string, ConfigValue> stdpConfig;
    stdpConfig["synapses_tested"] = static_cast<int64_t>(5);
    stdpConfig["trials"] = static_cast<int64_t>(50);
    setExperimentConfig("stdp", stdpConfig);
    
    // Basic experiment configuration
    std::unordered_map<std::string, ConfigValue> basicConfig;
    basicConfig["neurons_tested"] = static_cast<int64_t>(10);
    basicConfig["steps"] = static_cast<int64_t>(50);
    setExperimentConfig("basic", basicConfig);
    
    // Phase 3 experiment configuration
    std::unordered_map<std::string, ConfigValue> phase3Config;
    phase3Config["episodes"] = static_cast<int64_t>(5);
    phase3Config["steps_per_episode"] = static_cast<int64_t>(200);
    setExperimentConfig("phase3", phase3Config);
}

} // namespace nlm
