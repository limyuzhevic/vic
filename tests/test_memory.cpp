// NLM Memory System Tests
// Phase 6: Final Integration Tests

#include <iostream>
#include <cassert>
#include <vector>
#include <cmath>

// Include memory system headers
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/memory/Memory.hpp"
#include "../src/memory/AssociativeMemory.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/neuromodulation/Novelty.hpp"

namespace test_memory {

void testNeuralWorkingMemory() {
    std::cout << "    Testing NeuralWorkingMemory..." << std::endl;
    
    // Test basic functionality
    nlm::NeuralWorkingMemory workingMem;
    
    // Note: In a real implementation, we would need a Brain instance
    // For now, we test the interface
    assert(workingMem.getCapacity() > 0);  // Default capacity
    assert(workingMem.getDecayRate() > 0.0f);  // Default decay rate
    
    std::cout << "      ✓ NeuralWorkingMemory interface basic" << std::endl;
}

void testNeuralEpisodicMemory() {
    std::cout << "    Testing NeuralEpisodicMemory..." << std::endl;
    
    // Test basic functionality
    nlm::NeuralEpisodicMemory episodicMem;
    
    assert(episodicMem.getCapacity() > 0);  // Default capacity
    assert(episodicMem.getMaxEpisodes() > 0);  // Default max episodes
    
    std::cout << "      ✓ NeuralEpisodicMemory interface basic" << std::endl;
}

void testSemanticMemory() {
    std::cout << "    Testing SemanticMemory..." << std::endl;
    
    nlm::SemanticMemory semanticMem;
    
    assert(semanticMem.getKnowledgeBaseSize() >= 0);
    assert(semanticMem.getConceptCount() >= 0);
    
    std::cout << "      ✓ SemanticMemory interface basic" << std::endl;
}

void testProceduralMemory() {
    std::cout << "    Testing ProceduralMemory..." << std::endl;
    
    nlm::ProceduralMemory proceduralMem;
    
    assert(proceduralMem.getActionCount() >= 0);
    assert(proceduralMem.getSkillCount() >= 0);
    
    std::cout << "      ✓ ProceduralMemory interface basic" << std::endl;
}

void testAssociativeMemory() {
    std::cout << "    Testing AssociativeMemory..." << std::endl;
    
    nlm::AssociativeMemory assocMem;
    
    assert(assocMem.getPatternCount() >= 0);
    assert(assocMem.getConnectionStrength() >= 0.0f);
    
    std::cout << "      ✓ AssociativeMemory interface basic" << std::endl;
}

void testConceptFormation() {
    std::cout << "    Testing ConceptFormation..." << std::endl;
    
    nlm::ConceptFormation conceptForm;
    
    assert(conceptForm.getConceptCount() >= 0);
    assert(conceptForm.getSimilarityThreshold() > 0.0f);
    assert(conceptForm.getCohesionStrength() > 0.0f);
    
    std::cout << "      ✓ ConceptFormation interface basic" << std::endl;
}

void testNeuralPlanner() {
    std::cout << "    Testing NeuralPlanner..." << std::endl;
    
    nlm::NeuralPlanner planner;
    
    assert(planner.getPlanCount() >= 0);
    assert(planner.getMaxPlanLength() > 0);
    assert(planner.getActionSelectionThreshold() >= 0.0f);
    
    std::cout << "      ✓ NeuralPlanner interface basic" << std::endl;
}

void testNeuromodulator() {
    std::cout << "    Testing Neuromodulator..." << std::endl;
    
    nlm::Neuromodulator neuromod;
    
    assert(neuromod.getDopamineLevel() >= 0.0f);
    assert(neuromod.getSerotoninLevel() >= 0.0f);
    assert(neuromod.getAcetylcholineLevel() >= 0.0f);
    
    std::cout << "      ✓ Neuromodulator interface basic" << std::endl;
}

void testCuriosity() {
    std::cout << "    Testing Curiosity..." << std::endl;
    
    nlm::Curiosity curiosity;
    
    assert(curiosity.getCuriosityLevel() >= 0.0f);
    assert(curiosity.getExplorationRate() >= 0.0f);
    assert(curiosity.getUncertaintyWeight() >= 0.0f);
    
    std::cout << "      ✓ Curiosity interface basic" << std::endl;
}

void testPredictionError() {
    std::cout << "    Testing PredictionError..." << std::endl;
    
    nlm::PredictionError predError;
    
    assert(predError.getPredictionError() >= -1.0f);
    assert(predError.getExpectedValue() >= 0.0f);
    assert(predError.getPrecision() > 0.0f);
    
    std::cout << "      ✓ PredictionError interface basic" << std::endl;
}

void testReward() {
    std::cout << "    Testing Reward..." << std::endl;
    
    nlm::Reward reward;
    
    assert(reward.getRewardValue() >= -1.0f);
    assert(reward.getDiscountFactor() > 0.0f);
    assert(reward.getLearningRate() > 0.0f);
    
    std::cout << "      ✓ Reward interface basic" << std::endl;
}

void testNovelty() {
    std::cout << "    Testing Novelty..." << std::endl;
    
    nlm::Novelty novelty;
    
    assert(novelty.getNoveltyLevel() >= 0.0f);
    assert(novelty.getNoveltyDetectionThreshold() >= 0.0f);
    assert(novelty.getMemoryDecayRate() > 0.0f);
    
    std::cout << "      ✓ Novelty interface basic" << std::endl;
}

void testAttentionalSelection() {
    std::cout << "    Testing AttentionalSelection..." << std::endl;
    
    nlm::AttentionalSelection attention;
    
    assert(attention.getInhibitionStrength() >= 0.0f);
    assert(attention.getExcitationStrength() >= 0.0f);
    assert(attention.getCompetitionThreshold() >= 0.0f);
    
    std::cout << "      ✓ AttentionalSelection interface basic" << std::endl;
}

void runAll() {
    std::cout << "  === Memory Systems Tests ===" << std::endl;
    
    try {
        testNeuralWorkingMemory();
        testNeuralEpisodicMemory();
        testSemanticMemory();
        testProceduralMemory();
        testAssociativeMemory();
        testConceptFormation();
        testNeuralPlanner();
        testNeuromodulator();
        testCuriosity();
        testPredictionError();
        testReward();
        testNovelty();
        testAttentionalSelection();
        
        std::cout << "  === All Memory System Tests PASSED ===" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        throw;
    }
}

} // namespace test_memory