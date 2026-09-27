/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>

using namespace nlm;

class Phase6Demo {
public:
    Phase6Demo() : experiment(nullptr), startTime(0.0) {}
    ~Phase6Demo() {
        if (experiment) {
            delete experiment;
            experiment = nullptr;
        }
    }
    
    bool run(int argc, char* argv[]) {
        startTime = getCurrentTime();
        
        // Parse command line arguments
        Phase6Config config = parseArguments(argc, argv);
        
        // Initialize logging
        initializeLogging(config.logLevel);
        
        std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
        std::cout << "Testing the integrated artificial brain..." << std::endl;
        std::cout << "Configuration: " << config.neuronCount << " neurons, " 
                  << config.maxSteps << " steps" << std::endl;
        std::cout << std::endl;
        
        try {
            // Create experiment
            experiment = new Phase6IntegratedExperiment();
            
            // Run integration verification
            if (!runIntegrationVerification()) {
                return false;
            }
            
            // Run individual system tests
            if (!runSystemTests()) {
                return false;
            }
            
            // Run full integration experiment
            if (!runFullExperiment(config)) {
                return false;
            }
            
            // Display final results
            displayFinalResults();
            
            return true;
            
        } catch (const std::exception& e) {
            std::cerr << "ERROR: Demo failed with exception: " << e.what() << std::endl;
            return false;
        } catch (...) {
            std::cerr << "ERROR: Demo failed with unknown exception" << std::endl;
            return false;
        }
    }
    
private:
    double getCurrentTime() {
        auto now = std::chrono::high_resolution_clock::now();
        auto epoch = now.time_since_epoch();
        return std::chrono::duration<double>(epoch).count();
    }
    
