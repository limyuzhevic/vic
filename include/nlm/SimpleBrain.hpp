// Simplified NLM API for easier usage
// This provides a high-level interface for common brain operations

#pragma once

#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../environment/Environment.hpp"
#include "../agent/AgentBrain.hpp"
#include "../world/SimpleWorld.hpp"
#include <string>
#include <memory>

namespace nlm {

// SimpleBrain - high-level interface for basic brain operations
class SimpleBrain {
public:
    SimpleBrain();
    explicit SimpleBrain(std::shared_ptr<Config> config);
    
    // Initialize brain with default or custom config
    bool initialize(const std::string& configFile = "");
    
    // Main simulation step
    void step();
    
    // Process sensory input
    void processSensoryInput(const SensoryInput& input);
    
    // Get action output
    ActionType getAction();
    
    // Get brain statistics
    std::string getStatus() const;
    
    // Save/load brain state
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // Reset brain
    void reset();
    
    // Advanced users: access to internal systems
    Brain* getInternalBrain() { return brain_.get(); }
    const Brain* getInternalBrain() const { return brain_.get(); }
    
    // Get development stage
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Get neuromodulation levels
    float getDopamineLevel() const;
    float getCuriosityLevel() const;
    
private:
    std::shared_ptr<Config> config_;
    std::unique_ptr<Brain> brain_;
    SimulationStep currentStep_;
};

// QuickStart - convenience function for rapid prototyping
inline SimpleBrain createQuickBrain() {
    return SimpleBrain();
}

// Diagnostics - scientific metrics and validation
namespace Diagnostics {
    
    // Get comprehensive brain metrics
    struct BrainMetrics {
        size_t neuronCount;
        size_t synapseCount;
        size_t firingRate;
        float excitationInhibitionRatio;
        size_t activeNeurons;
        float workingMemoryActivity;
        size_t episodicMemorySize;
        float predictionError;
        float predictionConfidence;
        float dopamineLevel;
        float curiosityLevel;
        DevelopmentalStage developmentalStage;
        bool isResting;
        
        // Scientific validation metrics
        float learningProgress;
        float memoryRetention;
        float noveltyProcessing;
        float predictionAccuracy;
    };
    
    // Collect all metrics from a brain
    BrainMetrics collectMetrics(Brain* brain);
    
    // Validate brain health
    bool validateBrain(Brain* brain);
    
    // Get learning curve data
    std::vector<float> getLearningCurve(Brain* brain, size_t steps);
    
    // Get memory retention curve
    std::vector<float> getMemoryRetentionCurve(Brain* brain, size_t timeSteps);
    
    // Generate scientific report
    std::string generateScientificReport(Brain* brain);
}

// Advanced users - direct system access
namespace Advanced {
    
    // Direct system access for power users
    class SystemAccess {
    public:
        explicit SystemAccess(std::unique_ptr<Brain> brain) 
            : brain_(std::move(brain)) {}
        
        // Access to all internal systems
        SpikeSystem* getSpikeSystem() { return brain_->getSpikeSystem(); }
        STDP* getSTDP() { return brain_->getSTDP(); }
        Hebbian* getHebbian() { return brain_->getHebbian(); }
        StructuralPlasticity* getStructuralPlasticity() { return brain_->getStructuralPlasticity(); }
        
        NeuralWorkingMemory* getWorkingMemory() { return brain_->getWorkingMemory(); }
        NeuralEpisodicMemory* getEpisodicMemory() { return brain_->getEpisodicMemory(); }
        NeuralAssociativeMemory* getAssociativeMemory() { return brain_->getAssociativeMemory(); }
        
        PredictionSystem* getPredictionSystem() { return brain_->getPredictionSystem(); }
        NeuralPlanner* getPlanner() { return brain_->getPlanner(); }
        ConceptFormation* getConceptFormation() { return brain_->getConceptFormation(); }
        AttentionalSelection* getAttention() { return brain_->getAttention(); }
        
        DevelopmentSystem* getDevelopmentSystem() { return brain_->getDevelopmentSystem(); }
        
        Dopamine* getDopamine() { return brain_->getDopamine(); }
        Curiosity* getCuriosity() { return brain_->getCuriosity(); }
        Novelty* getNovelty() { return brain_->getNovelty(); }
        PredictionError* getPredictionError() { return brain_->getPredictionErrorSignal(); }
        
        // Direct neural access
        std::vector<Neuron*> getAllNeurons() {
            std::vector<Neuron*> allNeurons;
            for (const auto& region : brain_->getRegions()) {
                for (const auto& pop : region->getPopulations()) {
                    for (Neuron* n : pop->getNeurons()) {
                        allNeurons.push_back(n);
                    }
                }
            }
            return allNeurons;
        }
        
        // Configuration access
        std::shared_ptr<Config> getConfig() { return brain_->getConfig(); }
        
        // Performance tuning
        void enableAllPlasticity(bool hebbian, bool stdp, bool reward) {
            for (const auto& region : brain_->getRegions()) {
                for (const auto& pop : region->getPopulations()) {
                    for (Neuron* n : pop->getNeurons()) {
                        n->enablePlasticity(hebbian, stdp, reward);
                    }
                }
            }
        }
        
        // Custom neuromodulation signals
        void applyCustomNeuromodulation(float value) {
            // Apply custom neuromodulation to all plasticity systems
            auto* stdp = brain_->getSTDP();
            auto* hebbian = brain_->getHebbian();
            
            if (stdp) {
                stdp->setLTPWeight(0.01f * value);
                stdp->setLTDWeight(0.012f * value);
            }
            
            if (hebbian) {
                hebbian->setLearningRate(0.1f * value);
            }
        }
        
    private:
        std::unique_ptr<Brain> brain_;
    };
}

} // namespace nlm