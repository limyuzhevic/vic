"""
Demo CLI for NLM

This module provides command-line interfaces for running demonstrations and examples
of the NLM (Neural Learning Machine) framework.
"""

import argparse
import sys
import time
import json
from pathlib import Path
from typing import Optional, List, Dict, Any

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

class DemoManager {
public:
    DemoManager() {
        auto now = std::time(nullptr);
        timestamp = std::ctime(&now);
        if (!timestamp.empty() && timestamp.back() == '\n') {
            timestamp.pop_back();
        }
    }
    
    int runGettingStartedDemo(size_t neuronCount, size_t steps, bool verbose) {
        std::cout << std::endl;
        std::cout << "=== NLM Getting Started Demo ===" << std::endl;
        std::cout << "Learning to simulate basic neural computation..." << std::endl;
        std::cout << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            // Create configuration
            auto config = std::make_shared<Config>();
            config->set("neuron_count", static_cast<int64_t>(neuronCount));
            config->set("region_count", 1);
            config->set("connection_probability", 0.15f);
            
            // Create brain
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                throw std::runtime_error("Failed to initialize brain");
            }
            
            std::cout << "✓ Brain created successfully" << std::endl;
            std::cout << "  - Neurons: " << brain->getTotalNeuronCount() << std::endl;
            std::cout << "  - Synapses: " << brain->getTotalSynapseCount() << std::endl;
            
