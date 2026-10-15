# NLM - Advanced User Guide with Research Examples

## Overview

This guide provides advanced examples and research-grade techniques for using the Enhanced NLM Python API. It assumes familiarity with the basic API and focuses on research applications, performance optimization, and advanced usage patterns.

## Advanced Configuration Management

### Research Configuration Presets

The enhanced API provides four specialized configuration presets for different research scenarios:

```python
import pynlm

# Research Configuration (2000 neurons, enhanced learning)
research_config = pynlm.createResearchConfig()
print(f"Research config: {research_config.get('brain.neuron_count')} neurons")

# Development Configuration (1500 neurons, training focus)
dev_config = pynlm.createDevelopmentConfig()
print(f"Dev config: {dev_config.get('brain.neuron_count')} neurons")

# Testing Configuration (100 neurons, stable)
test_config = pynlm.createTestingConfig()
print(f"Test config: {test_config.get('brain.neuron_count')} neurons")

# Production Configuration (1000 neurons, stable)
prod_config = pynlm.createProductionConfig()
print(f"Prod config: {prod_config.get('brain.neuron_count')} neurons")
```

### Custom Configuration Templates

```python
import pynlm
import json

# Template for agent-based research
agent_research_template = {
    "brain": {
        "neuron_count": 2000,
        "synapse_density": 0.12,
        "plasticity": {
            "stdp": {"enable": True, "learning_rate": 0.002},
            "hebbian": {"enable": True, "learning_rate": 0.001},
            "structural": {"enable": True, "rate": 0.0001}
        }
    },
    "neuromodulation": {
        "dopamine": {"scale": 1.2},
        "curiosity": {"enable": True, "rate": 0.05},
        "novelty": {"enable": True, "sensitivity": 0.8}
    },
    "development": {
        "enable": True,
        "maturation_rate": 0.8,
        "critical_period": {"enabled": True, "duration": 1000}
    }
}

# Save configuration
with open("agent_research_config.json", "w") as f:
    json.dump(agent_research_template, f, indent=2)

# Load and modify
config = pynlm.load_config_from_file("agent_research_config.json")
config.set("brain.neuron_count", 2500)
pynlm.save_config_to_file(config, "custom_research_config.json")
```

## Performance-Optimized Research Examples

### Large-Scale Simulation with Performance Monitoring

```python
import pynlm
import time
import matplotlib.pyplot as plt

def run_large_scale_research(num_neurons=2000, num_steps=5000):
    """Run large-scale research simulation with performance monitoring."""
    
    # Create research configuration optimized for large-scale simulation
    config = pynlm.createResearchConfig()
    config.set("brain.neuron_count", num_neurons)
    
    print(f"Starting large-scale simulation with {num_neurons} neurons")
    
    # Use context manager for automatic resource management
    with pynlm.createBrain(config) as brain:
        # Benchmark performance
        performance_start = time.time()
        steps_completed = brain.runSteps(num_steps)
        performance_time = time.time() - performance_start
        
        # Get comprehensive performance metrics
        performance = pynlm.benchmark_brain_performance(brain, num_steps)
        
        print(f"Completed {steps_completed} steps in {performance_time:.2f}s")
        print(f"Performance: {performance['steps_per_second']:.0f} steps/second")
        print(f"Average spikes per step: {performance['spikes_per_step']:.2f}")
        
        # Get detailed statistics
        stats = brain.getFiringStatistics()
        print(f"Firing percentage: {stats['firing_percentage']:.1f}%")
        print(f"Activity level: {stats['activity_percentage']:.1f}%")
        print(f"E/I ratio: {stats['e_i_ratio']:.3f}")
        
        # Create research visualization
        viz = pynlm.create_neural_network_visualizer(brain)
        print("Neural network visualization:")
        print(viz)
        
        # Analyze brain activity patterns
        analysis = pynlm.analyze_brain_activity(brain)
        print(f"Activity level: {analysis['activity_level']}")
        print(f"Stability: {analysis['stability']}")
        
        return {
            "performance": performance,
            "statistics": stats,
            "analysis": analysis,
            "visualization": str(viz)
        }

# Run large-scale simulation
results = run_large_scale_research(1500, 3000)
```

