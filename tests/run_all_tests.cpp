// run_all_tests.cpp - Comprehensive test runner for NLM
#include "TestFramework.hpp"
#include "TestUtils.hpp"
#include "MemoryTests.cpp"
#include "NeuromodulationTests.cpp"
#include "CognitionTests.cpp"
#include "DevelopmentTests.cpp"
#include "BehavioralTests.cpp"
#include "MultiAgentTests.cpp"
#include "StatisticalTests.cpp"
#include "Phase6IntegratedExperiment.hpp"

namespace nlm {

// Test runner for all test categories
class AllTests {
public:
    void runTest() {
        TestRunner runner(true);
        
        // Register all test categories
        register_memory_tests(runner);
        register_neuromodulation_tests(runner);
        register_cognition_tests(runner);
        register_development_tests(runner);
        register_behavioral_tests(runner);
        register_multiagent_tests(runner);
        register_statistical_tests(runner);
        
        // Run Phase 6 integration tests
        register_phase6_integration_tests(runner);
        
        // Run enhanced Phase 6 experiment
        register_enhanced_phase6_tests(runner);
        
        runner.runAll();
    }
private:
    void register_memory_tests(TestRunner& runner) {
        // Unit tests for memory systems
        TEST_CASE(MemoryUnits, "Working Memory Unit Tests");
        TEST_CASE(MemoryUnits, "Episodic Memory Unit Tests");
        TEST_CASE(MemoryUnits, "Associative Memory Unit Tests");
        TEST_CASE(MemoryUnits, "Memory Consolidation Tests");
        TEST_CASE(MemoryUnits, "Memory Replay Tests");
    }
    
    void register_neuromodulation_tests(TestRunner& runner) {
        // Unit tests for neuromodulators
        TEST_CASE(NeuromodulationUnits, "Dopamine Unit Tests");
        TEST_CASE(NeuromodulationUnits, "Acetylcholine Unit Tests");
        TEST_CASE(NeuromodulationUnits, "Norepinephrine Unit Tests");
        TEST_CASE(NeuromodulationUnits, "Serotonin Unit Tests");
        TEST_CASE(NeuromodulationUnits, "Curiosity Unit Tests");
        TEST_CASE(NeuromodulationUnits, "Novelty Unit Tests");
        TEST_CASE(NeuromodulationUnits, "Prediction Error Unit Tests");
        TEST_CASE(NeuromodulationUnits, "Reward Unit Tests");
        
        // Neuromodulation integration tests
        TEST_CASE(NeuromodulationIntegration, "Neuromodulator Integration Tests");
    }
    
    void register_cognition_tests(TestRunner& runner) {
        // Unit tests for cognitive systems
        TEST_CASE(CognitionUnits, "Neural Planner Unit Tests");
        TEST_CASE(CognitionUnits, "Concept Formation Unit Tests");
        TEST_CASE(CognitionUnits, "Attentional Selection Unit Tests");
        
        // Cognition integration tests
        TEST_CASE(CognitionIntegration, "Cognition System Integration Tests");
    }
    
    void register_development_tests(TestRunner& runner) {
        // Unit tests for development system
        TEST_CASE(DevelopmentUnits, "Development System Unit Tests");
        TEST_CASE(DevelopmentUnits, "Maturation Unit Tests");
        TEST_CASE(DevelopmentUnits, "Synaptogenesis Unit Tests");
        TEST_CASE(DevelopmentUnits, "Pruning Unit Tests");
        
        // Development integration tests
        TEST_CASE(DevelopmentIntegration, "Development System Integration Tests");
    }
    
    void register_behavioral_tests(TestRunner& runner) {
        // Behavioral tests
        TEST_CASE(BehavioralMemory, "Memory Retention Tests");
        TEST_CASE(BehavioralLearning, "Learning Progress Tests");
        TEST_CASE(BehavioralContinual, "Continual Learning Tests");
        TEST_CASE(BehavioralGeneralization, "Generalization Tests");
        TEST_CASE(BehavioralPrediction, "Prediction Accuracy Tests");
        TEST_CASE(BehavioralIndividual, "Individual Differences Tests");
        
        // Statistical behavioral tests
        TEST_CASE(BehavioralStats, "Statistical Learning Tests");
        TEST_CASE(BehavioralDev, "Developmental Behavioral Tests");
    }
    
