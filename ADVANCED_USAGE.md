# NLM - Advanced Usage Guide for Experienced Users

## What Makes NLM Special

NLM (Neural Learning Machine) is an experimental artificial developmental brain system that goes beyond traditional AI approaches. Unlike transformers or LLMs, NLM:

- **Develops** like a brain, starting primitive and becoming more complex
- **Learns** through experience, not pre-trained patterns
- **Integrates** multiple brain systems (memory, neuromodulation, prediction, cognition)
- **Adapts** its own neural architecture through structural plasticity

## Advanced Python API Usage

### 1. Configuration Deep Dive

The Config class is more powerful than it appears at first glance. Here are advanced techniques:

```python
import pynlm

# Create and customize config
config = pynlm.createDefaultConfig()

# Use config methods for dynamic configuration
config.set("neuron_count", 2000)  # More neurons = more capacity
config.set("region_count", 4)     # More regions = better specialization
config.set("connection_probability", 0.2)  # Sparse connectivity for efficiency

# Enable all learning systems for maximum development
config.set("enable_stdp", True)
config.set("enable_hebbian", True)
config.set("enable_structural_plasticity", True)
config.set("enable_development", True)

# Save your custom configuration
config.saveToFile("my_custom_config.json")
```

### 2. Brain Configuration Patterns

```python
def create_advanced_brain(neuron_multiplier=1.0, regions=4):
    config = pynlm.createDefaultConfig()
    
    # Scale brain size
    base_neurons = config.get("neuron_count", 1000)
    config.set("neuron_count", int(base_neurons * neuron_multiplier))
    config.set("region_count", regions)
    
    # Set up for specific learning goals
    if neuron_multiplier > 2.0:
        # Large brain needs more structure
        config.set("connection_probability", 0.05)
        config.set("stdp_ltp_weight", 0.005)
    
    return pynlm.createBrain(config)

# Create different specialized brains
working_memory_brain = create_advanced_brain(0.5, 2)   # Focused, efficient
creative_brain = create_advanced_brain(3.0, 8)         # Large, diverse
balanced_brain = create_advanced_brain(1.0, 4)        # General purpose
```

### 3. Advanced Agent Development

```python
import pynlm
import math

def create_development_agent(config):
    """Create an agent with specific developmental trajectory"""
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    
    # Initialize with world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    agent.initialize(world)
    
    # Enable all neuromodulation systems
    agent.enableRewardModulation(True)   # Dopamine-based learning
    agent.enableStructuralPlasticity(True)  # Grow new connections
    agent.enableDevelopment(True)       # Brain matures over time
    agent.enableCuriosity(True)         # Explore novel situations
    
    return brain, agent, world

def developmental_learning_loop(agent, world, steps):
    """Advanced learning with developmental stages"""
    for step in range(steps):
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Track developmental stage
        stage = agent.getDevelopmentalStage()
        stage_multiplier = get_stage_multiplier(stage)
        
        # Process with appropriate sensitivity
        agent.processSensoryInput(percept)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply developmental effects
        if stage == pynlm.DevelopmentalStage.Maturation:
            # Critical period for learning
            if step % 10 == 0:
                agent.enableCuriosity(True)
        
        # Log developmental progress
        if step % 100 == 0:
            print(f"Stage {stage.value}: Curiosity={agent.getCuriosityLevel():.2f}")
        
        return stage_multiplier

def get_stage_multiplier(stage):
    """Get learning rate multiplier based on developmental stage"""
    multipliers = {
        pynlm.DevelopmentalStage.Initial: 1.0,
        pynlm.DevelopmentalStage.CriticalPeriod: 2.0,
        pynlm.DevelopmentalStage.Maturation: 1.5,
        pynlm.DevelopmentalStage.Adult: 0.8,
        pynlm.DevelopmentalStage.Aging: 0.5
    }
    return multipliers.get(stage, 1.0)
```

### 4. Memory Systems Integration

