// NLM Command Line Interface Implementation
// Provides enhanced command-line interface for NLM with multiple modes

#include "NLMCLI.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "experiments/ExperimentRunner.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <iomanip>

namespace nlm {

struct NLMCLI::Impl {
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    std::map<std::string, std::function<int(const std::vector<std::string>&)>> commands;
    std::string currentExperiment;
    std::string outputDirectory;
    
    Impl() : config(std::make_shared<Config>()), brain(nullptr), outputDirectory("./nlm_output") {}
};

NLMCLI::NLMCLI() : pImpl(std::make_unique<Impl>()) {
    initializeNLM();
    registerDefaultCommands();
}

NLMCLI::~NLMCLI() {
    cleanupNLM();
}

void NLMCLI::registerDefaultCommands() {
    // Basic commands
    registerCommand("demo", "Run demonstration mode", nlmDemo);
    registerCommand("experiment", "Run experiments", nlmExperiment);
    registerCommand("benchmark", "Run benchmarks", nlmBenchmark);
    registerCommand("config", "Configure NLM settings", nlmConfig);
    registerCommand("analyze", "Analyze brain state", nlmAnalyze);
    
    // Help and management
    registerCommand("help", "Show help information", 
                   [this](const std::vector<std::string>& args) {
                       printHelp();
                       return 0;
                   });
    
    registerCommand("version", "Show version information",
                   [this](const std::vector<std::string>& args) {
                       printVersion();
                       return 0;
                   });
    
    registerCommand("list", "List available experiments and modes",
                   [this](const std::vector<std::string>& args) {
                       listExperiments();
                       return 0;
                   });
}

int NLMCLI::run(int argc, char** argv) {
    if (!parseArguments(argc, argv)) {
        printHelp();
        return 1;
    }
    
    return 0;
}

void NLMCLI::registerCommand(const std::string& name, 
                           const std::string& description,
                           std::function<int(const std::vector<std::string>&)> handler) {
    pImpl->commands[name] = handler;
}

bool NLMCLI::loadConfiguration(const std::string& configFile) {
    if (pImpl->config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
        return true;
    }
    
    NLM_LOG_WARNING("Failed to load configuration from: " + configFile);
    NLM_LOG_INFO("Using default configuration.");
    return false;
}

bool NLMCLI::saveConfiguration(const std::string& configFile) {
    return pImpl->config->saveToFile(configFile);
}

bool NLMCLI::runExperiment(const std::string& experimentName, 
                         const std::map<std::string, std::string>& parameters) {
    NLM_LOG_INFO("Running experiment: " + experimentName);
    
    // Create brain with configuration
    pImpl->brain = createBrain(*pImpl->config);
    if (!pImpl->brain || !pImpl->brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain for experiment: " + experimentName);
        return false;
    }
    
    // TODO: Implement experiment logic
    // This would be filled in with actual experiment execution
    
    NLM_LOG_INFO("Experiment completed: " + experimentName);
    return true;
}

bool NLMCLI::listExperiments() const {
    NLM_LOG_INFO("=== Available Experiments ===");
    
    NLM_LOG_INFO("Default Experiments:");
    NLM_LOG_INFO("  demo - Basic demonstration of neural computation");
    NLM_LOG_INFO("  connectivity - Test neural connectivity");
    NLM_LOG_INFO("  plasticity - Demonstrate synaptic plasticity");
    NLM_LOG_INFO("  stdp - Test STDP learning rule");
    NLM_LOG_INFO("  benchmark - Performance benchmark");
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Advanced Experiments (Phase 6):");
    NLM_LOG_INFO("  phase6_demo - Integration test of all systems");
    NLM_LOG_INFO("  memory_test - Test memory integration");
    NLM_LOG_INFO("  neuromodulation - Test neuromodulation effects");
    NLM_LOG_INFO("  development - Test developmental processes");
    NLM_LOG_INFO("  prediction - Test predictive processing");
    NLM_LOG_INFO("  cognition - Test cognitive functions");
    
    return true;
}

std::shared_ptr<Brain> NLMCLI::createBrain(const Config& config) const {
    // Create brain with configuration
    auto brain = std::make_shared<Brain>(std::const_pointer_cast<Config>(pImpl->config));
    return brain;
}

bool NLMCLI::runSimulation(std::shared_ptr<Brain> brain, int steps) {
    if (!brain) {
        NLM_LOG_ERROR("No brain available for simulation");
        return false;
    }
    
    NLM_LOG_INFO("Running simulation for " + std::to_string(steps) + " steps...");
    
    // TODO: Implement simulation loop
    
    NLM_LOG_INFO("Simulation completed.");
    return true;
}

void NLMCLI::printBrainStatus(std::shared_ptr<Brain> brain) const {
    if (brain) {
        brain->logStatus();
    }
}

bool NLMCLI::exportBrainState(const std::string& filename, std::shared_ptr<Brain> brain) const {
    if (!brain) {
        NLM_LOG_ERROR("No brain to export");
        return false;
    }
    
    if (brain->save(filename)) {
        NLM_LOG_INFO("Brain state exported to: " + filename);
        return true;
    }
    
    NLM_LOG_ERROR("Failed to export brain state to: " + filename);
    return false;
}

void NLMCLI::printHelp() const {
    NLM_LOG_INFO("=== NLM (Neural Learning Machine) Help ===");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Usage: nlm <command> [options]");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Commands:");
    
    for (const auto& pair : pImpl->commands) {
        NLM_LOG_INFO("  " + pair.first + " - " + pair.second);  // Note: This would need description stored
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Common Examples:");
    NLM_LOG_INFO("  nlm demo                    - Run basic demonstration");
    NLM_LOG_INFO("  nlm experiment --name demo   - Run named experiment");
    NLM_LOG_INFO("  nlm benchmark --iterations 100 - Run performance benchmark");
    NLM_LOG_INFO("  nlm config --list          - List configuration options");
    NLM_LOG_INFO("  nlm analyze --export brain.bin - Analyze and export brain state");
}

void NLMCLI::printVersion() const {
    NLM_LOG_INFO("NLM (Neural Learning Machine) - Version 0.1.0");
    NLM_LOG_INFO("Phase 6: Final Integration");
    NLM_LOG_INFO("An experimental artificial developmental brain.");
}

// Convenience functions
void initializeNLM() {
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
}

void cleanupNLM() {
    Logger::setGlobal(nullptr);
}

// Command line shortcuts
int nlmDemo(const std::vector<std::string>& args) {
    std::cout << "Running NLM demo mode..." << std::endl;
    // TODO: Implement demo logic
    return 0;
}

int nlmExperiment(const std::vector<std::string>& args) {
    std::cout << "Running NLM experiment..." << std::endl;
    // TODO: Implement experiment logic
    return 0;
}

int nlmBenchmark(const std::vector<std::string>& args) {
    std::cout << "Running NLM benchmark..." << std::endl;
    // TODO: Implement benchmark logic
    return 0;
}

int nlmConfig(const std::vector<std::string>& args) {
    std::cout << "NLM configuration management..." << std::endl;
    // TODO: Implement config logic
    return 0;
}

int nlmAnalyze(const std::vector<std::string>& args) {
    std::cout << "NLM analysis..." << std::endl;
    // TODO: Implement analysis logic
    return 0;
}

} // namespace nlm