### Memory-Efficient Batch Processing

```python
import pynlm

def run_batch_research(config_type="research", batch_size=100, num_iterations=50):
    """Run multiple simulations with memory-efficient batch processing."""
    
    # Select configuration
    if config_type == "research":
        config = pynlm.createResearchConfig()
    elif config_type == "development":
        config = pynlm.createDevelopmentConfig()
    elif config_type == "testing":
        config = pynlm.createTestingConfig()
    else:
        config = pynlm.createProductionConfig()
    
    print(f"Running {num_iterations} iterations with {config_type} configuration")
    
    all_results = []
    
    for iteration in range(num_iterations):
        # Create new brain for each iteration (clean memory)
        with pynlm.createBrain(config) as brain:
            # Run batch simulation
            steps_per_batch = 100
            for batch in range(batch_size):
                brain.runSteps(steps_per_batch)
                
                # Collect statistics
                stats = brain.getFiringStatistics()
                
                all_results.append({
                    "iteration": iteration,
                    "batch": batch,
                    "firing_percentage": stats['firing_percentage'],
                    "activity_level": stats['activity_percentage'],
                    "e_i_ratio": stats['e_i_ratio']
                })
                
                # Create snapshot for key iterations
                if iteration % 10 == 0 and batch == 0:
                    snapshot = pynlm.create_brain_snapshot(
                        brain, 
                        f"iter_{iteration}_batch_{batch}"
                    )
    
    # Analyze batch results
    print(f"Collected {len(all_results)} data points")
    
    # Calculate statistics across iterations
    firing_percentages = [r['firing_percentage'] for r in all_results]
    activity_levels = [r['activity_level'] for r in all_results]
    
    print(f"Average firing percentage: {sum(firing_percentages)/len(firing_percentages):.1f}%")
    print(f"Average activity level: {sum(activity_levels)/len(activity_levels):.1f}%")
    
    return all_results

# Run batch research
batch_results = run_batch_research("research", batch_size=50, num_iterations=20)
```

## Agent-Based Research with Advanced Learning

### Complete Research Agent Simulation

