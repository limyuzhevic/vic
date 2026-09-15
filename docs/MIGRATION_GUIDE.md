# NLM Migration Guide: From Previous Versions to Phase 6

## Overview

This guide provides comprehensive information for migrating from previous NLM versions to Phase 6 (Final Integration). Phase 6 represents a major paradigm shift from disconnected component architectures to a fully integrated artificial brain system.

## Migration Summary

**Key Changes in Phase 6:**

1. **Integration vs. Disconnection**: Phase 6 connects previously standalone systems into a coherent brain loop
2. **Memory Systems**: Working, episodic, and associative memory are now integrated and operational
3. **Neuromodulation**: Full integration of dopamine, curiosity, and novelty systems
4. **Prediction**: Neural prediction becomes a central organizing principle
5. **Cognition**: Planning, concept formation, and attention systems are now integrated
6. **Development**: Developmental stages properly modulate plasticity and behavior
7. **Persistence**: Checkpoint system is fully connected to the brain

**Migration Impact**: Code written for Phase 4 or earlier will require significant changes to access Phase 6 capabilities.

---

## Migration Paths

### For Users (Research Programmers)

#### **Minimal Migration: Basic Functionality**
- Continue using existing basic APIs (Brain, SimpleWorld, AgentBrain)
- Add Phase 6 integration calls explicitly
- Focus on integration features rather than new systems

#### **Advanced Migration: Full Phase 6 Capabilities**
- Update code to use Phase 6 integrated systems
- Implement memory-based learning and prediction
- Leverage neuromodulation for behavior control
- Use developmental stages for adaptive learning

#### **Research Migration: Custom Integration**
- Design custom integration of specific Phase 6 components
- Use Phase 6 architecture as foundation for specialized systems
- Combine Phase 6 with external frameworks and tools

---

## API Changes and Migration Strategies

### Core Brain API

#### **Before Phase 6**
```cpp
// Memory systems returned nullptr - needed null checks
NeuralWorkingMemory* workingMemory = brain.getWorkingMemory(); // nullptr
if (workingMemory) {
    workingMemory->store(pattern);
}

// Episodic memory not available
NeuralEpisodicMemory* episodicMemory = brain.getEpisodicMemory(); // nullptr

// Prediction system disconnected
PredictionSystem* prediction = brain.getPredictionSystem(); // nullptr

// Save/load were stubs
bool saved = brain.save("checkpoint.bin"); // false
```

#### **Phase 6 Integration**
```cpp
// Phase 6: All memory systems available and integrated
NeuralWorkingMemory* workingMemory = brain.getWorkingMemory(); // working
if (workingMemory) {
    workingMemory->store(pattern);  // Now actually stores
    workingMemory->update(dt);     // Auto-updates each step
}

// Phase 6: Episodic memory operational
NeuralEpisodicMemory* episodicMemory = brain.getEpisodicMemory(); // working
if (episticMemory) {
    episodicMemory->storeEpisode(input, action, reward, timestamp);
    episodicMemory->replayEpisode(episodeId);  // Auto-replay every 100 steps
}

// Phase 6: Prediction system integrated
PredictionSystem* prediction = brain.getPredictionSystem(); // working
if (prediction) {
    prediction->train(input, timestamp);  // Auto-trains from experience
    prediction->getPrediction(pattern);  // Provides predictions
}

// Phase 6: Save/load fully functional
bool saved = brain.save("checkpoint.bin"); // true
bool loaded = brain.load("checkpoint.bin"); // true
```

### AgentBrain API

#### **Before Phase 6**
```cpp
// Basic neuromodulation
agent.enableRewardModulation(True);
agent.enableCuriosity(True);

// No planning or concept formation
NeuralPlanner* planner = brain.getPlanner(); // nullptr
ConceptFormation* concepts = brain.getConceptFormation(); // nullptr
```

#### **Phase 6 Integration**
```cpp
// Phase 6: Comprehensive neuromodulation
agent.enableRewardModulation(True);      // Reward-modulated learning
agent.enableStructuralPlasticity(True);  // Synaptogenesis
agent.enableDevelopment(True);           // Developmental stages
agent.enableCuriosity(True);             // Exploration drive
agent.enableEpisodicMemory(True);        // Experience storage
agent.enablePrediction(True);           // Forward models
agent.enableConceptFormation(True);     // Pattern abstraction
agent.enablePlanning(True);             // Action selection
```

