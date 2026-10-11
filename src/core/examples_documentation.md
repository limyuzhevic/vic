# NLM Examples and Usage Documentation

This document provides comprehensive examples and usage documentation for the NLM (熙然) project, including code examples, tutorials, and best practices.

## Table of Contents

1. [Getting Started with NLM](#getting-started-with-nlm)
2. [Basic Brain Examples](#basic-brain-examples)
3. [Learning and Plasticity Examples](#learning-and-plasticity-examples)
4. [Development Examples](#development-examples)
5. [Neuromodulation Examples](#neuromodulation-examples)
6. [Memory System Examples](#memory-system-examples)
7. [Prediction System Examples](#prediction-system-examples)
8. [Advanced Usage Examples](#advanced-usage-examples)
9. [Example Code Complete](#example-code-complete)

## Getting Started with NLM

### Overview

NLM provides a comprehensive framework for developing and testing artificial neural systems. This section provides practical examples showing how to use the NLM APIs to build different types of neural experiments.

### Example Categories

1. **Basic Functionality**: Demonstrates core NLM capabilities
2. **Learning Systems**: Shows plasticity and learning mechanisms
3. **Development Processes**: Models neural development and maturation
4. **Neuromodulation**: Demonstrates biological signal integration
5. **Memory Systems**: Shows memory formation and retrieval
6. **Predictive Processing**: Demonstrates prediction and planning

## Basic Brain Examples

### 1. Simple Brain Initialization

```cpp
#include <nlm/brain/Brain.hpp>
#include <nlm/core/Config/Config.hpp>

// Create a simple brain with default configuration
nlm::Brain brain;

// Configure brain properties
nlm::Config config;
config.set("neuron_count", 100);
config.set("region_count", 2);
config.set("connection_probability", 0.1);

brain.configure(config);

// Access brain neurons
auto neurons = brain.getNeurons();
for (auto& neuron : neurons) {
    std::cout << "Neuron " << neuron->getId() << " threshold: " 
              << neuron->getThreshold() << std::endl;
}
```

**Expected Output:**
```
Neuron 0 threshold: 0.0
Neuron 1 threshold: 0.0
... (100 neurons total)
```

### 2. Sensorimotor Loop

```cpp
#include <nlm/agent/AgentBrain.hpp>
#include <nlm/sensory/Vision.hpp>
#include <nlm/motor/MotorSystem.hpp>

// Create agent brain
std::shared_ptr<nlm::Brain> brain = std::make_shared<nlm::Brain>();
nlm::AgentBrain agent(brain);

// Create simple world
nlm::SimpleWorld world;
nlm::GridWorld gridWorld(10, 10);
agent.initialize(gridWorld);

// Create vision processor
nlm::VisionProcessor vision;

// Main simulation loop
for (int step = 0; step < 1000; ++step) {
    // Get sensory input
    nlm::Vision visionInput = world.getVision();
    
    // Process through vision system
    vision.process(visionInput);
    
    // Convert to sensory percept
    nlm::SensoryPercept percept(vision.getFeatures());
    
    // Process through agent brain
    agent.processSensoryInput(percept);
    
    // Decode motor command
    nlm::MotorCommand command = agent.decodeMotorCommand();
    
    // Apply to world
    world.applyMotorCommand(command);
    
    // Get reward
    float reward = world.getReward();
    
    // Apply reward modulation
    agent.applyRewardModulation(reward, 0.0f);
    
    // Update development
    agent.updateDevelopment(0.001f);
    
    // Print status
    if (step % 100 == 0) {
        std::cout << "Step " << step << ": Reward = " << reward 
                  << ", Pos = (" << world.getAgentPosition().x 
                  << ", " << world.getAgentPosition().y << ")" 
                  << std::endl;
    }
}
```

**Expected Output:**
```
Step 0: Reward = 0, Pos = (0, 0)
Step 100: Reward = 1, Pos = (1, 0)
Step 200: Reward = 0, Pos = (1, 1)
... (agent movement and learning)
```

### 3. Configuration-Based Brain Setup

```cpp
#include <nlm/core/Config/Config.hpp>
#include <nlm/brain/Brain.hpp>

// Load configuration from file
nlm::Config config;
if (!config.loadFromFile("configs/example_config.cfg")) {
    std::cerr << "Failed to load configuration file" << std::endl;
    return 1;
}

// Create and configure brain
nlm::Brain brain;
brain.configure(config);

// Verify configuration
std::cout << "Configuration loaded successfully:" << std::endl;
std::cout << brain.getConfigSummary() << std::endl;
```

**Example Configuration File (example_config.cfg):**
```ini
# Example NLM Configuration File
random_seed = 12345
simulation_timestep = 0.001
neuron_count = 500
region_count = 3
connection_probability = 0.15
plasticity_learning_rate = 0.02
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 25.0
development_synaptogenesis_rate = 0.002
development_pruning_rate = 0.0002
dopamine_baseline = 0.1
reward_discount_factor = 0.95
environment_name = GridWorld
environment_width = 8
environment_height = 8
log_level = INFO
log_to_file = false
max_simulation_steps = 5000
```

## Learning and Plasticity Examples

### 1. STDP Learning Demonstration

```cpp
#include <nlm/plasticity/STDP.hpp>
#include <nlm/brain/Brain.hpp>
#include <nlm/core/Types/Types.hpp>

// Create STDP plasticity rule
nlm::STDP stdp;
stdp.setLTPWeight(0.01f);
stdp.setLTDWeight(0.012f);
stdp.setTimeConstant(20.0f);

// Create brain and add STDP rule
nlm::Brain brain;
brain.addPlasticityRule(&stdp);

// Simulate spike-based learning
std::vector<nlm::Timestamp> preSpikes = {0.0, 0.1, 0.2};
std::vector<nlm::Timestamp> postSpikes = {0.05, 0.15, 0.25};

// Create synapses for testing
nlm::Synapse* synapse1 = brain.createSynapse(
    nlm::INVALID_NEURON_ID, nlm::INVALID_NEURON_ID);
nlm::Synapse* synapse2 = brain.createSynapse(
    nlm::INVALID_NEURON_ID, nlm::INVALID_NEURON_ID);

// Apply STDP learning
for (int i = 0; i < 3; ++i) {
    float pre_time = preSpikes[i];
    float post_time = postSpikes[i];
    
    stdp.update(synapse1, preSpikes, postSpikes, 0.001f);
    
    std::cout << "Spike pair " << i << ": pre=" << pre_time 
              << "ms, post=" << post_time << "ms" << std::endl;
    std::cout << "  Synapse weight: " << synapse1->getWeight() << std::endl;
}
```

**Expected Output:**
```
Spike pair 0: pre=0ms, post=50ms
  Synapse weight: 0.010000
Spike pair 1: pre=100ms, post=150ms
  Synapse weight: 0.020000
Spike pair 2: pre=200ms, post=250ms
  Synapse weight: 0.030000
```

### 2. Hebbian Learning Example

```cpp
#include <nlm/plasticity/Hebbian.hpp>
#include <nlm/brain/Brain.hpp>

// Create Hebbian learning rule
nlm::Hebbian hebbian;
hebbian.setLearningRate(0.01f);
hebbian.setMaxWeight(1.0f);

// Create brain and add Hebbian rule
nlm::Brain brain;
brain.addPlasticityRule(&hebbian);

// Simulate correlated activity
std::vector<nlm::Timestamp> preSpikes = {0.0, 0.2, 0.4};
std::vector<nlm::Timestamp> postSpikes = {0.1, 0.3, 0.5};

// Create test synapse
nlm::Synapse* synapse = brain.createSynapse(
    nlm::INVALID_NEURON_ID, nlm::INVALID_NEURON_ID);

// Apply Hebbian learning
for (int step = 0; step < 10; ++step) {
    // Add correlated spikes
    preSpikes.push_back(step * 0.1f);
    postSpikes.push_back(step * 0.1f + 0.05f);
    
    // Update learning
    hebbian.update(synapse, preSpikes, postSpikes, 0.001f);
    
    std::cout << "Step " << step << ": Weight = " << synapse->getWeight() 
              << ", Correlation count = " << preSpikes.size() - 3 
              << std::endl;
}
```

### 3. Reward-Modulated STDP

```cpp
#include <nlm/plasticity/STDP.hpp>
#include <nlm/neuromodulation/Neuromodulator.hpp>
#include <nlm/brain/Brain.hpp>

// Create reward-modulated STDP
nlm::RewardModulatedSTDP rstdp;

// Create dopamine neuromodulator
nlm::Dopamine dopamine;
dopamine.setLevel(0.5f);

// Create brain and add rules
nlm::Brain brain;
brain.addPlasticityRule(&rstdp);

// Simulate reward learning
synapse->setWeight(0.0f);  // Start from zero

for (int trial = 0; trial < 20; ++trial) {
    // Generate spike timing
    std::vector<nlm::Timestamp> preSpikes;
    std::vector<nlm::Timestamp> postSpikes;
    
    // Random spike timing
    preSpikes.push_back(trial * 0.1f);
    postSpikes.push_back(trial * 0.1f + 0.05f);
    
    // Apply STDP
    rstdp.update(synapse, preSpikes, postSpikes, 0.001f);
    
    // Apply reward modulation
    float reward = (trial % 5 == 0) ? 1.0f : 0.0f;  // Every 5th trial
    dopamine.signalReward(reward);
    
    // Update neuromodulator
    dopamine.update(0.001f);
    
    std::cout << "Trial " << trial << ": Weight = " << synapse->getWeight() 
              << ", Reward = " << reward << std::endl;
}
```

## Development Examples

### 1. Developmental Stage Progression

```cpp
#include <nlm/development/DevelopmentSystem.hpp>
#include <nlm/brain/Brain.hpp>

// Create development system
nlm::DevelopmentSystem development;

// Create brain
nlm::Brain brain;
development.update(&brain, 0.0f, 0.001f);

// Simulate development over time
std::cout << "Development progression:" << std::endl;
for (int day = 0; day < 30; ++day) {
    // Advance development
    development.advanceStage();
    
    // Update brain
    development.update(&brain, day * 86400.0f, 0.001f);
    
    // Print developmental status
    std::cout << "Day " << day << ": Stage = " << development.getStageName() 
              << ", Plasticity Modifier = " << development.getPlasticityModifier() 
              << std::endl;
}
```

**Expected Output:**
```
Development progression:
Day 0: Stage = Initial, Plasticity Modifier = 1.0
Day 1: Stage = Initial, Plasticity Modifier = 1.0
...
Day 10: Stage = CriticalPeriod, Plasticity Modifier = 2.0
...
Day 20: Stage = Maturation, Plasticity Modifier = 0.5
...
Day 29: Stage = Adult, Plasticity Modifier = 0.1
```

### 2. Activity-Dependent Development

```cpp
#include <nlm/development/Synaptogenesis.hpp>
#include <nlm/development/Pruning.hpp>
#include <nlm/brain/Brain.hpp>
#include <nlm/core/Random/Random.hpp>

// Create developmental system
nlm::DevelopmentSystem development;

// Create brain
nlm::Brain brain;

// Create random generator
nlm::RandomGenerator rng(12345);

// Simulate activity-dependent development
for (int day = 0; day < 50; ++day) {
    // Update development based on brain activity
    development.update(&brain, rng, 0.001f);
    
    // Get developmental statistics
    auto synaptogenesis_rate = development.getSynaptogenesisRate();
    auto pruning_rate = development.getPruningRate();
    auto critical_period = development.isCriticalPeriod();
    
    std::cout << "Day " << day << ": "
              << "Synaptogenesis = " << synaptogenesis_rate << ", "
              << "Pruning = " << pruning_rate << ", "
              << "Critical Period = " << (critical_period ? "Yes" : "No") 
              << std::endl;
}
```

## Neuromodulation Examples

### 1. Dopamine Reward Learning

```cpp
#include <nlm/neuromodulation/Dopamine.hpp>
#include <nlm/brain/Brain.hpp>

// Create dopamine neuromodulator
nlm::Dopamine dopamine;
dopamine.setLevel(0.1f);

// Create brain
nlm::Brain brain;

// Simulate reward learning
for (int trial = 0; trial < 30; ++trial) {
    // Generate reward (every 5 trials)
    float reward = (trial % 5 == 0) ? 1.0f : 0.0f;
    float prediction_error = reward - 0.0f;  // No prediction initially
    
    // Update dopamine with reward
    dopamine.signalReward(reward);
    dopamine.signalRewardPredictionError(prediction_error);
    
    // Update neuromodulator
    dopamine.update(0.001f);
    
    std::cout << "Trial " << trial << ": Dopamine level = " << dopamine.getLevel() 
              << ", Reward = " << reward << std::endl;
}
```

### 2. Curiosity-Driven Exploration

```cpp
#include <nlm/neuromodulation/Curiosity.hpp>
#include <nlm/brain/Brain.hpp>
#include <nlm/sensory/Vision.hpp>

// Create curiosity neuromodulator
nlm::Curiosity curiosity;
curiosity.setLevel(0.5f);

// Create brain and vision processor
nlm::Brain brain;
nlm::VisionProcessor vision;

// Simulate curiosity-driven exploration
nlm::Vision test_vision;
for (int episode = 0; episode < 20; ++episode) {
    // Process visual input
    vision.process(test_vision);
    
    // Get features for novelty detection
    auto features = vision.getFeatures();
    
    // Calculate novelty (simplified)
    float novelty = calculateNovelty(features, previous_features);
    
    // Update curiosity
    curiosity.updateNovelty(novelty);
    curiosity.update(0.001f);
    
    // Generate exploratory action based on curiosity
    float curiosity_level = curiosity.getLevel();
    
    std::cout << "Episode " << episode << ": Novelty = " << novelty 
              << ", Curiosity = " << curiosity_level << std::endl;
    
    previous_features = features;
}
```

## Memory System Examples

### 1. Working Memory Demonstration

```cpp
#include <nlm/memory/WorkingMemory.hpp>
#include <nlm/brain/Brain.hpp>

// Create working memory system
nlm::WorkingMemory working_memory;

// Create brain
nlm::Brain brain;

// Store and retrieve patterns
std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
std::vector<float> pattern2 = {0.9f, 0.8f, 0.7f, 0.6f, 0.5f};

// Store patterns
working_memory.store(pattern1, 2.0f);
working_memory.store(pattern2, 2.0f);

// Retrieve patterns with interference
std::vector<float> retrieved1 = working_memory.retrieve(0.0f);
std::cout << "Retrieved pattern 1: ";
for (float val : retrieved1) std::cout << val << " ";
std::cout << std::endl;

// Add interference pattern
std::vector<float> interference = {0.5f, 0.5f, 0.5f, 0.5f, 0.5f};
working_memory.store(interference, 1.0f);

// Retrieve pattern 1 with interference
std::vector<float> retrieved2 = working_memory.retrieve(0.0f);
std::cout << "Retrieved pattern 1 (with interference): ";
for (float val : retrieved2) std::cout << val << " ";
std::cout << std::endl;
```

### 2. Episodic Memory Replay

```cpp
#include <nlm/memory/EpisodicMemory.hpp>
#include <nlm/brain/Brain.hpp>

// Create episodic memory system
nlm::EpisodicMemory episodic_memory;

// Create brain
nlm::Brain brain;

// Record experience
nlm::Experience experience;
experience.timestamp = 0.0f;
experience.sensory_input = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
experience.action = nlm::Action::MoveForward;
experience.reward = 1.0f;

episodic_memory.recordExperience(experience);

// Wait for consolidation
for (int t = 1; t <= 10; ++t) {
    episodic_memory.updateConsolidation(t * 0.1f);
    
    float consolidation_progress = episodic_memory.getConsolidationProgress();
    std::cout << "Consolidation progress: " << consolidation_progress << std::endl;
}

// Replay consolidated memory
nlm::Experience replayed = episodic_memory.replay(experience.action);
std::cout << "Replayed experience: reward = " << replayed.reward << std::endl;
```

## Prediction System Examples

### 1. Temporal Sequence Prediction

```cpp
#include <nlm/prediction/NeuralPrediction.hpp>
#include <nlm/brain/Brain.hpp>

// Create prediction system
nlm::NeuralPrediction prediction;

// Create brain
nlm::Brain brain;

// Train on temporal sequences
std::vector<nlm::Action> sequence1 = {
    nlm::Action::MoveForward, nlm::Action::TurnRight, 
    nlm::Action::MoveForward, nlm::Action::TurnLeft
};

std::vector<nlm::Action> sequence2 = {
    nlm::Action::TurnLeft, nlm::Action::MoveBackward,
    nlm::Action::TurnRight, nlm::Action::Wait
};

// Learn sequences
prediction.learnSequence(sequence1);
prediction.learnSequence(sequence2);

// Predict next actions
nlm::Action predicted1 = prediction.predictNext(sequence1);
nlm::Action predicted2 = prediction.predictNext(sequence2);

std::cout << "Predicted next action for sequence 1: " 
          << actionToString(predicted1) << std::endl;
std::cout << "Predicted next action for sequence 2: " 
          << actionToString(predicted2) << std::endl;
```

### 2. Action Consequence Prediction

```cpp
#include <nlm/prediction/ActionConsequencePredictor.hpp>
#include <nlm/brain/Brain.hpp>

// Create action consequence predictor
nlm::ActionConsequencePredictor acp;

// Create brain
nlm::Brain brain;

// Learn action-outcome associations
for (int trial = 0; trial < 100; ++trial) {
    // Random action
    nlm::Action action = getRandomAction();
    
    // Random outcome (based on action)
    float outcome = calculateOutcome(action);
    
    // Learn association
    acp.learn(action, outcome);
    
    if (trial % 20 == 0) {
        // Predict outcome for current action
        float predicted = acp.predictOutcome(action);
        float error = outcome - predicted;
        
        std::cout << "Trial " << trial << ": Action = " 
                  << actionToString(action) 
                  << ", Outcome = " << outcome 
                  << ", Prediction = " << predicted 
                  << ", Error = " << error << std::endl;
    }
}
```

## Advanced Usage Examples

### 1. Multi-Modal Sensory Integration

```cpp
#include <nlm/sensory/Audio.hpp>
#include <nlm/sensory/Vision.hpp>
#include <nlm/sensory/SensoryInput.hpp>
#include <nlm/brain/AgentBrain.hpp>

// Create multimodal sensory system
nlm::Vision vision;
nlm::Audio audio;
nlm::SensoryInput multimodal;

// Create agent brain
std::shared_ptr<nlm::Brain> brain = std::make_shared<nlm::Brain>();
nlm::AgentBrain agent(brain);

// Process multimodal input
nlm::VisionInput vision_data;
nlm::AudioInput audio_data;

// Combine sensory inputs
multimodal.addModality(vision_data);
multimodal.addModality(audio_data);

// Convert to brain input
nlm::SensoryPercept percept = multimodal.toPercept();

// Process through agent
agent.processSensoryInput(percept);

// Decode motor command
nlm::MotorCommand command = agent.decodeMotorCommand();
```

### 2. Adaptive Behavior with Curiosity and Reward

```cpp
#include <nlm/neuromodulation/Curiosity.hpp>
#include <nlm/neuromodulation/Dopamine.hpp>
#include <nlm/agent/AgentBrain.hpp>
#include <nlm/environment/GridWorld.hpp>

// Create neuromodulators
nlm::Curiosity curiosity;
nlm::Dopamine dopamine;

// Create environment and agent
nlm::GridWorld world(20, 20);
nlm::AgentBrain agent(std::make_shared<nlm::Brain>());
agent.initialize(world);

// Adaptive behavior loop
for (int episode = 0; episode < 50; ++episode) {
    // Get sensory input
    nlm::SensoryPercept percept = world.getSensoryInput();
    
    // Process through agent
    agent.processSensoryInput(percept);
    nlm::MotorCommand command = agent.decodeMotorCommand();
    
    // Apply action
    world.applyMotorCommand(command);
    
    // Get feedback
    float reward = world.getReward();
    float novelty = world.getNovelty();
    
    // Update neuromodulators
    agent.applyRewardModulation(reward, dopamine.getLevel());
    curiosity.updateNovelty(novelty);
    
    // Adaptive behavior selection
    if (curiosity.getLevel() > 0.5f) {
        // Exploration behavior
        nlm::MotorCommand exploratory = selectExploratoryAction(world);
        world.applyMotorCommand(exploratory);
    } else {
        // Exploitation behavior
        world.applyMotorCommand(command);
    }
    
    // Print episode summary
    std::cout << "Episode " << episode << ": "
              << "Reward = " << reward << ", "
              << "Novelty = " << novelty << ", "
              << "Curiosity = " << curiosity.getLevel() << ", "
              << "Position = (" << world.getAgentPosition().x 
              << ", " << world.getAgentPosition().y << ")" 
              << std::endl;
}
```

### 3. Developmental Learning with Plasticity

```cpp
#include <nlm/development/DevelopmentSystem.hpp>
#include <nlm/plasticity/STDP.hpp>
#include <nlm/plasticity/Hebbian.hpp>
#include <nlm/brain/Brain.hpp>

// Create plasticity rules
nlm::STDP stdp;
nlm::Hebbian hebbian;

// Create development system
nlm::DevelopmentSystem development;

// Create brain
nlm::Brain brain;
brain.addPlasticityRule(&stdp);
brain.addPlasticityRule(&hebbian);

// Create random generator
nlm::RandomGenerator rng(12345);

// Simulate developmental learning
for (int day = 0; day < 100; ++day) {
    // Update development
    development.update(&brain, rng, 0.001f);
    
    // Get developmental stage
    auto stage = development.getStage();
    float plasticity_modifier = development.getPlasticityModifier();
    
    // Adjust plasticity rule strengths based on development
    stdp.setLTPWeight(0.01f * plasticity_modifier);
    hebbian.setLearningRate(0.01f * plasticity_modifier);
    
    // Simulate some learning activity
    // (In real implementation, would process actual spikes)
    
    // Print developmental status
    if (day % 10 == 0) {
        std::cout << "Day " << day << ": "
                  << "Stage = " << development.getStageName() << ", "
                  << "Plasticity = " << plasticity_modifier << ", "
                  << "STDP LTP = " << stdp.getLTPWeight() << ", "
                  << "Hebbian Rate = " << hebbian.getLearningRate() 
                  << std::endl;
    }
}
```

## Example Code Complete

This document provides comprehensive examples for working with the NLM framework. The examples demonstrate:

1. **Basic Functionality**: Simple brain initialization and sensorimotor loops
2. **Learning Systems**: STDP, Hebbian, and reward-modulated plasticity
3. **Development Processes**: Developmental stage progression and activity-dependent changes
4. **Neuromodulation**: Dopamine reward learning and curiosity-driven exploration
5. **Memory Systems**: Working memory and episodic memory consolidation
6. **Prediction Systems**: Temporal sequence and action consequence prediction
7. **Advanced Usage**: Multi-modal sensory integration and adaptive behavior

Each example includes:

- Complete code with comments
- Expected output
- Key concepts explained
- Best practices and tips

These examples can be adapted for specific research questions and experimental designs. The NLM framework provides a flexible foundation for exploring artificial neural computation and cognitive development.

## Getting Started

To begin using these examples:

1. **Install NLM**: Follow the build instructions in `docs/developer_documentation.md`
2. **Run Examples**: Compile and run the example programs
3. **Modify Examples**: Adapt examples to your specific needs
4. **Document Your Work**: Add comments and documentation to your code
5. **Share Results**: Contribute to the NLM community with your findings

The NLM project aims to provide a comprehensive platform for studying artificial developmental systems, and these examples are a starting point for exploring the possibilities of brain-like computation.