    Phase6Config parseArguments(int argc, char* argv[]) {
        Phase6Config config;
        
        // Set defaults
        config.neuronCount = 500;
        config.maxSteps = 2000;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        config.logLevel = Logger::Level::Info;
        config.checkpointPath = "/tmp/nlm_checkpoint_demo.bin";
        config.regionCount = 1;
        config.connectionProbability = 0.15f;
        
        // Parse command line arguments
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                printHelp();
                exit(0);
            } else if (arg.find("--neurons=") == 0) {
                config.neuronCount = std::stoi(arg.substr(10));
            } else if (arg.find("--steps=") == 0) {
                config.maxSteps = std::stoll(arg.substr(9));
            } else if (arg.find("--checkpoint=") == 0) {
                config.checkpointPath = arg.substr(13);
            } else if (arg == "--no-checkpoint") {
                config.enableCheckpointing = false;
            } else if (arg == "--no-replay") {
                config.enableReplay = false;
            } else if (arg == "--no-development") {
                config.enableDevelopment = false;
            } else if (arg.find("--log-level=") == 0) {
                std::string level = arg.substr(12);
                if (level == "debug") config.logLevel = Logger::Level::Debug;
                else if (level == "info") config.logLevel = Logger::Level::Info;
                else if (level == "warning") config.logLevel = Logger::Level::Warning;
                else if (level == "error") config.logLevel = Logger::Level::Error;
            }
        }
        
        return config;
    }
    
    void printHelp() {
        std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
        std::cout << "Usage: ./nlm_phase6_demo [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --help, -h                    Show this help message" << std::endl;
        std::cout << "  --neurons=<N>                 Number of neurons (default: 500)" << std::endl;
        std::cout << "  --steps=<N>                   Number of simulation steps (default: 2000)" << std::endl;
        std::cout << "  --checkpoint=<path>           Checkpoint file path (default: /tmp/nlm_checkpoint_demo.bin)" << std::endl;
        std::cout << "  --no-checkpoint               Disable checkpointing" << std::endl;
        std::cout << "  --no-replay                   Disable replay" << std::endl;
        std::cout << "  --no-development              Disable development" << std::endl;
        std::cout << "  --log-level=<level>           Logging level: debug, info, warning, error (default: info)" << std::endl;
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  ./nlm_phase6_demo --neurons=1000 --steps=5000" << std::endl;
        std::cout << "  ./nlm_phase6_demo --no-checkpoint --log-level=debug" << std::endl;
    }
    
    void initializeLogging(Logger::Level level) {
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(level);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        NLM_LOG_INFO("Demo initialized with log level: " + levelToString(level));
    }
    
    std::string levelToString(Logger::Level level) {
        switch (level) {
            case Logger::Level::Debug: return "Debug";
            case Logger::Level::Info: return "Info";
            case Logger::Level::Warning: return "Warning";
            case Logger::Level::Error: return "Error";
            case Logger::Level::Critical: return "Critical";
            default: return "Unknown";
        }
    }
    
    bool runIntegrationVerification() {
        std::cout << "--- Integration Verification ---" << std::endl;
        
        try {
            bool integrationOK = experiment->verifyIntegration();
            
            std::cout << std::endl;
            if (!integrationOK) {
                std::cerr << "ERROR: Integration verification failed!" << std::endl;
                return false;
            }
            
            std::cout << "Integration verification passed!" << std::endl << std::endl;
            return true;
            
        } catch (const std::exception& e) {
            std::cerr << "ERROR: Integration verification failed with exception: " << e.what() << std::endl;
            return false;
        }
    }
    
    bool runSystemTests() {
        bool success = true;
        
        std::cout << "--- System Integration Tests ---" << std::endl;
        
        // Test memory integration
        std::cout << "Memory Integration Test:" << std::endl;
        try {
            if (experiment->testMemoryIntegration()) {
                std::cout << "  ✓ PASSED" << std::endl;
            } else {
                std::cout << "  ✗ FAILED" << std::endl;
                success = false;
            }
        } catch (const std::exception& e) {
            std::cout << "  ✗ EXCEPTION: " << e.what() << std::endl;
            success = false;
        }
        std::cout << std::endl;
        
        // Test neuromodulation integration
        std::cout << "Neuromodulation Integration Test:" << std::endl;
        try {
            if (experiment->testNeuromodulationIntegration()) {
                std::cout << "  ✓ PASSED" << std::endl;
            } else {
                std::cout << "  ✗ FAILED" << std::endl;
                success = false;
            }
        } catch (const std::exception& e) {
            std::cout << "  ✗ EXCEPTION: " << e.what() << std::endl;
            success = false;
        }
        std::cout << std::endl;
        
        // Test checkpointing
        std::cout << "Checkpoint Test:" << std::endl;
        try {
            if (experiment->testCheckpointing()) {
                std::cout << "  ✓ PASSED" << std::endl;
            } else {
                std::cout << "  ✗ FAILED" << std::endl;
                success = false;
            }
        } catch (const std::exception& e) {
            std::cout << "  ✗ EXCEPTION: " << e.what() << std::endl;
            success = false;
        }
        std::cout << std::endl;
        
        // Test replay
        std::cout << "Replay Test:" << std::endl;
        try {
            if (experiment->testReplay()) {
                std::cout << "  ✓ PASSED" << std::endl;
            } else {
                std::cout << "  ✗ FAILED" << std::endl;
                success = false;
            }
        } catch (const std::exception& e) {
            std::cout << "  ✗ EXCEPTION: " << e.what() << std::endl;
            success = false;
        }
        std::cout << std::endl;
        
        return success;
    }
    
    bool runFullExperiment(const Phase6Config& config) {
        std::cout << "--- Full Integration Experiment ---" << std::endl;
        
        try {
            auto result = experiment->run(config);
            
            // Store result for later display
            currentResult = result;
            
            double elapsedTime = getCurrentTime() - startTime;
            std::cout << "Experiment completed in " << std::fixed << std::setprecision(2)
                      << elapsedTime << "s" << std::endl;
            
            return true;
            
        } catch (const std::exception& e) {
            std::cerr << "ERROR: Full experiment failed with exception: " << e.what() << std::endl;
            return false;
        }
    }
    
    void displayFinalResults() {
        std::cout << std::endl;
        std::cout << "=== FINAL RESULTS ===" << std::endl;
        
        if (currentResult.startTime > 0) {
            char timeStr[100];
            time_t start = currentResult.startTime;
            time_t end = currentResult.endTime;
            strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", localtime(&start));
            std::cout << "Start time: " << timeStr << std::endl;
            
            strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", localtime(&end));
            std::cout << "End time: " << timeStr << std::endl;
        }
        
        std::cout << std::fixed << std::setprecision(4);
        std::cout << "Total reward: " << currentResult.totalReward << std::endl;
        std::cout << "Avg firing rate: " << currentResult.avgFiringRate << std::endl;
        std::cout << "Episodes stored: " << currentResult.memoryEpisodesStored << std::endl;
        std::cout << "Novelty level: " << currentResult.noveltyLevel << std::endl;
        std::cout << "Curiosity level: " << currentResult.curiosityLevel << std::endl;
        std::cout << "Dopamine level: " << currentResult.dopamineLevel << std::endl;
        std::cout << std::endl;
        
        std::cout << "=== INTEGRATION STATUS ===" << std::endl;
        std::cout << "Working Memory: " << (currentResult.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Episodic Memory: " << (currentResult.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Neuromodulation: " << (currentResult.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Prediction: " << (currentResult.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Development: " << (currentResult.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Checkpointing: " << (currentResult.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
        std::cout << std::endl;
        
        std::cout << std::defaultfloat;
        std::cout << "Wall clock time: " << currentResult.totalWallClockTime << "s" << std::endl;
        
        // Display experiment summary
        std::cout << std::endl;
        std::cout << "=== EXPERIMENT SUMMARY ===" << std::endl;
        
        int integratedSystems = 0;
        if (currentResult.memoryWorkingMemoryIntegrated) integratedSystems++;
        if (currentResult.memoryEpisodicMemoryIntegrated) integratedSystems++;
        if (currentResult.neuromodulationIntegrated) integratedSystems++;
        if (currentResult.predictionIntegrated) integratedSystems++;
        if (currentResult.developmentIntegrated) integratedSystems++;
        
        std::cout << "Integrated systems: " << integratedSystems << "/5" << std::endl;
        std::cout << "Checkpointing functional: " << (currentResult.checkpointingWorks ? "Yes" : "No") << std::endl;
        std::cout << "Learning achieved: " << (currentResult.totalReward > 0.0f ? "Yes" : "No") << std::endl;
        
        if (integratedSystems >= 4 && currentResult.checkpointingWorks) {
            std::cout << "🎉 SUCCESS: Phase 6 integration experiment completed successfully!" << std::endl;
        } else {
            std::cout << "⚠️  WARNING: Phase 6 integration completed with some issues." << std::endl;
        }
    }
    
    Phase6IntegratedExperiment* experiment;
    Phase6IntegrationResult currentResult;
    double startTime;
};
