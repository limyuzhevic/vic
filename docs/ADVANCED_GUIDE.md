# NLM Advanced User Guide

## Overview for Advanced Users

This guide assumes familiarity with basic NLM usage and provides in-depth guidance for power users, researchers, and developers working with the Neural Learning Machine system.

## Advanced Configuration Management

### Custom Configuration Templates

This section provides templates for creating specialized NLM configurations:

#### High-Throughput Configuration
```cpp
// File: configs/high_throughput.cfg
# Optimized for speed over accuracy
random_seed = 42
simulation_timestep = 0.0005
neuron_count = 2000
region_count = 4
connection_probability = 0.08
plasticity_learning_rate = 0.005
stdp_ltp_weight = 0.015
stdp_ltd_weight = 0.012
synaptogenesis_rate = 0.0002nln pruning_rate = 0.00002nln neuromodulation.dopamine_baseline = 0.15
reward_discount_factor = 0.995nln visualization_enabled = true
visualization_update_rate = 60.0
```

#### Developmental Configuration
```cpp
// File: configs/developmental.cfg
# Optimized for developmental learning
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1500
region_count = 3
connection_probability = 0.12
plasticity_learning_rate = 0.02
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.018
neuromodulation.dopamine_baseline = 0.05
neuromodulation.curiosity.enable = true
neuromodulation.novelty.enable = true
neuromodulation.prediction_error.enable = true
```

#### Predictive Accuracy Configuration
```cpp
// File: configs/prediction.cfg
# Optimized for prediction accuracy
random_seed = 42
simulation_timestep = 0.002
neuron_count = 1200
region_count = 2
connection_probability = 0.15
prediction.horizon = 5
prediction.encoding = "temporal"
working_memory.capacity = 200
working_memory.decay_rate = 0.01
predictive_planning.enabled = true
predictive_planning.horizon = 10
```

### Runtime Configuration Modification

NLM allows dynamic configuration changes during simulation:

```python
import pynlm

# Create brain with initial config
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Modify configuration during simulation
config.set("neuromodulation.curiosity.level", 0.8)
config.set("plasticity.hebbian.learning_rate", 0.015)

# Apply changes affect future plasticity
agent.enableCuriosity(True)
agent.enableRewardModulation(True)

# Restart simulation with new config
brain.reset()
brain.initialize()
```

## Advanced Experiment Design

### Designing Multi-Agent Experiments

#### Coexistence Experiment
```cpp
// Complete multi-agent coexistence setup
void setupCoexistenceExperiment() {
    // Create multiple brains
    auto brain1 = createBrain(config1);
    auto brain2 = createBrain(config2);
    auto brain3 = createBrain(config3);
    
    // Create separate worlds
    SimpleWorld world1, world2, world3;
    world1.initialize(20, 20, 8, 8);
    world2.initialize(20, 20, 8, 8);
    world3.initialize(20, 20, 8, 8);
    
    // Create agents
    AgentBrain agent1(brain1), agent2(brain2), agent3(brain3);
    agent1.initialize(world1);
    agent2.initialize(world2);
    agent3.initialize(world3);
    
    // Enable different capabilities
    agent1.enableCuriosity(true);
    agent2.enableStructuralPlasticity(true);
    agent3.enableDevelopment(true);
    
    // Run separate simulations
    runSimulation(agent1, world1);
    runSimulation(agent2, world2);
    runSimulation(agent3, world3);
}
```

### Long-Duration Experiments

For experiments requiring extended simulation time:

```python
def longDurationExperiment(brain, world, agent, total_steps):
    """Run experiment with checkpointing and monitoring"""
    config = pynlm.createDefaultConfig()
    config.set("simulation_time_limit", 3600.0)  # 1 hour max
    
    checkpoint_interval = 10000
    last_checkpoint = 0
    metrics_log = []
    
    for step in range(total_steps):
        # Save periodic checkpoints
        if step % checkpoint_interval == 0 and step > last_checkpoint:
            brain.save(f"checkpoint_step_{step}.bin")
            last_checkpoint = step
        
        # Run simulation step
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Log metrics periodically
        if step % 1000 == 0:
            metrics = {
                'step': step,
                'firing_rate': brain.getAverageFiringRate(),
                'neurons_firing': brain.getFiringNeuronCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'development_stage': brain.getDevelopmentalStage()
            }
            metrics_log.append(metrics)
            print(f"Step {step}: {metrics}")
    
    return metrics_log
```

