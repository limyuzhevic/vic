#include "ParameterOptimizer.hpp"

#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <future>
#include <queue>
#include <algorithm>
#include <cmath>
#include <random>
#include <filesystem>

namespace nlm {

struct ParameterOptimizer::Impl {
    Algorithm algorithm;
    size_t populationSize;
    size_t maxGenerations;
    size_t currentGeneration;
    double convergenceThreshold;
    double mutationRate;
    double crossoverRate;
    bool earlyStoppingEnabled;
    double earlyStoppingThreshold;
    double progress;
    std::string status;
    
    std::vector<Parameter> parameters;
    ObjectiveFunction objectiveFunc;
    
    std::vector<OptimizationResult> optimizationResults;
    
    OptimizationProgressCallback progressCallback;
    
    bool visualizationEnabled;
    VisualizationInterface* visualizationInterface;
    
    bool parallelOptimizationEnabled;
    bool hyperbandOptimizationEnabled;
    bool asyncEvaluationEnabled;
    
    std::mutex mutex;
    std::random_device rd;
    std::mt19937 rng;
    
    Impl() : algorithm(Algorithm::BayesianOptimization), populationSize(50), 
             maxGenerations(100), currentGeneration(0), convergenceThreshold(0.001),
             mutationRate(0.1), crossoverRate(0.8), earlyStoppingEnabled(false),
             earlyStoppingThreshold(0.01), progress(0.0), parallelOptimizationEnabled(false),
             hyperbandOptimizationEnabled(false), asyncEvaluationEnabled(false),
             rng(rd()) {
    }
    
    ~Impl() {
        // Clean up resources
    }
    
    void updateProgress(double p, const std::string& statusMsg, size_t generation) {
        progress = p;
        status = statusMsg;
        currentGeneration = generation;
        
        if (progressCallback) {
            progressCallback(progress, status, currentGeneration);
        }
    }
    
    double randomDouble(double min, double max) {
        std::uniform_real_distribution<double> dist(min, max);
        return dist(rng);
    }
    
    double evaluateObjective(const std::vector<Parameter>& paramValues) {
        if (!objectiveFunc) {
            return 0.0; // Placeholder
        }
        
        // Create temporary parameters vector
        std::vector<Parameter> tempParams;
        for (size_t i = 0; i < paramValues.size() && i < parameters.size(); ++i) {
            Parameter p = parameters[i];
            p.currentValue = paramValues[i].currentValue;
            tempParams.push_back(p);
        }
        
        // Evaluate objective function
        return objectiveFunc(tempParams);
    }
    
    void mutateParameter(Parameter& param) {
        if (param.isDiscrete) {
            // Integer mutation
            int current = static_cast<int>(param.currentValue);
            int step = static_cast<int>(param.stepSize);
            int range = static_cast<int>(param.maxValue) - static_cast<int>(param.minValue);
            
            int newValue = current + (rand() % (2 * step + 1) - step);
            newValue = std::max(static_cast<int>(param.minValue), 
                             std::min(newValue, static_cast<int>(param.maxValue)));
            
            param.currentValue = static_cast<double>(newValue);
        } else {
            // Float mutation with Gaussian distribution
            std::normal_distribution<double> dist(param.currentValue, param.stepSize * 0.1);
            double newValue = dist(rng);
            
            newValue = std::max(param.minValue, std::min(newValue, param.maxValue));
            param.currentValue = newValue;
        }
    }
    
    std::vector<Parameter> crossover(const std::vector<Parameter>& parent1, 
                                    const std::vector<Parameter>& parent2) {
        std::vector<Parameter> child;
        
        for (size_t i = 0; i < parent1.size() && i < parent2.size(); ++i) {
            Parameter param;
            
            // Random selection from parents
            if (rand() % 100 < 50) { // 50% chance from parent1
                param = parent1[i];
            } else {
                param = parent2[i];
            }
            
            child.push_back(param);
        }
        
        return child;
    }
    
