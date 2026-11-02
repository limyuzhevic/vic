// NLM Utilities Implementation
// This file provides the implementation for all utility functions

#include "NLMUtils.hpp"
#include <numeric>
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <cstring>

namespace nlm {

// MathUtils implementation
float MathUtils::dotProduct(const std::vector<float>& a, const std::vector<float>& b) {
    if (a.size() != b.size()) {
        return 0.0f;
    }
    float result = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        result += a[i] * b[i];
    }
    return result;
}

float MathUtils::magnitude(const std::vector<float>& v) {
    float result = 0.0f;
    for (float x : v) {
        result += x * x;
    }
    return std::sqrt(result);
}

float MathUtils::normalize(std::vector<float>& v) {
    float mag = magnitude(v);
    if (mag == 0.0f) {
        return 0.0f;
    }
    float invMag = 1.0f / mag;
    for (float& x : v) {
        x *= invMag;
    }
    return mag;
}

float MathUtils::distance(const std::vector<float>& a, const std::vector<float>& b) {
    if (a.size() != b.size()) {
        return 0.0f;
    }
    float result = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        float diff = a[i] - b[i];
        result += diff * diff;
    }
    return std::sqrt(result);
}

std::vector<float> MathUtils::smoothSignal(const std::vector<float>& signal, int windowSize) {
    if (signal.empty() || windowSize < 1) {
        return signal;
    }
    
    std::vector<float> result;
    result.reserve(signal.size());
    
    int halfWindow = windowSize / 2;
    for (size_t i = 0; i < signal.size(); ++i) {
        float sum = 0.0f;
        size_t count = 0;
        
        for (int j = -halfWindow; j <= halfWindow; ++j) {
            size_t idx = i + j;
            if (idx < signal.size()) {
                sum += signal[idx];
                count++;
            }
        }
        
        result.push_back(sum / static_cast<float>(count));
    }
    
    return result;
}

float MathUtils::movingAverage(const std::vector<float>& signal, size_t index, size_t windowSize) {
    if (signal.empty() || windowSize == 0 || index >= signal.size()) {
        return 0.0f;
    }
    
    size_t start = std::max<size_t>(0, static_cast<size_t>(index) - windowSize / 2);
    size_t end = std::min(signal.size(), start + windowSize);
    
    if (start >= end) {
        return signal[index];
    }
    
    float sum = std::accumulate(signal.begin() + start, signal.begin() + end, 0.0f);
    return sum / static_cast<float>(end - start);
}

float MathUtils::variance(const std::vector<float>& data) {
    if (data.size() < 2) {
        return 0.0f;
    }
    
    float mean = MathUtils::mean(data);
    float sumSquares = 0.0f;
    
    for (float x : data) {
        float diff = x - mean;
        sumSquares += diff * diff;
    }
    
    return sumSquares / static_cast<float>(data.size() - 1);
}

float MathUtils::standardDeviation(const std::vector<float>& data) {
    return std::sqrt(variance(data));
}

float MathUtils::mean(const std::vector<float>& data) {
    if (data.empty()) {
        return 0.0f;
    }
    return std::accumulate(data.begin(), data.end(), 0.0f) / static_cast<float>(data.size());
}

float MathUtils::median(std::vector<float> data) {
    if (data.empty()) {
        return 0.0f;
    }
    
    std::sort(data.begin(), data.end());
    size_t n = data.size();
    
    if (n % 2 == 0) {
        return (data[n/2 - 1] + data[n/2]) / 2.0f;
    } else {
        return data[n/2];
    }
}

float MathUtils::correlation(const std::vector<float>& x, const std::vector<float>& y) {
    if (x.size() != y.size() || x.size() < 2) {
        return 0.0f;
    }
    
    float xMean = MathUtils::mean(x);
    float yMean = MathUtils::mean(y);
    
    float numerator = 0.0f;
    float xVar = 0.0f;
    float yVar = 0.0f;
    
    for (size_t i = 0; i < x.size(); ++i) {
        float xDiff = x[i] - xMean;
        float yDiff = y[i] - yMean;
        numerator += xDiff * yDiff;
        xVar += xDiff * xDiff;
        yVar += yDiff * yDiff;
    }
    
    float denominator = std::sqrt(xVar * yVar);
    if (denominator == 0.0f) {
        return 0.0f;
    }
    
    return numerator / denominator;
}

void MathUtils::seedRandom(uint64_t seed) {
    static std::mt19937 generator(seed);
    generator.seed(seed);
}

float MathUtils::randomFloat(float min, float max) {
    static std::mt19937 generator(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<float> distribution(min, max);
    return distribution(generator);
}

int MathUtils::randomInt(int min, int max) {
    static std::mt19937 generator(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(generator);
}

bool MathUtils::randomBool(float probability) {
    return randomFloat(0.0f, 1.0f) < probability;
}

std::vector<float> MathUtils::arrayAdd(const std::vector<float>& a, const std::vector<float>& b) {
    if (a.size() != b.size()) {
        throw std::invalid_argument("Array sizes must match for addition");
    }
    
    std::vector<float> result(a.size());
    for (size_t i = 0; i < a.size(); ++i) {
        result[i] = a[i] + b[i];
    }
    return result;
}

std::vector<float> MathUtils::arrayMultiply(const std::vector<float>& a, float scalar) {
    std::vector<float> result(a.size());
    for (size_t i = 0; i < a.size(); ++i) {
        result[i] = a[i] * scalar;
    }
    return result;
}

std::vector<float> MathUtils::arrayConvolve(const std::vector<float>& a, const std::vector<float>& b) {
    std::vector<float> result(a.size() + b.size() - 1, 0.0f);
    
    for (size_t i = 0; i < a.size(); ++i) {
        for (size_t j = 0; j < b.size(); ++j) {
            result[i + j] += a[i] * b[j];
        }
    }
    
    return result;
}

void MathUtils::arrayCopy(const std::vector<float>& source, std::vector<float>& destination) {
    destination = source;
}

std::vector<size_t> MathUtils::findPattern(const std::vector<float>& signal, const std::vector<float>& pattern) {
    std::vector<size_t> matches;
    
    if (pattern.size() > signal.size()) {
        return matches;
    }
    
    for (size_t i = 0; i <= signal.size() - pattern.size(); ++i) {
        bool match = true;
        for (size_t j = 0; j < pattern.size(); ++j) {
            if (std::abs(signal[i + j] - pattern[j]) > 0.01f) {
                match = false;
                break;
            }
        }
        if (match) {
            matches.push_back(i);
        }
    }
    
    return matches;
}

bool MathUtils::matchPattern(const std::vector<float>& signal, const std::vector<float>& pattern, size_t startIndex) {
    if (startIndex + pattern.size() > signal.size()) {
        return false;
    }
    
    for (size_t i = 0; i < pattern.size(); ++i) {
        if (std::abs(signal[startIndex + i] - pattern[i]) > 0.01f) {
            return false;
        }
    }
    
    return true;
}

double MathUtils::getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration<double>(duration).count();
}

double MathUtils::elapsedTime(double startTime) {
    return getCurrentTime() - startTime;
}

// DataUtils implementation
bool DataUtils::saveToFile(const std::string& filepath, const std::vector<float>& data) {
    std::ofstream file(filepath, std::ios::binary);
    if (!file) {
        return false;
    }
    
    // Write data size
    size_t size = data.size();
    file.write(reinterpret_cast<const char*>(&size), sizeof(size));
    
    // Write data
    file.write(reinterpret_cast<const char*>(data.data()), size * sizeof(float));
    
    return !file.fail();
}

bool DataUtils::loadFromFile(const std::string& filepath, std::vector<float>& data) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file) {
        return false;
    }
    
    // Read data size
    size_t size;
    file.read(reinterpret_cast<char*>(&size), sizeof(size));
    
    if (file.fail()) {
        return false;
    }
    
    // Read data
    data.resize(size);
    file.read(reinterpret_cast<char*>(data.data()), size * sizeof(float));
    
    return !file.fail();
}