## Performance Optimization

### Memory Management Strategies

For large-scale simulations:

```cpp
// Use memory-efficient neuron configurations
struct OptimizedNeuronConfig {
    size_t neuron_count;    // 1000-5000 typical
    float sparsity;        // 0.05-0.15 (5-15% connected)
    bool prune_unused;     // Enable synapse pruning
    size_t max_synapses;   // Limit per neuron
};

// Pre-allocate memory pools
class MemoryPool {
public:
    void* allocate(size_t size);
    void deallocate(void* ptr);
    void reset(); // Clear pool for next simulation
};

// Use deterministic PRNG for reproducibility
class DeterministicRNG {
    uint64_t seed;
    std::mt19937_64 engine;
    
    DeterministicRNG(uint64_t s = 42) : seed(s), engine(s) {}
    float uniformReal(float min, float max);
    uint64_t uniformInt(uint64_t min, uint64_t max);
};
```

### Performance Monitoring

Track simulation performance in real-time:

```python
class PerformanceMonitor:
    def __init__(self):
        self.step_times = []
        self.memory_usage = []
        self.firing_rates = []
        self.start_time = time.time()
    
    def step(self, step):
        step_start = time.time()
        # Your simulation code here
        step_end = time.time()
        
        self.step_times.append(step_end - step_start)
        self.firing_rates.append(brain.getAverageFiringRate())
        
        # Print performance summary
        if step % 100 == 0:
            avg_step_time = sum(self.step_times[-100:]) / 100
            print(f"Step {step}: {avg_step_time*1000:.2f}ms/step, "
                  f"Firing rate: {self.firing_rates[-1]:.2f}")
    
    def get_performance_report(self):
        total_time = time.time() - self.start_time
        avg_step_time = sum(self.step_times) / len(self.step_times) if self.step_times else 0
        
        return {
            'total_steps': len(self.step_times),
            'total_time': total_time,
            'steps_per_second': len(self.step_times) / total_time,
            'average_step_time': avg_step_time,
            'firing_rate_stats': {
                'mean': np.mean(self.firing_rates),
                'std': np.std(self.firing_rates),
                'min': np.min(self.firing_rates),
                'max': np.max(self.firing_rates)
            }
        }
```

## Advanced API Usage

### Custom Agent Brain Implementation

Extend the basic AgentBrain for specialized applications:

```cpp
class CustomAgentBrain : public AgentBrain {
public:
    CustomAgentBrain(std::shared_ptr<Brain> brain) 
        : AgentBrain(brain), customStrategy_(StrategyType::Adaptive) {}
    
    virtual MotorCommand decodeMotorCommand() override {
        MotorCommand cmd = decodeFromMotorNeurons();
        
        // Apply custom strategy
        switch (customStrategy_) {
            case StrategyType::Adaptive:
                cmd = applyAdaptiveStrategy(cmd);
                break;
            case StrategyType::Exploitive:
                cmd = applyExploitiveStrategy(cmd);
                break;
            case StrategyType::Random:
                cmd = applyRandomStrategy(cmd);
                break;
        }
        
        // Apply custom modifiers
        cmd = applyCustomModifiers(cmd);
        return cmd;
    }
    
protected:
    virtual MotorCommand applyAdaptiveStrategy(MotorCommand baseCmd) {
        // Adaptive strategy based on environment
        float curiosity = getCuriosityLevel();
        float novelty = getNoveltyLevel();
        
        if (curiosity > 0.7) {
            // High curiosity = exploration
            return selectWithCuriosity(baseCmd);
        } else {
            // Low curiosity = exploitation
            return baseCmd;
        }
    }
    
    virtual MotorCommand applyExploitiveStrategy(MotorCommand baseCmd) {
        // Always choose strongest action
        auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
            // Calculate activity as in parent class
        };
        
        // Modify motor neuron activities
        // Implementation details...
        
        return decodeFromMotorNeurons();
    }
    
    MotorCommand applyRandomStrategy(MotorCommand baseCmd) {
        // Random action selection
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < 0.3f) {  // 30% random
            return static_cast<MotorCommand>(brain_->getRandomGenerator()->uniformInt(0, 6));
        }
        return baseCmd;
    }
    
    MotorCommand applyCustomModifiers(MotorCommand cmd) {
        // Apply custom modifiers based on context
        // Implementation details...
        return cmd;
    }
    
private:
    enum class StrategyType { Adaptive, Exploitive, Random };
    StrategyType customStrategy_;
};
```

