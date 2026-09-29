#pragma once

#include <algorithm>
#include <cmath>
#include <fstream>
#include <limits>
#include <map>
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../environment/Environment.hpp"

namespace nlm {

// Environment parameter
struct EnvironmentParameter {
    std::string name;
    double value;
    double minValue;
    double maxValue;
    double defaultValue;
    std::string description;
    bool isDynamic;
    double evolutionRate;
    
    EnvironmentParameter(const std::string& paramName, double val,
                        double min = 0.0, double max = 1.0,
                        double def = 0.0, const std::string& desc = "",
                        bool dynamic = false, double evolution = 0.01)
        : name(paramName), value(val), minValue(min), maxValue(max),
          defaultValue(def), description(desc), isDynamic(dynamic),
          evolutionRate(evolution) {}
};

// Environmental factor
class EnvironmentalFactor {
private:
    std::string factorId;
    std::vector<double> influenceWeights;
    std::vector<double> sensitivityParameters;
    double baseValue;
    double volatility;
    
public:
    EnvironmentalFactor(const std::string& id, size_t dimension,
                       double base = 1.0, double vol = 0.1)
        : factorId(id), baseValue(base), volatility(vol) {
        influenceWeights.resize(dimension, 1.0 / dimension);
        sensitivityParameters.resize(dimension, 1.0);
    }
    
    double calculateInfluence(const std::vector<double>& brainState) {
        double influence = baseValue;
        
        for (size_t i = 0; i < std::min(brainState.size(), influenceWeights.size()); ++i) {
            influence += brainState[i] * influenceWeights[i] * sensitivityParameters[i];
        }
        
        // Add stochastic volatility
        std::normal_distribution<double> noise(0.0, volatility);
        influence += noise(rng);
        
        // Clamp to reasonable bounds
        influence = std::max(0.0, std::min(influence, 10.0));
        
        return influence;
    }
    
    void updateParameters(const std::vector<double>& newParameters) {
        for (size_t i = 0; i < std::min(newParameters.size(), sensitivityParameters.size()); ++i) {
            sensitivityParameters[i] = newParameters[i];
        }
    }
    
    std::string getFactorId() const { return factorId; }
    double getBaseValue() const { return baseValue; }
    
private:
    std::mt19937 rng{std::random_device{}()};
};

// Dynamic environment modifier
class DynamicEnvironmentModifier {
private:
    std::string modifierId;
    std::vector<EnvironmentParameter> parameters;
    std::vector<EnvironmentalFactor> factors;
    double evolutionSpeed;
    double adaptationThreshold;
    std::mt19937 rng;
    
public:
    DynamicEnvironmentModifier(const std::string& id, double speed = 0.01,
                              double threshold = 0.1)
        : modifierId(id), evolutionSpeed(speed), adaptationThreshold(threshold),
          rng(std::random_device{}()) {}
    
    void addParameter(const std::string& name, double value,
                      double min = 0.0, double max = 1.0,
                      double defaultVal = 0.0, const std::string& desc = "",
                      bool dynamic = false, double evolution = 0.01) {
        parameters.emplace_back(name, value, min, max, defaultVal, desc, dynamic, evolution);
    }
    
    void addFactor(const std::string& factorId, size_t dimension,
                   double base = 1.0, double volatility = 0.1) {
        factors.emplace_back(factorId, dimension, base, volatility);
    }
    
    void evolveEnvironment(const Brain& brain, const Config& config) {
        // Update dynamic parameters
        for (auto& param : parameters) {
            if (param.isDynamic) {
                double change = (std::uniform_real_distribution<double>(-1.0, 1.0)(rng) *
                               param.evolutionRate) * param.value;
                
                param.value += change;
                
                // Clamp to bounds
                param.value = std::max(param.minValue, std::min(param.value, param.maxValue));
                
                // Update factors based on parameter changes
                if (param.value > adaptationThreshold) {
                    for (auto& factor : factors) {
                        factor.updateParameters({param.value});
                    }
                }
            }
        }
        
        // Update factors based on brain state
        for (auto& factor : factors) {
            std::vector<double> brainState;
            // In real implementation, would extract brain state
            brainState = {brain.getGlobalExcitation(), brain.getGlobalInhibition(), 0.5};
            
            double influence = factor.calculateInfluence(brainState);
            // In real implementation, would apply influence to environment
        }
    }
    
