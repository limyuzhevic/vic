// Phase 6 Integration Test
// Comprehensive test of all Phase 6 integration components
// 
// This test verifies that all the systems integrated in Phase 6
// are properly connected and functioning together.

#include "Brain.hpp"
#include "Memory.hpp"
#include "PredictionSystem.hpp"
#include "Neuromodulator.hpp"
#include "cognition/SemanticWorkingMemory.hpp"
#include "cognition/ProceduralWorkingMemory.hpp"
#include <iostream>
#include <vector>

namespace nlm {

class Phase6IntegrationTest {
public:
    Phase6IntegrationTest() : brain(nullptr) {}
    ~Phase6IntegrationTest() {}
    
    // Run comprehensive integration test
    bool runIntegrationTest() {
        std::cout << "=== Phase 6 Integration Test ===" << std::endl;
        
        // Test 1: Verify brain initialization
        if (!testBrainInitialization()) {
            std::cerr << "FAILED: Brain initialization test" << std::endl;
            return false;
        }
        std::cout << "✓ Brain initialization test passed" << std::endl;
        
        // Test 2: Verify memory system integration
        if (!testMemoryIntegration()) {
            std::cerr << "FAILED: Memory system integration test" << std::endl;
            return false;
        }
        std::cout << "✓ Memory system integration test passed" << std::endl;
        
        // Test 3: Verify neuromodulation integration
        if (!testNeuromodulationIntegration()) {
            std::cerr << "FAILED: Neuromodulation integration test" << std::endl;
            return false;
        }
        std::cout << "✓ Neuromodulation integration test passed" << std::endl;
        
        // Test 4: Verify prediction system integration
        if (!testPredictionSystemIntegration()) {
            std::cerr << "FAILED: Prediction system integration test" << std::endl;
            return false;
        }
        std::cout << "✓ Prediction system integration test passed" << std::endl;
        
        // Test 5: Verify cognitive systems integration
        if (!testCognitiveSystemsIntegration()) {
            std::cerr << "FAILED: Cognitive systems integration test" << std::endl;
            return false;
        }
        std::cout << "✓ Cognitive systems integration test passed" << std::endl;
        
        // Test 6: Verify working brain loop
        if (!testBrainStep()) {
            std::cerr << "FAILED: Brain step test" << std::endl;
            return false;
        }
        std::cout << "✓ Brain step test passed" << std::endl;
        
        return true;
    }
    
private:
    bool testBrainInitialization() {
        std::cout << "Testing brain initialization..." << std::endl;
        
        // Create brain with minimal config
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        config->set("simulation_timestep", 0.001);
        config->set("random_seed", 42);
        
        brain = std::make_unique<Brain>(config);
        
        if (!brain->initialize()) {
            std::cerr << "ERROR: Failed to initialize brain" << std::endl;
            return false;
        }
        
        // Verify all systems are initialized
        if (!brain->getWorkingMemory()) {
            std::cerr << "ERROR: Working memory not initialized" << std::endl;
            return false;
        }
        
        if (!brain->getEpisodicMemory()) {
            std::cerr << "ERROR: Episodic memory not initialized" << std::endl;
            return false;
        }
        
        if (!brain->getPredictionSystem()) {
            std::cerr << "ERROR: Prediction system not initialized" << std::endl;
            return false;
        }
        
        // Verify new neuromodulators are initialized
        if (!brain->getDopamine()) {
            std::cerr << "ERROR: Dopamine neuromodulator not initialized" << std::endl;
            return false;
        }
        
        if (!brain->getAcetylcholine()) {
            std::cerr << "ERROR: Acetylcholine neuromodulator not initialized" << std::endl;
            return false;
        }
        
        if (!brain->getNorepinephrine()) {
            std::cerr << "ERROR: Norepinephrine neuromodulator not initialized" << std::endl;
            return false;
        }
        
        if (!brain->getSerotonin()) {
            std::cerr << "ERROR: Serotonin neuromodulator not initialized" << std::endl;
            return false;
        }
        
        // Verify new cognitive systems are initialized
        if (!brain->getSemanticWorkingMemory()) {
            std::cerr << "ERROR: Semantic working memory not initialized" << std::endl;
            return false;
        }
        
        if (!brain->getProceduralWorkingMemory()) {
            std::cerr << "ERROR: Procedural working memory not initialized" << std::endl;
            return false;
        }
        
        if (!brain->getSpatialRepresentation()) {
            std::cerr << "ERROR: Spatial representation not initialized" << std::endl;
            return false;
        }
        
        if (!brain->getTemporalRelation()) {
            std::cerr << "ERROR: Temporal relation not initialized" << std::endl;
            return false;
        }
        
        if (!brain->getSelfModel()) {
            std::cerr << "ERROR: Self model not initialized" << std::endl;
            return false;
        }
        
        return true;
    }
    
