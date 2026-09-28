# NLM Advanced Usage Guide

## Overview

This guide covers advanced usage patterns, optimization techniques, and complex simulation scenarios for experienced NLM users. It builds upon the beginner and intermediate concepts found in the basic usage guides.

## Advanced Python API Usage

### 1. Working with Plasticity Parameters

Now that STDP and StructuralPlasticity are properly exposed to Python, you can configure these parameters programmatically:

```python
import pynlm

# Create brain and get plasticity systems
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Access STDP system and configure it
stdp = brain.getSTDP()
if stdp:
    # Configure STDP parameters
    stdp.setLTPWeight(0.02)    # Long-term potentiation weight
    stdp.setLTDWeight(0.015)   # Long-term depression weight
    stdp.setTimeConstant(20.0) # STDP time constant
    print(f"STDP configured: LTP={stdp.getLTPWeight()}, LTD={stdp.getLTDWeight()}")

# Access structural plasticity system
sp = brain.getStructuralPlasticity()
if sp:
    # Configure structural plasticity rates
    sp.setSynaptogenesisRate(0.0002)
    sp.setPruningRate(0.00002)
    print(f"Structural plasticity: synaptogenesis={sp.getSynaptogenesisRate()}, pruning={sp.getPruningRate()}")
```

### 2. Advanced Agent Brain Configuration

The AgentBrain class supports advanced configuration patterns for complex agent behaviors:

```python
import pynlm
import numpy as np

def run_advanced_agent_simulation(num_steps=5000):
    """Advanced agent simulation with custom configuration and learning."""
    
    # Create configuration with advanced settings
    config = pynlm.createDefaultConfig()
    
    # Customize brain parameters
    config.set("brain.neuron_count", 2000)      # Larger network
    config.set("brain.synapse_density", 0.15)   # Higher connectivity
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    
    # Create and initialize systems
    brain = pynlm.createBrain(config)
    brain.initialize()
    agent = pynlm.createAgentBrain(brain)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    agent.initialize(world)
    
    # Enable all subsystems with custom parameters
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Advanced simulation loop with adaptive behavior
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process in brain with custom preprocessing
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Advanced motor decoding with confidence weighting
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply action with learning feedback
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Calculate reward with prediction error
        reward = calculate_advanced_reward(world, brain, agent)
        predicted = agent.getNeuromodulationLevel()
        agent.applyRewardModulation(reward, predicted)
        
        # Update development based on performance
        performance_metric = calculate_performance(brain, agent)
        agent.updateDevelopment(0.1 * performance_metric)
        
        # Log advanced metrics periodically
        if step % 500 == 0:
            log_advanced_metrics(step, brain, agent, world)
    
    return brain, agent, world

def calculate_advanced_reward(world, brain, agent):
    """Calculate reward based on multiple factors."""
    # Base reward from agent's neuromodulation level
    base_reward = agent.getNeuromodulationLevel()
    
    # Novelty bonus from high novelty levels
    novelty_bonus = agent.getNoveltyLevel() * 0.5
    
    # Survival bonus (energy > threshold)
    agent_body = world.getAgentBody()
    survival_bonus = 1.0 if agent_body.energy > 50.0 else 0.0
    
    # Exploration bonus (high curiosity)
    exploration_bonus = agent.getCuriosityLevel() * 0.3
    
    return base_reward + novelty_bonus + survival_bonus + exploration_bonus

def calculate_performance(brain, agent):
    """Calculate agent performance metric."""
    # Combine multiple performance indicators
    firing_rate = brain.getAverageFiringRate()
    curiosity = agent.getCuriosityLevel()
    novelty = agent.getNoveltyLevel()
    
    # Weighted performance score
    performance = (firing_rate * 0.4 + curiosity * 0.3 + novelty * 0.3)
    return performance

def log_advanced_metrics(step, brain, agent, world):
    """Log comprehensive metrics for analysis."""
    metrics = {
        "Step": step,
        "Firing Rate": f"{brain.getAverageFiringRate():.2f}",
        "Neurons Firing": brain.getFiringNeuronCount(),
        "Total Spikes": brain.getTotalSpikeCount(),
        "E/I Ratio": f"{brain.getExcitationInhibitionRatio():.2f}",
        "Curiosity Level": f"{agent.getCuriosityLevel():.3f}",
        "Novelty Level": f"{agent.getNoveltyLevel():.3f}",
        "Neuromodulation": f"{agent.getNeuromodulationLevel():.3f}",
        "Prediction Error": f"{agent.getPredictionError():.3f}",
        "Development Stage": str(brain.getDevelopmentalStage()),
        "Structural Plasticity": agent.isStructuralPlasticityEnabled(),
        "Reward Modulation": agent.isRewardModulationEnabled(),
        "Curiosity Enabled": agent.isCuriosityEnabled(),
    }
    
    print("Advanced Metrics:", metrics)
```

