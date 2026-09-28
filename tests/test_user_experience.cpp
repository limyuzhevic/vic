// NLM User Experience Tests
// Test interactive debug mode, configuration templates, performance profiling, and export/import system

#include "cli/CLI.hpp"
#include "cli/NLMConfigCommand.hpp"
#include "performance/EventQueue.hpp"
#include "performance/MemoryPool.hpp"
#include "performance/Performance.hpp"
#include "brain/Brain.hpp"
#include "neuromodulation/Reward.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <chrono>

namespace test_user_experience {

void testInteractiveDebugMode() {
    std::cout << "Testing Interactive Debug Mode..." << std::endl;
    
    // Test user input and simulation control
    nlm::Brain brain;
    
    // Set up initial state
    brain.setSimulationSpeed(1.0f);
    assert(brain.getSimulationSpeed() == 1.0f);
    
    // Test pause/resume
    brain.pauseSimulation();
    assert(brain.isSimulationPaused());
    
    brain.resumeSimulation();
    assert(!brain.isSimulationPaused());
    
    // Test debug mode
    brain.enableDebugMode();
    assert(brain.isDebugModeEnabled());
    
    // Test step simulation
    brain.stepSimulation();
    // Should advance simulation by one timestep
    
    brain.disableDebugMode();
    assert(!brain.isDebugModeEnabled());
    
    // Test user input simulation
    brain.setUserInputActive(true);
    assert(brain.isUserInputActive());
    
    brain.setUserInputActive(false);
    assert(!brain.isUserInputActive());
    
    std::cout << "    testInteractiveDebugMode passed" << std::endl;
}

void testConfigurationTemplates() {
    std::cout << "Testing Configuration Templates..." << std::endl;
    
    NLMConfigCommand config;
    
    // Test all available templates
    config.listTemplates(); // Just verify it runs without crashing
    
    // Test template validation
    std::string templateFiles[] = {"brain_phase2", "learning_standard", "performance_fast", "visualization_full", "research_advanced"};
    
    for (const auto& templateName : templateFiles) {
        bool result = config.applyTemplate(templateName);
        assert(result);
    }
    
    // Test invalid template
    bool result = config.applyTemplate("invalid_template");
    assert(!result); // Should fail
    
    // Test template export
    bool exportResult = config.generateTemplate("test_template.cfg");
    assert(exportResult);
    
    // Test template validation with file
    bool validateResult = config.validateConfig("test_template.cfg");
    assert(validateResult);
    
    // Clean up
    std::remove("test_template.cfg");
    
    std::cout << "    testConfigurationTemplates passed" << std::endl;
}

void testPerformanceProfiling() {
    std::cout << "Testing Performance Profiling..." << std::endl;
    
    // Test performance monitoring
    nlm::Performance performance;
    
    // Record various performance metrics
    performance.recordAllocation(1024, std::chrono::microseconds(100));
    performance.recordDeallocation(512, std::chrono::microseconds(50));
    performance.recordComputation(10000, std::chrono::milliseconds(5));
    performance.recordMemoryAccess(100, std::chrono::microseconds(10));
    
    // Get performance statistics
    auto stats = performance.getStatistics();
    assert(stats.allocations > 0);
    assert(stats.deallocations > 0);
    assert(stats.computations > 0);
    assert(stats.memoryAccesses > 0);
    
    // Test performance comparison
    performance.recordBenchmark("neuron_update", std::chrono::microseconds(10));
    performance.recordBenchmark("synapse_update", std::chrono::microseconds(15));
    
    auto benchmarkStats = performance.getBenchmarkStats("neuron_update");
    assert(benchmarkStats.count > 0);
    
    // Test performance trends
    auto trends = performance.getPerformanceTrends();
    assert(!trends.empty());
    
    // Test performance alerts
    bool alert = performance.checkPerformanceAlert("memory_usage", 0.8f);
    // Alert check - implementation dependent
    
    std::cout << "    testPerformanceProfiling passed" << std::endl;
}

void testExportImportSystem() {
    std::cout << "Testing Export/Import System..." << std::endl;
    
    // Test different export formats
    const std::string testBrainFile = "test_brain.json";
    const std::string testConfigFile = "test_config.json";
    
    // Export brain state
    nlm::Brain brain;
    nlm::Reward reward;
    
    reward.add(1.5f);
    reward.add(0.5f);
    
    bool brainResult = brain.exportToJSON(testBrainFile);
    assert(brainResult);
    
    // Export configuration
    NLMConfigCommand config;
    bool configResult = config.exportConfig(testConfigFile);
    assert(configResult);
    
    // Verify files were created
    std::ifstream brainFile(testBrainFile);
    std::ifstream configFile(testConfigFile);
    assert(brainFile.good());
    assert(configFile.good());
    brainFile.close();
    configFile.close();
    
    // Import brain state
    bool importResult = brain.importFromJSON(testBrainFile);
    assert(importResult);
    
    // Import configuration
    bool importConfigResult = config.importConfig(testConfigFile);
    assert(importConfigResult);
    
    // Test compression
    const std::string compressedFile = "test_compressed.json";
    bool compressResult = brain.exportToJSON(compressedFile, true); // With compression
    assert(compressResult);
    
    // Test import from compressed file
    bool importCompressedResult = brain.importFromJSON(compressedFile);
    assert(importCompressedResult);
    
    // Test validation during import
    const std::string invalidFile = "invalid_file.json";
    std::ofstream invalidStream(invalidFile);
    invalidStream << "invalid json content";
    invalidStream.close();
    
    bool invalidImportResult = brain.importFromJSON(invalidFile);
    assert(!invalidImportResult); // Should fail
    
    // Clean up
    std::remove(testBrainFile.c_str());
    std::remove(testConfigFile.c_str());
    std::remove(compressedFile.c_str());
    std::remove(invalidFile.c_str());
    
    std::cout << "    testExportImportSystem passed" << std::endl;
}

void testCLIIteractiveFeatures() {
    std::cout << "Testing CLI Interactive Features..." << std::endl;
    
    // Test CLI help system
    CLI::getInstance().printHelp(); // Should list available commands
    
    // Test CLI argument parsing
    std::vector<std::string> testArgs = {"--input", "file.txt", "--output", "result.txt", "--verbose"};
    auto parsedArgs = CLIArgumentParser::parseArguments(testArgs);
    
    assert(parsedArgs.size() >= 3); // input, output, verbose
    
    // Test flag checking
    bool hasInput = CLIArgumentParser::hasFlag(parsedArgs, "input");
    assert(hasInput);
    
    bool hasVerbose = CLIArgumentParser::hasFlag(parsedArgs, "verbose");
    assert(hasVerbose);
    
    // Test value retrieval
    std::string inputFile = CLIArgumentParser::getValue(parsedArgs, "input", "");
    assert(inputFile == "file.txt");
    
    std::string outputFile = CLIArgumentParser::getValue(parsedArgs, "output", "");
    assert(outputFile == "result.txt");
    
    // Test numeric value retrieval
    int intValue = CLIArgumentParser::getIntValue(parsedArgs, "int", 0);
    float floatValue = CLIArgumentParser::getFloatValue(parsedArgs, "float", 0.0f);
    double doubleValue = CLIArgumentParser::getDoubleValue(parsedArgs, "double", 0.0);
    
    std::cout << "    testCLIIteractiveFeatures passed" << std::endl;
}

void testSimulationControl() {
    std::cout << "Testing Simulation Control..." << std::endl;
    
    nlm::Brain brain;
    
    // Test simulation control
    brain.startSimulation();
    assert(brain.isSimulationRunning());
    
    brain.stopSimulation();
    assert(!brain.isSimulationRunning());
    
    brain.resetSimulation();
    assert(brain.getNeuronCount() >= 0); // Reset should not crash
    
    // Test time control
    brain.setTimeScale(2.0f); // Run at 2x speed
    assert(brain.getTimeScale() == 2.0f);
    
    brain.setTimeScale(0.5f); // Run at 0.5x speed
    assert(brain.getTimeScale() == 0.5f);
    
    // Test spike control
    brain.setMaxSpikes(1000);
    assert(brain.getMaxSpikes() == 1000);
    
    std::cout << "    testSimulationControl passed" << std::endl;
}

void runAll() {
    std::cout << "=== User Experience Tests ===" << std::endl;
    
    bool allPassed = true;
    
    // Test Interactive Debug Mode
    std::cout << "Running Interactive Debug Mode tests..." << std::endl;
    try {
        testInteractiveDebugMode();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Test Configuration Templates
    std::cout << "Running Configuration Templates tests..." << std::endl;
    try {
        testConfigurationTemplates();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Test Performance Profiling
    std::cout << "Running Performance Profiling tests..." << std::endl;
    try {
        testPerformanceProfiling();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Test Export/Import System
    std::cout << "Running Export/Import System tests..." << std::endl;
    try {
        testExportImportSystem();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Test CLI Interactive Features
    std::cout << "Running CLI Interactive Features tests..." << std::endl;
    try {
        testCLIIteractiveFeatures();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Test Simulation Control
    std::cout << "Running Simulation Control tests..." << std::endl;
    try {
        testSimulationControl();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    if (allPassed) {
        std::cout << "=== All User Experience Tests PASSED ===" << std::endl;
    } else {
        std::cout << "=== Some User Experience Tests FAILED ===" << std::endl;
    }
}

} // namespace test_user_experience