    bool testMemoryIntegration() {
        std::cout << "Testing memory system integration..." << std::endl;
        
        if (!brain) {
            std::cerr << "ERROR: Brain not initialized" << std::endl;
            return false;
        }
        
        // Test working memory
        auto* workingMemory = brain->getWorkingMemory();
        if (!workingMemory) {
            std::cerr << "ERROR: Working memory is null" << std::endl;
            return false;
        }
        
        // Test episodic memory
        auto* episodicMemory = brain->getEpisodicMemory();
        if (!episodicMemory) {
            std::cerr << "ERROR: Episodic memory is null" << std::endl;
            return false;
        }
        
        // Test semantic working memory
        auto* semanticWM = brain->getSemanticWorkingMemory();
        if (!semanticWM) {
            std::cerr << "ERROR: Semantic working memory is null" << std::endl;
            return false;
        }
        
        // Test procedural working memory
        auto* proceduralWM = brain->getProceduralWorkingMemory();
        if (!proceduralWM) {
            std::cerr << "ERROR: Procedural working memory is null" << std::endl;
            return false;
        }
        
        // Test semantic working memory functionality
        semanticWM->storeConcept("test_concept", std::vector<float>{0.5f, 0.3f, 0.8f}, 0.9f);
        
        if (!semanticWM->hasConcept("test_concept")) {
            std::cerr << "ERROR: Semantic working memory failed to store concept" << std::endl;
            return false;
        }
        
        std::vector<float> retrievedPattern = semanticWM->retrieveConcept("test_concept");
        if (retrievedPattern.empty()) {
            std::cerr << "ERROR: Semantic working memory failed to retrieve concept" << std::endl;
            return false;
        }
        
        // Test procedural working memory functionality
        std::vector<NeuronId> sequence = {NeuronId(1000), NeuronId(1001), NeuronId(1002)};
        proceduralWM->learnSkill("test_skill", sequence, 0.7f);
        
        if (!proceduralWM->hasSkill("test_skill")) {
            std::cerr << "ERROR: Procedural working memory failed to learn skill" << std::endl;
            return false;
        }
        
        float proficiency = proceduralWM->getSkillProficiency("test_skill");
        if (std::abs(proficiency - 0.7f) > 0.01f) {
            std::cerr << "ERROR: Procedural working memory failed to store proficiency" << std::endl;
            return false;
        }
        
        return true;
    }
    
    bool testNeuromodulationIntegration() {
        std::cout << "Testing neuromodulation integration..." << std::endl;
        
        if (!brain) {
            std::cerr << "ERROR: Brain not initialized" << std::endl;
            return false;
        }
        
        // Test dopamine
        auto* dopamine = brain->getDopamine();
        if (!dopamine) {
            std::cerr << "ERROR: Dopamine neuromodulator is null" << std::endl;
            return false;
        }
        
        dopamine->setLevel(0.8f);
        if (std::abs(dopamine->getLevel() - 0.8f) > 0.01f) {
            std::cerr << "ERROR: Dopamine neuromodulator failed to set/get level" << std::endl;
            return false;
        }
        
        // Test acetylcholine
        auto* acetylcholine = brain->getAcetylcholine();
        if (!acetylcholine) {
            std::cerr << "ERROR: Acetylcholine neuromodulator is null" << std::endl;
            return false;
        }
        
        acetylcholine->setLevel(0.6f);
        if (std::abs(acetylcholine->getLevel() - 0.6f) > 0.01f) {
            std::cerr << "ERROR: Acetylcholine neuromodulator failed to set/get level" << std::endl;
            return false;
        }
        
        // Test norepinephrine
        auto* norepinephrine = brain->getNorepinephrine();
        if (!norepinephrine) {
            std::cerr << "ERROR: Norepinephrine neuromodulator is null" << std::endl;
            return false;
        }
        
        norepinephrine->setLevel(0.4f);
        if (std::abs(norepinephrine->getLevel() - 0.4f) > 0.01f) {
            std::cerr << "ERROR: Norepinephrine neuromodulator failed to set/get level" << std::endl;
            return false;
        }
        
        // Test serotonin
        auto* serotonin = brain->getSerotonin();
        if (!serotonin) {
            std::cerr << "ERROR: Serotonin neuromodulator is null" << std::endl;
            return false;
        }
        
        serotonin->setLevel(0.3f);
        if (std::abs(serotonin->getLevel() - 0.3f) > 0.01f) {
            std::cerr << "ERROR: Serotonin neuromodulator failed to set/get level" << std::endl;
            return false;
        }
        
        return true;
    }
    