### 3. Batch Processing and Simulation Automation

Run multiple simulations efficiently with batch processing:

```python
import pynlm
import time
import json
from typing import List, Dict, Any

def run_parameter_sweep(config_template: Dict[str, Any], 
                       parameter_ranges: Dict[str, List],
                       num_simulations: int = 10) -> List[Dict]:
    """Run multiple simulations with different parameter combinations."""
    
    results = []
    total_start_time = time.time()
    
    for i in range(num_simulations):
        print(f"Running simulation {i+1}/{num_simulations}")
        
        # Create configuration with random parameters
        config = pynlm.createDefaultConfig()
        
        # Apply parameter ranges with random selection
        for param, values in parameter_ranges.items():
            selected_value = values[i % len(values)]
            config.set(param, selected_value)
        
        # Create and run simulation
        start_time = time.time()
        simulation_result = run_single_simulation(config)
        end_time = time.time()
        
        # Store results with parameters
        result = {
            "parameters": parameter_ranges,
            "final_metrics": simulation_result["metrics"],
            "learning_curve": simulation_result["learning_curve"],
            "simulation_time": end_time - start_time,
            "simulation_id": i
        }
        results.append(result)
    
    total_end_time = time.time()
    print(f"Total batch processing time: {total_end_time - total_start_time:.2f} seconds")
    
    return results

def run_single_simulation(config):
    """Run a single simulation with given configuration."""
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent and world
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Enable learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    learning_curve = []
    for step in range(1000):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        agent.applyRewardModulation(0.1, 0.0)  # Simple reward signal
        
        # Record metrics
        if step % 100 == 0:
            learning_curve.append({
                "step": step,
                "firing_rate": brain.getAverageFiringRate(),
                "curiosity": agent.getCuriosityLevel(),
                "novelty": agent.getNoveltyLevel(),
                "total_spikes": brain.getTotalSpikeCount()
            })
    
    # Final metrics
    metrics = {
        "final_firing_rate": brain.getAverageFiringRate(),
        "final_curiosity": agent.getCuriosityLevel(),
        "final_novelty": agent.getNoveltyLevel(),
        "final_reward_modulation": agent.getNeuromodulationLevel(),
        "total_spikes": brain.getTotalSpikeCount(),
        "development_stage": str(brain.getDevelopmentalStage())
    }
    
    return {"metrics": metrics, "learning_curve": learning_curve}

def analyze_batch_results(results: List[Dict]) -> Dict[str, Any]:
    """Analyze batch simulation results and generate statistics."""
    
    # Extract metrics from all simulations
    firing_rates = [r["final_metrics"]["final_firing_rate"] for r in results]
    curiosities = [r["final_metrics"]["final_curiosity"] for r in results]
    novelties = [r["final_metrics"]["final_novelty"] for r in results]
    
    # Calculate statistics
    analysis = {
        "parameter_analysis": {
            "firing_rate_mean": sum(firing_rates) / len(firing_rates),
            "firing_rate_std": (sum((x - sum(firing_rates)/len(firing_rates))**2 for x in firing_rates) / len(firing_rates))**0.5,
            "curiosity_mean": sum(curiosities) / len(curiosities),
            "novelty_mean": sum(novelties) / len(novelties),
        },
        "best_performers": [],
        "learning_patterns": []
    }
    
    # Find best performers based on composite score
    for result in results:
        metrics = result["final_metrics"]
        score = (metrics["final_firing_rate"] * 0.4 + 
                metrics["final_curiosity"] * 0.3 + 
                metrics["final_novelty"] * 0.3)
        result["composite_score"] = score
    
    top_results = sorted(results, key=lambda x: x["composite_score"], reverse=True)[:3]
    analysis["best_performers"] = top_results
    
    return analysis
```

