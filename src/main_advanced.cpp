// NLM Advanced Example
// Demonstrates new utilities and advanced features

#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "nlm/Utilities.hpp"
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>

void demonstrateConfigUtilities() {
    std::cout << "=== Config Utilities Demo ===" << std::endl;
    
    nlm::Config config;
    
    // Set up a basic configuration
    config.set("random_seed", static_cast<int64_t>(42));
    config.set("neuron_count", static_cast<int64_t>(500));
    config.set("simulation_timestep", 0.001);
    config.set("connection_probability", 0.15f);
    config.set("stdp_ltp_weight", 0.02f);
    config.set("stdp_ltd_weight", 0.015f);
    config.set("brain_name", std::string("MyAdvancedBrain"));
    
    std::cout << "Original config:" << std::endl;
    std::cout << config.summary();
    
    // Export to JSON
    if (nlm::ConfigUtilities::exportToJSON(config, "advanced_config.json")) {
        std::cout << "✓ Config exported to JSON" << std::endl;
    }
    
    // Export to YAML
    if (nlm::ConfigUtilities::exportToYAML(config, "advanced_config.yaml")) {
        std::cout << "✓ Config exported to YAML" << std::endl;
    }
    
    // Import from JSON (create a new config for this)
    nlm::Config config2;
    if (nlm::ConfigUtilities::importFromJSON(config2, "advanced_config.json")) {
        std::cout << "✓ Config imported from JSON" << std::endl;
        
        // Compare configs
        if (nlm::ConfigUtilities::compareConfigs(config, config2)) {
            std::cout << "✓ Configs are identical" << std::endl;
        } else {
            std::cout << "✗ Configs differ" << std::endl;
        }
        
        // Get config difference score
        double similarity = nlm::ConfigUtilities::configDifferenceScore(config, config2);
        std::cout << "Config similarity score: " << similarity << std::endl;
    }
    
    // Validate config
    auto validationErrors = nlm::ConfigUtilities::validateConfig(config);
    if (validationErrors.empty()) {
        std::cout << "✓ Config validation passed" << std::endl;
    } else {
        std::cout << "Config validation errors:" << std::endl;
        for (const auto& error : validationErrors) {
            std::cout << "  - " << error << std::endl;
        }
    }
    
    // Clean up
    std::remove("advanced_config.json");
    std::remove("advanced_config.yaml");
}

void demonstrateBrainUtilities() {
    std::cout << "\n=== Brain Utilities Demo ===" << std::endl;
    
    // Create a simple configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", static_cast<int64_t>(42));
    config->set("neuron_count", static_cast<int64_t>(100));
    config->set("region_count", static_cast<int64_t>(2));
    config->set("connection_probability", 0.1f);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    if (brain->initialize()) {
        std::cout << "✓ Brain initialized successfully" << std::endl;
        
        // Simulate some activity
        for (int step = 0; step < 100; ++step) {
            brain->step(step, step * 0.001);
        }
        
        // Demonstrate brain utilities
        std::cout << nlm::BrainUtilities::analyzeBrainState(brain);
        
        std::cout << nlm::BrainUtilities::getBrainHealthReport(brain);
        
        std::cout << nlm::BrainUtilities::getPerformanceMetrics(brain);
        
        // Validate brain
        auto brainErrors = nlm::BrainUtilities::validateBrain(brain);
        if (brainErrors.empty()) {
            std::cout << "✓ Brain validation passed" << std::endl;
        } else {
            std::cout << "Brain validation errors:" << std::endl;
            for (const auto& error : brainErrors) {
                std::cout << "  - " << error << std::endl;
            }
        }
        
        // Check integrity
        if (nlm::BrainUtilities::checkBrainIntegrity(brain)) {
            std::cout << "✓ Brain integrity check passed" << std::endl;
        } else {
            std::cout << "✗ Brain integrity check failed" << std::endl;
        }
        
        if (nlm::BrainUtilities::checkMemoryIntegrity(brain)) {
            std::cout << "✓ Memory integrity check passed" << std::endl;
        } else {
            std::cout << "✗ Memory integrity check failed" << std::endl;
        }
    } else {
        std::cout << "✗ Brain initialization failed" << std::endl;
    }
}

void demonstratePerformanceUtilities() {
    std::cout << "\n=== Performance Utilities Demo ===" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", static_cast<int64_t>(42));
    config->set("neuron_count", static_cast<int64_t>(50));
    
    auto brain = std::make_shared<nlm::Brain>(config);
    if (brain->initialize()) {
        // Start profiler
        nlm::PerformanceUtilities::startProfiler("brain_simulation");
        
        // Run simulation
        auto startTime = std::chrono::high_resolution_clock::now();
        for (int step = 0; step < 500; ++step) {
            brain->step(step, step * 0.001);
        }
        auto endTime = std::chrono::high_resolution_clock::now();
        
        // Stop profiler
        nlm::PerformanceUtilities::stopProfiler("brain_simulation");
        
        // Print profiler report
        nlm::PerformanceUtilities::printProfilerReport();
        
        // Measure time with utility function
        auto elapsed = nlm::PerformanceUtilities::measureTime([brain]() {
            for (int step = 0; step < 100; ++step) {
                brain->step(step, step * 0.001);
            }
        });
        
        std::cout << "Measured time for 100 steps: " << elapsed.count() << " seconds" << std::endl;
        
        // Log memory usage
        nlm::PerformanceUtilities::logMemoryUsage("Brain simulation");
        
        // Run benchmark
        nlm::PerformanceUtilities::runBenchmark("brain_step", [brain]() {
            brain->step(0, 0.001);
        }, 50);
    }
}

