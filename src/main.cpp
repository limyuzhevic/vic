// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation - REFACTORED VERSION
//
// This phase implements real spiking neural computation with improved modularity,
// command-line interface, and advanced features for expert users.
// Enhanced with comprehensive user feedback and error handling.

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "cli/CLI.hpp"
#include "configmanager/ConfigManager.hpp"
#include "brainmanager/BrainManager.hpp"
#include "experiments/ImprovedExperimentRunner.hpp"
#include "userfeedback/UserFeedback.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <chrono>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace nlm;

// ========== UTILITY FUNCTIONS ==========

void printBanner(const std::string& version = "Phase 2.1 - Enhanced");
void printProgress(const std::string& message, bool force = false);
void printSeparator(const std::string& title = "");
bool fileExists(const std::string& path);
std::string getTimestamp();

// ========== MAIN ENTRY POINT ==========

int main(int argc, char** argv) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        // Parse command line arguments
        CLI::Options options = CLI::parseArgs(argc, argv);
        
        // Handle help/version requests
        if (options.mode == CLI::Mode::HELP) {
            CLI::printHelp();
            return 0;
        }
        if (options.mode == CLI::Mode::VERSION) {
            CLI::printVersion();
            return 0;
        }
        
        // Initialize enhanced user feedback system
        UserFeedback::FeedbackOptions feedbackOptions;
        feedbackOptions.verbose = options.verbose;
        feedbackOptions.quiet = options.quiet;
        feedbackOptions.enableInteractive = true;
        feedbackOptions.useColorOutput = true;
        feedbackOptions.showProgress = true;
        feedbackOptions.showErrors = !options.quiet;
        feedbackOptions.showWarnings = true;
        feedbackOptions.showSuccess = true;
        
        UserFeedback userFeedback(feedbackOptions);
        
        // Initialize logging with error handling
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(options.quiet ? LogLevel::Error : 
                                                            (options.verbose ? LogLevel::Debug : LogLevel::Info));
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        // Register error handlers for user feedback
        userFeedback.registerErrorHandler([&logger](const ErrorInfo& error) {
            // Enhanced error logging with detailed context
            std::string errorCategory = error.category.empty() ? "general" : error.category;
            std::string errorMessage = error.message;
            std::string errorContext = error.context;
            
            // Log through enhanced system
            EnhancedLogger enhancedLogger;
            ErrorLevel logLevel;
            
            switch (error.level) {
                case ErrorLevel::DEBUG: logLevel = ErrorLevel::DEBUG; break;
                case ErrorLevel::INFO: logLevel = ErrorLevel::INFO; break;
                case ErrorLevel::WARNING: logLevel = ErrorLevel::WARNING; break;
                case ErrorLevel::ERROR: logLevel = ErrorLevel::ERROR; break;
                case ErrorLevel::FATAL: logLevel = ErrorLevel::FATAL; break;
                case ErrorLevel::SUCCESS: logLevel = ErrorLevel::INFO; break;
            }
            
            enhancedLogger.log(logLevel, errorCategory, errorMessage, errorContext);
            
            if (error.hasDetails && !error.technicalDetails.empty()) {
                enhancedLogger.debug("Technical details: " + error.technicalDetails, errorCategory, "error_details");
            }
        });
        
        NLM_LOG_INFO("=== NLM Phase 2 Enhanced - Neural Learning Machine ===");
        NLM_LOG_INFO("Started at: " + getTimestamp());
        
        // Start user feedback session with progress tracking
        userFeedback.startSession("NLM Enhanced Phase 2");
        
        // Display startup information
        userFeedback.showInfo("NLM Phase 2: Enhanced Neural Computation");
        userFeedback.showInfo("Loading configuration from: " + options.configFile);
        
        // Load configuration with error handling
        auto config = std::make_shared<Config>();
        bool configLoaded = ConfigManager::loadConfigFromFile(config, options.configFile);
        
        if (configLoaded) {
            userFeedback.showSuccess("Configuration loaded successfully");
        } else {
            userFeedback.showInfo("Using default configuration (file not found)");
        }
        
        // Apply defaults and command-line options
        ConfigManager::applyDefaults(config);
        ConfigManager::applyOptions(config, options);
        
        // Print configuration summary
        ConfigManager::printConfigSummary(config);
        
        // Initialize brain manager with progress feedback
        userFeedback.showProgress("Initializing brain...")
        
        BrainManager brainManager(config);
        
        if (!brainManager.initializeBrain()) {
            userFeedback.showError(
                "Failed to initialize brain!", 
                "Brain initialization", 
                "Check configuration files and system resources"
            );
            return 1;
        }
        
        userFeedback.showSuccess("Brain initialized successfully");
        
        // Display brain status
        auto brain = brainManager.getBrain();
        brain->logStatus();
        
        // Run selected tests based on mode with enhanced progress tracking
        switch (options.mode) {
            case CLI::Mode::CONNECTIVITY:
                printSeparator("Test 1: Basic Neural Connectivity");
                userFeedback.startSession("Connectivity Test");
                brainManager.runConnectivityTest();
                userFeedback.endSession();
                break;
                
            case CLI::Mode::PLASTICITY:
                printSeparator("Test 2: Plasticity Learning Experiment");
                userFeedback.startSession("Plasticity Test");
                brainManager.runPlasticityExperiment();
                userFeedback.endSession();
                break;
                
            case CLI::Mode::STDP:
                printSeparator("Test 3: STDP Verification");
                userFeedback.startSession("STDP Verification Test");
                brainManager.runStdpVerification();
                userFeedback.endSession();
                break;
                
            case CLI::Mode::ALL:
                printSeparator("Running All Tests");
                userFeedback.startSession("Full Test Suite");
                brainManager.runConnectivityTest();
                brainManager.resetBrain();
                brainManager.runPlasticityExperiment();
                brainManager.resetBrain();
                brainManager.runStdpVerification();
                userFeedback.endSession();
                break;
                
            case CLI::Mode::CONFIG:
                printSeparator("Configuration Test");
                userFeedback.startSession("Configuration Test");
                brainManager.runConnectivityTest();
                userFeedback.endSession();
                break;
                
            case CLI::Mode::DEFAULT:
            default:
                printSeparator("Default Test Suite");
                userFeedback.startSession("Standard Test Suite");
                brainManager.runConnectivityTest();
                brainManager.resetBrain();
                brainManager.runPlasticityExperiment();
                brainManager.resetBrain();
                brainManager.runStdpVerification();
                userFeedback.endSession();
                break;
        }
        
        // End user feedback session
        userFeedback.endSession();
        
        // Final brain status
        printSeparator("Final Brain Status");
        brain->logStatus();
        
        // Print performance summary
        if (options.timing) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            userFeedback.showInfo("Total execution time: " + std::to_string(duration.count()) + " ms");
        }
        
        // Save results if requested
        if (options.saveResults) {
            userFeedback.showInfo("Saving results to: " + options.outputDir);
            // Implementation would save detailed results
        }
        
        userFeedback.showSuccess("=== NLM Enhanced Phase Complete ===");
        return 0;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error: " + std::string(e.what()));
        return 1;
    }
}

