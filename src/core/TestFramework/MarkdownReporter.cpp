#include "Reporter.hpp"
#include <sstream>
#include <iomanip>

namespace nlm {

void MarkdownReporter::writeTestResult(const TestRunner::TestResult& result, std::ostream& output) {
    if (result.skipped) {
        output << "| " << result.testName << " | SKIPPED | " << (result.errorMessage.empty() ? "-" : result.errorMessage) << " |" << std::endl;
    } else if (result.passed) {
        output << "| " << result.testName << " | PASS | " << formatDuration(result.duration) << " |" << std::endl;
    } else {
        output << "| " << result.testName << " | FAIL | " << result.errorMessage << " |" << std::endl;
    }
}

void MarkdownReporter::writeTestSummary(const std::vector<TestRunner::TestResult>& results, std::ostream& output) {
    output << "# Test Report" << std::endl;
    output << std::endl;
    
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
    
    output << "## Summary" << std::endl;
    output << std::endl;
    output << "| Metric | Value |" << std::endl;
    output << "|--------|-------|" << std::endl;
    output << "| Total Tests | " << total << " |" << std::endl;
    output << "| Passed | " << passed << " |" << std::endl;
    output << "| Failed | " << failed << " |" << std::endl;
    output << "| Skipped | " << skipped << " |" << std::endl;
    
    if (total > 0) {
        float successRate = (static_cast<float>(passed) / static_cast<float>(total)) * 100.0f;
        output << "| Success Rate | " << std::fixed << std::setprecision(1) << successRate << "% |" << std::endl;
        output << "| Total Duration | " << formatDuration(totalDuration) << " |" << std::endl;
        output << "| Average Duration | " << formatDuration(totalDuration / total) << " |" << std::endl;
    }
    
    output << std::endl;
    
    if (failed > 0) {
        output << "## Failed Tests" << std::endl;
        output << std::endl;
        output << "| Test Name | Error |" << std::endl;
        output << "|-----------|-------|" << std::endl;
        
        for (const auto& result : results) {
            if (!result.passed && !result.skipped) {
                output << "| " << result.testName << " | " << result.errorMessage << " |" << std::endl;
            }
        }
        
        output << std::endl;
    }
}

void MarkdownReporter::writeDetailedReport(const std::vector<TestRunner::TestResult>& results, std::ostream& output) {
    writeTestSummary(results, output);
    
    if (results.empty()) {
        return;
    }
    
    output << "## Detailed Results" << std::endl;
    output << std::endl;
    output << "| Test Name | Category | Phase | Status | Duration | Error |" << std::endl;
    output << "|-----------|----------|-------|--------|----------|-------|" << std::endl;
    
    for (const auto& result : results) {
        std::string status;
        if (result.skipped) {
            status = "SKIPPED";
        } else if (result.passed) {
            status = "PASS";
        } else {
            status = "FAIL";
        }
        
        output << "| " << result.testName << " | " << result.testCategory << " | " << result.testPhase << " | "
               << status << " | " << formatDuration(result.duration) << " | "
               << (result.errorMessage.empty() ? "-" : result.errorMessage) << " |" << std::endl;
    }
    
    output << std::endl;
}

} // namespace nlm