```python
import pynlm
import numpy as np

def run_advanced_agent_research(
    world_size=30,
    num_steps=1000,
    enable_all_features=True,
    learning_rate=0.01,
    novelty_threshold=0.7
):
    """Run advanced agent research with comprehensive learning subsystems."""
    
    print(f"Starting advanced agent research: {num_steps} steps, {world_size}x{world_size} world")
    
    # Create ready-to-run environment with custom configuration
    brain, world, agent = pynlm.createReadyToRunEnvironment()
    
    # Customize configuration for research
    brain_config = brain.getConfig()
    brain_config.set("brain.neuron_count", 2000)
    
    # Enable/disable learning subsystems based on requirements
    if enable_all_features:
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        agent.enableStructuralPlasticity(True)
        agent.enableNoveltyDetection(True)
    else:
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(False)
        agent.enableStructuralPlasticity(False)
        agent.enableNoveltyDetection(False)
    
    # Research tracking
    research_data = {
        "steps": list(range(num_steps)),
        "curiosity_levels": [],
        "novelty_levels": [],
        "firing_rates": [],
        "rewards": [],
        "novel_events": 0,
        "learning_episodes": []
    }
    
    # Run simulation with detailed monitoring
    print("Running agent research simulation...")
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input with novelty detection
        agent.processSensoryInput(percept)
        
        # Check for novelty
        current_novelty = agent.getNoveltyLevel()
        if current_novelty > novelty_threshold:
            research_data["novel_events"] += 1
        
        # Run brain step with optional learning rate adjustment
        brain.step(step)
        
        # Get action and apply to world
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation if enabled
        if agent.isRewardModulationEnabled():
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            research_data["rewards"].append(reward)
        
        # Update development if enabled
        if agent.isDevelopmentEnabled():
            agent.updateDevelopment(0.1)
            research_data["learning_episodes"].append(step)
        
        # Collect metrics
        if step % 10 == 0:  # Sample every 10 steps
            research_data["curiosity_levels"].append(agent.getCuriosityLevel())
            research_data["novelty_levels"].append(current_novelty)
            research_data["firing_rates"].append(brain.getAverageFiringRate())
    
    # Calculate research statistics
    final_stats = brain.getFiringStatistics()
    final_status = agent.getAgentStatus(world)
    
    research_summary = {
        "total_spikes": brain.getTotalSpikeCount(),
        "average_firing_rate": brain.getAverageFiringRate(),
        "final_curiosity": agent.getCuriosityLevel(),
        "final_novelty": agent.getNoveltyLevel(),
        "novel_events": research_data["novel_events"],
        "learning_efficiency": len(research_data["learning_episodes"]) / num_steps,
        "final_status": final_status,
        "brain_statistics": final_stats
    }
    
    print(f"Research simulation completed!")
    print(f"Total spikes: {research_summary['total_spikes']}")
    print(f"Final curiosity: {research_summary['final_curiosity']:.3f}")
    print(f"Novel events detected: {research_summary['novel_events']}")
    print(f"Learning efficiency: {research_summary['learning_efficiency']:.3f}")
    
    return research_data, research_summary

# Run advanced agent research
research_data, research_summary = run_advanced_agent_research(
    world_size=25,
    num_steps=800,
    enable_all_features=True,
    novelty_threshold=0.6
)
```

### Multi-Agent Research Environment

```python
import pynlm
import multiprocessing

def create_multi_agent_research(
    num_agents=5,
    num_steps=500,
    world_size=40,
    shared_config=None
):
    """Create multi-agent research environment for studying social learning."""
    
    print(f"Creating multi-agent research environment: {num_agents} agents")
    
    if shared_config is None:
        # Shared configuration for all agents
        shared_config = pynlm.createResearchConfig()
        shared_config.set("brain.neuron_count", 1000)
    
    # Create shared world
    world = pynlm.createSimpleWorld()
    world.configure(world_size, world_size, 8, 8)
    world.setAgentStart(world_size // 2, world_size // 2)
    
    # Create agents with shared brain configuration
    agents = []
    worlds = []
    
    for i in range(num_agents):
        # Each agent gets its own brain (could be shared for efficiency)
        agent_brain = pynlm.createBrain(shared_config)
        agent_brain.initialize()
        
        # Create world copy for each agent
        agent_world = pynlm.createSimpleWorld()
        agent_world.configure(world_size, world_size, 8, 8)
        agent_world.reset()
        agent_world.setAgentStart(world_size // 2, world_size // 2)
        
        # Create agent interface
        agent = pynlm.createAgentBrain(agent_brain)
        agent.initialize(agent_world)
        
        # Enable different learning modes for each agent
        if i % 2 == 0:
            agent.enableRewardModulation(True)
            agent.enableCuriosity(False)
        else:
            agent.enableRewardModulation(False)
            agent.enableCuriosity(True)
            agent.enableDevelopment(True)
        
        agents.append(agent)
        worlds.append(agent_world)
    
    # Multi-agent research tracking
    multi_agent_data = {
        "agents": list(range(num_agents)),
        "steps": list(range(num_steps)),
        "agent_performances": [],
        "social_interactions": [],
        "learning_dynamics": []
    }
    
    # Run multi-agent simulation
    print("Running multi-agent research simulation...")
    for step in range(num_steps):
        # Synchronize world updates (optional)
        for i, (agent, agent_world) in enumerate(zip(agents, worlds)):
            agent_world.update(0.1)
            
            # Get percept and process
            percept = agent_world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Get action and apply
            action = agent.decodeMotorCommand()
            agent_world.applyMotorCommand(action, agent_world.getSimulationTime())
            
            # Record performance
            if step % 20 == 0:
                stats = agent_brain.getFiringStatistics()
                multi_agent_data["agent_performances"].append({
                    "agent": i,
                    "step": step,
                    "firing_percentage": stats['firing_percentage'],
                    "curiosity": agent.getCuriosityLevel(),
                    "novelty": agent.getNoveltyLevel()
                })
    
    # Analyze multi-agent results
    print(f"Multi-agent simulation completed with {num_agents} agents")
    
    for i, agent in enumerate(agents):
        final_stats = agent_brain.getFiringStatistics()
        final_status = agent.getAgentStatus(worlds[i])
        
        print(f"Agent {i} performance:")
        print(f"  - Firing percentage: {final_stats['firing_percentage']:.1f}%")
        print(f"  - Final curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  - Learning episodes: {len([d for d in multi_agent_data['agent_performances'] if d['agent'] == i])}")
    
    return agents, worlds, multi_agent_data

# Run multi-agent research
agents, worlds, multi_agent_data = create_multi_agent_research(
    num_agents=3,
    num_steps=300,
    world_size=30
)
```

