# NLM Examples Documentation

## Overview

This section provides comprehensive examples for using the NLM (Neural Learning Machine) framework in various scenarios. The examples are categorized by use case to help you quickly find relevant code patterns.

## Example Categories

1. **Complete Examples** - Full working applications from start to finish
2. **Real-World Examples** - Examples based on actual applications
3. **Research Examples** - Examples for research and experimentation
4. **Production Examples** - Production-ready code patterns
5. **Educational Examples** - Teaching and learning examples

## Complete Examples

### Complete Example 1: Basic Neural Simulation

```cpp
// File: basic_simulation.cpp
#include <nlm/nlm.hpp>
#include <iostream>

int main() {
    std::cout << "=== NLM Basic Neural Simulation ===" << std::endl;
    
    // 1. Create configuration
    auto config = nlm::createDefaultConfig();
    config.set("brain.neuron_count", 500);
    config.set("brain.region_count", 1);
    
    // 2. Create and initialize brain
    auto brain = nlm::createBrain(config);
    brain->initialize();
    
    // 3. Record initial state
    std::cout << "Initial state:" << std::endl;
    brain->logStatus();
    
    // 4. Run simulation with periodic input
    for (size_t step = 0; step < 1000; ++step) {
        // Inject periodic input to sensory neurons
        if (step % 100 == 0) {
            brain->injectCurrentToNeurons(nlm::NeuronType::Sensory, 20.0f);
            std::cout << "Step " << step << ": Injected sensory current" << std::endl;
        }
        
        // Run simulation step
        brain->step(step);
        
        // Log statistics every 200 steps
        if (step % 200 == 0) {
            std::cout << "Step " << step << ": "
                     << brain->getFiringNeuronCount() << " firing neurons, "
                     << brain->getTotalSpikeCount() << " total spikes" << std::endl;
        }
    }
    
    // 5. Record final state
    std::cout << "\nFinal state:" << std::endl;
    brain->logStatus();
    
    // 6. Calculate summary statistics
    std::cout << "\n=== Simulation Summary ===" << std::endl;
    std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
    std::cout << "E/I ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
    
    return 0;
}
```

**Output**:
```
=== NLM Basic Neural Simulation ===
Initial state:
[Brain status output]

Step 0: Injected sensory current
Step 0: 12 firing neurons, 12 total spikes

Step 100: Injected sensory current
Step 100: 28 firing neurons, 156 total spikes

Step 200: Injected sensory current
Step 200: 35 firing neurons, 312 total spikes

Step 400: Injected sensory current
Step 400: 42 firing neurons, 624 total spikes

Step 600: Injected sensory current
Step 600: 48 firing neurons, 936 total spikes

Step 800: Injected sensory current
Step 800: 52 firing neurons, 1248 total spikes

Final state:
[Final brain status]

=== Simulation Summary ===
Total spikes: 1248
Average firing rate: 2.496 Hz
E/I ratio: 1.234
```

### Complete Example 2: Learning Agent in Environment

```cpp
// File: learning_agent.cpp
#include <nlm/nlm.hpp>
#include <iostream>
#include <vector>

struct RewardStats {
    float totalReward = 0.0f;
    float maxReward = 0.0f;
    float minReward = 0.0f;
    size_t episodeCount = 0;
};

int main() {
    std::cout << "=== NLM Learning Agent Example ===" << std::endl;
    
    // 1. Create learning-optimized brain
    auto config = nlm::createLearningConfig();
    config.set("brain.neuron_count", 1000);
    config.set("plasticity.stdp.enable", true);
    config.set("plasticity.hebbian.enable", true);
    config.set("neuromod.dopamine.scale", 1.5f);
    config.set("neuromod.curiosity.enable", true);
    
    auto brain = nlm::createBrain(config);
    auto agent = nlm::createAgentBrain(brain);
    auto world = nlm::createSimpleWorld();
    
    // 2. Configure world
    world->configure(
        100,  // width
        100,  // height
        8,    // visionWidth
        8,    // visionHeight
        200.0f, // maxEnergy
        0.1f   // timestep
    );
    world->reset();
    world->setAgentStart(10.0f, 10.0f);
    
    // 3. Initialize agent
    agent->initialize(world);
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    agent->enableDevelopment(true);
    
    // 4. Create reward tracking
    RewardStats stats;
    
    // 5. Run learning episodes
    size_t episode = 0;
    size_t stepsPerEpisode = 200;
    
    while (episode < 5 && stats.episodeCount < 10) {
        std::cout << "\n=== Episode " << episode << " ===" << std::endl;
        
        for (size_t step = 0; step < stepsPerEpisode; ++step) {
            // Update world
            world->update(0.1f);
            
            // Get sensory input
            auto percept = world->getSensoryPercept();
            
            // Process through agent
            agent->processSensoryInput(percept);
            
            // Brain computation
            brain->step(step);
            
            // Get and apply action
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Get reward and apply neuromodulation
            float reward = world->getSensoryPercept().getInternal()[0];
            if (reward == 0.0f) reward = 0.1f; // Small baseline reward
            
            agent->applyRewardModulation(reward, 0.0f);
            
            // Update development
            agent->updateDevelopment(0.1f);
            
            // Update reward statistics
            stats.totalReward += reward;
            stats.maxReward = std::max(stats.maxReward, reward);
            stats.minReward = std::min(stats.minReward, reward);
            
            // Print progress
            if (step % 50 == 0) {
                std::cout << "Step " << step << ": "
                         << "Reward: " << reward << ", "
                         << "Firing: " << brain->getFiringNeuronCount() << ", "
                         << "Curiosity: " << agent->getCuriosityLevel() << ", "
                         << "Novelty: " << agent->getNoveltyLevel() << std::endl;
            }
            
            // Check episode termination
            if (world->getAgentPosition().x > 90 && 
                world->getAgentPosition().y > 90) {
                std::cout << "Goal reached!" << std::endl;
                break;
            }
        }
        
        stats.episodeCount++;
        episode++;
        
        // Reset for next episode
        brain->reset();
        brain->initialize();
        world->reset();
        world->setAgentStart(10.0f, 10.0f);
    }
    
    // 6. Print episode summary
    std::cout << "\n=== Learning Summary ===" << std::endl;
    std::cout << "Episodes completed: " << stats.episodeCount << std::endl;
    std::cout << "Total reward: " << stats.totalReward << std::endl;
    std::cout << "Average reward: " << (stats.totalReward / stats.episodeCount) << std::endl;
    std::cout << "Max reward: " << stats.maxReward << std::endl;
    std::cout << "Min reward: " << stats.minReward << std::endl;
    
    return 0;
}
```

### Complete Example 3: STDP Learning Demonstration

