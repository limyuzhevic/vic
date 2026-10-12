// nlm_cli.cpp - Main CLI implementation for NLM framework

#include "cli.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "environment/SimpleWorld.hpp"
#include <CLI11.hpp>
#include <fstream>
#include <iostream>
#include <chrono>
#include <filesystem>
#include <algorithm>

namespace nlm {

class CLIAppImpl : public CLIApp {
private:
    CLI11::App cliApp;
    CLIConfig config;
    CLIResult result;
    std::unique_ptr<CLI11::Subcommand> currentSubcommand;

public:
    CLIAppImpl() {
        // Create main application
        cliApp.setName("nlm");
        cliApp.setDescription("NLM (熙然) - Neural Learning Machine Framework");
        cliApp.setVersion(std::string("NLM ") + "0.1.0");
        
        // Global options
        cliApp.add_option("-c", config.configFile, "Configuration file path");
        cliApp.add_option("--config", config.configFile, "Configuration file path");
        cliApp.add_option("--steps", config.steps, "Number of simulation steps");
        cliApp.add_option("--time", config.timeLimit, "Time limit in seconds");
        cliApp.add_option("--pause", config.pauseDuration, "Pause duration in seconds");
        cliApp.add_option("--output", config.outputFile, "Output file path");
        cliApp.add_flag("--verbose", config.verbose, "Enable verbose output");
        cliApp.add_flag("--quiet", config.quiet, "Suppress output");
        cliApp.add_flag("--debug", config.debug, "Enable debug mode");
        cliApp.add_flag("--test", config.test, "Run in test mode");
        cliApp.add_flag("--validate", config.validate, "Validate configuration only");
        
        // Subcommands
        auto* runCmd = cliApp.add_subcommand("run", "Run a simulation");
        runCmd->add_option("--config", config.configFile, "Configuration file");
        runCmd->add_option("--steps", config.steps, "Number of steps");
        runCmd->add_option("--output", config.outputFile, "Output file");
        runCmd->add_option("--pause", config.pauseDuration, "Pause duration");
        
        auto* createCmd = cliApp.add_subcommand("create", "Create new configuration");
        createCmd->add_option("--type", config.createType, "Configuration type (brain or world)");
        createCmd->add_option("--output", config.createOutput, "Output file path");
        
        auto* validateCmd = cliApp.add_subcommand("validate", "Validate configuration file");
        validateCmd->add_option("--config", config.configFile, "Configuration file");
        validateCmd->add_flag("--strict", config.strictValidation, "Enable strict validation");
        
        auto* infoCmd = cliApp.add_subcommand("info", "Display information");
        infoCmd->add_option("--config", config.configFile, "Configuration file");
        infoCmd->add_option("--brain", config.brainFile, "Brain file");
        infoCmd->add_option("--field", config.infoField, "Field to display (default: all)");
        
        auto* benchmarkCmd = cliApp.add_subcommand("benchmark", "Run performance benchmark");
        benchmarkCmd->add_option("--config", config.configFile, "Configuration file");
        benchmarkCmd->add_option("--duration", config.benchmarkDuration, "Benchmark duration in seconds");
        benchmarkCmd->add_option("--iterations", config.benchmarkIterations, "Number of iterations");
        
        // Set up subcommand callbacks
        runCmd->callback([this]() { result.success = executeRun(); });
        createCmd->callback([this]() { result.success = executeCreate(); });
        validateCmd->callback([this]() { result.success = executeValidate(); });
        infoCmd->callback([this]() { result.success = executeInfo(); });
        benchmarkCmd->callback([this]() { result.success = executeBenchmark(); });
    }
    
    void addSubcommand(const std::string& name, const std::string& description, std::function<bool(const CLIConfig&)> func) override {
        // Subcommands are added in constructor for simplicity
    }
    
    void addOption(const std::string& name, const std::string& description, std::function<void(CLIConfig&)> func) override {
        // Options are added in constructor for simplicity
    }
    
    bool parse(int argc, char* argv[]) override {
        try {
            cliApp.parse(argc, argv);
            return true;
        } catch (const CLI11::ParseError& e) {
            std::cerr << cliApp.help() << std::endl;
            return false;
        }
    }
    
    int run() override {
        if (!result.success) {
            return 1;
        }
        return 0;
    }
    
    const CLIConfig& getConfig() const override {
        return config;
    }
    
    const CLIResult& getResult() const override {
        return result;
    }
    
private:
    bool executeRun() {
        NLM_LOG_INFO("=== Running NLM Simulation ===");
        
        // Load configuration
        auto nlmConfig = std::make_shared<Config>();
        
        if (!config.configFile.empty()) {
            if (!nlmConfig->loadFromFile(config.configFile)) {
                result.errorType = CLIErrorType::InvalidFormat;
                result.errorMessage = "Failed to load configuration from: " + config.configFile;
                return false;
            }
            NLM_LOG_INFO("Configuration loaded from: " + config.configFile);
        }
        
        // Override with command line options
        if (config.steps > 0) {
            nlmConfig->set("max_simulation_steps", static_cast<int64_t>(config.steps), ConfigSource::CommandLine);
        }
        
        // Create brain
        auto brain = std::make_shared<Brain>(nlmConfig);
        
        if (!brain->initialize()) {
            result.errorType = CLIErrorType::ConfigurationError;
            result.errorMessage = "Failed to initialize brain";
            return false;
        }
        
        // Run simulation
        NLM_LOG_INFO("Starting simulation with " + std::to_string(config.steps) + " steps");
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (int step = 0; step < config.steps; ++step) {
            brain->step(step, step * nlmConfig->getOr<double>("simulation_timestep", 0.001));
            
            if (config.pauseDuration > 0.0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(config.pauseDuration * 1000)));
            }
            
