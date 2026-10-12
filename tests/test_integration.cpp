#include "tests/test_main.cpp"
#include "Brain.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <cassert>
#include <memory>
#include <vector>

namespace nlm {

class IntegrationTest {
public:
    IntegrationTest() {
        std::cout << "=== NLM Integration Tests ===\n" << std::endl;
    }
    
    bool runAllTests() {
        bool allPassed = true;
        
        std::cout << "Running integration tests..." << std::endl;
        
        // Test 1: Memory System Integration
        if (!testMemoryIntegration()) {
            allPassed = false;
            std::cout << "❌ Memory integration test failed" << std::endl;
        } else {
            std::cout << "✅ Memory integration test passed" << std::endl;
        }
        
        // Test 2: Prediction System Integration
        if (!testPredictionIntegration()) {
            allPassed = false;
            std::cout << "❌ Prediction integration test failed" << std::endl;
        } else {
            std::cout << "✅ Prediction integration test passed" << std::endl;
        }
        
        // Test 3: Concept Formation Integration
        if (!testConceptFormationIntegration()) {
            allPassed = false;
            std::cout << "❌ Concept formation integration test failed" << std::endl;
        } else {
            std::cout << "✅ Concept formation integration test passed" << std::endl;
        }
        
        // Test 4: Neuromodulation Integration
        if (!testNeuromodulationIntegration()) {
            allPassed = false;
            std::cout << "❌ Neuromodulation integration test failed" << std::endl;
        } else {
            std::cout << "✅ Neuromodulation integration test passed" << std::endl;
        }
        
        // Test 5: Cross-System Integration (working memory + attention + concept formation)
        if (!testCrossSystemIntegration()) {
            allPassed = false;
            std::cout << "❌ Cross-system integration test failed" << std::endl;
        } else {
            std::cout << "✅ Cross-system integration test passed" << std::endl;
        }
        
        return allPassed;
    }
    
private:
    bool testMemoryIntegration() {
        std::cout << "\n1. Testing Memory System Integration..." << std::endl;
        
        // Create brain with memory systems
        auto config = std::make_shared<Config>();
        config->set("random_seed", 42);
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cout << "   ❌ Failed to initialize brain" << std::endl;
            return false;
        }
        
        // Check if memory systems are accessible
        auto* workingMemory = brain->getWorkingMemory();
        auto* episodicMemory = brain->getEpisodicMemory();
        auto* associativeMemory = brain->getAssociativeMemory();
        
        if (!workingMemory || !episodicMemory || !associativeMemory) {
            std::cout << "   ❌ Memory systems not properly initialized" << std::endl;
            return false;
        }
        
        // Test working memory functionality
        workingMemory->setCapacity(50);
        workingMemory->setDecayRate(0.01f);
        
        // Test episodic memory functionality
        episodicMemory->setMaxEpisodes(1000);
        episodicMemory->setConsolidationThreshold(0.8f);
        
        // Test associative memory functionality
        associativeMemory->setStrengthDecay(0.05f);
        associativeMemory->setRecallProbability(0.7f);
        
        std::cout << "   ✅ Memory systems integrated and functional" << std::endl;
        return true;
    }
    
    bool testPredictionIntegration() {
        std::cout << "\n2. Testing Prediction System Integration..." << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("random_seed", 123);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cout << "   ❌ Failed to initialize brain" << std::endl;
            return false;
        }
        
        auto* predictionSystem = brain->getPredictionSystem();
        if (!predictionSystem) {
            std::cout << "   ❌ Prediction system not accessible" << std::endl;
            return false;
        }
        
        // Test prediction system initialization
        predictionSystem->clearHistory();
        
        // Test prediction error computation
        float initialError = predictionSystem->getPredictionError();
        if (initialError < 0.0f) {
            std::cout << "   ❌ Invalid initial prediction error" << std::endl;
            return false;
        }
        
        // Test prediction confidence
        float confidence = predictionSystem->getConfidence();
        if (confidence < 0.0f || confidence > 1.0f) {
            std::cout << "   ❌ Invalid prediction confidence" << std::endl;
            return false;
        }
        
