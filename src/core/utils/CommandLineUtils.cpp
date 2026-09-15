// Command-line argument parser with help and validation
Options CommandLineParser::parse(int argc, char** argv) {
    Options opts;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            opts.showHelp = true;
        } else if (arg == "--version") {
            opts.showVersion = true;
        } else if (arg == "--verbose" || arg == "-v") {
            opts.verbose = true;
            opts.logLevel = "debug";
        } else if (arg == "--debug") {
            opts.debug = true;
            opts.logLevel = "trace";
        } else if (arg == "--quiet" || arg == "-q") {
            opts.quiet = true;
            opts.logLevel = "error";
        } else if (arg == "--config") {
            if (i + 1 < argc) {
                opts.configFile = argv[++i];
            }
        } else if (arg.find("--config=") == 0) {
            opts.configFile = arg.substr(9);
        } else if (arg == "--output") {
            if (i + 1 < argc) {
                opts.outputFile = argv[++i];
            }
        } else if (arg.find("--output=") == 0) {
            opts.outputFile = arg.substr(10);
        } else if (arg == "--checkpoint") {
            if (i + 1 < argc) {
                opts.checkpointFile = argv[++i];
            }
        } else if (arg.find("--checkpoint=") == 0) {
            opts.checkpointFile = arg.substr(13);
        } else if (arg == "--log-level") {
            if (i + 1 < argc) {
                opts.logLevel = argv[++i];
            }
        } else if (arg.find("--log-level=") == 0) {
            opts.logLevel = arg.substr(12);
        } else if (arg == "--sim-speed") {
            if (i + 1 < argc) {
                opts.simulationSpeed = std::stod(argv[++i]);
            }
        } else if (arg.find("--sim-speed=") == 0) {
            opts.simulationSpeed = std::stod(arg.substr(12));
        } else if (arg == "--max-steps") {
            if (i + 1 < argc) {
                opts.maxSteps = std::stoull(argv[++i]);
            }
        } else if (arg.find("--max-steps=") == 0) {
            opts.maxSteps = std::stoull(arg.substr(12));
        } else if (arg == "--enable-gpu") {
            opts.enableGPU = true;
        } else if (arg == "--enable-monitoring") {
            opts.enableMonitoring = true;
        } else if (arg == "--monitoring-port") {
            if (i + 1 < argc) {
                opts.monitoringPort = argv[++i];
            }
        } else if (arg.find("--monitoring-port=") == 0) {
            opts.monitoringPort = arg.substr(19);
        } else if (arg == "--plugin-path") {
            if (i + 1 < argc) {
                opts.pluginPath = argv[++i];
            }
        } else if (arg.find("--plugin-path=") == 0) {
            opts.pluginPath = arg.substr(14);
        } else if (arg == "--list-plugins") {
            opts.listPlugins = true;
        } else if (arg == "--agent-mode") {
            if (i + 1 < argc) {
                opts.agentMode = argv[++i];
            }
        } else if (arg.find("--agent-mode=") == 0) {
            opts.agentMode = arg.substr(13);
        } else if (arg == "--learning-mode") {
            if (i + 1 < argc) {
                opts.learningMode = argv[++i];
            }
        } else if (arg.find("--learning-mode=") == 0) {
            opts.learningMode = arg.substr(16);
        } else if (arg == "--memory-mode") {
            if (i + 1 < argc) {
                opts.memoryMode = argv[++i];
            }
        } else if (arg.find("--memory-mode=") == 0) {
            opts.memoryMode = arg.substr(14);
        } else if (arg == "--neuromodulation-mode") {
            if (i + 1 < argc) {
                opts.neuromodulationMode = argv[++i];
            }
        } else if (arg.find("--neuromodulation-mode=") == 0) {
            opts.neuromodulationMode = arg.substr(25);
        } else if (arg == "--curiosity-threshold") {
            if (i + 1 < argc) {
                opts.curiosityThreshold = std::stod(argv[++i]);
            }
        } else if (arg.find("--curiosity-threshold=") == 0) {
            opts.curiosityThreshold = std::stod(arg.substr(24));
        } else if (arg == "--novelty-threshold") {
            if (i + 1 < argc) {
                opts.noveltyThreshold = std::stod(argv[++i]);
            }
        } else if (arg.find("--novelty-threshold=") == 0) {
            opts.noveltyThreshold = std::stod(arg.substr(21));
        } else if (arg == "--plasticity-rate") {
            if (i + 1 < argc) {
                opts.plasticityRate = std::stof(argv[++i]);
            }
        } else if (arg.find("--plasticity-rate=") == 0) {
            opts.plasticityRate = std::stof(arg.substr(18));
        } else if (arg == "--no-replay") {
            opts.enableReplay = false;
        } else if (arg == "--no-checkpointing") {
            opts.enableCheckpointing = false;
        } else if (arg == "--no-development") {
            opts.enableDevelopment = false;
        } else if (arg == "--development-profile") {
            if (i + 1 < argc) {
                opts.developmentProfile = argv[++i];
            }
        } else if (arg.find("--development-profile=") == 0) {
            opts.developmentProfile = arg.substr(23);
        } else if (arg == "--experiment") {
            if (i + 1 < argc) {
                opts.experimentModes.push_back(argv[++i]);
            }
        } else if (arg == "--simulation") {
            if (i + 1 < argc) {
                opts.simulationModes.push_back(argv[++i]);
            }
        } else {
            // Unknown argument, treat as experiment mode
            opts.experimentModes.push_back(arg);
        }
    }
    
    return opts;
}

