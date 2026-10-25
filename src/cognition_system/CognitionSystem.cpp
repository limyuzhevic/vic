#include "CognitionSystem.h"
#include "core/Logger/Logger.hpp"

namespace nlm {

class CognitionSystemImpl : public CognitionSystem {
private:
    std::unique_ptr<NeuralPlanner> planner_;
    std::unique_ptr<ConceptFormation> conceptFormation_;
    std::unique_ptr<AttentionalSelection> attention_;
    
public:
    CognitionSystemImpl() {
        NLM_LOG_INFO("CognitionSystem initialized");
    }
    
    void initialize(Brain* brain) override {
        planner_ = std::make_unique<NeuralPlanner>();
        conceptFormation_ = std::make_unique<ConceptFormation>();
        attention_ = std::make_unique<AttentionalSelection>();
        
        if (planner_) {
            planner_->initialize(brain);
            planner_->setPlanningDepth(5);
        }
        
        if (conceptFormation_) {
            conceptFormation_->initialize(brain);
        }
        
        if (attention_) {
            attention_->initialize(brain);
            attention_->setInhibitionStrength(0.5f);
            attention_->setExcitationStrength(1.5f);
        }
        
        NLM_LOG_INFO("Cognition system components initialized");
    }
    
    void update(TimestepDuration timestep) override {
        NLM_LOG_DEBUG("Updating cognition systems");
        
        // Update attention system
        if (attention_) {
            attention_->update(timestep);
            applyAttentionToWorkingMemory();
        }
        
        // Form concepts from neural activity
        formConceptsFromActivity();
        
        // Plan actions
        planActions();
        
        NLM_LOG_DEBUG("Cognition system update complete");
    }
    
    // Cognition system accessors
    NeuralPlanner* getPlanner() override {
        return planner_.get();
    }
    
    ConceptFormation* getConceptFormation() override {
        return conceptFormation_.get();
    }
    
    AttentionalSelection* getAttention() override {
        return attention_.get();
    }
    
    void applyAttentionToWorkingMemory() override {
        if (attention_ && planner_) {
            // Get working memory and apply attentional selection
            auto* workingMemory = planner_->getBrain()->getWorkingMemory();
            if (workingMemory) {
                auto memoryNeurons = workingMemory->getMemoryNeurons();
                attention_->processCompetition(memoryNeurons);
                NLM_LOG_DEBUG("Applied attentional selection to " + 
                             std::to_string(memoryNeurons.size()) + " working memory traces");
            }
        }
    }
    
    void formConceptsFromActivity() override {
        if (conceptFormation_) {
            // In a real implementation, this would process neural activity
            // to form abstract concepts
            NLM_LOG_DEBUG("Concept formation would process neural activity here");
        }
    }
    
    void planActions() override {
        if (planner_) {
            // Neural planning would generate action sequences
            // In a real implementation, this would evaluate possible actions
            NLM_LOG_DEBUG("Neural planning would generate action sequences here");
        }
    }
};

} // namespace nlm