bool DataUtils::saveToCSV(const std::string& filepath, const std::vector<std::vector<float>>& data, const std::vector<std::string>& headers) {
    std::ofstream file(filepath);
    if (!file) {
        return false;
    }
    
    // Write headers
    for (size_t i = 0; i < headers.size(); ++i) {
        file << headers[i];
        if (i < headers.size() - 1) {
            file << ",";
        }
    }
    file << "\n";
    
    // Write data
    for (size_t i = 0; i < data.size(); ++i) {
        for (size_t j = 0; j < data[i].size(); ++j) {
            file << data[i][j];
            if (j < data[i].size() - 1) {
                file << ",";
            }
        }
        if (i < data.size() - 1) {
            file << "\n";
        }
    }
    
    return !file.fail();
}

bool DataUtils::loadFromCSV(const std::string& filepath, std::vector<std::vector<float>>& data) {
    std::ifstream file(filepath);
    if (!file) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        std::vector<float> row;
        std::stringstream ss(line);
        std::string token;
        
        while (std::getline(ss, token, ',')) {
            try {
                row.push_back(std::stof(token));
            } catch (...) {
                row.push_back(0.0f);
            }
        }
        
        data.push_back(row);
    }
    
    return !file.fail();
}

std::vector<float> DataUtils::calculateRunningStats(const std::vector<float>& data, size_t windowSize) {
    std::vector<float> result;
    if (data.empty() || windowSize == 0) {
        return result;
    }
    
    result.reserve(data.size());
    
    for (size_t i = 0; i < data.size(); ++i) {
        size_t start = std::max<size_t>(0, i - windowSize + 1);
        float sum = std::accumulate(data.begin() + start, data.begin() + i + 1, 0.0f);
        result.push_back(sum / static_cast<float>(i - start + 1));
    }
    
    return result;
}

float DataUtils::detectOutliers(const std::vector<float>& data, float threshold) {
    if (data.size() < 2) {
        return 0.0f;
    }
    
    float mean = MathUtils::mean(data);
    float std = MathUtils::standardDeviation(data);
    
    float outlierCount = 0;
    for (float x : data) {
        if (std::abs(x - mean) > threshold * std) {
            outlierCount++;
        }
    }
    
    return (outlierCount / static_cast<float>(data.size())) * 100.0f;
}

std::vector<float> DataUtils::normalizeData(const std::vector<float>& data, float min, float max) {
    if (data.empty()) {
        return data;
    }
    
    float dataMin = *std::min_element(data.begin(), data.end());
    float dataMax = *std::max_element(data.begin(), data.end());
    
    std::vector<float> result(data.size());
    for (size_t i = 0; i < data.size(); ++i) {
        float normalized = (data[i] - dataMin) / (dataMax - dataMin);
        result[i] = min + normalized * (max - min);
    }
    
    return result;
}

std::vector<float> DataUtils::zScoreNormalize(const std::vector<float>& data) {
    if (data.size() < 2) {
        return data;
    }
    
    float mean = MathUtils::mean(data);
    float std = MathUtils::standardDeviation(data);
    
    std::vector<float> result(data.size());
    for (size_t i = 0; i < data.size(); ++i) {
        result[i] = (data[i] - mean) / std;
    }
    
    return result;
}

std::vector<float> DataUtils::concatenate(const std::vector<float>& a, const std::vector<float>& b) {
    std::vector<float> result;
    result.reserve(a.size() + b.size());
    result.insert(result.end(), a.begin(), a.end());
    result.insert(result.end(), b.begin(), b.end());
    return result;
}

std::vector<float> DataUtils::subsample(const std::vector<float>& data, size_t step) {
    std::vector<float> result;
    if (step == 0) {
        return result;
    }
    
    for (size_t i = 0; i < data.size(); i += step) {
        result.push_back(data[i]);
    }
    
    return result;
}

std::vector<float> DataUtils::interpolate(const std::vector<float>& data, float newSamplingRate) {
    std::vector<float> result;
    if (data.empty() || newSamplingRate <= 0.0f) {
        return result;
    }
    
    float originalInterval = 1.0f / data.size();
    size_t newSize = static_cast<size_t>(data.size() * newSamplingRate / originalInterval);
    
    result.reserve(newSize);
    for (size_t i = 0; i < newSize; ++i) {
        float t = static_cast<float>(i) / static_cast<float>(newSize - 1) * data.size();
        size_t i1 = static_cast<size_t>(t);
        size_t i2 = std::min<size_t>(i1 + 1, data.size() - 1);
        
        float weight = t - i1;
        result.push_back(data[i1] * (1.0f - weight) + data[i2] * weight);
    }
    
    return result;
}

std::vector<float> DataUtils::resample(const std::vector<float>& data, size_t newSize) {
    if (data.empty() || newSize == 0) {
        return std::vector<float>();
    }
    
    std::vector<float> result(newSize);
    for (size_t i = 0; i < newSize; ++i) {
        float t = static_cast<float>(i) / static_cast<float>(newSize - 1) * (data.size() - 1);
        size_t i1 = static_cast<size_t>(t);
        size_t i2 = std::min<size_t>(i1 + 1, data.size() - 1);
        
        float weight = t - i1;
        result[i] = data[i1] * (1.0f - weight) + data[i2] * weight;
    }
    
    return result;
}

std::vector<float> DataUtils::bandpassFilter(const std::vector<float>& signal, float lowFreq, float highFreq, float samplingRate) {
    // Simple implementation - in real use, consider using proper DSP libraries
    std::vector<float> result = signal;
    
    // Apply a simple moving average filter
    int windowSize = static_cast<int>(std::round(highFreq / samplingRate * 10));
    if (windowSize < 3) windowSize = 3;
    
    result = MathUtils::smoothSignal(result, windowSize);
    
    return result;
}

std::vector<float> DataUtils::removeDC(const std::vector<float>& signal) {
    float mean = MathUtils::mean(signal);
    std::vector<float> result(signal.size());
    
    for (size_t i = 0; i < signal.size(); ++i) {
        result[i] = signal[i] - mean;
    }
    
    return result;
}