### Memory System API Changes

#### **Working Memory**
**Before**: Placeholder with basic storage
```cpp
class NeuralWorkingMemory {
public:
    void store(const std::vector<float>& pattern, float strength = 1.0f);
    std::vector<float> retrieve() const;
    // Basic implementation with limited functionality
};
```

**Phase 6**: Advanced neural dynamics-based system
```cpp
class NeuralWorkingMemory {
public:
    // Neural persistence mechanisms
    void store(const std::vector<float>& pattern, float strength = 1.0f);
    std::vector<float> retrieve() const;
    
    // Automatic updates and maintenance
    void update(TimestepDuration dt);
    
    // Advanced features
    void strengthenMemory(float factor);
    void runCompetition();
    size_t getActiveTraces() const;
    float getMemoryUtilization() const;
    
    // Integration with brain dynamics
    void initialize(Brain* brain);
};
```

#### **Episodic Memory**
**Before**: Basic struct, never updated
```cpp
class NeuralEpisodicMemory {
public:
    // Placeholder - never updated in brain loop
    void storeEpisode(const SensoryInput& input, const Action& action, float reward, Timestamp timestamp) {}
    // No replay mechanism
};
```

**Phase 6**: Full experience storage and consolidation
```cpp
class NeuralEpisodicMemory {
public:
    // Experience encoding
    void storeEpisode(const SensoryInput& input, const Action& action, float reward, Timestamp timestamp);
    std::unique_ptr<EpisodicMemoryEntry> retrieveEpisode(size_t episodeId) const;
    
    // Replay and consolidation
    void replayEpisode(size_t episodeId, float strength = 1.0f);
    void replayAllEpisodes(float strength = 1.0f);
    void consolidateOldEpisodes(Timestamp cutoffTime);
    
    // Auto-updates
    void update(TimestepDuration dt);
    
    // Statistics and management
    size_t getEpisodeCount() const;
    size_t getActiveReplayCount() const;
    float getMemoryUtilization() const;
};
```

---

## Migration Examples

### Example 1: Basic Sensorimotor Loop (Phase 4 → Phase 6)

#### **Before Phase 6**
```cpp
// Phase 4: Limited sensorimotor integration
void run_sensorimotor_loop(Brain& brain, SimpleWorld& world, AgentBrain& agent) {
    for (int step = 0; step < 1000; ++step) {
        // Basic sensory processing
        auto percept = world.getSensoryPercept();
        agent.processSensoryInput(percept);
        
        // Basic neural processing
        brain.step(step);
        
        // Basic motor decoding
        auto action = agent.decodeMotorCommand();
        world.applyMotorCommand(action, world.getSimulationTime());
        
        // Basic reward
        float reward = percept.getInternal()[0];
        agent.applyRewardModulation(reward, 0.0);
    }
}
```

#### **Phase 6: Enhanced Sensorimotor Integration**
```cpp
// Phase 6: Full integration with memory and prediction
void run_phase6_sensorimotor_loop(Brain& brain, SimpleWorld& world, AgentBrain& agent) {
    for (int step = 0; step < 1000; ++step) {
        // Update world
        world.update(0.1);
        
        // Get sensory input
        auto percept = world.getSensoryPercept();
        
        // Process sensory input (includes attention)
        agent.processSensoryInput(percept);
        
        // Full brain processing (integrates ALL systems)
        brain.step(step);
        
        // Decode motor command (includes planning and curiosity)
        auto action = agent.decodeMotorCommand();
        world.applyMotorCommand(action, world.getSimulationTime());
        
        // Reward modulation with prediction error
        float reward = percept.getInternal()[0];
        float predicted_reward = agent.getPredictionError();
        agent.applyRewardModulation(reward, predicted_reward);
        
        // Development update
        agent.updateDevelopment(0.1);
        
        // Optional: memory statistics
        if (step % 100 == 0) {
            auto working_mem = brain.getWorkingMemory();
            auto episodic_mem = brain.getEpisodicMemory();
            
            if (working_mem) {
                std::cout << "Working memory traces: " << working_mem->getActiveTraces() << std::endl;
            }
            
            if (episodic_mem) {
                std::cout << "Episodic memories: " << episodic_mem->getEpisodeCount() << std::endl;
            }
        }
    }
}
```

