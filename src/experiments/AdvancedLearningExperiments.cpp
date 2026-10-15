// Advanced Learning and Analysis Experiments
// Sophisticated experiments for power users and research

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <map>
#include <unordered_set>
#include <chrono>

using namespace nlm;

// Advanced experiment result structure
struct AdvancedExperimentResult {
    std::string experimentName;
    std::string experimentType;
    size_t neurons;
    size_t steps;
    double totalTime;
    
    // Performance metrics
    double averageFiringRate;
    double networkStability;
    double learningProgress;
    double convergenceRate;
    
    // Advanced metrics
    double plasticityStrength;
    double informationStorage;
    double generalizationAbility;
    double adaptationSpeed;
    double memoryRetention;
    
    // Learning dynamics
    std::vector<double> learningCurve;
    std::vector<double> stabilityMetrics;
    std::vector<double> plasticityMetrics;
    
    // Detailed statistics
    size_t totalSpikes;
    size_t maxSpikeCount;
    double spikeVariability;
    
    // Experiment-specific results
    std::map<std::string, double> customMetrics;
    
    // Timestamp
    std::string timestamp;
};

// Meta-learning experiment: Learning how to learn
class MetaLearningExperiment {
private:
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    
    // Meta-learning state
    double learningRateAdaptation;
    double explorationExploitationBalance;
    double curiosityLevel;
    double metaPlasticity;
    
    // Performance tracking
    std::vector<double> adaptationHistory;
    std::vector<double> explorationHistory;
    std::vector<double> learningEfficiency;
    
public:
    MetaLearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
        : brain(b), seed(s), learningRateAdaptation(0.01), explorationExploitationBalance(0.5),
          curiosityLevel(0.3), metaPlasticity(0.005) {}
    
    AdvancedExperimentResult run(size_t steps) {
        AdvancedExperimentResult result;
        result.experimentName = "Meta-Learning Experiment";
        result.experimentType = "meta_learning";
        result.neurons = brain->getTotalNeuronCount();
        result.steps = steps;
        
        // Initialize meta-learning state
        double baseLearningRate = 0.01;
        double currentLearningRate = baseLearningRate;
        double performanceHistory[100] = {0};
        
        // Meta-learning loop
        for (size_t step = 0; step < steps; ++step) {
            // Adapt learning rate based on performance
            double performance = calculateMetaLearningPerformance(brain, step);
            adaptLearningRate(currentLearningRate, performance);
            
            // Balance exploration and exploitation
            double exploration = calculateExplorationLevel(step, steps);
            double exploitation = 1.0 - exploration;
            
            // Apply meta-plastic changes
            applyMetaPlasticity(brain, currentLearningRate, exploration, exploitation);
            
            // Run brain simulation
            injectMetaLearningInput(brain, step, exploration);
            brain->step(step, step * 0.001);
            
            // Track performance
            performanceHistory[step % 100] = performance;
            updateMetaLearningHistory(performance, exploration, currentLearningRate);
            
            // Log progress occasionally
            if (step % 500 == 0) {
                NLM_LOG_INFO("Meta-learning step " + std::to_string(step) + 
                           ", learning rate: " + std::to_string(currentLearningRate) +
                           ", performance: " + std::to_string(performance));
            }
        }
        
        // Calculate results
        result.totalTime = 0.0;  // Would be measured
        result.averageFiringRate = calculateAverageFiringRate(brain);
        result.totalSpikes = brain->getTotalSpikeCount();
        result.maxSpikeCount = calculateMaxSpikeCount(brain);
        result.spikeVariability = calculateSpikeVariability(brain, steps);
        
        // Calculate meta-learning specific metrics
        result.plasticityStrength = calculateMetaPlasticityStrength();
        result.informationStorage = calculateInformationStorage();
        result.generalizationAbility = calculateGeneralizationAbility();
        result.adaptationSpeed = calculateAdaptationSpeed();
        result.memoryRetention = calculateMemoryRetention();
        
        // Calculate learning dynamics
        result.learningCurve = calculateLearningCurve(performanceHistory);
        result.stabilityMetrics = calculateStabilityMetrics();
        result.plasticityMetrics = calculatePlasticityMetrics();
        
        // Set timestamp
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        result.timestamp = ss.str();
        
        return result;
    }
    
private:
    double calculateMetaLearningPerformance(std::shared_ptr<Brain> brain, size_t step) {
        // Calculate how well the meta-learning is performing
        double firingRate = calculateAverageFiringRate(brain);
        double plasticityEffect = calculatePlasticityEffect(brain, step);
        double stability = calculateNetworkStability(brain);
        
        // Combine metrics into a single performance score
        return (firingRate * 0.4 + plasticityEffect * 0.3 + stability * 0.3);
    }
    
    void adaptLearningRate(double& learningRate, double performance) {
        // Adapt learning rate based on performance
        if (performance > 0.8) {
            // Reduce learning rate if performance is good
            learningRate *= 0.95;
        } else if (performance < 0.3) {
            // Increase learning rate if performance is poor
            learningRate *= 1.05;
        }
        
        // Apply meta-plasticity
        learningRate += metaPlasticity * (0.5 - performance);
        
        // Clamp learning rate
        learningRate = std::max(0.001, std::min(0.1, learningRate));
    }
    
    double calculateExplorationLevel(size_t step, size_t totalSteps) {
        // Balance exploration and exploitation over time
        double timeProgress = static_cast<double>(step) / totalSteps;
        
        if (timeProgress < 0.3) {
            return 0.7;  // High exploration early
        } else if (timeProgress < 0.7) {
            return 0.3;  // Moderate exploration
        } else {
            return 0.1;  // Low exploration late
        }
    }
    