### 4. Checkpoint and Save/Load Operations

Advanced checkpoint management for long-running simulations:

```python
import pynlm
import os
import glob

def run_persistent_simulation(checkpoint_interval: int = 1000,
                              checkpoint_dir: str = "./checkpoints"):
    """Run simulation with automatic checkpointing."""
    
    # Ensure checkpoint directory exists
    os.makedirs(checkpoint_dir, exist_ok=True)
    
    # Create brain with configuration
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=25, height=25, visionWidth=10, visionHeight=10)
    world.reset()
    agent.initialize(world)
    
    # Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Starting persistent simulation with checkpointing...")
    
    for step in range(10000):
        # Update world
        world.update(0.1)
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action and apply to world
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply learning
        reward = calculate_task_reward(world, agent)
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Checkpoint at intervals
        if step % checkpoint_interval == 0 and step > 0:
            checkpoint_path = f"{checkpoint_dir}/checkpoint_step_{step}.bin"
            if brain.save(checkpoint_path):
                print(f"Checkpoint saved: {checkpoint_path}")
            else:
                print(f"Failed to save checkpoint: {checkpoint_path}")
    
    # Save final state
    final_checkpoint = f"{checkpoint_dir}/checkpoint_final.bin"
    if brain.save(final_checkpoint):
        print(f"Final checkpoint saved: {final_checkpoint}")
    
    return brain, agent, world

def load_checkpoint(checkpoint_path: str):
    """Load a saved simulation checkpoint."""
    
    try:
        # Create default brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        # Initialize brain
        brain.initialize()
        
        # Load checkpoint
        if brain.load(checkpoint_path):
            print(f"Successfully loaded checkpoint: {checkpoint_path}")
            return brain
        else:
            print(f"Failed to load checkpoint: {checkpoint_path}")
            return None
            
    except Exception as e:
        print(f"Error loading checkpoint {checkpoint_path}: {e}")
        return None

def find_available_checkpoints(checkpoint_dir: str = "./checkpoints"):
    """Find all available checkpoint files."""
    
    checkpoint_files = glob.glob(f"{checkpoint_dir}/checkpoint_*.bin")
    
    # Extract step numbers from filenames
    checkpoints_with_steps = []
    for file_path in checkpoint_files:
        try:
            # Extract step number from filename
            filename = os.path.basename(file_path)
            if "step_" in filename:
                step_str = filename.split("step_")[1].replace(".bin", "")
                step = int(step_str)
                checkpoints_with_steps.append((step, file_path))
        except ValueError:
            continue
    
    # Sort by step number
    checkpoints_with_steps.sort(key=lambda x: x[0])
    
    return checkpoints_with_steps

def calculate_task_reward(world, agent):
    """Calculate task-specific reward based on agent performance."""
    
    agent_body = world.getAgentBody()
    
    # Energy reward (maintain energy above threshold)
    energy_reward = max(0.0, agent_body.energy - 20.0) / 20.0
    
    # Survival reward (positive energy)
    survival_reward = 1.0 if agent_body.energy > 0.0 else -1.0
    
    # Exploration reward (agent is moving)
    exploration_reward = 1.0 if agent_body.isMoving else 0.0
    
    # Neuromodulation-based reward
    neuromodulation_reward = agent.getNeuromodulationLevel()
    
    # Weighted sum of rewards
    total_reward = (energy_reward * 0.4 + 
                   survival_reward * 0.3 + 
                   exploration_reward * 0.2 + 
                   neuromodulation_reward * 0.1)
    
    return total_reward
```

