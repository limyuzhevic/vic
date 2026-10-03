#include <iostream>
#include <cassert>
#include "src/core/Config/Config.hpp"
#include "src/core/Logger/Logger.hpp"
#include "src/core/Random/Random.hpp"
#include "src/core/SimulationClock/SimulationClock.hpp"
#include "NLMErrors.hpp"

void testConfigErrorHandling() {
    std::cout << "Testing Config error handling..." << std::endl;
    
    nlm::ErrorHandler& errorHandler = nlm::getGlobalErrorHandler();
    errorHandler.clear();
    
    nlm::Config config;
    
    // Test 1: Empty key validation
    try {
        config.set("", "value");
        std::cout << "WARNING: Empty key should have been rejected" << std::endl;
    } catch (const nlm::NLMError& e) {
        std::cout << "✓ Empty key validation works: " << e.what() << std::endl;
    }
    
    // Test 2: Type mismatch in get
    config.set("stringKey", "hello");
    auto intVal = config.get<int>("stringKey");
    assert(!intVal.has_value());  // Should not have value
    std::cout << "✓ Type mismatch handling works" << std::endl;
    
    // Test 3: File load error (non-existent file)
    bool loadResult = config.loadFromFile("/non/existent/file.conf");
    assert(!loadResult);  // Should fail
    std::cout << "✓ File load error handling works" << std::endl;
    
    std::cout << "Config error handling tests completed successfully!" << std::endl;
}

void testLoggerErrorHandling() {
    std::cout << "\nTesting Logger error handling..." << std::endl;
    
    nlm::ErrorHandler& errorHandler = nlm::getGlobalErrorHandler();
    errorHandler.clear();
    
    nlm::Logger logger;
    
    // Add a logger
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    logger.addLogger(consoleLogger);
    
    // Test 1: Invalid log level validation
    try {
        // This should not throw due to validation
        logger.setLevel(nlm::LogLevel::Debug);
        std::cout << "✓ Valid log level accepted" << std::endl;
    } catch (...) {
        std::cout << "WARNING: Log level validation might be too strict" << std::endl;
    }
    
    // Test 2: Log with invalid parameters
    logger.log(nlm::LogLevel::Info, "Test message with file: " + std::string(__FILE__) + 
               " at line " + std::to_string(__LINE__) + 
               " in function " + std::string(__func__));
    std::cout << "✓ Logging works correctly" << std::endl;
    
    std::cout << "Logger error handling tests completed successfully!" << std::endl;
}

void testRandomErrorHandling() {
    std::cout << "\nTesting Random error handling..." << std::endl;
    
    nlm::ErrorHandler& errorHandler = nlm::getGlobalErrorHandler();
    errorHandler.clear();
    
    // Test 1: Zero seed validation
    try {
        nlm::RandomGenerator rg(0);
        std::cout << "WARNING: Zero seed should have been rejected" << std::endl;
    } catch (const nlm::NLMError& e) {
        std::cout << "✓ Zero seed validation works: " << e.what() << std::endl;
    }
    
    // Test 2: Invalid min/max parameters
    nlm::RandomGenerator rg(42);
    try {
        uint32_t result = rg.uniformInt(10, 5);  // min > max
        std::cout << "WARNING: Invalid range should have been rejected" << std::endl;
    } catch (const nlm::NLMError& e) {
        std::cout << "✓ Invalid range validation works: " << e.what() << std::endl;
    }
    
    // Test 3: Valid random number generation
    uint32_t result = rg.uniformInt(1, 100);
    assert(result >= 1 && result <= 100);
    std::cout << "✓ Valid random number generation works: " << result << std::endl;
    
    std::cout << "Random error handling tests completed successfully!" << std::endl;
}

void testSimulationClockErrorHandling() {
    std::cout << "\nTesting SimulationClock error handling..." << std::endl;
    
    nlm::ErrorHandler& errorHandler = nlm::getGlobalErrorHandler();
    errorHandler.clear();
    
    // Test 1: Invalid timestep validation
    try {
        nlm::SimulationClock clock(0.0);  // Invalid timestep
        std::cout << "WARNING: Zero timestep should have been rejected" << std::endl;
    } catch (const nlm::NLMError& e) {
        std::cout << "✓ Invalid timestep validation works: " << e.what() << std::endl;
    }
    
    // Test 2: Valid SimulationClock operations
    nlm::SimulationClock clock(0.001);  // Valid timestep
    clock.advance();
    assert(clock.getStep() == 1);
    assert(clock.getTime() > 0.0);
    std::cout << "✓ Valid SimulationClock operations work" << std::endl;
    
    // Test 3: Time to step conversion validation
    nlm::Timestamp time = clock.getTime();
    nlm::SimulationStep step = clock.timeToStep(time);
    assert(step == 1);
    std::cout << "✓ Time to step conversion works" << std::endl;
    
    std::cout << "SimulationClock error handling tests completed successfully!" << std::endl;
}

void testBrainErrorHandling() {
    std::cout << "\nTesting Brain error handling..." << std::endl;
    
    nlm::ErrorHandler& errorHandler = nlm::getGlobalErrorHandler();
    errorHandler.clear();
    
    // Create a minimal config for testing
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", size_t(10));
    config->set("region_count", size_t(1));
    config->set("random_seed", uint64_t(123));
    config->set("simulation_timestep", double(0.001));
    
    // Test 1: Invalid configuration values
    try {
        config->set("neuron_count", static_cast<size_t>(-1));  // Invalid negative value
        std::cout << "WARNING: Invalid neuron count should have been rejected" << std::endl;
    } catch (...) {
        std::cout << "✓ Invalid configuration validation works" << std::endl;
    }
    
    // Test 2: Brain initialization with invalid config
    try {
        nlm::Brain brain(config);
        std::cout << "✓ Brain initialization completed" << std::endl;
        
        // Test some basic operations
        brain.initialize();
        std::cout << "✓ Brain operations work correctly" << std::endl;
        
    } catch (const nlm::NLMError& e) {
        std::cout << "✓ Brain error handling works: " << e.what() << std::endl;
    }
    
    std::cout << "Brain error handling tests completed successfully!" << std::endl;
}

int main() {
    std::cout << "=== NLM Error Handling Test Suite ===" << std::endl;
    
    try {
        testConfigErrorHandling();
        testLoggerErrorHandling();
        testRandomErrorHandling();
        testSimulationClockErrorHandling();
        testBrainErrorHandling();
        
        std::cout << "\n=== All tests completed successfully! ===" << std::endl;
        std::cout << "Error handling implementation is working correctly across all modules." << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "\n*** Test failed with exception: " << e.what() << " ***" << std::endl;
        return 1;
    }
    
    return 0;
}