    void applyMetaPlasticity(std::shared_ptr<Brain> brain, double learningRate, 
                            double exploration, double exploitation) {
        // Apply meta-plastic changes to synapses
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                // Modify plasticity based on meta-learning state
                double plasticChange = metaPlasticity * learningRate;
                
                if (exploration > 0.5) {
                    // Exploration: increase plasticity
                    syn->enablePlasticity(true, true, true);
                    // Add small random modifications
                    syn->setWeight(syn->getWeight() * (1.0 + plasticChange * 0.1));
                } else {
                    // Exploitation: refine existing connections
                    syn->enablePlasticity(true, true, false);
                }
            }
        }
    }
    
    void injectMetaLearningInput(std::shared_ptr<Brain> brain, size_t step, double exploration) {
        // Inject input based on exploration-exploitation balance
        if (exploration > 0.6) {
            // Exploration: random, varied input
            for (size_t i = 0; i < brain->getTotalNeuronCount() / 20; ++i) {
                double currentStrength = 20.0 + (exploration * 30.0);
                brain->injectCurrentToNeurons(static_cast<NeuronType>(i % 3), currentStrength);
            }
        } else {
            // Exploitation: focused, learned input
            for (size_t i = 0; i < brain->getTotalNeuronCount() / 10; ++i) {
                double currentStrength = 30.0 + (1.0 - exploration) * 20.0;
                brain->injectCurrentToNeurons(static_cast<NeuronType>(i % 3), currentStrength);
            }
        }
    }
    
    void updateMetaLearningHistory(double performance, double exploration, double learningRate) {
        adaptationHistory.push_back(performance);
        explorationHistory.push_back(exploration);
        learningEfficiency.push_back(performance * learningRate);
    }
    
    double calculateAverageFiringRate(std::shared_ptr<Brain> brain) {
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return 0.0;
        
        auto neurons = region->getAllNeurons();
        double totalFiringRate = 0.0;
        size_t activeNeurons = 0;
        
        for (auto* neuron : neurons) {
            if (neuron->isFiring()) {
                totalFiringRate += neuron->getFiringRate();
                ++activeNeurons;
            }
        }
        
        return activeNeurons > 0 ? totalFiringRate / activeNeurons : 0.0;
    }
    
    size_t calculateMaxSpikeCount(std::shared_ptr<Brain> brain) {
        return brain->getTotalSpikeCount();  // Simplified
    }
    
    double calculateSpikeVariability(std::shared_ptr<Brain> brain, size_t steps) {
        // Calculate spike rate variability
        double meanSpikes = static_cast<double>(brain->getTotalSpikeCount()) / steps;
        return meanSpikes * 0.5;  // Placeholder
    }
    
    double calculateMetaPlasticityStrength() {
        // Calculate how strong the meta-plastic changes are
        double totalChange = 0.0;
        for (double rate : adaptationHistory) {
            totalChange += std::abs(rate - 0.5);  // Deviation from baseline
        }
        return std::min(1.0, totalChange / adaptationHistory.size());
    }
    
    double calculateInformationStorage() {
        // Calculate how much information the network stores
        return 0.8;  // Placeholder
    }
    
    double calculateGeneralizationAbility() {
        // Calculate ability to generalize learned patterns
        return 0.7;  // Placeholder
    }
    
    double calculateAdaptationSpeed() {
        // Calculate how quickly the system adapts to changes
        if (adaptationHistory.size() < 2) return 0.5;
        
        double totalChange = 0.0;
        for (size_t i = 1; i < adaptationHistory.size(); ++i) {
            totalChange += std::abs(adaptationHistory[i] - adaptationHistory[i-1]);
        }
        
        return std::min(1.0, totalChange / (adaptationHistory.size() - 1));
    }
    
    double calculateMemoryRetention() {
        // Calculate how well the network retains information over time
        return 0.6;  // Placeholder
    }
    
    std::vector<double> calculateLearningCurve(const double* performanceHistory) {
        std::vector<double> curve;
        for (size_t i = 0; i < 100; ++i) {
            curve.push_back(performanceHistory[i]);
        }
        return curve;
    }
    
    std::vector<double> calculateStabilityMetrics() {
        std::vector<double> metrics;
        for (double perf : adaptationHistory) {
            metrics.push_back(1.0 - std::abs(perf - 0.5));  // Stability = deviation from baseline
        }
        return metrics;
    }
    
    std::vector<double> calculatePlasticityMetrics() {
        std::vector<double> metrics;
        for (size_t i = 0; i < std::min(size_t(100), explorationHistory.size()); ++i) {
            metrics.push_back(explorationHistory[i]);
        }
        return metrics;
    }
    
    double calculatePlasticityEffect(std::shared_ptr<Brain> brain, size_t step) {
        // Calculate the effect of plasticity on network performance
        double weightChange = 0.0;
        size_t synapseCount = 0;
        
        // This would require accessing synapse history
        return 0.5;  // Placeholder
    }
    
    double calculateNetworkStability(std::shared_ptr<Brain> brain) {
        // Calculate overall network stability
        return 0.8;  // Placeholder
    }
};

// Transfer learning experiment: Apply learned skills to new domains
class TransferLearningExperiment {
private:
    std::shared_ptr<Brain> sourceBrain;
    std::shared_ptr<Brain> targetBrain;
    uint64_t seed;
    
    // Transfer state
    std::vector<std::vector<double>> knowledgeTransfer;
    std::vector<double> transferEfficiency;
    std::vector<double> taskSimilarity;
    
public:
    TransferLearningExperiment(std::shared_ptr<Brain> source, std::shared_ptr<Brain> target, uint64_t s)
        : sourceBrain(source), targetBrain(target), seed(s) {}
    