    double getParameterValue(const std::string& name) const {
        for (const auto& param : parameters) {
            if (param.name == name) {
                return param.value;
            }
        }
        return 0.0; // Default if not found
    }
    
    std::vector<double> getAllParameterValues() const {
        std::vector<double> values;
        for (const auto& param : parameters) {
            values.push_back(param.value);
        }
        return values;
    }
    
    std::string getModifierId() const { return modifierId; }
    
    std::string getEnvironmentSummary() const {
        std::stringstream ss;
        ss << "Environment Modifier: " << modifierId << "\n";
        ss << "  Evolution Speed: " << evolutionSpeed << "\n";
        ss << "  Adaptation Threshold: " << adaptationThreshold << "\n";
        ss << "  Parameters: " << parameters.size() << "\n";
        ss << "  Factors: " << factors.size() << "\n";
        
        for (const auto& param : parameters) {
            ss << "    " << param.name << ": " << param.value;
            if (param.isDynamic) ss << " (dynamic)";
            ss << "\n";
        }
        
        return ss.str();
    }
    
    void saveEnvironment(const Config& config) const {
        std::string filename = config.getOr("environment.state_file", "environment_state.json");
        // Implementation for saving environment
    }
    
    void loadEnvironment(const Config& config) {
        std::string filename = config.getOr("environment.state_file", "environment_state.json");
        // Implementation for loading environment
    }
};

// Dynamic environment simulation
class DynamicEnvironmentSimulation {
private:
    std::string simId;
    std::unique_ptr<DynamicEnvironmentModifier> modifier;
    std::vector<double> historicalData;
    std::vector<double> currentParameters;
    double simulationTime;
    double timeStep;
    double stabilityThreshold;
    
public:
    DynamicEnvironmentSimulation(const std::string& id, const Config& config)
        : simId(id), simulationTime(0.0), timeStep(config.getOr("environment.time_step", 0.1)),
          stabilityThreshold(config.getOr("environment.stability_threshold", 0.5)) {
        
        // Create environment modifier
        modifier = std::make_unique<DynamicEnvironmentModifier>(
            "main_modifier",
            config.getOr("environment.evolution_speed", 0.01),
            config.getOr("environment.adaptation_threshold", 0.1)
        );
        
        // Initialize parameters based on config
        if (config.has("environment.initial_parameters")) {
            auto initParams = config.getOr<std::vector<double>>("environment.initial_parameters", {});
            currentParameters = initParams;
            
            // Add parameters to modifier
            std::vector<std::string> paramNames = {"temperature", "humidity", "pressure", "light", "gravity"};
            for (size_t i = 0; i < std::min(initParams.size(), paramNames.size()); ++i) {
                modifier->addParameter(paramNames[i], initParams[i],
                                     0.0, 10.0, initParams[i], "Environmental parameter", true);
            }
        } else {
            // Create default parameters
            modifier->addParameter("temperature", 25.0, 0.0, 100.0, 25.0, "Temperature (°C)", true);
            modifier->addParameter("humidity", 0.5, 0.0, 1.0, 0.5, "Humidity level", true);
            modifier->addParameter("pressure", 101.3, 0.0, 200.0, 101.3, "Pressure (kPa)", true);
            modifier->addParameter("light", 500.0, 0.0, 1000.0, 500.0, "Light intensity (lux)", true);
            modifier->addParameter("gravity", 9.8, 0.0, 20.0, 9.8, "Gravity (m/s²)", true);
        }
        
        // Add environmental factors
        modifier->addFactor("thermal", 3, 25.0, 0.5);
        modifier->addFactor("magnetic", 2, 0.0, 0.1);
        modifier->addFactor("electromagnetic", 4, 1.0, 0.2);
    }
    
    void runStep(const Brain& brain) {
        // Evolve environment
        modifier->evolveEnvironment(brain, Config{});
        
        // Update historical data
        historicalData.push_back(simulationTime);
        for (double param : modifier->getAllParameterValues()) {
            historicalData.push_back(param);
        }
        
        // Keep only recent history
        if (historicalData.size() > 10000) {
            historicalData.erase(historicalData.begin(), historicalData.begin() + 100);
        }
        
        simulationTime += timeStep;
    }
    
    double getParameter(const std::string& name) const {
        return modifier->getParameterValue(name);
    }
    
    std::vector<double> getCurrentParameters() const {
        return modifier->getAllParameterValues();
    }
    
