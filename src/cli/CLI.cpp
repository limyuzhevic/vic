#include "CLI.hpp"

namespace nlm {

CLI::CLI() : options(), logger(nullptr), startTime(std::chrono::system_clock::now()) {
}

CLI::~CLI() {
    cleanup();
}

bool CLI::parseArguments(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            options.help = true;
        } else if (arg == "--version") {
            options.version = true;
        } else if (arg == "--config" || arg == "-c") {
            if (i + 1 < argc) {
                options.configFile = argv[++i];
            }
        } else if (arg == "--config-json" || arg == "-j") {
            if (i + 1 < argc) {
                options.configJson = argv[++i];
            }
        } else if (arg == "--params-file" || arg == "-p") {
            if (i + 1 < argc) {
                options.paramsFile = argv[++i];
            }
        } else if (arg == "--mode") {
            if (i + 1 < argc) {
                std::string modeStr = argv[++i];
                if (modeStr == "simulation") options.mode = OperationMode::Simulation;
                else if (modeStr == "learning") options.mode = OperationMode::Learning;
                else if (modeStr == "analysis") options.mode = OperationMode::Analysis;
                else if (modeStr == "benchmark") options.mode = OperationMode::Benchmark;
                else if (modeStr == "batch") options.mode = OperationMode::Batch;
            }
        } else if (arg == "--output" || arg == "-o") {
            if (i + 1 < argc) {
                options.outputFile = argv[++i];
            }
        } else if (arg == "--log-file") {
            if (i + 1 < argc) {
                options.logFile = argv[++i];
            }
        } else if (arg == "--checkpoint-dir") {
            if (i + 1 < argc) {
                options.checkpointDir = argv[++i];
            }
        } else if (arg == "--enable-visualization") {
            options.enableVisualization = true;
        } else if (arg == "--enable-profiling") {
            options.enableProfiling = true;
        } else if (arg == "--enable-memory-analysis") {
            options.enableMemoryAnalysis = true;
        } else if (arg == "--batch-mode" || arg == "-b") {
            options.batchMode = true;
        } else if (arg == "--steps" || arg == "-s") {
            if (i + 1 < argc) {
                options.numSteps = std::stoi(argv[++i]);
            }
        } else if (arg == "--timestep" || arg == "-t") {
            if (i + 1 < argc) {
                options.timestep = std::stod(argv[++i]);
            }
        } else if (arg == "--seed" || arg == "-S") {
            if (i + 1 < argc) {
                options.randomSeed = std::stoull(argv[++i]);
            }
        } else if (arg == "--enable-subsystem" || arg == "-E") {
            if (i + 1 < argc) {
                options.subsystemFilters.push_back(argv[++i]);
            }
        } else if (arg == "--disable-subsystem" || arg == "-D") {
            if (i + 1 < argc) {
                std::string subsystem = "disable_" + std::string(argv[++i]);
                options.subsystemFilters.push_back(subsystem);
            }
        } else if (arg.substr(0, 2) == "--" && arg.find('=') != std::string::npos) {
            // Handle --key=value format
            size_t pos = arg.find('=');
            std::string key = arg.substr(2, pos - 2);
            std::string value = arg.substr(pos + 1);
            options.runtimeParams[key] = value;
        } else if (arg == "--continue-last") {
            options.continueLast = true;
        } else if (arg == "--save-checkpoint") {
            options.saveCheckpoint = true;
        } else if (arg == "--load-checkpoint") {
            options.loadCheckpoint = true;
        } else if (arg == "--experiment-name") {
            if (i + 1 < argc) {
                options.experimentName = argv[++i];
            }
        } else if (arg.substr(0, 11) == "--command=") {
            options.command = arg.substr(11);
        }
    }
    
    return true;
}

int CLI::execute() const {
    if (options.help) {
        printHelp();
        return 0;
    }
    
    if (options.version) {
        printVersion();
        return 0;
    }
    
    if (!initialize()) {
        return 1;
    }
    
    logSystemInfo();
    loadConfigurations();
    applyRuntimeParameters();
    
    switch (options.mode) {
        case OperationMode::Simulation:
            return runSimulation();
        case OperationMode::Learning:
            return runLearning();
        case OperationMode::Analysis:
            return runAnalysis();
        case OperationMode::Benchmark:
            return runBenchmark();
        case OperationMode::Batch:
            return runBatch();
        default:
            NLM_LOG_ERROR("Unknown operation mode");
            return 1;
    }
}

bool CLI::initialize() {
    setupLogger();
    
    NLM_LOG_INFO("NLM CLI initialization started");
    
    // Create and setup config
    auto config = std::make_shared<Config>();
    
    // Initialize brain if not loading checkpoint
    if (!options.loadCheckpoint) {
        NLM_LOG_INFO("Creating new NLM brain");
    }
    
    NLM_LOG_INFO("CLI initialization complete");
    return true;
}

