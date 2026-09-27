#include "Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <cassert>
#include <chrono>

using namespace nlm;

/**
 * Test framework for Phase 6 integration
 */
class Phase6IntegrationTest {
public:
    static void runAllTests() {
        std::cout << "=== NLM Phase 6 Integration Tests ===" << std::endl;
        
        // Initialize logging for tests
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        // Run tests
        testIntegrationVerification();
        testMemoryIntegration();
        testNeuromodulationIntegration();
        testCheckpointing();
        testReplay();
        testResultsExport();
        testPerformanceBenchmark();
        
        std::cout << "\n=== All Phase 6 Integration Tests Passed ===" << std::endl;
    }
    
private:
    static void testIntegrationVerification() {
        std::cout << "\n--- Testing Integration Verification ---" << std::endl;
        
        Phase6IntegratedExperiment experiment;
        bool result = experiment.verifyIntegration();
        
        assert(result && "Integration verification failed");
        std::cout << "✓ Integration verification passed" << std::endl;
    }
    
    static void testMemoryIntegration() {
        std::cout << "\n--- Testing Memory Integration ---" << std::endl;
        
        Phase6IntegratedExperiment experiment;
        bool result = experiment.testMemoryIntegration();
        
        assert(result && "Memory integration test failed");
        std::cout << "✓ Memory integration test passed" << std::endl;
    }
    
    static void testNeuromodulationIntegration() {
        std::cout << "\n--- Testing Neuromodulation Integration ---" << std::endl;
        
        Phase6IntegratedExperiment experiment;
        bool result = experiment.testNeuromodulationIntegration();
        
        assert(result && "Neuromodulation integration test failed");
        std::cout << "✓ Neuromodulation integration test passed" << std::endl;
    }
    
    static void testCheckpointing() {
        std::cout << "\n--- Testing Checkpoint Functionality ---" << std::endl;
        
        Phase6IntegratedExperiment experiment;
        bool result = experiment.testCheckpointing();
        
        assert(result && "Checkpointing test failed");
        std::cout << "✓ Checkpointing test passed" << std::endl;
    }
    
    static void testReplay() {
        std::cout << "\n--- Testing Replay Functionality ---" << std::endl;
        
        Phase6IntegratedExperiment experiment;
        bool result = experiment.testReplay();
        
        assert(result && "Replay test failed");
        std::cout << "✓ Replay test passed" << std::endl;
    }
    
    static void testResultsExport() {
        std::cout << "\n--- Testing Results Export ---" << std::endl;
        
        // Create a Phase 6 integration result
        Phase6IntegrationResult result;
        result.memoryWorkingMemoryIntegrated = true;
        result.memoryEpisodicMemoryIntegrated = true;
        result.neuromodulationIntegrated = true;
        result.predictionIntegrated = true;
        result.developmentIntegrated = true;
        result.checkpointingWorks = true;
        result.replayWorks = true;
        result.totalReward = 150.75f;
        result.avgFiringRate = 12.5f;
        result.avgSynapticWeight = 0.02f;
        result.memoryEpisodesStored = 25.0f;
        result.noveltyLevel = 0.8f;
        result.curiosityLevel = 0.6f;
        result.dopamineLevel = 0.75f;
        result.totalWallClockTime = 125.3;
        
        // Test JSON output
        std::string jsonOutput = result.toJSON();
        assert(!jsonOutput.empty() && "JSON output should not be empty");
        assert(jsonOutput.find("totalReward") != std::string::npos && "JSON should contain totalReward");
        std::cout << "✓ JSON export test passed" << std::endl;
        
        // Test summary output
        std::string summaryOutput = result.summary();
        assert(!summaryOutput.empty() && "Summary output should not be empty");
        assert(summaryOutput.find("Working Memory") != std::string::npos && "Summary should contain Working Memory");
        std::cout << "✓ Summary export test passed" << std::endl;
        
        // Test results to file
        std::string testFile = "/tmp/phase6_test_results.json";
        std::ofstream file(testFile);
        assert(file.is_open() && "Should be able to open results file for writing");
        file << jsonOutput;
        file.close();
        
        // Verify file was written
        std::ifstream readFile(testFile);
        assert(readFile.is_open() && "Should be able to open results file for reading");
        readFile.close();
        std::cout << "✓ File export test passed" << std::endl;
        
        // Clean up
        std::remove(testFile.c_str());
        
        std::cout << "✓ Results export test passed" << std::endl;
    }
    
    static void testPerformanceBenchmark() {
        std::cout << "\n--- Testing Performance Benchmark ---" << std::endl;
        
        // Run a performance benchmark
        Phase6Config config;
        config.neuronCount = 500;
        config.maxSteps = 2000;
        config.enableCheckpointing = false;
        config.enableReplay = false;
        config.enableDevelopment = false;
        
        Phase6IntegratedExperiment experiment;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        Phase6IntegrationResult result = experiment.run(config);
        auto endTime = std::chrono::high_resolution_clock::now();
        
        double elapsedTime = std::chrono::duration<double>(endTime - startTime).count();
        
        // Performance assertions
        assert(result.totalReward >= 0.0f && "Total reward should be non-negative");
        assert(result.avgFiringRate >= 0.0f && "Average firing rate should be non-negative");
        assert(result.totalWallClockTime >= 0.0 && "Wall clock time should be non-negative");
        
        // Performance metrics
        float stepsPerSecond = config.maxSteps / elapsedTime;
        std::cout << "  Performance: " << stepsPerSecond << " steps/second" << std::endl;
        std::cout << "  Wall clock time: " << elapsedTime << " seconds" << std::endl;
        std::cout << "  Total reward: " << result.totalReward << std::endl;
        
        std::cout << "✓ Performance benchmark test passed" << std::endl;
    }
};

/**
 * Main test function
 */
int main(int argc, char* argv[]) {
    try {
        Phase6IntegrationTest::runAllTests();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "ERROR: Test failed with unknown exception" << std::endl;
        return 1;
    }
}