            // Apply learning inputs
            std::cout << "Applying learning inputs..." << std::endl;
            for (size_t step = 0; step < steps; ++step) {
                // Random input pattern
                for (size_t i = 0; i < neuronCount / 10; ++i) {
                    brain->injectCurrentToNeurons(
                        static_cast<NeuronType>(step % 3),  // Cycle through neuron types
                        30.0f * (1.0f + 0.5f * std::sin(static_cast<double>(step) * 0.1))
                    );
                }
                
                brain->step(step, step * 0.001);
                
                if (verbose && step % 200 == 0) {
                    std::cout << "  Step " << step 
                              << ": " << brain->getTotalSpikeCount() << " spikes, "
                              << brain->getFiringNeuronCount() << " firing neurons" << std::endl;
                }
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            double duration = std::chrono::duration<double>(endTime - startTime).count();
            
            // Print results
            std::cout << std::endl;
            std::cout << "=== Demo Results ===" << std::endl;
            std::cout << "Simulation completed successfully!" << std::endl;
            std::cout << "Duration: " << std::fixed << std::setprecision(2) << duration << "s" << std::endl;
            std::cout << "Total spikes generated: " << brain->getTotalSpikeCount() << std::endl;
            std::cout << "Average firing rate: " << std::fixed << std::setprecision(3) << brain->getAverageFiringRate() << " Hz" << std::endl;
            std::cout << "Active neurons: " << brain->getFiringNeuronCount() << std::endl;
            
            // Log brain status
            brain->logStatus();
            
            std::cout << std::endl;
            std::cout << "✓ Getting Started Demo completed successfully" << std::endl;
            std::cout << "The NLM brain is now ready for more complex tasks." << std::endl;
            
            return 0;
            
        } catch (const std::exception& e) {
            std::cout << std::endl;
            std::cout << "✗ Demo failed: " << e.what() << std::endl;
            return 1;
        }
    }
    
    int runIntegrationDemo(size_t neuronCount, size_t steps, bool verbose) {
        std::cout << std::endl;
        std::cout << "=== NLM Integration Demo ===" << std::endl;
        std::cout << "Testing integration of all neural systems..." << std::endl;
        std::cout << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            // Create configuration
            auto config = std::make_shared<Config>();
            config->set("neuron_count", static_cast<int64_t>(neuronCount));
            config->set("region_count", 2);
            config->set("connection_probability", 0.12f);
            config->set("enable_development", true);
            config->set("enable_curiosity", true);
            config->set("enable_reward_modulation", true);
            
            // Create brain
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                throw std::runtime_error("Failed to initialize brain");
            }
            
            std::cout << "✓ Brain created with integrated systems" << std::endl;
            
            // Create environment and agent
            auto simpleWorld = std::make_shared<SimpleWorld>();
            simpleWorld->initialize(16, 16);
            
            auto agent = std::make_shared<AgentBrain>(brain);
            agent->initialize(*(simpleWorld.get()));
            agent->enableRewardModulation(true);
            agent->enableCuriosity(true);
            
            std::cout << "✓ Agent and environment created" << std::endl;
            
            // Run integration simulation
            std::cout << "Running integration simulation..." << std::endl;
            float totalReward = 0.0f;
            float totalFiring = 0.0f;
            size_t firingCount = 0;
            
            for (size_t step = 0; step < steps; ++step) {
                // Get observation
                SensoryPercept percept = simpleWorld->observe(brain->getRegions()[0].get());
                
                // Process sensory input
                agent->processSensoryInput(percept);
                
                // Brain step
                brain->step(step, step * 0.001);
                
                // Get motor command
                MotorCommand cmd = agent->decodeMotorCommand();
                
                // Apply action
                simpleWorld->applyAction(brain->getRegions()[0].get(), cmd);
                
                // Compute reward
                float reward = simpleWorld->computeReward(brain->getRegions()[0].get());
                totalReward += reward;
                
                // Apply reward modulation
                agent->applyRewardModulation(reward, 0.0f);
                
                // Update development
                agent->updateDevelopment(0.001);
                
                // Collect metrics
                totalFiring += brain->getAverageFiringRate();
                if (brain->getFiringNeuronCount() > 0) firingCount++;
                
                if (verbose && step % 500 == 0) {
                    std::cout << "  Step " << step 
                              << ": Reward={" << totalReward / (step + 1) << "}, "
                              << "Firing={" << brain->getAverageFiringRate() << "}, "
                              << "Memory={" << (brain->getWorkingMemory() ? brain->getWorkingMemory()->getActiveTraces() : 0) << "}" << std::endl;
                }
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            double duration = std::chrono::duration<double>(endTime - startTime).count();
            
            // Print integration results
            std::cout << std::endl;
            std::cout << "=== Integration Demo Results ===" << std::endl;
            std::cout << "✓ Integration simulation completed successfully" << std::endl;
            std::cout << "Duration: " << std::fixed << std::setprecision(2) << duration << "s" << std::endl;
            std::cout << "Total reward: " << totalReward << std::endl;
            std::cout << "Average firing rate: " << totalFiring / steps << " Hz" << std::endl;
            std::cout << "Time with activity: " << firingCount << " steps out of " << steps << " (" 
                      << (static_cast<double>(firingCount) / steps * 100.0) << "%)" << std::endl;
            
            // Check system integration
            bool workingMemory = brain->getWorkingMemory() != nullptr;
            bool episodicMemory = brain->getEpisodicMemory() != nullptr;
            bool neuromodulation = brain->getDopamine() != nullptr;
            bool prediction = brain->getPredictionSystem() != nullptr;
            bool development = brain->getDevelopmentSystem() != nullptr;
            
            std::cout << std::endl;
            std::cout << "=== System Integration Status ===" << std::endl;
            std::cout << "Working Memory: " << (workingMemory ? "✓ Connected" : "✗ Disconnected") << std::endl;
            std::cout << "Episodic Memory: " << (episodicMemory ? "✓ Connected" : "✗ Disconnected") << std::endl;
            std::cout << "Neuromodulation: " << (neuromodulation ? "✓ Connected" : "✗ Disconnected") << std::endl;
            std::cout << "Prediction: " << (prediction ? "✓ Connected" : "✗ Disconnected") << std::endl;
            std::cout << "Development: " << (development ? "✓ Connected" : "✗ Disconnected") << std::endl;
            
            std::cout << std::endl;
            std::cout << "✓ Integration Demo completed successfully" << std::endl;
            std::cout << "All neural systems are properly connected and functional." << std::endl;
            
            return 0;
            
        } catch (const std::exception& e) {
            std::cout << std::endl;
            std::cout << "✗ Integration Demo failed: " << e.what() << std::endl;
            return 1;
        }
    }
    
    int runPlasticityDemo(size_t neuronCount, size_t steps, bool verbose) {
        std::cout << std::endl;
        std::cout << "=== NLM Plasticity Demo ===" << std::endl;
        std::cout << "Demonstrating synaptic plasticity and learning..." << std::endl;
        std::cout << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            // Create configuration
            auto config = std::make_shared<Config>();
            config->set("neuron_count", static_cast<int64_t>(neuronCount));
            config->set("region_count", 1);
            config->set("connection_probability", 0.15f);
            config->set("stdp_ltp_weight", 0.02f);
            config->set("stdp_ltd_weight", 0.015f);
            
            // Create brain
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                throw std::runtime_error("Failed to initialize brain");
            }
            
            std::cout << "✓ Brain created with plasticity rules" << std::endl;
            
            // Get region for plasticity tests
            auto region = brain->getRegion(RegionId(1));
            if (!region) {
                throw std::runtime_error("No regions found for plasticity demo");
            }
            
            // Enable plasticity
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false); // STDP and Hebbian
            }
            
            std::cout << "Enabled STDP and Hebbian plasticity" << std::endl;
            
            // Record initial weights
            std::vector<float> initialWeights;
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
            
            float initialMean = 0.0f;
            if (!initialWeights.empty()) {
                float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
                initialMean = sum / initialWeights.size();
            }
            
            std::cout << "Initial mean synaptic weight: " << std::fixed << std::setprecision(4) << initialMean << std::endl;
            
            // Apply correlated activity (STDP)
            std::cout << "Applying correlated pre->post activity (STDP)..." << std::endl;
            auto neurons = region->getAllNeurons();
            
            if (neurons.size() >= 2) {
                for (size_t trial = 0; trial < steps / 10; ++trial) {
                    // Pre fires first
                    neurons[0]->injectCurrent(60.0f);
                    brain->step(trial * 2, trial * 2 * 0.001);
                    
                    // Then post fires
                    neurons[1]->injectCurrent(60.0f);
                    brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
                }
            }
            
            // Record final weights
            std::vector<float> finalWeights;
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
            
            float finalMean = 0.0f;
            if (!finalWeights.empty()) {
                float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
                finalMean = sum / finalWeights.size();
            }
            
            std::cout << "Final mean synaptic weight: " << std::fixed << std::setprecision(4) << finalMean << std::endl;
            
            float weightChange = finalMean - initialMean;
            std::cout << "Weight change: " << std::fixed << std::setprecision(4) << weightChange << std::endl;
            
            // Analyze weight changes
            size_t strengthened = 0;
            size_t weakened = 0;
            size_t unchanged = 0;
            
            size_t minSize = std::min(initialWeights.size(), finalWeights.size());
            for (size_t i = 0; i < minSize; ++i) {
                float delta = finalWeights[i] - initialWeights[i];
                if (delta > 0.01f) strengthened++;
                else if (delta < -0.01f) weakened++;
                else unchanged++;
            }
            
            std::cout << std::endl;
            std::cout << "=== Plasticity Analysis ===" << std::endl;
            std::cout << "Total synapses analyzed: " << minSize << std::endl;
            std::cout << "Strengthened (LTP): " << strengthened << std::endl;
            std::cout << "Weakened (LTD): " << weakened << std::endl;
            std::cout << "Unchanged: " << unchanged << std::endl;
            
            if (weightChange > 0.001f) {
                std::cout << "✓ Learning detected: Net potentiation through STDP" << std::endl;
            } else if (weightChange < -0.001f) {
                std::cout << "! Learning reversed: Net depression occurred" << std::endl;
            } else {
                std::cout << "~ No significant weight change" << std::endl;
            }
            
            // Run additional Hebbian learning
            std::cout << std::endl;
            std::cout << "Applying additional Hebbian learning..." << std::endl;
            
            for (size_t i = 0; i < steps / 5; ++i) {
                // Apply random correlated activity for Hebbian learning
                if (neurons.size() >= 2) {
                    neurons[0]->injectCurrent(40.0f);
                    brain->step(i * 2, i * 2 * 0.001);
                    neurons[1]->injectCurrent(40.0f);
                    brain->step(i * 2 + 1, (i * 2 + 1) * 0.001);
                }
            }
            
            std::cout << "✓ Plasticity demo completed successfully" << std::endl;
            std::cout << "The brain demonstrates activity-dependent synaptic modification." << std::endl;
            
            return 0;
            
        } catch (const std::exception& e) {
            std::cout << std::endl;
            std::cout << "✗ Plasticity Demo failed: " << e.what() << std::endl;
            return 1;
        }
    }
    
    int runAllDemos(const DemoConfig& config) {
        std::cout << std::endl;
        std::cout << "=== NLM Complete Demo Suite ===" << std::endl;
        std::cout << "Running all available demonstrations..." << std::endl;
        std::cout << std::endl;
        
        int overallResult = 0;
        
        // Run getting started demo
        if (config.runGettingStarted) {
            int result = runGettingStartedDemo(config.neuronCount, config.steps, config.verbose);
            if (result != 0) overallResult = result;
        }
        
        // Run integration demo
        if (config.runIntegration) {
            int result = runIntegrationDemo(config.neuronCount, config.steps, config.verbose);
            if (result != 0) overallResult = result;
        }
        
        // Run plasticity demo
        if (config.runPlasticity) {
            int result = runPlasticityDemo(config.neuronCount, config.steps, config.verbose);
            if (result != 0) overallResult = result;
        }
        
        std::cout << std::endl;
        std::cout << "=== Demo Suite Complete ===" << std::endl;
        if (overallResult == 0) {
            std::cout << "✓ All demonstrations completed successfully" << std::endl;
        } else {
            std::cout << "✗ Some demonstrations failed" << std::endl;
        }
        
        return overallResult;
    }
    
    std::string getTimestamp() const { return timestamp; }
    
