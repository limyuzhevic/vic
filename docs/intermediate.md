# NLM Intermediate Documentation

Welcome to the intermediate documentation! This guide is designed for users who have completed the beginner materials and are ready to:

- Build more complex simulations
- Understand and configure advanced features
- Debug common issues
- Create custom configurations
- Analyze performance and results

## What You'll Learn Here

By the end of this guide, you'll be able to:

✅ Design custom brain configurations
✅ Run advanced simulation experiments
✅ Debug and troubleshoot simulation issues
✅ Optimize performance and memory usage
✅ Create custom worlds and environments
✅ Analyze and visualize brain activity
✅ Integrate NLM with other Python tools

## Core API Reference

This section provides detailed information about NLM's core classes and functions.

### Configuration Class Deep Dive

```python
# Advanced configuration techniques
config = nlm.createDefaultConfig()

# Batch configuration updates
config.batch_set({
    "brain.neuron_count": 5000,
    "brain.synapse_density": 0.05,
    "plasticity.stdp.learning_rate": 0.001,
    "plasticity.hebbian.learning_rate": 0.01,
    "neuromod.dopamine.scale": 1.5,
    "neuromod.curiosity.exploration_weight": 0.7,
    "development.initial_stage": "CriticalPeriod",
    "simulation.max_memory": 1000.0,
    "simulation.timestep": 0.05
})

# Configuration validation and debugging
validation_result = nlm.validateConfig(config)
if not validation_result:
    errors = nlm.getConfigValidationError(config)
    print("Configuration validation failed:")
    print(errors)

# Get configuration templates
from pprint import pprint
template = nlm.getDefaultConfigTemplate()
pprint(dict(template))  # Convert to dict for display
```

#### Configuration Patterns

| Pattern | Use Case | Example |
|---------|----------|---------|
| `batch_set()` | Update multiple parameters | `config.batch_set({...})` |
| `validateConfig()` | Check configuration validity | `nlm.validateConfig(config)` |
| `getConfigValidationError()` | Get specific errors | `nlm.getConfigValidationError(config)` |
| `getDefaultConfigTemplate()` | Get all defaults | `nlm.getDefaultConfigTemplate()` |

### Brain Class Operations

```python
# Advanced brain operations
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

# Brain state management
brain.save("checkpoints/brain_state_001.brain")
brain.save("checkpoints/brain_state_002.brain")

# Restore from checkpoint
brain.load("checkpoints/brain_state_001.brain")

# Batch operations with error handling
def safe_brain_operations(brain, steps):
    """Perform batch brain operations with error handling."""
    
    operations = []
    for step in range(steps):
        try:
            brain.step(step)
            operations.append(f"Step {step} completed")
            
            # Collect statistics every 100 steps
            if step % 100 == 0:
                stats = brain.getBrainStats()
                operations.append(f"Step {step}: {stats['firingNeurons']} neurons firing")
                
        except nlm.OperationError as e:
            operations.append(f"Step {step} failed: {e}")
            continue
        except Exception as e:
            operations.append(f"Step {step} unexpected error: {e}")
            continue
    
    return operations

# Execute batch operations
results = safe_brain_operations(brain, 1000)
for result in results:
    if "failed" not in result.lower():
        print(f"✓ {result}")
    else:
        print(f"✗ {result}")
```

#### Brain Operations

| Operation | When to Use | Best Practices |
|-----------|-------------|-----------------|
| `batch_step()` | Large-scale simulations | Use loops, handle errors |
| `save()`/`load()` | Checkpointing | Save frequently, compress |
| `reset()` | State management | Reset after failed runs |
| `receiveSensoryInput()` | Custom input | Validate input format |
| `produceAction()` | Motor control | Check action validity |

### AgentBrain Advanced Usage

```python
# Advanced agent operations
agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Multi-system coordination
def coordinated_agent_control(agent, world, steps):
    """Control agent with multiple coordinated systems."""
    
    # Enable all systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    for step in range(steps):
        # Coordinated update sequence
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Coordinated neuromodulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Coordinated development
        agent.updateDevelopment(0.1)
        
        # Coordinated statistics collection
        if step % 50 == 0:
            stats = {
                "step": step,
                "position": (world.getAgentBody().x, world.getAgentBody().y),
                "curiosity": agent.getCuriosityLevel(),
                "novelty": agent.getNoveltyLevel(),
                "brain_firing": brain.getFiringNeuronCount(),
                "dev_stage": agent.getDevelopmentalStage()
            }
            print(f"Stats: {stats}")
    
    return agent

# Run coordinated control
agent = coordinated_agent_control(agent, world, 500)
```

#### AgentBrain Patterns

| Pattern | Description | When to Use |
|---------|-------------|-------------|
| `enableAllSystems()` | Enable all subsystems | Initial setup |
| `synchronizedUpdate()` | Coordinated system updates | Multi-system control |
| `statCollection()` | Periodic statistics | Monitoring/debugging |
| `errorHandling()` | Robust error management | Production code |

### SimpleWorld Advanced Features