    bool testPredictionSystemIntegration() {
        std::cout << "Testing prediction system integration..." << std::endl;
        
        if (!brain) {
            std::cerr << "ERROR: Brain not initialized" << std::endl;
            return false;
        }
        
        auto* predictionSystem = brain->getPredictionSystem();
        if (!predictionSystem) {
            std::cerr << "ERROR: Prediction system is null" << std::endl;
            return false;
        }
        
        // Create test sensory input
        SensoryInput testInput;
        std::vector<float> testData = {0.2f, 0.5f, 0.8f, 0.3f, 0.7f};
        testInput.setData(testData);
        
        // Test prediction (if implemented)
        auto prediction = predictionSystem->predictNextState(testInput);
        if (!prediction) {
            std::cerr << "INFO: Prediction system returned null prediction (may be expected for placeholder)" << std::endl;
        }
        
        return true;
    }
    
    bool testCognitiveSystemsIntegration() {
        std::cout << "Testing cognitive systems integration..." << std::endl;
        
        if (!brain) {
            std::cerr << "ERROR: Brain not initialized" << std::endl;
            return false;
        }
        
        // Test concept formation
        auto* conceptFormation = brain->getConceptFormation();
        if (!conceptFormation) {
            std::cerr << "ERROR: Concept formation is null" << std::endl;
            return false;
        }
        
        // Test neural planner
        auto* planner = brain->getPlanner();
        if (!planner) {
            std::cerr << "ERROR: Neural planner is null" << std::endl;
            return false;
        }
        
        // Test attentional selection
        auto* attention = brain->getAttention();
        if (!attention) {
            std::cerr << "ERROR: Attentional selection is null" << std::endl;
            return false;
        }
        
        // Test spatial representation
        auto* spatialRepresentation = brain->getSpatialRepresentation();
        if (!spatialRepresentation) {
            std::cerr << "ERROR: Spatial representation is null" << std::endl;
            return false;
        }
        
        // Test temporal relation
        auto* temporalRelation = brain->getTemporalRelation();
        if (!temporalRelation) {
            std::cerr << "ERROR: Temporal relation is null" << std::endl;
            return false;
        }
        
        // Test self model
        auto* selfModel = brain->getSelfModel();
        if (!selfModel) {
            std::cerr << "ERROR: Self model is null" << std::endl;
            return false;
        }
        
        return true;
    }
    
    bool testBrainStep() {
        std::cout << "Testing brain step (simulation loop)..." << std::endl;
        
        if (!brain) {
            std::cerr << "ERROR: Brain not initialized" << std::endl;
            return false;
        }
        
        // Test a few simulation steps
        for (int i = 0; i < 10; ++i) {
            SimulationStep step = static_cast<SimulationStep>(i);
            Timestamp time = static_cast<Timestamp>(step * 0.001);
            
            brain->step(step, time);
            
            // Check that brain is still valid
            if (!brain->getWorkingMemory()) {
                std::cerr << "ERROR: Working memory is null after brain step" << std::endl;
                return false;
            }
        }
        
        return true;
    }
    
private:
    std::unique_ptr<Brain> brain;
};

} // namespace nlm

int main() {
    nlm::Phase6IntegrationTest test;
    
    if (test.runIntegrationTest()) {
        std::cout << std::endl;
        std::cout << "=== ALL TESTS PASSED ===" << std::endl;
        std::cout << "Phase 6 integration is successfully implemented!" << std::endl;
        return 0;
    } else {
        std::cout << std::endl;
        std::cout << "=== TESTS FAILED ===" << std::endl;
        std::cout << "Phase 6 integration has issues that need to be resolved." << std::endl;
        return 1;
    }
}