    AdvancedExperimentResult run(size_t steps, const std::vector<std::string>& sourceTasks) {
        AdvancedExperimentResult result;
        result.experimentName = "Transfer Learning Experiment";
        result.experimentType = "transfer_learning";
        result.neurons = targetBrain->getTotalNeuronCount();
        result.steps = steps;
        
        // Phase 1: Train source brain
        NLM_LOG_INFO("Training source brain on tasks: " + formatTasks(sourceTasks));
        trainSourceBrain(sourceTasks, steps / 3);
        
        // Phase 2: Transfer knowledge
        NLM_LOG_INFO("Transferring knowledge to target brain...");
        transferKnowledge(sourceBrain, targetBrain, steps / 3);
        
        // Phase 3: Fine-tune target brain
        NLM_LOG_INFO("Fine-tuning target brain...");
        fineTuneTargetBrain(targetBrain, steps / 3);
        
        // Calculate results
        result.totalTime = 0.0;  // Would be measured
        result.averageFiringRate = calculateAverageFiringRate(targetBrain);
        result.totalSpikes = targetBrain->getTotalSpikeCount();
        result.maxSpikeCount = calculateMaxSpikeCount(targetBrain);
        result.spikeVariability = calculateSpikeVariability(targetBrain, steps);
        
        // Calculate transfer-specific metrics
        result.plasticityStrength = calculateTransferPlasticity();
        result.informationStorage = calculateTransferredInformation();
        result.generalizationAbility = calculateTransferGeneralization();
        result.adaptationSpeed = calculateTransferAdaptationSpeed();
        result.memoryRetention = calculateTransferMemoryRetention();
        
        // Set timestamp
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        result.timestamp = ss.str();
        
        return result;
    }
    
private:
    std::string formatTasks(const std::vector<std::string>& tasks) {
        std::stringstream ss;
        for (size_t i = 0; i < tasks.size(); ++i) {
            ss << tasks[i];
            if (i < tasks.size() - 1) ss << ", ";
        }
        return ss.str();
    }
    
    void trainSourceBrain(const std::vector<std::string>& tasks, size_t steps) {
        // Train source brain on original tasks
        for (const auto& task : tasks) {
            NLM_LOG_INFO("Training on task: " + task);
            trainOnTask(sourceBrain, task, steps / tasks.size());
        }
    }
    
    void trainOnTask(std::shared_ptr<Brain> brain, const std::string& task, size_t steps) {
        // Train brain on specific task
        for (size_t step = 0; step < steps; ++step) {
            // Task-specific input
            injectTaskInput(brain, task, step);
            brain->step(step, step * 0.001);
        }
    }
    
    void injectTaskInput(std::shared_ptr<Brain> brain, const std::string& task, size_t step) {
        // Inject task-specific input based on task type
        if (task == "pattern_recognition") {
            for (size_t i = 0; i < brain->getTotalNeuronCount() / 15; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 40.0f + (step % 20));
            }
        } else if (task == "sequence_learning") {
            // Create sequential patterns
            double patternStrength = 30.0 + 10.0 * std::sin(step * 0.1);
            for (size_t i = 0; i < brain->getTotalNeuronCount() / 20; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Internal, patternStrength);
            }
        } else if (task == "spatial_reasoning") {
            for (size_t i = 0; i < brain->getTotalNeuronCount() / 12; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 35.0f);
            }
        } else {
            // Default task
            for (size_t i = 0; i < brain->getTotalNeuronCount() / 10; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
        }
    }
    
    void transferKnowledge(std::shared_ptr<Brain> source, std::shared_ptr<Brain> target, size_t steps) {
        // Transfer knowledge from source to target
        for (size_t step = 0; step < steps; ++step) {
            // Combine source and target states
            combineBrainStates(source, target, step);
            
            // Apply transferred plasticity rules
            applyTransferredPlasticity(source, target, step);
            
            target->step(step, step * 0.001);
        }
    }
    
    void combineBrainStates(std::shared_ptr<Brain> source, std::shared_ptr<Brain> target, size_t step) {
        // Combine neural states between brains
        double transferStrength = calculateTransferStrength(step);
        
        // Transfer synaptic weights
        if (auto* sourceRegion = source->getRegion(RegionId(1))) {
            if (auto* targetRegion = target->getRegion(RegionId(1))) {
                auto& sourceSynapses = sourceRegion->getSynapses();
                auto& targetSynapses = targetRegion->getSynapses();
                
                size_t minSize = std::min(sourceSynapses.size(), targetSynapses.size());
                for (size_t i = 0; i < minSize; ++i) {
                    double transferredWeight = sourceSynapses[i]->getWeight() * transferStrength;
                    targetSynapses[i]->setWeight(transferredWeight);
                    targetSynapses[i]->enablePlasticity(true, true, false);
                }
            }
        }
    }
    
    void applyTransferredPlasticity(std::shared_ptr<Brain> source, std::shared_ptr<Brain> target, size_t step) {
        // Apply plasticity rules learned from source to target
        double plasticityTransfer = calculatePlasticityTransfer(step);
        
        if (auto* targetRegion = target->getRegion(RegionId(1))) {
            for (auto& syn : targetRegion->getSynapses()) {
                // Apply source-derived plasticity rules
                syn->enablePlasticity(true, true, plasticityTransfer > 0.5);
                
                if (plasticityTransfer > 0.7) {
                    // Strong transfer: enable all plasticity types
                    syn->enablePlasticity(true, true, true);
                }
            }
        }
    }
    
    void fineTuneTargetBrain(std::shared_ptr<Brain> brain, size_t steps) {
        // Fine-tune the target brain for the new task
        for (size_t step = 0; step < steps; ++step) {
            // Fine-tuning input (less structured than source)
            for (size_t i = 0; i < brain->getTotalNeuronCount() / 8; ++i) {
                double fineTuneStrength = 25.0 + 5.0 * std::sin(step * 0.05);
                brain->injectCurrentToNeurons(NeuronType::Internal, fineTuneStrength);
            }
            
            brain->step(step, step * 0.001);
        }
    }
    
    double calculateTransferStrength(size_t step) {
        // Calculate how strong the knowledge transfer is
        return 0.8 + 0.2 * std::sin(step * 0.02);  // Oscillating transfer strength
    }
    
    double calculatePlasticityTransfer(size_t step) {
        // Calculate how much plasticity is transferred
        return 0.7 + 0.3 * std::cos(step * 0.03);  // Oscillating plasticity transfer
    }
    
    double calculateAverageFiringRate(std::shared_ptr<Brain> brain) {
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return 0.0;
        
        auto neurons = region->getAllNeurons();
        double totalFiringRate = 0.0;
        size_t activeNeurons = 0;
        
        for (auto* neuron : neurons) {
            if (neuron->isFiring()) {
                totalFiringRate += neuron->getFiringRate();
                ++activeNeurons;
            }
        }
        
        return activeNeurons > 0 ? totalFiringRate / activeNeurons : 0.0;
    }
    
    size_t calculateMaxSpikeCount(std::shared_ptr<Brain> brain) {
        return brain->getTotalSpikeCount();  // Simplified
    }
    
    double calculateSpikeVariability(std::shared_ptr<Brain> brain, size_t steps) {
        // Calculate spike rate variability
        double meanSpikes = static_cast<double>(brain->getTotalSpikeCount()) / steps;
        return meanSpikes * 0.4;  // Placeholder
    }
    
    double calculateTransferPlasticity() {
        // Calculate how effective the plasticity transfer was
        return 0.75;  // Placeholder
    }
    
    double calculateTransferredInformation() {
        // Calculate how much information was successfully transferred
        return 0.8;  // Placeholder
    }
    
    double calculateTransferGeneralization() {
        // Calculate how well the transferred knowledge generalizes
        return 0.7;  // Placeholder
    }
    
    double calculateTransferAdaptationSpeed() {
        // Calculate how quickly the target adapts to transferred knowledge
        return 0.6;  // Placeholder
    }
    
    double calculateTransferMemoryRetention() {
        // Calculate how well the target retains transferred knowledge
        return 0.65;  // Placeholder
    }
};

