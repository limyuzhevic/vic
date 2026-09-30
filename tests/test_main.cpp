// NLM Integration Test Main
// Comprehensive integration test runner for all fixed systems

#include "integration/test_memory_integration.cpp"
#include "integration/test_cognition_integration.cpp"
#include "integration/test_prediction_integration.cpp"
#include "integration/test_neuromodulation_integration.cpp"
#include "integration/test_development_integration.cpp"
#include "integration/test_agentbrain_integration.cpp"

int main() {
    std::cout << "=== NLM Integration Tests ===" << std::endl;
    std::cout << std::endl;
    
    bool allTestsPassed = true;
    
    // Run all integration test categories
    allTestsPassed &= test_memory_integration::runMemoryIntegrationTests();
    
    allTestsPassed &= test_cognition_integration::runCognitionIntegrationTests();
    
    allTestsPassed &= test_prediction_integration::runPredictionIntegrationTests();
    
    allTestsPassed &= test_neuromodulation_integration::runNeuromodulationIntegrationTests();
    
    allTestsPassed &= test_development_integration::runDevelopmentIntegrationTests();
    
    allTestsPassed &= test_agentbrain_integration::runAgentBrainIntegrationTests();
    
    std::cout << std::endl;
    
    if (allTestsPassed) {
        std::cout << "All integration tests passed! (100% pass rate)" << std::endl;
        std::cout << std::endl;
        std::cout << "=== Integration Test Summary ===" << std::endl;
        std::cout << "✓ Memory Integration Tests" << std::endl;
        std::cout << "  - WorkingMemory: OK" << std::endl;
        std::cout << "  - EpisodicMemory: OK" << std::endl;
        std::cout << "  - AssociativeMemory: OK" << std::endl;
        std::cout << "✓ Cognition Integration Tests" << std::endl;
        std::cout << "  - NeuralPlanner: OK" << std::endl;
        std::cout << "  - ConceptFormation: OK" << std::endl;
        std::cout << "  - AttentionalSelection: OK" << std::endl;
        std::cout << "✓ Prediction Integration Tests" << std::endl;
        std::cout << "  - NeuralPrediction: OK" << std::endl;
        std::cout << "  - PredictionError: OK" << std::endl;
        std::cout << "  - ActionConsequencePredictor: OK" << std::endl;
        std::cout << "✓ Neuromodulation Integration Tests" << std::endl;
        std::cout << "  - Dopamine: OK" << std::endl;
        std::cout << "  - Curiosity: OK" << std::endl;
        std::cout << "  - Novelty: OK" << std::endl;
        std::cout << "  - PredictionError: OK" << std::endl;
        std::cout << "✓ Development Integration Tests" << std::endl;
        std::cout << "  - DevelopmentalStage: OK" << std::endl;
        std::cout << "  - DevelopmentSystem: OK" << std::endl;
        std::cout << "  - StructuralPlasticity: OK" << std::endl;
        std::cout << "✓ AgentBrain Integration Tests" << std::endl;
        std::cout << "  - MemoryIntegration: OK" << std::endl;
        std::cout << "  - PredictionIntegration: OK" << std::endl;
        std::cout << "  - ActionSelection: OK" << std::endl;
        std::cout << "  - Neuromodulation: OK" << std::endl;
        return 0;
    } else {
        std::cout << "Some integration tests failed!" << std::endl;
        return 1;
    }
}