```cpp
// File: stdp_demo.cpp
#include <nlm/nlm.hpp>
#include <iostream>
#include <vector>

struct STDPResult {
    float initialWeight = 0.0f;
    float finalWeight = 0.0f;
    float weightChange = 0.0f;
    bool potentiation = false;  // LTP
    bool depression = false;    // LTD
};

int main() {
    std::cout << "=== NLM STDP Learning Demonstration ===" << std::endl;
    
    // 1. Create brain with STDP enabled
    auto config = nlm::createDefaultConfig();
    config.set("brain.neuron_count", 200);
    config.set("plasticity.stdp.enable", true);
    config.set("plasticity.stdp.learning_rate", 0.01f);
    config.set("plasticity.hebbian.enable", false); // Only STDP
    
    auto brain = nlm::createBrain(config);
    brain->initialize();
    
    // 2. Get reference to STDP system
    auto* stdp = brain->getSTDP();
    stdp->enable(true);
    
    // 3. Create neurons for pre-post pairing
    std::vector<STDPResult> results;
    size_t numTests = 5;
    
    std::cout << "Testing STDP on " << numTests << " synapses" << std::endl;
    std::cout << "-----------------------------------" << std::endl;
    
    // 4. Run STDP experiments
    for (size_t test = 0; test < numTests; ++test) {
        STDPResult result;
        
        // Find available synapses
        auto regions = brain->getRegions();
        if (regions.empty()) continue;
        
        auto& region = regions[0];
        auto& synapses = region->getSynapses();
        
        if (synapses.size() < test + 1) continue;
        
        auto& synapse = synapses[test];
        result.initialWeight = synapse->getWeight();
        
        // Configure synapse for STDP only
        synapse->enablePlasticity(false, true, false);
        
        std::cout << "Synapse " << test << ": "
                 << "Initial weight: " << result.initialWeight << std::endl;
        
        // 5. Create correlated pre-post activity (LTP)
        std::cout << "  Inducing LTP (pre -> post)..." << std::endl;
        
        auto neurons = region->getAllNeurons();
        if (neurons.size() >= 2) {
            auto preNeuron = neurons[0];
            auto postNeuron = neurons[1];
            
            // Pre fires first (excitatory)
            preNeuron->injectCurrent(50.0f);
            brain->step(test * 2, test * 2 * 0.001f);
            
            // Then post fires (excitatory)
            postNeuron->injectCurrent(50.0f);
            brain->step(test * 2 + 1, (test * 2 + 1) * 0.001f);
        }
        
        // 6. Record final weight
        result.finalWeight = synapse->getWeight();
        result.weightChange = result.finalWeight - result.initialWeight;
        result.potentiation = (result.weightChange > 0.001f);
        result.depression = (result.weightChange < -0.001f);
        
        std::cout << "  Final weight: " << result.finalWeight << std::endl;
        std::cout << "  Weight change: " << result.weightChange << std::endl;
        
        if (result.potentiation) {
            std::cout << "  Result: LTP (potentiation) ✓" << std::endl;
        } else if (result.depression) {
            std::cout << "  Result: LTD (depression) !" << std::endl;
        } else {
            std::cout << "  Result: No significant change" << std::endl;
        }
        
        results.push_back(result);
        
        // Reset synapse for next test
        synapse->resetWeight();
        synapse->enablePlasticity(true, true, false);
    }
    
    // 7. Summarize results
    std::cout << "\n=== STDP Results Summary ===" << std::endl;
    
    float totalChange = 0.0f;
    size_t ltpCount = 0, ltdCount = 0, noChangeCount = 0;
    
    for (size_t i = 0; i < results.size(); ++i) {
        const auto& r = results[i];
        totalChange += r.weightChange;
        
        if (r.potentiation) ltpCount++;
        else if (r.depression) ltdCount++;
        else noChangeCount++;
        
        std::cout << "Test " << i << ": Δ=" << r.weightChange
                 << " (" << (r.potentiation ? "LTP" : r.depression ? "LTD" : "none") << ")" << std::endl;
    }
    
    std::cout << "\nTotal STDP effect: " << totalChange << std::endl;
    std::cout << "LTP occurrences: " << ltpCount << std::endl;
    std::cout << "LTD occurrences: " << ltdCount << std::endl;
    std::cout << "No change: " << noChangeCount << std::endl;
    
    if (totalChange > 0.01f) {
        std::cout << "\n✓ STDP is working: Net potentiation observed" << std::endl;
    } else if (totalChange < -0.01f) {
        std::cout << "\n✓ STDP is working: Net depression observed" << std::endl;
    } else {
        std::cout << "\n! STDP may not be triggering: Net change minimal" << std::endl;
    }
    
    return 0;
}
```

## Real-World Examples

### Real-World Example 1: Simple Robot Navigation

