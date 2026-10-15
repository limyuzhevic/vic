#include "experiment/ExperimentManager.hpp"
#include "experiment/BatchProcessor.hpp"
#include "experiment/ParameterSweep.hpp"
#include "experiment/ExperimentTemplate.hpp"
#include "experiment/ExperimentResult.hpp"
#include "experiment/ResultAnalyzer.hpp"

namespace nlm {

// Global experiment management instance
static std::unique_ptr<ExperimentManager> gExperimentManager;
static std::unique_ptr<BatchProcessor> gBatchProcessor;

// Initialize global experiment system
void initializeExperimentSystem() {
    gExperimentManager = std::make_unique<ExperimentManager>();
    gBatchProcessor = std::make_unique<BatchProcessor>();
    NLM_LOG_INFO("NLM Experiment System initialized");
}

// Shutdown experiment system
void shutdownExperimentSystem() {
    gBatchProcessor.reset();
    gExperimentManager.reset();
    NLM_LOG_INFO("NLM Experiment System shutdown");
}

// Experiment Manager factory functions
std::shared_ptr<ExperimentManager> createExperimentManager() {
    if (!gExperimentManager) {
        initializeExperimentSystem();
    }
    return gExperimentManager;
}

// Batch Processor factory functions
std::shared_ptr<BatchProcessor> createBatchProcessor() {
    if (!gBatchProcessor) {
        initializeExperimentSystem();
    }
    return gBatchProcessor;
}

// Parameter Sweep factory functions
std::shared_ptr<ParameterSweep> createParameterSweep(
    const std::vector<std::pair<std::string, std::vector<double>>>& parameters) {
    return std::make_shared<ParameterSweep>(parameters);
}

// Experiment Template factory functions
std::shared_ptr<ExperimentTemplate> createExperimentTemplate(const std::string& name) {
    return std::make_shared<ExperimentTemplate>(name);
}

// Experiment Result factory functions
std::shared_ptr<ExperimentResult> createExperimentResult(const std::string& experimentName) {
    return std::make_shared<ExperimentResult>(experimentName);
}

// Result Analyzer factory functions
std::shared_ptr<ResultAnalyzer> createResultAnalyzer() {
    return std::make_shared<ResultAnalyzer>();
}

// Experiment system utilities
std::string getExperimentSystemStatus() {
    std::stringstream ss;
    ss << "Experiment System Status:\n";
    ss << "========================\n\n";
    
    if (gExperimentManager) {
        ss << "Experiment Manager:\n";
        ss << "  - Total experiments: " << gExperimentManager->getExperimentCount() << "\n";
        ss << "  - Templates: " << gExperimentManager->getTemplates().size() << "\n";
        ss << "  - Active experiments: " << gExperimentManager->getExperimentCount() << "\n";
    }
    
    if (gBatchProcessor) {
        ss << "Batch Processor:\n";
        auto activeBatches = gBatchProcessor->getActiveBatches();
        ss << "  - Active batches: " << activeBatches.size() << "\n";
        ss << "  - Max parallel batches: " << gBatchProcessor->getMaxParallelBatches() << "\n";
    }
    
    return ss.str();
}

// Batch processing factory functions
BatchConfig createParameterSweepBatch(
    const std::string& batchName,
    const ParameterSweep& sweep,
    size_t batchSize) {
    BatchConfig config;
    config.batchName = batchName;
    config.type = "parameter_sweep";
    config.progressTracking = true;
    config.parallelExecution = true;
    
    // Generate parameter sets
    std::vector<SimulationConfig> simulations;
    simulations.push_back(SimulationConfig{ "parameter_sweep", {}, 1 });
    
    config.simulations = simulations;
    
    return config;
}

// Hyperparameter optimization factory functions
HyperparameterConfig createHyperparameterConfig(
    const std::string& objectiveFunction,
    const std::vector<std::pair<std::string, std::vector<double>>>& parameterRanges,
    size_t numIterations) {
    HyperparameterConfig config;
    config.objectiveFunction = objectiveFunction;
    config.numIterations = numIterations;
    
    for (const auto& param : parameterRanges) {
        config.parameterRanges[param.first] = param.second;
    }
    
    return config;
}

// Evolutionary algorithm factory functions
EvolutionConfig createEvolutionConfig(
    const std::string& objectiveFunction,
    const std::vector<std::pair<std::string, std::vector<double>>>& parameterRanges,
    size_t generations,
    size_t populationSize) {
    EvolutionConfig config;
    config.objectiveFunction = objectiveFunction;
    config.generations = generations;
    config.populationSize = populationSize;
    
    for (const auto& param : parameterRanges) {
        config.parameterRanges[param.first] = param.second;
    }
    
    return config;
}

// Genetic algorithm factory functions
GeneticAlgorithmConfig createGeneticAlgorithmConfig(
    const std::string& objectiveFunction,
    const std::vector<std::pair<std::string, std::vector<double>>>& parameterRanges,
    size_t generations,
    size_t populationSize) {
    GeneticAlgorithmConfig config;
    config.objectiveFunction = objectiveFunction;
    config.generations = generations;
    config.populationSize = populationSize;
    
    for (const auto& param : parameterRanges) {
        config.parameterRanges[param.first] = param.second;
    }
    
    return config;
}

// Particle swarm optimization factory functions
ParticleSwarmConfig createParticleSwarmConfig(
    const std::string& objectiveFunction,
    const std::vector<std::pair<std::string, std::vector<double>>>& parameterRanges,
    size_t maxIterations,
    size_t numParticles) {
    ParticleSwarmConfig config;
    config.objectiveFunction = objectiveFunction;
    config.maxIterations = maxIterations;
    config.numParticles = numParticles;
    
    for (const auto& param : parameterRanges) {
        config.parameterRanges[param.first] = param.second;
    }
    
    return config;
}

// Simulated annealing factory functions
SimulatedAnnealingConfig createSimulatedAnnealingConfig(
    const std::string& objectiveFunction,
    const std::vector<std::pair<std::string, std::vector<double>>>& parameterRanges,
    size_t maxIterations,
    double initialTemp,
    double coolingRate) {
    SimulatedAnnealingConfig config;
    config.objectiveFunction = objectiveFunction;
    config.maxIterations = maxIterations;
    config.initialTemp = initialTemp;
    config.coolingRate = coolingRate;
    
    for (const auto& param : parameterRanges) {
        config.parameterRanges[param.first] = param.second;
    }
    
    return config;
}

// Bayesian optimization factory functions
BayesianOptimizationConfig createBayesianOptimizationConfig(
    const std::string& objectiveFunction,
    const std::vector<std::pair<std::string, std::vector<double>>>& parameterRanges,
    size_t numIterations,
    const std::string& acquisitionFunction) {
    BayesianOptimizationConfig config;
    config.objectiveFunction = objectiveFunction;
    config.numIterations = numIterations;
    config.acquisitionFunction = acquisitionFunction;
    
    for (const auto& param : parameterRanges) {
        config.parameterRanges[param.first] = param.second;
    }
    
    return config;
}

// Random search factory functions
RandomSearchConfig createRandomSearchConfig(
    const std::string& objectiveFunction,
    const std::vector<std::pair<std::string, std::vector<double>>>& parameterRanges,
    size_t numSamples) {
    RandomSearchConfig config;
    config.objectiveFunction = objectiveFunction;
    config.numSamples = numSamples;
    
    for (const auto& param : parameterRanges) {
        config.parameterRanges[param.first] = param.second;
    }
    
    return config;
}

// Grid search factory functions
GridSearchConfig createGridSearchConfig(
    const std::string& objectiveFunction,
    const std::vector<std::pair<std::string, std::vector<double>>>& parameterRanges,
    size_t maxCombinations) {
    GridSearchConfig config;
    config.objectiveFunction = objectiveFunction;
    config.maxCombinations = maxCombinations;
    
    for (const auto& param : parameterRanges) {
        config.parameterRanges[param.first] = param.second;
    }
    
    return config;
}

} // namespace nlm
