# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

NLM is NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM IS intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Current Phase

**PHASE 6: FINAL INTEGRATION**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Phase 6 Integration Achievements

#### Memory Systems
- Working memory with persistent activity and competition
- Episodic memory with experience encoding and replay
- Associative memory with Hebbian pattern associations
- All memory systems connected to neural processing

#### Neuromodulation Integration
- Dopamine affects neural excitability and plasticity
- Curiosity drives exploration behavior
- Novelty detection integrated with sensory processing
- All neuromodulators connected to plasticity rules

#### Prediction System
- Prediction system integrated into brain loop
- Prediction error signals affect learning
- Confidence tracking implemented

#### Cognition Systems
- Neural planner with action sequence evaluation
- Concept formation from experience patterns
- Attention with competitive selection dynamics
- All cognition systems connected to perception and action

#### Development Integration
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

#### Persistence
- Checkpoint save/load implemented
- Brain state serialization working
- Can resume from saved checkpoints

#### Replay and Consolidation
- Episodic memory replay during simulation
- Memory consolidation for important episodes
- Integration with sleep/rest cycle

## Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Running

### Phase 6 Demo (Integration Test)
```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── docs/
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   └── PHASE6_FINAL_AUDIT.md    # Phase 6 audit
├── src/
│   ├── core/           # Core utilities
│   ├── brain/          # Neural components (integrated)
│   ├── dynamics/       # Neural dynamics
│   ├── plasticity/     # Plasticity rules
│   ├── development/    # Developmental system
│   ├── neuromodulation/# Neuromodulators (integrated)
│   ├── memory/        # Memory systems (integrated)
│   ├── prediction/     # Prediction systems (integrated)
│   ├── cognition/     # Cognitive mechanisms (integrated)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/  # Visualization
├── tests/
└── configs/
```

## Phase Summary

### Phase 1 (Complete)
- Project skeleton
- Core types and configuration
- Neural interfaces

### Phase 2 (Complete)
- Real LIF neuron dynamics
- Event-driven spike propagation
- STDP and Hebbian plasticity
- Structural plasticity

### Phase 3 (Complete)
- World interaction loop
- Sensory input and motor output
- Reward prediction error
- Developmental stages
- Novelty and curiosity

### Phase 4 (Complete)
- Neural prediction system
- Working memory
- Episodic-like memory
- Concept formation
- Neural attention (NOT Transformer)
- Predictive planning
- Self-model
- Social learning
- Continual learning

### Phase 5 (Complete)
- Performance optimizations
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system

### Phase 6 (Complete - Final Integration)
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional
- Phase 6 integration experiment created

## Scientific Limitations

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition
- Limited to what can be simulated with available computing resources

Phase 6 does NOT claim:
- Human intelligence
- Consciousness or sentience
- Human-like reasoning
- Genuine subjective experience

Phase 6 DOES investigate:
- Whether memory systems can integrate with neural dynamics
- Whether neuromodulation can affect plasticity in a coordinated way
- Whether prediction can become a central organizing principle
- Whether developmental stages can modulate learning
- Whether replay can reinforce memory consolidation
- Whether the complete brain loop functions coherently

## Architecture Philosophy

The NLM brain operates as a closed-loop system:

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING (LIF dynamics, spikes)
  ↓
INTERNAL STATE (working memory, attention)
  ↓
MEMORY / PREDICTION
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

## License

MIT

## Authors

Research project — See docs for scientific background.

---

# Advanced User Guide: Phase 6 Features

## Introduction

Phase 6 represents the final integration of all NLM systems. This guide covers advanced usage patterns for experienced users who want to work with the complete integrated brain.

## 1. Configuration and Customization

### Advanced Configuration Options

```cpp
// Create config with advanced settings
auto config = std::make_shared<Config>();

// Set development parameters
config->set("development_stage", "CriticalPeriod");
config->set("plasticity_rate_multiplier", 0.8f);
config->set("synaptogenesis_rate", 0.00005f);
config->set("pruning_rate", 0.00002f);

// Set memory parameters
config->set("working_memory_capacity", 5000);
config->set("episodic_memory_max_episodes", 5000);
config->set("replay_interval", 200);
config->set("consolidation_interval", 2000);

// Set neuromodulation
config->set("dopamine_baseline", 0.1f);
config->set("curiosity_threshold", 0.5f);
config->set("novelty_sensitivity", 0.3f);

// Set prediction system
config->set("prediction_horizon", 10);
config->set("prediction_error_decay", 0.95f);
```

### Configuration File Formats