```python
# Advanced world operations
world = nlm.createSimpleWorld()

# World configuration with constraints
def create_constrained_world(width, height, vision_width, vision_height):
    """Create world with operational constraints."""
    
    world = nlm.createSimpleWorld()
    
    # Set constraints
    world.configure(
        width=width,
        height=height,
        visionWidth=vision_width,
        visionHeight=vision_height
    )
    
    # Add environmental constraints
    world.setMaxEnergy(100.0)  # Energy budget
    world.setRandomSeed(42)     # Reproducible results
    
    # Add world objects
    world.addObject(nlm.WorldObject(5, 5, nlm.WorldObjectType.Resource, value=10.0))
    world.addObject(nlm.WorldObject(10, 10, nlm.WorldObjectType.Hazard, value=-5.0))
    
    return world

# Constrained world
constrained_world = create_constrained_world(20, 20, 10, 10)

# World object management
def manage_world_objects(world):
    """Manage world objects with validation."""
    
    # Get current objects
    objects = []
    for x in range(world.getWidth()):
        for y in range(world.getHeight()):
            if world.isValidPosition(x, y):
                # Would need access to internal object storage
                pass
    
    # Object interactions
    if world.isValidPosition(5, 5):
        # Check what's at position
        # (Would need object query method)
        pass
    
    return objects
```

#### World Management

| Feature | Usage | Best Practices |
|---------|-------|-----------------|
| `configure()` | Set world parameters | Validate dimensions |
| `setMaxEnergy()` | Control energy budget | Balance realism/performance |
| `setRandomSeed()` | Reproducible results | Document seed values |
| `addObject()` | Place objects | Validate object parameters |

## Advanced Simulation Techniques

### Batch Processing

```python
# Batch processing for efficiency
def batch_simulation_scenarios(config, scenarios, steps_per_scenario):
    """Run multiple simulation scenarios efficiently."""
    
    results = []
    
    for scenario_name, scenario_config in scenarios.items():
        print(f"Running scenario: {scenario_name}")
        
        # Create configuration for scenario
        scenario_config = config.copy()
        scenario_config.update(scenario_config)
        
        # Create brain and world
        brain = nlm.createBrain(scenario_config)
        brain.initialize()
        
        world = nlm.createSimpleWorld(20, 20, 8, 8)
        world.configure(**scenario_config.get("world", {}))
        world.reset()
        
        agent = nlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run scenario
        scenario_reward = nlm.runEpisode(agent, world, steps_per_scenario)
        
        results.append({
            "scenario": scenario_name,
            "reward": scenario_reward,
            "final_curiosity": agent.getCuriosityLevel(),
            "firing_rate": brain.getAverageFiringRate()
        })
        
        # Save results
        scenario_name = f"results/scenario_{scenario_name}.brain"
        nlm.exportBrainState(brain, scenario_name)
        
        print(f"  Completed: Reward = {scenario_reward:.3f}")
    
    return results

# Define multiple scenarios
scenarios = {
    "simple": {
        "brain.neuron_count": 1000,
        "plasticity.stdp.learning_rate": 0.001
    },
    "complex": {
        "brain.neuron_count": 5000,
        "plasticity.stdp.learning_rate": 0.01,
        "plasticity.hebbian.learning_rate": 0.005,
        "neuromod.curiosity.exploration_weight": 0.8
    },
    "balanced": {
        "brain.neuron_count": 2000,
        "plasticity.stdp.learning_rate": 0.001,
        "plasticity.hebbian.learning_rate": 0.001,
        "neuromod.curiosity.exploration_weight": 0.5
    }
}

# Run batch simulation
results = batch_simulation_scenarios(
    nlm.createDefaultConfig(),
    scenarios,
    500
)

# Analyze results
for result in results:
    print(f"{result['scenario']}: {result['reward']:.3f} reward, "
          f"{result['final_curiosity']:.2f} curiosity")
```

### Performance Optimization

```python
# Performance monitoring and optimization
def performance_monitoring(brain, benchmark_steps=1000):
    """Monitor and optimize brain performance."""
    
    print("Starting performance monitoring...")
    
    # Get baseline configuration
    baseline_config = nlm.createDefaultConfig()
    
    # Run performance benchmark
    benchmark_results = nlm.benchmarkStep(brain, benchmark_steps)
    
    print("Performance baseline:")
    print(f"  Steps per second: {benchmark_results['stepsPerSecond']:.1f}")
    print(f"  Average step time: {benchmark_results['avgTimePerStepMs']:.2f} ms")
    print(f"  Memory usage: {benchmark_results['memoryUsagePercent']:.1f}%")
    print(f"  Efficiency score: {benchmark_results['efficiencyScore']:.3f}")
    
    # Configuration optimization
    optimized_config = nlm.createDefaultConfig()
    
    # Optimize based on performance data
    if benchmark_results['avgTimePerStepMs'] > 10.0:  # Slow performance
        print("  -> Optimizing for speed...")
        optimized_config.set("brain.learningRate", 0.005)
        optimized_config.set("brain.connectionProbability", 0.03)
    
    if benchmark_results['memoryUsagePercent'] > 80.0:  # High memory usage
        print("  -> Optimizing for memory...")
        optimized_config.set("brain.neuronCount", 2000)
        optimized_config.set("simulation.maxMemory", 500.0)
    
    # Test optimized configuration
    optimized_brain = nlm.createBrain(optimized_config)
    optimized_brain.initialize()
    
    optimized_results = nlm.benchmarkStep(optimized_brain, benchmark_steps)
    
    print("\nOptimized performance:")
    print(f"  Steps per second: {optimized_results['stepsPerSecond']:.1f}")
    print(f"  Average step time: {optimized_results['avgTimePerStepMs']:.2f} ms")
    print(f"  Memory usage: {optimized_results['memoryUsagePercent']:.1f}%")
    
    # Compare performance
    improvement = (optimized_results['stepsPerSecond'] - 
                  benchmark_results['stepsPerSecond']) / benchmark_results['stepsPerSecond'] * 100
    
    print(f"\nPerformance improvement: {improvement:.1f}%")
    
    return benchmark_results, optimized_results

# Run performance monitoring
baseline, optimized = performance_monitoring(brain, 500)
```

