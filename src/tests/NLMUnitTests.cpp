#include "NLMUnitTests.hpp"

namespace nlm {
namespace unit_tests {

// Implementation of unit test functions
void runAllUnitTests() {
    std::cout << "=== NLM Comprehensive Integration Unit Tests ===" << std::endl;
    std::cout << "Testing extended integration and higher-level systems..." << std::endl;
    std::cout << std::endl;
    
    // Run comprehensive integration tests
    SystemIntegrationUnitTests::testCompleteIntegration();
    
    std::cout << std::endl;
    std::cout << "=== All Comprehensive Integration Tests Passed! ===" << std::endl;
    std::cout << "NLM unit test suite with extended integration tests completed successfully." << std::endl;
    std::cout << "This version includes testing of memory systems, neuromodulation, cognition, and full integration." << std::endl;
}

} // namespace unit_tests
} // namespace nlm