    void register_multiagent_tests(TestRunner& runner) {
        // Multi-agent tests
        TEST_CASE(MultiAgent, "Multi-Agent Individual Differences");
        TEST_CASE(MultiAgent, "Social Learning Tests");
        TEST_CASE(MultiAgent, "Emergent Behaviors");
        
        // Social and cooperative tests
        TEST_CASE(MultiAgentSocial, "Social Cooperation Tests");
        TEST_CASE(MultiAgentCompetition, "Agent Competition Tests");
    }
    
    void register_statistical_tests(TestRunner& runner) {
        // Statistical validation tests
        TEST_CASE(StatisticalLearning, "Learning Curve Statistical Tests");
        TEST_CASE(StatisticalMemory, "Memory Effect Hypothesis Tests");
        TEST_CASE(StatisticalDevelopment, "Developmental Regression Tests");
        TEST_CASE(StatisticalPrediction, "Prediction Accuracy Regression");
        
        // Performance and reliability tests
        TEST_CASE(StatisticalPerformance, "Performance Regression Tests");
        TEST_CASE(StatisticalRobustness, "System Robustness Tests");
    }
    
    void register_phase6_integration_tests(TestRunner& runner) {
        // Phase 6 integration tests
        TEST_CASE(Phase6Integration, "Phase 6 System Integration Verification");
        TEST_CASE(Phase6MemoryIntegration, "Phase 6 Memory Integration Tests");
        TEST_CASE(Phase6NeuromodulationIntegration, "Phase 6 Neuromodulation Integration Tests");
        TEST_CASE(Phase6DevelopmentIntegration, "Phase 6 Development Integration Tests");
        TEST_CASE(Phase6PredictionIntegration, "Phase 6 Prediction Integration Tests");
        TEST_CASE(Phase6Checkpointing, "Phase 6 Checkpointing Tests");
        TEST_CASE(Phase6Replay, "Phase 6 Replay System Tests");
    }
    
    void register_enhanced_phase6_tests(TestRunner& runner) {
        // Enhanced Phase 6 comprehensive tests
        TEST_CASE(Phase6Enhanced, "Phase 6 Enhanced Comprehensive Tests");
        TEST_CASE(Phase6Scaling, "Phase 6 Scaling Benchmark Tests");
        TEST_CASE(Phase6Ablation, "Phase 6 Ablation System Tests");
    }
};

// Phase 6 integration test helper functions
namespace Phase6TestHelpers {
    bool runPhase6IntegrationTest() {
        Phase6Config config;
        config.neuronCount = 100;
        config.maxSteps = 1000;
        config.enableDevelopment = true;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        
        Phase6IntegratedExperiment experiment;
        auto result = experiment.run(config);
        
        // Check all integration points
        bool success = true;
        if (!result.memoryWorkingMemoryIntegrated) {
            std::cout << "FAIL: Working memory not integrated" << std::endl;
            success = false;
        }
        if (!result.memoryEpisodicMemoryIntegrated) {
            std::cout << "FAIL: Episodic memory not integrated" << std::endl;
            success = false;
        }
        if (!result.neuromodulationIntegrated) {
            std::cout << "FAIL: Neuromodulation not integrated" << std::endl;
            success = false;
        }
        if (!result.predictionIntegrated) {
            std::cout << "FAIL: Prediction not integrated" << std::endl;
            success = false;
        }
        if (!result.developmentIntegrated) {
            std::cout << "FAIL: Development not integrated" << std::endl;
            success = false;
        }
        
        if (result.checkpointingWorks) {
            std::cout << "PASS: Checkpointing works" << std::endl;
        }
        
        return success;
    }
    
