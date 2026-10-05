// Simplified Plasticity Integration Performance Benchmark
// Benchmarks combined STDP and Hebbian operations without complex dependencies

#include "brain/Synapse.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include <chrono>
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <cmath>
#include <iomanip>
#include <fstream>

namespace benchmark_plasticity {

class IntegrationBenchmarker {
public:
    // Generate spike trains with realistic dynamics
    std::vector<double> generateRealisticSpikeTrain(size_t count, double baseRate, 
                                                   double duration, double burstProbability = 0.1) {
        std::vector<double> spikes;
        spikes.reserve(count);
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::exponential_distribution<> rateDist(1.0/baseRate);
        
        double time = 0.0;
        for (size_t i = 0; i < count; ++i) {
            if (std::bernoulli_distribution(burstProbability)(gen)) {
                // Burst: generate spikes with short intervals
                double burstTime = time + rateDist(gen) * 0.1;
                for (size_t j = 0; j < 3 && burstTime < duration && spikes.size() < count; ++j) {
                    spikes.push_back(burstTime);
                    burstTime += 1.0;
                }
                if (!spikes.empty()) {
                    time = spikes.back();
                }
            } else {
                // Normal firing
                time += rateDist(gen);
                if (time < duration && spikes.size() < count) {
                    spikes.push_back(time);
                }
            }
        }
        return spikes;
    }
    
    // Combined naive implementation for both STDP and Hebbian
    void combinedNaiveSTDP(const std::vector<std::vector<double>>& allPreSpikes,
                          const std::vector<std::vector<double>>& allPostSpikes,
                          std::vector<double>& totalDelta,
                          double ltpWeight = 0.01, double ltdWeight = 0.012, 
                          double timeConstant = 20.0, double cutoffTime = 180.0) {
        for (size_t i = 0; i < allPreSpikes.size(); ++i) {
            double delta = 0.0;
            for (double pre : allPreSpikes[i]) {
                for (double post : allPostSpikes[i]) {
                    double dt = post - pre;
                    if (std::abs(dt) <= cutoffTime) {
                        if (dt > 0) {
                            delta += ltpWeight * std::exp(-dt / timeConstant);
                        } else {
                            delta += -ltdWeight * std::exp(dt / timeConstant);
                        }
                    }
                }
            }
            totalDelta[i] = delta;
        }
    }
    
    void combinedNaiveHebbian(const std::vector<std::vector<double>>& allPreSpikes,
                             const std::vector<std::vector<double>>& allPostSpikes,
                             std::vector<double>& correlation,
                             double correlationWindow = 100.0) {
        for (size_t i = 0; i < allPreSpikes.size(); ++i) {
            double corr = 0.0;
            for (double pre : allPreSpikes[i]) {
                for (double post : allPostSpikes[i]) {
                    if (std::abs(pre - post) <= correlationWindow) {
                        corr++;
                    }
                }
            }
            correlation[i] = corr;
        }
    }
    
