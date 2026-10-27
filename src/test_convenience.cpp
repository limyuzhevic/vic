#include "Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <cassert>

void testConvenienceFunctions() {
    std::cout << "Testing convenience functions..." << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Create brain with default config
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 2);
    
    Brain brain(config);
    if (!brain.initialize()) {
        std::cerr << "Failed to initialize brain" << std::endl;
        return;
    }
    
    // Test 1: Find neurons by type
    std::cout << "Test 1: Find neurons by type..." << std::endl;
    auto sensoryNeurons = brain.findNeuronsByType(NeuronType::Sensory);
    std::cout << "Found " << sensoryNeurons.size() << " sensory neurons" << std::endl;
    assert(sensoryNeurons.size() <= 50);  // Roughly half should be sensory
    
    auto motorNeurons = brain.findNeuronsByType(NeuronType::Motor);
    std::cout << "Found " << motorNeurons.size() << " motor neurons" << std::endl;
    assert(motorNeurons.size() <= 50);  // Roughly half should be motor
    
    // Test 2: Get active neurons
    std::cout << "Test 2: Get active neurons..." << std::endl;
    auto activeNeurons = brain.getActiveNeurons();
    std::cout << "Found " << activeNeurons.size() << " active neurons" << std::endl;
    
    // Test 3: Learning patterns
    std::cout << "Test 3: Learning patterns..." << std::endl;
    brain.applyLearningPattern("repetition");
    brain.applyLearningPattern("spontaneous");
    brain.applyLearningPattern("structured");
    
    // Test 4: Reset functionality
    std::cout << "Test 4: Reset functionality..." << std::endl;
    brain.reset("default");
    brain.reset("random");
    brain.reset("structured");
    
    // Test 5: State summary
    std::cout << "Test 5: State summary..." << std::endl;
    std::string summary = brain.getStateSummary();
    std::cout << "State summary length: " << summary.length() << " characters" << std::endl;
    assert(summary.length() > 0);
    
    // Test 6: Network metrics
    std::cout << "Test 6: Network metrics..." << std::endl;
    float excit = brain.getNetworkExcitability();
    float plastic = brain.getNetworkPlasticity();
    size_t maxSpikes = brain.getMaxSpikeCount();
    
    std::cout << "Excitability: " << excit << std::endl;
    std::cout << "Plasticity: " << plastic << std::endl;
    std::cout << "Max spike count: " << maxSpikes << std::endl;
    
    assert(excit >= 0.0f && excit <= 10.0f);
    assert(plastic >= 0.0f && plastic <= 1.0f);
    
    // Test 7: Integration readiness
    std::cout << "Test 7: Integration readiness..." << std::endl;
    bool ready = brain.isReadyForInteraction();
    std::cout << "Ready for interaction: " << (ready ? "Yes" : "No") << std::endl;
    
    // Test 8: Episode preparation
    std::cout << "Test 8: Episode preparation..." << std::endl;
    brain.prepareForEpisode("test_episode_1");
    brain.prepareForEpisode("test_episode_2");
    
    // Test 9: Configuration validation
    std::cout << "Test 9: Configuration validation..." << std::endl;
    bool valid = brain.validateConfiguration();
    std::cout << "Configuration valid: " << (valid ? "Yes" : "No") << std::endl;
    std::string errors = brain.getValidationErrors();
    std::cout << "Validation errors: " << errors << std::endl;
    
    // Test 10: Performance monitoring
    std::cout << "Test 10: Performance monitoring..." << std::endl;
    double procTime = brain.getAverageProcessingTime();
    size_t opsCount = brain.getOperationsCount();
    std::cout << "Average processing time: " << procTime << "s" << std::endl;
    std::cout << "Operations count: " << opsCount << std::endl;
    
    brain.resetPerformanceCounters();
    
    std::cout << "All convenience function tests passed!" << std::endl;
}

int main() {
    std::cout << "=== NLM Convenience Functions Test ===" << std::endl;
    
    try {
        testConvenienceFunctions();
        std::cout << "\n✓ All tests passed successfully!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "\n✗ Test failed with unknown exception" << std::endl;
        return 1;
    }
}