## Research Workflow and Experiment Management

### Experiment Framework

```python
import pynlm
import json
import time
from typing import Dict, List, Any

class NLMSResearchExperiment:
    """Framework for managing NLM research experiments."""
    
    def __init__(self, experiment_name: str):
        self.experiment_name = experiment_name
        self.results = {}
        self.start_time = time.time()
    
    def create_experiment_config(
        self,
        config_type: str = "research",
        custom_settings: Dict[str, Any] = None
    ) -> pynlm.Config:
        """Create experiment configuration."""
        if config_type == "research":
            config = pynlm.createResearchConfig()
        elif config_type == "development":
            config = pynlm.createDevelopmentConfig()
        elif config_type == "testing":
            config = pynlm.createTestingConfig()
        else:
            config = pynlm.createProductionConfig()
        
        if custom_settings:
            for key, value in custom_settings.items():
                config.set(key, value)
        
        return config
    
    def run_simulation(
        self,
        config: pynlm.Config,
        simulation_type: str = "basic",
        num_steps: int = 1000,
        **kwargs
    ) -> Dict[str, Any]:
        """Run simulation with comprehensive data collection."""
        
        print(f"Running {simulation_type} simulation with {num_steps} steps")
        
        if simulation_type == "basic":
            return self._run_basic_simulation(config, num_steps)
        elif simulation_type == "agent":
            return self._run_agent_simulation(config, num_steps, **kwargs)
        elif simulation_type == "research":
            return self._run_research_simulation(config, num_steps, **kwargs)
        else:
            raise ValueError(f"Unknown simulation type: {simulation_type}")
    
    def _run_basic_simulation(
        self,
        config: pynlm.Config,
        num_steps: int
    ) -> Dict[str, Any]:
        """Run basic neural simulation."""
        
        with pynlm.createBrain(config) as brain:
            # Run simulation
            brain.runSteps(num_steps)
            
            # Collect data
            stats = brain.getFiringStatistics()
            
            return {
                "simulation_type": "basic",
                "config_type": "research",
                "neurons": config.get("brain.neuron_count", 0),
                "total_steps": num_steps,
                "total_spikes": brain.getTotalSpikeCount(),
                "firing_percentage": stats['firing_percentage'],
                "activity_level": stats['activity_percentage'],
                "e_i_ratio": stats['e_i_ratio'],
                "spikes_per_neuron": stats['spikes_per_neuron'],
                "firing_rate": stats['average_firing_rate']
            }
    
    def _run_agent_simulation(
        self,
        config: pynlm.Config,
        num_steps: int,
        world_size: int = 20,
        enable_reward: bool = True,
        enable_curiosity: bool = True,
        enable_development: bool = False
    ) -> Dict[str, Any]:
        """Run agent-based simulation."""
        
        # Create environment
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(world_size, world_size, 8, 8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning subsystems
        agent.enableRewardModulation(enable_reward)
        agent.enableCuriosity(enable_curiosity)
        agent.enableDevelopment(enable_development)
        
        # Run simulation
        for step in range(num_steps):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            if enable_reward:
                reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
                agent.applyRewardModulation(reward, 0.0)
            
            if enable_development:
                agent.updateDevelopment(0.1)
        
        # Collect results
        stats = brain.getFiringStatistics()
        status = agent.getAgentStatus(world)
        
        return {
            "simulation_type": "agent",
            "config_type": "research",
            "world_size": world_size,
            "neurons": config.get("brain.neuron_count", 0),
            "total_steps": num_steps,
            "total_spikes": brain.getTotalSpikeCount(),
            "firing_percentage": stats['firing_percentage'],
            "activity_level": stats['activity_percentage'],
            "final_curiosity": agent.getCuriosityLevel(),
            "final_novelty": agent.getNoveltyLevel(),
            "agent_status": status,
            "brain_statistics": stats
        }
    
    def _run_research_simulation(
        self,
        config: pynlm.Config,
        num_steps: int,
        world_size: int = 25,
        custom_learning_config: Dict[str, Any] = None
    ) -> Dict[str, Any]:
        """Run research-grade simulation with advanced features."""
        
        # Use ready-to-run environment
        brain, world, agent = pynlm.createReadyToRunEnvironment()
        
        # Override configuration if provided
        if custom_learning_config:
            brain_config = brain.getConfig()
            for key, value in custom_learning_config.items():
                brain_config.set(key, value)
        
        # Run comprehensive simulation
        return self._run_agent_simulation(
            config, num_steps, world_size,
            enable_reward=True,
            enable_curiosity=True,
            enable_development=True
        )
    
    def save_results(self, filename: str):
        """Save experiment results to file."""
        results = {
            "experiment_name": self.experiment_name,
            "start_time": self.start_time,
            "end_time": time.time(),
            "duration": time.time() - self.start_time,
            "results": self.results
        }
        
        with open(filename, 'w') as f:
            json.dump(results, f, indent=2, default=str)
        
        print(f"Results saved to {filename}")
    
    def add_result(self, name: str, data: Dict[str, Any]):
        """Add result to experiment."""
        self.results[name] = data

# Example usage
experiment = NLMSResearchExperiment("advanced_research_study")

# Run basic simulation
basic_result = experiment.run_simulation(
    experiment.create_experiment_config("research"),
    "basic",
    num_steps=500
)
experiment.add_result("basic_simulation", basic_result)

# Run agent simulation
agent_result = experiment.run_simulation(
    experiment.create_experiment_config("development"),
    "agent",
    num_steps=1000,
    world_size=30,
    enable_reward=True,
    enable_curiosity=True,
    enable_development=True
)
experiment.add_result("agent_simulation", agent_result)

# Run research simulation
research_result = experiment.run_simulation(
    experiment.create_experiment_config("research"),
    "research",
    num_steps=800,
    world_size=25,
    custom_learning_config={
        "brain.neuron_count": 2000,
        "neuromod.dopamine.scale": 1.5
    }
)
experiment.add_result("research_simulation", research_result)

# Save experiment results
experiment.save_results("advanced_research_experiment.json")
```

