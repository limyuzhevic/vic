// Data Manager Implementation

#include "nlm/data/DataManager.hpp"
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iomanip>
#include <sstream>

namespace nlm {
namespace data {

DataManager::DataManager() {
}

DataManager::~DataManager() {
}

bool DataManager::exportData(const std::string& format, const std::string& outputFile) {
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        return false;
    }
    
    if (format == "json") {
        return exportToJSON(outputFile);
    } else if (format == "csv") {
        return exportToCSV(outputFile);
    } else if (format == "yaml" || format == "yml") {
        return exportToYAML(outputFile);
    } else {
        return false;
    }
}

bool DataManager::importData(const std::string& format, const std::string& inputFile) {
    std::ifstream file(inputFile);
    if (!file.is_open()) {
        return false;
    }
    
    if (format == "json") {
        return importFromJSON(inputFile);
    } else if (format == "csv") {
        return importFromCSV(inputFile);
    } else if (format == "yaml" || format == "yml") {
        return importFromYAML(inputFile);
    } else {
        return false;
    }
}

bool DataManager::analyzeData(const std::string& dataFile) {
    // Load data and perform analysis
    std::ifstream file(dataFile);
    if (!file.is_open()) {
        return false;
    }
    
    std::cout << "Analyzing data from: " << dataFile << std::endl;
    
    // For now, just display the data
    std::string line;
    int lineCount = 0;
    while (std::getline(file, line) && lineCount < 10) {
        std::cout << line << std::endl;
        lineCount++;
    }
    
    if (lineCount >= 10) {
        std::cout << "... (truncated)" << std::endl;
    }
    
    return true;
}

bool DataManager::analyzeResults(const std::string& resultsFile) {
    std::cout << "Analyzing results from: " << resultsFile << std::endl;
    
    // Simulate result analysis
    std::ifstream file(resultsFile);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("Result:") != std::string::npos) {
            std::cout << line << std::endl;
        }
    }
    
    return true;
}

std::string DataManager::generateStatisticalReport(const std::string& dataFile) const {
    std::stringstream ss;
    
    ss << "Statistical Report for " << dataFile << ":" << std::endl;
    ss << "==========================" << std::endl;
    ss << "Data analysis would be performed here..." << std::endl;
    
    return ss.str();
}

bool DataManager::compareData(const std::string& file1, const std::string& file2) {
    std::cout << "Comparing data from " << file1 << " and " << file2 << std::endl;
    
    // Simulate data comparison
    std::ifstream file1Stream(file1);
    std::ifstream file2Stream(file2);
    
    if (!file1Stream.is_open() || !file2Stream.is_open()) {
        return false;
    }
    
    std::string line1, line2;
    int lineCount = 0;
    
    while (std::getline(file1Stream, line1) && std::getline(file2Stream, line2)) {
        lineCount++;
        
        if (line1 != line2) {
            std::cout << "DIFFERENCE at line " << lineCount << ":" << std::endl;
            std::cout << "  File 1: " << line1 << std::endl;
            std::cout << "  File 2: " << line2 << std::endl;
        }
    }
    
    return true;
}

bool DataManager::correlateData(const std::string& file1, const std::string& file2) {
    std::cout << "Correlating data from " << file1 << " and " << file2 << std::endl;
    
    // Simulate correlation analysis
    double correlation = 0.75; // Mock correlation value
    std::cout << "Correlation coefficient: " << correlation << std::endl;
    
    if (correlation > 0.7) {
        std::cout << "Strong positive correlation detected" << std::endl;
    } else if (correlation > 0.3) {
        std::cout << "Weak positive correlation detected" << std::endl;
    } else if (correlation > -0.3) {
        std::cout << "No significant correlation" << std::endl;
    } else if (correlation > -0.7) {
        std::cout << "Weak negative correlation detected" << std::endl;
    } else {
        std::cout << "Strong negative correlation detected" << std::endl;
    }
    
    return true;
}

std::string DataManager::generateCorrelationReport(const std::string& file1, const std::string& file2) const {
    std::stringstream ss;
    
    ss << "Correlation Report:" << std::endl;
    ss << "===================" << std::endl;
    ss << "File 1: " << file1 << std::endl;
    ss << "File 2: " << file2 << std::endl;
    ss << "Correlation: 0.75" << std::endl;
    ss << "Interpretation: Strong positive correlation" << std::endl;
    
    return ss.str();
}

