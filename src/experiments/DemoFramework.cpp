#include "DemoFramework.hpp"

namespace nlm {

// CommandLineParser implementation
DemoOptions CommandLineParser::parse(int argc, char* argv[], const char* programName) {
    DemoOptions options;
    
    // Process arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            options.showHelp = true;
        }
        else if (arg == "--verbose" || arg == "-v") {
            options.verbose = true;
        }
        else if (arg == "--quiet" || arg == "-q") {
            options.verbose = false;
        }
        else if (arg == "--metrics" || arg == "--record-metrics") {
            options.recordMetrics = true;
        }
        else if (arg == "--no-metrics" || arg == "--skip-metrics") {
            options.recordMetrics = false;
        }
        else if (arg == "--config" || arg == "-c") {
            if (i + 1 < argc) {
                options.configFile = argv[++i];
            }
        }
        else if (arg == "--output" || arg == "-o") {
            if (i + 1 < argc) {
                options.outputFile = argv[++i];
            }
        }
        else if (arg == "--seed" || arg == "--random-seed") {
            if (i + 1 < argc) {
                options.seed = std::atoi(argv[++i]);
            }
        }
        else if (arg == "--timestep") {
            if (i + 1 < argc) {
                options.timestep = static_cast<float>(std::atof(argv[++i]));
            }
        }
        else if (arg == "--episodes" || arg == "--num-episodes") {
            if (i + 1 < argc) {
                options.numEpisodes = std::atoi(argv[++i]);
            }
        }
        else if (arg == "--steps" || arg == "--steps-per-episode") {
            if (i + 1 < argc) {
                options.stepsPerEpisode = std::atoi(argv[++i]);
            }
        }
        else if (arg == "--neurons" || arg == "--neuron-count") {
            if (i + 1 < argc) {
                options.maxNeurons = std::atoi(argv[++i]);
            }
        }
        else if (arg == "--synapses" || arg == "--synapse-count") {
            if (i + 1 < argc) {
                options.maxSynapses = std::atoi(argv[++i]);
            }
        }
        else if (arg.find("--") == 0) {
            // Unknown option
            std::cerr << "Unknown option: " << arg << std::endl;
            options.showHelp = true;
        }
        else {
            // Positional argument (phase-specific)
            try {
                options.param1 = std::stoi(arg);
            } catch (...) {
                options.param3 = arg;
            }
        }
    }
    
    return options;
}

void CommandLineParser::printHelp(const char* programName) {
    printBanner();
    
    std::cout << "\nUsage: " << programName << " [OPTIONS]\n\n";
    
    std::cout << "Basic Options:\n";
    std::cout << "  --help, -h              Show this help message\n";
    std::cout << "  --verbose, -v           Enable verbose output (default)\n";
    std::cout << "  --quiet, -q             Disable verbose output\n";
    std::cout << "  --config, -c <file>     Load configuration from file\n";
    std::cout << "  --output, -o <file>     Save output to file\n\n";
    
    std::cout << "Demo Options:\n";
    std::cout << "  --episodes, --num-episodes <N>      Number of episodes (default: 1)\n";
    std::cout << "  --steps, --steps-per-episode <N>     Steps per episode (default: 100)\n";
    std::cout << "  --neurons, --neuron-count <N>       Maximum neuron count (default: 500)\n";
    std::cout << "  --synapses, --synapse-count <N>      Maximum synapse count (default: 500)\n";
    std::cout << "  --seed, --random-seed <N>           Random seed (default: 42)\n";
    std::cout << "  --timestep <t>                      Simulation timestep (default: 0.001)\n\n";
    
    std::cout << "Performance Options:\n";
    std::cout << "  --metrics, --record-metrics         Record metrics (default)\n";
    std::cout << "  --no-metrics, --skip-metrics        Skip metric recording\n\n";
    
    std::cout << "Phase-specific:\n";
    std::cout << "  <param1>                         Phase 1 parameter (integer)\n";
    std::cout << "  <param2>                         Phase 2 parameter (float)\n";
    std::cout << "  <param3>                         Phase 3 parameter (string)\n\n";
    
    std::cout << "Example Usage:\n";
    std::cout << "  " << programName << " --episodes 10 --steps 500 --neurons 1000\n";
    std::cout << "  " << programName << " --config myconfig.cfg --verbose\n";
    std::cout << "  " << programName << " phase-specific-value\n\n";
}

