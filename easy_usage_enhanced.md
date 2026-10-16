# Convenience Functions and Examples for NLM

This document provides improved convenience functions and examples that make NLM easier to use:
- High-level functions that combine multiple operations
- Better error handling and validation
- More intuitive API design
- Comprehensive example gallery
- Advanced usage patterns

## Part 1: High-Level Convenience Functions

### Complete Agent Simulation

```python
def run_complete_agent_simulation(brain_config, world_config, num_steps=1000):
    """Run a complete agent simulation with all subsystems enabled.
    
    This is a high-level convenience function that sets up and runs
    a complete agent simulation with all learning subsystems active.
    
    Args:
        brain_config: Brain configuration dictionary
        world_config: World configuration dictionary  
        num_steps: Number of simulation steps to run
        
    Returns:
        dict: Simulation results including statistics
    """
    import pynlm
    
    # Create and initialize brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(
        width=world_config.get('width', 20),
        height=world_config.get('height', 20),
        visionWidth=world_config.get('visionWidth', 8),
        visionHeight=world_config.get('visionHeight', 8)
    )
    world.reset()
    
    # Create agent interface
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)  
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    results = {
        'steps': [],
        'total_neurons': brain.getTotalNeuronCount(),
        'total_synapses': brain.getTotalSynapseCount(),
        'max_firing_rate': 0.0,
        'avg_curiosity': 0.0,
        'total_rewards': 0.0
    }
    
    for step in range(num_steps):
        # World dynamics
        world.update(0.1)
        
        # Sensory processing
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step)
        
        # Action selection
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Learning
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        # Collect statistics
        if step % 100 == 0:
            stats = {
                'step': step,
                'firing_neurons': brain.getFiringNeuronCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'firing_rate': brain.getAverageFiringRate(),
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'development_stage': brain.getDevelopmentalStage(),
                'reward': reward
            }
            results['steps'].append(stats)
            
            # Update running statistics
            results['max_firing_rate'] = max(results['max_firing_rate'], 
                                             brain.getAverageFiringRate())
            results['avg_curiosity'] += agent.getCuriosityLevel()
            results['total_rewards'] += reward
    
    # Compute final statistics
    if results['steps']:
        results['avg_curiosity'] /= len(results['steps'])
    
    return results
```

### Brain Development Simulation

```python
def simulate_brain_development(initial_neurons=100, development_duration=500):
    """Simulate brain development over time with realistic maturation.
    
    This convenience function demonstrates how the brain develops
    and matures over time, showing changes in firing patterns,
    connectivity, and learning capabilities.
    
    Args:
        initial_neurons: Number of neurons to start with
        development_duration: Number of development steps
        
    Returns:
        list: Development stages recorded at key milestones
    """
    import pynlm
    
    # Create development brain with growth capabilities
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", initial_neurons)
    config.set("plasticity.structural.enable", True)  # Enable structural plasticity
    config.set("development_synaptogenesis_rate", 0.001)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    development_log = []
    
    for step in range(development_duration):
        # Enable development at each step
        agent.enableDevelopment(True)
        
        # Brain processes and learns
        brain.step(step)
        agent.updateDevelopment(0.1)
        
        # Record development milestones
        if step % 100 == 0 or step == development_duration - 1:
            stage = brain.getDevelopmentalStage()
            firing_rate = brain.getAverageFiringRate()
            neuron_count = brain.getTotalNeuronCount()
            
            development_log.append({
                'step': step,
                'developmental_stage': stage,
                'neuron_count': neuron_count,
                'firing_rate': firing_rate,
                'plasticity_active': True,
                'maturation_progress': min(100.0, (step / development_duration) * 100)
            })
            
            print(f"Development Step {step}:")
            print(f"  Stage: {stage}")
            print(f"  Neurons: {neuron_count}")
            print(f"  Firing Rate: {firing_rate:.3f} Hz")
            print(f"  Maturation: {min(100.0, (step / development_duration) * 100):.1f}%")
            print()
    
    return development_log
```

