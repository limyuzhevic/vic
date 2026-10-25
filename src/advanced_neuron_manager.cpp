// ADVANCED NEURON MANAGEMENT IMPLEMENTATION

#include "advanced_neuron_manager.h"
#include "core/Logger/Logger.hpp"
#include <random>
#include <algorithm>
#include <numeric>
#include <cmath>

using namespace nlm;

// Initialize random device for various random operations
static std::random_device rd;
static std::mt19937 gen(rd());

// Helper functions for advanced neuron management
namespace AdvancedNeuronUtils {
    
    float calculateFiringRate(const Neuron* neuron) {
        if (!neuron) return 0.0f;
        return neuron->getFiringRate();
    }
    
    float calculateMembraneStability(const Neuron* neuron) {
        if (!neuron) return 0.0f;
        float v = neuron->getMembranePotential();
        float threshold = neuron->getThreshold();
        return std::abs(v - threshold) / std::abs(threshold);
    }
    
    bool isNeuronStable(const Neuron* neuron) {
        if (!neuron) return false;
        float v = neuron->getMembranePotential();
        return v > -60.0f && v < -30.0f;  // Stable range
    }
    
    NeuronType determineOptimalType(const Neuron* neuron, const Brain* brain) {
        if (!neuron || !brain) return NeuronType::Internal;
        
        float v = neuron->getMembranePotential();
        float threshold = neuron->getThreshold();
        
        // Determine optimal type based on membrane potential dynamics
        if (v > threshold * 0.8f) {
            return NeuronType::Excitatory;
        } else if (v < threshold * 0.5f) {
            return NeuronType::Inhibitory;
        } else if (std::abs(v - threshold) < 5.0f) {
            return NeuronType::Sensory;
        } else {
            return NeuronType::Internal;
        }
    }
    
    void redistributeNeuronTypes(Brain* brain, float balanceRatio) {
        if (!brain) return;
        
        auto* brainImpl = static_cast<Brain::Impl*>(brain->pImpl);
        
        // Count current type distribution
        std::map<NeuronType, size_t> currentCounts;
        for (auto& region : brainImpl->regions) {
            for (auto* neuron : region->getAllNeurons()) {
                currentCounts[neuron->getType()]++;
            }
        }
        
        // Calculate target distribution based on balance ratio
        size_t totalNeurons = brain->getTotalNeuronCount();
        std::map<NeuronType, size_t> targetCounts;
        
        // Target: 40% excitatory, 30% inhibitory, 20% sensory, 10% internal
        targetCounts[NeuronType::Excitatory] = static_cast<size_t>(totalNeurons * 0.4f * balanceRatio);
        targetCounts[NeuronType::Inhibitory] = static_cast<size_t>(totalNeurons * 0.3f * balanceRatio);
        targetCounts[NeuronType::Sensory] = static_cast<size_t>(totalNeurons * 0.2f * balanceRatio);
        targetCounts[NeuronType::Internal] = static_cast<size_t>(totalNeurons * 0.1f * balanceRatio);
        
        // Redistribute neurons to match target distribution
        for (auto& region : brainImpl->regions) {
            auto neurons = region->getAllNeurons();
            if (neurons.empty()) continue;
            
            // Classify neurons by current type
            std::vector<std::pair<NeuronType, Neuron*>> neuronsByType;
            for (auto* neuron : neurons) {
                neuronsByType.emplace_back(neuron->getType(), neuron);
            }
            
            // Sort by type priority for efficient redistribution
            std::sort(neuronsByType.begin(), neuronsByType.end(),
                     [](const auto& a, const auto& b) {
                         return static_cast<int>(a.first) < static_cast<int>(b.first);
                     });
            
            // Redistribute types to match target
            size_t neuronsProcessed = 0;
            for (auto& [type, neuron] : neuronsByType) {
                if (neuronsProcessed >= totalNeurons) break;
                
                // Check if we need to change this neuron's type
                size_t currentCount = currentCounts[type];
                size_t targetCount = targetCounts[type];
                
                if (currentCount > targetCount && balanceRatio > 0.8f) {
                    // Can safely change some neurons
                    NeuronType newType = determineOptimalType(neuron, brain);
                    if (newType != type) {
                        neuron->setType(newType);
                        currentCounts[type]--;
                        currentCounts[newType]++;
                    }
                }
                
                neuronsProcessed++;
            }
        }
    }
    