    double calculateStability() const {
        if (historicalData.size() < 100) return 1.0;
        
        // Calculate coefficient of variation for parameters
        double sum = 0.0, sumSq = 0.0;
        size_t count = 0;
        
        for (size_t i = 1; i < historicalData.size(); i += 5) {  // Skip time entries
            if (i + 1 < historicalData.size()) {
                double val = historicalData[i];
                sum += val;
                sumSq += val * val;
                count++;
            }
        }
        
        if (count == 0) return 1.0;
        
        double mean = sum / count;
        double variance = (sumSq / count) - (mean * mean);
        double stddev = std::sqrt(std::max(0.0, variance));
        
        double coefficientOfVariation = (mean > 0) ? (stddev / mean) : 0.0;
        
        return std::max(0.0, 1.0 - coefficientOfVariation);
    }
    
    std::string getSimulationStatus() const {
        std::stringstream ss;
        ss << "Dynamic Environment Simulation: " << simId << "\n";
        ss << "  Simulation Time: " << simulationTime << "\n";
        ss << "  Time Step: " << timeStep << "\n";
        ss << "  Stability: " << calculateStability() << "\n";
        ss << "  Historical Data Points: " << historicalData.size() << "\n";
        
        return ss.str();
    }
    
    void saveState(const Config& config) const {
        modifier->saveEnvironment(config);
    }
    
    void loadState(const Config& config) {
        modifier->loadEnvironment(config);
        currentParameters = modifier->getAllParameterValues();
    }
};

// Advanced dynamic environment manager
class AdvancedDynamicEnvironment {
private:
    std::string envManagerId;
    std::vector<std::unique_ptr<DynamicEnvironmentSimulation>> simulations;
    std::vector<double> globalEvolutionRates;
    double overallStability;
    std::mt19937 rng;
    
public:
    AdvancedDynamicEnvironment(const std::string& id, const Config& config) : envManagerId(id), overallStability(1.0), rng(std::random_device{}()) {
        // Create multiple simulations for different environment types
        size_t numSimulations = config.getOr("environment.num_simulations", 3);
        
        for (size_t i = 0; i < numSimulations; ++i) {
            std::string simId = "simulation_" + std::to_string(i);
            simulations.push_back(std::make_unique<DynamicEnvironmentSimulation>(simId, config));
        }
        
        // Set up global evolution rates
        globalEvolutionRates.resize(simulations.size());
        for (size_t i = 0; i < globalEvolutionRates.size(); ++i) {
            globalEvolutionRates[i] = config.getOr("environment.evolution_rate_" + std::to_string(i), 0.01);
        }
    }
    
    void updateAllEnvironments(const Brain& brain) {
        overallStability = 1.0;
        
        for (size_t i = 0; i < simulations.size(); ++i) {
            // Update simulation
            simulations[i]->runStep(brain);
            
            // Update overall stability
            double simStability = simulations[i]->calculateStability();
            overallStability = overallStability * 0.9 + simStability * 0.1;
        }
    }
    
    double getOverallStability() const {
        return overallStability;
    }
    
    std::vector<double> getEnvironmentParameters(size_t simulationIndex) const {
        if (simulationIndex < simulations.size()) {
            return simulations[simulationIndex]->getCurrentParameters();
        }
        return std::vector<double>();
    }
    
    std::string getEnvironmentManagerStatus() const {
        std::stringstream ss;
        ss << "Advanced Dynamic Environment Manager: " << envManagerId << "\n";
        ss << "  Number of Simulations: " << simulations.size() << "\n";
        ss << "  Overall Stability: " << overallStability << "\n";
        ss << "  Global Evolution Rates: ";
        for (double rate : globalEvolutionRates) {
            ss << rate << " ";
        }
        ss << "\n";
        
        for (size_t i = 0; i < simulations.size(); ++i) {
            ss << "  Simulation " << i << ": " << simulations[i]->getSimulationStatus() << "\n";
        }
        
        return ss.str();
    }
    
    void saveAllStates(const Config& config) const {
        for (size_t i = 0; i < simulations.size(); ++i) {
            simulations[i]->saveState(config);
        }
    }
    
    void loadAllStates(const Config& config) {
        for (size_t i = 0; i < simulations.size(); ++i) {
            simulations[i]->loadState(config);
        }
    }
};

} // namespace nlm
