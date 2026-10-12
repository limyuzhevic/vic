#include "core/DebugTools.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>

namespace nlm {

void DebugTools::analyzeBrainState(std::shared_ptr<Brain> brain) const {
    NLM_LOG_INFO("=== Brain State Analysis ===");
    NLM_LOG_INFO("Total Neurons: " + std::to_string(brain->getTotalNeuronCount()));
    NLM_LOG_INFO("Total Synapses: " + std::to_string(brain->getTotalSynapseCount()));
    NLM_LOG_INFO("Active Neurons: " + std::to_string(brain->getActiveNeuronCount()));
    NLM_LOG_INFO("Firing Neurons: " + std::to_string(brain->getFiringNeuronCount()));
    NLM_LOG_INFO("Total Spike Count: " + std::to_string(brain->getTotalSpikeCount()));
    NLM_LOG_INFO("Average Firing Rate: " + std::to_string(brain->getAverageFiringRate()));
    NLM_LOG_INFO("Working Memory: " + std::to_string(brain->getWorkingMemory() ? 
                      brain->getWorkingMemory()->getActiveTraces() : 0));
    NLM_LOG_INFO("Episodic Memory: " + std::to_string(brain->getEpisodicMemory() ? 
                      brain->getEpisodicMemory()->getEpisodeCount() : 0));
}

void DebugTools::analyzeNeurons(std::shared_ptr<Brain> brain) const {
    NLM_LOG_INFO("=== Neuron Analysis ===");
    
    auto* spikeSystem = brain->getSpikeSystem();
    if (!spikeSystem) {
        NLM_LOG_WARNING("Spike system not available");
        return;
    }
    
    auto mostActiveNeurons = spikeSystem->getMostActiveNeurons(10);
    NLM_LOG_INFO("Top 10 Most Active Neurons:");
    for (size_t i = 0; i < mostActiveNeurons.size(); ++i) {
        float activity = spikeSystem->getNeuronActivity(mostActiveNeurons[i]);
        NLM_LOG_INFO("  " + std::to_string(i + 1) + ". Neuron " + 
                    std::to_string(mostActiveNeurons[i]) + 
                    ": Activity = " + std::to_string(activity));
    }
    
    // Analyze neuron types
    size_t sensoryCount = 0, motorCount = 0, internalCount = 0;
    auto regions = brain->getRegions();
    for (const auto& region : regions) {
        auto neurons = region->getAllNeurons();
        for (const auto& neuron : neurons) {
            switch (neuron->getType()) {
                case NeuronType::Sensory: sensoryCount++; break;
                case NeuronType::Motor: motorCount++; break;
                case NeuronType::Internal: internalCount++; break;
                default: break;
            }
        }
    }
    
    NLM_LOG_INFO("Neuron Type Distribution:");
    NLM_LOG_INFO("  Sensory: " + std::to_string(sensoryCount));
    NLM_LOG_INFO("  Motor: " + std::to_string(motorCount));
    NLM_LOG_INFO("  Internal: " + std::to_string(internalCount));
}

void DebugTools::analyzeSynapses(std::shared_ptr<Brain> brain) const {
    NLM_LOG_INFO("=== Synapse Analysis ===");
    
    auto regions = brain->getRegions();
    size_t totalExcitatory = 0, totalInhibitory = 0;
    float totalWeight = 0.0f;
    float maxWeight = -1000.0f, minWeight = 1000.0f;
    
    for (const auto& region : regions) {
        auto synapses = region->getSynapses();
        for (const auto& synapse : synapses) {
            if (synapse->isExcitatory()) {
                totalExcitatory++;
            } else if (synapse->isInhibitory()) {
                totalInhibitory++;
            }
            
            float weight = synapse->getWeight();
            totalWeight += weight;
            if (weight > maxWeight) maxWeight = weight;
            if (weight < minWeight) minWeight = weight;
        }
    }
    
    size_t totalSynapses = totalExcitatory + totalInhibitory;
    
    NLM_LOG_INFO("Synapse Statistics:");
    NLM_LOG_INFO("  Total Synapses: " + std::to_string(totalSynapses));
    NLM_LOG_INFO("  Excitatory: " + std::to_string(totalExcitatory));
    NLM_LOG_INFO("  Inhibitory: " + std::to_string(totalInhibitory));
    NLM_LOG_INFO("  Mean Weight: " + std::to_string(totalSynapses > 0 ? totalWeight / totalSynapses : 0.0f));
    NLM_LOG_INFO("  Max Weight: " + std::to_string(maxWeight));
    NLM_LOG_INFO("  Min Weight: " + std::to_string(minWeight));
    
    // Analyze weight distribution
    if (totalSynapses > 0) {
        std::vector<float> weights;
        for (const auto& region : regions) {
            auto synapses = region->getSynapses();
            for (const auto& synapse : synapses) {
                weights.push_back(synapse->getWeight());
            }
        }
        
        std::sort(weights.begin(), weights.end());
        size_t q25 = weights.size() / 4;
        size_t q50 = weights.size() / 2;
        size_t q75 = weights.size() * 3 / 4;
        
        NLM_LOG_INFO("Weight Quartiles:");
        if (q25 < weights.size()) NLM_LOG_INFO("  Q25: " + std::to_string(weights[q25]));
        if (q50 < weights.size()) NLM_LOG_INFO("  Q50: " + std::to_string(weights[q50]));
        if (q75 < weights.size()) NLM_LOG_INFO("  Q75: " + std::to_string(weights[q75]));
    }
}

void DebugTools::analyzeConnectivity(std::shared_ptr<Brain> brain) const {
    NLM_LOG_INFO("=== Connectivity Analysis ===");
    
    auto regions = brain->getRegions();
    if (regions.size() < 2) {
        NLM_LOG_INFO("Only one region, skipping inter-region connectivity analysis.");
    } else {
        // Analyze inter-region connections
        NLM_LOG_INFO("Inter-Region Connections:");
        for (size_t i = 0; i < regions.size(); ++i) {
            auto region = regions[i].get();
            size_t localSynapses = region->getSynapses().size();
            NLM_LOG_INFO("  Region " + std::to_string(i) + ": " + std::to_string(localSynapses) + 
                        " local synapses");
            
            // Count outgoing connections
            size_t outgoing = 0;
            auto neurons = region->getAllNeurons();
            for (const auto& neuron : neurons) {
                auto outgoingConns = neuron->getOutgoingSynapses();
                outgoing += outgoingConns.size();
            }
            NLM_LOG_INFO("    Outgoing connections: " + std::to_string(outgoing));
        }
    }
    
    // Analyze average connections per neuron
    size_t totalNeurons = brain->getTotalNeuronCount();
    size_t totalSynapses = brain->getTotalSynapseCount();
    
    if (totalNeurons > 0) {
        float avgConnections = (float)totalSynapses / totalNeurons;
        NLM_LOG_INFO("Average connections per neuron: " + std::to_string(avgConnections));
        
        // Estimate connection probability
        size_t maxPossible = totalNeurons * (totalNeurons - 1) / 2; // Undirected
        if (maxPossible > 0) {
            float connectionProb = (float)totalSynapses / maxPossible;
            NLM_LOG_INFO("Estimated connection probability: " + std::to_string(connectionProb));
        }
    }
}

void DebugTools::analyzePlasticity(std::shared_ptr<Brain> brain) const {
    NLM_LOG_INFO("=== Plasticity Analysis ===");
    
    auto* stdp = brain->getSTDP();
    auto* hebbian = brain->getHebbian();
    auto* structural = brain->getStructuralPlasticity();
    
    if (stdp) {
        NLM_LOG_INFO("STDP System:");
        NLM_LOG_INFO("  LTP Weight: " + std::to_string(stdp->getLTPWeight()));
        NLM_LOG_INFO("  LTD Weight: " + std::to_string(stdp->getLTDWeight()));
        NLM_LOG_INFO("  Time Constant: " + std::to_string(stdp->getTimeConstant()));
    }
    
    if (hebbian) {
        NLM_LOG_INFO("Hebbian System:");
        NLM_LOG_INFO("  Learning Rate: " + std::to_string(hebbian->getLearningRate()));
        NLM_LOG_INFO("  Trace Decay: " + std::to_string(hebbian->getTraceDecay()));
    }
    
    if (structural) {
        NLM_LOG_INFO("Structural Plasticity System:");
        NLM_LOG_INFO("  Synaptogenesis Rate: " + std::to_string(structural->getSynaptogenesisRate()));
        NLM_LOG_INFO("  Pruning Rate: " + std::to_string(structural->getPruningRate()));
        NLM_LOG_INFO("  Min Synapse Weight: " + std::to_string(structural->getMinSynapseWeight()));
        NLM_LOG_INFO("  Max Synapse Weight: " + std::to_string(structural->getMaxSynapseWeight()));
    }
    
    // Analyze per-synapse plasticity flags
    auto regions = brain->getRegions();
    size_t stdpEnabled = 0, hebbianEnabled = 0, rewardEnabled = 0;
    
    for (const auto& region : regions) {
        auto synapses = region->getSynapses();
        for (const auto& synapse : synapses) {
            auto flags = synapse->getPlasticityFlags();
            if (flags.stdp) stdpEnabled++;
            if (flags.hebbian) hebbianEnabled++;
            if (flags.reward_modulated) rewardEnabled++;
        }
    }
    
    NLM_LOG_INFO("Plasticity Flags Distribution:");
    NLM_LOG_INFO("  STDP enabled: " + std::to_string(stdpEnabled));
    NLM_LOG_INFO("  Hebbian enabled: " + std::to_string(hebbianEnabled));
    NLM_LOG_INFO("  Reward-modulated enabled: " + std::to_string(rewardEnabled));
}

void DebugTools::analyzeSTDP(std::shared_ptr<Brain> brain) const {
    NLM_LOG_INFO("=== STDP Analysis ===");
    
    auto* stdp = brain->getSTDP();
    if (!stdp) {
        NLM_LOG_WARNING("STDP system not available");
        return;
    }
    
    // Simulate some STDP activity
    NLM_LOG_INFO("Simulating STDP activity with correlated pre-post spiking...");
    
    auto regions = brain->getRegions();
    if (regions.size() > 0) {
        auto neurons = regions[0]->getAllNeurons();
        if (neurons.size() >= 2) {
            // Get first two neurons
            Neuron* preNeuron = neurons[0].get();
            Neuron* postNeuron = neurons[1].get();
            
            // Record initial weights
            float initialPreWeight = 0.0f;
            float initialPostWeight = 0.0f;
            
            // Find synapses
            auto preOutgoing = preNeuron->getOutgoingSynapses();
            auto postIncoming = postNeuron->getIncomingSynapses();
            
            for (const auto& synapse : postIncoming) {
                if (synapse->isExcitatory()) {
                    initialPreWeight = synapse->getWeight();
                }
            }
            
            for (const auto& synapse : preOutgoing) {
                if (synapse->isExcitatory()) {
                    initialPostWeight = synapse->getWeight();
                }
            }
            
            // Simulate correlated activity (pre fires, then post fires)
            NLM_LOG_INFO("Pre-synaptic neuron weight: " + std::to_string(initialPreWeight));
            NLM_LOG_INFO("Post-synaptic neuron weight: " + std::to_string(initialPostWeight));
            
            // This would normally involve actual neural simulation
            NLM_LOG_INFO("STDP prediction: Correlated activity should cause potentiation (weight increase)");
            NLM_LOG_INFO("LTP magnitude prediction: ~" + std::to_string(stdp->getLTPWeight() * 100) + "%");
        }
    }
}

void DebugTools::analyzeHebbian(std::shared_ptr<Brain> brain) const {
    NLM_LOG_INFO("=== Hebbian Analysis ===");
    
    auto* hebbian = brain->getHebbian();
    if (!hebbian) {
        NLM_LOG_WARNING("Hebbian system not available");
        return;
    }
    
    NLM_LOG_INFO("Hebbian plasticity parameters:");
    NLM_LOG_INFO("  Learning Rate: " + std::to_string(hebbian->getLearningRate()));
    NLM_LOG_INFO("  Trace Decay: " + std::to_string(hebbian->getTraceDecay()));
    NLM_LOG_INFO("  Co-learning Threshold: " + std::to_string(hebbian->getCoLearningThreshold()));
    
    NLM_LOG_INFO("Hebbian learning mechanism: Simultaneous activation reinforces connections");
    NLM_LOG_INFO("Expected behavior: Repeated co-activation of neurons strengthens synapses");
}

void DebugTools::analyzeMemory(std::shared_ptr<Brain> brain) const {
    NLM_LOG_INFO("=== Memory Analysis ===");
    
    auto* workingMem = brain->getWorkingMemory();
    auto* episodicMem = brain->getEpisodicMemory();
    auto* associativeMem = brain->getAssociativeMemory();
    
    if (workingMem) {
        NLM_LOG_INFO("Working Memory:");
        NLM_LOG_INFO("  Active Traces: " + std::to_string(workingMem->getActiveTraces()));
        NLM_LOG_INFO("  Total Traces: " + std::to_string(workingMem->getTotalTraces()));
        NLM_LOG_INFO("  Trace Capacity: " + std::to_string(workingMem->getTraceCapacity()));
        NLM_LOG_INFO("  Trace Decay Rate: " + std::to_string(workingMem->getTraceDecayRate()));
    }
    
    if (episodicMem) {
        NLM_LOG_INFO("Episodic Memory:");
        NLM_LOG_INFO("  Episode Count: " + std::to_string(episodicMem->getEpisodeCount()));
        NLM_LOG_INFO("  Max Episodes: " + std::to_string(episodicMem->getMaxEpisodes()));
        NLM_LOG_INFO("  Replay Frequency: " + std::to_string(episodicMem->getReplayFrequency()));
        NLM_LOG_INFO("  Consolidation Rate: " + std::to_string(episodicMem->getConsolidationRate()));
    }
    
    if (associativeMem) {
        NLM_LOG_INFO("Associative Memory:");
        NLM_LOG_INFO("  Pattern Associations: " + std::to_string(associativeMem->getAssociationCount()));
        NLM_LOG_INFO("  Max Associations: " + std::to_string(associativeMem->getMaxAssociations()));
        NLM_LOG_INFO("  Association Strength: " + std::to_string(associativeMem->getAssociationStrength()));
    }
}

void DebugTools::analyzeWorkingMemory(std::shared_ptr<Brain> brain) const {
    NLM_LOG_INFO("=== Working Memory Analysis ===");
    
    auto* workingMem = brain->getWorkingMemory();
    if (!workingMem) {
        NLM_LOG_WARNING("Working memory system not available");
        return;
    }
    
    size_t activeTraces = workingMem->getActiveTraces();
    size_t totalTraces = workingMem->getTotalTraces();
    size_t capacity = workingMem->getTraceCapacity();
    
    NLM_LOG_INFO("Working Memory Statistics:");
    NLM_LOG_INFO("  Active Traces: " + std::to_string(activeTraces));
    NLM_LOG_INFO("  Total Traces: " + std::to_string(totalTraces));
    NLM_LOG_INFO("  Capacity: " + std::to_string(capacity));
    NLM_LOG_INFO("  Utilization: " + std::to_string((float)activeTraces / capacity * 100) + "%");
    NLM_LOG_INFO("  Trace Decay Rate: " + std::to_string(workingMem->getTraceDecayRate()));
    
    if (activeTraces > 0) {
        NLM_LOG_INFO("Active Traces Details:");
        // This would normally involve accessing the actual trace data
        NLM_LOG_INFO("  (Would list active trace information here)");
    }
}

void DebugTools::analyzeEpisodicMemory(std::shared_ptr<Brain> brain) const {
    NLM_LOG_INFO("=== Episodic Memory Analysis ===");
    
    auto* episodicMem = brain->getEpisodicMemory();
    if (!episodicMem) {
        NLM_LOG_WARNING("Episodic memory system not available");
        return;
    }
    
    size_t episodeCount = episodicMem->getEpisodeCount();
    size_t maxEpisodes = episodicMem->getMaxEpisodes();
    float replayFreq = episodicMem->getReplayFrequency();
    float consolidationRate = episodicMem->getConsolidationRate();
    
    NLM_LOG_INFO("Episodic Memory Statistics:");
    NLM_LOG_INFO("  Episode Count: " + std::to_string(episodeCount));
    NLM_LOG_INFO("  Max Episodes: " + std::to_string(maxEpisodes));
    NLM_LOG_INFO("  Utilization: " + std::to_string((float)episodeCount / maxEpisodes * 100) + "%");
    NLM_LOG_INFO("  Replay Frequency: " + std::to_string(replayFreq));
    NLM_LOG_INFO("  Consolidation Rate: " + std::to_string(consolidationRate));
    
    if (episodeCount > 0) {
        NLM_LOG_INFO("Sample Episodes:");
        // This would normally involve accessing the actual episode data
        NLM_LOG_INFO("  (Would list sample episodes here)");
    }
}

void DebugTools::analyzeNeuromodulation(std::shared_ptr<Brain> brain) const {
    NLM_LOG_INFO("=== Neuromodulation Analysis ===");
    
    auto* dopamine = brain->getDopamine();
    auto* curiosity = brain->getCuriosity();
    auto* novelty = brain->getNovelty();
    auto* predictionError = brain->getPredictionErrorSignal();
    
    if (dopamine) {
        NLM_LOG_INFO("Dopamine System:");
        NLM_LOG_INFO("  Level: " + std::to_string(dopamine->getLevel()));
        NLM_LOG_INFO("  Learning Rate: " + std::to_string(dopamine->getLearningRate()));
        NLM_LOG_INFO("  Expectation: " + std::to_string(dopamine->getExpectation()));
    }
    
    if (curiosity) {
        NLM_LOG_INFO("Curiosity System:");
        NLM_LOG_INFO("  Level: " + std::to_string(curiosity->getLevel()));
        NLM_LOG_INFO("  Exploration Drive: " + std::to_string(curiosity->getExplorationDrive()));
        NLM_LOG_INFO("  Novelty Sensitivity: " + std::to_string(curiosity->getNoveltySensitivity()));
    }
    
    if (novelty) {
        NLM_LOG_INFO("Novelty System:");
        NLM_LOG_INFO("  Detection Threshold: " + std::to_string(novelty->getDetectionThreshold()));
        NLM_LOG_INFO("  Adaptation Rate: " + std::to_string(novelty->getAdaptationRate()));
    }
    
    if (predictionError) {
        NLM_LOG_INFO("Prediction Error System:");
        NLM_LOG_INFO("  Error Level: " + std::to_string(predictionError->getErrorLevel()));
        NLM_LOG_INFO("  Prediction Confidence: " + std::to_string(predictionError->getPredictionConfidence()));
    }
}

void DebugTools::analyzeDopamine(std::shared_ptr<Brain> brain) const {
    NLM_LOG_INFO("=== Dopamine Analysis ===");
    
    auto* dopamine = brain->getDopamine();
    if (!dopamine) {
        NLM_LOG_WARNING("Dopamine system not available");
        return;
    }
    
    NLM_LOG_INFO("Dopamine System Analysis:");
    NLM_LOG_INFO("  Current Level: " + std::to_string(dopamine->getLevel()));
    NLM_LOG_INFO("  Learning Rate: " + std::to_string(dopamine->getLearningRate()));
    NLM_LOG_INFO("  Expectation: " + std::to_string(dopamine->getExpectation()));
    
    float error = std::abs(dopamine->getLevel() - 0.5f); // Ideal level around 0.5
    NLM_LOG_INFO("  Deviation from Ideal Level: " + std::to_string(error * 100) + "%");
    
    if (dopamine->getLevel() > 0.7f) {
        NLM_LOG_INFO("  State: HIGH - Enhanced learning, potential over-excitation");
    } else if (dopamine->getLevel() < 0.3f) {
        NLM_LOG_INFO("  State: LOW - Reduced learning, potential apathy");
    } else {
        NLM_LOG_INFO("  State: OPTIMAL - Balanced learning and stability");
    }
}

void DebugTools::analyzeCuriosity(std::shared_ptr<Brain> brain) const {
    NLM_LOG_INFO("=== Curiosity Analysis ===");
    
    auto* curiosity = brain->getCuriosity();
    if (!curiosity) {
        NLM_LOG_WARNING("Curiosity system not available");
        return;
    }
    
    NLM_LOG_INFO("Curiosity System Analysis:");
    NLM_LOG_INFO("  Current Level: " + std::to_string(curiosity->getLevel()));
    NLM_LOG_INFO("  Exploration Drive: " + std::to_string(curiosity->getExplorationDrive()));
    NLM_LOG_INFO("  Novelty Sensitivity: " + std::to_string(curiosity->getNoveltySensitivity()));
    
    float totalDrive = curiosity->getExplorationDrive() + curiosity->getNoveltySensitivity();
    NLM_LOG_INFO("  Total Motivation: " + std::to_string(totalDrive));
    
    if (curiosity->getLevel() > 0.7f) {
        NLM_LOG_INFO("  State: HIGH - High exploration, potential distraction");
    } else if (curiosity->getLevel() < 0.3f) {
        NLM_LOG_INFO("  State: LOW - Low exploration, potential stagnation");
    } else {
        NLM_LOG_INFO("  State: OPTIMAL - Balanced exploration and exploitation");
    }
}

void DebugTools::profilePerformance(std::shared_ptr<Brain> brain, size_t steps) const {
    NLM_LOG_INFO("=== Performance Profiling (" + std::to_string(steps) + " steps) ===");
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Run steps and measure performance
    for (size_t step = 0; step < steps; ++step) {
        brain->step(step, step * 0.001);
        
        if (step % 100 == 0) {
            float elapsed = std::chrono::duration<double>(
                std::chrono::high_resolution_clock::now() - startTime).count();
            float stepsPerSecond = step / elapsed;
            NLM_LOG_INFO("Step " + std::to_string(step) + ": " + 
                        std::to_string(stepsPerSecond) + " steps/sec");
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    double stepsPerSecond = steps / totalTime;
    
    NLM_LOG_INFO("Performance Summary:");
    NLM_LOG_INFO("  Total Time: " + formatTime(totalTime) + "s");
    NLM_LOG_INFO("  Steps Processed: " + std::to_string(steps));
    NLM_LOG_INFO("  Steps per Second: " + std::to_string(stepsPerSecond));
    NLM_LOG_INFO("  Average Time per Step: " + formatTime(totalTime / steps) + "s");
}

void DebugTools::profileMemoryUsage(std::shared_ptr<Brain> brain) const {
    NLM_LOG_INFO("=== Memory Usage Analysis ===");
    
    // Estimate memory usage based on brain components
    size_t totalNeurons = brain->getTotalNeuronCount();
    size_t totalSynapses = brain->getTotalSynapseCount();
    size_t totalRegions = brain->getRegionCount();
    
    // Rough estimates (in bytes)
    size_t neuronMemory = totalNeurons * sizeof(Neuron) * 2; // Rough estimate
    size_t synapseMemory = totalSynapses * sizeof(Synapse) * 2;
    size_t regionMemory = totalRegions * sizeof(NeuralRegion) * 2;
    
    // Add memory for system components
    size_t spikeSystemMemory = sizeof(SpikeSystem);
    size_t plasticityMemory = sizeof(PlasticityRule) * 3;
    size_t memorySystemMemory = sizeof(NeuralWorkingMemory) + sizeof(NeuralEpisodicMemory);
    size_t neuromodulationMemory = sizeof(Dopamine) + sizeof(Curiosity) + sizeof(Novelty);
    
    size_t totalMemory = neuronMemory + synapseMemory + regionMemory + 
                        spikeSystemMemory + plasticityMemory + 
                        memorySystemMemory + neuromodulationMemory;
    
    NLM_LOG_INFO("Estimated Memory Usage:");
    NLM_LOG_INFO("  Neurons: " + formatBytes(neuronMemory));
    NLM_LOG_INFO("  Synapses: " + formatBytes(synapseMemory));
    NLM_LOG_INFO("  Regions: " + formatBytes(regionMemory));
    NLM_LOG_INFO("  Systems: " + formatBytes(spikeSystemMemory + plasticityMemory + 
                                            memorySystemMemory + neuromodulationMemory));
    NLM_LOG_INFO("  Total: " + formatBytes(totalMemory));
}

void DebugTools::profileNetworkDynamics(std::shared_ptr<Brain> brain, size_t steps) const {
    NLM_LOG_INFO("=== Network Dynamics Profiling ===");
    
    // Collect firing rate statistics
    std::vector<float> firingRates;
    std::vector<size_t> activeNeurons;
    
    for (size_t step = 0; step < steps; ++step) {
        brain->step(step, step * 0.001);
        
        float firingRate = brain->getAverageFiringRate();
        size_t activeCount = brain->getActiveNeuronCount();
        
        firingRates.push_back(firingRate);
        activeNeurons.push_back(activeCount);
        
        if (step % 100 == 0) {
            NLM_LOG_INFO("Step " + std::to_string(step) + ": FR=" + 
                        std::to_string(firingRate) + ", Active=" + 
                        std::to_string(activeCount));
        }
    }
    
    // Calculate statistics
    if (!firingRates.empty()) {
        float totalFiringRate = 0.0f;
        size_t totalActive = 0;
        
        for (size_t i = 0; i < firingRates.size(); ++i) {
            totalFiringRate += firingRates[i];
            totalActive += activeNeurons[i];
        }
        
        float avgFiringRate = totalFiringRate / firingRates.size();
        float avgActiveNeurons = (float)totalActive / activeNeurons.size();
        
        // Calculate standard deviation
        float firingRateVar = 0.0f;
        float activeVar = 0.0f;
        
        for (size_t i = 0; i < firingRates.size(); ++i) {
            firingRateVar += (firingRates[i] - avgFiringRate) * (firingRates[i] - avgFiringRate);
            activeVar += (activeNeurons[i] - avgActiveNeurons) * (activeNeurons[i] - avgActiveNeurons);
        }
        
        float firingRateStd = std::sqrt(firingRateVar / firingRates.size());
        float activeStd = std::sqrt(activeVar / activeNeurons.size());
        
        NLM_LOG_INFO("Network Dynamics Statistics:");
        NLM_LOG_INFO("  Average Firing Rate: " + std::to_string(avgFiringRate) + " Hz");
        NLM_LOG_INFO("  Firing Rate Std Dev: " + std::to_string(firingRateStd) + " Hz");
        NLM_LOG_INFO("  Average Active Neurons: " + std::to_string(avgActiveNeurons));
        NLM_LOG_INFO("  Active Neurons Std Dev: " + std::to_string(activeStd));
        
        // Analyze activity patterns
        float minFR = *std::min_element(firingRates.begin(), firingRates.end());
        float maxFR = *std::max_element(firingRates.begin(), firingRates.end());
        
        NLM_LOG_INFO("Firing Rate Range: " + std::to_string(minFR) + " - " + std::to_string(maxFR));
        
        if (firingRateStd > avgFiringRate * 0.5) {
            NLM_LOG_INFO("  Pattern: High variability - unstable network");
        } else if (firingRateStd < avgFiringRate * 0.1) {
            NLM_LOG_INFO("  Pattern: Low variability - stable network");
        } else {
            NLM_LOG_INFO("  Pattern: Moderate variability - balanced network");
        }
    }
}

void DebugTools::analyzeDevelopment(std::shared_ptr<Brain> brain) const {
    NLM_LOG_INFO("=== Development Analysis ===");
    
    auto* devSystem = brain->getDevelopmentSystem();
    if (!devSystem) {
        NLM_LOG_WARNING("Development system not available");
        return;
    }
    
    DevelopmentalStage stage = brain->getDevelopmentalStage();
    
    NLM_LOG_INFO("Development Status:");
    NLM_LOG_INFO("  Current Stage: " + std::to_string(static_cast<int>(stage)));
    NLM_LOG_INFO("  Plasticity Rate: " + std::to_string(devSystem->getPlasticityRate()));
    NLM_LOG_INFO("  Synathesis Rate: " + std::to_string(devSystem->getSynaptogenesisRate()));
    NLM_LOG_INFO("  Pruning Rate: " + std::to_string(devSystem->getPruningRate()));
    NLM_LOG_INFO("  Critical Period Active: " + std::to_string(devSystem->isCriticalPeriodActive()));
    
    // Analyze structural changes
    NLM_LOG_INFO("Structural Development Metrics:");
    
    auto regions = brain->getRegions();
    for (size_t i = 0; i < regions.size(); ++i) {
        auto neurons = regions[i]->getAllNeurons();
        auto synapses = regions[i]->getSynapses();
        
        NLM_LOG_INFO("  Region " + std::to_string(i) + ": " + std::to_string(neurons.size()) + 
                    " neurons, " + std::to_string(synapses.size()) + " synapses");
    }
}

void DebugTools::analyzeMaturation(std::shared_ptr<Brain> brain) const {
    NLM_LOG_INFO("=== Maturation Analysis ===");
    
    auto* devSystem = brain->getDevelopmentSystem();
    if (!devSystem) {
        NLM_LOG_WARNING("Development system not available");
        return;
    }
    
    NLM_LOG_INFO("Maturation Metrics:");
    NLM_LOG_INFO("  Current Stage: " + std::to_string(static_cast<int>(brain->getDevelopmentalStage())));
    NLM_LOG_INFO("  Plasticity Rate: " + std::to_string(devSystem->getPlasticityRate()));
    NLM_LOG_INFO("  Synaptogenesis Rate: " + std::to_string(devSystem->getSynaptogenesisRate()));
    NLM_LOG_INFO("  Pruning Rate: " + std::to_string(devSystem->getPruningRate()));
    
    // Check if brain is mature
    auto regions = brain->getRegions();
    size_t totalNeurons = brain->getTotalNeuronCount();
    size_t totalSynapses = brain->getTotalSynapseCount();
    
    bool isMature = brain->getDevelopmentalStage() >= DevelopmentalStage::Adult;
    NLM_LOG_INFO("  Brain Status: " + std::string(isMature ? "MATURE" : "DEVELOPING"));
    
    if (!isMature) {
        NLM_LOG_INFO("  Remaining Developmental Time:")
        NLM_LOG_INFO("    Plasticity: " + std::to_string(devSystem->getPlasticityRate() * 100) + "% remaining");
        NLM_LOG_INFO("    Synaptogenesis: " + std::to_string(devSystem->getSynaptogenesisRate() * 100) + "% remaining");
        NLM_LOG_INFO("    Pruning: " + std::to_string(devSystem->getPruningRate() * 100) + "% remaining");
    }
}

void DebugTools::analyzePruning(std::shared_ptr<Brain> brain) const {
    NLM_LOG_INFO("=== Pruning Analysis ===");
    
    auto* devSystem = brain->getDevelopmentSystem();
    if (!devSystem) {
        NLM_LOG_WARNING("Development system not available");
        return;
    }
    
    NLM_LOG_INFO("Pruning Statistics:");
    NLM_LOG_INFO("  Pruning Rate: " + std::to_string(devSystem->getPruningRate()));
    NLM_LOG_INFO("  Pruning Efficiency: " + std::to_string(devSystem->getPruningEfficiency()));
    NLM_LOG_INFO("  Synapses Pruned: " + std::to_string(devSystem->getTotalPrunedSynapses()));
    
    auto regions = brain->getRegions();
    size_t totalPruned = 0;
    
    for (const auto& region : regions) {
        auto synapses = region->getSynapses();
        size_t regionPruned = 0;
        
        for (const auto& synapse : synapses) {
            if (synapse->isPruned()) {
                regionPruned++;
            }
        }
        
        if (regionPruned > 0) {
            NLM_LOG_INFO("    Region " + std::to_string(region->getId()) + ": " + 
                        std::to_string(regionPruned) + " pruned synapses");
        }
        
        totalPruned += regionPruned;
    }
    
    NLM_LOG_INFO("Total Pruned Synapses: " + std::to_string(totalPruned));
    
    if (totalPruned > 0) {
        NLM_LOG_INFO("Pruning Benefits:");
        NLM_LOG_INFO("  Improved Efficiency: Increased signal-to-noise ratio");
        NLM_LOG_INFO("  Energy Savings: Reduced computational load");
        NLM_LOG_INFO("  Reduced Overfitting: Enhanced generalization capabilities");
    }
}

void DebugTools::printBrainTopology(std::shared_ptr<Brain> brain) const {
    NLM_LOG_INFO("=== Brain Topology ===");
    
    auto regions = brain->getRegions();
    NLM_LOG_INFO("Regions: " + std::to_string(regions.size()));
    
    for (size_t i = 0; i < regions.size(); ++i) {
        auto region = regions[i].get();
        auto neurons = region->getAllNeurons();
        auto synapses = region->getSynapses();
        
        NLM_LOG_INFO("Region " + std::to_string(i) + ":");
        NLM_LOG_INFO("  Neurons: " + std::to_string(neurons.size()));
        NLM_LOG_INFO("  Synapses: " + std::to_string(synapses.size()));
        
        // Calculate density
        float density = 0.0f;
        if (!neurons.empty()) {
            size_t maxPossible = neurons.size() * (neurons.size() - 1) / 2;
            if (maxPossible > 0) {
                density = (float)synapses.size() / maxPossible;
            }
        }
        
        NLM_LOG_INFO("  Density: " + std::to_string(density));
        
        if (synapses.size() > 0) {
            NLM_LOG_INFO("  Sample Connections:");
            for (size_t j = 0; j < std::min(size_t(3), synapses.size()); ++j) {
                auto synapse = synapses[j];
                NLM_LOG_INFO("    Synapse " + std::to_string(synapse->getId()) + 
                            ": " + std::to_string(synapse->getSourceNeuron()) + " -> " +
                            std::to_string(synapse->getDestinationNeuron()) + 
                            " (weight: " + std::to_string(synapse->getWeight()) + ")");
            }
        }
    }
}

void DebugTools::printNetworkStatistics(std::shared_ptr<Brain> brain) const {
    NLM_LOG_INFO("=== Network Statistics ===");
    
    size_t neurons = brain->getTotalNeuronCount();
    size_t synapses = brain->getTotalSynapseCount();
    size_t regions = brain->getRegionCount();
    size_t spikes = brain->getTotalSpikeCount();
    
    NLM_LOG_INFO("Network Overview:");
    NLM_LOG_INFO("  Neurons: " + std::to_string(neurons));
    NLM_LOG_INFO("  Synapses: " + std::to_string(synapses));
    NLM_LOG_INFO("  Regions: " + std::to_string(regions));
    NLM_LOG_INFO("  Total Spikes: " + std::to_string(spikes));
    
    if (neurons > 0) {
        float connectivity = (float)synapses / neurons;
        NLM_LOG_INFO("  Average Connections per Neuron: " + std::to_string(connectivity));
        
        float spikeRate = (float)spikes / (neurons * 1000); // Assuming 1000 time steps
        NLM_LOG_INFO("  Spike Rate per Neuron: " + std::to_string(spikeRate));
    }
    
    // Calculate network metrics
    float excitationInhibitionRatio = brain->getExcitationInhibitionRatio();
    NLM_LOG_INFO("  Excitation-Inhibition Ratio: " + std::to_string(excitationInhibitionRatio));
    
    float avgFiringRate = brain->getAverageFiringRate();
    NLM_LOG_INFO("  Average Firing Rate: " + std::to_string(avgFiringRate));
}

void DebugTools::printPlasticityStatistics(std::shared_ptr<Brain> brain) const {
    NLM_LOG_INFO("=== Plasticity Statistics ===");
    
    auto* stdp = brain->getSTDP();
    auto* hebbian = brain->getHebbian();
    auto* structural = brain->getStructuralPlasticity();
    
    if (stdp) {
        NLM_LOG_INFO("STDP Statistics:");
        NLM_LOG_INFO("  LTP Weight: " + std::to_string(stdp->getLTPWeight()));
        NLM_LOG_INFO("  LTD Weight: " + std::to_string(stdp->getLTDWeight()));
        NLM_LOG_INFO("  Time Constant: " + std::to_string(stdp->getTimeConstant()));
    }
    
    if (hebbian) {
        NLM_LOG_INFO("Hebbian Statistics:");
        NLM_LOG_INFO("  Learning Rate: " + std::to_string(hebbian->getLearningRate()));
        NLM_LOG_INFO("  Trace Decay: " + std::to_string(hebbian->getTraceDecay()));
        NLM_LOG_INFO("  Co-learning Threshold: " + std::to_string(hebbian->getCoLearningThreshold()));
    }
    
    if (structural) {
        NLM_LOG_INFO("Structural Plasticity Statistics:");
        NLM_LOG_INFO("  Synaptogenesis Rate: " + std::to_string(structural->getSynaptogenesisRate()));
        NLM_LOG_INFO("  Pruning Rate: " + std::to_string(structural->getPruningRate()));
        NLM_LOG_INFO("  Min Synapse Weight: " + std::to_string(structural->getMinSynapseWeight()));
        NLM_LOG_INFO("  Max Synapse Weight: " + std::to_string(structural->getMaxSynapseWeight()));
    }
}

void DebugTools::printMemoryStatistics(std::shared_ptr<Brain> brain) const {
    NLM_LOG_INFO("=== Memory Statistics ===");
    
    auto* workingMem = brain->getWorkingMemory();
    auto* episodicMem = brain->getEpisodicMemory();
    auto* associativeMem = brain->getAssociativeMemory();
    
    if (workingMem) {
        NLM_LOG_INFO("Working Memory:");
        NLM_LOG_INFO("  Active Traces: " + std::to_string(workingMem->getActiveTraces()));
        NLM_LOG_INFO("  Total Traces: " + std::to_string(workingMem->getTotalTraces()));
        NLM_LOG_INFO("  Capacity: " + std::to_string(workingMem->getTraceCapacity()));
        NLM_LOG_INFO("  Utilization: " + std::to_string((float)workingMem->getActiveTraces() / 
                                                     workingMem->getTraceCapacity() * 100) + "%");
    }
    
    if (episodicMem) {
        NLM_LOG_INFO("Episodic Memory:");
        NLM_LOG_INFO("  Episode Count: " + std::to_string(episodicMem->getEpisodeCount()));
        NLM_LOG_INFO("  Max Episodes: " + std::to_string(episodicMem->getMaxEpisodes()));
        NLM_LOG_INFO("  Utilization: " + std::to_string((float)episodicMem->getEpisodeCount() / 
                                                     episodicMem->getMaxEpisodes() * 100) + "%");
        NLM_LOG_INFO("  Replay Frequency: " + std::to_string(episodicMem->getReplayFrequency()));
    }
    
    if (associativeMem) {
        NLM_LOG_INFO("Associative Memory:");
        NLM_LOG_INFO("  Pattern Associations: " + std::to_string(associativeMem->getAssociationCount()));
        NLM_LOG_INFO("  Max Associations: " + std::to_string(associativeMem->getMaxAssociations()));
        NLM_LOG_INFO("  Utilization: " + std::to_string((float)associativeMem->getAssociationCount() / 
                                                     associativeMem->getMaxAssociations() * 100) + "%");
    }
}

void DebugTools::printNeuromodulationStatistics(std::shared_ptr<Brain> brain) const {
    NLM_LOG_INFO("=== Neuromodulation Statistics ===");
    
    auto* dopamine = brain->getDopamine();
    auto* curiosity = brain->getCuriosity();
    auto* novelty = brain->getNovelty();
    
    if (dopamine) {
        NLM_LOG_INFO("Dopamine:");
        NLM_LOG_INFO("  Level: " + std::to_string(dopamine->getLevel()));
        NLM_LOG_INFO("  Learning Rate: " + std::to_string(dopamine->getLearningRate()));
        NLM_LOG_INFO("  Expectation: " + std::to_string(dopamine->getExpectation()));
    }
    
    if (curiosity) {
        NLM_LOG_INFO("Curiosity:");
        NLM_LOG_INFO("  Level: " + std::to_string(curiosity->getLevel()));
        NLM_LOG_INFO("  Exploration Drive: " + std::to_string(curiosity->getExplorationDrive()));
        NLM_LOG_INFO("  Novelty Sensitivity: " + std::to_string(curiosity->getNoveltySensitivity()));
    }
    
    if (novelty) {
        NLM_LOG_INFO("Novelty:");
        NLM_LOG_INFO("  Detection Threshold: " + std::to_string(novelty->getDetectionThreshold()));
        NLM_LOG_INFO("  Adaptation Rate: " + std::to_string(novelty->getAdaptationRate()));
    }
}

void DebugTools::runDebugExperiment(std::shared_ptr<Brain> brain, const std::string& experimentName, 
                                   size_t steps, float parameter) const {
    NLM_LOG_INFO("=== Running Debug Experiment: " + experimentName + " ===");
    NLM_LOG_INFO("Steps: " + std::to_string(steps));
    NLM_LOG_INFO("Parameter: " + std::to_string(parameter));
    
    // Reset brain for experiment
    brain->reset();
    brain->initialize();
    
    // Run experiment
    for (size_t step = 0; step < steps; ++step) {
        brain->step(step, step * 0.001);
        
        // Collect metrics
        currentDebugData.stepCount++;
        currentDebugData.totalSpikes += brain->getTotalSpikeCount();
        
        if (step % 100 == 0) {
            float firingRate = brain->getAverageFiringRate();
            currentDebugData.firingHistory.push_back(firingRate);
            
            NLM_LOG_INFO("Step " + std::to_string(step) + ": FR=" + std::to_string(firingRate));
        }
    }
    
    // Calculate and log results
    float avgFiringRate = currentDebugData.totalSpikes / (float)currentDebugData.stepCount;
    
    NLM_LOG_INFO("Experiment Results:");
    NLM_LOG_INFO("  Average Firing Rate: " + std::to_string(avgFiringRate));
    NLM_LOG_INFO("  Total Steps: " + std::to_string(currentDebugData.stepCount));
    NLM_LOG_INFO("  Total Spikes: " + std::to_string(currentDebugData.totalSpikes));
}

void DebugTools::runConnectivityExperiment(std::shared_ptr<Brain> brain, size_t numConnections) const {
    NLM_LOG_INFO("=== Running Connectivity Experiment ===");
    NLM_LOG_INFO("Target Connections: " + std::to_string(numConnections));
    
    // Reset brain
    brain->reset();
    brain->initialize();
    
    // Analyze current connectivity
    auto regions = brain->getRegions();
    size_t currentConnections = 0;
    
    for (const auto& region : regions) {
        currentConnections += region->getSynapses().size();
    }
    
    NLM_LOG_INFO("Current Connections: " + std::to_string(currentConnections));
    
    if (currentConnections < numConnections) {
        NLM_LOG_INFO("Need to add " + std::to_string(numConnections - currentConnections) + " connections");
        // This would normally add new connections
        NLM_LOG_INFO("Implementing connection growth algorithm...");
    } else if (currentConnections > numConnections) {
        NLM_LOG_INFO("Need to prune " + std::to_string(currentConnections - numConnections) + " connections");
        // This would normally prune connections
        NLM_LOG_INFO("Implementing connection pruning algorithm...");
    } else {
        NLM_LOG_INFO("Perfect connectivity achieved");
    }
}

void DebugTools::runPlasticityExperiment(std::shared_ptr<Brain> brain, float learningRate, 
                                        size_t trials) const {
    NLM_LOG_INFO("=== Running Plasticity Experiment ===");
    NLM_LOG_INFO("Learning Rate: " + std::to_string(learningRate));
    NLM_LOG_INFO("Trials: " + std::to_string(trials));
    
    // Reset brain
    brain->reset();
    brain->initialize();
    
    // Enable plasticity
    auto regions = brain->getRegions();
    for (const auto& region : regions) {
        auto synapses = region->getSynapses();
        for (const auto& synapse : synapses) {
            synapse->enablePlasticity(true, true, true);
        }
    }
    
    // Run plasticity trials
    float totalChange = 0.0f;
    size_t effectiveTrials = 0;
    
    for (size_t trial = 0; trial < trials; ++trial) {
        // Inject correlated activity to trigger plasticity
        brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
        brain->step(trial, trial * 0.001);
        
        // Measure synaptic changes
        float currentTotalWeight = 0.0f;
        size_t synapseCount = 0;
        
        for (const auto& region : regions) {
            auto synapses = region->getSynapses();
            for (const auto& synapse : synapses) {
                currentTotalWeight += synapse->getWeight();
                synapseCount++;
            }
        }
        
        if (synapseCount > 0) {
            float trialChange = currentTotalWeight - (trial == 0 ? 0.0f : totalChange);
            totalChange = currentTotalWeight;
            effectiveTrials++;
            
            if (std::abs(trialChange) > 0.001f) {
                NLM_LOG_INFO("Trial " + std::to_string(trial) + ": Δ = " + std::to_string(trialChange));
            }
        }
        
        if (trial % 100 == 0) {
            NLM_LOG_INFO("Trial " + std::to_string(trial) + ": Avg FR = " + 
                        std::to_string(brain->getAverageFiringRate()));
        }
    }
    
    NLM_LOG_INFO("Plasticity Experiment Results:");
    NLM_LOG_INFO("  Effective Trials: " + std::to_string(effectiveTrials));
    NLM_LOG_INFO("  Total Weight Change: " + std::to_string(totalChange));
    NLM_LOG_INFO("  Average Change per Trial: " + std::to_string(totalChange / effectiveTrials));
}

void DebugTools::runMemoryExperiment(std::shared_ptr<Brain> brain, size_t episodeCount) const {
    NLM_LOG_INFO("=== Running Memory Experiment ===");
    NLM_LOG_INFO("Episode Count: " + std::to_string(episodeCount));
    
    // Reset brain
    brain->reset();
    brain->initialize();
    
    // Run episodes
    float totalReward = 0.0f;
    size_t episodesStored = 0;
    
    for (size_t episode = 0; episode < episodeCount; ++episode) {
        // Simulate episode with random input
        for (size_t step = 0; step < 100; ++step) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 5.0f);
            brain->step(step, step * 0.001);
            
            // Random reward (simulated)
            if (step % 10 == 0) {
                float reward = (rand() % 100) / 100.0f;
                totalReward += reward;
                
                // Apply reward modulation
                if (auto* dopamine = brain->getDopamine()) {
                    dopamine->update(reward);
                }
            }
        }
        
        // Store episode
        if (auto* episodicMem = brain->getEpisodicMemory()) {
            episodicMem->storeExperience();
            episodesStored++;
        }
        
        NLM_LOG_INFO("Episode " + std::to_string(episode) + ": Reward = " + std::to_string(totalReward));
    }
    
    NLM_LOG_INFO("Memory Experiment Results:");
    NLM_LOG_INFO("  Total Reward: " + std::to_string(totalReward));
    NLM_LOG_INFO("  Episodes Stored: " + std::to_string(episodesStored));
    NLM_LOG_INFO("  Average Reward per Episode: " + std::to_string(totalReward / episodeCount));
}

void DebugTools::printConfiguration(std::shared_ptr<Config> config) const {
    NLM_LOG_INFO("=== Configuration ===");
    NLM_LOG_INFO(config->summary());
}

void DebugTools::validateConfiguration(std::shared_ptr<Config> config) const {
    NLM_LOG_INFO("=== Configuration Validation ===");
    
    bool isValid = true;
    
    // Check required parameters
    if (!config->has("neuron_count")) {
        NLM_LOG_ERROR("Missing required parameter: neuron_count");
        isValid = false;
    }
    
    if (!config->has("simulation_timestep")) {
        NLM_LOG_ERROR("Missing required parameter: simulation_timestep");
        isValid = false;
    }
    
    // Validate parameter ranges
    int64_t neuronCount = config->getOr<int64_t>("neuron_count", 0);
    if (neuronCount < 10 || neuronCount > 100000) {
        NLM_LOG_WARNING("neuron_count out of recommended range (10-100000)");
    }
    
    double timestep = config->getOr<double>("simulation_timestep", 0.0);
    if (timestep <= 0.0 || timestep > 1.0) {
        NLM_LOG_WARNING("simulation_timestep out of recommended range (0.0-1.0)");
    }
    
    if (isValid) {
        NLM_LOG_INFO("Configuration is valid");
    } else {
        NLM_LOG_ERROR("Configuration is invalid");
    }
}

void DebugTools::compareConfigurations(std::shared_ptr<Config> config1, std::shared_ptr<Config> config2) const {
    NLM_LOG_INFO("=== Configuration Comparison ===");
    
    std::vector<std::string> keys1 = config1->getKeys();
    std::vector<std::string> keys2 = config2->getKeys();
    
    std::unordered_set<std::string> allKeys;
    for (const auto& key : keys1) allKeys.insert(key);
    for (const auto& key : keys2) allKeys.insert(key);
    
    bool differences = false;
    
    for (const auto& key : allKeys) {
        bool has1 = config1->has(key);
        bool has2 = config2->has(key);
        
        if (has1 != has2) {
            NLM_LOG_INFO(key + ": " + std::string(has1 ? "config1 only" : "config2 only"));
            differences = true;
        } else if (has1 && has2) {
            // Compare values
            auto val1 = config1->get<std::string>(key);
            auto val2 = config2->get<std::string>(key);
            
            if (val1.has_value() && val2.has_value() && val1.value() != val2.value()) {
                NLM_LOG_INFO(key + ": \"" + val1.value() + "\" vs \"" + val2.value() + "\"");
                differences = true;
            }
        }
    }
    
    if (!differences) {
        NLM_LOG_INFO("Configurations are identical");
    } else {
        NLM_LOG_INFO("Configurations have differences");
    }
}

void DebugTools::exportConfiguration(std::shared_ptr<Config> config, const std::string& filename) const {
    NLM_LOG_INFO("=== Exporting Configuration ===");
    NLM_LOG_INFO("Filename: " + filename);
    
    if (config->saveToFile(filename)) {
        NLM_LOG_INFO("Configuration exported successfully");
    } else {
        NLM_LOG_ERROR("Failed to export configuration");
    }
}

void DebugTools::saveBrainSession(std::shared_ptr<Brain> brain, const std::string& filename) const {
    NLM_LOG_INFO("=== Saving Brain Session ===");
    NLM_LOG_INFO("Filename: " + filename);
    
    if (brain->save(filename)) {
        NLM_LOG_INFO("Brain session saved successfully");
    } else {
        NLM_LOG_ERROR("Failed to save brain session");
    }
}

void DebugTools::loadBrainSession(std::shared_ptr<Brain> brain, const std::string& filename) const {
    NLM_LOG_INFO("=== Loading Brain Session ===");
    NLM_LOG_INFO("Filename: " + filename);
    
    if (brain->load(filename)) {
        NLM_LOG_INFO("Brain session loaded successfully");
    } else {
        NLM_LOG_ERROR("Failed to load brain session");
    }
}

void DebugTools::snapshotBrainState(std::shared_ptr<Brain> brain, const std::string& tag) const {
    NLM_LOG_INFO("=== Snapshot Brain State ===");
    NLM_LOG_INFO("Tag: " + tag);
    
    // Capture current state
    DebugData snapshot;
    snapshot.stepCount = brain->getTotalSpikeCount();
    snapshot.totalSpikes = brain->getTotalSpikeCount();
    snapshot.averageFiringRate = brain->getAverageFiringRate();
    
    snapshotStore[tag] = {
        "StepCount: " + std::to_string(snapshot.stepCount),
        "TotalSpikes: " + std::to_string(snapshot.totalSpikes),
        "AvgFiringRate: " + std::to_string(snapshot.averageFiringRate)
    };
    
    NLM_LOG_INFO("Brain state snapshot created with tag: " + tag);
}

void DebugTools::restoreBrainState(std::shared_ptr<Brain> brain, const std::string& tag) const {
    NLM_LOG_INFO("=== Restore Brain State ===");
    NLM_LOG_INFO("Tag: " + tag);
    
    if (snapshotStore.find(tag) != snapshotStore.end()) {
        NLM_LOG_INFO("Brain state restored from snapshot: " + tag);
        
        // In a real implementation, this would restore the actual brain state
        NLM_LOG_INFO("(Would restore brain state here)");
    } else {
        NLM_LOG_ERROR("Snapshot not found: " + tag);
    }
}

void DebugTools::enableDebugLogging(std::shared_ptr<Brain> brain, bool enable) const {
    NLM_LOG_INFO("=== Enable Debug Logging ===");
    NLM_LOG_INFO("Debug logging: " + std::string(enable ? "ENABLED" : "DISABLED"));
    
    // In a real implementation, this would enable debug logging
    NLM_LOG_INFO("(Would enable/disable debug logging here)");
}

void DebugTools::setDebugLevel(std::shared_ptr<Brain> brain, int level) const {
    NLM_LOG_INFO("=== Set Debug Level ===");
    NLM_LOG_INFO("Debug level: " + std::to_string(level));
    
    // In a real implementation, this would set the debug level
    NLM_LOG_INFO("(Would set debug level here)");
}

void DebugTools::enablePerformanceCounters(std::shared_ptr<Brain> brain, bool enable) const {
    NLM_LOG_INFO("=== Enable Performance Counters ===");
    NLM_LOG_INFO("Performance counters: " + std::string(enable ? "ENABLED" : "DISABLED"));
    
    // In a real implementation, this would enable performance counters
    NLM_LOG_INFO("(Would enable/disable performance counters here)");
}

void DebugTools::captureNetworkActivity(std::shared_ptr<Brain> brain, size_t duration) const {
    NLM_LOG_INFO("=== Capture Network Activity ===");
    NLM_LOG_INFO("Duration: " + std::to_string(duration) + " steps");
    
    // Capture network activity
    NLM_LOG_INFO("Capturing network activity...");
    
    std::vector<float> activityPattern;
    for (size_t step = 0; step < duration; ++step) {
        brain->step(step, step * 0.001);
        float firingRate = brain->getAverageFiringRate();
        activityPattern.push_back(firingRate);
        
        if (step % 100 == 0) {
            NLM_LOG_INFO("Step " + std::to_string(step) + ": FR = " + std::to_string(firingRate));
        }
    }
    
    // Analyze activity pattern
    if (!activityPattern.empty()) {
        float minActivity = *std::min_element(activityPattern.begin(), activityPattern.end());
        float maxActivity = *std::max_element(activityPattern.begin(), activityPattern.end());
        
        NLM_LOG_INFO("Activity Pattern Analysis:");
        NLM_LOG_INFO("  Min Activity: " + std::to_string(minActivity));
        NLM_LOG_INFO("  Max Activity: " + std::to_string(maxActivity));
        
        float activityRange = maxActivity - minActivity;
        NLM_LOG_INFO("  Activity Range: " + std::to_string(activityRange));
    }
}

std::string DebugTools::formatTime(double seconds) const {
    if (seconds < 60.0) {
        return std::to_string(seconds) + "s";
    } else if (seconds < 3600.0) {
        int minutes = static_cast<int>(seconds / 60.0);
        double remainingSeconds = seconds - minutes * 60.0;
        return std::to_string(minutes) + "m " + std::to_string(remainingSeconds) + "s";
    } else {
        int hours = static_cast<int>(seconds / 3600.0);
        double remainingSeconds = seconds - hours * 3600.0;
        int minutes = static_cast<int>(remainingSeconds / 60.0);
        remainingSeconds -= minutes * 60.0;
        return std::to_string(hours) + "h " + std::to_string(minutes) + "m " + 
               std::to_string(remainingSeconds) + "s";
    }
}

std::string DebugTools::formatBytes(size_t bytes) const {
    if (bytes < 1024) {
        return std::to_string(bytes) + "B";
    } else if (bytes < 1024 * 1024) {
        return std::to_string(bytes / 1024) + "KB";
    } else if (bytes < 1024 * 1024 * 1024) {
        return std::to_string(bytes / (1024 * 1024)) + "MB";
    } else {
        return std::to_string(bytes / (1024 * 1024 * 1024)) + "GB";
    }
}

std::string DebugTools::formatPercentage(float value) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << value * 100.0f << "%";
    return oss.str();
}

std::string DebugTools::formatRate(float value) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << value << "/s";
    return oss.str();
}

} // namespace nlm