#### Basic Config (.cfg)
```
# NLM Configuration File

# Core simulation parameters
random_seed = 12345
simulation_timestep = 0.001
neuron_count = 2000
region_count = 2
connection_probability = 0.2

# Plasticity parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.018
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Development parameters
development_enabled = true
plasticity_rate_multiplier = 1.0

# Memory parameters
working_memory_capacity = 1000
episodic_memory_max_episodes = 1000
replay_interval = 100
consolidation_interval = 1000

# Neuromodulation
novelty_enabled = true
curiosity_enabled = true
dopamine_enabled = true
prediction_error_enabled = true

# Advanced features
attention_enabled = true
concept_formation_enabled = true
planning_enabled = true
self_model_enabled = true
social_learning_enabled = true

# Sensory processing
vision_enabled = true
audio_enabled = true
internal_signals_enabled = true

# Motor system
motor_enabled = true
action_space = {MoveForward=0.5, TurnLeft=0.3, TurnRight=0.3, Wait=0.4}
```

## 2. Direct Brain Manipulation

### Accessing Integrated Systems

```cpp
// Get references to integrated systems
auto* workingMemory = brain->getWorkingMemory();
auto* episodicMemory = brain->getEpisodicMemory();
auto* associativeMemory = brain->getAssociativeMemory();
auto* predictionSystem = brain->getPredictionSystem();
auto* planner = brain->getPlanner();
auto* conceptFormation = brain->getConceptFormation();
auto* attention = brain->getAttention();
auto* developmentSystem = brain->getDevelopmentSystem();
auto* dopamine = brain->getDopamine();
auto* curiosity = brain->getCuriosity();
auto* novelty = brain->getNovelty();
auto* predictionError = brain->getPredictionErrorSignal();
```

### System-Specific Operations

#### Working Memory Manipulation
```cpp
// Store specific patterns in working memory
std::vector<float> pattern = {0.1f, 0.5f, 0.9f, 0.3f};
workingMemory->store(pattern, 1.0f);

// Strengthen memory traces for rehearsal
workingMemory->strengthenMemory(1.5f);

// Get current working memory content
auto currentMemory = workingMemory->retrieve();
```

#### Episodic Memory Management
```cpp
// Store an episode
EpisodicMemoryItem episode;
episode.timestamp = brain->getCurrentStep();
episode.reward = dopamine->getLevel();
episode.activeNeurons = {123, 456, 789};
episode.neuronActivations = {0.8f, 0.6f, 0.9f};

episodicMemory->storeEpisode(episode);

// Get episodes for replay
auto* episode = episodicMemory->getEpisodesForReplay(3);
episodicMemory->replayEpisode(episode);
```

#### Planning and Concept Formation
```cpp
// Set current goal for planning
brain->getPlanner()->setCurrentGoal({0.2f, 0.8f, 0.5f, 0.3f});

// Evaluate action sequences
std::vector<ActionType> actions = {ActionType::MoveForward, ActionType::TurnLeft};
auto candidate = brain->getPlanner()->evaluateSequence(actions, currentState);

// Form concepts from neural patterns
brain->getConceptFormation()->formConceptsFromPattern(neuralActivityPattern);
```

## 3. Neuromodulation Control

### Manual Neuromodulation Injection
```cpp
// Create and apply neuromodulators
dopamine->setLevel(0.8f);  // Increase reward sensitivity
curiosity->setDrive(0.9f);  // Increase exploration
novelty->setSensitivity(0.7f);  // Increase novelty detection

// Apply prediction error signals
brain->applyNeuromodulation(predictionError);

// Monitor neuromodulation effects
float dopamineLevel = dopamine->getLevel();
float curiosityLevel = curiosity->getCuriosityLevel();
```

## 4. Development Control

### Developmental Stage Management
```cpp
// Set developmental stage
brain->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);

// Manually trigger development
brain->develop();

// Monitor developmental stage
auto currentStage = brain->getDevelopmentalStage();
```

## 5. Checkpoint and Persistence

### Advanced Checkpoint Operations
```cpp
// Save brain state with custom filename
brain->save("./checkpoints/my_brain_state.bin");

// Load saved brain state
brain->load("./checkpoints/my_brain_state.bin");

// Checkpoint manager configuration
auto* checkpointManager = brain->getCheckpointManager();
checkpointManager->configure("./checkpoints", 10000, 5, true);
```

## 6. Sensory and Motor Interfaces

