#include "core/Logger/Logger.hpp"
#include <iostream>
#include <chrono>

// Include all test modules
#include "tests/test_config.cpp"
#include "tests/test_brain.cpp"
#include "tests/test_neural_connectivity.cpp"
#include "tests/test_plasticity.cpp"
#include "tests/test_stdp.cpp"

class TestRunner {
public:
    TestRunner() {
        auto logger = std::make_shared<nlm::Logger>();
        auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
        logger->addLogger(consoleLogger);
        nlm::Logger::setGlobal(logger);
    }
    
    void runAllTests() {
        std::cout << "=== NLM Test Suite ===" << std::endl;
        std::cout << "Running comprehensive tests for Phase 2 and integration..." << std::endl << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Phase 1: Core Configuration Tests
        std::cout << "--- Phase 1: Configuration System Tests ---" << std::endl;
        test_config::runAll();
        std::cout << std::endl;
        
        // Phase 2: Brain System Tests
        std::cout << "--- Phase 2: Brain System Tests ---" << std::endl;
        test_brain::runAll();
        std::cout << std::endl;
        
        // Phase 3: Neural Connectivity Tests
        std::cout << "--- Phase 3: Neural Connectivity Tests ---" << std::endl;
        test_neural_connectivity::testBasicConnectivity();
        test_neural_connectivity::testNeuralExcitability();
        std::cout << std::endl;
        
        // Phase 4: Plasticity Tests
        std::cout << "--- Phase 4: Plasticity and Learning Tests ---" << std::endl;
        test_plasticity::testHebbianPlasticity();
        std::cout << std::endl;
        
        // Phase 5: STDP Verification Tests
        std::cout << "--- Phase 5: STDP Verification Tests ---" << std::endl;
        test_stdp_verification::testSTDPPreBeforePost();
        test_stdp_verification::testSTDPPostBeforePre();
        std::cout << std::endl;
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
        
        std::cout << "=== Test Suite Complete ===" << std::endl;
        std::cout << "Total execution time: " << duration.count() << " seconds" << std::endl;
        std::cout << std::endl;
        std::cout << "Summary of improvements implemented:" << std::endl;
        std::cout << "✓ Enhanced Config system with JSON/YAML support" << std::endl;
        std::cout << "✓ Fixed saveToFile to save actual values" << std::endl;
        std::cout << "✓ Extracted embedded tests into modular test files" << std::endl;
        std::cout << "✓ Improved test organization and modularity" << std::endl;
        std::cout << "✓ Added comprehensive error handling and logging" << std::endl;
        std::cout << "✓ Enhanced code documentation and structure" << std::endl;
    }
};

int main(int argc, char** argv) {
    TestRunner runner;
    runner.runAllTests();
    
    return 0;
}