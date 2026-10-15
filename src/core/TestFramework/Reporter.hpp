#pragma once

#include "TestRunner.hpp"
#include <string>
#include <ostream>
#include <memory>
#include <map>
#include <vector>

namespace nlm {

class Reporter {
public:
    enum class ReportFormat { PlainText, JSON, XML, CSV, Markdown };
    
    virtual ~Reporter() = default;
    
    virtual std::string getName() const = 0;
    virtual ReportFormat getFormat() const = 0;
    virtual bool isEnabled() const { return true; }
    
    // Configuration
    virtual void configure(const std::map<std::string, std::string>& options) {}
    
    // Report writing
    virtual void writeTestResult(const TestRunner::TestResult& result, std::ostream& output) = 0;
    virtual void writeTestSummary(const std::vector<TestRunner::TestResult>& results, std::ostream& output) = 0;
    virtual void writeDetailedReport(const std::vector<TestRunner::TestResult>& results, std::ostream& output) = 0;
    
    // Special report types
    virtual void writeProgressUpdate(const TestRunner::TestResult& result, std::ostream& output) {}
    virtual void writeErrorReport(const std::vector<TestRunner::TestResult>& results, std::ostream& output) {}
    virtual void writePerformanceReport(const std::vector<TestRunner::TestResult>& results, std::ostream& output) {}
    
protected:
    // Helper methods for formatting
    std::string formatTimestamp(const std::chrono::system_clock::time_point& time) const;
    std::string escapeXML(const std::string& text) const;
    std::string escapeJSON(const std::string& text) const;
    std::string formatDuration(std::chrono::milliseconds duration) const;
};

// Plain text reporter
class PlainTextReporter : public Reporter {
public:
    std::string getName() const override { return "PlainText"; }
    ReportFormat getFormat() const override { return ReportFormat::PlainText; }
    
    void writeTestResult(const TestRunner::TestResult& result, std::ostream& output) override;
    void writeTestSummary(const std::vector<TestRunner::TestResult>& results, std::ostream& output) override;
    void writeDetailedReport(const std::vector<TestRunner::TestResult>& results, std::ostream& output) override;
};

// JSON reporter
class JSONReporter : public Reporter {
public:
    std::string getName() const override { return "JSON"; }
    ReportFormat getFormat() const override { return ReportFormat::JSON; }
    
    void writeTestResult(const TestRunner::TestResult& result, std::ostream& output) override;
    void writeTestSummary(const std::vector<TestRunner::TestResult>& results, std::ostream& output) override;
    void writeDetailedReport(const std::vector<TestRunner::TestResult>& results, std::ostream& output) override;
    
private:
    void writeTestResultJSON(const TestRunner::TestResult& result, std::ostream& output);
    void writeSummaryJSON(const std::vector<TestRunner::TestResult>& results, std::ostream& output);
};

// XML reporter
class XMLReporter : public Reporter {
public:
    std::string getName() const override { return "XML"; }
    ReportFormat getFormat() const override { return ReportFormat::XML; }
    
    void writeTestResult(const TestRunner::TestResult& result, std::ostream& output) override;
    void writeTestSummary(const std::vector<TestRunner::TestResult>& results, std::ostream& output) override;
    void writeDetailedReport(const std::vector<TestRunner::TestResult>& results, std::ostream& output) override;
};

// CSV reporter
class CSVReporter : public Reporter {
public:
    std::string getName() const override { return "CSV"; }
    ReportFormat getFormat() const override { return ReportFormat::CSV; }
    
    void writeTestResult(const TestRunner::TestResult& result, std::ostream& output) override;
    void writeTestSummary(const std::vector<TestRunner::TestResult>& results, std::ostream& output) override;
    void writeDetailedReport(const std::vector<TestRunner::TestResult>& results, std::ostream& output) override;
};

// Markdown reporter
class MarkdownReporter : public Reporter {
public:
    std::string getName() const override { return "Markdown"; }
    ReportFormat getFormat() const override { return ReportFormat::Markdown; }
    
    void writeTestResult(const TestRunner::TestResult& result, std::ostream& output) override;
    void writeTestSummary(const std::vector<TestRunner::TestResult>& results, std::ostream& output) override;
    void writeDetailedReport(const std::vector<TestRunner::TestResult>& results, std::ostream& output) override;
};

// Report factory for creating reporters
class ReportFactory {
public:
    static std::unique_ptr<Reporter> createReporter(const std::string& name, const std::map<std::string, std::string>& options = {});
    static std::vector<std::unique_ptr<Reporter>> createReporters(const std::vector<std::string>& reporterNames, 
                                                                   const std::map<std::string, std::string>& globalOptions = {});
    static std::vector<std::string> getAvailableReporters();
};

} // namespace nlm