### Advanced Debugging

```python
# Advanced debugging with detailed logging
def debug_simulation(brain, world, agent, max_steps=100):
    """Debug simulation with comprehensive logging."""
    
    debug_log = {
        "initialization": {},
        "steps": [],
        "errors": [],
        "warnings": []
    }
    
    # Log initialization
    debug_log["initialization"] = {
        "brain_neurons": brain.getTotalNeuronCount(),
        "brain_synapses": brain.getTotalSynapseCount(),
        "world_dimensions": (world.getWidth(), world.getHeight()),
        "agent_systems": {
            "reward": agent.getRewardLevel() if hasattr(agent, 'getRewardLevel') else None,
            "curiosity": agent.getCuriosityLevel(),
            "development": agent.getDevelopmentalStage()
        }
    }
    
    # Run simulation with debugging
    for step in range(max_steps):
        step_log = {
            "step": step,
            "actions": [],
            "errors": [],
            "metrics": {}
        }
        
        try:
            # Record before state
            before_state = {
                "brain_firing": brain.getFiringNeuronCount(),
                "agent_position": (world.getAgentBody().x, world.getAgentBody().y),
                "agent_curiosity": agent.getCuriosityLevel()
            }
            
            # Execute step
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Record after state
            after_state = {
                "brain_firing": brain.getFiringNeuronCount(),
                "agent_position": (world.getAgentBody().x, world.getAgentBody().y),
                "agent_curiosity": agent.getCuriosityLevel()
            }
            
            # Calculate changes
            firing_change = after_state["brain_firing"] - before_state["brain_firing"]
            curiosity_change = after_state["agent_curiosity"] - before_state["agent_curiosity"]
            
            step_log["metrics"] = {
                "firing_change": firing_change,
                "curiosity_change": curiosity_change,
                "steps_per_second": 1.0 / 0.1  # 10 Hz
            }
            
            # Check for anomalies
            if firing_change > 100:  # Sudden spike in firing
                step_log["warnings"].append(f"Large firing increase: {firing_change}")
            
            if abs(curiosity_change) > 0.5:  # Large curiosity change
                step_log["warnings"].append(f"Large curiosity change: {curiosity_change}")
            
            # Check for errors
            if brain.getFiringNeuronCount() <= 0 and step > 10:
                step_log["errors"].append("No neurons firing")
            
            if agent.getCuriosityLevel() > 1.0:
                step_log["warnings"].append(f"Curiosity exceeded 1.0: {agent.getCuriosityLevel()}")
            
        except Exception as e:
            step_log["errors"].append(str(e))
            debug_log["errors"].append({
                "step": step,
                "error": str(e),
                "type": type(e).__name__
            })
        
        debug_log["steps"].append(step_log)
        
        # Print progress for debugging
        if step % 20 == 0:
            print(f"Step {step}: Brain {before_state['brain_firing']}->{after_state['brain_firing']} "
                  f"neurons, Curiosity {before_state['agent_curiosity']:.2f}->{after_state['agent_curiosity']:.2f}")
    
    return debug_log

# Run debugging
debug_results = debug_simulation(brain, world, agent, 100)

# Analyze debug results
if debug_results["errors"]:
    print(f"\nFound {len(debug_results['errors'])} errors:")
    for error in debug_results["errors"]:
        print(f"  Step {error['step']}: {error['error']} ({error['type']})")

if debug_results["warnings"]:
    print(f"\nFound {len(debug_results['warnings'])} warnings:")
    for warning in debug_results["warnings"]:
        print(f"  {warning}")
```

## Neuromodulation Deep Dive

### Dopamine System

```python
# Advanced dopamine system control
def dopamine_learning_control(brain, world, agent, steps):
    """Control dopamine system for learning optimization."""
    
    dopamine = brain.getDopamine()
    if not dopamine:
        print("Dopamine system not available")
        return
    
    learning_metrics = {
        "rewards": [],
        "dopamine_levels": [],
        "prediction_errors": [],
        "learning_rates": []
    }
    
    for step in range(steps):
        # Get reward from world
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        
        # Get predicted reward (could be from agent's prediction)
        predicted_reward = 0.0  # Simplified
        
        # Calculate prediction error
        prediction_error = reward - predicted_reward
        
        # Signal reward to dopamine system
        dopamine.signalReward(reward)
        
        # Record metrics
        learning_metrics["rewards"].append(reward)
        learning_metrics["dopamine_levels"].append(dopamine.getLevel())
        learning_metrics["prediction_errors"].append(prediction_error)
        
        # Adjust learning rate based on dopamine
        dopamine_level = dopamine.getLevel()
        if dopamine_level > 0.5:  # High confidence
            learning_rate = 0.01
        elif dopamine_level > 0.2:  # Medium confidence
            learning_rate = 0.005
        else:  # Low confidence
            learning_rate = 0.001
        
        learning_metrics["learning_rates"].append(learning_rate)
        
        # Run simulation step
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print dopamine metrics
        if step % 50 == 0:
            print(f"Step {step}: Reward={reward:.3f}, "
                  f"Dopamine={dopamine.getLevel():.3f}, "
                  f"Learning Rate={learning_rate:.3f}")
    
    # Analyze learning metrics
    avg_reward = sum(learning_metrics["rewards"]) / len(learning_metrics["rewards"])
    avg_dopamine = sum(learning_metrics["dopamine_levels"]) / len(learning_metrics["dopamine_levels"])
    avg_error = sum(abs(e) for e in learning_metrics["prediction_errors"]) / len(learning_metrics["prediction_errors"])
    
    print(f"\nDopamine learning analysis:")
    print(f"  Average reward: {avg_reward:.3f}")
    print(f"  Average dopamine: {avg_dopamine:.3f}")
    print(f"  Average prediction error: {avg_error:.3f}")
    
    return learning_metrics

# Run dopamine learning control
learning_data = dopamine_learning_control(brain, world, agent, 200)
```