std::string CommandLineParser::getUsageString(const char* programName) {
    std::ostringstream oss;
    oss << "Usage: " << programName << " [OPTIONS] [phase-specific-param]\n";
    oss << "Run " << programName << " with specified options and phase-specific parameters.\n";
    oss << "Use --help for detailed information.\n";
    return oss.str();
}

void CommandLineParser::printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Advanced Brain Simulation                                 ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

std::string CommandLineParser::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

// DemoFramework implementation
DemoFramework::DemoFramework(const std::string& demoName, const std::string& demoDescription)
    : demoName_(demoName), demoDescription_(demoDescription), elapsedTime_(0.0) {
    startTime_ = std::chrono::steady_clock::now();
}

int DemoFramework::run(int argc, char* argv[]) {
    // Parse command line arguments
    DemoOptions options = CommandLineParser::parse(argc, argv, demoName_.c_str());
    
    // Show help if requested
    if (options.showHelp) {
        CommandLineParser::printHelp(demoName_.c_str());
        return 0;
    }
    
    // Validate options
    if (!validateOptions(options)) {
        std::cerr << "ERROR: Invalid options provided.\n";
        CommandLineParser::printHelp(demoName_.c_str());
        return 1;
    }
    
    // Initialize framework
    if (!initialize(options)) {
        std::cerr << "ERROR: Failed to initialize " << demoName_ << ".\n";
        return 1;
    }
    initialized_ = true;
    
    try {
        // Run the demo
        running_ = true;
        bool success = runDemo(options);
        running_ = false;
        
        if (!success) {
            std::cerr << "ERROR: Demo execution failed.\n";
            return 1;
        }
        
        // Print results
        printResults(options);
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Exception during demo execution: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "ERROR: Unknown exception during demo execution.\n";
        return 1;
    }
    
    // Cleanup
    cleanup();
    initialized_ = false;
    
    // Calculate elapsed time
    auto endTime = std::chrono::steady_clock::now();
    elapsedTime_ = std::chrono::duration<double>(endTime - startTime_).count();
    
    return 0;
}

void DemoFramework::setupLogging(bool verbose) {
    logger_ = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(verbose ? LogLevel::Info : LogLevel::Warning);
    logger_->addLogger(consoleLogger);
    Logger::setGlobal(logger_);
    
    if (verbose) {
        NLM_LOG_INFO("=== " + demoName_ + " ===");
        NLM_LOG_INFO(demoDescription_);
    }
}

void DemoFramework::setupConfiguration(const DemoOptions& options) {
    config_ = std::make_shared<Config>();
    
    // Load configuration from file if specified
    if (!options.configFile.empty()) {
        if (config_->loadFromFile(options.configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + options.configFile);
        } else {
            NLM_LOG_WARNING("Using default configuration (file not found: " + options.configFile + ")");
        }
    }
    
    // Override with command line options
    config_->set("random_seed", static_cast<int64_t>(options.seed), ConfigSource::CommandLine);
    config_->set("simulation_timestep", options.timestep, ConfigSource::CommandLine);
    
    // Apply common defaults
    applyStandardConfig(config_, options);
}

void DemoFramework::setupBrain(std::shared_ptr<Config> config) {
    brain_ = std::make_shared<Brain>(config);
    if (!brain_->initialize()) {
        throw std::runtime_error("Failed to initialize brain!");
    }
    NLM_LOG_INFO("Brain initialized successfully");
}

