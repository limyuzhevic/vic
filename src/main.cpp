// NLM - Unified Neural Learning Machine
// Single entry point for all Phase demonstrations with enhanced CLI

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <map>

using namespace nlm;

// Forward declarations for demo functions
void printBanner();
void runPhase2Demo(std::shared_ptr<Config> config);
void runPhase3Demo(std::shared_ptr<Config> config);
void runPhase4Demo(std::shared_ptr<Config> config);
void runPhase5Demo(std::shared_ptr<Config> config);
void runPhase6Demo(std::shared_ptr<Config> config);
void runPhase6TextDemo(std::shared_ptr<Config> config);
void runBenchmark(std::shared_ptr<Config> config);
void runCustomExperiment(std::shared_ptr<Config> config);

// CLI argument parsing and help
void printUsage(const char* programName);
void printAvailableModes();
std::string toLowerCase(const std::string& str);
std::string trim(const std::string& str);

// Configuration presets
struct DemoConfig {
    int phase = 2;
    std::string mode = "demo";
    int neuronCount = 500;
    int maxSteps = 1000;
    std::string configFile = "configs/default.cfg";
    bool enableCheckpointing = false;
    bool enableDevelopment = false;
    bool enableCuriosity = true;
    bool enableRewardModulation = true;
    std::string outputFile = "";
    bool verbose = false;
    int stepsPerLog = 100;
    bool batchMode = false;
    int batchIterations = 1;
    bool scriptMode = false;
    std::string scriptFile = "";
};

DemoConfig parseArguments(int argc, char** argv);
void loadConfigFromFile(std::shared_ptr<Config> config, const std::string& filename);

int main(int argc, char** argv) {
    printBanner();
    
    try {
        DemoConfig config = parseArguments(argc, argv);
        
        // Initialize logger
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(config.verbose ? LogLevel::Debug : LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        // Create NLM config
        auto nlmConfig = std::make_shared<Config>();
        
        // Load configuration
        if (config.configFile.empty() || config.configFile == "configs/default.cfg") {
            loadConfigFromFile(nlmConfig, config.configFile);
            NLM_LOG_INFO("Loaded configuration from: " + config.configFile);
        } else {
            NLM_LOG_INFO("Using default configuration.");
        }
        
        // Apply custom configuration
        nlmConfig->set("neuron_count", static_cast<int64_t>(config.neuronCount), ConfigSource::Default);
        nlmConfig->set("simulation_timestep", 0.001, ConfigSource::Default);
        nlmConfig->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        
        // Run selected phase
        switch (config.phase) {
            case 2: runPhase2Demo(nlmConfig); break;
            case 3: runPhase3Demo(nlmConfig); break;
            case 4: runPhase4Demo(nlmConfig); break;
            case 5: runPhase5Demo(nlmConfig); break;
            case 6: runPhase6Demo(nlmConfig); break;
            case 7: runPhase6TextDemo(nlmConfig); break;
            default: 
                NLM_LOG_ERROR("Invalid phase: " + std::to_string(config.phase));
                printUsage(argv[0]);
                return 1;
        }
        
        return 0;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error: " + std::string(e.what()));
        return 1;
    }
}

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然 (Serene Flow)                                  ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6 Unified Interface                                ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    std::cout << "Neural Learning Machine - Phase 6: Final Integration" << std::endl;
    std::cout << "=====================================================" << std::endl << std::endl;
}

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [OPTIONS]" << std::endl << std::endl;
    
    std::cout << "Run NLM Phase demonstrations with enhanced features:" << std::endl << std::endl;
    
    std::cout << "Basic Options:" << std::endl;
    std::cout << "  --phase N           Run Phase N (2-6, or 7 for text demo)" << std::endl;
    std::cout << "  --config FILE       Load configuration from FILE" << std::endl;
    std::cout << "  --neurons N         Number of neurons (default: 500)" << std::endl;
    std::cout << "  --steps N           Maximum simulation steps (default: 1000)" << std::endl;
    std::cout << "  --help              Show this help message" << std::endl << std::endl;
    
    std::cout << "Advanced Options:" << std::endl;
    std::cout << "  --checkpoint        Enable checkpointing" << std::endl;
    std::cout << "  --development       Enable developmental stages" << std::endl;
    std::cout << "  --curiosity         Enable curiosity-driven exploration" << std::endl;
    std::cout << "  --reward-modulation Enable reward modulation" << std::endl;
    std::cout << "  --verbose           Enable verbose logging" << std::endl;
    std::cout << "  --batch N           Run N iterations in batch mode" << std::endl;
    std::cout << "  --script FILE       Load commands from FILE" << std::endl;
    std::cout << "  --output FILE       Save results to FILE" << std::endl << std::endl;
    
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " --phase 6 --neurons 1000" << std::endl;
    std::cout << "  " << programName << " --phase 2 --verbose --checkpoint" << std::endl;
    std::cout << "  " << programName << " --phase 6 --batch 5" << std::endl;
    std::cout << "  " << programName << " --phase 6 --script demo_script.txt" << std::endl << std::endl;
}

void printAvailableModes() {
    std::cout << "Available NLM Phases:" << std::endl;
    std::cout << "  2: Basic Neural Computation (STDP, Hebbian, Structural Plasticity)" << std::endl;
    std::cout << "  3: World Interaction (sensory-motor loop, reward learning)" << std::endl;
    std::cout << "  4: Emerging Cognition (working memory, episodic memory, planning)" << std::endl;
    std::cout << "  5: Performance Optimization (checkpointing, pools, SIMD)" << std::endl;
    std::cout << "  6: Final Integration (all systems combined)" << std::endl;
    std::cout << "  7: Text/Thought Demo (advanced cognitive capabilities)" << std::endl << std::endl;
}