### Curiosity and Exploration

```python
# Advanced curiosity system
class CuriositiyManager:
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.curiosity = brain.getCuriosity()
        self.novelty = brain.getNovelty()
        self.exploration_history = []
        self.novelty_history = []
    
    def run_exploration_episode(self, max_steps=100):
        """Run exploration episode with curiosity optimization."""
        
        exploration_metrics = {
            "steps": [],
            "novelty_levels": [],
            "curiosity_levels": [],
            "exploration_drives": [],
            "visited_positions": set(),
            "unique_objects": set()
        }
        
        for step in range(max_steps):
            # Update world
            self.world.update(0.1)
            
            # Get current observations
            percept = self.world.getSensoryPercept()
            current_position = (self.world.getAgentBody().x, self.world.getAgentBody().y)
            
            # Record visited positions
            exploration_metrics["visited_positions"].add(current_position)
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Get novelty from previous observations
            previous_percept = percept  # Simplified
            novelty = self.novelty.detectNovelty(percept, previous_percept)
            self.novelty.setLevel(novelty)
            
            # Update curiosity
            prediction_error = self.agent.getPredictionError()
            self.curiosity.update(novelty, prediction_error, 0.1)
            
            # Get brain step
            self.brain.step(step)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            
            # Execute action
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Record metrics
            exploration_metrics["steps"].append(step)
            exploration_metrics["novelty_levels"].append(novelty)
            exploration_metrics["curiosity_levels"].append(self.curiosity.getLevel())
            exploration_metrics["exploration_drives"].append(self.curiosity.getExplorationDrive())
            
            # Print exploration status
            if step % 20 == 0:
                print(f"Step {step}: Novelty={novelty:.3f}, "
                      f"Curiosity={self.curiosity.getLevel():.3f}, "
                      f"Exploration={self.curiosity.getExplorationDrive():.3f}, "
                      f"Visited={len(exploration_metrics['visited_positions'])} positions")
        
        # Analyze exploration patterns
        avg_novelty = sum(exploration_metrics["novelty_levels"]) / len(exploration_metrics["novelty_levels"])
        avg_curiosity = sum(exploration_metrics["curiosity_levels"]) / len(exploration_metrics["curiosity_levels"])
        avg_exploration = sum(exploration_metrics["exploration_drives"]) / len(exploration_metrics["exploration_drives"])
        
        print(f"\nExploration analysis:")
        print(f"  Average novelty: {avg_novelty:.3f}")
        print(f"  Average curiosity: {avg_curiosity:.3f}")
        print(f"  Average exploration drive: {avg_exploration:.3f}")
        print(f"  Total unique positions visited: {len(exploration_metrics['visited_positions'])}")
        
        return exploration_metrics

# Run curiosity management
curiosity_manager = CuriositiyManager(brain, world, agent)
exploration_data = curiosity_manager.run_exploration_episode(200)
```

## Development System Deep Dive

### Brain Maturation

```python
# Advanced development system control
def brain_maturation_simulation(brain, world, agent, maturation_days=50):
    """Simulate brain maturation over time."""
    
    dev_system = brain.getDevelopmentSystem()
    if not dev_system:
        print("Development system not available")
        return
    
    maturation_log = {
        "days": [],
        "stages": [],
        "plasticity_modifiers": [],
        "critical_period_flags": [],
        "learning_metrics": []
    }
    
    print("Starting brain maturation simulation...")
    
    for day in range(maturation_days):
        # Set developmental stage based on day
        if day < 10:
            stage = nlm.DevelopmentalStage.Initial
        elif day < 20:
            stage = nlm.DevelopmentalStage.CriticalPeriod
        elif day < 40:
            stage = nlm.DevelopmentalStage.Maturation
        else:
            stage = nlm.DevelopmentalStage.Adult
        
        dev_system.setStage(stage)
        
        # Get stage information
        stage_name = dev_system.getStageName()
        plasticity_modifier = dev_system.getPlasticityModifier()
        is_critical = dev_system.isCriticalPeriod()
        
        # Record development metrics
        maturation_log["days"].append(day)
        maturation_log["stages"].append(stage_name)
        maturation_log["plasticity_modifiers"].append(plasticity_modifier)
        maturation_log["critical_period_flags"].append(is_critical)
        
        # Run simulation step
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(day)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Calculate learning metrics
        learning_metric = {
            "day": day,
            "stage": stage_name,
            "plasticity": plasticity_modifier,
            "curiosity": agent.getCuriosityLevel(),
            "novelty": agent.getNoveltyLevel(),
            "brain_firing": brain.getFiringNeuronCount(),
            "spikes_generated": brain.getTotalSpikeCount()
        }
        
        maturation_log["learning_metrics"].append(learning_metric)
        
        # Print development status
        if day % 10 == 0:
            print(f"Day {day}: Stage={stage_name}, "
                  f"Plasticity={plasticity_modifier:.3f}, "
                  f"Critical={is_critical}, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}")
    
    # Analyze maturation patterns
    print(f"\nBrain maturation analysis:")
    print(f"  Days in critical period: {sum(1 for x in maturation_log['critical_period_flags'] if x)}")
    print(f"  Average plasticity: {sum(maturation_log['plasticity_modifiers']) / len(maturation_log['plasticity_modifiers']):.3f}")
    print(f"  Final stage: {maturation_log['stages'][-1]}")
    
    return maturation_log

# Run maturation simulation
maturation_data = brain_maturation_simulation(brain, world, agent, 50)
```