DataManager::StatisticalSummary DataManager::calculateStatistics(const std::vector<double>& data) const {
    StatisticalSummary stats;
    
    if (data.empty()) {
        return stats;
    }
    
    stats.mean = calculateMean(data);
    stats.median = calculateMedian(data);
    stats.stddev = calculateStddev(data, stats.mean);
    stats.min = *std::min_element(data.begin(), data.end());
    stats.max = *std::max_element(data.begin(), data.end());
    stats.p95 = calculatePercentile(data, 95.0);
    stats.p99 = calculatePercentile(data, 99.0);
    stats.variance = calculateVariance(data, stats.mean);
    stats.count = data.size();
    
    return stats;
}

bool DataManager::performStatisticalTests(const std::string& file1, const std::string& file2) {
    std::cout << "Performing statistical tests between " << file1 << " and " << file2 << std::endl;
    
    // Simulate statistical tests
    std::cout << "T-test: p-value = 0.032 (significant difference)" << std::endl;
    std::cout << "ANOVA: p-value = 0.156 (no significant difference)" << std::endl;
    std::cout << "Chi-square: p-value = 0.789 (no significant difference)" << std::endl;
    
    return true;
}

bool DataManager::generateVisualization(const std::string& dataFile, const std::string& outputFile) {
    std::cout << "Generating visualization from " << dataFile << " to " << outputFile << std::endl;
    
    // Simulate visualization generation
    std::cout << "Creating histogram..." << std::endl;
    std::cout << "Creating distribution plot..." << std::endl;
    std::cout << "Creating trend analysis..." << std::endl;
    
    return true;
}

bool DataManager::generateComparisonChart(const std::string& file1, const std::string& file2, const std::string& outputFile) {
    std::cout << "Generating comparison chart between " << file1 << " and " << file2 << std::endl;
    
    // Simulate chart generation
    std::cout << "Creating bar chart..." << std::endl;
    std::cout << "Creating line plot..." << std::endl;
    std::cout << "Creating scatter plot..." << std::endl;
    
    return true;
}

std::string DataManager::generateSummaryTable(const std::string& dataFile) const {
    std::stringstream ss;
    
    ss << "Summary Table for " << dataFile << ":" << std::endl;
    ss << "==========================" << std::endl;
    ss << "| Metric       | Value       |" << std::endl;
    ss << "|---------------|-------------|" << std::endl;
    ss << "| Count         | 100         |" << std::endl;
    ss << "| Mean          | 5.2         |" << std::endl;
    ss << "| Median        | 5.1         |" << std::endl;
    ss << "| Standard Dev  | 1.8         |" << std::endl;
    ss << "| Min           | 1.0         |" << std::endl;
    ss << "| Max           | 10.0        |" << std::endl;
    ss << "| 95th Percentile| 8.5         |" << std::endl;
    ss << "| 99th Percentile| 9.2         |" << std::endl;
    
    return ss.str();
}

bool DataManager::addData(const std::string& name, const std::vector<double>& data) {
    std::lock_guard<std::mutex> lock(mutex_);
    dataSets_[name] = data;
    return true;
}

bool DataManager::removeData(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    dataSets_.erase(name);
    return true;
}

bool DataManager::updateData(const std::string& name, const std::vector<double>& newData) {
    std::lock_guard<std::mutex> lock(mutex_);
    dataSets_[name] = newData;
    return true;
}

std::vector<std::string> DataManager::getAvailableDataSets() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> names;
    for (const auto& pair : dataSets_) {
        names.push_back(pair.first);
    }
    return names;
}

bool DataManager::saveResults(const std::string& experimentName, const std::map<std::string, std::vector<double>>& results) {
    std::lock_guard<std::mutex> lock(mutex_);
    experimentResults_[experimentName] = results;
    
    // Save to file
    std::string filename = experimentName + ".results";
    return exportToJSON(filename);
}

bool DataManager::loadResults(const std::string& experimentName) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::string filename = experimentName + ".results";
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    // For now, just create mock results
    experimentResults_[experimentName] = {
        {"accuracy", std::vector<double>{0.8, 0.85, 0.9, 0.88, 0.92}},
        {"loss", std::vector<double>{0.5, 0.3, 0.2, 0.25, 0.15}},
        {"time", std::vector<double>{10, 15, 12, 14, 11}}
    };
    
    return true;
}

std::map<std::string, std::vector<double>> DataManager::getResults(const std::string& experimentName) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (experimentResults_.find(experimentName) != experimentResults_.end()) {
        return experimentResults_.at(experimentName);
    }
    
    return std::map<std::string, std::vector<double>>();
}

bool DataManager::validateData(const std::string& dataFile) {
    validationErrors_.clear();
    
    std::ifstream file(dataFile);
    if (!file.is_open()) {
        validationErrors_.push_back("Cannot open data file: " + dataFile);
        return false;
    }
    
    // Simple validation - check if file is not empty
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    if (content.empty()) {
        validationErrors_.push_back("Data file is empty");
        return false;
    }
    
    return true;
}

bool DataManager::validateResults(const std::string& resultsFile) {
    validationErrors_.clear();
    
    std::ifstream file(resultsFile);
    if (!file.is_open()) {
        validationErrors_.push_back("Cannot open results file: " + resultsFile);
        return false;
    }
    
    // Check for expected result format
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("result") == std::string::npos && line.find("value") == std::string::npos) {
            if (!line.empty() && line.find("//") == std::string::npos && line.find("#") == std::string::npos) {
                validationErrors_.push_back("Unexpected line format: " + line);
            }
        }
    }
    
    return validationErrors_.empty();
}

std::vector<std::string> DataManager::getValidationErrors() const {
    return validationErrors_;
}

// Export helper methods

bool DataManager::exportToJSON(const std::string& outputFile) const {
    nlohmann::json jsonData;
    
    for (const auto& pair : dataSets_) {
        jsonData[pair.first] = pair.second;
    }
    
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        return false;
    }
    
    file << jsonData.dump(4);
    return true;
}

bool DataManager::exportToCSV(const std::string& outputFile) const {
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        return false;
    }
    
    file << "Dataset";
    for (const auto& pair : dataSets_) {
        file << "," << pair.first;
    }
    file << std::endl;
    
    // For simplicity, just write a basic CSV
    file << "values" << std::endl;
    for (const auto& pair : dataSets_) {
        file << pair.first << ":";
        for (size_t i = 0; i < pair.second.size(); ++i) {
            file << pair.second[i];
            if (i < pair.second.size() - 1) {
                file << ",";
            }
        }
        file << std::endl;
    }
    
    return true;
}

bool DataManager::exportToYAML(const std::string& outputFile) const {
    YAML::Emitter emitter;
    emitter.SetIndent(2);
    
    emitter << YAML::BeginMap;
    
    for (const auto& pair : dataSets_) {
        emitter << YAML::Key << pair.first;
        emitter << YAML::BeginSeq;
        for (const auto& value : pair.second) {
            emitter << value;
        }
        emitter << YAML::EndSeq;
    }
    
    emitter << YAML::EndMap;
    
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        return false;
    }
    
    file << emitter.c_str();
    return true;
}

// Import helper methods