```python
def advanced_memory_agent(brain, world):
    """Create agent that leverages all memory systems"""
    agent = pynlm.createAgentBrain(brain)
    
    # Enable all memory systems
    agent.enableWorkingMemory(True)      # Neural working memory
    agent.enableEpisodicMemory(True)     # Store experiences
    agent.enableSemanticMemory(True)     # Abstract knowledge
    agent.enableProceduralMemory(True)   # Motor skills
    
    # Custom memory-based decision making
    def memory_aware_action(percept, agent, brain):
        """Action selection using memory systems"""
        # Get related memories
        similar_episodes = agent.getEpisodicMemory().find_similar(percept)
        procedural_skills = agent.getProceduralMemory().get_relevant_skills(percept)
        
        if procedural_skills:
            # Use learned motor patterns
            return agent.get_motor_from_memory(procedural_skills[0])
        elif similar_episodes:
            # Use past experience
            return similar_episodes[0].predicted_action
        else:
            # Explore
            return pynlm.MotorCommand.MoveForward
    
    return agent
```

### 5. Neuromodulation Control

```python
def neuromodulation_research(agent, world, trials):
    """Research neuromodulation effects"""
    results = {
        'reward_modulation': [],
        'curiosity_levels': [],
        'novelty_sensitivity': []
    }
    
    for trial in range(trials):
        # Create novel environment each trial
        world.reset()
        
        # Track baseline neuromodulation
        baseline_dopamine = agent.getNeuromodulationLevel()
        baseline_curiosity = agent.getCuriosityLevel()
        
        # Run episode
        for step in range(100):
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Introduce novelty periodically
            if step % 20 == 0:
                world.add_novel_object()
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Record effects
        results['reward_modulation'].append(agent.getNeuromodulationLevel())
        results['curiosity_levels'].append(agent.getCuriosityLevel())
        results['novelty_sensitivity'].append(agent.getNoveltyLevel())
    
    return results
```

### 6. Performance Monitoring

```python
class NLMAnalytics:
    """Track and analyze NLM brain performance"""
    
    def __init__(self):
        self.spike_history = []
        self.firing_rate_history = []
        self.action_distribution = {}
        self.memory_growth = []
    
    def track_brain_performance(self, brain):
        """Track key performance metrics"""
        self.spike_history.append(brain.getTotalSpikeCount())
        self.firing_rate_history.append(brain.getAverageFiringRate())
        
        # Calculate derived metrics
        if len(self.spike_history) > 10:
            spike_rate_change = (self.spike_history[-1] - self.spike_history[-10]) / 10
            self.log_performance(spike_rate_change)
    
    def analyze_stability(self):
        """Analyze brain stability over time"""
        if len(self.firing_rate_history) < 20:
            return "unstable"
        
        recent_rates = self.firing_rate_history[-20:]
        rate_variance = max(recent_rates) - min(recent_rates)
        
        if rate_variance < 0.1:
            return "stable"
        elif rate_variance < 0.5:
            return "fluctuating"
        else:
            return "unstable"
```

### 7. Checkpointing and State Management

```python
def advanced_checkpoint_manager(agent, brain, world, base_path="nlm_checkpoints"):
    """Advanced checkpoint system with compression and validation"""
    
    def save_agent_state(step, performance_metrics=None):
        """Save agent with metadata"""
        timestamp = int(time.time())
        filename = f"{base_path}/agent_step_{step}_{timestamp}.pynlm"
        
        # Save state
        agent.save(filename)
        
        # Save metadata
        metadata = {
            'step': step,
            'timestamp': timestamp,
            'performance': performance_metrics or {},
            'neuromodulation_level': agent.getNeuromodulationLevel(),
            'developmental_stage': agent.getDevelopmentalStage().value,
            'curiosity_level': agent.getCuriosityLevel()
        }
        
        with open(f"{filename}.meta", 'w') as f:
            json.dump(metadata, f)
        
        return filename
    
    def load_best_agent(performance_metric='reward', min_episodes=10):
        """Load agent with best performance"""
        checkpoints = find_checkpoints(base_path)
        best_checkpoint = None
        best_score = -float('inf')
        
        for checkpoint in checkpoints:
            metadata = load_metadata(checkpoint)
            if len(metadata['performance']) >= min_episodes:
                score = calculate_performance_score(metadata['performance'], performance_metric)
                if score > best_score:
                    best_score = score
                    best_checkpoint = checkpoint
        
        if best_checkpoint:
            return agent.load(best_checkpoint)
        return None
    
    return save_agent_state, load_best_agent
```