### Custom World Implementation

Create specialized environments for research:

```python
class ResearchWorld:
    def __init__(self, width=20, height=20):
        self.width = width
        self.height = height
        self.objects = []
        self.dynamic_walls = []
        self.reward_zones = []
        
        # Custom sensory features
        self.vision_features = ["color", "texture", "shape"]
        self.internal_features = ["energy", "thirst", "fatigue"]
        self.proprioception_features = ["position", "orientation"]
    
    def addObject(self, obj_type, position, properties):
        """Add custom world objects"""
        self.objects.append({
            'type': obj_type,
            'position': position,
            'properties': properties
        })
    
    def getSensoryPercept(self):
        """Enhanced sensory input with custom features"""
        percept = SensoryPercept()
        
        # Custom vision processing
        percept.vision = self.getCustomVision()
        
        # Custom touch sensing
        percept.touch = self.getCustomTouch()
        
        # Custom internal signals
        percept.internal = self.getCustomInternal()
        
        # Custom proprioception
        percept.proprioception = self.getCustomProprioception()
        
        return percept
    
    def getCustomVision(self):
        """Extended vision with custom features"""
        vision = []
        for y in range(self.vision_height):
            for x in range(self.vision_width):
                feature_vector = []
                
                # Add custom features
                for feature in self.vision_features:
                    feature_vector.append(self.computeFeature(x, y, feature))
                
                vision.append(feature_vector)
        
        return vision
    
    def applyAction(self, agent, action):
        """Custom action application"""
        if action == ActionType.Interact:
            self.handleInteraction(agent)
        elif action == ActionType.Eat:
            self.handleEating(agent)
        elif action == ActionType.LookLeft:
            self.handleLooking(agent, -1, 0)
        elif action == ActionType.LookRight:
            self.handleLooking(agent, 1, 0)
        
        # Update world state
        self.updateWorldState()
    
    def computeFeature(self, x, y, feature):
        """Compute custom sensory features"""
        # Implementation details...
        return 0.0
```

## Debugging and Analysis Tools

### Advanced Debugging

Debug complex neural interactions:

```python
class BrainDebugger:
    def __init__(self, brain):
        self.brain = brain
        self.neuron_history = {}
        self.synapse_history = {}
        self.spike_patterns = {}
    
    def traceNeuronActivity(self, neuron_id, duration_steps):
        """Trace detailed activity of specific neuron"""
        activity = []
        
        for step in range(duration_steps):
            # Get neuron state
            neuron_state = self.getNeuronState(neuron_id, step)
            activity.append(neuron_state)
            
            # Record synaptic inputs
            if step % 10 == 0:
                self.recordSynapticInputs(neuron_id)
        
        return activity
    
    def analyzeSpikingPatterns(self, neuron_ids):
        """Analyze timing and patterns of specific neurons"""
        patterns = {}
        
        for neuron_id in neuron_ids:
            spikes = self.getSpikeTimes(neuron_id)
            if len(spikes) > 1:
                intervals = self.computeIntervals(spikes)
                patterns[neuron_id] = {
                    'spike_count': len(spikes),
                    'mean_interval': np.mean(intervals),
                    'interval_variance': np.var(intervals),
                    'pattern_type': self.classifyPattern(intervals)
                }
        
        return patterns
    
    def identifyProblematicConnections(self):
        """Identify synapses that might be causing issues"""
        problematic = []
        
        for region in self.brain.getRegions():
            for synapse in region.getSynapses():
                if self.isProblematicSynapse(synapse):
                    problematic.append(synapse)
        
        return problematic
    
    def isProblematicSynapse(self, synapse):
        """Check if synapse has problematic properties"""
        weight = synapse.getWeight()
        
        # Check for extreme weights
        if abs(weight) > 10.0:
            return True
        
        # Check for unstable connections
        if synapse.isUnstable():
            return True
        
        # Check for zero weight (effectively disconnected)
        if weight < 0.001:
            return True
        
        return False
```