### Example 2: Learning and Memory (Phase 3 → Phase 6)

#### **Before Phase 6**
```cpp
// Phase 3: Basic learning with limited memory
void learn_and_store(Brain& brain, const std::vector<float>& pattern) {
    // No episodic memory available
    // Working memory has limited functionality
    auto working_mem = brain.getWorkingMemory();
    if (working_mem) {
        working_mem->store(pattern);
        // Basic decay, no replay
    }
    
    // No prediction system
    // No concept formation
    // No episodic replay
}
```

#### **Phase 6: Comprehensive Learning and Memory**
```cpp
// Phase 6: Full learning and memory integration
void learn_and_store_phase6(Brain& brain, const std::vector<float>& pattern, 
                           const SensoryInput& sensoryInput, const Action& action, float reward) {
    // Working memory with automatic updates
    auto working_mem = brain.getWorkingMemory();
    if (working_mem) {
        working_mem->store(pattern);
        working_mem->update(brain.getConfig()->getOr<double>("simulation_timestep", 0.001));
    }
    
    // Episodic memory with automatic storage and replay
    auto episodic_mem = brain.getEpisodicMemory();
    if (episodic_mem) {
        // Current timestamp
        double timestamp = brain.getConfig()->getOr<double>("simulation_time", 0.0);
        episodic_mem->storeEpisode(sensoryInput, action, reward, timestamp);
        
        // Check for automatic replay (every 100 steps)
        if (brain.getConfig()->getOr<size_t>("replay_interval", 100) > 0) {
            episodic_mem->replayAllEpisodes();
        }
    }
    
    // Prediction system with learning
    auto prediction = brain.getPredictionSystem();
    if (prediction) {
        prediction->train(sensoryInput, timestamp);
    }
    
    // Concept formation
    auto concepts = brain.getConceptFormation();
    if (concepts) {
        concepts->processExperience(sensoryInput, action, reward);
    }
    
    // Planning and attention
    auto planner = brain.getPlanner();
    auto attention = brain.getAttention();
    
    if (planner && attention) {
        // Update cognitive systems
        planner->update(brain.getConfig()->getOr<double>("simulation_timestep", 0.001));
        attention->update(brain.getConfig()->getOr<double>("simulation_timestep", 0.001));
    }
}
```

### Example 3: Development and Adaptation (Phase 3 → Phase 6)

#### **Before Phase 6**
```cpp
// Phase 3: Basic development stages
void run_development(Brain& brain, double timestep) {
    // Limited developmental effects
    brain.setDevelopmentalStage(DevelopmentalStage::Adult);
    
    // Minimal plasticity changes
    // No age-based effects on memory
    // No critical period integration
}
```

#### **Phase 6: Advanced Development**
```cpp
// Phase 6: Comprehensive developmental system
void run_development_phase6(Brain& brain, double timestep) {
    auto dev_system = brain.getDevelopmentSystem();
    if (dev_system) {
        // Update development system
        dev_system->update(timestep);
        
        // Get current stage and age
        DevelopmentalStage stage = brain.getDevelopmentalStage();
        double age = dev_system->getAge();
        
        // Age-based effects on all systems
        if (age > 100.0) {  // After 100 time units
            // Enhanced memory consolidation in mature stage
            auto episodic_mem = brain.getEpisodicMemory();
            if (episodic_mem) {
                episodic_mem->consolidateOldEpisodes(age - 100.0);
            }
            
            // Improved planning capabilities
            auto planner = brain.getPlanner();
            if (planner) {
                // Planning adapts with development
            }
        }
    }
}
```

---

## Migration Checklist

### Code Analysis

