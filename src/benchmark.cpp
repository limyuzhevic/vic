// NLM Performance Benchmark
// Comprehensive performance analysis with detailed metrics

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "memory/WorkingMemory.hpp"
#include "prediction/PredictionSystem.hpp"

#include <iostream>
#include <chrono>
#include <vector>
#include <iomanip>
#include <string>
#include <algorithm>
#include <numeric>
#include <map>
#include <cmath>
#include <fstream>
#include <random>

using namespace nlm;

// Comprehensive benchmark configuration
struct BenchmarkConfig {
    size_t minNeurons = 100;
    size_t maxNeurons = 5000;
    size_t neuronStep = 500;
    size_t baseSteps = 1000;
    float inputCurrent = 30.0f;
    std::vector<std::string> enabledMetrics = {
        "neurons", "synapses", "spikes", "time", 
        "plasticity", "memory", "prediction", "energy"
    };
    bool enableDetailedAnalysis = true;
    bool enableEnergyMetrics = true;
    bool enableNeuralStats = true;
    bool enablePlasticityStats = true;
    bool enableMemoryStats = true;
    bool enablePredictionStats = true;
    bool enableJSONOutput = false;
    bool enableCSVOutput = false;
    std::string jsonOutputPath = "benchmark_results.json";
    std::string csvOutputPath = "benchmark_results.csv";
};

// Comprehensive benchmark results
struct BenchmarkResult {
    size_t neurons;
    size_t synapses;
    size_t steps;
    double totalTime;          // seconds
    double stepsPerSecond;
    double spikesPerStep;
    double synapsesPerNeuron;
    double avgFiringRate;      // Hz
    double maxFiringRate;
    double avgMembranePotential;
    double avgThreshold;
    double excitationInhibitionRatio;
    double plasticityEffect;
    double memoryUtilization;
    double predictionAccuracy;
    double energyConsumption;  // estimated joules
    double memoryUsage;        // MB
    size_t activeNeurons;
    size_t firingNeurons;
    size_t synapticConnections;
    std::map<std::string, double> customMetrics;
    
    // Helper method to calculate performance score
    double calculatePerformanceScore() const {
        double score = 0.0;
        if (stepsPerSecond > 0) score += std::log10(stepsPerSecond) * 10.0;
        score += spikesPerStep * 5.0;
        score += synapsesPerNeuron * 2.0;
        score += avgFiringRate * 0.1;
        if (energyConsumption > 0) score -= energyConsumption / 100.0;
        return score;
    }
};

// Statistics helper functions
struct Statistics {
    static double calculateMean(const std::vector<double>& values) {
        if (values.empty()) return 0.0;
        double sum = std::accumulate(values.begin(), values.end(), 0.0);
        return sum / values.size();
    }
    
    static double calculateVariance(const std::vector<double>& values, double mean) {
        if (values.size() <= 1) return 0.0;
        double sum = 0.0;
        for (double val : values) {
            sum += (val - mean) * (val - mean);
        }
        return sum / (values.size() - 1);
    }
    
    static double calculateStdDev(const std::vector<double>& values) {
        double mean = calculateMean(values);
        double variance = calculateVariance(values, mean);
        return std::sqrt(variance);
    }
    
    static double calculatePercentile(const std::vector<double>& values, double percentile) {
        if (values.empty()) return 0.0;
        size_t index = static_cast<size_t>(percentile / 100.0 * (values.size() - 1));
        std::vector<double> sorted = values;
        std::sort(sorted.begin(), sorted.end());
        return sorted[index];
    }
};

// Neural system performance metrics collector
class NeuralMetricsCollector {
public:
    struct Metrics {
        double avgFiringRate;
        double maxFiringRate;
        double avgMembranePotential;
        double avgThreshold;
        double excitationInhibitionRatio;
        size_t activeNeurons;
        size_t firingNeurons;
        double totalCurrent;
        size_t neuronsExceedingThreshold;
    };
    
