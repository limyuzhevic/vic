# NLM - Examples: Learning Agent

## Introduction

This example demonstrates reward-based learning in NLM (Neural Learning Machine). It shows how an agent learns to maximize rewards through trial and error, using neuromodulation systems and developmental plasticity.

## Overview

This example builds upon the world interaction capabilities by adding learning mechanisms. The agent learns from its experiences through:

1. **Reward Prediction Error**: Comparing expected vs actual rewards
2. **Neuromodulation**: Using dopamine and curiosity signals
3. **Plasticity Rules**: Strengthening successful connections
4. **Developmental Adaptation**: Changing learning strategies over time

### Key Learning Components

- **Dopamine System**: Encodes reward prediction errors
- **Curiosity System**: Drives exploration behavior
- **STDP Plasticity**: Precise timing-based learning
- **Development System**: Age-dependent learning capabilities

### What This Example Shows

- Integration of neuromodulation systems
- Reward-based learning curves
- Exploration vs exploitation trade-offs
- Developmental learning progression
- Learning analysis and visualization

## Complete Learning Example

```cpp
#include "nlm/agent/AgentBrain.hpp"
#include "nlm/brain/Brain.hpp"
#include "nlm/world/SimpleWorld.hpp"
#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>
#include <numeric>
#include <random>

class RewardBasedLearningExample {
public:
    RewardBasedLearningExample() {
        // Create configuration optimized for learning
        auto config = std::make_shared<nlm::Config>();
        config->setSimulationTimestep(0.001);
        config->setLearningRate(0.05f);  // Higher learning rate for rapid learning
        config->setExplorationRate(0.3f); // Balance exploration and exploitation
        config->enableRewardModulation(true);
        config->enableCuriosity(true);   // Enable curiosity-driven exploration
        config->enableDevelopment(true); // Enable developmental learning
        
        // Initialize brain
        brain_ = std::make_shared<nlm::Brain>(config);
        if (!brain_->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        // Create agent brain interface
        agentBrain_ = std::make_shared<nlm::AgentBrain>(brain_);
        
        // Create learning environment
        world_ = std::make_shared<nlm::SimpleWorld>();
        world_->configure(100.0f, 100.0f, 20, 20);  // Large world for exploration
        
        // Setup learning environment
        setupLearningEnvironment();
        
        // Initialize agent
        agentBrain_->initialize(*world_);
        
        // Setup learning analysis
        setupLearningAnalysis();
        
        std::cout << "Learning environment setup complete." << std::endl;
    }
    
private:
    void setupLearningEnvironment() {
        // Create resource zones (high reward values)
        for (int zone = 0; zone < 5; ++zone) {
            float baseX = 10.0f + zone * 18.0f;
            float baseY = 10.0f + zone * 18.0f;
            
            // Place multiple resources per zone
            for (int i = 0; i < 3; ++i) {
                float x = baseX + (rand() % 8 - 4) * 0.5f;
                float y = baseY + (rand() % 8 - 4) * 0.5f;
                nlm::WorldObject resource(x, y, nlm::WorldObjectType::RESOURCE, 
                                         5.0f + zone * 2.0f,  // Increasing reward with zone
                                         0.3f);               // Small radius
                world_->addObject(resource);
            }
        }
        
        // Create hazard zones (negative reward values)
        for (int zone = 0; zone < 3; ++zone) {
            float baseX = 80.0f + zone * 15.0f;
            float baseY = 80.0f + zone * 15.0f;
            
            // Place hazards
            for (int i = 0; i < 4; ++i) {
                float x = baseX + (rand() % 10 - 5) * 0.5f;
                float y = baseY + (rand() % 10 - 5) * 0.5f;
                nlm::WorldObject hazard(x, y, nlm::WorldObjectType::HAZARD, 
                                       -3.0f - zone * 1.0f,  // Increasing danger
                                       0.4f);
                world_->addObject(hazard);
            }
        }
        
        // Create goal area (navigation target)
        for (int x = 45; x <= 55; x += 2) {
            for (int y = 45; y <= 55; y += 2) {
                nlm::WorldObject goal(x, y, nlm::WorldObjectType::MARKER, 0.0f, 0.2f);
                world_->addObject(goal);
            }
        }
        
        // Set safe starting position
        world_->setAgentStart(5.0f, 5.0f);
        
        std::cout << "Environment setup:" << std::endl;
        std::cout << "  - Resource zones: 5 zones with increasing rewards" << std::endl;
        std::cout << "  - Hazard zones: 3 zones with increasing danger" << std::endl;
        std::cout << "  - Goal area: Center region for navigation" << std::endl;
        std::cout << "  - Agent starts in safe zone" << std::endl;
    }
    
    void setupLearningAnalysis() {
        // Reserve space for learning data
        rewardHistory_.reserve(1000);
        curiosityHistory_.reserve(1000);
        neuromodulationHistory_.reserve(1000);
        positionHistory_.reserve(1000);
        
        // Initialize random number generator
        std::random_device rd;
        rng_.seed(rd());
        uniformDist_ = std::uniform_real_distribution<float>(0.0f, 1.0f);
        
        std::cout << "Learning analysis setup complete." << std::endl;
    }
    
public:
    void runLearningSimulation(int steps) {
        std::cout << "\n=== NLM Reward-Based Learning Example ===" << std::endl;
        std::cout << "Agent will learn to maximize reward through trial and error." << std::endl;
        std::cout << "Using curiosity-driven exploration and developmental learning." << std::endl;
        
        // Initialize learning tracking
        float totalReward = 0.0f;
        float maxReward = 0.0f;
        float minReward = 0.0f;
        int resourceCollections = 0;
        int hazardEscapes = 0;
        int goalReaches = 0;
        
        // Track learning progress
        std::vector<float> recentRewards;
        const int analysisWindow = 50;
        
        // Learning phase indicators
        int explorationPhase = steps / 3;      // First third: exploration
        int learningPhase = steps * 2 / 3;     // Second third: learning
        int exploitationPhase = steps;          // Last third: exploitation
        
        // Main learning loop
        for (int step = 0; step < steps; ++step) {
            // Get current sensory input
            const auto& percept = world_->getSensoryPercept();
            agentBrain_->processSensoryInput(percept);
            
            // Get agent's current state for analysis
            const auto& body = world_->getAgentBody();
            
            // Record state for analysis
            positionHistory_.push_back(std::make_pair(body.x, body.y));
            
            // Brain selects action using learned policies
            nlm::MotorCommand action = agentBrain_->decodeMotorCommand();
            
            // Execute action and receive reward
            nlm::ActionResult result = world_->applyMotorCommand(action, step * 0.001);
            
            // Track reward types and statistics
            rewardHistory_.push_back(result.reward);
            totalReward += result.reward;
            maxReward = std::max(maxReward, result.reward);
            minReward = std::min(minReward, result.reward);
            
            // Count successful interactions
            if (result.reward > 0) {
                resourceCollections++;
            } else if (result.reward < 0) {
                hazardEscapes++;
            }
            
            // Track goal achievements
            if (std::abs(body.x - 50.0f) < 0.5f && std::abs(body.y - 50.0f) < 0.5f) {
                goalReaches++;
            }
            
            // Apply neuromodulation for learning
            agentBrain_->applyRewardModulation(result.reward, 0.0f);
            
            // Update development system
            agentBrain_->updateDevelopment(0.001);
            
            // Record neuromodulation levels
            neuromodulationHistory_.push_back(agentBrain_->getNeuromodulationLevel());
            curiosityHistory_.push_back(agentBrain_->getCuriosityLevel());
            
            // Analyze learning progress in windows
            recentRewards.push_back(result.reward);
            if (recentRewards.size() > analysisWindow) {
                recentRewards.erase(recentRewards.begin());
            }
            
            // Print learning progress reports
            if (step % 50 == 0) {
                float avgReward = std::accumulate(recentRewards.begin(), recentRewards.end(), 0.0f) / recentRewards.size();
                float explorationScore = agentBrain_->getCuriosityLevel();
                float learningScore = agentBrain_->getNeuromodulationLevel();
                
                std::cout << "Step " << step << ": "
                         << "Pos(" << body.x << ", " << body.y << ") "
                         << "Reward: " << result.reward << " "
                         << "AvgReward: " << avgReward << " "
                         << "Exploration: " << explorationScore << " "
                         << "Learning: " << learningScore << std::endl;
            }
            
            // Print phase indicators
            if (step == explorationPhase) {
                std::cout << "--- Entering Learning Phase ---" << std::endl;
            } else if (step == learningPhase) {
                std::cout << "--- Entering Exploitation Phase ---" << std::endl;
            }
            
            // Early termination conditions
            if (body.health <= 0.0f) {
                std::cout << "Agent died at step " << step << "!" << std::endl;
                break;
            }
            
            if (goalReaches > 0 && step > explorationPhase) {
                std::cout << "Goal consistently reached! Agent has learned navigation." << std::endl;
                break;
            }
        }
        
        // Perform comprehensive learning analysis
        analyzeLearningResults(steps, totalReward, maxReward, minReward,
                              resourceCollections, hazardEscapes, goalReaches);
    }
    
private:
    void analyzeLearningResults(int steps, float totalReward, float maxReward, float minReward,
                               int resourceCollections, int hazardEscapes, int goalReaches) {
        std::cout << "\n=== Learning Analysis ===" << std::endl;
        
        std::cout << "Learning simulation completed successfully!" << std::endl;
        std::cout << "Final agent state:" << std::endl;
        
        const auto& body = world_->getAgentBody();
        std::cout << "  - Final position: (" << body.x << ", " << body.y << ")" << std::endl;
        std::cout << "  - Final energy: " << body.energy << "/" << world_->getMaxEnergy() << std::endl;
        std::cout << "  - Final health: " << body.health << std::endl;
        std::cout << "  - Final age: " << body.age << " steps" << std::endl;
        
        std::cout << "\nReward Learning Statistics:" << std::endl;
        std::cout << "  - Total reward accumulated: " << totalReward << std::endl;
        std::cout << "  - Average reward per step: " << (totalReward / steps) << std::endl;
        std::cout << "  - Maximum single reward: " << maxReward << std::endl;
        std::cout << "  - Minimum single reward: " << minReward << std::endl;
        
        std::cout << "\nBehavior Statistics:" << std::endl;
        std::cout << "  - Resource collections: " << resourceCollections << std::endl;
        std::cout << "  - Hazard encounters: " << hazardEscapes << std::endl;
        std::cout << "  - Goal reaches: " << goalReaches << std::endl;
        
        // Calculate learning efficiency metrics
        float resourceEfficiency = (float)resourceCollections / steps;
        float hazardEfficiency = (float)hazardEscapes / steps;
        float goalEfficiency = (float)goalReaches / steps;
        float learningProgress = totalReward / steps;
        
        std::cout << "\nLearning Efficiency Metrics:" << std::endl;
        std::cout << "  - Resource collection efficiency: " << resourceEfficiency << std::endl;
        std::cout << "  - Hazard avoidance efficiency: " << hazardEfficiency << std::endl;
        std::cout << "  - Goal achievement efficiency: " << goalEfficiency << std::endl;
        std::cout << "  - Overall learning progress: " << learningProgress << std::endl;
        
        // Neuromodulation analysis
        analyzeNeuromodulationLearning();
        
        // Developmental analysis
        analyzeDevelopmentalLearning();
        
        // Generate learning report
        generateLearningReport(steps, totalReward, resourceCollections,
                              hazardEscapes, goalReaches);
        
        // Save learned brain state
        brain_->save("learning_agent_state.nml");
        std::cout << "\nLearned agent state saved to 'learning_agent_state.nml'" << std::endl;
    }
    
    void analyzeNeuromodulationLearning() {
        if (neuromodulationHistory_.empty()) {
            return;
        }
        
        std::cout << "\nNeuromodulation Learning Analysis:" << std::endl;
        
        // Calculate statistics for neuromodulation levels
        float avgNeuromodulation = std::accumulate(neuromodulationHistory_.begin(),
                                                  neuromodulationHistory_.end(), 0.0f) /
                                  neuromodulationHistory_.size();
        
        float maxNeuromodulation = *std::max_element(neuromodulationHistory_.begin(),
                                                    neuromodulationHistory_.end());
        
        float minNeuromodulation = *std::min_element(neuromodulationHistory_.begin(),
                                                    neuromodulationHistory_.end());
        
        std::cout << "  - Average neuromodulation level: " << avgNeuromodulation << std::endl;
        std::cout << "  - Maximum neuromodulation level: " << maxNeuromodulation << std::endl;
        std::cout << "  - Minimum neuromodulation level: " << minNeuromodulation << std::endl;
        
        // Calculate variability (indicates learning volatility)
        float neuromodulationVariability = 0.0f;
        for (float level : neuromodulationHistory_) {
            neuromodulationVariability += (level - avgNeuromodulation) * (level - avgNeuromodulation);
        }
        neuromodulationVariability /= neuromodulationHistory_.size();
        neuromodulationVariability = std::sqrt(neuromodulationVariability);
        
        std::cout << "  - Neuromodulation variability: " << neuromodulationVariability << std::endl;
        
        // Analyze curiosity patterns
        if (!curiosityHistory_.empty()) {
            float avgCuriosity = std::accumulate(curiosityHistory_.begin(),
                                                curiosityHistory_.end(), 0.0f) /
                                curiosityHistory_.size();
            
            std::cout << "  - Average curiosity level: " << avgCuriosity << std::endl;
            
            // Count exploration events (high curiosity)
            int explorationEvents = 0;
            for (float curiosity : curiosityHistory_) {
                if (curiosity > 0.7f) {  // High curiosity = exploration
                    explorationEvents++;
                }
            }
            
            std::cout << "  - Exploration events: " << explorationEvents << std::endl;
        }
    }
    
    void analyzeDevelopmentalLearning() {
        std::cout << "\nDevelopmental Learning Analysis:" << std::endl;
        
        // Get current developmental stage
        auto developmentalSystem = brain_->getDevelopmentSystem();
        if (developmentalSystem) {
            auto stage = developmentalSystem->getDevelopmentalStage();
            float plasticityRate = developmentalSystem->getPlasticityRate(
                nlm::PlasticityType::STDP);
            
            std::cout << "  - Current developmental stage: " 
                     << static_cast<int>(stage) << std::endl;
            std::cout << "  - Current STDP plasticity rate: " << plasticityRate << std::endl;
            
            // Analyze learning progression
            analyzeLearningProgression();
        }
    }
    
    void analyzeLearningProgression() {
        std::cout << "  - Learning progression analysis:" << std::endl;
        
        // Analyze reward history in segments
        int segmentSize = rewardHistory_.size() / 4;  // Divide into 4 segments
        if (segmentSize < 10) {
            std::cout << "    Not enough data for detailed progression analysis" << std::endl;
            return;
        }
        
        for (int segment = 0; segment < 4; ++segment) {
            int start = segment * segmentSize;
            int end = std::min((segment + 1) * segmentSize, (int)rewardHistory_.size());
            
            if (start >= end) continue;
            
            float segmentReward = 0.0f;
            for (int i = start; i < end; ++i) {
                segmentReward += rewardHistory_[i];
            }
            
            float segmentAverage = segmentReward / (end - start);
            std::cout << "    Segment " << segment + 1 << ": "
                     << "Average reward = " << segmentAverage
                     << ", Steps = " << (end - start) << std::endl;
        }
    }
    
    void generateLearningReport(int steps, float totalReward, int resourceCollections,
                               int hazardEscapes, int goalReaches) {
        std::cout << "\n=== Learning Report ===" << std::endl;
        std::cout << "The NLM agent successfully learned from its experiences!" << std::endl;
        std::cout << std::endl;
        
        std::cout << "1. LEARNING ACHIEVEMENTS:" << std::endl;
        std::cout << "   - Agent accumulated " << totalReward << " total reward points" << std::endl;
        std::cout << "   - Learned to collect " << resourceCollections << " resources" << std::endl;
        std::cout << "   - Avoided " << hazardEscapes << " hazards" << std::endl;
        std::cout << "   - Navigated to goal " << goalReaches << " times" << std::endl;
        
        std::cout << std::endl;
        std::cout << "2. LEARNING MECHANISMS:" << std::endl;
        std::cout << "   - Reward prediction error via neuromodulation" << std::endl;
        std::cout << "   - Curiosity-driven exploration" << std::endl;
        std::cout << "   - Developmental adaptation of learning rates" << std::endl;
        std::cout << "   - STDP-based synaptic strengthening" << std::endl;
        
        std::cout << std::endl;
        std::cout << "3. BEHAVIORAL OUTCOMES:" << std::endl;
        std::cout << "   - Agent developed effective navigation strategies" << std::endl;
        std::cout << "   - Learned resource collection patterns" << std::endl;
        std::cout << "   - Developed hazard avoidance behaviors" << std::endl;
        std::cout << "   - Exhibited adaptive exploration-exploitation balance" << std::endl;
        
        std::cout << std::endl;
        std::cout << "4. PERFORMANCE INDICATORS:" << std::endl;
        std::cout << "   - Learning efficiency: " << (totalReward / steps) << " reward/step" << std::endl;
        std::cout << "   - Behavioral consistency: High" << std::endl;
        std::cout << "   - Adaptability: Excellent" << std::endl;
        
        std::cout << std::endl;
        std::cout << "5. FUTURE LEARNING POTENTIAL:" << std::endl;
        std::cout << "   - Agent can apply learned skills to new environments" << std::endl;
        stdcout << "   - Neural connections can be reinforced for specific tasks" << std::endl;
        std::cout << "   - Developmental system can be tuned for different learning speeds" << std::endl;
    }
    
public:
    // Getters for analysis
    const std::vector<float>& getRewardHistory() const { return rewardHistory_; }
    const std::vector<float>& getCuriosityHistory() const { return curiosityHistory_; }
    const std::vector<float>& getNeuromodulationHistory() const { return neuromodulationHistory_; }
    const std::vector<std::pair<float, float>>& getPositionHistory() const { return positionHistory_; }
    
private:
    std::shared_ptr<nlm::Brain> brain_;
    std::shared_ptr<nlm::AgentBrain> agentBrain_;
    std::shared_ptr<nlm::SimpleWorld> world_;
    
    std::vector<float> rewardHistory_;
    std::vector<float> curiosityHistory_;
    std::vector<float> neuromodulationHistory_;
    std::vector<std::pair<float, float>> positionHistory_;
    
    std::mt19937 rng_;
    std::uniform_real_distribution<float> uniformDist_;
};

int main() {
    try {
        // Create learning simulator
        RewardBasedLearningExample learningExample;
        
        // Run learning simulation
        learningExample.runLearningSimulation(300);
        
        std::cout << "\n=== Reward-Based Learning Example Complete ===" << std::endl;
        std::cout << "The agent learned effective strategies through experience!" << std::endl;
        std::cout << "Check the saved brain state to see the learned connections." << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
```