    double calculateConvergence(const std::vector<double>& scores) {
        if (scores.size() < 2) return 1.0;
        
        // Calculate relative improvement between last two scores
        double lastScore = scores.back();
        double secondLastScore = scores[scores.size() - 2];
        
        if (std::abs(secondLastScore) < 1e-10) return 1.0;
        
        double improvement = std::abs(lastScore - secondLastScore) / std::abs(secondLastScore);
        return 1.0 - std::min(improvement, 1.0); // Higher = more converged
    }
};

ParameterOptimizer::ParameterOptimizer() : pImpl(std::make_unique<Impl>()) {}

ParameterOptimizer::~ParameterOptimizer() = default;

bool ParameterOptimizer::initialize(Algorithm algorithm, size_t populationSize,
                                   size_t maxGenerations, double convergenceThreshold) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->algorithm = algorithm;
    pImpl->populationSize = populationSize;
    pImpl->maxGenerations = maxGenerations;
    pImpl->convergenceThreshold = convergenceThreshold;
    pImpl->optimizationResults.clear();
    pImpl->currentGeneration = 0;
    pImpl->progress = 0.0;
    
    return true;
}

void ParameterOptimizer::addParameter(const Parameter& param) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->parameters.push_back(param);
}

void ParameterOptimizer::setParameterValue(const std::string& name, double value) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (auto& param : pImpl->parameters) {
        if (param.name == name) {
            param.currentValue = std::max(param.minValue, std::min(value, param.maxValue));
            break;
        }
    }
}

double ParameterOptimizer::getParameterValue(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (const auto& param : pImpl->parameters) {
        if (param.name == name) {
            return param.currentValue;
        }
    }
    
    return 0.0; // Return default if not found
}

const std::vector<ParameterOptimizer::Parameter>& ParameterOptimizer::getParameters() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->parameters;
}

void ParameterOptimizer::clearParameters() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->parameters.clear();
}

void ParameterOptimizer::setObjectiveFunction(ObjectiveFunction func) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->objectiveFunc = func;
}

std::vector<ParameterOptimizer::OptimizationResult> ParameterOptimizer::runOptimization(
    std::shared_ptr<Brain> brainTemplate,
    std::shared_ptr<Environment> environmentTemplate,
    SimulationStep maxSteps,
    size_t numRuns) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->optimizationResults.clear();
    pImpl->currentGeneration = 0;
    pImpl->progress = 0.0;
    
    // For now, use random search as placeholder
    // Real implementation would use actual optimization algorithms
    
    pImpl->updateProgress(0.0, "Starting parameter optimization...", 0);
    
    // Run optimization
    for (size_t run = 0; run < numRuns; ++run) {
        pImpl->updateProgress(
            static_cast<double>(run) / numRuns,
            "Running optimization iteration " + std::to_string(run + 1) + "/" + std::to_string(numRuns),
            run
        );
        
        OptimizationResult result = optimizeSingleRun(brainTemplate, environmentTemplate, maxSteps, run);
        pImpl->optimizationResults.push_back(result);
        
        // Small delay between runs
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    return pImpl->optimizationResults;
}

ParameterOptimizer::OptimizationResult ParameterOptimizer::optimizeSingleRun(
    std::shared_ptr<Brain> brainTemplate,
    std::shared_ptr<Environment> environmentTemplate,
    SimulationStep maxSteps,
    uint64_t seed) {
    OptimizationResult result;
    result.experimentId = pImpl->optimizationResults.size();
    result.experimentName = "Optimization_Run_" + std::to_string(result.experimentId);
    result.executionTime = 0.0; // Placeholder
    
    // Set random seed for reproducibility
    if (seed > 0) {
        // In real implementation, would set seed for brain/environment
    }
    
    // Create brain and environment
    auto brain = std::make_shared<Brain>(brainTemplate->getConfig());
    brain->initialize();
    
    // Run experiment (placeholder)
    // In real implementation, would run the brain simulation
    
    // Generate random score
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    result.score = dist(pImpl->rng);
    
    // Set parameters
    result.parameters = pImpl->parameters;
    
    // Add metric values (placeholder)
    result.metricValues["fitness"] = result.score;
    result.metricValues["execution_time"] = result.executionTime;
    
    return result;
}

ParameterOptimizer::OptimizationResult ParameterOptimizer::getBestResult() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->optimizationResults.empty()) {
        return OptimizationResult{}; // Return empty result
    }
    
    // Find best result (highest score)
    auto bestIt = std::max_element(pImpl->optimizationResults.begin(), pImpl->optimizationResults.end(),
                                  [](const OptimizationResult& a, const OptimizationResult& b) {
                                      return a.score < b.score;
                                  });
    
    return *bestIt;
}