### Visualization Tools

Create custom visualizations:

```python
class BrainVisualizer:
    def __init__(self, brain):
        self.brain = brain
        self.neurons = self.extractNeuronData()
        self.connections = self.extractConnectionData()
    
    def createBrainMap(self, output_file):
        """Create 2D brain connectivity map"""
        plt.figure(figsize=(12, 10))
        
        # Plot neurons as points
        for neuron in self.neurons:
            plt.scatter(neuron['x'], neuron['y'], 
                       c=self.getNeuronColor(neuron), 
                       s=self.getNeuronSize(neuron),
                       alpha=0.6)
        
        # Plot connections
        for conn in self.connections:
            plt.plot([conn['pre_x'], conn['post_x']],
                    [conn['pre_y'], conn['post_y']],
                    c=self.getConnectionColor(conn),
                    alpha=0.3, linewidth=0.5)
        
        plt.title('NLM Brain Connectivity Map')
        plt.savefig(output_file, dpi=150, bbox_inches='tight')
        plt.close()
    
    def createActivityTimeline(self, steps, output_file):
        """Create timeline of neural activity"""
        fig, axes = plt.subplots(2, 2, figsize=(15, 10))
        
        # Total spikes over time
        ax = axes[0, 0]
        spike_counts = self.getSpikeCounts(steps)
        ax.plot(spike_counts)
        ax.set_title('Total Spikes Over Time')
        ax.set_xlabel('Step')
        ax.set_ylabel('Spike Count')
        
        # Firing rate over time
        ax = axes[0, 1]
        firing_rates = self.getFiringRates(steps)
        ax.plot(firing_rates)
        ax.set_title('Average Firing Rate Over Time')
        ax.set_xlabel('Step')
        ax.set_ylabel('Firing Rate')
        
        # Neuron type distribution
        ax = axes[1, 0]
        type_counts = self.getNeuronTypeCounts()
        ax.bar(type_counts.keys(), type_counts.values())
        ax.set_title('Neuron Type Distribution')
        ax.set_ylabel('Count')
        
        # Connectivity strength
        ax = axes[1, 1]
        conn_strengths = self.getConnectionStrengths()
        ax.hist(conn_strengths, bins=50)
        ax.set_title('Synapse Weight Distribution')
        ax.set_xlabel('Weight')
        ax.set_ylabel('Frequency')
        
        plt.tight_layout()
        plt.savefig(output_file, dpi=150, bbox_inches='tight')
        plt.close()
    
    def createDevelopmentTimeline(self, output_file):
        """Create timeline of brain development"""
        stages = [DevelopmentalStage.Initial,
                 DevelopmentalStage.CriticalPeriod,
                 DevelopmentalStage.Maturation,
                 DevelopmentalStage.Adult,
                 DevelopmentalStage.Aging]
        
        fig, ax = plt.subplots(figsize=(12, 6))
        
        # Development stage progression
        stage_times = []
        stage_plasticity = []
        stage_activity = []
        
        for stage in stages:
            stage_times.append(stage)
            stage_plasticity.append(self.getPlasticityForStage(stage))
            stage_activity.append(self.getActivityForStage(stage))
        
        x = range(len(stage_times))
        ax.plot(x, stage_plasticity, 'o-', label='Plasticity')
        ax.plot(x, stage_activity, 's-', label='Activity')
        
        ax.set_xticks(x)
        ax.set_xticklabels([str(s) for s in stage_times])
        ax.set_title('Brain Development Timeline')
        ax.set_ylabel('Level (0-1)')
        ax.legend()
        ax.grid(True, alpha=0.3)
        
        plt.tight_layout()
        plt.savefig(output_file, dpi=150, bbox_inches='tight')
        plt.close()
```

## Best Practices for Advanced Usage

### Memory Management Best Practices