void CommandLineParser::printHelp(const std::string& programName) {
    std::cout << "NLM (熙然) - Neural Learning Machine" << std::endl;
    std::cout << "Usage: " << programName << " [options] [experiment_mode]" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Basic Options:" << std::endl;
    std::cout << "  -h, --help              Show this help message" << std::endl;
    std::cout << "  -v, --verbose           Enable verbose logging" << std::endl;
    std::cout << "  -q, --quiet             Quiet mode (errors only)" << std::endl;
    std::cout << "  --debug                 Enable debug logging" << std::endl;
    std::cout << "  --version               Show version information" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Configuration Options:" << std::endl;
    std::cout << "  --config <file>         Configuration file (default: configs/default.cfg)" << std::endl;
    std::cout << "  --output <file>         Output file for results" << std::endl;
    std::cout << "  --checkpoint <file>     Checkpoint file for save/load" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Simulation Options:" << std::endl;
    std::cout << "  --sim-speed <speed>     Simulation speed factor (default: 1.0)" << std::endl;
    std::cout << "  --max-steps <steps>     Maximum number of simulation steps" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Advanced Options:" << std::endl;
    std::cout << "  --enable-gpu            Enable GPU acceleration if available" << std::endl;
    std::cout << "  --enable-monitoring      Enable monitoring system" << std::endl;
    std::cout << "  --monitoring-port <port> Monitoring port (default: 8080)" << std::endl;
    std::cout << "  --plugin-path <path>    Path to plugins directory" << std::endl;
    std::cout << "  --list-plugins          List available plugins and exit" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Agent Configuration Options:" << std::endl;
    std::cout << "  --agent-mode <mode>     Agent mode: auto, basic, advanced, custom" << std::endl;
    std::cout << "  --learning-mode <mode>  Learning mode: auto, episodic, procedural, semantic" << std::endl;
    std::cout << "  --memory-mode <mode>    Memory mode: auto, working, episodic, semantic" << std::endl;
    std::cout << "  --neuromodulation-mode <mode> Neuromodulation mode: auto, dopamine, curiosity, novelty" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Neuromodulation Parameters:" << std::endl;
    std::cout << "  --curiosity-threshold <value> Curiosity threshold (default: 0.3)" << std::endl;
    std::cout << "  --novelty-threshold <value> Novelty threshold (default: 0.5)" << std::endl;
    std::cout << "  --plasticity-rate <rate> Plasticity rate (default: 0.01)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Feature Toggles:" << std::endl;
    std::cout << "  --no-replay             Disable replay system" << std::endl;
    std::cout << "  --no-checkpointing      Disable checkpointing" << std::endl;
    std::cout << "  --no-development        Disable development system" << std::endl;
    std::cout << "  --development-profile <profile> Development profile: standard, aggressive, conservative" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Experiment Modes:" << std::endl;
    std::cout << "  --experiment <mode>     Run specific experiment mode" << std::endl;
    std::cout << "  --simulation <mode>     Run specific simulation mode" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Experiment Modes:" << std::endl;
    std::cout << "  basic                   Basic neural computation test" << std::endl;
    std::cout << "  learning                Plasticity learning experiment" << std::endl;
    std::cout << "  stdp                    STDP verification" << std::endl;
    std::cout << "  connectivity            Basic connectivity test" << std::endl;
    std::cout << "  phase3                  Phase 3 experiments" << std::endl;
    std::cout << "  phase4                  Phase 4 experiments" << std::endl;
    std::cout << "  phase6                  Phase 6 integration test" << std::endl;
    std::cout << std::endl;
}