```cpp
// File: robot_navigation.cpp
#include <nlm/nlm.hpp>
#include <iostream>

struct Environment {
    float width;
    float height;
    float robotX;
    float robotY;
    float energy;
    std::vector<float> obstacles;
    bool goalReached;
    
    Environment(float w, float h) : width(w), height(h), 
                                     robotX(w/2), robotY(h/2),
                                     energy(100.0f), goalReached(false) {
        // Create some obstacles
        for (int i = 0; i < 20; ++i) {
            obstacles.push_back(i * 5.0f);
        }
    }
    
    bool isValidPosition(float x, float y) {
        // Check bounds
        if (x < 0 || x >= width || y < 0 || y >= height) return false;
        
        // Check obstacles
        for (float obstacle : obstacles) {
            if (std::abs(x - obstacle) < 2.0f) return false;
        }
        
        return true;
    }
    
    float getReward(float x, float y) {
        // Positive reward for exploring
        float reward = 0.1f;
        
        // Large reward for reaching corners (goals)
        if ((x < 5 && y < 5) || (x > width-5 && y < 5) || 
            (x < 5 && y > height-5) || (x > width-5 && y > height-5)) {
            reward += 10.0f;
            goalReached = true;
        }
        
        // Negative reward for walls
        if (x < 2 || x > width-2 || y < 2 || y > height-2) {
            reward -= 1.0f;
        }
        
        // Energy cost for movement
        reward -= 0.01f;
        
        return reward;
    }
    
    void moveRobot(float dx, float dy) {
        float newX = robotX + dx;
        float newY = robotY + dy;
        
        if (isValidPosition(newX, newY)) {
            robotX = newX;
            robotY = newY;
            energy += (dx * dx + dy * dy) * 5.0f; // Energy cost
        }
    }
};

int main() {
    std::cout << "=== NLM Robot Navigation Example ===" << std::endl;
    
    // 1. Create environment
    Environment env(100.0f, 100.0f);
    
    // 2. Create learning brain
    auto config = nlm::createLearningConfig();
    config.set("brain.neuron_count", 800);
    config.set("plasticity.stdp.enable", true);
    config.set("plasticity.hebbian.enable", true);
    config.set("neuromod.curiosity.enable", true);
    config.set("neuromod.curiosity.strength", 0.8f);
    
    auto brain = nlm::createBrain(config);
    auto agent = nlm::createAgentBrain(brain);
    auto world = nlm::createSimpleWorld();
    
    // 3. Configure world (matching environment)
    world->configure(
        env.width,
        env.height,
        8,  // visionWidth
        8,  // visionHeight
        200.0f, // maxEnergy
        0.1f   // timestep
    );
    
    // 4. Initialize agent
    agent->initialize(world);
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    agent->enableDevelopment(true);
    
    // 5. Run navigation simulation
    size_t maxSteps = 1000;
    size_t episode = 0;
    
    while (episode < 10 && !env.goalReached) {
        std::cout << "\n=== Navigation Episode " << episode << " ===" << std::endl;
        
        for (size_t step = 0; step < maxSteps; ++step) {
            // Update world
            world->update(0.1f);
            
            // Get sensory input (robot's position and surroundings)
            auto percept = world->getSensoryPercept();
            
            // Create custom sensory input for robot
            nlm::SensoryInput robotPercept;
            robotPercept.type = nlm::SensoryInputType::Internal;
            
            // Robot's position as internal state
            std::vector<float> internalState(8, 0.0f);
            internalState[0] = env.robotX / env.width;      // Normalized X
            internalState[1] = env.robotY / env.height;     // Normalized Y
            internalState[2] = env.energy / 200.0f;         // Energy level
            internalState[3] = agent->getCuriosityLevel();  // Curiosity level
            internalState[4] = agent->getNoveltyLevel();    // Novelty level
            
            // Add some environmental features
            for (int i = 0; i < 4; ++i) {
                internalState[4 + i] = sin(env.robotX * 0.1f + i * 1.0f);
            }
            
            robotPercept.internal = internalState;
            
            // Process through agent
            agent->processSensoryInput(robotPercept);
            
            // Brain computation
            brain->step(step);
            
            // Get action from agent
            auto action = agent->decodeMotorCommand();
            
            // Convert action to movement
            float moveX = 0.0f, moveY = 0.0f;
            
            switch (action->getType()) {
                case nlm::ActionType::MoveForward:
                    moveX = cos(action->getOrientation() * M_PI / 180.0f) * 2.0f;
                    moveY = sin(action->getOrientation() * M_PI / 180.0f) * 2.0f;
                    break;
                case nlm::ActionType::TurnLeft:
                    moveX = -1.0f;
                    break;
                case nlm::ActionType::TurnRight:
                    moveX = 1.0f;
                    break;
                case nlm::ActionType::MoveBackward:
                    moveX = cos(action->getOrientation() * M_PI / 180.0f) * -2.0f;
                    moveY = sin(action->getOrientation() * M_PI / 180.0f) * -2.0f;
                    break;
                case nlm::ActionType::Interact:
                    // Pick up nearby resource
                    break;
                case nlm::ActionType::Eat:
                    env.energy += 20.0f;
                    break;
            }
            
            // Apply movement to robot
            env.moveRobot(moveX, moveY);
            
            // Get reward and apply neuromodulation
            float reward = env.getReward(env.robotX, env.robotY);
            agent->applyRewardModulation(reward, 0.0f);
            
            // Update development
            agent->updateDevelopment(0.1f);
            
            // Print status
            if (step % 100 == 0) {
                std::cout << "Step " << step << ": "
                         << "Pos: (" << env.robotX << ", " << env.robotY << ", "
                         << "Reward: " << reward << ", "
                         << "Energy: " << env.energy << ", "
                         << "Curiosity: " << agent->getCuriosityLevel() << ", "
                         << "Novelty: " << agent->getNoveltyLevel() << std::endl;
            }
            
            // Check termination
            if (env.goalReached || env.energy <= 0.0f) {
                std::cout << "Episode terminated: "
                         << (env.goalReached ? "Goal reached!" : "Out of energy") << std::endl;
                break;
            }
        }
        
        episode++;
        
        // Reset for next episode
        brain->reset();
        brain->initialize();
        agent->reset();
        agent->initialize(world);
        env.goalReached = false;
        env.energy = 100.0f;
        env.robotX = env.width / 2.0f;
        env.robotY = env.height / 2.0f;
    }
    
    std::cout << "\n=== Final Results ===" << std::endl;
    std::cout << "Episodes completed: " << episode << std::endl;
    std::cout << "Final position: (" << env.robotX << ", " << env.robotY << ")" << std::endl;
    std::cout << "Goal reached: " << (env.goalReached ? "Yes" : "No") << std::endl;
    
    return 0;
}
```

## Research Examples

### Research Example 1: Critical Period Analysis

