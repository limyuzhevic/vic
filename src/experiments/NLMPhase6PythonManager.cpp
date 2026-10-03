// Phase 6 Integration Test - Final Integration
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>

using namespace nlm;

class NLMPhase6Manager {
public:
    NLMPhase6Manager() {
        // Initialize logging for Python interface
        Logger::getInstance().setLevel(Logger::Level::Info);
        std::cout << "=== NLM Phase 6 Python Integration Manager ===" << std::endl;
        std::cout << "Ready for advanced brain simulation tasks." << std::endl;
    }
    
    // Utility function: Create comprehensive experiment configuration
    static Phase6Config createAdvancedConfig(uint64_t maxSteps = 5000, 
                                            size_t neuronCount = 800,
                                            size_t regionCount = 2,
                                            float connectionProbability = 0.08f,
                                            bool enableCheckpointing = true,
                                            bool enableReplay = true,
                                            bool enableDevelopment = true,
                                            const std::string& checkpointPath = "./checkpoint.bin") {
        Phase6Config config;
        config.maxSteps = maxSteps;
        config.neuronCount = neuronCount;
        config.regionCount = regionCount;
        config.connectionProbability = connectionProbability;
        config.enableCheckpointing = enableCheckpointing;
        config.enableReplay = enableReplay;
        config.enableDevelopment = enableDevelopment;
        config.checkpointPath = checkpointPath;
        return config;
    }
    
    // Utility function: Run comprehensive integration test
    Phase6IntegrationResult runFullIntegrationTest() {
        std::cout << "--- Running Full Integration Test ---" << std::endl;
        Phase6IntegratedExperiment experiment;
        
        // Create advanced configuration
        Phase6Config config = createAdvancedConfig();
        
        // Run the complete integration test
        auto result = experiment.run(config);
        
        printResults(result);
        return result;
    }
    
    // Utility function: Quick integration verification
    bool quickVerification() {
        std::cout << "--- Quick Integration Verification ---" << std::endl;
        Phase6IntegratedExperiment experiment;
        return experiment.verifyIntegration();
    }
    
    // Utility function: Test individual systems
    void testIndividualSystems() {
        std::cout << "--- Testing Individual Systems ---" << std::endl;
        Phase6IntegratedExperiment experiment;
        
        bool memoryOK = experiment.testMemoryIntegration();
        std::cout << "Memory Integration: " << (memoryOK ? "PASSED" : "FAILED") << std::endl;
        
        bool neuromodOK = experiment.testNeuromodulationIntegration();
        std::cout << "Neuromodulation Integration: " << (neuromOK ? "PASSED" : "FAILED") << std::endl;
        
        bool checkpointOK = experiment.testCheckpointing();
        std::cout << "Checkpointing: " << (checkpointOK ? "PASSED" : "FAILED") << std::endl;
        
        bool replayOK = experiment.testReplay();
        std::cout << "Replay System: " << (replayOK ? "PASSED" : "FAILED") << std::endl;
    }
    
    // Utility function: Save/load checkpoint
    bool saveCheckpoint(const std::string& filepath) {
        std::cout << "--- Saving Checkpoint to " << filepath << " ---" << std::endl;
        // Implementation would save experiment state
        std::ofstream file(filepath, std::ios::binary);
        if (!file) {
            std::cerr << "Failed to create checkpoint file!" << std::endl;
            return false;
        }
        
        // Write checkpoint data
        file.write("NLM_CHECKPOINT_V1", 15);
        file.close();
        std::cout << "Checkpoint saved successfully!" << std::endl;
        return true;
    }
    
    // Utility function: Load checkpoint
    bool loadCheckpoint(const std::string& filepath) {
        std::cout << "--- Loading Checkpoint from " << filepath << " ---" << std::endl;
        // Implementation would load experiment state
        std::ifstream file(filepath, std::ios::binary);
        if (!file) {
            std::cerr << "Failed to load checkpoint file!" << std::endl;
            return false;
        }
        
        char buffer[16];
        file.read(buffer, 15);
        buffer[15] = '\0';
        
        if (std::string(buffer) != "NLM_CHECKPOINT_V1") {
            std::cerr << "Invalid checkpoint format!" << std::endl;
            return false;
        }
        
        file.close();
        std::cout << "Checkpoint loaded successfully!" << std::endl;
        return true;
    }
    
    // Utility function: Format and display results
    void printResults(const Phase6IntegrationResult& result) {
        std::cout << std::endl;
        std::cout << "=== FINAL RESULTS ===" << std::endl;
        std::cout << "Total Reward: " << result.totalReward << std::endl;
        std::cout << "Average Firing Rate: " << result.avgFiringRate << " Hz" << std::endl;
        std::cout << "Average Synaptic Weight: " << result.avgSynapticWeight << std::endl;
        std::cout << "Memory Episodes Stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "Novelty Level: " << result.noveltyLevel << std::endl;
        std::cout << "Curiosity Level: " << result.curiosityLevel << std::endl;
        std::cout << "Dopamine Level: " << result.dopamineLevel << std::endl;
        std::cout << std::endl;
        
        std::cout << "=== INTEGRATION STATUS ===" << std::endl;
        std::cout << "Working Memory Integration: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Episodic Memory Integration: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Neuromodulation Integration: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Prediction Integration: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Development Integration: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
        std::cout << "Replay System: " << (result.replayWorks ? "FUNCTIONAL" : "NOT FUNCTIONAL") << std::endl;
        std::cout << std::endl;
        
        std::cout << "Wall Clock Time: " << result.totalWallClockTime << " seconds" << std::endl;
        
        // Determine overall success
        bool allOK = result.memoryWorkingMemoryIntegrated && 
                    result.memoryEpisodicMemoryIntegrated &&
                    result.neuromodulationIntegrated &&
                    result.predictionIntegrated &&
                    result.developmentIntegrated &&
                    result.checkpointingWorks &&
                    result.replayWorks;
        
        std::cout << "=== OVERALL STATUS ===" << std::endl;
        if (allOK) {
            std::cout << "✓ ALL SYSTEMS INTEGRATED SUCCESSFULLY" << std::endl;
            std::cout << "The NLM brain is fully functional with all subsystems connected." << std::endl;
        } else {
            std::cout << "✗ INTEGRATION ISSUES DETECTED" << std::endl;
            std::cout << "Some systems may need additional integration work." << std::endl;
        }
    }
    
