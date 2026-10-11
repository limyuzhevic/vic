// Advanced Example: Using the New CLI and Plugin System

#include "cli/NLMSession.hpp"
#include "cli/CommandLineParser.hpp"
#include "cli/PluginManager.hpp"
#include "plugins/CustomPlasticityPlugin.hpp"
#include "plugins/CustomNeuromodulatorPlugin.hpp"
#include "plugins/CustomSensoryPlugin.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>

void demonstrateAdvancedCLISession() {
    std::cout << "NLM Advanced CLI and Plugin System Demo" << std::endl;
    std::cout << "======================================" << std::endl << std::endl;
    
    // Initialize logging
    auto logger = std::make_shared<nlm::Logger>();
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    logger->addLogger(consoleLogger);
    nlm::Logger::setGlobal(logger);
    
    // Initialize configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<size_t>(500));
    config->set("region_count", static_cast<size_t>(2));
    config->set("random_seed", static_cast<int64_t>(42));
    
    std::cout << "1. Creating Plugin Manager..." << std::endl;
    auto pluginManager = std::make_shared<nlm::cli::PluginManager>(config, logger);
    
    // Configure plugin directory
    pluginManager->setPluginDirectory("./plugins");
    
    std::cout << "\n2. Loading Example Plugins..." << std::endl;
    
    // Load custom plasticity plugin
    std::cout << "   Loading CustomPlasticityPlugin..." << std::endl;
    bool plasticityLoaded = pluginManager->loadPlugin("./plugins/libCustomPlasticityPlugin.so");
    std::cout << "   Result: " << (plasticityLoaded ? "SUCCESS" : "FAILED") << std::endl;
    
    // Load custom neuromodulator plugin
    std::cout << "   Loading CustomNeuromodulatorPlugin..." << std::endl;
    bool neuromodulatorLoaded = pluginManager->loadPlugin("./plugins/libCustomNeuromodulatorPlugin.so");
    std::cout << "   Result: " << (neuromodulatorLoaded ? "SUCCESS" : "FAILED") << std::endl;
    
    // Load custom sensory plugin
    std::cout << "   Loading CustomSensoryPlugin..." << std::endl;
    bool sensoryLoaded = pluginManager->loadPlugin("./plugins/libCustomSensoryPlugin.so");
    std::cout << "   Result: " << (sensoryLoaded ? "SUCCESS" : "FAILED") << std::endl;
    
    std::cout << "\n3. Creating Command Line Parser..." << std::endl;
    auto parser = std::make_shared<nlm::cli::CommandLineParser>(pluginManager, config, logger);
    
    std::cout << "\n4. Creating Advanced NLM Session..." << std::endl;
    auto session = std::make_shared<nlm::cli::NLMSession>(config, logger, parser);
    session->setPluginManager(pluginManager);
    
    std::cout << "\n5. Creating and Initializing Brain..." << std::endl;
    auto brain = std::make_shared<nlm::Brain>(config);
    if (brain->initialize()) {
        std::cout << "   Brain initialized successfully" << std::endl;
        session->initialize(brain);
        
        // Get plugin manager reference
        auto sessionPluginManager = session->getPluginManager();
        if (sessionPluginManager) {
            std::cout << "\n6. Demonstrating Plugin Management..." << std::endl;
            
            // List loaded plugins
            std::cout << "   Loaded plugins:" << std::endl;
            const auto& plugins = sessionPluginManager->getLoadedPlugins();
            for (const auto& pluginName : plugins) {
                std::cout << "     - " << pluginName << std::endl;
                
                // Get plugin info
                std::string info = sessionPluginManager->getPluginInfo(pluginName);
                std::cout << "       Info: " << info.substr(0, 100) << "..." << std::endl;
            }
            
            // Execute plugin function
            std::cout << "\n   Executing plugin function: getPluginSpecificInfo for CustomPlasticityPlugin" << std::endl;
            std::string pluginInfo = sessionPluginManager->executePluginFunction(
                "CustomPlasticityPlugin",
                "getPluginSpecificInfo",
                {}
            );
            std::cout << "       Result: " << pluginInfo.substr(0, 150) << "..." << std::endl;
        }
        
        std::cout << "\n7. Demonstrating Brain State Management..." << std::endl;
        
        // Save brain state
        std::cout << "   Saving brain state to 'test_brain_state.json'..." << std::endl;
        session->saveBrainState("test_brain_state.json");
        
        // Run experiment
        std::cout << "   Running plasticity experiment..." << std::endl;
        session->runExperiment("plasticity", {});
        
        // Load brain state (from save)
        std::cout << "   Loading brain state from 'test_brain_state.json'..." << std::endl;
        if (session->loadBrainState("test_brain_state.json")) {
            std::cout << "   Successfully loaded brain state" << std::endl;
        }
        
        std::cout << "\n8. Demonstrating Performance Monitoring..." << std::endl;
        session->setPerformanceMonitoring(true);
        
        // Start profiling
        std::cout << "   Starting performance profiling..." << std::endl;
        session->startProfiling();
        
        // Do some work
        for (int i = 0; i < 100; ++i) {
            brain->step(i, i * 0.001);
        }
        
        // Stop profiling
        session->stopProfiling();
        double profileTime = session->getProfilingTime();
        std::cout << "   Profiling completed. Duration: " << profileTime << " seconds" << std::endl;
        
        std::cout << "\n9. Demonstrating Batch Processing..." << std::endl;
        
        // Create a batch script
        std::cout << "   Creating batch script 'demo_commands.txt'..." << std::endl;
        std::ofstream batchFile("demo_commands.txt");
        batchFile << "# NLM Advanced CLI Demo Script\n";
        batchFile << "# This script demonstrates batch processing capabilities\n\n";
        batchFile << "# Save current brain state\n";
        batchFile << "save demo_brain_state.json\n\n";
        batchFile << "# Run plasticity experiment\n";
        batchFile << "run plasticity\n\n";
        batchFile << "# Show plugin status\n";
        batchFile << "plugins\n\n";
        batchFile << "# Load different brain state\n";
        batchFile << "load demo_brain_state.json\n";
        batchFile.close();
        
        // Run batch script
        std::cout << "   Running batch script..." << std::endl;
        bool batchSuccess = session->runBatchFile("demo_commands.txt");
        std::cout << "   Batch execution: " << (batchSuccess ? "SUCCESS" : "FAILED") << std::endl;
        
        std::cout << "\n10. Cleanup..." << std::endl;
        
        // Shutdown session
        session->shutdown();
        
        // Clean up plugins
        std::cout << "   Unloading plugins..." << std::endl;
        for (const auto& pluginName : sessionPluginManager->getLoadedPlugins()) {
            std::cout << "     Unloading " << pluginName << "..." << std::endl;
            sessionPluginManager->unloadPlugin(pluginName);
        }
        
        // Shutdown brain
        brain->reset();
    }
    
    std::cout << "\n11. Summary..." << std::endl;
    std::cout << "   The advanced CLI and plugin system demonstration completed successfully!" << std::endl;
    std::cout << "\nKey features demonstrated:" << std::endl;
    std::cout << "   ✓ Dynamic plugin loading and management" << std::endl;
    std::cout << "   ✓ Brain state save/load/reset" << std::endl;
    std::cout << "   ✓ Experiment execution" << std::endl;
    std::cout << "   ✓ Performance monitoring and profiling" << std::endl;
    std::cout << "   ✓ Batch processing and scripting" << std::endl;
    std::cout << "   ✓ Plugin information and statistics" << std::endl;
    std::cout << "   ✓ Error handling and logging" << std::endl;
    
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "Advanced CLI and Plugin System Demo Complete!" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
}

int main() {
    demonstrateAdvancedCLISession();
    return 0;
}