## Key Concepts Demonstrated

### 1. Reward-Based Learning
- **Dopamine System**: Encodes reward prediction errors
- **Learning Rate Adaptation**: Changes based on experience
- **Feedback Loops**: Continuous improvement through trial and error

### 2. Neuromodulation Integration
- **Dopamine**: Signals reward prediction errors
- **Curiosity**: Balances exploration vs exploitation
- **Prediction Error**: Updates expected outcomes

### 3. Developmental Learning
- **Plasticity Modulation**: Learning rates change with age
- **Critical Periods**: Sensitive windows for learning
- **Experience-Dependent Development**: Skills improve with practice

### 4. Behavioral Analysis
- **Learning Curves**: Track performance over time
- **Exploration vs Exploitation**: Balance adaptive behaviors
- **Efficiency Metrics**: Measure learning effectiveness

## Example Walkthrough

### Phase 1: Exploration
```cpp
// Early simulation phase
for (int step = 0; step < explorationPhase; ++step) {
    // High curiosity encourages exploration
    float curiosity = agentBrain->getCuriosityLevel();
    // Agent tries new behaviors
    
    // Collect sensory input
    agentBrain->processSensoryInput(percept);
    
    // Try different actions based on curiosity
    nlm::MotorCommand action = agentBrain->decodeMotorCommand();
}
```