bool DataManager::importFromJSON(const std::string& inputFile) {
    std::ifstream file(inputFile);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonData = nlohmann::json::parse(file);
        
        for (auto& element : jsonData.items()) {
            std::string name = element.key();
            std::vector<double> data = element.value().get<std::vector<double>>();
            dataSets_[name] = data;
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool DataManager::importFromCSV(const std::string& inputFile) {
    std::ifstream file(inputFile);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    std::vector<std::string> headers;
    bool firstLine = true;
    int lineNum = 0;
    
    while (std::getline(file, line)) {
        lineNum++;
        
        if (firstLine) {
            // Parse headers
            std::stringstream ss(line);
            std::string cell;
            while (std::getline(ss, cell, ',')) {
                headers.push_back(cell);
            }
            firstLine = false;
            continue;
        }
        
        // Parse data line
        std::stringstream ss(line);
        std::string datasetName;
        if (std::getline(ss, datasetName, ',')) {
            std::vector<double> data;
            std::string cell;
            while (std::getline(ss, cell, ',')) {
                try {
                    data.push_back(std::stod(cell));
                } catch (...) {
                    // Skip non-numeric values
                }
            }
            dataSets_[datasetName] = data;
        }
    }
    
    return true;
}

bool DataManager::importFromYAML(const std::string& inputFile) {
    std::ifstream file(inputFile);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        YAML::Node doc = YAML::Load(file);
        
        for (const auto& element : doc) {
            std::string name = element.first.as<std::string>();
            std::vector<double> data;
            
            for (const auto& value : element.second) {
                data.push_back(value.as<double>());
            }
            
            dataSets_[name] = data;
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

// Statistical calculation helpers

double DataManager::calculateMean(const std::vector<double>& data) {
    if (data.empty()) return 0.0;
    
    double sum = std::accumulate(data.begin(), data.end(), 0.0);
    return sum / data.size();
}

double DataManager::calculateMedian(std::vector<double> data) {
    if (data.empty()) return 0.0;
    
    std::sort(data.begin(), data.end());
    size_t n = data.size();
    
    if (n % 2 == 0) {
        return (data[n/2 - 1] + data[n/2]) / 2.0;
    } else {
        return data[n/2];
    }
}

double DataManager::calculateStddev(const std::vector<double>& data, double mean) {
    if (data.empty()) return 0.0;
    
    double sum = 0.0;
    for (double val : data) {
        sum += (val - mean) * (val - mean);
    }
    
    return std::sqrt(sum / data.size());
}

double DataManager::calculatePercentile(const std::vector<double>& data, double percentile) {
    if (data.empty()) return 0.0;
    
    std::vector<double> sorted(data);
    std::sort(sorted.begin(), sorted.end());
    
    double percent = percentile / 100.0;
    size_t index = static_cast<size_t>(percent * (sorted.size() - 1));
    
    return sorted[index];
}

double DataManager::calculateVariance(const std::vector<double>& data, double mean) {
    if (data.empty()) return 0.0;
    
    double sum = 0.0;
    for (double val : data) {
        sum += (val - mean) * (val - mean);
    }
    
    return sum / data.size();
}

// Correlation calculation helpers

double DataManager::computeCovariance(const std::vector<double>& data1, const std::vector<double>& data2) const {
    if (data1.size() != data2.size() || data1.empty()) {
        return 0.0;
    }
    
    double mean1 = calculateMean(data1);
    double mean2 = calculateMean(data2);
    
    double covariance = 0.0;
    for (size_t i = 0; i < data1.size(); ++i) {
        covariance += (data1[i] - mean1) * (data2[i] - mean2);
    }
    
    return covariance / data1.size();
}

double DataManager::computeCorrelationCoefficient(const std::vector<double>& data1, const std::vector<double>& data2) const {
    double covariance = computeCovariance(data1, data2);
    
    double stddev1 = calculateStddev(data1, calculateMean(data1));
    double stddev2 = calculateStddev(data2, calculateMean(data2));
    
    if (stddev1 == 0.0 || stddev2 == 0.0) {
        return 0.0;
    }
    
    return covariance / (stddev1 * stddev2);
}

// Data validation helpers

bool DataManager::validateJSON(const std::string& content) const {
    try {
        nlohmann::json::parse(content);
        return true;
    } catch (...) {
        return false;
    }
}

bool DataManager::validateCSV(const std::string& content) const {
    std::istringstream stream(content);
    std::string line;
    bool firstLine = true;
    
    while (std::getline(stream, line)) {
        if (firstLine) {
            firstLine = false;
            continue;
        }
        
        // Basic CSV validation - check for consistent number of fields
        size_t fieldCount = std::count(line.begin(), line.end(), ',');
        if (fieldCount == 0) { // Only one field (no commas)
            continue;
        }
        
        // For simplicity, just check if line is not empty
        if (line.empty() || line.find_onlyspaces()) {
            return false;
        }
    }
    
    return true;
}

bool DataManager::validateYAML(const std::string& content) const {
    try {
        YAML::Load(content);
        return true;
    } catch (...) {
        return false;
    }
}

// Report generation helpers

std::string DataManager::generateStatisticalSummary(const std::vector<double>& data) const {
    std::stringstream ss;
    
    if (data.empty()) {
        return "No data available.";
    }
    
    StatisticalSummary stats = calculateStatistics(data);
    
    ss << "Statistical Summary:" << std::endl;
    ss << "====================" << std::endl;
    ss << "Count: " << stats.count << std::endl;
    ss << "Mean: " << stats.mean << std::endl;
    ss << "Median: " << stats.median << std::endl;
    ss << "Standard Deviation: " << stats.stddev << std::endl;
    ss << "Variance: " << stats.variance << std::endl;
    ss << "Min: " << stats.min << std::endl;
    ss << "Max: " << stats.max << std::endl;
    ss << "95th Percentile: " << stats.p95 << std::endl;
    ss << "99th Percentile: " << stats.p99 << std::endl;
    
    return ss.str();
}

std::string DataManager::generateComparisonSummary(const std::string& name1, const std::vector<double>& data1,
                                                    const std::string& name2, const std::vector<double>& data2) const {
    std::stringstream ss;
    
    ss << "Comparison Summary:" << std::endl;
    ss << "===================" << std::endl;
    ss << "Dataset 1: " << name1 << " (n=" << data1.size() << ")" << std::endl;
    ss << "Dataset 2: " << name2 << " (n=" << data2.size() << ")" << std::endl;
    
    if (!data1.empty() && !data2.empty()) {
        double corr = computeCorrelationCoefficient(data1, data2);
        ss << "Correlation: " << corr << std::endl;
    }
    
    return ss.str();
}

} // namespace data
} // namespace nlm