std::vector<float> DataUtils::applyHanningWindow(const std::vector<float>& signal) {
    std::vector<float> result = signal;
    
    if (result.size() > 0) {
        for (size_t i = 0; i < result.size(); ++i) {
            float window = 0.5f * (1.0f - std::cos(2.0f * M_PI * static_cast<float>(i) / static_cast<float>(result.size() - 1)));
            result[i] *= window;
        }
    }
    
    return result;
}

// ConfigUtils implementation
bool ConfigUtils::validateConfig(const std::string& filepath) {
    std::ifstream file(filepath);
    return file.good();
}

bool ConfigUtils::validateConfigKeys(const std::map<std::string, std::string>& config) {
    // Basic validation - check for required keys
    static const std::vector<std::string> requiredKeys = {
        "brain.neuron_count",
        "brain.region_count",
        "brain.connection_probability"
    };
    
    for (const auto& key : requiredKeys) {
        if (config.find(key) == config.end()) {
            return false;
        }
    }
    
    return true;
}

std::vector<std::string> ConfigUtils::getMissingKeys(const std::map<std::string, std::string>& config, const std::vector<std::string>& requiredKeys) {
    std::vector<std::string> missing;
    
    for (const auto& key : requiredKeys) {
        if (config.find(key) == config.end()) {
            missing.push_back(key);
        }
    }
    
    return missing;
}

std::map<std::string, std::string> ConfigUtils::loadConfigFromString(const std::string& configString) {
    std::map<std::string, std::string> config;
    
    // Simple key=value parser
    std::istringstream stream(configString);
    std::string line;
    
    while (std::getline(stream, line)) {
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            config[key] = value;
        }
    }
    
    return config;
}

std::string ConfigUtils::configToString(const std::map<std::string, std::string>& config) {
    std::ostringstream oss;
    
    for (const auto& pair : config) {
        oss << pair.first << "=" << pair.second << "\n";
    }
    
    return oss.str();
}

std::string ConfigUtils::generateConfigHash(const std::map<std::string, std::string>& config) {
    // Simple hash generation
    std::hash<std::string> hasher;
    std::string configStr = configToString(config);
    size_t hash = hasher(configStr);
    
    return std::to_string(hash);
}

bool ConfigUtils::configsAreEqual(const std::map<std::string, std::string>& config1, const std::map<std::string, std::string>& config2) {
    return config1 == config2;
}

float ConfigUtils::configSimilarity(const std::map<std::string, std::string>& config1, const std::map<std::string, std::string>& config2) {
    if (config1.empty() && config2.empty()) {
        return 1.0f;
    }
    
    if (config1.empty() || config2.empty()) {
        return 0.0f;
    }
    
    size_t commonKeys = 0;
    for (const auto& pair : config1) {
        if (config2.find(pair.first) != config2.end()) {
            commonKeys++;
        }
    }
    
    return static_cast<float>(commonKeys) / static_cast<float>(std::max(config1.size(), config2.size()));
}

std::map<std::string, std::string> ConfigUtils::getStandardConfig() {
    return {
        {"brain.neuron_count", "1000"},
        {"brain.region_count", "2"},
        {"brain.connection_probability", "0.1"},
        {"plasticity.stdp_enabled", "true"},
        {"plasticity.hebbian_enabled", "true"},
        {"neuromodulation.reward_scale", "1.0"},
        {"neuromodulation.curiosity_enabled", "true"},
        {"environment.width", "20.0"},
        {"environment.height", "20.0"},
        {"environment.vision_width", "16"},
        {"environment.vision_height", "16"}
    };
}

std::map<std::string, std::string> ConfigUtils::getExplorationConfig() {
    return {
        {"brain.neuron_count", "2000"},
        {"brain.region_count", "3"},
        {"brain.connection_probability", "0.15"},
        {"plasticity.structural_enabled", "true"},
        {"plasticity.hebbian_enabled", "true"},
        {"neuromodulation.curiosity_enabled", "true"},
        {"neuromodulation.novelty_enabled", "true"},
        {"environment.width", "30.0"},
        {"environment.height", "30.0"},
        {"environment.vision_width", "32"},
        {"environment.vision_height", "32"}
    };
}

std::map<std::string, std::string> ConfigUtils::getExploitationConfig() {
    return {
        {"brain.neuron_count", "500"},
        {"brain.region_count", "1"},
        {"brain.connection_probability", "0.05"},
        {"plasticity.hebbian_enabled", "true"},
        {"plasticity.stdp_enabled", "false"},
        {"neuromodulation.reward_scale", "2.0"},
        {"neuromodulation.curiosity_enabled", "false"},
        {"environment.width", "15.0"},
        {"environment.height", "15.0"},
        {"environment.vision_width", "8"},
        {"environment.vision_height", "8"}
    };
}

std::map<std::string, std::string> ConfigUtils::getDevelopmentConfig() {
    return {
        {"brain.development_enabled", "true"},
        {"brain.maturation_rate", "0.01"},
        {"plasticity.age_dependent", "true"},
        {"brain.neuron_count", "800"},
        {"brain.region_count", "2"},
        {"brain.connection_probability", "0.12"},
        {"plasticity.stdp_enabled", "true"},
        {"plasticity.hebbian_enabled", "true"},
        {"environment.width", "25.0"},
        {"environment.height", "25.0"},
        {"environment.vision_width", "24"},
        {"environment.vision_height", "24"}
    };
}

// SimulationUtils implementation
void SimulationUtils::sleep(double seconds) {
    std::this_thread::sleep_for(std::chrono::duration<double>(seconds));
}

void SimulationUtils::busyWait(double seconds) {
    auto start = std::chrono::high_resolution_clock::now();
    while (std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - start).count() < seconds) {
        // Busy wait
    }
}

void SimulationUtils::yieldCPU() {
    std::this_thread::yield();
}

double SimulationUtils::measureExecutionTime(std::function<void()> func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double>(end - start).count();
}

double SimulationUtils::measureMemoryUsage() {
    // This is platform-specific - return 0 for now
    return 0.0;
}

double SimulationUtils::measureCPUUsage() {
    // This is platform-specific - return 0 for now
    return 0.0;
}

void SimulationUtils::waitForSteps(int currentStep, int targetStep, double timestep) {
    while (currentStep < targetStep) {
        // Implementation would depend on simulation context
        currentStep++;
    }
}

void SimulationUtils::stepUntil(double targetTime, std::function<void(double, double)> stepFunction, double timestep) {
    double currentTime = 0.0;
    while (currentTime < targetTime) {
        stepFunction(currentTime, timestep);
        currentTime += timestep;
    }
}

std::map<std::string, double> SimulationUtils::getSystemStats() {
    std::map<std::string, double> stats;
    
    // Basic system statistics
    stats["cpu_usage"] = 0.0;
    stats["memory_usage"] = 0.0;
    stats["disk_usage"] = 0.0;
    
    return stats;
}

std::map<std::string, double> SimulationUtils::getPerformanceStats() {
    std::map<std::string, double> stats;
    
    // Performance statistics
    stats["steps_per_second"] = 0.0;
    stats["simulation_speed"] = 1.0;
    stats["memory_efficiency"] = 0.0;
    
    return stats;
}

void SimulationUtils::printSystemStats() {
    auto stats = getSystemStats();
    
    std::cout << "System Statistics:" << std::endl;
    for (const auto& pair : stats) {
        std::cout << "  " << pair.first << ": " << pair.second << std::endl;
    }
}

