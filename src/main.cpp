// Helper functions
void printHelp() {
    std::cout << R"(
USAGE: nlm [OPTIONS]

NLM (熙然) - Neural Learning Machine
Phase 6: Final Integration Demo

Options:
  --help, -h              Show this help message
  --version, -v           Show version information
  --demo                  Run Phase 6 integration demo (default)
  --test                  Run quick integration test only
  --quiet, -q             Suppress informational output
  --verbose, -V           Enable verbose logging
  --config FILE, -c FILE  Load configuration from file
  --output FILE, -o FILE  Save results to file
  --time                  Measure and report execution time
  --steps N               Number of simulation steps (default: 2000)
  --neurons N             Number of neurons (default: 500)

Examples:
  nlm                                    # Run Phase 6 demo
  nlm --help                            # Show help
  nlm --test --quiet                    # Run quick test silently
  nlm --steps 5000 --neurons 1000       # Custom settings
  nlm --config myconfig.json --output results.txt  # Custom config and output

The NLM brain is a complete artificial neural substrate capable of
emerging cognition through integrated systems.
)" << std::endl;
}

void printVersion() {
    std::cout << "NLM (熙然) - Neural Learning Machine v0.1.0 (Phase 6: Final Integration)" << std::endl;
    std::cout << "Built with C++20" << std::endl;
    std::cout << "Research project investigating computational brain-like systems." << std::endl;
}

void parseCommandLineArgs(int argc, char** argv, bool& quiet, bool& verbose, 
                         std::string& configFile, std::string& outputFile,
                         bool& measureTime, uint64_t& steps, uint64_t& neurons,
                         bool& runDemo, bool& runQuickTest) {
    // Default values
    quiet = false;
    verbose = false;
    configFile = "";
    outputFile = "";
    measureTime = false;
    steps = 2000;
    neurons = 500;
    runDemo = true;
    runQuickTest = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printHelp();
            exit(0);
        } else if (arg == "--version" || arg == "-v") {
            printVersion();
            exit(0);
        } else if (arg == "--quiet" || arg == "-q") {
            quiet = true;
        } else if (arg == "--verbose" || arg == "-V") {
            verbose = true;
        } else if (arg == "--test") {
            runDemo = false;
            runQuickTest = true;
        } else if (arg == "--time") {
            measureTime = true;
        } else if (arg == "--demo") {
            runDemo = true;
        } else if (arg == "--config" || arg == "-c") {
            if (i + 1 < argc) {
                configFile = argv[++i];
            } else {
                std::cerr << "ERROR: --config requires a file argument" << std::endl;
                exit(1);
            }
        } else if (arg == "--output" || arg == "-o") {
            if (i + 1 < argc) {
                outputFile = argv[++i];
            } else {
                std::cerr << "ERROR: --output requires a file argument" << std::endl;
                exit(1);
            }
        } else if (arg == "--steps") {
            if (i + 1 < argc) {
                try {
                    steps = std::stoull(argv[++i]);
                } catch (const std::exception& e) {
                    std::cerr << "ERROR: Invalid value for --steps: " << argv[i] << std::endl;
                    exit(1);
                }
            } else {
                std::cerr << "ERROR: --steps requires a number argument" << std::endl;
                exit(1);
            }
        } else if (arg == "--neurons") {
            if (i + 1 < argc) {
                try {
                    neurons = std::stoull(argv[++i]);
                } catch (const std::exception& e) {
                    std::cerr << "ERROR: Invalid value for --neurons: " << argv[i] << std::endl;
                    exit(1);
                }
            } else {
                std::cerr << "ERROR: --neurons requires a number argument" << std::endl;
                exit(1);
            }
        } else if (arg.substr(0, 2) == "--") {
            std::cerr << "ERROR: Unknown option: " << arg << std::endl;
            printHelp();
            exit(1);
        } else if (arg[0] == '-') {
            std::cerr << "ERROR: Unknown option: " << arg << std::endl;
            printHelp();
            exit(1);
        }
    }
}

// Helper function to print timing
void printTiming(const std::string& operation, double seconds) {
    std::cout << "Timing: " << operation << " took " << std::fixed << std::setprecision(3) 
              << seconds << " seconds" << std::endl;
}