// Continual learning experiment: Learn new tasks without forgetting old ones
class ContinualLearningExperiment {
private:
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    
    // Continual learning state
    std::vector<std::vector<double>> taskPerformance;
    std::vector<std::vector<double>> memoryTraces;
    std::vector<double> forgettingRates;
    std::vector<double> interferenceLevels;
    
public:
    ContinualLearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
        : brain(b), seed(s) {
        forgettingRates = {0.01, 0.02, 0.03, 0.04, 0.05};
        interferenceLevels = {0.1, 0.2, 0.3, 0.4, 0.5};
    }
    
    AdvancedExperimentResult run(size_t stepsPerTask, size_t numTasks) {
        AdvancedExperimentResult result;
        result.experimentName = "Continual Learning Experiment";
        result.experimentType = "continual_learning";
        result.neurons = brain->getTotalNeuronCount();
        result.steps = stepsPerTask * numTasks;
        
        // Initialize performance tracking
        taskPerformance.resize(numTasks);
        memoryTraces.resize(numTasks);
        
        // Continual learning loop
        for (size_t taskId = 0; taskId < numTasks; ++taskId) {
            NLM_LOG_INFO("Training on task " + std::to_string(taskId + 1) + "/" + std::to_string(numTasks));
            
            // Train on current task
            trainOnTask(taskId, stepsPerTask);
            
            // Test on all tasks (including current)
            testAllTasks(taskId, tasks);
            
            // Record performance
            recordTaskPerformance(taskId);
        }
        
        // Calculate continual learning specific metrics
        result.totalTime = 0.0;  // Would be measured
        result.averageFiringRate = calculateAverageFiringRate();
        result.totalSpikes = brain->getTotalSpikeCount();
        result.maxSpikeCount = calculateMaxSpikeCount();
        result.spikeVariability = calculateSpikeVariability();
        
        // Calculate continual learning metrics
        result.plasticityStrength = calculateContinualPlasticity();
        result.informationStorage = calculateContinualInformationStorage();
        result.generalizationAbility = calculateContinualGeneralization();
        result.adaptationSpeed = calculateContinualAdaptationSpeed();
        result.memoryRetention = calculateContinualMemoryRetention();
        
        // Calculate learning dynamics
        result.learningCurve = calculateContinualLearningCurve();
        result.stabilityMetrics = calculateContinualStabilityMetrics();
        result.plasticityMetrics = calculateContinualPlasticityMetrics();
        
        // Set timestamp
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        result.timestamp = ss.str();
        
        return result;
    }
    
private:
    void trainOnTask(size_t taskId, size_t steps) {
        // Train brain on specific task
        for (size_t step = 0; step < steps; ++step) {
            // Task-specific input with increasing difficulty
            injectContinualTaskInput(taskId, step, steps);
            brain->step(step, step * 0.001);
        }
    }
    
    void injectContinualTaskInput(size_t taskId, size_t step, size_t totalSteps) {
        // Inject task-specific input with continual learning considerations
        double taskDifficulty = static_cast<double>(taskId + 1) / (totalSteps + 1);
        double patternComplexity = 0.5 + 0.5 * std::sin(step * 0.1 + taskId * 0.5);
        
        // Task ID influences input pattern
        size_t inputNeurons = brain->getTotalNeuronCount() / (8 - taskId);  // More neurons for harder tasks
        
        for (size_t i = 0; i < inputNeurons; ++i) {
            double baseStrength = 25.0 + taskDifficulty * 20.0;
            double patternModulation = patternComplexity * 15.0;
            double interference = calculateInterferenceLevel(taskId);
            
            double finalStrength = baseStrength + patternModulation - interference;
            brain->injectCurrentToNeurons(static_cast<NeuronType>((i + taskId) % 3), finalStrength);
        }
    }
    
    void testAllTasks(size_t currentTask, const std::vector<std::string>& taskTypes) {
        // Test performance on all tasks (including current)
        for (size_t taskId = 0; taskId <= currentTask; ++taskId) {
            testOnTask(taskId, taskTypes);
        }
    }
    
    void testOnTask(size_t taskId, const std::vector<std::string>& taskTypes) {
        // Test brain performance on specific task
        size_t testSteps = 100;  // Fixed test duration
        for (size_t step = 0; step < testSteps; ++step) {
            injectContinualTaskInput(taskId, step + 1000, 1000);  // Different seed for testing
            brain->step(step + 1000, (step + 1000) * 0.001);
        }
    }
    
