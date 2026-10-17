// NLM Parameter Sweeper
// Implementation of parameter sweeps for hyperparameter optimization

#include "ParameterSweeper.hpp"
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <iomanip>
#include <sstream>

namespace nlm {

struct ParameterSweeper::Impl {
    std::mt19937 randomGenerator;
    bool shuttingDown = false;
    
    Impl() {
        // Seed with current time for reproducible results
        randomGenerator.seed(std::chrono::system_clock::now().time_since_epoch().count());
    }
};

ParameterSweeper::ParameterSweeper() : pImpl(std::make_unique<Impl>()) {}
ParameterSweeper::~ParameterSweeper() = default;

std::vector<std::string> ParameterSweeper::generateValues(const ParameterSweepConfig& config) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    std::vector<std::string> values;
    
    if (!validateConfig(config)) {
        return values;
    }
    
    if (config.type == "linear") {
        values = generateLinearSweep(config);
    } else if (config.type == "geometric") {
        values = generateGeometricSweep(config);
    } else if (config.type == "random") {
        values = generateRandomSweep(config);
    } else if (config.type == "grid") {
        values = generateGridSweep(config);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    stats.generationTime = std::chrono::duration<double>(endTime - startTime).count();
    stats.totalValuesGenerated += values.size();
    stats.uniqueValues = values.size();
    
    // Update frequency statistics
    for (const auto& value : values) {
        stats.valueFrequency[value]++;
    }
    
    return values;
}

std::vector<std::map<std::string, std::string>> ParameterSweeper::generateCombinations(
    const std::vector<std::pair<std::string, ParameterSweepConfig>>& sweepConfigs) {
    
    std::vector<std::map<std::string, std::string>> combinations;
    
    if (sweepConfigs.empty()) {
        return combinations;
    }
    
    // Generate values for each parameter
    std::vector<std::vector<std::string>> paramValues;
    std::vector<std::string> paramNames;
    
    for (const auto& pair : sweepConfigs) {
        paramNames.push_back(pair.first);
        auto values = generateValues(pair.second);
        paramValues.push_back(values);
    }
    
    // Generate all combinations (Cartesian product)
    if (paramValues.empty()) return combinations;
    
    // Start with first parameter
    combinations.reserve(paramValues[0].size());
    for (const auto& value : paramValues[0]) {
        std::map<std::string, std::string> combo;
        combo[paramNames[0]] = value;
        combinations.push_back(combo);
    }
    
    // Add remaining parameters
    for (size_t i = 1; i < paramValues.size(); ++i) {
        std::vector<std::map<std::string, std::string>> newCombinations;
        newCombinations.reserve(combinations.size() * paramValues[i].size());
        
        for (const auto& combo : combinations) {
            for (const auto& value : paramValues[i]) {
                std::map<std::string, std::string> newCombo = combo;
                newCombo[paramNames[i]] = value;
                newCombinations.push_back(newCombo);
            }
        }
        
        combinations = std::move(newCombinations);
    }
    
    return combinations;
}

std::vector<std::string> ParameterSweeper::generateLinearSweep(const ParameterSweepConfig& config) {
    std::vector<std::string> values;
    
    try {
        double start = stringToNumber<double>(config.start);
        double end = stringToNumber<double>(config.end);
        
        std::vector<double> numericValues = generateLinearValues(
            start, end, config.steps, config.includeStart, config.includeEnd);
        
        for (double value : numericValues) {
            values.push_back(numberToString(value));
        }
        
    } catch (const std::exception& e) {
        // Log error in real implementation
    }
    
    return values;
}

std::vector<std::string> ParameterSweeper::generateGeometricSweep(const ParameterSweepConfig& config) {
    std::vector<std::string> values;
    
    try {
        double start = stringToNumber<double>(config.start);
        double end = stringToNumber<double>(config.end);
        
        if (start == 0.0 || end == 0.0) {
            return values;  // Avoid division by zero
        }
        
        std::vector<double> numericValues = generateGeometricValues(
            start, end, config.steps, config.includeStart, config.includeEnd);
        
        for (double value : numericValues) {
            values.push_back(numberToString(value));
        }
        
    } catch (const std::exception& e) {
        // Log error in real implementation
    }
    
    return values;
}

std::vector<std::string> ParameterSweeper::generateRandomSweep(const ParameterSweepConfig& config) {
    std::vector<std::string> values;
    
    try {
        size_t count = config.steps > 0 ? config.steps : 10;
        
        // Parse start and end for random range if provided
        double start = 0.0, end = 1.0;
        if (!config.start.empty()) {
            start = parseParameterValue(config.start);
        }
        if (!config.end.empty()) {
            end = parseParameterValue(config.end);
        }
        
        std::vector<double> numericValues = generateRandomValues(count, config.seed);
        
        for (double value : numericValues) {
            values.push_back(numberToString(value));
        }
        
    } catch (const std::exception& e) {
        // Log error in real implementation
    }
    
    return values;
}

std::vector<std::string> ParameterSweeper::generateGridSweep(const ParameterSweepConfig& config) {
    std::vector<std::string> values;
    
    try {
        if (config.predefinedValues.empty()) {
            // Generate grid points between start and end
            double start = config.start.empty() ? 0.0 : parseParameterValue(config.start);
            double end = config.end.empty() ? 1.0 : parseParameterValue(config.end);
            
            std::vector<double> numericValues = generateGridValues(
                config.steps > 0 ? config.steps : 10, start, end);
            
            for (double value : numericValues) {
                values.push_back(numberToString(value));
            }
        } else {
            // Use predefined values
            values = config.predefinedValues;
        }
        
    } catch (const std::exception& e) {
        // Log error in real implementation
    }
    
    return values;
}

template<typename T>
T ParameterSweeper::stringToNumber(const std::string& str) const {
    std::istringstream iss(str);
    T result;
    iss >> result;
    return result;
}

template<typename T>
std::string ParameterSweeper::numberToString(T value) const {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

std::vector<double> ParameterSweeper::generateLinearValues(double start, double end, size_t steps,
                                                           bool includeStart, bool includeEnd) {
    std::vector<double> values;
    
    if (steps == 0) return values;
    
    if (steps == 1) {
        values.push_back(start);
        return values;
    }
    
    double stepSize = (end - start) / (steps - 1);
    
    if (!includeStart) {
        start += stepSize;
        steps--;
    }
    
    if (!includeEnd) {
        end -= stepSize;
        steps--;
    }
    
    for (size_t i = 0; i < steps; ++i) {
        double value = start + i * stepSize;
        values.push_back(value);
    }
    
    return values;
}

std::vector<double> ParameterSweeper::generateGeometricValues(double start, double end, size_t steps,
                                                               bool includeStart, bool includeEnd) {
    std::vector<double> values;
    
    if (start == 0.0 || end == 0.0 || steps == 0) return values;
    
    double ratio = std::pow(end / start, 1.0 / (steps - 1));
    
    if (!includeStart) {
        start *= ratio;
        steps--;
    }
    
    if (!includeEnd) {
        end /= ratio;
        steps--;
    }
    
    for (size_t i = 0; i < steps; ++i) {
        double value = start * std::pow(ratio, i);
        values.push_back(value);
    }
    
    return values;
}

std::vector<double> ParameterSweeper::generateRandomValues(size_t count, const std::string& seed) {
    std::vector<double> values;
    
    if (count == 0) return values;
    
    std::mt19937 gen;
    if (!seed.empty()) {
        // Seed from string
        std::hash<std::string> hasher;
        gen.seed(hasher(seed));
    } else {
        gen.seed(std::chrono::system_clock::now().time_since_epoch().count());
    }
    
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    
    for (size_t i = 0; i < count; ++i) {
        values.push_back(dist(gen));
    }
    
    return values;
}

std::vector<double> ParameterSweeper::generateGridValues(size_t count, double start, double end) {
    std::vector<double> values;
    
    if (count == 0 || start == end) return values;
    
    double stepSize = (end - start) / (count - 1);
    
    for (size_t i = 0; i < count; ++i) {
        double value = start + i * stepSize;
        values.push_back(value);
    }
    
    return values;
}

bool ParameterSweeper::validateConfig(const ParameterSweepConfig& config) {
    if (config.type.empty()) {
        return false;
    }
    
    if (config.type == "linear" || config.type == "geometric" || config.type == "grid") {
        if (config.start.empty() || config.end.empty()) {
            return false;
        }
        
        try {
            parseParameterValue(config.start);
            parseParameterValue(config.end);
        } catch (...) {
            return false;
        }
    }
    
    return true;
}

double ParameterSweeper::parseParameterValue(const std::string& valueStr) const {
    try {
        return std::stod(valueStr);
    } catch (...) {
        return 0.0;
    }
}

std::string ParameterSweeper::formatParameterValue(double value) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(6) << value;
    return oss.str();
}

} // namespace nlm