## Configuration Management

### Configuration Templates

```python
# Configuration template system
class ConfigTemplateManager:
    def __init__(self):
        self.templates = {
            "beginner": self.get_beginner_template(),
            "intermediate": self.get_intermediate_template(),
            "advanced": self.get_advanced_template(),
            "research": self.get_research_template()
        }
    
    def get_beginner_template(self):
        """Configuration for beginners."""
        return {
            "brain.neuron_count": 1000,
            "brain.synapse_density": 0.1,
            "plasticity.stdp.learning_rate": 0.001,
            "plasticity.hebbian.learning_rate": 0.01,
            "neuromod.curiosity.exploration_weight": 0.3,
            "neuromod.novelty.threshold": 0.5,
            "development.initial_stage": "CriticalPeriod",
            "simulation.timestep": 0.1,
            "simulation.max_steps": 100
        }
    
    def get_intermediate_template(self):
        """Configuration for intermediate users."""
        return {
            "brain.neuron_count": 3000,
            "brain.synapse_density": 0.05,
            "brain.learning_rate": 0.01,
            "plasticity.stdp.learning_rate": 0.005,
            "plasticity.hebbian.learning_rate": 0.02,
            "plasticity.structural.enable": True,
            "neuromod.curiosity.exploration_weight": 0.7,
            "neuromod.novelty.threshold": 0.3,
            "neuromod.dopamine.scale": 1.2,
            "development.initial_stage": "CriticalPeriod",
            "development.critical_period_length": 0.25,
            "simulation.timestep": 0.05,
            "simulation.max_steps": 500
        }
    
    def get_advanced_template(self):
        """Configuration for advanced users."""
        return {
            "brain.neuron_count": 10000,
            "brain.synapse_density": 0.02,
            "brain.learning_rate": 0.005,
            "brain.momentum": 0.9,
            "brain.discount_factor": 0.95,
            "plasticity.stdp.learning_rate": 0.001,
            "plasticity.stdp.tau_plus": 30.0,
            "plasticity.stdp.tau_minus": 20.0,
            "plasticity.hebbian.learning_rate": 0.005,
            "plasticity.structural.enable": True,
            "plasticity.structural.synaptogenesis_rate": 0.01,
            "plasticity.structural.pruning_rate": 0.005,
            "neuromod.curiosity.exploration_weight": 0.9,
            "neuromod.novelty.threshold": 0.1,
            "neuromod.dopamine.scale": 1.5,
            "neuromod.acetylcholine.scale": 1.0,
            "neuromod.norepinephrine.scale": 1.0,
            "neuromod.serotonin.scale": 1.0,
            "development.initial_stage": "CriticalPeriod",
            "development.critical_period_length": 0.3,
            "development.maturation_rate": 0.03,
            "development.plasticity_decay": 0.02,
            "simulation.timestep": 0.01,
            "simulation.max_steps": 2000,
            "simulation.max_memory": 2000.0
        }
    
    def get_research_template(self):
        """Configuration for research experiments."""
        return {
            "brain.neuron_count": 50000,
            "brain.synapse_density": 0.01,
            "brain.learning_rate": 0.001,
            "brain.momentum": 0.95,
            "brain.discount_factor": 0.99,
            "plasticity.stdp.learning_rate": 0.0005,
            "plasticity.stdp.tau_plus": 50.0,
            "plasticity.stdp.tau_minus": 30.0,
            "plasticity.hebbian.learning_rate": 0.001,
            "plasticity.structural.enable": True,
            "plasticity.structural.synaptogenesis_rate": 0.005,
            "plasticity.structural.pruning_rate": 0.002,
            "neuromod.curiosity.exploration_weight": 1.0,
            "neuromod.novelty.threshold": 0.05,
            "neuromod.dopamine.scale": 2.0,
            "neuromod.acetylcholine.scale": 1.5,
            "neuromod.norepinephrine.scale": 1.5,
            "neuromod.serotonin.scale": 1.5,
            "development.initial_stage": "CriticalPeriod",
            "development.critical_period_length": 0.4,
            "development.maturation_rate": 0.02,
            "development.plasticity_decay": 0.01,
            "simulation.timestep": 0.005,
            "simulation.max_steps": 5000,
            "simulation.max_memory": 5000.0,
            "simulation.benchmark_enabled": True
        }
    
    def get_template(self, template_name):
        """Get configuration template by name."""
        return self.templates.get(template_name.lower())
    
    def apply_template(self, config, template_name):
        """Apply configuration template to config."""
        template = self.get_template(template_name)
        if template:
            for key, value in template.items():
                config.set(key, value)
        return config

# Use template manager
template_manager = ConfigTemplateManager()

# Get and apply templates
beginner_config = template_manager.get_template("beginner")
if beginner_config:
    config = nlm.createDefaultConfig()
    for key, value in beginner_config.items():
        config.set(key, value)
    print("Applied beginner template")

advanced_config = template_manager.get_template("advanced")
if advanced_config:
    config = nlm.createDefaultConfig()
    for key, value in advanced_config.items():
        config.set(key, value)
    print("Applied advanced template")
```

### Configuration Validation and Optimization

