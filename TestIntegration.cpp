#include "src/brain/Brain.hpp"
#include "src/memory/SemanticMemory.hpp"
#include "src/memory/ProceduralMemory.hpp"
#include "src/cognition/SelfModel.hpp"
#include "src/cognition/SocialLearning.hpp"
#include "src/cognition/SpatialRepresentation.hpp"
#include "src/cognition/TemporalRelation.hpp"

int main() {
    // Test that all the new getter methods are properly declared
    nlm::Brain brain(nullptr);  // Create with null config for testing
    
    // Test SemanticMemory getter
    nlm::SemanticMemory* semanticMem = brain.getSemanticMemory();
    
    // Test ProceduralMemory getter
    nlm::ProceduralMemory* proceduralMem = brain.getProceduralMemory();
    
    // Test cognitive system getters
    nlm::SelfModel* selfModel = brain.getSelfModel();
    nlm::SocialLearning* socialLearning = brain.getSocialLearning();
    nlm::SpatialRepresentation* spatialRepresentation = brain.getSpatialRepresentation();
    nlm::TemporalRelation* temporalRelation = brain.getTemporalRelation();
    
    // Test other getters for compatibility
    nlm::NeuralWorkingMemory* workingMem = brain.getWorkingMemory();
    nlm::NeuralEpisodicMemory* episodicMem = brain.getEpisodicMemory();
    nlm::NeuralAssociativeMemory* associativeMem = brain.getAssociativeMemory();
    
    nlm::PredictionSystem* prediction = brain.getPredictionSystem();
    nlm::NeuralPlanner* planner = brain.getPlanner();
    nlm::ConceptFormation* conceptFormation = brain.getConceptFormation();
    nlm::AttentionalSelection* attention = brain.getAttention();
    nlm::DevelopmentSystem* development = brain.getDevelopmentSystem();
    
    // Test that Phase6IntegratedExperiment has the new fields
    nlm::Phase6IntegrationResult result;
    
    // Check that the new integration fields exist
    bool memorySemanticMemoryIntegrated = result.memorySemanticMemoryIntegrated;
    bool memoryProceduralMemoryIntegrated = result.memoryProceduralMemoryIntegrated;
    bool cognitionSelfModelIntegrated = result.cognitionSelfModelIntegrated;
    bool cognitionSocialLearningIntegrated = result.cognitionSocialLearningIntegrated;
    bool cognitionSpatialRepresentationIntegrated = result.cognitionSpatialRepresentationIntegrated;
    bool cognitionTemporalRelationIntegrated = result.cognitionTemporalRelationIntegrated;
    
    return 0;
}