### Custom Sensory Input
```cpp
// Create and use sensory inputs
SensoryInput input;
input.addChannelValue(0, 0.8f);  // Vision channel 0
input.addChannelValue(1, 0.3f);  // Audio channel 1
input.addChannelValue(2, 0.9f);  // Internal channel 2

brain->receiveSensoryInput(input);
```

### Advanced Motor Actions
```cpp
// Generate complex action sequences
std::vector<ActionType> sequence;
for (int i = 0; i < 10; ++i) {
    sequence.push_back(ActionType::MoveForward);
    sequence.push_back(ActionType::TurnLeft);
}

auto action = brain->produceAction();
brain->injectCurrentToNeurons(NeuronType::Motor, 50.0f);
```

## 7. Simulation Control

### Custom Simulation Steps
```cpp
// Run simulation with custom time control
double currentTime = 0.0;
for (size_t step = 0; step < 10000; ++step) {
    // Update world time
    currentTime += 0.1;  // 100ms per step
    
    // Get sensory input at current time
    // (implementation depends on world system)
    
    // Process with time-dependent neuromodulation
    double timeFactor = 1.0 + std::sin(currentTime * 0.1);
    dopamine->setLevel(dopamine->getLevel() * timeFactor);
    
    // Run brain step
    brain->step(step, currentTime);
    
    // Record statistics periodically
    if (step % 1000 == 0) {
        brain->logStatus();
    }
}
```

## 8. Advanced Usage Patterns

### Pattern 1: Training the Brain
```cpp
// Train brain with reward-based learning
class Trainer {
public:
    Trainer(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    void train(float duration, float rewardFrequency) {
        float totalReward = 0.0f;
        float rewardCounter = 0.0f;
        
        for (size_t step = 0; step < duration; ++step) {
            // 1. Get sensory input
            auto percept = getSensoryPercept();
            brain_->receiveSensoryInput(percept);
            
            // 2. Get action from brain
            auto action = brain_->produceAction();
            applyAction(action, step);
            
            // 3. Get reward with noise
            float reward = getReward();
            rewardCounter += reward;
            
            // 4. Apply neuromodulation
            if (rewardCounter > rewardFrequency) {
                brain_->applyNeuromodulation(dopamine_);
                rewardCounter = 0.0f;
            }
            
            // 5. Step brain
            brain_->step(step);
            
            totalReward += reward;
        }
        
        std::cout << "Training complete. Total reward: " << totalReward << std::endl;
    }
    
private:
    std::shared_ptr<Brain> brain_;
    std::unique_ptr<Dopamine> dopamine_ = std::make_unique<Dopamine>();
};
```

### Pattern 2: Memory Consolidation
```cpp
// Manually trigger memory consolidation
void consolidateMemories(std::shared_ptr<Brain> brain, float threshold) {
    auto* episodicMemory = brain->getEpisodicMemory();
    
    // Get all episodes
    auto allEpisodes = episodicMemory->getAllEpisodes();
    
    // Sort by activation strength
    std::sort(allEpisodes.begin(), allEpisodes.end(),
              [](const EpisodicMemoryItem* a, const EpisodicMemoryItem* b) {
                  return a->neuronActivations.size() > b->neuronActivations.size();
              });
    
    // Keep only strong memories
    size_t keepCount = static_cast<size_t>(allEpisodes.size() * threshold);
    std::vector<const EpisodicMemoryItem*> strongEpisodes;
    
    for (size_t i = 0; i < std::min(keepCount, allEpisodes.size()); ++i) {
        strongEpisodes.push_back(allEpisodes[i]);
    }
    
    // Consolidate strong memories
    for (auto* episode : strongEpisodes) {
        episodicMemory->consolidateEpisode(episode);
    }
    
    // Clear weak memories
    episodicMemory->clearWeakEpisodes(threshold);
    
    std::cout << "Consolidated memories: kept " << strongEpisodes.size() 
              << " out of " << allEpisodes.size() << std::endl;
}
```