    static Metrics collectMetrics(std::shared_ptr<Brain> brain, double timestep) {
        Metrics metrics = {0.0, 0.0, 0.0, 0.0, 1.0, 0, 0, 0.0, 0};
        
        if (!brain) return metrics;
        
        auto neurons = brain->getAllNeurons();
        if (neurons.empty()) return metrics;
        
        double totalFiringRate = 0.0;
        double totalMembranePotential = 0.0;
        double totalThreshold = 0.0;
        double totalCurrent = 0.0;
        size_t activeCount = 0;
        size_t firingCount = 0;
        size_t exceedingCount = 0;
        
        for (const auto& neuron : neurons) {
            totalFiringRate += neuron->getFiringRate();
            totalMembranePotential += neuron->getMembranePotential();
            totalThreshold += neuron->getThreshold();
            totalCurrent += neuron->getTotalCurrent();
            
            if (neuron->isFiring()) firingCount++;
            if (neuron->getMembranePotential() > neuron->getThreshold()) {
                exceedingCount++;
            }
            
            if (std::abs(neuron->getTotalCurrent()) > 0.1f) {
                activeCount++;
            }
        }
        
        size_t neuronCount = neurons.size();
        metrics.avgFiringRate = totalFiringRate / neuronCount;
        metrics.maxFiringRate = calculateMaxFiringRate(neurons);
        metrics.avgMembranePotential = totalMembranePotential / neuronCount;
        metrics.avgThreshold = totalThreshold / neuronCount;
        metrics.excitationInhibitionRatio = calculateExcitationInhibitionRatio(brain);
        metrics.activeNeurons = activeCount;
        metrics.firingNeurons = firingCount;
        metrics.totalCurrent = totalCurrent;
        metrics.neuronsExceedingThreshold = exceedingCount;
        
        return metrics;
    }
    
private:
    static double calculateMaxFiringRate(const std::vector<Neuron*>& neurons) {
        double maxRate = 0.0;
        for (const auto& neuron : neurons) {
            double rate = neuron->getFiringRate();
            if (rate > maxRate) maxRate = rate;
        }
        return maxRate;
    }
    
    static double calculateExcitationInhibitionRatio(std::shared_ptr<Brain> brain) {
        if (!brain) return 1.0;
        
        double excitation = 0.0;
        double inhibition = 0.0;
        
        auto neurons = brain->getAllNeurons();
        for (const auto& neuron : neurons) {
            double current = neuron->getTotalCurrent();
            if (current > 0.0f) {
                excitation += current;
            } else if (current < 0.0f) {
                inhibition -= current;
            }
        }
        
        if (inhibition > 0.0) {
            return excitation / inhibition;
        }
        return (excitation > 0.0) ? 1000.0 : 1.0;
    }
};

// Additional utility functions
namespace BenchmarkUtils {
    double calculatePlasticityEffect(std::shared_ptr<Brain> brain) {
        if (!brain) return 0.0;
        
        double totalWeightChange = 0.0;
        auto neurons = brain->getAllNeurons();
        for (const auto& neuron : neurons) {
            totalWeightChange += neuron->getTotalCurrent() * 0.01;
        }
        
        return std::min(1.0, std::abs(totalWeightChange) / 100.0);
    }
    
    double calculateMemoryUtilization(std::shared_ptr<Brain> brain) {
        if (!brain) return 0.0;
        
        size_t totalMemory = brain->getTotalNeuronCount() * 100; // Estimated bytes per neuron
        size_t usedMemory = brain->getFiringNeuronCount() * 50;  // Estimated bytes per firing neuron
        
        if (totalMemory > 0) {
            return static_cast<double>(usedMemory) / totalMemory;
        }
        return 0.0;
    }
    
    double calculatePredictionAccuracy(std::shared_ptr<Brain> brain) {
        if (!brain) return 0.0;
        
        // Simplified prediction accuracy calculation
        double accuracy = 0.0;
        auto neurons = brain->getAllNeurons();
        for (const auto& neuron : neurons) {
            accuracy += neuron->getFiringRate() * 0.001;
        }
        
        return std::min(1.0, accuracy);
    }
    
    double estimateEnergyConsumption(std::shared_ptr<Brain> brain, double totalTime) {
        if (!brain) return 0.0;
        
        // Simplified energy consumption estimation
        double basePower = 1.0;  // Watts
        double activityMultiplier = 2.0;
        double neuronCount = brain->getTotalNeuronCount();
        
        double power = basePower + (neuronCount * 0.001) * activityMultiplier;
        return power * totalTime;  // Joules
    }
    