1. **Use appropriate configuration sizes**:
   - Small simulations (<100 neurons): Development and testing
   - Medium simulations (100-1000 neurons): Research and demos
   - Large simulations (>1000 neurons): Production and research

2. **Implement checkpointing for long experiments**:
   ```python
   # Save checkpoints periodically
   def saveCheckpoint(step):
       brain.save(f"checkpoints/checkpoint_{step:06d}.bin")
       config.save(f"checkpoints/config_{step:06d}.txt")
   ```

3. **Monitor memory usage**:
   ```python
   import psutil
   process = psutil.Process()
   memory_mb = process.memory_info().rss / 1024 / 1024
   print(f"Memory usage: {memory_mb:.1f} MB")
   ```

### Performance Optimization Best Practices

1. **Use deterministic random seeds** for reproducible experiments:
   ```python
   config.set("random_seed", 42)
   brain.initialize()
   ```

2. **Limit simulation timesteps for testing**:
   ```python
   # For testing, use larger timestep
   config.set("simulation_timestep", 0.01)  # 10ms instead of 1ms
   ```

3. **Use appropriate neuron counts**:
   ```python
   # Start small and increase gradually
   config.set("neuron_count", 100)   # Test: 100 neurons
   config.set("neuron_count", 1000)  # Research: 1000 neurons
   config.set("neuron_count", 5000)  # Production: 5000 neurons
   ```

### Experiment Design Best Practices

1. **Isolate variables** in experiments:
   - Test one parameter at a time
   - Use control groups where possible
   - Repeat experiments with different random seeds

2. **Document all experimental parameters**:
   ```python
   experiment_params = {
       'neuron_count': 1000,
       'connection_probability': 0.1,
       'plasticity_learning_rate': 0.01,
       'random_seed': 42,
       'simulation_timestep': 0.001,
       'max_steps': 10000,
       'enable_curiosity': True,
       'enable_development': True,
       'enable_reward_modulation': True
   }
   ```

3. **Use statistical analysis**:
   - Run multiple trials with different seeds
   - Calculate means and standard deviations
   - Use appropriate statistical tests

## Troubleshooting Guide

### Common Issues and Solutions

#### Issue: Brain initialization fails
**Symptoms**: Brain initialization returns false or crashes
**Solutions**:

1. **Check configuration values**:
   ```python
   config = pynlm.createDefaultConfig()
   config.set("neuron_count", 100)  # Too small?
   config.set("region_count", 5)   # Too many regions?
   ```

2. **Increase memory allocation**:
   ```python
   # For very large networks
   import psutil
   process = psutil.Process()
   process.memory_info().rss  # Check available memory
   ```

3. **Fix configuration file**:
   ```bash
   # Remove duplicate values
   grep -n "dopamine_baseline" configs/default.cfg
   # Keep only one instance
   ```

#### Issue: Simulation runs too slowly
**Symptoms**: Very low steps per second
**Solutions**:

1. **Reduce neuron count**:
   ```python
   # From 5000 to 1000 neurons
   config.set("neuron_count", 1000)
   ```

2. **Increase simulation timestep**:
   ```python
   # From 0.001s to 0.01s
   config.set("simulation_timestep", 0.01)
   ```

3. **Disable expensive features**:
   ```python
   # Disable visualization
   config.set("visualization_enabled", False)
   
   # Disable development for speed
   agent.enableDevelopment(False)
   ```

#### Issue: Memory leaks in long simulations
**Symptoms**: Memory usage increases continuously
**Solutions**:

1. **Implement checkpoint cleanup**:
   ```python
   import shutil
   import glob
   
   # Clean up old checkpoints
   checkpoints = glob.glob("checkpoints/checkpoint_*.bin")
   if len(checkpoints) > 10:
       checkpoints.sort()  # Keep newest
       for old_checkpoint in checkpoints[:-10]:
           os.remove(old_checkpoint)
   ```

2. **Use garbage collection**:
   ```python
   import gc
   gc.collect()  # Force garbage collection
   ```

3. **Monitor memory usage**:
   ```python
   def monitor_memory():
       process = psutil.Process()
       memory_mb = process.memory_info().rss / 1024 / 1024
       print(f"Memory: {memory_mb:.1f} MB")
       
       if memory_mb > 1000:  # 1GB limit
           print("Warning: High memory usage!")
   ```

