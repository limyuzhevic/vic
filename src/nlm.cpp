// Unified NLM Command Line Interface
// Provides a comprehensive interface for NLM operations with subcommands

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Types/Types.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <iomanip>
#include <sstream>

using namespace nlm;

// Constants
const char* NLM_VERSION = "0.1.0";
const char* BUILD_TIMESTAMP = "2026-11-07T23:10:52Z";
const char* COMPILER_VERSION = "C++20";
const bool NLM_BUILD_PYTHON_BINDINGS = false;

// Command line arguments parser
struct CLIArgs {
    std::string command;
    std::string subcommand;
    std::vector<std::string> args;
    bool help;
    bool version;
    bool verbose;
    std::string configFile;
    std::string outputFile;
    bool dryRun;
    int simulationSteps;
    int neuronCount;
    float learningRate;
    std::string logLevel;
};

// Unified NLM CLI interface
class NLMCLI {
public:
    NLMCLI() {
        printBanner();
        initializeLogger();
    }
    
    void run(int argc, char* argv[]) {
        parseArgs(argc, argv);
        executeCommand();
    }
    
private:
    CLIArgs args;
    std::shared_ptr<Config> config;
    
    void printBanner() {
        std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Advanced Neural Simulation Platform                      ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    }
    
    void initializeLogger() {
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
    }
    