void SimulationUtils::logPerformanceStats(const std::string& filepath) {
    auto stats = getPerformanceStats();
    
    std::ofstream file(filepath, std::ios::app);
    if (!file) {
        return;
    }
    
    file << "Performance Statistics at " << MathUtils::getCurrentTime() << ":" << std::endl;
    for (const auto& pair : stats) {
        file << "  " << pair.first << ": " << pair.second << std::endl;
    }
    file << "\n";
}

// ResearchUtils implementation
std::vector<std::vector<float>> ResearchUtils::runParameterSweep(
    std::function<std::vector<float>(const std::vector<float>&)> func,
    const std::vector<std::vector<float>>& parameterSpace,
    const std::vector<std::string>& parameterNames
) {
    std::vector<std::vector<float>> results;
    
    // Simple implementation - generate parameter combinations
    if (parameterSpace.empty()) {
        return results;
    }
    
    // For demonstration, use first parameter set
    results.push_back(func(parameterSpace[0]));
    
    return results;
}

std::map<std::string, float> ResearchUtils::analyzeExperimentResults(
    const std::vector<std::map<std::string, float>>& results,
    const std::vector<std::string>& metrics
) {
    std::map<std::string, float> analysis;
    
    for (const auto& metric : metrics) {
        std::vector<float> values;
        for (const auto& result : results) {
            if (result.find(metric) != result.end()) {
                values.push_back(result.at(metric));
            }
        }
        
        if (!values.empty()) {
            analysis[metric + "_mean"] = MathUtils::mean(values);
            analysis[metric + "_std"] = MathUtils::standardDeviation(values);
            analysis[metric + "_min"] = *std::min_element(values.begin(), values.end());
            analysis[metric + "_max"] = *std::max_element(values.begin(), values.end());
        }
    }
    
    return analysis;
}

bool ResearchUtils::saveExperimentData(
    const std::string& experimentName,
    const std::vector<std::map<std::string, float>>& results,
    const std::vector<std::string>& metrics
) {
    std::string filepath = experimentName + "_results.txt";
    
    std::ofstream file(filepath);
    if (!file) {
        return false;
    }
    
    // Write experiment header
    file << "Experiment: " << experimentName << std::endl;
    file << "Timestamp: " << MathUtils::getCurrentTime() << std::endl;
    file << "Number of trials: " << results.size() << std::endl;
    file << "Metrics: ";
    for (size_t i = 0; i < metrics.size(); ++i) {
        file << metrics[i];
        if (i < metrics.size() - 1) {
            file << ", ";
        }
    }
    file << std::endl;
    file << std::endl;
    
    // Write results
    file << "Results:" << std::endl;
    for (size_t i = 0; i < results.size(); ++i) {
        file << "Trial " << i << ": ";
        for (size_t j = 0; j < metrics.size(); ++j) {
            if (results[i].find(metrics[j]) != results[i].end()) {
                file << metrics[j] << "=" << results[i].at(metrics[j]);
                if (j < metrics.size() - 1) {
                    file << ", ";
                }
            }
        }
        file << std::endl;
    }
    
    return !file.fail();
}

std::map<std::string, float> ResearchUtils::calculateStatistics(const std::vector<float>& data) {
    std::map<std::string, float> stats;
    
    if (data.empty()) {
        return stats;
    }
    
    stats["count"] = static_cast<float>(data.size());
    stats["mean"] = MathUtils::mean(data);
    stats["std"] = MathUtils::standardDeviation(data);
    stats["min"] = *std::min_element(data.begin(), data.end());
    stats["max"] = *std::max_element(data.begin(), data.end());
    stats["median"] = MathUtils::median(data);
    
    return stats;
}

std::map<std::string, float> ResearchUtils::compareGroups(const std::vector<std::vector<float>>& groups) {
    std::map<std::string, float> analysis;
    
    for (size_t i = 0; i < groups.size(); ++i) {
        std::map<std::string, float> groupStats = calculateStatistics(groups[i]);
        
        for (const auto& pair : groupStats) {
            analysis[std::to_string(i) + "_" + pair.first] = pair.second;
        }
    }
    
    return analysis;
}

std::vector<float> ResearchUtils::performANOVA(const std::vector<std::vector<float>>& groups) {
    std::vector<float> results;
    
    if (groups.empty()) {
        return results;
    }
    
    // Simplified ANOVA calculation
    float totalMean = 0.0f;
    float totalSS = 0.0f;
    float betweenSS = 0.0f;
    
    // Calculate overall mean
    size_t totalN = 0;
    for (const auto& group : groups) {
        totalN += group.size();
    }
    
    if (totalN == 0) {
        return results;
    }
    
    for (const auto& group : groups) {
        float groupMean = MathUtils::mean(group);
        totalMean += groupMean * group.size();
        
        for (float x : group) {
            totalSS += (x - groupMean) * (x - groupMean);
        }
    }
    
    totalMean /= totalN;
    
    // Calculate between-group sum of squares
    for (const auto& group : groups) {
        float groupMean = MathUtils::mean(group);
        for (float x : group) {
            betweenSS += (groupMean - totalMean) * (groupMean - totalMean) * group.size();
        }
    }
    
    // Calculate F-statistic
    float withinSS = totalSS - betweenSS;
    float dfBetween = groups.size() - 1;
    float dfWithin = totalN - groups.size();
    
    if (dfWithin <= 0) {
        return results;
    }
    
    float msBetween = betweenSS / dfBetween;
    float msWithin = withinSS / dfWithin;
    float fValue = msBetween / msWithin;
    
    results.push_back(fValue);
    results.push_back(dfBetween);
    results.push_back(dfWithin);
    
    return results;
}

std::string ResearchUtils::createHistogram(const std::vector<float>& data, size_t width) {
    if (data.empty()) {
        return "No data available";
    }
    
    std::string result = "Histogram:\n";
    
    // Create bins
    float min = *std::min_element(data.begin(), data.end());
    float max = *std::max_element(data.begin(), data.end());
    
    if (max == min) {
        result += "All values are identical\n";
        return result;
    }
    
    size_t bins = width;
    float binWidth = (max - min) / bins;
    
    std::vector<size_t> binCounts(bins, 0);
    
    for (float x : data) {
        size_t binIndex = static_cast<size_t>((x - min) / binWidth);
        if (binIndex >= bins) binIndex = bins - 1;
        binCounts[binIndex]++;
    }
    
    // Create histogram bars
    float maxCount = *std::max_element(binCounts.begin(), binCounts.end());
    for (size_t i = 0; i < bins; ++i) {
        result += std::to_string(i) + ": ";
        size_t count = binCounts[i];
        size_t barLength = static_cast<size_t>((count / maxCount) * width);
        for (size_t j = 0; j < barLength; ++j) {
            result += "*";
        }
        result += " (" + std::to_string(count) + ")\n";
    }
    
    return result;
}