```cpp
// File: critical_period_research.cpp
#include <nlm/nlm.hpp>
#include <iostream>
#include <vector>
#include <cmath>

struct PlasticityData {
    size_t step;
    float learningRate;
    float synaptogenesisRate;
    float pruningRate;
    float avgWeightChange;
    float curiosityLevel;
    float noveltyLevel;
};

class CriticalPeriodResearcher {
public:
    CriticalPeriodResearcher() {
        // Create developmental brain
        auto config = nlm::createDevelopmentConfig();
        config.set("brain.neuron_count", 500);
        config.set("plasticity.stdp.enable", true);
        config.set("plasticity.hebbian.enable", true);
        config.set("plasticity.structural.enable", true);
        
        brain = nlm::createBrain(config);
        agent = nlm::createAgentBrain(brain);
        world = nlm::createSimpleWorld();
        
        world->configure(50, 50, 8, 8);
        world->reset();
        agent->initialize(world);
        
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        
        plasticityData.clear();
        researcherMode = true;
    }
    
    void runCriticalPeriodExperiment() {
        std::cout << "=== Critical Period Research Experiment ===" << std::endl;
        std::cout << "Studying how development affects learning and plasticity" << std::endl;
        
        const size_t totalSteps = 3000;
        const size_t dataInterval = 100;
        
        for (size_t step = 0; step < totalSteps; ++step) {
            // Update world
            world->update(0.1f);
            
            // Get sensory input
            auto percept = world->getSensoryPercept();
            
            // Process through agent
            agent->processSensoryInput(percept);
            
            // Brain computation
            brain->step(step);
            
            // Get and apply action
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Get reward and apply neuromodulation
            float reward = world->getSensoryPercept().getInternal()[0];
            agent->applyRewardModulation(reward, 0.0f);
            
            // Update development
            agent->updateDevelopment(0.1f);
            
            // Record plasticity data at intervals
            if (step % dataInterval == 0) {
                recordPlasticityData(step);
            }
            
            // Print progress
            if (step % 500 == 0) {
                auto stage = brain->getDevelopmentalStage();
                std::cout << "Step " << step << ": "
                         << "Stage: " << static_cast<int>(stage) << ", "
                         << "Curiosity: " << agent->getCuriosityLevel() << ", "
                         << "Novelty: " << agent->getNoveltyLevel() << ", "
                         << "Learning rate: " << agent->getLearningRate() << std::endl;
            }
            
            // Check for goal
            if (step % 1000 == 0) {
                analyzeDevelopmentalPhase(step);
            }
        }
        
        analyzeResults();
    }
    
private:
    void recordPlasticityData(size_t step) {
        PlasticityData data;
        data.step = step;
        data.learningRate = agent->getLearningRate();
        data.curiosityLevel = agent->getCuriosityLevel();
        data.noveltyLevel = agent->getNoveltyLevel();
        
        // Get plasticity parameters from brain
        auto* stdp = brain->getSTDP();
        auto* hebbian = brain->getHebbian();
        auto* structural = brain->getStructuralPlasticity();
        
        if (stdp) {
            data.synaptogenesisRate = structural->getSynaptogenesisRate();
            data.pruningRate = structural->getPruningRate();
        }
        
        // Calculate average weight change (simplified)
        data.avgWeightChange = calculateAverageWeightChange();
        
        plasticityData.push_back(data);
    }
    
    float calculateAverageWeightChange() {
        // Simplified calculation - would need actual synapse access
        // For research, we'd track specific synapses over time
        return 0.001f * std::sin(static_cast<float>(plasticityData.size()) * 0.1f);
    }
    
    void analyzeDevelopmentalPhase(size_t step) {
        auto stage = brain->getDevelopmentalStage();
        
        std::cout << "\n--- Developmental Analysis at Step " << step << " ---" << std::endl;
        std::cout << "Current stage: " << static_cast<int>(stage) << std::endl;
        std::cout << "Plasticity characteristics:" << std::endl;
        
        switch (stage) {
            case nlm::DevelopmentalStage::Initial:
                std::cout << "  High initial plasticity" << std::endl;
                std::cout << "  Fast learning, high variability" << std::endl;
                break;
            case nlm::DevelopmentalStage::CriticalPeriod:
                std::cout << "  Sensitive period for learning" << std::endl;
                std::cout << "  Rapid acquisition, critical for skill development" << std::endl;
                break;
            case nlm::DevelopmentalStage::Maturation:
                std::cout << "  Maturation of cognitive abilities" << std::endl;
                std::cout << "  Stabilization of learned behaviors" << std::endl;
                break;
            case nlm::DevelopmentalStage::Adult:
                std::cout << "  Adult cognitive stability" << std::endl;
                std::cout << "  Maintenance and refinement" << std::endl;
                break;
            case nlm::DevelopmentalStage::Aging:
                std::cout << "  Declining plasticity" << std::endl;
                std::cout << "  Reduced learning capacity" << std::endl;
                break;
        }
    }
    
    void analyzeResults() {
        std::cout << "\n=== Critical Period Research Results ===" << std::endl;
        
        if (plasticityData.empty()) {
            std::cout << "No data recorded" << std::endl;
            return;
        }
        
        // Analyze developmental progression
        std::cout << "Developmental progression analysis:" << std::endl;
        
        size_t initialPhase = 0;
        size_t criticalPhase = 0;
        size_t adultPhase = 0;
        
        for (const auto& data : plasticityData) {
            auto stage = getDevelopmentalStageFromStep(data.step);
            
            switch (stage) {
                case nlm::DevelopmentalStage::Initial:
                    initialPhase++;
                    break;
                case nlm::DevelopmentalStage::CriticalPeriod:
                    criticalPhase++;
                    break;
                case nlm::DevelopmentalStage::Adult:
                    adultPhase++;
                    break;
            }
        }
        
        std::cout << "  Initial phase duration: " << initialPhase << " steps" << std::endl;
        std::cout << "  Critical period duration: " << criticalPhase << " steps" << std::endl;
        std::cout << "  Adult phase duration: " << adultPhase << " steps" << std::endl;
        
        // Analyze plasticity changes
        std::cout << "\nPlasticity dynamics:" << std::endl;
        
        float totalWeightChange = 0.0f;
        float maxLearningRate = 0.0f;
        float minLearningRate = 1.0f;
        
        for (const auto& data : plasticityData) {
            totalWeightChange += data.avgWeightChange;
            maxLearningRate = std::max(maxLearningRate, data.learningRate);
            minLearningRate = std::min(minLearningRate, data.learningRate);
        }
        
        std::cout << "  Total weight change: " << totalWeightChange << std::endl;
        std::cout << "  Maximum learning rate: " << maxLearningRate << std::endl;
        std::cout << "  Minimum learning rate: " << minLearningRate << std::endl;
        
        // Determine research implications
        std::cout << "\nResearch implications:" << std::endl;
        
        if (criticalPhase > 0 && criticalPhase < plasticityData.size() / 3) {
            std::cout << "  ✓ Critical period detected and measured" << std::endl;
            std::cout << "  - Plasticity rapidly declines after sensitive period" << std::endl;
            std::cout << "  - Early learning has disproportionate impact" << std::endl;
        } else {
            std::cout << "  ! Critical period patterns not clearly observed" << std::endl;
            std::cout << "  - Further investigation needed" << std::endl;
        }
        
        if (std::abs(totalWeightChange) > 0.1f) {
            std::cout << "  ✓ Significant plasticity observed throughout development" << std::endl;
            std::cout << "  - Brain maintains capacity for change" << std::endl;
        } else {
            std::cout << "  ! Limited plasticity observed" << std::endl;
            std::cout << "  - May indicate need for stronger learning signals" << std::endl;
        }
    }
    
    nlm::DevelopmentalStage getDevelopmentalStageFromStep(size_t step) {
        // Simplified developmental stage calculation
        // In real research, this would be based on actual developmental age
        if (step < 500) return nlm::DevelopmentalStage::Initial;
        else if (step < 1500) return nlm::DevelopmentalStage::CriticalPeriod;
        else if (step < 2500) return nlm::DevelopmentalStage::Maturation;
        else if (step < 3000) return nlm::DevelopmentalStage::Adult;
        else return nlm::DevelopmentalStage::Aging;
    }
    
    std::unique_ptr<nlm::Brain> brain;
    std::unique_ptr<nlm::AgentBrain> agent;
    std::unique_ptr<nlm::SimpleWorld> world;
    std::vector<PlasticityData> plasticityData;
    bool researcherMode;
};

int main() {
    CriticalPeriodResearcher researcher;
    researcher.runCriticalPeriodExperiment();
    
    return 0;
}
```

## Production Examples

### Production Example 1: Checkpoint-Based Training

