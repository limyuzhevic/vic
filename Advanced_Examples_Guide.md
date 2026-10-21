# NLM - Neural Learning Machine: Advanced Examples Guide

## Overview

This guide provides comprehensive examples demonstrating the **enhanced capabilities** of the NLM (熙然) Phase 6 system. It covers practical implementation patterns, advanced configuration scenarios, and complex usage patterns for various user needs.

## Example 1: Complete Enhanced Simulation

```python
import pynlm
import time

def run_complete_enhanced_simulation(steps=5000):
    """Run a complete enhanced simulation with all Phase 6 features."""
    
    # Configure Phase 6 with all enhancements
    config = pynlm.createDefaultConfig()
    config.set("development.enable", True)
    config.set("neuromodulation.dopamine.enable", True)
    config.set("neuromodulation.curiosity.enable", True)
    config.set("memory.working_memory.capacity", 2000)
    config.set("memory.episodic_memory.capacity", 10000)
    config.set("prediction.enable", True)
    config.set("cognition.attention.enable", True)
    
    # Initialize enhanced brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create enhanced world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    world.setMaxEnergy(200.0)
    
    # Create enhanced agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all advanced features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    agent.enablePrediction(True)
    agent.enableWorkingMemory(True)
    agent.enableEpisodicMemory(True)
    agent.enableAttention(True)
    
    # Enhanced simulation loop
    start_time = time.time()
    metrics = []
    
    for step in range(steps):
        # World dynamics
        world.update(0.1)
        
        # Enhanced perception
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Enhanced neural processing
        brain.step(step)
        
        # Enhanced action selection
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Enhanced learning
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        # Collect metrics
        if step % 100 == 0:
            metrics.append({
                'step': step,
                'reward': agent.getTotalReward(),
                'firing_rate': brain.getAverageFiringRate(),
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'development_stage': brain.getDevelopmentalStage()
            })
            
            print(f"Step {step}: "
                  f"Reward={agent.getTotalReward():.4f}, "
                  f"Firing={brain.getAverageFiringRate():.2f}Hz, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}, "
                  f"Development={brain.getDevelopmentalStage()}")
    
    # Enhanced results
    elapsed = time.time() - start_time
    print(f"\n=== Enhanced Simulation Complete ===")
    print(f"Duration: {elapsed:.2f}s")
    print(f"Steps/sec: {steps/elapsed:.1f}")
    print(f"Final reward: {agent.getTotalReward():.4f}")
    print(f"Avg firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"Development stage: {brain.getDevelopmentalStage()}")
    print(f"Memory episodes: {brain.getEpisodicMemoryCount()}")
    print(f"Working memory usage: {brain.getWorkingMemoryUsage()}/{brain.getWorkingMemoryCapacity()}")
    
    return metrics
```

## Enhanced Examples

### Example 2: Enhanced Parameter Optimization

```python
import pynlm
import numpy as np
from multiprocessing import Pool

def evaluate_configuration(params):
    """Evaluate a specific brain configuration."""
    
    neuron_count, learning_rate, connection_prob, development_enabled = params
    
    # Create enhanced configuration
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", neuron_count)
    config.set("plasticity.stdp.learning_rate", learning_rate)
    config.set("plasticity.stdp.connection_probability", connection_prob)
    config.set("development.enable", development_enabled)
    config.set("neuromodulation.curiosity.enable", True)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    if not brain.initialize():
        return {'params': params, 'success': False}
    
    # Enhanced simulation
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableDevelopment(True)
    
    total_reward = 0.0
    for step in range(1000):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        total_reward += reward
    
    return {
        'params': params,
        'success': True,
        'performance': total_reward / 1000,
        'firing_rate': brain.getAverageFiringRate(),
        'development_stage': brain.getDevelopmentalStage()
    }
```

### Example 3: Enhanced Batch Operations

```python
import pynlm
from concurrent.futures import ThreadPoolExecutor

class EnhancedBatchProcessor:
    """Process multiple enhanced simulations in batch."""
    
    def run_enhanced_simulation_batch(self, simulation_configs):
        """Run multiple enhanced simulations in parallel."""
        
        print(f"Running enhanced batch simulation with {len(simulation_configs)} configurations...")
        
        with ThreadPoolExecutor(max_workers=3) as executor:
            future_to_config = {
                executor.submit(self.run_single_enhanced_simulation, config): config 
                for config in simulation_configs
            }
            
            results = []
            for future in future_to_config:
                config = future_to_config[future]
                try:
                    result = future.result()
                    results.append(result)
                    print(f"Completed: {config['name']}")
                except Exception as e:
                    print(f"Error in {config['name']}: {e}")
                    results.append({'config': config, 'success': False, 'error': str(e)})
            
            return results
```

### Example 4: Enhanced Configuration Management

```python
import pynlm
import json
from pathlib import Path

class EnhancedConfigurationManager:
    """Manage enhanced NLM configurations with validation and export."""
    
    def create_enhanced_config(self, name, **kwargs):
        """Create an enhanced configuration with defaults."""
        
        # Default Phase 6 enhanced configuration
        config = {
            'neuron_count': 1000,
            'steps': 5000,
            'regions': 4,
            'development': True,
            'checkpoint': False,
            'replay': False,
            'verbose': False,
            'enable_system_monitoring': True,
            'enable_profiling': False,
            'enable_statistics_export': True,
            'enable_scripted_ops': True,
            'auto_recover': True,
            'error_recovery_mode': 'graceful',
            'learning_rate': 0.01,
            'connection_probability': 0.1,
            'random_seeds': 5,
            'batch_mode': False,
            'parameter_sweep': False,
            'random_search': False,
            'multi_run_mode': False,
            'curiosity': True,
            'novelty': True,
            'prediction': True,
            'working_memory_capacity': 1000,
            'episodic_memory_capacity': 5000,
            **kwargs  # Override with user parameters
        }
        
        return config
```

This guide provides comprehensive examples for leveraging the enhanced Phase 6 capabilities of the NLM system.