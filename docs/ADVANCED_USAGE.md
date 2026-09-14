# NLM - Advanced Usage Guide for Power Users

**Warning:** This guide assumes familiarity with NLM basics. Advanced users will benefit from understanding the complete integrated brain system.

## Table of Contents

1. [Advanced Configuration Management](#advanced-configuration-management)
2. [Configuration Validation and Profiles](#configuration-validation-and-profiles)
3. [Advanced Brain Control Methods](#advanced-brain-control-methods)
4. [Power User Examples](#power-user-examples)
5. [Performance Optimization](#performance-optimization)
6. [Debugging and Monitoring](#debugging-and-monitoring)

---

## 1. Advanced Configuration Management

### Configuration Sources and Precedence

NLM supports multiple configuration sources with clear precedence:

```cpp
// Configuration source precedence (highest to lowest):
// 1. Runtime settings (set() calls)
// 2. Command line arguments
// 3. Configuration file (JSON format)
// 4. Default values
// 5. Essential system parameters
```

### JSON Configuration Schema

Advanced users can create complete configuration schemas:

```json
{
    "schema_version": "1.0",
    "strict_mode": true,
    "required_keys": [
        "neuron_count",
        "connection_probability",
        "simulation_timestep"
    ],
    "default_values": {
        "neuron_count": {"__type": "int", "value": 1000},
        "connection_probability": {"__type": "double", "value": 0.1},
        "plasticity.stdp.enable": {"__type": "bool", "value": true},
        "neuromod.dopamine.scale": {"__type": "double", "value": 1.0}
    },
    "configuration": {
        "neuron_count": {
            "__type": "int",
            "value": 2000,
            "description": "Total number of neurons in the brain",
            "category": "brain"
        },
        "plasticity.stdp.learning_rate": {
            "__type": "double", 
            "value": 0.005,
            "description": "STDP learning rate (higher = faster learning)",
            "category": "plasticity"
        }
    }
}
```

### Configuration Categories

- **Brain**: neuron count, region count, connection parameters
- **Dynamics**: membrane time constants, thresholds, refractory periods
- **Plasticity**: STDP, Hebbian, structural plasticity rates
- **Neuromodulation**: dopamine, curiosity, novelty parameters
- **Memory**: working memory capacity, episodic memory settings
- **Prediction**: prediction accuracy thresholds, confidence levels
- **Development**: stage transition ages, plasticity modifiers
- **Performance**: simulation timestep, recording rates, checkpoints

---

## 2. Configuration Validation and Profiles

### Configuration Validation

Validate configurations before use:

```cpp
#include "pynlm.h"

void validateConfiguration() {
    pynlm::Config config = pynlm::createDefaultConfig();
    
    // Set advanced parameters
    config.set("brain.neuron_count", 5000);
    config.set("plasticity.stdp.learning_rate", 0.01);
    config.set("neuromod.dopamine.scale", 2.0);
    config.set("memory.working.capacity", 100);
    
    pynlm::Brain brain(config);
    
    // Validate configuration
    if (!brain.validateConfig()) {
        std::cerr << "Configuration validation failed!" << std::endl;
        auto errors = brain.getConfigValidationErrors();
        for (const auto& error : errors) {
            std::cerr << "Error: " << error << std::endl;
        }
        return;
    }
    
    // Initialize with validated config
    brain.initialize();
}
```

### Configuration Profiles

Create and use configuration profiles:

```cpp
void useConfigurationProfiles() {
    pynlm::Config config = pynlm::createDefaultConfig();
    pynlm::Brain brain(config);
    
    // Create different profiles for different use cases
    brain.addConfigProfile("high_performance");
    brain.addConfigProfile("deep_learning");
    brain.addConfigProfile("fast_exploration");
    
    // Load a specific profile
    if (brain.loadConfigProfile("deep_learning")) {
        std::cout << "Loaded deep_learning profile" << std::endl;
        brain.initialize();
        
        // Run simulation with profile
        for (int step = 0; step < 1000; ++step) {
            brain.step(step);
            // Monitor performance
            if (step % 100 == 0) {
                brain.logBrainStatistics();
            }
        }
    }
    
    // Merge configurations
    pynlm::Config otherConfig;
    otherConfig.set("plasticity.hebbian.enable", true);
    brain.mergeConfig(otherConfig);
}
```

### Advanced Profile Features

```cpp
void advancedProfileUsage() {
    pynlm::Config config = pynlm::createDefaultConfig();
    pynlm::Brain brain(config);
    
    // Add profile with custom validation
    brain.addConfigProfile("strict_validation");
    
    // Apply validation rules
    bool valid = brain.updateConfig("brain.neuron_count", 10000);
    if (valid) {
        std::cout << "Neuron count updated successfully" << std::endl;
    }
    
    // List available profiles
    std::vector<std::string> profiles;
    brain.listConfigProfiles(profiles);
    
    // Reset to profile
    if (brain.loadConfigProfile("high_performance")) {
        brain.resetToTemplate("high_performance");
    }
}
```

---

## 3. Advanced Brain Control Methods

### Direct Neural Manipulation

Access and manipulate neurons directly:

```cpp
void directNeuralControl() {
    pynlm::Config config = pynlm::createDefaultConfig();
    pynlm::Brain brain(config);
    brain.initialize();
    
    // Get specific neurons
    auto neurons = brain.getNeuronsByType(pynlm::NeuronType::Sensory);
    auto highestActivity = brain.getHighestActivityNeuron();
    auto lowestActivity = brain.getLowestActivityNeuron();
    
    // Manipulate neuron states
    if (highestActivity) {
        brain.setNeuronActive(highestActivity->getId(), true);
        brain.setNeuronMembranePotential(highestActivity->getId(), -40.0f);
    }
    
    // Trigger specific neuron spikes
    if (lowestActivity) {
        brain.triggerNeuronSpike(lowestActivity->getId());
    }
    
    // Get neuron statistics
    pynlm::NeuronStats stats;
    brain.getNeuronStats(highestActivity->getId(), stats);
}
```

### Synapse Control

Manipulate synaptic connections:

```cpp
void advancedSynapseControl() {
    pynlm::Config config = pynlm::createDefaultConfig();
    pynlm::Brain brain(config);
    brain.initialize();
    
    // Get all synapses from a source neuron
    auto sourceNeurons = brain.getNeuronsByType(pynlm::NeuronType::Motor);
    if (!sourceNeurons.empty()) {
        auto synapses = brain.getSynapsesFrom(sourceNeurons[0]->getId());
        
        for (auto* syn : synapses) {
            // Modify synaptic weights
            float currentWeight = syn->getWeight();
            syn->setWeight(currentWeight * 1.5f);  // Strengthen
            
            // Modify delay
            syn->setDelay(2);
            
            // Enable/disable plasticity
            syn->enablePlasticity(true, true, true);
            
            // Set eligibility trace
            syn->setEligibilityTrace(0.8f);
        }
    }
    
    // Get synapse statistics
    auto targetNeurons = brain.getNeuronsByType(pynlm::NeuronType::Sensory);
    if (!targetNeurons.empty() && !synapses.empty()) {
        pynlm::SynapseStats stats;
        brain.getSynapseStats(synapses[0], stats);
    }
}
```

### Memory System Control

Direct access to memory systems:

```cpp
void memorySystemControl() {
    pynlm::Config config = pynlm::createDefaultConfig();
    pynlm::Brain brain(config);
    brain.initialize();
    
    // Working memory operations
    auto workingMem = brain.getWorkingMemory();
    if (workingMem) {
        workingMem->setCapacity(500);
        workingMem->setDecayRate(0.01f);
        
        // Access current traces
        auto traces = workingMem->getActiveTraces();
        for (const auto& trace : traces) {
            // Process working memory content
        }
    }
    
    // Episodic memory operations
    auto episodicMem = brain.getEpisodicMemory();
    if (episodicMem) {
        episodicMem->setMaxEpisodes(10000);
        episodicMem->setReplayInterval(50);
        episodicMem->setConsolidationInterval(500);
        
        // Replay specific episodes
        auto episodes = episodicMem->getEpisodesForReplay(5);
        for (const auto* episode : episodes) {
            episodicMem->replayEpisode(episode);
        }
    }
    
    // Attention system
    auto attention = brain.getAttention();
    if (attention) {
        attention->setInhibitionStrength(0.6f);
        attention->setExcitationStrength(1.2f);
        attention->setBiasStrength(0.5f);
    }
}
```

### Neuromodulation Control

Advanced neuromodulation settings:

```cpp
void advancedNeuromodulationControl() {
    pynlm::Config config = pynlm::createDefaultConfig();
    pynlm::Brain brain(config);
    brain.initialize();
    
    // Set neuromodulation levels directly
    brain.setDopamineLevel(0.5f);
    brain.setCuriosityLevel(0.8f);
    brain.setNoveltyLevel(0.3f);
    brain.setPredictionError(0.2f);
    
    // Batch neuromodulation updates
    std::unordered_map<pynlm::NeuromodulatorType, float> levels;
    levels[pynlm::NeuromodulatorType::Dopamine] = 0.7f;
    levels[pynlm::NeuromodulatorType::Curiosity] = 0.6f;
    levels[pynlm::NeuromodulatorType::Novelty] = 0.4f;
    levels[pynlm::NeuromodulatorType::PredictionError] = 0.1f;
    
    for (const auto& pair : levels) {
        brain.setNeuromodulationLevel(pair.first, pair.second);
    }
    
    // Get all neuromodulation levels
    std::unordered_map<pynlm::NeuromodulatorType, float> currentLevels;
    brain.getAllNeuromodulationLevels(currentLevels);
    
    // Apply neuromodulation signals
    pynlm::Dopamine dopamine(0.5f);
    brain.applyNeuromodulationSignal(pynlm::NeuromodulatorType::Dopamine, dopamine);
}
```

---

## 4. Power User Examples

### Example 1: Curiosity-Driven Exploration

Create an agent that learns through curiosity-driven exploration:

```python
def curiosity_driven_exploration():
    """Power user example: Agent that explores through curiosity"""
    
    # Configure for exploration
    config = pynlm.createDefaultConfig()
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("brain.development.stage_transitions", True)
    config.set("plasticity.structural.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable exploration subsystems
    agent.enableCuriosity(True)
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    
    # Run exploration simulation
    exploration_history = []
    novel_situations = 0
    
    for step in range(5000):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Track novelty
        novelty = agent.getNoveltyLevel()
        if novelty > 0.7:
            novel_situations += 1
        
        exploration_history.append({
            'step': step,
            'action': action,
            'curiosity': agent.getCuriosityLevel(),
            'novelty': novelty,
            'reward': reward
        })
        
        # Log periodically
        if step % 500 == 0:
            print(f"Step {step}: Curiosity={agent.getCuriosityLevel():.2f}, Novelty={novelty:.2f}, Action={action}")
    
    return exploration_history, novel_situations
```

### Example 2: Developmental Learning

Create an agent that develops over time:

```python
def developmental_learning():
    """Power user example: Agent with developmental stages"""
    
    # Configure for development
    config = pynlm.createDefaultConfig()
    config.set("brain.development.enabled", True)
    config.set("plasticity.structural.enable", True)
    config.set("neuromod.dopamine.scale", 1.5)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Track development stages
    developmental_stages = []
    
    for step in range(10000):
        # Update development
        agent.updateDevelopment(0.1)
        
        # Get current stage
        stage = brain.getDevelopmentalStage()
        developmental_stages.append(stage)
        
        # Adapt based on stage
        if stage == pynlm.DevelopmentalStage.CriticalPeriod:
            # Critical period for learning - increase curiosity
            agent.enableCuriosity(True)
            config.set("plasticity.stdp.learning_rate", 0.02)
            
        elif stage == pynlm.DevelopmentalStage.Maturation:
            # Maturation - decrease plasticity
            agent.enableCuriosity(False)
            config.set("plasticity.stdp.learning_rate", 0.005)
            
        elif stage == pynlm.DevelopmentalStage.Adult:
            # Adult - stable learning
            config.set("plasticity.stdp.learning_rate", 0.001)
        
        # Run simulation
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Record stage characteristics
        if step % 1000 == 0:
            print(f"Stage {stage} at step {step}: "
                  f"Plasticity={agent.getPlasticityLevel():.2f}, "
                  f"Curiosity={agent.getCuriosityLevel():.2f}")
    
    return developmental_stages
```

### Example 3: Memory Management

Advanced memory system control:

```python
def advanced_memory_management():
    """Power user example: Custom memory management"""
    
    # Configure memory systems
    config = pynlm.createDefaultConfig()
    config.set("memory.working.capacity", 200)
    config.set("memory.episodic.max_episodes", 5000)
    config.set("memory.replay.interval", 50)
    config.set("memory.consolidation.interval", 500)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Custom memory monitoring
    memory_stats = []
    replay_sessions = []
    
    for step in range(3000):
        # Run simulation
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Monitor memory systems
        working_mem = brain.getWorkingMemory()
        episodic_mem = brain.getEpisodicMemory()
        
        if working_mem:
            active_traces = working_mem.getActiveTraces()
            memory_stats.append({
                'step': step,
                'working_memory_traces': active_traces,
                'working_memory_utilization': active_traces / working_mem.getCapacity()
            })
        
        if episodic_mem:
            episode_count = episodic_mem.getEpisodeCount()
            memory_stats[-1]['episodic_episodes'] = episode_count
            
            # Trigger custom replay
            if step % 100 == 0 and step > 0:
                recent_episodes = episodic_mem.getEpisodesForReplay(3)
                replay_sessions.append({
                    'step': step,
                    'episodes': len(recent_episodes),
                    'duration': 100
                })
                
                for episode in recent_episodes:
                    episodic_mem.replayEpisode(episode)
        
        # Custom memory consolidation
        if step % 1000 == 0 and episodic_mem:
            episodic_mem.consolidate(0.5)  # Aggressive consolidation
    
    return memory_stats, replay_sessions
```

---

## 5. Performance Optimization

### Advanced Performance Tuning

```cpp
void performance_optimization() {
    pynlm::Config config = pynlm.createDefaultConfig();
    
    // Performance-oriented configuration
    config.set("simulation_timestep", 0.0005);  // Smaller timestep for accuracy
    config.set("performance.record_rate", 100.0);  // High recording rate
    config.set("checkpoint.interval", 10000);  // Frequent checkpoints
    config.set("brain.region_count", 4);  // More regions for parallelization
    
    pynlm::Brain brain(config);
    
    // Configure recording
    brain.startRecording("./high_performance_recording");
    brain.setRecordingRate(50.0);  // 50 Hz recording
    
    // Run optimized simulation
    for (int step = 0; step < 50000; ++step) {
        brain.step(step);
        
        if (step % 1000 == 0) {
            brain.logBrainStatistics();
        }
        
        if (step % 5000 == 0) {
            brain.exportToCSV("./performance_data.csv");
        }
    }
    
    brain.stopRecording();
}
```

### Memory Pool Configuration

```cpp
void memory_pool_tuning() {
    pynlm::Config config = pynlm.createDefaultConfig();
    
    // Optimize memory pools for large simulations
    config.set("performance.memory_pool.neuron_pool_size", 100000);
    config.set("performance.memory_pool.synapse_pool_size", 5000000);
    config.set("performance.event_queue.size", 100000);
    config.set("performance.sparse_connectivity.threshold", 0.01);
    
    pynlm::Brain brain(config);
    brain.initialize();
    
    // Monitor memory usage
    for (int step = 0; step < 20000; ++step) {
        brain.step(step);
        
        if (step % 1000 == 0) {
            brain.logBrainStatistics();
        }
    }
}
```

---

## 6. Debugging and Monitoring

### Advanced Debugging Techniques

```cpp
void advanced_debugging() {
    pynlm::Config config = pynlm.createDefaultConfig();
    config.set("logging.level", "DEBUG");  // Enable debug logging
    
    pynlm::Brain brain(config);
    brain.initialize();
    
    // Set up comprehensive logging
    brain.startVideoRecording("./debug_recording");
    
    // Run with detailed monitoring
    for (int step = 0; step < 10000; ++step) {
        brain.step(step);
        
        // Log detailed statistics
        if (step % 100 == 0) {
            brain.logBrainStatistics();
        }
        
        // Check for anomalies
        auto neurons = brain.getAllNeurons();
        for (auto* neuron : neurons) {
            const auto& state = neuron->getState();
            if (std::abs(state.membranePotential) > 100.0f) {
                // Log potential instability
                NLM_LOG_ERROR("Neuronal instability detected at step " + 
                             std::to_string(step) + 
                             ", potential: " + 
                             std::to_string(state.membranePotential));
            }
        }
        
        // Check synaptic stability
        auto synapses = brain.getAllSynapses();
        for (auto* syn : synapses) {
            float weight = syn->getWeight();
            if (std::abs(weight) > 100.0f) {
                NLM_LOG_WARNING("Synaptic weight instability at step " + 
                               std::to_string(step) + 
                               ", weight: " + std::to_string(weight));
            }
        }
    }
    
    brain.stopVideoRecording();
    brain.snapshot("debug_final_state");
}
```

### Performance Profiling

```cpp
void performance_profiling() {
    pynlm::Config config = pynlm.createDefaultConfig();
    config.set("performance.profiling.enable", true);
    
    pynlm::Brain brain(config);
    brain.initialize();
    
    // Run profiling simulation
    brain.startRecording("./profiling_data");
    
    for (int step = 0; step < 20000; ++step) {
        brain.step(step);
        
        if (step % 1000 == 0) {
            // Export profiling data
            brain.exportToCSV("./profile_data_step_" + 
                            std::to_string(step) + ".csv");
        }
    }
    
    brain.stopRecording();
    
    // Analyze profiling data
    analyzeProfilingData();
}
```

---

## Key Takeaways for Power Users

1. **Configuration Mastery**: Understand configuration sources, profiles, and validation for optimal control
2. **Direct Neural Access**: Use low-level neuron and synapse manipulation for custom behaviors
3. **Memory System Control**: Configure working memory, episodic memory, and attention systems
4. **Neuromodulation Precision**: Fine-tune dopamine, curiosity, and novelty for desired learning patterns
5. **Development Control**: Leverage developmental stages for ontogenetic learning curves
6. **Performance Optimization**: Configure memory pools, recording, and simulation parameters
7. **Comprehensive Monitoring**: Use advanced logging and profiling for debugging and optimization

These examples demonstrate how power users can leverage NLM's full capabilities for advanced artificial brain research and development.

---

*This document is intended for advanced users familiar with NLM concepts. Begin with basic examples in `easy_usage.md` and `HOW_TO_USE.md` before attempting advanced configurations.*
