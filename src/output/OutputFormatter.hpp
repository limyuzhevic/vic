#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <ctime>
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include "test/TestRunner.hpp"

namespace nlm {

// Output format enumeration
enum class OutputFormat {
    TEXT,
    JSON,
    CSV,
    XML,
    YAML,
    HTML,
    MARKDOWN
};

// Output configuration
struct OutputConfig {
    OutputFormat format;
    std::string filepath;
    bool append;
    bool prettyPrint;
    std::string title;
    std::string description;
    std::map<std::string, std::string> metadata;
    bool includeTimestamp;
    bool includeMetrics;
    bool includeProgress;
    std::string timestampFormat;
    std::string dateFormat;
};

// Base output formatter interface
class OutputFormatter {
public:
    virtual ~OutputFormatter() = default;
    
    virtual bool initialize(const OutputConfig& config) = 0;
    virtual bool write(const std::string& content) = 0;
    virtual bool write(const std::vector<TestResult>& testResults) = 0;
    virtual bool write(const std::map<std::string, double>& metrics) = 0;
    virtual bool write(const std::string& title, const std::string& content) = 0;
    virtual void flush() = 0;
    virtual void close() = 0;
    virtual bool isOpen() const = 0;
    
    // Static factory methods
    static std::unique_ptr<OutputFormatter> create(OutputFormat format);
    static OutputFormat detectFormat(const std::string& filepath);
    static std::string formatToString(OutputFormat format);
};

// Text output formatter
class TextFormatter : public OutputFormatter {
public:
    TextFormatter() = default;
    ~TextFormatter() override = default;
    
    bool initialize(const OutputConfig& config) override;
    bool write(const std::string& content) override;
    bool write(const std::vector<TestResult>& testResults) override;
    bool write(const std::map<std::string, double>& metrics) override;
    bool write(const std::string& title, const std::string& content) override;
    void flush() override;
    void close() override;
    bool isOpen() const override;
    
private:
    std::ofstream outputFile_;
    std::ostringstream buffer_;
    OutputConfig config_;
    
    std::string formatTestResult(const TestResult& result) const;
    std::string formatMetrics(const std::map<std::string, double>& metrics) const;
};

// JSON output formatter
class JsonFormatter : public OutputFormatter {
public:
    JsonFormatter() = default;
    ~JsonFormatter() override = default;
    
    bool initialize(const OutputConfig& config) override;
    bool write(const std::string& content) override;
    bool write(const std::vector<TestResult>& testResults) override;
    bool write(const std::map<std::string, double>& metrics) override;
    bool write(const std::string& title, const std::string& content) override;
    void flush() override;
    void close() override;
    bool isOpen() const override;
    
private:
    std::ofstream outputFile_;
    std::ostringstream buffer_;
    OutputConfig config_;
    
    std::string formatTestResult(const TestResult& result) const;
    std::string formatMetrics(const std::map<std::string, double>& metrics) const;
    std::string escapeJsonString(const std::string& str) const;
};

// CSV output formatter
class CsvFormatter : public OutputFormatter {
public:
    CsvFormatter() = default;
    ~CsvFormatter() override = default;
    
    bool initialize(const OutputConfig& config) override;
    bool write(const std::string& content) override;
    bool write(const std::vector<TestResult>& testResults) override;
    bool write(const std::map<std::string, double>& metrics) override;
    bool write(const std::string& title, const std::string& content) override;
    void flush() override;
    void close() override;
    bool isOpen() const override;
    
private:
    std::ofstream outputFile_;
    std::ostringstream buffer_;
    OutputConfig config_;
    bool headerWritten_ = false;
    
    std::string formatTestResult(const TestResult& result) const;
    std::string formatMetrics(const std::map<std::string, double>& metrics) const;
};

// XML output formatter
class XmlFormatter : public OutputFormatter {
public:
    XmlFormatter() = default;
    ~XmlFormatter() override = default;
    
    bool initialize(const OutputConfig& config) override;
    bool write(const std::string& content) override;
    bool write(const std::vector<TestResult>& testResults) override;
    bool write(const std::map<std::string, double>& metrics) override;
    bool write(const std::string& title, const std::string& content) override;
    void flush() override;
    void close() override;
    bool isOpen() const override;
    
private:
    std::ofstream outputFile_;
    std::ostringstream buffer_;
    OutputConfig config_;
    