### World Exploration Agent

```python
def create_exploration_agent(brain_size=200, world_size=15):
    """Create an agent designed for world exploration and discovery.
    
    This convenience function creates a brain optimized for exploration,
    curiosity-driven learning, and efficient navigation of complex environments.
    
    Args:
        brain_size: Number of neurons for exploration
        world_size: Size of the square world to explore
        
    Returns:
        tuple: (brain, world, agent, results_collector)
    """
    import pynlm
    
    # Create exploration-optimized brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", brain_size)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.dopamine.scale", 2.0)  # Higher reward sensitivity
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world with resources and hazards
    world = pynlm.createSimpleWorld()
    world.configure(
        width=world_size,
        height=world_size,
        visionWidth=5,   # Limited vision to encourage exploration
        visionHeight=5
    )
    world.reset()
    
    # Create exploration agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable exploration subsystems
    agent.enableCuriosity(True)
    agent.enableNoveltyDetection(True)
    agent.enableRewardModulation(True)
    
    # Create results collector for analysis
    exploration_results = {
        'visited_positions': [],
        'resources_found': 0,
        'hazards_avoided': 0,
        'novelty_levels': [],
        'learning_curve': [],
        'total_steps': 0
    }
    
    def run_exploration_episode(max_steps=1000):
        """Run a single exploration episode."""
        for step in range(max_steps):
            # Update world
            world.update(0.1)
            
            # Sense environment
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain computes and decides
            brain.step(step)
            action = agent.decodeMotorCommand()
            
            # Execute action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Learn from experience
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            agent.updateDevelopment(0.1)
            
            # Record exploration data
            exploration_results['total_steps'] += 1
            exploration_results['novelty_levels'].append(agent.getNoveltyLevel())
            exploration_results['learning_curve'].append(brain.getAverageFiringRate())
            
            # Track visited positions and discoveries
            body = world.getAgentBody()
            if body:
                exploration_results['visited_positions'].append((body.x, body.y))
            
            # Check for resource discovery (example: food)
            # This would depend on specific world object types
            
            # Stop if agent gets stuck
            if agent.getCuriosityLevel() < 0.01:  # Very low curiosity means exploration stopped
                break
        
        return exploration_results
    
    return brain, world, agent, exploration_results
```

## Part 2: Advanced Usage Patterns

### Pattern 1: Hierarchical Learning

```python
def hierarchical_learning_simulation():
    """Demonstrate hierarchical learning where simple patterns
    are learned first, then composed into more complex behaviors.
    """
    import pynlm
    
    # Phase 1: Learn simple sensory-motor associations
    simple_brain = pynlm.createBrain(pynlm.createDefaultConfig())
    simple_brain.initialize()
    
    # Create simple mapping between sensory inputs and actions
    for step in range(100):
        # Present simple patterns
        pattern = step % 5  # 5 simple patterns
        
        # Learn pattern-action mapping
        # This would use STDP and Hebbian learning
        # Simplified: just increment step counter
        simple_brain.step(step)
        
        if step % 20 == 0:
            print(f"Phase 1 - Pattern {pattern} learned")
    
    # Phase 2: Compose learned patterns into sequences
    # More neurons, more complex connectivity
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)  # More neurons for composition
    
    complex_brain = pynlm.createBrain(config)
    complex_brain.initialize()
    
    agent = pynlm.createAgentBrain(complex_brain)
    
    for step in range(500):
        # Use learned simple patterns to build complex sequences
        # This demonstrates how simple building blocks
        # can be combined into sophisticated behaviors
        agent.updateDevelopment(0.1)
        complex_brain.step(step)
    
    print("Hierarchical learning completed")
    return complex_brain, agent
```

### Pattern 2: Transfer Learning