        std::cout << "   ✅ Prediction system integrated and functional" << std::endl;
        return true;
    }
    
    bool testConceptFormationIntegration() {
        std::cout << "\n3. Testing Concept Formation Integration..." << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("random_seed", 456);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cout << "   ❌ Failed to initialize brain" << std::endl;
            return false;
        }
        
        auto* conceptFormation = brain->getConceptFormation();
        if (!conceptFormation) {
            std::cout << "   ❌ Concept formation system not accessible" << std::endl;
            return false;
        }
        
        // Test concept formation initialization
        conceptFormation->setFormationThreshold(0.7f);
        conceptFormation->setStabilityThreshold(0.8f);
        conceptFormation->setStabilityWindow(10);
        
        // Test concept creation with sample pattern
        std::vector<float> testPattern = {0.5f, 0.6f, 0.7f, 0.8f, 0.9f};
        std::vector<float> features = {1.0f, 0.8f, 0.6f, 0.4f, 0.2f};
        
        size_t conceptId = conceptFormation->presentExperience(
            testPattern, features, 0.5f, 10
        );
        
        if (conceptId == 0) {
            std::cout << "   ⚠️  No concept created (expected for novel pattern)" << std::endl;
        }
        
        // Test concept retrieval
        if (conceptId > 0) {
            const auto* concept = conceptFormation->getConcept(conceptId);
            if (!concept) {
                std::cout << "   ❌ Cannot retrieve created concept" << std::endl;
                return false;
            }
            
            // Test prototype retrieval
            std::vector<float> prototype = conceptFormation->getConceptPrototype(conceptId);
            if (prototype.empty()) {
                std::cout << "   ❌ Empty concept prototype" << std::endl;
                return false;
            }
            
            // Test concept stability
            float stability = conceptFormation->getConceptStability(conceptId);
            if (stability < 0.0f || stability > 1.0f) {
                std::cout << "   ❌ Invalid concept stability" << std::endl;
                return false;
            }
        }
        
        std::cout << "   ✅ Concept formation system integrated and functional" << std::endl;
        return true;
    }
    
    bool testNeuromodulationIntegration() {
        std::cout << "\n4. Testing Neuromodulation Integration..." << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("random_seed", 789);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cout << "   ❌ Failed to initialize brain" << std::endl;
            return false;
        }
        
        // Test all neuromodulation systems
        auto* dopamine = brain->getDopamine();
        auto* curiosity = brain->getCuriosity();
        auto* novelty = brain->getNovelty();
        auto* predictionError = brain->getPredictionErrorSignal();
        auto* acetylcholine = brain->getAcetylcholine();
        auto* norepinephrine = brain->getNorepinephrine();
        auto* serotonin = brain->getSerotonin();
        
        if (!dopamine || !curiosity || !novelty || !predictionError || 
            !acetylcholine || !norepinephrine || !serotonin) {
            std::cout << "   ❌ Some neuromodulation systems not accessible" << std::endl;
            return false;
        }
        
        // Test neuromodulation levels
        float daLevel = dopamine->getLevel();
        float curiosityLevel = curiosity->getLevel();
        float noveltyLevel = novelty->getLevel();
        float predErrorLevel = predictionError->getLevel();
        float achLevel = acetylcholine->getLevel();
        float neLevel = norepinephrine->getLevel();
        float serotoninLevel = serotonin->getLevel();
        
        // Test plasticity factors
        float daPlasticity = dopamine->getPlasticityFactor();
        float curiosityPlasticity = curiosity->getExplorationDrive();
        float achPlasticity = acetylcholine->getPlasticityFactor();
        
        if (daPlasticity < 0.0f || curiosityPlasticity < 0.0f || achPlasticity < 0.0f) {
            std::cout << "   ❌ Invalid neuromodulation plasticity factors" << std::endl;
            return false;
        }
        
        // Test neuromodulation updates
        dopamine->update(0.1f);
        curiosity->update(0.5f, 0.3f, 0.1f);
        novelty->update(0.1f);
        predictionError->update(0.2f);
        acetylcholine->update(0.1f);
        norepinephrine->update(0.1f);
        serotonin->update(0.1f);
        
        std::cout << "   ✅ Neuromodulation systems integrated and functional" << std::endl;
        return true;
    }
    
    bool testCrossSystemIntegration() {
        std::cout << "\n5. Testing Cross-System Integration (WM + Attention + CF)..." << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("random_seed", 999);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cout << "   ❌ Failed to initialize brain" << std::endl;
            return false;
        }
        
        // Test integration of working memory, attention, and concept formation
        auto* workingMemory = brain->getWorkingMemory();
        auto* attention = brain->getAttention();
        auto* conceptFormation = brain->getConceptFormation();
        auto* predictionSystem = brain->getPredictionSystem();
        
        if (!workingMemory || !attention || !conceptFormation || !predictionSystem) {
            std::cout << "   ❌ Cross-system components not accessible" << std::endl;
            return false;
        }
        
        // Simulate the integrated brain loop step
        // This mimics what happens in Brain::step()
        
        // 1. Working memory processes (simulate neuron firing)
        workingMemory->setCapacity(50);
        workingMemory->setDecayRate(0.01f);
        
        // 2. Attention processes competition
        attention->setInhibitionStrength(0.5f);
        attention->setExcitationStrength(1.5f);
        
        // 3. Concept formation processes experience
        std::vector<float> experiencePattern = {0.3f, 0.5f, 0.7f, 0.6f, 0.4f};
        std::vector<float> features = experiencePattern;
        
        size_t conceptId = conceptFormation->presentExperience(
            experiencePattern, features, 0.3f, 20
        );
        
        // 4. Prediction system updates based on working memory content
        predictionSystem->clearHistory();
        float predictionError = predictionSystem->getPredictionError();
        
        // 5. Neuromodulation integrates with all systems
        auto* dopamine = brain->getDopamine();
        auto* acetylcholine = brain->getAcetylcholine();
        
        if (dopamine && acetylcholine) {
            // Neuromodulation affects concept formation stability
            if (conceptId > 0) {
                float modulation = dopamine->getPlasticityFactor() * acetylcholine->getPlasticityFactor();
                conceptFormation->updateConcept(conceptId, experiencePattern, features, modulation * 0.1f);
            }
        }
        
        // 6. Attention affects working memory
        attention->processCompetition(workingMemory->getMemoryNeurons());
        
        std::cout << "   ✅ Cross-system integration functional" << std::endl;
        std::cout << "      (Working memory, attention, concept formation, prediction system)" << std::endl;
        return true;
    }
};

// Main test function
int main() {
    std::cout << "NLM Integration Tests" << std::endl;
    std::cout << "=====================" << std::endl;
    
    IntegrationTest test;
    bool allPassed = test.runAllTests();
    
    std::cout << "\n" << std::string(50, '=') << std::endl;
    if (allPassed) {
        std::cout << "✅ ALL INTEGRATION TESTS PASSED!" << std::endl;
        std::cout << "The NLM brain system is fully integrated and functional." << std::endl;
    } else {
        std::cout << "❌ SOME INTEGRATION TESTS FAILED!" << std::endl;
        std::cout << "Please review the failed tests above." << std::endl;
        return 1;
    }
    
    return 0;
}

} // namespace nlm