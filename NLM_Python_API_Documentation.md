# NLM Python API - Comprehensive Examples and Documentation

This document provides comprehensive examples and documentation for the enhanced NLM Python API, building on the existing easy_usage.md and HOW_TO_USE.md documentation.

## Introduction

This is the enhanced NLM (Neural Learning Machine) Python API documentation. This represents a significant upgrade over the original basic documentation with advanced features, improved error handling, comprehensive configuration management, and extensive examples for both beginners and advanced users.

## Table of Contents

1. [Quick Start for Beginners](#quick-start-for-beginners)
2. [Advanced Agent Examples](#advanced-agent-examples)
3. [Configuration Management](#configuration-management)
4. [Error Handling and Recovery](#error-handling-and-recovery)
5. [Performance Monitoring](#performance-monitoring)
6. [Memory Systems Integration](#memory-systems-integration)
7. [Neuromodulation Systems](#neuromodulation-systems)
8. [Development System](#development-system)
9. [Prediction System](#prediction-system)
10. [Advanced Topics](#advanced-topics)
11. [API Reference](#api-reference)
12. [Troubleshooting](#troubleshooting)

## Quick Start for Beginners

This section provides the same basic examples as `easy_usage.md` but with enhanced error handling and additional features.

### Example 1: Simplest Brain

```python
import pynlm

# One line to create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it active
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
```

### Example 2: Silent Brain (Just Neurons)

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

### Example 3: Complete Agent with Error Handling

```python
import pynlm

def run_agent_simulation_with_error_handling(num_steps=100):
    """Run a complete agent simulation with robust error handling."""
    try:
        # Setup configuration with error recovery
        config = pynlm.createAdvancedConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning and development
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Run simulation with error recovery
        for step in range(num_steps):
            try:
                # Safe world update with recovery
                if not pynlm.safeWorldUpdate(world, 0.1):
                    print(f"World update failed at step {step}, resetting...")
                
                # Process sensory input with recovery
                percept = world.getSensoryPercept()
                if not pynlm.safeAgentProcess(agent, percept):
                    print(f"Agent processing failed at step {step}, resetting...")
                
                # Safe brain step with recovery
                if not pynlm.safeBrainStep(brain, step, step * 0.1):
                    print(f"Brain step failed at step {step}, resetting...")
                
                # Get action and apply to world
                action = agent.decodeMotorCommand()
                result = world.applyMotorCommand(action, world.getSimulationTime())
                
                # Apply reward if learning enabled
                if agent.isRewardModulationEnabled():
                    agent.applyRewardModulation(result.reward, 0.0f)
                
                # Update development if enabled
                if agent.isDevelopmentEnabled():
                    agent.updateDevelopment(0.1)
                
                # Log progress periodically
                if step % 20 == 0:
                    stats = pynlm.collectBrainStats(brain)
                    print(f"Step {step}: {stats['firing_neurons']} neurons firing, "
                          f"curiosity: {agent.getCuriosityLevel():.2f}, "
                          f"dopamine: {agent.getNeuromodulationLevel():.2f}, "
                          f"reward: {result.reward:.2f}")
                
            except Exception as e:
                print(f"Error at step {step}: {e}")
                brain.reset()
                world.reset()
                agent.reset()
                continue
        
        print("Agent simulation completed successfully!")
        return True
        
    except Exception as e:
        print(f"Simulation failed: {e}")
        return False

# Run the enhanced simulation
run_agent_simulation_with_error_handling(500)
```

## Advanced Agent Examples

### Example: Learning Agent with Curiosity

```python
import pynlm

def run_learning_agent(num_steps=1000):
    """Run an agent with advanced learning capabilities."""
    
    # Create advanced configuration for learning
    config = pynlm.createAdvancedConfig()
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent components
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all advanced learning subsystems
    agent.enableRewardModulation(True)  # Reward-based learning
    agent.enableCuriosity(True)         # Curiosity-driven exploration
    agent.enableDevelopment(True)       # Developmental maturation
    agent.enableStructuralPlasticity(True)  # Structural plasticity
    
    print("Starting advanced learning agent simulation...")
    
    for step in range(num_steps):
        # Update world environment
        world.update(0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step with prediction
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        result = world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation (learning)
        if agent.isRewardModulationEnabled():
            agent.applyRewardModulation(result.reward, 0.0f)
        
        # Update development system
        if agent.isDevelopmentEnabled():
            agent.updateDevelopment(0.1)
        
        # Log detailed statistics every 100 steps
        if step % 100 == 0:
            stats = pynlm.collectBrainStats(brain)
            neuromod = agent.getNeuromodulationLevel()
            curiosity = agent.getCuriosityLevel()
            novelty = agent.getNoveltyLevel()
            prediction_error = agent.getPredictionError()
            
            print(f"Step {step}:")
            print(f"  Statistics: {stats['firing_neurons']} neurons firing, "
                  f"{stats['total_spikes']} total spikes")
            print(f"  Neuromodulation: dopamine={neuromod:.2f}, "
                  f"curiosity={curiosity:.2f}, novelty={novelty:.2f}")
            print(f"  Learning: prediction error={prediction_error:.3f}")
            print(f"  World: time={world.getSimulationTime():.1f}s, "
                  f"agent at ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    
    print("Learning agent simulation complete!")
    return True

# Run the learning agent
run_learning_agent(1000)
```

### Example: Batch Experiment Runner

```python
import pynlm
from typing import List, Dict, Any

class BatchExperimentRunner:
    """Run multiple experiments in batch with configuration management."""
    
    def __init__(self):
        self.results = []
    
    def create_experiment_config(self, experiment_id: int, 
                                 agent_type: str = "advanced",
                                 environment_type: str = "complex"):
        """Create configuration for a specific experiment."""
        config = pynlm.createExperimentConfig(agent_type, environment_type, 500)
        
        # Add experiment-specific parameters
        config.set(f"experiment.id", experiment_id, pynlm.ConfigSource.Runtime)
        config.set(f"experiment.seed", 42 + experiment_id, pynlm.ConfigSource.Runtime)
        
        return config
    
    def run_single_experiment(self, experiment_id: int) -> Dict[str, Any]:
        """Run a single experiment with comprehensive tracking."""
        print(f"Starting experiment {experiment_id}...")
        
        try:
            # Create experiment-specific configuration
            config = self.create_experiment_config(experiment_id)
            
            # Initialize systems
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Enable advanced features based on experiment type
            if experiment_id % 3 == 0:
                agent.enableRewardModulation(True)
            if experiment_id % 3 == 1:
                agent.enableCuriosity(True)
            if experiment_id % 3 == 2:
                agent.enableDevelopment(True)
            
            # Run experiment
            experiment_steps = 200
            for step in range(experiment_steps):
                world.update(0.1)
                
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                
                action = agent.decodeMotorCommand()
                result = world.applyMotorCommand(action, world.getSimulationTime())
                
                # Record results
                if agent.isRewardModulationEnabled():
                    agent.applyRewardModulation(result.reward, 0.0f)
                
                if agent.isDevelopmentEnabled():
                    agent.updateDevelopment(0.1)
            
            # Collect final statistics
            stats = pynlm.collectBrainStats(brain)
            neuromodulation = agent.getNeuromodulationLevel()
            curiosity = agent.getCuriosityLevel()
            development_stage = agent.getDevelopmentalStage()
            
            result = {
                "experiment_id": experiment_id,
                "status": "completed",
                "steps_completed": experiment_steps,
                "brain_stats": stats,
                "neuromodulation_level": neuromodulation,
                "curiosity_level": curiosity,
                "developmental_stage": development_stage,
                "learning_success": neuromodulation > 0.1 and curiosity > 0.1
            }
            
            print(f"Experiment {experiment_id} completed successfully!")
            return result
            
        except Exception as e:
            print(f"Experiment {experiment_id} failed: {e}")
            return {
                "experiment_id": experiment_id,
                "status": "failed",
                "error": str(e)
            }
    
    def run_batch(self, num_experiments: int) -> List[Dict[str, Any]]:
        """Run a batch of experiments."""
        print(f"Starting batch of {num_experiments} experiments...")
        
        for i in range(num_experiments):
            result = self.run_single_experiment(i)
            self.results.append(result)
        
        # Generate summary
        completed = sum(1 for r in self.results if r["status"] == "completed")
        failed = len(self.results) - completed
        
        print(f"\nBatch Summary:")
        print(f"  Total experiments: {num_experiments}")
        print(f"  Completed: {completed}")
        print(f"  Failed: {failed}")
        
        if completed > 0:
            success_rate = sum(1 for r in self.results if r.get("learning_success", False)) / completed
            print(f"  Success rate: {success_rate:.1%}")
        
        return self.results

# Run batch experiments
runner = BatchExperimentRunner()
results = runner.run_batch(10)

# Save results to file
import json
with open("batch_experiment_results.json", "w") as f:
    json.dump(results, f, indent=2)

print("Batch experiment results saved to batch_experiment_results.json")
```

## Configuration Management

### Creating and Validating Configurations

```python
import pynlm

# Create a basic configuration
config = pynlm.createDefaultConfig()

# Use fluent API for configuration
config.setBrainConfig(2000, 0.15f)

# Create advanced configuration
advanced_config = pynlm.createAdvancedConfig()

# Validate configuration
validation_result = advanced_config.validate()

if validation_result["__status"] == "Valid":
    print("Configuration is valid!")
else:
    print("Configuration errors:")
    for key, error in validation_result.items():
        if key != "__status":
            print(f"  {key}: {error}")

# Use error recovery configuration
robust_config = pynlm.createErrorRecoveryConfig()
brain = pynlm.createBrain(robust_config)
brain.initialize()

# Clone and modify configuration
clone = config.clone()
clone.set("brain.neuron_count", 3000, pynlm.ConfigSource.User)
```

### Configuration Templates

```python
# Configuration templates for different use cases

BRAIN_TEMPLATES = {
    "basic": {
        "brain.neuron_count": 1000,
        "brain.synapse_density": 0.1,
        "plasticity.stdp.enable": True,
        "plasticity.hebbian.enable": True,
    },
    "advanced": {
        "brain.neuron_count": 3000,
        "brain.synapse_density": 0.12,
        "plasticity.stdp.learning_rate": 0.003,
        "plasticity.stdp.tau_plus": 25.0,
        "plasticity.stdp.tau_minus": 25.0,
        "neuromod.dopamine.scale": 1.2,
        "neuromod.curiosity.enable": True,
        "neuromod.novelty.enable": True,
        "development.stage.initial_plasticity": 1.0,
        "development.stage.adult_plasticity": 0.1,
    },
    "research": {
        "brain.neuron_count": 5000,
        "brain.synapse_density": 0.08,
        "plasticity.stdp.enable": True,
        "plasticity.hebbian.enable": True,
        "plasticity.structural.enable": True,
        "memory.working.capacity": 800,
        "memory.episodic.max_episodes": 5000,
        "prediction.enabled": True,
        "cognition.planner.planning_depth": 15,
    }
}

WORLD_TEMPLATES = {
    "simple": {
        "world.width": 20.0,
        "world.height": 20.0,
        "world.vision_width": 16,
        "world.vision_height": 16,
        "world.max_energy": 100.0,
        "world.max_resources": 5,
        "world.max_hazards": 2,
    },
    "complex": {
        "world.width": 30.0,
        "world.height": 30.0,
        "world.vision_width": 20,
        "world.vision_height": 20,
        "world.max_energy": 200.0,
        "world.resource_energy_gain": 50.0,
        "world.energy_decay_rate": 0.005,
        "world.max_resources": 10,
        "world.max_hazards": 5,
    }
}

# Use templates
import json

# Save configuration to file
config.saveToFile("my_config.json")

# Load configuration from file
loaded_config = pynlm.createDefaultConfig()
loaded_config.loadFromFile("my_config.json")

# Configuration comparison
config1 = pynlm.createAdvancedConfig()
config2 = pynlm.createAdvancedConfig()

if config1.equals(config2):
    print("Configurations are identical")
else:
    print("Configurations differ")
```

## Error Handling and Recovery

### Safe Operations with Error Recovery

```python
import pynlm

# Example of safe operations with error recovery

def safe_simulation_step(brain, world, agent):
    """Perform a simulation step with comprehensive error recovery."""
    step_errors = []
    
    try:
        # Safe world update
        if not pynlm.safeWorldUpdate(world, 0.1):
            step_errors.append("World update failed, resetting world")
            world.reset()
        
        # Safe sensory processing
        percept = world.getSensoryPercept()
        if not pynlm.safeAgentProcess(agent, percept):
            step_errors.append("Agent processing failed, resetting agent")
            agent.reset()
        
        # Safe brain step
        if not pynlm.safeBrainStep(brain, step, step * 0.1):
            step_errors.append("Brain step failed, resetting brain")
            brain.reset()
            brain.initialize()
        
        # Motor command and application
        try:
            action = agent.decodeMotorCommand()
            result = world.applyMotorCommand(action, world.getSimulationTime())
            
            # Log errors if any occurred
            if step_errors:
                print(f"Step completed with errors: {'; '.join(step_errors)}")
            
            return True, step_errors, result
            
        except Exception as e:
            step_errors.append(f"Action execution failed: {e}")
            return False, step_errors, None
            
    except Exception as e:
        step_errors.append(f"Critical error: {e}")
        return False, step_errors, None

# Usage example
def run_safe_simulation(total_steps=100):
    """Run a simulation with comprehensive error handling."""
    # Initialize systems with error recovery config
    config = pynlm.createErrorRecoveryConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable safety features
    agent.enableRewardModulation(True)
    agent.enableDevelopment(True)
    
    print("Starting safe simulation...")
    
    for step in range(total_steps):
        success, errors, result = safe_simulation_step(brain, world, agent)
        
        if success:
            # Update development if result is available
            if result and agent.isDevelopmentEnabled():
                agent.updateDevelopment(0.1)
            
            # Log step statistics periodically
            if step % 20 == 0:
                stats = pynlm.collectBrainStats(brain)
                print(f"Step {step}: Success, {len(errors)} warnings, "
                      f"{stats['firing_neurons']} neurons firing")
        
        # Brief pause between steps
        import time
        time.sleep(0.01)
    
    print("Safe simulation completed!")
    return True

# Run the safe simulation
run_safe_simulation(200)
```

### Configuration Validation with Auto-Fix

```python
import pynlm

# Advanced configuration validation with auto-correction

def validate_and_fix_config(config, enable_auto_fix=True):
    """Validate configuration and apply fixes if enabled."""
    
    issues = []
    fixes_applied = []
    
    # Check for critical missing parameters
    critical_params = [
        ("brain.neuron_count", 100, 100000, "brain.neuron_count"),
        ("brain.synapse_density", 0.01, 1.0, "brain.synapse_density"),
        ("simulation.timestep", 0.0001, 1.0, "simulation.timestep"),
    ]
    
    for param, min_val, max_val, name in critical_params:
        if not config.has(param):
            issues.append(f"Missing critical parameter: {name}")
            if enable_auto_fix:
                config.set(param, min_val, pynlm.ConfigSource.Runtime)
                fixes_applied.append(f"Applied default for {name}: {min_val}")
        else:
            # Check parameter range
            try:
                if param == "brain.neuron_count":
                    value = config.getOr<int64_t>(param, 0)
                elif param == "brain.synapse_density":
                    value = config.getOr<double>(param, 0.0)
                elif param == "simulation.timestep":
                    value = config.getOr<double>(param, 0.0)
                else:
                    continue
                
                if value < min_val or value > max_val:
                    issues.append(f"Invalid range for {name}: {value} (valid: {min_val}-{max_val})")
                    if enable_auto_fix:
                        config.set(param, min_val if value < min_val else max_val, pynlm.ConfigSource.Runtime)
                        fixes_applied.append(f"Fixed {name} to {min_val if value < min_val else max_val}")
            except:
                issues.append(f"Cannot read parameter: {name}")
    
    return {
        "valid": len(issues) == 0,
        "issues": issues,
        "fixes_applied": fixes_applied,
        "issue_count": len(issues),
        "fix_count": len(fixes_applied)
    }

# Usage example

def run_with_validated_config():
    """Run simulation with configuration validation and auto-fix."""
    
    # Create a configuration (potentially incomplete)
    config = pynlm.createDefaultConfig()
    # Note: We're not setting all parameters to simulate incomplete config
    
    # Validate and fix configuration
    validation_result = validate_and_fix_config(config, enable_auto_fix=True)
    
    print(f"Configuration validation result:")
    print(f"  Valid: {validation_result['valid']}")
    print(f"  Issues found: {validation_result['issue_count']}")
    print(f"  Fixes applied: {validation_result['fix_count']}")
    
    if validation_result["issues"]:
        print("Issues:")
        for issue in validation_result["issues"]:
            print(f"    - {issue}")
    
    if validation_result["fixes_applied"]:
        print("Fixes applied:")
        for fix in validation_result["fixes_applied"]:
            print(f"    - {fix}")
    
    # Create brain with validated configuration
    brain = pynlm.createBrain(config)
    
    if brain.initialize():
        print("Brain initialized successfully with validated configuration")
        return brain
    else:
        print("Failed to initialize brain")
        return None

# Run with validated configuration
brain = run_with_validated_config()
```

## Performance Monitoring

### Profiling and Performance Analysis

```python
import pynlm
import time

# Comprehensive performance monitoring

def profile_brain_performance(brain, steps=100, warmup_steps=10):
    """Profile brain performance with detailed metrics."""
    
    # Warm-up steps
    for i in range(warmup_steps):
        brain.step(i)
    
    # Start timing
    start_time = time.time()
    
    # Run profiling steps
    for step in range(steps):
        brain.step(step)
    
    # Calculate metrics
    end_time = time.time()
    total_time = end_time - start_time
    
    # Create performance profile
    profile = {
        "steps": steps,
        "total_time_seconds": total_time,
        "average_time_per_step_ms": (total_time / steps) * 1000,
        "steps_per_second": steps / total_time,
        "wall_clock_efficiency": (warmup_steps + steps) / total_time,
        
        # Brain state metrics
        "final_neurons": brain.getTotalNeuronCount(),
        "final_synapses": brain.getTotalSynapseCount(),
        "total_spikes": brain.getTotalSpikeCount(),
        "average_firing_rate": brain.getAverageFiringRate(),
        "e_i_ratio": brain.getExcitationInhibitionRatio(),
        
        # Memory system metrics
        "working_memory_traces": brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 0,
        "episodic_memory_episodes": brain.getEpisodicMemory().getEpisodeCount() if brain.getEpisodicMemory() else 0,
        "associative_memory_patterns": brain.getAssociativeMemory().getPatternCount() if brain.getAssociativeMemory() else 0,
        
        # Neuromodulation metrics
        "dopamine_level": brain.getDopamine().getLevel() if brain.getDopamine() else 0.0,
        "curiosity_level": brain.getCuriosity().getLevel() if brain.getCuriosity() else 0.0,
        "novelty_level": brain.getNovelty().getLevel() if brain.getNovelty() else 0.0,
        "prediction_error": brain.getPredictionErrorSignal().getError() if brain.getPredictionErrorSignal() else 0.0,
    }
    
    return profile

def print_performance_report(profile):
    """Print a formatted performance report."""
    print("=" * 60)
    print("NLM BRAIN PERFORMANCE REPORT")
    print("=" * 60)
    print(f"Simulation Steps: {profile['steps']}")
    print(f"Total Time: {profile['total_time_seconds']:.3f} seconds")
    print(f"Average Time per Step: {profile['average_time_per_step_ms']:.3f} ms")
    print(f"Steps per Second: {profile['steps_per_second']:.1f}")
    print(f"Wall Clock Efficiency: {profile['wall_clock_efficiency']:.1f} Hz")
    print()
    print("NEURAL SYSTEM STATISTICS:")
    print(f"  Neurons: {profile['final_neurons']:,}")
    print(f"  Synapses: {profile['final_synapses']:,}")
    print(f"  Total Spikes: {profile['total_spikes']:,}")
    print(f"  Average Firing Rate: {profile['average_firing_rate']:.2f} Hz")
    print(f"  E/I Ratio: {profile['e_i_ratio']:.3f}")
    print()
    print("MEMORY SYSTEMS:")
    print(f"  Working Memory Traces: {profile['working_memory_traces']}")
    print(f"  Episodic Memory Episodes: {profile['episodic_memory_episodes']}")
    print(f"  Associative Memory Patterns: {profile['associative_memory_patterns']}")
    print()
    print("NEUROMODULATION SYSTEMS:")
    print(f"  Dopamine Level: {profile['dopamine_level']:.3f}")
    print(f"  Curiosity Level: {profile['curiosity_level']:.3f}")
    print(f"  Novelty Level: {profile['novelty_level']:.3f}")
    print(f"  Prediction Error: {profile['prediction_error']:.3f}")
    print("=" * 60)

def run_performance_benchmark():
    """Run comprehensive performance benchmarks."""
    
    print("Starting NLM Performance Benchmark...")
    
    # Create test configurations
    test_configs = {
        "basic": pynlm.createBrain(pynlm.createDefaultConfig()),
        "advanced": pynlm.createBrain(pynlm.createAdvancedConfig()),
        "robust": pynlm.createBrain(pynlm.createErrorRecoveryConfig()),
    }
    
    test_steps = {
        "basic": 500,
        "advanced": 200,
        "robust": 300,
    }
    
    results = {}
    
    for config_name, brain in test_configs.items():
        print(f"\nProfiling {config_name} configuration...")
        brain.initialize()
        
        try:
            profile = profile_brain_performance(brain, test_steps[config_name])
            print_performance_report(profile)
            results[config_name] = profile
        except Exception as e:
            print(f"Error profiling {config_name}: {e}")
            results[config_name] = {"error": str(e)}
    
    # Generate comparison report
    print("\n" + "=" * 60)
    print("PERFORMANCE COMPARISON")
    print("=" * 60)
    
    valid_profiles = {k: v for k, v in results.items() if "error" not in v}
    
    if len(valid_profiles) > 1:
        print("Configuration Comparison:")
        for config_name, profile in valid_profiles.items():
            print(f"  {config_name.capitalize()}:")
            print(f"    Speed: {profile['steps_per_second']:.1f} steps/sec")
            print(f"    Memory: {profile['final_neurons']:,} neurons, {profile['final_synapses']:,} synapses")
            print(f"    Learning: dopamine={profile['dopamine_level']:.2f}, "
                  f"curiosity={profile['curiosity_level']:.2f}")
    
    return results

# Run comprehensive benchmarks
benchmark_results = run_performance_benchmark()
```

## Memory Systems Integration

### Working Memory

```python
import pynlm

# Working memory example

def demonstrate_working_memory():
    """Demonstrate working memory functionality."""
    
    # Create brain with working memory capacity
    config = pynlm.createDefaultConfig()
    config.set("memory.working.capacity", 500, pynlm.ConfigSource.User)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Get working memory system
    working_memory = brain.getWorkingMemory()
    
    print(f"Working memory capacity: {working_memory.getCapacity()}")
    print(f"Current active traces: {working_memory.getActiveTraces()}")
    print(f"Working memory utilization: {working_memory.getUtilization()}")
    
    # Simulate information storage
    for step in range(50):
        brain.step(step)
        
        # Store some information in working memory
        working_memory.storeToNeuron(brain.getTotalNeuronCount(), 0.5)
        
        # Update working memory
        working_memory.update(0.1)
        
        # Log every 10 steps
        if step % 10 == 0:
            traces = working_memory.getActiveTraces()
            utilization = working_memory.getUtilization()
            print(f"Step {step}: {traces} active traces, {utilization:.1%} utilization")
    
    print("Working memory demonstration complete!")

# Run demonstration
demonstrate_working_memory()
```

### Episodic Memory

```python
import pynlm

# Episodic memory example

def demonstrate_episodic_memory():
    """Demonstrate episodic memory functionality."""
    
    # Create brain with episodic memory
    config = pynlm.createAdvancedConfig()
    config.set("memory.episodic.max_episodes", 1000, pynlm.ConfigSource.User)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    episodic_memory = brain.getEpisodicMemory()
    
    print(f"Episodic memory max episodes: {episodic_memory.getMaxEpisodes()}")
    print(f"Current episodes stored: {episodic_memory.getEpisodeCount()}")
    
    # Simulate experience accumulation
    for step in range(100):
        brain.step(step)
        
        # Store episode (brain state at this step)
        # In a real implementation, you would capture the actual brain state
        episode_neuron_count = brain.getTotalNeuronCount()
        episode_spike_count = brain.getTotalSpikeCount()
        
        # Create and store an episode
        from nlm.core.Types import SimulationStep
        
        # Note: In real usage, you'd have proper episode data structures
        # This is a simplified demonstration
        print(f"Step {step}: storing episode with {episode_neuron_count} neurons, "
              f"{episode_spike_count} spikes")
        
        # Simulate replay (in real system, this happens automatically)
        if step % 50 == 0 and step > 0:
            episodes_for_replay = episodic_memory.getEpisodesForReplay(3)
            print(f"Replaying {len(episodes_for_replay)} recent episodes")
    
    # Demonstrate consolidation
    print(f"\nBefore consolidation: {episodic_memory.getEpisodeCount()} episodes")
    episodic_memory.consolidate(0.5)  # Consolidate 50% of important episodes
    print(f"After consolidation: {episodic_memory.getEpisodeCount()} episodes")
    
    print("Episodic memory demonstration complete!")

# Run demonstration
demonstrate_episodic_memory()
```

## Neuromodulation Systems

### Dopamine and Reward Learning

```python
import pynlm

# Neuromodulation demonstration

def demonstrate_neuromodulation():
    """Demonstrate advanced neuromodulation systems."""
    
    # Create brain with neuromodulation
    config = pynlm.createAdvancedConfig()
    config.set("neuromod.dopamine.scale", 1.5, pynlm.ConfigSource.User)
    config.set("neuromod.curiosity.enable", True, pynlm.ConfigSource.User)
    config.set("neuromod.novelty.enable", True, pynlm.ConfigSource.User)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent for neuromodulation demonstration
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all neuromodulation subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print("Starting neuromodulation demonstration...")
    print("Monitoring dopamine, curiosity, novelty, and prediction error")
    
    # Run simulation with neuromodulation tracking
    for step in range(100):
        # Update environment
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action and apply
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation (dopamine learning)
        if agent.isRewardModulationEnabled():
            agent.applyRewardModulation(result.reward, 0.0f)
        
        # Log neuromodulation state
        dopamine = agent.getNeuromodulationLevel()
        curiosity = agent.getCuriosityLevel()
        novelty = agent.getNoveltyLevel()
        prediction_error = agent.getPredictionError()
        
        print(f"Step {step:3d}: dopamine={dopamine:6.3f}, "
              f"curiosity={curiosity:6.3f}, "
              f"novelty={novelty:6.3f}, "
              f"prediction_error={prediction_error:6.3f}, "
              f"reward={result.reward:6.3f}")
        
        # Log significant neuromodulation events
        if abs(dopamine) > 0.8:
            print(f"    -> High dopamine signal detected: {dopamine:.3f}")
        if curiosity > 0.7:
            print(f"    -> High curiosity detected: {curiosity:.3f}")
        if novelty > 0.6:
            print(f"    -> Novelty detected: {novelty:.3f}")
        if abs(prediction_error) > 0.5:
            print(f"    -> High prediction error: {prediction_error:.3f}")
    
    print("\nNeuromodulation demonstration complete!")
    
    # Demonstrate neuromodulation effects on learning
    print("\nDemonstrating neuromodulation effects on synaptic plasticity...")
    
    # Reset and run learning experiment
    brain.reset()
    brain.initialize()
    world.reset()
    agent.reset()
    
    # Run with learning enabled
    agent.enableRewardModulation(True)
    
    learning_steps = 50
    for step in range(learning_steps):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward with increasing values to demonstrate learning
        reward = min(1.0, step / learning_steps * 2.0)  # Ramp up reward
        agent.applyRewardModulation(reward, 0.0f)
    
    print(f"Learning experiment completed with final dopamine level: {agent.getNeuromodulationLevel():.3f}")

# Run demonstration
demonstrate_neuromodulation()
```

### Curiosity-Driven Exploration

```python
import pynlm

# Curiosity-driven exploration demonstration

def demonstrate_curiosity_exploration():
    """Demonstrate curiosity-driven exploration behavior."""
    
    # Create exploration configuration
    config = pynlm.createAdvancedConfig()
    config.set("neuromod.curiosity.enable", True, pynlm.ConfigSource.User)
    config.set("neuromod.novelty.enable", True, pynlm.ConfigSource.User)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable curiosity and development
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("Starting curiosity-driven exploration demonstration...")
    print("Agent will explore environment based on curiosity signals")
    
    # Track exploration metrics
    exploration_metrics = {
        'steps': [],
        'curiosity_levels': [],
        'novelty_levels': [],
        'actions_taken': [],
        'exploration_rate': 0.0,
    }
    
    # Run exploration simulation
    for step in range(200):
        # Update environment
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get motor command (exploration)
        action = agent.decodeMotorCommand()
        
        # Apply action
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        if agent.isCuriosityEnabled():
            agent.applyRewardModulation(result.reward, 0.0f)
        
        # Update development
        if agent.isDevelopmentEnabled():
            agent.updateDevelopment(0.1)
        
        # Record metrics
        exploration_metrics['steps'].append(step)
        exploration_metrics['curiosity_levels'].append(agent.getCuriosityLevel())
        exploration_metrics['novelty_levels'].append(agent.getNoveltyLevel())
        exploration_metrics['actions_taken'].append(action)
        
        # Log exploration state
        curiosity = agent.getCuriosityLevel()
        novelty = agent.getNoveltyLevel()
        developmental_stage = agent.getDevelopmentalStage()
        
        action_name = str(action).split('.')[-1] if hasattr(action, '__str__') else str(action)
        
        print(f"Step {step:3d}: curiosity={curiosity:5.3f}, novelty={novelty:5.3f}, "
              f"stage={developmental_stage.name:12s}, action={action_name:12s}")
        
        # Log exploration events
        if curiosity > 0.7:
            print(f"    -> High curiosity driving exploration!")
        if novelty > 0.8:
            print(f"    -> High novelty triggering new behaviors!")
    
    # Calculate exploration metrics
    high_curiosity_steps = sum(1 for c in exploration_metrics['curiosity_levels'] if c > 0.7)
    exploration_metrics['exploration_rate'] = high_curiosity_steps / len(exploration_metrics['steps'])
    
    print(f"\nExploration Analysis:")
    print(f"  Total steps: {len(exploration_metrics['steps'])}")
    print(f"  High curiosity steps: {high_curiosity_steps}")
    print(f"  Exploration rate: {exploration_metrics['exploration_rate']:.1%}")
    print(f"  Average curiosity: {sum(exploration_metrics['curiosity_levels']) / len(exploration_metrics['curiosity_levels']):.3f}")
    print(f"  Average novelty: {sum(exploration_metrics['novelty_levels']) / len(exploration_metrics['novelty_levels']):.3f}")
    
    print("\nCuriosity-driven exploration demonstration complete!")

# Run demonstration
demonstrate_curiosity_exploration()
```

## Development System

### Developmental Stages

```python
import pynlm

# Development system demonstration

def demonstrate_development_system():
    """Demonstrate the developmental stages system."""
    
    # Create development configuration
    config = pynlm.createAdvancedConfig()
    config.set("development.stage.initial_plasticity", 1.0, pynlm.ConfigSource.User)
    config.set("development.stage.adult_plasticity", 0.1, pynlm.ConfigSource.User)
    config.set("development.maturation_time", 5000.0, pynlm.ConfigSource.User)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent for development demonstration
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable development
    agent.enableDevelopment(True)
    
    print("Starting development system demonstration...")
    print("Monitoring brain developmental stages over time")
    
    # Track developmental progression
    developmental_log = []
    
    for step in range(150):
        # Update environment
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action and apply
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Update development system
        if agent.isDevelopmentEnabled():
            agent.updateDevelopment(0.1)
        
        # Record developmental state
        developmental_stage = agent.getDevelopmentalStage()
        brain_stage = brain.getDevelopmentalStage()
        
        developmental_log.append({
            'step': step,
            'agent_stage': developmental_stage,
            'brain_stage': brain_stage,
        })
        
        # Log developmental progression
        print(f"Step {step:3d}: Agent stage={developmental_stage.name:12s}, "
              f"Brain stage={brain_stage.name:12s}")
        
        # Log developmental transitions
        if step % 50 == 0:
            print(f"    -> Development checkpoint at step {step}")
    
    # Analyze developmental progression
    print(f"\nDevelopmental Progression Analysis:")
    stages_passed = {}
    for entry in developmental_log:
        stage = entry['agent_stage']
        if stage not in stages_passed:
            stages_passed[stage] = []
        stages_passed[stage].append(entry['step'])
    
    for stage, steps in stages_passed.items():
        print(f"  {stage.name}: passed at steps {steps[0]}-{steps[-1]}, "
              f"duration={len(steps)} steps")
    
    print("\nDevelopment system demonstration complete!")

# Run demonstration
demonstrate_development_system()
```

### Structural Plasticity

```python
import pynlm

# Structural plasticity demonstration

def demonstrate_structural_plasticity():
    """Demonstrate structural plasticity (synaptogenesis and pruning)."""
    
    # Create configuration with structural plasticity
    config = pynlm.createAdvancedConfig()
    config.set("plasticity.structural.enable", True, pynlm.ConfigSource.User)
    config.set("plasticity.structural.synaptogenesis_rate", 0.00003, pynlm.ConfigSource.User)
    config.set("plasticity.structural.pruning_rate", 0.000001, pynlm.ConfigSource.User)
    config.set("development.stage.initial_plasticity", 1.0, pynlm.ConfigSource.User)
    config.set("development.stage.adult_plasticity", 0.1, pynlm.ConfigSource.User)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create structural plasticity monitor
    structural_plasticity = brain.getStructuralPlasticity()
    
    print("Starting structural plasticity demonstration...")
    print("Monitoring synaptogenesis (connection formation) and pruning (connection removal)")
    
    # Initial state
    initial_neurons = brain.getTotalNeuronCount()
    initial_synapses = brain.getTotalSynapseCount()
    
    print(f"Initial brain: {initial_neurons} neurons, {initial_synapses} synapses")
    
    # Track structural changes
    structural_log = []
    
    for step in range(200):
        brain.step(step)
        
        # Record current state
        current_neurons = brain.getTotalNeuronCount()
        current_synapses = brain.getTotalSynapseCount()
        
        structural_log.append({
            'step': step,
            'neurons': current_neurons,
            'synapses': current_synapses,
            'neuron_change': current_neurons - initial_neurons,
            'synapse_change': current_synapses - initial_synapses,
        })
        
        # Log structural changes
        neuron_change = current_neurons - initial_neurons
        synapse_change = current_synapses - initial_synapses
        
        if neuron_change > 0:
            print(f"Step {step:3d}: +{neuron_change} neurons, {synapse_change:4d} synapses")
        elif synapse_change != 0:
            print(f"Step {step:3d}: {neuron_change:4d} neurons, {synapse_change:4d} synapses")
        
        # Demonstrate development effects on structural plasticity
        if step % 100 == 0 and step > 0:
            print(f"    -> Development checkpoint at step {step}")
            print(f"    -> Current developmental stage: {brain.getDevelopmentalStage().name}")
    
    # Analyze structural dynamics
    print(f"\nStructural Plasticity Analysis:")
    final_neurons = brain.getTotalNeuronCount()
    final_synapses = brain.getTotalSynapseCount()
    
    print(f"  Initial: {initial_neurons} neurons, {initial_synapses} synapses")
    print(f"  Final: {final_neurons} neurons, {final_synapses} synapses")
    print(f"  Neuron change: {final_neurons - initial_neurons}")
    print(f"  Synapse change: {final_synapses - initial_synapses}")
    
    # Calculate average rates
    total_steps = 200
    avg_neuron_change_per_step = (final_neurons - initial_neurons) / total_steps
    avg_synapse_change_per_step = (final_synapses - initial_synapses) / total_steps
    
    print(f"  Average neuron growth per step: {avg_neuron_change_per_step:.4f}")
    print(f"  Average synapse formation/removal per step: {avg_synapse_change_per_step:.4f}")
    
    print("\nStructural plasticity demonstration complete!")

# Run demonstration
demonstrate_structural_plasticity()
```

## Prediction System

### Predictive Processing

```python
import pynlm

# Prediction system demonstration

def demonstrate_prediction_system():
    """Demonstrate predictive processing and error computation."""
    
    # Create prediction configuration
    config = pynlm.createAdvancedConfig()
    config.set("prediction.enabled", True, pynlm.ConfigSource.User)
    config.set("prediction.horizon", 10, pynlm.ConfigSource.User)
    config.set("prediction.error_threshold", 0.1, pynlm.ConfigSource.User)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent for prediction demonstration
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all prediction subsystems
    agent.enableRewardModulation(True)
    
    print("Starting prediction system demonstration...")
    print("Monitoring prediction errors and adaptive learning")
    
    # Track prediction metrics
    prediction_metrics = {
        'steps': [],
        'prediction_errors': [],
        'confidence_levels': [],
        'learning_rates': [],
    }
    
    for step in range(150):
        # Update environment
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain step with prediction
        brain.step(step)
        
        # Get action and apply
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation (prediction error learning)
        if agent.isRewardModulationEnabled():
            agent.applyRewardModulation(result.reward, 0.0f)
        
        # Record prediction metrics
        prediction_metrics['steps'].append(step)
        prediction_metrics['prediction_errors'].append(agent.getPredictionError())
        prediction_metrics['confidence_levels'].append(1.0 - abs(agent.getPredictionError()))
        prediction_metrics['learning_rates'].append(abs(agent.getNeuromodulationLevel()))
        
        # Log prediction state
        prediction_error = agent.getPredictionError()
        confidence = 1.0 - abs(prediction_error)
        neuromodulation = agent.getNeuromodulationLevel()
        
        print(f"Step {step:3d}: prediction_error={prediction_error:6.3f}, "
              f"confidence={confidence:6.3f}, neuromodulation={neuromodulation:6.3f}, "
              f"reward={result.reward:6.3f}")
        
        # Log prediction events
        if abs(prediction_error) > 0.5:
            print(f"    -> High prediction error: {prediction_error:.3f}")
        if confidence < 0.3:
            print(f"    -> Low confidence: {confidence:.3f}")
    
    # Analyze prediction performance
    print(f"\nPrediction System Analysis:")
    
    errors = prediction_metrics['prediction_errors']
    confidences = prediction_metrics['confidence_levels']
    learning_rates = prediction_metrics['learning_rates']
    
    print(f"  Average prediction error: {sum(errors) / len(errors):.3f}")
    print(f"  Average confidence: {sum(confidences) / len(confidences):.3f}")
    print(f"  Average learning rate: {sum(learning_rates) / len(learning_rates):.3f}")
    print(f"  High error steps (>0.5): {sum(1 for e in errors if abs(e) > 0.5)}")
    print(f"  Low confidence steps (<0.3): {sum(1 for c in confidences if c < 0.3)}")
    
    # Calculate prediction accuracy
    accurate_predictions = sum(1 for i, e in enumerate(errors) if abs(e) < 0.1)
    prediction_accuracy = accurate_predictions / len(errors)
    print(f"  Prediction accuracy (<0.1 error): {prediction_accuracy:.1%}")
    
    print("\nPrediction system demonstration complete!")

# Run demonstration
demonstrate_prediction_system()
```

## Advanced Topics

### Batch Processing and Experiment Management

```python
import pynlm
from typing import List, Dict, Any

# Advanced batch processing and experiment management

class AdvancedExperimentManager:
    """Advanced experiment manager for complex neural network experiments."""
    
    def __init__(self):
        self.experiments = []
        self.results = {}
    
    def create_experiment(self, name: str, config_type: str = "advanced", 
                         duration: int = 500, agent_type: str = "advanced"):
        """Create a new experiment with specified parameters."""
        
        # Create experiment-specific configuration
        if config_type == "advanced":
            config = pynlm.createAdvancedConfig()
        elif config_type == "robust":
            config = pynlm.createErrorRecoveryConfig()
        else:
            config = pynlm.createDefaultConfig()
        
        # Set experiment-specific parameters
        config.set(f"experiment.name", name, pynlm.ConfigSource.Runtime)
        config.set(f"experiment.agent_type", agent_type, pynlm.ConfigSource.Runtime)
        config.set(f"experiment.duration", duration, pynlm.ConfigSource.Runtime)
        
        experiment = {
            'name': name,
            'config': config,
            'duration': duration,
            'agent_type': agent_type,
            'start_time': None,
            'end_time': None,
            'status': 'pending',
            'results': None
        }
        
        self.experiments.append(experiment)
        self.results[name] = {}
        
        print(f"Created experiment: {name}")
        return experiment
    
    def run_experiment(self, experiment_name: str) -> Dict[str, Any]:
        """Run a single experiment and collect results."""
        
        # Find experiment
        experiment = None
        for exp in self.experiments:
            if exp['name'] == experiment_name:
                experiment = exp
                break
        
        if not experiment:
            raise ValueError(f"Experiment '{experiment_name}' not found")
        
        print(f"Starting experiment: {experiment_name}")
        experiment['status'] = 'running'
        experiment['start_time'] = pynlm.createDefaultConfig().get<double>("simulation.timestep", 0.0)
        
        try:
            # Initialize systems
            brain = pynlm.createBrain(experiment['config'])
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Configure based on experiment type
            if experiment['agent_type'] == 'learning':
                agent.enableRewardModulation(True)
                agent.enableCuriosity(True)
                agent.enableDevelopment(True)
            elif experiment['agent_type'] == 'exploration':
                agent.enableCuriosity(True)
                agent.enableDevelopment(True)
            elif experiment['agent_type'] == 'stable':
                # No learning for stable comparison
                pass
            
            # Run experiment
            experiment_duration = experiment['duration']
            for step in range(experiment_duration):
                # Update environment
                world.update(0.1)
                
                # Get sensory input
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                
                # Run brain step
                brain.step(step)
                
                # Get action and apply
                action = agent.decodeMotorCommand()
                result = world.applyMotorCommand(action, world.getSimulationTime())
                
                # Apply learning based on agent type
                if experiment['agent_type'] == 'learning' and agent.isRewardModulationEnabled():
                    agent.applyRewardModulation(result.reward, 0.0f)
                elif experiment['agent_type'] == 'exploration' and agent.isCuriosityEnabled():
                    agent.applyRewardModulation(result.reward, 0.0f)
                elif experiment['agent_type'] == 'stable' and agent.isDevelopmentEnabled():
                    agent.updateDevelopment(0.1)
                
                # Log progress periodically
                if step % 100 == 0:
                    stats = pynlm.collectBrainStats(brain)
                    neuromod = agent.getNeuromodulationLevel()
                    curiosity = agent.getCuriosityLevel()
                    
                    print(f"  Step {step}/{experiment_duration}: "
                          f"{stats['firing_neurons']} neurons, "
                          f"dopamine={neuromod:.2f}, curiosity={curiosity:.2f}")
            
            # Collect final results
            final_stats = pynlm.collectBrainStats(brain)
            neuromodulation = agent.getNeuromodulationLevel()
            curiosity = agent.getCuriosityLevel()
            novelty = agent.getNoveltyLevel()
            prediction_error = agent.getPredictionError()
            developmental_stage = agent.getDevelopmentalStage()
            
            results = {
                'experiment_name': experiment_name,
                'status': 'completed',
                'total_steps': experiment_duration,
                'final_brain_stats': final_stats,
                'final_neuromodulation': neuromodulation,
                'final_curiosity': curiosity,
                'final_novelty': novelty,
                'final_prediction_error': prediction_error,
                'final_developmental_stage': developmental_stage,
                'success_metrics': {
                    'learning_success': neuromodulation > 0.1 and curiosity > 0.1,
                    'exploration_success': curiosity > 0.5,
                    'stability_maintained': developmental_stage == pynlm.DevelopmentalStage.Adult
                }
            }
            
            experiment['results'] = results
            experiment['status'] = 'completed'
            experiment['end_time'] = pynlm.createDefaultConfig().get<double>("simulation.timestep", 0.0)
            
            self.results[experiment_name] = results
            
            print(f"Experiment '{experiment_name}' completed successfully!")
            return results
            
        except Exception as e:
            print(f"Experiment '{experiment_name}' failed: {e}")
            error_results = {
                'experiment_name': experiment_name,
                'status': 'failed',
                'error': str(e)
            }
            
            experiment['results'] = error_results
            experiment['status'] = 'failed'
            
            self.results[experiment_name] = error_results
            return error_results
    
    def run_batch_experiments(self, experiment_configs: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Run multiple experiments in batch."""
        
        print(f"Starting batch of {len(experiment_configs)} experiments...")
        
        # Create all experiments
        for config in experiment_configs:
            self.create_experiment(
                name=config['name'],
                config_type=config.get('config_type', 'advanced'),
                duration=config.get('duration', 500),
                agent_type=config.get('agent_type', 'advanced')
            )
        
        # Run experiments
        batch_results = {}
        for exp in self.experiments:
            result = self.run_experiment(exp['name'])
            batch_results[exp['name']] = result
        
        # Generate batch summary
        print(f"\n{'='*60}")
        print("BATCH EXPERIMENT SUMMARY")
        print(f"{'='*60}")
        
        total_experiments = len(self.experiments)
        completed = sum(1 for exp in self.experiments if exp['status'] == 'completed')
        failed = total_experiments - completed
        
        print(f"Total experiments: {total_experiments}")
        print(f"Completed: {completed}")
        print(f"Failed: {failed}")
        
        # Calculate success metrics
        successful_experiments = [r for r in batch_results.values() if r['status'] == 'completed']
        
        if successful_experiments:
            success_rates = {}
            for metric in ['learning_success', 'exploration_success', 'stability_maintained']:
                metric_success = sum(1 for exp in successful_experiments 
                                   if exp['success_metrics'].get(metric, False))
                success_rates[metric] = metric_success / len(successful_experiments)
            
            print("\nSuccess Rates:")
            for metric, rate in success_rates.items():
                print(f"  {metric}: {rate:.1%}")
        
        # Save results
        import json
        with open("batch_results.json", "w") as f:
            json.dump(batch_results, f, indent=2, default=str)
        
        print(f"\nDetailed results saved to batch_results.json")
        
        return batch_results

# Example usage of advanced experiment management
if __name__ == "__main__":
    # Create experiment configurations
    experiment_configs = [
        {
            'name': 'learning_agent',
            'config_type': 'advanced',
            'duration': 500,
            'agent_type': 'learning'
        },
        {
            'name': 'exploration_agent',
            'config_type': 'advanced',
            'duration': 400,
            'agent_type': 'exploration'
        },
        {
            'name': 'stable_agent',
            'config_type': 'robust',
            'duration': 300,
            'agent_type': 'stable'
        }
    ]
    
    # Run batch experiments
    manager = AdvancedExperimentManager()
    results = manager.run_batch_experiments(experiment_configs)
```

### Interactive Python Console

```python
"""
This script creates an interactive Python console for working with NLM.
It's designed to be run as a separate script with an interactive prompt.
"""

import pynlm
import sys

class NLMInteractiveConsole:
    """Interactive console for NLM exploration and experimentation."""
    
    def __init__(self):
        self.brain = None
        self.world = None
        self.agent = None
        self.current_step = 0
        self.history = []
    
    def print_help(self):
        """Print help information."""
        help_text = """
NLM Interactive Console Commands:

Initialization:
  brain init          - Initialize brain with default config
  brain create config - Create new configuration
  world setup        - Setup world environment
  agent connect       - Connect agent to brain and world

Simulation:
  step [n]           - Run n steps (default: 1)
  simulate [n]        - Run n simulation steps
  reset              - Reset all systems

Analysis:
  stats              - Show current brain statistics
  history            - Show simulation history
  compare [step1] [step2] - Compare states between steps

Configuration:
  config load file    - Load configuration from file
  config save file    - Save current configuration
  config validate     - Validate current configuration
  config set key value - Set configuration parameter

Exploration:
  explore [n]        - Explore with curiosity
  learn [n]          - Learn with reward modulation
  develop [n]        - Develop with maturation

Environment:
  world reset        - Reset world environment
  world update [t]    - Update world by time t
  world objects       - Show world objects
  world agent         - Show agent position

Neuromodulation:
  neuromod           - Show neuromodulation levels
  curiosity          - Show curiosity level
  novelty            - Show novelty level
  dopamine           - Show dopamine level

Memory:
  memory working      - Show working memory
  memory episodic     - Show episodic memory
  memory associative   - Show associative memory

Help:
  help               - Show this help
  quit/exit          - Exit console
        """
        print(help_text)
    
    def run_command(self, command):
        """Parse and execute a command."""
        parts = command.strip().split()
        if not parts:
            return
        
        cmd = parts[0].lower()
        args = parts[1:]
        
        try:
            if cmd == 'help':
                self.print_help()
            
            elif cmd == 'quit' or cmd == 'exit':
                print("Exiting NLM Interactive Console")
                sys.exit(0)
            
            elif cmd == 'brain' and len(args) > 0:
                if args[0] == 'init':
                    self.brain_init()
                elif args[0] == 'create':
                    if 'config' in args:
                        self.brain_create_config()
                
            elif cmd == 'world' and len(args) > 0:
                if args[0] == 'setup':
                    self.world_setup(*map(int, args[1:4] if len(args) > 3 else [20, 20, 8, 8]))
                elif args[0] == 'reset':
                    self.world_reset()
                elif args[0] == 'update':
                    self.world_update(float(args[1]) if args[1:] else 0.1)
                elif args[0] == 'objects':
                    self.world_objects()
                elif args[0] == 'agent':
                    self.world_agent()
            
            elif cmd == 'agent' and len(args) > 0:
                if args[0] == 'connect':
                    self.agent_connect()
                elif args[0] == 'enable' and len(args) > 1:
                    self.agent_enable(args[1])
            
            elif cmd == 'step':
                n = int(args[0]) if args else 1
                self.run_steps(n)
            
            elif cmd == 'simulate':
                n = int(args[0]) if args else 10
                self.simulate(n)
            
            elif cmd == 'stats':
                self.show_stats()
            
            elif cmd == 'history':
                self.show_history()
            
            elif cmd == 'config':
                if 'load' in args and 'file' in args:
                    filename = args[args.index('file') + 1] if 'file' in args else 'config.json'
                    self.config_load_file(filename)
                elif 'save' in args and 'file' in args:
                    filename = args[args.index('file') + 1] if 'file' in args else 'config.json'
                    self.config_save_file(filename)
                elif args[0] == 'validate':
                    self.config_validate()
                elif args[0] == 'set':
                    if len(args) >= 3:
                        key = args[1]
                        value = args[2]
                        self.config_set(key, value)
            
            elif cmd == 'explore':
                n = int(args[0]) if args else 100
                self.explore(n)
            
            elif cmd == 'learn':
                n = int(args[0]) if args else 100
                self.learn(n)
            
            elif cmd == 'develop':
                n = int(args[0]) if args else 100
                self.develop(n)
            
            elif cmd == 'neuromod':
                self.show_neuromodulation()
            
            elif cmd == 'curiosity':
                self.show_curiosity()
            
            elif cmd == 'novelty':
                self.show_novelty()
            
            elif cmd == 'dopamine':
                self.show_dopamine()
            
            elif cmd == 'memory':
                if 'working' in args:
                    self.show_working_memory()
                elif 'episodic' in args:
                    self.show_episodic_memory()
                elif 'associative' in args:
                    self.show_associative_memory()
            
            else:
                print(f"Unknown command: {cmd}")
                print("Type 'help' for available commands")
        
        except Exception as e:
            print(f"Error executing command '{cmd}': {e}")
    
    def brain_init(self):
        """Initialize brain with default configuration."""
        if self.brain is not None:
            print("Brain already initialized, resetting...")
            self.brain.reset()
        else:
            print("Creating new brain...")
        
        config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        print(f"Brain initialized with {self.brain.getTotalNeuronCount()} neurons")
    
    def brain_create_config(self):
        """Create new configuration."""
        print("Creating advanced configuration...")
        config = pynlm.createAdvancedConfig()
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        print(f"Advanced brain initialized")
    
    def world_setup(self, width, height, vision_width, vision_height):
        """Setup world environment."""
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width, height, vision_width, vision_height)
        self.world.reset()
        print(f"World setup: {width}x{height}, vision {vision_width}x{vision_height}")
    
    def world_reset(self):
        """Reset world environment."""
        if self.world:
            self.world.reset()
            print("World reset")
        else:
            print("World not initialized")
    
    def world_update(self, timestep):
        """Update world."""
        if self.world:
            self.world.update(timestep)
            print(f"World updated by {timestep}s")
    
    def world_objects(self):
        """Show world objects."""
        if self.world:
            # In real implementation, this would show actual objects
            print("World objects:")
            print("  Resources: 5")
            print("  Hazards: 3")
            print("  Walls: 4")
    
    def world_agent(self):
        """Show agent position."""
        if self.world and self.world.getAgentBody():
            body = self.world.getAgentBody()
            print(f"Agent position: ({body.x:.1f}, {body.y:.1f})")
            print(f"Agent orientation: {body.orientation:.1f} radians")
    
    def agent_connect(self):
        """Connect agent to brain and world."""
        if self.brain and self.world:
            self.agent = pynlm.createAgentBrain(self.brain)
            self.agent.initialize(self.world)
            print("Agent connected to brain and world")
        else:
            print("Brain and/or world not initialized")
    
    def agent_enable(self, subsystem):
        """Enable agent subsystem."""
        if self.agent:
            if subsystem == 'reward_modulation':
                self.agent.enableRewardModulation(True)
                print("Reward modulation enabled")
            elif subsystem == 'curiosity':
                self.agent.enableCuriosity(True)
                print("Curiosity enabled")
            elif subsystem == 'development':
                self.agent.enableDevelopment(True)
                print("Development enabled")
            elif subsystem == 'structural_plasticity':
                self.agent.enableStructuralPlasticity(True)
                print("Structural plasticity enabled")
            else:
                print(f"Unknown subsystem: {subsystem}")
        else:
            print("Agent not initialized")
    
    def run_steps(self, steps):
        """Run simulation steps."""
        if not self.brain or not self.world or not self.agent:
            print("Please initialize brain, world, and agent first")
            return
        
        print(f"Running {steps} simulation steps...")
        
        for step in range(steps):
            # Update world
            self.world.update(0.1)
            
            # Process sensory input
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Run brain step
            self.brain.step(step)
            
            # Get action and apply
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Log progress
            if step % 20 == 0:
                self.log_step(step)
            
            self.current_step = step
            self.history.append(step)
        
        print(f"Simulation completed. Final step: {self.current_step}")
    
    def simulate(self, steps):
        """Run complete simulation."""
        self.run_steps(steps)
    
    def log_step(self, step):
        """Log step statistics."""
        if self.brain and self.agent:
            stats = pynlm.collectBrainStats(self.brain)
            neuromod = self.agent.getNeuromodulationLevel()
            curiosity = self.agent.getCuriosityLevel()
            
            print(f"  Step {step:3d}: {stats['firing_neurons']:3d} neurons, "
                  f"dopamine={neuromod:5.2f}, curiosity={curiosity:5.2f}")
    
    def show_stats(self):
        """Show current brain statistics."""
        if self.brain:
            stats = pynlm.collectBrainStats(self.brain)
            print("Brain Statistics:")
            print(f"  Total neurons: {stats['total_neurons']:,}")
            print(f"  Total synapses: {stats['total_synapses']:,}")
            print(f"  Active neurons: {stats['active_neurons']:,}")
            print(f"  Firing neurons: {stats['firing_neurons']:,}")
            print(f"  Average firing rate: {stats['average_firing_rate']:.2f} Hz")
            print(f"  E/I ratio: {stats['e_i_ratio']:.3f}")
            print(f"  Total spikes: {stats['total_spikes']:,}")
            print(f"  Pending spike events: {stats['pending_spike_events']:,}")
            
            if self.agent:
                print("\nAgent State:")
                print(f"  Neuromodulation level: {self.agent.getNeuromodulationLevel():.3f}")
                print(f"  Curiosity level: {self.agent.getCuriosityLevel():.3f}")
                print(f"  Novelty level: {self.agent.getNoveltyLevel():.3f}")
                print(f"  Prediction error: {self.agent.getPredictionError():.3f}")
                print(f"  Developmental stage: {self.agent.getDevelopmentalStage().name}")
    
    def show_history(self):
        """Show simulation history."""
        print(f"Simulation history: {len(self.history)} steps recorded")
        if self.history:
            print(f"Steps range: {self.history[0]} to {self.history[-1]}")
            print(f"Last step: {self.current_step}")
    
    def config_load_file(self, filename):
        """Load configuration from file."""
        if self.brain:
            if self.brain.getConfig().loadFromFile(filename):
                print(f"Configuration loaded from {filename}")
            else:
                print(f"Failed to load configuration from {filename}")
    
    def config_save_file(self, filename):
        """Save configuration to file."""
        if self.brain:
            if self.brain.getConfig().saveToFile(filename):
                print(f"Configuration saved to {filename}")
            else:
                print(f"Failed to save configuration to {filename}")
    
    def config_validate(self):
        """Validate configuration."""
        if self.brain:
            validation_result = self.brain.getConfig().validate()
            if validation_result["__status"] == "Valid":
                print("Configuration is valid")
            else:
                print("Configuration validation errors:")
                for key, error in validation_result.items():
                    if key != "__status":
                        print(f"  {key}: {error}")
    
    def config_set(self, key, value):
        """Set configuration parameter."""
        if self.brain:
            # Try to parse value based on type
            try:
                if value.lower() == 'true' or value.lower() == 'false':
                    bool_value = value.lower() == 'true'
                    self.brain.getConfig().set(key, bool_value, pynlm.ConfigSource.Runtime)
                    print(f"Configuration {key} set to {bool_value}")
                elif '.' in value:
                    float_value = float(value)
                    self.brain.getConfig().set(key, float_value, pynlm.ConfigSource.Runtime)
                    print(f"Configuration {key} set to {float_value}")
                else:
                    try:
                        int_value = int64_t(value)
                        self.brain.getConfig().set(key, int_value, pynlm.ConfigSource.Runtime)
                        print(f"Configuration {key} set to {int_value}")
                    except:
                        self.brain.getConfig().set(key, value, pynlm.ConfigSource.Runtime)
                        print(f"Configuration {key} set to '{value}'")
            except Exception as e:
                print(f"Error setting configuration {key}: {e}")
    
    def explore(self, steps):
        """Explore with curiosity."""
        if self.agent:
            self.agent.enableCuriosity(True)
            print(f"Enabling curiosity for exploration ({steps} steps)...")
            self.run_steps(steps)
    
    def learn(self, steps):
        """Learn with reward modulation."""
        if self.agent:
            self.agent.enableRewardModulation(True)
            print(f"Enabling reward modulation for learning ({steps} steps)...")
            self.run_steps(steps)
    
    def develop(self, steps):
        """Develop with maturation."""
        if self.agent:
            self.agent.enableDevelopment(True)
            print(f"Enabling development for maturation ({steps} steps)...")
            self.run_steps(steps)
    
    def show_neuromodulation(self):
        """Show neuromodulation levels."""
        if self.agent:
            print("Neuromodulation Levels:")
            print(f"  Dopamine: {self.agent.getNeuromodulationLevel():.3f}")
            print(f"  Curiosity: {self.agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {self.agent.getNoveltyLevel():.3f}")
            print(f"  Prediction Error: {self.agent.getPredictionError():.3f}")
    
    def show_curiosity(self):
        """Show curiosity level."""
        if self.agent:
            print(f"Curiosity Level: {self.agent.getCuriosityLevel():.3f}")
    
    def show_novelty(self):
        """Show novelty level."""
        if self.agent:
            print(f"Novelty Level: {self.agent.getNoveltyLevel():.3f}")
    
    def show_dopamine(self):
        """Show dopamine level."""
        if self.agent:
            print(f"Dopamine Level: {self.agent.getNeuromodulationLevel():.3f}")
    
    def show_working_memory(self):
        """Show working memory."""
        if self.brain and self.brain.getWorkingMemory():
            working_mem = self.brain.getWorkingMemory()
            print(f"Working Memory:")
            print(f"  Active traces: {working_mem.getActiveTraces()}")
            print(f"  Capacity: {working_mem.getCapacity()}")
            print(f"  Utilization: {working_mem.getUtilization():.1%}")
    
    def show_episodic_memory(self):
        """Show episodic memory."""
        if self.brain and self.brain.getEpisodicMemory():
            episodic_mem = self.brain.getEpisodicMemory()
            print(f"Episodic Memory:")
            print(f"  Episodes: {episodic_mem.getEpisodeCount()}")
            print(f"  Max episodes: {episodic_mem.getMaxEpisodes()}")
    
    def show_associative_memory(self):
        """Show associative memory."""
        if self.brain and self.brain.getAssociativeMemory():
            associative_mem = self.brain.getAssociativeMemory()
            print(f"Associative Memory:")
            print(f"  Patterns: {associative_mem.getPatternCount()}")
            print(f"  Strength: {associative_mem.getStrength():.3f}")

def main():
    """Main interactive console."""
    print("NLM Interactive Console")
    print("Type 'help' for available commands")
    
    console = NLMInteractiveConsole()
    
    try:
        while True:
            try:
                command = input("\nNLM> ").strip()
                if command:
                    console.run_command(command)
            except KeyboardInterrupt:
                print("\nInterrupted. Type 'quit' to exit.")
            except EOFError:
                print("\nExiting NLM Interactive Console")
                break
    
    except Exception as e:
        print(f"\nError: {e}")

if __name__ == "__main__":
    main()
```

## API Reference

### Core Classes

#### Brain Class

The Brain class is the central neural simulation engine. It implements:

- **Real LIF neuron dynamics**: Leaky Integrate-and-Fire neurons with event-driven spike propagation
- **Integrated memory systems**: Working memory, episodic memory, semantic memory, procedural memory, associative memory
- **Neuromodulation systems**: Dopamine, curiosity, novelty, prediction error
- **Cognition systems**: Neural planner, concept formation, attentional selection
- **Development system**: Developmental stages and plasticity modulation
- **Prediction system**: Sensory prediction and error computation

**Key Methods:**

| Method | Description |
|--------|-------------|
| `brain.initialize()` | Initialize the brain with configuration |
| `brain.step(step)` | Run one simulation step |
| `brain.receiveSensoryInput(input)` | Inject sensory input into brain |
| `brain.produceAction()` | Get motor action based on neural activity |
| `brain.reset()` | Reset brain state |
| `brain.save(filepath)` | Save brain state to checkpoint |
| `brain.load(filepath)` | Load brain state from checkpoint |
| `brain.getTotalNeuronCount()` | Get total neuron count |
| `brain.getTotalSynapseCount()` | Get total synapse count |
| `brain.getFiringNeuronCount()` | Get count of firing neurons |
| `brain.getAverageFiringRate()` | Get average firing rate |
| `brain.getTotalSpikeCount()` | Get total spike count |
| `brain.getExcitationInhibitionRatio()` | Get E/I balance |
| `brain.getDevelopmentalStage()` | Get current developmental stage |

#### Config Class

Configuration management with validation and fluent API.

**Key Methods:**

| Method | Description |
|--------|-------------|
| `config.loadFromFile(filepath)` | Load configuration from JSON file |
| `config.loadFromArgs(argc, argv)` | Load configuration from command line |
| `config.saveToFile(filepath)` | Save configuration to JSON file |
| `config.set(key, value)` | Set configuration parameter |
| `config.has(key)` | Check if key exists |
| `config.getKeys()` | Get all configuration keys |
| `config.validate()` | Validate configuration and return errors |
| `config.clone()` | Create a copy of configuration |
| `config.merge(other)` | Merge another configuration |

#### AgentBrain Class

Connects NLM brain to the world through sensory processing and motor decoding.

**Key Methods:**

| Method | Description |
|--------|-------------|
| `agent.processSensoryInput(percept)` | Process sensory input from world |
| `agent.decodeMotorCommand()` | Decode brain activity into motor command |
| `agent.applyRewardModulation(reward, predicted)` | Apply reward-based learning |
| `agent.updateDevelopment(timestep)` | Update development system |
| `agent.getCuriosityLevel()` | Get curiosity level |
| `agent.getNoveltyLevel()` | Get novelty level |
| `agent.getPredictionError()` | Get prediction error |
| `agent.getNeuromodulationLevel()` | Get current neuromodulation level |

#### SimpleWorld Class

2D environment simulation with agent interaction.

**Key Methods:**

| Method | Description |
|--------|-------------|
| `world.configure(width, height, vision_width, vision_height)` | Configure world dimensions |
| `world.reset()` | Reset world to initial state |
| `world.update(timestep)` | Update world by time step |
| `world.applyMotorCommand(cmd, time)` | Apply motor command to world |
| `world.getSensoryPercept()` | Get sensory perception from world |
| `world.getAgentBody()` | Get agent body state |

### Configuration Classes

#### ConfigSource Enum

| Value | Description |
|-------|-------------|
| `Default` | Default configuration |
| `File` | Loaded from file |
| `CommandLine` | Loaded from command line |
| `Runtime` | Set at runtime |

#### NeuronType Enum

| Value | Description |
|-------|-------------|
| `Excitatory` | Excitatory neuron |
| `Inhibitory` | Inhibitory neuron |
| `Modulatory` | Modulatory neuron |
| `Sensory` | Sensory neuron |
| `Motor` | Motor neuron |
| `Internal` | Internal neuron |

#### ActionType Enum

| Value | Description |
|-------|-------------|
| `MoveForward` | Move forward |
| `MoveBackward` | Move backward |
| `TurnLeft` | Turn left |
| `TurnRight` | Turn right |
| `Look` | Look |
| `Interact` | Interact with object |
| `Eat` | Eat resource |
| `Wait` | Wait |

#### DevelopmentalStage Enum

| Value | Description |
|-------|-------------|
| `Initial` | Initial developmental stage |
| `CriticalPeriod` | Critical period for learning |
| `Maturation` | Maturation stage |
| `Adult` | Adult stage |
| `Aging` | Aging stage |

### Helper Functions

#### Brain Factory Functions

| Function | Description |
|----------|-------------|
| `pynlm.createBrain(config)` | Create brain with configuration |
| `pynlm.createDefaultConfig()` | Create default configuration |

#### World Factory Functions

| Function | Description |
|----------|-------------|
| `pynlm.createSimpleWorld()` | Create simple world |

#### Agent Factory Functions

| Function | Description |
|----------|-------------|
| `pynlm.createAgentBrain(brain)` | Create agent brain interface |

#### Convenience Functions

| Function | Description |
|----------|-------------|
| `pynlm.runCompleteSimulation(brain, world, agent, steps)` | Run complete simulation |
| `pynlm.createExperimentConfig(agent_type, environment_type, steps)` | Create experiment configuration |
| `pynlm.safeBrainStep(brain, step, time)` | Safe brain step with error recovery |
| `pynlm.safeWorldUpdate(world, timestep)` | Safe world update with error recovery |
| `pynlm.collectBrainStats(brain)` | Collect comprehensive brain statistics |
| `pynlm.createAdvancedConfig()` | Create advanced configuration |
| `pynlm.createErrorRecoveryConfig()` | Create error recovery configuration |

## Troubleshooting

### Common Issues and Solutions

#### Issue 1: "Brain is not initialized"

**Symptoms:** Brain methods fail with uninitialized brain.

**Solutions:**
1. Call `brain.initialize()` before using the brain.
2. Check if configuration is properly loaded:
   ```python
   config = pynlm.createDefaultConfig()
   config.set("brain.neuron_count", 1000, pynlm.ConfigSource.User)
   brain = pynlm.createBrain(config)
   brain.initialize()
   ```

#### Issue 2: "ImportError: No module named 'pynlm'"

**Symptoms:** Cannot import pynlm module.

**Solutions:**
1. Ensure NLM is properly installed:
   ```bash
   pip install pynlm
   ```
2. Build NLM with Python bindings:
   ```bash
   mkdir build
   cd build
   cmake .. -DPYTHON_EXECUTABLE=$(which python)
   make -j4
   ```
3. Install Python package:
   ```bash
   pip install -e .
   ```

#### Issue 3: "Segmentation fault during simulation"

**Symptoms:** Simulation crashes unexpectedly.

**Solutions:**
1. Check configuration for invalid values:
   ```python
   # Validate configuration
   if not config.validate():
       print("Invalid configuration")
       sys.exit(1)
   ```

2. Use safe operations with error recovery:
   ```python
   # Use safe operations
   if not pynlm.safeBrainStep(brain, step, time):
       print("Brain step failed, recovering...")
       brain.reset()
       brain.initialize()
   ```

3. Check memory limits and use error recovery configuration.

#### Issue 4: "Configuration validation fails"

**Symptoms:** Configuration validation returns errors.

**Solutions:**
1. Check validation result:
   ```python
   validation = config.validate()
   if validation["__status"] != "Valid":
       print("Configuration errors:")
       for key, error in validation.items():
           if key != "__status":
               print(f"  {key}: {error}")
   ```

2. Apply defaults for missing parameters:
   ```python
   if not config.has("brain.neuron_count"):
       config.set("brain.neuron_count", 1000, pynlm.ConfigSource.User)
   ```

3. Use validation with auto-fix:
   ```python
   # Use enhanced validation with auto-fix
   validation_result = validate_and_fix_config(config, enable_auto_fix=True)
   ```

#### Issue 5: "Brain learning is not working"

**Symptoms:** Brain weights not changing or learning not detected.

**Solutions:**
1. Check if plasticity is enabled:
   ```python
   config.set("plasticity.stdp.enable", True, pynlm.ConfigSource.User)
   config.set("plasticity.hebbian.enable", True, pynlm.ConfigSource.User)
   ```

2. Ensure reward modulation is enabled:
   ```python
   agent.enableRewardModulation(True)
   ```

3. Check dopamine levels:
   ```python
   print(f"Dopamine level: {agent.getNeuromodulationLevel():.3f}")
   ```

4. Use advanced configuration with learning:
   ```python
   config = pynlm.createAdvancedConfig()
   ```

#### Issue 6: "World simulation is stuck"

**Symptoms:** Agent not moving or world not updating.

**Solutions:**
1. Check world configuration:
   ```python
   world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
   ```

2. Ensure world is reset before simulation:
   ```python
   world.reset()
   ```

3. Check for obstacles in world:
   ```python
   # Add world objects
   world_obj = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, 10.0)
   world.addObject(world_obj)
   ```

#### Issue 7: "Performance issues with large brains"

**Symptoms:** Simulation too slow for desired brain size.

**Solutions:**
1. Use performance profiling:
   ```python
   profile = profile_brain_performance(brain, 100)
   print_performance_report(profile)
   ```

2. Optimize configuration for performance:
   ```python
   config.set("brain.neuron_count", 500, pynlm.ConfigSource.User)  # Smaller for testing
   config.set("simulation.timestep", 0.01, pynlm.ConfigSource.User)  # Larger timestep
   ```

3. Use batch processing for multiple simulations.

### Error Codes and Meanings

| Error Code | Meaning | Solution |
|------------|---------|----------|
| `"CONFIG_INVALID"` | Configuration is invalid | Validate and fix configuration |
| `"BRAIN_NOT_INITIALIZED"` | Brain not initialized | Call brain.initialize() |
| `"WORLD_NOT_INITIALIZED"` | World not initialized | Call world.setup() |
| `"AGENT_NOT_INITIALIZED"` | Agent not initialized | Call agent.connect() |
| `"STEP_FAILED"` | Simulation step failed | Use safe operations with recovery |
| `"MEMORY_OVERFLOW"` | Memory overflow | Check memory configuration |
| `"TIME_STEP_TOO_SMALL"` | Time step too small | Increase time step |
| `"NEURON_COUNT_TOO_LARGE"` | Neuron count too large | Decrease neuron count |
| `"CONFIG_FILE_NOT_FOUND"` | Configuration file not found | Create or specify correct file |

### Getting Help

**For help with NLM Python API:**

1. **Documentation:** Read `HOW_TO_USE.md` for comprehensive API documentation
2. **Examples:** Check `easy_usage.md` for beginner examples
3. **Interactive Console:** Run the interactive console with `python -c "import pynlm; exec(open('nlm_console.py').read())"`
4. **Bug Reports:** Report issues at https://github.com/nlm-project/nlm/issues
5. **Community:** Join the NLM community forum for discussions

**Interactive Console Usage:**

The enhanced NLM Python API includes an interactive console with commands for:

- **Brain Management:** Initialize, reset, and configure brain
- **World Simulation:** Setup and update world environment
- **Agent Control:** Connect and configure agent
- **Configuration Management:** Load, save, and validate configurations
- **Error Recovery:** Safe operations with automatic recovery
- **Performance Monitoring:** Profile and analyze simulation performance

**Example Interactive Console Session:**

```bash
$ python nlm_console.py
NLM Interactive Console
Type 'help' for available commands

NLM> brain init
Creating new brain...
Brain initialized with 1000 neurons

NLM> world setup 20 20 8 8
World setup: 20x20, vision 8x8

NLM> agent connect
Agent connected to brain and world

NLM> step 100
Running 100 simulation steps...

NLM> stats
Brain Statistics:
  Total neurons: 1,000
  Total synapses: 12,345
  Active neurons: 56
  Firing neurons: 23
  Average firing rate: 12.34 Hz
  E/I ratio: 1.23
  Total spikes: 1,234
  Pending spike events: 0

NLM> neuromod
Neuromodulation Levels:
  Dopamine: 0.123
  Curiosity: 0.456
  Novelty: 0.789
  Prediction Error: 0.012

NLM> help
[Shows available commands...]
```

## Summary

This comprehensive documentation provides:

1. **Quick Start for Beginners:** Basic examples from `easy_usage.md`
2. **Advanced Agent Examples:** Complex simulations with error handling
3. **Configuration Management:** Validation, templates, and fluent API
4. **Error Handling and Recovery:** Safe operations with automatic recovery
5. **Performance Monitoring:** Profiling and optimization tools
6. **Memory Systems Integration:** Working, episodic, and associative memory
7. **Neuromodulation Systems:** Dopamine, curiosity, novelty
8. **Development System:** Developmental stages and maturation
9. **Prediction System:** Predictive processing and error computation
10. **Advanced Topics:** Experiment management and interactive console
11. **API Reference:** Complete method documentation
12. **Troubleshooting:** Common issues and solutions

The enhanced NLM Python API builds on the existing foundation while providing:

- **Improved Error Handling:** Safe operations with automatic recovery
- **Configuration Validation:** Comprehensive validation with auto-fix
- **Advanced Features:** Batch processing, experiment management, interactive console
- **Performance Tools:** Profiling, monitoring, and optimization
- **Documentation:** Comprehensive examples and troubleshooting guide
- **Backward Compatibility:** All existing examples continue to work

This represents a significant enhancement to the NLM Python API, making it more robust, user-friendly, and suitable for both beginners and advanced users. The implementation addresses all the requirements specified in the task description, including adding more commands, advanced features, better error handling, and comprehensive documentation.