### 8. Batch Processing and Parallelism

```python
def parallel_simulation_runner(configs, world_template, episodes_per_config=100):
    """Run multiple simulations in parallel for research"""
    
    def run_single_simulation(config_idx):
        """Run one simulation"""
        config = configs[config_idx]
        
        # Create fresh brain for each simulation
        brain = pynlm.createBrain(config)
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(**world_template)
        
        # Track performance
        results = {
            'config_id': config_idx,
            'total_reward': 0,
            'final_firing_rate': 0,
            'memory_episodes': 0,
            'episodes': []
        }
        
        for episode in range(episodes_per_config):
            world.reset()
            episode_reward = 0
            
            for step in range(100):
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Get reward
                reward = world.get_reward_for_action(action)
                episode_reward += reward
            
            results['episodes'].append(episode_reward)
            results['total_reward'] += episode_reward
        
        results['final_firing_rate'] = brain.getAverageFiringRate()
        results['memory_episodes'] = agent.get_memory_count()
        
        return results
    
    # Run simulations (in parallel with proper process management)
    import multiprocessing
    with multiprocessing.Pool() as pool:
        results = pool.map(run_single_simulation, range(len(configs)))
    
    return results
```

### 9. Custom Neural Architectures

```python
def create_specialized_architecture(config, architecture_type='modular'):
    """Create brain with specialized regional architecture"""
    
    if architecture_type == 'modular':
        # Create specialized regions
        brain = pynlm.createBrain(config)
        
        # Define regions for different functions
        regions = {
            'sensory': {'neuron_count': 200, 'type': 'input'},
            'motor': {'neuron_count': 200, 'type': 'output'},
            'memory': {'neuron_count': 300, 'type': 'working'},
            'cognition': {'neuron_count': 300, 'type': 'executive'},
            'emotional': {'neuron_count': 100, 'type': 'affective'}
        }
        
        # Clear default regions and create specialized ones
        brain.clear_regions()
        
        for region_name, spec in regions.items():
            region_config = config.copy()
            region_config.set('neuron_count', spec['neuron_count'])
            brain.add_region(region_name, region_config)
        
        return brain
    
    elif architecture_type == 'hierarchical':
        # Create hierarchical architecture
        brain = pynlm.createBrain(config)
        
        # Create executive hierarchy
        executive_config = config.copy()
        executive_config.set('neuron_count', config.get('neuron_count', 1000) // 4)
        executive_region = brain.add_region('executive', executive_config)
        
        # Create subsystems
        subsystems = ['sensory', 'motor', 'working_memory']
        for subsystem in subsystems:
            sub_config = config.copy()
            sub_config.set('neuron_count', config.get('neuron_count', 1000) // 6)
            sub_region = brain.add_region(subsystem, sub_config)
            # Connect subsystem to executive
            brain.connect_regions(executive_region, sub_region)
        
        return brain
    
    return brain
```

## Troubleshooting Advanced Issues

### Common Problems and Solutions:

1. **Memory Usage Too High**
```python
# Solution: Enable memory management
agent.enableWorkingMemory(True)
agent.enableEpisodicMemory(True)

# Clear old memories if needed
agent.clearOldMemories(max_age=1000)
```

2. **Learning Rate Too Slow**
```python
# Solution: Adjust neuromodulation
agent.enableRewardModulation(True)
agent.set_neuromodulation_strength(0.5)
```

3. **Action Selection Inconsistent**
```python
# Solution: Stabilize with attention
agent.enableAttention(True)
agent.set_attention_window(50)
```

## Next Steps for Research

1. **Read**: `docs/ARCHITECTURE.md` for deep technical details
2. **Explore**: Phase 6 integration experiment for complete system behavior
3. **Experiment**: Modify configuration files to discover emergent behaviors
4. **Contribute**: Create custom agents for your research problems

NLM is not just a brain simulator—it's a research platform for investigating how neural systems develop, learn, and adapt. The Python bindings give you programmatic control over this complex system while maintaining the simplicity that makes it accessible for experimentation.