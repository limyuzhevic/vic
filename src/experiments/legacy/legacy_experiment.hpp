#pragma once

#include "Experiment.hpp"
#include "ExperimentRunner.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

// Legacy experiment compatibility - maintains backward compatibility with existing code
class LegacyExperiment {
public:
    LegacyExperiment(const std::string& name) {
        experiment_ = std::make_shared<Experiment>(name);
    }
    
    ~LegacyExperiment() = default;
    
    // Forward all methods to new Experiment class
    const std::string& getName() const { return experiment_->getName(); }
    void setName(const std::string& name) { experiment_->setName(name); }
    
    uint64_t getSeed() const { return experiment_->getSeed(); }
    void setSeed(uint64_t seed) { experiment_->setSeed(seed); }
    
    void setConfigValue(const std::string& key, const std::string& value) {
        experiment_->setConfigValue(key, value);
    }
    std::string getConfigValue(const std::string& key) const {
        return experiment_->getConfigValue(key);
    }
    
    void recordMetric(const std::string& name, double value) {
        experiment_->recordMetric(name, value);
    }
    
    const std::vector<std::pair<std::string, double>>& getMetrics() const {
        return experiment_->getMetrics();
    }
    
    double getMetric(const std::string& name) const {
        return experiment_->getMetric(name);
    }
    
    bool hasMetric(const std::string& name) const {
        return experiment_->hasMetric(name);
    }
    
    void recordStep(SimulationStep step, size_t neuronCount, size_t synapseCount,
                    size_t firingCount, float avgFiringRate) {
        experiment_->recordStep(step, neuronCount, synapseCount, firingCount, avgFiringRate);
    }
    
    const std::vector<Experiment::StepData>& getStepData() const {
        return experiment_->getStepData();
    }
    
    SimulationStep getStartStep() const { return experiment_->getStartStep(); }
    void setStartStep(SimulationStep step) { experiment_->setStartStep(step); }
    SimulationStep getEndStep() const { return experiment_->getEndStep(); }
    void setEndStep(SimulationStep step) { experiment_->setEndStep(step); }
    
    void addNote(const std::string& note) { experiment_->addNote(note); }
    const std::vector<std::string>& getNotes() const { return experiment_->getNotes(); }
    
    // Additional compatibility methods
    void setSeed(const std::string& seedStr) {
        try {
            uint64_t seed = std::stoul(seedStr);
            setSeed(seed);
        } catch (...) {
            setSeed(42); // Default seed
        }
    }
    
    std::string getStatus() const {
        std::stringstream ss;
        ss << "Experiment: " << getName() << std::endl;
        ss << "  Seed: " << getSeed() << std::endl;
        ss << "  Start Step: " << getStartStep() << std::endl;
        ss << "  End Step: " << getEndStep() << std::endl;
        ss << "  Metrics Count: " << getMetrics().size() << std::endl;
        ss << "  Notes Count: " << getNotes().size() << std::endl;
        return ss.str();
    }
    
    bool saveToFile(const std::string& filename) {
        // Use the new Experiment's save functionality if available
        return experiment_->saveState(filename);
    }
    
    bool loadFromFile(const std::string& filename) {
        return experiment_->loadState(filename);
    }

private:
    std::shared_ptr<Experiment> experiment_;
};

// Legacy experiment runner compatibility - maintains backward compatibility
class LegacyExperimentRunner {
public:
    LegacyExperimentRunner() {
        runner_ = std::make_shared<ExperimentRunner>();
    }
    
    ~LegacyExperimentRunner() = default;
    
    // Forward all methods to new ExperimentRunner class
    std::shared_ptr<LegacyExperiment> createExperiment(const std::string& name) {
        auto exp = runner_->createExperiment(name);
        return std::make_shared<LegacyExperiment>(name);
    }
    
    bool runExperiment(std::shared_ptr<LegacyExperiment> experiment,
                      std::shared_ptr<Brain> brain,
                      std::shared_ptr<Environment> environment,
                      SimulationStep maxSteps) {
        return runner_->runExperiment(experiment->getSharedPtr(), brain, environment, maxSteps);
    }
    
    // Legacy method for backward compatibility
    bool runExperimentLegacy(std::shared_ptr<LegacyExperiment> experiment,
                            std::shared_ptr<Brain> brain,
                            std::shared_ptr<Environment> environment,
                            SimulationStep maxSteps) {
        // Legacy interface - convert to new interface
        if (!experiment || !brain || !environment) {
            return false;
        }
        
        experiment->setStartStep(0);
        
        // Reset environment and brain
        auto observation = environment->reset();
        brain->receiveSensoryInput(*observation);
        
        // Run simulation loop
        for (SimulationStep step = 0; step < maxSteps && !environment->isDone(); ++step) {
            // Brain step
            brain->step(step);
            
            // Record data
            experiment->recordStep(
                step,
                brain->getTotalNeuronCount(),
                brain->getTotalSynapseCount(),
                brain->getFiringNeuronCount(),
                brain->getAverageFiringRate()
            );
            
            // Get action from brain
            auto action = brain->produceAction();
            
            // Environment step
            observation = environment->step(*action);
            
            // Send observation to brain
            brain->receiveSensoryInput(*observation);
            
            // Record reward
            float reward = environment->getLastReward();
            experiment->recordMetric("total_reward", 
                experiment->getMetric("total_reward") + reward);
        }
        
        experiment->setEndStep(maxSteps);
        return true;
    }
    
    const std::vector<std::shared_ptr<LegacyExperiment>>& getExperiments() const {
        static std::vector<std::shared_ptr<LegacyExperiment>> empty;
        return empty; // Legacy interface - would need implementation
    }
    
    std::shared_ptr<LegacyExperiment> getExperiment(const std::string& name) const {
        // Legacy interface - would need implementation
        return nullptr;
    }
    
    void clearExperiments() {
        runner_->clearExperiments();
    }
    
    void setGlobalSeed(uint64_t seed) {
        runner_->setGlobalSeed(seed);
    }
    
    bool saveResults(const std::string& filepath) const {
        return runner_->saveResults(filepath);
    }
    
    // Legacy helper methods
    void setGlobalSeed(const std::string& seedStr) {
        try {
            uint64_t seed = std::stoul(seedStr);
            setGlobalSeed(seed);
        } catch (...) {
            setGlobalSeed(42); // Default seed
        }
    }
    
    std::string getStatus() const {
        std::stringstream ss;
        ss << "Legacy Experiment Runner Status:" << std::endl;
        ss << "  Number of experiments: " << getExperiments().size() << std::endl;
        ss << "  Global seed: " << runner_->getGlobalSeed() << std::endl; // Would need to add method
        return ss.str();
    }
    
    // Convert between legacy and new interfaces
    std::shared_ptr<Experiment> getNewExperiment(const std::string& name) const {
        return runner_->getExperiment(name);
    }

private:
    std::shared_ptr<ExperimentRunner> runner_;
    
    // Helper method to get shared_ptr from LegacyExperiment
    std::shared_ptr<Experiment> getSharedPtr(std::shared_ptr<LegacyExperiment> legacyExp) {
        // In real implementation, would need to store mapping between legacy and new experiments
        return nullptr;
    }
};

} // namespace nlm