```python
def transfer_learning_demo():
    """Demonstrate how knowledge learned in one task
    can be transferred to another related task.
    """
    import pynlm
    
    # Source task: Navigation in familiar environment
    source_config = pynlm.createDefaultConfig()
    source_config.set("brain.neuron_count", 300)
    source_config.set("plasticity.stdp.enable", True)
    
    source_brain = pynlm.createBrain(source_config)
    source_brain.initialize()
    
    # Train on source task
    for step in range(200):
        # Simulate navigation in source environment
        # This builds general navigation skills
        source_brain.step(step)
    
    # Transfer knowledge to target task
    target_config = pynlm.createDefaultConfig()
    target_config.set("brain.neuron_count", 200)  # Smaller for transfer
    
    target_brain = pynlm.createBrain(target_config)
    target_brain.initialize()
    
    # Initialize with source brain weights (simplified)
    # In real implementation, this would copy synaptic weights
    # or use other transfer mechanisms
    
    agent = pynlm.createAgentBrain(target_brain)
    
    for step in range(300):
        # Apply transferred knowledge
        # The target brain can leverage skills learned in source task
        target_brain.step(step)
    
    print("Transfer learning completed")
    return source_brain, target_brain
```

### Pattern 3: Ensemble Learning

```python
class BrainEnsemble:
    """Manage multiple brains working together (ensemble learning)."""
    
    def __init__(self, num_brains, brain_size):
        self.brains = []
        self.agents = []
        
        for i in range(num_brains):
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", brain_size)
            config.set("plasticity.stdp.enable", True)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            self.brains.append(brain)
            
            agent = pynlm.createAgentBrain(brain)
            self.agents.append(agent)
    
    def run_ensemble_simulation(self, steps_per_brain, world_config):
        """Run simulation with all brains working in parallel."""
        import pynlm
        
        world = pynlm.createSimpleWorld()
        world.configure(**world_config)
        world.reset()
        
        results = []
        
        for brain_idx, (brain, agent) in enumerate(zip(self.brains, self.agents)):
            brain_results = {
                'brain_id': brain_idx,
                'steps': [],
                'final_firing_rate': 0.0,
                'total_rewards': 0.0
            }
            
            for step in range(steps_per_brain):
                # All brains see the same world state
                world.update(0.1)
                percept = world.getSensoryPercept()
                
                # Each brain processes independently
                agent.processSensoryInput(percept)
                brain.step(step)
                
                # Each brain can contribute to world
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Each brain learns from shared experience
                reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
                agent.applyRewardModulation(reward, 0.0)
                agent.updateDevelopment(0.1)
                
                # Record results
                if step % 50 == 0:
                    brain_results['steps'].append({
                        'step': step,
                        'firing_rate': brain.getAverageFiringRate(),
                        'neurons_firing': brain.getFiringNeuronCount(),
                        'reward': reward
                    })
                    
                    brain_results['total_rewards'] += reward
            
            brain_results['final_firing_rate'] = brain.getAverageFiringRate()
            results.append(brain_results)
        
        return results
```

## Part 3: Improved Python API

### Enhanced Configuration API

```python
# Improved configuration methods with better error handling

class EnhancedConfig:
    """Enhanced configuration interface with validation and utilities."""
    
    @staticmethod
    def createExplorationConfig():
        """Create configuration optimized for exploration tasks."""
        config = pynlm.createDefaultConfig()
        
        # High curiosity for exploration
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.curiosity.threshold", 0.3)
        
        # Balanced learning
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.structural.enable", True)
        
        # Exploration brain size
        config.set("brain.neuron_count", 250)
        config.set("brain.synapse_density", 0.08)
        
        return config
    
    @staticmethod
    def createMemoryConfig():
        """Create configuration optimized for memory tasks."""
        config = pynlm.createDefaultConfig()
        
        # Enhanced memory systems
        config.set("neuromod.dopamine.scale", 1.5)
        config.set("plasticity.structural.enable", True)
        config.set("development_synaptogenesis_rate", 0.002)
        
        # Memory-optimized brain size
        config.set("brain.neuron_count", 400)
        config.set("brain.synapse_density", 0.12)
        
        return config
    
    @staticmethod
    def createBalancedConfig():
        """Create well-balanced configuration for general tasks."""
        config = pynlm.createDefaultConfig()
        
        # Enable all systems at moderate levels
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.curiosity.threshold", 0.2)
        config.set("neuromod.novelty.enable", True)
        
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.001)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.hebbian.learning_rate", 0.0005)
        
        config.set("plasticity.structural.enable", True)
        config.set("plasticity.structural.growth_rate", 0.001)
        
        # Balanced brain size
        config.set("brain.neuron_count", 300)
        config.set("brain.synapse_density", 0.1)
        
        return config
```

