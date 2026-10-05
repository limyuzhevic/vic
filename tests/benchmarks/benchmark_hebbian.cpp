// Hebbian Performance Benchmark
// Compares O(n²) naive implementation with O(n log n) optimized implementation

#include "brain/Synapse.hpp"
#include "plasticity/Hebbian.hpp"
#include <chrono>
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <cmath>
#include <iomanip>

namespace benchmark_hebbian {

class AlgorithmBenchmarker {
public:
    // Generate spike patterns for Hebbian learning
    std::vector<double> generateCorrelatedSpikes(size_t count, double preRate, double postRate, 
                                                double correlationWindow, double duration) {
        std::vector<double> spikes;
        spikes.reserve(count);
        double time = 0.0;
        
        std::random_device rd;
        std::mt19937 gen(rd());
        
        double preInterval = (preRate > 0) ? (1.0 / preRate) : 0.0;
        double postInterval = (postRate > 0) ? (1.0 / postRate) : 0.0;
        
        if (correlationWindow > 0 && correlationWindow < 100.0) {
            // Generate correlated spikes
            for (size_t i = 0; i < count; ++i) {
                double spikeTime;
                if (i % 10 < 7) { // 70% correlation
                    // Correlated spikes - generate with small time difference
                    spikeTime = time + (i % 3) * 5.0; // 0-10ms offset
                } else {
                    // Uncorrelated spikes - generate with random timing
                    double isi = (preInterval > 0) ? std::exponential_distribution<>(1.0/preInterval)(gen) : 0.0;
                    spikeTime = time + isi;
                }
                
                if (spikeTime < duration) {
                    spikes.push_back(spikeTime);
                    time = spikeTime + 1.0; // Simple inter-spike interval
                }
            }
        } else {
            // Random uncorrelated spikes
            for (size_t i = 0; i < count; ++i) {
                double isi = (preInterval > 0) ? std::exponential_distribution<>(1.0/preInterval)(gen) : 0.0;
                time += isi;
                if (time < duration) {
                    spikes.push_back(time);
                }
            }
        }
        return spikes;
    }
    
    std::vector<double> generateBurstSpikes(size_t numBursts, double burstRate, double duration) {
        std::vector<double> spikes;
        spikes.reserve(numBursts * 10); // 10 spikes per burst
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::exponential_distribution<> burstDist(1.0/burstRate);
        
        for (size_t burst = 0; burst < numBursts; ++burst) {
            double burstTime = burstDist(gen);
            if (burstTime >= duration) break;
            
            // Generate spikes within burst
            for (size_t spike = 0; spike < 10; ++spike) {
                double intraBurstTime = static_cast<double>(spike) * 2.0; // 2ms between spikes
                double spikeTime = burstTime + intraBurstTime;
                if (spikeTime < duration) {
                    spikes.push_back(spikeTime);
                }
            }
        }
        return spikes;
    }
    
    // Naive O(n²) implementation for Hebbian learning
    double naiveHebbian(const std::vector<double>& preSpikes, 
                        const std::vector<double>& postSpikes,
                        double correlationWindow = 100.0) {
        double correlationCount = 0.0;
        
        for (double pre : preSpikes) {
            for (double post : postSpikes) {
                if (std::abs(pre - post) <= correlationWindow) {
                    correlationCount++;
                }
            }
        }
        return correlationCount;
    }
    
