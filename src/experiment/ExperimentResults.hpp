#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "ExperimentProtocol.hpp"
#include "ExperimentStateManager.hpp"

namespace nlm {

// Experiment results configuration
struct ExperimentResultsConfig {
    std::string experimentName;
    std::chrono::system_clock::time_point creationTime;
    std::string description;
    std::string version;
    bool enableCompression;
    bool enableEncryption;
    std::string dataFormat;
    std::map<std::string, std::string> exportSettings;
    
    ExperimentResultsConfig() 
        : creationTime(std::chrono::system_clock::now()), enableCompression(true), 
          enableEncryption(false), dataFormat("json") {}
};

// Experiment statistics
struct ExperimentStatistics {
    std::string statisticName;
    double value;
    double standardDeviation;
    double variance;
    size_t sampleSize;
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, std::string> metadata;
    
    ExperimentStatistics() : value(0.0), standardDeviation(0.0), variance(0.0), 
                            sampleSize(0), timestamp(std::chrono::system_clock::now()) {}
};

// Comparison result
struct ExperimentComparison {
    std::string experimentName1;
    std::string experimentName2;
    std::map<std::string, double> metricDifferences;
    std::map<std::string, double> metricRatios;
    std::map<std::string, double> metricPercentages;
    std::chrono::system_clock::time_point comparisonTime;
    std::string comparisonMethod;
    std::map<std::string, std::string> comparisonMetadata;
    
    ExperimentComparison() : comparisonTime(std::chrono::system_clock::now()) {}
};

class ExperimentResults {
public:
    ExperimentResults(const std::string& experimentName);
    ~ExperimentResults();
    
    // Disable copying, enable moving
    ExperimentResults(const ExperimentResults&) = delete;
    ExperimentResults& operator=(const ExperimentResults&) = delete;
    ExperimentResults(ExperimentResults&&) noexcept;
    ExperimentResults& operator=(ExperimentResults&&) noexcept;
    
    // Results storage
    void addMetric(const std::string& name, double value, const std::string& unit = "");
    void addObservation(const std::string& name, const std::vector<double>& data);
    void addParameter(const std::string& name, const ConfigValue& value);
    void addStatistic(const std::string& name, const ExperimentStatistics& statistic);
    
    // Results querying
    double getMetric(const std::string& name) const;
    std::vector<double> getObservation(const std::string& name) const;
    ConfigValue getParameter(const std::string& name) const;
    ExperimentStatistics getStatistic(const std::string& name) const;
    
    // Results analysis
    double calculateStatistic(const std::string& metricName, const std::string& statistic) const;
    std::vector<double> normalizeResults(const std::string& metricName) const;
    std::vector<double> standardizeResults(const std::string& metricName) const;
    std::vector<double> scaleResults(const std::string& metricName, double min, double max) const;
    std::vector<double> logTransformResults(const std::string& metricName) const;
    
    // Statistical operations
    double calculateMean(const std::vector<double>& data) const;
    double calculateStandardDeviation(const std::vector<double>& data, double mean = 0.0) const;
    double calculateVariance(const std::vector<double>& data, double mean = 0.0) const;
    double calculateMedian(const std::vector<double>& data) const;
    double calculatePercentile(const std::vector<double>& data, double percentile) const;
    double calculateCorrelation(const std::vector<double>& x, const std::vector<double>& y) const;
    double calculateRegression(const std::vector<double>& x, const std::vector<double>& y) const;
    
    // Results export
    bool exportToJSON(const std::string& filepath) const;
    bool exportToCSV(const std::string& filepath) const;
    bool exportToXML(const std::string& filepath) const;
    bool exportToDatabase(const std::string& connectionString, const std::string& tableName) const;
    
    // Results import
    bool loadFromJSON(const std::string& filepath);
    bool loadFromCSV(const std::string& filepath);
    bool loadFromXML(const std::string& filepath);
    
    // Results access
    const std::map<std::string, double>& getMetrics() const;
    const std::map<std::string, std::vector<double>>& getObservations() const;
    const std::map<std::string, ConfigValue>& getParameters() const;
    const std::map<std::string, ExperimentStatistics>& getStatistics() const;
    std::string getName() const;
    std::chrono::system_clock::time_point getCreationTime() const;
    
    // Results manipulation
    void clear();
    bool merge(const ExperimentResults& otherResults);
    bool filter(const std::function<bool(const std::string&, double)>& predicate);
    bool transform(const std::function<double(double)> transformFunc);
    bool aggregate(const ExperimentResults& otherResults);
    
    // Results validation
    bool validate() const;
    bool isValidMetric(const std::string& name) const;
    bool isValidObservation(const std::string& name, const std::vector<double>& data) const;
    bool isValidParameter(const std::string& name, const ConfigValue& value) const;
    
    // Results comparison
    ExperimentComparison compare(const ExperimentResults& otherResults) const;
    bool isComparable(const ExperimentResults& otherResults) const;
    
    // Results metadata
    void setMetadata(const std::string& key, const std::string& value);
    std::string getMetadata(const std::string& key) const;
    std::map<std::string, std::string> getAllMetadata() const;
    
    // Results optimization
    bool optimizeStorage();
    bool compressResults();
    bool encryptResults(const std::string& password);
    bool decryptResults(const std::string& password);
    
    // Results cleanup
    bool cleanup();
    bool removeExpiredData(const std::chrono::system_clock::time_point& cutoffTime);
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm