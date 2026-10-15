#pragma once

#include "ParameterSweep.hpp"
#include <algorithm>
#include <random>
#include <chrono>
#include <cmath>

namespace nlm {

ParameterSweep::ParameterSweep() {
    rng.seed(std::chrono::steady_clock::now().time_since_epoch().count());
}

ParameterSweep::ParameterSweep(const std::vector<std::pair<std::string, std::vector<double>>>& parameters) {
    rng.seed(std::chrono::steady_clock::now().time_since_epoch().count());
    addParameters(parameters);
}

ParameterSweep::~ParameterSweep() = default;

bool ParameterSweep::addParameter(const std::string& name, const std::vector<double>& values) {
    if (values.empty()) {
        return false;
    }
    parameters.push_back({name, values});
    return true;
}

bool ParameterSweep::addParameter(const std::string& name, double min, double max, 
                                  size_t numPoints) {
    std::vector<double> values;
    if (numPoints == 1) {
        values.push_back(min);
    } else {
        double step = (max - min) / (numPoints - 1);
        for (size_t i = 0; i < numPoints; ++i) {
            values.push_back(min + i * step);
        }
    }
    
    return addParameter(name, values);
}

bool ParameterSweep::addParameter(const std::string& name, double min, double max) {
    return addParameter(name, min, max, 5); // Default to 5 points
}

const std::vector<std::pair<std::string, std::vector<double>>>& ParameterSweep::getParameters() const {
    return parameters;
}

std::vector<std::map<std::string, std::any>> ParameterSweep::generateGrid(const GridConfig& config) const {
    std::vector<std::map<std::string, std::any>> combinations;
    
    // For grid search, generate all combinations
    std::vector<std::string> paramNames;
    std::vector<std::vector<double>> paramValues;
    
    for (const auto& param : parameters) {
        paramNames.push_back(param.first);
        paramValues.push_back(param.second);
    }
    
    if (paramNames.empty()) {
        return combinations;
    }
    
    // Generate all combinations using recursive approach
    generateCombinations(paramNames, 0, std::map<std::string, std::any>(), combinations, paramValues);
    
    // Limit number of combinations if specified
    if (config.maxCombinations > 0 && combinations.size() > config.maxCombinations) {
        combinations.resize(config.maxCombinations);
    }
    
    return combinations;
}

std::vector<std::map<std::string, std::any>> ParameterSweep::generateRandom(const RandomConfig& config) const {
    std::vector<std::map<std::string, std::any>> combinations;
    
    // Generate random combinations
    for (size_t i = 0; i < config.numSamples; ++i) {
        std::map<std::string, std::any> combo;
        
        for (const auto& param : parameters) {
            std::vector<double> values = param.second;
            if (values.empty()) continue;
            
            // Randomly select a value
            std::uniform_int_distribution<size_t> dist(0, values.size() - 1);
            size_t index = dist(rng);
            combo[param.first] = values[index];
        }
        
        combinations.push_back(combo);
    }
    
    return combinations;
}

std::vector<std::map<std::string, std::any>> ParameterSweep::generateLatinHypercube(const LatinHypercubeConfig& config) const {
    std::vector<std::map<std::string, std::any>> combinations;
    
    // For Latin Hypercube Sampling
    // For now, fall back to random generation
    RandomConfig randomConfig;
    randomConfig.numSamples = config.numSamples;
    combinations = generateRandom(randomConfig);
    
    return combinations;
}

std::vector<std::map<std::string, std::any>> ParameterSweep::generateResponseSurface(const ResponseSurfaceConfig& config) const {
    std::vector<std::map<std::string, std::any>> combinations;
    
    // For response surface methodology, generate around optimal points
    // For now, return a simple subset
    GridConfig gridConfig;
    gridConfig.maxCombinations = config.numPoints;
    combinations = generateGrid(gridConfig);
    
    return combinations;
}

std::vector<std::map<std::string, std::any>> ParameterSweep::optimize(
    const std::function<double(const std::map<std::string, std::any>&)>& objective,
    const OptimizationConfig& config) {
    std::vector<std::map<std::string, std::any>> bestCombinations;
    
    // For optimization, we need to search parameter space
    // Generate initial population/sample points
    std::vector<std::map<std::string, std::any>> samples;
    
    switch (config.samplingMethod) {
        case SamplingMethod::GRID:
        {
            GridConfig gridConfig;
            gridConfig.maxCombinations = config.maxSamples;
            samples = generateGrid(gridConfig);
            break;
        }
        case SamplingMethod::RANDOM:
        {
            RandomConfig randomConfig;
            randomConfig.numSamples = config.maxSamples;
            samples = generateRandom(randomConfig);
            break;
        }
        case SamplingMethod::LATIN_HYPERCUBE:
        {
            LatinHypercubeConfig lhsConfig;
            lhsConfig.numSamples = config.maxSamples;
            samples = generateLatinHypercube(lhsConfig);
            break;
        }
        default:
            RandomConfig randomConfig;
            randomConfig.numSamples = config.maxSamples;
            samples = generateRandom(randomConfig);
    }
    
    // Evaluate samples
    double bestValue = std::numeric_limits<double>::max();
    std::map<std::string, std::any> bestSample;
    
    for (const auto& sample : samples) {
        double value = objective(sample);
        
        if (config.maximize) {
            if (value > bestValue) {
                bestValue = value;
                bestSample = sample;
            }
        } else {
            if (value < bestValue) {
                bestValue = value;
                bestSample = sample;
            }
        }
    }
    
    bestCombinations.push_back(bestSample);
    
    return bestCombinations;
}

std::string ParameterSweep::generateSweepReport(
    const std::vector<std::map<std::string, std::any>>& combinations) const {
    std::stringstream ss;
    ss << "Parameter Sweep Report:\n";
    ss << "======================\n\n";
    
    ss << "Parameter ranges:\n";
    for (const auto& param : parameters) {
        ss << "  " << param.first << ": [";
        for (size_t i = 0; i < param.second.size(); ++i) {
            ss << param.second[i];
            if (i < param.second.size() - 1) ss << ", ";
        }
        ss << "]\n";
    }
    
    ss << "\nGenerated combinations: " << combinations.size() << "\n";
    
    if (!combinations.empty()) {
        ss << "\nFirst few combinations:\n";
        for (size_t i = 0; i < std::min(size_t(5), combinations.size()); ++i) {
            ss << "  Combination " << i + 1 << ": ";
            for (const auto& pair : combinations[i]) {
                ss << pair.first << "=" << std::any_cast<double>(pair.second) << "; ";
            }
            ss << "\n";
        }
    }
    
    return ss.str();
}

std::string ParameterSweep::getSweepSummary() const {
    std::stringstream ss;
    ss << "Parameter Sweep: " << parameters.size() << " parameters\n";
    
    for (const auto& param : parameters) {
        ss << "  " << param.first << ": " << param.second.size() << " values\n";
    }
    
    return ss.str();
}

void ParameterSweep::generateCombinations(
    const std::vector<std::string>& paramNames,
    size_t index,
    std::map<std::string, std::any>& current,
    std::vector<std::map<std::string, std::any>>& combinations,
    const std::vector<std::vector<double>>& paramValues) {
    
    if (index >= paramNames.size()) {
        combinations.push_back(current);
        return;
    }
    
    std::string paramName = paramNames[index];
    
    for (double value : paramValues[index]) {
        current[paramName] = value;
        generateCombinations(paramNames, index + 1, current, combinations, paramValues);
    }
    
    current.erase(paramName);
}

} // namespace nlm
