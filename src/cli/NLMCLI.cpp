#include "cli/NLMCLI.hpp"

void NLMCLI::printHelp() {
    std::cout << "NLM (熙然) - Neural Learning Machine CLI" << std::endl;
    std::cout << "=====================================" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Usage: nlm-cli [command] [options]" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Available commands:" << std::endl;
    std::cout << "  run      Run a simulation with specified configuration" << std::endl;
    std::cout << "  config    Configuration management (list, set, get, save)" << std::endl;
    std::cout << "  monitor   Performance monitoring (start, stop, report)" << std::endl;
    std::cout << "  profile    Performance profiling (generate profile data)" << std::endl;
    std::cout << "  checkpoint Checkpoint management (save, load, list)" << std::endl;
    std::cout << "  debug      Debug utilities (memory, threads, validation)" << std::endl;
    std::cout << "  help      Show this help message" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm-cli run --config configs/experiment.cfg" << std::endl;
    std::cout << "  nlm-cli config set random_seed 12345" << std::endl;
    std::cout << "  nlm-cli monitor start --duration 60 --interval 0.1" << std::endl;
    std::cout << "  nlm-cli profile generate --config configs/fast.cfg --steps 1000" << std::endl;
    std::cout << "  nlm-cli checkpoint save --step 5000 --name my_checkpoint" << std::endl;
}

bool NLMCLI::parseCommandLine(int argc, char** argv) {
    if (argc < 2) {
        printHelp();
        return true;
    }

    std::string command = argv[1];

    if (command == "help") {
        printHelp();
        return true;
    } else if (command == "run") {
        return handleRunCommand(argc - 1, &argv[1]);
    } else if (command == "config") {
        return handleConfigCommand(argc - 1, &argv[1]);
    } else if (command == "monitor") {
        return handleMonitorCommand(argc - 1, &argv[1]);
    } else if (command == "profile") {
        return handleProfileCommand(argc - 1, &argv[1]);
    } else if (command == "checkpoint") {
        return handleCheckpointCommand(argc - 1, &argv[1]);
    } else if (command == "debug") {
        return handleDebugCommand(argc - 1, &argv[1]);
    } else {
        std::cerr << "Unknown command: " << command << std::endl;
        std::cerr << "Use 'nlm-cli help' for available commands." << std::endl;
        return false;
    }
}

int main(int argc, char** argv) {
    NLMCLI cli;
    if (cli.parseCommandLine(argc, argv)) {
        return 0;
    } else {
        return 1;
    }
}

bool NLMCLI::parseCommandLine(int argc, char** argv) {
    if (argc < 2) {
        printHelp();
        return true;
    }

    std::string command = argv[1];

    if (command == "help") {
        printHelp();
        return true;
    } else if (command == "run") {
        return handleRunCommand(argc - 1, &argv[1]);
    } else if (command == "config") {
        return handleConfigCommand(argc - 1, &argv[1]);
    } else if (command == "monitor") {
        return handleMonitorCommand(argc - 1, &argv[1]);
    } else if (command == "profile") {
        return handleProfileCommand(argc - 1, &argv[1]);
    } else if (command == "checkpoint") {
        return handleCheckpointCommand(argc - 1, &argv[1]);
    } else if (command == "debug") {
        return handleDebugCommand(argc - 1, &argv[1]);
    } else {
        std::cerr << "Unknown command: " << command << std::endl;
        std::cerr << "Use 'nlm help' for available commands." << std::endl;
        return false;
    }
}