            if (config.verbose && step % 100 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + ": " + 
                    std::to_string(brain->getTotalSpikeCount()) + " spikes");
            }
            
            if (config.timeLimit > 0.0) {
                auto currentTime = std::chrono::high_resolution_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();
                if (elapsed >= config.timeLimit) {
                    NLM_LOG_INFO("Time limit reached, stopping simulation");
                    break;
                }
            }
        }
        
        // Save output if requested
        if (!config.outputFile.empty()) {
            brain->save(config.outputFile);
            NLM_LOG_INFO("Brain state saved to: " + config.outputFile);
        }
        
        NLM_LOG_INFO("Simulation completed successfully");
        return true;
    }
    
    bool executeCreate() {
        NLM_LOG_INFO("=== Creating NLM Configuration ===");
        
        if (config.createType.empty()) {
            result.errorType = CLIErrorType::CommandError;
            result.errorMessage = "Configuration type must be specified (--type brain or --type world)";
            return false;
        }
        
        return CLIUtils::createConfigFile(config.createType, config.createOutput, config.createOptions);
    }
    
    bool executeValidate() {
        NLM_LOG_INFO("=== Validating NLM Configuration ===");
        
        if (config.configFile.empty()) {
            result.errorType = CLIErrorType::CommandError;
            result.errorMessage = "Configuration file must be specified (--config)";
            return false;
        }
        
        CLIResult validationResult = CLIUtils::validateConfig(config.configFile, config.strictValidation);
        
        if (!validationResult.success) {
            result.errorType = validationResult.errorType;
            result.errorMessage = validationResult.errorMessage;
            result.validationErrors = validationResult.validationErrors;
            result.validationWarnings = validationResult.validationWarnings;
            return false;
        }
        
        NLM_LOG_INFO("Configuration validation completed");
        return true;
    }
    
    bool executeInfo() {
        NLM_LOG_INFO("=== Displaying NLM Information ===");
        
        if (!config.configFile.empty()) {
            CLIResult infoResult = CLIUtils::displayConfigInfo(config.configFile, config.infoField);
            result.output = infoResult.output;
        } else if (!config.brainFile.empty()) {
            CLIResult infoResult = CLIUtils::displayBrainInfo(config.brainFile);
            result.output = infoResult.output;
        } else {
            result.errorType = CLIErrorType::CommandError;
            result.errorMessage = "Either --config or --brain must be specified";
            return false;
        }
        
        return true;
    }
    
    bool executeBenchmark() {
        NLM_LOG_INFO("=== Running NLM Benchmark ===");
        
        auto nlmConfig = std::make_shared<Config>();
        
        if (!config.configFile.empty()) {
            if (!nlmConfig->loadFromFile(config.configFile)) {
                result.errorType = CLIErrorType::InvalidFormat;
                result.errorMessage = "Failed to load configuration from: " + config.configFile;
                return false;
            }
        }
        
        auto brain = std::make_shared<Brain>(nlmConfig);
        
        if (!brain->initialize()) {
            result.errorType = CLIErrorType::ConfigurationError;
            result.errorMessage = "Failed to initialize brain for benchmark";
            return false;
        }
        
        NLM_LOG_INFO("Benchmark starting with " + 
            std::to_string(config.benchmarkIterations > 0 ? config.benchmarkIterations : 10) + " iterations");
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (int iteration = 0; iteration < (config.benchmarkIterations > 0 ? config.benchmarkIterations : 10); ++iteration) {
            brain->reset();
            brain->initialize();
            
            for (int step = 0; step < 100; ++step) {
                brain->step(step, step * 0.001);
            }
            
            if ((iteration + 1) % 10 == 0) {
                NLM_LOG_INFO("Completed " + std::to_string(iteration + 1) + " iterations");
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto totalTime = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count();
        
        int iterations = config.benchmarkIterations > 0 ? config.benchmarkIterations : 10;
        double timePerIteration = static_cast<double>(totalTime) / iterations;
        
        NLM_LOG_INFO("Benchmark results:");
        NLM_LOG_INFO("  Total time: " + std::to_string(totalTime) + " seconds");
        NLM_LOG_INFO("  Iterations: " + std::to_string(iterations));
        NLM_LOG_INFO("  Time per iteration: " + std::to_string(timePerIteration) + " seconds");
        
        return true;
    }
};

CLIApp& createCLIApp() {
    static CLIAppImpl app;
    return app;
}

} // namespace nlm