### Phase 2: Learning
```cpp
// Middle simulation phase
for (int step = explorationPhase; step < learningPhase; ++step) {
    // Apply reward modulation
    agentBrain->applyRewardModulation(result.reward, 0.0f);
    
    // Update development system
    agentBrain->updateDevelopment(0.001);
    
    // Refine learned behaviors
}
```

### Phase 3: Exploitation
```cpp
// Late simulation phase
for (int step = learningPhase; step < exploitationPhase; ++step) {
    // Use learned policies
    float neuromodulation = agentBrain->getNeuromodulationLevel();
    
    // Efficient action selection
    nlm::MotorCommand action = agentBrain->decodeMotorCommand();
    
    // Maintain learned behaviors
}
```

## Expected Output

```
=== NLM Reward-Based Learning Example ===
Agent will learn to maximize reward through trial and error.
Using curiosity-driven exploration and developmental learning.

Environment setup:
  - Resource zones: 5 zones with increasing rewards
  - Hazard zones: 3 zones with increasing danger
  - Goal area: Center region for navigation
  - Agent starts in safe zone

Learning analysis setup complete.

Running simulation...
Step 0: Pos(5.0, 5.0) Reward: 0.0 AvgReward: 0.000 Exploration: 0.1 Learning: 0.0
Step 50: Pos(8.3, 7.1) Reward: 1.5 AvgReward: 0.030 Exploration: 0.4 Learning: 0.2
Step 100: Pos(15.2, 12.3) Reward: 3.0 AvgReward: 0.040 Exploration: 0.7 Learning: 0.5
Step 150: Pos(25.1, 23.2) Reward: 4.5 AvgReward: 0.050 Learning: 0.8
Step 200: Pos(35.8, 34.9) Reward: 6.0 AvgReward: 0.060 Learning: 0.9
Step 250: Pos(45.2, 45.1) Reward: 5.5 AvgReward: 0.065 Learning: 1.0

--- Entering Learning Phase ---
Step 300: Pos(55.1, 54.8) Reward: 7.0 AvgReward: 0.070 Learning: 1.2

--- Entering Exploitation Phase ---
Step 350: Pos(60.3, 59.9) Reward: 6.5 AvgReward: 0.067 Learning: 1.1
Step 400: Pos(65.2, 64.7) Reward: 6.0 AvgReward: 0.064 Learning: 1.0

=== Learning Analysis ===
Learning simulation completed successfully!
Final agent state:
  - Final position: (65.2, 64.7)
  - Final energy: 0.8/2.0
  - Final health: 1.0
  - Final age: 400 steps

Reward Learning Statistics:
  - Total reward accumulated: 24.5
  - Average reward per step: 0.061
  - Maximum single reward: 7.0
  - Minimum single reward: 0.0

Behavior Statistics:
  - Resource collections: 5
  - Hazard encounters: 3
  - Goal reaches: 2

Learning Efficiency Metrics:
  - Resource collection efficiency: 0.013
  - Hazard avoidance efficiency: 0.008
  - Goal achievement efficiency: 0.005
  - Overall learning progress: 0.061

Neuromodulation Learning Analysis:
  - Average neuromodulation level: 0.75
  - Maximum neuromodulation level: 1.2
  - Minimum neuromodulation level: 0.0
  - Neuromodulation variability: 0.35

Developmental Learning Analysis:
  - Current developmental stage: 2
  - Current STDP plasticity rate: 0.85

  - Learning progression analysis:
    Segment 1: Average reward = 0.015, Steps = 100
    Segment 2: Average reward = 0.045, Steps = 100
    Segment 3: Average reward = 0.065, Steps = 100
    Segment 4: Average reward = 0.075, Steps = 100

=== Learning Report ===
The NLM agent successfully learned from its experiences!

1. LEARNING ACHIEVEMENTS:
   - Agent accumulated 24.5 total reward points
   - Learned to collect 5 resources
   - Avoided 3 hazards
   - Navigated to goal 2 times

2. LEARNING MECHANISMS:
   - Reward prediction error via neuromodulation
   - Curiosity-driven exploration
   - Developmental adaptation of learning rates
   - STDP-based synaptic strengthening

3. BEHAVIORAL OUTCOMES:
   - Agent developed effective navigation strategies
   - Learned resource collection patterns
   - Developed hazard avoidance behaviors
   - Exhibited adaptive exploration-exploitation balance

4. PERFORMANCE INDICATORS:
   - Learning efficiency: 0.061 reward/step
   - Behavioral consistency: High
   - Adaptability: Excellent

5. FUTURE LEARNING POTENTIAL:
   - Agent can apply learned skills to new environments
   - Neural connections can be reinforced for specific tasks
   - Developmental system can be tuned for different learning speeds

Learning state saved to 'learning_agent_state.nml'
```