DemoConfig parseArguments(int argc, char** argv) {
    DemoConfig config;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printUsage(argv[0]);
            printAvailableModes();
            exit(0);
        } else if (arg == "--phase" && i + 1 < argc) {
            config.phase = std::stoi(argv[++i]);
        } else if (arg == "--config" && i + 1 < argc) {
            config.configFile = argv[++i];
        } else if (arg == "--neurons" && i + 1 < argc) {
            config.neuronCount = std::stoi(argv[++i]);
        } else if (arg == "--steps" && i + 1 < argc) {
            config.maxSteps = std::stoi(argv[++i]);
        } else if (arg == "--checkpoint") {
            config.enableCheckpointing = true;
        } else if (arg == "--development") {
            config.enableDevelopment = true;
        } else if (arg == "--curiosity") {
            config.enableCuriosity = true;
        } else if (arg == "--reward-modulation") {
            config.enableRewardModulation = true;
        } else if (arg == "--verbose") {
            config.verbose = true;
        } else if (arg == "--batch" && i + 1 < argc) {
            config.batchMode = true;
            config.batchIterations = std::stoi(argv[++i]);
        } else if (arg == "--script" && i + 1 < argc) {
            config.scriptMode = true;
            config.scriptFile = argv[++i];
        } else if (arg == "--output" && i + 1 < argc) {
            config.outputFile = argv[++i];
        } else if (arg == "--version") {
            std::cout << "NLM Phase 6 v0.1.0" << std::endl;
            exit(0);
        } else {
            NLM_LOG_WARNING("Unknown argument: " + arg);
        }
    }
    
    return config;
}

void loadConfigFromFile(std::shared_ptr<Config> config, const std::string& filename) {
    if (filename.empty()) return;
    
    // Simple config file parsing - looks for key=value pairs
    std::ifstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Could not open config file: " + filename);
        return;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#') continue;
        
        size_t pos = line.find('=');
        if (pos == std::string::npos) continue;
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Convert string value to appropriate type based on key
        if (key.find("neuron_count") != std::string::npos) {
            config->set(key, static_cast<int64_t>(std::stoll(value)), ConfigSource::File);
        } else if (key.find("connection_probability") != std::string::npos) {
            config->set(key, std::stof(value), ConfigSource::File);
        } else if (key.find("stdp_ltp_weight") != std::string::npos) {
            config->set(key, std::stof(value), ConfigSource::File);
        } else if (key.find("stdp_ltd_weight") != std::string::npos) {
            config->set(key, std::stof(value), ConfigSource::File);
        }
    }
    
    file.close();
}

// Implementation stubs for demo functions - these would be expanded with full implementations
void runPhase2Demo(std::shared_ptr<Config> config) {
    NLM_LOG_INFO("=== NLM Phase 2: Basic Neural Computation ===");
    NLM_LOG_INFO("Implementing LIF neurons, STDP, Hebbian, and structural plasticity");
    NLM_LOG_INFO("This is the classic neural computation foundation.");
}

void runPhase3Demo(std::shared_ptr<Config> config) {
    NLM_LOG_INFO("=== NLM Phase 3: World Interaction ===");
    NLM_LOG_INFO("Sensory-motor loop with reward prediction and learning.");
    NLM_LOG_INFO("The brain interacts with and learns from the environment.");
}

void runPhase4Demo(std::shared_ptr<Config> config) {
    NLM_LOG_INFO("=== NLM Phase 4: Emerging Cognition ===");
    NLM_LOG_INFO("Working memory, episodic memory, concept formation, planning.");
    NLM_LOG_INFO("Cognitive abilities emerging from neural dynamics.");
}

void runPhase5Demo(std::shared_ptr<Config> config) {
    NLM_LOG_INFO("=== NLM Phase 5: Performance Optimization ===");
    NLM_LOG_INFO("Memory pools, event queues, SIMD vectorization, parallel processing.");
    NLM_LOG_INFO("Optimized for speed and efficiency.");
}

void runPhase6Demo(std::shared_ptr<Config> config) {
    NLM_LOG_INFO("=== NLM Phase 6: Final Integration ===");
    NLM_LOG_INFO("All systems integrated: memory, neuromodulation, prediction, cognition.");
    NLM_LOG_INFO("Complete artificial brain simulation with developmental stages.");
}

void runPhase6TextDemo(std::shared_ptr<Config> config) {
    NLM_LOG_INFO("=== NLM Phase 6 Text/Thought Demo ===");
    NLM_LOG_INFO("Text-as-a-sense, association probes, concept/social probes.");
    NLM_LOG_INFO("Advanced cognitive capabilities with teacher and planner overrides.");
}

void runBenchmark(std::shared_ptr<Config> config) {
    NLM_LOG_INFO("=== NLM Benchmark ===");
    NLM_LOG_INFO("Performance benchmarking and optimization analysis.");
}

void runCustomExperiment(std::shared_ptr<Config> config) {
    NLM_LOG_INFO("=== NLM Custom Experiment ===");
    NLM_LOG_INFO("User-defined experiment configuration.");
}

std::string toLowerCase(const std::string& str) {
    std::string result = str;
    for (char& c : result) {
        c = std::tolower(c);
    }
    return result;
}

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}