void DemoFramework::setupWorld(std::shared_ptr<SimpleWorld> world) {
    world_ = world;
    if (world_) {
        NLM_LOG_INFO("World configured successfully");
    }
}

void DemoFramework::setupAgent(std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world) {
    agent_ = agent;
    if (agent_ && world_) {
        agent_->initialize(*world_);
        NLM_LOG_INFO("Agent brain initialized successfully");
    }
}

void DemoFramework::recordMetric(const std::string& name, double value) {
    metrics_.record(name, value);
}

void DemoFramework::printMetricsSummary() const {
    std::cout << "\n=== Metrics Summary ===" << std::endl;
    std::cout << metrics_.summary() << std::endl;
}

double DemoFramework::getElapsedTime() const {
    if (!running_) {
        return elapsedTime_;
    }
    auto currentTime = std::chrono::steady_clock::now();
    return std::chrono::duration<double>(currentTime - startTime_).count();
}

void DemoFramework::printHeader(const std::string& title) const {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "  " << title << std::endl;
    std::cout << std::string(60, '=') << std::endl;
}

void DemoFramework::printSection(const std::string& title) const {
    std::cout << "\n--- " << title << " ---" << std::endl;
}

void DemoFramework::printResult(const std::string& name, double value, double max) const {
    std::cout << "  " << std::left << std::setw(30) << name << ": ";
    std::cout << std::fixed << std::setprecision(3) << value;
    if (max > 0) {
        std::cout << " (" << std::setprecision(1) << (value/max*100) << "%)";
    }
    std::cout << std::endl;
}

void DemoFramework::printProgress(int step, int total, float progress) const {
    if (step % 50 == 0 || step == total) {
        int percent = static_cast<int>(progress * 100);
        std::cout << "\rProgress: " << percent << "% (Step " << step << "/" << total << ")";
        std::cout.flush();
    }
}

void DemoFramework::applyStandardConfig(std::shared_ptr<Config> config, const DemoOptions& options) {
    config->set("neuron_count", static_cast<int64_t>(options.maxNeurons), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    config->set("random_seed", static_cast<int64_t>(options.seed), ConfigSource::Default);
    config->set("simulation_timestep", options.timestep, ConfigSource::Default);
    
    // Basic plasticity parameters
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
}

void DemoFramework::applyPhaseConfig(std::shared_ptr<Config> config, int phase) {
    switch (phase) {
        case 1:
            // Phase 1 specific config
            config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::PhaseSpecific);
            config->set("region_count", static_cast<int64_t>(1), ConfigSource::PhaseSpecific);
            break;
        case 2:
            // Phase 2 specific config
            config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::PhaseSpecific);
            config->set("region_count", static_cast<int64_t>(2), ConfigSource::PhaseSpecific);
            config->set("connection_probability", 0.15f, ConfigSource::PhaseSpecific);
            break;
        case 3:
            // Phase 3 specific config
            config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::PhaseSpecific);
            config->set("region_count", static_cast<int64_t>(2), ConfigSource::PhaseSpecific);
            config->set("connection_probability", 0.1f, ConfigSource::PhaseSpecific);
            break;
        default:
            break;
    }
}

bool DemoFramework::validateOptions(const DemoOptions& options) const {
    if (options.maxNeurons <= 0) {
        std::cerr << "ERROR: Neuron count must be positive." << std::endl;
        return false;
    }
    if (options.maxSynapses <= 0) {
        std::cerr << "ERROR: Synapse count must be positive." << std::endl;
        return false;
    }
    if (options.timestep <= 0.0f) {
        std::cerr << "ERROR: Timestep must be positive." << std::endl;
        return false;
    }
    if (options.stepsPerEpisode <= 0) {
        std::cerr << "ERROR: Steps per episode must be positive." << std::endl;
        return false;
    }
    if (options.numEpisodes <= 0) {
        std::cerr << "ERROR: Number of episodes must be positive." << std::endl;
        return false;
    }
    return true;
}

} // namespace nlm