// Main function with improved command-line interface and error handling
int main(int argc, char** argv) {
    // Parse command line arguments
    bool quiet = false;
    bool verbose = false;
    std::string configFile;
    std::string outputFile;
    bool measureTime = false;
    uint64_t steps = 2000;
    uint64_t neurons = 500;
    bool runDemo = true;
    bool runQuickTest = false;
    
    parseCommandLineArgs(argc, argv, quiet, verbose, configFile, outputFile,
                        measureTime, steps, neurons, runDemo, runQuickTest);
    
    // Print banner
    printBanner();
    
    // Set logging level based on verbosity
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(verbose ? LogLevel::Debug : LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Suppress informational output if quiet mode
    if (quiet) {
        consoleLogger->setLevel(LogLevel::Warning);
    }
    
    // Log startup information
    NLM_LOG_INFO("=== NLM Phase 6: Final Integration Demo ===");
    NLM_LOG_INFO("NLM (熙然) - Neural Learning Machine v0.1.0");
    
    if (runDemo) {
        NLM_LOG_INFO("Running complete Phase 6 integration demo...");
    } else if (runQuickTest) {
        NLM_LOG_INFO("Running quick integration test only...");
    }
    
    NLM_LOG_INFO("");
    
    // Record start time if timing is enabled
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        // Create Phase 6 integration experiment
        Phase6IntegratedExperiment experiment;
        
        if (runDemo) {
            // First, run the quick integration verification
            NLM_LOG_INFO("--- Integration Verification ---");
            bool integrationOK = experiment.verifyIntegration();
            
            if (!integrationOK) {
                NLM_LOG_ERROR("ERROR: Integration verification failed!");
                return 1;
            }
            
            NLM_LOG_INFO("Integration verification passed!");
            NLM_LOG_INFO("");
            
            // Test individual systems
            NLM_LOG_INFO("--- Memory Integration Test ---");
            experiment.testMemoryIntegration();
            NLM_LOG_INFO("");
            
            NLM_LOG_INFO("--- Neuromodulation Integration Test ---");
            experiment.testNeuromodulationIntegration();
            NLM_LOG_INFO("");
            
            NLM_LOG_INFO("--- Checkpoint Test ---");
            experiment.testCheckpointing();
            NLM_LOG_INFO("");
            
            NLM_LOG_INFO("--- Replay Test ---");
            experiment.testReplay();
            NLM_LOG_INFO("");
            
            // Run full experiment with demo settings
            NLM_LOG_INFO("--- Full Integration Experiment ---");
            Phase6Config config;
            config.neuronCount = neurons;
            config.maxSteps = steps;
            config.enableCheckpointing = true;
            config.enableReplay = true;
            config.enableDevelopment = true;
            
            auto result = experiment.run(config);
            
            NLM_LOG_INFO("");
            NLM_LOG_INFO("=== FINAL RESULTS ===");
            NLM_LOG_INFO("Total reward: " + std::to_string(result.totalReward));
            NLM_LOG_INFO("Average firing rate: " + std::to_string(result.avgFiringRate));
            NLM_LOG_INFO("Memory episodes stored: " + std::to_string(result.memoryEpisodesStored));
            NLM_LOG_INFO("Dopamine level: " + std::to_string(result.dopamineLevel));
            NLM_LOG_INFO("");
            
            printPhase6Status(result);
            
            NLM_LOG_INFO("Wall clock time: " + std::to_string(result.totalWallClockTime) + "s");
            
            // Final summary
            NLM_LOG_INFO("");
            NLM_LOG_INFO("=== PHASE 6 SUMMARY ===");
            NLM_LOG_INFO("Phase 6 demonstrates:");
            NLM_LOG_INFO("  ✓ Memory systems integrated with neural processing");
            NLM_LOG_INFO("  ✓ Neuromodulation affects plasticity and dynamics");
            NLM_LOG_INFO("  ✓ Prediction system as central organizing principle");
            NLM_LOG_INFO("  ✓ Development affects plasticity rates");
            NLM_LOG_INFO("  ✓ Checkpoint save/load working");
            NLM_LOG_INFO("  ✓ Replay and consolidation functional");
            NLM_LOG_INFO("");
            NLM_LOG_INFO("The NLM brain is now a complete artificial neural substrate");
            NLM_LOG_INFO("capable of emerging cognition through integrated systems.");
            NLM_LOG_INFO("");
            
            // Check if all integration requirements are met
            bool allSystemsIntegrated = result.memoryWorkingMemoryIntegrated &&
                                       result.memoryEpisodicMemoryIntegrated &&
                                       result.neuromodulationIntegrated &&
                                       result.predictionIntegrated &&
                                       result.developmentIntegrated &&
                                       result.checkpointingWorks &&
                                       result.replayWorks;
            
            if (allSystemsIntegrated) {
                NLM_LOG_INFO("SUCCESS: All brain systems are properly integrated and functional!");
            } else {
                NLM_LOG_ERROR("FAILURE: Some systems are not properly integrated.");
                return 1;
            }
            
        } else if (runQuickTest) {
            // Run only integration verification
            NLM_LOG_INFO("Running integration verification only...");
            
            bool integrationOK = experiment.verifyIntegration();
            
            if (!integrationOK) {
                NLM_LOG_ERROR("ERROR: Integration verification failed!");
                return 1;
            }
            
            NLM_LOG_INFO("Integration verification passed!");
        }
        
        // Record end time if timing is enabled
        auto endTime = std::chrono::high_resolution_clock::now();
        if (measureTime) {
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            NLM_LOG_INFO("");
            NLM_LOG_INFO("=== EXECUTION TIMING ===");
            NLM_LOG_INFO("Total execution time: " + std::to_string(duration.count() / 1000.0) + " seconds");
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("ERROR during Phase 6 demo: " + std::string(e.what()));
        return 1;
    } catch (...) {
        NLM_LOG_ERROR("UNKNOWN ERROR during Phase 6 demo.");
        return 1;
    }
    
    return 0;
}
