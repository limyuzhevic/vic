#include "Reporter.hpp"
#include <nlohmann/json.hpp>
#include <sstream>

namespace nlm {

void JSONReporter::writeTestResult(const TestRunner::TestResult& result, std::ostream& output) {
    nlohmann::json jsonResult;
    
    jsonResult["test_name"] = result.testName;
    jsonResult["category"] = result.testCategory;
    jsonResult["phase"] = result.testPhase;
    jsonResult["passed"] = result.passed;
    jsonResult["skipped"] = result.skipped;
    
    if (!result.errorMessage.empty()) {
        jsonResult["error"] = result.errorMessage;
    }
    
    jsonResult["duration_ms"] = result.duration.count();
    
    if (!result.metrics.empty()) {
        for (const auto& metric : result.metrics) {
            jsonResult["metrics"][metric.first] = metric.second;
        }
    }
    
    output << jsonResult.dump(2) << std::endl;
}

void JSONReporter::writeTestSummary(const std::vector<TestRunner::TestResult>& results, std::ostream& output) {
    nlohmann::json jsonSummary;
    
    if (results.empty()) {
        jsonSummary["message"] = "No tests were run.";
        output << jsonSummary.dump(2) << std::endl;
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
    
    jsonSummary["total_tests"] = total;
    jsonSummary["passed_tests"] = passed;
    jsonSummary["failed_tests"] = failed;
    jsonSummary["skipped_tests"] = skipped;
    
    if (total > 0) {
        float successRate = (static_cast<float>(passed) / static_cast<float>(total)) * 100.0f;
        jsonSummary["success_rate"] = successRate;
        jsonSummary["total_duration_ms"] = totalDuration.count();
        jsonSummary["average_duration_ms"] = total > 0 ? totalDuration.count() / total : 0;
    }
    
    // Add individual test results
    jsonSummary["tests"] = nlohmann::json::array();
    for (const auto& result : results) {
        nlohmann::json testJson;
        testJson["name"] = result.testName;
        testJson["category"] = result.testCategory;
        testJson["phase"] = result.testPhase;
        testJson["passed"] = result.passed;
        testJson["skipped"] = result.skipped;
        
        if (!result.errorMessage.empty()) {
            testJson["error"] = result.errorMessage;
        }
        
        testJson["duration_ms"] = result.duration.count();
        
        if (!result.metrics.empty()) {
            testJson["metrics"] = result.metrics;
        }
        
        jsonSummary["tests"].push_back(testJson);
    }
    
    output << jsonSummary.dump(2) << std::endl;
}

void JSONReporter::writeDetailedReport(const std::vector<TestRunner::TestResult>& results, std::ostream& output) {
    writeTestSummary(results, output);
}

void JSONReporter::writeTestResultJSON(const TestRunner::TestResult& result, std::ostream& output) {
    // This is a helper method - same as writeTestResult
    writeTestResult(result, output);
}

void JSONReporter::writeSummaryJSON(const std::vector<TestRunner::TestResult>& results, std::ostream& output) {
    // This is a helper method - same as writeTestSummary
    writeTestSummary(results, output);
}

} // namespace nlm