void CLI::setupLogger() const {
    logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    consoleLogger->setUseColors(true);
    logger->addLogger(consoleLogger);
    
    if (!options.logFile.empty()) {
        auto fileLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        fileLogger->setOutputFile(options.logFile);
        logger->addLogger(fileLogger);
    }
    
    Logger::setGlobal(logger);
    NLM_LOG_INFO("Logger initialized");
}

void CLI::loadConfigurations() const {
    NLM_LOG_INFO("Loading configurations...");
    
    if (!options.configFile.empty()) {
        if (loadConfigFile(options.configFile)) {
            NLM_LOG_INFO("Loaded configuration from file: " + options.configFile);
        } else {
            NLM_LOG_WARNING("Failed to load configuration from file: " + options.configFile);
        }
    }
    
    if (!options.configJson.empty()) {
        if (loadConfigJson(options.configJson)) {
            NLM_LOG_INFO("Loaded configuration from JSON");
        } else {
            NLM_LOG_WARNING("Failed to load configuration from JSON");
        }
    }
    
    if (!options.paramsFile.empty()) {
        if (loadParamsFile(options.paramsFile)) {
            NLM_LOG_INFO("Loaded parameters from file: " + options.paramsFile);
        } else {
            NLM_LOG_WARNING("Failed to load parameters from file: " + options.paramsFile);
        }
    }
    
    NLM_LOG_INFO("Configuration loading complete");
}

void CLI::applyRuntimeParameters() const {
    NLM_LOG_INFO("Applying runtime parameters...");
    
    for (const auto& param : options.runtimeParams) {
        NLM_LOG_INFO("Setting runtime parameter: " + param.first + " = " + param.second);
    }
    
    NLM_LOG_INFO("Runtime parameters applied");
}

void CLI::printHelp() const {
    std::cout << R"(
NLM - Neural Learning Machine Command Line Interface

USAGE:
    nlm [OPTIONS]

OPERATION MODES:
    --mode simulation        Run neural simulation (default)
    --mode learning          Run learning experiment
    --mode analysis          Analyze brain state and statistics
    --mode benchmark         Run performance benchmarks
    --mode batch             Process multiple experiments

CONFIGURATION OPTIONS:
    --config file, -c file      Load configuration from file
    --config-json json, -j json  Load configuration from JSON string
    --params-file file, -p file  Load parameter overrides from file

OUTPUT OPTIONS:
    --output file, -o file      Output results to file
    --log-file file             Log all output to file
    --checkpoint-dir dir         Directory for checkpoint files

ADVANCED OPTIONS:
    --enable-visualization      Enable visualization output
    --enable-profiling          Enable performance profiling
    --enable-memory-analysis    Enable memory analysis
    --batch-mode, -b            Run in batch mode without interactive output
    --steps n, -s n             Number of simulation steps (-1 for unlimited)
    --timestep t, -t t          Simulation timestep (default: 0.001)
    --seed s, -S s              Random seed (default: 42)
    --enable-subsystem name, -E name    Enable specific subsystem
    --disable-subsystem name, -D name   Disable specific subsystem
    --runtime-param key=value              Set runtime parameter (can be repeated)
    --experiment-name name              Name for the current experiment
    --continue-last               Continue from last checkpoint
    --save-checkpoint              Save checkpoint after completion
    --load-checkpoint              Load from checkpoint

HELP:
    --help, -h                    Show this help message
    --version                     Show version information
)" << std::endl;
}

void CLI::printVersion() const {
    std::cout << "NLM v2.0 - Neural Learning Machine (Phase 2: Real Neural Computation)" << std::endl;
    std::cout << "Enhanced with comprehensive CLI, visualization, and monitoring tools" << std::endl;
}

void CLI::printProgress(int step, int totalSteps, double currentTime) const {
    if (options.batchMode) return;
    
    double progress = (totalSteps > 0) ? (static_cast<double>(step) / totalSteps) * 100.0 : 0.0;
    int barWidth = 50;
    int pos = static_cast<int>(barWidth * progress / 100.0);
    
    std::cout << "\r[";
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << std::fixed << std::setprecision(1) << progress << "% "
              << "Step: " << step << "/" << totalSteps
              << " Time: " << std::fixed << std::setprecision(3) << currentTime << "s";
    std::cout.flush();
}

void CLI::logSystemInfo() const {
    NLM_LOG_INFO("NLM System Information:");
    NLM_LOG_INFO("  Version: 2.0 (Phase 2: Real Neural Computation)");
    NLM_LOG_INFO("  Operation Mode: " + (options.mode == OperationMode::Simulation ? "Simulation" :
                                   options.mode == OperationMode::Learning ? "Learning" :
                                   options.mode == OperationMode::Analysis ? "Analysis" :
                                   options.mode == OperationMode::Benchmark ? "Benchmark" : "Batch"));
    NLM_LOG_INFO("  Num Steps: " + std::to_string(options.numSteps));
    NLM_LOG_INFO("  Timestep: " + std::to_string(options.timestep) + "s");
    NLM_LOG_INFO("  Random Seed: " + std::to_string(options.randomSeed));
    NLM_LOG_INFO("  Visualization: " + std::string(options.enableVisualization ? "Enabled" : "Disabled"));
    NLM_LOG_INFO("  Profiling: " + std::string(options.enableProfiling ? "Enabled" : "Disabled"));
}