    // Comprehensive benchmark for plasticity algorithms
    void benchmarkPlasticityIntegration(size_t neuronCount, size_t spikeCountPerNeuron, 
                                        const std::string& scenarioName) {
        std::cout << "\n=== Integration Benchmark: " << scenarioName << " ===\n";
        std::cout << "Network: " << neuronCount << " neurons, " << spikeCountPerNeuron << " spikes/neuron\n";
        
        nlm::STDP stdp;
        nlm::Hebbian hebbian;
        stdp.configure(0.01f, 0.012f, 20.0f);
        hebbian.setLearningRate(0.01f);
        
        // Generate spike data
        std::vector<std::vector<double>> allPreSpikes;
        std::vector<std::vector<double>> allPostSpikes;
        double baseRate = 10.0; // Hz
        double duration = 1000.0; // ms
        
        for (size_t neuron = 0; neuron < neuronCount; ++neuron) {
            std::vector<double> preSpikes, postSpikes;
            
            if (scenarioName == "FeedForward") {
                preSpikes = generateRealisticSpikeTrain(spikeCountPerNeuron, baseRate, duration, 0.15);
                postSpikes = generateRealisticSpikeTrain(spikeCountPerNeuron, baseRate * 0.8, duration, 0.1);
            } else if (scenarioName == "Recurrent") {
                preSpikes = generateRealisticSpikeTrain(spikeCountPerNeuron, baseRate * 1.2, duration, 0.2);
                postSpikes = generateRealisticSpikeTrain(spikeCountPerNeuron, baseRate * 1.1, duration, 0.2);
            } else if (scenarioName == "Sparse") {
                preSpikes = generateRealisticSpikeTrain(spikeCountPerNeuron, baseRate * 0.3, duration, 0.05);
                postSpikes = generateRealisticSpikeTrain(spikeCountPerNeuron, baseRate * 0.3, duration, 0.05);
            } else { // Balanced
                preSpikes = generateRealisticSpikeTrain(spikeCountPerNeuron, baseRate, duration, 0.12);
                postSpikes = generateRealisticSpikeTrain(spikeCountPerNeuron, baseRate, duration, 0.12);
            }
            
            allPreSpikes.push_back(preSpikes);
            allPostSpikes.push_back(postSpikes);
        }
        
        // Prepare storage for results
        std::vector<double> naiveSTDPDeltas(allPreSpikes.size(), 0.0);
        std::vector<double> optimizedSTDPDeltas(allPreSpikes.size(), 0.0);
        std::vector<double> naiveHebbianCorrs(allPreSpikes.size(), 0.0);
        std::vector<double> optimizedHebbianCorrs(allPreSpikes.size(), 0.0);
        
        // Benchmark naive implementations
        auto start = std::chrono::high_resolution_clock::now();
        combinedNaiveSTDP(allPreSpikes, allPostSpikes, naiveSTDPDeltas);
        combinedNaiveHebbian(allPreSpikes, allPostSpikes, naiveHebbianCorrs);
        auto end = std::chrono::high_resolution_clock::now();
        double naiveTime = std::chrono::duration<double, std::milli>(end - start).count();
        
        // Benchmark optimized implementations
        start = std::chrono::high_resolution_clock::now();
        
        for (size_t synIdx = 0; synIdx < neuronCount; ++synIdx) {
            // STDP
            std::vector<nlm::Timestamp> preVec(allPreSpikes[synIdx].begin(), allPreSpikes[synIdx].end());
            std::vector<nlm::Timestamp> postVec(allPostSpikes[synIdx].begin(), allPostSpikes[synIdx].end());
            
            nlm::Synapse stdpSynapse(nlm::SynapseId(synIdx), nlm::NeuronId(synIdx + 10000), nlm::NeuronId(synIdx + 1000));
            float stdpInitialWeight = stdpSynapse.getWeight();
            stdp.update(&stdpSynapse, preVec, postVec, 0.001);
            float stdpFinalWeight = stdpSynapse.getWeight();
            optimizedSTDPDeltas[synIdx] = (stdpFinalWeight - stdpInitialWeight);
            
            // Hebbian
            nlm::Synapse hebbianSynapse(nlm::SynapseId(synIdx + 20000), nlm::NeuronId(synIdx + 10000), nlm::NeuronId(synIdx + 2000));
            float hebbianInitialWeight = hebbianSynapse.getWeight();
            hebbian.update(&hebbianSynapse, preVec, postVec, 0.001);
            float hebbianFinalWeight = hebbianSynapse.getWeight();
            optimizedHebbianCorrs[synIdx] = (hebbianFinalWeight - hebbianInitialWeight);
        }
        
        end = std::chrono::high_resolution_clock::now();
        double optimizedTime = std::chrono::duration<double, std::milli>(end - start).count();
        
        // Calculate statistics
        double naiveSTDPMean = std::accumulate(naiveSTDPDeltas.begin(), naiveSTDPDeltas.end(), 0.0) / naiveSTDPDeltas.size();
        double naiveHebbianMean = std::accumulate(naiveHebbianCorrs.begin(), naiveHebbianCorrs.end(), 0.0) / naiveHebbianCorrs.size();
        
        double optimizedSTDPMean = std::accumulate(optimizedSTDPDeltas.begin(), optimizedSTDPDeltas.end(), 0.0) / optimizedSTDPDeltas.size();
        double optimizedHebbianMean = std::accumulate(optimizedHebbianCorrs.begin(), optimizedHebbianCorrs.end(), 0.0) / optimizedHebbianCorrs.size();
        
        // Calculate accuracies
        double stddpAccuracy = std::abs(optimizedSTDPMean - naiveSTDPMean) / std::max(std::abs(naiveSTDPMean), 1e-10);
        double hebbianAccuracy = std::abs(optimizedHebbianMean - naiveHebbianMean) / std::max(std::abs(naiveHebbianMean), 1e-10);
        
        // Calculate operations
        size_t naiveOps = neuronCount * spikeCountPerNeuron * spikeCountPerNeuron * 2; // Both STDP and Hebbian
        size_t optimizedOps = neuronCount * (spikeCountPerNeuron * std::log2(spikeCountPerNeuron)) * 2;
        
        // Output results
        std::cout << std::fixed << std::setprecision(6);
        std::cout << "=== Performance Comparison ===\n";
        std::cout << "Naive O(n²) implementation:\n";
        std::cout << "  Time: " << naiveTime << " ms\n";
        std::cout << "  STDP Δw mean: " << naiveSTDPMean << "\n";
        std::cout << "  Hebbian correlation mean: " << naiveHebbianMean << "\n\n";
        
        std::cout << "Optimized O(n log n) implementation:\n";
        std::cout << "  Time: " << optimizedTime << " ms\n";
        std::cout << "  STDP Δw mean: " << optimizedSTDPMean << "\n";
        std::cout << "  Hebbian correlation mean: " << optimizedHebbianMean << "\n\n";
        
        double speedup = naiveTime / std::max(optimizedTime, 0.001);
        std::cout << "Overall speedup: " << speedup << "x\n";
        std::cout << "STDP accuracy: " << (1.0 - stddpAccuracy) * 100 << "%\n";
        std::cout << "Hebbian accuracy: " << (1.0 - hebbianAccuracy) * 100 << "%\n";
    }
};

void runPlasticityIntegrationBenchmarks() {
    std::cout << "=== Plasticity Integration Performance Benchmark ===\n";
    
    IntegrationBenchmarker benchmarker;
    
    // Integration scenarios
    std::vector<std::string> scenarios = {"FeedForward", "Recurrent", "Sparse", "Balanced"};
    
    // Network sizes
    std::vector<std::pair<size_t, size_t>> networkSizes = {
        {10, 5},     // Small
        {100, 10},   // Medium
        {1000, 20}   // Large
    };
    
    for (auto [neuronCount, spikeCount] : networkSizes) {
        for (const auto& scenario : scenarios) {
            benchmarker.benchmarkPlasticityIntegration(neuronCount, spikeCount, scenario);
        }
    }
}

} // namespace benchmark_plasticity

int main() {
    benchmark_plasticity::runPlasticityIntegrationBenchmarks();
    return 0;
}
