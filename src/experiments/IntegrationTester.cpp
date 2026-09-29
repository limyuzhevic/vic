#include "IntegrationTester.hpp"
#include "../core/Logger/Logger.hpp"
#include <chrono>

namespace nlm {

bool IntegrationTester::testAllSystems(std::shared_ptr<Brain> brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot test integration: invalid brain");
        return false;
    }
    
    NLM_LOG_INFO("=== Testing All Brain Systems ===");
    
    // Reset test results
    testResults_ = IntegrationTestResults();
    
    // Test each system
    testResults_.memoryWorkingMemoryIntegrated = testMemoryIntegration(brain);
    testResults_.memoryEpisodicMemoryIntegrated = testMemoryIntegration(brain); // Same test for both memory types
    testResults_.neuromodulationIntegrated = testNeuromodulationIntegration(brain);
    testResults_.predictionIntegrated = testPredictionIntegration(brain);
    testResults_.developmentIntegrated = testDevelopmentIntegration(brain);
    testResults_.basicConnectivity = testBasicConnectivity(brain);
    
    // Log results
    NLM_LOG_INFO("=== Integration Test Results ===");
    NLM_LOG_INFO("Working Memory: " + std::string(testResults_.memoryWorkingMemoryIntegrated ? "PASS" : "FAIL"));
    NLM_LOG_INFO("Episodic Memory: " + std::string(testResults_.memoryEpisodicMemoryIntegrated ? "PASS" : "FAIL"));
    NLM_LOG_INFO("Neuromodulation: " + std::string(testResults_.neuromodulationIntegrated ? "PASS" : "FAIL"));
    NLM_LOG_INFO("Prediction: " + std::string(testResults_.predictionIntegrated ? "PASS" : "FAIL"));
    NLM_LOG_INFO("Development: " + std::string(testResults_.developmentIntegrated ? "PASS" : "FAIL"));
    NLM_LOG_INFO("Basic Connectivity: " + std::string(testResults_.basicConnectivity ? "PASS" : "FAIL"));
    
    return testResults_.isAllSystemsIntegrated();
}

bool IntegrationTester::testMemoryIntegration(std::shared_ptr<Brain> brain) {
    if (!brain) return false;
    
    try {
        // Check memory systems
        bool hasWorkingMemory = brain->getWorkingMemory() != nullptr;
        bool hasEpisodicMemory = brain->getEpisodicMemory() != nullptr;
        
        if (hasWorkingMemory) {
            NLM_LOG_INFO("[PASS] Memory system accessible");
        } else {
            NLM_LOG_ERROR("[FAIL] Memory system not accessible");
            testResults_.errors.push_back("Memory system not accessible");
        }
        
        // Run some steps to test memory functionality
        for (int i = 0; i < ExperimentConstants::MEMORY_TEST_STEPS; ++i) {
            brain->step(i, i * 0.001);
        }
        
        return hasWorkingMemory && hasEpisodicMemory;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Memory integration test failed: " + std::string(e.what()));
        testResults_.errors.push_back("Memory integration test exception: " + std::string(e.what()));
        return false;
    }
}

bool IntegrationTester::testNeuromodulationIntegration(std::shared_ptr<Brain> brain) {
    if (!brain) return false;
    
    try {
        // Check neuromodulation systems
        bool hasDopamine = brain->getDopamine() != nullptr;
        bool hasCuriosity = brain->getCuriosity() != nullptr;
        bool hasNovelty = brain->getNovelty() != nullptr;
        
        if (hasDopamine) {
            NLM_LOG_INFO("[PASS] Dopamine system accessible");
        } else {
            NLM_LOG_ERROR("[FAIL] Dopamine system not accessible");
            testResults_.errors.push_back("Dopamine system not accessible");
        }
        
        if (hasCuriosity) {
            NLM_LOG_INFO("[PASS] Curiosity system accessible");
        } else {
            NLM_LOG_ERROR("[FAIL] Curiosity system not accessible");
            testResults_.errors.push_back("Curiosity system not accessible");
        }
        
        if (hasNovelty) {
            NLM_LOG_INFO("[PASS] Novelty system accessible");
        } else {
            NLM_LOG_ERROR("[FAIL] Novelty system not accessible");
            testResults_.errors.push_back("Novelty system not accessible");
        }
        
        // Inject some activity to test neuromodulation
        for (int i = 0; i < ExperimentConstants::NEUROMODULATION_TEST_STEPS; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 5.0f);
            brain->step(i, i * 0.001);
        }
        
        return hasDopamine && hasCuriosity && hasNovelty;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Neuromodulation integration test failed: " + std::string(e.what()));
        testResults_.errors.push_back("Neuromodulation integration test exception: " + std::string(e.what()));
        return false;
    }
}

bool IntegrationTester::testPredictionIntegration(std::shared_ptr<Brain> brain) {
    if (!brain) return false;
    
    try {
        // Check prediction system
        bool hasPredictionSystem = brain->getPredictionSystem() != nullptr;
        
        if (hasPredictionSystem) {
            NLM_LOG_INFO("[PASS] Prediction system accessible");
        } else {
            NLM_LOG_ERROR("[FAIL] Prediction system not accessible");
            testResults_.errors.push_back("Prediction system not accessible");
        }
        
        return hasPredictionSystem;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Prediction integration test failed: " + std::string(e.what()));
        testResults_.errors.push_back("Prediction integration test exception: " + std::string(e.what()));
        return false;
    }
}

bool IntegrationTester::testDevelopmentIntegration(std::shared_ptr<Brain> brain) {
    if (!brain) return false;
    
    try {
        // Check development system
        bool hasDevelopmentSystem = brain->getDevelopmentSystem() != nullptr;
        
        if (hasDevelopmentSystem) {
            NLM_LOG_INFO("[PASS] Development system accessible");
        } else {
            NLM_LOG_ERROR("[FAIL] Development system not accessible");
            testResults_.errors.push_back("Development system not accessible");
        }
        
        return hasDevelopmentSystem;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Development integration test failed: " + std::string(e.what()));
        testResults_.errors.push_back("Development integration test exception: " + std::string(e.what()));
        return false;
    }
}

bool IntegrationTester::testBasicConnectivity(std::shared_ptr<Brain> brain) {
    if (!brain) return false;
    
    try {
        // Run basic connectivity test
        for (int i = 0; i < ExperimentConstants::INTEGRATION_TEST_STEPS; ++i) {
            brain->step(i, i * 0.001);
        }
        
        // Check if brain is responsive
        bool hasAverageFiringRate = !std::isnan(brain->getAverageFiringRate());
        bool hasTotalSpikeCount = brain->getTotalSpikeCount() >= 0;
        
        if (hasAverageFiringRate && hasTotalSpikeCount) {
            NLM_LOG_INFO("[PASS] Basic connectivity test passed");
        } else {
            NLM_LOG_ERROR("[FAIL] Basic connectivity test failed");
            testResults_.errors.push_back("Brain connectivity check failed");
        }
        
        return hasAverageFiringRate && hasTotalSpikeCount;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Basic connectivity test failed: " + std::string(e.what()));
        testResults_.errors.push_back("Basic connectivity test exception: " + std::string(e.what()));
        return false;
    }
}

} // namespace nlm