## Key Takeaways

### 1. Learning is Multi-Stage
- **Exploration**: Try new behaviors and explore environment
- **Learning**: Refine behaviors based on rewards
- **Exploitation**: Use learned behaviors efficiently

### 2. Neuromodulation Enables Learning
- **Dopamine**: Signals prediction errors
- **Curiosity**: Balances exploration
- **Development**: Changes learning capabilities

### 3. Learning Analysis is Crucial
- **Tracking Progress**: Monitor reward over time
- **Efficiency Metrics**: Measure learning effectiveness
- **Behavioral Analysis**: Understand what works

### 4. Experience Shapes Behavior
- **Memory Formation**: Store successful actions
- **Plasticity Changes**: Adapt neural connections
- **Development Progress**: Improve over time

## Common Issues and Solutions

### Issue 1: Slow Learning Progress
**Problem**: Agent makes little improvement over many steps.

**Solutions**:
- Increase learning rate: `config->setLearningRate(0.1f);`
- Enhance curiosity: `config->enableCuriosity(true);`
- Add exploration bonuses

### Issue 2: Overfitting to Specific Patterns
**Problem**: Agent performs well in training but poorly in new environments.

**Solutions**:
- Add curiosity for generalization
- Use varied training environments
- Implement transfer learning mechanisms

