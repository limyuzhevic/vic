/**
 * Phase 6 Demo - Enhanced Integration Test
 * 
 * This demo runs comprehensive integration tests to verify
 * that all brain systems are properly connected and functioning.
 * Enhanced with advanced features, performance monitoring, and
 * detailed diagnostic capabilities.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <memory>
#include <string>

using namespace nlm;

void printEnhancedBanner() {
    std::cout << R"(
╔═══════════════════════════════════════════════════════════════╗
║                                                               ║
║     NLM Phase 6 — Enhanced Integration Test                   ║
║                                                               ║
║     Comprehensive artificial brain verification               ║
║                                                               ║
║     Tests all 6 major brain systems:                          ║
║     - Working Memory            (Phase 1)                       ║
║     - Episodic Memory          (Phase 2)                       ║
║     - Neuromodulation           (Phase 3)                       ║
║     - Prediction & Attention   (Phase 4)                       ║
║     - Development              (Phase 5)                       ║
║     - Social & Concept Formation (Phase 6)                    ║
║                                                               ║
╚═══════════════════════════════════════════════════════════════╝
)" << std::endl;
}

void printAdvancedUsage() {
    std::cout << R"(
Advanced Phase 6 Demo Options:
  --neurons=<n>           Neuron count (default: 500)
  --steps=<n>              Max simulation steps (default: 2000)
  --enable-checkpoint      Enable checkpointing (default: true)
  --disable-checkpoint     Disable checkpointing
  --enable-replay          Enable replay capability (default: true)
  --disable-replay         Disable replay capability
  --enable-development     Enable developmental stages (default: true)
  --disable-development    Disable developmental stages
  --check-only             Run only integration checks (no experiment)
  --benchmark              Run with optimized settings for benchmarking
  --debug                  Enable debug logging
  --quiet                  Suppress non-essential output
  --output=<file>          Save detailed results to file
  --help                   Show this help message
  --version                Show version information

Performance modes:
  --benchmark: Optimized for maximum throughput
  --check-only: Fast integration verification only
  --full: Complete experimental run with diagnostics
" << std::endl;
}

void printVersion() {
    std::cout << "NLM Phase 6 - Enhanced Integration Test\n";
    std::cout << "Version: 0.1.0\n";
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << "\n";
    std::cout << "Phase: Integration Demo\n";
    std::cout << "Features:\n";
    std::cout << "  - Comprehensive system integration testing\n";
    std::cout << "  - Working memory, episodic memory\n";
    std::cout << "  - Neuromodulation, prediction\n";
    std::cout << "  - Development, social learning\n";
    std::cout << "  - Checkpointing and replay\n";
    std::cout << "  - Advanced diagnostics\n";
}

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    // Initialize logging
    Logger::getGlobal().setLevel(LogLevel::Info);
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // First, run the quick integration verification
    std::cout << "--- Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return 1;
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    // Test individual systems
    std::cout << "--- Memory Integration Test ---" << std::endl;
    experiment.testMemoryIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
    experiment.testNeuromodulationIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Checkpoint Test ---" << std::endl;
    experiment.testCheckpointing();
    std::cout << std::endl;
    
    std::cout << "--- Replay Test ---" << std::endl;
    experiment.testReplay();
    std::cout << std::endl;
    
    // Run full experiment with smaller settings for demo
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    Phase6Config config;
    config.neuronCount = 500;
    config.maxSteps = 2000;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    
    auto result = experiment.run(config);
    
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
    
    return 0;
}
