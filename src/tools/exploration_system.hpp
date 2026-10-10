// NLM Advanced CLI - Exploration System Implementation
#pragma once

#include "tools/exploration_system.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <random>
#include <chrono>
#include <vector>
#include <algorithm>

namespace nlm {

exploration_system::exploration_system(std::shared_ptr<Brain> brain)
    : brain_(brain) {
    // Initialize random number generator
    rng_.seed(std::chrono::system_clock::now().time_since_epoch().count());
}

exploration_system::~exploration_system() = default;

bool exploration_system::randomExploration(int steps, float intensity) {
    if (!brain_) {
        std::cout << "Error: No brain provided for exploration." << std::endl;
        return false;
    }
    
    std::cout << "=== Random Exploration ===\n" << std::endl;
    std::cout << "Running random exploration for " << steps << " steps with intensity: " << intensity << std::endl;
    std::cout << "(This will generate random sensory inputs and motor outputs)\n" << std::endl;
    
    // Generate random exploration behavior
    std::uniform_int_distribution<int> neuronDistribution(0, brain_->getTotalNeuronCount() - 1);
    std::uniform_real_distribution<float> currentDistribution(0.0f, 1.0f);
    std::uniform_real_distribution<float> currentMagnitude(0.0f, 50.0f);
    
    for (int step = 0; step < steps && globalState.running; ++step) {
        // Randomly select and activate neurons
        int randomNeuron = neuronDistribution(rng_);
        float randomCurrent = currentMagnitude(rng_) * intensity;
        
        // Inject random current into random neurons
        if (brain_->getTotalNeuronCount() > 0) {
            brain_->injectCurrentToNeurons(static_cast<NeuronType>(rng_() % 4), randomCurrent);
        }
        
        // Step the brain
        brain_->step(step, step * 0.001);
        
        // Log progress periodically
        if (step % 100 == 0) {
            std::cout << "  Step " << step << ": " << brain_->getFiringNeuronCount() << " firing neurons\n" << std::endl;
        }
        
        // Sleep briefly to avoid overwhelming the system
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        
        if (globalState.debugTools) {
            globalState.debugTools->logEvent("Exploration step " + std::to_string(step) + 
                                           " with intensity " + std::to_string(intensity));
        }
    }
    
    std::cout << "\nRandom exploration completed.\n" << std::endl;
    std::cout << "Steps executed: " << steps << std::endl;
    std::cout << "Final firing neurons: " << brain_->getFiringNeuronCount() << std::endl;
    std::cout << "Final total spikes: " << brain_->getTotalSpikeCount() << std::endl;
    
    return true;
}

bool exploration_system::curiosityDrivenExploration(int steps, float baseCuriosity) {
    std::cout << "=== Curiosity-Driven Exploration ===\n" << std::endl;
    std::cout << "Running curiosity-driven exploration for " << steps << " steps\n" << std::endl;
    
    if (!brain_) {
        std::cout << "Error: No brain provided for exploration." << std::endl;
        return false;
    }
    
    std::uniform_real_distribution<float> noveltyDistribution(0.0f, 1.0f);
    std::uniform_real_distribution<float> curiosityMagnitude(baseCuriosity, baseCuriosity * 2.0f);
    
    for (int step = 0; step < steps && globalState.running; ++step) {
        // Calculate curiosity level based on novelty
        float novelty = noveltyDistribution(rng_);
        float curiosity = curiosityMagnitude(rng_);
        
        // Adjust brain state based on curiosity
        if (curiosity > 0.5f) {
            // High curiosity - encourage exploration
            int randomNeuron = std::uniform_int_distribution<int>(0, brain_->getTotalNeuronCount() - 1)(rng_);
            float explorationCurrent = 30.0f * curiosity;
            brain_->injectCurrentToNeurons(NeuronType::Internal, explorationCurrent);
        }
        
        // Step the brain
        brain_->step(step, step * 0.001);
        
        // Log curiosity level periodically
        if (step % 50 == 0) {
            std::cout << "  Step " << step << ": Novelty " << novelty << ", Curiosity " << curiosity << std::endl;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    std::cout << "\nCuriosity-driven exploration completed.\n" << std::endl;
    
    return true;
}

bool exploration_system::noveltyBasedExploration(int steps) {
    std::cout << "=== Novelty-Based Exploration ===\n" << std::endl;
    std::cout << "Running novelty-driven exploration for " << steps << " steps\n" << std::endl;
    
    if (!brain_) {
        std::cout << "Error: No brain provided for exploration." << std::endl;
        return false;
    }
    
    std::uniform_real_distribution<float> patternDistribution(0.0f, 1.0f);
    
    for (int step = 0; step < steps && globalState.running; ++step++) {
        // Generate novel neural activity patterns
        float noveltyThreshold = 0.7f + (patternDistribution(rng_) * 0.3f);
        
        // Activate neurons based on novelty threshold
        if (brain_->getFiringNeuronCount() < brain_->getTotalNeuronCount() * noveltyThreshold) {
            // Increase activity to reach novelty threshold
            int neuronsToActivate = std::min(static_cast<int>(brain_->getTotalNeuronCount() * (noveltyThreshold - 
                                            static_cast<float>(brain_->getFiringNeuronCount()) / brain_->getTotalNeuronCount())),
                                           static_cast<int>(brain_->getTotalNeuronCount()) / 10);
            
            if (neuronsToActivate > 0) {
                for (int i = 0; i < neuronsToActivate; ++i) {
                    int neuronId = std::uniform_int_distribution<int>(0, brain_->getTotalNeuronCount() - 1)(rng_);
                    brain_->injectCurrentToNeuron(neuronId, 20.0f);
                }
            }
        }
        
        // Step the brain
        brain_->step(step, step * 0.001);
        
        if (step % 100 == 0) {
            std::cout << "  Step " << step << ": Novelty-driven activity" << std::endl;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    std::cout << "\nNovelty-based exploration completed.\n" << std::endl;
    
    return true;
}

exploration_system::exploration_system() : brain_(nullptr) {
    rng_.seed(std::chrono::system_clock::now().time_since_epoch().count());
}

} // namespace nlm