    void recordTaskPerformance(size_t taskId) {
        // Record performance metrics for current task
        double performance = calculateTaskPerformance(taskId);
        
        taskPerformance[taskId].push_back(performance);
        memoryTraces[taskId].push_back(performance * 0.8);  // Memory trace approximation
    }
    
    double calculateTaskPerformance(size_t taskId) {
        // Calculate how well the brain performs on specific task
        double firingRate = calculateAverageFiringRate();
        double stability = calculateNetworkStability();
        double plasticityEffect = calculatePlasticityEffect();
        
        // Task-specific factors
        double taskDifficulty = static_cast<double>(taskId + 1) / 10.0;
        double interference = calculateInterferenceLevel(taskId);
        
        return (firingRate * 0.3 + stability * 0.4 + plasticityEffect * 0.3) *
               (1.0 - interference * taskDifficulty);
    }
    
    double calculateAverageFiringRate() {
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return 0.0;
        
        auto neurons = region->getAllNeurons();
        double totalFiringRate = 0.0;
        size_t activeNeurons = 0;
        
        for (auto* neuron : neurons) {
            if (neuron->isFiring()) {
                totalFiringRate += neuron->getFiringRate();
                ++activeNeurons;
            }
        }
        
        return activeNeurons > 0 ? totalFiringRate / activeNeurons : 0.0;
    }
    
    size_t calculateMaxSpikeCount() {
        return brain->getTotalSpikeCount();  // Simplified
    }
    
    double calculateSpikeVariability() {
        // Calculate spike rate variability across all tasks
        double totalSpikes = brain->getTotalSpikeCount();
        double totalSteps = 10000;  // Would be calculated
        return totalSpikes / totalSteps * 0.5;  // Placeholder
    }
    
    double calculateNetworkStability() {
        // Calculate overall network stability
        return 0.75;  // Placeholder
    }
    
    double calculatePlasticityEffect() {
        // Calculate the effect of plasticity on performance
        return 0.8;  // Placeholder
    }
    
    double calculateInterferenceLevel(size_t taskId) {
        // Calculate interference between tasks
        if (taskId < interferenceLevels.size()) {
            return interferenceLevels[taskId];
        }
        return 0.3;  // Default interference
    }
    
    double calculateContinualPlasticity() {
        // Calculate continual learning plasticity effectiveness
        double totalPlasticity = 0.0;
        for (size_t i = 0; i < numTasks; ++i) {
            for (double perf : taskPerformance[i]) {
                totalPlasticity += perf * forgettingRates[i % forgettingRates.size()];
            }
        }
        
        return std::min(1.0, totalPlasticity / (numTasks * 100.0));
    }
    
    double calculateContinualInformationStorage() {
        // Calculate information storage in continual learning
        double totalInformation = 0.0;
        for (size_t i = 0; i < numTasks; ++i) {
            for (double trace : memoryTraces[i]) {
                totalInformation += trace;
            }
        }
        
        return std::min(1.0, totalInformation / (numTasks * 50.0));
    }
    
    double calculateContinualGeneralization() {
        // Calculate generalization ability across tasks
        double totalGeneralization = 0.0;
        for (size_t i = 0; i < numTasks; ++i) {
            if (!taskPerformance[i].empty()) {
                double avgPerf = 0.0;
                for (double perf : taskPerformance[i]) {
                    avgPerf += perf;
                }
                avgPerf /= taskPerformance[i].size();
                totalGeneralization += avgPerf;
            }
        }
        
        return std::min(1.0, totalGeneralization / numTasks);
    }
    
    double calculateContinualAdaptationSpeed() {
        // Calculate adaptation speed in continual learning
        double totalAdaptation = 0.0;
        for (size_t i = 0; i < numTasks; ++i) {
            if (taskPerformance[i].size() > 1) {
                for (size_t j = 1; j < taskPerformance[i].size(); ++j) {
                    totalAdaptation += std::abs(taskPerformance[i][j] - taskPerformance[i][j-1]);
                }
            }
        }
        
        return std::min(1.0, totalAdaptation / 50.0);
    }
    
    double calculateContinualMemoryRetention() {
        // Calculate memory retention across tasks
        double totalRetention = 0.0;
        for (size_t i = 0; i < numTasks; ++i) {
            if (!memoryTraces[i].empty()) {
                double avgRetention = 0.0;
                for (double trace : memoryTraces[i]) {
                    avgRetention += trace;
                }
                avgRetention /= memoryTraces[i].size();
                totalRetention += avgRetention;
            }
        }
        
        return std::min(1.0, totalRetention / numTasks);
    }
    
    std::vector<double> calculateContinualLearningCurve() {
        // Calculate overall learning curve across all tasks
        std::vector<double> curve;
        size_t maxLength = 0;
        for (const auto& perf : taskPerformance) {
            maxLength = std::max(maxLength, (size_t)perf.size());
        }
        
        for (size_t i = 0; i < maxLength; ++i) {
            double avgPerformance = 0.0;
            size_t taskCount = 0;
            for (size_t taskId = 0; taskId < numTasks; ++taskId) {
                if (i < taskPerformance[taskId].size()) {
                    avgPerformance += taskPerformance[taskId][i];
                    ++taskCount;
                }
            }
            
            curve.push_back(taskCount > 0 ? avgPerformance / taskCount : 0.0);
        }
        
        return curve;
    }
    
    std::vector<double> calculateContinualStabilityMetrics() {
        // Calculate stability metrics across tasks
        std::vector<double> metrics;
        for (size_t i = 0; i < numTasks; ++i) {
            double stability = 0.0;
            if (!taskPerformance[i].empty()) {
                double variance = 0.0;
                double mean = 0.0;
                for (double perf : taskPerformance[i]) {
                    mean += perf;
                }
                mean /= taskPerformance[i].size();
                
                for (double perf : taskPerformance[i]) {
                    variance += (perf - mean) * (perf - mean);
                }
                variance /= taskPerformance[i].size();
                
                stability = 1.0 - std::sqrt(variance);  // Stability = 1 - sqrt(variance)
            }
            metrics.push_back(stability);
        }
        return metrics;
    }
    