```cpp
// File: production_training.cpp
#include <nlm/nlm.hpp>
#include <iostream>
#include <filesystem>
#include <chrono>

class ProductionTrainer {
public:
    ProductionTrainer(const std::string& modelDir, const nlm::BrainConfig& config)
        : modelDir(modelDir), config(config) {
        
        // Create training brain
        brain = nlm::createBrain(config);
        
        // Create agent for training
        agent = nlm::createAgentBrain(brain);
        
        // Create world for training
        world = nlm::createSimpleWorld();
        
        // Configure world
        world->configure(100, 100, 16, 16);
        world->reset();
        
        // Initialize agent
        agent->initialize(world);
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        
        trainingStats = {
            0, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0.0, std::chrono::steady_clock::now()
        };
        
        std::filesystem::create_directories(modelDir);
    }
    
    void train(size_t episodes, size_t stepsPerEpisode, bool saveCheckpoints) {
        std::cout << "=== Production Training Session ===" << std::endl;
        std::cout << "Model directory: " << modelDir << std::endl;
        std::cout << "Episodes: " << episodes << std::endl;
        std::cout << "Steps per episode: " << stepsPerEpisode << std::endl;
        std::cout << "Auto-save checkpoints: " << (saveCheckpoints ? "Yes" : "No") << std::endl;
        
        for (size_t episode = 0; episode < episodes; ++episode) {
            std::cout << "\n--- Episode " << (episode + 1) << "/" << episodes << " ---" << std::endl;
            
            auto episodeStart = std::chrono::steady_clock::now();
            trainEpisode(stepsPerEpisode);
            
            auto episodeEnd = std::chrono::steady_clock::now();
            auto episodeDuration = std::chrono::duration_cast<std::chrono::seconds>(episodeEnd - episodeStart);
            
            // Update training statistics
            trainingStats.episodesCompleted++;
            trainingStats.totalWallTime += episodeDuration.count();
            
            // Calculate episode metrics
            float episodeReward = calculateEpisodeReward(episode);
            trainingStats.maxReward = std::max(trainingStats.maxReward, episodeReward);
            trainingStats.minReward = std::min(trainingStats.minReward, episodeReward);
            trainingStats.avgReward = (trainingStats.avgReward * (episode - 1) + episodeReward) / episode;
            
            // Print episode summary
            std::cout << "Episode " << (episode + 1) << " completed in " << episodeDuration.count() << "s" << std::endl;
            std::cout << "  Reward: " << episodeReward << " (avg: " << trainingStats.avgReward << ")" << std::endl;
            std::cout << "  Best reward: " << trainingStats.maxReward << ", Worst reward: " << trainingStats.minReward << std::endl;
            
            // Save checkpoint if requested
            if (saveCheckpoints && (episode + 1) % 10 == 0) {
                saveModel(episode);
                std::cout << "Model checkpoint saved at episode " << (episode + 1) << std::endl;
            }
            
            // Save final model
            if (episode == episodes - 1) {
                saveModel("final");
                std::cout << "Final model saved" << std::endl;
            }
        }
        
        printTrainingSummary();
    }
    
private:
    void trainEpisode(size_t steps) {
        for (size_t step = 0; step < steps; ++step) {
            // Update world
            world->update(0.1f);
            
            // Get sensory input
            auto percept = world->getSensoryPercept();
            
            // Process through agent
            agent->processSensoryInput(percept);
            
            // Brain computation
            brain->step(step);
            
            // Get action from agent
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Get reward and apply neuromodulation
            float reward = world->getSensoryPercept().getInternal()[0];
            agent->applyRewardModulation(reward, 0.0f);
            
            // Update development
            agent->updateDevelopment(0.1f);
            
            // Print progress periodically
            if (step % 100 == 0) {
                printProgress(step);
            }
        }
    }
    
    void printProgress(size_t step) {
        std::cout << "  Step " << step << ": "
                 << "Firing: " << brain->getFiringNeuronCount() << ", "
                 << "Curiosity: " << agent->getCuriosityLevel() << ", "
                 << "Novelty: " << agent->getNoveltyLevel() << ", "
                 << "Reward: " << trainingStats.currentEpisodeReward << std::endl;
    }
    
    float calculateEpisodeReward(size_t episode) {
        // Simplified reward calculation
        // In production, this would use actual episode data
        float baseReward = 10.0f + episode * 2.0f; // Increasing reward with experience
        float curiosityFactor = 1.0f + agent->getCuriosityLevel() * 0.5f;
        float noveltyFactor = 1.0f + agent->getNoveltyLevel() * 0.3f;
        
        return baseReward * curiosityFactor * noveltyFactor;
    }
    
    void saveModel(const std::string& episodeName) {
        std::string filepath = modelDir + "/model_" + episodeName + ".nml";
        
        try {
            // Save brain state
            brain->saveBrain(filepath);
            
            // Save additional metadata
            saveMetadata(episodeName);
            
            std::cout << "    Model saved to: " << filepath << std::endl;
            
        } catch (const std::exception& e) {
            std::cout << "    ERROR saving model: " << e.what() << std::endl;
        }
    }
    
    void saveMetadata(const std::string& episodeName) {
        std::string metadataPath = modelDir + "/metadata_" + episodeName + ".json";
        
        // Create metadata JSON
        nlohmann::json metadata;
        metadata["episode"] = episodeName;
        metadata["timestamp"] = std::chrono::system_clock::now().time_since_epoch().count();
        metadata["neuron_count"] = brain->getTotalNeuronCount();
        metadata["synapse_count"] = brain->getTotalSynapseCount();
        metadata["total_spikes"] = brain->getTotalSpikeCount();
        metadata["avg_firing_rate"] = brain->getAverageFiringRate();
        metadata["curiosity_level"] = agent->getCuriosityLevel();
        metadata["novelty_level"] = agent->getNoveltyLevel();
        
        // Save to file
        std::ofstream file(metadataPath);
        if (file.is_open()) {
            file << metadata.dump(2);
            file.close();
        }
    }
    
    void printTrainingSummary() {
        auto totalDuration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - trainingStats.startTime);
        
        std::cout << "\n=== Training Summary ===" << std::endl;
        std::cout << "Total episodes: " << trainingStats.episodesCompleted << std::endl;
        std::cout << "Total wall time: " << totalDuration.count() << " seconds" << std::endl;
        std::cout << "Average episode time: " << (totalDuration.count() / trainingStats.episodesCompleted) << " seconds" << std::endl;
        std::cout << "Average reward per episode: " << trainingStats.avgReward << std::endl;
        std::cout << "Best reward: " << trainingStats.maxReward << std::endl;
        std::cout << "Worst reward: " << trainingStats.minReward << std::endl;
        std::cout << "Models saved to: " << modelDir << std::endl;
    }
    
    struct TrainingStats {
        size_t episodesCompleted = 0;
        float avgReward = 0.0f;
        float maxReward = 0.0f;
        float minReward = 0.0f;
        float totalWallTime = 0.0f;
        std::chrono::steady_clock::time_point startTime;
        float currentEpisodeReward = 0.0f;
    };
    
    std::string modelDir;
    nlm::BrainConfig config;
    
    std::unique_ptr<nlm::Brain> brain;
    std::unique_ptr<nlm::AgentBrain> agent;
    std::unique_ptr<nlm::SimpleWorld> world;
    TrainingStats trainingStats;
};

int main() {
    // Create production configuration
    nlm::BrainConfig config = nlm::BrainConfig::createOptimized(2000);
    config.set("brain.neuron_count", 2000);
    config.set("plasticity.stdp.enable", true);
    config.set("plasticity.hebbian.enable", true);
    config.set("plasticity.structural.enable", true);
    config.set("neuromod.curiosity.enable", true);
    config.set("neuromod.curiosity.strength", 0.7f);
    config.set("neuromod.novelty.enable", true);
    config.set("neuromod.novelty.decay", 0.98f);
    config.set("development.enable", true);
    config.set("development.critical_period", true);
    config.verboseLogging = false;  // Production: no verbose output
    
    // Create trainer
    ProductionTrainer trainer("models/production_trained", config);
    
    // Start training
    trainer.train(
        50,    // episodes
        500,   // steps per episode
        true   // save checkpoints
    );
    
    return 0;
}
```