std::string ResearchUtils::createScatterPlot(const std::vector<float>& x, const std::vector<float>& y, size_t width, size_t height) {
    if (x.size() != y.size() || x.empty()) {
        return "Invalid data for scatter plot";
    }
    
    std::string result = "Scatter Plot:\n";
    
    // Find min and max for scaling
    float xMin = *std::min_element(x.begin(), x.end());
    float xMax = *std::max_element(x.begin(), x.end());
    float yMin = *std::min_element(y.begin(), y.end());
    float yMax = *std::max_element(y.begin(), y.end());
    
    // Create a simple text-based scatter plot
    for (size_t j = 0; j < height; ++j) {
        float yValue = yMin + (static_cast<float>(j) / static_cast<float>(height - 1)) * (yMax - yMin);
        result += "| ";
        
        for (size_t i = 0; i < width; ++i) {
            float xValue = xMin + (static_cast<float>(i) / static_cast<float>(width - 1)) * (xMax - xMin);
            
            // Check if point exists near this position
            bool found = false;
            for (size_t k = 0; k < x.size(); ++k) {
                if (std::abs(x[k] - xValue) < (xMax - xMin) / static_cast<float>(width * 2) &&
                    std::abs(y[k] - yValue) < (yMax - yMin) / static_cast<float>(height * 2)) {
                    found = true;
                    break;
                }
            }
            
            if (found) {
                result += "*";
            } else {
                result += " ";
            }
        }
        
        result += " |\n";
    }
    
    // Add axes
    result += std::string(width + 4, '-') + "\n";
    result += "x values: " + std::to_string(xMin) + " to " + std::to_string(xMax) + "\n";
    result += "y values: " + std::to_string(yMin) + " to " + std::to_string(yMax) + "\n";
    
    return result;
}

std::string ResearchUtils::createTimeSeries(const std::vector<double>& timestamps, const std::vector<float>& values, size_t width) {
    if (timestamps.empty() || values.empty() || timestamps.size() != values.size()) {
        return "Invalid data for time series";
    }
    
    std::string result = "Time Series:\n";
    
    // Find min and max
    float min = *std::min_element(values.begin(), values.end());
    float max = *std::max_element(values.begin(), values.end());
    double timeMin = *std::min_element(timestamps.begin(), timestamps.end());
    double timeMax = *std::max_element(timestamps.begin(), timestamps.end());
    
    // Create time series plot
    for (size_t j = 0; j < width; ++j) {
        float value = max - (static_cast<float>(j) / static_cast<float>(width - 1)) * (max - min);
        result += "| ";
        
        for (size_t i = 0; i < timestamps.size(); ++i) {
            if (std::abs(values[i] - value) < (max - min) / static_cast<float>(width * 2)) {
                result += "*";
                break;
            }
        }
        
        // Add horizontal grid lines
        if (j == 0 || j == width - 1) {
            result += " ";
        }
        
        result += "\n";
    }
    
    // Add time axis
    result += std::string(width + 4, '-') + "\n";
    result += "Time: " << timeMin << " to " << timeMax << "\n";
    
    return result;
}

// ExportUtils implementation
bool ExportUtils::exportToJSON(const std::string& filepath, const std::map<std::string, float>& data) {
    std::ofstream file(filepath);
    if (!file) {
        return false;
    }
    
    file << "{" << std::endl;
    for (auto it = data.begin(); it != data.end(); ++it) {
        file << "  \"" << it->first << "\": " << it->second;
        if (std::next(it) != data.end()) {
            file << ",";
        }
        file << std::endl;
    }
    file << "}" << std::endl;
    
    return !file.fail();
}

bool ExportUtils::exportToCSV(const std::string& filepath, const std::vector<std::vector<float>>& data, const std::vector<std::string>& headers) {
    std::ofstream file(filepath);
    if (!file) {
        return false;
    }
    
    // Write headers
    for (size_t i = 0; i < headers.size(); ++i) {
        file << headers[i];
        if (i < headers.size() - 1) {
            file << ",";
        }
    }
    file << "\n";
    
    // Write data
    for (size_t i = 0; i < data.size(); ++i) {
        for (size_t j = 0; j < data[i].size(); ++j) {
            file << data[i][j];
            if (j < data[i].size() - 1) {
                file << ",";
            }
        }
        if (i < data.size() - 1) {
            file << "\n";
        }
    }
    
    return !file.fail();
}

bool ExportUtils::exportToTXT(const std::string& filepath, const std::map<std::string, std::string>& data) {
    std::ofstream file(filepath);
    if (!file) {
        return false;
    }
    
    for (const auto& pair : data) {
        file << pair.first << " = " << pair.second << std::endl;
    }
    
    return !file.fail();
}

bool ExportUtils::exportToXML(const std::string& filepath, const std::map<std::string, std::string>& data) {
    std::ofstream file(filepath);
    if (!file) {
        return false;
    }
    
    file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    file << "<data>" << std::endl;
    
    for (const auto& pair : data) {
        file << "  <" << pair.first << ">" << pair.second << "</" << pair.first << ">" << std::endl;
    }
    
    file << "</data>" << std::endl;
    
    return !file.fail();
}

bool ExportUtils::exportConfig(const std::string& filepath, const std::map<std::string, std::string>& config) {
    std::ofstream file(filepath);
    if (!file) {
        return false;
    }
    
    for (const auto& pair : config) {
        file << pair.first << "=" << pair.second << "\n";
    }
    
    return !file.fail();
}

bool ExportUtils::importConfig(const std::string& filepath, std::map<std::string, std::string>& config) {
    std::ifstream file(filepath);
    if (!file) {
        return false;
    }
    
    config.clear();
    std::string line;
    
    while (std::getline(file, line)) {
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            config[key] = value;
        }
    }
    
    return !file.fail();
}

bool ExportUtils::exportResults(const std::string& filepath, const std::vector<std::map<std::string, float>>& results) {
    std::ofstream file(filepath);
    if (!file) {
        return false;
    }
    
    if (results.empty()) {
        return false;
    }
    
    // Write headers
    file << "Trial";
    for (const auto& result : results) {
        if (!result.empty()) {
            file << "," << result.begin()->first;
        }
    }
    file << "\n";
    
    // Write data
    for (size_t i = 0; i < results.size(); ++i) {
        file << i;
        for (const auto& pair : results[i]) {
            file << "," << pair.second;
        }
        if (i < results.size() - 1) {
            file << "\n";
        }
    }
    
    return !file.fail();
}

bool ExportUtils::exportResultsWithMetadata(const std::string& filepath, const std::vector<std::map<std::string, float>>& results, const std::map<std::string, std::string>& metadata) {
    std::ofstream file(filepath);
    if (!file) {
        return false;
    }
    
    // Write metadata
    file << "# Metadata:" << std::endl;
    for (const auto& pair : metadata) {
        file << "# " << pair.first << " = " << pair.second << std::endl;
    }
    file << std::endl;
    
    // Write data
    if (!results.empty()) {
        file << "# Results:" << std::endl;
        for (size_t i = 0; i < results.size(); ++i) {
            file << i;
            for (const auto& pair : results[i]) {
                file << "," << pair.second;
            }
            if (i < results.size() - 1) {
                file << std::endl;
            }
        }
    }
    
    return !file.fail();
}