std::vector<ParameterOptimizer::OptimizationResult> ParameterOptimizer::getTopResults(size_t count) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<OptimizationResult> topResults = pImpl->optimizationResults;
    
    // Sort by score (descending)
    std::sort(topResults.begin(), topResults.end(),
              [](const OptimizationResult& a, const OptimizationResult& b) {
                  return a.score > b.score;
              });
    
    // Return top N results
    if (count > topResults.size()) {
        count = topResults.size();
    }
    
    return std::vector<OptimizationResult>(topResults.begin(), topResults.begin() + count);
}

bool ParameterOptimizer::exportOptimizationResults(const std::string& filename) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "=== Parameter Optimization Results ===\n\n";
    file << "Algorithm: " << (pImpl->algorithm == Algorithm::RandomSearch ? "Random Search" :
                             pImpl->algorithm == Algorithm::GridSearch ? "Grid Search" :
                             pImpl->algorithm == Algorithm::BayesianOptimization ? "Bayesian Optimization" :
                             pImpl->algorithm == Algorithm::GeneticAlgorithm ? "Genetic Algorithm" :
                             pImpl->algorithm == Algorithm::SimulatedAnnealing ? "Simulated Annealing" :
                             pImpl->algorithm == Algorithm::ParticleSwarm ? "Particle Swarm" : "Unknown") << "\n";
    file << "Population Size: " << pImpl->populationSize << "\n";
    file << "Max Generations: " << pImpl->maxGenerations << "\n";
    file << "Current Generation: " << pImpl->currentGeneration << "\n";
    file << "Optimization Progress: " << (pImpl->progress * 100) << "%\n\n";
    
    file << "=== Parameters ===\n";
    for (const auto& param : pImpl->parameters) {
        file << param.name << "\n";
        file << "  Min: " << param.minValue << "\n";
        file << "  Max: " << param.maxValue << "\n";
        file << "  Current: " << param.currentValue << "\n";
        file << "  Type: " << (param.isDiscrete ? "Discrete" : "Continuous") << "\n\n";
    }
    
    file << "=== Optimization Results ===\n";
    file << "Total Runs: " << pImpl->optimizationResults.size() << "\n";
    
    for (size_t i = 0; i < pImpl->optimizationResults.size(); ++i) {
        const auto& result = pImpl->optimizationResults[i];
        file << "Run " << i << ": " << result.experimentName << "\n";
        file << "  Score: " << result.score << "\n";
        file << "  Execution Time: " << result.executionTime << "s\n";
        file << "  Metrics: " << result.metricValues.size() << " values\n\n";
    }
    
    file.close();
    return true;
}

bool ParameterOptimizer::generateConvergenceReport(const std::string& filename) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "=== Convergence Report ===\n\n";
    
    // Calculate convergence metrics
    std::vector<double> scores;
    for (const auto& result : pImpl->optimizationResults) {
        scores.push_back(result.score);
    }
    
    if (scores.empty()) {
        file << "No optimization results available\n";
        file.close();
        return true;
    }
    
    file << "Final Score: " << scores.back() << "\n";
    file << "Average Score: " << std::accumulate(scores.begin(), scores.end(), 0.0) / scores.size() << "\n";
    file << "Best Score: " << *std::max_element(scores.begin(), scores.end()) << "\n";
    
    file.close();
    return true;
}

bool ParameterOptimizer::generateParameterSensitivityReport(const std::string& filename) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "=== Parameter Sensitivity Analysis ===\n\n";
    
    // Analyze parameter sensitivity
    std::vector<ParameterAnalysis> analyses = analyzeParameterSensitivity(pImpl->optimizationResults);
    
    file << "Parameter Importance Ranking:\n";
    for (size_t i = 0; i < analyses.size(); ++i) {
        const auto& analysis = analyses[i];
        file << i + 1 << ". " << analysis.parameterName << " (Importance: " 
             << (calculateParameterImportance(analysis) * 100) << ")\n";
    }
    
    file.close();
    return true;
}

void ParameterOptimizer::setPopulationSize(size_t size) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->populationSize = size;
}

void ParameterOptimizer::setMaxGenerations(size_t generations) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->maxGenerations = generations;
}

void ParameterOptimizer::setConvergenceThreshold(double threshold) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->convergenceThreshold = threshold;
}

void ParameterOptimizer::setMutationRate(double rate) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->mutationRate = std::max(0.0, std::min(rate, 1.0));
}