    std::vector<double> calculateContinualPlasticityMetrics() {
        // Calculate plasticity metrics across tasks
        std::vector<double> metrics;
        for (size_t i = 0; i < numTasks; ++i) {
            double plasticity = 0.0;
            if (!memoryTraces[i].empty()) {
                double avgTrace = 0.0;
                for (double trace : memoryTraces[i]) {
                    avgTrace += trace;
                }
                avgTrace /= memoryTraces[i].size();
                plasticity = avgTrace;
            }
            metrics.push_back(plasticity);
        }
        return metrics;
    }
    
    size_t numTasks = 5;  // Number of tasks in continual learning
};

// Social learning experiment: Learn by observing other agents
class SocialLearningExperiment {
private:
    std::shared_ptr<Brain> ownBrain;
    std::vector<std::shared_ptr<Brain>> otherBrains;
    uint64_t seed;
    
    // Social learning state
    std::vector<std::vector<double>> observationHistory;
    std::vector<double> imitationAccuracy;
    std::vector<double> socialLearningEffect;
    std::vector<std::vector<NeuronId>> socialConnections;
    
public:
    SocialLearningExperiment(std::shared_ptr<Brain> own, std::vector<std::shared_ptr<Brain>> others, uint64_t s)
        : ownBrain(own), otherBrains(others), seed(s) {}
    
    AdvancedExperimentResult run(size_t steps, size_t numObservations) {
        AdvancedExperimentResult result;
        result.experimentName = "Social Learning Experiment";
        result.experimentType = "social_learning";
        result.neurons = ownBrain->getTotalNeuronCount();
        result.steps = steps;
        
        // Initialize social learning state
        observationHistory.resize(numObservations);
        imitationAccuracy.resize(numObservations);
        socialLearningEffect.resize(numObservations);
        socialConnections.resize(numObservations);
        
        // Social learning loop
        for (size_t obsId = 0; obsId < numObservations; ++obsId) {
            NLM_LOG_INFO("Observation " + std::to_string(obsId + 1) + "/" + std::to_string(numObservations));
            
            // Observe other agents
            observeOtherAgents(obsId);
            
            // Imitate observed behaviors
            imitateObservations(obsId);
            
            // Test imitated behaviors
            testImitatedBehaviors(obsId);
            
            // Record social learning metrics
            recordSocialLearningMetrics(obsId);
        }
        
        // Calculate social learning specific results
        result.totalTime = 0.0;  // Would be measured
        result.averageFiringRate = calculateAverageFiringRate();
        result.totalSpikes = ownBrain->getTotalSpikeCount();
        result.maxSpikeCount = calculateMaxSpikeCount();
        result.spikeVariability = calculateSpikeVariability();
        
        // Calculate social learning metrics
        result.plasticityStrength = calculateSocialPlasticity();
        result.informationStorage = calculateSocialInformationStorage();
        result.generalizationAbility = calculateSocialGeneralization();
        result.adaptationSpeed = calculateSocialAdaptationSpeed();
        result.memoryRetention = calculateSocialMemoryRetention();
        
        // Calculate learning dynamics
        result.learningCurve = calculateSocialLearningCurve();
        result.stabilityMetrics = calculateSocialStabilityMetrics();
        result.plasticityMetrics = calculateSocialPlasticityMetrics();
        
        // Set timestamp
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        result.timestamp = ss.str();
        
        return result;
    }
    
private:
    void observeOtherAgents(size_t obsId) {
        // Observe other agents' behaviors and neural states
        observationHistory[obsId].clear();
        socialConnections[obsId].clear();
        
        for (size_t agentId = 0; agentId < otherBrains.size(); ++agentId) {
            // Observe neural activity
            double agentPerformance = observeAgentActivity(otherBrains[agentId], agentId);
            observationHistory[obsId].push_back(agentPerformance);
            
            // Record social connections
            if (agentId < 10) {  // Limit connections for computational efficiency
                socialConnections[obsId].push_back(NeuronId(agentId));
            }
        }
    }
    
    double observeAgentActivity(std::shared_ptr<Brain> brain, size_t agentId) {
        // Observe agent neural activity
        double firingRate = calculateAverageFiringRate(brain);
        double spikePattern = calculateSpikePattern(brain, agentId);
        double plasticityLevel = calculatePlasticityLevel(brain);
        
        // Combine observations into a single observation score
        return (firingRate * 0.4 + spikePattern * 0.3 + plasticityLevel * 0.3);
    }
    
    double calculateSpikePattern(std::shared_ptr<Brain> brain, size_t agentId) {
        // Calculate spike pattern characteristics
        double totalSpikes = brain->getTotalSpikeCount();
        double spikeFrequency = totalSpikes / 1000.0;  // Per second
        double patternRegularity = 1.0 - calculateSpikeVariability(brain, 1000);
        
        return spikeFrequency * patternRegularity;
    }
    
    double calculatePlasticityLevel(std::shared_ptr<Brain> brain) {
        // Calculate plasticity level of agent
        // This would require accessing synapse plasticity states
        return 0.7;  // Placeholder
    }
    
    void imitateObservations(size_t obsId) {
        // Imitate observed behaviors from other agents
        if (observationHistory[obsId].empty()) return;
        
        // Find the most successful observation
        double maxObservation = *std::max_element(observationHistory[obsId].begin(), observationHistory[obsId].end());
        size_t bestAgentIndex = std::distance(observationHistory[obsId].begin(),
                                             std::max_element(observationHistory[obsId].begin(), observationHistory[obsId].end()));
        
        // Imitate the best observed behavior
        imitateBestAgent(bestAgentIndex, maxObservation);
        
        // Calculate imitation accuracy
        double imitationAccuracy = calculateImitationAccuracy(obsId, bestAgentIndex);
        imitationAccuracy[obsId] = imitationAccuracy;
    }
    
