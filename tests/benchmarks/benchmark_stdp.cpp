// STDP Performance Benchmark
// Compares O(n²) naive implementation with O(n log n) optimized implementation

#include "brain/Synapse.hpp"
#include "plasticity/STDP.hpp"
#include <chrono>
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <cmath>
#include <iomanip>

namespace benchmark_stdp {

class AlgorithmBenchmarker {
public:
    // Generate spike patterns with different statistical properties
    std::vector<double> generatePoissonSpikes(size_t count, double rate, double duration) {
        std::vector<double> spikes;
        spikes.reserve(count);
        double time = 0.0;
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::exponential_distribution<> expDist(1.0/rate);
        
        for (size_t i = 0; i < count; ++i) {
            time += expDist(gen);
            if (time < duration) {
                spikes.push_back(time);
            } else {
                break;
            }
        }
        return spikes;
    }
    
    std::vector<double> generateSynchronizedSpikes(size_t count, double interval, double offset = 0.0) {
        std::vector<double> spikes;
        spikes.reserve(count);
        for (size_t i = 0; i < count; ++i) {
            spikes.push_back(offset + i * interval);
        }
        return spikes;
    }
    
    std::vector<double> generateRealisticSpikes(size_t count, double meanISI, double stdDevISI) {
        std::vector<double> spikes;
        spikes.reserve(count);
        double time = 0.0;
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<> normDist(meanISI, stdDevISI);
        
        for (size_t i = 0; i < count; ++i) {
            double isi = normDist(gen);
            while (isi <= 0.0) {
                isi = normDist(gen);
            }
            time += isi;
            spikes.push_back(time);
        }
        return spikes;
    }
    
    // Naive O(n²) implementation for comparison
    double naiveSTDP(const std::vector<double>& preSpikes, 
                    const std::vector<double>& postSpikes,
                    double ltpWeight = 0.01, 
                    double ltdWeight = 0.012, 
                    double timeConstant = 20.0,
                    double cutoffTime = 180.0) {
        double totalDelta = 0.0;
        
        for (double pre : preSpikes) {
            for (double post : postSpikes) {
                double dt = post - pre;
                if (std::abs(dt) <= cutoffTime) {
                    if (dt > 0) {
                        // Pre before post: POTENTIATION
                        totalDelta += ltpWeight * std::exp(-dt / timeConstant);
                    } else {
                        // Post before pre: DEPRESSION
                        totalDelta += -ltdWeight * std::exp(dt / timeConstant);
                    }
                }
            }
        }
        return totalDelta;
    }
    