```python
# Configuration validation and optimization
def validate_and_optimize_config(config, target_performance=None):
    """Validate configuration and optimize for target performance."""
    
    # Validate current configuration
    is_valid = nlm.validateConfig(config)
    if not is_valid:
        errors = nlm.getConfigValidationError(config)
        print(f"Configuration validation failed:")
        print(errors)
        return None
    
    print("Configuration validation passed")
    
    # Get configuration statistics
    config_stats = {
        "total_parameters": 0,
        "valid_parameters": 0,
        "invalid_parameters": [],
        "memory_usage_estimate": 0
    }
    
    # Analyze configuration
    if hasattr(config, 'getKeys'):
        keys = config.getKeys()
        config_stats["total_parameters"] = len(keys)
        
        for key in keys:
            try:
                value = config.get(key)
                config_stats["valid_parameters"] += 1
                
                # Estimate memory usage based on parameters
                if "neuron" in key.lower():
                    config_stats["memory_usage_estimate"] += value * 0.1  # Rough estimate
                elif "synapse" in key.lower():
                    config_stats["memory_usage_estimate"] += value * 0.01
                    
            except Exception:
                config_stats["invalid_parameters"].append(key)
    
    print(f"Configuration analysis:")
    print(f"  Total parameters: {config_stats['total_parameters']}")
    print(f"  Valid parameters: {config_stats['valid_parameters']}")
    print(f"  Invalid parameters: {len(config_stats['invalid_parameters'])}")
    print(f"  Estimated memory usage: {config_stats['memory_usage_estimate']:.1f}%")
    
    # Optimize configuration based on performance target
    if target_performance:
        optimized_config = config.copy()
        
        # Optimize for speed
        if target_performance == "speed":
            optimized_config.set("brain.learningRate", 0.01)
            optimized_config.set("brain.connectionProbability", 0.03)
            optimized_config.set("simulation.timestep", 0.05)
        
        # Optimize for memory
        elif target_performance == "memory":
            optimized_config.set("brain.neuronCount", 2000)
            optimized_config.set("simulation.maxMemory", 500.0)
            optimized_config.set("plasticity.structural.enable", False)
        
        # Optimize for learning
        elif target_performance == "learning":
            optimized_config.set("plasticity.stdp.learningRate", 0.01)
            optimized_config.set("plasticity.hebbian.learningRate", 0.02)
            optimized_config.set("neuromod.curiosity.explorationWeight", 0.9)
        
        return optimized_config
    
    return config

# Validate and optimize configuration
config = nlm.createDefaultConfig()
config.set("brain.neuron_count", 3000)
config.set("plasticity.stdp.learning_rate", 0.005)

validated_config = validate_and_optimize_config(config, "balanced")
if validated_config:
    brain = nlm.createBrain(validated_config)
    brain.initialize()
    print("Configuration validated and optimized successfully")
```

## Advanced Examples

### Example 1: Multi-Agent Environment