    std::string formatTestResult(const TestResult& result) const;
    std::string formatMetrics(const std::map<std::string, double>& metrics) const;
    std::string escapeXmlString(const std::string& str) const;
};

// YAML output formatter
class YamlFormatter : public OutputFormatter {
public:
    YamlFormatter() = default;
    ~YamlFormatter() override = default;
    
    bool initialize(const OutputConfig& config) override;
    bool write(const std::string& content) override;
    bool write(const std::vector<TestResult>& testResults) override;
    bool write(const std::map<std::string, double>& metrics) override;
    bool write(const std::string& title, const std::string& content) override;
    void flush() override;
    void close() override;
    bool isOpen() const override;
    
private:
    std::ofstream outputFile_;
    std::ostringstream buffer_;
    OutputConfig config_;
    
    std::string formatTestResult(const TestResult& result) const;
    std::string formatMetrics(const std::map<std::string, double>& metrics) const;
    std::string escapeYamlString(const std::string& str) const;
};

// HTML output formatter
class HtmlFormatter : public OutputFormatter {
public:
    HtmlFormatter() = default;
    ~HtmlFormatter() override = default;
    
    bool initialize(const OutputConfig& config) override;
    bool write(const std::string& content) override;
    bool write(const std::vector<TestResult>& testResults) override;
    bool write(const std::map<std::string, double>& metrics) override;
    bool write(const std::string& title, const std::string& content) override;
    void flush() override;
    void close() override;
    bool isOpen() const override;
    
private:
    std::ofstream outputFile_;
    std::ostringstream buffer_;
    OutputConfig config_;
    
    std::string formatTestResult(const TestResult& result) const;
    std::string formatMetrics(const std::map<std::string, double>& metrics) const;
    std::string escapeHtmlString(const std::string& str) const;
};

// Markdown output formatter
class MarkdownFormatter : public OutputFormatter {
public:
    MarkdownFormatter() = default;
    ~MarkdownFormatter() override = default;
    
    bool initialize(const OutputConfig& config) override;
    bool write(const std::string& content) override;
    bool write(const std::vector<TestResult>& testResults) override;
    bool write(const std::map<std::string, double>& metrics) override;
    bool write(const std::string& title, const std::string& content) override;
    void flush() override;
    void close() override;
    bool isOpen() const override;
    
private:
    std::ofstream outputFile_;
    std::ostringstream buffer_;
    OutputConfig config_;
    
    std::string formatTestResult(const TestResult& result) const;
    std::string formatMetrics(const std::map<std::string, double>& metrics) const;
    std::string escapeMarkdownString(const std::string& str) const;
};

// Output formatter manager for handling multiple output formats
class OutputManager {
public:
    OutputManager() = default;
    ~OutputManager() = default;
    
    // Set output configuration
    void setConfig(const OutputConfig& config);
    
    // Get output configuration
    const OutputConfig& getConfig() const;
    
    // Write different types of data
    bool write(const std::string& content);
    bool write(const std::vector<TestResult>& testResults);
    bool write(const std::map<std::string, double>& metrics);
    bool write(const std::string& title, const std::string& content);
    
    // Set active formatter
    bool setFormatter(OutputFormat format);
    
    // Get active formatter
    OutputFormat getCurrentFormat() const;
    
    // Close current output
    void close();
    
    // Flush output buffer
    void flush();
    
    // Check if output is open
    bool isOpen() const;
    
    // Generate timestamp
    std::string generateTimestamp(const std::string& format = "ISO") const;
    
    // Generate filename with timestamp
    std::string generateFilename(const std::string& baseName) const;
    
    // Add metadata
    void addMetadata(const std::string& key, const std::string& value);
    
    // Clear metadata
    void clearMetadata();

private:
    std::unique_ptr<OutputFormatter> formatter_;
    OutputConfig config_;
    std::map<std::string, std::string> metadata_;
    std::chrono::system_clock::time_point sessionStart_;
};

} // namespace nlm