    std::vector<NeuronId> findTopActiveNeurons(const Brain* brain, size_t count) {
        std::vector<std::pair<float, NeuronId>> neuronActivity;
        
        for (auto& region : brain->getRegions()) {
            for (auto* neuron : region->getAllNeurons()) {
                float activity = neuron->getFiringRate() * std::abs(neuron->getMembranePotential());
                neuronActivity.emplace_back(activity, neuron->getId());
            }
        }
        
        std::partial_sort(neuronActivity.begin(), 
                         neuronActivity.begin() + std::min(count, neuronActivity.size()),
                         neuronActivity.end(), std::greater<>());
        
        std::vector<NeuronId> topNeurons;
        for (size_t i = 0; i < std::min(count, neuronActivity.size()); ++i) {
            topNeurons.push_back(neuronActivity[i].second);
        }
        
        return topNeurons;
    }
    
    void adjustConnectivityBasedOnActivity(Brain* brain, float activityThreshold) {
        if (!brain) return;
        
        auto* brainImpl = static_cast<Brain::Impl*>(brain->pImpl);
        
        for (auto& region : brainImpl->regions) {
            auto neurons = region->getAllNeurons();
            
            // Find highly active neurons
            std::vector<Neuron*> activeNeurons;
            for (auto* neuron : neurons) {
                if (neuron->getFiringRate() > activityThreshold) {
                    activeNeurons.push_back(neuron);
                }
            }
            
            if (activeNeurons.empty()) continue;
            
            // Strengthen connections from active neurons
            for (auto* activeNeuron : activeNeurons) {
                for (auto* target : neurons) {
                    if (activeNeuron == target) continue;
                    
                    auto* syn = region->getSynapse(activeNeuron->getId(), target->getId());
                    if (syn) {
                        // Strengthen connection slightly
                        syn->addToWeight(0.01f);
                    }
                }
            }
        }
    }
}

// Advanced Neuron Manager method implementations

AdvancedNeuronManager::NeuronStats AdvancedNeuronManager::analyzeNeuronPopulation() {
    NeuronStats stats;
    auto* brainImpl = static_cast<Brain::Impl*>(brain->pImpl);
    
    // Collect statistics from all regions
    for (auto& region : brainImpl->regions) {
        auto neurons = region->getAllNeurons();
        stats.totalNeurons += neurons.size();
        
        for (auto* neuron : neurons) {
            // Count by type
            stats.typeCounts[neuron->getType()]++;
            
            // Collect membrane potential statistics
            float v = neuron->getMembranePotential();
            stats.averageMembranePotential += v;
            
            // Collect firing rate statistics
            stats.averageFiringRate += neuron->getFiringRate();
            
            // Check active status
            if (neuron->isFiring()) stats.firingNeuronCount++;
            if (neuron->getMembranePotential() > -40.0f) stats.activeNeuronCount++;
        }
    }
    
    // Calculate averages
    if (stats.totalNeurons > 0) {
        stats.averageMembranePotential /= stats.totalNeurons;
        stats.averageFiringRate /= stats.totalNeurons;
    }
    
    // Calculate membrane potential standard deviation
    float variance = 0.0f;
    for (auto& region : brainImpl->regions) {
        auto neurons = region->getAllNeurons();
        for (auto* neuron : neurons) {
            float v = neuron->getMembranePotential();
            variance += (v - stats.averageMembranePotential) * (v - stats.averageMembranePotential);
        }
    }
    
    if (stats.totalNeurons > 0) {
        stats.membranePotentialStdDev = std::sqrt(variance / stats.totalNeurons);
    }
    
    // Find top active neurons using utility function
    stats.topActiveNeurons = AdvancedNeuronUtils::findTopActiveNeurons(brain.get(), 10);
    
    return stats;
}