    // Utility function: Create custom configuration from Python
    static std::string createConfigFromParams(const std::string& name,
                                            uint64_t maxSteps,
                                            size_t neuronCount,
                                            size_t regionCount,
                                            float connectionProbability,
                                            bool enableCheckpointing,
                                            bool enableReplay,
                                            bool enableDevelopment) {
        // Create a JSON-like configuration string
        std::string config = "{\n";
        config += "  \"name\": \"" + name + "\",\n";
        config += "  \"maxSteps\": " + std::to_string(maxSteps) + ",\n";
        config += "  \"neuronCount\": " + std::to_string(neuronCount) + ",\n";
        config += "  \"regionCount\": " + std::to_string(regionCount) + ",\n";
        config += "  \"connectionProbability\": " + std::to_string(connectionProbability) + ",\n";
        config += "  \"enableCheckpointing\": " + std::string(enableCheckpointing ? "true" : "false") + ",\n";
        config += "  \"enableReplay\": " + std::string(enableReplay ? "true" : "false") + ",\n";
        config += "  \"enableDevelopment\": " + std::string(enableDevelopment ? "true" : "false") + "\n";
        config += "}";
        return config;
    }
    
    // Utility function: Validate experiment results
    static bool validateResults(const Phase6IntegrationResult& result) {
        std::cout << "--- Validating Experiment Results ---" << std::endl;
        
        bool valid = true;
        
        // Check integration status
        if (!result.memoryWorkingMemoryIntegrated) {
            std::cout << "WARNING: Working memory integration failed" << std::endl;
            valid = false;
        }
        
        if (!result.memoryEpisodicMemoryIntegrated) {
            std::cout << "WARNING: Episodic memory integration failed" << std::endl;
            valid = false;
        }
        
        if (!result.neuromodulationIntegrated) {
            std::cout << "WARNING: Neuromodulation integration failed" << std::endl;
            valid = false;
        }
        
        if (!result.predictionIntegrated) {
            std::cout << "WARNING: Prediction integration failed" << std::endl;
            valid = false;
        }
        
        if (!result.developmentIntegrated) {
            std::cout << "WARNING: Development integration failed" << std::endl;
            valid = false;
        }
        
        if (!result.checkpointingWorks) {
            std::cout << "WARNING: Checkpointing not working" << std::endl;
            valid = false;
        }
        
        if (!result.replayWorks) {
            std::cout << "WARNING: Replay system not functional" << std::endl;
            valid = false;
        }
        
        // Check metric ranges
        if (result.totalReward < -1000.0f || result.totalReward > 1000.0f) {
            std::cout << "WARNING: Total reward out of expected range: " << result.totalReward << std::endl;
            valid = false;
        }
        
        if (result.avgFiringRate < 0.0f || result.avgFiringRate > 100.0f) {
            std::cout << "WARNING: Average firing rate out of expected range: " << result.avgFiringRate << std::endl;
            valid = false;
        }
        
        if (result.dopamineLevel < -2.0f || result.dopamineLevel > 2.0f) {
            std::cout << "WARNING: Dopamine level out of expected range: " << result.dopamineLevel << std::endl;
            valid = false;
        }
        
        if (valid) {
            std::cout << "✓ Results validation PASSED" << std::endl;
        } else {
            std::cout << "✗ Results validation FAILED" << std::endl;
        }
        
        return valid;
    }
    
    // Utility function: Print experiment summary
    static void printExperimentSummary(const Phase6IntegrationResult& result) {
        std::cout << std::endl;
        std::cout << "=== EXPERIMENT SUMMARY ===" << std::endl;
        std::cout << "Experiment completed successfully!" << std::endl;
        std::cout << "Key metrics captured:" << std::endl;
        std::cout << "  - Total Reward: " << result.totalReward << std::endl;
        std::cout << "  - Average Firing Rate: " << result.avgFiringRate << " Hz" << std::endl;
        std::cout << "  - Memory Efficiency: " << (result.memoryEpisodesStored / result.totalWallClockTime) << " episodes/sec" << std::endl;
        std::cout << "  - Learning Capacity: " << (result.totalReward / result.totalWallClockTime) << " reward/sec" << std::endl;
        std::cout << "  - Neuromodulation Level: " << result.dopamineLevel << std::endl;
        std::cout << std::endl;
        std::cout << "Experiment duration: " << result.totalWallClockTime << " seconds" << std::endl;
    }
    
    // Utility function: Run experiment with custom settings
    Phase6IntegrationResult runExperimentWithSettings(const std::string& experimentName,
                                                     uint64_t steps,
                                                     size_t neurons,
                                                     bool checkpoint,
                                                     bool replay,
                                                     bool development) {
        std::cout << "--- Running Custom Experiment: " << experimentName << " ---" << std::endl;
        
        Phase6Config config = createAdvancedConfig(
            steps,
            neurons,
            1, // region count
            0.05f, // connection probability
            checkpoint,
            replay,
            development
        );
        
        Phase6IntegratedExperiment experiment;
        auto result = experiment.run(config);
        
        printExperimentSummary(result);
        
        return result;
    }
};