### 5. Performance Monitoring and Optimization

Monitor and optimize simulation performance:

```python
import pynlm
import time
import psutil
import gc
from typing import Dict, Any

def profile_simulation_performance(brain, world, agent, 
                                   num_steps: int = 1000) -> Dict[str, Any]:
    """Profile simulation performance and resource usage."""
    
    # Force garbage collection before profiling
    gc.collect()
    
    # Get initial memory usage
    process = psutil.Process()
    initial_memory = process.memory_info().rss / 1024 / 1024  # MB
    
    # Profile time per step
    step_times = []
    memory_samples = []
    
    for step in range(num_steps):
        step_start = time.time()
        
        # Run one simulation step
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        
        step_end = time.time()
        step_times.append(step_end - step_start)
        
        # Sample memory usage every 100 steps
        if step % 100 == 0:
            memory_samples.append(process.memory_info().rss / 1024 / 1024)
    
    # Calculate performance metrics
    avg_step_time = sum(step_times) / len(step_times)
    max_step_time = max(step_times)
    min_step_time = min(step_times)
    
    avg_memory = sum(memory_samples) / len(memory_samples) if memory_samples else 0
    memory_peak = max(memory_samples) if memory_samples else 0
    
    # Calculate brain performance metrics
    brain_metrics = {
        "avg_step_time_ms": avg_step_time * 1000,
        "max_step_time_ms": max_step_time * 1000,
        "min_step_time_ms": min_step_time * 1000,
        "steps_per_second": num_steps / sum(step_times) if sum(step_times) > 0 else 0,
        "avg_memory_mb": avg_memory,
        "peak_memory_mb": memory_peak,
        "memory_growth_mb": memory_samples[-1] - initial_memory if memory_samples else 0,
    }
    
    # Add neural activity metrics
    brain_metrics.update({
        "final_firing_rate": brain.getAverageFiringRate(),
        "total_spikes": brain.getTotalSpikeCount(),
        "neurons_firing": brain.getFiringNeuronCount(),
        "development_stage": str(brain.getDevelopmentalStage()),
        "plasticity_enabled": {
            "stdp": brain.getSTDP() is not None,
            "hebbian": brain.getHebbian() is not None,
            "structural": brain.getStructuralPlasticity() is not None,
            "reward_modulation": agent.isRewardModulationEnabled(),
            "curiosity": agent.isCuriosityEnabled(),
            "development": agent.isDevelopmentEnabled(),
        }
    })
    
    print(f"Performance Profile (after {num_steps} steps):")
    print(f"  Average step time: {avg_step_time*1000:.2f} ms")
    print(f"  Steps per second: {num_steps/sum(step_times):.1f}")
    print(f"  Average memory: {avg_memory:.1f} MB")
    print(f"  Peak memory: {memory_peak:.1f} MB")
    print(f"  Neural activity: {brain.getAverageFiringRate():.2f} firing rate, {brain.getTotalSpikeCount()} total spikes")
    
    return brain_metrics

def optimize_for_performance(brain, world, agent, 
                           target_step_time_ms: float = 10.0,
                           memory_limit_mb: float = 500.0) -> Dict[str, Any]:
    """Optimize simulation for better performance."""
    
    optimization_suggestions = []
    
    # Check step time performance
    step_profile = profile_simulation_performance(brain, world, agent, 500)
    
    if step_profile["avg_step_time_ms"] > target_step_time_ms:
        optimization_suggestions.append(
            f"Step time too slow ({step_profile['avg_step_time_ms']:.1f} ms > {target_step_time_ms} ms). "
            "Consider reducing network size or optimizing plasticity calculations."
        )
    
    if step_profile["peak_memory_mb"] > memory_limit_mb:
        optimization_suggestions.append(
            f"Memory usage too high ({step_profile['peak_memory_mb']:.1f} MB > {memory_limit_mb} MB). "
            "Consider disabling structural plasticity or reducing checkpoint frequency."
        )
    
    # Optimize configuration for better performance
    optimization_config = {}
    
    if step_profile["avg_step_time_ms"] > target_step_time_ms * 1.5:
        # Slow simulation - reduce complexity
        optimization_config["brain.neuron_count"] = min(500, max(100, 
            int(1000 * (target_step_time_ms / step_profile["avg_step_time_ms"]))))
        optimization_config["plasticity.structural.enable"] = False
        optimization_suggestions.append(
            "Reduced neuron count and disabled structural plasticity for performance."
        )
    
    if step_profile["peak_memory_mb"] > memory_limit_mb * 0.8:
        # High memory usage - disable memory-intensive features
        optimization_config["plasticity.structural.enable"] = False
        optimization_suggestions.append(
            "Disabled structural plasticity to reduce memory usage."
        )
    
    # Re-run with optimized configuration if needed
    if optimization_config:
        print("Applying performance optimizations...")
        config = pynlm.createDefaultConfig()
        
        for key, value in optimization_config.items():
            config.set(key, value)
        
        # Create new optimized brain
        optimized_brain = pynlm.createBrain(config)
        optimized_brain.initialize()
        
        optimized_agent = pynlm.createAgentBrain(optimized_brain)
        optimized_world = pynlm.createSimpleWorld()
        optimized_world.configure(
            width=world.getWidth(), 
            height=world.getHeight(),
            visionWidth=world.getVisionWidth(),
            visionHeight=world.getVisionHeight()
        )
        optimized_world.reset()
        optimized_agent.initialize(optimized_world)
        
        # Profile optimized version
        optimized_profile = profile_simulation_performance(
            optimized_brain, optimized_world, optimized_agent, 500
        )
        
        optimization_suggestions.append(
            f"Optimized performance: step time {step_profile['avg_step_time_ms']:.1f} ms -> {optimized_profile['avg_step_time_ms']:.1f} ms"
        )
        
        return {
            "original_profile": step_profile,
            "optimized_profile": optimized_profile,
            "suggestions": optimization_suggestions,
            "optimization_config": optimization_config,
            "optimized_systems": {
                "brain": optimized_brain,
                "agent": optimized_agent,
                "world": optimized_world,
            }
        }
    
    return {
        "profile": step_profile,
        "suggestions": optimization_suggestions,
        "optimization_config": {}
    }
```