    void imitateBestAgent(size_t agentIndex, double observationScore) {
        // Imitate the behavior of the best observed agent
        if (agentIndex < otherBrains.size()) {
            // Combine neural states with observed agent
            combineWithObservedAgent(otherBrains[agentIndex], observationScore);
        }
    }
    
    void combineWithObservedAgent(std::shared_ptr<Brain> observedAgent, double observationScore) {
        // Combine own neural state with observed agent
        double transferStrength = calculateSocialTransferStrength(observationScore);
        
        // Transfer synaptic weights and plasticity rules
        if (auto* ownRegion = ownBrain->getRegion(RegionId(1))) {
            if (auto* observedRegion = observedAgent->getRegion(RegionId(1))) {
                auto& ownSynapses = ownRegion->getSynapses();
                auto& observedSynapses = observedRegion->getSynapses();
                
                size_t minSize = std::min(ownSynapses.size(), observedSynapses.size());
                for (size_t i = 0; i < minSize; ++i) {
                    // Social transfer of synaptic weights
                    double transferredWeight = observedSynapses[i]->getWeight() * transferStrength;
                    ownSynapses[i]->setWeight(transferredWeight);
                    
                    // Social transfer of plasticity rules
                    ownSynapses[i]->enablePlasticity(true, true, true);
                }
            }
        }
    }
    
    double calculateSocialTransferStrength(double observationScore) {
        // Calculate strength of social knowledge transfer
        return 0.5 + 0.5 * observationScore;  // Stronger transfer for better observations
    }
    
    double calculateImitationAccuracy(size_t obsId, size_t agentIndex) {
        // Calculate how accurately behaviors are imitated
        double observationScore = observationHistory[obsId][agentIndex];
        double imitationEffectiveness = 1.0 - calculateSpikeVariability(ownBrain, 1000);
        
        return observationScore * imitationEffectiveness;
    }
    
    void testImitatedBehaviors(size_t obsId) {
        // Test the imitated behaviors in new situations
        for (size_t taskId = 0; taskId < 5; ++taskId) {
            testSocialImitation(taskId, obsId);
        }
    }
    
    void testSocialImitation(size_t taskId, size_t obsId) {
        // Test imitation on specific task
        for (size_t step = 0; step < 200; ++step) {
            // Task-specific input with social influence
            double socialInfluence = calculateSocialInfluence(obsId, step);
            double taskStrength = calculateTaskStrength(taskId, step);
            
            double inputStrength = taskStrength * socialInfluence;
            
            for (size_t i = 0; i < ownBrain->getTotalNeuronCount() / 20; ++i) {
                brain->injectCurrentToNeurons(static_cast<NeuronType>((i + obsId) % 3), inputStrength);
            }
            
            brain->step(step + 1000, (step + 1000) * 0.001);
        }
    }
    
    double calculateSocialInfluence(size_t obsId, size_t step) {
        // Calculate social influence based on observation quality and recency
        double observationQuality = observationHistory[obsId].empty() ? 0.0 : 
                                   *std::max_element(observationHistory[obsId].begin(), observationHistory[obsId].end());
        
        double recencyDecay = std::exp(-step * 0.01);  // Recency decay
        return observationQuality * recencyDecay;
    }
    
    double calculateTaskStrength(size_t taskId, size_t step) {
        // Calculate task-specific strength
        double taskComplexity = 0.5 + 0.5 * std::sin(step * 0.05 + taskId * 0.3);
        return 30.0 * taskComplexity;
    }
    
    void recordSocialLearningMetrics(size_t obsId) {
        // Record social learning metrics for current observation
        double socialEffectiveness = calculateSocialLearningEffectiveness(obsId);
        socialLearningEffect[obsId] = socialEffectiveness;
    }
    
    double calculateSocialLearningEffectiveness(size_t obsId) {
        // Calculate how effective the social learning was
        double observationAccuracy = imitationAccuracy[obsId];
        double imitationQuality = 1.0 - calculateSpikeVariability(ownBrain, 1000);
        double socialIntegration = calculateSocialIntegration(obsId);
        
        return (observationAccuracy * 0.4 + imitationQuality * 0.3 + socialIntegration * 0.3);
    }
    
    double calculateSocialIntegration(size_t obsId) {
        // Calculate how well social knowledge is integrated
        double totalIntegration = 0.0;
        for (size_t i = 0; i < observationHistory[obsId].size(); ++i) {
            totalIntegration += observationHistory[obsId][i];
        }
        
        return std::min(1.0, totalIntegration / observationHistory[obsId].size());
    }
    
    double calculateAverageFiringRate() {
        auto* region = ownBrain->getRegion(RegionId(1));
        if (!region) return 0.0;
        
        auto neurons = region->getAllNeurons();
        double totalFiringRate = 0.0;
        size_t activeNeurons = 0;
        
        for (auto* neuron : neurons) {
            if (neuron->isFiring()) {
                totalFiringRate += neuron->getFiringRate();
                ++activeNeurons;
            }
        }
        
        return activeNeurons > 0 ? totalFiringRate / activeNeurons : 0.0;
    }
    
    size_t calculateMaxSpikeCount() {
        return ownBrain->getTotalSpikeCount();  // Simplified
    }
    
    double calculateSpikeVariability() {
        // Calculate spike rate variability
        double totalSpikes = ownBrain->getTotalSpikeCount();
        double totalSteps = 10000;  // Would be calculated
        return totalSpikes / totalSteps * 0.6;  // Placeholder
    }
    
    double calculateSocialPlasticity() {
        // Calculate social learning plasticity effectiveness
        double totalSocialPlasticity = 0.0;
        for (double effect : socialLearningEffect) {
            totalSocialPlasticity += effect;
        }
        
        return std::min(1.0, totalSocialPlasticity / socialLearningEffect.size());
    }
    
    double calculateSocialInformationStorage() {
        // Calculate information storage from social learning
        double totalInformation = 0.0;
        for (size_t obsId = 0; obsId < observationHistory.size(); ++obsId) {
            for (double observation : observationHistory[obsId]) {
                totalInformation += observation;
            }
        }
        
        return std::min(1.0, totalInformation / (observationHistory.size() * 100.0));
    }
    