#### **Systematic Migration**
- [ ] Identify all disconnected system calls in existing code
- [ ] Map disconnected systems to their Phase 6 counterparts
- [ ] Plan integration order based on system dependencies
- [ ] Test each system integration before connecting others

#### **Memory System Integration**
- [ ] Check for existing `getWorkingMemory()` calls
- [ ] Add episodic memory storage where experiences occur
- [ ] Implement automatic replay for episodic memories
- [ ] Add memory statistics and monitoring

#### **Neuromodulation Integration**
- [ ] Update `enableRewardModulation()` to use prediction error
- [ ] Add `enableCuriosity()` for exploration
- [ ] Add `enableDevelopment()` for adaptive behavior
- [ ] Integrate with dopamine system for learning modulation

#### **Prediction and Cognition**
- [ ] Add prediction training from sensory input
- [ ] Implement concept formation for pattern discovery
- [ ] Add planning for action sequence evaluation
- [ ] Integrate attention for selective processing

### Testing and Validation

#### **Unit Testing**
- [ ] Test individual Phase 6 systems
- [ ] Verify memory storage and retrieval
- [ ] Test neuromodulation effects on learning
- [ ] Validate prediction accuracy

#### **Integration Testing**
- [ ] Test full Phase 6 brain loop
- [ ] Verify memory replay mechanisms
- [ ] Check neuromodulation system interactions
- [ ] Validate development progression

#### **Performance Testing**
- [ ] Monitor memory usage with `getMemoryUtilization()`
- [ ] Benchmark Phase 6 performance vs. Phase 4
- [ ] Test scalability with different configuration sizes
- [ ] Validate checkpoint save/load functionality

---

## Configuration Migration

### Phase 4 Configuration → Phase 6 Configuration

#### **Phase 4 Configuration**
```json
{
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1,
        "plasticity": {
            "stdp": {
                "enable": true,
                "learning_rate": 0.001
            }
        }
    },
    "neuromod": {
        "curiosity": {
            "enable": true
        }
    }
}
```

#### **Phase 6 Enhanced Configuration**
```json
{
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1,
        "plasticity": {
            "stdp": {
                "enable": true,
                "learning_rate": 0.001,
                "tau_plus": 20.0,
                "tau_minus": 20.0
            },
            "structural": {
                "enable": true,
                "synaptogenesis_rate": 0.0001,
                "pruning_rate": 0.00001
            }
        }
    },
    "neuromod": {
        "dopamine": {
            "scale": 1.0
        },
        "curiosity": {
            "enable": true
        },
        "novelty": {
            "enable": true
        }
    },
    "memory": {
        "replay_interval": 100,
        "consolidation_interval": 1000,
        "working_memory_capacity": 100,
        "episodic_memory_window": 10000
    },
    "prediction": {
        "learning_rate": 0.01,
        "prediction_window": 10
    },
    "cognition": {
        "planning_horizon": 5.0,
        "concept_learning_rate": 0.001,
        "attention_strength": 0.5
    },
    "development": {
        "maturation_rate": 0.001,
        "critical_period_strength": 1.0,
        "homeostatic_drive": 0.1
    }
}
```

---

## Performance Considerations

### Memory Usage in Phase 6

| System | Memory Usage | Notes |
|--------|--------------|-------|
| Working Memory | ~10-50 traces | Depends on activity, auto-compression |
| Episodic Memory | 100-500 episodes | Per 10K steps, replay optimization |
| Associative Memory | ~5-10% of working memory | Pattern associations |
| Prediction System | ~20-50 MB | History tracking and models |
| Concept Formation | ~5-15 MB | Abstract representations |
| Total (1K neurons) | ~50-80 MB | With Phase 6 optimizations |

### Performance Optimization Strategies

1. **Memory Management**:
   - Use `getMemoryUtilization()` to monitor load
   - Configure replay intervals for performance vs. retention trade-offs
   - Adjust working memory capacity based on task requirements

2. **Computation Optimization**:
   - Phase 6 adds overhead for system integration
   - Benchmark performance for your specific use case
   - Consider configuration optimization for your target platform

3. **Scalability**
   - Phase 6 scales better with larger networks
   - Memory systems provide more value as neural population increases
   - Consider larger configurations for complex tasks

