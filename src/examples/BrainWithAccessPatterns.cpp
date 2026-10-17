#include "BrainAccessor.hpp"
#include "LoggerHelper.hpp"
#include <iostream>

namespace nlm {

// Example: Brain class using new accessor patterns
class BrainWithAccessPatterns {
public:
    BrainWithAccessPatterns(std::shared_ptr<Config> config) : config_(config) {}
    
    // Initialize using SystemInitializer
    bool initialize() {
        SystemInitializer initializer;
        
        // Add systems to initialize
        initializer.addSystemInitializer("WorkingMemory",
            [this](std::shared_ptr<Config> config) {
                return initializeWorkingMemory(config);
            });
            
        initializer.addSystemInitializer("EpisodicMemory",
            [this](std::shared_ptr<Config> config) {
                return initializeEpisodicMemory(config);
            });
            
        // Use template-based access for configuration
        size_t neuronCount = InitHelper::getConfigValue<size_t>(
            config_, "neuron_count", 1000, true);
            
        LoggerHelper::info("Initializing brain with " + std::to_string(neuronCount) + " neurons");
        
        return initializer.initialize(config_);
    }
    
    // Example method using BrainAccessor patterns
    void demonstrateAccessPatterns() {
        // Using BrainAccessor for neural system access
        auto workingMem = BrainAccessor::getWorkingMemory(this);
        if (workingMem) {
            LoggerHelper::info("Working memory active: " + 
                std::to_string(workingMem->getActiveTraces()) + " traces");
        }
        
        auto episodicMem = BrainAccessor::getEpisodicMemory(this);
        if (episodicMem) {
            LoggerHelper::info("Episodic memory: " + 
                std::to_string(episodicMem->getEpisodeCount()) + " episodes");
        }
        
        // Using MathUtils for calculations
        std::vector<float> activations = getCurrentActivations();
        float meanActivation = MathUtils::mean(activations);
        float maxActivation = MathUtils::max(activations);
        
        LoggerHelper::info("Current activation stats - mean: " + 
            std::to_string(meanActivation) + ", max: " + std::to_string(maxActivation));
        
        // Using TimerUtils for performance monitoring
        TimerUtils::ScopedTimer timer("BrainStepAnalysis");
        
        // Analyze current state
        analyzeBrainState();
        
        // Log the timing
        double elapsed = timer.elapsed();
        LoggerHelper::info("Analysis completed in " + TimerUtils::formatTime(elapsed));
    }
    
    // Example of creating and initializing a neural system
    template<typename T>
    std::shared_ptr<T> createAndInitializeSystem(const std::string& systemName) {
        return InitHelper::createAndInitialize<T>(config_, systemName);
    }
    
private:
    bool initializeWorkingMemory(std::shared_ptr<Config> config) {
        auto workingMem = std::make_shared<NeuralWorkingMemory>();
        workingMem->initialize(this);
        
        // Use MathUtils for configuration
        size_t capacity = InitHelper::getConfigValue<size_t>(
            config, "working_memory_capacity", 1000, false);
            
        workingMem->setCapacity(capacity);
        return true;
    }
    
    bool initializeEpisodicMemory(std::shared_ptr<Config> config) {
        auto episodicMem = std::make_shared<NeuralEpisodicMemory>();
        episodicMem->initialize(this);
        
        size_t maxEpisodes = InitHelper::getConfigValue<size_t>(
            config, "episodic_memory_max_episodes", 1000, false);
            
        episodicMem->setMaxEpisodes(maxEpisodes);
        return true;
    }
    
    std::vector<float> getCurrentActivations() {
        std::vector<float> activations;
        // Collect activations from all neurons
        for (const auto& region : regions_) {
            for (const auto& population : region->getPopulations()) {
                for (const auto& neuron : population->getNeurons()) {
                    activations.push_back(neuron->getActivationLevel());
                }
            }
        }
        return activations;
    }
    
    void analyzeBrainState() {
        // Use MathUtils for statistical analysis
        auto activations = getCurrentActivations();
        
        float mean = MathUtils::mean(activations);
        float stddev = MathUtils::standardDeviation(activations, mean);
        float min = MathUtils::min(activations);
        float max = MathUtils::max(activations);
        
        LoggerHelper::info("Brain state analysis - mean: " + std::to_string(mean) + 
            ", stddev: " + std::to_string(stddev) + 
            ", range: [" + std::to_string(min) + ", " + std::to_string(max) + "]");
    }
    
    std::vector<std::unique_ptr<NeuralRegion>> regions_;
    std::shared_ptr<Config> config_;
};

} // namespace nlm