    double estimateMemoryUsage(std::shared_ptr<Brain> brain) {
        if (!brain) return 0.0;
        
        // Simplified memory usage estimation
        double neuronMemory = 100.0;  // Bytes per neuron
        double synapseMemory = 50.0;  // Bytes per synapse
        double neuronCount = brain->getTotalNeuronCount();
        double synapseCount = brain->getTotalSynapseCount();
        
        return (neuronMemory * neuronCount + synapseMemory * synapseCount) / (1024.0 * 1024.0);  // MB
    }
    
    void exportJSONResults(const std::vector<BenchmarkResult>& results, const std::string& filepath) {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open JSON file for writing: " << filepath << std::endl;
            return;
        }
        
        file << "{\n";
        file << "  \"benchmarks\": [\n";
        for (size_t i = 0; i < results.size(); ++i) {
            const auto& result = results[i];
            file << "    {\n";
            file << "      \"neurons\": " << result.neurons << ",\n";
            file << "      \"synapses\": " << result.synapses << ",\n";
            file << "      \"steps\": " << result.steps << ",\n";
            file << "      \"totalTime\": " << std::fixed << std::setprecision(3) << result.totalTime << ",\n";
            file << "      \"stepsPerSecond\": " << std::fixed << std::setprecision(0) << result.stepsPerSecond << ",\n";
            file << "      \"spikesPerStep\": " << std::fixed << std::setprecision(3) << result.spikesPerStep << ",\n";
            file << "      \"avgFiringRate\": " << std::fixed << std::setprecision(3) << result.avgFiringRate << ",\n";
            file << "      \"performanceScore\": " << std::fixed << std::setprecision(3) << result.calculatePerformanceScore() << "\n";
            file << "    }";
            if (i < results.size() - 1) file << ",\n";
            else file << "\n";
        }
        file << "  ],\n";
        file << "  \"summary\": {\n";
        file << "    \"avgStepsPerSecond\": " << std::fixed << std::setprecision(0) << BenchmarkUtils::calculateAverageStepsPerSecond(results) << ",\n";
        file << "    \"avgSpikesPerStep\": " << std::fixed << std::setprecision(3) << BenchmarkUtils::calculateAverageSpikesPerStep(results) << ",\n";
        file << "    \"bestPerformance\": " << std::fixed << std::setprecision(3) << BenchmarkUtils::findBestPerformance(results).calculatePerformanceScore() << "\n";
        file << "  }\n";
        file << "}\n";
        
        file.close();
    }
    
    void exportCSVResults(const std::vector<BenchmarkResult>& results, const std::string& filepath) {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open CSV file for writing: " << filepath << std::endl;
            return;
        }
        
        // Header
        file << "Neurons,Synapses,Steps,TotalTime(s),StepsPerSecond,SpikesPerStep,AvgFiringRate,Hz,MaxFiringRate,MembranePotential(Threshold),ExcitationInhibitionRatio,PlasticityEffect,MemoryUtilization,PredictionAccuracy,EnergyConsumption(J),MemoryUsage(MB),ActiveNeurons,FiringNeurons,PerformanceScore\n";
        
        // Data rows
        for (const auto& result : results) {
            file << result.neurons << ",";
            file << result.synapses << ",";
            file << result.steps << ",";
            file << std::fixed << std::setprecision(3) << result.totalTime << ",";
            file << std::fixed << std::setprecision(0) << result.stepsPerSecond << ";";
            file << std::fixed << std::setprecision(3) << result.spikesPerStep << ";";
            file << std::fixed << std::setprecision(3) << result.avgFiringRate << ";";
            file << std::fixed << std::setprecision(3) << result.maxFiringRate << ";";
            file << std::fixed << std::setprecision(3) << result.avgMembranePotential << ";";
            file << std::fixed << std::setprecision(3) << result.avgThreshold << ";";
            file << std::fixed << std::setprecision(3) << result.excitationInhibitionRatio << ";";
            file << std::fixed << std::setprecision(3) << result.plasticityEffect << ";";
            file << std::fixed << std::setprecision(3) << result.memoryUtilization << ";";
            file << std::fixed << std::setprecision(3) << result.predictionAccuracy << ";";
            file << std::fixed << std::setprecision(3) << result.energyConsumption << ";";
            file << std::fixed << std::setprecision(3) << result.memoryUsage << ";";
            file << result.activeNeurons << ";";
            file << result.firingNeurons << ";";
            file << std::fixed << std::setprecision(3) << result.calculatePerformanceScore() << "\n";
        }
        
        file.close();
    }
    
    double calculateAverageStepsPerSecond(const std::vector<BenchmarkResult>& results) {
        if (results.empty()) return 0.0;
        double sum = 0.0;
        for (const auto& result : results) {
            sum += result.stepsPerSecond;
        }
        return sum / results.size();
    }
    
    double calculateAverageSpikesPerStep(const std::vector<BenchmarkResult>& results) {
        if (results.empty()) return 0.0;
        double sum = 0.0;
        for (const auto& result : results) {
            sum += result.spikesPerStep;
        }
        return sum / results.size();
    }
    
    BenchmarkResult findBestPerformance(const std::vector<BenchmarkResult>& results) {
        if (results.empty()) {
            return BenchmarkResult{};
        }
        auto best = results[0];
        for (const auto& result : results) {
            if (result.calculatePerformanceScore() > best.calculatePerformanceScore()) {
                best = result;
            }
        }
        return best;
    }
}}

