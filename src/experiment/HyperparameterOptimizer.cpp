#include "AdvancedExperiment.hpp"
#include "../core/Logger/Logger.hpp"
#include <random>
#include <algorithm>
#include <cmath>
#include <unordered_set>

namespace nlm {

struct RandomSearchOptimizer::Impl {
    std::map<std::string, std::vector<double>> searchSpaces;
    std::map<std::string, std::function<double(const Config&)>> objectiveFunctions;
    Config optimalParameters;
    double optimalObjectiveValue;
    size_t iterationCount;
    std::mt19937 rng;
    
    Impl() : optimalObjectiveValue(std::numeric_limits<double>::max()), iterationCount(0) {
        std::random_device rd;
        rng.seed(rd());
    }
};

RandomSearchOptimizer::Impl::Impl() : optimalObjectiveValue(std::numeric_limits<double>::max()), iterationCount(0) {
    std::random_device rd;
    rng.seed(rd());
}

bool RandomSearchOptimizer::initialize(const Config& config) {
    pImpl->logger->info("Random search optimizer initialized");
    return true;
}

void RandomSearchOptimizer::update(double timestep) {
    // Update optimizer state
}

void RandomSearchOptimizer::shutdown() {
    pImpl->logger->info("Random search optimizer shutdown");
}

bool RandomSearchOptimizer::addSearchSpace(const std::string& name, const std::vector<double>& values) {
    if (name.empty()) {
        pImpl->logger->error("Search space name cannot be empty");
        return false;
    }
    
    pImpl->searchSpaces[name] = values;
    return true;
}

bool RandomSearchOptimizer::removeSearchSpace(const std::string& name) {
    auto it = pImpl->searchSpaces.find(name);
    if (it != pImpl->searchSpaces.end()) {
        pImpl->searchSpaces.erase(it);
        return true;
    }
    return false;
}

bool RandomSearchOptimizer::addObjectiveFunction(const std::string& name,
                                                std::function<double(const Config&)> objective) {
    if (name.empty() || !objective) {
        pImpl->logger->error("Invalid objective function name or function");
        return false;
    }
    
    pImpl->objectiveFunctions[name] = objective;
    return true;
}

bool RandomSearchOptimizer::removeObjectiveFunction(const std::string& name) {
    auto it = pImpl->objectiveFunctions.find(name);
    if (it != pImpl->objectiveFunctions.end()) {
        pImpl->objectiveFunctions.erase(it);
        return true;
    }
    return false;
}

Config RandomSearchOptimizer::optimize(const std::string& objectiveName) {
    if (pImpl->objectiveFunctions.find(objectiveName) == pImpl->objectiveFunctions.end()) {
        pImpl->logger->error("Objective function not found: " + objectiveName);
        return Config();
    }
    
    auto& objectiveFunction = pImpl->objectiveFunctions[objectiveName];
    
    // Generate random configuration
    Config config;
    for (const auto& pair : pImpl->searchSpaces) {
        const auto& values = pair.second;
        if (!values.empty()) {
            std::uniform_int_distribution<size_t> dist(0, values.size() - 1);
            size_t index = dist(pImpl->rng);
            config.set(pair.first, values[index]);
        }
    }
    
    // Evaluate objective
    double objectiveValue = objectiveFunction(config);
    pImpl->iterationCount++;
    
    // Update optimal if better
    if (objectiveValue < pImpl->optimalObjectiveValue) {
        pImpl->optimalObjectiveValue = objectiveValue;
        pImpl->optimalParameters = config;
    }
    
    pImpl->logger->info("Random search iteration completed. Objective value: " + std::to_string(objectiveValue));
    
    return config;
}

std::map<std::string, double> RandomSearchOptimizer::optimize(const std::string& objectiveName,
                                                               const std::vector<std::string>& parameterNames) {
    std::map<std::string, double> result;
    
    if (pImpl->objectiveFunctions.find(objectiveName) == pImpl->objectiveFunctions.end()) {
        pImpl->logger->error("Objective function not found: " + objectiveName);
        return result;
    }
    
    auto& objectiveFunction = pImpl->objectiveFunctions[objectiveName];
    
    Config config;
    for (const auto& paramName : parameterNames) {
        auto it = pImpl->searchSpaces.find(paramName);
        if (it != pImpl->searchSpaces.end() && !it->second.empty()) {
            std::uniform_int_distribution<size_t> dist(0, it->second.size() - 1);
            size_t index = dist(pImpl->rng);
            config.set(paramName, it->second[index]);
        }
    }
    
    // Run multiple iterations and return best parameters
    for (size_t i = 0; i < 10; ++i) { // Run 10 iterations
        Config testConfig;
        for (const auto& paramName : parameterNames) {
            auto it = pImpl->searchSpaces.find(paramName);
            if (it != pImpl->searchSpaces.end() && !it->second.empty()) {
                std::uniform_int_distribution<size_t> dist(0, it->second.size() - 1);
                size_t index = dist(pImpl->rng);
                testConfig.set(paramName, it->second[index]);
            }
        }
        
        double objectiveValue = objectiveFunction(testConfig);
        
        if (objectiveValue < pImpl->optimalObjectiveValue) {
            pImpl->optimalObjectiveValue = objectiveValue;
            pImpl->optimalParameters = testConfig;
        }
    }
    
    for (const auto& pair : pImpl->optimalParameters.getAllValues()) {
        result[pair.first] = std::visit([](auto&& arg) { return static_cast<double>(arg); }, pair.second);
    }
    
    return result;
}

bool RandomSearchOptimizer::setAlgorithm(const std::string& algorithm) {
    // Random search is a single algorithm
    return algorithm == "random_search";
}

std::string RandomSearchOptimizer::getAlgorithm() const {
    return "random_search";
}

bool RandomSearchOptimizer::setOptimizationParameters(const std::map<std::string, double>& parameters) {
    return true;
}

std::map<std::string, double> RandomSearchOptimizer::getOptimizationParameters() const {
    return std::map<std::string, double>();
}

Config RandomSearchOptimizer::getOptimalParameters() const {
    return pImpl->optimalParameters;
}

double RandomSearchOptimizer::getOptimalObjectiveValue() const {
    return pImpl->optimalObjectiveValue;
}

size_t RandomSearchOptimizer::getIterationCount() const {
    return pImpl->iterationCount;
}

double RandomSearchOptimizer::getConvergenceRate() const {
    // In random search, convergence is not well-defined
    return 0.0;
}

std::string RandomSearchOptimizer::getOptimizationStats() const {
    std::stringstream ss;
    ss << "Random Search Optimizer Statistics:\n";
    ss << "=============================\n";
    ss << "Algorithm: " << getAlgorithm() << "\n";
    ss << "Iterations: " << pImpl->iterationCount << "\n";
    ss << "Best objective value: " << pImpl->optimalObjectiveValue << "\n";
    ss << "Parameters evaluated: " << pImpl->iterationCount << "\n";
    
    return ss.str();
}

bool RandomSearchOptimizer::runGradientDescent(const std::string& objectiveName) {
    pImpl->logger->warning("Random search optimizer cannot run gradient descent");
    return false;
}

bool RandomSearchOptimizer::runRandomSearch(const std::string& objectiveName) {
    // Run multiple random searches
    if (pImpl->objectiveFunctions.find(objectiveName) == pImpl->objectiveFunctions.end()) {
        return false;
    }
    
    auto& objectiveFunction = pImpl->objectiveFunctions[objectiveName];
    
    for (size_t i = 0; i < 50; ++i) { // Run 50 iterations
        Config config;
        for (const auto& pair : pImpl->searchSpaces) {
            const auto& values = pair.second;
            if (!values.empty()) {
                std::uniform_int_distribution<size_t> dist(0, values.size() - 1);
                size_t index = dist(pImpl->rng);
                config.set(pair.first, values[index]);
            }
        }
        
        double objectiveValue = objectiveFunction(config);
        
        if (objectiveValue < pImpl->optimalObjectiveValue) {
            pImpl->optimalObjectiveValue = objectiveValue;
            pImpl->optimalParameters = config;
        }
        
        pImpl->iterationCount++;
    }
    
    return true;
}

bool RandomSearchOptimizer::runBayesianOptimization(const std::string& objectiveName) {
    pImpl->logger->warning("Random search optimizer cannot run Bayesian optimization");
    return false;
}

bool RandomSearchOptimizer::runGeneticAlgorithm(const std::string& objectiveName) {
    pImpl->logger->warning("Random search optimizer cannot run genetic algorithm");
    return false;
}

bool RandomSearchOptimizer::runSimulatedAnnealing(const std::string& objectiveName) {
    pImpl->logger->warning("Random search optimizer cannot run simulated annealing");
    return false;
}

} // namespace nlm