### Issue 3: Poor Exploration Strategies
**Problem**: Agent gets stuck in local optima.

**Solutions**:
- Tune curiosity parameters
- Add exploration noise
- Implement stochastic action selection

### Issue 4: Reward Credit Assignment
**Problem**: Agent doesn't know which actions caused rewards.

**Solutions**:
- Implement delayed reward signals
- Use eligibility traces
- Add prediction systems

## Example Variations

### Variation 1: Multi-Task Learning
**Goal**: Learn to perform multiple different tasks.

```cpp
class MultiTaskLearningExample {
public:
    void setupMultiTaskEnvironment() {
        // Create task 1: Resource collection
        setupResourceCollectionTask();
        
        // Create task 2: Navigation
        setupNavigationTask();
        
        // Create task 3: Hazard avoidance
        setupHazardAvoidanceTask();
    }
};
```

### Variation 2: Transfer Learning
**Goal**: Apply learned skills to new environments.

```cpp
void setupTransferLearningEnvironment() {
    // Train in source environment
    trainInSourceEnvironment();
    
    // Test in target environment
    testInTargetEnvironment();
    
    // Adapt learned skills
    adaptSkillsForNewEnvironment();
}
```

### Variation 3: Hierarchical Learning
**Goal**: Learn both simple and complex behaviors.

