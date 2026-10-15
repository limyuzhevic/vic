#pragma once

#include "ExperimentResult.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>

namespace nlm {

ExperimentResult::ExperimentResult(const std::string& experimentName) 
    : experimentName(experimentName), status(ResultStatus::PENDING) {
    timestamp = std::chrono::system_clock::now();
}

ExperimentResult::~ExperimentResult() = default;

bool ExperimentResult::addMetric(const std::string& name, double value) {
    metrics[name] = value;
    return true;
}

bool ExperimentResult::addMetric(const std::string& name, float value) {
    return addMetric(name, static_cast<double>(value));
}

bool ExperimentResult::addMetric(const std::string& name, int value) {
    return addMetric(name, static_cast<double>(value));
}

bool ExperimentResult::addMetric(const std::string& name, const std::string& value) {
    stringMetrics[name] = value;
    return true;
}

const std::map<std::string, double>& ExperimentResult::getMetrics() const {
    return metrics;
}

const std::map<std::string, std::string>& ExperimentResult::getStringMetrics() const {
    return stringMetrics;
}

double ExperimentResult::getMetric(const std::string& name) const {
    auto it = metrics.find(name);
    if (it != metrics.end()) {
        return it->second;
    }
    return 0.0;
}

std::string ExperimentResult::getStringMetric(const std::string& name) const {
    auto it = stringMetrics.find(name);
    if (it != stringMetrics.end()) {
        return it->second;
    }
    return "";
}

bool ExperimentResult::hasMetric(const std::string& name) const {
    return metrics.find(name) != metrics.end() || 
           stringMetrics.find(name) != stringMetrics.end();
}

const std::vector<ExperimentResult::SimulationStepData>& ExperimentResult::getStepData() const {
    return stepData;
}

bool ExperimentResult::addStepData(SimulationStepData data) {
    stepData.push_back(data);
    return true;
}

const std::vector<std::string>& ExperimentResult::getNotes() const {
    return notes;
}

bool ExperimentResult::addNote(const std::string& note) {
    notes.push_back(note);
    return true;
}

ResultStatus ExperimentResult::getStatus() const {
    return status;
}

void ExperimentResult::setStatus(ResultStatus newStatus) {
    status = newStatus;
}

const std::chrono::system_clock::time_point& ExperimentResult::getTimestamp() const {
    return timestamp;
}

void ExperimentResult::setTimestamp(const std::chrono::system_clock::time_point& time) {
    timestamp = time;
}

const std::string& ExperimentResult::getExperimentName() const {
    return experimentName;
}

void ExperimentResult::setExperimentName(const std::string& name) {
    experimentName = name;
}

const std::string& ExperimentResult::getConfiguration() const {
    return configuration;
}

void ExperimentResult::setConfiguration(const std::string& config) {
    configuration = config;
}

const std::map<std::string, std::string>& ExperimentResult::getParameters() const {
    return parameters;
}

bool ExperimentResult::setParameter(const std::string& key, const std::string& value) {
    parameters[key] = value;
    return true;
}

std::string ExperimentResult::getParameter(const std::string& key) const {
    auto it = parameters.find(key);
    if (it != parameters.end()) {
        return it->second;
    }
    return "";
}

const std::vector<std::string>& ExperimentResult::getTags() const {
    return tags;
}

void ExperimentResult::addTag(const std::string& tag) {
    tags.push_back(tag);
}

bool ExperimentResult::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // TODO: Implement proper serialization
    file << "ExperimentResult serialization not yet implemented\n";
    
    file.close();
    return true;
}

bool ExperimentResult::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // TODO: Implement proper deserialization
    std::string line;
    std::getline(file, line);
    
    file.close();
    return true;
}

std::string ExperimentResult::generateSummary() const {
    std::stringstream ss;
    ss << "Experiment Result: " << experimentName << "\n";
    ss << "===================\n\n";
    ss << "Status: " << static_cast<int>(status) << "\n";
    ss << "Timestamp: " << std::chrono::system_clock::to_time_t(timestamp) << "\n";
    ss << "Configuration: " << configuration << "\n";
    ss << "Parameters: ";
    for (const auto& pair : parameters) {
        ss << pair.first << "=" << pair.second << "; ";
    }
    ss << "\n\n";
    
    if (!metrics.empty()) {
        ss << "Metrics:\n";
        for (const auto& metric : metrics) {
            ss << "  " << metric.first << ": " << metric.second << "\n";
        }
        ss << "\n";
    }
    
    if (!stringMetrics.empty()) {
        ss << "String Metrics:\n";
        for (const auto& metric : stringMetrics) {
            ss << "  " << metric.first << ": " << metric.second << "\n";
        }
        ss << "\n";
    }
    
    ss << "Step Data: " << stepData.size() << " entries\n";
    
    if (!notes.empty()) {
        ss << "\nNotes:\n";
        for (const auto& note : notes) {
            ss << "  - " << note << "\n";
        }
    }
    
    return ss.str();
}

std::string ExperimentResult::generateComparison(
    const ExperimentResult& other) const {
    std::stringstream ss;
    ss << "Comparison: " << experimentName << " vs " << other.experimentName << "\n";
    ss << "============================\n\n";
    
    // Compare metrics
    std::set<std::string> allMetricNames;
    for (const auto& metric : metrics) {
        allMetricNames.insert(metric.first);
    }
    for (const auto& metric : other.metrics) {
        allMetricNames.insert(metric.first);
    }
    
    for (const auto& metricName : allMetricNames) {
        double value1 = (metrics.find(metricName) != metrics.end()) ? metrics.at(metricName) : 0.0;
        double value2 = (other.metrics.find(metricName) != other.metrics.end()) ? other.metrics.at(metricName) : 0.0;
        double diff = value1 - value2;
        double percentDiff = (value1 != 0) ? (diff / value1) * 100.0 : 0.0;
        
        ss << metricName << ": " << value1 << " vs " << value2 
           << " (diff: " << diff << ", " << percentDiff << "%)\n";
    }
    
    return ss.str();
}

double ExperimentResult::computeStatisticalMeasure(const std::string& measure, 
                                                   const std::vector<ExperimentResult>& results) const {
    if (results.empty()) {
        return 0.0;
    }
    
    std::vector<double> values;
    
    for (const auto& result : results) {
        if (result.metrics.find(measure) != result.metrics.end()) {
            values.push_back(result.metrics.at(measure));
        }
    }
    
    if (values.empty()) {
        return 0.0;
    }
    
    if (measure == "mean" || measure == "average") {
        double sum = 0.0;
        for (double value : values) sum += value;
        return sum / values.size();
    } else if (measure == "std" || measure == "standard_deviation") {
        double mean = computeStatisticalMeasure("mean", results);
        double sum = 0.0;
        for (double value : values) sum += (value - mean) * (value - mean);
        return std::sqrt(sum / values.size());
    } else if (measure == "min") {
        return *std::min_element(values.begin(), values.end());
    } else if (measure == "max") {
        return *std::max_element(values.begin(), values.end());
    } else if (measure == "median") {
        std::vector<double> sorted = values;
        std::sort(sorted.begin(), sorted.end());
        size_t mid = sorted.size() / 2;
        return (sorted.size() % 2 == 0) ? (sorted[mid-1] + sorted[mid]) / 2 : sorted[mid];
    }
    
    return 0.0;
}

} // namespace nlm