### Performance Comparison

| Metric | Phase 4 | Phase 6 |
|--------|--------|--------|
| Basic sensorimotor loop | Fast, simple | Slower, but richer behavior |
| Memory storage | Limited | Comprehensive episodic memory |
| Learning | Basic plasticity | Integrated neuromodulation + prediction |
| Planning | None | Hierarchical planning with curiosity |
| Concept formation | None | Automatic pattern discovery |
| Development | Basic stages | Age-dependent system modulation |
| Replay | None | Automatic consolidation |

---

## Troubleshooting Migration Issues

### Common Migration Problems

**1. Memory Systems Not Working**
```cpp
// Problem: Memory systems returning nullptr or empty
if (!brain.getWorkingMemory() || !brain.getEpisodicMemory()) {
    std::cout << "ERROR: Phase 6 memory systems not properly initialized!" << std::endl;
    // Check that Phase 6 is properly configured and all features are enabled
}
```

**Solution**: Verify Phase 6 configuration and ensure all subsystems are enabled.

**2. Prediction System Not Learning**
```cpp
// Problem: Prediction system not updating
if (brain.getPredictionSystem() && 
    brain.getPredictionErrorSignal()->getPredictionError() > 0.9) {
    std::cout << "WARNING: Prediction system may not be training properly" << std::endl;
    // Check if prediction system is receiving sensory input
}
```

**Solution**: Ensure prediction system receives sensory input and has proper training configuration.

**3. Development Not Progressing**
```cpp
// Problem: Development stages not changing
if (brain.getDevelopmentalStage() == DevelopmentalStage::Initial) {
    std::cout << "WARNING: Development system may not be updating" << std::endl;
    // Check development system initialization
}
```

**Solution**: Verify development system is properly initialized and receives time updates.

### Migration Debugging Tools

```cpp
// Phase 6 integration diagnostic function
void debug_phase6_integration(Brain& brain, AgentBrain& agent) {
    std::cout << "=== Phase 6 Integration Debug ===" << std::endl;
    
    // Check memory systems
    std::cout << "Memory Systems:" << std::endl;
    auto working_mem = brain.getWorkingMemory();
    if (working_mem) {
        std::cout << "  Working Memory: Active traces = " << working_mem->getActiveTraces() << std::endl;
        std::cout << "               Memory utilization = " << working_mem->getMemoryUtilization() << std::endl;
    } else {
        std::cout << "  Working Memory: NOT AVAILABLE" << std::endl;
    }
    
    auto episodic_mem = brain.getEpisodicMemory();
    if (episodic_mem) {
        std::cout << "  Episodic Memory: Episodes = " << episodic_mem->getEpisodeCount() << std::endl;
        std::cout << "                Replay count = " << episodic_mem->getActiveReplayCount() << std::endl;
    } else {
        std::cout << "  Episodic Memory: NOT AVAILABLE" << std::endl;
    }
    
    // Check neuromodulation
    std::cout << "Neuromodulation:" << std::endl;
    std::cout << "  Curiosity level = " << agent.getCuriosityLevel() << std::endl;
    std::cout << "  Novelty level = " << agent.getNoveltyLevel() << std::endl;
    std::cout << "  Prediction error = " << agent.getPredictionError() << std::endl;
    
    // Check development
    std::cout << "Development:" << std::endl;
    std::cout << "  Current stage = " << static_cast<int>(brain.getDevelopmentalStage()) << std::endl;
    std::cout << "  Age = " << brain.getDevelopmentSystem()->getAge() << std::endl;
    
    // Check prediction
    std::cout << "Prediction System:" << std::endl;
    if (brain.getPredictionSystem()) {
        std::cout << "  Prediction system: ACTIVE" << std::endl;
    } else {
        std::cout << "  Prediction system: INACTIVE" << std::endl;
    }
    
    std::cout << "=== End Debug ===" << std::endl;
}
```

---

## Advanced Migration Techniques

### Gradual Integration

**Strategy**: Migrate from Phase 4 to Phase 6 incrementally

```cpp
// Gradual migration: Step 1 - Add basic Phase 6 features
void migrate_step1(Brain& brain, AgentBrain& agent) {
    // Enable core Phase 6 features
    agent.enableRewardModulation(true);
    agent.enableCuriosity(true);
    agent.enableDevelopment(true);
    
    // Add episodic memory
    agent.enableEpisodicMemory(true);
    
    // Basic prediction
    agent.enablePrediction(true);
}

// Gradual migration: Step 2 - Add cognitive features
void migrate_step2(Brain& brain, AgentBrain& agent) {
    // Add concept formation
    agent.enableConceptFormation(true);
    
    // Add planning
    agent.enablePlanning(true);
    
    // Add attention
    agent.enableAttention(true);
    
    // Optimize for memory usage
    agent.setMemoryCapacity(100);
}

// Gradual migration: Step 3 - Full integration
void migrate_step3(Brain& brain, AgentBrain& agent) {
    // Full Phase 6 integration
    agent.enableAllPhase6Features();
    
    // Configure for performance
    agent.configureForTask("learning");
    
    // Enable replay mechanisms
    agent.enableReplay(true);
    agent.enableConsolidation(true);
}
```

### Custom Integration

**Strategy**: Use Phase 6 as foundation for custom systems

```cpp
// Custom integration for specific research task
class CustomResearchIntegration {
public:
    CustomResearchIntegration(Brain& brain, AgentBrain& agent) 
        : brain_(brain), agent_(agent) {}
    
    void setup_for_memory_research() {
        // Enable Phase 6 memory systems
        agent_.enableEpisodicMemory(true);
        agent_.enablePrediction(true);
        
        // Configure for memory research
        agent_.setMemoryParameters({
            {"replay_interval", 50},
            {"consolidation_window", 5000},
            {"working_memory_capacity", 200}
        });
        
        std::cout << "Custom memory research integration complete" << std::endl;
    }
    
    void setup_for_planning_research() {
        // Enable Phase 6 planning systems
        agent_.enablePlanning(true);
        agent_.enableConceptFormation(true);
        
        // Configure for planning research
        agent_.setPlanningParameters({
            {"planning_horizon", 10.0},
            {"exploration_rate", 0.3},
            {"exploitation_rate", 0.7}
        });
        
        std::cout << "Custom planning research integration complete" << std::endl;
    }
    
private:
    Brain& brain_;
    AgentBrain& agent_;
};
```

---

## Migration Success Criteria

### Technical Success
- [ ] All Phase 6 memory systems operational
- [ ] Neuromodulation systems integrated and functional
- [ ] Prediction system providing accurate predictions
- [ ] Development system progressing through stages
- [ ] Cognitive systems (planning, attention) operational
- [ ] Checkpoint save/load working correctly
- [ ] Replay and consolidation mechanisms functional

### Behavioral Success
- [ ] Agent exhibits curiosity-driven exploration
- [ ] Memory systems store and retrieve experiences
- [ ] Learning progresses over time
- [ ] Development affects behavior appropriately
- [ ] Prediction improves with experience
- [ ] Cognitive abilities emerge from integration
- [ ] Agent adapts to changing environment

### Performance Success
- [ ] Memory usage within acceptable limits
- [ ] Simulation performance meets requirements
- [ ] Integration overhead acceptable for functionality gains
- [ ] Scalability tested for target application

---

## Conclusion

Migration from Phase 4 to Phase 6 represents a significant paradigm shift from disconnected components to an integrated artificial brain. The journey requires:

1. **Understanding Phase 6 Architecture**: Recognize how all systems interconnect
2. **Comprehensive Testing**: Validate each system before integration
3. **Performance Optimization**: Balance functionality with computational costs
4. **Gradual Migration**: Integrate systems incrementally to manage complexity
5. **Research Validation**: Ensure Phase 6 achieves research goals

The payoff of Phase 6 migration is substantial: an artificial brain capable of
memory-based learning, prediction-driven behavior, curiosity-guided exploration,
and developmental adaptation that emerges from system integration rather than
pre-programmed behaviors.

---

*This migration guide is designed to help researchers and developers successfully transition from Phase 4 to Phase 6, leveraging the full power of the integrated artificial brain architecture.*
