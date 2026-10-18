#pragma once

// brain/cognition/BrainCognition.hpp
// Cognition system management for the brain
// Extracts cognition systems from monolithic Brain.cpp

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include <memory>
#include <vector>
#include <string>
#include <map>

namespace nlm {

// Forward declarations
class Config;
class NeuralWorkingMemory;
class Curiosity;
class Dopamine;

// Brain cognition system manager
class BrainCognition {
public:
    // Constructor
    explicit BrainCognition(std::shared_ptr<Config> config);
    
    // Destructor
    ~BrainCognition();
    
    // Disable copying
    BrainCognition(const BrainCognition&) = delete;
    BrainCognition& operator=(const BrainCognition&) = delete;
    
    // Configuration and initialization
    bool initialize(const std::vector<Neuron*>& sensoryNeurons,
                   const std::vector<Neuron*>& motorNeurons);
    void reset();
    
    // Cognition system access
    NeuralPlanner* getPlanner() { return planner_.get(); }
    ConceptFormation* getConceptFormation() { return conceptFormation_.get(); }
    AttentionalSelection* getAttention() { return attention_.get(); }
    
    // Memory system access (for integration)
    NeuralWorkingMemory* getWorkingMemory() { return workingMemory_.get(); }
    void setWorkingMemory(std::unique_ptr<NeuralWorkingMemory> memory) {
        workingMemory_ = std::move(memory);
    }
    
    // Neuromodulation system access
    Curiosity* getCuriosity() { return curiosity_.get(); }
    Dopamine* getDopamine() { return dopamine_.get(); }
    
    // Configuration
    std::shared_ptr<const Config> getConfig() const { return config_; }
    
    // Cognition operations
    void updatePlanning(double timestep);
    void updateConceptFormation(double timestep);
    void updateAttention(double timestep);
    void processSensoryInput(const std::vector<float>& sensoryData);
    void integrateWithWorkingMemory(const std::vector<Neuron*>& activeNeurons);
    
    // Action selection
    size_t selectAction() const;
    std::string getSelectedActionDescription() const;
    
    // Learning and development
    void learnFromExperience(const std::vector<float>& experience);
    void updateConcepts(const std::vector<float>& patterns);
    void adjustAttentionWeights(float novelty, float predictionError);
    
    // Statistics
    size_t getActiveConcepts() const { return activeConcepts_; }
    size_t getCurrentPlanDepth() const { return currentPlanDepth_; }
    float getAttentionFocus() const { return attentionFocus_; }
    float getPlanningConfidence() const { return planningConfidence_; }
    
    // Configuration utilities
    static bool validateCognitionConfig(const std::shared_ptr<const Config>& config);
    static void applyDefaultCognitionSettings(std::shared_ptr<Config> config);
    
    // Stimulus-response mapping
    struct StimulusResponse {
        std::vector<float> stimulus;
        size_t actionIndex;
        float confidence;
        size_t responseCount;
    };
    
    // Record experience for learning
    void recordExperience(const std::vector<float>& stimulus, size_t action, float outcome);
    
    // Get learned stimulus-response mappings
    std::vector<StimulusResponse> getStimulusResponses() const { return stimulusResponses_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Core components
    std::shared_ptr<Config> config_;
    
    // Cognition systems
    std::unique_ptr<NeuralPlanner> planner_;
    std::unique_ptr<ConceptFormation> conceptFormation_;
    std::unique_ptr<AttentionalSelection> attention_;
    
    // Integration systems
    std::unique_ptr<NeuralWorkingMemory> workingMemory_;
    std::unique_ptr<Curiosity> curiosity_;
    std::unique_ptr<Dopamine> dopamine_;
    
    // Cognition state
    std::vector<size_t> actionHistory_;
    std::map<size_t, std::vector<StimulusResponse>> stimulusResponseMap_;
    std::vector<StimulusResponse> stimulusResponses_;
    
    // Statistics
    size_t activeConcepts_;
    size_t currentPlanDepth_;
    float attentionFocus_;
    float planningConfidence_;
    
    // Action space
    std::vector<std::string> actionDescriptions_;
};

} // namespace nlm