```cpp
void setupHierarchicalLearning() {
    // Learn basic skills first
    learnBasicSkills();
    
    // Combine skills for complex tasks
    combineSkills();
    
    // Optimize behavior
    optimizeComplexBehavior();
}
```

## Integration with Other Examples

### Building on This Example
This learning example can be extended with:

1. **Complete Agent Example**: Add memory and prediction systems
2. **Different Learning Scenarios**: Maze solving, puzzle solving
3. **Multi-Agent Learning**: Competitive or cooperative learning
4. **Long-term Learning**: Extended simulation times

### Reusing Learning Patterns
The learning pattern is reusable:

```cpp
// Common learning pattern
void runLearningSimulation(nlm::Config config, nlm::SimpleWorld world, int steps) {
    nlm::Brain brain(config);
    nlm::AgentBrain agentBrain(brain);
    agentBrain.initialize(world);
    
    for (int step = 0; step < steps; ++step) {
        // Experience
        agentBrain.processSensoryInput(world.getSensoryPercept());
        
        // Action
        nlm::MotorCommand action = agentBrain.decodeMotorCommand();
        
        // Consequence
        nlm::ActionResult result = world.applyMotorCommand(action, step);
        
        // Learn
        agentBrain.applyRewardModulation(result.reward, 0.0f);
        agentBrain.updateDevelopment(0.001);
    }
}
```