## Performance Optimization Examples

### Memory-Optimized Research

```python
import pynlm
import gc
import psutil
import os

class MemoryOptimizedResearch:
    """Memory-optimized research framework."""
    
    def __init__(self, max_memory_gb: float = 8.0):
        self.max_memory_gb = max_memory_gb
        self.memory_monitor = psutil.Process(os.getpid())
    
    def get_memory_usage(self) -> float:
        """Get current memory usage in GB."""
        return self.memory_monitor.memory_info().rss / (1024**3)
    
    def check_memory(self) -> bool:
        """Check if memory usage is within limits."""
        current_memory = self.get_memory_usage()
        return current_memory < self.max_memory_gb
    
    def run_memory_efficient_research(
        self,
        config: pynlm.Config,
        num_steps: int,
        chunk_size: int = 1000,
        cleanup_interval: int = 5
    ) -> Dict[str, Any]:
        """Run research with memory monitoring and cleanup."""
        
        print(f"Running memory-efficient research with {num_steps} steps")
        print(f"Memory limit: {self.max_memory_gb}GB")
        
        # Calculate number of chunks
        num_chunks = (num_steps + chunk_size - 1) // chunk_size
        
        all_data = {
            "chunks": [],
            "total_spikes": 0,
            "total_steps": num_steps,
            "chunk_summaries": []
        }
        
        for chunk in range(num_chunks):
            print(f"Processing chunk {chunk + 1}/{num_chunks}")
            
            # Check memory
            if not self.check_memory():
                print(f"Warning: Memory usage approaching limit ({self.get_memory_usage():.1f}GB)")
                gc.collect()  # Force garbage collection
            
            # Create brain for this chunk
            with pynlm.createBrain(config) as brain:
                chunk_steps = min(chunk_size, num_steps - chunk * chunk_size)
                
                # Run chunk
                brain.runSteps(chunk_steps)
                
                # Collect statistics
                stats = brain.getFiringStatistics()
                chunk_data = {
                    "chunk": chunk,
                    "steps_in_chunk": chunk_steps,
                    "total_spikes": brain.getTotalSpikeCount(),
                    "firing_percentage": stats['firing_percentage'],
                    "activity_level": stats['activity_percentage'],
                    "e_i_ratio": stats['e_i_ratio'],
                    "spikes_per_neuron": stats['spikes_per_neuron']
                }
                
                all_data["chunks"].append(chunk_data)
                all_data["total_spikes"] += brain.getTotalSpikeCount()
                
                # Add chunk summary
                all_data["chunk_summaries"].append({
                    "chunk": chunk,
                    "spikes_per_step": brain.getTotalSpikeCount() / chunk_steps,
                    "memory_usage": self.get_memory_usage()
                })
                
                # Periodic cleanup
                if chunk % cleanup_interval == 0:
                    gc.collect()
                    print(f"Cleaned up memory after chunk {chunk}")
        
        # Calculate final statistics
        final_firing_percentage = sum(c["firing_percentage"] for c in all_data["chunks"]) / len(all_data["chunks"])
        final_activity_level = sum(c["activity_level"] for c in all_data["chunks"]) / len(all_data["chunks"])
        
        print(f"Memory-efficient research completed!")
        print(f"Total spikes: {all_data['total_spikes']}")
        print(f"Average firing percentage: {final_firing_percentage:.1f}%")
        print(f"Average activity level: {final_activity_level:.1f}%")
        
        return all_data

# Example usage
memory_research = MemoryOptimizedResearch(max_memory_gb=4.0)

config = pynlm.createResearchConfig()
results = memory_research.run_memory_efficient_research(
    config,
    num_steps=5000,
    chunk_size=1000,
    cleanup_interval=3
)
```