```python
# Multi-agent environment simulation
class MultiAgentEnvironment:
    def __init__(self, world_width=50, world_height=50, num_agents=3):
        self.world_width = world_width
        self.world_height = world_height
        self.num_agents = num_agents
        self.world = nlm.createSimpleWorld(world_width, world_height, 10, 10)
        self.agents = []
        self.resources = []
        
        # Initialize world
        self.world.configure(
            width=world_width,
            height=world_height,
            visionWidth=10,
            visionHeight=10
        )
        self.world.reset()
        
        # Create agents with different strategies
        self.create_agents()
        
        # Place resources
        self.place_resources()
    
    def create_agents(self):
        """Create multiple agents with different configurations."""
        
        # Agent 1: High curiosity, low learning rate
        config1 = nlm.createDefaultConfig()
        config1.set("brain.neuron_count", 1500)
        config1.set("plasticity.stdp.learning_rate", 0.001)
        config1.set("neuromod.curiosity.exploration_weight", 0.8)
        
        brain1 = nlm.createBrain(config1)
        brain1.initialize()
        
        agent1 = nlm.createAgentBrain(brain1)
        agent1.initialize(self.world)
        agent1.enableRewardModulation(True)
        agent1.enableCuriosity(True)
        agent1.enableDevelopment(True)
        
        self.agents.append(("Agent1", brain1, agent1))
        
        # Agent 2: Balanced strategy
        config2 = nlm.createDefaultConfig()
        config2.set("brain.neuron_count", 2000)
        config2.set("plasticity.stdp.learning_rate", 0.005)
        config2.set("plasticity.hebbian.learning_rate", 0.01)
        config2.set("neuromod.curiosity.exploration_weight", 0.5)
        
        brain2 = nlm.createBrain(config2)
        brain2.initialize()
        
        agent2 = nlm.createAgentBrain(brain2)
        agent2.initialize(self.world)
        agent2.enableRewardModulation(True)
        agent2.enableStructuralPlasticity(True)
        agent2.enableCuriosity(True)
        
        self.agents.append(("Agent2", brain2, agent2))
        
        # Agent 3: Low curiosity, high learning rate (exploiter)
        config3 = nlm.createDefaultConfig()
        config3.set("brain.neuron_count", 1000)
        config3.set("plasticity.stdp.learning_rate", 0.02)
        config3.set("plasticity.hebbian.learning_rate", 0.03)
        config3.set("neuromod.curiosity.exploration_weight", 0.2)
        
        brain3 = nlm.createBrain(config3)
        brain3.initialize()
        
        agent3 = nlm.createAgentBrain(brain3)
        agent3.initialize(self.world)
        agent3.enableRewardModulation(True)
        agent3.enableDevelopment(True)
        
        self.agents.append(("Agent3", brain3, agent3))
    
    def place_resources(self):
        """Place resources in the world."""
        
        import random
        
        # Place food resources
        for _ in range(10):
            x = random.randint(0, self.world_width - 1)
            y = random.randint(0, self.world_height - 1)
            if self.world.isValidPosition(x, y):
                self.resources.append((x, y, nlm.WorldObjectType.Resource, 10.0))
                self.world.addObject(nlm.WorldObject(x, y, nlm.WorldObjectType.Resource, value=10.0))
        
        # Place hazard zones
        for _ in range(3):
            x = random.randint(0, self.world_width - 1)
            y = random.randint(0, self.world_height - 1)
            if self.world.isValidPosition(x, y):
                self.resources.append((x, y, nlm.WorldObjectType.Hazard, -5.0))
                self.world.addObject(nlm.WorldObject(x, y, nlm.WorldObjectType.Hazard, value=-5.0))
    
    def run_multi_agent_episode(self, max_steps=100):
        """Run multi-agent episode."""
        
        episode_data = {
            "steps": [],
            "agent_performance": {},
            "resource_changes": [],
            "agent_interactions": []
        }
        
        for step in range(max_steps):
            # Record state before actions
            before_state = {
                "step": step,
                "agent_positions": [(name, brain, agent.getAgentBody().x, agent.getAgentBody().y) 
                                  for name, brain, agent in self.agents],
                "resource_count": len(self.resources)
            }
            
            # Process actions for all agents
            agent_actions = []
            
            for name, brain, agent in self.agents:
                # Get sensory input
                percept = self.world.getSensoryPercept()
                agent.processSensoryInput(percept)
                
                # Get action
                action = agent.decodeMotorCommand()
                agent_actions.append((name, action))
                
                # Apply action
                self.world.applyMotorCommand(action, self.world.getSimulationTime())
                
                # Apply reward modulation
                reward = self.world.getSensoryPercept().getInternal()[0] if self.world.getSensoryPercept().getInternal() else 0.0
                agent.applyRewardModulation(reward, 0.0)
                
                # Update development
                agent.updateDevelopment(0.1)
            
            # Record episode data
            episode_data["steps"].append({
                "step": step,
                "actions": agent_actions,
                "before_state": before_state
            })
            
            # Update world
            self.world.update(0.1)
            
            # Record resource changes
            current_resources = [(obj.body.x, obj.body.y, obj.type, obj.value) 
                               for obj in self.world.getObjects() if obj]
            if current_resources != [r for r, *_ in self.resources]:
                episode_data["resource_changes"].append(current_resources)
            
            # Print step information
            if step % 20 == 0:
                print(f"Step {step}:")
                for name, brain, agent in self.agents:
                    print(f"  {name}: Pos({agent.getAgentBody().x:.1f}, {agent.getAgentBody().y:.1f}), "
                          f"Curiosity={agent.getCuriosityLevel():.2f}")
        
        # Calculate agent performance
        for name, brain, agent in self.agents:
            episode_data["agent_performance"][name] = {
                "final_curiosity": agent.getCuriosityLevel(),
                "firing_rate": brain.getAverageFiringRate(),
                "total_spikes": brain.getTotalSpikeCount()
            }
        
        return episode_data

# Run multi-agent simulation
env = MultiAgentEnvironment(50, 50, 3)
episode_data = env.run_multi_agent_episode(100)

print(f"\nMulti-agent simulation complete!")
print(f"Episode steps: {len(episode_data['steps'])}")
print(f"Agent performance:")
for agent_name, perf in episode_data["agent_performance"].items():
    print(f"  {agent_name}: Curiosity={perf['final_curiosity']:.2f}, "
          f"Firing={perf['firing_rate']:.2f} Hz, "
          f"Spikes={perf['total_spikes']}")
```

### Example 2: Research Experiment Template