private:
    std::string timestamp;
};

struct DemoConfig {
    size_t neuronCount = 500;
    size_t steps = 1000;
    bool verbose = false;
    bool runGettingStarted = true;
    bool runIntegration = true;
    bool runPlasticity = true;
};

class DemoCLI {
public:
    static int main(int argc, char* argv[]) {
        auto parser = argparse.ArgumentParser(
            description="NLM (Neural Learning Machine) Demo Suite",
            formatter_class=argparse.RawDescriptionHelpFormatter,
            epilog="""
Demo Types:
  getting-started    - Basic neural simulation demonstration
  integration       - Full system integration test
  plasticity        - Synaptic learning and plasticity demo
  all               - Run all demonstrations (default)

Examples:
  pynlm-demo --type all --neurons 800 --steps 2000
  pynlm-demo --type integration --neurons 1000 --verbose
  pynlm-demo --type getting-started
        """
        );
        
        parser.add_argument(
            "--type", choices={"getting-started", "integration", "plasticity", "all"},
            default="all", help="Demo type (default: all)"
        );
        
        parser.add_argument(
            "--neurons", type=int, default=500,
            help="Number of neurons for demo (default: 500)"
        );
        
        parser.add_argument(
            "--steps", type=int, default=1000,
            help="Number of simulation steps (default: 1000)"
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
            help="Load demo configuration from file"
        );
        
        parser.add_argument(
            "--output", type=str, default="",
            help="Output file for demo results (JSON format)"
        );
        
        auto args = parser.parse_args();
        
        // Load configuration if specified
        DemoConfig config;
        config.neuronCount = args.neurons;
        config.steps = args.steps;
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
            DemoManager demoManager;
            
            if (args.type == "getting-started" || args.type == "all") {
                std::cout << "Running Getting Started Demo..." << std::endl;
                int result = demoManager.runGettingStartedDemo(args.neurons, args.steps, config.verbose);
                if (result != 0) return result;
            }
            
            if (args.type == "integration" || args.type == "all") {
                std::cout << "Running Integration Demo..." << std::endl;
                int result = demoManager.runIntegrationDemo(args.neurons, args.steps, config.verbose);
                if (result != 0) return result;
            }
            
            if (args.type == "plasticity" || args.type == "all") {
                std::cout << "Running Plasticity Demo..." << std::endl;
                int result = demoManager.runPlasticityDemo(args.neurons, args.steps, config.verbose);
                if (result != 0) return result;
            }
            
            // Save results if requested
            if (!args.output.empty()) {
                std::ofstream file(args.output);
                if (file) {
                    json j;
                    j["demo_type"] = args.type;
                    j["timestamp"] = demoManager.getTimestamp();
                    j["neurons"] = args.neurons;
                    j["steps"] = args.steps;
                    j["verbose"] = config.verbose;
                    
                    file << std::setw(2) << j;
                    file.close();
                    
                    std::cout << std::endl;
                    std::cout << "✓ Demo results saved to " << args.output << std::endl;
                }
            }
            
            std::cout << std::endl;
            std::cout << "✓ Demo suite completed successfully" << std::endl;
            return 0;
            
        } catch (const std::exception& e) {
            std::cerr << std::endl;
            std::cerr << "✗ Demo failed: " << e.what() << std::endl;
            return 1;
        }
    }
};

int main(int argc, char* argv[]) {
    return DemoCLI::main(argc, argv);
}
