#include "Reporter.hpp"
#include <sstream>
#include <iomanip>

namespace nlm {

void CSVReporter::writeTestResult(const TestRunner::TestResult& result, std::ostream& output) {
    // CSV format: test_name,category,phase,passed,skipped,duration_ms,error
    output << "\"" << result.testName << "\",";
    output << "\"" << result.testCategory << "\",";
    output << "\"" << result.testPhase << "\",";
    output << (result.passed ? "PASS" : "FAIL") << ",";
    output << (result.skipped ? "YES" : "NO") << ",";
    output << result.duration.count() << ",";
    
    // Escape quotes and commas in error message
    std::string error = result.errorMessage;
    if (!error.empty()) {
        std::replace(error.begin(), error.end(), '\"', '\"\"');
        output << "\"" << error << "\"";
    }
    output << std::endl;
}

void CSVReporter::writeTestSummary(const std::vector<TestRunner::TestResult>& results, std::ostream& output) {
    if (results.empty()) {
        output << "No tests were run." << std::endl;
        return;
    }
    
    // Header
    output << "test_name,category,phase,passed,skipped,duration_ms,error" << std::endl;
    
    // Data
    for (const auto& result : results) {
        writeTestResult(result, output);
    }
    
    // Summary statistics
    output << std::endl;
    output << "Summary:" << std::endl;
    size_t total = results.size();
    size_t passed = 0, failed = 0, skipped = 0;
    
    for (const auto& result : results) {
        if (result.skipped) {
            skipped++;
        } else if (result.passed) {
            passed++;
        } else {
            failed++;
        }
    }
    
    output << "Total Tests: " << total << std::endl;
    output << "Passed:      " << passed << std::endl;
    output << "Failed:      " << failed << std::endl;
    output << "Skipped:     " << skipped << std::endl;
}

void CSVReporter::writeDetailedReport(const std::vector<TestRunner::TestResult>& results, std::ostream& output) {
    writeTestSummary(results, output);
    
    if (!results.empty()) {
        output << std::endl;
        output << "Detailed Results:" << std::endl;
        output << "=================" << std::endl;
        
        for (const auto& result : results) {
            output << std::endl;
            output << "Test Name: " << result.testName << std::endl;
            output << "Category:  " << result.testCategory << std::endl;
            output << "Phase:     " << result.testPhase << std::endl;
            output << "Status:    " << (result.skipped ? "SKIPPED" : (result.passed ? "PASSED" : "FAILED")) << std::endl;
            
            if (!result.errorMessage.empty()) {
                output << "Error:     " << result.errorMessage << std::endl;
            }
            
            output << "Duration:  " << formatDuration(result.duration) << std::endl;
            
            if (!result.metrics.empty()) {
                output << "Metrics:" << std::endl;
                for (const auto& metric : result.metrics) {
                    output << "  " << metric.first << ": " << metric.second << std::endl;
                }
            }
            
            output << "---" << std::endl;
        }
    }
}

} // namespace nlm
