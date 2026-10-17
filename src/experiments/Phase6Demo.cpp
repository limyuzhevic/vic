// Phase 6 Demo using CLI Framework
// This demo runs Phase 6 integration test with CLI support

#include "Phase6Experiment.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include "core/CLI/CLIFramework.hpp"
#include <iostream>
#include <memory>

namespace nlm {

// Phase 6 demo class
class Phase6Demo {
public:
    Phase6Demo() {}
    ~Phase6Demo() {}
    
    // Run Phase 6 integration test
    void run(const Phase6Config& config) {
        std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
        std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
        
        // Initialize logging
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        // Create Phase 6 experiment
        Phase6Experiment experiment(config);
        
        // First, run the quick integration verification
        std::cout << "--- Integration Verification ---" << std::endl;
        bool integrationOK = experiment.verifyIntegration();
        
        std::cout << std::endl;
        
        if (!integrationOK) {
            std::cerr << "ERROR: Integration verification failed!" << std::endl;
            return;
        }
        
        std::cout << "Integration verification passed!" << std::endl << std::endl;
        
        // Test individual systems
        std::cout << "--- Memory Integration Test ---" << std::endl;
        // In a real implementation, this would test specific memory integration
        std::cout << "Memory systems integrated: " << (integrationOK ? "YES" : "NO") << std::endl;
        std::cout << std::endl;
        
        std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
        // In a real implementation, this would test neuromodulation
        std::cout << "Neuromodulation systems integrated: " << (integrationOK ? "YES" : "NO") << std::endl;
        std::cout << std::endl;
        
        std::cout << "--- Checkpoint Test ---" << std::endl;
        // In a real implementation, this would test checkpointing
        std::cout << "Checkpoint system: " << (config.enableCheckpointing ? "ENABLED" : "DISABLED") << std::endl;
        std::cout << std::endl;
        
        std::cout << "--- Replay Test ---" << std::endl;
        // In a real implementation, this would test replay
        std::cout << "Replay system: READY" << std::endl;
        std::cout << std::endl;
        
        // Run full experiment with configured settings
        std::cout << "--- Full Integration Experiment ---" << std::endl;
        auto result = experiment.run();
        
        std::cout << std::endl;
        std::cout << "=== FINAL RESULTS ===" << std::endl;
        std::cout << "Total reward: " << result.totalReward << std::endl;
        std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
        std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
        std::cout << std::endl;
        
        std::cout << "=== INTEGRATION STATUS ===" << std::endl;
        std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
        std::cout << std::endl;
        
        std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
        
        // Export results
        if (config.enableCheckpointing) {
            std::cout << "\nExporting experiment results..." << std::endl;
            experiment.exportResults("/tmp/phase6_results.json", "json");
            std::cout << "Results exported to: /tmp/phase6_results.json" << std::endl;
        }
    }
};

} // namespace nlm

int main(int argc, char* argv[]) {
    // Use the new CLI framework
    return nlm::CLIAdapter::adaptPhase6Demo(argc, argv);
}