    // Performance benchmark for STDP
    void benchmarkSTDP(size_t neuronCount, size_t spikeCountPerNeuron, std::string scenarioName) {
        std::cout << "\n=== STDP Benchmark: " << scenarioName << " ===\n";
        std::cout << "Network size: " << neuronCount << " neurons, " << spikeCountPerNeuron << " spikes per neuron\n";
        
        nlm::STDP stdp;
        stdp.configure(0.01f, 0.012f, 20.0f);
        
        // Generate spike pairs for each synapse
        std::vector<std::vector<double>> allPreSpikes;
        std::vector<std::vector<double>> allPostSpikes;
        
        for (size_t neuron = 0; neuron < neuronCount; ++neuron) {
            std::vector<double> preSpikes, postSpikes;
            
            if (scenarioName == "Poisson") {
                preSpikes = generatePoissonSpikes(spikeCountPerNeuron, 10.0, 1000.0);
                postSpikes = generatePoissonSpikes(spikeCountPerNeuron, 10.0, 1000.0);
            } else if (scenarioName == "Synchronized") {
                preSpikes = generateSynchronizedSpikes(spikeCountPerNeuron, 10.0, 50.0);
                postSpikes = generateSynchronizedSpikes(spikeCountPerNeuron, 10.0, 60.0);
            } else if (scenarioName == "Realistic") {
                preSpikes = generateRealisticSpikes(spikeCountPerNeuron, 20.0, 5.0);
                postSpikes = generateRealisticSpikes(spikeCountPerNeuron, 20.0, 5.0);
            } else if (scenarioName == "Sparse") {
                preSpikes = generatePoissonSpikes(spikeCountPerNeuron, 2.0, 2000.0);
                postSpikes = generatePoissonSpikes(spikeCountPerNeuron, 2.0, 2000.0);
            } else { // HighDensity
                preSpikes = generatePoissonSpikes(spikeCountPerNeuron, 100.0, 200.0);
                postSpikes = generatePoissonSpikes(spikeCountPerNeuron, 100.0, 200.0);
            }
            
            allPreSpikes.push_back(preSpikes);
            allPostSpikes.push_back(postSpikes);
        }
        
        // Benchmark naive implementation
        auto start = std::chrono::high_resolution_clock::now();
        double naiveTotalDelta = 0.0;
        
        for (size_t i = 0; i < neuronCount; ++i) {
            naiveTotalDelta += naiveSTDP(allPreSpikes[i], allPostSpikes[i]);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto naiveTime = std::chrono::duration<double, std::milli>(end - start).count();
        
        // Benchmark optimized implementation
        start = std::chrono::high_resolution_clock::now();
        double optimizedTotalDelta = 0.0;
        
        for (size_t synIdx = 0; synIdx < neuronCount; ++synIdx) {
            // Convert to appropriate types for the actual STDP implementation
            std::vector<nlm::Timestamp> preVec(allPreSpikes[synIdx].begin(), allPreSpikes[synIdx].end());
            std::vector<nlm::Timestamp> postVec(allPostSpikes[synIdx].begin(), allPostSpikes[synIdx].end());
            
            nlm::Synapse synapse(nlm::SynapseId(synIdx), nlm::NeuronId(synIdx + 10000), nlm::NeuronId(synIdx + 1000));
            // Set initial weight for measurable change
            float initialWeight = synapse.getWeight();
            stdp.update(&synapse, preVec, postVec, 0.001);
            float finalWeight = synapse.getWeight();
            optimizedTotalDelta += (finalWeight - initialWeight);
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
        std::cout << "  Total Δw: " << naiveTotalDelta << "\n\n";
        
        std::cout << "Optimized O(n log n) implementation:\n";
        std::cout << "  Operations: " << optimizedOps << "\n";
        std::cout << "  Time: " << optimizedTime << " ms\n";
        std::cout << "  Total Δw: " << optimizedTotalDelta << "\n\n";
        
        if (optimizedTime > 0.0) {
            double speedup = naiveTime / optimizedTime;
            std::cout << "Speedup: " << speedup << "x\n";
        }
        
        // Check correctness
        double relativeError = std::abs(optimizedTotalDelta - naiveTotalDelta) / std::max(std::abs(naiveTotalDelta), 1e-10);
        std::cout << "Relative error: " << relativeError << "\n";
    }
};

void runSTDPBenchmarks() {
    std::cout << "=== STDP Performance Benchmark Suite ===\n";
    
    AlgorithmBenchmarker benchmarker;
    
    // Test scenarios
    std::vector<std::string> scenarios = {"Poisson", "Synchronized", "Realistic", "Sparse", "HighDensity"};
    
    // Network sizes
    std::vector<std::pair<size_t, size_t>> networkSizes = {
        {10, 5},     // Small
        {100, 10},   // Medium
        {1000, 20}   // Large
    };
    
    for (auto [neuronCount, spikeCount] : networkSizes) {
        for (const auto& scenario : scenarios) {
            benchmarker.benchmarkSTDP(neuronCount, spikeCount, scenario);
        }
    }
}

} // namespace benchmark_stdp

int main() {
    benchmark_stdp::runSTDPBenchmarks();
    return 0;
}
