Advanced Usage Examples

This section demonstrates advanced NLM usage patterns for Phase 6 features and complex simulation scenarios.

### Advanced Phase 6 Integration

The Phase 6 integration brings together multiple advanced features:

#### Text Encoding and Concept Formation

```python
import pynlm

# Create a brain with Phase 6 text encoding support
config = pynlm.createDefaultConfig()

# Enable advanced configuration for text processing
config.set("text_encoder_max_tokens", 200)
config.set("text_embedding_dimension", 32)

brain = pynlm.createBrain(config)
brain.initialize()

# Create agent with full Phase 6 capabilities
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all Phase 6 subsystems for comprehensive learning
agent.enableRewardModulation(True)      # Reward-based plasticity
agent.enableStructuralPlasticity(True)   # Synaptogenesis/pruning
agent.enableDevelopment(True)           # Developmental stages
agent.enableCuriosity(True)             # Exploration motivation

print("Phase 6 brain initialized with text encoding support")
```

#### Social Learning and Observation

```python
import pynlm

# Create two agents for social learning simulation
brain1 = pynlm.createBrain(pynlm.createDefaultConfig())
brain1.initialize()

brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
brain2.initialize()

# Create social learning environment
agent1 = pynlm.createAgentBrain(brain1)
agent2 = pynlm.createAgentBrain(brain2)

# Enable social learning for observation and imitation
agent1.enableDevelopment(True)
agent2.enableDevelopment(True)

# Configure social learning parameters
config.set("social_observation_capacity", 500)
config.set("social_forgetting_rate", 0.05)

# Run social learning simulation
for step in range(1000):
    # Agent 1 observes Agent 2's behavior
    if step % 100 == 0:
        # Observe other agent's actions
        percept = world.getSensoryPercept()
        agent2.observeOtherAgent(agent2.decodeMotorCommand(), percept.getVision(), percept.getTouch())
    
    # Both agents learn from the shared environment
    percept = world.getSensoryPercept()
    agent1.processSensoryInput(percept)
    agent2.processSensoryInput(percept)
    
    brain1.step(step)
    brain2.step(step)
    
    if step % 200 == 0:
        print(f"Social learning simulation step {step}")
        print(f"  Agent 1 concepts: {agent1.getConceptCount()}")
        print(f"  Agent 2 concepts: {agent2.getConceptCount()}")
        print(f"  Social observations: {agent2.getSocialObservationCount()}")
```

#### Memory System Configuration and Usage

```python
import pynlm

# Advanced memory system configuration
config = pynlm.createDefaultConfig()

# Configure working memory (transient activity)
config.set("working_memory_capacity", 100)
config.set("working_memory_decay_rate", 0.98)

# Configure episodic memory (experience storage)
config.set("episodic_memory_capacity", 500)
config.set("episodic_memory_decay_rate", 0.95)

# Configure associative memory (pattern associations)
config.set("concept_min_instances", 10)
config.set("concept_similarity_threshold", 0.9)

brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run extended simulation to build memory systems
for step in range(2000):
    # Update world state
    world.update(0.1)
    
    # Process sensory input to populate memory systems
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Run brain computation
    brain.step(step)
    
    # Apply reward and update memory
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    agent.updateDevelopment(0.1)
    
    if step % 500 == 0:
        working_mem = brain.getWorkingMemory()
        episodic_mem = brain.getEpisodicMemory()
        concept_former = brain.getConceptFormation()
        
        print(f"Memory system stats (step {step}):")
        print(f"  Working memory patterns: {working_mem.getPatternCount() if working_mem else 0}")
        print(f"  Episodic memory episodes: {episodic_mem.getEpisodeCount() if episodic_mem else 0}")
        print(f"  Concept formation categories: {concept_former.getConceptCount() if concept_former else 0}")
```

#### Prediction System and Planning

```python
import pynlm

# Configure advanced prediction systems
config = pynlm.createDefaultConfig()
config.set("prediction_horizon", 50)
config.set("prediction_accuracy_threshold", 0.8)

brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable prediction system for forward planning
prediction_system = brain.getPredictionSystem()
planner = brain.getPlanner()

print("Advanced prediction and planning system initialized")
print("  Prediction horizon:", prediction_system.getPredictionHorizon())
print("  Planner action quality samples:")

# Run planning simulation
for step in range(100):
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Get current brain state
    brain.step(step)
    
    # Get planned action from neural planner
    planned_action = planner.planAction(percept.getVision())
    decoded_action = agent.decodeMotorCommand()
    
    if step % 20 == 0:
        # Get action quality from planner
        action_quality = planner.getActionQuality(planned_action)
        print(f"    Step {step}: Planned quality {action_quality:.3f}")
        
        # Compare planned vs decoded
        if planned_action != decoded_action:
            print(f"      Planner override: {planned_action} vs decoded {decoded_action}")
```

#### Performance Monitoring and Optimization