std::map<std::string, float> AdvancedNeuronManager::analyzeConnectivityPatterns() {
    std::map<std::string, float> patterns;
    auto* brainImpl = static_cast<Brain::Impl*>(brain->pImpl);
    
    float totalExcitatoryConnections = 0;
    float totalInhibitoryConnections = 0;
    float totalSelfConnections = 0;
    float totalCrossRegionConnections = 0;
    
    for (auto& region : brainImpl->regions) {
        auto neurons = region->getAllNeurons();
        
        for (size_t i = 0; i < neurons.size(); ++i) {
            for (size_t j = 0; j < neurons.size(); ++j) {
                auto* syn = region->getSynapse(neurons[i]->getId(), neurons[j]->getId());
                if (syn) {
                    if (neurons[i]->getId() == neurons[j]->getId()) {
                        totalSelfConnections++;
                    } else {
                        if (syn->isExcitatory()) {
                            totalExcitatoryConnections++;
                        } else {
                            totalInhibitoryConnections++;
                        }
                    }
                }
            }
        }
    }
    
    float totalConnections = totalExcitatoryConnections + totalInhibitoryConnections + totalSelfConnections;
    if (totalConnections > 0) {
        patterns["excitatory_ratio"] = totalExcitatoryConnections / totalConnections;
        patterns["inhibitory_ratio"] = totalInhibitoryConnections / totalConnections;
        patterns["self_connection_ratio"] = totalSelfConnections / totalConnections;
    }
    
    return patterns;
}

void AdvancedNeuronManager::applyNeuronPattern(const std::string& patternName, float parameter) {
    auto* brainImpl = static_cast<Brain::Impl*>(brain->pImpl);
    
    if (patternName == "all_to_excitatory") {
        for (auto& region : brainImpl->regions) {
            for (auto& neuron : region->getAllNeurons()) {
                neuron->setType(NeuronType::Excitatory);
            }
        }
    } else if (patternName == "all_to_inhibitory") {
        for (auto& region : brainImpl->regions) {
            for (auto& neuron : region->getAllNeurons()) {
                neuron->setType(NeuronType::Inhibitory);
            }
        }
    } else if (patternName == "balanced_random") {
        std::uniform_real_distribution<float> dis(0.0f, 1.0f);
        
        for (auto& region : brainImpl->regions) {
            for (auto& neuron : region->getAllNeurons()) {
                float r = dis(gen);
                if (r < 0.3f) neuron->setType(NeuronType::Excitatory);
                else if (r < 0.6f) neuron->setType(NeuronType::Inhibitory);
                else neuron->setType(NeuronType::Sensory);
            }
        }
    } else if (patternName == "modify_threshold") {
        for (auto& region : brainImpl->regions) {
            for (auto& neuron : region->getAllNeurons()) {
                float current = neuron->getThreshold();
                neuron->setThreshold(current * parameter);
            }
        }
    } else if (patternName == "modify_refractory") {
        for (auto& region : brainImpl->regions) {
            for (auto& neuron : region->getAllNeurons()) {
                uint32_t current = neuron->getRefractoryPeriod();
                neuron->setRefractoryPeriod(static_cast<uint32_t>(current * parameter));
            }
        }
    }
}