// Enhanced benchmark runner with detailed metrics
BenchmarkResult runBenchmark(size_t neuronCount, size_t steps) {
    std::cout << "  Running benchmark: " << neuronCount << " neurons, " 
              << steps << " steps..." << std::flush;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Run simulation with detailed metrics collection
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (size_t step = 0; step < steps; ++step) {
        // Inject some random input
        for (size_t i = 0; i < std::min(size_t(10), neuronCount / 10); ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Collect comprehensive results
    BenchmarkResult result;
    result.neurons = brain->getTotalNeuronCount();
    result.synapses = brain->getTotalSynapseCount();
    result.steps = steps;
    result.totalTime = totalTime;
    result.stepsPerSecond = steps / totalTime;
    result.spikesPerStep = static_cast<double>(brain->getTotalSpikeCount()) / steps;
    result.synapsesPerNeuron = static_cast<double>(result.synapses) / result.neurons;
    
    // Collect neural metrics
    auto neuralMetrics = NeuralMetricsCollector::collectMetrics(brain, 0.001);
    result.avgFiringRate = neuralMetrics.avgFiringRate;
    result.maxFiringRate = neuralMetrics.maxFiringRate;
    result.avgMembranePotential = neuralMetrics.avgMembranePotential;
    result.avgThreshold = neuralMetrics.avgThreshold;
    result.excitationInhibitionRatio = neuralMetrics.excitationInhibitionRatio;
    result.activeNeurons = neuralMetrics.activeNeurons;
    result.firingNeurons = neuralMetrics.firingNeurons;
    result.synapticConnections = result.synapses;
    
    // Estimate other metrics using utility functions
    result.plasticityEffect = BenchmarkUtils::calculatePlasticityEffect(brain);
    result.memoryUtilization = BenchmarkUtils::calculateMemoryUtilization(brain);
    result.predictionAccuracy = BenchmarkUtils::calculatePredictionAccuracy(brain);
    result.energyConsumption = BenchmarkUtils::estimateEnergyConsumption(brain, totalTime);
    result.memoryUsage = BenchmarkUtils::estimateMemoryUsage(brain);
    
    // Display detailed results
    std::cout << " Done!" << std::endl;
    std::cout << "    Time: " << std::fixed << std::setprecision(2) << totalTime << "s" << std::endl;
    std::cout << "    Steps/sec: " << std::fixed << std::setprecision(0) << result.stepsPerSecond << std::endl;
    std::cout << "    Spikes/step: " << std::fixed << std::setprecision(2) << result.spikesPerStep << std::endl;
    std::cout << "    Avg firing rate: " << std::fixed << std::setprecision(2) << result.avgFiringRate << " Hz" << std::endl;
    std::cout << "    Active neurons: " << result.activeNeurons << "/" << result.neurons << std::endl;
    
    return result;
}