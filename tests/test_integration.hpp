#ifndef NLM_TEST_INTEGRATION_HPP
#define NLM_TEST_INTEGRATION_HPP

#include <cassert>
#include <iostream>
#include <memory>
#include <vector>
#include <cmath>

// Forward declarations
namespace nlm {
    class Config;
    class Brain;
    class NeuralWorkingMemory;
    class NeuralEpisodicMemory;
    class NeuralAssociativeMemory;
    class PredictionSystem;
    class NeuralPlanner;
    class ConceptFormation;
    class AttentionalSelection;
    class DevelopmentSystem;
    class Dopamine;
    class Curiosity;
    class Novelty;
    class PredictionError;
    class SpikeSystem;
    class STDP;
    class Hebbian;
    class StructuralPlasticity;
    class CheckpointManager;
}

namespace test_integration {
    
    // Test utilities
    struct TestConfig {
        size_t neuronCount = 1000;
        size_t regionCount = 1;
        float connectionProbability = 0.1f;
        std::string checkpointDir = "./test_checkpoints";
        
        std::shared_ptr<nlm::Config> createConfig() const;
    };
    
    // Test fixtures
    class BrainTestFixture {
    public:
        BrainTestFixture();
        virtual ~BrainTestFixture();
        
        bool setup();
        void teardown();
        
        nlm::Brain* getBrain() const { return brain.get(); }
        const TestConfig& getConfig() const { return config; }
        
    protected:
        TestConfig config;
        std::shared_ptr<nlm::Brain> brain;
    };
    
    // Phase 6 Integration Tests
    namespace phase6_tests {
        
        void testMemorySystemIntegration(BrainTestFixture& fixture);
        void testNeuromodulationIntegration(BrainTestFixture& fixture);
        void testPredictionSystemIntegration(BrainTestFixture& fixture);
        void testCognitiveSystemIntegration(BrainTestFixture& fixture);
        void testDevelopmentSystemIntegration(BrainTestFixture& fixture);
        void testPlasticitySystemIntegration(BrainTestFixture& fixture);
        void testSpikeSystemIntegration(BrainTestFixture& fixture);
        void testCompleteBrainLoop(BrainTestFixture& fixture);
        void testMemoryReplay(BrainTestFixture& fixture);
        void testCheckpointSystem(BrainTestFixture& fixture);
        void testLearningDynamics(BrainTestFixture& fixture);
        
    } // namespace phase6_tests
    
    // Performance Tests
    namespace performance_tests {
        
        void testInitializationPerformance(BrainTestFixture& fixture);
        void testStepPerformance(BrainTestFixture& fixture);
        void testMemoryUsage(BrainTestFixture& fixture);
        void testSpikeSystemPerformance(BrainTestFixture& fixture);
        void testPlasticityPerformance(BrainTestFixture& fixture);
        
    } // namespace performance_tests
    
    // Stress Tests
    namespace stress_tests {
        
        void testLargeScaleNetwork(BrainTestFixture& fixture);
        void testHighSpikeActivity(BrainTestFixture& fixture);
        void testMemoryGrowth(BrainTestFixture& fixture);
        void testPlasticityRate(BrainTestFixture& fixture);
        
    } // namespace stress_tests
    
    // Test utilities
    namespace test_utils {
        
        template<typename T>
        T clamp(T value, T min, T max) {
            return std::max(min, std::min(max, value));
        }
        
        double getCurrentTime();
        void cleanupTestFiles();
        
    } // namespace test_utils
    
    // Test runner
    class TestRunner {
    public:
        static TestRunner& getInstance();
        
        void runAllTests();
        bool runPhase6Tests();
        bool runPerformanceTests();
        bool runStressTests();
        
        void printTestSummary() const;
        
    private:
        TestRunner() {}
        ~TestRunner() {}
        TestRunner(const TestRunner&) = delete;
        TestRunner& operator=(const TestRunner&) = delete;
        
        struct TestResults {
            int passed = 0;
            int failed = 0;
            int skipped = 0;
            
            void addPassed() { passed++; }
            void addFailed() { failed++; }
            void addSkipped() { skipped++; }
            
            double getPassRate() const {
                int total = passed + failed + skipped;
                return total > 0 ? (double)passed / total * 100.0 : 0.0;
            }
        };
        
        TestResults results;
    };
    
} // namespace test_integration

#endif // NLM_TEST_INTEGRATION_HPP