### 6. Advanced Integration Patterns

Integrate NLM with external systems and workflows:

```python
import pynlm
import json
import threading
import queue
from concurrent.futures import ThreadPoolExecutor
from typing import Callable, Any

class NLMSimulationManager:
    """Manage multiple NLM simulations with advanced integration."""
    
    def __init__(self):
        self.simulation_queue = queue.Queue()
        self.results_queue = queue.Queue()
        self.active_simulations = {}
        self.executor = ThreadPoolExecutor(max_workers=4)
    
    def add_simulation(self, simulation_config: Dict[str, Any],
                      result_callback: Callable[[Dict[str, Any]], None]):
        """Add a simulation to the queue."""
        self.simulation_queue.put({
            "config": simulation_config,
            "callback": result_callback,
            "simulation_id": len(self.active_simulations)
        })
    
    def run_simulation_async(self, simulation_config: Dict[str, Any]) -> str:
        """Run a simulation asynchronously."""
        
        simulation_id = f"sim_{len(self.active_simulations)}"
        
        def run_simulation():
            try:
                result = self.run_single_simulation(simulation_config)
                self.results_queue.put({
                    "simulation_id": simulation_id,
                    "status": "completed",
                    "result": result,
                    "error": None
                })
                
                # Call user callback if provided
                if "callback" in simulation_config:
                    simulation_config["callback"](result)
                    
            except Exception as e:
                self.results_queue.put({
                    "simulation_id": simulation_id,
                    "status": "error",
                    "result": None,
                    "error": str(e)
                })
        
        self.active_simulations[simulation_id] = {
            "config": simulation_config,
            "thread": threading.Thread(target=run_simulation),
            "status": "running"
        }
        
        self.active_simulations[simulation_id]["thread"].start()
        return simulation_id
    
    def run_single_simulation(self, config: Dict[str, Any]) -> Dict[str, Any]:
        """Run a single simulation with given configuration."""
        
        # Create brain from configuration
        brain_config = pynlm.createDefaultConfig()
        for key, value in config.get("brain_config", {}).items():
            brain_config.set(key, value)
        
        brain = pynlm.createBrain(brain_config)
        brain.initialize()
        
        # Create agent and world
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        
        # Configure world based on environment config
        env_config = config.get("environment_config", {})
        world.configure(
            width=env_config.get("width", 20),
            height=env_config.get("height", 20),
            visionWidth=env_config.get("vision_width", 8),
            visionHeight=env_config.get("vision_height", 8)
        )
        world.reset()
        
        agent.initialize(world)
        
        # Enable systems based on learning config
        learning_config = config.get("learning_config", {})
        agent.enableRewardModulation(learning_config.get("reward_modulation", True))
        agent.enableStructuralPlasticity(learning_config.get("structural_plasticity", True))
        agent.enableDevelopment(learning_config.get("development", True))
        agent.enableCuriosity(learning_config.get("curiosity", True))
        
        # Run simulation
        num_steps = config.get("num_steps", 1000)
        reward_callback = config.get("reward_callback")
        
        for step in range(num_steps):
            # Update world
            world.update(config.get("timestep", 0.1))
            
            # Get sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Run brain
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply learning if callback provided
            if reward_callback:
                reward = reward_callback(world, agent, step)
                agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(config.get("development_rate", 0.1))
        
        # Collect results
        results = {
            "final_brain_state": brain,
            "final_agent_state": agent,
            "final_world_state": world,
            "metrics": {
                "final_firing_rate": brain.getAverageFiringRate(),
                "final_curiosity": agent.getCuriosityLevel(),
                "final_novelty": agent.getNoveltyLevel(),
                "total_spikes": brain.getTotalSpikeCount(),
                "development_stage": str(brain.getDevelopmentalStage()),
                "energy": world.getAgentBody().energy,
            },
            "simulation_id": config.get("simulation_id", "unknown")
        }
        
        return results
    
    def get_simulation_status(self, simulation_id: str) -> Dict[str, Any]:
        """Get status of a running simulation."""
        
        if simulation_id not in self.active_simulations:
            return {"status": "not_found"}
        
        simulation = self.active_simulations[simulation_id]
        return {
            "simulation_id": simulation_id,
            "status": simulation["status"],
            "config": simulation["config"],
            "start_time": simulation.get("start_time"),
            "current_step": simulation.get("current_step", 0)
        }
    
    def stop_simulation(self, simulation_id: str):
        """Stop a running simulation."""
        
        if simulation_id in self.active_simulations:
            simulation = self.active_simulations[simulation_id]
            if simulation["status"] == "running":
                # Stop the thread
                simulation["thread"].join(timeout=5.0)
                simulation["status"] = "stopped"
                print(f"Stopped simulation {simulation_id}")
    
    def collect_results(self) -> List[Dict[str, Any]]:
        """Collect all completed simulation results."""
        
        results = []
        while not self.results_queue.empty():
            result = self.results_queue.get()
            results.append(result)
            
            # Remove completed simulation from active list
            if result["simulation_id"] in self.active_simulations:
                del self.active_simulations[result["simulation_id"]]
        
        return results
    
    def run_batched_simulations(self, configs: List[Dict[str, Any]], 
                               batch_size: int = 4) -> List[Dict[str, Any]]:
        """Run multiple simulations in batches."""
        
        all_results = []
        
        for i in range(0, len(configs), batch_size):
            batch_configs = configs[i:i + batch_size]
            
            # Submit batch to executor
            futures = []
            for config in batch_configs:
                future = self.executor.submit(
                    self.run_single_simulation, config
                )
                futures.append((config.get("simulation_id", f"batch_{i//batch_size}_{j}"), future))
            
            # Collect results
            for simulation_id, future in futures:
                try:
                    result = future.result(timeout=300)  # 5 minute timeout
                    result["simulation_id"] = simulation_id
                    all_results.append(result)
                except Exception as e:
                    print(f"Simulation {simulation_id} failed: {e}")
                    all_results.append({
                        "simulation_id": simulation_id,
                        "status": "failed",
                        "error": str(e)
                    })
        
        return all_results

# Example usage
if __name__ == "__main__":
    # Create simulation manager
    manager = NLMSimulationManager()
    
    # Define different simulation configurations
    simple_config = {
        "simulation_id": "simple_brain",
        "brain_config": {
            "brain.neuron_count": 500,
            "brain.synapse_density": 0.1,
        },
        "environment_config": {
            "width": 20,
            "height": 20,
            "vision_width": 8,
            "vision_height": 8,
        },
        "learning_config": {
            "reward_modulation": True,
            "structural_plasticity": False,
            "development": False,
            "curiosity": True,
        },
        "num_steps": 500,
        "timestep": 0.1,
        "development_rate": 0.1,
    }
    
    complex_config = {
        "simulation_id": "complex_brain",
        "brain_config": {
            "brain.neuron_count": 2000,
            "brain.synapse_density": 0.15,
            "plasticity.stdp.enable": True,
            "plasticity.hebbian.enable": True,
            "plasticity.structural.enable": True,
        },
        "environment_config": {
            "width": 30,
            "height": 30,
            "vision_width": 12,
            "vision_height": 12,
        },
        "learning_config": {
            "reward_modulation": True,
            "structural_plasticity": True,
            "development": True,
            "curiosity": True,
        },
        "num_steps": 2000,
        "timestep": 0.1,
        "development_rate": 0.2,
    }
    
    # Add simulations to queue
    manager.add_simulation(simple_config)
    manager.add_simulation(complex_config)
    
    # Run simulations asynchronously
    simple_id = manager.run_simulation_async(simple_config)
    complex_id = manager.run_simulation_async(complex_config)
    
    print(f"Started simple simulation: {simple_id}")
    print(f"Started complex simulation: {complex_id}")
    
    # Wait for results and process them
    time.sleep(10)  # Wait for simulations to complete
    
    results = manager.collect_results()
    print(f"Completed {len(results)} simulations")
    
    # Analyze results
    for result in results:
        if result["status"] == "completed":
            metrics = result["result"]["metrics"]
            print(f"Simulation {result['simulation_id']}: {metrics['final_firing_rate']:.2f} firing rate")
        else:
            print(f"Simulation {result['simulation_id']} failed: {result['error']}")
```

## Summary

This advanced usage guide provides:

1. **Advanced Python API usage** for working with new plasticity system bindings
2. **Complex simulation scenarios** including parameter sweeps and batch processing
3. **Performance monitoring and optimization** tools for efficient simulations
4. **Advanced integration patterns** for workflow management
5. **Real-world application examples** for different use cases

These advanced techniques enable users to:

- Configure and customize STDP and structural plasticity systems
- Run complex multi-simulation experiments
- Monitor and optimize performance
- Integrate NLM with external systems and workflows
- Build production-ready simulation applications

The combination of the new plasticity system bindings and these advanced usage patterns provides a comprehensive toolkit for building sophisticated neural learning applications.