```python
# Research experiment template for reproducibility
def run_research_experiment(experiment_config, experiment_name):
    """Run research experiment with full reproducibility."""
    
    # Set up experiment environment
    import time
    import json
    from datetime import datetime
    
    experiment_id = f"{experiment_name}_{datetime.now().strftime('%Y%m%d_%H%M%S')}"
    
    # Create experiment directory
    experiment_dir = f"experiments/{experiment_id}"
    os.makedirs(experiment_dir, exist_ok=True)
    
    # Save experiment configuration
    config_file = f"{experiment_dir}/config.json"
    with open(config_file, 'w') as f:
        json.dump(experiment_config, f, indent=2)
    
    print(f"Starting experiment: {experiment_name}")
    print(f"Experiment ID: {experiment_id}")
    print(f"Config saved to: {config_file}")
    
    # Initialize experiment tracking
    experiment_data = {
        "experiment_id": experiment_id,
        "name": experiment_name,
        "timestamp": datetime.now().isoformat(),
        "config": experiment_config,
        "results": {},
        "performance": {},
        "neural_activity": {},
        "agent_behavior": {}
    }
    
    # Run experiment based on configuration
    if experiment_config["type"] == "curiosity_study":
        experiment_data["results"] = run_curiosity_experiment(experiment_config)
    
    elif experiment_config["type"] == "development_study":
        experiment_data["results"] = run_development_experiment(experiment_config)
    
    elif experiment_config["type"] == "multi_factor_study":
        experiment_data["results"] = run_multi_factor_experiment(experiment_config)
    
    else:
        raise ValueError(f"Unknown experiment type: {experiment_config['type']}")
    
    # Save experiment results
    results_file = f"{experiment_dir}/results.json"
    with open(results_file, 'w') as f:
        json.dump(experiment_data, f, indent=2)
    
    # Save neural activity
    if "neural_activity" in experiment_data:
        neural_file = f"{experiment_dir}/neural_activity.json"
        with open(neural_file, 'w') as f:
            json.dump(experiment_data["neural_activity"], f, indent=2)
    
    # Save performance data
    if "performance" in experiment_data:
        performance_file = f"{experiment_dir}/performance.json"
        with open(performance_file, 'w') as f:
            json.dump(experiment_data["performance"], f, indent=2)
    
    # Save agent behavior
    if "agent_behavior" in experiment_data:
        behavior_file = f"{experiment_dir}/agent_behavior.json"
        with open(behavior_file, 'w') as f:
            json.dump(experiment_data["agent_behavior"], f, indent=2)
    
    print(f"Experiment completed successfully")
    print(f"Results saved to: {experiment_dir}")
    
    return experiment_data

def run_curiosity_experiment(config):
    """Run curiosity-focused experiment."""
    
    # Create brain with controlled curiosity
    brain_config = config["brain_config"]
    brain = nlm.createBrain(brain_config)
    brain.initialize()
    
    # Create world
    world = nlm.createSimpleWorld(30, 30, 8, 8)
    world.configure(**config["world_config"])
    world.reset()
    
    # Create agent
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable curiosity
    agent.enableCuriosity(True)
    
    # Run curiosity experiment
    curiosity_data = {
        "steps": [],
        "novelty_responses": [],
        "exploration_patterns": [],
        "curiosity_dynamics": []
    }
    
    for step in range(config["max_steps"]):
        # Record before state
        before_novelty = agent.getNoveltyLevel()
        before_curiosity = agent.getCuriosityLevel()
        
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Detect novelty
        if step > 0:
            previous_percept = percept  # Simplified
            novelty = agent.getNovelty().detectNovelty(percept, previous_percept)
        else:
            novelty = 0.0
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Update curiosity
        prediction_error = agent.getPredictionError()
        agent.updateDevelopment(0.1)
        
        # Record step data
        step_data = {
            "step": step,
            "novelty": novelty,
            "curiosity_before": before_curiosity,
            "curiosity_after": agent.getCuriosityLevel(),
            "curiosity_change": agent.getCuriosityLevel() - before_curiosity,
            "exploration_drive": agent.getCuriosity().getExplorationDrive() if agent.getCuriosity() else 0.0,
            "position": (world.getAgentBody().x, world.getAgentBody().y)
        }
        
        curiosity_data["steps"].append(step)
        curiosity_data["novelty_responses"].append(novelty)
        curiosity_data["curiosity_dynamics"].append(step_data)
        
        # Print progress
        if step % 20 == 0:
            print(f"Step {step}: Novelty={novelty:.3f}, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}, "
                  f"Exploration={agent.getCuriosity().getExplorationDrive() if agent.getCuriosity() else 0.0:.3f}")
    
    # Analyze curiosity dynamics
    curiosity_changes = [d["curiosity_change"] for d in curiosity_data["curiosity_dynamics"]]
    avg_curiosity_change = sum(curiosity_changes) / len(curiosity_changes)
    max_curiosity_change = max(abs(x) for x in curiosity_changes)
    
    print(f"\nCuriosity experiment analysis:")
    print(f"  Average curiosity change: {avg_curiosity_change:.3f}")
    print(f"  Maximum curiosity change: {max_curiosity_change:.3f}")
    print(f"  Final curiosity level: {agent.getCuriosityLevel():.3f}")
    
    return curiosity_data

# Define research experiment configurations
research_experiments = {
    "curiosity_study_1": {
        "type": "curiosity_study",
        "name": "Curiosity Study - Low vs High Exploration",
        "brain_config": nlm.createDefaultConfig(),
        "world_config": {"width": 30, "height": 30, "visionWidth": 8, "visionHeight": 8},
        "max_steps": 200
    },
    "development_study_1": {
        "type": "development_study", 
        "name": "Development Study - Critical Period",
        "brain_config": nlm.createDefaultConfig(),
        "world_config": {"width": 40, "height": 40, "visionWidth": 10, "visionHeight": 10},
        "max_steps": 300
    }
}

# Run research experiments
for exp_name, exp_config in research_experiments.items():
    results = run_research_experiment(exp_config, exp_name)
```

## Conclusion

This intermediate documentation covers:

### Core Topics:
- ✅ Detailed API reference for all NLM classes
- ✅ Advanced configuration and parameter optimization
- ✅ Complex simulation techniques and batch processing
- ✅ Performance monitoring and optimization
- ✅ Comprehensive debugging and troubleshooting
- ✅ Neuromodulation system deep dive
- ✅ Development system analysis
- ✅ Configuration management and templates
- ✅ Multi-agent environment simulation
- ✅ Research experiment design

### Skills Developed:
- ✅ Design custom brain configurations for specific use cases
- ✅ Run and analyze complex simulation experiments
- ✅ Debug and troubleshoot simulation issues
- ✅ Optimize performance for speed, memory, or learning
- ✅ Create reproducible research experiments
- ✅ Analyze neural and behavioral data
- ✅ Integrate NLM with Python analysis tools

You're now ready to:

- ✅ Build complex, customized NLM simulations
- ✅ Analyze performance and optimize configurations
- ✅ Debug and troubleshoot advanced issues
- ✅ Create research-grade experiments
- ✅ Integrate NLM with other tools and workflows

The foundation is solid - you're well-prepared to tackle advanced NLM applications and research!