#### Issue: Neurons don't fire
**Symptoms**: No spikes generated, brain appears inactive
**Solutions**:

1. **Check current injection**:
   ```python
   # Ensure neurons receive input
   brain.injectCurrentToNeurons(NeuronType::Sensory, 10.0f)
   ```

2. **Verify neuron parameters**:
   ```python
   # Check neuron threshold
   for neuron in brain.getNeurons():
       if neuron.getMembranePotential() < neuron.getThreshold():
           print("Neuron below threshold")
   ```

3. **Check synaptic connections**:
   ```python
   # Verify neurons have synapses
   for neuron in brain.getNeurons():
       if neuron.getIncomingSynapseCount() == 0:
           print("Neuron has no incoming synapses")
   ```

#### Issue: Plasticity not working
**Symptoms**: Synaptic weights don't change over time
**Solutions**:

1. **Check plasticity settings**:
   ```python
   # Ensure plasticity is enabled
   agent.enableRewardModulation(True)
   agent.enableCuriosity(True)
   agent.enableDevelopment(True)
   ```

2. **Verify plasticity parameters**:
   ```python
   config.set("plasticity.hebbian.enable", True)
   config.set("plasticity.stdp.enable", True)
   config.set("plasticity.reward_modulated.enable", True)
   ```

3. **Check reward signals**:
   ```python
   # Ensure rewards are being generated
   reward = world.computeReward(agent.getBrain())
   print(f"Reward: {reward}")
   ```

### Getting Help

#### Online Resources

- **NLM Documentation**: https://github.com/yourusername/nlm/docs/
- **API Reference**: https://github.com/yourusername/nlm/api/
- **Tutorials**: https://github.com/yourusername/nlm/tutorials/

#### Community Support

- **GitHub Issues**: Report bugs and request features
- **Discussions**: Ask questions and share experiences
- **Contributing**: Contribute code and documentation

#### Technical Support

- **Email**: support@nlm.org
- **Slack**: Join our community channel
- **Discord**: Real-time help and discussions

## Future Enhancements

### Planned Features

1. **Advanced Visualization System**
   - 3D neural network visualization
   - Real-time activity monitoring
   - Multi-layer brain view

2. **Enhanced Python API**
   - Jupyter notebook integration
   - Advanced plotting tools
   - Machine learning integration

3. **Distributed Computing**
   - Multi-GPU support
   - Cluster computing
   - Cloud deployment

4. **Advanced Learning Algorithms**
   - Deep integration with spiking networks
   - Meta-learning capabilities
   - Transfer learning

5. **Enhanced Research Tools**
   - Experiment tracking
   - Statistical analysis
   - Data visualization

### Research Applications

NLM is suitable for various research applications:

#### Cognitive Neuroscience
- Study of neural coding
- Investigation of memory systems
- Analysis of attention mechanisms

#### Artificial Intelligence
- Development of intelligent agents
- Study of emergent behaviors
- Investigation of learning algorithms

#### Computational Neuroscience
- Testing neural hypotheses
- Development of brain models
- Study of neural dynamics

#### Psychology
- Study of learning and memory
- Investigation of decision-making
- Analysis of cognitive processes

### Integration Possibilities

NLM can be integrated with other systems:

#### Hardware
- Neuromorphic chips
- Brain-computer interfaces
- Neural implants

#### Software
- Machine learning frameworks
- Robotics control systems
- Virtual environments

#### Data
- Neuroscience datasets
- Behavioral data
- Physiological measurements

## Conclusion

This advanced user guide provides comprehensive information for working with NLM at a professional level. By following the best practices and using the advanced features described in this guide, you can effectively:

- Design and run complex experiments
- Optimize performance for large-scale simulations
- Debug and troubleshoot issues
- Analyze and visualize results
- Integrate NLM with other systems

The NLM framework offers a powerful platform for exploring artificial neural systems and understanding the principles of brain-like computation. With the tools and techniques described in this guide, you can push the boundaries of what's possible with spiking neural networks.

For questions, suggestions, or feedback, please visit our community resources or contact our technical support team. Happy experimenting!