```python
import pynlm
import time

def monitor_performance(brain, agent, duration_steps=1000):
    """Monitor brain performance and resource usage during simulation."""
    
    start_time = time.time()
    
    performance_data = {
        'spike_counts': [],
        'firing_rates': [],
        'step_times': []
    }
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print("Starting performance monitoring...")
    print(f"  Monitoring for {duration_steps} simulation steps")
    
    for step in range(duration_steps):
        step_start = time.time()
        
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Collect performance metrics
        performance_data['spike_counts'].append(brain.getTotalSpikeCount())
        performance_data['firing_rates'].append(brain.getAverageFiringRate())
        performance_data['step_times'].append(time.time() - step_start)
        
        if step % 200 == 0:
            avg_step_time = sum(performance_data['step_times'][-200:]) / 200
            avg_firing_rate = sum(performance_data['firing_rates'][-200:]) / 200
            print(f"    Step {step}: Avg step time {avg_step_time*1000:.1f}ms, "
                  f"Avg firing rate {avg_firing_rate:.1f} Hz")
    
    end_time = time.time()
    
    # Calculate performance statistics
    total_time = end_time - start_time
    avg_step_time = total_time / duration_steps
    avg_firing_rate = sum(performance_data['firing_rates']) / len(performance_data['firing_rates'])
    
    print(f"\nPerformance Summary:")
    print(f"  Total simulation time: {total_time:.1f}s")
    print(f"  Average step time: {avg_step_time*1000:.1f}ms")
    print(f"  Average firing rate: {avg_firing_rate:.1f} Hz")
    print(f"  Steps per second: {duration_steps / total_time:.1f}")
    
    return performance_data

# Run performance monitoring
performance_data = monitor_performance(brain, agent, 1000)
```

#### Large-Scale Simulation with Structural Plasticity

```python
import pynlm

def run_large_scale_simulation(neuron_count=5000, structural_plasticity=True):
    """Run a large-scale simulation with structural plasticity."""
    
    # Configure large-scale simulation
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.region_count", 4)
    config.set("brain.connection_probability", 0.08)
    
    # Enable structural plasticity for network growth
    config.set("plasticity.structural.enable", structural_plasticity)
    config.set("development_synaptogenesis_rate", 0.0005)
    config.set("development_pruning_rate", 0.00005)
    
    # Configure advanced memory systems for scale
    config.set("working_memory_capacity", 200)
    config.set("episodic_memory_capacity", 1000)
    config.set("concept_min_instances", 20)
    
    print(f"Starting large-scale simulation with {neuron_count} neurons")
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Create complex world for large-scale simulation
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=16, visionHeight=16)
    world.reset()
    world.setAgentStart(25.0, 25.0)
    agent.initialize(world)
    
    # Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(structural_plasticity)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run extended large-scale simulation
    total_steps = 5000
    for step in range(total_steps):
        world.update(0.05)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        agent.updateDevelopment(0.05)
        
        if step % 1000 == 0:
            print(f"\nLarge-scale simulation progress (step {step}):")
            print(f"  Total neurons: {brain.getTotalNeuronCount()}")
            print(f"  Total synapses: {brain.getTotalSynapseCount()}")
            print(f"  Active neurons: {brain.getActiveNeuronCount()}")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    
    print(f"\nLarge-scale simulation completed successfully!")
    print(f"Final statistics:")
    print(f"  - Total spikes generated: {brain.getTotalSpikeCount()}")
    print(f"  - Average firing rate: {brain.getAverageFiringRate():.1f} Hz")
    
    return brain, agent, world

# Run large-scale simulation
large_brain, large_agent, large_world = run_large_scale_simulation(5000, True)
```

---

## Advanced Commands and Configuration

### Configuration File Format

Advanced configuration uses the following format in `.nlm` or `.cfg` files:

```ini
# Core brain configuration
brain.neuron_count = 5000
brain.region_count = 4
brain.connection_probability = 0.08
brain.v_thresh = -50.0
brain.v_rest = -70.0
brain.tau_mem = 20.0
brain.tau_ref = 5.0

# Advanced Phase 6 configuration
text_encoder.max_tokens = 500
text_embedding.dimension = 64

social.observation.capacity = 1000
social.forgetting.rate = 0.02

memory.working.capacity = 200
memory.working.decay = 0.95
memory.episodic.capacity = 1000
memory.episodic.decay = 0.98

concept.min_instances = 20
concept.similarity.threshold = 0.9

prediction.horizon = 30
prediction.accuracy.threshold = 0.8

curiosity.exploration.bonus = 0.7
curiosity.decay.rate = 0.99

novelty.detection.threshold = 0.25
novelty.weighting = 1.2

development.plasticity.decay = 0.9
development.critical.period.duration = 300.0
development.maturation.threshold = 600.0

social.influence.window = 100
social.influence.decay = 0.85

thought.trace.capacity = 500
thought.trace.decay = 0.9
internal.firing.rate.smoothing = 0.05

checkpoint.interval = 2000
checkpoint.history.size = 10
```

### Command-Line Interface Extensions

Advanced usage through command-line arguments:

```bash
# Build with Python bindings and Phase 6 features
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DNLM_BUILD_PYTHON_BINDINGS=ON \
         -DNLM_ENABLE_PHASE6=ON \
         -DNLM_ENABLE_SOCIAL_LEARNING=ON \
         -DNLM_ENABLE_TEXT_PROCESSING=ON

make -j$(nproc)

# Run Phase 6 demo with custom configuration
./nlm_phase6_demo \
  --config custom_phase6_config.json \
  --neurons 10000 \
  --regions 8 \
  --steps 5000 \
  --world-width 100 \
  --world-height 100 \
  --enable-text \
  --enable-social \
  --enable-concepts \
  --enable-memory
```

### Advanced Python API Extensions

Extend the Python API with custom classes and methods:

```python
import pynlm

class AdvancedConfig(pynlm.Config):
    def __init__(self):
        super().__init__()
        self._validate_config()
    
    def _validate_config(self):
        """Validate configuration parameters."""
        # Validate neural parameters
        neuron_count = self.getOr<int64_t>("brain.neuron_count", 1000)
        if neuron_count < 100 or neuron_count > 100000:
            raise ValueError(f"Invalid neuron count: {neuron_count}")
        
        # Validate plasticity parameters
        if self.has("plasticity.stdp.learning_rate"):
            rate = self.getOr<float>("plasticity.stdp.learning_rate", 0.001)
            if rate < 0.0 or rate > 1.0:
                raise ValueError(f"Invalid STDP learning rate: {rate}")
    
    def configure_for_performance(self, neurons, mode="balanced"):
        """Configure for specific performance modes."""
        
        if mode == "fast":
            self.set("brain.neuron_count", neurons)
            self.set("brain.region_count", max(2, neurons // 2000))
            self.set("connection_probability", 0.05)
            self.set("simulation.max.steps", 1000)
        
        elif mode == "balanced":
            self.set("brain.neuron_count", neurons)
            self.set("brain.region_count", 4)
            self.set("connection_probability", 0.1)
            self.set("simulation.max.steps", 5000)
    
    def configure_for_phase6(self, enable_text=True, enable_social=True, 
                           enable_memory=True, enable_prediction=True):
        """Configure specifically for Phase 6 features."""
        
        # Advanced text encoding
        if enable_text:
            self.set("text_encoder.max_tokens", 500)
            self.set("text_embedding.dimension", 64)
        
        # Social learning
        if enable_social:
            self.set("social.observation.capacity", 500)
            self.set("social.forgetting.rate", 0.02)

# Use advanced configuration
advanced_config = AdvancedConfig()
advanced_config.configure_for_performance(5000, mode="balanced")
advanced_config.configure_for_phase6(enable_text=True, enable_social=True)

brain = pynlm.createBrain(advanced_config)
brain.initialize()
```

---

## Best Practices and Tips

### Configuration Best Practices

1. **Start with defaults, then customize**
   - Always start with `createDefaultConfig()`
   - Modify specific parameters for your use case
   - Test each change independently

2. **Validation before use**
   - Always validate critical parameters before running
   - Check neuron count vs. available memory
   - Ensure region count matches network topology needs

3. **Performance tuning**
   - Use smaller neuron counts for prototyping
   - Enable only necessary features to reduce overhead
   - Monitor memory usage during large simulations

### Simulation Best Practices

1. **Initialization sequence**
   ```python
   config = pynlm.createDefaultConfig()
   brain = pynlm.createBrain(config)
   brain.initialize()  # CRITICAL - must be called first
   agent = pynlm.createAgentBrain(brain)
   agent.initialize(world)
   ```

2. **Error handling**
   ```python
   try:
       world.update(0.1)
       percept = world.getSensoryPercept()
       agent.processSensoryInput(percept)
       brain.step(step)
   except Exception as e:
       print(f"Error at step {step}: {e}")
       break
   ```

3. **Progress monitoring**
   ```python
   if step % 100 == 0:
       print(f"Step {step}:")
       print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
       print(f"  Average firing rate: {brain.getAverageFiringRate():.1f} Hz")
   ```

### Advanced Tips

1. **Memory management**
   - Use checkpointing for long simulations
   - Monitor memory usage regularly
   - Consider smaller configurations for testing

2. **Feature combinations**
   - Enable curiosity and development together for exploration
   - Combine reward modulation with structural plasticity for adaptation
   - Use social learning with observation capabilities

3. **Performance optimization**
   - Profile your simulation to find bottlenecks
   - Adjust timesteps based on your use case
   - Consider using optimized configuration presets

---

## Conclusion

You now have a comprehensive understanding of NLM usage, from basic simulations to advanced Phase 6 integrations. The key takeaways are:

1. **Start simple, then extend**: Begin with basic configurations and gradually add complexity
2. **Always validate**: Check configuration parameters before running simulations
3. **Monitor performance**: Keep track of firing rates, memory usage, and simulation progress
4. **Use error handling**: Implement robust error handling for production systems
5. **Leverage Phase 6 features**: Take advantage of advanced memory, prediction, and social learning capabilities

The NLM framework provides powerful tools for neural simulation, cognitive modeling, and agent-based learning. Whether you're prototyping small networks or running large-scale simulations, the configuration system and Python API provide flexibility and extensibility for your specific needs.

---

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