bool NLMCLI::handleRunCommand(int argc, char** argv) {
    std::string configFile = "configs/default.cfg";
    bool enablePerformance = true;
    bool verbose = false;
    bool enableMonitoring = false;
    bool runPhase1 = false;
    bool runPhase2 = true;
    bool runPhase3 = false;
    bool runPhase4 = false;
    bool runPhase5 = false;
    bool runPhase6 = false;
    bool useOptimizedBrain = true;
    bool enableMultithreading = false;
    bool enableSIMD = true;
    bool enableEventDriven = true;
    bool enableSparseConnectivity = true;
    int maxSteps = 10000;

    // Parse command line arguments
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--config" && i + 1 < argc) {
            configFile = argv[++i];
        } else if (arg == "--no-performance") {
            enablePerformance = false;
        } else if (arg == "--verbose") {
            verbose = true;
        } else if (arg == "--enable-monitoring") {
            enableMonitoring = true;
        } else if (arg == "--phase1") {
            runPhase1 = true;
            runPhase2 = runPhase3 = runPhase4 = runPhase5 = runPhase6 = false;
        } else if (arg == "--phase2") {
            runPhase2 = true;
        } else if (arg == "--phase3") {
            runPhase3 = true;
        } else if (arg == "--phase4") {
            runPhase4 = true;
        } else if (arg == "--phase5") {
            runPhase5 = true;
        } else if (arg == "--phase6") {
            runPhase6 = true;
        } else if (arg == "--disable-optimized-brain") {
            useOptimizedBrain = false;
        } else if (arg == "--enable-multithreading") {
            enableMultithreading = true;
        } else if (arg == "--disable-simd") {
            enableSIMD = false;
        } else if (arg == "--disable-event-driven") {
            enableEventDriven = false;
        } else if (arg == "--disable-sparse-connectivity") {
            enableSparseConnectivity = false;
        } else if (arg == "--steps" && i + 1 < argc) {
            maxSteps = std::stoi(argv[++i]);
        } else if (arg == "--help") {
            printRunHelp();
            return true;
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            return false;
        }
    }

    if (verbose) {
        std::cout << "Starting NLM simulation..." << std::endl;
        std::cout << "Configuration: " << configFile << std::endl;
        std::cout << "Performance optimizations: " << (enablePerformance ? "Enabled" : "Disabled") << std::endl;
        std::cout << "Verbose mode: Enabled" << std::endl;
    }

    return runSimulation(configFile, enablePerformance, verbose, enableMonitoring,
                        runPhase1, runPhase2, runPhase3, runPhase4, runPhase5, runPhase6,
                        useOptimizedBrain, enableMultithreading, enableSIMD,
                        enableEventDriven, enableSparseConnectivity, maxSteps);
}

void NLMCLI::printRunHelp() {
    std::cout << "NLM Run Command" << std::endl;
    std::cout << "===============" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Run a simulation with the specified configuration." << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Usage: nlm run [options]" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --config FILE       Configuration file (default: configs/default.cfg)" << std::endl;
    std::cout << "  --no-performance    Disable all performance optimizations" << std::endl;
    std::cout << "  --verbose           Enable verbose output" << std::endl;
    std::cout << "  --enable-monitoring Enable performance monitoring during run" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Phase selection:" << std::endl;
    std::cout << "  --phase1             Run only Phase 1 (skeleton implementation)" << std::endl;
    std::cout << "  --phase2             Run only Phase 2 (real neural computation) [default]" << std::endl;
    std::cout << "  --phase3             Run only Phase 3 (simplified agent)" << std::endl;
    std::cout << "  --phase4             Run only Phase 4 (basic world simulation)" << std::endl;
    std::cout << "  --phase5             Run only Phase 5 (performance optimizations)" << std::endl;
    std::cout << "  --phase6             Run only Phase 6 (advanced features)" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Performance options:" << std::endl;
    std::cout << "  --disable-optimized-brain  Use baseline brain implementation" << std::endl;
    std::cout << "  --enable-multithreading    Enable multi-threaded simulation" << std::endl;
    std::cout << "  --disable-simd             Disable SIMD optimizations" << std::endl;
    std::cout << "  --disable-event-driven     Disable event-driven processing" << std::endl;
    std::cout << "  --disable-sparse-connectivity Disable sparse connectivity" << std::endl;
    std::cout << "  --steps N                  Maximum simulation steps (default: 10000)" << std::endl;
}