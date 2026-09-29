// NLM Advanced Learning Stress Tests
// Comprehensive stress testing for Advanced Learning system

#include "stress_tests/AdvancedLearningStressTests.hpp"
#include "learning/AdvancedLearning.hpp"
#include "learning/TransferLearning.hpp"
#include "learning/SocialLearning.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "core/Config/Config.hpp"
#include "core/Profiler.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <thread>
#include <memory>
#include <vector>
#include <algorithm>
#include <random>
#include <map>
#include <numeric>
#include <cmath>
#include <unordered_map>
#include <set>
#include <string>

using namespace nlm;

namespace stress_tests {

// Transfer learning stress tests
void AdvancedLearningStressTests::runTransferLearningStressTests() {
    std::cout << "=== Transfer Learning Stress Tests ===" << std::endl;
    
    const std::vector<size_t> baseTaskSizes = {50, 100, 200, 400};
    const size_t steps = 150;
    const std::vector<float> transferStrengths = {0.1f, 0.5f, 1.0f, 2.0f, 5.0f};
    
    TransferLearningResults globalResults;
    globalResults.totalTests = baseTaskSizes.size() * transferStrengths.size();
    
    for (size_t baseTaskSize : baseTaskSizes) {
        for (float transferStrength : transferStrengths) {
            std::cout << "Transfer learning stress test: base task " << baseTaskSize << ", transfer strength " << transferStrength << "..." << std::flush;
            
            auto config = std::make_shared<Config>();
            config->set("base_task_size", static_cast<int64_t>(baseTaskSize), ConfigSource::Default);
            config->set("transfer_task_size", static_cast<int64_t>(baseTaskSize), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
            config->set("connection_probability", 0.1f, ConfigSource::Default);
            config->set("transfer_strength", transferStrength, ConfigSource::Default);
            
            auto transferLearning = std::make_shared<TransferLearning>(config);
            transferLearning->initialize();
            
            // Start profiling transfer learning operations
            startPhase(SimulationStepProfiler::PHASE_SYNAPSE_ACTIVATION);
            
            TransferLearningStressTestResults result;
            result.baseTaskSize = baseTaskSize;
            result.transferStrength = transferStrength;
            result.totalSteps = steps;
            
            auto startTime = std::chrono::high_resolution_clock::now();
            
            // Run transfer learning simulation
            for (size_t step = 0; step < steps; ++step) {
                // Generate base task pattern
                std::vector<float> basePattern(baseTaskSize);
                for (size_t i = 0; i < baseTaskSize; ++i) {
                    basePattern[i] = (static_cast<float>(std::rand()) / RAND_MAX) * 2.0f - 1.0f;
                }
                
                // Generate transfer task pattern (different but related)
                std::vector<float> transferPattern(baseTaskSize);
                for (size_t i = 0; i < baseTaskSize; ++i) {
                    // Transfer with some variation
                    float noise = (static_cast<float>(std::rand()) / RAND_MAX - 0.5f) * (1.0f / transferStrength);
                    transferPattern[i] = basePattern[i] + noise;
                }
                
                // Perform transfer learning
                float transferEfficiency = transferLearning->transferKnowledge(basePattern, transferPattern);
                
                // Get transfer learning metrics
                auto transferStats = transferLearning->getTransferStats();
                result.totalTransferEfficiency += transferEfficiency;
                result.maxTransferGain = std::max(result.maxTransferGain, transferEfficiency);
                result.averageTransferTime += transferStats.averageTransferTime;
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            stopPhase(); // PHASE_SYNAPSE_ACTIVATION
            
            double totalTime = std::chrono::duration<double>(endTime - startTime).count();
            
            // Get profiling statistics
            auto& stepProfiler = SimulationStepProfiler::getInstance();
            auto synapseActivationStats = stepProfiler.getPhaseStats(SimulationStepProfiler::PHASE_SYNAPSE_ACTIVATION);
            
            TransferLearningStats transferStats;
            transferStats.totalBaseTasks = baseTaskSize;
            transferStats.totalTransferTasks = baseTaskSize;
            transferStats.averageTransferTime = result.averageTransferTime / steps;
            transferStats.transferEfficiency = calculateTransferLearningEfficiency(baseTaskSize, transferStrength);
            
            result.totalTime = totalTime;
            result.stepsPerSecond = steps / totalTime;
            result.averageTransferEfficiency = result.totalTransferEfficiency / steps;
            result.maxTransferGain = result.maxTransferGain;
            result.averageTransferTime = transferStats.averageTransferTime;
            
            globalResults.testResults.push_back(result);
            
            std::cout << " Done!" << std::endl;
            std::cout << "    Steps/sec: " << std::fixed << std::setprecision(1) << result.stepsPerSecond << std::endl;
            std::cout << "    Avg transfer efficiency: " << std::fixed << std::setprecision(2) << result.averageTransferEfficiency << std::endl;
            std::cout << "    Max transfer gain: " << std::fixed << std::setprecision(2) << result.maxTransferGain << std::endl;
            
            resetProfiling(); // Reset for next test
        }
    }
    
    printTransferLearningStressResults(globalResults);
}

// Meta-learning stress tests
void AdvancedLearningStressTests::runMetaLearningStressTests() {
    std::cout << "=== Meta-Learning Stress Tests ===" << std::endl;
    
    const std::vector<size_t> taskVarieties = {20, 50, 100, 200};
    const size_t metaLearningSteps = 200;
    const std::vector<float> metaLearningRates = {0.01f, 0.05f, 0.1f, 0.2f, 0.5f};
    
    MetaLearningResults globalResults;
    globalResults.totalTests = taskVarieties.size() * metaLearningRates.size();
    
    for (size_t taskVariety : taskVarieties) {
        for (float metaLearningRate : metaLearningRates) {
            std::cout << "Meta-learning stress test: " << taskVariety << " task varieties, meta LR " << metaLearningRate << "..." << std::flush;
            
            auto config = std::make_shared<Config>();
            config->set("task_variety", static_cast<int64_t>(taskVariety), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
            config->set("connection_probability", 0.08f, ConfigSource::Default);
            config->set("meta_learning_rate", metaLearningRate, ConfigSource::Default);
            
            auto metaLearning = std::make_shared<AdvancedLearning>(config);
            metaLearning->initialize();
            
            // Start profiling meta-learning operations
            startPhase(SimulationStepProfiler::PHASE_SYNAPSE_ACTIVATION);
            startPhase(SimulationStepProfiler::PHASE_NEURON_ACTIVATION);
            
            MetaLearningStressTestResults result;
            result.taskVariety = taskVariety;
            result.metaLearningRate = metaLearningRate;
            result.totalSteps = metaLearningSteps;
            
            auto startTime = std::chrono::high_resolution_clock::now();
            
            // Run meta-learning simulation
            for (size_t step = 0; step < metaLearningSteps; ++step) {
                // Generate diverse task
                std::vector<float> taskPattern(taskVariety);
                for (size_t i = 0; i < taskVariety; ++i) {
                    taskPattern[i] = (static_cast<float>(std::rand()) / RAND_MAX) * 2.0f - 1.0f;
                }
                
                // Meta-learning operation
                float metaLearningResult = metaLearning->optimizeLearningStrategy(taskPattern);
                
                // Get meta-learning metrics
                auto metaStats = metaLearning->getMetaLearningStats();
                result.totalMetaLearningEfficiency += metaLearningResult;
                result.maxMetaLearningGain = std::max(result.maxMetaLearningGain, metaLearningResult);
                result.averageMetaLearningTime += metaStats.averageMetaLearningTime;
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            stopPhase(); // PHASE_NEURON_ACTIVATION
            stopPhase(); // PHASE_SYNAPSE_ACTIVATION
            
            double totalTime = std::chrono::duration<double>(endTime - startTime).count();
            
            // Get profiling statistics
            auto& stepProfiler = SimulationStepProfiler::getInstance();
            auto synapseActivationStats = stepProfiler.getPhaseStats(SimulationStepProfiler::PHASE_SYNAPSE_ACTIVATION);
            auto neuronActivationStats = stepProfiler.getPhaseStats(SimulationStepProfiler::PHASE_NEURON_ACTIVATION);
            
            MetaLearningStats metaStats;
            metaStats.totalTasksLearned = taskVariety;
            metaStats.averageMetaLearningTime = result.averageMetaLearningTime / metaLearningSteps;
            metaStats.metaLearningEfficiency = calculateMetaLearningEfficiency(taskVariety, metaLearningRate);
            
            result.totalTime = totalTime;
            result.stepsPerSecond = metaLearningSteps / totalTime;
            result.averageMetaLearningEfficiency = result.totalMetaLearningEfficiency / metaLearningSteps;
            result.maxMetaLearningGain = result.maxMetaLearningGain;
            result.averageMetaLearningTime = metaStats.averageMetaLearningTime;
            
            globalResults.testResults.push_back(result);
            
            std::cout << " Done!" << std::endl;
            std::cout << "    Steps/sec: " << std::fixed << std::setprecision(1) << result.stepsPerSecond << std::endl;
            std::cout << "    Avg meta learning efficiency: " << std::fixed << std::setprecision(2) << result.averageMetaLearningEfficiency << std::endl;
            std::cout << "    Max meta learning gain: " << std::fixed << std::setprecision(2) << result.maxMetaLearningGain << std::endl;
            
            resetProfiling(); // Reset for next test
        }
    }
    
    printMetaLearningStressResults(globalResults);
}

// Fast synaptic plasticity stress tests
void AdvancedLearningStressTests::runFastSynapticPlasticityStressTests() {
    std::cout << "=== Fast Synaptic Plasticity Stress Tests ===" << std::endl;
    
    const std::vector<size_t> neuronCounts = {100, 300, 600, 1200};
    const size_t fastPlasticitySteps = 100;
    const std::vector<float> plasticityRates = {0.5f, 1.0f, 2.0f, 5.0f, 10.0f};
    
    FastPlasticityResults globalResults;
    globalResults.totalTests = neuronCounts.size() * plasticityRates.size();
    
    for (size_t neuronCount : neuronCounts) {
        for (float plasticityRate : plasticityRates) {
            std::cout << "Fast synaptic plasticity stress test: " << neuronCount << " neurons, plasticity rate " << plasticityRate << "..." << std::flush;
            
            auto config = std::make_shared<Config>();
            config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
            config->set("connection_probability", 0.1f, ConfigSource::Default);
            config->set("fast_plasticity_rate", plasticityRate, ConfigSource::Default);
            
            auto fastPlasticity = std::make_shared<AdvancedLearning>(config);
            fastPlasticity->initialize();
            
            // Start profiling fast plasticity operations
            startPhase(SimulationStepProfiler::PHASE_SYNAPSE_ACTIVATION);
            
            FastPlasticityStressTestResults result;
            result.neuronCount = neuronCount;
            result.plasticityRate = plasticityRate;
            result.totalSteps = fastPlasticitySteps;
            
            auto startTime = std::chrono::high_resolution_clock::now();
            
            // Run fast plasticity simulation
            for (size_t step = 0; step < fastPlasticitySteps; ++step) {
                // Generate spike pairs for fast plasticity
                for (size_t i = 0; i < neuronCount / 10; ++i) {
                    float preTime = static_cast<float>(step * 1000 + i * 5); // 5ms intervals (fast)
                    float postTime = preTime + 10.0f; // 10ms post (rapid plasticity)
                    
                    // Fast synaptic plasticity operation
                    float plasticityEffect = fastPlasticity->applyFastPlasticity(preTime, postTime);
                    
                    // Get fast plasticity metrics
                    auto plasticityStats = fastPlasticity->getFastPlasticityStats();
                    result.totalPlasticityEffect += plasticityEffect;
                    result.maxPlasticityChange = std::max(result.maxPlasticityChange, plasticityEffect);
                }
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            stopPhase(); // PHASE_SYNAPSE_ACTIVATION
            
            double totalTime = std::chrono::duration<double>(endTime - startTime).count();
            
            // Get profiling statistics
            auto& stepProfiler = SimulationStepProfiler::getInstance();
            auto synapseActivationStats = stepProfiler.getPhaseStats(SimulationStepProfiler::PHASE_SYNAPSE_ACTIVATION);
            
            FastPlasticityStats plasticityStats;
            plasticityStats.totalPlasticityEvents = neuronCount * fastPlasticitySteps / 10;
            plasticityStats.averagePlasticityTime = synapseActivationStats.averageTimeNs() / 1000.0;
            plasticityStats.plasticityEfficiency = calculateFastPlasticityEfficiency(neuronCount, plasticityRate);
            
            result.totalTime = totalTime;
            result.stepsPerSecond = fastPlasticitySteps / totalTime;
            result.averagePlasticityEffect = result.totalPlasticityEffect / (neuronCount * fastPlasticitySteps / 10);
            result.maxPlasticityChange = result.maxPlasticityChange;
            result.averagePlasticityTime = plasticityStats.averagePlasticityTime;
            
            globalResults.testResults.push_back(result);
            
            std::cout << " Done!" << std::endl;
            std::cout << "    Steps/sec: " << std::fixed << std::setprecision(1) << result.stepsPerSecond << std::endl;
            std::cout << "    Avg plasticity effect: " << std::fixed << std::setprecision(3) << result.averagePlasticityEffect << std::endl;
            std::cout << "    Max plasticity change: " << std::fixed << std::setprecision(3) << result.maxPlasticityChange << std::endl;
            
            resetProfiling(); // Reset for next test
        }
    }
    
    printFastPlasticityStressResults(globalResults);
}

// Bayesian learning stress tests
void AdvancedLearningStressTests::runBayesianLearningStressTests() {
    std::cout << "=== Bayesian Learning Stress Tests ===" << std::endl;
    
    const std::vector<size_t> beliefSystems = {50, 100, 200, 500};
    const size_t bayesianSteps = 80;
    const std::vector<float> learningPriors = {0.1f, 0.5f, 1.0f, 2.0f, 5.0f};
    
    BayesianLearningResults globalResults;
    globalResults.totalTests = beliefSystems.size() * learningPriors.size();
    
    for (size_t beliefSystem : beliefSystems) {
        for (float learningPrior : learningPriors) {
            std::cout << "Bayesian learning stress test: " << beliefSystem << " beliefs, prior " << learningPrior << "..." << std::flush;
            
            auto config = std::make_shared<Config>();
            config->set("belief_system_size", static_cast<int64_t>(beliefSystem), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
            config->set("connection_probability", 0.08f, ConfigSource::Default);
            config->set("bayesian_prior", learningPrior, ConfigSource::Default);
            
            auto bayesianLearning = std::make_shared<AdvancedLearning>(config);
            bayesianLearning->initialize();
            
            // Start profiling Bayesian learning operations
            startPhase(SimulationStepProfiler::PHASE_NEURON_ACTIVATION);
            
            BayesianLearningStressTestResults result;
            result.beliefSystemSize = beliefSystem;
            result.learningPrior = learningPrior;
            result.totalSteps = bayesianSteps;
            
            auto startTime = std::chrono::high_resolution_clock::now();
            
            // Run Bayesian learning simulation
            for (size_t step = 0; step < bayesianSteps; ++step) {
                // Generate observation
                std::vector<float> observation(beliefSystem);
                for (size_t i = 0; i < beliefSystem; ++i) {
                    observation[i] = (static_cast<float>(std::rand()) / RAND_MAX) * 2.0f - 1.0f;
                }
                
                // Bayesian learning operation
                float posteriorUpdate = bayesianLearning->updateBeliefDistribution(observation);
                
                // Get Bayesian learning metrics
                auto bayesianStats = bayesianLearning->getBayesianLearningStats();
                result.totalPosteriorUpdate += posteriorUpdate;
                result.maxPosteriorChange = std::max(result.maxPosteriorChange, posteriorUpdate);
                result.averageBeliefConvergence += bayesianStats.beliefConvergenceRate;
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            stopPhase(); // PHASE_NEURON_ACTIVATION
            
            double totalTime = std::chrono::duration<double>(endTime - startTime).count();
            
            // Get profiling statistics
            auto& stepProfiler = SimulationStepProfiler::getInstance();
            auto neuronActivationStats = stepProfiler.getPhaseStats(SimulationStepProfiler::PHASE_NEURON_ACTIVATION);
            
            BayesianLearningStats bayesianStats;
            bayesianStats.totalBeliefs = beliefSystem;
            bayesianStats.averageBeliefConvergenceRate = result.averageBeliefConvergence / bayesianSteps;
            bayesianStats.bayesianEfficiency = calculateBayesianLearningEfficiency(beliefSystem, learningPrior);
            
            result.totalTime = totalTime;
            result.stepsPerSecond = bayesianSteps / totalTime;
            result.averagePosteriorUpdate = result.totalPosteriorUpdate / (beliefSystem * bayesianSteps);
            result.maxPosteriorChange = result.maxPosteriorChange;
            result.averageBeliefConvergence = bayesianStats.averageBeliefConvergenceRate;
            
            globalResults.testResults.push_back(result);
            
            std::cout << " Done!" << std::endl;
            std::cout << "    Steps/sec: " << std::fixed << std::setprecision(1) << result.stepsPerSecond << std::endl;
            std::cout << "    Avg posterior update: " << std::fixed << std::setprecision(3) << result.averagePosteriorUpdate << std::endl;
            std::cout << "    Max posterior change: " << std::fixed << std::setprecision(3) << result.maxPosteriorChange << std::endl;
            
            resetProfiling(); // Reset for next test
        }
    }
    
    printBayesianLearningStressResults(globalResults);
}

// Helper functions for advanced learning stress test analysis

double AdvancedLearningStressTests::calculateTransferLearningEfficiency(size_t baseTaskSize, float transferStrength) {
    double sizeFactor = 100.0 / baseTaskSize;
    double strengthFactor = std::min(1.0, transferStrength / 5.0f); // Normalize to max 5.0
    
    return sizeFactor * strengthFactor;
}

double AdvancedLearningStressTests::calculateMetaLearningEfficiency(size_t taskVariety, float metaLearningRate) {
    double varietyFactor = 1000.0 / taskVariety;
    double rateFactor = std::log(1.0 + metaLearningRate); // Logarithmic scaling
    
    return varietyFactor * rateFactor;
}

double AdvancedLearningStressTests::calculateFastPlasticityEfficiency(size_t neuronCount, float plasticityRate) {
    double neuronFactor = 10000.0 / neuronCount;
    double rateFactor = std::min(1.0, plasticityRate / 10.0f); // Normalize to max 10.0
    
    return neuronFactor * rateFactor;
}

double AdvancedLearningStressTests::calculateBayesianLearningEfficiency(size_t beliefSystem, float learningPrior) {
    double beliefFactor = 1000.0 / beliefSystem;
    double priorFactor = std::min(1.0, learningPrior / 5.0f); // Normalize to max 5.0
    
    return beliefFactor * priorFactor;
}

// Result printing functions

void AdvancedLearningStressTests::printTransferLearningStressResults(const TransferLearningResults& results) {
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                Transfer Learning Stress Test Results                    ║" << std::endl;
    std::cout << "╠════════════╦════════════╦════════╦════════════╦══════════╦════════════╗" << std::endl;
    std::cout << "║  Base Task ║ Transfer  ║ Steps/s ║ Avg Eff   ║ Max Gain ║ Avg Time  ║" << std::endl;
    std::cout << "╠════════════╬════════════╬════════╬════════════╬══════════╬════════════╗" << std::endl;
    
    for (const auto& r : results.testResults) {
        std::cout << "║ " << std::setw(10) << r.baseTaskSize
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(1) << r.transferStrength
                  << " ║ " << std::setw(7) << std::fixed << std::setprecision(1) << r.stepsPerSecond
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(2) << r.averageTransferEfficiency
                  << " ║ " << std::setw(10) << std::fixed << std::setprecision(2) << r.maxTransferGain
                  << " ║ " << std::setw(12) << std::fixed << std::setprecision(3) << r.averageTransferTime
                  << " ║" << std::endl;
    }
    
    std::cout << "╚════════════╩════════════╩════════╩════════════╩══════════╩════════════╝" << std::endl;
    std::cout << std::endl;
}

void AdvancedLearningStressTests::printMetaLearningStressResults(const MetaLearningResults& results) {
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                 Meta-Learning Stress Test Results                        ║" << std::endl;
    std::cout << "╠════════════╦═════════╦════════╦════════════╦══════════╦════════════╗" << std::endl;
    std::cout << "║  Varieties ║ Meta LR  ║ Steps/s ║ Avg Eff   ║ Max Gain ║ Avg Time  ║" << std::endl;
    std::cout << "╠════════════╬═════════╬════════╬════════════╬══════════╬════════════╗" << std::endl;
    
    for (const auto& r : results.testResults) {
        std::cout << "║ " << std::setw(10) << r.taskVariety
                  << " ║ " << std::setw(8) << std::fixed << std::setprecision(2) << r.metaLearningRate
                  << " ║ " << std::setw(7) << std::fixed << std::setprecision(1) << r.stepsPerSecond
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(2) << r.averageMetaLearningEfficiency
                  << " ║ " << std::setw(10) << std::fixed << std::setprecision(2) << r.maxMetaLearningGain
                  << " ║ " << std::setw(12) << std::fixed << std::setprecision(3) << r.averageMetaLearningTime
                  << " ║" << std::endl;
    }
    
    std::cout << "╚════════════╩═════════╩════════╩════════════╩══════════╩════════════╝" << std::endl;
    std::cout << std::endl;
}

void AdvancedLearningStressTests::printFastPlasticityStressResults(const FastPlasticityResults& results) {
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║               Fast Synaptic Plasticity Stress Test Results               ║" << std::endl;
    std::cout << "╠════════════╦════════════╦════════╦════════════╦════════╦════════╗" << std::endl;
    std::cout << "║  Neurons   ║ Rate      ║ Steps/s ║ Avg Eff   ║ Max Chg ║ Avg Time ║" << std::endl;
    std::cout << "╠════════════╬════════════╬════════╬════════════╬════════╬════════╗" << std::endl;
    
    for (const auto& r : results.testResults) {
        std::cout << "║ " << std::setw(10) << r.neuronCount
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(1) << r.plasticityRate
                  << " ║ " << std::setw(7) << std::fixed << std::setprecision(1) << r.stepsPerSecond
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(3) << r.averagePlasticityEffect
                  << " ║ " << std::setw(7) << std::fixed << std::setprecision(3) << r.maxPlasticityChange
                  << " ║ " << std::setw(9) << std::fixed << std::setprecision(3) << r.averagePlasticityTime
                  << " ║" << std::endl;
    }
    
    std::cout << "╚════════════╩════════════╩════════╩════════════╩════════╩════════╝" << std::endl;
    std::cout << std::endl;
}

void AdvancedLearningStressTests::printBayesianLearningStressResults(const BayesianLearningResults& results) {
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                 Bayesian Learning Stress Test Results                   ║" << std::endl;
    std::cout << "╠════════════╦═════════╦════════╦════════════╦════════════╦════════╗" << std::endl;
    std::cout << "║  Beliefs   ║ Prior   ║ Steps/s ║ Avg Update ║ Max Change ║ Convergence ║" << std::endl;
    std::cout << "╠════════════╬═════════╬════════╬════════════╬════════════╬════════╗" << std::endl;
    
    for (const auto& r : results.testResults) {
        std::cout << "║ " << std::setw(10) << r.beliefSystemSize
                  << " ║ " << std::setw(8) << std::fixed << std::setprecision(1) << r.learningPrior
                  << " ║ " << std::setw(7) << std::fixed << std::setprecision(1) << r.stepsPerSecond
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(3) << r.averagePosteriorUpdate
                  << " ║ " << std::setw(12) << std::fixed << std::setprecision(3) << r.maxPosteriorChange
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(2) << r.averageBeliefConvergence
                  << " ║" << std::endl;
    }
    
    std::cout << "╚════════════╩═════════╩════════╩════════════╩════════════╩════════╝" << std::endl;
    std::cout << std::endl;
}

} // namespace stress_tests

// Main advanced learning stress test runner
int main() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — Advanced Learning Stress Tests                     ║
    ║                                                               ║
    ║     Comprehensive Advanced Learning System Stress Testing      ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Initialize random number generator
    auto rng = std::make_shared<RandomGenerator>();
    RandomGenerator::setGlobal(rng);
    
    std::cout << "Running Advanced Learning System Stress Tests..." << std::endl;
    std::cout << std::endl;
    
    // Run all advanced learning stress test suites
    stress_tests::AdvancedLearningStressTests::runTransferLearningStressTests();
    std::cout << std::endl;
    
    stress_tests::AdvancedLearningStressTests::runMetaLearningStressTests();
    std::cout << std::endl;
    
    stress_tests::AdvancedLearningStressTests::runFastSynapticPlasticityStressTests();
    std::cout << std::endl;
    
    stress_tests::AdvancedLearningStressTests::runBayesianLearningStressTests();
    std::cout << std::endl;
    
    // Generate final profiling report
    std::cout << "=== FINAL ADVANCED LEARNING STRESS TEST PROFILING REPORT ===" << std::endl;
    std::cout << generateProfilingReport() << std::endl;
    
    std::cout << "Advanced Learning System Stress Tests complete." << std::endl;
    
    return 0;
}
