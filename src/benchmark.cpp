// NLM Performance Benchmark
// Enhanced with comprehensive analysis, multiple test scenarios, and integration with logging system

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "experiments/Experiment.hpp"

#include <iostream>
#include <chrono>
#include <vector>
#include <iomanip>
#include <string>
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <numeric>

using namespace nlm;

// Enhanced performance measurement with more metrics
struct BenchmarkResult {
    std::string testName;
    size_t neurons;
    size_t synapses;
    size_t steps;
    double totalTime;              // seconds
    double stepsPerSecond;
    double spikesPerStep;
    double synapsesPerNeuron;
    double memoryUsage;           // MB (estimated)
    double energyEstimate;        // arbitrary units
    std::string performanceGrade;
    
    // Scenario-specific metrics
    double plasticitySpeed;       // synaptic changes per second
    double stabilityIndex;        // network stability measure
    double learningRate;          // adaptation per step
    std::unordered_map<std::string, double> scenarioMetrics;
};

// Benchmark configuration
struct BenchmarkConfig {
    std::string name;
    size_t neuronCount;
    size_t steps;
    LogLevel logLevel;
    std::string testScenario;     // "connectivity", "plasticity", "stdp", "learning", "mixed"
    bool enableDetailedMetrics;
    bool enableScalabilityTesting;
};

// Advanced benchmark class
class BenchmarkSuite {
private:
    std::vector<BenchmarkResult> results;
    std::vector<BenchmarkConfig> configs;
    LogLevel verboseLevel;
    
public:
    BenchmarkSuite() : verboseLevel(LogLevel::Info) {}
    
    void addBenchmark(const BenchmarkConfig& config) {
        configs.push_back(config);
        NLM_LOG_INFO("Added benchmark: " + config.name + " (" + 
                    std::to_string(config.neuronCount) + " neurons, " +
                    std::to_string(config.steps) + " steps, scenario: " +
                    config.testScenario + ")");
    }
    
    void runAllBenchmarks() {
        NLM_LOG_INFO("=== Starting Enhanced NLM Performance Benchmark Suite ===");
        
        for (const auto& config : configs) {
            NLM_LOG_INFO("");
            NLM_LOG_INFO("Running benchmark: " + config.name);
            NLM_LOG_INFO("Scenario: " + config.testScenario);
            
            BenchmarkResult result;
            if (config.testScenario == "connectivity") {
                result = runConnectivityBenchmark(config);
            } else if (config.testScenario == "plasticity") {
                result = runPlasticityBenchmark(config);
            } else if (config.testScenario == "stdp") {
                result = runStdpBenchmark(config);
            } else if (config.testScenario == "learning") {
                result = runLearningBenchmark(config);
            } else if (config.testScenario == "mixed") {
                result = runMixedBenchmark(config);
            } else {
                result = runConnectivityBenchmark(config);
            }
            
            result.testName = config.name;
            results.push_back(result);
            
            // Assign performance grade based on overall performance
            assignPerformanceGrade(result);
        }
        
        printComprehensiveReport();
    }
    
    void setVerboseLevel(LogLevel level) {
        verboseLevel = level;
    }
    
    const std::vector<BenchmarkResult>& getResults() const {
        return results;
    }
    
    BenchmarkResult runConnectivityBenchmark(const BenchmarkConfig& config) {
        NLM_LOG_INFO("  Scenario: Basic Neural Connectivity Test");
        
        auto benchmarkStart = std::chrono::high_resolution_clock::now();
        
        // Create configuration
        auto configPtr = std::make_shared<Config>();
        configPtr->set("neuron_count", static_cast<int64_t>(config.neuronCount), ConfigSource::Default);
        configPtr->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        configPtr->set("connection_probability", 0.15f, ConfigSource::Default);
        
        // Create brain
        auto brain = std::make_shared<Brain>(configPtr);
        brain->initialize();
        
        // Run connectivity test
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        for (size_t step = 0; step < config.steps; ++step) {
            // Inject input to test propagation
            for (size_t i = 0; i < std::min(size_t(20), config.neuronCount / 20); ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 40.0f);
            }
            
            brain->step(step, step * 0.001);
            
            if (config.enableDetailedMetrics && step % 100 == 0) {
                NLM_LOG_DEBUG("    Step " + std::to_string(step) + ": " +
                             std::to_string(brain->getFiringNeuronCount()) + " firing neurons");
            }
        }
        
        auto benchmarkEnd = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(benchmarkEnd - benchmarkStart).count();
        
