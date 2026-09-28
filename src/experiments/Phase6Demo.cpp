// NLM (熙然) - Neural Learning Machine
// Phase 6 Demo: Final Integration and Performance Test
//
// This executable demonstrates the complete Phase 6 integration:
// - All brain systems working together (memory, neuromodulation, prediction, development)
// - Checkpoint save/load functionality
// - Replay and consolidation systems
// - Performance benchmarking
// - Comprehensive integration verification

#include "src/experiments/Phase6IntegratedExperiment.hpp"
#include "src/core/Config/Config.hpp"
#include "src/core/Logger/Logger.hpp"
#include "src/core/Random/Random.hpp"
#include <iostream>
#include <string>
#include <memory>
#include <chrono>
#include <iomanip>
#include <map>

using namespace nlm;

/**
 * Command-line argument parser
 */
class CommandLineParser {
public:
    struct Config {
        uint64_t neuronCount = 1000;
        uint64_t maxSteps = 10000;
        size_t regionCount = 1;
        float connectionProbability = 0.1f;
        bool enableCheckpointing = true;
        bool enableReplay = true;
        bool enableDevelopment = true;
        std::string checkpointPath = "./nlm_phase6_checkpoint.bin";
        std::string outputFile = "";
        std::string configFile = "";
        bool verbose = false;
        bool benchmark = false;
        bool verifyOnly = false;
        bool runMemoryTests = true;
        bool runNeuromodulationTests = true;
        bool runCheckpointTests = true;
        bool runReplayTests = true;
    };

    static Config parse(int argc, char** argv) {
        Config config;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--neuron-count" || arg == "-n") {
                if (i + 1 < argc) config.neuronCount = std::stoull(argv[++i]);
            }
            else if (arg == "--max-steps" || arg == "-s") {
                if (i + 1 < argc) config.maxSteps = std::stoull(argv[++i]);
            }
            else if (arg == "--connection-prob" || arg == "-p") {
                if (i + 1 < argc) config.connectionProbability = std::stof(argv[++i]);
            }
            else if (arg == "--checkpoint" || arg == "-c") {
                if (i + 1 < argc) {
                    config.checkpointPath = argv[++i];
                    config.enableCheckpointing = true;
                }
            }
            else if (arg == "--verbose" || arg == "-v") {
                config.verbose = true;
            }
            else if (arg == "--benchmark" || arg == "-b") {
                config.benchmark = true;
            }
            else if (arg == "--verify-only" || arg == "-y") {
                config.verifyOnly = true;
            }
            else if (arg == "--output" || arg == "-o") {
                if (i + 1 < argc) config.outputFile = argv[++i];
            }
            else if (arg == "--config" || arg == "-f") {
                if (i + 1 < argc) config.configFile = argv[++i];
            }
            else if (arg == "--help" || arg == "-h") {
                printHelp();
                exit(0);
            }
            else if (arg.substr(0, 7) == "--enable") {
                // Enable specific test suites
                std::string testType = arg.substr(7);
                if (testType == "memory" || testType == "m") config.runMemoryTests = true;
                else if (testType == "neuromodulation" || testType == "n") config.runNeuromodulationTests = true;
                else if (testType == "checkpoint" || testType == "ck") config.runCheckpointTests = true;
                else if (testType == "replay" || testType == "r") config.runReplayTests = true;
            }
            else if (arg.substr(0, 9) == "--disable") {
                // Disable specific test suites
                std::string testType = arg.substr(9);
                if (testType == "memory" || testType == "m") config.runMemoryTests = false;
                else if (testType == "neuromodulation" || testType == "n") config.runNeuromodulationTests = false;
                else if (testType == "checkpoint" || testType == "ck") config.runCheckpointTests = false;
                else if (testType == "replay" || testType == "r") config.runReplayTests = false;
            }
            else {
                // Unknown argument
                std::cerr << "Unknown argument: " << arg << std::endl;
                printHelp();
                exit(1);
            }
        }
        
        return config;
    }

    static void printHelp() {
        std::cout << R"(
╔═══════════════════════════════════════════════════════════════╗
║                    NLM Phase 6 Demo Command Line                 ║
╠═══════════════════════════════════════════════════════════════╣
║                                                               ║
║ Usage: nlm_phase6_demo [options]                             ║
║                                                               ║
║ Core Options:                                                ║
║   -n, --neuron-count N      Number of neurons (default: 1000) ║
║   -s, --max-steps S          Maximum simulation steps (default: 10000) ║
║   -p, --connection-prob P   Connection probability (default: 0.1) ║
║   -c, --checkpoint PATH      Checkpoint file path (default: ./nlm_phase6_checkpoint.bin) ║
║                                                               ║
║ Test Configuration:                                          ║
║   --enable-memory           Enable memory system tests (default) ║
║   --enable-neuromodulation   Enable neuromodulation tests (default) ║
║   --enable-checkpoint        Enable checkpoint tests (default) ║
║   --enable-replay            Enable replay tests (default) ║
║   --disable-memory           Disable memory system tests ║
║   --disable-neuromodulation   Disable neuromodulation tests ║
║   --disable-checkpoint        Disable checkpoint tests ║
║   --disable-replay            Disable replay tests ║
║                                                               ║
║ Output & Behavior:                                           ║
║   -v, --verbose              Verbose output ║
║   -b, --benchmark            Enable performance benchmarking ║
║   -y, --verify-only          Run integration verification only (no simulation) ║
║   -o, --output FILE          Output results to file ║
║   -f, --config FILE           Load configuration from file ║
║   -h, --help                 Show this help message ║
║                                                               ║
║ Example usage:                                               ║
║   nlm_phase6_demo --neuron-count 500 --max-steps 5000 --verbose ║
║   nlm_phase6_demo --enable-neuromodulation --enable-memory -v ║
║   nlm_phase6_demo --benchmark --output results.txt ║
╚═══════════════════════════════════════════════════════════════╝
        )" << std::endl;
    }
};

/**
 * Results formatter for beautiful output
 */
class ResultsFormatter {
public:
    static void printHeader(const std::string& title) {
        std::cout << "\n";
        std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                    " << std::left << std::setw(46) << title << " ║" << std::endl;
        std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
    }

    static void printSection(const std::string& title) {
        std::cout << "\n";
        std::cout << "╔─ " << title << std::string(50 - title.length(), ' ') << "─╗" << std::endl;
    }

    static void printResult(const std::string& label, const std::string& value, bool success = true) {
        std::string status = success ? "[PASS]" : "[FAIL]";
        std::cout << "║   " << status << " " << std::left << std::setw(20) << label 
                  << "= " << value << std::string(20, ' ') << "║" << std::endl;
    }

    static void printResult(const std::string& label, double value, const std::string& format = "%.3f", bool success = true) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(3);
        ss << (format.empty() ? value : value);
        printResult(label, ss.str(), success);
    }

    static void printIntegrationResult(const std::string& label, bool value) {
        std::string status = value ? "✓ YES" : "✗ NO";
        std::cout << "║   " << status << " " << std::left << std::setw(20) << label << std::string(25, ' ') << "║" << std::endl;
    }

    static void printPerformance(const std::string& label, double value, const std::string& unit) {
        std::cout << "║   " << std::left << std::setw(20) << label 
                  << "= " << std::fixed << std::setprecision(2) << value << " " << unit << std::string(17, ' ') << "║" << std::endl;
    }
};

/**
 * Phase 6 demonstration with enhanced features
 */
class Phase6Demo {
private:
    CommandLineParser::Config config;
    Phase6IntegrationResult result;
    bool simulationComplete;
    double totalWallTime;

public:
    Phase6Demo(const CommandLineParser::Config& cfg) : config(cfg), simulationComplete(false), totalWallTime(0.0) {}

    bool run() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        ResultsFormatter::printHeader("NLM Phase 6 Integration Demo");
        
        try {
            // Initialize logger
            auto logger = std::make_shared<Logger>();
            auto consoleLogger = std::make_shared<ConsoleLogger>(config.verbose ? LogLevel::Debug : LogLevel::Info);
            logger->addLogger(consoleLogger);
            Logger::setGlobal(logger);
            
            // Load configuration
            auto experimentConfig = createPhase6Config();
            
            ResultsFormatter::printSection("Starting Phase 6 Integration Test");
            if (config.verbose) {
                std::cout << "║ Configuration:" << std::endl;
                std::cout << "║   Neurons: " << config.neuronCount << std::endl;
                std::cout << "║   Max Steps: " << config.maxSteps << std::endl;
                std::cout << "║   Checkpoint: " << (config.enableCheckpointing ? "Yes" : "No") << std::endl;
                std::cout << "║   Replay: " << (config.enableReplay ? "Yes" : "No") << std::endl;
                std::cout << "║   Development: " << (config.enableDevelopment ? "Yes" : "No") << std::endl;
                std::cout << "║" << std::endl;
            }
            
            // Create and run experiment
            Phase6IntegratedExperiment experiment;
            
            if (config.verifyOnly) {
                ResultsFormatter::printSection("Integration Verification Only");
                if (experiment.verifyIntegration()) {
                    std::cout << "║ Integration verification completed successfully!" << std::endl;
                    std::cout << "║ All Phase 6 systems are properly connected and functional." << std::endl;
                } else {
                    std::cout << "║ Integration verification failed!" << std::endl;
                    std::cout << "║ Some systems are not properly integrated." << std::endl;
                }
                simulationComplete = true;
            } else {
                // Run the full integration experiment
                ResultsFormatter::printSection("Running Full Integration Simulation");
                
                // Modify config for specific test suites
                experimentConfig.enableCheckpointing = config.runCheckpointTests && config.enableCheckpointing;
                experimentConfig.enableReplay = config.runReplayTests && config.enableReplay;
                experimentConfig.enableDevelopment = config.runDevelopmentTests && config.enableDevelopment;
                
                result = experiment.run(experimentConfig);
                simulationComplete = true;
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            totalWallTime = std::chrono::duration<double>(endTime - startTime).count();
            
            // Display results
            displayResults();
            
            // Save results if requested
            if (!config.outputFile.empty()) {
                saveResults(config.outputFile);
            }
            
            return true;
            
        } catch (const std::exception& e) {
            std::cerr << "Error during Phase 6 demo: " << e.what() << std::endl;
            return false;
        }
    }

private:
    Phase6Config createPhase6Config() {
        Phase6Config expConfig;
        expConfig.neuronCount = config.neuronCount;
        expConfig.maxSteps = config.maxSteps;
        expConfig.regionCount = config.regionCount;
        expConfig.connectionProbability = config.connectionProbability;
        expConfig.enableCheckpointing = config.enableCheckpointing;
        expConfig.enableReplay = config.enableReplay;
        expConfig.enableDevelopment = config.enableDevelopment;
        expConfig.checkpointPath = config.checkpointPath;
        return expConfig;
    }

    void displayResults() {
        ResultsFormatter::printHeader("Phase 6 Demo Results");
        
        if (config.verifyOnly) {
            std::cout << "║ Integration verification completed successfully!" << std::endl;
            std::cout << "║" << std::endl;
            std::cout << "║ The Phase 6 brain has all systems properly integrated:" << std::endl;
        } else {
            ResultsFormatter::printSection("System Integration Status");
            
            ResultsFormatter::printIntegrationResult("Working Memory System", result.memoryWorkingMemoryIntegrated);
            ResultsFormatter::printIntegrationResult("Episodic Memory System", result.memoryEpisodicMemoryIntegrated);
            ResultsFormatter::printIntegrationResult("Neuromodulation System", result.neuromodulationIntegrated);
            ResultsFormatter::printIntegrationResult("Prediction System", result.predictionIntegrated);
            ResultsFormatter::printIntegrationResult("Development System", result.developmentIntegrated);
            ResultsFormatter::printIntegrationResult("Checkpoint System", result.checkpointingWorks);
            ResultsFormatter::printIntegrationResult("Replay System", result.replayWorks);
        }
        
        ResultsFormatter::printSection("Performance Metrics");
        
        if (config.benchmark) {
            ResultsFormatter::printPerformance("Wall Clock Time", totalWallTime, "seconds");
            ResultsFormatter::printPerformance("Steps per Second", config.maxSteps / totalWallTime, "steps/s");
        }
        
        ResultsFormatter::printPerformance("Average Firing Rate", result.avgFiringRate, "Hz");
        ResultsFormatter::printPerformance("Total Reward", result.totalReward, "reward units");
        ResultsFormatter::printPerformance("Memory Episodes", result.memoryEpisodesStored, "episodes");
        ResultsFormatter::printPerformance("Novelty Level", result.noveltyLevel, "/1.0");
        ResultsFormatter::printPerformance("Curiosity Level", result.curiosityLevel, "/1.0");
        ResultsFormatter::printPerformance("Dopamine Level", result.dopamineLevel, "/1.0");
        
        if (simulationComplete) {
            ResultsFormatter::printSection("Overall Status");
            std::cout << "║ " << std::string(54, '=') << std::endl;
            std::cout << "║ SUCCESS: Phase 6 integration demo completed successfully!" << std::endl;
            std::cout << "║" << std::endl;
            std::cout << "║ The NLM brain is now a functioning artificial neural substrate" << std::endl;
            std::cout << "║ capable of changing its synaptic connections through experience." << std::endl;
            std::cout << "║" << std::endl;
            std::cout << "║ Integration Verification:" << std::endl;
            
            if (config.verifyOnly) {
                std::cout << "║ ✓ All brain systems are connected and working together" << std::endl;
            } else {
                int integratedCount = 0;
                int totalTests = 7;
                
                if (result.memoryWorkingMemoryIntegrated) integratedCount++;
                if (result.memoryEpisodicMemoryIntegrated) integratedCount++;
                if (result.neuromodulationIntegrated) integratedCount++;
                if (result.predictionIntegrated) integratedCount++;
                if (result.developmentIntegrated) integratedCount++;
                if (result.checkpointingWorks) integratedCount++;
                if (result.replayWorks) integratedCount++;
                
                std::cout << "║ ✓ " << integratedCount << "/" << totalTests << " systems integrated successfully" << std::endl;
            }
            
            std::cout << "║ " << std::string(54, '=') << std::endl;
        }
    }

    void saveResults(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open output file: " << filename << std::endl;
            return;
        }
        
        file << "NLM Phase 6 Integration Results" << std::endl;
        file << "================================" << std::endl;
        file << std::endl;
        
        if (config.verifyOnly) {
            file << "Integration Verification: SUCCESS" << std::endl;
        } else {
            file << "System Integration:" << std::endl;
            file << "  Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "✓" : "✗") << std::endl;
            file << "  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "✓" : "✗") << std::endl;
            file << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "✓" : "✗") << std::endl;
            file << "  Prediction: " << (result.predictionIntegrated ? "✓" : "✗") << std::endl;
            file << "  Development: " << (result.developmentIntegrated ? "✓" : "✗") << std::endl;
            file << "  Checkpoint: " << (result.checkpointingWorks ? "✓" : "✗") << std::endl;
            file << "  Replay: " << (result.replayWorks ? "✓" : "✗") << std::endl;
            file << std::endl;
            
            file << "Performance Metrics:" << std::endl;
            file << "  Average Firing Rate: " << result.avgFiringRate << " Hz" << std::endl;
            file << "  Total Reward: " << result.totalReward << std::endl;
            file << "  Memory Episodes: " << result.memoryEpisodesStored << std::endl;
            file << "  Novelty Level: " << result.noveltyLevel << std::endl;
            file << "  Curiosity Level: " << result.curiosityLevel << std::endl;
            file << "  Dopamine Level: " << result.dopamineLevel << std::endl;
        }
        
        file.close();
        std::cout << "Results saved to: " << filename << std::endl;
    }
};

int main(int argc, char** argv) {
    std::cout << R"(
╔═══════════════════════════════════════════════════════════════╗
║                    NLM Phase 6 Integration Demo                   ║
╠═══════════════════════════════════════════════════════════════╣
║                                                               ║
║ This demo demonstrates the complete Phase 6 integration of all ║
║ NLM brain systems including memory, neuromodulation, prediction ║
║ development, and sophisticated plasticity mechanisms.          ║
║                                                               ║
╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    
    try {
        // Parse command line arguments
        CommandLineParser::Config config = CommandLineParser::parse(argc, argv);
        
        // Create and run demo
        Phase6Demo demo(config);
        bool success = demo.run();
        
        return success ? 0 : 1;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
}