#include "TestFramework.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>

namespace nlm {

std::vector<std::shared_ptr<TestSuite>> TestFactory::getAllTestSuites() {
    static std::vector<std::shared_ptr<TestSuite>> testSuites = registerTestSuites();
    return testSuites;
}

std::shared_ptr<TestSuite> TestFactory::getTestSuite(const std::string& name) {
    auto testSuites = getAllTestSuites();
    for (const auto& suite : testSuites) {
        if (suite->getName() == name) {
            return suite;
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<TestSuite>> TestFactory::registerTestSuites() {
    std::vector<std::shared_ptr<TestSuite>> suites;
    
    // Register built-in test suites here
    // examples:
    // suites.push_back(std::make_shared<TestSuite>("UnitTests", "Unit tests for NLM components"));
    // suites.push_back(std::make_shared<TestSuite>("IntegrationTests", "Integration tests for NLM systems"));
    
    return suites;
}

} // namespace nlm
