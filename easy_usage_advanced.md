# Advanced Usage Examples

### Mini Project 4: Memory-Enhanced Agent

```python
import pynlm
import random

# Create agent with enhanced memory capabilities
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Add memory experience history
for step in range(100):
    # Create varied experiences
    experience = [random.random(), random.random(), random.random(), 
                 random.random(), random.random()]
    pynlm.store_experience(brain, experience)
    
    brain.step(step)

# Show memory benefits
stats = pynlm.memory_stats(brain)
print("Memory-enhanced agent stats:", stats)

# Run with world interaction
world = pynlm.createSimpleWorld()
world.configure(width=25, height=25)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    # Store each sensory percept as experience
    percept_data = world.getSensoryPercept().getVision()
    pynlm.store_experience(brain, percept_data)

print("Memory-enhanced agent completed!")
```

### Mini Project 5: Prediction-Based Decision Making

```python
import pynlm
import random

# Create prediction-aware agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Generate some experience data for training
for step in range(50):
    # Simulate experiences
    experience = [random.random() for _ in range(5)]
    pynlm.store_experience(brain, experience)
    
    # Make predictions
    predicted = brain.get_prediction_error(brain)
    
    brain.step(step)

# Create agent
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run prediction-based simulation
for step in range(100):
    world.update(0.1)
    
    # Check prediction accuracy
    error = pynlm.get_prediction_error(brain)
    confidence = pynlm.get_prediction_confidence(brain)
    
    if confidence > 0.7 and error < 0.1:
        # High confidence, low error = good prediction
        print(f"Step {step}: High confidence prediction ({confidence:.2f})")
    
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)

print("Prediction-based decision making completed!")
```

### Mini Project 6: Development Tracking

```python
import pynlm

# Create development-monitoring agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.enableRewardModulation(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)
world.reset()
agent.initialize(world)

# Track development over time
development_log = []

for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    if step % 50 == 0:
        dev_status = pynlm.get_development_summary(brain)
        neuromod = pynlm.get_neuromodulation_summary(brain)
        status = {
            'step': step,
            'stage': dev_status['stage'],
            'stage_name': dev_status['stage_name'],
            'age': dev_status['age'],
            'plasticity': dev_status['plasticity_modifier'],
            'curiosity': neuromod.get('curiosity', 0),
            'dopamine': neuromod.get('dopamine', 0)
        }
        development_log.append(status)
        
        print(f"Step {step}: {dev_status['stage_name']} (plasticity: {dev_status['plasticity_modifier']:.2f})")

# Analyze development progression
print("\nDevelopment Analysis:")
print(f"Initial stage: {development_log[0]['stage_name']}")
print(f"Final stage: {development_log[-1]['stage_name']}")
print(f"Stage progression: {[d['stage'] for d in development_log]}")
print(f"Average curiosity: {sum(d['curiosity'] for d in development_log) / len(development_log):.3f}")
```

---

## Quick Reference

### New Advanced Functions

| Function | Description | Example |
|----------|-------------|---------|
| `pynlm.memory_stats(brain)` | Get memory system statistics | `stats = pynlm.memory_stats(brain)` |
| `pynlm.get_working_memory_traces(brain)` | Get working memory activity | `traces = pynlm.get_working_memory_traces(brain)` |
| `pynlm.get_episodic_memory_episodes(brain)` | Get episodic memory episodes | `episodes = pynlm.get_episodic_memory_episodes(brain)` |
| `pynlm.store_experience(brain, experience)` | Store experience in memory | `pynlm.store_experience(brain, [1.0, 2.0, 3.0])` |
| `pynlm.get_prediction_error(brain)` | Get prediction error | `error = pynlm.get_prediction_error(brain)` |
| `pynlm.get_prediction_confidence(brain)` | Get prediction confidence | `confidence = pynlm.get_prediction_confidence(brain)` |
| `pynlm.get_prediction_history(brain)` | Get prediction error history | `history = pynlm.get_prediction_history(brain)` |
| `pynlm.clear_prediction_history(brain)` | Clear prediction history | `pynlm.clear_prediction_history(brain)` |
| `pynlm.get_neuromodulation_summary(brain)` | Get neuromodulation status | `summary = pynlm.get_neuromodulation_summary(brain)` |
| `pynlm.apply_external_stimulus(brain, stimulus)` | Apply external stimulus | `pynlm.apply_external_stimulus(brain, [0.5, -0.3, 0.8])` |
| `pynlm.get_development_summary(brain)` | Get development status | `dev = pynlm.get_development_summary(brain)` |
| `pynlm.trigger_development_event(brain, event_name)` | Trigger development event | `pynlm.trigger_development_event(brain, "CriticalPeriodClosure")` |
| `pynlm.get_system_status(brain)` | Get system status | `status = pynlm.get_system_status(brain)` |

### Development Event Names

- `"CriticalPeriodClosure"` - Close critical period for neural plasticity
- `"PlasticityShift"` - Shift plasticity mechanisms
- `"MemoryReorganization"` - Reorganize memory systems
- `"AttentionCalibration"` - Calibrate attention systems
- `"ConceptIntegration"` - Integrate new concepts

### New Brain Methods

- `getDevelopmentalStageName()` - Get developmental stage as string
- `getDevelopmentAge()` - Get current developmental age
- `getPlasticityModifier()` - Get current plasticity modifier
- `getAttentionFocus()` - Get current attention focus
- `getNeuromodulationLevel()` - Get overall neuromodulation level
- `getCuriosityLevel()` - Get curiosity level
- `getNoveltyLevel()` - Get novelty level
- `getPredictionError()` - Get prediction error level
- `triggerEvent(event)` - Trigger development event
- `applyExternalStimulus(stimulus)` - Apply external stimulus to sensory neurons
- `storeExperience(experience)` - Store experience in episodic memory
- `getMemoryStats()` - Get comprehensive memory statistics
- `getPredictionStats()` - Get prediction system statistics
- `getDevelopmentStats()` - Get development system statistics

---

## More Information

### Read These Documents Next:

1. **`HOW_TO_USE.md`** - Detailed technical documentation
2. **`docs/ARCHITECTURE.md`** - System architecture and design
3. **`docs/PHASE6_FINAL_AUDIT.md`** - Phase 6 final integration audit
4. **`docs/SCIENCE.md`** - Scientific background and methodology

### Get Help:

- **Python API Support**: Check the function documentation above
- **System Status**: Use `pynlm.get_system_status(brain)` to debug issues
- **Development Tracking**: Use `pynlm.get_development_summary(brain)` to monitor progress

### For Developers:

- **Source Code**: All NLM source is in the `src/` directory
- **Tests**: Tests are in the `tests/` directory
- **Documentation**: Comprehensive documentation in the `docs/` directory
- **Python Bindings**: Modified in `python/bindings.cpp`

---

That's it! You're now ready to use NLM's advanced features with the enhanced Python API.

The new advanced functions give you much more control over:

- **Memory Systems**: Store and retrieve experiences, track memory statistics
- **Prediction Systems**: Monitor prediction errors, get confidence levels, train prediction models
- **Neuromodulation**: Track neuromodulator levels, understand their effects
- **Development**: Monitor developmental stages, trigger events, analyze progression
- **System Status**: Get comprehensive system status and debugging information

Use these advanced features to build more sophisticated NLM simulations!