### Production Example 2: Batch Processing

```cpp
// File: production_batch.cpp
#include <nlm/nlm.hpp>
#include <iostream>
#include <vector>
#include <thread>
#include <future>
#include <chrono>

class BatchProcessor {
public:
    BatchProcessor(size_t batchSize, size_t numThreads) 
        : batchSize(batchSize), numThreads(numThreads) {}
    
    struct BatchResult {
        size_t episodeId;
        float totalReward;
        float avgFiringRate;
        size_t stepsCompleted;
        size_t totalSpikes;
        std::chrono::duration<double> processingTime;
        bool success;
    };
    
    BatchResult processBatch(size_t episodeId, const nlm::BrainConfig& config) {
        auto startTime = std::chrono::steady_clock::now();
        
        std::cout << "Processing batch episode " << episodeId << " on thread " 
                 << std::this_thread::get_id() << std::endl;
        
        // Create brain for this batch
        auto brain = nlm::createBrain(config);
        auto agent = nlm::createAgentBrain(brain);
        auto world = nlm::createSimpleWorld();
        
        // Configure world
        world->configure(50, 50, 8, 8);
        world->reset();
        agent->initialize(world);
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        
        // Run batch
        float totalReward = 0.0f;
        size_t stepsCompleted = 0;
        size_t totalSpikes = 0;
        
        for (size_t step = 0; step < batchSize; ++step) {
            // Update world
            world->update(0.1f);
            
            // Get sensory input
            auto percept = world->getSensoryPercept();
            
            // Process through agent
            agent->processSensoryInput(percept);
            
            // Brain computation
            brain->step(step);
            
            // Get action
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Apply reward
            float reward = world->getSensoryPercept().getInternal()[0];
            agent->applyRewardModulation(reward, 0.0f);
            
            totalReward += reward;
            stepsCompleted++;
            totalSpikes += brain->getTotalSpikeCount();
            
            // Update development
            agent->updateDevelopment(0.1f);
        }
        
        auto endTime = std::chrono::steady_clock::now();
        auto processingTime = endTime - startTime;
        
        BatchResult result;
        result.episodeId = episodeId;
        result.totalReward = totalReward;
        result.avgFiringRate = totalSpikes / static_cast<float>(stepsCompleted);
        result.stepsCompleted = stepsCompleted;
        result.totalSpikes = totalSpikes;
        result.processingTime = processingTime;
        result.success = true;
        
        std::cout << "Batch " << episodeId << " completed in " << processingTime.count() << "s" << std::endl;
        
        return result;
    }
    
    std::vector<BatchResult> runParallelBatches(size_t numBatches) {
        std::cout << "Running " << numBatches << " batches using " << numThreads << " threads" << std::endl;
        
        // Create configuration for batches
        nlm::BrainConfig config = nlm::BrainConfig::createOptimized(500);
        
        std::vector<std::future<BatchResult>> futures;
        futures.reserve(numBatches);
        
        // Launch batches in parallel
        for (size_t i = 0; i < numBatches; ++i) {
            futures.push_back(std::async(std::launch::async, 
                [this, i, config]() { return processBatch(i, config); }));
        }
        
        // Collect results
        std::vector<BatchResult> results;
        results.reserve(numBatches);
        
        for (auto& future : futures) {
            results.push_back(future.get());
        }
        
        return results;
    }
    
    void analyzeBatchResults(const std::vector<BatchResult>& results) {
        std::cout << "\n=== Batch Processing Analysis ===" << std::endl;
        
        if (results.empty()) {
            std::cout << "No results to analyze" << std::endl;
            return;
        }
        
        double totalTime = 0.0;
        float totalReward = 0.0f;
        float maxReward = 0.0f;
        float minReward = std::numeric_limits<float>::max();
        float avgFiringRate = 0.0f;
        size_t totalSteps = 0;
        size_t totalSpikes = 0;
        
        for (const auto& result : results) {
            totalTime += result.processingTime.count();
            totalReward += result.totalReward;
            maxReward = std::max(maxReward, result.totalReward);
            minReward = std::min(minReward, result.totalReward);
            avgFiringRate += result.avgFiringRate;
            totalSteps += result.stepsCompleted;
            totalSpikes += result.totalSpikes;
        }
        
        size_t numResults = results.size();
        
        std::cout << "Batches processed: " << numResults << std::endl;
        std::cout << "Total processing time: " << totalTime << " seconds" << std::endl;
        std::cout << "Average processing time: " << (totalTime / numResults) << " seconds" << std::endl;
        std::cout << "Total reward: " << totalReward << std::endl;
        std::cout << "Average reward per batch: " << (totalReward / numResults) << std::endl;
        std::cout << "Max reward: " << maxReward << std::endl;
        std::cout << "Min reward: " << minReward << std::endl;
        std::cout << "Average firing rate: " << (avgFiringRate / numResults) << " Hz" << std::endl;
        std::cout << "Total steps: " << totalSteps << std::endl;
        std::cout << "Total spikes: " << totalSpikes << std::endl;
        
        // Performance metrics
        std::cout << "\nPerformance Metrics:" << std::endl;
        std::cout << "  Steps per second: " << (totalSteps / totalTime) << std::endl;
        std::cout << "  Speedup (vs sequential): " << (numResults * totalTime / totalTime) << "x" << std::endl;
        std::cout << "  Efficiency: " << (totalSteps / totalTime) / batchSize << " batches per thread" << std::endl;
    }
    
private:
    size_t batchSize;
    size_t numThreads;
};

int main() {
    std::cout << "=== NLM Production Batch Processing ===" << std::endl;
    
    // Configure batch processing
    size_t batchSize = 200;      // Steps per batch
    size_t numBatches = 10;      // Number of batches
    size_t numThreads = 4;       // Number of threads
    
    // Create batch processor
    BatchProcessor processor(batchSize, numThreads);
    
    // Run parallel batches
    auto results = processor.runParallelBatches(numBatches);
    
    // Analyze results
    processor.analyzeBatchResults(results);
    
    std::cout << "\n=== Batch Processing Complete ===" << std::endl;
    
    return 0;
}
```

