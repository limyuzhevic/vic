#include "Reporter.hpp"
#include <iomanip>
#include <sstream>

namespace nlm {

void PlainTextReporter::writeTestResult(const TestRunner::TestResult& result, std::ostream& output) {
    if (result.skipped) {
        output << "[SKIPPED] " << result.testName;
        if (!result.errorMessage.empty()) {
            output << " - " << result.errorMessage;
        }
        output << std::endl;
    } else if (result.passed) {
        output << "[PASS]   " << result.testName;
        if (!result.metrics.empty()) {
            output << " - Duration: " << formatDuration(result.duration);
        }
        output << std::endl;
    } else {
        output << "[FAIL]   " << result.testName << " - " << result.errorMessage << std::endl;
    }
}

void PlainTextReporter::writeTestSummary(const std::vector<TestRunner::TestResult>& results, std::ostream& output) {
    output << "Test Summary" << std::endl;
    output << "============" << std::endl;
    
    if (results.empty()) {
        output << "No tests were run." << std::endl;
        return;
    }
    
    size_t total = results.size();
    size_t passed = 0, failed = 0, skipped = 0;
    std::chrono::milliseconds totalDuration(0);
    
    for (const auto& result : results) {
        if (result.skipped) {
            skipped++;
        } else if (result.passed) {
            passed++;
        } else {
            failed++;
        }
        totalDuration += result.duration;
    }
    
    output << "Total Tests: " << total << std::endl;
    output << "Passed:      " << passed << std::endl;
    output << "Failed:      " << failed << std::endl;
    output << "Skipped:     " << skipped << std::endl;
    
    if (total > 0) {
        float successRate = (static_cast<float>(passed) / static_cast<float>(total)) * 100.0f;
        output << "Success Rate: " << std::fixed << std::setprecision(1) << successRate << "%" << std::endl;
        output << "Total Time:  " << formatDuration(totalDuration) << std::endl;
        output << "Average Time: " << formatDuration(totalDuration / total) << std::endl;
    }
}

void PlainTextReporter::writeDetailedReport(const std::vector<TestRunner::TestResult>& results, std::ostream& output) {
    writeTestSummary(results, output);
    output << std::endl;
    
    if (results.empty()) {
        return;
    }
    
    output << "Detailed Results:" << std::endl;
    output << "=================" << std::endl;
    
    for (const auto& result : results) {
        output << std::endl;
        output << "Test: " << result.testName << std::endl;
        output << "Category: " << result.testCategory << std::endl;
        output << "Phase: " << result.testPhase << std::endl;
        
        if (result.skipped) {
            output << "Status: SKIPPED" << std::endl;
            if (!result.errorMessage.empty()) {
                output << "Reason: " << result.errorMessage << std::endl;
            }
        } else if (result.passed) {
            output << "Status: PASSED" << std::endl;
            output << "Duration: " << formatDuration(result.duration) << std::endl;
            
            if (!result.metrics.empty()) {
                output << "Metrics:" << std::endl;
                for (const auto& metric : result.metrics) {
                    output << "  " << metric.first << ": " << metric.second << std::endl;
                }
            }
        } else {
            output << "Status: FAILED" << std::endl;
            output << "Error: " << result.errorMessage << std::endl;
            output << "Duration: " << formatDuration(result.duration) << std::endl;
        }
        
        output << "---" << std::endl;
    }
}

} // namespace nlm