    // Performance benchmark for Hebbian
    void benchmarkHebbian(size_t neuronCount, size_t spikeCountPerNeuron, std::string scenarioName) {
        std::cout << "\n=== Hebbian Benchmark: " << scenarioName << " ===\n";
        std::cout << "Network size: " << neuronCount << " neurons, " << spikeCountPerNeuron << " spikes per neuron\n";
        
        nlm::Hebbian hebbian;
        hebbian.setLearningRate(0.01f);
        hebbian.setMaxWeight(1.0f);
        
        // Generate spike pairs for each synapse
        std::vector<std::vector<double>> allPreSpikes;
        std::vector<std::vector<double>> allPostSpikes;
        
        double correlationWindow = 100.0; // Default 100ms window
        
        if (scenarioName == "Tight") {
            correlationWindow = 10.0;
        } else if (scenarioName == "Loose") {
            correlationWindow = 1000.0;
        } else if (scenarioName == "NoCorrelation") {
            correlationWindow = 0.0; // No correlation
        } else if (scenarioName == "Variable") {
            correlationWindow = 200.0; // Variable window
        } else { // Burst
            correlationWindow = 50.0; // Short window for burst detection
        }
        
        for (size_t neuron = 0; neuron < neuronCount; ++neuron) {
            std::vector<double> preSpikes, postSpikes;
            
            if (scenarioName == "Burst") {
                preSpikes = generateBurstSpikes(spikeCountPerNeuron/10, 10.0, 1000.0);
                postSpikes = generateBurstSpikes(spikeCountPerNeuron/10, 10.0, 1000.0);
            } else {
                preSpikes = generateCorrelatedSpikes(spikeCountPerNeuron, 10.0, 10.0, correlationWindow, 1000.0);
                postSpikes = generateCorrelatedSpikes(spikeCountPerNeuron, 10.0, 10.0, correlationWindow, 1000.0);
            }
            
            allPreSpikes.push_back(preSpikes);
            allPostSpikes.push_back(postSpikes);
        }
        
        // Benchmark naive implementation
        auto start = std::chrono::high_resolution_clock::now();
        double naiveCorrelation = 0.0;
        
        for (size_t i = 0; i < neuronCount; ++i) {
            naiveCorrelation += naiveHebbian(allPreSpikes[i], allPostSpikes[i], correlationWindow);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto naiveTime = std::chrono::duration<double, std::milli>(end - start).count();
        
        // Benchmark optimized implementation
        start = std::chrono::high_resolution_clock::now();
        double optimizedCorrelation = 0.0;
        
        for (size_t synIdx = 0; synIdx < neuronCount; ++synIdx) {
            std::vector<nlm::Timestamp> preVec(allPreSpikes[synIdx].begin(), allPreSpikes[synIdx].end());
            std::vector<nlm::Timestamp> postVec(allPostSpikes[synIdx].begin(), allPostSpikes[synIdx].end());
            
            nlm::Synapse synapse(nlm::SynapseId(synIdx), nlm::NeuronId(synIdx + 10000), nlm::NeuronId(synIdx + 2000));
            float initialWeight = synapse.getWeight();
            hebbian.update(&synapse, preVec, postVec, 0.001);
            float finalWeight = synapse.getWeight();
            optimizedCorrelation += (finalWeight - initialWeight);
        }
        
        end = std::chrono::high_resolution_clock::now();
        auto optimizedTime = std::chrono::duration<double, std::milli>(end - start).count();
        
        // Calculate operations
        size_t naiveOps = neuronCount * spikeCountPerNeuron * spikeCountPerNeuron;
        size_t optimizedOps = neuronCount * (spikeCountPerNeuron * std::log2(spikeCountPerNeuron));
        
        // Output results
        std::cout << std::fixed << std::setprecision(6);
        std::cout << "Naive O(n²) implementation:\n";
        std::cout << "  Operations: " << naiveOps << "\n";
        std::cout << "  Time: " << naiveTime << " ms\n";
        std::cout << "  Correlation count: " << naiveCorrelation << "\n\n";
        
        std::cout << "Optimized O(n log n) implementation:\n";
        std::cout << "  Operations: " << optimizedOps << "\n";
        std::cout << "  Time: " << optimizedTime << " ms\n";
        std::cout << "  Correlation count: " << optimizedCorrelation << "\n\n";
        
        if (optimizedTime > 0.0) {
            double speedup = naiveTime / optimizedTime;
            std::cout << "Speedup: " << speedup << "x\n";
        }
        
        // Check correctness
        double relativeError = std::abs(optimizedCorrelation - naiveCorrelation) / std::max(std::abs(naiveCorrelation), 1e-10);
        std::cout << "Relative error: " << relativeError << "\n";
    }
};

void runHebbianBenchmarks() {
    std::cout << "=== Hebbian Performance Benchmark Suite ===\n";
    
    AlgorithmBenchmarker benchmarker;
    
    // Test scenarios
    std::vector<std::string> scenarios = {"Tight", "Loose", "NoCorrelation", "Variable", "Burst"};
    
    // Network sizes
    std::vector<std::pair<size_t, size_t>> networkSizes = {
        {10, 5},     // Small
        {100, 10},   // Medium
        {1000, 20}   // Large
    };
    
    for (auto [neuronCount, spikeCount] : networkSizes) {
        for (const auto& scenario : scenarios) {
            benchmarker.benchmarkHebbian(neuronCount, spikeCount, scenario);
        }
    }
}

} // namespace benchmark_hebbian

int main() {
    benchmark_hebbian::runHebbianBenchmarks();
    return 0;
}
