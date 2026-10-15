#pragma once

/**
 * Benchmark Data Export System for NLM
 * 
 * Provides comprehensive export functionality for benchmark results,
 * including CSV and JSON formats for visualization and analysis.
 */

#include "BenchmarkSuite.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <memory>
#include <chrono>
#include <algorithm>

namespace nlm {

/**
 * Export format enumeration
 */
enum class ExportFormat {
    NONE,
    CSV,
    JSON,
    BOTH
};

/**
 * Export configuration
 */
struct ExportConfig {
    ExportFormat format;           // Export format
    std::string outputPath;        // Output directory/file path
    bool includeMetadata;          // Include benchmark metadata
    bool includeStatistics;        // Include statistical analysis
    bool includeRawData;           // Include raw measurement data
    bool prettyPrint;              // Pretty-print JSON output
    std::string filenamePrefix;    // Prefix for output files
    std::string timestampFormat;   // Timestamp format for filenames
    
    ExportConfig()
        : format(ExportFormat::CSV), includeMetadata(true), includeStatistics(true),
          includeRawData(true), prettyPrint(true), timestampFormat("YYYYMMDD_HHMMSS") {}
};

/**
 * Benchmark result metadata
 */
struct BenchmarkMetadata {
    std::string benchmarkName;
    std::string version;
    std::string description;
    std::chrono::high_resolution_clock::time_point timestamp;
    std::string configFile;
    std::string platformInfo;
    std::string compilerInfo;
    
    BenchmarkMetadata()
        : version("1.0.0"), description("NLM Benchmark Results") {}
};

/**
 * Statistical summary
 */
struct StatisticalSummary {
    std::string metricName;
    double mean;
    double median;
    double stdDev;
    double min;
    double max;
    double coefficientOfVariation;
    double percentile25;
    double percentile50;
    double percentile75;
    double percentile95;
    
    StatisticalSummary(const std::string& name = "")
        : metricName(name), mean(0.0), median(0.0), stdDev(0.0), min(0.0), max(0.0),
          coefficientOfVariation(0.0), percentile25(0.0), percentile50(0.0),
          percentile75(0.0), percentile95(0.0) {}
};

/**
 * CSV exporter
 */
class CSVExporter {
public:
    CSVExporter();
    ~CSVExporter();
    
    // Export functions
    bool exportBenchmarkResults(const std::vector<BenchmarkResult>& results,
                                const std::string& filename,
                                const BenchmarkMetadata& metadata = BenchmarkMetadata());
    
    bool exportBenchmarkResults(const std::vector<BenchmarkResult>& results,
                                const ExportConfig& config);
    
    bool exportComparisonResults(const std::vector<BenchmarkResult>& baseline,
                                 const std::vector<BenchmarkResult>& current,
                                 const std::string& filename);
    
    // Statistical export
    bool exportStatisticalAnalysis(const std::vector<StatisticalSummary>& stats,
                                   const std::string& filename);
    
    // Raw data export
    bool exportRawMeasurements(const std::vector<double>& measurements,
                               const std::string& filename,
                               const std::string& metricName = "measurement");
    
    // Configuration export
    bool exportConfiguration(const Config& config, const std::string& filename);
    
    // Generate CSV content
    std::string generateBenchmarkCSV(const std::vector<BenchmarkResult>& results,
                                      const BenchmarkMetadata& metadata);
    
    std::string generateStatisticalCSV(const std::vector<StatisticalSummary>& stats);
    std::string generateRawDataCSV(const std::vector<double>& measurements,
                                   const std::string& metricName);
    
private:
    std::string escapeCSVString(const std::string& str) const;
    std::string generateTimestamp() const;
};

/**
 * JSON exporter
 */
class JSONExporter {
public:
    JSONExporter();
    ~JSONExporter();
    
    // Export functions
    bool exportBenchmarkResults(const std::vector<BenchmarkResult>& results,
                                const std::string& filename,
                                const BenchmarkMetadata& metadata = BenchmarkMetadata());
    
    bool exportBenchmarkResults(const std::vector<BenchmarkResult>& results,
                                const ExportConfig& config);
    
    bool exportComparisonResults(const std::vector<BenchmarkResult>& baseline,
                                 const std::vector<BenchmarkResult>& current,
                                 const std::string& filename);
    
    // Statistical export
    bool exportStatisticalAnalysis(const std::vector<StatisticalSummary>& stats,
                                   const std::string& filename);
    
    // Configuration export
    bool exportConfiguration(const Config& config, const std::string& filename);
    
    // Generate JSON content
    std::string generateBenchmarkJSON(const std::vector<BenchmarkResult>& results,
                                       const BenchmarkMetadata& metadata);
    
    std::string generateStatisticalJSON(const std::vector<StatisticalSummary>& stats);
    std::string generateConfigurationJSON(const Config& config);
    
private:
    std::string generateTimestampJSON(const std::chrono::high_resolution_clock::time_point& tp) const;
    std::string escapeJSONString(const std::string& str) const;
    std::string generatePlatformInfo() const;
    std::string generateCompilerInfo() const;
};

/**
 * Benchmark Exporter - Main export interface
 */
class BenchmarkExporter {
public:
    BenchmarkExporter();
    ~BenchmarkExporter();
    
    // Configuration
    void configure(const ExportConfig& config) { config_ = config; }
    void setMetadata(const BenchmarkMetadata& metadata) { metadata_ = metadata; }
    void setBenchmarkName(const std::string& name) { metadata_.benchmarkName = name; }
    
    // Export functions
    bool exportResults(const std::vector<BenchmarkResult>& results,
                      const std::string& baseFilename);
    
    bool exportResults(const std::vector<BenchmarkResult>& results,
                      const std::vector<BenchmarkResult>& baseline,
                      const std::string& baseFilename);
    
    bool exportStatisticalAnalysis(const std::vector<StatisticalSummary>& stats,
                                   const std::string& filename);
    
    // Convenience functions
    bool exportToCSV(const std::vector<BenchmarkResult>& results,
                     const std::string& filename);
    
    bool exportToJSON(const std::vector<BenchmarkResult>& results,
                      const std::string& filename);
    
    bool exportToBoth(const std::vector<BenchmarkResult>& results,
                      const std::string& baseFilename);
    
    // File operations
    bool exportToDirectory(const std::vector<BenchmarkResult>& results,
                          const std::string& directory);
    
    std::vector<std::string> getExportedFiles() const { return exportedFiles_; }
    
    // Get generated content
    std::string getCSVContent(const std::vector<BenchmarkResult>& results);
    std::string getJSONContent(const std::vector<BenchmarkResult>& results);
    
private:
    ExportConfig config_;
    BenchmarkMetadata metadata_;
    std::vector<std::string> exportedFiles_;
    
    std::unique_ptr<CSVExporter> csvExporter_;
    std::unique_ptr<JSONExporter> jsonExporter_;
    
    bool ensureDirectoryExists(const std::string& path) const;
    std::string generateFilename(const std::string& base, const std::string& extension) const;
}; // class BenchmarkExporter

} // namespace nlm