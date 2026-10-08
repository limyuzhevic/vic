# NLM (熙然) - User Guide

**An Experimental Artificial Developmental Brain**

## Table of Contents

1. [Quick Start Guide](#quick-start-guide)
   - [1.1 Installation](#11-installation)
   - [1.2 Basic World Setup](#12-basic-world-setup)
   - [1.3 Running Your First Brain](#13-running-your-first-brain)

2. [Basic Concepts and Terminology](#basic-concepts-and-terminology)
   - [2.1 Core Components](#21-core-components)
   - [2.2 Neural Architecture](#22-neural-architecture)
   - [2.3 Memory Systems](#23-memory-systems)
   - [2.4 Neuromodulation](#24-neuromodulation)
   - [2.5 Development](#25-development)

3. [Common Use Cases and Patterns](#common-use-cases-and-patterns)
   - [3.1 Simple Agent Behavior](#31-simple-agent-behavior)
   - [3.2 Learning Environments](#32-learning-environments)
   - [3.3 Sensory-Motor Integration](#33-sensory-motor-integration)
   - [3.4 Cognitive Development](#34-cognitive-development)

## Quick Start Guide

### 1.1 Installation

#### System Requirements
- **Platform**: Linux/macOS (Windows with WSL)
- **Compiler**: C++17 compatible compiler (g++ 9+, clang 12+)
- **RAM**: 8GB minimum, 16GB+ recommended for full brain simulation
- **Storage**: ~500MB for core NLM system

#### Building from Source

```bash
# Clone the repository
mkdir -p ~/projects
cd ~/projects
cp /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481 nlm
# Or use git clone

cd nlm

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DNLM_PHASE=6 \
    -DNLM_ENABLE_EXPERIMENTS=ON \
    -DNLM_ENABLE_VISUALIZATION=OFF

# Build with parallel compilation
make -j$(nproc)
```

### 1.2 Basic World Setup

Create a simple world simulation environment:

```cpp
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include "brain/Brain.hpp"

int main() {
    // Create a simple world
    SimpleWorld world;
    
    // Create an agent brain
    AgentBrain agentBrain;
    
    // Create the neural brain
    Brain brain;
    
    // Initialize with configuration
    Config config;
    config.neuronCount = 1000;
    config.maxSteps = 10000;
    
    if (!brain.initialize(config)) {
        std::cerr << "Failed to initialize brain" << std::endl;
        return 1;
    }
    
    // Create agent
    Agent agent(&agentBrain, &brain, &world);
    
    return 0;
}
```

### 1.3 Running Your First Brain

Run the Phase 6 integration demo to see all systems working together:

```bash
# Navigate to build directory
cd build

# Run the demo (you can modify with custom parameters)
./nlm_phase6_demo --steps 1000

# Or run with checkpointing
./nlm_phase6_demo --steps 500 --save-state demo1.nlm

# Or load from checkpoint
./nlm_phase6_demo --load-state demo1.nlm --steps 1000
```

## Basic Concepts and Terminology

### 2.1 Core Components

#### Brain Class
The main entry point for neural computation. Coordinates all brain systems:
- **Neurons**: ~7,500 spiking neurons with LIF dynamics
- **Memory systems**: Working, episodic, and associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty signals
- **Prediction**: Forward models for sensory prediction
- **Cognition**: Planning, attention, concept formation

#### AgentBrain Class
Integrates the neural brain with the world environment:
- **Sensory Processing**: Converts world percepts to neural input
- **Motor Decoding**: Converts neural activity to actions
- **Reward Processing**: Applies learning signals
- **Development**: Manages developmental stages

#### Key Interfaces
- **SensoryInput**: Provides environmental input to brain
- **Action**: Represents behavioral output
- **Observation**: Structured sensory data

### 2.2 Neural Architecture

#### Neurons
Spiking neurons using Leaky Integrate-and-Fire (LIF) dynamics:
- **Membrane potential**: Changes over time with decay
- **Threshold**: When reached, neuron fires a spike
- **Refractory period**: Temporary inability to fire after spike
- **Synaptic inputs**: Summation of incoming currents

#### Synapses
Connections between neurons:
- **Excitatory/Inhibitory**: Can be positive or negative weight
- **Delay**: 1-5ms spike transmission delay
- **Plasticity**: Can change strength over time

#### Neural Populations
Groups of neurons with shared properties:
- **Sensory neurons**: Process environmental input
- **Motor neurons**: Generate behavioral output
- **Internally connected**: Maintain working memory

### 2.3 Memory Systems

#### Working Memory
Transient active information storage:
- **Location**: Distributed across neural populations
- **Mechanism**: Persistent activity through recurrent connections
- **Capacity**: ~384 neurons across sensory modalities
- **Function**: Maintains information for immediate use

#### Episodic Memory
Experience storage with replay:
- **Storage**: Neural patterns of sensory states
- **Replay**: During rest/consolidation periods
- **Retrieval**: Pattern completion from partial cues
- **Relevance**: Weighted by reward signals

#### Associative Memory
Pattern associations:
- **Learning**: Hebbian synaptic connections between experiences
- **Recall**: Spread activation from related memories
- **Completion**: Recover full patterns from partial cues

### 2.4 Neuromodulation

#### Dopamine
Reward prediction error signaling:
- **Phasic signals**: Immediate RPEs for learning
- **Tonic signals**: Background learning rate modulation
- **Eligibility traces**: Assign credit to recent actions
- **Synaptic scaling**: Adjust synaptic strength

#### Curiosity
Exploration motivation:
- **Novelty detection**: Identifies new sensory patterns
- **Exploration drive**: Balances exploration vs. exploitation
- **Attention modulation**: Prioritizes novel information
- **Learning scheduling**: Controls exploration timing

#### Novelty
Change detection:
- **Pattern comparison**: Detects sensory changes over time
- **Decay-based tracking**: Maintains novelty memory
- **Threshold-based detection**: Identifies significant changes

### 2.5 Development

#### Developmental Stages
Age-dependent plasticity and learning:
- **Initial**: High plasticity, rapid learning
- **Critical Period**: Sensitive periods for skill acquisition
- **Maturation**: Specialization and refinement
- **Adult**: Stable function, fine-tuning
- **Aging**: Declining plasticity, wisdom accumulation

#### Synaptogenesis and Pruning
Activity-dependent structural changes:
- **Synaptogenesis**: Use-dependent synapse formation
- **Competitive pruning**: Eliminate weak connections
- **Homeostatic regulation**: Maintain network stability
- **Developmental timing**: Age-dependent rates

## Common Use Cases and Patterns

### 3.1 Simple Agent Behavior

Basic reinforcement learning in a controlled environment:

```cpp
// Simple navigation task
class NavigationTask {
private:
    SimpleWorld& world;
    AgentBrain& agentBrain;
    Brain& brain;
    
public:
    NavigationTask(SimpleWorld& w, AgentBrain& ab, Brain& b) 
        : world(w), agentBrain(ab), brain(b) {}
    
    void run(int steps) {
        for (int step = 0; step < steps; ++step) {
            // 1. Observe world
            SensoryPercept percept = world.observe(agent);
            
            // 2. Process sensory input
            agentBrain.processSensoryInput(percept);
            
            // 3. Run brain computation
            brain.step(step);
            
            // 4. Generate action
            MotorCommand cmd = agentBrain.decodeMotorCommand();
            
            // 5. Apply action to world
            world.applyAction(agent, cmd);
            
            // 6. Get reward
            float reward = world.computeReward(agent);
            
            // 7. Apply reward modulation
            agentBrain.applyRewardModulation(reward);
            
            // Progress reporting
            if (step % 100 == 0) {
                std::cout << "Step: " << step 
                         << " Reward: " << reward 
                         << " Avg Rate: " << brain.getAverageFiringRate()
                         << std::endl;
            }
        }
    }
};
```

### 3.2 Learning Environments

Create environments that promote learning:

```cpp
class LearningEnvironment {
public:
    enum ChallengeType {
        NAVIGATION,
        OBJECT_RECOGNITION,
        SEQUENCE_LEARNING,
        SOCIAL_INTERACTION
    };
    
    static std::unique_ptr<SimpleWorld> createChallenge(
        ChallengeType type,
        const Config& config) {
        
        auto world = std::make_unique<SimpleWorld>();
        
        switch (type) {
            case NAVIGATION:
                setupNavigationWorld(*world, config);
                break;
            case OBJECT_RECOGNITION:
                setupObjectRecognitionWorld(*world, config);
                break;
            case SEQUENCE_LEARNING:
                setupSequenceLearningWorld(*world, config);
                break;
            case SOCIAL_INTERACTION:
                setupSocialLearningWorld(*world, config);
                break;
        }
        
        return world;
    }
    
private:
    static void setupNavigationWorld(SimpleWorld& world, const Config& config) {
        // Create maze environment
        // Add reward for reaching goals
        // Implement exploration incentives
    }
    
    // Other setup methods...
};
```

### 3.3 Sensory-Motor Integration

Combine sensory processing with action selection:

```cpp
class SensoryMotorIntegration {
public:
    SensoryMotorIntegration(Brain& brain, AgentBrain& agentBrain)
        : brain(brain), agentBrain(agentBrain) {}
    
    void integrate() {
        // Configure sensory input pathways
        configureSensoryInput();
        
        // Configure motor output pathways  
        configureMotorOutput();
        
        // Establish feedback loops
        establishFeedbackLoops();
        
        // Test integration
        testIntegration();
    }
    
private:
    void configureSensoryInput() {
        // Map sensory modalities to neural populations
        // Set up input scaling and filtering
        // Configure attention modulation
    }
    
    void configureMotorOutput() {
        // Map neural activity to motor commands
        // Set up action selection mechanisms
        // Configure reward-based modulation
    }
    
    void establishFeedbackLoops() {
        // Connect prediction errors to learning
        // Link attention to sensory priority
        // Integrate development with plasticity
    }
    
    void testIntegration() {
        // Run simple stimulus-response test
        // Verify memory storage and retrieval
        // Check action selection consistency
    }
}; 
```

### 3.4 Cognitive Development

Simulate developmental progression:

```cpp
class CognitiveDevelopment {
public:
    enum DevelopmentalStage {
        INFANT,
        CHILD,
        ADOLESCENT,
        ADULT,
        AGED
    };
    
    CognitiveDevelopment(Brain& brain, DevelopmentSystem& devSys)
        : brain(brain), developmentSystem(devSys) {
        currentStage = INFANT;
        stageProgress = 0.0f;
    }
    
    void advanceStage() {
        switch (currentStage) {
            case INFANT:
                currentStage = CHILD;
                stageProgress = 0.0f;
                initializeChildStage();
                break;
            case CHILD:
                currentStage = ADOLESCENT;
                stageProgress = 0.0f;
                initializeAdolescentStage();
                break;
            case ADOLESCENT:
                currentStage = ADULT;
                stageProgress = 0.0f;
                initializeAdultStage();
                break;
            case ADULT:
                currentStage = AGED;
                stageProgress = 0.0f;
                initializeAgedStage();
                break;
        }
    }
    
    void simulateDevelopment(int steps) {
        for (int step = 0; step < steps; ++step) {
            // Update developmental stage based on progress
            updateDevelopmentalStage(step);
            
            // Apply stage-specific effects
            applyStageEffects();
            
            // Progress development
            if (stageProgress >= stageDuration) {
                advanceStage();
            }
            
            stageProgress += getStageProgressRate();
            
            // Run brain computation
            brain.step(step);
        }
    }
    
private:
    void initializeChildStage() {
        // Increase plasticity rates
        // Enable new learning mechanisms
        // Reduce exploration drive
    }
    
    // Other stage initialization methods...
    
    float getStageProgressRate() {
        switch (currentStage) {
            case INFANT: return 0.1f;   // Rapid development
            case CHILD: return 0.05f;   // Moderate development
            case ADOLESCENT: return 0.02f; // Slow development
            case ADULT: return 0.01f;  // Minimal development
            case AGED: return 0.005f;  // Very slow development
        }
        return 0.01f;
    }
}; 
```

## References

For more detailed information:

- [NLM Phase 6 Final Audit](docs/PHASE6_FINAL_AUDIT.md)
- [NLM Architecture](docs/ARCHITECTURE.md)
- [Building Instructions](README.md#building-and-installation)
- [Example Applications](examples/)

For technical support, visit the [GitHub repository](https://github.com/Kilo-Org/nlm).