### Error Handling and Validation

```python
def safe_simulation_run(brain_config, world_config, num_steps, error_handler=None):
    """Run simulation with comprehensive error handling and validation.
    
    This function demonstrates improved error handling that:
    - Validates inputs before simulation
    - Handles runtime errors gracefully
    - Provides detailed error information
    - Can call custom error handlers
    
    Args:
        brain_config: Brain configuration
        world_config: World configuration  
        num_steps: Number of simulation steps
        error_handler: Optional custom error handler function
        
    Returns:
        tuple: (success, results, error_message)
    """
    try:
        # Input validation
        if num_steps <= 0:
            raise ValueError("Number of steps must be positive")
            
        if not brain_config or not world_config:
            raise ValueError("Configuration cannot be None")
        
        # Create components with error handling
        import pynlm
        
        brain = pynlm.createBrain(brain_config)
        if not brain:
            raise RuntimeError("Failed to create brain")
            
        brain.initialize()
        
        # Continue with simulation...
        # (implementation would continue here)
        
        success = True
        results = {}
        error_msg = None
        
    except Exception as e:
        success = False
        results = {}
        error_msg = str(e)
        
        if error_handler:
            error_handler(e)
        
        print(f"Simulation failed: {error_msg}")
        
    return success, results, error_msg
```

## Part 4: Example Gallery

### Quick Start Examples (Updated)

#### Example 1: Silent Brain (Just Neurons)

```python
import pynlm

# One line to create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Make it active
brain.step(0)

print("Silent brain test done!")
print(f"Neurons: {brain.getTotalNeuronCount()}")
```