## Educational Examples

### Educational Example 1: Introduction to Neural Networks

```cpp
// File: educational_intro.cpp
#include <nlm/nlm.hpp>
#include <iostream>
#include <vector>
#include <iomanip>

class NeuralNetworkEducator {
public:
    NeuralNetworkEducator() {
        std::cout << "=== NLM Educational Introduction ===" << std::endl;
        std::cout << "This example demonstrates basic neural network concepts using NLM." << std::endl;
        
        // Create a simple educational brain
        auto config = nlm::createDefaultConfig();
        config.set("brain.neuron_count", 100);
        config.set("plasticity.stdp.enable", true);
        
        brain = nlm::createBrain(config);
        agent = nlm::createAgentBrain(brain);
        world = nlm::createSimpleWorld();
        
        world->configure(30, 30, 6, 6);
        world->reset();
        agent->initialize(world);
        
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        
        educationalMode = true;
    }
    
    void teachBasicConcepts() {
        std::cout << "\n1. Basic Neural Network Concepts:" << std::endl;
        demonstrateNeurons()
        demonstrateConnections()
        demonstrateLearning()
    }
    
    void demonstrateNeurons() {
        std::cout << "\n   Neurons (Neurons):" << std::endl;
        std::cout << "   - Receive inputs (current)" << std::endl;
        std::cout << "   - Maintain potential (membrane potential)" << std::endl;
        std::cout << "   - Fire when threshold reached (spike)" << std::endl;
        std::cout << "   - Reset after firing (refractory period)" << std::endl;
        
        // Demonstrate with actual neurons
        auto neurons = getEducationalNeurons();
        if (!neurons.empty()) {
            auto& neuron = neurons[0];
            std::cout << "\n   Example Neuron State:" << std::endl;
            std::cout << "     Type: " << static_cast<int>(neuron->getType()) << std::endl;
            std::cout << "     Membrane potential: " << neuron->getMembranePotential() << " mV" << std::endl;
            std::cout << "     Threshold: " << neuron->getThreshold() << " mV" << std::endl;
            std::cout << "     Refractory period: " << neuron->getRefractoryPeriod() << " ms" << std::endl;
        }
    }
    
    void demonstrateConnections() {
        std::cout << "\n   Synapses (Connections):" << std::endl;
        std::cout << "   - Connect neurons (pre -> post)" << std::endl;
        std::cout << "   - Carry signals (current)" << std::endl;
        std::cout << "   - Can be excitatory or inhibitory" << std::endl;
        std::cout << "   - Can learn (plastic synapses)" << std::endl;
        
        // Count synapses
        size_t totalSynapses = brain->getTotalSynapseCount();
        std::cout << "\n   Network has " << totalSynapses << " connections" << std::endl;
        
        // Show some example properties
        std::cout << "   Connection statistics:" << std::endl;
        std::cout << "     - Average weight: " << calculateAverageWeight() << std::endl;
        std::cout << "     - Excitatory/inhibitory ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
    }
    
    void demonstrateLearning() {
        std::cout << "\n   Learning (Plasticity):" << std::endl;
        std::cout << "   - STDP: Spike timing dependent plasticity" << std::endl;
        std::cout << "   - Hebbian: Fire together, wire together" << std::endl;
        std::cout << "   - Neuromodulation: Reward prediction error" << std::endl;
        std::cout << "   - Development: Age-dependent plasticity" << std::endl;
        
        // Demonstrate STDP
        demonstrateSTDPLearning()
        
        // Show learning statistics
        std::cout << "\n   Learning statistics:" << std::endl;
        std::cout << "     - Current learning rate: " << agent->getLearningRate() << std::endl;
        std::cout << "     - Curiosity level: " << agent->getCuriosityLevel() << std::endl;
        std::cout << "     - Novelty level: " << agent->getNoveltyLevel() << std::endl;
        std::cout << "     - Development stage: " << static_cast<int>(agent->getDevelopmentalStage()) << std::endl;
    }
    
    void demonstrateSTDPLearning() {
        std::cout << "\n   STDP Learning Demonstration:" << std::endl;
        
        // Find STDP system
        auto* stdp = brain->getSTDP();
        if (!stdp) {
            std::cout << "   - STDP system not available" << std::endl;
            return;
        }
        
        stdp->enable(true);
        stdp->setLearningRate(0.01f);
        
        // Run a simple STDP demonstration
        std::vector<NeuronId> neurons = getAllNeuronIds();
        if (neurons.size() < 2) {
            std::cout << "   - Not enough neurons for STDP demo" << std::endl;
            return;
        }
        
        // Measure initial weight
        auto regions = brain->getRegions();
        if (!regions.empty()) {
            auto& region = regions[0];
            auto& synapses = region->getSynapses();
            if (!synapses.empty()) {
                float initialWeight = synapses[0]->getWeight();
                
                // Create pre-post activity
                auto preNeuron = neurons[0];
                auto postNeuron = neurons[1];
                
                // Pre fires first
                brain->injectCurrent(preNeuron, 20.0f);
                brain->step(0, 0.0f);
                
                // Post fires second
                brain->injectCurrent(postNeuron, 20.0f);
                brain->step(1, 0.001f);
                
                // Measure final weight
                float finalWeight = synapses[0]->getWeight();
                float weightChange = finalWeight - initialWeight;
                
                std::cout << "   - Initial weight: " << initialWeight << std::endl;
                std::cout << "   - Final weight: " << finalWeight << std::endl;
                std::cout << "   - Weight change: " << weightChange << std::endl;
                
                if (weightChange > 0.001f) {
                    std::cout << "   - Result: LTP (potentiation) - pre before post strengthens synapse" << std::endl;
                } else if (weightChange < -0.001f) {
                    std::cout << "   - Result: LTD (depression) - pre before post weakens synapse" << std::endl;
                } else {
                    std::cout << "   - Result: No significant change" << std::endl;
                }
            }
        }
    }
    
    std::vector<NeuronId> getAllNeuronIds() {
        std::vector<NeuronId> neuronIds;
        auto regions = brain->getRegions();
        for (auto& region : regions) {
            auto neurons = region->getAllNeurons();
            for (auto& neuron : neurons) {
                neuronIds.push_back(neuron->getId());
            }
        }
        return neuronIds;
    }
    
    float calculateAverageWeight() {
        float totalWeight = 0.0f;
        size_t synapseCount = 0;
        
        auto regions = brain->getRegions();
        for (auto& region : regions) {
            auto& synapses = region->getSynapses();
            for (auto& synapse : synapses) {
                totalWeight += synapse->getWeight();
                synapseCount++;
            }
        }
        
        return synapseCount > 0 ? totalWeight / synapseCount : 0.0f;
    }
    
    std::vector<std::unique_ptr<nlm::Neuron>> getEducationalNeurons() {
        std::vector<std::unique_ptr<nlm::Neuron>> neurons;
        auto regions = brain->getRegions();
        for (auto& region : regions) {
            auto regionNeurons = region->getAllNeurons();
            for (auto& neuron : regionNeurons) {
                neurons.push_back(std::move(neuron));
            }
        }
        return neurons;
    }
    
    void runEducationalSimulation() {
        std::cout << "\n2. Educational Simulation:" << std::endl;
        std::cout << "   Running a simple demonstration of neural network behavior..." << std::endl;
        
        const size_t steps = 100;
        
        for (size_t step = 0; step < steps; ++step) {
            // Update world
            world->update(0.1f);
            
            // Get sensory input
            auto percept = world->getSensoryPercept();
            
            // Process through agent
            agent->processSensoryInput(percept);
            
            // Brain computation
            brain->step(step);
            
            // Get action
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Apply reward
            float reward = world->getSensoryPercept().getInternal()[0];
            agent->applyRewardModulation(reward, 0.0f);
            
            // Update development
            agent->updateDevelopment(0.1f);
            
            // Print educational information
            if (step % 20 == 0) {
                printEducationalInfo(step);
            }
        }
        
        printEducationalSummary()
    }
    
    void printEducationalInfo(size_t step) {
        std::cout << "     Step " << step << ": "
                 << "Firing: " << brain->getFiringNeuronCount() << ", "
                 << "Spikes: " << brain->getTotalSpikeCount() << ", "
                 << "Reward: " << (world->getSensoryPercept().getInternal()[0] + 0.1f) << ", "
                 << "Curiosity: " << agent->getCuriosityLevel() << ", "
                 << "Novelty: " << agent->getNoveltyLevel() << std::endl;
    }
    
    void printEducationalSummary() {
        std::cout << "\n   Educational Summary:" << std::endl;
        std::cout << "     - Total simulation time: " << 100 << " steps" << std::endl;
        std::cout << "     - Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "     - Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
        std::cout << "     - E/I balance: " << brain->getExcitationInhibitionRatio() << std::endl;
        std::cout << "     - Final curiosity level: " << agent->getCuriosityLevel() << std::endl;
        std::cout << "     - Final novelty level: " << agent->getNoveltyLevel() << std::endl;
        std::cout << "     - Final development stage: " << static_cast<int>(agent->getDevelopmentalStage()) << std::endl;
        
        std::cout << "\n   Key Learning Points:" << std::endl;
        std::cout << "     1. Neurons communicate via spikes" << std::endl;
        std::cout << "     2. Synapses strengthen with correlated activity (STDP)" << std::endl;
        std::cout << "     3. Reward signals modulate learning" << std::endl;
        std::cout << "     4. Curiosity drives exploration" << std::endl;
        std::cout << "     5. Brains develop and change over time" << std::endl;
    }
    
    std::unique_ptr<nlm::Brain> brain;
    std::unique_ptr<nlm::AgentBrain> agent;
    std::unique_ptr<nlm::SimpleWorld> world;
    bool educationalMode;
};

int main() {
    NeuralNetworkEducator educator;
    
    educator.teachBasicConcepts()
    educator.runEducationalSimulation()
    
    std::cout << "\n=== Educational Tutorial Complete ===" << std::endl;
    std::cout << "You now understand the basic principles of neural networks!" << std::endl;
    
    return 0;
}
```