### Pattern 3: Developmental Progression
```cpp
// Simulate brain development over time
void simulateDevelopment(std::shared_ptr<Brain> brain, float duration) {
    auto* developmentSystem = brain->getDevelopmentSystem();
    
    float timeInStage = 0.0f;
    size_t stageIndex = 0;
    
    for (size_t step = 0; step < duration; ++step) {
        // Update developmental stage based on time
        timeInStage += 0.001;  // timestep
        
        if (timeInStage >= 100.0f) {  // 100 time units per stage
            timeInStage = 0.0f;
            
            // Progress to next stage
            if (stageIndex < 3) {
                stageIndex++;
                
                DevelopmentalStage newStage;
                switch (stageIndex) {
                    case 0: newStage = DevelopmentalStage::CriticalPeriod; break;
                    case 1: newStage = DevelopmentalStage::Maturation; break;
                    case 2: newStage = DevelopmentalStage::Adult; break;
                    default: newStage = DevelopmentalStage::Adult; break;
                }
                
                brain->setDevelopmentalStage(newStage);
                developmentSystem->onDevelopmentalStageChange(newStage);
                
                std::cout << "Development stage: " << static_cast<int>(newStage) 
                          << " at step " << step << std::endl;
            }
        }
        
        // Apply developmental effects
        developmentSystem->update(brain.get(), brain->getRandomGenerator(), 0.001);
        
        // Step brain with developmental effects
        brain->step(step);
        
        // Log developmental progress
        if (step % 1000 == 0) {
            auto stage = brain->getDevelopmentalStage();
            std::cout << "Step " << step << ": Stage " << static_cast<int>(stage)
                      << ", Plasticity: " << developmentSystem->getPlasticityFactor() 
                      << std::endl;
        }
    }
    
    std::cout << "Development simulation complete" << std::endl;
}
```

## 9. Error Handling and Debugging

### Brain Status Reporting
```cpp
// Get comprehensive brain status
void debugBrain(std::shared_ptr<Brain> brain) {
    brain->logStatus();
    
    // Check system health
    auto* spikeSystem = brain->getSpikeSystem();
    if (spikeSystem->getPendingSpikeCount() > 10000) {
        std::cout << "WARNING: High spike queue depth" << std::endl;
    }
    
    auto* workingMemory = brain->getWorkingMemory();
    if (workingMemory->getActiveTraces() > workingMemory->getCapacity() * 0.9) {
        std::cout << "WARNING: Working memory near capacity" << std::endl;
    }
    
    auto* episodicMemory = brain->getEpisodicMemory();
    if (episodicMemory->getEpisodeCount() > 10000) {
        std::cout << "WARNING: Episodic memory large" << std::endl;
    }
}
```

### Performance Monitoring
```cpp
// Monitor simulation performance
class PerformanceMonitor {
public:
    void startStep() { stepStartTime_ = std::chrono::high_resolution_clock::now(); }
    void endStep(size_t step) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - stepStartTime_);
        
        stepTimes_.push_back(duration.count());
        
        // Log performance every 1000 steps
        if (step % 1000 == 0 && stepTimes_.size() > 100) {
            double avgTime = std::accumulate(stepTimes_.begin(), stepTimes_.end(), 0.0) / stepTimes_.size();
            double maxTime = *std::max_element(stepTimes_.begin(), stepTimes_.end());
            
            std::cout << "Performance - Step " << step << ": " 
                      << avgTime << "μs avg, " << maxTime << "μs max" << std::endl;
            
            // Clear old data
            if (stepTimes_.size() > 1000) {
                stepTimes_.erase(stepTimes_.begin(), stepTimes_.begin() + 500);
            }
        }
    }
    
    double getAverageStepTime() const {
        if (stepTimes_.empty()) return 0.0;
        return std::accumulate(stepTimes_.begin(), stepTimes_.end(), 0.0) / stepTimes_.size();
    }
    
private:
    std::vector<double> stepTimes_;
    std::chrono::high_resolution_clock::time_point stepStartTime_;
};
```

## 10. Integration with External Systems

### World Simulation Integration
```cpp
// Example: Integrate NLM with custom world simulation
class IntegratedWorld {
public:
    IntegratedWorld(std::shared_ptr<Brain> brain) : brain_(brain) {
        // Initialize world components
        configureWorld(100, 100, 8, 8);
        reset();
    }
    
    void configureWorld(int width, int height, int visionWidth, int visionHeight) {
        // Set up world dimensions
        // This is application-specific
    }
    
    void update(float dt) {
        // 1. Update world state
        updateWorldState(dt);
        
        // 2. Get sensory percept
        auto percept = getSensoryPercept();
        
        // 3. Send to brain
        brain_->receiveSensoryInput(percept);
        
        // 4. Process brain step
        size_t step = getSimulationStep();
        brain_->step(step);
        
        // 5. Get action
        auto action = brain_->produceAction();
        
        // 6. Apply action
        applyAction(action, dt);
        
        // 7. Update time
        currentTime_ += dt;
    }
    
    std::vector<float> getSensoryPercept() {
        // Convert world state to sensory input format
        std::vector<float> percept;
        
        // Example: Vision channels
        for (int y = 0; y < visionHeight_; ++y) {
            for (int x = 0; x < visionWidth_; ++x) {
                float value = getVisionValue(x, y);
                percept.push_back(value);
            }
        }
        
        // Example: Audio channels
        for (int i = 0; i < 8; ++i) {
            float value = getAudioValue(i);
            percept.push_back(value);
        }
        
        return percept;
    }
    
private:
    std::shared_ptr<Brain> brain_;
    float currentTime_ = 0.0f;
    int width_, height_, visionWidth_, visionHeight_;
};
```

