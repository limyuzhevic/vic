// NLM Result Exporter
// Exports experiment results in various formats

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace nlm {

// Result export format types
enum class ExportFormat {
    Unknown,
    JSON,
    CSV,
    Text,
    YAML,
    HDF5,
    Parquet,
    ApacheArrow
};

// Result export configuration
struct ExportConfig {
    std::string outputFile;
    ExportFormat format = ExportFormat::JSON;
    bool includeMetadata = true;
    bool includeStatistics = true;
    bool includeRawResults = true;
    std::string compression;  // none, gzip, bz2, xz
    std::string delimiter;    // "," for CSV, "\t" for TSV
    std::string decimalSeparator;  // ".", ","
    bool quoteAllStrings = true;
    bool escapeSpecialChars = true;
    std::map<std::string, std::string> columnOrder;  // Custom column ordering
    std::map<std::string, std::string> columnFormatting;  // Custom formatting
    
    ExportConfig() {
        delimiter = ",";
        decimalSeparator = ".";
    }
};

// Result export statistics
struct ExportStatistics {
    uint64_t totalResultsExported = 0;
    double exportTime = 0.0;
    uint64_t bytesWritten = 0;
    std::string outputFile;
    ExportFormat format;
    std::chrono::steady_clock::time_point exportTimeStamp;
    
    ExportStatistics() {
        exportTimeStamp = std::chrono::steady_clock::now();
    }
};

// Main result exporter class
class ResultExporter {
public:
    ResultExporter();
    ~ResultExporter();
    
    // Export results to file
    bool exportResults(const std::string& inputFile, const ExportConfig& config);
    
    // Export results from memory to file
    bool exportResults(const std::string& filename, const ExportConfig& config,
                      const std::vector<std::map<std::string, std::string>>& results);
    
    // Export JSON format
    bool exportToJSON(const std::string& filename, const ExportConfig& config,
                     const std::vector<std::map<std::string, std::string>>& results);
    
    // Export CSV format
    bool exportToCSV(const std::string& filename, const ExportConfig& config,
                    const std::vector<std::map<std::string, std::string>>& results);
    
    // Export text format
    bool exportToText(const std::string& filename, const ExportConfig& config,
                     const std::vector<std::map<std::string, std::string>>& results);
    
    // Export YAML format
    bool exportToYAML(const std::string& filename, const ExportConfig& config,
                     const std::vector<std::map<std::string, std::string>>& results);
    
    // Get last export statistics
    const ExportStatistics& getLastExportStats() const { return lastExportStats; }
    
    // Static exporter instance
    static ResultExporter& getInstance() {
        static ResultExporter instance;
        return instance;
    }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    ExportStatistics lastExportStats;
    
    // Format conversion helpers
    std::string formatValue(const std::string& value, const std::string& format) const;
    std::string escapeString(const std::string& value, bool escapeSpecial) const;
    std::string generateTimestamp() const;
    
    // File operations with compression support
    bool writeToFile(const std::string& filename, const std::string& content,
                    const std::string& compression);
    std::string compressContent(const std::string& content, const std::string& compression);
    
    // JSON formatting helpers
    std::string formatJSONValue(const std::map<std::string, std::string>& value) const;
    
    // CSV formatting helpers
    std::string formatCSVRow(const std::map<std::string, std::string>& row,
                            const ExportConfig& config) const;
    std::string getCSVHeader(const std::vector<std::map<std::string, std::string>>& results,
                            const ExportConfig& config) const;
    
    // Text formatting helpers
    std::string formatTextTable(const std::vector<std::map<std::string, std::string>>& results,
                               const ExportConfig& config) const;
    
    // YAML formatting helpers
    std::string formatYAMLValue(const std::map<std::string, std::string>& value) const;
    
    // Validation
    bool validateExportConfig(const ExportConfig& config) const;
    bool validateResults(const std::vector<std::map<std::string, std::string>>& results) const;
};

} // namespace nlm