// ========== UTILITY FUNCTIONS IMPLEMENTATION ==========

void printBanner(const std::string& version) {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Enhanced Neural Computation                     ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.        ║
    ║     This phase implements:                                    ║
    ║     - Real LIF neuron dynamics                                ║
    ║     - Event-driven spike propagation                          ║
    ║     - Enhanced STDP and Hebbian plasticity                   ║
    ║     - Advanced structural plasticity                         ║
    ║     - Integrated memory and cognition                        ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    Version: )" << version << std::endl;
}

void printProgress(const std::string& message, bool force) {
    static int progressCount = 0;
    static auto lastTime = std::chrono::high_resolution_clock::now();
    
    progressCount++;
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTime);
    
    if (force || duration.count() > 1000) { // Every second or forced
        std::cout << "\r[" << message << "] Progress: " << progressCount << "%";
        std::cout.flush();
        lastTime = now;
    }
}

void printSeparator(const std::string& title) {
    std::cout << "\n";
    if (!title.empty()) {
        std::cout << "=" << std::string(60, '=') << std::endl;
        std::cout << title << std::endl;
        std::cout << "=" << std::string(60, '=') << std::endl;
    } else {
        std::cout << "=" << std::string(60, '=') << std::endl;
    }
}

bool fileExists(const std::string& path) {
    std::ifstream file(path);
    return file.good();
}

std::string getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}