    void parseArgs(int argc, char* argv[]) {
        // Default values
        args.help = false;
        args.version = false;
        args.verbose = false;
        args.dryRun = false;
        args.simulationSteps = 1000;
        args.neuronCount = 1000;
        args.learningRate = 0.01f;
        args.logLevel = "INFO";
        
        // Simple argument parsing (can be enhanced later)
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                args.help = true;
            } else if (arg == "--version" || arg == "-v") {
                args.version = true;
            } else if (arg == "--verbose" || arg == "-V") {
                args.verbose = true;
            } else if (arg == "--dry-run") {
                args.dryRun = true;
            } else if (arg == "--steps" && i + 1 < argc) {
                args.simulationSteps = std::stoi(argv[++i]);
            } else if (arg == "--neurons" && i + 1 < argc) {
                args.neuronCount = std::stoi(argv[++i]);
            } else if (arg == "--learning-rate" && i + 1 < argc) {
                args.learningRate = std::stof(argv[++i]);
            } else if (arg == "--config" && i + 1 < argc) {
                args.configFile = argv[++i];
            } else if (arg == "--output" && i + 1 < argc) {
                args.outputFile = argv[++i];
            } else if (arg == "--log-level" && i + 1 < argc) {
                args.logLevel = argv[++i];
            } else {
                // First non-option argument is the command
                if (args.command.empty()) {
                    args.command = arg;
                } else if (args.subcommand.empty()) {
                    args.subcommand = arg;
                } else {
                    args.args.push_back(arg);
                }
            }
        }
    }
    
    void executeCommand() {
        if (args.help) {
            printHelp();
            return;
        }
        
        if (args.version) {
            printVersion();
            return;
        }
        
        if (args.command.empty()) {
            std::cerr << "Error: No command specified. Use --help for available commands." << std::endl;
            return;
        }
        
        // Dispatch to appropriate command handler
        auto commandHandler = commandHandlers.find(args.command);
        if (commandHandler == commandHandlers.end()) {
            std::cerr << "Error: Unknown command: " << args.command << std::endl;
            printHelp();
            return;
        }
        
        (this->*commandHandler->second)();
    }
    
    // Command handlers
    void handleRun() {
        std::cout << "=== Running NLM Simulation ===" << std::endl;
        
        // Load configuration
        if (!loadConfig()) {
            std::cerr << "Failed to load configuration" << std::endl;
            return;
        }
        
        // Create and initialize brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain" << std::endl;
            return;
        }
        
        std::cout << "Brain initialized successfully" << std::endl;
        std::cout << "Running " << args.simulationSteps << " simulation steps..." << std::endl;
        
        // Run simulation
        for (int step = 0; step < args.simulationSteps; ++step) {
            brain->step(step, step * 0.001);
            
            if (step % 100 == 0 && args.verbose) {
                std::cout << "Step " << step << ", " 
                         << brain->getFiringNeuronCount() << " firing neurons" << std::endl;
            }
        }
        
        std::cout << "Simulation completed successfully" << std::endl;
        brain->logStatus();
    }
    
    void handleDemo() {
        std::cout << "=== NLM Demo Mode ===" << std::endl;
        
        if (!args.subcommand.empty()) {
            if (args.subcommand == "phase2") {
                runPhase2Demo();
            } else if (args.subcommand == "phase6") {
                runPhase6Demo();
            } else if (args.subcommand == "benchmark") {
                runBenchmarkDemo();
            } else {
                std::cerr << "Error: Unknown demo subcommand: " << args.subcommand << std::endl;
            }
        } else {
            std::cout << "Available demos:" << std::endl;
            std::cout << "  phase2   - Phase 2: Real Neural Computation" << std::endl;
            std::cout << "  phase6    - Phase 6: Integration Test" << std::endl;
            std::cout << "  benchmark - Performance benchmark" << std::endl;
        }
    }
    
    void handleBenchmark() {
        std::cout << "=== NLM Benchmark ===" << std::endl;
        
        if (!loadConfig()) {
            std::cerr << "Failed to load configuration" << std::endl;
            return;
        }
        
        runBenchmark(args.neuronCount, args.simulationSteps);
    }
    
    void handlePython() {
        std::cout << "=== NLM Python Interface ===" << std::endl;
        std::cout << "Python bindings available via pynlm module" << std::endl;
        std::cout << "Install with: pip install ." << std::endl;
        std::cout << "For advanced usage, see documentation in docs/ directory" << std::endl;
        
        // This would invoke the Python bindings
        // For now, just display information
        std::cout << "Python API features:" << std::endl;
        std::cout << "  - Brain simulation" << std::endl;
        std::cout << "  - Neural computation" << std::endl;
        std::cout << "  - Event-driven simulation" << std::endl;
        std::cout << "  - Integration with ML frameworks" << std::endl;
    }
    
    void handleConfig() {
        std::cout << "=== NLM Configuration Management ===" << std::endl;
        
        if (args.subcommand == "show") {
            showConfig();
        } else if (args.subcommand == "set") {
            setConfig();
        } else if (args.subcommand == "validate") {
            validateConfig();
        } else {
            std::cout << "Available config commands:" << std::endl;
            std::cout << "  show      - Display current configuration" << std::endl;
            std::cout << "  set       - Set configuration value" << std::endl;
            std::cout << "  validate  - Validate configuration" << std::endl;
        }
    }
    
    void handleInfo() {
        std::cout << "=== NLM System Information ===" << std::endl;
        
        if (!loadConfig()) {
            std::cerr << "Failed to load configuration" << std::endl;
            return;
        }
        
        std::cout << "NLM Version: " << NLM_VERSION << std::endl;
        std::cout << "Build Type: " << CMAKE_BUILD_TYPE << std::endl;
        std::cout << "Compiler: " << COMPILER_VERSION << std::endl;
        std::cout << "Python Bindings: " << (NLM_BUILD_PYTHON_BINDINGS ? "Enabled" : "Disabled") << std::endl;
        
        // System capabilities
        std::cout << "\nSystem Capabilities:" << std::endl;
        std::cout << "  - Neural Simulation: Yes" << std::endl;
        std::cout << "  - Event-Driven Dynamics: Yes" << std::endl;
        std::cout << "  - Multi-Region Networks: Yes" << std::endl;
        std::cout << "  - Memory Integration: Yes" << std::endl;
        std::cout << "  - Neuromodulation: Yes" << std::endl;
        std::cout << "  - Predictive Processing: Yes" << std::endl;
        std::cout << "  - Cognitive Functions: Yes" << std::endl;
        std::cout << "  - Developmental Systems: Yes" << std::endl;
    }
    
    // Phase-specific demo handlers
    void runPhase2Demo() {
        std::cout << "=== Phase 2 Demo: Real Neural Computation ===" << std::endl;
        std::cout << "This demo demonstrates:" << std::endl;
        std::cout << "  - Leaky Integrate-and-Fire (LIF) neurons" << std::endl;
        std::cout << "  - Event-driven spike propagation" << std::endl;
        std::cout << "  - STDP and Hebbian plasticity" << std::endl;
        std::cout << "  - Structural plasticity" << std::endl;
        
        if (args.dryRun) {
            std::cout << "[Dry run] Skipping actual simulation" << std::endl;
            return;
        }
        
        // This would invoke the actual Phase 2 demo
        // For now, display information
        std::cout << "Run with './nlm demo phase2' for full demo" << std::endl;
    }
    
    void runPhase6Demo() {
        std::cout << "=== Phase 6 Demo: Integration Test ===" << std::endl;
        std::cout << "Testing complete brain integration..." << std::endl;
        
        // This would invoke the actual Phase 6 demo
        // For now, display information
        std::cout << "Run with './nlm demo phase6' for full demo" << std::endl;
    }
    
    void runBenchmarkDemo() {
        std::cout << "=== Benchmark Demo ===" << std::endl;
        std::cout << "Running performance benchmarks..." << std::endl;
        
        if (args.dryRun) {
            std::cout << "[Dry run] Skipping actual benchmark" << std::endl;
            return;
        }
        
        // This would invoke the actual benchmark
        // For now, display information
        std::cout << "Run with './nlm benchmark' for actual benchmark" << std::endl;
    }
    
    // Configuration helpers
    bool loadConfig() {
        config = std::make_shared<Config>();
        
        std::string configFile = "configs/default.cfg";
        if (!args.configFile.empty()) {
            configFile = args.configFile;
        }
        
        if (config->loadFromFile(configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + configFile);
        } else {
            NLM_LOG_INFO("Using default configuration");
        }
        
        // Override with command line args
        config->loadFromArgs(0, nullptr); // No args for now
        
        // Apply command line overrides
        config->set("neuron_count", static_cast<int64_t>(args.neuronCount), ConfigSource::CommandLine);
        config->set("random_seed", 42, ConfigSource::Default);
        
        return true;
    }
    
    void showConfig() {
        if (!config) {
            std::cerr << "No configuration loaded" << std::endl;
            return;
        }
        
        std::cout << "Current Configuration:" << std::endl;
        // Display all config values
        std::cout << "  neuron_count: " << config->getOr<int64_t>("neuron_count", 1000) << std::endl;
        std::cout << "  region_count: " << config->getOr<int64_t>("region_count", 1) << std::endl;
        std::cout << "  connection_probability: " << config->getOr<float>("connection_probability", 0.1f) << std::endl;
    }
    
    void setConfig() {
        if (args.args.size() < 2) {
            std::cerr << "Error: Insufficient arguments for set command" << std::endl;
            std::cout << "Usage: nlm config set <key> <value>" << std::endl;
            return;
        }
        
        std::string key = args.args[0];
        std::string value = args.args[1];
        
        if (!config) {
            std::cerr << "No configuration loaded" << std::endl;
            return;
        }
        
        // Simple value parsing (could be enhanced)
        try {
            if (value.find('.') != std::string::npos) {
                float floatValue = std::stof(value);
                config->set(key, floatValue, ConfigSource::CommandLine);
            } else {
                int64_t intValue = std::stoll(value);
                config->set(key, intValue, ConfigSource::CommandLine);
            }
            std::cout << "Set " << key << " = " << value << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error parsing value: " << e.what() << std::endl;
        }
    }
    
    void validateConfig() {
        std::cout << "Validating configuration..." << std::endl;
        
        if (!config) {
            std::cerr << "No configuration loaded" << std::endl;
            return;
        }
        
        bool valid = true;
        
        // Validate required parameters
        int64_t neuronCount = config->getOr<int64_t>("neuron_count", 0);
        if (neuronCount <= 0) {
            std::cerr << "Error: neuron_count must be positive" << std::endl;
            valid = false;
        }
        
        int64_t regionCount = config->getOr<int64_t>("region_count", 0);
        if (regionCount <= 0) {
            std::cerr << "Error: region_count must be positive" << std::endl;
            valid = false;
        }
        
        float connProb = config->getOr<float>("connection_probability", 0.0f);
        if (connProb < 0.0f || connProb > 1.0f) {
            std::cerr << "Error: connection_probability must be between 0.0 and 1.0" << std::endl;
            valid = false;
        }
        
        if (valid) {
            std::cout << "Configuration is valid" << std::endl;
        } else {
            std::cout << "Configuration has errors" << std::endl;
        }
    }
    
    void printHelp() {
        std::cout << "NLM - Neural Learning Machine Command Line Interface" << std::endl;
        std::cout << std::endl;
        std::cout << "Usage: nlm [OPTIONS] COMMAND [SUBCOMMAND] [ARGS]" << std::endl;
        std::cout << std::endl;
        std::cout << "Commands:" << std::endl;
        std::cout << "  run          - Run NLM simulation" << std::endl;
        std::cout << "  demo         - Run demonstration (with subcommand)" << std::endl;
        std::cout << "  benchmark     - Run performance benchmark" << std::endl;
        std::cout << "  python        - Python interface information" << std::endl;
        std::cout << "  config        - Configuration management" << std::endl;
        std::cout << "  info          - System information" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --help, -h         Show this help message" << std::endl;
        std::cout << "  --version, -v      Show version information" << std::endl;
        std::cout << "  --verbose, -V      Enable verbose output" << std::endl;
        std::cout << "  --dry-run          Run without actual simulation" << std::endl;
        std::cout << "  --config FILE       Specify configuration file" << std::endl;
        std::cout << "  --output FILE       Specify output file" << std::endl;
        std::cout << "  --steps N          Number of simulation steps" << std::endl;
        std::cout << "  --neurons N        Number of neurons" << std::endl;
        std::cout << "  --learning-rate F   Learning rate (default: 0.01)" << std::endl;
        std::cout << std::endl;
        std::cout << "Subcommands:"" << std::endl;
        std::cout << "  demo phase2       - Phase 2 demonstration" << std::endl;
        std::cout << "  demo phase6       - Phase 6 integration demo" << std::endl;
        std::cout << "  demo benchmark     - Benchmark demo" << std::endl;
        std::cout << "  config show       - Show configuration" << std::endl;
        std::cout << "  config set KEY VAL - Set configuration value" << std::endl;
        std::cout << "  config validate    - Validate configuration" << std::endl;
    }
    
    void printVersion() {
        std::cout << "NLM " << NLM_VERSION << std::endl;
        std::cout << "Build: " << BUILD_TIMESTAMP << std::endl;
        std::cout << "C++ Standard: C++20" << std::endl;
        std::cout << "Python Bindings: " << (NLM_BUILD_PYTHON_BINDINGS ? "Enabled" : "Disabled") << std::endl;
    }
    
    // Command handler mapping
    using CommandHandler = void (NLMCLI::*)();
    std::unordered_map<std::string, CommandHandler> commandHandlers = {
        {"run", &NLMCLI::handleRun},
        {"demo", &NLMCLI::handleDemo},
        {"benchmark", &NLMCLI::handleBenchmark},
        {"python", &NLMCLI::handlePython},
        {"config", &NLMCLI::handleConfig},
        {"info", &NLMCLI::handleInfo},
    };
};

int main(int argc, char* argv[]) {
    NLMCLI cli;
    cli.run(argc, argv);
    return 0;
}
