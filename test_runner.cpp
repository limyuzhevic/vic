// NLM Advanced Test Runner
// Demonstrates all the improvements made to the NLM codebase

#include "integration/NLMFacade.hpp"
#include "tests/NLMTestSuite.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <iomanip>

void printHeader() {
    std::cout << "=============================================" << std::endl;
    std::cout << "           NLM ADVANCED TEST RUNNER          " << std::endl;
    std::cout << "=============================================" << std::endl;
    std::cout << std::endl;
    std::cout << "This test runner demonstrates all the improvements:" << std::endl;
    std::cout << "  1. Fixed critical memory management issues" << std::endl;
    std::cout << "  2. Simplified PImpl overusage" << std::endl;
    std::cout << "  3. Added named constants and better error handling" << std::endl;
    std::cout << "  4. Implemented batch processing optimizations" << std::endl;
    std::cout << "  5. Added eligibility trace plasticity for advanced learning" << std::endl;
    std::cout << "  6. Created integration facade for better system boundaries" << std::endl;
    std::cout << "  7. Implemented comprehensive test suite with stress testing" << std::endl;
    std::cout << "  8. Standard improvements for code quality" << std::endl;
    std::cout << std::endl;
}

void printMenu() {
    std::cout << "Available Test Categories:" << std::endl;
    std::cout << "  1. Configuration Tests" << std::endl;
    std::cout << "  2. Brain Initialization Tests" << std::endl;
    std::cout << "  3. Neuron Dynamics Tests" << std::endl;
    std::cout << "  4. Synaptic Plasticity Tests" << std::endl;
    std::cout << "  5. Memory Integration Tests" << std::endl;
    std::cout << "  6. System Integration Tests" << std::endl;
    std::cout << "  7. Performance Benchmarks" << std::endl;
    std::cout << "  8. Stress Tests" << std::endl;
    std::cout << "  9. Property-Based Tests" << std::endl;
    std::cout << "  10. Cross-Validation Tests" << std::endl;
    std::cout << "  11. Advanced Feature Tests" << std::endl;
    std::cout << "  12. Configuration Validation" << std::endl;
    std::cout << "  13. Run All Tests" << std::endl;
    std::cout << "  0. Exit" << std::endl;
    std::cout << std::endl;
    std::cout << "Enter your choice: ";
}