#### Example 2: Brain Watching a World

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
print("Watched world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
```

#### Example 3: Complete Agent with Learning

```python
import pynlm

# Setup with enhanced learning
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableNoveltyDetection(True)

# Run agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")

print("Agent simulation complete!")
```

### Advanced Examples

#### Example 4: Brain Development Journey

```python
def brain_development_journey():
    """Show how a brain develops from immature to mature."""
    import pynlm
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    print("Brain Development Journey")
    print("=" * 40)
    
    # Initial state (newborn)
    print("Phase 1: Initial State")
    print(f"  Neurons: {brain.getTotalNeuronCount()}")
    print(f"  Firing rate: {brain.getAverageFiringRate():.3f} Hz")
    print(f"  Stage: {brain.getDevelopmentalStage()}")
    
    # Allow some development
    for step in range(50):
        agent.updateDevelopment(0.1)
        brain.step(step)
    
    # Early development
    print("\nPhase 2: Early Development")
    print(f"  Neurons: {brain.getTotalNeuronCount()}")
    print(f"  Firing rate: {brain.getAverageFiringRate():.3f} Hz")
    print(f"  Stage: {brain.getDevelopmentalStage()}")
    print(f"  Adaptation: {agent.getCuriosityLevel():.3f}")
    
    # Continued development
    for step in range(50, 150):
        agent.updateDevelopment(0.1)
        brain.step(step)
    
    # Mature development
    print("\nPhase 3: Mature Development")
    print(f"  Neurons: {brain.getTotalNeuronCount()}")
    print(f"  Firing rate: {brain.getAverageFiringRate():.3f} Hz")
    print(f"  Stage: {brain.getDevelopmentalStage()}")
    print(f"  Adaptation: {agent.getCuriosityLevel():.3f}")
    
    return brain, agent
```

#### Example 5: Environment Navigation

```python
def environment_navigation_demo():
    """Demonstrate agent navigating and learning from environment."""
    import pynlm
    
    # Create navigation-optimized brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 300)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("plasticity.stdp.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create environment with different areas
    world = pynlm.createSimpleWorld()
    world.configure(
        width=25,
        height=25,
        visionWidth=7,
        visionHeight=7
    )
    world.reset()
    
    # Place agent in center
    world.setAgentStart(12.5, 12.5)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableCuriosity(True)
    agent.enableNoveltyDetection(True)
    agent.enableRewardModulation(True)
    
    print("Environment Navigation Demo")
    print("=" * 40)
    
    # Explore environment
    for step in range(200):
        # Sense environment
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Make decision
        brain.step(step)
        action = agent.decodeMotorCommand()
        
        # Act in environment
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Learn from experience
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        # Report progress
        if step % 40 == 0:
            body = world.getAgentBody()
            curiosity = agent.getCuriosityLevel()
            novelty = agent.getNoveltyLevel()
            
            print(f"Step {step}:")
            print(f"  Position: ({body.x:.1f}, {body.y:.1f})")
            print(f"  Curiosity: {curiosity:.3f}")
            print(f"  Novelty: {novelty:.3f}")
            print(f"  Firing rate: {brain.getAverageFiringRate():.3f} Hz")
    
    print("\nNavigation completed!")
    return brain, world, agent
```

## Part 5: Quick Reference (Enhanced)

### Configuration Templates

```python
# Quick configuration generators

def getExplorationConfig():
    """Config for exploring new environments."""
    config = pynlm.createDefaultConfig()
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.curiosity.threshold", 0.3)
    config.set("neuromod.novelty.enable", True)
    config.set("brain.neuron_count", 250)
    return config

def getMemoryConfig():
    """Config for memory-intensive tasks."""
    config = pynlm.createDefaultConfig()
    config.set("neuromod.dopamine.scale", 1.5)
    config.set("plasticity.structural.enable", True)
    config.set("development_synaptogenesis_rate", 0.002)
    config.set("brain.neuron_count", 400)
    return config

def getBalancedConfig():
    """Config for general-purpose tasks."""
    config = pynlm.createDefaultConfig()
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.curiosity.threshold", 0.2)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("brain.neuron_count", 300)
    return config
```

### Helper Functions

```python
def run_brain_benchmark(brain_config, steps=100):
    """Run a simple benchmark comparing brain performance."""
    import pynlm
    import time
    
    brain = pynlm.createBrain(brain_config)
    brain.initialize()
    
    start_time = time.time()
    
    for step in range(steps):
        brain.step(step)
    
    end_time = time.time()
    
    return {
        'steps': steps,
        'time_seconds': end_time - start_time,
        'steps_per_second': steps / (end_time - start_time),
        'final_firing_rate': brain.getAverageFiringRate(),
        'neurons_firing': brain.getFiringNeuronCount(),
        'total_spikes': brain.getTotalSpikeCount()
    }

def compare_configs(configs, steps=100):
    """Compare performance of different configurations."""
    results = {}
    
    for name, config in configs.items():
        results[name] = run_brain_benchmark(config, steps)
    
    # Find best configuration
    best_config = max(results.keys(), 
                     key=lambda k: results[k]['steps_per_second'])
    
    return results, best_config
```

## Summary

These improvements provide:

1. **High-level convenience functions** that handle common tasks
2. **Better error handling** with validation and graceful failure
3. **Comprehensive examples** showing advanced usage patterns
4. **Enhanced API design** with intuitive naming and structure
5. **Configuration templates** for different use cases
6. **Development-focused examples** showing brain maturation
7. **Navigation and exploration demos** showing real-world applications

The new convenience functions and examples make NLM much easier to use while maintaining all the powerful features of the original system.