// ErrorUtils implementation
std::function<void(const std::string&)> ErrorUtils::errorHandler_ = nullptr;
std::string ErrorUtils::lastError_ = "";

void ErrorUtils::setErrorHandler(std::function<void(const std::string&)> handler) {
    errorHandler_ = handler;
}

void ErrorUtils::clearErrorHandler() {
    errorHandler_ = nullptr;
}

void ErrorUtils::throwError(const std::string& message) {
    lastError_ = message;
    if (errorHandler_) {
        errorHandler_(message);
    }
    throw std::runtime_error(message);
}

void ErrorUtils::logError(const std::string& message, LogLevel level) {
    lastError_ = message;
    // In a real implementation, this would log to a logger
    std::cout << "[" << levelToString(level) << "] " << message << std::endl;
}

std::string ErrorUtils::getLastError() {
    return lastError_;
}

void ErrorUtils::clearLastError() {
    lastError_ = "";
}

bool ErrorUtils::hasError() {
    return !lastError_.empty();
}

bool ErrorUtils::validateInput(const std::string& name, const std::string& value, const std::string& type) {
    if (value.empty()) {
        warn("Empty value for parameter: " + name);
        return false;
    }
    
    if (type == "number") {
        try {
            std::stof(value);
            return true;
        } catch (...) {
            warn("Invalid number for parameter: " + name);
            return false;
        }
    }
    
    return true;
}

bool ErrorUtils::validateRange(float value, float min, float max, const std::string& parameterName) {
    if (value < min || value > max) {
        warn("Value out of range for parameter '" + parameterName + "': " + std::to_string(value) +
             ". Valid range: [" + std::to_string(min) + ", " + std::to_string(max) + "]");
        return false;
    }
    return true;
}

bool ErrorUtils::validateFile(const std::string& filepath, bool mustExist) {
    bool exists = std::filesystem::exists(filepath);
    
    if (mustExist && !exists) {
        warn("File does not exist: " + filepath);
        return false;
    }
    
    if (!mustExist && exists && !std::filesystem::is_regular_file(filepath)) {
        warn("Path is not a regular file: " + filepath);
        return false;
    }
    
    return true;
}

void ErrorUtils::warn(const std::string& message) {
    logError(message, LogLevel::Warning);
}

void ErrorUtils::info(const std::string& message) {
    logError(message, LogLevel::Info);
}

void ErrorUtils::debug(const std::string& message) {
    logError(message, LogLevel::Debug);
}

// TimeSeriesUtils implementation
std::vector<std::pair<double, float>> TimeSeriesUtils::convertToTimeSeries(const std::vector<float>& values, double timestep) {
    std::vector<std::pair<double, float>> series;
    
    for (size_t i = 0; i < values.size(); ++i) {
        series.emplace_back(i * timestep, values[i]);
    }
    
    return series;
}

std::vector<float> TimeSeriesUtils::resampleTimeSeries(const std::vector<std::pair<double, float>>& series, double newSamplingInterval) {
    std::vector<float> result;
    if (series.empty() || newSamplingInterval <= 0.0) {
        return result;
    }
    
    double currentTime = series[0].first;
    result.push_back(series[0].second);
    
    for (size_t i = 1; i < series.size(); ++i) {
        double targetTime = currentTime + newSamplingInterval;
        
        while (i < series.size() && series[i].first < targetTime) {
            i++;
        }
        
        if (i < series.size()) {
            result.push_back(series[i].second);
            currentTime = series[i].first;
        } else {
            break;
        }
    }
    
    return result;
}

double TimeSeriesUtils::calculateTemporalCorrelation(const std::vector<std::pair<double, float>>& series1, const std::vector<std::pair<double, float>>& series2) {
    if (series1.empty() || series2.empty()) {
        return 0.0f;
    }
    
    std::vector<float> values1, values2;
    double timeOffset = series2[0].first - series1[0].first;
    
    for (const auto& point : series2) {
        double targetTime = point.first - timeOffset;
        
        // Find closest point in series1
        auto it = std::lower_bound(series1.begin(), series1.end(), targetTime,
                                   [](const std::pair<double, float>& a, double t) { return a.first < t; });n
        if (it != series1.end()) {
            values1.push_back(it->second);
            values2.push_back(point.second);
        }
    }
    
    if (values1.size() < 2) {
        return 0.0f;
    }
    
    return MathUtils::correlation(values1, values2);
}

std::pair<float, float> TimeSeriesUtils::calculateTrend(const std::vector<std::pair<double, float>>& series) {
    if (series.size() < 2) {
        return {0.0f, 0.0f};
    }
    
    return linearRegression(series);
}

float TimeSeriesUtils::calculateRateOfChange(const std::vector<std::pair<double, float>>& series) {
    if (series.size() < 2) {
        return 0.0f;
    }
    
    float startValue = series.front().second;
    float endValue = series.back().second;
    double timeSpan = series.back().first - series.front().first;
    
    if (timeSpan == 0.0) {
        return 0.0f;
    }
    
    return (endValue - startValue) / static_cast<float>(timeSpan);
}

float TimeSeriesUtils::calculateAutocorrelation(const std::vector<float>& signal, size_t lag) {
    if (signal.size() <= lag || lag == 0) {
        return 0.0f;
    }
    
    size_t n = signal.size();
    float mean = MathUtils::mean(signal);
    
    float numerator = 0.0f;
    float denominator = 0.0f;
    
    for (size_t i = 0; i < n - lag; ++i) {
        float x = signal[i] - mean;
        float y = signal[i + lag] - mean;
        numerator += x * y;
        denominator += x * x;
    }
    
    if (denominator == 0.0f) {
        return 0.0f;
    }
    
    return numerator / denominator;
}

std::vector<size_t> TimeSeriesUtils::detectPeaks(const std::vector<float>& signal, float threshold) {
    std::vector<size_t> peaks;
    
    if (signal.size() < 3) {
        return peaks;
    }
    
    for (size_t i = 1; i < signal.size() - 1; ++i) {
        if (signal[i] > signal[i-1] && signal[i] > signal[i+1] && signal[i] > threshold) {
            peaks.push_back(i);
        }
    }
    
    return peaks;
}

std::vector<size_t> TimeSeriesUtils::detectValleys(const std::vector<float>& signal, float threshold) {
    std::vector<size_t> valleys;
    
    if (signal.size() < 3) {
        return valleys;
    }
    
    for (size_t i = 1; i < signal.size() - 1; ++i) {
        if (signal[i] < signal[i-1] && signal[i] < signal[i+1] && signal[i] < threshold) {
            valleys.push_back(i);
        }
    }
    
    return valleys;
}

std::vector<std::pair<size_t, size_t>> TimeSeriesUtils::detectCycles(const std::vector<float>& signal, float minPeriod, float maxPeriod) {
    std::vector<std::pair<size_t, size_t>> cycles;
    
    // Simple cycle detection based on peaks
    std::vector<size_t> peaks = detectPeaks(signal, 0.0f);
    
    for (size_t i = 0; i + 1 < peaks.size(); ++i) {
        size_t period = peaks[i + 1] - peaks[i];
        if (period >= minPeriod && period <= maxPeriod) {
            cycles.emplace_back(peaks[i], period);
        }
    }
    
    return cycles;
}

