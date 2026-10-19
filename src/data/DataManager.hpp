// Data Manager
// This class provides data management capabilities for NLM experiments

#pragma once

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <unordered_map>
#include <cmath>
#include <numeric>

namespace nlm {
namespace data {

class DataManager {
public:
    DataManager();
    ~DataManager();
    
    // Data export and import
    bool exportData(const std::string& format, const std::string& outputFile);
    bool importData(const std::string& format, const std::string& inputFile);
    
    // Data analysis
    bool analyzeData(const std::string& dataFile);
    bool analyzeResults(const std::string& resultsFile);
    std::string generateStatisticalReport(const std::string& dataFile) const;
    
    // Data comparison and correlation
    bool compareData(const std::string& file1, const std::string& file2);
    bool correlateData(const std::string& file1, const std::string& file2);
    std::string generateCorrelationReport(const std::string& file1, const std::string& file2) const;
    
    // Statistical analysis
    struct StatisticalSummary {
        std::string operation;
        double mean;
        double median;
        double stddev;
        double min;
        double max;
        double p95;
        double p99;
        double variance;
        size_t count;
        
        StatisticalSummary() : mean(0), median(0), stddev(0), min(0), max(0), 
                              p95(0), p99(0), variance(0), count(0) {}
    };
    
    StatisticalSummary calculateStatistics(const std::vector<double>& data) const;
    bool performStatisticalTests(const std::string& file1, const std::string& file2);
    
    // Data visualization
    bool generateVisualization(const std::string& dataFile, const std::string& outputFile);
    bool generateComparisonChart(const std::string& file1, const std::string& file2, const std::string& outputFile);
    std::string generateSummaryTable(const std::string& dataFile) const;
    
    // Data management
    bool addData(const std::string& name, const std::vector<double>& data);
    bool removeData(const std::string& name);
    bool updateData(const std::string& name, const std::vector<double>& newData);
    std::vector<std::string> getAvailableDataSets() const;
    
    // Results management
    bool saveResults(const std::string& experimentName, const std::map<std::string, std::vector<double>>& results);
    bool loadResults(const std::string& experimentName);
    std::map<std::string, std::vector<double>> getResults(const std::string& experimentName) const;
    
    // Data validation
    bool validateData(const std::string& dataFile);
    bool validateResults(const std::string& resultsFile);
    std::vector<std::string> getValidationErrors() const;
    
private:
    // Internal data storage
    std::map<std::string, std::vector<double>> dataSets_;
    std::map<std::string, std::map<std::string, std::vector<double>>> experimentResults_;
    std::vector<std::string> validationErrors_;
    
    // Statistical calculation helpers
    static double calculateMean(const std::vector<double>& data);
    static double calculateMedian(const std::vector<double>& data);
    static double calculateStddev(const std::vector<double>& data, double mean);
    static double calculatePercentile(const std::vector<double>& data, double percentile);
    static double calculateVariance(const std::vector<double>& data, double mean);
    
    // Data file format support
    bool exportToJSON(const std::string& outputFile) const;
    bool exportToCSV(const std::string& outputFile) const;
    bool exportToYAML(const std::string& outputFile) const;
    bool importFromJSON(const std::string& inputFile);
    bool importFromCSV(const std::string& inputFile);
    bool importFromYAML(const std::string& inputFile);
    
    // Statistical analysis helpers
    double computeCovariance(const std::vector<double>& data1, const std::vector<double>& data2) const;
    double computeCorrelationCoefficient(const std::vector<double>& data1, const std::vector<double>& data2) const;
    
    // Data validation
    bool validateJSON(const std::string& content) const;
    bool validateCSV(const std::string& content) const;
    bool validateYAML(const std::string& content) const;
    
    // Report generation
    std::string generateStatisticalSummary(const std::vector<double>& data) const;
    std::string generateComparisonSummary(const std::string& name1, const std::vector<double>& data1,
                                          const std::string& name2, const std::vector<double>& data2) const;
};

} // namespace data
} // namespace nlm