    bool verifyAllSystems() {
        bool success = true;
        
        // Create minimal brain
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        cfg->set("region_count", 1);
        
        auto brain = std::make_shared<Brain>(cfg);
        if (!brain->initialize()) {
            std::cout << "FAIL: Brain initialization failed" << std::endl;
            return false;
        }
        
        // Test all systems
        if (brain->getWorkingMemory() != nullptr) {
            std::cout << "PASS: Working memory is integrated" << std::endl;
        } else {
            std::cout << "FAIL: Working memory is NOT integrated" << std::endl;
            success = false;
        }
        
        if (brain->getEpisodicMemory() != nullptr) {
            std::cout << "PASS: Episodic memory is integrated" << std::endl;
        } else {
            std::cout << "FAIL: Episodic memory is NOT integrated" << std::endl;
            success = false;
        }
        
        if (brain->getDopamine() != nullptr) {
            std::cout << "PASS: Dopamine system is integrated" << std::endl;
        } else {
            std::cout << "FAIL: Dopamine system is NOT integrated" << std::endl;
            success = false;
        }
        
        if (brain->getCuriosity() != nullptr) {
            std::cout << "PASS: Curiosity system is integrated" << std::endl;
        } else {
            std::cout << "FAIL: Curiosity system is NOT integrated" << std::endl;
            success = false;
        }
        
        if (brain->getPredictionSystem() != nullptr) {
            std::cout << "PASS: Prediction system is integrated" << std::endl;
        } else {
            std::cout << "FAIL: Prediction system is NOT integrated" << std::endl;
            success = false;
        }
        
        if (brain->getPlanner() != nullptr) {
            std::cout << "PASS: Neural planner is integrated" << std::endl;
        } else {
            std::cout << "FAIL: Neural planner is NOT integrated" << std::endl;
            success = false;
        }
        
        if (brain->getConceptFormation() != nullptr) {
            std::cout << "PASS: Concept formation is integrated" << std::endl;
        } else {
            std::cout << "FAIL: Concept formation is NOT integrated" << std::endl;
            success = false;
        }
        
        if (brain->getAttention() != nullptr) {
            std::cout << "PASS: Attention is integrated" << std::endl;
        } else {
            std::cout << "FAIL: Attention is NOT integrated" << std::endl;
            success = false;
        }
        
        return success;
    }
}};

// Entry point for all tests
int main(int argc, char* argv[]) {
    std::cout << "=== NLM Comprehensive Test Suite ===" << std::endl;
    
    bool runAll = true;
    bool runRegression = false;
    bool runBehavioral = false;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--regression") {
            runAll = false;
            runRegression = true;
        } else if (arg == "--behavioral") {
            runAll = false;
            runBehavioral = true;
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: nlm_test [options]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --all          Run all tests (default)" << std::endl;
            std::cout << "  --regression   Run regression tests only" << std::endl;
            std::cout << "  --behavioral   Run behavioral tests only" << std::endl;
            std::cout << "  --help, -h     Show this help message" << std::endl;
            return 0;
        }
    }
    
    AllTests testSuite;
    
    if (runRegression) {
        std::cout << "=== Running Regression Tests ===" << std::endl;
        
        // Run regression tests only
        TestRunner regressionRunner(false);
        
        // Register regression tests
        TEST_CASE(StatisticalLearning, "Learning Curve Regression Tests");
        TEST_CASE(StatisticalMemory, "Memory Effect Regression Tests");
        TEST_CASE(StatisticalDevelopment, "Developmental Regression Tests");
        TEST_CASE(StatisticalPerformance, "Performance Regression Tests");
        
        regressionRunner.runAll();
    } else if (runBehavioral) {
        std::cout << "=== Running Behavioral Tests ===" << std::endl;
        
        // Run behavioral tests only
        TestRunner behavioralRunner(true);
        
        // Register behavioral tests
        TEST_CASE(BehavioralMemory, "Memory Retention Behavioral Tests");
        TEST_CASE(BehavioralLearning, "Learning Progress Behavioral Tests");
        TEST_CASE(BehavioralContinual, "Continual Learning Behavioral Tests");
        TEST_CASE(BehavioralGeneralization, "Generalization Behavioral Tests");
        TEST_CASE(BehavioralPrediction, "Prediction Accuracy Behavioral Tests");
        TEST_CASE(BehavioralIndividual, "Individual Differences Behavioral Tests");
        
        behavioralRunner.runAll();
    } else {
        // Run all tests
        testSuite.runTest();
    }
    
    return 0;
}