// VisualizationUtils implementation
std::string VisualizationUtils::createBarChart(const std::vector<float>& data, const std::vector<std::string>& labels, size_t width) {
    std::string result = "Bar Chart:\n";
    
    if (data.empty() || labels.empty() || data.size() != labels.size()) {
        return result + "Invalid data\n";
    }
    
    // Find max value
    float maxValue = *std::max_element(data.begin(), data.end());
    if (maxValue == 0.0f) {
        maxValue = 1.0f;
    }
    
    // Create bars
    for (size_t i = 0; i < data.size(); ++i) {
        result += labels[i] + ": ";
        size_t barLength = static_cast<size_t>((data[i] / maxValue) * width);
        for (size_t j = 0; j < barLength; ++j) {
            result += "*";
        }
        result += " (" + std::to_string(data[i]) + ")\n";
    }
    
    return result;
}

std::string VisualizationUtils::createLineChart(const std::vector<float>& data, size_t width, size_t height) {
    std::string result = "Line Chart:\n";
    
    if (data.empty()) {
        return result + "No data\n";
    }
    
    // Find min and max
    float min = *std::min_element(data.begin(), data.end());
    float max = *std::max_element(data.begin(), data.end());
    
    if (max == min) {
        max = min + 1.0f;
    }
    
    // Create chart
    for (size_t j = 0; j < height; ++j) {
        float value = max - (static_cast<float>(j) / static_cast<float>(height - 1)) * (max - min);
        result += "| ";
        
        for (size_t i = 0; i < data.size(); ++i) {
            if (std::abs(data[i] - value) < (max - min) / static_cast<float>(width * 2)) {
                result += "*";
                break;
            }
        }
        
        // Add horizontal grid lines
        if (j == 0 || j == height - 1) {
            result += " ";
        }
        
        result += "\n";
    }
    
    // Add x-axis
    result += std::string(width + 4, '-') + "\n";
    
    return result;
}

std::string VisualizationUtils::createHeatmap(const std::vector<std::vector<float>>& data, const std::vector<std::string>& xLabels, const std::vector<std::string>& yLabels) {
    std::string result = "Heatmap:\n";
    
    if (data.empty() || xLabels.empty() || yLabels.empty()) {
        return result + "Invalid data\n";
    }
    
    // Find min and max
    float min = 0.0f, max = 1.0f;
    bool first = true;
    
    for (const auto& row : data) {
        for (float value : row) {
            if (first) {
                min = max = value;
                first = false;
            } else {
                if (value < min) min = value;
                if (value > max) max = value;
            }
        }
    }
    
    if (max == min) {
        max = min + 1.0f;
    }
    
    // Create heatmap
    for (size_t j = 0; j < yLabels.size(); ++j) {
        result += yLabels[j] + ": ";
        
        for (size_t i = 0; i < xLabels.size(); ++i) {
            if (i < data.size() && j < data[i].size()) {
                float value = data[i][j];
                size_t intensity = static_cast<size_t>((value - min) / (max - min) * 10);
                
                switch (intensity) {
                    case 0: result += "  "; break;
                    case 1: result += "░"; break;
                    case 2: result += "▒"; break;
                    case 3: result += "▓"; break;
                    case 4: result += "█"; break;
                    default: result += "█"; break;
                }
            } else {
                result += "  ";
            }
            
            result += " ";
        }
        
        result += "\n";
    }
    
    // Add color legend
    result += "Legend: ";
    result += "  █: High, ";
    result += "█: Medium, ";
    result += "  : Low\n";
    
    return result;
}

std::string VisualizationUtils::createAsciiPlot(const std::vector<float>& data, char marker, char axis) {
    std::string result = "ASCII Plot:\n";
    
    if (data.empty()) {
        return result + "No data\n";
    }
    
    // Find min and max
    float min = *std::min_element(data.begin(), data.end());
    float max = *std::max_element(data.begin(), data.end());
    
    if (max == min) {
        max = min + 1.0f;
    }
    
    // Create plot
    for (size_t i = 0; i < data.size(); ++i) {
        result += axis;
        
        float normalized = (data[i] - min) / (max - min);
        size_t plotLength = static_cast<size_t>(normalized * 50);
        
        for (size_t j = 0; j < plotLength; ++j) {
            result += marker;
        }
        
        result += " (" + std::to_string(data[i]) + ")\n";
    }
    
    return result;
}

std::string VisualizationUtils::createComparisonChart(const std::vector<std::vector<float>>& datasets, const std::vector<std::string>& labels) {
    std::string result = "Comparison Chart:\n";
    
    if (datasets.empty() || labels.empty() || datasets.size() != labels.size()) {
        return result + "Invalid data\n";
    }
    
    // Find max value
    float maxValue = 0.0f;
    for (const auto& dataset : datasets) {
        if (!dataset.empty()) {
            float datasetMax = *std::max_element(dataset.begin(), dataset.end());
            maxValue = std::max(maxValue, datasetMax);
        }
    }
    
    if (maxValue == 0.0f) {
        maxValue = 1.0f;
    }
    
    // Create comparison chart
    for (size_t i = 0; i < datasets.size(); ++i) {
        result += labels[i] + ": ";
        
        const auto& dataset = datasets[i];
        if (!dataset.empty()) {
            float datasetMax = *std::max_element(dataset.begin(), dataset.end());
            size_t barLength = static_cast<size_t>((datasetMax / maxValue) * 50);
            
            for (size_t j = 0; j < barLength; ++j) {
                result += "*";
            }
            
            result += " (" + std::to_string(datasetMax) + ")\n";
        } else {
            result += "(no data)\n";
        }
    }
    
    return result;
}

std::string VisualizationUtils::createProgressBar(float progress, size_t width) {
    std::string result = "Progress: ";
    
    size_t filled = static_cast<size_t>(progress * width);
    
    result += "[";
    for (size_t i = 0; i < filled; ++i) {
        result += "=";
    }
    for (size_t i = filled; i < width; ++i) {
        result += " ";
    }
    result += "] ";
    
    result += std::to_string(static_cast<int>(progress * 100)) + "%\n";
    
    return result;
}

std::string VisualizationUtils::createBoxPlot(const std::vector<float>& data) {
    std::string result = "Box Plot:\n";
    
    if (data.size() < 5) {
        return result + "Not enough data for box plot\n";
    }
    
    float min = *std::min_element(data.begin(), data.end());
    float max = *std::max_element(data.begin(), data.end());
    float median = MathUtils::median(data);
    float q1 = MathUtils::getPercentile(data, 25);
    float q3 = MathUtils::getPercentile(data, 75);
    
    // Create box plot
    result += "Min: " << min << std::endl;
    result += "Q1: " << q1 << std::endl;
    result += "Median: " << median << std::endl;
    result += "Q3: " << q3 << std::endl;
    result += "Max: " << max << std::endl;
    
    float iqr = q3 - q1;
    result << "IQR: " << iqr << std::endl;
    
    // Outliers
    float lowerBound = q1 - 1.5f * iqr;
    float upperBound = q3 + 1.5f * iqr;
    
    std::vector<float> outliers;
    for (float x : data) {
        if (x < lowerBound || x > upperBound) {
            outliers.push_back(x);
        }
    }
    
    if (!outliers.empty()) {
        result << "Outliers: ";
        for (size_t i = 0; i < outliers.size(); ++i) {
            result << outliers[i];
            if (i < outliers.size() - 1) {
                result << ", ";
            }
        }
        result << std::endl;
    }
    
    return result;
}

