#pragma once

#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"

namespace nlm {

class BrainCognitiveManager {
public:
    // Create cognitive manager with configuration
    explicit BrainCognitiveManager(std::shared_ptr<Config> config);
    
    ~BrainCognitiveManager();
    
    // Initialize cognitive manager
    bool initialize(BrainNeuralCore* neuralCore);
    
    // Update cognitive systems
    void update(TimestepDuration timestep);
    
    // Cognitive functions
    void processCompetition(const std::vector<NeuronId>& competitors);
    void processCurrentActivity(const std::vector<NeuronId>& activeNeurons);
    void formConcepts(const std::vector<NeuronId>& activeNeurons);
    
    // Get access to cognitive components
    NeuralPlanner* getPlanner();
    const NeuralPlanner* getPlanner() const;
    
    ConceptFormation* getConceptFormation();
    const ConceptFormation* getConceptFormation() const;
    
    AttentionalSelection* getAttention();
    const AttentionalSelection* getAttention() const;
    
    // Configuration
    void setPlanningDepth(size_t depth);
    void setInhibitionStrength(float strength);
    void setExcitationStrength(float strength);
    
    // Reset cognitive state
    void reset();
    
    // Get current configuration
    std::shared_ptr<const Config> getConfig() const;
    
    // Log cognitive manager status
    void logStatus() const;

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm