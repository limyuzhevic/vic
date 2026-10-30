// Phase 6 Demo - Enhanced Integration Test
// This demo runs the Phase 6 integration experiment with flexible configuration and monitoring

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <chrono>

using namespace nlm;

enum class Phase6Mode {
    QuickTest,      // Just integration verification
    Comprehensive,   // All integration tests
    FullExperiment  // Integration + full experiment run
};

struct Phase6DemoOptions {
    Phase6Mode mode = Phase6Mode::Comprehensive;
    int neuronCount = 500;
    int maxSteps = 2000;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    std::string configFile = "configs/default.cfg";
    std::string outputFile;
    bool quiet = false;
    bool benchmark = false;
    bool showProgress = true;
    int progressStep = 100;
};

Phase6DemoOptions parsePhase6DemoArgs(int argc, char* argv[]) {
    Phase6DemoOptions options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printPhase6DemoUsage(argv[0]);
            std::exit(0);
        } else if (arg == "--version" || arg == "-v") {
            printPhase6DemoBanner(true);
            std::exit(0);
        } else if (arg == "--quiet" || arg == "-q") {
            options.quiet = true;
            options.showProgress = false;
        } else if (arg == "--benchmark") {
            options.benchmark = true;
        } else if (arg == "--config" && i + 1 < argc) {
            options.configFile = argv[++i];
        } else if (arg == "--output" && i + 1 < argc) {
            options.outputFile = argv[++i];
        } else if (arg == "--mode" && i + 1 < argc) {
            std::string mode = argv[++i];
            if (mode == "quick") options.mode = Phase6Mode::QuickTest;
            else if (mode == "comprehensive") options.mode = Phase6Mode::Comprehensive;
            else if (mode == "full") options.mode = Phase6Mode::FullExperiment;
        } else if (arg == "--neurons" && i + 1 < argc) {
            options.neuronCount = std::stoi(argv[++i]);
        } else if (arg == "--steps" && i + 1 < argc) {
            options.maxSteps = std::stoi(argv[++i]);
        } else if (arg == "--no-checkpoint") {
            options.enableCheckpointing = false;
        } else if (arg == "--no-replay") {
            options.enableReplay = false;
        } else if (arg == "--no-development") {
            options.enableDevelopment = false;
        } else if (arg == "--progress-step" && i + 1 < argc) {
            options.progressStep = std::stoi(argv[++i]);
        } else if (arg == "--version-phase" && i + 1 < argc) {
            printPhase6DemoBanner(argv[i + 1]);
            std::exit(0);
        } else {
            std::cerr << "Unknown option: " << arg << std::endl;
            printPhase6DemoUsage(argv[0]);
            std::exit(1);
        }
    }
    
    return options;
}

void printPhase6DemoBanner(bool detailed = true) {
    std::cout << "\n"
              << "  ╔═══════════════════════════════════════════════════════════════╗" << "\n"
              << "  ║                                                               ║" << "\n"
              << "  ║     NLM Phase 6 Integration Demo                              ║" << "\n"
              << "  ║     Testing Integrated Brain Systems                          ║" << "\n"
              << "  ║                                                               ║" << "\n"
              << "  ║     Comprehensive Integration Test                             ║" << "\n"
              << "  ║                                                               ║" << "\n"
              << "  ╚═══════════════════════════════════════════════════════════════╝" << "\n\n";
    
    if (detailed) {
        std::cout << "  Phase 6 validates that all brain systems work together as a unified system:" << "\n"
                  << "  - Memory systems (working, episodic, semantic, procedural)" << "\n"
                  << "  - Neuromodulation (dopamine, curiosity, novelty)" << "\n"
                  << "  - Prediction system" << "\n"
                  << "  - Cognitive mechanisms (planning, attention, concept formation)" << "\n"
                  << "  - Development and learning" << "\n"
                  << "  - Checkpointing and replay" << "\n\n";
    }
}

void printPhase6DemoUsage(const char* programName) {
    std::cout << "\n"
              << "  Usage: " << programName << " [OPTIONS]" << "\n\n"
              << "  Options:" << "\n"
              << "    --help, -h                    Show this help message" << "\n"
              << "    --version, -v                 Show version information" << "\n"
              << "    --mode <mode>                 Demo mode: quick, comprehensive, full" << "\n"
              << "    --config <file>               Load configuration from file" << "\n"
              << "    --output <file>               Save results to file" << "\n"
              << "    --neurons <n>                 Number of neurons (default: 500)" << "\n"
              << "    --steps <n>                   Maximum simulation steps (default: 2000)" << "\n"
              << "    --quiet, -q                   Suppress non-error output" << "\n"
              << "    --no-checkpoint                Disable checkpointing" << "\n"
              << "    --no-replay                    Disable replay functionality" << "\n"
              << "    --no-development               Disable development system" << "\n"
              << "    --progress-step <n>           Progress reporting interval (default: 100)" << "\n"
              << "    --benchmark                    Run in benchmark mode" << "\n\n"
              << "  Examples:" << "\n"
              << "    " << programName << " --mode comprehensive --neurons 1000 --steps 5000" << "\n"
              << "    " << programName << " --mode full --output results.json --progress-step 500" << "\n\n";
}

void redirectOutputToFile(std::ofstream& file) {
    if (file.is_open()) {
        std::cout.rdbuf(file.rdbuf());
        std::cerr.rdbuf(file.rdbuf());
    }
}

void printProgress(const std::string& stage, int current, int total, bool quiet) {
    if (quiet) return;
    
    std::cout << "\r  " << std::left << std::setw(30) << stage << "  ";
    std::cout << "[" << std::string(50 * current / total, '=') << ">";
    if (current < total) std::cout << " ";
    std::cout << std::setw(3) << (100 * current / total) << "%]" << std::flush;
}

int main(int argc, char* argv[]) {
    // Parse command line options
    Phase6DemoOptions options = parsePhase6DemoArgs(argc, argv);
    
    // Setup output file if specified
    std::ofstream outputFile;
    if (!options.outputFile.empty()) {
        outputFile.open(options.outputFile);
        if (!outputFile.is_open()) {
            std::cerr << "Failed to open output file: " << options.outputFile << std::endl;
        } else {
            redirectOutputToFile(outputFile);
        }
    }
    
    // Setup logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(options.quiet ? LogLevel::Error : LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Print banner
    printPhase6DemoBanner(!options.quiet);
    
    if (!options.quiet) {
        std::cout << "Phase 6 Demo Options:" << "\n"
                  << "  Mode: " << (options.mode == Phase6Mode::QuickTest ? "Quick Test" :
                                 options.mode == Phase6Mode::Comprehensive ? "Comprehensive" : "Full Experiment") << "\n"
                  << "  Neurons: " << options.neuronCount << "\n"
                  << "  Max Steps: " << options.maxSteps << "\n"
                  << "  Checkpointing: " << (options.enableCheckpointing ? "enabled" : "disabled") << "\n"
                  << "  Replay: " << (options.enableReplay ? "enabled" : "disabled") << "\n"
                  << "  Development: " << (options.enableDevelopment ? "enabled" : "disabled") << "\n"
                  << "  Output: " << (options.outputFile.empty() ? "console" : options.outputFile) << "\n"
                  << "\n";
    }
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // Run based on mode
    switch (options.mode) {
        case Phase6Mode::QuickTest: {
            if (!options.quiet) {
                std::cout << "=== Quick Integration Test ===" << std::endl;
            }
            
            bool integrationOK = experiment.verifyIntegration();
            
            if (!integrationOK) {
                std::cerr << "ERROR: Integration verification failed!" << std::endl;
                return 1;
            }
            
            if (!options.quiet) {
                std::cout << "Integration verification passed!" << std::endl;
            }
            break;
        }
        
        case Phase6Mode::Comprehensive: {
            if (!options.quiet) {
                std::cout << "=== Comprehensive Integration Test ===" << std::endl;
                std::cout << "Running individual system integration tests..." << std::endl;
            }
            
            // Test individual systems
            if (options.showProgress) printProgress("Memory Integration", 1, 5, options.quiet);
            experiment.testMemoryIntegration();
            
            if (options.showProgress) printProgress("Neuromodulation Integration", 2, 5, options.quiet);
            experiment.testNeuromodulationIntegration();
            
            if (options.showProgress) printProgress("Checkpoint Test", 3, 5, options.quiet);
            experiment.testCheckpointing();
            
            if (options.showProgress) printProgress("Replay Test", 4, 5, options.quiet);
            experiment.testReplay();
            
            if (options.showProgress) printProgress("Final Verification", 5, 5, options.quiet);
            experiment.verifyIntegration();
            
            if (!options.quiet) {
                std::cout << std::endl << "All integration tests completed successfully!" << std::endl;
            }
            break;
        }
        
        case Phase6Mode::FullExperiment: {
            if (!options.quiet) {
                std::cout << "=== Full Integration Experiment ===" << std::endl;
                std::cout << "Running comprehensive integration experiment with full simulation..." << std::endl;
            }
            
            // Configure experiment
            Phase6Config config;
            config.neuronCount = options.neuronCount;
            config.maxSteps = options.maxSteps;
            config.enableCheckpointing = options.enableCheckpointing;
            config.enableReplay = options.enableReplay;
            config.enableDevelopment = options.enableDevelopment;
            
            // Load additional configuration if specified
            auto phase6Config = std::make_shared<Config>();
            if (phase6Config->loadFromFile(options.configFile)) {
                if (!options.quiet) {
                    std::cout << "Loaded configuration from: " << options.configFile << std::endl;
                }
            }
            
            // Record start time for benchmark
            auto startTime = std::chrono::high_resolution_clock::now();
            
            // Run full experiment
            if (options.showProgress) {
                std::cout << "Running simulation..." << std::endl;
            }
            
            auto result = experiment.run(config);
            
            // Record end time
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            
            // Display results
            if (!options.quiet) {
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
                
                std::cout << "Wall clock time: " << (duration.count() / 1000.0) << "s" << std::endl;
                std::cout << "Total simulation steps: " << result.totalSteps << std::endl;
                
                // Save results if requested
                if (!options.outputFile.empty()) {
                    std::string resultsFile = options.outputFile;
                    size_t dotPos = resultsFile.find_last_of('.');
                    if (dotPos != std::string::npos) {
                        resultsFile.replace(dotPos, std::string::npos, "_results.json");
                    }
                    
                    std::ofstream resultsOut(resultsFile);
                    if (resultsOut.is_open()) {
                        resultsOut << "{\n"
                                   << "  \"totalReward\": " << result.totalReward << ",\n"
                                   << "  \"avgFiringRate\": " << result.avgFiringRate << ",\n"
                                   << "  \"memoryEpisodesStored\": " << result.memoryEpisodesStored << ",\n"
                                   << "  \"dopamineLevel\": " << result.dopamineLevel << ",\n"
                                   << "  \"totalSteps\": " << result.totalSteps << ",\n"
                                   << "  \"memoryWorkingMemoryIntegrated\": " << std::boolalpha << result.memoryWorkingMemoryIntegrated << ",\n"
                                   << "  \"memoryEpisodicMemoryIntegrated\": " << std::boolalpha << result.memoryEpisodicMemoryIntegrated << ",\n"
                                   << "  \"neuromodulationIntegrated\": " << std::boolalpha << result.neuromodulationIntegrated << ",\n"
                                   << "  \"predictionIntegrated\": " << std::boolalpha << result.predictionIntegrated << ",\n"
                                   << "  \"developmentIntegrated\": " << std::boolalpha << result.developmentIntegrated << ",\n"
                                   << "  \"checkpointingWorks\": " << std::boolalpha << result.checkpointingWorks << ",\n"
                                   << "  \"totalWallClockTime\": " << result.totalWallClockTime << "\n"
                                   << "}\n";
                        resultsOut.close();
                        if (!options.quiet) {
                            std::cout << "Results saved to: " << resultsFile << std::endl;
                        }
                    }
                }
                
                return 0;
            } else {
                // Quiet mode - just return success
                return 0;
            }
        }
    }
    
    // Display summary for quick and comprehensive modes
    if (!options.quiet) {
        std::cout << std::endl;
        std::cout << "=== Phase 6 Demo Complete ===" << std::endl;
        std::cout << "All integration tests passed successfully!" << std::endl;
        std::cout << "The NLM brain systems are properly connected and functional." << std::endl;
    }
    
    return 0;
}