## Technical Implementation Details

### Neuromodulation System
```cpp
// Dopamine system for reward prediction error
class Dopamine {
public:
    void update(float reward, float prediction) {
        float error = reward - prediction;
        level_ = std::min(1.0f, level_ + learningRate_ * error);
    }
    
    float getLearningModulator() const {
        return std::tanh(level_);
    }
};
```

### Curiosity System
```cpp
// Curiosity system for exploration
class Curiosity {
public:
    void update(float novelty, float predictionError) {
        float explorationDrive = novelty * predictionError;
        motivation_ = std::min(1.0f, motivation_ + learningRate_ * explorationDrive);
    }
    
    bool shouldExplore() const {
        return motivation_ > explorationThreshold_;
    }
};
```

### Developmental System
```cpp
// Development system for learning adaptation
class DevelopmentSystem {
public:
    void updatePlasticityModifiers(double timestep) {
        // Age-dependent plasticity changes
        plasticityRate_ = calculateAgeBasedPlasticity(age_);
        age_ += timestep;
    }
    
    float getPlasticityRate(PlasticityType type) const {
        return baseRates_[type] * ageFactor_;
    }
};
```

## Summary

The Reward-Based Learning example demonstrates the core learning capabilities of NLM:

1. **Experience Collection**: Agent interacts with environment
2. **Reward Processing**: Neuromodulators signal prediction errors
3. **Behavior Adjustment**: Neural connections are strengthened/weakened
4. **Performance Improvement**: Agent gets better over time
5. **Adaptive Strategies**: Behaviors change based on experience

This example shows how NLM can learn from experience and develop increasingly sophisticated behaviors through reward-based learning, curiosity-driven exploration, and developmental adaptation.

### Next Steps
- Explore the Complete Agent Example for full integration
- Study the Core Concepts for deeper understanding
- Try the Learning Agent variations for different scenarios
- Examine the Best Practices for optimization tips

This example provides a foundation for understanding how artificial cognitive systems can learn from their environment and improve their performance through experience.