void CommandLineParser::printVersion() {
    std::cout << "NLM (熙然) - Neural Learning Machine" << std::endl;
    std::cout << "Version: 0.1.0 (Phase 6 Integration)" << std::endl;
    std::cout << "Built with C++20, SIMD optimizations, and GPU support" << std::endl;
    std::cout << "Author: NLM Research Team" << std::endl;
    std::cout << "License: MIT" << std::endl;
}

bool CommandLineParser::validateOptions(const Options& options) {
    bool valid = true;
    
    // Validate log level
    if (!options.quiet && !options.debug && !options.verbose) {
        std::cerr << "Error: Log level must be specified (--quiet, --verbose, or --debug)" << std::endl;
        valid = false;
    }
    
    // Validate simulation speed
    if (options.simulationSpeed <= 0.0) {
        std::cerr << "Error: Simulation speed must be positive" << std::endl;
        valid = false;
    }
    
    // Validate curiosity threshold
    if (options.curiosityThreshold < 0.0 || options.curiosityThreshold > 1.0) {
        std::cerr << "Error: Curiosity threshold must be between 0.0 and 1.0" << std::endl;
        valid = false;
    }
    
    // Validate novelty threshold
    if (options.noveltyThreshold < 0.0 || options.noveltyThreshold > 1.0) {
        std::cerr << "Error: Novelty threshold must be between 0.0 and 1.0" << std::endl;
        valid = false;
    }
    
    // Validate plasticity rate
    if (options.plasticityRate < 0.0) {
        std::cerr << "Error: Plasticity rate must be non-negative" << std::endl;
        valid = false;
    }
    
    // Validate development profile
    if (options.developmentProfile != "standard" && options.developmentProfile != "aggressive" && options.developmentProfile != "conservative") {
        std::cerr << "Error: Invalid development profile. Must be 'standard', 'aggressive', or 'conservative'" << std::endl;
        valid = false;
    }
    
    // Validate agent mode
    if (options.agentMode != "auto" && options.agentMode != "basic" && options.agentMode != "advanced" && options.agentMode != "custom") {
        std::cerr << "Error: Invalid agent mode. Must be 'auto', 'basic', 'advanced', or 'custom'" << std::endl;
        valid = false;
    }
    
    // Validate learning mode
    if (options.learningMode != "auto" && options.learningMode != "episodic" && options.learningMode != "procedural" && options.learningMode != "semantic") {
        std::cerr << "Error: Invalid learning mode. Must be 'auto', 'episodic', 'procedural', or 'semantic'" << std::endl;
        valid = false;
    }
    
    return valid;
}