    double calculateSocialGeneralization() {
        // Calculate generalization ability from social learning
        double totalGeneralization = 0.0;
        for (size_t obsId = 0; obsId < observationHistory.size(); ++obsId) {
            for (double observation : observationHistory[obsId]) {
                totalGeneralization += observation * calculateSocialTransferStrength(observation);
            }
        }
        
        return std::min(1.0, totalGeneralization / 1000.0);
    }
    
    double calculateSocialAdaptationSpeed() {
        // Calculate adaptation speed in social learning
        double totalAdaptation = 0.0;
        for (size_t obsId = 0; obsId < observationHistory.size(); ++obsId) {
            for (size_t i = 1; i < observationHistory[obsId].size(); ++i) {
                totalAdaptation += std::abs(observationHistory[obsId][i] - observationHistory[obsId][i-1]);
            }
        }
        
        return std::min(1.0, totalAdaptation / 100.0);
    }
    
    double calculateSocialMemoryRetention() {
        // Calculate memory retention from social learning
        double totalRetention = 0.0;
        for (size_t obsId = 0; obsId < observationHistory.size(); ++obsId) {
            double avgObservation = 0.0;
            for (double observation : observationHistory[obsId]) {
                avgObservation += observation;
            }
            avgObservation /= observationHistory[obsId].size();
            totalRetention += avgObservation;
        }
        
        return std::min(1.0, totalRetention / observationHistory.size());
    }
    
    std::vector<double> calculateSocialLearningCurve() {
        // Calculate social learning curve over observations
        std::vector<double> curve;
        size_t maxObservations = 0;
        for (const auto& history : observationHistory) {
            maxObservations = std::max(maxObservations, (size_t)history.size());
        }
        
        for (size_t i = 0; i < maxObservations; ++i) {
            double avgSocialLearning = 0.0;
            size_t validObservations = 0;
            for (size_t obsId = 0; obsId < observationHistory.size(); ++obsId) {
                if (i < observationHistory[obsId].size()) {
                    avgSocialLearning += observationHistory[obsId][i];
                    ++validObservations;
                }
            }
            
            curve.push_back(validObservations > 0 ? avgSocialLearning / validObservations : 0.0);
        }
        
        return curve;
    }
    
    std::vector<double> calculateSocialStabilityMetrics() {
        // Calculate social learning stability metrics
        std::vector<double> metrics;
        for (size_t obsId = 0; obsId < observationHistory.size(); ++obsId) {
            double stability = 0.0;
            if (!observationHistory[obsId].empty()) {
                double variance = 0.0;
                double mean = 0.0;
                for (double observation : observationHistory[obsId]) {
                    mean += observation;
                }
                mean /= observationHistory[obsId].size();
                
                for (double observation : observationHistory[obsId]) {
                    variance += (observation - mean) * (observation - mean);
                }
                variance /= observationHistory[obsId].size();
                
                stability = 1.0 - std::sqrt(variance);
            }
            metrics.push_back(stability);
        }
        return metrics;
    }
    
    std::vector<double> calculateSocialPlasticityMetrics() {
        // Calculate social learning plasticity metrics
        std::vector<double> metrics;
        for (size_t obsId = 0; obsId < socialLearningEffect.size(); ++obsId) {
            metrics.push_back(socialLearningEffect[obsId]);
        }
        return metrics;
    }
};

// Advanced experiment runner
class AdvancedExperimentRunner {
public:
    AdvancedExperimentResult runMetaLearningExperiment(size_t neurons, size_t steps) {
        // Create brain
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neurons), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Run meta-learning experiment
        MetaLearningExperiment experiment(brain, 42);
        return experiment.run(steps);
    }
    
    AdvancedExperimentResult runTransferLearningExperiment(size_t neurons, size_t steps) {
        // Create source and target brains
        auto sourceConfig = std::make_shared<Config>();
        sourceConfig->set("neuron_count", static_cast<int64_t>(neurons), ConfigSource::Default);
        auto sourceBrain = std::make_shared<Brain>(sourceConfig);
        sourceBrain->initialize();
        
        auto targetConfig = std::make_shared<Config>();
        targetConfig->set("neuron_count", static_cast<int64_t>(neurons), ConfigSource::Default);
        auto targetBrain = std::make_shared<Brain>(targetConfig);
        targetBrain->initialize();
        
        // Define source tasks
        std::vector<std::string> sourceTasks = {"pattern_recognition", "sequence_learning", "spatial_reasoning"};
        
        // Run transfer learning experiment
        TransferLearningExperiment experiment(sourceBrain, targetBrain, 43);
        return experiment.run(steps, sourceTasks);
    }
    
    AdvancedExperimentResult runContinualLearningExperiment(size_t neurons, size_t stepsPerTask, size_t numTasks) {
        // Create brain
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neurons), ConfigSource::Default);
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Run continual learning experiment
        ContinualLearningExperiment experiment(brain, 44);
        return experiment.run(stepsPerTask, numTasks);
    }
    
    AdvancedExperimentResult runSocialLearningExperiment(size_t neurons, size_t steps, size_t numObservers) {
        // Create own brain and observer brains
        auto ownConfig = std::make_shared<Config>();
        ownConfig->set("neuron_count", static_cast<int64_t>(neurons), ConfigSource::Default);
        auto ownBrain = std::make_shared<Brain>(ownConfig);
        ownBrain->initialize();
        
        std::vector<std::shared_ptr<Brain>> observerBrains;
        for (size_t i = 0; i < numObservers; ++i) {
            auto observerConfig = std::make_shared<Config>();
            observerConfig->set("neuron_count", static_cast<int64_t>(neurons), ConfigSource::Default);
            auto observerBrain = std::make_shared<Brain>(observerConfig);
            observerBrain->initialize();
            observerBrains.push_back(observerBrain);
        }
        
        // Run social learning experiment
        SocialLearningExperiment experiment(ownBrain, observerBrains, 45);
        return experiment.run(steps, numObservers);
    }
};