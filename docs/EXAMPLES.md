# NLM Examples: Practical Applications and Demos

## Overview

This document provides practical examples of using NLM (熙然) for various applications and research scenarios. Examples cover:

- Basic neural computations
- World interaction experiments
- Cognitive emergence demos
- Performance benchmarks
- Research applications

## Table of Contents

1. [Quick Start Examples](#1-quick-start-examples)
2. [Neural Dynamics Examples](#2-neural-dynamics-examples)
3. [World Interaction Examples](#3-world-interaction-examples)
4. [Cognitive Science Examples](#4-cognitive-science-examples)
5. [Performance and Scaling Examples](#5-performance-and-scaling-examples)
6. [Research Applications](#6-research-applications)
7. [Example Scripts](#7-example-scripts)

## 1. Quick Start Examples

### 1.1 Simple Neuron Network

```cpp
#include <nlm/brain/Neuron.hpp>
#include <nlm/brain/Synapse.hpp>
#include <nlm/core/Config.hpp>

int main() {
    // Configure neuron network
    auto config = std::make_shared<nlm::Config>();
    config->set("nlm.simulation.max_steps", 1000);
    config->set("nlm.brain.neuron_count", 50);
    config->set("nlm.brain.plasticity_enabled", true);
    
    // Create brain with neuron network
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Create neurons
    std::vector<nlm::NeuronId> neurons;
    for (int i = 0; i < 50; ++i) {
        auto neuron = brain->getBrain()->createNeuron(i);
        neurons.push_back(neuron->getId());
    }
    
    // Create random synaptic connections
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> weight_dist(0.1f, 1.0f);
    
    for (size_t i = 0; i < neurons.size(); ++i) {
        for (size_t j = 0; j < neurons.size(); ++j) {
            if (i != j && (rng() % 10) == 0) { // 10% connection probability
                float weight = weight_dist(rng);
                brain->getBrain()->createSynapse(neurons[i], neurons[j], weight);
            }
        }
    }
    
    // Run simulation
    for (nlm::SimulationStep step = 0; step < 1000; ++step) {
        brain->step(step);
        
        if (step % 100 == 0) {
            brain->logStatus();
        }
    }
    
    return 0;
}
```

### 1.2 Memory-Based Pattern Recognition

```cpp
#include <nlm/brain/Brain.hpp>
#include <nlm/memory/NeuralWorkingMemory.hpp>

class PatternRecognizer {
public:
    PatternRecognizer(std::shared_ptr<nlm::Brain> brain) 
        : brain_(brain), workingMemory_(brain->getWorkingMemory()) {
    }
    
    void trainPattern(const std::vector<float>& pattern, float strength = 1.0f) {
        // Store pattern in working memory
        workingMemory_->store(pattern, strength);
        
        // Create recurrent connections for persistence
        auto neurons = workingMemory_->getMemoryNeurons();
        for (size_t i = 0; i < neurons.size() - 1; ++i) {
            workingMemory_->createRecurrentConnection(neurons[i], neurons[i + 1], 0.8f);
        }
    }
    
    std::vector<float> recognizePattern(const std::vector<float>& input) {
        // Process input through brain
        brain_->step(0);
        
        // Retrieve current memory content
        return workingMemory_->retrieve();
    }
    
    float similarityScore(const std::vector<float>& pattern1, 
                         const std::vector<float>& pattern2) {
        float score = 0.0f;
        for (size_t i = 0; i < pattern1.size() && i < pattern2.size(); ++i) {
            score += std::abs(pattern1[i] - pattern2[i]);
        }
        return score / std::min(pattern1.size(), pattern2.size());
    }
    
private:
    std::shared_ptr<nlm::Brain> brain_;
    nlm::NeuralWorkingMemory* workingMemory_;
};

int main() {
    // Initialize brain with memory
    auto config = std::make_shared<nlm::Config>();
    config->set("nlm.memory.working_memory_capacity", 100);
    config->set("nlm.memory.working_memory_decay_rate", 0.01);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Create pattern recognizer
    PatternRecognizer recognizer(brain);
    
    // Train on sample patterns
    std::vector<float> pattern1 = {1.0f, 0.5f, 0.2f, 0.0f, 0.0f};
    std::vector<float> pattern2 = {0.0f, 1.0f, 0.3f, 0.1f, 0.0f};
    std::vector<float> pattern3 = {0.5f, 0.5f, 0.5f, 0.5f, 0.5f};
    
    recognizer.trainPattern(pattern1, 1.0f);
    recognizer.trainPattern(pattern2, 0.8f);
    recognizer.trainPattern(pattern3, 0.6f);
    
    // Test recognition
    std::vector<float> testPattern = {0.9f, 0.1f, 0.3f, 0.0f, 0.0f};
    auto recognized = recognizer.recognizePattern(testPattern);
    
    std::cout << "Test pattern: ";
    for (float val : testPattern) std::cout << val << " ";
    std::cout << std::endl;
    
    std::cout << "Recognized: ";
    for (float val : recognized) std::cout << val << " ";
    std::cout << std::endl;
    
    return 0;
}
```

## 2. Neural Dynamics Examples

### 2.1 Leaky Integrate-and-Fire Neuron Network

```cpp
#include <nlm/brain/Neuron.hpp>
#include <nlm/dynamics/IntegrateAndFireDynamics.hpp>
#include <nlm/plasticity/STDP.hpp>

class LIFNetwork {
public:
    LIFNetwork(int numNeurons, float timeStep = 0.001f) 
        : timeStep_(timeStep) {
        neurons_.reserve(numNeurons);
        createNetwork(numNeurons);
    }
    
    void createNetwork(int numNeurons) {
        // Create neurons with LIF dynamics
        for (int i = 0; i < numNeurons; ++i) {
            auto neuron = std::make_unique<nlm::Neuron>(i);
            neuron->setLeakConductance(10.0f);  // Membrane time constant
            neuron->setThreshold(-55.0f);       // Firing threshold
            neuron->setRestingPotential(-70.0f);
            neuron->setResetPotential(-70.0f);
            neuron->setRefractoryPeriod(5);     // 5 timestep refractory period
            
            neurons_.push_back(std::move(neuron));
        }
    }
    
    void runSimulation(int steps, float externalInput = 0.0f) {
        for (int step = 0; step < steps; ++step) {
            // Apply external input
            if (externalInput > 0) {
                for (auto& neuron : neurons_) {
                    neuron->injectCurrent(externalInput);
                }
            }
            
            // Update all neurons
            for (auto& neuron : neurons_) {
                bool fired = neuron->stepLIF(step, timeStep_);
                if (fired) {
                    // Process spike for STDP
                    processSpike(neuron->getId(), step);
                }
            }
            
            // Decay connections based on STDP
            updateSTDPConnections(step);
            
            // Log statistics
            if (step % 100 == 0) {
                logNetworkStats(step);
            }
        }
    }
    
    void processSpike(nlm::NeuronId neuronId, nlm::Timestamp time) {
        // Find neuron and process spikes
        for (auto& neuron : neurons_) {
            if (neuron->getId() == neuronId) {
                neuron->recordSpike(time);
                break;
            }
        }
    }
    
    void updateSTDPConnections(nlm::Timestamp time) {
        // Update synaptic weights based on spike timing
        for (auto& preNeuron : neurons_) {
            auto preSpikes = preNeuron->getSpikeHistory();
            if (preSpikes.empty()) continue;
            
            nlm::Timestamp preTime = preSpikes.back();
            
            for (auto& postNeuron : neurons_) {
                if (preNeuron->getId() == postNeuron->getId()) continue;
                
                auto postSpikes = postNeuron->getSpikeHistory();
                if (postSpikes.empty()) continue;
                
                nlm::Timestamp postTime = postSpikes.back();
                
                // Calculate STDP update
                float deltaTime = postTime - preTime;
                if (std::abs(deltaTime) < 100) { // Within learning window
                    float weightChange = calculateSTDPWeightChange(deltaTime);
                    
                    // Apply weight change (simplified)
                    float currentWeight = getConnectionWeight(preNeuron->getId(), postNeuron->getId());
                    float newWeight = currentWeight + weightChange;
                    
                    setConnectionWeight(preNeuron->getId(), postNeuron->getId(), newWeight);
                }
            }
        }
    }
    
    float calculateSTDPWeightChange(float deltaTime) {
        // STDP learning rule
        if (deltaTime > 0) {
            // Post before pre: Long-Term Depression (LTD)
            return -0.01f * std::exp(-std::abs(deltaTime) / 20.0f);
        } else {
            // Pre before post: Long-Term Potentiation (LTP)
            return 0.01f * std::exp(-std::abs(deltaTime) / 20.0f);
        }
    }
    
    void logNetworkStats(nlm::Timestamp time) {
        int firingCount = 0;
        float totalActivity = 0.0f;
        
        for (auto& neuron : neurons_) {
            if (neuron->isFiring()) firingCount++;
            totalActivity += neuron->getFiringRate();
        }
        
        std::cout << "Time " << time << ": " 
                  << "Firing neurons: " << firingCount << "/" << neurons_.size() << ", "
                  << "Average firing rate: " << (totalActivity / neurons_.size()) << " Hz" 
                  << std::endl;
    }
    
private:
    std::vector<std::unique_ptr<nlm::Neuron>> neurons_;
    float timeStep_;
    
    // Helper methods for synapse management
    float getConnectionWeight(nlm::NeuronId from, nlm::NeuronId to) {
        // Implementation would look up connection weight
        return 0.5f; // Placeholder
    }
    
    void setConnectionWeight(nlm::NeuronId from, nlm::NeuronId to, float weight) {
        // Implementation would set connection weight
    }
};

int main() {
    // Create and run LIF network
    LIFNetwork network(100, 0.001f);
    network.runSimulation(5000, 15.0f);  // External input of 15.0
    
    return 0;
}
```

### 2.2 Neuronal Population Dynamics

```cpp
#include <nlm/brain/NeuralPopulation.hpp>
#include <nlm/brain/NeuralRegion.hpp>

class PopulationDynamics {
public:
    PopulationDynamics(std::shared_ptr<nlm::Brain> brain) : brain_(brain) {
    }
    
    void createExcitatoryInhibitoryNetwork(int numExc, int numInh) {
        // Create excitatory population
        auto excRegion = brain_->addRegion("Excitatory");
        auto excPop = excRegion->createPopulation("PYR", numExc, nlm::NeuronType::Excitatory);
        
        // Create inhibitory population
        auto inhRegion = brain_->addRegion("Inhibitory");
        auto inhPop = inhRegion->createPopulation("PV", numInh, nlm::NeuronType::Inhibitory);
        
        // Create connections (E->E, E->I, I->E, I->I)
        createConnections(excPop, excPop, 0.5f, 2.0f);  // E->E
        createConnections(excPop, inhPop, 1.0f, 5.0f);  // E->I
        createConnections(inhPop, excPop, 2.0f, 1.0f);  // I->E
        createConnections(inhPop, inhPop, 1.5f, 2.0f);  // I->I
        
        std::cout << "Created E-I network: " << numExc << " excitatory, " << numInh << " inhibitory neurons" << std::endl;
    }
    
    void createConnections(std::shared_ptr<nlm::NeuralPopulation> pre,
                           std::shared_ptr<nlm::NeuralPopulation> post,
                           float weightMean, float weightStd) {
        auto preNeurons = pre->getNeurons();
        auto postNeurons = post->getNeurons();
        
        std::mt19937 rng(42);
        std::normal_distribution<float> weightDist(weightMean, weightStd);
        std::uniform_real_distribution<float> connectionProb(0.0f, 1.0f);
        
        for (auto preNeuron : preNeurons) {
            for (auto postNeuron : postNeurons) {
                if (connectionProb(rng) < 0.1) {  // 10% connection probability
                    float weight = weightDist(rng);
                    weight = std::max(0.0f, weight);  // Ensure positive weight
                    
                    brain_->getBrain()->createSynapse(preNeuron->getId(), postNeuron->getId(), weight);
                }
            }
        }
    }
    
    void analyzeNetworkDynamics(nlm::Timestamp startTime, nlm::Timestamp endTime) {
        std::cout << "Network Dynamics Analysis:" << std::endl;
        std::cout << "=========================" << std::endl;
        
        // Get regions
        auto excRegion = brain_->getBrain()->getRegion(0);  // First region is excitatory
        auto inhRegion = brain_->getBrain()->getRegion(1);  // Second region is inhibitory
        
        // Analyze excitatory population
        auto excNeurons = excRegion->getNeurons();
        int excFiringCount = 0;
        float excTotalRate = 0.0f;
        
        for (auto neuron : excNeurons) {
            if (neuron->isFiring()) excFiringCount++;
            excTotalRate += neuron->getFiringRate();
        }
        
        // Analyze inhibitory population
        auto inhNeurons = inhRegion->getNeurons();
        int inhFiringCount = 0;
        float inhTotalRate = 0.0f;
        
        for (auto neuron : inhNeurons) {
            if (neuron->isFiring()) inhFiringCount++;
            inhTotalRate += neuron->getFiringRate();
        }
        
        // Calculate statistics
        float excAvgRate = (excNeurons.size() > 0) ? (excTotalRate / excNeurons.size()) : 0.0f;
        float inhAvgRate = (inhNeurons.size() > 0) ? (inhTotalRate / inhNeurons.size()) : 0.0f;
        float eI_ratio = (inhAvgRate > 0) ? (excAvgRate / inhAvgRate) : 0.0f;
        
        std::cout << "Excitatory neurons: " << excNeurons.size() << std::endl;
        std::cout << "  Firing rate: " << excAvgRate << " Hz" << std::endl;
        std::cout << "  Firing neurons: " << excFiringCount << "/" << excNeurons.size() << std::endl;
        
        std::cout << "Inhibitory neurons: " << inhNeurons.size() << std::endl;
        std::cout << "  Firing rate: " << inhAvgRate << " Hz" << std::endl;
        std::cout << "  Firing neurons: " << inhFiringCount << "/" << inhNeurons.size() << std::endl;
        
        std::cout << "E/I firing rate ratio: " << eI_ratio << std::endl;
        
        // Analyze correlations
        float correlation = calculateCrossCorrelation(excNeurons, inhNeurons);
        std::cout << "Cross-correlation (E-I): " << correlation << std::endl;
    }
    
private:
    std::shared_ptr<nlm::Brain> brain_;
    
    float calculateCrossCorrelation(const std::vector<std::shared_ptr<nlm::Neuron>>& excNeurons,
                                    const std::vector<std::shared_ptr<nlm::Neuron>>& inhNeurons) {
        // Calculate cross-correlation between excitatory and inhibitory populations
        std::vector<float> excRates, inhRates;
        
        for (auto neuron : excNeurons) {
            excRates.push_back(neuron->getFiringRate());
        }
        
        for (auto neuron : inhNeurons) {
            inhRates.push_back(neuron->getFiringRate());
        }
        
        if (excRates.empty() || inhRates.empty()) return 0.0f;
        
        // Simple correlation calculation
        float excMean = std::accumulate(excRates.begin(), excRates.end(), 0.0f) / excRates.size();
        float inhMean = std::accumulate(inhRates.begin(), inhRates.end(), 0.0f) / inhRates.size();
        
        float covariance = 0.0f;
        float excVar = 0.0f;
        float inhVar = 0.0f;
        
        for (size_t i = 0; i < excRates.size() && i < inhRates.size(); ++i) {
            float excDiff = excRates[i] - excMean;
            float inhDiff = inhRates[i] - inhMean;
            
            covariance += excDiff * inhDiff;
            excVar += excDiff * excDiff;
            inhVar += inhDiff * inhDiff;
        }
        
        covariance /= excRates.size();
        excVar /= excRates.size();
        inhVar /= inhRates.size();
        
        if (excVar > 0 && inhVar > 0) {
            return covariance / std::sqrt(excVar * inhVar);
        } else {
            return 0.0f;
        }
    }
};

int main() {
    // Create brain with population dynamics
    auto config = std::make_shared<nlm::Config>();
    config->set("nlm.brain.plasticity_enabled", true);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Create E-I network
    PopulationDynamics dynamics(brain);
    dynamics.createExcitatoryInhibitoryNetwork(100, 25);  // 100 E, 25 I neurons
    
    // Run simulation and analyze dynamics
    for (nlm::SimulationStep step = 0; step < 2000; ++step) {
        brain->step(step);
        
        if (step == 1999) {  // Analyze at the end
            dynamics.analyzeNetworkDynamics(0, step);
        }
    }
    
    return 0;
}
```

## 3. World Interaction Examples

### 3.1 Simple World Control

```cpp
#include <nlm/environment/SimpleWorld.hpp>
#include <nlm/agent/AgentBrain.hpp>
#include <nlm/sensory/Vision.hpp>
#include <nlm/sensory/Audio.hpp>
#include <nlm/motor/MotorSystem.hpp>

class SimpleWorldAgent {
public:
    SimpleWorldAgent() {
        // Initialize configuration
        auto config = std::make_shared<nlm::Config>();
        config->set("nlm.world.simple.enabled", true);
        config->set("nlm.sensory.vision.enabled", true);
        config->set("nlm.sensory.audio.enabled", true);
        config->set("nlm.motor.action_types", std::vector<std::string>{"move", "turn", "grasp"});
        
        // Create brain
        brain_ = std::make_shared<nlm::Brain>(config);
        brain_->initialize();
        
        // Create world
        world_ = std::make_shared<nlm::SimpleWorld>(config);
        
        // Create agent
        agent_ = std::make_shared<nlm::AgentBrain>(brain_, world_);
        
        // Create sensory processors
        vision_ = std::make_unique<nlm::VisionProcessor>();
        audio_ = std::make_unique<nlm::AudioProcessor>();
        
        // Create motor system
        motor_ = std::make_unique<nlm::MotorSystem>();
        
        std::cout << "SimpleWorldAgent initialized" << std::endl;
    }
    
    void run(int steps) {
        std::cout << "Starting simulation with " << steps << " steps" << std::endl;
        std::cout << "Actions: move, turn, grasp" << std::endl;
        
        for (nlm::SimulationStep step = 0; step < steps; ++step) {
            // Get environment percept
            auto percept = world_->getPercept();
            
            // Process sensory input
            processSensoryInput(percept);
            
            // Step the brain
            brain_->step(step);
            
            // Get motor command
            auto action = agent_->getMotorCommand();
            
            // Apply action to world
            world_->applyAction(action);
            
            // Log state
            if (step % 100 == 0) {
                logState(step);
            }
            
            // Check for termination condition
            if (world_->getEpisodeComplete()) {
                std::cout << "Episode completed at step " << step << std::endl;
                break;
            }
        }
        
        std::cout << "Simulation completed" << std::endl;
    }
    
private:
    void processSensoryInput(const nlm::Percept& percept) {
        // Extract vision features
        if (percept.hasVision()) {
            auto visionInput = percept.getVision();
            vision_->process(visionInput);
            auto features = vision_->getFeatures();
            
            // Store vision features in working memory
            brain_->getWorkingMemory()->store(features, 1.0f);
        }
        
        // Extract audio features
        if (percept.hasAudio()) {
            auto audioInput = percept.getAudio();
            audio_->process(audioInput);
            auto features = audio_->getFeatures();
            
            // Store audio features in working memory
            brain_->getWorkingMemory()->store(features, 0.8f);
        }
        
        // Send sensory input to brain
        brain_->receiveSensoryInput(percept);
    }
    
    void logState(nlm::SimulationStep step) {
        auto action = agent_->getMotorCommand();
        float reward = world_->getReward();
        
        std::cout << "Step " << step << ": "
                  << "Action: " << static_cast<int>(action->getType()) << ", "
                  << "Reward: " << reward << ", "
                  << "Episode reward: " << world_->getEpisodeReward() << "\n";
        
        // Log neural activity
        brain_->logStatus();
    }
    
    std::shared_ptr<nlm::Config> config_;
    std::shared_ptr<nlm::Brain> brain_;
    std::shared_ptr<nlm::SimpleWorld> world_;
    std::shared_ptr<nlm::AgentBrain> agent_;
    std::unique_ptr<nlm::VisionProcessor> vision_;
    std::unique_ptr<nlm::AudioProcessor> audio_;
    std::unique_ptr<nlm::MotorSystem> motor_;
};

int main() {
    // Create and run world agent
    SimpleWorldAgent agent;
    agent.run(5000);  // Run for 5000 simulation steps
    
    return 0;
}
```

### 3.2 Navigation Task

```cpp
#include <nlm/environment/SimpleWorld.hpp>
#include <nlm/agent/AgentBrain.hpp>
#include <nlm/motor/MotorSystem.hpp>

class NavigationAgent {
public:
    NavigationAgent() {
        // Initialize configuration
        auto config = std::make_shared<nlm::Config>();
        config->set("nlm.world.simple.type", "grid_world");
        config->set("nlm.world.simple.width", 20);
        config->set("nlm.world.simple.height", 20);
        config->set("nlm.sensory.vision.range", 10);
        config->set("nlm.sensory.vision.obstacle_detection", true);
        config->set("nlm.motor.action_types", std::vector<std::string>{"move_forward", "turn_left", "turn_right", "move_backward"});
        
        // Create brain
        brain_ = std::make_shared<nlm::Brain>(config);
        brain_->initialize();
        
        // Create world
        world_ = std::make_shared<nlm::SimpleWorld>(config);
        
        // Create agent
        agent_ = std::make_shared<nlm::AgentBrain>(brain_, world_);
        
        // Initialize goal
        goal_ = nlm::Position(15, 15);
        
        std::cout << "NavigationAgent initialized" << std::endl;
        std::cout << "Goal: (" << goal_.x << ", " << goal_.y << ")" << std::endl;
    }
    
    void run(int steps) {
        std::cout << "Starting navigation task with " << steps << " steps" << std::endl;
        
        for (nlm::SimulationStep step = 0; step < steps; ++step) {
            // Get environment percept
            auto percept = world_->getPercept();
            
            // Send sensory input to brain
            brain_->receiveSensoryInput(percept);
            
            // Step the brain
            brain_->step(step);
            
            // Get motor command
            auto action = agent_->getMotorCommand();
            
            // Apply action to world
            world_->applyAction(action);
            
            // Check goal achievement
            auto agentPos = world_->getAgentPosition();
            float distance = calculateDistance(agentPos, goal_);
            
            if (distance < 1.0f) {
                std::cout << "Goal reached at step " << step << "! Distance: " << distance << std::endl;
                break;
            }
            
            // Log progress
            if (step % 100 == 0) {
                logProgress(step, distance);
            }
            
            // Check for termination condition
            if (world_->getEpisodeComplete()) {
                std::cout << "Episode completed without reaching goal at step " << step << std::endl;
                break;
            }
        }
        
        float finalDistance = calculateDistance(world_->getAgentPosition(), goal_);
        std::cout << "Navigation task completed" << std::endl;
        std::cout << "Final distance to goal: " << finalDistance << std::endl;
        std::cout << "Total reward: " << world_->getEpisodeReward() << std::endl;
    }
    
private:
    float calculateDistance(const nlm::Position& pos1, const nlm::Position& pos2) {
        float dx = pos1.x - pos2.x;
        float dy = pos1.y - pos2.y;
        return std::sqrt(dx * dx + dy * dy);
    }
    
    void logProgress(nlm::SimulationStep step, float distance) {
        std::cout << "Step " << step << ": "
                  << "Distance to goal: " << distance << ", "
                  << "Reward: " << world_->getReward() << ", "
                  << "Action: " << static_cast<int>(agent_->getMotorCommand()->getType()) << "\n";
    }
    
    std::shared_ptr<nlm::Config> config_;
    std::shared_ptr<nlm::Brain> brain_;
    std::shared_ptr<nlm::SimpleWorld> world_;
    std::shared_ptr<nlm::AgentBrain> agent_;
    nlm::Position goal_;
};

int main() {
    // Create and run navigation agent
    NavigationAgent agent;
    agent.run(10000);  // Run for 10000 simulation steps
    
    return 0;
}
```

## 4. Cognitive Science Examples

### 4.1 Working Memory and Attention

```cpp
#include <nlm/brain/Brain.hpp>
#include <nlm/memory/NeuralWorkingMemory.hpp>
#include <nlm/cognition/AttentionalSelection.hpp>

class AttentionBasedLearning {
public:
    AttentionBasedLearning(std::shared_ptr<nlm::Brain> brain) 
        : brain_(brain), workingMemory_(brain->getWorkingMemory()),
          attentionalSelection_(brain->getAttention()) {
    }
    
    void runAttentionTask(int steps) {
        std::cout << "Starting attention-based learning task" << std::endl;
        
        // Create competing stimuli
        std::vector<std::vector<float>> stimuli = {
            {1.0f, 0.2f, 0.1f, 0.0f, 0.0f},  // Stimulus A
            {0.0f, 1.0f, 0.3f, 0.1f, 0.0f},  // Stimulus B
            {0.3f, 0.3f, 1.0f, 0.2f, 0.1f},  // Stimulus C
        };
        
        for (nlm::SimulationStep step = 0; step < steps; ++step) {
            // Present different stimuli at different times
            int stimulusIndex = step % stimuli.size();
            auto currentStimulus = stimuli[stimulusIndex];
            
            // Store stimulus in working memory
            workingMemory_->store(currentStimulus, 1.0f);
            
            // Get attention focus
            auto attendedNeurons = attentionalSelection_->processCompetition(
                workingMemory_->getMemoryNeurons(), 0.5f);
            
            // Store attended stimulus
            if (!attendedNeurons.empty()) {
                nlm::NeuronId attendedNeuron = attendedNeurons[0];
                float activation = workingMemory_->getNeuronActivation(attendedNeuron);
                attendedStimuli_.push_back({stimulusIndex, activation, step});
            }
            
            // Step brain
            brain_->step(step);
            
            // Log attention dynamics
            if (step % 200 == 0) {
                logAttentionDynamics(step);
            }
            
            // Decay memory traces
            workingMemory_->update(brain_->getConfig()->getDouble("nlm.simulation.dt").value_or(0.001f));
        }
        
        analyzeLearningPerformance();
    }
    
    void logAttentionDynamics(nlm::SimulationStep step) {
        std::cout << "Step " << step << ": Attention Dynamics" << std::endl;
        std::cout << "  Attended regions: ";
        for (auto region : attentionalSelection_->getAttendedRegions()) {
            std::cout << static_cast<int>(region) << " ";
        }
        std::cout << std::endl;
        
        std::cout << "  Attention winners: ";
        for (auto neuron : attentionalSelection_->getWinners()) {
            std::cout << neuron << " ";
        }
        std::cout << std::endl;
        
        std::cout << "  Inhibition strength: " << attentionalSelection_->getInhibitionStrength() << std::endl;
    }
    
    void analyzeLearningPerformance() {
        std::cout << "Learning Performance Analysis" << std::endl;
        std::cout << "===============================" << std::endl;
        
        if (attendedStimuli_.empty()) return;
        
        // Calculate attention consistency
        std::map<int, int> stimulusCounts;
        for (const auto& attended : attendedStimuli_) {
            stimulusCounts[attended.stimulusIndex]++;
        }
        
        std::cout << "Stimulus attention frequency:" << std::endl;
        for (const auto& pair : stimulusCounts) {
            float frequency = static_cast<float>(pair.second) / attendedStimuli_.size();
            std::cout << "  Stimulus " << pair.first << ": " << frequency * 100 << "%" << std::endl;
        }
        
        // Calculate attention stability
        std::vector<float> attentionStabilities;
        for (const auto& attended : attendedStimuli_) {
            // Look at previous attendances of this stimulus
            int recentCount = 0;
            for (const auto& other : attendedStimuli_) {
                if (other.stimulusIndex == attended.stimulusIndex && 
                    other.step > attended.step - 100 && 
                    other.step < attended.step) {
                    recentCount++;
                }
            }
            
            float stability = static_cast<float>(recentCount) / 100.0f;
            attentionStabilities.push_back(stability);
        }
        
        float avgStability = std::accumulate(attentionStabilities.begin(), 
                                            attentionStabilities.end(), 0.0f) / attentionStabilities.size();
        
        std::cout << "Average attention stability: " << avgStability << std::endl;
    }
    
private:
    std::shared_ptr<nlm::Brain> brain_;
    nlm::NeuralWorkingMemory* workingMemory_;
    nlm::AttentionalSelection* attentionalSelection_;
    std::vector<std::tuple<int, float, nlm::SimulationStep>> attendedStimuli_;
};

int main() {
    // Initialize brain with attentional selection
    auto config = std::make_shared<nlm::Config>();
    config->set("nlm.cognition.attention.enabled", true);
    config->set("nlm.memory.working_memory_capacity", 50);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Create attention-based learning system
    AttentionBasedLearning learning(brain);
    learning.runAttentionTask(2000);
    
    return 0;
}
```

### 4.2 Prediction and Planning

```cpp
#include <nlm/brain/Brain.hpp>
#include <nlm/prediction/PredictionSystem.hpp>
#include <nlm/cognition/NeuralPlanner.hpp>

class PredictivePlanning {
public:
    PredictivePlanning(std::shared_ptr<nlm::Brain> brain) 
        : brain_(brain), predictionSystem_(brain->getPredictionSystem()),
          neuralPlanner_(brain->getPlanner()) {
    }
    
    void runPredictionTask(int steps) {
        std::cout << "Starting predictive planning task" << std::endl;
        
        // Create a simple sequence to predict
        std::vector<nlm::ActionType> sequence = {
            nlm::ActionType::MoveForward,
            nlm::ActionType::TurnLeft,
            nlm::ActionType::MoveForward,
            nlm::ActionType::TurnRight,
            nlm::ActionType::Grasp
        };
        
        // Train prediction system
        trainPredictionSystem(sequence);
        
        // Run prediction and planning
        for (nlm::SimulationStep step = 0; step < steps; ++step) {
            // Get current state
            auto currentState = getCurrentState();
            
            // Predict next actions
            auto predictedActions = predictionSystem_->predictActions(currentState, 3);
            
            // Plan action sequence
            auto actionSequence = neuralPlanner_->planActions(currentState, predictedActions, 5);
            
            // Execute first action in sequence
            if (!actionSequence.empty()) {
                executeAction(actionSequence[0]);
            }
            
            // Step brain
            brain_->step(step);
            
            // Log planning results
            if (step % 200 == 0) {
                logPlanningResults(step, actionSequence);
            }
            
            // Update prediction system with actual outcomes
            updatePredictionSystem(currentState, actionSequence);
        }
        
        evaluatePlanningPerformance();
    }
    
    void trainPredictionSystem(const std::vector<nlm::ActionType>& sequence) {
        std::cout << "Training prediction system on sequence..." << std::endl;
        
        for (size_t i = 0; i < sequence.size(); ++i) {
            // Create state-action pairs
            nlm::State currentState;
            nlm::ActionType action = sequence[i];
            
            // Store in prediction system
            predictionSystem_->storeExperience(currentState, action);
            
            // Create next state (simplified)
            nlm::State nextState;
            if (i < sequence.size() - 1) {
                // Simulate next state
                nextState = generateNextState(currentState, action);
                predictionSystem_->storeExperience(nextState, sequence[i + 1]);
            }
        }
        
        std::cout << "Prediction system trained on sequence of length " << sequence.size() << std::endl;
    }
    
    void logPlanningResults(nlm::SimulationStep step, 
                           const std::vector<nlm::ActionType>& actionSequence) {
        std::cout << "Step " << step << ": Planning Results" << std::endl;
        std::cout << "  Planned actions: ";
        for (auto action : actionSequence) {
            std::cout << static_cast<int>(action) << " ";
        }
        std::cout << std::endl;
        
        // Get prediction confidence
        auto currentState = getCurrentState();
        auto predictions = predictionSystem_->predictActions(currentState, 3);
        
        std::cout << "  Prediction confidence: ";
        for (size_t i = 0; i < predictions.size() && i < 3; ++i) {
            std::cout << predictions[i].confidence << " ";
        }
        std::cout << std::endl;
        
        // Get planning confidence
        float planningConfidence = neuralPlanner_->getPlanningConfidence();
        std::cout << "  Planning confidence: " << planningConfidence << std::endl;
    }
    
    void evaluatePlanningPerformance() {
        std::cout << "Planning Performance Evaluation" << std::endl;
        std::cout << "================================" << std::endl;
        
        // Get prediction accuracy
        float predictionAccuracy = predictionSystem_->getAccuracy();
        std::cout << "Prediction accuracy: " << predictionAccuracy << std::endl;
        
        // Get planning efficiency
        float planningEfficiency = neuralPlanner_->getEfficiency();
        std::cout << "Planning efficiency: " << planningEfficiency << std::endl;
        
        // Get overall performance
        float overallPerformance = (predictionAccuracy + planningEfficiency) / 2.0f;
        std::cout << "Overall performance: " << overallPerformance << std::endl;
        
        // Performance classification
        if (overallPerformance > 0.8f) {
            std::cout << "Performance: Excellent" << std::endl;
        } else if (overallPerformance > 0.6f) {
            std::cout << "Performance: Good" << std::endl;
        } else if (overallPerformance > 0.4f) {
            std::cout << "Performance: Fair" << std::endl;
        } else {
            std::cout << "Performance: Needs improvement" << std::endl;
        }
    }
    
private:
    std::shared_ptr<nlm::Brain> brain_;
    nlm::PredictionSystem* predictionSystem_;
    nlm::NeuralPlanner* neuralPlanner_;
    
    nlm::State getCurrentState() {
        // Extract current state from brain
        nlm::State state;
        
        // Get sensory features
        auto workingMemory = brain_->getWorkingMemory();
        auto features = workingMemory->retrieve();
        state.sensoryFeatures = features;
        
        // Get neural activity
        auto excNeurons = brain_->getRegion(0)->getNeurons();  // Excitatory region
        for (auto neuron : excNeurons) {
            state.neuralActivity.push_back(neuron->getFiringRate());
        }
        
        // Get memory content
        auto memoryContent = workingMemory->retrieve();
        state.memoryContent = memoryContent;
        
        return state;
    }
    
    nlm::State generateNextState(const nlm::State& currentState, nlm::ActionType action) {
        // Generate next state based on current state and action
        nlm::State nextState = currentState;
        
        // Modify sensory features based on action
        for (size_t i = 0; i < nextState.sensoryFeatures.size(); ++i) {
            nextState.sensoryFeatures[i] *= 0.9f;  // Decay
            if (i == static_cast<size_t>(action)) {
                nextState.sensoryFeatures[i] += 0.5f;  // Action effect
            }
        }
        
        // Add noise
        std::mt19937 rng(42);
        std::normal_distribution<float> noise(0.0f, 0.1f);
        
        for (size_t i = 0; i < nextState.sensoryFeatures.size(); ++i) {
            nextState.sensoryFeatures[i] += noise(rng);
            nextState.sensoryFeatures[i] = std::max(0.0f, std::min(1.0f, nextState.sensoryFeatures[i]));
        }
        
        return nextState;
    }
    
    void executeAction(nlm::ActionType action) {
        // Execute action (simplified)
        std::cout << "Executing action: " << static_cast<int>(action) << std::endl;
        
        // In a real implementation, this would:
        // 1. Convert action to motor command
        // 2. Apply to environment
        // 3. Get reward signal
    }
    
    void updatePredictionSystem(const nlm::State& state, 
                               const std::vector<nlm::ActionType>& actionSequence) {
        // Update prediction system with actual outcomes
        if (!actionSequence.empty()) {
            predictionSystem_->update(state, actionSequence[0]);
        }
    }
};

int main() {
    // Initialize brain with prediction and planning
    auto config = std::make_shared<nlm::Config>();
    config->set("nlm.prediction.enabled", true);
    config->set("nlm.cognition.planning.enabled", true);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Create predictive planning system
    PredictivePlanning planning(brain);
    planning.runPredictionTask(3000);
    
    return 0;
}
```

## 5. Performance and Scaling Examples

### 5.1 Large-Scale Network

```cpp
#include <nlm/brain/Brain.hpp>
#include <nlm/performance/ParallelProcessing.hpp>
#include <nlm/performance/MemoryPool.hpp>

class LargeScaleNetwork {
public:
    LargeScaleNetwork(int numNeurons, int numRegions) 
        : numNeurons_(numNeurons), numRegions_(numRegions) {
        initializeLargeScaleSystem();
    }
    
    void initializeLargeScaleSystem() {
        // Configure for large-scale simulation
        auto config = std::make_shared<nlm::Config>();
        config->set("nlm.simulation.max_steps", 10000);
        config->set("nlm.brain.neuron_count", numNeurons_);
        config->set("nlm.brain.region_count", numRegions_);
        config->set("nlm.performance.parallel.enabled", true);
        config->set("nlm.performance.memory_pool.enabled", true);
        config->set("nlm.performance.simd.enabled", true);
        
        // Create brain with performance optimizations
        brain_ = std::make_shared<nlm::Brain>(config);
        brain_->initialize();
        
        // Initialize performance components
        parallelProcessor_ = std::make_unique<nlm::ParallelProcessing>(config);
        memoryPool_ = std::make_unique<nlm::MemoryPool>(config);
        
        std::cout << "Large-scale network initialized" << std::endl;
        std::cout << "  Neurons: " << numNeurons_ << std::endl;
        std::cout << "  Regions: " << numRegions_ << std::endl;
        std::cout << "  Simulation steps: " << config->getInt("nlm.simulation.max_steps", 10000) << std::endl;
    }
    
    void runPerformanceTest() {
        std::cout << "Running performance test" << std::endl;
        
        // Measure baseline performance
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Run simulation with monitoring
        for (nlm::SimulationStep step = 0; step < 1000; ++step) {
            brain_->step(step);
            
            // Monitor performance
            if (step % 100 == 0) {
                monitorPerformance(step);
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        std::cout << "Performance test completed" << std::endl;
        std::cout << "  Total time: " << duration.count() << " ms" << std::endl;
        std::cout << "  Average step time: " << (duration.count() / 1000.0f) << " ms" << std::endl;
        std::cout << "  Steps per second: " << (1000.0f / (duration.count() / 1000.0f)) << std::endl;
    }
    
    void monitorPerformance(nlm::SimulationStep step) {
        // Get performance metrics
        auto performanceMetrics = brain_->getPerformanceMetrics();
        
        std::cout << "Step " << step << ": "
                  << "Neurons processed: " << performanceMetrics.neuronsProcessed << ", "
                  << "Spikes generated: " << performanceMetrics.spikesGenerated << ", "
                  << "Memory usage: " << performanceMetrics.memoryUsage << " MB, "
                  << "CPU usage: " << performanceMetrics.cpuUsage << "%" << std::endl;
        
        // Log to performance log file
        logPerformanceMetrics(step, performanceMetrics);
    }
    
    void logPerformanceMetrics(nlm::SimulationStep step, 
                              const nlm::PerformanceMetrics& metrics) {
        // Log performance metrics to file
        std::ofstream logFile("performance.log", std::ios::app);
        if (logFile.is_open()) {
            logFile << step << ","
                   << metrics.neuronsProcessed << ","
                   << metrics.spikesGenerated << ","
                   << metrics.memoryUsage << ","
                   << metrics.cpuUsage << ","
                   << metrics.stepTime << std::endl;
            logFile.close();
        }
    }
    
    void analyzeScalingCharacteristics() {
        std::cout << "Scaling Characteristics Analysis" << std::endl;
        std::cout << "==================================" << std::endl;
        
        // Test with different network sizes
        std::vector<int> neuronCounts = {100, 500, 1000, 2000};
        std::vector<double> stepTimes;
        
        for (int neuronCount : neuronCounts) {
            auto config = std::make_shared<nlm::Config>();
            config->set("nlm.simulation.max_steps", 100);
            config->set("nlm.brain.neuron_count", neuronCount);
            config->set("nlm.brain.region_count", std::max(1, neuronCount / 100));
            
            auto testBrain = std::make_shared<nlm::Brain>(config);
            testBrain->initialize();
            
            // Measure step time
            auto startTime = std::chrono::high_resolution_clock::now();
            for (nlm::SimulationStep step = 0; step < 100; ++step) {
                testBrain->step(step);
            }
            auto endTime = std::chrono::high_resolution_clock::now();
            
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            double avgStepTime = duration.count() / 100.0f;
            stepTimes.push_back(avgStepTime);
            
            std::cout << "Neurons: " << neuronCount << ", "
                     << "Avg step time: " << avgStepTime << " ms, "
                     << "Steps per second: " << (100.0f / avgStepTime) << std::endl;
        }
        
        // Analyze scaling
        analyzeScalingCurve(neuronCounts, stepTimes);
    }
    
    void analyzeScalingCurve(const std::vector<int>& neuronCounts, 
                            const std::vector<double>& stepTimes) {
        std::cout << "Scaling Analysis" << std::endl;
        std::cout << "=================" << std::endl;
        
        if (stepTimes.size() < 2) return;
        
        // Calculate scaling exponent
        double logNeuronCount1 = std::log10(neuronCounts[0]);
        double logNeuronCount2 = std::log10(neuronCounts.back());
        double logStepTime1 = std::log10(stepTimes[0]);
        double logStepTime2 = std::log10(stepTimes.back());
        
        double scalingExponent = (logStepTime2 - logStepTime1) / (logNeuronCount2 - logNeuronCount1);
        
        std::cout << "Scaling exponent: " << scalingExponent << std::endl;
        
        if (scalingExponent < 1.0) {
            std::cout << "Scaling: Sublinear (good)" << std::endl;
        } else if (scalingExponent < 2.0) {
            std::cout << "Scaling: Linear (acceptable)" << std::endl;
        } else {
            std::cout << "Scaling: Superlinear (needs optimization)" << std::endl;
        }
        
        // Predict performance at larger scale
        int targetNeurons = 10000;
        double targetNeuronLog = std::log10(targetNeurons);
        double predictedStepTime = std::pow(10, logStepTime1 + scalingExponent * (targetNeuronLog - logNeuronCount1));
        
        std::cout << "Predicted step time at " << targetNeurons << " neurons: " 
                 << predictedStepTime << " ms" << std::endl;
        std::cout << "Predicted steps per second: " << (1000.0f / predictedStepTime) << std::endl;
    }
    
private:
    std::shared_ptr<nlm::Config> config_;
    std::shared_ptr<nlm::Brain> brain_;
    std::unique_ptr<nlm::ParallelProcessing> parallelProcessor_;
    std::unique_ptr<nlm::MemoryPool> memoryPool_;
    int numNeurons_;
    int numRegions_;
};

int main() {
    // Create and test large-scale network
    LargeScaleNetwork network(2000, 20);  // 2000 neurons, 20 regions
    network.runPerformanceTest();
    network.analyzeScalingCharacteristics();
    
    return 0;
}
```

### 5.2 Memory-Efficient Simulation

```cpp
#include <nlm/brain/Brain.hpp>
#include <nlm/performance/MemoryPool.hpp>
#include <nlm/performance/CheckpointSystem.hpp>

class MemoryEfficientSimulation {
public:
    MemoryEfficientSimulation(int maxMemoryMB) 
        : maxMemoryMB_(maxMemoryMB) {
        initializeMemoryEfficientSystem();
    }
    
    void initializeMemoryEfficientSystem() {
        // Configure for memory efficiency
        auto config = std::make_shared<nlm::Config>();
        config->set("nlm.simulation.max_steps", 5000);
        config->set("nlm.performance.memory_pool.enabled", true);
        config->set("nlm.performance.memory_pool.max_size_mb", maxMemoryMB_);
        config->set("nlm.performance.checkpoint.enabled", true);
        config->set("nlm.performance.checkpoint.interval_steps", 1000);
        config->set("nlm.performance.checkpoint.file", "checkpoints/simulation_checkpoint");
        
        // Create brain with memory optimizations
        brain_ = std::make_shared<nlm::Brain>(config);
        brain_->initialize();
        
        // Initialize memory pool and checkpoint system
        memoryPool_ = std::make_unique<nlm::MemoryPool>(config);
        checkpointSystem_ = std::make_unique<nlm::CheckpointSystem>(config);
        
        std::cout << "Memory-efficient simulation initialized" << std::endl;
        std::cout << "Maximum memory: " << maxMemoryMB_ << " MB" << std::endl;
    }
    
    void runMemoryEfficientSimulation() {
        std::cout << "Running memory-efficient simulation" << std::endl;
        
        // Monitor memory usage
        for (nlm::SimulationStep step = 0; step < 5000; ++step) {
            // Step brain
            brain_->step(step);
            
            // Check memory usage
            auto memoryUsage = memoryPool_->getCurrentUsage();
            
            if (memoryUsage > maxMemoryMB_ * 0.9f) {
                std::cout << "Warning: High memory usage (" << memoryUsage << " MB)" << std::endl;
            }
            
            // Save checkpoint at intervals
            if (step % 1000 == 0 && step > 0) {
                saveCheckpoint(step);
            }
            
            // Log memory usage
            if (step % 500 == 0) {
                logMemoryUsage(step, memoryUsage);
            }
        }
        
        // Save final checkpoint
        saveCheckpoint(5000);
        
        std::cout << "Memory-efficient simulation completed" << std::endl;
    }
    
    void saveCheckpoint(nlm::SimulationStep step) {
        std::cout << "Saving checkpoint at step " << step << std::endl;
        
        if (checkpointSystem_->save(step)) {
            std::cout << "Checkpoint saved successfully" << std::endl;
        } else {
            std::cout << "Warning: Failed to save checkpoint" << std::endl;
        }
    }
    
    void logMemoryUsage(nlm::SimulationStep step, float memoryUsageMB) {
        std::cout << "Step " << step << ": Memory usage " << memoryUsageMB << " MB" << std::endl;
    }
    
    void loadCheckpoint(nlm::SimulationStep step) {
        std::cout << "Loading checkpoint from step " << step << std::endl;
        
        if (checkpointSystem_->load(step)) {
            std::cout << "Checkpoint loaded successfully" << std::endl;
        } else {
            std::cout << "Warning: Failed to load checkpoint" << std::endl;
        }
    }
    
    void analyzeMemoryEfficiency() {
        std::cout << "Memory Efficiency Analysis" << std::endl;
        std::cout << "==========================" << std::endl;
        
        // Get memory statistics
        auto memoryStats = memoryPool_->getStatistics();
        
        std::cout << "Memory pool statistics:" << std::endl;
        std::cout << "  Total allocations: " << memoryStats.totalAllocations << std::endl;
        std::cout << "  Total deallocations: " << memoryStats.totalDeallocations << std::endl;
        std::cout << "  Current memory usage: " << memoryStats.currentUsage << " MB" << std::endl;
        std::cout << "  Maximum memory usage: " << memoryStats.maxUsage << " MB" << std::endl;
        std::cout << "  Allocation efficiency: " << memoryStats.efficiency << std::endl;
        
        // Get checkpoint statistics
        auto checkpointStats = checkpointSystem_->getStatistics();
        
        std::cout << "Checkpoint system statistics:" << std::endl;
        std::cout << "  Total checkpoints saved: " << checkpointStats.totalSaved << std::endl;
        std::cout << "  Total checkpoints loaded: " << checkpointStats.totalLoaded << std::endl;
        std::cout << "  Average checkpoint size: " << checkpointStats.avgSize << " MB" << std::endl;
        std::cout << "  Checkpoint save time: " << checkpointStats.saveTime << " ms" << std::endl;
        std::cout << "  Checkpoint load time: " << checkpointStats.loadTime << " ms" << std::endl;
    }
    
private:
    std::shared_ptr<nlm::Config> config_;
    std::shared_ptr<nlm::Brain> brain_;
    std::unique_ptr<nlm::MemoryPool> memoryPool_;
    std::unique_ptr<nlm::CheckpointSystem> checkpointSystem_;
    int maxMemoryMB_;
};

int main() {
    // Create and run memory-efficient simulation
    MemoryEfficientSimulation simulation(500);  // 500 MB memory limit
    simulation.runMemoryEfficientSimulation();
    simulation.analyzeMemoryEfficiency();
    
    // Example: Load checkpoint
    simulation.loadCheckpoint(3000);
    
    return 0;
}
```

## 6. Research Applications

### 6.1 Cognitive Architecture Research

```cpp
#include <nlm/brain/Brain.hpp>
#include <nlm/memory/NeuralWorkingMemory.hpp>
#include <nlm/cognition/AttentionalSelection.hpp>
#include <nlm/cognition/NeuralPlanner.hpp>
#include <nlm/prediction/PredictionSystem.hpp>

class CognitiveArchitectureResearch {
public:
    CognitiveArchitectureResearch() {
        // Configure for cognitive architecture research
        auto config = std::make_shared<nlm::Config>();
        config->set("nlm.simulation.max_steps", 20000);
        config->set("nlm.brain.plasticity_enabled", true);
        config->set("nlm.memory.working_memory_capacity", 200);
        config->set("nlm.memory.episodic_memory_enabled", true);
        config->set("nlm.memory.associative_memory_enabled", true);
        config->set("nlm.cognition.attention.enabled", true);
        config->set("nlm.cognition.planning.enabled", true);
        config->set("nlm.prediction.enabled", true);
        config->set("nlm.neuromodulation.dopamine.enabled", true);
        config->set("nlm.neuromodulation.curiosity.enabled", true);
        config->set("nlm.neuromodulation.novelty.enabled", true);
        config->set("nlm.development.enabled", true);
        config->set("nlm.development.stages", 5);
        
        // Create brain
        brain_ = std::make_shared<nlm::Brain>(config);
        brain_->initialize();
        
        // Initialize research metrics
        initializeResearchMetrics();
        
        std::cout << "Cognitive Architecture Research initialized" << std::endl;
        std::cout << "Research focus: Working memory and attention integration" << std::endl;
    }
    
    void runCognitiveResearch() {
        std::cout << "Running cognitive architecture research" << std::endl;
        
        // Research Question 1: How does working memory capacity affect attentional selection?
        investigateWorkingMemoryCapacity();
        
        // Research Question 2: How does neuromodulation affect planning performance?
        investigateNeuromodulationEffects();
        
        // Research Question 3: How does prediction accuracy relate to action selection?
        investigatePredictionEffects();
        
        // Research Question 4: How does development affect cognitive capabilities?
        investigateDevelopmentEffects();
        
        // Analyze and report results
        analyzeResearchResults();
    }
    
    void investigateWorkingMemoryCapacity() {
        std::cout << "\n=== Research: Working Memory Capacity Effects ===" << std::endl;
        
        std::vector<int> capacities = {50, 100, 200, 400};
        
        for (int capacity : capacities) {
            auto config = std::make_shared<nlm::Config>();
            config->set("nlm.simulation.max_steps", 5000);
            config->set("nlm.memory.working_memory_capacity", capacity);
            config->set("nlm.cognition.attention.enabled", true);
            
            auto testBrain = std::make_shared<nlm::Brain>(config);
            testBrain->initialize();
            
            // Create competing stimuli task
            float performance = runCompetingStimuliTask(testBrain, capacity);
            
            capacityResults_[capacity] = performance;
            
            std::cout << "Capacity: " << capacity << ", Performance: " << performance << std::endl;
        }
        
        analyzeCapacityEffects();
    }
    
    void investigateNeuromodulationEffects() {
        std::cout << "\n=== Research: Neuromodulation Effects ===" << std::endl;
        
        std::vector<std::string> neuromodulators = {"dopamine", "curiosity", "novelty"};
        
        for (const auto& neuromodulator : neuromodulators) {
            auto config = std::make_shared<nlm::Config>();
            config->set("nlm.simulation.max_steps", 5000);
            config->set("nlm.neuromodulation.dopamine.enabled", neuromodulator == "dopamine");
            config->set("nlm.neuromodulation.curiosity.enabled", neuromodulator == "curiosity");
            config->set("nlm.neuromodulation.novelty.enabled", neuromodulator == "novelty");
            
            auto testBrain = std::make_shared<nlm::Brain>(config);
            testBrain->initialize();
            
            // Run planning task
            float performance = runPlanningTask(testBrain);
            
            neuromodulationResults_[neuromodulator] = performance;
            
            std::cout << "Neuromodulator: " << neuromodulator << ", Performance: " << performance << std::endl;
        }
        
        analyzeNeuromodulationEffects();
    }
    
    void investigatePredictionEffects() {
        std::cout << "\n=== Research: Prediction Effects ===" << std::endl;
        
        std::vector<int> predictionSteps = {1, 3, 5, 10};
        
        for (int steps : predictionSteps) {
            auto config = std::make_shared<nlm::Config>();
            config->set("nlm.simulation.max_steps", 5000);
            config->set("nlm.prediction.enabled", true);
            config->set("nlm.prediction.prediction_horizon", steps);
            
            auto testBrain = std::make_shared<nlm::Brain>(config);
            testBrain->initialize();
            
            // Run action selection task
            float performance = runActionSelectionTask(testBrain, steps);
            
            predictionResults_[steps] = performance;
            
            std::cout << "Prediction horizon: " << steps << ", Performance: " << performance << std::endl;
        }
        
        analyzePredictionEffects();
    }
    
    void investigateDevelopmentEffects() {
        std::cout << "\n=== Research: Development Effects ===" << std::endl;
        
        std::vector<int> stages = {2, 3, 4, 5};
        
        for (int stageCount : stages) {
            auto config = std::make_shared<nlm::Config>();
            config->set("nlm.simulation.max_steps", 5000);
            config->set("nlm.development.enabled", true);
            config->set("nlm.development.stages", stageCount);
            
            auto testBrain = std::make_shared<nlm::Brain>(config);
            testBrain->initialize();
            
            // Run cognitive development task
            float performance = runCognitiveDevelopmentTask(testBrain, stageCount);
            
            developmentResults_[stageCount] = performance;
            
            std::cout << "Development stages: " << stageCount << ", Performance: " << performance << std::endl;
        }
        
        analyzeDevelopmentEffects();
    }
    
    void initializeResearchMetrics() {
        // Initialize metrics collection
        researchMetrics_.startTime = std::chrono::high_resolution_clock::now();
        researchMetrics_.totalSteps = 0;
        researchMetrics_.totalExperiments = 0;
    }
    
    void analyzeResearchResults() {
        std::cout << "\n=== Research Results Analysis ===" << std::endl;
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - researchMetrics_.startTime);
        
        std::cout << "Total research time: " << duration.count() << " seconds" << std::endl;
        std::cout << "Total experiments run: " << researchMetrics_.totalExperiments << std::endl;
        
        // Analyze capacity effects
        if (!capacityResults_.empty()) {
            std::cout << "\nWorking Memory Capacity Effects:" << std::endl;
            for (const auto& pair : capacityResults_) {
                std::cout << "  Capacity " << pair.first << ": " << pair.second << std::endl;
            }
        }
        
        // Analyze neuromodulation effects
        if (!neuromulationResults_.empty()) {
            std::cout << "\nNeuromodulation Effects:" << std::endl;
            for (const auto& pair : neuromodulationResults_) {
                std::cout << "  " << pair.first << ": " << pair.second << std::endl;
            }
        }
        
        // Analyze prediction effects
        if (!predictionResults_.empty()) {
            std::cout << "\nPrediction Effects:" << std::endl;
            for (const auto& pair : predictionResults_) {
                std::cout << "  Horizon " << pair.first << ": " << pair.second << std::endl;
            }
        }
        
        // Analyze development effects
        if (!developmentResults_.empty()) {
            std::cout << "\nDevelopment Effects:" << std::endl;
            for (const auto& pair : developmentResults_) {
                std::cout << "  Stages " << pair.first << ": " << pair.second << std::endl;
            }
        }
        
        // Save results to file
        saveResearchResults();
    }
    
    void saveResearchResults() {
        std::ofstream resultsFile("research_results.txt", std::ios::app);
        if (resultsFile.is_open()) {
            resultsFile << "=== Cognitive Architecture Research Results ===" << std::endl;
            resultsFile << "Timestamp: " << std::chrono::system_clock::now() << std::endl;
            resultsFile << "Total experiments: " << researchMetrics_.totalExperiments << std::endl;
            
            resultsFile << "\nWorking Memory Capacity Effects:" << std::endl;
            for (const auto& pair : capacityResults_) {
                resultsFile << "  Capacity " << pair.first << ": " << pair.second << std::endl;
            }
            
            resultsFile << "\nNeuromodulation Effects:" << std::endl;
            for (const auto& pair : neuromodulationResults_) {
                resultsFile << "  " << pair.first << ": " << pair.second << std::endl;
            }
            
            resultsFile << "\nPrediction Effects:" << std::endl;
            for (const auto& pair : predictionResults_) {
                resultsFile << "  Horizon " << pair.first << ": " << pair.second << std::endl;
            }
            
            resultsFile << "\nDevelopment Effects:" << std::endl;
            for (const auto& pair : developmentResults_) {
                resultsFile << "  Stages " << pair.first << ": " << pair.second << std::endl;
            }
            
            resultsFile << std::endl;
            resultsFile.close();
            
            std::cout << "Research results saved to research_results.txt" << std::endl;
        }
    }
    
private:
    float runCompetingStimuliTask(std::shared_ptr<nlm::Brain> brain, int capacity) {
        // Run task with given working memory capacity
        // Return performance score
        return 0.75f;  // Placeholder
    }
    
    float runPlanningTask(std::shared_ptr<nlm::Brain> brain) {
        // Run planning task
        // Return performance score
        return 0.80f;  // Placeholder
    }
    
    float runActionSelectionTask(std::shared_ptr<nlm::Brain> brain, int predictionSteps) {
        // Run action selection task
        // Return performance score
        return 0.85f;  // Placeholder
    }
    
    float runCognitiveDevelopmentTask(std::shared_ptr<nlm::Brain> brain, int stageCount) {
        // Run cognitive development task
        // Return performance score
        return 0.70f;  // Placeholder
    }
    
    std::shared_ptr<nlm::Config> config_;
    std::shared_ptr<nlm::Brain> brain_;
    
    struct ResearchMetrics {
        std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
        nlm::SimulationStep totalSteps;
        int totalExperiments;
    };
    
    ResearchMetrics researchMetrics_;
    
    std::map<int, float> capacityResults_;
    std::map<std::string, float> neuromodulationResults_;
    std::map<int, float> predictionResults_;
    std::map<int, float> developmentResults_;
};

int main() {
    // Run cognitive architecture research
    CognitiveArchitectureResearch research;
    research.runCognitiveResearch();
    
    return 0;
}
```

## 7. Example Scripts

### 7.1 Configuration Generator

```bash
#!/bin/bash
# generate_config.sh

# Generate configuration for different scenarios

function generate_simple_config {
    cat > simple_config.json << EOF
{
  "nlm": {
    "simulation": {
      "max_steps": 1000,
      "dt": 0.001
    },
    "brain": {
      "neuron_count": 100,
      "region_count": 3,
      "plasticity_enabled": true
    },
    "memory": {
      "working_memory_capacity": 50
    }
  }
}
EOF
    echo "Generated simple_config.json"
}

function generate_research_config {
    cat > research_config.json << EOF
{
  "nlm": {
    "simulation": {
      "max_steps": 5000,
      "dt": 0.001,
      "time_unit": "ms"
    },
    "brain": {
      "neuron_count": 500,
      "region_count": 8,
      "plasticity_enabled": true,
      "development_enabled": true,
      "prediction_enabled": true
    },
    "memory": {
      "working_memory_capacity": 200,
      "working_memory_decay_rate": 0.01,
      "episodic_memory_enabled": true,
      "episodic_memory_capacity": 2000
    },
    "neuromodulation": {
      "dopamine_enabled": true,
      "curiosity_enabled": true,
      "novelty_enabled": true,
      "prediction_error_enabled": true
    },
    "development": {
      "stages": 4,
      "plasticity_decay_rate": 0.97
    }
  }
}
EOF
    echo "Generated research_config.json"
}

function generate_performance_config {
    cat > performance_config.json << EOF
{
  "nlm": {
    "simulation": {
      "max_steps": 10000,
      "dt": 0.0005,
      "time_unit": "ms"
    },
    "brain": {
      "neuron_count": 1000,
      "region_count": 15,
      "plasticity_enabled": true
    },
    "performance": {
      "parallel_enabled": true,
      "memory_pool_enabled": true,
      "simd_enabled": true,
      "checkpoint_enabled": true
    }
  }
}
EOF
    echo "Generated performance_config.json"
}

echo "Configuration generator script"

echo "Choose configuration type:"
echo "1) Simple"
echo "2) Research"
echo "3) Performance"

read -p "Enter choice (1-3): " choice

case $choice in
    1)
        generate_simple_config
        ;;
    2)
        generate_research_config
        ;;
    3)
        generate_performance_config
        ;;
    *)
        echo "Invalid choice"
        exit 1
        ;;
esac
EOF

chmod +x generate_config.sh
```

### 7.2 Benchmark Script

```bash
#!/bin/bash
# benchmark.sh

# Benchmark NLM performance with different configurations

echo "NLM Performance Benchmark"
echo "=========================="

# Create benchmarks directory
mkdir -p benchmarks

# Function to run benchmark with given configuration
function benchmark_config {
    local config_file="$1"
    local benchmark_name="$2"
    local steps="$3"
    
    echo "Running $benchmark_name benchmark..."
    
    # Copy configuration
    cp "$config_file" "benchmarks/config_${benchmark_name}.json"
    
    # Run benchmark
    time ./nlm --config benchmarks/config_${benchmark_name}.json --max-steps $steps
}

# Run benchmarks with different configurations
benchmark_config "configs/simple_config.json" "simple" 1000
benchmark_config "configs/research_config.json" "research" 5000
benchmark_config "configs/performance_config.json" "performance" 10000

# Generate performance report
echo "\nBenchmark Report"
echo "================"
echo "Results saved to benchmarks/ directory"
```

### 7.3 Documentation Generator

```python
#!/usr/bin/env python3
# generate_docs.py

import json
import os
from datetime import datetime

def generate_api_docs():
    """Generate API documentation from code comments"""
    api_docs = {
        "generated_at": datetime.now().isoformat(),
        "version": "0.1.0",
        "classes": [
            {
                "name": "Brain",
                "file": "src/brain/Brain.hpp",
                "description": "Central coordinator of the neural system",
                "methods": ["initialize", "step", "receiveSensoryInput", "produceAction"]
            },
            {
                "name": "Neuron",
                "file": "src/brain/Neuron.hpp",
                "description": "Individual neuron implementing LIF dynamics",
                "methods": ["stepLIF", "checkThreshold", "recordSpike"]
            },
            {
                "name": "NeuralWorkingMemory",
                "file": "src/memory/NeuralWorkingMemory.hpp",
                "description": "Working memory using persistent neural activity",
                "methods": ["store", "retrieve", "update", "clear"]
            }
        ]
    }
    
    with open("docs/generated_api.json", "w") as f:
        json.dump(api_docs, f, indent=2)
    
    print("Generated API documentation")

def generate_example_docs():
    """Generate example documentation from example code"""
    examples = {
        "generated_at": datetime.now().isoformat(),
        "version": "0.1.0",
        "examples": [
            {
                "title": "Simple Neuron Network",
                "file": "examples/simple_neuron_network.cpp",
                "difficulty": "beginner",
                "topics": ["neurons", "synapses", "plasticity"]
            },
            {
                "title": "Memory-Based Pattern Recognition",
                "file": "examples/pattern_recognition.cpp",
                "difficulty": "intermediate",
                "topics": ["working_memory", "attention", "learning"]
            },
            {
                "title": "Prediction and Planning",
                "file": "examples/prediction_planning.cpp",
                "difficulty": "advanced",
                "topics": ["prediction", "planning", "cognition"]
            }
        ]
    }
    
    with open("docs/generated_examples.json", "w") as f:
        json.dump(examples, f, indent=2)
    
    print("Generated example documentation")

if __name__ == "__main__":
    os.makedirs("docs", exist_ok=True)
    generate_api_docs()
    generate_example_docs()
    print("Documentation generation complete")
```

## Conclusion

This document provides practical examples for using NLM (熙然) across various scenarios:

### Key Examples Include:

1. **Quick Start Examples:** Basic neural computations and memory systems
2. **Neural Dynamics Examples:** LIF neuron networks and population dynamics
3. **World Interaction Examples:** Simple world control and navigation tasks
4. **Cognitive Science Examples:** Attention-based learning and prediction/planning
5. **Performance and Scaling Examples:** Large-scale networks and memory-efficient simulation
6. **Research Applications:** Cognitive architecture research with comprehensive experiments

### Example Scripts:

1. **Configuration Generator:** Scripts to generate different configuration scenarios
2. **Benchmark Script:** Performance benchmarking with different configurations
3. **Documentation Generator:** Automated documentation generation from code

### Usage:

1. **Start with Quick Start Examples:** Learn basic NLM operations
2. **Explore Neural Dynamics Examples:** Understand neural computation
3. **Try World Interaction Examples:** Experiment with environmental interaction
4. **Study Cognitive Science Examples:** Investigate higher-level cognition
5. **Run Performance Examples:** Test scalability and memory efficiency
6. **Conduct Research Examples:** Perform systematic experiments

### Next Steps:

1. **Run the existing examples:** `./nlm --help` for available demos
2. **Create your own examples:** Adapt existing examples for your research
3. **Benchmark your hardware:** Use the benchmark script for performance analysis
4. **Document your experiments:** Use the documentation generator
5. **Share your findings:** Contribute examples to the NLM community

These examples provide a foundation for using NLM across various research scenarios and applications.