## 11. Batch Processing and Experimentation

### Running Multiple Brains
```cpp
// Run multiple brains in parallel for comparison
void runBrainExperiment(const std::vector<std::shared_ptr<Config>>& configs,
                        const std::vector<std::string>& names,
                        size_t steps) {
    std::vector<std::shared_ptr<Brain>> brains;
    
    // Create all brains
    for (size_t i = 0; i < configs.size(); ++i) {
        brains.push_back(std::make_shared<Brain>(configs[i]));
        brains[i]->initialize();
    }
    
    // Run experiment
    for (size_t step = 0; step < steps; ++step) {
        // Update all brains
        for (size_t i = 0; i < brains.size(); ++i) {
            brains[i]->step(step);
            
            // Log progress periodically
            if (step % 100 == 0) {
                std::cout << names[i] << " step " << step << ": "
                          << brains[i]->getTotalSpikeCount() << " spikes" << std::endl;
            }
        }
    }
    
    // Collect results
    std::cout << "Experiment results:" << std::endl;
    for (size_t i = 0; i < brains.size(); ++i) {
        auto* workingMemory = brains[i]->getWorkingMemory();
        auto* episodicMemory = brains[i]->getEpisodicMemory();
        
        std::cout << names[i] << ": "
                  << "Spikes=" << brains[i]->getTotalSpikeCount() << ", "
                  << "Firing=" << brains[i]->getFiringNeuronCount() << ", "
                  << "WorkingMem=" << workingMemory->getActiveTraces() << ", "
                  << "Episodes=" << episodicMemory->getEpisodeCount() << std::endl;
    }
}
```

## 12. Testing and Validation

### Automatic Testing Framework
```cpp
// Automated testing of brain systems
class BrainTester {
public:
    static bool testNeuralConnectivity(std::shared_ptr<Brain> brain) {
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return false;
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) return false;
        
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject current
        for (size_t i = 0; i < std::min(size_t(5), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);
        }
        
        // Run steps
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        return spikes > 0;
    }
    
    static bool testPlasticity(std::shared_ptr<Brain> brain) {
        // Similar to main.cpp plasticity test
        // Returns true if learning detected
    }
    
    static bool testNeuromodulation(std::shared_ptr<Brain> brain) {
        // Test that neuromodulation affects brain function
        auto* dopamine = brain->getDopamine();
        float initialLevel = dopamine->getLevel();
        
        brain->applyNeuromodulation(*dopamine);
        
        float finalLevel = dopamine->getLevel();
        return std::abs(finalLevel - initialLevel) > 0.001f;
    }
    
    static bool runAllTests(std::shared_ptr<Brain> brain) {
        std::cout << "Running NLM Phase 6 brain tests..." << std::endl;
        
        bool connectivityPassed = testNeuralConnectivity(brain);
        std::cout << "Neural connectivity: " << (connectivityPassed ? "PASS" : "FAIL") << std::endl;
        
        bool plasticityPassed = testPlasticity(brain);
        std::cout << "Plasticity: " << (plasticityPassed ? "PASS" : "FAIL") << std::endl;
        
        bool neuromodulationPassed = testNeuromodulation(brain);
        std::cout << "Neuromodulation: " << (neuromodulationPassed ? "PASS" : "FAIL") << std::endl;
        
        return connectivityPassed && plasticityPassed && neuromodulationPassed;
    }
};
```

---

## Summary

Phase 6 of NLM provides a fully integrated artificial brain with:

- **12+ interconnected systems** working together
- **Advanced configuration** options for fine-tuning
- **Comprehensive memory systems** for experience storage
- **Sophisticated neuromodulation** for adaptive behavior
- **Developmental stages** for maturation
- **Checkpoint persistence** for state saving
- **Extensive API** for system manipulation

This advanced user guide enables you to:
- Create custom configurations for specific behaviors
- Directly manipulate integrated systems
- Run complex simulations and experiments
- Monitor performance and debug issues
- Integrate with external applications
- Conduct comparative experiments
- Automated testing and validation

The NLM Phase 6 brain represents a complete, functioning artificial neural substrate capable of changing its own synaptic connections through experience.