void AdvancedNeuronManager::runAdaptiveSimulation(size_t steps, PerformanceMonitor& perfMonitor, LearningExperiment& experiment) {
    NLM_LOG_INFO("Running adaptive simulation with dynamic input patterns...");
    
    for (size_t step = 0; step < steps; ++step) {
        // Adaptive input based on network state
        auto stats = analyzeNeuronPopulation();
        float avgFiring = stats.averageFiringRate;
        
        // Adjust input based on current state
        if (avgFiring < 0.5f) {
            // Network is too quiet, increase input
            brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f * (0.5f - avgFiring));
        } else if (avgFiring > 5.0f) {
            // Network is too active, decrease input
            brain->injectCurrentToNeurons(NeuronType::Sensory, -10.0f * (avgFiring - 5.0f));
        }
        
        // Random pattern injection
        if (step % 10 == 0) {
            size_t patternSize = std::min(size_t(50), brain->getTotalNeuronCount() / 10);
            for (size_t i = 0; i < patternSize; ++i) {
                brain->injectCurrentToNeurons(static_cast<NeuronType>(i % 6), 15.0f);
            }
        }
        
        brain->step(step, step * 0.001);
        perfMonitor.recordStep();
        
        printProgress(step + 1, steps, 100);
    }
}

void AdvancedNeuronManager::runStressTestSimulation(size_t steps, PerformanceMonitor& perfMonitor, LearningExperiment& experiment) {
    NLM_LOG_INFO("Running stress test with high activity patterns...");
    
    // Set up all neurons to be highly excitable
    applyNeuronPattern("all_to_excitatory");
    
    for (size_t step = 0; step < steps; ++step) {
        // Strong continuous input
        brain->injectCurrentToNeurons(NeuronType::Sensory, 100.0f);
        
        brain->step(step, step * 0.001);
        perfMonitor.recordStep();
        
        // Monitor for runaway activity
        auto stats = analyzeNeuronPopulation();
        if (stats.firingNeuronCount > stats.totalNeurons * 0.9f) {
            NLM_LOG_INFO("High activity detected: " << stats.firingNeuronCount << "/" << stats.totalNeurons);
        }
        
        printProgress(step + 1, steps, 100);
    }
}

void AdvancedNeuronManager::runLearningSimulation(size_t steps, PerformanceMonitor& perfMonitor, LearningExperiment& experiment) {
    NLM_LOG_INFO("Running learning simulation with structured input patterns...");
    
    // Enable all plasticity for maximum learning
    for (auto& region : brain->getRegions()) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, true);
        }
    }
    
    experiment.recordInitialState();
    
    for (size_t step = 0; step < steps; ++step) {
        // Structured learning patterns
        if (step < steps / 3) {
            // Pattern 1: Sequential activation
            for (size_t i = 0; i < std::min(size_t(20), brain->getTotalNeuronCount() / 50); ++i) {
                brain->injectCurrentToNeurons(static_cast<NeuronType>(i % 3), 30.0f);
            }
        } else if (step < 2 * steps / 3) {
            // Pattern 2: Random correlated activity
            size_t n = std::min(size_t(10), brain->getTotalNeuronCount() / 100);
            for (size_t i = 0; i < n; ++i) {
                size_t type1 = gen() % 3;
                size_t type2 = (type1 + 1) % 3;
                brain->injectCurrentToNeurons(static_cast<NeuronType>(type1), 25.0f);
                brain->injectCurrentToNeurons(static_cast<NeuronType>(type2), 25.0f);
            }
        } else {
            // Pattern 3: Burst activity
            if (step % 5 == 0) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 80.0f);
            }
        }
        
        brain->step(step, step * 0.001);
        perfMonitor.recordStep();
        
        printProgress(step + 1, steps, 100);
    }
}

void AdvancedNeuronManager::runStandardSimulation(size_t steps, PerformanceMonitor& perfMonitor, LearningExperiment& experiment) {
    NLM_LOG_INFO("Running standard simulation with mixed input patterns...");
    
    experiment.recordInitialState();
    
    for (size_t step = 0; step < steps; ++step) {
        // Mixed input patterns
        if (step % 7 == 0) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 40.0f);
        } else if (step % 13 == 0) {
            brain->injectCurrentToNeurons(NeuronType::Motor, 20.0f);
        } else if (step % 17 == 0) {
            brain->injectCurrentToNeurons(NeuronType::Internal, 15.0f);
        }
        
        brain->step(step, step * 0.001);
        perfMonitor.recordStep();
        
        printProgress(step + 1, steps, 100);
    }
}