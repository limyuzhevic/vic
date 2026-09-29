# Advanced NLM Examples for Power Users

This document provides advanced usage examples for the NLM Python bindings, demonstrating sophisticated techniques for simulation, agent training, and performance optimization.

## Table of Contents

1. [Context Manager Best Practices](#context-manager-best-practices)
2. [Advanced Agent Training](#advanced-agent-training)
3. [Performance Optimization](#performance-optimization)
4. [Research-Grade Experiments](#research-grade-experiments)
5. [Error Handling Strategies](#error-handling-strategies)
6. [Configuration Management](#configuration-management)
7. [Custom Visualization & Monitoring](#custom-visualization--monitoring)
8. [Parallel Processing](#parallel-processing)
9. [Ablation Studies](#ablation-studies)
10. [Migration from Legacy Code](#migration-from-legacy-code)

---

## Context Manager Best Practices

### 1.1 Resource Management with Context Managers

```python
import pynlm
from contextlib import contextmanager
import time

@contextmanager
def experiment_session(config_template=None):
    """Context manager for experiment sessions with automatic cleanup."""
    config = config_template or pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    try:
        brain.initialize()
        yield brain
    finally:
        brain.reset()
        print("Experiment session completed and cleaned up")

# Use the context manager
with experiment_session() as brain:
    # Run multiple experiments
    for episode in range(5):
        print(f"Running episode {episode + 1}/5")
        brain.step(episode * 100)
```

### 1.2 Nested Context Managers for Complex Experiments

```python
import pynlm
import threading
import multiprocessing

class ExperimentOrchestrator:
    def __init__(self):
        self.experiments = []
        self.results = []
    
    @contextmanager
    def run_parallel_experiments(self, num_experiments=4):
        """Run multiple experiments in parallel with shared resources."""
        # Create shared world
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20)
        world.reset()
        
        # Create shared agent brain interface
        config = pynlm.createDefaultConfig()
        agent_brain = pynlm.createAgentBrain(config)
        agent_brain.initialize(world)
        
        # Configure all experiments
        for i in range(num_experiments):
            exp_config = pynlm.createDefaultConfig()
            exp_config.set("brain.neuron_count", 100 + i * 50)  # Vary neuron counts
            exp_config.set("brain.connection_probability", 0.1 + i * 0.02)  # Vary connectivity
            
            brain = pynlm.createBrain(exp_config)
            self.experiments.append((brain, agent_brain))
        
        try:
            yield self.experiments
        finally:
            # Cleanup all experiments
            for brain, _ in self.experiments:
                brain.reset()
            self.experiments.clear()

# Run parallel experiments
orchestrator = ExperimentOrchestrator()
with orchestrator.run_parallel_experiments(3) as experiments:
    for i, (brain, agent_brain) in enumerate(experiments):
        print(f"Starting parallel experiment {i + 1}")
        brain.initialize()
        for step in range(100):
            brain.step(step)
```

---

## Advanced Agent Training

### 2.1 Curriculum Learning Implementation

```python
import pynlm
import numpy as np

class CurriculumLearner:
    def __init__(self):
        self.episodes = []
        self.performance_history = []
    
    def create_curriculum(self, stages):
        """Create curriculum with progressively harder tasks."""
        curriculum = []
        for stage_name, params in stages.items():
            curriculum.append({
                'name': stage_name,
                'neuron_count': params.get('neuron_count', 100),
                'connection_probability': params.get('connection_probability', 0.1),
                'steps_per_stage': params.get('steps_per_stage', 100),
                'difficulty': params.get('difficulty', 1)
            })
        return curriculum
    
    def train_with_curriculum(self, curriculum, world):
        """Train agent using curriculum learning."""
        for stage in curriculum:
            print(f"Stage: {stage['name']} (Difficulty: {stage['difficulty']})")
            
            # Create stage-specific configuration
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", stage['neuron_count'])
            config.set("brain.connection_probability", stage['connection_probability'])
            
            # Create and train agent
            brain = pynlm.createBrain(config)
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Enable all learning subsystems
            agent.enableRewardModulation(True)
            agent.enableStructuralPlasticity(True)
            agent.enableDevelopment(True)
            agent.enableCuriosity(True)
            
            # Train for this stage
            stage_results = self.train_stage(agent, world, stage['steps_per_stage'])
            self.episodes.append(stage_results)
            
            # Cleanup
            brain.reset()
    
    def train_stage(self, agent, world, num_steps):
        """Train agent for a specific stage."""
        stage_results = {
            'steps_completed': 0,
            'average_reward': 0.0,
            'final_performance': 0.0,
            'convergence_step': None
        }
        
        rewards = []
        for step in range(num_steps):
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Agent thinks
            brain = agent.getBrain()
            brain.step(step)
            
            # Get action and execute
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            rewards.append(reward)
        
        stage_results['steps_completed'] = num_steps
        stage_results['average_reward'] = np.mean(rewards)
        stage_results['final_performance'] = np.std(rewards) if len(rewards) > 1 else 0.0
        
        # Check for convergence
        if len(rewards) >= 10:
            recent_rewards = rewards[-10:]
            if np.std(recent_rewards) < 0.01:  # Low variance indicates convergence
                stage_results['convergence_step'] = num_steps - len(recent_rewards)
        
        return stage_results

# Create curriculum
stages = {
    'simple': {
        'neuron_count': 100,
        'connection_probability': 0.05,
        'steps_per_stage': 200,
        'difficulty': 1
    },
    'moderate': {
        'neuron_count': 200,
        'connection_probability': 0.1,
        'steps_per_stage': 300,
        'difficulty': 2
    },
    'complex': {
        'neuron_count': 300,
        'connection_probability': 0.15,
        'steps_per_stage': 400,
        'difficulty': 3
    }
}

# Run curriculum learning
learner = CurriculumLearner()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

learner.train_with_curriculum(learner.create_curriculum(stages), world)

# Analyze results
print("Curriculum Learning Results:")
for i, episode in enumerate(learner.episodes):
    print(f"  Stage {i + 1}: Avg reward={episode['average_reward']:.3f}, "
          f"Std={episode['final_performance']:.3f}, "
          f"Convergence at step {episode['convergence_step'] or 'N/A'}")
```

### 2.2 Meta-Learning with Multiple Agents

```python
import pynlm
import copy

class MetaLearner:
    def __init__(self, num_agents=5):
        self.num_agents = num_agents
        self.agents = []
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
        self.world.reset()
    
    def initialize_agents(self):
        """Initialize multiple agents with different configurations."""
        for i in range(self.num_agents):
            # Create diverse agent configurations
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", 100 + i * 50)  # Vary neuron counts
            config.set("brain.connection_probability", 0.05 + i * 0.02)  # Vary connectivity
            config.set("random_seed", i * 1000)  # Different random seeds
            
            brain = pynlm.createBrain(config)
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(self.world)
            
            # Enable learning for all agents
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            
            self.agents.append(agent)
    
    def run_meta_training(self, episodes_per_agent=100):
        """Train all agents in parallel and collect meta-data."""
        meta_data = {
            'agent_configs': [],
            'performance_history': [],
            'convergence_curves': []
        }
        
        for agent_idx, agent in enumerate(self.agents):
            print(f"Training agent {agent_idx + 1}/{self.num_agents}")
            
            # Track performance over time
            rewards = []
            learning_rates = []
            curiosity_levels = []
            
            for episode in range(episodes_per_agent):
                # Reset environment
                self.world.reset()
                
                # Run episode
                episode_rewards = self.run_episode(agent)
                rewards.extend(episode_rewards)
                
                # Record learning metrics
                learning_rates.append(agent.getNeuromodulationLevel())
                curiosity_levels.append(agent.getCuriosityLevel())
            
            # Store agent data
            meta_data['agent_configs'].append({
                'neuron_count': agent.getBrain().getTotalNeuronCount(),
                'connection_probability': self.get_connection_probability(agent)
            })
            
            meta_data['performance_history'].append({
                'rewards': rewards,
                'average_reward': np.mean(rewards),
                'reward_std': np.std(rewards),
                'learning_rates': learning_rates,
                'curiosity_levels': curiosity_levels
            })
            
            # Calculate convergence
            convergence_data = self.analyze_convergence(rewards)
            meta_data['convergence_curves'].append(convergence_data)
        
        return meta_data
    
    def run_episode(self, agent, max_steps=100):
        """Run single episode and return rewards."""
        episode_rewards = []
        
        for step in range(max_steps):
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Agent thinks
            brain = agent.getBrain()
            brain.step(step)
            
            # Get action and execute
            action = agent.decodeMotorCommand()
            result = self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply reward modulation
            agent.applyRewardModulation(result.reward, 0.0)
            
            episode_rewards.append(result.reward)
        
        return episode_rewards
    
    def analyze_convergence(self, rewards, window=20):
        """Analyze convergence of learning curve."""
        if len(rewards) < window:
            return {'converged': False, 'convergence_step': None}
        
        # Check for low variance in recent rewards
        recent_rewards = rewards[-window:]
        if np.std(recent_rewards) < 0.01:
            return {
                'converged': True,
                'convergence_step': len(rewards) - window,
                'final_performance': np.mean(recent_rewards)
            }
        else:
            return {'converged': False, 'convergence_step': None}
    
    def get_connection_probability(self, agent):
        """Extract connection probability from agent brain."""
        # This would need to be implemented based on the actual agent configuration
        return 0.1  # Placeholder

# Run meta-learning
meta_learner = MetaLearner(num_agents=3)
meta_learner.initialize_agents()

meta_data = meta_learner.run_meta_training(episodes_per_agent=50)

# Analyze meta-data
print("Meta-Learning Results:")
for agent_idx, (config, perf_history) in enumerate(zip(
    meta_data['agent_configs'], meta_data['performance_history'])):
    print(f"  Agent {agent_idx + 1}:")
    print(f"    Config: {config}")
    print(f"    Average reward: {perf_history['average_reward']:.3f}")
    print(f"    Reward std: {perf_history['reward_std']:.3f}")
    
    convergence = meta_data['convergence_curves'][agent_idx]
    if convergence['converged']:
        print(f"    Converged at step: {convergence['convergence_step']}")
        print(f"    Final performance: {convergence['final_performance']:.3f}")
```

---

## Performance Optimization

### 3.1 Memory Optimization Techniques

```python
import pynlm
import psutil
import gc
import time

class MemoryOptimizedSimulation:
    def __init__(self, brain_config, world_config):
        self.brain_config = brain_config
        self.world_config = world_config
        self.memory_history = []
        self.time_history = []
    
    def run_memory_efficient(self, num_steps=1000, checkpoint_interval=100):
        """Run simulation with memory optimization."""
        import tracemalloc
        
        # Start memory tracking
        tracemalloc.start()
        
        # Create brain and world
        brain = pynlm.createBrain(self.brain_config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(**self.world_config)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable memory-efficient settings
        agent.enableCuriosity(True)
        
        try:
            for step in range(num_steps):
                # Force garbage collection periodically
                if step % 50 == 0:
                    gc.collect()
                
                # Run simulation step
                world.update(0.1)
                
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                
                brain.step(step)
                
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Record memory usage
                current, peak = tracemalloc.get_traced_memory()
                self.memory_history.append(peak / 1024 / 1024)  # MB
                self.time_history.append(time.time())
                
                # Create checkpoints
                if step % checkpoint_interval == 0:
                    checkpoint_path = f"checkpoint_step_{step}.bin"
                    brain.save(checkpoint_path)
                    print(f"Checkpoint saved: {checkpoint_path}")
                    
        finally:
            # Cleanup
            brain.reset()
            tracemalloc.stop()
        
        return self.get_memory_efficiency_report()
    
    def get_memory_efficiency_report(self):
        """Generate memory efficiency report."""
        if not self.memory_history:
            return {}
        
        return {
            'total_steps': len(self.memory_history),
            'peak_memory_mb': max(self.memory_history),
            'average_memory_mb': sum(self.memory_history) / len(self.memory_history),
            'memory_growth_rate': self.calculate_memory_growth_rate(),
            'steps_per_mb': len(self.memory_history) / max(self.memory_history, 1)
        }
    
    def calculate_memory_growth_rate(self):
        """Calculate memory growth rate over time."""
        if len(self.memory_history) < 2:
            return 0.0
        
        # Simple linear regression
        x = np.arange(len(self.memory_history))
        y = np.array(self.memory_history)
        
        slope = np.polyfit(x, y, 1)[0]
        return slope

# Create memory-optimized configuration
brain_config = pynlm.createDefaultConfig()
brain_config.set("brain.neuron_count", 200)
brain_config.set("brain.connection_probability", 0.05)

world_config = {
    'width': 20,
    'height': 20,
    'visionWidth': 8,
    'visionHeight': 8
}

# Run memory-optimized simulation
simulator = MemoryOptimizedSimulation(brain_config, world_config)
results = simulator.run_memory_efficient(num_steps=500, checkpoint_interval=100)

print("Memory Optimization Results:")
for key, value in results.items():
    if isinstance(value, float):
        print(f"  {key}: {value:.2f}")
    else:
        print(f"  {key}: {value}")
```

### 3.2 Parallel Processing for Large-Scale Simulations

```python
import pynlm
import multiprocessing as mp
from functools import partial
import time
import numpy as np

class ParallelSimulation:
    def __init__(self, world_config):
        self.world_config = world_config
        self.results_queue = mp.Queue()
        self.processes = []
    
    def run_single_simulation(self, config_tuple):
        """Run single simulation in separate process."""
        import pynlm
        import numpy as np
        
        neuron_count, connection_prob, random_seed = config_tuple
        
        # Create configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", neuron_count)
        config.set("brain.connection_probability", connection_prob)
        config.set("random_seed", random_seed)
        
        # Run simulation
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(**self.world_config)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Collect performance metrics
        rewards = []
        for step in range(100):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            result = world.applyMotorCommand(action, world.getSimulationTime())
            
            agent.applyRewardModulation(result.reward, 0.0)
            
            rewards.append(result.reward)
        
        # Return results
        return {
            'neuron_count': neuron_count,
            'connection_prob': connection_prob,
            'average_reward': np.mean(rewards),
            'reward_std': np.std(rewards),
            'max_reward': max(rewards),
            'min_reward': min(rewards)
        }
    
    def run_parallel_simulations(self, config_variations, num_processes=None):
        """Run multiple simulations in parallel."""
        if num_processes is None:
            num_processes = mp.cpu_count()
        
        # Create process pool
        with mp.Pool(processes=num_processes) as pool:
            # Prepare configurations
            config_tuples = []
            for i, variation in enumerate(config_variations):
                config_tuples.append((variation['neuron_count'], 
                                    variation['connection_prob'],
                                    variation['random_seed']))
            
            # Run simulations in parallel
            start_time = time.time()
            results = pool.map(self.run_single_simulation, config_tuples)
            parallel_time = time.time() - start_time
            
            # Collect results
            successful_results = [r for r in results if r is not None]
        
        return {
            'results': successful_results,
            'parallel_time': parallel_time,
            'num_processes': num_processes,
            'speedup': len(config_variations) * 100 / parallel_time if parallel_time > 0 else 0
        }
    
    def analyze_parallel_results(self, parallel_results):
        """Analyze results from parallel simulations."""
        results = parallel_results['results']
        
        if not results:
            return {}
        
        # Analyze across different configurations
        analysis = {
            'by_neuron_count': {},
            'by_connection_prob': {},
            'overall': {
                'total_simulations': len(results),
                'average_reward': np.mean([r['average_reward'] for r in results]),
                'reward_std': np.std([r['average_reward'] for r in results]),
                'best_performance': max(results, key=lambda x: x['average_reward'])
            }
        }
        
        # Group by neuron count
        for result in results:
            neuron_count = result['neuron_count']
            if neuron_count not in analysis['by_neuron_count']:
                analysis['by_neuron_count'][neuron_count] = []
            analysis['by_neuron_count'][neuron_count].append(result['average_reward'])
        
        # Group by connection probability
        for result in results:
            conn_prob = result['connection_prob']
            if conn_prob not in analysis['by_connection_prob']:
                analysis['by_connection_prob'][conn_prob] = []
            analysis['by_connection_prob'][conn_prob].append(result['average_reward'])
        
        return analysis

# Define configuration variations for parallel testing
config_variations = []
for neuron_count in [100, 200, 300, 400]:
    for conn_prob in [0.05, 0.1, 0.15]:
        config_variations.append({
            'neuron_count': neuron_count,
            'connection_prob': conn_prob,
            'random_seed': hash((neuron_count, conn_prob)) % 10000
        })

# Run parallel simulations
world_config = {
    'width': 15,
    'height': 15,
    'visionWidth': 6,
    'visionHeight': 6
}

parallel_sim = ParallelSimulation(world_config)
parallel_results = parallel_sim.run_parallel_simulations(config_variations, num_processes=4)
analysis = parallel_sim.analyze_parallel_results(parallel_results)

print("Parallel Simulation Results:")
print(f"  Total simulations: {parallel_results['total_simulations']}")
print(f"  Parallel time: {parallel_results['parallel_time']:.2f}s")
print(f"  Speedup: {parallel_results['speedup']:.2f}x")
print(f"  Overall average reward: {analysis['overall']['average_reward']:.3f}")

print("\nResults by neuron count:")
for neuron_count, rewards in analysis['by_neuron_count'].items():
    print(f"  {neuron_count} neurons: Avg reward={np.mean(rewards):.3f}, Std={np.std(rewards):.3f}")
```

---

## Research-Grade Experiments

### 4.1 Neural Development Studies

```python
import pynlm
import matplotlib.pyplot as plt
import numpy as np

class NeuralDevelopmentStudy:
    def __init__(self):
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=25, height=25, visionWidth=10, visionHeight=10)
        self.world.reset()
        
        self.results = {
            'developmental_stages': [],
            'performance_by_stage': {},
            'synaptic_changes': [],
            'behavior_patterns': []
        }
    
    def run_development_study(self, num_stages=5, steps_per_stage=200):
        """Run comprehensive neural development study."""
        print("Starting Neural Development Study...")
        
        # Define developmental stages
        stages = self.define_development_stages(num_stages)
        
        for stage_idx, (stage_name, stage_params) in enumerate(stages.items()):
            print(f"\n--- Stage {stage_idx + 1}: {stage_name} ---")
            
            # Create stage-specific configuration
            config = pynlm.createDefaultConfig()
            self.configure_for_stage(config, stage_params)
            
            # Create brain and agent
            brain = pynlm.createBrain(config)
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(self.world)
            
            # Enable all learning subsystems
            agent.enableRewardModulation(True)
            agent.enableStructuralPlasticity(True)
            agent.enableDevelopment(True)
            agent.enableCuriosity(True)
            
            # Record initial state
            initial_neural_info = self.record_neural_state(brain, agent)
            
            # Run stage simulation
            stage_results = self.run_development_stage(agent, stage_params, steps_per_stage)
            
            # Record final state
            final_neural_info = self.record_neural_state(brain, agent)
            
            # Analyze developmental changes
            developmental_analysis = self.analyze_developmental_changes(
                initial_neural_info, final_neural_info, stage_results
            )
            
            # Store results
            self.results['developmental_stages'].append({
                'name': stage_name,
                'parameters': stage_params,
                'initial_state': initial_neural_info,
                'final_state': final_neural_info,
                'developmental_changes': developmental_analysis,
                'performance': stage_results['performance']
            })
            
            # Analyze behavior patterns
            behavior_pattern = self.analyze_behavior_patterns(stage_results)
            self.results['behavior_patterns'].append(behavior_pattern)
            
            # Cleanup
            brain.reset()
        
        # Generate comprehensive analysis
        self.generate_development_report()
        
        return self.results
    
    def define_development_stages(self, num_stages):
        """Define neural development stages with progressive complexity."""
        stages = {}
        
        # Early developmental stages (simple)
        stages['sensory_processing'] = {
            'description': 'Basic sensory processing and response',
            'neuron_count': 50 + num_stages * 20,
            'connection_probability': 0.03 + num_stages * 0.01,
            'plasticity_rate': 0.5 + num_stages * 0.1,
            'developmental_focus': ['sensory_integration', 'basic_learning']
        }
        
        # Mid developmental stages
        stages['cognitive_formation'] = {
            'description': 'Concept formation and cognitive mapping',
            'neuron_count': 100 + num_stages * 30,
            'connection_probability': 0.08 + num_stages * 0.02,
            'plasticity_rate': 1.0 + num_stages * 0.15,
            'developmental_focus': ['concept_formation', 'predictive_learning', 'attention']
        }
        
        # Late developmental stages (complex)
        stages['executive_function'] = {
            'description': 'Executive function and planning',
            'neuron_count': 150 + num_stages * 40,
            'connection_probability': 0.15 + num_stages * 0.03,
            'plasticity_rate': 2.0 + num_stages * 0.2,
            'developmental_focus': ['planning', 'problem_solving', 'working_memory']
        }
        
        if num_stages > 3:
            stages['specialization'] = {
                'description': 'Functional specialization and refinement',
                'neuron_count': 200 + num_stages * 50,
                'connection_probability': 0.20 + num_stages * 0.04,
                'plasticity_rate': 3.0 + num_stages * 0.25,
                'developmental_focus': ['specialization', 'efficiency', 'adaptation']
            }
        
        return stages
    
    def configure_for_stage(self, config, stage_params):
        """Configure brain for specific developmental stage."""
        config.set("brain.neuron_count", stage_params['neuron_count'])
        config.set("brain.connection_probability", stage_params['connection_probability'])
        config.set("brain.plasticity_rate", stage_params['plasticity_rate'])
        config.set("development.stage", stage_params['developmental_focus'][0])
        config.set("neuromodulation.curiosity.enabled", True)
        config.set("neuromodulation.reward.scale", stage_params['plasticity_rate'] / 3.0)
    
    def record_neural_state(self, brain, agent):
        """Record comprehensive neural state for a given point in development."""
        return {
            'developmental_stage': brain.getDevelopmentalStage(),
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'active_neurons': brain.getActiveNeuronCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'learning_rate': agent.getNeuromodulationLevel(),
            'curiosity_level': agent.getCuriosityLevel(),
            'novelty_level': agent.getNoveltyLevel(),
            'prediction_error': agent.getPredictionError(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'excitatory_inhibition_ratio': brain.getExcitationInhibitionRatio()
        }
    
    def run_development_stage(self, agent, stage_params, num_steps):
        """Run simulation for a specific developmental stage."""
        brain = agent.getBrain()
        
        performance_metrics = {
            'steps_completed': num_steps,
            'average_reward': 0.0,
            'reward_variance': 0.0,
            'learning_curve': [],
            'behavioral_diversity': 0.0
        }
        
        rewards = []
        
        for step in range(num_steps):
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain thinks
            brain.step(step)
            
            # Get action and execute
            action = agent.decodeMotorCommand()
            result = self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply reward modulation
            agent.applyRewardModulation(result.reward, 0.0)
            
            rewards.append(result.reward)
        
        performance_metrics['average_reward'] = np.mean(rewards)
        performance_metrics['reward_variance'] = np.var(rewards) if len(rewards) > 1 else 0.0
        performance_metrics['learning_curve'] = rewards
        performance_metrics['behavioral_diversity'] = self.calculate_behavioral_diversity(rewards)
        
        return {
            'performance': performance_metrics,
            'rewards': rewards,
            'final_agent_state': self.record_neural_state(brain, agent)
        }
    
    def analyze_developmental_changes(self, initial_state, final_state, stage_results):
        """Analyze changes that occur during developmental stage."""
        changes = {
            'growth_rate': {},
            'functional_changes': {},
            'learning_efficiency': 0.0
        }
        
        # Calculate growth rates
        for key in initial_state:
            if key in final_state:
                initial_val = initial_state[key]
                final_val = final_state[key]
                
                if initial_val != 0:
                    growth_rate = (final_val - initial_val) / initial_val
                    changes['growth_rate'][key] = growth_rate
        
        # Analyze functional changes
        changes['functional_changes'] = {
            'developmental_progress': (
                final_state['developmental_stage'].value - 
                initial_state['developmental_stage'].value
            ) / initial_state['developmental_stage'].value,
            'performance_improvement': (
                stage_results['performance']['average_reward'] - 
                initial_state.get('average_reward', 0)
            ) / max(initial_state.get('average_reward', 1), 0.001),
            'neural_efficiency': (
                final_state['average_firing_rate'] / 
                initial_state['average_firing_rate']
            ) if initial_state['average_firing_rate'] > 0 else 1.0
        }
        
        # Calculate learning efficiency
        if len(stage_results['rewards']) > 10:
            recent_rewards = stage_results['rewards'][-10:]
            learning_efficiency = np.mean(recent_rewards) / np.std(recent_rewards) if np.std(recent_rewards) > 0 else 0
            changes['learning_efficiency'] = learning_efficiency
        
        return changes
    
    def calculate_behavioral_diversity(self, rewards):
        """Calculate diversity of behavioral patterns."""
        if len(rewards) < 2:
            return 0.0
        
        # Calculate coefficient of variation as diversity measure
        mean_reward = np.mean(rewards)
        std_reward = np.std(rewards)
        
        if mean_reward == 0:
            return 0.0
        
        return std_reward / abs(mean_reward)
    
    def analyze_behavior_patterns(self, stage_results):
        """Analyze behavioral patterns during stage."""
        rewards = stage_results['rewards']
        
        if len(rewards) < 10:
            return {'pattern_type': 'insufficient_data', 'stability': 0.0}
        
        # Identify patterns
        pattern_analysis = {
            'pattern_type': self.identify_pattern_type(rewards),
            'stability': self.calculate_stability(rewards),
            'trend': self.calculate_trend(rewards),
            'cycles': self.detect_cycles(rewards)
        }
        
        return pattern_analysis
    
    def identify_pattern_type(self, rewards):
        """Identify the type of behavioral pattern."""
        if len(rewards) < 20:
            return 'unstable'
        
        # Check for different pattern types
        if np.std(rewards) < 0.01 * np.mean(abs(rewards)):
            return 'stable'
        elif np.mean(rewards) > 0.5 * np.max(rewards):
            return 'high_performance'
        elif np.mean(rewards) < -0.5 * np.max(abs(rewards)):
            return 'low_performance'
        else:
            return 'exploratory'
    
    def calculate_stability(self, rewards):
        """Calculate stability of behavioral patterns."""
        if len(rewards) < 10:
            return 0.0
        
        # Calculate autocorrelation
        autocorr = np.corrcoef(rewards[:-1], rewards[1:])
        return autocorr[0, 1] if not np.isnan(autocorr[0, 1]) else 0.0
    
    def calculate_trend(self, rewards):
        """Calculate trend in behavioral patterns."""
        if len(rewards) < 2:
            return 0.0
        
        x = np.arange(len(rewards))
        slope = np.polyfit(x, rewards, 1)[0]
        return slope
    
    def detect_cycles(self, rewards):
        """Detect cycles in behavioral patterns."""
        if len(rewards) < 20:
            return {'has_cycles': False, 'cycle_period': None}
        
        # Simple cycle detection using peak finding
        from scipy.signal import find_peaks
        peaks, properties = find_peaks(np.array(rewards))
        
        if len(peaks) >= 3:
            cycle_lengths = np.diff(peaks)
            avg_cycle_length = np.mean(cycle_lengths)
            return {'has_cycles': True, 'cycle_period': avg_cycle_length}
        else:
            return {'has_cycles': False, 'cycle_period': None}
    
    def generate_development_report(self):
        """Generate comprehensive development report."""
        print("\n" + "="*60)
        print("NEURAL DEVELOPMENT STUDY REPORT")
        print("="*60)
        
        for i, stage in enumerate(self.results['developmental_stages']):
            print(f"\nStage {i + 1}: {stage['name']}")
            print(f"  Description: {stage['parameters']['description']}")
            print(f"  Neurons: {stage['initial_state']['neuron_count']} -> {stage['final_state']['neuron_count']}")
            print(f"  Growth Rate: {stage['developmental_changes']['growth_rate'].get('neuron_count', 0):.2%}")
            print(f"  Performance Improvement: {stage['developmental_changes']['functional_changes']['performance_improvement']:.2%}")
            print(f"  Learning Efficiency: {stage['developmental_changes']['learning_efficiency']:.3f}")
            
            behavior = self.results['behavior_patterns'][i]
            print(f"  Behavior Pattern: {behavior['pattern_type']}")
            print(f"  Pattern Stability: {behavior['stability']:.3f}")
            if behavior['has_cycles']:
                print(f"  Cycle Period: {behavior['cycle_period']:.1f} steps")
        
        print("\n" + "="*60)
        print("DEVELOPMENT STUDY COMPLETE")
        print("="*60)

# Run comprehensive development study
print("Running Neural Development Study...")

development_study = NeuralDevelopmentStudy()
results = development_study.run_development_study(num_stages=3, steps_per_stage=200)

print("\nDevelopment study completed successfully!")
```

---

These advanced examples demonstrate the power and flexibility of the improved NLM Python bindings, showing how to use context managers, error recovery, configuration validation, and create sophisticated simulations for research and development purposes.