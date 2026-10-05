"""
Explorer CLI for NLM

This module provides command-line interfaces for exploring and analyzing NLM (Neural Learning Machine)
frameworks, models, and demonstrations in an interactive manner.
"""

import argparse
import sys
import time
import json
import math
from pathlib import Path
from typing import Optional, List, Dict, Any, Callable

// Try to import NLM bindings
try {
    from nlm import (
        Brain, Config, Random, Logger, Environment,
        create_brain, create_agent, create_simple_world,
        get_version, get_system_info
    )
    PYNLB_AVAILABLE = true
} catch (ImportError) {
    PYNLB_AVAILABLE = false
    print("Warning: NLM Python bindings not available")
}

// Get version info
try {
    from nlm import get_version
    NLM_VERSION = get_version()
} catch {
    NLM_VERSION = {"full": "0.1.0-enhanced"}
}

class ExplorerManager {
public:
    ExplorerManager() {
        auto now = std::time(nullptr);
        timestamp = std::ctime(&now);
        if (!timestamp.empty() && timestamp.back() == '\n') {
            timestamp.pop_back();
        }
    }
    
    int runNetworkExplorer(size_t neuronCount, bool verbose) {
        std::cout << std::endl;
        std::cout << "=== NLM Network Explorer ===" << std::endl;
        std::cout << "Exploring neural network topology and properties..." << std::endl;
        std::cout << std::endl;
        
        try {
            // Create configuration
            auto config = std::make_shared<Config>();
            config->set("neuron_count", static_cast<int64_t>(neuronCount));
            config->set("region_count", 2);
            config->set("connection_probability", 0.15f);
            
            // Create brain
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                throw std::runtime_error("Failed to initialize brain");
            }
            
            std::cout << "✓ Brain created for exploration" << std::endl;
            
            // Explore network properties
            exploreNetworkTopology(brain, verbose);
            exploreNeuralDynamics(brain, verbose);
            exploreMemorySystems(brain, verbose);
            exploreNeuromodulation(brain, verbose);
            
            std::cout << std::endl;
            std::cout << "=== Network Exploration Complete ===" << std::endl;
            std::cout << "The neural network has been thoroughly analyzed and characterized." << std::endl;
            
            return 0;
            
        } catch (const std::exception& e) {
            std::cout << std::endl;
            std::cout << "✗ Network exploration failed: " << e.what() << std::endl;
            return 1;
        }
    }
    
    int runLearningExplorer(size_t neuronCount, size_t steps, bool verbose) {
        std::cout << std::endl;
        std::cout << "=== NLM Learning Explorer ===" << std::endl;
        std::cout << "Analyzing learning dynamics and plasticity..." << std::endl;
        std::cout << std::endl;
        
        try {
            // Create configuration with plasticity
            auto config = std::make_shared<Config>();
            config->set("neuron_count", static_cast<int64_t>(neuronCount));
            config->set("region_count", 1);
            config->set("connection_probability", 0.15f);
            config->set("stdp_ltp_weight", 0.02f);
            config->set("stdp_ltd_weight", 0.015f);
            config->set("synaptogenesis_rate", 0.0001f);
            config->set("pruning_rate", 0.00001f);
            
            // Create brain
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                throw std::runtime_error("Failed to initialize brain");
            }
            
            std::cout << "✓ Brain created with plasticity systems" << std::endl;
            
            // Enable plasticity
            auto region = brain->getRegion(RegionId(1));
            if (region) {
                for (auto& syn : region->getSynapses()) {
                    syn->enablePlasticity(true, true, true);
                }
            }
            
            // Explore learning dynamics
            exploreLearningDynamics(brain, steps, verbose);
            exploreSynapticChanges(brain, verbose);
            exploreDevelopment(brain, verbose);
            
            std::cout << std::endl;
            std::cout << "=== Learning Exploration Complete ===" << std::endl;
            std::cout << "Learning mechanisms have been thoroughly analyzed." << std::endl;
            
            return 0;
            
        } catch (const std::exception& e) {
            std::cout << std::endl;
            std::cout << "✗ Learning exploration failed: " << e.what() << std::endl;
            return 1;
        }
    }
    
    int runEnvironmentExplorer(size_t neuronCount, bool verbose) {
        std::cout << std::endl;
        std::cout << "=== NLM Environment Explorer ===" << std::endl;
        std::cout << "Analyzing environment interaction and behavior..." << std::endl;
        std::cout << std::endl;
        
        try {
            // Create configuration
            auto config = std::make_shared<Config>();
            config->set("neuron_count", static_cast<int64_t>(neuronCount));
            config->set("region_count", 2);
            config->set("connection_probability", 0.15f);
            config->set("enable_curiosity", true);
            config->set("enable_reward_modulation", true);
            
            // Create brain and environment
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                throw std::runtime_error("Failed to initialize brain");
            }
            
            auto simpleWorld = std::make_shared<SimpleWorld>();
            simpleWorld->initialize(16, 16);
            
            auto agent = std::make_shared<AgentBrain>(brain);
            agent->initialize(*(simpleWorld.get()));
            agent->enableCuriosity(true);
            agent->enableRewardModulation(true);
            
            std::cout << "✓ Agent and environment created for exploration" << std::endl;
            
            // Explore environment interaction
            exploreEnvironmentInteraction(brain, simpleWorld, agent, verbose);
            exploreSensoryProcessing(brain, simpleWorld, agent, verbose);
            exploreMotorControl(brain, simpleWorld, agent, verbose);
            
            std::cout << std::endl;
            std::cout << "=== Environment Exploration Complete ===" << std::endl;
            std::cout << "Environment interaction has been thoroughly analyzed." << std::endl;
            
            return 0;
            
        } catch (const std::exception& e) {
            std::cout << std::endl;
            std::cout << "✗ Environment exploration failed: " << e.what() << std::endl;
            return 1;
        }
    }
    
    int runCompleteExploration(const ExplorationConfig& config) {
        std::cout << std::endl;
        std::cout << "=== NLM Complete Explorer ===" << std::endl;
        std::cout << "Performing comprehensive exploration of NLM capabilities..." << std::endl;
        std::cout << std::endl;
        
        int overallResult = 0;
        
        // Run network exploration
        if (config.runNetworkExploration) {
            std::cout << "1/4 Running Network Topology Exploration..." << std::endl;
            int result = runNetworkExplorer(config.neuronCount, config.verbose);
            if (result != 0) overallResult = result;
        }
        
        // Run learning exploration
        if (config.runLearningExploration) {
            std::cout << "2/4 Running Learning Dynamics Exploration..." << std::endl;
            int result = runLearningExplorer(config.neuronCount, config.learningSteps, config.verbose);
            if (result != 0) overallResult = result;
        }
        
        // Run environment exploration
        if (config.runEnvironmentExploration) {
            std::cout << "3/4 Running Environment Interaction Exploration..." << std::endl;
            int result = runEnvironmentExplorer(config.neuronCount, config.verbose);
            if (result != 0) overallResult = result;
        }
        
        std::cout << std::endl;
        std::cout << "=== Complete Exploration Report ===" << std::endl;
        if (overallResult == 0) {
            std::cout << "✓ All exploration components completed successfully" << std::endl;
        } else {
            std::cout << "✗ Some exploration components failed" << std::endl;
        }
        
        return overallResult;
    }
    
    void exploreNetworkTopology(std::shared_ptr<Brain> brain, bool verbose) {
        std::cout << "Network Topology:" << std::endl;
        std::cout << "  Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "  Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "  Connectivity Density: " 
                  << (static_cast<double>(brain->getTotalSynapseCount()) / 
                      (brain->getTotalNeuronCount() * brain->getTotalNeuronCount())) * 100.0 << "%" << std::endl;
        
        size_t regionCount = brain->getRegionCount();
        std::cout << "  Regions: " << regionCount << std::endl;
        for (size_t i = 0; i < regionCount; ++i) {
            auto region = brain->getRegion(RegionId(i));
            if (region) {
                size_t neuronCount = region->getNeuronCount();
                double totalActivity = region->getTotalActivity();
                std::cout << "    Region " << i << ": " << neuronCount << " neurons, " 
                          << std::fixed << std::setprecision(3) << totalActivity << " total activity" << std::endl;
            }
        }
        
        std::cout << "  Average Firing Rate: " << std::fixed << std::setprecision(3) << brain->getAverageFiringRate() << " Hz" << std::endl;
        std::cout << "  Active Neurons: " << brain->getFiringNeuronCount() << std::endl;
    }
    
    void exploreNeuralDynamics(std::shared_ptr<Brain> brain, bool verbose) {
        std::cout << std::endl;
        std::cout << "Neural Dynamics:" << std::endl;
        
        // Get some statistics
        size_t totalSteps = 1000; // Example
        std::vector<size_t> spikeCounts;
        std::vector<double> firingRates;
        
        // Run a quick dynamics analysis
        for (size_t step = 0; step < totalSteps; ++step) {
            brain->step(step, step * 0.001);
            
            size_t spikes = brain->getTotalSpikeCount();
            double firing = brain->getAverageFiringRate();
            
            spikeCounts.push_back(spikes);
            firingRates.push_back(firing);
        }
        
        if (!spikeCounts.empty()) {
            double avgSpikes = std::accumulate(spikeCounts.begin(), spikeCounts.end(), 0.0) / spikeCounts.size();
            double maxSpikes = *std::max_element(spikeCounts.begin(), spikeCounts.end());
            double spikeStd = 0.0;
            if (spikeCounts.size() > 1) {
                double sumSquares = 0.0;
                for (size_t s : spikeCounts) {
                    double diff = s - avgSpikes;
                    sumSquares += diff * diff;
                }
                spikeStd = std::sqrt(sumSquares / (spikeCounts.size() - 1));
            }
            
            std::cout << "  Average Spikes per Step: " << std::fixed << std::setprecision(1) << avgSpikes << std::endl;
            std::cout << "  Maximum Spikes (any step): " << maxSpikes << std::endl;
            std::cout << "  Spike Count Std Dev: " << std::fixed << std::setprecision(3) << spikeStd << std::endl;
            
            double avgFiring = std::accumulate(firingRates.begin(), firingRates.end(), 0.0) / firingRates.size();
            double firingStd = 0.0;
            if (firingRates.size() > 1) {
                double sumSquares = 0.0;
                for (double f : firingRates) {
                    double diff = f - avgFiring;
                    sumSquares += diff * diff;
                }
                firingStd = std::sqrt(sumSquares / (firingRates.size() - 1));
            }
            
            std::cout << "  Average Firing Rate: " << std::fixed << std::setprecision(3) << avgFiring << " Hz" << std::endl;
            std::cout << "  Firing Rate Std Dev: " << std::fixed << std::setprecision(3) << firingStd << " Hz" << std::endl;
        }
    }
    
    void exploreMemorySystems(std::shared_ptr<Brain> brain, bool verbose) {
        std::cout << std::endl;
        std::cout << "Memory Systems:" << std::endl;
        
        auto workingMem = brain->getWorkingMemory();
        if (workingMem) {
            std::cout << "  Working Memory:" << std::endl;
            std::cout << "    Active Traces: " << workingMem->getActiveTraces() << std::endl;
            std::cout << "    Trace Capacity: " << workingMem->getCapacity() << std::endl;
            std::cout << "    Trace Decay Rate: " << std::fixed << std::setprecision(4) << workingMem->getDecayRate() << std::endl;
        } else {
            std::cout << "  Working Memory: Not Available" << std::endl;
        }
        
        auto episodicMem = brain->getEpisodicMemory();
        if (episodicMem) {
            std::cout << "  Episodic Memory:" << std::endl;
            std::cout << "    Episode Count: " << episodicMem->getEpisodeCount() << std::endl;
            std::cout << "    Storage Capacity: " << episodicMem->getCapacity() << std::endl;
        } else {
            std::cout << "  Episodic Memory: Not Available" << std::endl;
        }
        
        auto associativeMem = brain->getAssociativeMemory();
        if (associativeMem) {
            std::cout << "  Associative Memory:" << std::endl;
            std::cout << "    Pattern Associations: " << associativeMem->getAssociationCount() << std::endl;
            std::cout << "    Hebbian Strength: " << std::fixed << std::setprecision(4) << associativeMem->getHebbianStrength() << std::endl;
        } else {
            std::cout << "  Associative Memory: Not Available" << std::endl;
        }
    }
    
    void exploreNeuromodulation(std::shared_ptr<Brain> brain, bool verbose) {
        std::cout << std::endl;
        std::cout << "Neuromodulation Systems:" << std::endl;
        
        auto dopamine = brain->getDopamine();
        if (dopamine) {
            std::cout << "  Dopamine:" << std::endl;
            std::cout << "    Current Level: " << std::fixed << std::setprecision(3) << dopamine->getLevel() << std::endl;
            std::cout << "    Learning Rate: " << std::fixed << std::setprecision(4) << dopamine->getLearningRate() << std::endl;
            std::cout << "    Excitability Modifier: " << std::fixed << std::setprecision(3) << dopamine->getExcitabilityModifier() << std::endl;
        } else {
            std::cout << "  Dopamine: Not Available" << std::endl;
        }
        
        auto curiosity = brain->getCuriosity();
        if (curiosity) {
            std::cout << "  Curiosity:" << std::endl;
            std::cout << "    Current Level: " << std::fixed << std::setprecision(3) << curiosity->getLevel() << std::endl;
            std::cout << "    Exploration Drive: " << std::fixed << std::setprecision(3) << curiosity->getExplorationDrive() << std::endl;
        } else {
            std::cout << "  Curiosity: Not Available" << std::endl;
        }
        
        auto novelty = brain->getNovelty();
        if (novelty) {
            std::cout << "  Novelty Detection:" << std::endl;
            std::cout << "    Sensitivity: " << std::fixed << std::setprecision(3) << novelty->getSensitivity() << std::endl;
        } else {
            std::cout << "  Novelty Detection: Not Available" << std::endl;
        }
    }
    
    void exploreLearningDynamics(std::shared_ptr<Brain> brain, size_t steps, bool verbose) {
        std::cout << "Learning Dynamics:" << std::endl;
        
        auto region = brain->getRegion(RegionId(1));
        if (!region) {
            std::cout << "  No regions found for learning analysis" << std::endl;
            return;
        }
        
        std::vector<float> weightHistory;
        std::vector<double> spikeHistory;
        
        // Run learning simulation
        for (size_t step = 0; step < steps; ++step) {
            // Apply learning inputs
            brain->injectCurrentToNeurons(NeuronType::Sensory, 40.0f);
            
            // Step the brain
            brain->step(step, step * 0.001);
            
            // Record metrics
            double spikeCount = brain->getTotalSpikeCount();
            spikeHistory.push_back(spikeCount);
            
            // Record average synaptic weight
            double totalWeight = 0.0;
            size_t synapseCount = 0;
            for (const auto& syn : region->getSynapses()) {
                totalWeight += syn->getWeight();
                synapseCount++;
            }
            
            if (synapseCount > 0) {
                weightHistory.push_back(totalWeight / synapseCount);
            }
            
            if (verbose && step % 200 == 0) {
                std::cout << "  Step " << step 
                          << ": " << spikeCount << " spikes, " 
                          << (synapseCount > 0 ? weightHistory.back() : 0.0) << " avg weight" << std::endl;
            }
        }
        
        if (!weightHistory.empty() && !spikeHistory.empty()) {
            double initialWeight = weightHistory[0];
            double finalWeight = weightHistory.back();
            double weightChange = finalWeight - initialWeight;
            
            std::cout << "  Weight Change: " << std::fixed << std::setprecision(5) << weightChange << std::endl;
            std::cout << "  Weight Stability: " << (weightChange < 0.01 ? "High" : "Moderate") << std::endl;
            
            double initialSpike = spikeHistory[0];
            double finalSpike = spikeHistory.back();
            double spikeChange = finalSpike - initialSpike;
            
            std::cout << "  Spike Activity Change: " << std::fixed << std::setprecision(1) << spikeChange << std::endl;
        }
    }
    
    void exploreSynapticChanges(std::shared_ptr<Brain> brain, bool verbose) {
        std::cout << std::endl;
        std::cout << "Synaptic Plasticity Analysis:" << std::endl;
        
        auto region = brain->getRegion(RegionId(1));
        if (!region) {
            std::cout << "  No regions found for plasticity analysis" << std::endl;
            return;
        }
        
        std::vector<std::pair<std::string, double>> synapseStats;
        
        for (const auto& syn : region->getSynapses()) {
            std::string type = "Hebbian";
            if (syn->isPlastic()) {
                double weight = syn->getWeight();
                synapseStats.push_back({type, weight});
            }
        }
        
        if (!synapseStats.empty()) {
            std::cout << "  Synapses Analyzed: " << synapseStats.size() << std::endl;
            
            double totalWeight = 0.0;
            for (const auto& stat : synapseStats) {
                totalWeight += stat.second;
            }
            
            double avgWeight = totalWeight / synapseStats.size();
            double minWeight = synapseStats[0].second;
            double maxWeight = synapseStats[0].second;
            
            for (const auto& stat : synapseStats) {
                if (stat.second < minWeight) minWeight = stat.second;
                if (stat.second > maxWeight) maxWeight = stat.second;
            }
            
            std::cout << "  Average Weight: " << std::fixed << std::setprecision(5) << avgWeight << std::endl;
            std::cout << "  Min Weight: " << std::fixed << std::setprecision(5) << minWeight << std::endl;
            std::cout << "  Max Weight: " << std::fixed << std::setprecision(5) << maxWeight << std::endl;
            std::cout << "  Weight Range: " << std::fixed << std::setprecision(5) << (maxWeight - minWeight) << std::endl;
        }
    }
    
    void exploreDevelopment(std::shared_ptr<Brain> brain, bool verbose) {
        std::cout << std::endl;
        std::cout << "Development Systems:" << std::endl;
        
        auto devSystem = brain->getDevelopmentSystem();
        if (devSystem) {
            std::cout << "  Development System: Active" << std::endl;
            std::cout << "  Current Development Stage: " << devSystem->getCurrentStage() << std::endl;
            std::cout << "  Plasticity Rate: " << std::fixed << std::setprecision(6) << devSystem->getPlasticityRate() << std::endl;
            std::cout << "  Structural Plasticity: " << (devSystem->isStructuralPlasticityEnabled() ? "Enabled" : "Disabled") << std::endl;
            
            // Simulate development progression
            for (int stage = 1; stage <= 5; ++stage) {
                std::cout << "    Stage " << stage << ": " 
                          << std::fixed << std::setprecision(4) << devSystem->getPlasticityRateAtStage(stage) << " plasticity rate" << std::endl;
            }
        } else {
            std::cout << "  Development System: Not Available" << std::endl;
        }
    }
    
    void exploreEnvironmentInteraction(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> env,
                                        std::shared_ptr<AgentBrain> agent, bool verbose) {
        std::cout << "Environment Interaction:" << std::endl;
        
        size_t totalSteps = 500;
        std::vector<float> rewards;
        std::vector<double> firingRates;
        
        // Run environment interaction simulation
        for (size_t step = 0; step < totalSteps; ++step) {
            // Get observation
            SensoryPercept percept = env->observe(brain->getRegions()[0].get());
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Brain step
            brain->step(step, step * 0.001);
            
            // Get motor command
            MotorCommand cmd = agent->decodeMotorCommand();
            
            // Apply action
            env->applyAction(brain->getRegions()[0].get(), cmd);
            
            // Compute reward
            float reward = env->computeReward(brain->getRegions()[0].get());
            rewards.push_back(reward);
            
            firingRates.push_back(brain->getAverageFiringRate());
            
            // Apply reward modulation
            agent->applyRewardModulation(reward, 0.0f);
            
            if (verbose && step % 100 == 0) {
                std::cout << "  Step " << step 
                          << ": Reward={" << reward << "}, "
                          << "Firing={" << firingRates.back() << "}" << std::endl;
            }
        }
        
        if (!rewards.empty()) {
            double avgReward = std::accumulate(rewards.begin(), rewards.end(), 0.0) / rewards.size();
            double totalReward = std::accumulate(rewards.begin(), rewards.end(), 0.0);
            
            std::cout << "  Average Reward: " << std::fixed << std::setprecision(3) << avgReward << std::endl;
            std::cout << "  Total Reward: " << totalReward << std::endl;
            
            double avgFiring = std::accumulate(firingRates.begin(), firingRates.end(), 0.0) / firingRates.size();
            std::cout << "  Average Firing Rate: " << std::fixed << std::setprecision(3) << avgFiring << " Hz" << std::endl;
            
            // Calculate reward variance
            double rewardVariance = 0.0;
            if (rewards.size() > 1) {
                double rewardMean = avgReward;
                double sumSquares = 0.0;
                for (float r : rewards) {
                    double diff = r - rewardMean;
                    sumSquares += diff * diff;
                }
                rewardVariance = sumSquares / (rewards.size() - 1);
            }
            
            std::cout << "  Reward Variance: " << std::fixed << std::setprecision(6) << rewardVariance << std::endl;
        }
    }
    
    void exploreSensoryProcessing(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> env,
                                  std::shared_ptr<AgentBrain> agent, bool verbose) {
        std::cout << std::endl;
        std::cout << "Sensory Processing:" << std::endl;
        
        // Get sensory input
        SensoryPercept percept = env->observe(brain->getRegions()[0].get());
        
        std::cout << "  Observation Processing: " << (percept.isValid() ? "Successful" : "Failed") << std::endl;
        std::cout << "  Sensory Dimensions: " << percept.getFeatureCount() << std::endl;
        
        if (percept.isValid()) {
            // Process sensory input
            agent->processSensoryInput(percept);
            
            std::cout << "  Neural Activation: " << brain->getFiringNeuronCount() << " neurons" << std::endl;
            std::cout << "  Firing Rate: " << std::fixed << std::setprecision(3) << brain->getAverageFiringRate() << " Hz" << std::endl;
            
            // Analyze neural response
            auto region = brain->getRegion(RegionId(0));
            if (region) {
                double totalActivity = region->getTotalActivity();
                std::cout << "  Regional Activity: " << std::fixed << std::setprecision(4) << totalActivity << std::endl;
            }
        }
    }
    
    void exploreMotorControl(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> env,
                             std::shared_ptr<AgentBrain> agent, bool verbose) {
        std::cout << std::endl;
        std::cout << "Motor Control:" << std::endl;
        
        // Get motor command
        MotorCommand cmd = agent->decodeMotorCommand();
        
        std::cout << "  Action Type: " << cmd.getActionType() << std::endl;
        std::cout << "  Action Strength: " << std::fixed << std::setprecision(3) << cmd.getStrength() << std::endl;
        std::cout << "  Action Direction: " << cmd.getDirection() << std::endl;
        
        // Apply action to environment
        env->applyAction(brain->getRegions()[0].get(), cmd);
        
        // Get environment feedback
        float reward = env->computeReward(brain->getRegions()[0].get());
        
        std::cout << "  Environmental Reward: " << std::fixed << std::setprecision(3) << reward << std::endl;
        std::cout << "  Neuromodulation: " << agent->getDopamineLevel() << " dopamine level" << std::endl;
    }
    
    std::string getTimestamp() const { return timestamp; }
    