void demonstrateFileUtilities() {
    std::cout << "\n=== File Utilities Demo ===" << std::endl;
    
    // Create test files
    const std::string testDir = "test_data";
    const std::string testFile = "test_file.txt";
    const std::string testContent = "Hello, NLM Advanced Features!\nThis is a test file.\nLine 3.\n";
    
    // Test file operations
    if (nlm::FileUtilities::writeFile(testFile, testContent)) {
        std::cout << "✓ File written successfully" << std::endl;
        
        if (nlm::FileUtilities::fileExists(testFile)) {
            std::cout << "✓ File exists" << std::endl;
            
            std::string readContent = nlm::FileUtilities::readFile(testFile);
            if (readContent == testContent) {
                std::cout << "✓ File content matches" << std::endl;
            } else {
                std::cout << "✗ File content mismatch" << std::endl;
            }
            
            std::vector<std::string> lines = nlm::FileUtilities::readLines(testFile);
            std::cout << "File has " << lines.size() << " lines" << std::endl;
        }
        
        // Test directory operations
        if (nlm::FileUtilities::createDirectory(testDir)) {
            std::cout << "✓ Directory created" << std::endl;
            
            std::string absPath = nlm::FileUtilities::getAbsolutePath(testFile);
            std::cout << "Absolute path: " << absPath << std::endl;
        }
        
        // Test file copying
        const std::string copyFile = "test_file_copy.txt";
        if (nlm::FileUtilities::copyFile(testFile, copyFile)) {
            std::cout << "✓ File copied successfully" << std::endl;
            std::remove(copyFile.c_str());
        }
        
        // Clean up
        std::remove(testFile.c_str());
        nlm::FileUtilities::deleteFile(testFile); // Should still be gone
    }
}

void demonstrateCommandLineUtilities() {
    std::cout << "\n=== Command Line Utilities Demo ===" << std::endl;
    
    // Simulate command-line arguments
    std::vector<std::string> args = {"nlm_advanced", "--config", "config.json", "--verbose", "--neurons", "500"};
    
    if (nlm::CommandLineUtilities::hasArgument(args, "--verbose")) {
        std::cout << "✓ Verbose mode enabled" << std::endl;
    }
    
    std::string configFile = nlm::CommandLineUtilities::getArgumentValue(args, "--config");
    if (!configFile.empty()) {
        std::cout << "✓ Config file: " << configFile << std::endl;
    }
    
    int neuronCount;
    if (nlm::CommandLineUtilities::validateInteger(nlm::CommandLineUtilities::getArgumentValue(args, "--neurons"), neuronCount, 1, 1000000)) {
        std::cout << "✓ Valid neuron count: " << neuronCount << std::endl;
    }
    
    // Test help functionality
    std::cout << "\nExample help output:" << std::endl;
    nlm::CommandLineUtilities::printHelp("nlm_advanced", "NLM Advanced Brain Simulation Framework");
}

void demonstrateLoggerUtilities() {
    std::cout << "\n=== Logger Utilities Demo ===" << std::endl;
    
    // Set up logging to file
    if (nlm::LoggingUtilities::setupLogFile("nlm_advanced.log")) {
        std::cout << "✓ Log file setup successful" << std::endl;
        
        // Log some messages
        nlm::Logger::getGlobal().info("Starting advanced demo");
        nlm::Logger::getGlobal().debug("Debug message for testing");
        nlm::Logger::getGlobal().warning("This is a warning message");
        nlm::Logger::getGlobal().error("This is an error message");
        
        // Log performance
        auto startTime = std::chrono::high_resolution_clock::now();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        nlm::LoggingUtilities::logPerformance("Sleep operation", duration);
        
        // Analyze log file
        std::string analysis = nlm::LoggingUtilities::analyzeLogFile("nlm_advanced.log");
        std::cout << "Log analysis:\n" << analysis;
        
        std::string errors = nlm::LoggingUtilities::extractLogErrors("nlm_advanced.log");
        if (!errors.empty()) {
            std::cout << "Errors found:\n" << errors;
        }
        
        // Clean up
        std::remove("nlm_advanced.log");
    }
}

int main() {
    std::cout << "=== NLM Advanced Utilities Demo ===" << std::endl;
    std::cout << "Demonstrating new utilities and advanced features" << std::endl;
    
    // Initialize logger
    auto& logger = nlm::Logger::getGlobal();
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    consoleLogger->setUseColors(true);
    logger.addLogger(consoleLogger);
    
    // Run demonstrations
    demonstrateConfigUtilities();
    demonstrateBrainUtilities();
    demonstratePerformanceUtilities();
    demonstrateFileUtilities();
    demonstrateCommandLineUtilities();
    demonstrateLoggerUtilities();
    
    std::cout << "\n=== Demo completed successfully ===" << std::endl;
    std::cout << "All utilities are working correctly!" << std::endl;
    
    return 0;
}
