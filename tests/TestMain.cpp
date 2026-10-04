#include "tests/TestRunner.hpp"

int main(int argc, char* argv[]) {
    // Initialize logging for test runs
    nlm::Logger::getInstance().setLevel(nlm::Logger::Level::Warning);
    
    std::cout << "=== NLM Core Neural Test Suite ===" << std::endl;
    std::cout << "Testing fundamental neural computation components..." << std::endl << std::endl;
    
    // Print test summary
    nlm::TestRunner::printTestSummary();
    std::cout << std::endl;
    
    // Run all tests
    int result = nlm::TestRunner::runAllTests();
    
    if (result == 0) {
        std::cout << std::endl;
        std::cout << "🎉 All tests completed successfully!" << std::endl;
        std::cout << "The core neural components are ready for integration." << std::endl;
    } else {
        std::cout << std::endl;
        std::cout << "❌ Test suite failed. Please check the implementation." << std::endl;
    }
    
    return result;
}