### Parallel Research Processing

```python
import pynlm
import multiprocessing
from concurrent.futures import ThreadPoolExecutor
import time

class ParallelResearchFramework:
    """Parallel processing framework for research simulations."""
    
    def __init__(self, num_workers: int = None):
        self.num_workers = num_workers or multiprocessing.cpu_count()
        self.executor = ThreadPoolExecutor(max_workers=self.num_workers)
    
    def run_parallel_simulation(
        self,
        config: pynlm.Config,
        num_simulations: int,
        num_steps: int,
        simulation_type: str = "basic"
    ) -> Dict[str, Any]:
        """Run multiple simulations in parallel."""
        
        print(f"Running {num_simulations} simulations in parallel with {self.num_workers} workers")
        
        # Create futures for each simulation
        futures = []
        for i in range(num_simulations):
            future = self.executor.submit(
                self._run_single_simulation,
                config, num_steps, simulation_type, i
            )
            futures.append(future)
        
        # Collect results
        all_results = []
        for future in futures:
            try:
                result = future.result()
                all_results.append(result)
            except Exception as e:
                print(f"Simulation failed: {e}")
        
        # Analyze results
        parallel_results = self._analyze_parallel_results(all_results)
        
        return parallel_results
    
    def _run_single_simulation(
        self,
        config: pynlm.Config,
        num_steps: int,
        simulation_type: str,
        simulation_id: int
    ) -> Dict[str, Any]:
        """Run a single simulation (for parallel processing)."""
        
        if simulation_type == "basic":
            with pynlm.createBrain(config) as brain:
                brain.runSteps(num_steps)
                stats = brain.getFiringStatistics()
                
                return {
                    "simulation_id": simulation_id,
                    "type": simulation_type,
                    "total_spikes": brain.getTotalSpikeCount(),
                    "firing_percentage": stats['firing_percentage'],
                    "activity_level": stats['activity_percentage'],
                    "e_i_ratio": stats['e_i_ratio'],
                    "spikes_per_neuron": stats['spikes_per_neuron'],
                    "firing_rate": stats['average_firing_rate']
                }
        
        elif simulation_type == "agent":
            # Create environment
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            world.configure(20, 20, 8, 8)
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Enable learning
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            
            # Run simulation
            for step in range(num_steps):
                world.update(0.1)
                
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                
                brain.step(step)
                
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
                agent.applyRewardModulation(reward, 0.0)
            
            stats = brain.getFiringStatistics()
            
            return {
                "simulation_id": simulation_id,
                "type": simulation_type,
                "total_spikes": brain.getTotalSpikeCount(),
                "firing_percentage": stats['firing_percentage'],
                "activity_level": stats['activity_percentage'],
                "final_curiosity": agent.getCuriosityLevel(),
                "final_novelty": agent.getNoveltyLevel()
            }
        
        else:
            raise ValueError(f"Unknown simulation type: {simulation_type}")
    
    def _analyze_parallel_results(self, results: List[Dict]) -> Dict[str, Any]:
        """Analyze results from parallel simulations."""
        
        if not results:
            return {}
        
        # Group results by type
        basic_results = [r for r in results if r['type'] == 'basic']
        agent_results = [r for r in results if r['type'] == 'agent']
        
        analysis = {
            "total_simulations": len(results),
            "basic_simulations": len(basic_results),
            "agent_simulations": len(agent_results),
            "basic_performance": self._analyze_performance(basic_results) if basic_results else None,
            "agent_performance": self._analyze_performance(agent_results) if agent_results else None,
            "comparison": self._compare_simulations(basic_results, agent_results)
        }
        
        return analysis
    
    def _analyze_performance(self, results: List[Dict]) -> Dict[str, Any]:
        """Analyze performance metrics."""
        
        if not results:
            return None
        
        spikes = [r['total_spikes'] for r in results]
        firing_percentages = [r['firing_percentage'] for r in results]
        activity_levels = [r['activity_level'] for r in results]
        
        return {
            "total_spikes": {
                "mean": sum(spikes) / len(spikes),
                "std": (sum((x - sum(spikes)/len(spikes))**2 for x in spikes) / len(spikes))**0.5,
                "min": min(spikes),
                "max": max(spikes)
            },
            "firing_percentage": {
                "mean": sum(firing_percentages) / len(firing_percentages),
                "std": (sum((x - sum(firing_percentages)/len(firing_percentages))**2 for x in firing_percentages) / len(firing_percentages))**0.5,
                "min": min(firing_percentages),
                "max": max(firing_percentages)
            },
            "activity_level": {
                "mean": sum(activity_levels) / len(activity_levels),
                "std": (sum((x - sum(activity_levels)/len(activity_levels))**2 for x in activity_levels) / len(activity_levels))**0.5,
                "min": min(activity_levels),
                "max": max(activity_levels)
            }
        }
    
    def _compare_simulations(self, basic_results: List[Dict], agent_results: List[Dict]) -> Dict[str, Any]:
        """Compare basic and agent simulation results."""
        
        if not basic_results or not agent_results:
            return None
        
        basic_spikes = [r['total_spikes'] for r in basic_results]
        agent_spikes = [r['total_spikes'] for r in agent_results]
        
        basic_firing = [r['firing_percentage'] for r in basic_results]
        agent_firing = [r['firing_percentage'] for r in agent_results]
        
        return {
            "spike_count_difference": {
                "mean": sum(agent_spikes) / len(agent_spikes) - sum(basic_spikes) / len(basic_spikes),
                "percent": (sum(agent_spikes) / len(agent_spikes) - sum(basic_spikes) / len(basic_spikes)) / sum(basic_spikes) / len(basic_spikes) * 100
            },
            "firing_rate_difference": {
                "mean": sum(agent_firing) / len(agent_firing) - sum(basic_firing) / len(basic_firing),
                "percent": (sum(agent_firing) / len(agent_firing) - sum(basic_firing) / len(basic_firing)) / sum(basic_firing) / len(basic_firing) * 100
            }
        }
    
    def shutdown(self):
        """Shutdown the executor."""
        self.executor.shutdown(wait=True)

# Example usage
parallel_research = ParallelResearchFramework(num_workers=4)

# Create configurations
research_config = pynlm.createResearchConfig()
dev_config = pynlm.createDevelopmentConfig()

# Run parallel basic simulations
basic_results = parallel_research.run_parallel_simulation(
    research_config,
    num_simulations=8,
    num_steps=1000,
    simulation_type="basic"
)

print("\nParallel basic simulation results:")
print(f"Mean spikes: {basic_results['basic_performance']['total_spikes']['mean']:.0f}")
print(f"Std spikes: {basic_results['basic_performance']['total_spikes']['std']:.0f}")

# Run parallel agent simulations
agent_results = parallel_research.run_parallel_simulation(
    dev_config,
    num_simulations=6,
    num_steps=800,
    simulation_type="agent"
)

print("\nParallel agent simulation results:")
print(f"Mean spikes: {agent_results['agent_performance']['total_spikes']['mean']:.0f}")
print(f"Mean firing percentage: {agent_results['agent_performance']['firing_percentage']['mean']:.1f}%")

# Shutdown
parallel_research.shutdown()
```

## Conclusion

This advanced user guide provides comprehensive examples for research-grade NLM simulations, including:

1. **Advanced Configuration Management**: Custom configuration templates and preset management
2. **Performance-Optimized Research**: Large-scale simulations with monitoring and optimization
3. **Memory-Efficient Processing**: Chunk-based processing with memory monitoring
4. **Parallel Processing**: Multi-simulation parallel execution
5. **Agent-Based Research**: Complete agent simulations with advanced learning
6. **Multi-Agent Environments**: Social learning and interaction studies
7. **Research Workflow Management**: Experiment framework and result tracking

These examples enable researchers to conduct sophisticated neural network simulations with research-grade tools and workflows, while maintaining good performance and memory efficiency.

Note: Requires matplotlib for plotting in some examples. Install with `pip install matplotlib`.

-----
*Requires matplotlib for plotting functionality. Install with: `pip install matplotlib`*