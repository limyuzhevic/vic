// Test framework implementation
// Provides comprehensive testing framework for NLM

#include "performance/utilities/test_framework.h"
#include <fstream>

namespace nlm {
namespace test {

// Implementation of TestFramework class methods

void TestFramework::generateConsoleReport() {
    std::cout << "=== Test Framework Report ===" << std::endl;
    int passed = 0, failed = 0, skipped = 0;
    
    for (const auto& result : test_results) {
        std::cout << "Test: " << result.name << std::endl;
        std::cout << "  Status: " << result.status << std::endl;
        std::cout << "  Duration: " << result.duration_ms << "ms" << std::endl;
        if (!result.details.empty()) {
            std::cout << "  Details: " << result.details << std::endl;
        }
        if (!result.metrics.empty()) {
            std::cout << "  Metrics:" << std::endl;
            for (const auto& metric : result.metrics) {
                std::cout << "    " << metric.first << ": " << metric.second << std::endl;
            }
        }
        std::cout << std::endl;
        
        if (result.status == "PASSED") passed++;
        else if (result.status == "FAILED") failed++;
        else if (result.status == "SKIPPED") skipped++;
    }
    
    std::cout << "=== Summary ===" << std::endl;
    std::cout << "Total tests: " << test_results.size() << std::endl;
    std::cout << "Passed: " << passed << std::endl;
    std::cout << "Failed: " << failed << std::endl;
    std::cout << "Skipped: " << skipped << std::endl;
}

void TestFramework::generateJSONReport() {
    std::cout << "{" << std::endl;
    std::cout << "  \"test_results\": [" << std::endl;
    
    for (size_t i = 0; i < test_results.size(); ++i) {
        const auto& result = test_results[i];
        std::cout << "    {" << std::endl;
        std::cout << "      \"name\": \"" << result.name << "\"," << std::endl;
        std::cout << "      \"status\": \"" << result.status << "\"," << std::endl;
        std::cout << "      \"duration_ms\": " << result.duration_ms << "," << std::endl;
        std::cout << "      \"details\": \"" << result.details << "\"" << std::endl;
        
        if (!result.metrics.empty()) {
            std::cout << "      \"metrics\": {" << std::endl;
            bool first_metric = true;
            for (const auto& metric : result.metrics) {
                if (!first_metric) std::cout << "," << std::endl;
                std::cout << "        \"" << metric.first << "\": " << metric.second;
                first_metric = false;
            }
            std::cout << std::endl << "      }" << std::endl;
        }
        
        if (i < test_results.size() - 1) std::cout << "    }," << std::endl;
        else std::cout << "    }" << std::endl;
    }
    
    std::cout << "  ]" << std::endl;
    std::cout << "}" << std::endl;
}

void TestFramework::generateXMLReport() {
    std::cout << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    std::cout << "<test_results>" << std::endl;
    
    for (const auto& result : test_results) {
        std::cout << "  <test>" << std::endl;
        std::cout << "    <name>" << result.name << "</name>" << std::endl;
        std::cout << "    <status>" << result.status << "</status>" << std::endl;
        std::cout << "    <duration_ms>" << result.duration_ms << "</duration_ms>" << std::endl;
        if (!result.details.empty()) {
            std::cout << "    <details>" << result.details << "</details>" << std::endl;
        }
        if (!result.metrics.empty()) {
            std::cout << "    <metrics>" << std::endl;
            for (const auto& metric : result.metrics) {
                std::cout << "      <metric name=\"" << metric.first << "\">" 
                         << metric.second << "</metric>" << std::endl;
            }
            std::cout << "    </metrics>" << std::endl;
        }
        std::cout << "  </test>" << std::endl;
    }
    
    std::cout << "</test_results>" << std::endl;
}

} // namespace test
} // namespace nlm