std::string VisualizationUtils::createHistogram(const std::vector<float>& data, size_t bins) {
    std::string result = "Histogram:\n";
    
    if (data.empty()) {
        return result + "No data\n";
    }
    
    float min = *std::min_element(data.begin(), data.end());
    float max = *std::max_element(data.begin(), data.end());
    
    if (max == min) {
        result += "All values are identical\n";
        return result;
    }
    
    float binWidth = (max - min) / bins;
    std::vector<size_t> binCounts(bins, 0);
    
    for (float x : data) {
        size_t binIndex = static_cast<size_t>((x - min) / binWidth);
        if (binIndex >= bins) binIndex = bins - 1;
        binCounts[binIndex]++;
    }
    
    // Create histogram
    float maxCount = *std::max_element(binCounts.begin(), binCounts.end());
    for (size_t i = 0; i < bins; ++i) {
        result += std::to_string(i) + ": ";
        size_t count = binCounts[i];
        size_t barLength = static_cast<size_t>((count / maxCount) * 50);
        for (size_t j = 0; j < barLength; ++j) {
            result += "*";
        }
        result += " (" + std::to_string(count) + ")\n";
    }
    
    return result;
}

std::string VisualizationUtils::createQ_QPlot(const std::vector<float>& data, const std::vector<float>& theoreticalData) {
    std::string result = "Q-Q Plot:\n";
    
    if (data.empty() || theoreticalData.empty() || data.size() != theoreticalData.size()) {
        return result + "Invalid data\n";
    }
    
    // Sort both datasets
    std::vector<float> sortedData = data;
    std::vector<float> sortedTheoretical = theoreticalData;
    std::sort(sortedData.begin(), sortedData.end());
    std::sort(sortedTheoretical.begin(), sortedTheoretical.end());
    
    // Create Q-Q plot
    result += "Data points (x=data, y=theoretical):" << std::endl;
    for (size_t i = 0; i < data.size(); ++i) {
        result << "(" << sortedData[i] << ", " << sortedTheoretical[i] << ")\n";
    }
    
    // Add diagonal line
    float minX = *std::min_element(sortedData.begin(), sortedData.end());
    float maxX = *std::max_element(sortedData.begin(), sortedData.end());
    
    result += "Diagonal line: y = x" << std::endl;
    
    return result;
}

// PluginUtils implementation
bool PluginUtils::loadPlugin(const std::string& pluginPath) {
    // In a real implementation, this would dynamically load shared libraries
    // For now, return false to indicate not implemented
    return false;
}

bool PluginUtils::unloadPlugin(const std::string& pluginName) {
    // In a real implementation, this would unload shared libraries
    // For now, return false to indicate not implemented
    return false;
}

bool PluginUtils::isPluginLoaded(const std::string& pluginName) {
    // In a real implementation, check if plugin is loaded
    // For now, return false
    return false;
}

std::vector<std::string> PluginUtils::getLoadedPlugins() {
    return std::vector<std::string>();
}

std::vector<std::string> PluginUtils::discoverPlugins(const std::string& directory) {
    return std::vector<std::string>();
}

std::map<std::string, std::string> PluginUtils::getPluginInfo(const std::string& pluginName) {
    return std::map<std::string, std::string>();
}

void PluginUtils::registerHook(const std::string& hookName, std::function<void()> callback) {
    // In a real implementation, register hooks
    // For now, do nothing
}

void PluginUtils::unregisterHook(const std::string& hookName) {
    // In a real implementation, unregister hooks
    // For now, do nothing
}

void PluginUtils::executeHooks(const std::string& hookName) {
    // In a real implementation, execute registered hooks
    // For now, do nothing
}

bool PluginUtils::configurePlugin(const std::string& pluginName, const std::map<std::string, std::string>& config) {
    // In a real implementation, configure plugin
    // For now, return false
    return false;
}

std::map<std::string, std::string> PluginUtils::getPluginConfig(const std::string& pluginName) {
    return std::map<std::string, std::string>();
}

// MemoryUtils implementation
void* MemoryUtils::allocate(size_t size) {
    return std::malloc(size);
}

void MemoryUtils::deallocate(void* ptr) {
    std::free(ptr);
}

void* MemoryUtils::allocateAligned(size_t size, size_t alignment) {
    // Platform-specific aligned allocation
    // For now, use regular allocation
    return std::malloc(size);
}

void MemoryUtils::deallocateAligned(void* ptr) {
    std::free(ptr);
}

size_t MemoryUtils::getAvailableMemory() {
    // Platform-specific memory query
    // For now, return a large value
    return static_cast<size_t>(-1);
}

size_t MemoryUtils::getUsedMemory() {
    // Platform-specific memory query
    // For now, return 0
    return 0;
}

size_t MemoryUtils::getTotalMemory() {
    // Platform-specific memory query
    // For now, return a large value
    return static_cast<size_t>(-1);
}

float MemoryUtils::getMemoryUsagePercentage() {
    size_t used = getUsedMemory();
    size_t total = getTotalMemory();
    
    if (total == 0 || total == static_cast<size_t>(-1)) {
        return 0.0f;
    }
    
    return (used / static_cast<float>(total)) * 100.0f;
}

void MemoryUtils::startMemoryProfile() {
    // Start memory profiling
}

void MemoryUtils::stopMemoryProfile() {
    // Stop memory profiling
}

std::map<std::string, size_t> MemoryUtils::getMemoryProfile() {
    return std::map<std::string, size_t>();
}

void MemoryUtils::optimizeMemoryUsage() {
    // Memory optimization
}

void MemoryUtils::garbageCollect() {
    // Garbage collection
}

void MemoryUtils::compactMemory() {
    // Memory compaction
}

// ProfilingUtils implementation
void ProfilingUtils::startProfile(const std::string& functionName) {
    // Start function profiling
}

void ProfilingUtils::stopProfile(const std::string& functionName) {
    // Stop function profiling
}

double ProfilingUtils::getProfileTime(const std::string& functionName) {
    return 0.0;
}

std::map<std::string, double> ProfilingUtils::getAllProfileTimes() {
    return std::map<std::string, double>();
}

void ProfilingUtils::startSystemProfile() {
    // Start system profiling
}

void ProfilingUtils::stopSystemProfile() {
    // Stop system profiling
}

std::map<std::string, double> ProfilingUtils::getSystemProfile() {
    return std::map<std::string, double>();
}

std::vector<std::string> ProfilingUtils::profilePerformance() {
    return std::vector<std::string>();
}

std::map<std::string, double> ProfilingUtils::optimizePerformance() {
    return std::map<std::string, double>();
}

void ProfilingUtils::printPerformanceReport() {
    // Print performance report
}

} // namespace nlm
EOF