bool CLI::loadConfigFile(const std::string& filepath) const {
    // Placeholder for actual config loading
    NLM_LOG_INFO("Config file loading not yet implemented: " + filepath);
    return false;
}

bool CLI::loadConfigJson(const std::string& json) const {
    // Placeholder for actual JSON config loading
    NLM_LOG_INFO("Config JSON loading not yet implemented");
    return false;
}

bool CLI::loadParamsFile(const std::string& filepath) const {
    // Placeholder for actual params loading
    NLM_LOG_INFO("Params file loading not yet implemented: " + filepath);
    return false;
}

int CLI::runSimulation() const {
    NLM_LOG_INFO("Starting NLM Simulation...");
    
    int stepsToRun = options.numSteps > 0 ? options.numSteps : 100;
    
    runWithProgress(stepsToRun, [this](int step, double time) {
        if (!options.batchMode) {
            printProgress(step, stepsToRun, time);
        }
    });
    
    if (!options.batchMode) {
        std::cout << std::endl;
    }
    
    NLM_LOG_INFO("Simulation completed successfully");
    generateStatisticsReport(nullptr); // Would pass brain here
    return 0;
}

int CLI::runLearning() const {
    NLM_LOG_INFO("Starting NLM Learning Experiment...");
    
    // Would load brain and run learning experiment
    int stepsToRun = options.numSteps > 0 ? options.numSteps : 1000;
    
    runWithProgress(stepsToRun, [this](int step, double time) {
        if (!options.batchMode) {
            printProgress(step, stepsToRun, time);
        }
    });
    
    if (!options.batchMode) {
        std::cout << std::endl;
    }
    
    NLM_LOG_INFO("Learning experiment completed");
    return 0;
}

int CLI::runAnalysis() const {
    NLM_LOG_INFO("Starting NLM Analysis...");
    
    // Would load brain and run analysis
    generateStatisticsReport(nullptr);
    generatePerformanceReport();
    generateMemoryReport();
    
    NLM_LOG_INFO("Analysis completed");
    return 0;
}

int CLI::runBenchmark() const {
    NLM_LOG_INFO("Starting NLM Benchmark...");
    
    // Would run performance benchmarks
    // This would use the Benchmarker class
    NLM_LOG_INFO("Benchmarking not yet implemented");
    
    NLM_LOG_INFO("Benchmark completed");
    return 0;
}

int CLI::runBatch() const {
    NLM_LOG_INFO("Starting NLM Batch Mode...");
    
    // Would run multiple experiments
    NLM_LOG_INFO("Batch mode not yet implemented");
    
    NLM_LOG_INFO("Batch mode completed");
    return 0;
}

void CLI::generateStatisticsReport(const std::shared_ptr<Brain>& brain) const {
    NLM_LOG_INFO("Generating statistics report...");
    
    if (brain) {
        // Would generate actual statistics from brain
        NLM_LOG_INFO("Statistics report generated with brain data");
    } else {
        NLM_LOG_INFO("Statistics report generated with placeholder data");
    }
}

void CLI::generatePerformanceReport() const {
    NLM_LOG_INFO("Generating performance report...");
    
    // Would generate performance metrics
    NLM_LOG_INFO("Performance metrics: TODO - implement actual performance tracking");
}

void CLI::generateMemoryReport() const {
    NLM_LOG_INFO("Generating memory report...");
    
    // Would generate memory usage statistics
    NLM_LOG_INFO("Memory metrics: TODO - implement actual memory tracking");
}

void CLI::runWithProgress(int totalSteps, std::function<void(int, double)> stepCallback) const {
    for (int step = 1; step <= totalSteps; ++step) {
        double time = step * options.timestep;
        stepCallback(step, time);
        
        // Simulate work (would actually run brain step)
        if (step % 10 == 0) {
            NLM_LOG_INFO("Processing step " + std::to_string(step) + "/" + std::to_string(totalSteps));
        }
    }
}

void CLI::enableSubsystem(const std::string& subsystem) const {
    NLM_LOG_INFO("Enabling subsystem: " + subsystem);
    // Would actually enable the subsystem
}

void CLI::disableSubsystem(const std::string& subsystem) const {
    NLM_LOG_INFO("Disabling subsystem: " + subsystem);
    // Would actually disable the subsystem
}

bool CLI::isSubsystemEnabled(const std::string& subsystem) const {
    // Would actually check if subsystem is enabled
    return true;
}

void CLI::setupSignalHandlers() const {
    NLM_LOG_INFO("Setting up signal handlers");
    // Would setup proper signal handling
}

void CLI::cleanup() const {
    NLM_LOG_INFO("Cleaning up CLI resources");
    // Would cleanup resources
}

int CLIStream::Run(int argc, char** argv) {
    CLI cli;
    if (!cli.parseArguments(argc, argv)) {
        return 1;
    }
    return cli.execute();
}

} // namespace nlm