private:
    std::string timestamp;
};

struct ExplorationConfig {
    size_t neuronCount = 1000;
    size_t learningSteps = 1000;
    bool verbose = false;
    bool runNetworkExploration = true;
    bool runLearningExploration = true;
    bool runEnvironmentExploration = true;
};

class ExplorerCLI {
public:
    static int main(int argc, char* argv[]) {
        auto parser = argparse.ArgumentParser(
            description="NLM (Neural Learning Machine) Explorer",
            formatter_class=argparse.RawDescriptionHelpFormatter,
            epilog="""
Exploration Types:
  network    - Explore neural network topology and properties
  learning   - Analyze learning dynamics and plasticity
  environment - Study environment interaction and behavior
  all        - Run all explorations (default)

Examples:
  pynlm-explore --type all --neurons 1500
  pynlm-explore --type learning --neurons 800 --steps 1500
  pynlm-explore --type network --neurons 2000
        """
        );
        
        parser.add_argument(
            "--type", choices={"network", "learning", "environment", "all"},
            default="all", help="Exploration type (default: all)"
        );
        
        parser.add_argument(
            "--neurons", type=int, default=1000,
            help="Number of neurons for exploration (default: 1000)"
        );
        
        parser.add_argument(
            "--steps", type=int, default=1000,
            help="Number of steps for learning exploration (default: 1000)"
        );
        
        parser.add_argument(
            "--verbose", action="store_true",
            help="Enable verbose output"
        );
        
        parser.add_argument(
            "--quiet", action="store_true",
            help="Suppress progress output"
        );
        
        parser.add_argument(
            "--config", type=str, default="",
            help="Load exploration configuration from file"
        );
        
        parser.add_argument(
            "--output", type=str, default="",
            help="Output file for exploration results (JSON format)"
        );
        
        auto args = parser.parse_args();
        
        // Load configuration if specified
        ExplorationConfig config;
        config.neuronCount = args.neurons;
        config.learningSteps = args.steps;
        config.verbose = !args.quiet && args.verbose;
        
        if (!args.config.empty()) {
            // Load config from file (JSON/YAML implementation needed)
            std::cout << "Loading configuration from " << args.config << std::endl;
            // TODO: Implement config loading
        }
        
        // Set up logging
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(args.quiet ? LogLevel::Error : LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        try {
            ExplorerManager explorerManager;
            
            if (args.type == "network" || args.type == "all") {
                std::cout << "1/3 Running Network Topology Exploration..." << std::endl;
                int result = explorerManager.runNetworkExplorer(args.neurons, config.verbose);
                if (result != 0) return result;
            }
            
            if (args.type == "learning" || args.type == "all") {
                std::cout << "2/3 Running Learning Dynamics Exploration..." << std::endl;
                int result = explorerManager.runLearningExplorer(args.neurons, args.steps, config.verbose);
                if (result != 0) return result;
            }
            
            if (args.type == "environment" || args.type == "all") {
                std::cout << "3/3 Running Environment Interaction Exploration..." << std::endl;
                int result = explorerManager.runEnvironmentExplorer(args.neurons, config.verbose);
                if (result != 0) return result;
            }
            
            // Save results if requested
            if (!args.output.empty()) {
                std::ofstream file(args.output);
                if (file) {
                    json j;
                    j["exploration_type"] = args.type;
                    j["timestamp"] = explorerManager.getTimestamp();
                    j["neurons"] = args.neurons;
                    j["steps"] = args.steps;
                    j["verbose"] = config.verbose;
                    
                    file << std::setw(2) << j;
                    file.close();
                    
                    std::cout << std::endl;
                    std::cout << "✓ Exploration results saved to " << args.output << std::endl;
                }
            }
            
            std::cout << std::endl;
            std::cout << "✓ Complete exploration completed successfully" << std::endl;
            return 0;
            
        } catch (const std::exception& e) {
            std::cerr << std::endl;
            std::cerr << "✗ Exploration failed: " << e.what() << std::endl;
            return 1;
        }
    }
};

int main(int argc, char* argv[]) {
    return ExplorerCLI::main(argc, argv);
}