void ParameterOptimizer::setCrossoverRate(double rate) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->crossoverRate = std::max(0.0, std::min(rate, 1.0));
}

void ParameterOptimizer::enableEarlyStopping(bool enable, double improvementThreshold) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->earlyStoppingEnabled = enable;
    pImpl->earlyStoppingThreshold = improvementThreshold;
}

double ParameterOptimizer::getOptimizationProgress() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->progress;
}

std::string ParameterOptimizer::getCurrentOptimizationStatus() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->status;
}

size_t ParameterOptimizer::getCurrentGeneration() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->currentGeneration;
}

size_t ParameterOptimizer::getTotalGenerations() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->maxGenerations;
}

void ParameterOptimizer::setProgressCallback(OptimizationProgressCallback callback) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->progressCallback = callback;
}

void ParameterOptimizer::enableVisualization(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->visualizationEnabled = enable;
}

void ParameterOptimizer::setVisualizationInterface(VisualizationInterface* viz) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->visualizationInterface = viz;
}

std::vector<ParameterOptimizer::ParameterAnalysis> ParameterOptimizer::analyzeParameterSensitivity(
    const std::vector<OptimizationResult>& results) const {
    std::vector<ParameterAnalysis> analyses;
    
    if (results.empty()) {
        return analyses;
    }
    
    // Get parameter names from first result
    if (!results.empty() && !results[0].parameters.empty()) {
        for (const auto& param : results[0].parameters) {
            ParameterAnalysis analysis;
            analysis.parameterName = param.name;
            analyses.push_back(analysis);
        }
    }
    
    // Analyze each parameter
    for (auto& analysis : analyses) {
        std::vector<double> values;
        std::vector<double> scores;
        
        for (const auto& result : results) {
            // Find parameter in result
            for (const auto& param : result.parameters) {
                if (param.name == analysis.parameterName) {
                    values.push_back(param.currentValue);
                    scores.push_back(result.score);
                    break;
                }
            }
        }
        
        analysis.values = values;
        analysis.scores = scores;
        
        // Calculate statistics
        if (!values.empty()) {
            analysis.mean = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
            
            double sumSq = 0.0;
            for (double val : values) {
                sumSq += (val - analysis.mean) * (val - analysis.mean);
            }
            analysis.stddev = std::sqrt(sumSq / values.size());
            
            analysis.min = *std::min_element(values.begin(), values.end());
            analysis.max = *std::max_element(values.begin(), values.end());
            
            // Calculate correlation with score
            if (values.size() == scores.size() && values.size() > 1) {
                double cov = 0.0;
                double meanVal = analysis.mean;
                double meanScore = std::accumulate(scores.begin(), scores.end(), 0.0) / scores.size();
                
                for (size_t i = 0; i < values.size(); ++i) {
                    cov += (values[i] - meanVal) * (scores[i] - meanScore);
                }
                cov /= values.size();
                
                double stddevScore = 0.0;
                for (double score : scores) {
                    stddevScore += (score - meanScore) * (score - meanScore);
                }
                stddevScore = std::sqrt(stddevScore / scores.size());
                
                if (stddevScore > 0.0) {
                    analysis.correlationWithScore = cov / (analysis.stddev * stddevScore);
                } else {
                    analysis.correlationWithScore = 0.0;
                }
            }
        }
    }
    
    return analyses;
}

double ParameterOptimizer::calculateParameterImportance(const ParameterAnalysis& analysis) const {
    if (analysis.stddev < 1e-10) return 0.0;
    
    // Use absolute correlation as importance measure
    return std::abs(analysis.correlationWithScore);
}

void ParameterOptimizer::enableParallelOptimization(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->parallelOptimizationEnabled = enable;
}

void ParameterOptimizer::enableHyperbandOptimization(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->hyperbandOptimizationEnabled = enable;
}

void ParameterOptimizer::enableAsyncEvaluation(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->asyncEvaluationEnabled = enable;
}

bool ParameterOptimizer::validateResults(const std::vector<OptimizationResult>& results) const {
    if (results.empty()) {
        return false;
    }
    
    // Check for NaN or infinite values
    for (const auto& result : results) {
        if (std::isnan(result.score) || std::isinf(result.score)) {
            return false;
        }
        
        for (const auto& metric : result.metricValues) {
            if (std::isnan(metric.second) || std::isinf(metric.second)) {
                return false;
            }
        }
    }
    
    return true;
}

} // namespace nlm