        BenchmarkResult result;
        result.neurons = brain->getTotalNeuronCount();
        result.synapses = brain->getTotalSynapseCount();
        result.steps = config.steps;
        result.totalTime = totalTime;
        result.stepsPerSecond = config.steps / totalTime;
        
        size_t finalSpikes = brain->getTotalSpikeCount();
        result.spikesPerStep = static_cast<double>(finalSpikes - initialSpikes) / config.steps;
        result.synapsesPerNeuron = static_cast<double>(result.synapses) / result.neurons;
        
        // Estimate memory usage (rough approximation)
        result.memoryUsage = (result.neurons * 100.0 + result.synapses * 50.0) / (1024 * 1024);  // MB
        
        // Calculate connectivity metric
        double connectivity = result.synapsesPerNeuron;
        if (connectivity > 0.5) result.scenarioMetrics["connectivity"] = 0.9;  // Excellent
        else if (connectivity > 0.3) result.scenarioMetrics["connectivity"] = 0.7;  // Good
        else if (connectivity > 0.1) result.scenarioMetrics["connectivity"] = 0.5;  // Moderate
        else result.scenarioMetrics["connectivity"] = 0.3;  // Poor
        
        NLM_LOG_INFO("  Connectivity benchmark completed: " + std::to_string(result.stepsPerSecond) + " steps/sec");
        return result;
    }
    
    BenchmarkResult runPlasticityBenchmark(const BenchmarkConfig& config) {
        NLM_LOG_INFO("  Scenario: Plasticity Learning Test");
        
        auto benchmarkStart = std::chrono::high_resolution_clock::now();
        
        auto configPtr = std::make_shared<Config>();
        configPtr->set("neuron_count", static_cast<int64_t>(config.neuronCount), ConfigSource::Default);
        configPtr->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        configPtr->set("connection_probability", 0.1f, ConfigSource::Default);
        configPtr->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        configPtr->set("stdp_ltd_weight", 0.018f, ConfigSource::Default);
        configPtr->set("synaptogenesis_rate", 0.00005f, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(configPtr);
        brain->initialize();
        
        // Enable plasticity
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        // Run plasticity learning
        size_t initialSpikes = brain->getTotalSpikeCount();
        size_t initialSynapses = brain->getTotalSynapseCount();
        
        std::vector<size_t> spikeCounts;
        std::vector<size_t> synapseCounts;
        
        for (size_t step = 0; step < config.steps; ++step) {
            // Apply learning input pattern
            std::vector<NeuronId> patternNeurons;
            if (auto* region = brain->getRegion(RegionId(1))) {
                for (auto* neuron : region->getAllNeurons()) {
                    if (neuron->getType() == NeuronType::Sensory && patternNeurons.size() < 20) {
                        patternNeurons.push_back(neuron->getId());
                    }
                }
            }
            
            for (NeuronId id : patternNeurons) {
                brain->injectCurrent(id, 35.0f);
            }
            
            brain->step(step, step * 0.001);
            
            spikeCounts.push_back(brain->getTotalSpikeCount() - initialSpikes);
            synapseCounts.push_back(brain->getTotalSynapseCount() - initialSynapses);
            
            if (config.enableDetailedMetrics && step % 200 == 0) {
                NLM_LOG_DEBUG("    Step " + std::to_string(step) + ": " +
                             std::to_string(spikeCounts.back()) + " spikes, " +
                             std::to_string(synapseCounts.back()) + " new synapses");
            }
        }
        
        auto benchmarkEnd = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(benchmarkEnd - benchmarkStart).count();
        
        BenchmarkResult result;
        result.neurons = brain->getTotalNeuronCount();
        result.synapses = brain->getTotalSynapseCount();
        result.steps = config.steps;
        result.totalTime = totalTime;
        result.stepsPerSecond = config.steps / totalTime;
        
        size_t finalSpikes = brain->getTotalSpikeCount();
        result.spikesPerStep = static_cast<double>(finalSpikes - initialSpikes) / config.steps;
        result.synapsesPerNeuron = static_cast<double>(result.synapses) / result.neurons;
        
        // Calculate plasticity metrics
        if (!spikeCounts.empty()) {
            double avgSpikeChange = calculateMean(spikeCounts);
            result.scenarioMetrics["plasticity_stability"] = calculateStability(spikeCounts);
            result.scenarioMetrics["plasticity_efficiency"] = avgSpikeChange / result.stepsPerSecond;
        }
        
        // Estimate learning rate
        result.learningRate = (result.synapses - initialSynapses) / (result.steps * result.totalTime);
        
        NLM_LOG_INFO("  Plasticity benchmark completed: " + std::to_string(result.learningRate) + " synapses learned/sec");
        return result;
    }
    
    BenchmarkResult runStdpBenchmark(const BenchmarkConfig& config) {
        NLM_LOG_INFO("  Scenario: STDP Verification Test");
        
        auto benchmarkStart = std::chrono::high_resolution_clock::now();
        
        auto configPtr = std::make_shared<Config>();
        configPtr->set("neuron_count", static_cast<int64_t>(config.neuronCount), ConfigSource::Default);
        configPtr->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        configPtr->set("connection_probability", 0.2f, ConfigSource::Default);
        configPtr->set("stdp_ltp_weight", 0.03f, ConfigSource::Default);
        configPtr->set("stdp_ltd_weight", 0.025f, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(configPtr);
        brain->initialize();
        
        // Enable STDP only
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(false, false, true);
            }
        }
        
        // Get reference synapse for STDP measurement
        Synapse* stdpSyn = nullptr;
        if (auto* region = brain->getRegion(RegionId(1))) {
            auto& synapses = region->getSynapses();\n            if (!synapses.empty()) {
                stdpSyn = synapses[0].get();
            }
        }
        
        if (stdpSyn) {
            float initialWeight = stdpSyn->getWeight();
            
            // Apply STDP stimulation
            for (int trial = 0; trial < 50; ++trial) {
                brain->injectCurrent(stdpSyn->getSourceNeuron(), 60.0f);
                brain->step(trial * 2, trial * 2 * 0.001);
                brain->injectCurrent(stdpSyn->getDestinationNeuron(), 60.0f);
                brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            }
            
            float finalWeight = stdpSyn->getWeight();
            float weightChange = finalWeight - initialWeight;
            result.scenarioMetrics["stdp_potentiation"] = (weightChange > 0) ? weightChange : 0;
            result.scenarioMetrics["stdp_depression"] = (weightChange < 0) ? std::abs(weightChange) : 0;
        }
        
        auto benchmarkEnd = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(benchmarkEnd - benchmarkStart).count();
        
        BenchmarkResult result;
        result.neurons = brain->getTotalNeuronCount();
        result.synapses = brain->getTotalSynapseCount();
        result.steps = config.steps;
        result.totalTime = totalTime;
        result.stepsPerSecond = config.steps / totalTime;
        
        size_t initialSpikes = brain->getTotalSpikeCount();
        brain->step(0, 0.001);  // Final step
        size_t finalSpikes = brain->getTotalSpikeCount();
        result.spikesPerStep = static_cast<double>(finalSpikes - initialSpikes) / config.steps;
        result.synapsesPerNeuron = static_cast<double>(result.synapses) / result.neurons;
        
        NLM_LOG_INFO("  STDP benchmark completed: weight change = " + std::to_string(
            (result.scenarioMetrics.find("stdp_potentiation") != result.scenarioMetrics.end() ? 
             result.scenarioMetrics["stdp_potentiation"] : 0.0)) + " (potentiation) / " +
            std::to_string((result.scenarioMetrics.find("stdp_depression") != result.scenarioMetrics.end() ? 
                          result.scenarioMetrics["stdp_depression"] : 0.0)) + " (depression)");
        
        return result;
    }
    
    BenchmarkResult runLearningBenchmark(const BenchmarkConfig& config) {
        NLM_LOG_INFO("  Scenario: Learning Efficiency Test");
        
        auto benchmarkStart = std::chrono::high_resolution_clock::now();
        
        auto configPtr = std::make_shared<Config>();
        configPtr->set("neuron_count", static_cast<int64_t>(config.neuronCount), ConfigSource::Default);
        configPtr->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        configPtr->set("connection_probability", 0.1f, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(configPtr);
        brain->initialize();
        
        // Enable all plasticity for learning
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        // Run learning task with reward feedback
        size_t initialSpikes = brain->getTotalSpikeCount();
        float initialPerformance = calculatePerformance(brain);
        
        for (size_t step = 0; step < config.steps; ++step) {
            // Simulate learning task with varying difficulty
            float taskDifficulty = 0.3f + 0.4f * (static_cast<float>(step) / config.steps);
            
            // Apply input based on task
            for (size_t i = 0; i < std::min(size_t(15), config.neuronCount / 15); ++i) {
                float inputStrength = 30.0f + 20.0f * taskDifficulty;
                brain->injectCurrentToNeurons(NeuronType::Sensory, inputStrength);
            }
            
            brain->step(step, step * 0.001);
            
            if (config.enableDetailedMetrics && step % 100 == 0) {
                float currentPerformance = calculatePerformance(brain);
                float improvement = currentPerformance - initialPerformance;
                NLM_LOG_DEBUG("    Step " + std::to_string(step) + ": performance = " +
                             std::to_string(currentPerformance) + ", improvement = " +
                             std::to_string(improvement));
            }
        }
        
        auto benchmarkEnd = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(benchmarkEnd - benchmarkStart).count();
        
        BenchmarkResult result;
        result.neurons = brain->getTotalNeuronCount();
        result.synapses = brain->getTotalSynapseCount();
        result.steps = config.steps;
        result.totalTime = totalTime;
        result.stepsPerSecond = config.steps / totalTime;
        
        size_t finalSpikes = brain->getTotalSpikeCount();
        result.spikesPerStep = static_cast<double>(finalSpikes - initialSpikes) / config.steps;
        result.synapsesPerNeuron = static_cast<double>(result.synapses) / result.neurons;
        
        // Calculate learning metrics
        float finalPerformance = calculatePerformance(brain);
        float learningEfficiency = (finalPerformance - initialPerformance) / totalTime;
        result.scenarioMetrics["learning_efficiency"] = learningEfficiency;
        result.scenarioMetrics["performance_gain"] = finalPerformance - initialPerformance;
        result.learningRate = learningEfficiency;
        
        NLM_LOG_INFO("  Learning benchmark completed: efficiency = " + std::to_string(learningEfficiency) + " per second");
        return result;
    }
    
    BenchmarkResult runMixedBenchmark(const BenchmarkConfig& config) {
        NLM_LOG_INFO("  Scenario: Mixed Workload Test");
        
        auto benchmarkStart = std::chrono::high_resolution_clock::now();
        
        auto configPtr = std::make_shared<Config>();
        configPtr->set("neuron_count", static_cast<int64_t>(config.neuronCount), ConfigSource::Default);
        configPtr->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        configPtr->set("connection_probability", 0.12f, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(configPtr);
        brain->initialize();
        
        // Enable all plasticity
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        // Simulate mixed workload with different phases
        std::vector<std::string> phases = {"initialization", "exploration", "exploitation", "refinement"};
        size_t phaseDuration = config.steps / phases.size();
        
        for (size_t phase = 0; phase < phases.size(); ++phase) {
            NLM_LOG_INFO("    Phase " + std::to_string(phase + 1) + "/" + std::to_string(phases.size()) +
                        ": " + phases[phase]);
            
            for (size_t step = 0; step < phaseDuration; ++step) {
                // Vary stimulation based on phase
                float baseInput = 25.0f + static_cast<float>(phase) * 10.0f;
                
                for (size_t i = 0; i < std::min(size_t(10), config.neuronCount / 20); ++i) {
                    brain->injectCurrentToNeurons(NeuronType::Sensory, baseInput);
                }
                
                brain->step(step + phase * phaseDuration, (step + phase * phaseDuration) * 0.001);
            }
        }
        
        auto benchmarkEnd = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(benchmarkEnd - benchmarkStart).count();
        
        BenchmarkResult result;
        result.neurons = brain->getTotalNeuronCount();
        result.synapses = brain->getTotalSynapseCount();
        result.steps = config.steps;
        result.totalTime = totalTime;
        result.stepsPerSecond = config.steps / totalTime;
        
        size_t initialSpikes = brain->getTotalSpikeCount();
        brain->step(0, 0.001);
        size_t finalSpikes = brain->getTotalSpikeCount();
        result.spikesPerStep = static_cast<double>(finalSpikes - initialSpikes) / config.steps;
        result.synapsesPerNeuron = static_cast<double>(result.synapses) / result.neurons;
        
        // Calculate mixed workload metrics
        result.scenarioMetrics["workload_balancing"] = calculateWorkloadBalancing(brain, phases);
        result.scenarioMetrics["adaptation_speed"] = calculateAdaptationSpeed(brain, config.steps);
        
        NLM_LOG_INFO("  Mixed benchmark completed: " + phases.size() + " phases processed successfully");
        return result;
    }
    
    void assignPerformanceGrade(BenchmarkResult& result) {
        // Simple grading based on performance metrics
        double score = 0.0;
        
        // Steps per second (0-50)
        score += std::min(25.0, result.stepsPerSecond / 1000.0);
        
        // Spike efficiency (0-25)
        double spikeEfficiency = std::min(1.0, result.spikesPerStep / 5.0);
        score += spikeEfficiency * 25.0;
        
        // Synapse efficiency (0-25)
        double synapseEfficiency = std::min(1.0, result.synapsesPerNeuron / 10.0);
        score += synapseEfficiency * 25.0;
        
        // Scale to letter grade
        if (score >= 75) result.performanceGrade = "A";
        else if (score >= 60) result.performanceGrade = "B";
        else if (score >= 45) result.performanceGrade = "C";
        else if (score >= 30) result.performanceGrade = "D";
        else result.performanceGrade = "F";
    }
    
    void printComprehensiveReport() {
        std::cout << std::endl;
        std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                      Enhanced NLM Performance Benchmark Suite                         ║" << std::endl;
        std::cout << "╠══════════════╦═══════════════════════════╦═══════════╦═══════════╦═══════════╦════════════╣" << std::endl;
        std::cout << "║    Test     ║   Scenario           ║ Neurons   ║ Steps/sec ║ Spikes/step ║ Performance ║" << std::endl;
        std::cout << "╠══════════════╬═══════════════════════════╬═══════════╬═══════════╬═══════════╬════════════╣" << std::endl;
        
        for (const auto& r : results) {
            std::cout << "║ " << std::setw(12) << r.testName 
                      << " ║ " << std::setw(23) << getScenarioName(r.testName)
                      << " ║ " << std::setw(9) << r.neurons 
                      << " ║ " << std::setw(9) << std::fixed << std::setprecision(0) << r.stepsPerSecond
                      << " ║ " << std::setw(11) << std::fixed << std::setprecision(2) << r.spikesPerStep
                      << " ║ " << std::setw(12) << r.performanceGrade << " ║" << std::endl;
        }
        
        std::cout << "╚══════════════╩═══════════════════════════╩═══════════╩═══════════╩═══════════╩════════════╝" << std::endl;
        std::cout << std::endl;
        
        // Detailed results
        std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                              Detailed Performance Analysis                             ║" << std::endl;
        std::cout << "╠══════════════╦══════════════════════════════════════════════════════════════════════════╣" << std::endl;
        
        for (const auto& r : results) {
            std::cout << "║ " << std::setw(12) << r.testName << " ║\n";
            
            std::cout << "║   Network Configuration: " << r.neurons << " neurons, " << r.synapses << " synapses\n";
            std::cout << "║   Performance: " << std::fixed << std::setprecision(1) << r.stepsPerSecond << " steps/sec, "
                      << r.spikesPerStep << " spikes/step, " << r.synapsesPerNeuron << " synapses/neuron\n";
            
            if (!r.scenarioMetrics.empty()) {
                std::cout << "║   Scenario Metrics:";
                for (const auto& [key, value] : r.scenarioMetrics) {
                    std::cout << "\n║     " << key << ": " << std::fixed << std::setprecision(4) << value;
                }
                std::cout << "\n";
            }
            
            std::cout << "║\n";
        }
        
        std::cout << "╚══════════════╩══════════════════════════════════════════════════════════════════════════╝" << std::endl;
        std::cout << std::endl;
        
        // Summary statistics
        std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                              Summary Statistics                                   ║" << std::endl;
        std::cout << "╠══════════════╦══════════════════════════════════════════════════════════════════════════╣" << std::endl;
        
        double avgStepsPerSecond = calculateMean(results, [](const BenchmarkResult& r) { return r.stepsPerSecond; });
        double avgSpikesPerStep = calculateMean(results, [](const BenchmarkResult& r) { return r.spikesPerStep; });\n        double avgSynapsesPerNeuron = calculateMean(results, [](const BenchmarkResult& r) { return r.synapsesPerNeuron; });
        
        std::cout << "║   Average Performance:                          ║\n";
        std::cout << "║   • Steps/sec: " << std::fixed << std::setprecision(1) << avgStepsPerSecond << "                    ║\n";
        std::cout << "║   • Spikes/step: " << std::fixed << std::setprecision(2) << avgSpikesPerStep << "               ║\n";
        std::cout << "║   • Synapses/neuron: " << std::fixed << std::setprecision(2) << avgSynapsesPerNeuron << "          ║\n";
        
        std::cout << "╚══════════════╩══════════════════════════════════════════════════════════════════════════╝" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Benchmark Suite Complete. Processed " << results.size() << " test configurations." << std::endl;
    }
    
    double calculateMean(const std::vector<BenchmarkResult>& results, 
                         std::function<double(const BenchmarkResult&)> getter) const {
        if (results.empty()) return 0.0;
        double sum = 0.0;
        for (const auto& r : results) {
            sum += getter(r);
        }
        return sum / results.size();
    }
    
    double calculateStability(const std::vector<size_t>& values) const {
        if (values.size() <= 1) return 1.0;
        double mean = calculateMean(values);
        double variance = 0.0;
        for (size_t v : values) {
            variance += std::pow(static_cast<double>(v) - mean, 2);
        }
        variance /= values.size();
        double stdDev = std::sqrt(variance);
        return 1.0 / (1.0 + stdDev);  // Higher stability = lower std dev
    }
    
    double calculateWorkloadBalancing(const std::shared_ptr<Brain>& brain, const std::vector<std::string>& phases) const {
        // Simple metric based on final spike count distribution
        size_t totalSpikes = brain->getTotalSpikeCount();
        double balanceScore = 1.0 / phases.size();  // Equal contribution from each phase
        return balanceScore * totalSpikes / 1000.0;  // Scale to reasonable range
    }
    
    double calculateAdaptationSpeed(const std::shared_ptr<Brain>& brain, size_t totalSteps) const {
        // Estimate adaptation based on current spike activity
        double firingRate = brain->getAverageFiringRate();
        return firingRate / 10.0;  // Normalize to 0-1 range
    }
    
    float calculatePerformance(const std::shared_ptr<Brain>& brain) const {
        // Simple performance metric combining firing rate and activity
        float firingRate = brain->getAverageFiringRate();
        size_t spikeCount = brain->getTotalSpikeCount();
        size_t neuronCount = brain->getTotalNeuronCount();
        
        // Normalize to 0-1 range
        float normalizedFiring = std::min(1.0f, firingRate / 100.0f);
        float normalizedActivity = std::min(1.0f, static_cast<float>(spikeCount) / (neuronCount * 10.0f));
        
        return (normalizedFiring + normalizedActivity) / 2.0f;
    }
    
    std::string getScenarioName(const std::string& testName) const {
        // Extract scenario from test name or default
        if (testName.find("plasticity") != std::string::npos) return "Plasticity Learning";
        else if (testName.find("stdp") != std::string::npos) return "STDP Verification";
        else if (testName.find("learning") != std::string::npos) return "Learning Efficiency";
        else if (testName.find("mixed") != std::string::npos) return "Mixed Workload";
        else return "Neural Connectivity";
    }
};

int main() {
    // Initialize enhanced logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — Enhanced Performance Benchmark Suite                   ║
    ║     Comprehensive Neural Network Performance Testing              ║
    ║                                                               ║
    ║     Comprehensive benchmarking with multiple scenarios:         ║
    ║     • Connectivity Tests                                       ║
    ║     • Plasticity Learning Tests                                ║
    ║     • STDP Verification Tests                                 ║
    ║     • Learning Efficiency Tests                               ║
    ║     • Mixed Workload Tests                                   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════════════════════════════════════╝
    )" << std::endl;
    
    NLM_LOG_INFO("Starting Enhanced NLM Performance Benchmark Suite");
    NLM_LOG_INFO("Build Configuration: Comprehensive multi-scenario benchmarking");
    
    // Create benchmark suite
    BenchmarkSuite suite;
    
    // Configure benchmarks with different scenarios
    std::vector<BenchmarkConfig> configs = {
        {"Basic Connectivity", 100, 1000, LogLevel::Info, "connectivity", true, true},
        {"Small Network Plasticity", 200, 1500, LogLevel::Info, "plasticity", true, true},
        {"Medium Network STDP", 500, 800, LogLevel::Info, "stdp", true, true},
        {"Learning Efficiency", 300, 2000, LogLevel::Info, "learning", true, true},
        {"Mixed Workload", 400, 1200, LogLevel::Info, "mixed", true, true},
        {"Large Network Connectivity", 1000, 500, LogLevel::Info, "connectivity", true, false},
        {"Scalability Test 1", 1500, 300, LogLevel::Info, "connectivity", false, true},
        {"Scalability Test 2", 2000, 200, LogLevel::Info, "connectivity", false, true},
    };
    
    for (const auto& config : configs) {
        suite.addBenchmark(config);
    }
    
    // Run all benchmarks
    suite.runAllBenchmarks();
    
    NLM_LOG_INFO("Benchmark Suite Complete. Results saved to console output.");
    
    return 0;
}

} // namespace nlm