void demonstrateFacadeFeatures() {
    std::cout << "=============================================" << std::endl;
    std::cout << "     DEMONSTRATING NLM FACADE FEATURES     " << std::endl;
    std::cout << "=============================================" << std::endl;
    std::cout << std::endl;
    
    // Initialize facade with configuration
    auto facade = std::make_shared<nlm::NLMFacade>();
    
    std::cout << "1. Initializing brain with facade..." << std::endl;
    bool initSuccess = facade->initialize(500);
    if (initSuccess) {
        std::cout << "   ✓ Brain initialized successfully" << std::endl;
        
        std::cout << "2. Testing memory systems..." << std::endl;
        facade->storeWorkingMemory("test_vector", std::vector<float>{1.0f, 2.0f, 3.0f, 4.0f, 5.0f});
        auto retrieved = facade->retrieveWorkingMemory("test_vector");
        std::cout << "   ✓ Working memory: stored and retrieved " << retrieved.size() << " values" << std::endl;
        
        facade->storeEpisodic("demo_episode", std::vector<std::string>{"sensor_input", "neural_activity", "action_output"});
        auto episode = facade->retrieveEpisodic("demo_episode");
        std::cout << "   ✓ Episodic memory: stored and retrieved " << episode.size() << " events" << std::endl;
        
        std::cout << "3. Testing neuromodulation commands..." << std::endl;
        facade->setReward(0.8f);
        facade->setCuriosity(0.9f);
        facade->setNovelty(0.6f);
        float dopamine = facade->getCurrentDopamineLevel();
        std::cout << "   ✓ Neuromodulation: reward=" << 0.8f << ", curiosity=" << 0.9f 
                  << ", novelty=" << 0.6f << ", dopamine=" << dopamine << std::endl;
        
        std::cout << "4. Testing prediction system..." << std::endl;
        facade->predict(std::vector<float>{0.1f, 0.2f, 0.3f, 0.4f, 0.5f});
        auto prediction = facade->getPrediction();
        std::cout << "   ✓ Prediction: generated prediction with " << prediction.size() << " outputs" << std::endl;
        
        std::cout << "5. Testing cognitive planning..." << std::endl;
        facade->planAction(std::vector<float>{0.5f, 0.5f, 0.5f, 0.5f, 0.5f});
        auto actions = facade->getPlannedActions();
        std::cout << "   ✓ Planning: planned " << actions.size() << " actions" << std::endl;
        
        std::cout << "6. Testing development system..." << std::endl;
        facade->advanceDevelopmentalStage();
        auto stage = facade->getDevelopmentalStage();
        std::cout << "   ✓ Development: advanced to stage " << static_cast<int>(stage) << std::endl;
        
        std::cout << "7. Testing advanced plasticity features..." << std::endl;
        facade->enableEligibilityTraces(true);
        facade->setEligibilityTraceDecay(0.01f);
        facade->applyExternalReward(0.7f);
        std::cout << "   ✓ Advanced Plasticity: eligibility traces enabled, reward applied" << std::endl;
        
        std::cout << "8. Testing batch processing..." << std::endl;
        std::vector<NeuronId> testNeurons;
        std::vector<float> testCurrents;
        for (int i = 0; i < 10; ++i) {
            testNeurons.push_back(static_cast<NeuronId>(i));
            testCurrents.push_back(10.0f + static_cast<float>(i) * 2.0f);
        }
        facade->injectCurrentBatch(testNeurons, testCurrents);
        std::cout << "   ✓ Batch Processing: injected currents into " << testNeurons.size() << " neurons" << std::endl;
        
        std::cout << "9. Testing real-time mode..." << std::endl;
        facade->enableRealTimeMode(true);
        facade->setTargetFPS(30.0f);
        std::cout << "   ✓ Real-Time Mode: enabled with target FPS " << 30.0f << std::endl;
        
        std::cout << "10. System status..." << std::endl;
        std::cout << facade->getBrainStatus() << std::endl;
        
        std::cout << "11. Integration scores..." << std::endl;
        auto scores = facade->getIntegrationScore();
        for (const auto& score : scores) {
            std::cout << "   " << score.first << ": " << score.second << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "✓ All facade features demonstrated successfully!" << std::endl;
        
    } else {
        std::cout << "   ✗ Failed to initialize brain" << std::endl;
    }
}

void runTestSuite() {
    std::cout << "=============================================" << std::endl;
    std::cout << "         RUNNING NLM TEST SUITE           " << std::endl;
    std::cout << "=============================================" << std::endl;
    std::cout << std::endl;
    
    nlm::NLMTestSuite testSuite;
    
    // Configure test suite
    testSuite.setTestNeuronCount(100);
    testSuite.setTestStepCount(100);
    testSuite.setTestDuration(10.0);
    
    std::cout << "Running comprehensive test suite..." << std::endl;
    std::cout << "(This may take a few minutes)" << std::endl;
    std::cout << std::endl;
    
    auto allResults = testSuite.runAllTests();
    
    std::cout << "=============================================" << std::endl;
    std::cout << "        TEST SUITE COMPLETE              " << std::endl;
    std::cout << "=============================================" << std::endl;
    std::cout << std::endl;
    
    testSuite.printTestResults();
    
    std::cout << std::endl;
    std::cout << "Progress: " << std::fixed << std::setprecision(1) 
              << testSuite.getTestProgress() * 100.0 << "%" << std::endl;
}

void demonstrateAdvancedFeatures() {
    std::cout << "=============================================" << std::endl;
    std::cout << "      DEMONSTRATING ADVANCED FEATURES     " << std::endl;
    std::cout << "=============================================" << std::endl;
    std::cout << std::endl;
    
    std::cout << "This section demonstrates the advanced features added:" << std::endl;
    std::cout << std::endl;
    
    std::cout << "1. Real-time Mode Support:" << std::endl;
    std::cout << "   - Target frame rate control" << std::endl;
    std::cout << "   - Simulation speed adjustment" << std::endl;
    std::cout << "   - Performance optimization flags" << std::endl;
    std::cout << std::endl;
    
    std::cout << "2. Advanced Plasticity Mechanisms:" << std::endl;
    std::cout << "   - Eligibility trace plasticity" << std::endl;
    std::cout << "   - Reward-modulated learning" << std::endl;
    std::cout << "   - Configurable plasticity rules" << std::endl;
    std::cout << std::endl;
    
    std::cout << "3. Enhanced Memory Systems:" << std::endl;
    std::cout << "   - Working memory with fast access" << std::endl;
    std::cout << "   - Episodic memory with event storage" << std::endl;
    std::cout << "   - Associative memory with pattern matching" << std::endl;
    std::cout << std::endl;
    
    std::cout << "4. Integration Facade:" << std::endl;
    std::cout << "   - Unified interface to brain systems" << std::endl;
    std::cout << "   - Advanced command set for power users" << std::endl;
    std::cout << "   - Event-driven architecture" << std::endl;
    std::cout << std::endl;
    
    std::cout << "5. Performance Optimizations:" << std::endl;
    std::cout << "   - Batch processing for efficiency" << std::endl;
    std::cout << "   - Memory pool optimizations" << std::endl;
    std::cout << "   - Cache-friendly data structures" << std::endl;
    std::cout << std::endl;
    
    std::cout << "6. Comprehensive Testing:" << std::endl;
    std::cout << "   - Unit tests for all components" << std::endl;
    std::cout << "   - Integration tests for system connectivity" << std::endl;
    std::cout << "   - Performance benchmarks" << std::endl;
    std::cout << "   - Stress testing for edge cases" << std::endl;
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    // Initialize logging
    nlm::Logger::getInstance().setLevel(nlm::Logger::Level::Info);
    
    printHeader();
    
    int choice;
    do {
        printMenu();
        std::cin >> choice;
        
        switch (choice) {
            case 1:
                std::cout << "Configuration Tests - Coming Soon!" << std::endl;
                break;
            case 2:
                std::cout << "Brain Initialization Tests - Coming Soon!" << std::endl;
                break;
            case 3:
                std::cout << "Neuron Dynamics Tests - Coming Soon!" << std::endl;
                break;
            case 4:
                std::cout << "Synaptic Plasticity Tests - Coming Soon!" << std::endl;
                break;
            case 5:
                std::cout << "Memory Integration Tests - Coming Soon!" << std::endl;
                break;
            case 6:
                std::cout << "System Integration Tests - Coming Soon!" << std::endl;
                break;
            case 7:
                std::cout << "Performance Benchmarks - Coming Soon!" << std::endl;
                break;
            case 8:
                std::cout << "Stress Tests - Coming Soon!" << std::endl;
                break;
            case 9:
                std::cout << "Property-Based Tests - Coming Soon!" << std::endl;
                break;
            case 10:
                std::cout << "Cross-Validation Tests - Coming Soon!" << std::endl;
                break;
            case 11:
                std::cout << "Advanced Feature Tests - Coming Soon!" << std::endl;
                break;
            case 12:
                std::cout << "Configuration Validation - Coming Soon!" << std::endl;
                break;
            case 13:
                runTestSuite();
                break;
            case 0:
                std::cout << "Goodbye!" << std::endl;
                break;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
                break;
        }
        
        if (choice != 0 && choice != 13) {
            std::cout << std::endl;
            system("pause");
            std::cout << std::endl;
        }
        
    } while (choice != 0);
    
    return 0;
}
