#include "Reporter.hpp"
#include <sstream>

namespace nlm {

void XMLReporter::writeTestResult(const TestRunner::TestResult& result, std::ostream& output) {
    output << "    <test>" << std::endl;
    output << "        <name>" << escapeXML(result.testName) << "</name>" << std::endl;
    output << "        <category>" << escapeXML(result.testCategory) << "</category>" << std::endl;
    output << "        <phase>" << escapeXML(result.testPhase) << "</phase>" << std::endl;
    output << "        <passed>" << (result.passed ? "true" : "false") << "</passed>" << std::endl;
    output << "        <skipped>" << (result.skipped ? "true" : "false") << "</skipped>" << std::endl;
    
    if (!result.errorMessage.empty()) {
        output << "        <error>" << escapeXML(result.errorMessage) << "</error>" << std::endl;
    }
    
    output << "        <duration_ms>" << result.duration.count() << "</duration_ms>" << std::endl;
    
    if (!result.metrics.empty()) {
        output << "        <metrics>" << std::endl;
        for (const auto& metric : result.metrics) {
            output << "            <metric name=\"" << escapeXML(metric.first) << "\">" << escapeXML(metric.second) << "</metric>" << std::endl;
        }
        output << "        </metrics>" << std::endl;
    }
    
    output << "    </test>" << std::endl;
}

void XMLReporter::writeTestSummary(const std::vector<TestRunner::TestResult>& results, std::ostream& output) {
    output << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    output << "<test_report>" << std::endl;
    output << "    <summary>" << std::endl;
    
    if (results.empty()) {
        output << "        <message>No tests were run.</message>" << std::endl;
    } else {
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
        
        output << "        <total_tests>" << total << "</total_tests>" << std::endl;
        output << "        <passed_tests>" << passed << "</passed_tests>" << std::endl;
        output << "        <failed_tests>" << failed << "</failed_tests>" << std::endl;
        output << "        <skipped_tests>" << skipped << "</skipped_tests>" << std::endl;
    }
    
    output << "    </summary>" << std::endl;
    
    if (!results.empty()) {
        output << "    <results>" << std::endl;
        for (const auto& result : results) {
            writeTestResult(result, output);
        }
        output << "    </results>" << std::endl;
    }
    
    output << "</test_report>" << std::endl;
}

void XMLReporter::writeDetailedReport(const std::vector<TestRunner::TestResult>& results, std::ostream& output) {
    writeTestSummary(results, output);
}

std::string XMLReporter::escapeXML(const std::string& text) const {
    std::string escaped = text;
    
    // Replace XML special characters
    size_t pos = 0;
    while ((pos = escaped.find('&', pos)) != std::string::npos) {
        escaped.replace(pos, 1, "&amp;");
        pos += 5;
    }
    
    pos = 0;
    while ((pos = escaped.find('<', pos)) != std::string::npos) {
        escaped.replace(pos, 1, "&lt;");
        pos += 4;
    }
    
    pos = 0;
    while ((pos = escaped.find('>', pos)) != std::string::npos) {
        escaped.replace(pos, 1, "&gt;");
        pos += 4;
    }
    
    pos = 0;
    while ((pos = escaped.find('"', pos)) != std::string::npos) {
        escaped.replace(pos, 1, "&quot;");
        pos += 6;
    }
    
    pos = 0;
    while ((pos = escaped.find('\'', pos)) != std::string::npos) {
        escaped.replace(pos, 1, "&apos;");
        pos += 6;
    }
    
    return escaped;
}

} // namespace nlm