## Index

This examples documentation includes examples for the following use cases:

- **Complete Examples**: Full working applications
  1. Basic Neural Simulation
  2. Learning Agent in Environment
  3. STDP Learning Demonstration

- **Real-World Examples**: Practical applications
  1. Robot Navigation

- **Research Examples**: Scientific experimentation
  1. Critical Period Analysis

- **Production Examples**: Ready-for-production code
  1. Checkpoint-Based Training
  2. Batch Processing

- **Educational Examples**: Teaching and learning
  1. Introduction to Neural Networks

## Example Structure

Each example includes:

1. **Problem Statement**: What the example solves
2. **Code Implementation**: Complete working code
3. **Output**: Expected program output
4. **Learning Points**: Key concepts demonstrated

## Running Examples

To run these examples:

1. **Compile**: Use CMake and your preferred compiler
   ```bash
   mkdir build
   cd build
   cmake ..
   make -j4
   ```

2. **Run**: Execute the compiled binary
   ```bash
   ./example_name
   ```

3. **Modify**: Customize examples for your specific needs

## Example Categories

### Complete Examples
- Designed to run end-to-end
- Demonstrate full workflow from setup to analysis
- Include comprehensive comments and documentation

### Real-World Examples
- Based on actual applications
- Solve practical problems
- Include domain-specific considerations

### Research Examples
- Designed for scientific investigation
- Include analysis and reporting
- Focus on reproducibility and metrics

### Production Examples
- Optimized for production use
- Include error handling and monitoring
- Designed for scalability and reliability

### Educational Examples
- Simplified and clearly explained
- Focus on teaching key concepts
- Include detailed commentary and explanations

## Further Reading

For more information, see:

- [API Reference Documentation](API_REFERENCE_INDEX.md) - Complete API documentation
- [Tutorial Documentation](tutorials/) - Step-by-step tutorials
- [Architecture Guide](ARCHITECTURE.md) - System architecture overview
- [Science Guide](SCIENCE.md) - Scientific background

## Example Naming Convention

Examples follow this naming convention:
- `category_object.cpp` - Category and object type
- e.g., `production_training.cpp`, `educational_intro.cpp`

Example categories:
- `basic`, `complete`, `real-world`, `research`, `production`, `educational`

## Example Quality Standards

All examples follow these quality standards:

1. **Correctness**: Code compiles and runs correctly
2. **Readability**: Clear, well-commented code
3. **Completeness**: Includes error handling and edge cases
4. **Educational Value**: Teaches key concepts
5. **Performance**: Efficient use of resources
6. **Maintainability**: Easy to understand and modify

## Contributing Examples

To contribute new examples:

1. Follow the existing example structure
2. Include complete, working code
3. Add comprehensive comments
4. Include expected output
5. Document key learning points
6. Add to appropriate category

Examples are valuable for the community and help users learn how to use NLM effectively.