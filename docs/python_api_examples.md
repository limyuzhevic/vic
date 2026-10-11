# NLM Python API - Comprehensive Examples

This document provides comprehensive examples for using the NLM (Neural Learning Machine) Python API.
It covers basic usage, advanced features, performance optimization, and real-world applications.

## Table of Contents

1. [Basic Neuron Simulation](#basic-neuron-simulation)
2. [World-Environment Interaction](#world-environment-interaction)
3. [Complete Agent Training Workflows](#complete-agent-training-workflows)
4. [Batch Processing for Large-Scale Simulations](#batch-processing-for-large-scale-simulations)
5. [Performance Optimization Techniques](#performance-optimization-techniques)
6. [Error Handling and Debugging](#error-handling-and-debugging)
7. [Configuration File Examples](#configuration-file-examples)
8. [Multi-Agent Systems](#multi-agent-systems)
9. [Real-Time Control Systems](#real-time-control-systems)
10. [Experimental Protocol Examples](#experimental-protocol-examples)

---

## Basic Neuron Simulation

### Example 1: Simple Neuron Dynamics

```python
import pynlm
import time

# Create a simple brain configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("brain.synapse_density", 0.1)
config.set("brain.v_thresh", -50.0)
config.set("brain.v_rest", -70.0)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Initial firing rate: {brain.getAverageFiringRate():.2f} Hz")

# Run simulation with statistics collection
stats = {
    'max_firing': 0,
    'avg_firing': 0,
    'total_spikes': 0,
    'steps_completed': 0
}

start_time = time.time()
for step in range(1000):
    brain.step(step)
    
    # Collect statistics
    firing = brain.getFiringNeuronCount()
    spikes = brain.getTotalSpikeCount()
    
    stats['max_firing'] = max(stats['max_firing'], firing)
    stats['total_spikes'] += spikes
    
    if step % 100 == 0:
        print(f"Step {step}: {firing} neurons firing, {spikes} total spikes")

end_time = time.time()
stats['avg_firing'] = stats['total_spikes'] / (end_time - start_time) * 1000

print(f"\nSimulation Results:")
print(f"  Duration: {end_time - start_time:.2f} seconds")
print(f"  Max firing neurons: {stats['max_firing']}")
print(f"  Total spikes: {stats['total_spikes']}")
print(f"  Average firing rate: {stats['avg_firing']:.2f} Hz")
```

### Example 2: Neural Population Analysis

```python
import pynlm
import numpy as np

# Create brain with multiple regions
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.05)

brain = pynlm.createBrain(config)
brain.initialize()

# Add different neural regions
region_ids = []
for i in range(5):
    region_id = brain.addRegion(f"cortex_region_{i}")
    region_ids.append(region_id)

# Simulate and collect regional statistics
regional_stats = {}
for step in range(100):
    brain.step(step)
    
    for region_id in region_ids:
        region = brain.getRegion(region_id)
        if region:
            pop_stats = {
                'firing': region.getFiringNeuronCount(),
                'excitatory_inhibitory_ratio': region.getExcitationInhibitionRatio(),
                'developmental_stage': region.getDevelopmentalStage()
            }
            
            if region_id not in regional_stats:
                regional_stats[region_id] = []
            regional_stats[region_id].append(pop_stats)

print("\nRegional Analysis Results:")
for region_id, stats_list in regional_stats.items():
    avg_firing = np.mean([s['firing'] for s in stats_list])
    avg_ei_ratio = np.mean([s['excitatory_inhibitory_ratio'] for s in stats_list])
    print(f"  Region {region_id}:")
    print(f"    Average firing neurons: {avg_firing:.1f}")
    print(f"    Average E/I ratio: {avg_ei_ratio:.3f}")
```

---

## World-Environment Interaction

### Example 3: Complete Agent in Dynamic Environment

```python
import pynlm
import numpy as np
import matplotlib.pyplot as plt

class LearningAgent:
    def __init__(self, config_size=100):
        self.config = pynlm.createDefaultConfig()
        self.config.set("brain.neuron_count", config_size)
        self.config.set("brain.synapse_density", 0.08)
        
        # Create brain, world, and agent
        self.brain = pynlm.createBrain(self.config)
        self.world = pynlm.createSimpleWorld()
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Configure world
        self.world.configure(
            width=30,
            height=30,
            visionWidth=12,
            visionHeight=12
        )
        self.world.reset()
        self.world.setAgentStart(15.0, 15.0)
        
        # Initialize agent
        self.agent.initialize(self.world)
        
        # Enable learning systems
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        
        # Performance tracking
        self.performance = {
            'total_reward': 0.0,
            'novelty_score': 0.0,
            'curiosity_level': 0.0,
            'steps_completed': 0,
            'position_history': [],
            'reward_history': []
        }
    
    def run_episode(self, max_steps=500):
        """Run a complete episode of agent-environment interaction."""
        
        for step in range(max_steps):
            # Update world physics
            self.world.update(0.1)
            
            # Get sensory percept
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Brain computation
            self.brain.step(step)
            
            # Decode motor command
            motor_cmd = self.agent.decodeMotorCommand()
            
            # Apply action to world
            self.world.applyMotorCommand(motor_cmd, self.world.getSimulationTime())
            
            # Calculate reward (example: get closer to goal)
            agent_pos = self.world.getAgentBody()
            distance_to_center = np.sqrt(agent_pos.x**2 + agent_pos.y**2)
            reward = max(0, 1.0 - distance_to_center / 30.0)
            
            # Apply reward modulation
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Track performance
            self.performance['total_reward'] += reward
            self.performance['novelty_score'] = self.agent.getNoveltyLevel()
            self.performance['curiosity_level'] = self.agent.getCuriosityLevel()
            self.performance['steps_completed'] += 1
            self.performance['position_history'].append((agent_pos.x, agent_pos.y))
            self.performance['reward_history'].append(reward)
            
            # Early stopping if agent gets stuck
            if step > 50 and len(self.performance['position_history']) > 20:
                last_positions = self.performance['position_history'][-20:]
                if len(set(last_positions)) < 5:  # Agent not exploring
                    print(f"  Agent appears stuck at step {step}")
                    break
        
        return self.get_performance_summary()
    
    def get_performance_summary(self):
        """Return detailed performance metrics."""
        
        if not self.performance['position_history']:
            return None
        
        pos_history = np.array(self.performance['position_history'])
        reward_history = np.array(self.performance['reward_history'])
        
        # Calculate metrics
        start_pos = pos_history[0]
        end_pos = pos_history[-1]
        distance_travelled = np.linalg.norm(end_pos - start_pos)
        
        # Calculate exploration score (variance of positions)
        exploration_score = np.std(pos_history, axis=0).mean()
        
        # Calculate learning curves (moving average of rewards)
        window_size = min(50, len(reward_history))
        learning_curve = np.convolve(reward_history, np.ones(window_size)/window_size, mode='valid')
        
        summary = {
            'total_reward': self.performance['total_reward'],
            'avg_reward': self.performance['total_reward'] / self.performance['steps_completed'],
            'distance_travelled': distance_travelled,
            'final_position': (end_pos[0], end_pos[1]),
            'exploration_score': exploration_score,
            'final_curiosity': self.performance['curiosity_level'],
            'final_novelty': self.performance['novelty_score'],
            'developmental_stage': self.brain.getDevelopmentalStage(),
            'learning_curve': learning_curve.tolist(),
            'steps_completed': self.performance['steps_completed']
        }
        
        return summary

# Run the learning agent
print("=== Learning Agent Example ===")
agent = LearningAgent(config_size=1000)
performance = agent.run_episode(max_steps=1000)

print(f"Episode completed in {performance['steps_completed']} steps")
print(f"Total reward: {performance['total_reward']:.2f}")
print(f"Average reward per step: {performance['avg_reward']:.4f}")
print(f"Final position: {performance['final_position']}")
print(f"Exploration score: {performance['exploration_score']:.3f}")
print(f"Final developmental stage: {performance['developmental_stage']}")
```

### Example 4: Multi-Sensory Integration

```python
import pynlm
import numpy as np

# Create agent with enhanced sensory processing
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1500)

brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Create world with multiple sensory modalities
world = pynlm.createSimpleWorld()
world.configure(
    width=25,
    height=25,
    visionWidth=10,
    visionHeight=10,
    enableTouch=True,
    enableInternalSignals=True
)
world.reset()
agent.initialize(world)

# Enable all neuromodulation systems
agent.enableRewardModulation(True)
agent.enableNoveltyDetection(True)
agent.enableCuriosityDrivenExploration(True)
agent.enablePredictionErrorLearning(True)

print("=== Multi-Sensory Integration Example ===")

# Run simulation with detailed sensory analysis
sensory_data = {
    'vision': [],
    'touch': [],
    'internal': [],
    'time_stamps': []
}

for step in range(200):
    world.update(0.1)
    percept = world.getSensoryPercept()
    
    # Analyze each sensory modality
    vision_data = percept.getVision()
    touch_data = percept.getTouch() if hasattr(percept, 'getTouch') else []
    internal_data = percept.getInternal() if hasattr(percept, 'getInternal') else []
    
    sensory_data['vision'].append(np.mean(vision_data))
    sensory_data['touch'].append(np.mean(touch_data) if touch_data else 0.0)
    sensory_data['internal'].append(np.mean(internal_data) if internal_data else 0.0)
    sensory_data['time_stamps'].append(step * 0.1)
    
    # Process in brain
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Analyze sensory integration patterns
print("Sensory Integration Analysis:")
for modality, data in sensory_data.items():
    if data:
        mean_val = np.mean(data)
        std_val = np.std(data)
        print(f"  {modality.capitalize()}:")
        print(f"    Mean: {mean_val:.4f}")
        print(f"    Std: {std_val:.4f}")
        print(f"    Range: {np.min(data):.4f} to {np.max(data):.4f}")

print(f"Brain developmental stage: {brain.getDevelopmentalStage()}")
print(f"Agent curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Agent novelty level: {agent.getNoveltyLevel():.3f}")
```

---

## Complete Agent Training Workflows

### Example 5: Hierarchical Training Protocol

```python
import pynlm
import time
import numpy as np

class HierarchicalTrainingProtocol:
    def __init__(self, base_config_size=1000):
        self.config = pynlm.createDefaultConfig()
        self.config.set("brain.neuron_count", base_config_size)
        self.training_phases = {}
        
    def create_agent(self, config_size_multiplier=1.0):
        """Create a new agent with scaled configuration."""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", int(self.config.get("brain.neuron_count") * config_size_multiplier))
        config.set("brain.synapse_density", 0.05 / config_size_multiplier)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        return brain
    
    def phase1_basic_learning(self, brain, steps=500):
        """Phase 1: Basic synaptic learning and stabilization."""
        print("Phase 1: Basic Learning")
        print("-" * 30)
        
        stats = {'spikes': [], 'firing_rates': [], 'plasticity': []}
        
        for step in range(steps):
            brain.step(step)
            
            # Collect statistics
            stats['spikes'].append(brain.getTotalSpikeCount())
            stats['firing_rates'].append(brain.getAverageFiringRate())
            
            if step % 100 == 0:
                print(f"  Step {step}: {brain.getFiringNeuronCount()} firing neurons")
        
        return stats
    
    def phase2_reward_learning(self, brain, world, agent, steps=1000):
        """Phase 2: Reward-modulated learning."""
        print("\nPhase 2: Reward-Modulated Learning")
        print("-" * 30)
        
        # Enable learning systems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        episode_rewards = []
        exploration_scores = []
        
        for episode in range(steps // 100):
            world.reset()
            episode_reward = 0.0
            max_curiosity = 0.0
            
            for step in range(100):
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(episode * 100 + step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Calculate reward
                agent_pos = world.getAgentBody()
                distance = np.sqrt(agent_pos.x**2 + agent_pos.y**2)
                reward = max(0, 1.0 - distance / 25.0)
                
                agent.applyRewardModulation(reward, 0.0)
                episode_reward += reward
                max_curiosity = max(max_curiosity, agent.getCuriosityLevel())
            
            episode_rewards.append(episode_reward)
            exploration_scores.append(max_curiosity)
            
            if episode % 10 == 0:
                avg_reward = np.mean(episode_rewards[-10:])
                avg_curiosity = np.mean(exploration_scores[-10:])
                print(f"  Episode {episode}: Avg reward {avg_reward:.3f}, Max curiosity {avg_curiosity:.3f}")
        
        return {
            'episode_rewards': episode_rewards,
            'exploration_scores': exploration_scores,
            'final_curiosity': agent.getCuriosityLevel(),
            'final_novelty': agent.getNoveltyLevel()
        }
    
    def phase3_advanced_learning(self, brain, world, agent, steps=2000):
        """Phase 3: Advanced learning with multiple strategies."""
        print("\nPhase 3: Advanced Learning")
        print("-" * 30)
        
        # Enable all advanced systems
        agent.enableRewardModulation(True)
        agent.enableNoveltyDetection(True)
        agent.enableCuriosityDrivenExploration(True)
        agent.enablePredictionErrorLearning(True)
        agent.enableDevelopment(True)
        
        learning_metrics = {
            'reward_modulation': [],
            'curiosity_driven': [],
            'prediction_errors': [],
            'developmental_stages': []
        }
        
        for step in range(steps):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Calculate and apply multiple reward signals
            base_reward = 0.1  # Small constant reward for survival
            agent_pos = world.getAgentBody()
            goal_reward = max(0, 1.0 - np.sqrt(agent_pos.x**2 + agent_pos.y**2) / 25.0)
            total_reward = base_reward + goal_reward
            
            # Apply different types of modulation
            agent.applyRewardModulation(total_reward, 0.0)
            
            # Record metrics
            learning_metrics['reward_modulation'].append(total_reward)
            learning_metrics['curiosity_driven'].append(agent.getCuriosityLevel())
            learning_metrics['prediction_errors'].append(agent.getPredictionError())
            learning_metrics['developmental_stages'].append(brain.getDevelopmentalStage())
            
            if step % 200 == 0:
                stage = brain.getDevelopmentalStage()
                curiosity = agent.getCuriosityLevel()
                print(f"  Step {step}: Stage {stage}, Curiosity {curiosity:.3f}")
        
        return learning_metrics
    
    def run_complete_training(self):
        """Execute the complete hierarchical training protocol."""
        print("=== Hierarchical Training Protocol ===")
        
        # Phase 1: Create and basic train
        brain1 = self.create_agent(1.0)
        phase1_stats = self.phase1_basic_learning(brain1, 500)
        
        # Phase 2: Reward learning with world interaction
        world = pynlm.createSimpleWorld()
        world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
        agent1 = pynlm.createAgentBrain(brain1)
        world.reset()
        agent1.initialize(world)
        
        phase2_results = self.phase2_reward_learning(brain1, world, agent1, 1000)
        
        # Phase 3: Advanced learning with enhanced capabilities
        brain2 = self.create_agent(1.5)
        agent2 = pynlm.createAgentBrain(brain2)
        
        # Reuse the same world but create new agent
        world2 = pynlm.createSimpleWorld()
        world2.configure(width=35, height=35, visionWidth=12, visionHeight=12)
        world2.reset()
        agent2.initialize(world2)
        
        phase3_results = self.phase3_advanced_learning(brain2, world2, agent2, 2000)
        
        # Training summary
        print("\n=== Training Summary ===")
        print(f"Phase 1 - Basic Learning:")
        print(f"  Total spikes: {sum(phase1_stats['spikes'])}")
        print(f"  Average firing rate: {np.mean(phase1_stats['firing_rates']):.2f} Hz")
        
        print(f"\nPhase 2 - Reward Learning:")
        print(f"  Final episode reward: {phase2_results['episode_rewards'][-1]:.3f}")
        print(f"  Average episode reward: {np.mean(phase2_results['episode_rewards']):.3f}")
        print(f"  Final curiosity level: {phase2_results['final_curiosity']:.3f}")
        
        print(f"\nPhase 3 - Advanced Learning:")
        print(f"  Final developmental stage: {phase3_results['developmental_stages'][-1]}")
        print(f"  Average curiosity: {np.mean(phase3_results['curiosity_driven']):.3f}")
        print(f"  Average prediction error: {np.mean(phase3_results['prediction_errors']):.3f}")
        
        return {
            'phase1_stats': phase1_stats,
            'phase2_results': phase2_results,
            'phase3_results': phase3_results,
            'final_brain1_stage': brain1.getDevelopmentalStage(),
            'final_brain2_stage': brain2.getDevelopmentalStage()
        }

# Execute the training protocol
protocol = HierarchicalTrainingProtocol()
results = protocol.run_complete_training()
```

---

## Batch Processing for Large-Scale Simulations

### Example 6: Batch Processing Framework

```python
import pynlm
import numpy as np
import time
import multiprocessing as mp
from concurrent.futures import ProcessPoolExecutor, ThreadPoolExecutor
import os

class BatchSimulationProcessor:
    def __init__(self, config_template, num_workers=None):
        self.config_template = config_template
        self.num_workers = num_workers or mp.cpu_count()
        self.results = []
        
    @staticmethod
    def run_single_simulation(config_dict, simulation_id, seed=None):
        """Run a single simulation with given configuration."""
        if seed is not None:
            np.random.seed(seed)
        
        config = pynlm.createDefaultConfig()
        
        # Apply configuration
        for key, value in config_dict.items():
            if '.' in key:
                # Nested configuration
                config.set(key, value)
            else:
                # Top-level configuration
                if key == "brain.neuron_count":
                    config.set(key, value)
                elif key == "brain.synapse_density":
                    config.set(key, value)
        
        # Create and run brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        simulation_results = {
            'simulation_id': simulation_id,
            'config': config_dict.copy(),
            'total_spikes': 0,
            'max_firing': 0,
            'avg_firing_rate': 0,
            'final_developmental_stage': 0,
            'total_time': 0
        }
        
        start_time = time.time()
        for step in range(config_dict.get('steps', 100)):
            brain.step(step)
            
            simulation_results['total_spikes'] += brain.getTotalSpikeCount()
            simulation_results['max_firing'] = max(
                simulation_results['max_firing'], 
                brain.getFiringNeuronCount()
            )
        
        end_time = time.time()
        simulation_results['total_time'] = end_time - start_time
        simulation_results['avg_firing_rate'] = (
            simulation_results['total_spikes'] / simulation_results['total_time'] * 1000
        )
        simulation_results['final_developmental_stage'] = brain.getDevelopmentalStage()
        
        return simulation_results
    
    def generate_config_variations(self, base_config, variations):
        """Generate multiple configuration variations for batch processing."""
        configs = []
        
        for var_name, var_params in variations.items():
            config = base_config.copy()
            config.update(var_params)
            configs.append(config)
        
        return configs
    
    def run_batch_synchronous(self, configs):
        """Run batch processing synchronously."""
        print(f"Running batch processing with {len(configs)} configurations (synchronous)")
        
        results = []
        for i, config in enumerate(configs):
            print(f"  Running simulation {i+1}/{len(configs)}")
            result = self.run_single_simulation(config, i)
            results.append(result)
        
        self.results = results
        return results
    
    def run_batch_parallel(self, configs, use_processes=False):
        """Run batch processing in parallel."""
        print(f"Running batch processing with {len(configs)} configurations (parallel, {'processes' if use_processes else 'threads'})")
        
        if use_processes:
            executor = ProcessPoolExecutor(max_workers=self.num_workers)
        else:
            executor = ThreadPoolExecutor(max_workers=self.num_workers)
        
        # Prepare arguments for parallel execution
        tasks = [(config, i, i * 42) for i, config in enumerate(configs)]
        
        results = []
        with executor as executor:
            # Submit all tasks
            future_to_config = {
                executor.submit(self.run_single_simulation, config, sim_id, seed): (i, config)
                for i, config, sim_id, seed in tasks
            }
            
            # Collect results as they complete
            for future in future_to_config:
                try:
                    result = future.result(timeout=300)  # 5 minute timeout
                    results.append(result)
                    print(f"  Completed simulation {result['simulation_id'] + 1}")
                except Exception as e:
                    config_info = future_to_config[future]
                    print(f"  Simulation {config_info[0] + 1} failed: {e}")
        
        self.results = results
        return results
    
    def analyze_batch_results(self, results):
        """Analyze and summarize batch results."""
        print("\n=== Batch Results Analysis ===")
        
        if not results:
            print("No results to analyze")
            return
        
        # Basic statistics
        total_spikes = [r['total_spikes'] for r in results]
        firing_rates = [r['avg_firing_rate'] for r in results]
        developmental_stages = [r['final_developmental_stage'] for r in results]
        total_times = [r['total_time'] for r in results]
        
        print(f"Total simulations: {len(results)}")
        print(f"Average total spikes: {np.mean(total_spikes):.0f}")
        print(f"Std total spikes: {np.std(total_spikes):.0f}")
        print(f"Average firing rate: {np.mean(firing_rates):.2f} Hz")
        print(f"Std firing rate: {np.std(firing_rates):.2f} Hz")
        print(f"Average simulation time: {np.mean(total_times):.2f}s")
        print(f"Std simulation time: {np.std(total_times):.2f}s")
        
        # Performance ranking
        print(f"\nTop 5 performing simulations by firing rate:")
        sorted_by_firing = sorted(results, key=lambda x: x['avg_firing_rate'], reverse=True)
        for i, result in enumerate(sorted_by_firing[:5]):
            print(f"  {i+1}. Sim {result['simulation_id']}: {result['avg_firing_rate']:.2f} Hz "
                  f"(config: {result['config'].get('brain.neuron_count', 'N/A')} neurons)")
        
        # Configuration effectiveness analysis
        config_performance = {}
        for result in results:
            config_key = f"{result['config'].get('brain.neuron_count', 'N/A')} neurons, " \
                        f"{result['config'].get('brain.synapse_density', 'N/A')} density"
            
            if config_key not in config_performance:
                config_performance[config_key] = []
            
            config_performance[config_key].append(result['avg_firing_rate'])
        
        print(f"\nPerformance by configuration:")
        for config_key, rates in config_performance.items():
            print(f"  {config_key}: {np.mean(rates):.2f} Hz ± {np.std(rates):.2f} Hz")
        
        # Save detailed results
        self.save_results(results, "batch_results.json")
        
        return results
    
    def save_results(self, results, filename):
        """Save results to JSON file."""
        import json
        
        # Convert numpy arrays to lists for JSON serialization
        serializable_results = []
        for result in results:
            serializable_result = result.copy()
            serializable_result['config'] = json.loads(json.dumps(serializable_result['config']))
            serializable_results.append(serializable_result)
        
        with open(filename, 'w') as f:
            json.dump(serializable_results, f, indent=2)
        
        print(f"Results saved to {filename}")

# Example batch processing configuration
if __name__ == "__main__":
    print("=== Batch Processing Example ===")
    
    # Base configuration template
    base_config = {
        "brain.neuron_count": 1000,
        "brain.synapse_density": 0.1,
        "steps": 100
    }
    
    # Generate configuration variations
    variations = {
        "small_network": {
            "brain.neuron_count": 500,
            "steps": 50
        },
        "medium_network": {
            "brain.neuron_count": 1500,
            "steps": 150
        },
        "large_network": {
            "brain.neuron_count": 3000,
            "steps": 300,
            "brain.synapse_density": 0.05
        },
        "high_connectivity": {
            "brain.neuron_count": 1000,
            "steps": 100,
            "brain.synapse_density": 0.2
        },
        "low_connectivity": {
            "brain.neuron_count": 1000,
            "steps": 100,
            "brain.synapse_density": 0.02
        }
    }
    
    # Create processor
    processor = BatchSimulationProcessor(base_config)
    
    # Generate all configurations
    configs = processor.generate_config_variations(base_config, variations)
    
    # Add base configuration
    configs.insert(0, base_config)
    
    # Run batch processing (parallel for better performance)
    results = processor.run_batch_parallel(configs, use_processes=True)
    
    # Analyze results
    analyzed_results = processor.analyze_batch_results(results)
```

### Example 7: Memory-Efficient Batch Processing

```python
import pynlm
import numpy as np
import gc
import psutil
import os

class MemoryEfficientBatchProcessor:
    def __init__(self, max_memory_gb=8.0):
        self.max_memory_gb = max_memory_gb
        self.results = []
        
    def get_memory_usage(self):
        """Get current memory usage in GB."""
        process = psutil.Process(os.getpid())
        return process.memory_info().rss / (1024**3)
    
    def should_continue_processing(self):
        """Check if we should continue based on memory constraints."""
        current_memory = self.get_memory_usage()
        return current_memory < self.max_memory_gb
    
    def create_chunked_configurations(self, base_config, variations, chunk_size=3):
        """Create configurations in chunks to manage memory usage."""
        configs = []
        config_items = list(variations.items())
        
        for i in range(0, len(config_items), chunk_size):
            chunk = dict(config_items[i:i + chunk_size])
            config = base_config.copy()
            config.update(chunk)
            configs.append((config, f"chunk_{i//chunk_size + 1}"))
        
        return configs
    
    def run_chunked_batch(self, base_config, variations):
        """Run batch processing in memory-managed chunks."""
        print("=== Memory-Efficient Chunked Batch Processing ===")
        print(f"Max memory: {self.max_memory_gb} GB")
        print(f"Initial memory usage: {self.get_memory_usage():.2f} GB")
        
        # Create chunked configurations
        chunked_configs = self.create_chunked_configurations(base_config, variations)
        print(f"Processing in {len(chunked_configs)} chunks")
        
        all_results = []
        
        for config, chunk_name in chunked_configs:
            print(f"\nProcessing {chunk_name}: {config}")
            print(f"Current memory usage: {self.get_memory_usage():.2f} GB")
            
            if not self.should_continue_processing():
                print(f"Memory limit reached. Skipping remaining chunks.")
                break
            
            # Run batch for this chunk
            chunk_results = self._run_single_chunk(config, chunk_name)
            all_results.extend(chunk_results)
            
            # Force garbage collection
            gc.collect()
            
            # Small delay to allow memory cleanup
            time.sleep(0.5)
            
            print(f"Chunk {chunk_name} completed. Memory usage: {self.get_memory_usage():.2f} GB")
        
        self.results = all_results
        return all_results
    
    def _run_single_chunk(self, config, chunk_name):
        """Run a single chunk of configurations."""
        chunk_results = []
        
        # Create multiple configurations within this chunk
        config_variants = [config]  # You can add more variants here
        
        for i, variant_config in enumerate(config_variants):
            # Scale down network size for memory efficiency
            scaled_config = variant_config.copy()
            scaled_config["brain.neuron_count"] = min(
                scaled_config.get("brain.neuron_count", 1000) // 2,
                1000  # Cap at reasonable size
            )
            
            # Create brain
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            brain.initialize()
            
            # Run simulation
            steps = scaled_config.get("steps", 50)
            for step in range(steps):
                brain.step(step)
            
            # Collect results
            result = {
                'chunk': chunk_name,
                'variant': i,
                'neuron_count': scaled_config.get("brain.neuron_count"),
                'steps': steps,
                'total_spikes': brain.getTotalSpikeCount(),
                'max_firing': brain.getFiringNeuronCount(),
                'avg_firing_rate': brain.getAverageFiringRate(),
                'developmental_stage': brain.getDevelopmentalStage()
            }
            
            chunk_results.append(result)
        
        return chunk_results
    
    def analyze_memory_efficient_results(self, results):
        """Analyze results from memory-efficient batch processing."""
        print("\n=== Memory-Efficient Results Analysis ===")
        
        if not results:
            print("No results to analyze")
            return
        
        print(f"Total chunks processed: {len(set(r['chunk'] for r in results))}")
        print(f"Total configurations tested: {len(results)}")
        
        # Performance summary by chunk
        for chunk in sorted(set(r['chunk'] for r in results)):
            chunk_results = [r for r in results if r['chunk'] == chunk]
            avg_firing = np.mean([r['avg_firing_rate'] for r in chunk_results])
            
            print(f"\nChunk {chunk}:")
            print(f"  Configurations: {len(chunk_results)}")
            print(f"  Average firing rate: {avg_firing:.2f} Hz")
        
        # Memory efficiency report
        print(f"\n=== Memory Efficiency Report ===")
        print(f"Successfully processed configurations within {self.max_memory_gb} GB memory limit")
        print(f"Final memory usage: {self.get_memory_usage():.2f} GB")
```

---

## Performance Optimization Techniques

### Example 8: Performance-Optimized Brain Configuration

```python
import pynlm
import time
import numpy as np
import matplotlib.pyplot as plt

class PerformanceOptimizedBrain:
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
        self.optimization_results = {}
        
    def optimize_for_performance(self, target_neurons=2000, target_time=None):
        """Optimize brain configuration for performance targets."""
        print("=== Performance Optimization ===")
        
        # Performance optimization strategies
        optimization_strategies = {
            'high_throughput': {
                'brain.neuron_count': target_neurons,
                'brain.synapse_density': 0.05,  # Lower density for faster processing
                'brain.v_thresh': -45.0,       # Higher threshold for fewer spikes
                'brain.tau_mem': 10.0,         # Faster membrane time constant
                'plasticity': False             # Disable plasticity for speed
            },
            'balanced': {
                'brain.neuron_count': target_neurons,
                'brain.synapse_density': 0.08,
                'brain.v_thresh': -50.0,
                'brain.tau_mem': 15.0,
                'plasticity': True
            },
            'learning_focused': {
                'brain.neuron_count': target_neurons,
                'brain.synapse_density': 0.12,  # Higher density for learning
                'brain.v_thresh': -55.0,       # Lower threshold for more spikes
                'brain.tau_mem': 25.0,         # Slower for integration
                'plasticity': True
            }
        }
        
        results = {}
        
        for strategy_name, strategy_config in optimization_strategies.items():
            print(f"\nTesting {strategy_name} strategy...")
            
            # Apply configuration
            for key, value in strategy_config.items():
                if key == 'plasticity' and value:
                    self.config.set("plasticity.stdp.enable", True)
                    self.config.set("plasticity.hebbian.enable", True)
                else:
                    self.config.set(key, value)
            
            # Run performance tests
            strategy_results = self._run_performance_test(strategy_config)
            results[strategy_name] = strategy_results
            
            print(f"  Average firing rate: {strategy_results['avg_firing_rate']:.2f} Hz")
            print(f"  Total processing time: {strategy_results['total_time']:.2f}s")
            print(f"  Spikes per second: {strategy_results['spikes_per_second']:.0f}")
        
        self.optimization_results = results
        return self._select_best_strategy(results, target_time)
    
    def _run_performance_test(self, config_overrides):
        """Run a single performance test."""
        config = pynlm.createDefaultConfig()
        
        # Apply performance optimizations
        for key, value in config_overrides.items():
            if key == 'plasticity' and value:
                config.set("plasticity.stdp.enable", True)
                config.set("plasticity.hebbian.enable", True)
            else:
                config.set(key, value)
        
        # Create and initialize brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run performance benchmark
        steps = config_overrides.get('steps', 100)
        spike_counts = []
        start_time = time.time()
        
        for step in range(steps):
            brain.step(step)
            spike_counts.append(brain.getTotalSpikeCount())
        
        end_time = time.time()
        
        total_time = end_time - start_time
        total_spikes = sum(spike_counts)
        
        results = {
            'avg_firing_rate': brain.getAverageFiringRate(),
            'total_time': total_time,
            'spikes_per_second': total_spikes / total_time,
            'final_developmental_stage': brain.getDevelopmentalStage(),
            'memory_usage': self._get_memory_usage()
        }
        
        return results
    
    def _get_memory_usage(self):
        """Get current memory usage."""
        import psutil
        import os
        process = psutil.Process(os.getpid())
        return process.memory_info().rss / (1024**2)  # MB
    
    def _select_best_strategy(self, results, target_time=None):
        """Select the best performing strategy."""
        print(f"\n=== Strategy Selection ===")
        
        if target_time:
            # Choose strategy that meets target time
            suitable_strategies = {
                name: res for name, res in results.items()
                if res['total_time'] <= target_time * 1.1  # Allow 10% tolerance
            }
            
            if suitable_strategies:
                best_name = max(suitable_strategies.keys(),
                               key=lambda x: suitable_strategies[x]['spikes_per_second'])
                print(f"Selected {best_name} strategy (meets {target_time}s target)")
                return best_name, suitable_strategies[best_name]
        
        # Default to highest spikes per second
        best_name = max(results.keys(), key=lambda x: results[x]['spikes_per_second'])
        best_results = results[best_name]
        
        print(f"Selected {best_name} strategy (best spikes/second)")
        print(f"Performance: {best_results['spikes_per_second']:.0f} spikes/second")
        print(f"Time: {best_results['total_time']:.2f}s")
        
        return best_name, best_results
    
    def visualize_optimization_results(self):
        """Create visualization of optimization results."""
        if not self.optimization_results:
            print("No optimization results to visualize")
            return
        
        fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(12, 10))
        fig.suptitle("Performance Optimization Results", fontsize=16)
        
        strategies = list(self.optimization_results.keys())
        
        # Firing rate comparison
        firing_rates = [self.optimization_results[s]['avg_firing_rate'] for s in strategies]
        ax1.bar(strategies, firing_rates, color=['skyblue', 'lightgreen', 'salmon'])
        ax1.set_title("Average Firing Rate")
        ax1.set_ylabel("Firing Rate (Hz)")
        ax1.grid(True, alpha=0.3)
        
        # Spikes per second comparison
        spikes_per_sec = [self.optimization_results[s]['spikes_per_second'] for s in strategies]
        ax2.bar(strategies, spikes_per_sec, color=['skyblue', 'lightgreen', 'salmon'])
        ax2.set_title("Spikes per Second")
        ax2.set_ylabel("Spikes/Second")
        ax2.grid(True, alpha=0.3)
        
        # Processing time comparison
        processing_times = [self.optimization_results[s]['total_time'] for s in strategies]
        ax3.bar(strategies, processing_times, color=['skyblue', 'lightgreen', 'salmon'])
        ax3.set_title("Processing Time")
        ax3.set_ylabel("Time (seconds)")
        ax3.grid(True, alpha=0.3)
        
        # Memory usage comparison
        memory_usages = [self.optimization_results[s]['memory_usage'] for s in strategies]
        ax4.bar(strategies, memory_usages, color=['skyblue', 'lightgreen', 'salmon'])
        ax4.set_title("Memory Usage")
        ax4.set_ylabel("Memory (MB)")
        ax4.grid(True, alpha=0.3)
        
        plt.tight_layout()
        plt.savefig('optimization_results.png', dpi=150, bbox_inches='tight')
        plt.show()
        
        print("Optimization results visualization saved as 'optimization_results.png'")

# Example performance optimization
if __name__ == "__main__":
    optimizer = PerformanceOptimizedBrain()
    
    # Optimize for different performance targets
    best_strategy, best_results = optimizer.optimize_for_performance(
        target_neurons=2000,
        target_time=2.0  # Target 2 seconds
    )
    
    print(f"\n=== Final Recommendation ===")
    print(f"Best strategy: {best_strategy}")
    print(f"Performance metrics:")
    for key, value in best_results.items():
        print(f"  {key}: {value:.4f}")
    
    # Visualize results
    optimizer.visualize_optimization_results()
```

---

## Error Handling and Debugging

### Example 9: Comprehensive Error Handling

```python
import pynlm
import traceback
import logging
import sys

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler('nlm_debug.log'),
        logging.StreamHandler(sys.stdout)
    ]
)

class SafeNLMExecutor:
    def __init__(self):
        self.logger = logging.getLogger('nlm_executor')
        self.error_history = []
        
    def safe_brain_operation(self, operation_func, *args, **kwargs):
        """Execute brain operations with comprehensive error handling."""
        try:
            result = operation_func(*args, **kwargs)
            self.logger.info(f"Operation successful: {operation_func.__name__}")
            return result, None
        except Exception as e:
            error_info = {
                'timestamp': time.time(),
                'operation': operation_func.__name__,
                'error_type': type(e).__name__,
                'error_message': str(e),
                'traceback': traceback.format_exc()
            }
            self.error_history.append(error_info)
            self.logger.error(f"Operation failed: {operation_func.__name__}", exc_info=True)
            return None, error_info
    
    def validate_brain_config(self, config):
        """Validate brain configuration before creation."""
        validation_errors = []
        
        # Check required parameters
        required_params = ['brain.neuron_count', 'brain.v_thresh', 'brain.v_rest']
        for param in required_params:
            if not config.has(param):
                validation_errors.append(f"Missing required parameter: {param}")
        
        # Validate parameter ranges
        if config.has('brain.neuron_count'):
            neuron_count = config.get('brain.neuron_count')
            if not isinstance(neuron_count, int) or neuron_count <= 0:
                validation_errors.append("neuron_count must be a positive integer")
        
        if config.has('brain.v_thresh'):
            v_thresh = config.get('brain.v_thresh')
            if not isinstance(v_thresh, (int, float)):
                validation_errors.append("v_thresh must be a numeric value")
        
        return len(validation_errors) == 0, validation_errors
    
    def create_brain_with_validation(self, config):
        """Create brain with comprehensive validation and error handling."""
        
        # Validate configuration
        is_valid, validation_errors = self.validate_brain_config(config)
        if not is_valid:
            error_info = {
                'operation': 'create_brain',
                'error_type': 'ConfigurationError',
                'error_message': f"Configuration validation failed: {validation_errors}",
                'traceback': None
            }
            self.error_history.append(error_info)
            raise ConfigurationError(f"Invalid configuration: {validation_errors}")
        
        # Try to create brain
        def create_brain():
            brain = pynlm.createBrain(config)
            brain.initialize()
            return brain
        
        result, error = self.safe_brain_operation(create_brain)
        
        if error:
            # Try alternative configuration if initialization failed
            if error['error_type'] == 'NLMException':
                self.logger.warning("Attempting alternative configuration...")
                return self._create_fallback_brain()
        
        return result
    
    def _create_fallback_brain(self):
        """Create a minimal brain configuration for fallback."""
        fallback_config = pynlm.createDefaultConfig()
        fallback_config.set("brain.neuron_count", 100)
        fallback_config.set("brain.v_thresh", -50.0)
        
        brain = pynlm.createBrain(fallback_config)
        brain.initialize()
        self.logger.info("Created fallback brain successfully")
        return brain
    
    def run_safe_simulation(self, brain, steps=100, checkpoint_interval=50):
        """Run simulation with safety checks and checkpointing."""
        simulation_results = {
            'steps_completed': 0,
            'errors': [],
            'checkpoints': [],
            'performance_metrics': {}
        }
        
        for step in range(steps):
            try:
                # Check system health
                if not self._check_system_health():
                    self.logger.warning("System health check failed, attempting recovery")
                    if not self._attempt_recovery():
                        simulation_results['errors'].append({
                            'step': step,
                            'error': 'System recovery failed'
                        })
                        break
                
                # Run brain step with error handling
                def run_step():
                    brain.step(step)
                    return True
                
                success, error = self.safe_brain_operation(run_step)
                
                if not success:
                    simulation_results['errors'].append({
                        'step': step,
                        'error': error['error_message']
                    })
                    # Continue with next step if possible
                    continue
                
                # Collect metrics
                simulation_results['steps_completed'] += 1
                
                # Create checkpoint
                if step % checkpoint_interval == 0:
                    checkpoint = self._create_checkpoint(brain, step)
                    simulation_results['checkpoints'].append(checkpoint)
                
                # Log progress periodically
                if step % 10 == 0:
                    self.logger.info(f"Step {step}/{steps} completed")
                
            except KeyboardInterrupt:
                self.logger.info("Simulation interrupted by user")
                break
            except Exception as e:
                error_info = {
                    'step': step,
                    'error': str(e),
                    'traceback': traceback.format_exc()
                }
                simulation_results['errors'].append(error_info)
                self.logger.error(f"Unexpected error at step {step}", exc_info=True)
                
                # Attempt recovery for non-critical errors
                if step < steps - 10:  # Don't attempt recovery near end
                    if not self._attempt_recovery():
                        break
        
        simulation_results['performance_metrics'] = self._calculate_performance_metrics(brain)
        return simulation_results
    
    def _check_system_health(self):
        """Check system health indicators."""
        # Check memory usage
        import psutil
        memory = psutil.virtual_memory()
        if memory.percent > 90:
            self.logger.warning(f"High memory usage: {memory.percent}%")
            return False
        
        # Check CPU usage
        cpu_percent = psutil.cpu_percent(interval=0.1)
        if cpu_percent > 95:
            self.logger.warning(f"High CPU usage: {cpu_percent}%")
            return False
        
        return True
    
    def _attempt_recovery(self):
        """Attempt system recovery."""
        self.logger.info("Attempting system recovery...")
        # Force garbage collection
        import gc
        gc.collect()
        
        # Wait a moment
        time.sleep(0.1)
        
        # Check if memory is better now
        import psutil
        memory = psutil.virtual_memory()
        return memory.percent < 90
    
    def _create_checkpoint(self, brain, step):
        """Create a simulation checkpoint."""
        checkpoint = {
            'step': step,
            'timestamp': time.time(),
            'brain_stats': {
                'firing_neurons': brain.getFiringNeuronCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'avg_firing_rate': brain.getAverageFiringRate(),
                'developmental_stage': brain.getDevelopmentalStage()
            }
        }
        
        # Try to save brain state if implemented
        try:
            # This would call brain.save() if available
            checkpoint['save_status'] = 'attempted'
        except Exception as e:
            checkpoint['save_status'] = f'failed: {str(e)}'
        
        return checkpoint
    
    def _calculate_performance_metrics(self, brain):
        """Calculate comprehensive performance metrics."""
        return {
            'final_firing_rate': brain.getAverageFiringRate(),
            'total_neurons': brain.getTotalNeuronCount(),
            'total_synapses': brain.getTotalSynapseCount(),
            'current_firing': brain.getFiringNeuronCount(),
            'efficiency_ratio': (
                brain.getAverageFiringRate() / 
                brain.getTotalSynapseCount() * 1000 if brain.getTotalSynapseCount() > 0 else 0
            )
        }
    
    def generate_error_report(self):
        """Generate comprehensive error report."""
        if not self.error_history:
            return "No errors encountered"
        
        report = f"Error Report (Total errors: {len(self.error_history)})\n"
        report += "=" * 50 + "\n"
        
        for i, error in enumerate(self.error_history):
            report += f"Error {i+1}:\n"
            report += f"  Time: {error['timestamp']}\n"
            report += f"  Operation: {error['operation']}\n"
            report += f"  Type: {error['error_type']}\n"
            report += f"  Message: {error['error_message']}\n"
            
            if error['traceback']:
                # Show last few lines of traceback
                lines = error['traceback'].split('\n')
                report += f"  Traceback (last 5 lines):\n"
                for line in lines[-5:]:
                    if line.strip():
                        report += f"    {line}\n"
            
            report += "\n"
        
        return report

# Example error handling and debugging
if __name__ == "__main__":
    print("=== Error Handling and Debugging Example ===")
    
    executor = SafeNLMExecutor()
    
    # Create configuration with potential issues
    problematic_config = pynlm.createDefaultConfig()
    # Intentionally skip some validations to demonstrate error handling
    
    try:
        # This should demonstrate validation and error handling
        brain = executor.create_brain_with_validation(problematic_config)
        print("Brain created successfully")
        
        # Run simulation with safety features
        results = executor.run_safe_simulation(brain, steps=200)
        
        print(f"\nSimulation completed: {results['steps_completed']}/{200} steps")
        print(f"Errors encountered: {len(results['errors'])}")
        
        # Generate error report
        error_report = executor.generate_error_report()
        print(f"\nError Report:\n{error_report}")
        
    except Exception as e:
        print(f"Critical error: {str(e)}")
        import traceback
        traceback.print_exc()
```

---

## Configuration File Examples

### Example 10: JSON Configuration Templates

```python
import pynlm
import json
import os

class ConfigurationManager:
    def __init__(self, config_dir="configs"):
        self.config_dir = config_dir
        self.default_config_template = {
            "brain": {
                "neuron_count": 1000,
                "synapse_density": 0.1,
                "v_thresh": -50.0,
                "v_rest": -70.0,
                "v_reset": -75.0,
                "tau_mem": 20.0,
                "tau_ref": 2.0,
                "initial_weight_mean": 0.5,
                "initial_weight_std": 0.1
            },
            "plasticity": {
                "stdp": {
                    "enable": True,
                    "learning_rate": 0.001,
                    "tau_plus": 20.0,
                    "tau_minus": 20.0
                },
                "hebbian": {
                    "enable": True,
                    "learning_rate": 0.0005
                },
                "structural": {
                    "enable": True,
                    "synaptogenesis_rate": 0.01,
                    "pruning_threshold": 0.1
                }
            },
            "neuromodulation": {
                "dopamine": {
                    "scale": 1.0,
                    "learning_modulation": True
                },
                "curiosity": {
                    "enable": True,
                    "novelty_weight": 0.7,
                    "prediction_error_weight": 0.3
                },
                "novelty": {
                    "enable": True,
                    "detection_threshold": 0.3
                }
            },
            "development": {
                "enable": True,
                "critical_period_length": 100,
                "plasticity_modifier": 0.5
            }
        }
    
    def create_default_config(self, output_path=None):
        """Create a default configuration file."""
        config = self.default_config_template.copy()
        
        if output_path:
            with open(output_path, 'w') as f:
                json.dump(config, f, indent=2)
            print(f"Default configuration saved to {output_path}")
        
        return config
    
    def load_config_from_file(self, config_path):
        """Load configuration from JSON file."""
        try:
            with open(config_path, 'r') as f:
                config_data = json.load(f)
            
            # Validate configuration structure
            self._validate_config_structure(config_data)
            
            print(f"Configuration loaded from {config_path}")
            return config_data
            
        except FileNotFoundError:
            print(f"Configuration file not found: {config_path}")
            return self.default_config_template.copy()
        except json.JSONDecodeError as e:
            print(f"Invalid JSON in configuration file: {e}")
            return self.default_config_template.copy()
    
    def _validate_config_structure(self, config_data):
        """Validate configuration structure."""
        required_sections = ['brain', 'plasticity', 'neuromodulation', 'development']
        
        for section in required_sections:
            if section not in config_data:
                raise ValueError(f"Missing required section: {section}")
        
        # Validate brain section
        if 'neuron_count' not in config_data['brain']:
            raise ValueError("brain section must contain neuron_count")
        
        # Validate plasticity section
        if 'stdp' not in config_data['plasticity']:
            raise ValueError("plasticity section must contain stdp configuration")
        
        return True
    
    def create_config_from_template(self, template_name, variations=None):
        """Create configuration from predefined templates with variations."""
        templates = {
            'small_network': {
                "brain": {"neuron_count": 500, "synapse_density": 0.05},
                "plasticity": {"stdp": {"learning_rate": 0.002}},
                "neuromodulation": {"curiosity": {"novelty_weight": 0.8}}
            },
            'medium_network': {
                "brain": {"neuron_count": 2000, "synapse_density": 0.1},
                "plasticity": {"stdp": {"learning_rate": 0.001}},
                "neuromodulation": {"curiosity": {"novelty_weight": 0.7}}
            },
            'large_network': {
                "brain": {"neuron_count": 5000, "synapse_density": 0.05},
                "plasticity": {"stdp": {"learning_rate": 0.0005}},
                "neuromodulation": {"curiosity": {"novelty_weight": 0.6}}
            },
            'high_learning': {
                "brain": {"neuron_count": 1000, "synapse_density": 0.15},
                "plasticity": {"stdp": {"learning_rate": 0.003}, "hebbian": {"learning_rate": 0.001}},
                "neuromodulation": {"curiosity": {"novelty_weight": 0.9}}
            },
            'low_learning': {
                "brain": {"neuron_count": 1000, "synapse_density": 0.05},
                "plasticity": {"stdp": {"learning_rate": 0.0001}},
                "neuromodulation": {"curiosity": {"novelty_weight": 0.3}}
            }
        }
        
        if template_name not in templates:
            available_templates = ', '.join(templates.keys())
            raise ValueError(f"Unknown template: {template_name}. Available: {available_templates}")
        
        # Start with template
        config = self.default_config_template.copy()
        config.update(templates[template_name])
        
        # Apply variations if provided
        if variations:
            config = self._apply_variations(config, variations)
        
        return config
    
    def _apply_variations(self, base_config, variations):
        """Apply variations to a configuration."""
        for key, value in variations.items():
            if '.' in key:
                # Nested key
                sections = key.split('.')
                current = base_config
                for section in sections[:-1]:
                    if section not in current:
                        current[section] = {}
                    current = current[section]
                current[sections[-1]] = value
            else:
                # Top-level key
                base_config[key] = value
        
        return base_config
    
    def save_config_to_file(self, config, filename):
        """Save configuration to file."""
        filepath = os.path.join(self.config_dir, filename)
        
        # Ensure config directory exists
        os.makedirs(self.config_dir, exist_ok=True)
        
        with open(filepath, 'w') as f:
            json.dump(config, f, indent=2)
        
        print(f"Configuration saved to {filepath}")
        return filepath
    
    def create_complete_config_set(self):
        """Create a complete set of example configuration files."""
        print("Creating complete configuration set...")
        
        # Create templates directory
        os.makedirs(self.config_dir, exist_ok=True)
        
        # Create various configuration files
        configs = {
            'default_config.json': self.default_config_template,
            'small_network_config.json': self.create_config_from_template('small_network'),
            'medium_network_config.json': self.create_config_from_template('medium_network'),
            'large_network_config.json': self.create_config_from_template('large_network'),
            'high_learning_config.json': self.create_config_from_template('high_learning'),
            'low_learning_config.json': self.create_config_from_template('low_learning')
        }
        
        # Save all configurations
        for filename, config in configs.items():
            self.save_config_to_file(config, filename)
        
        # Create configuration comparison script
        self._create_config_comparison_script(configs)
        
        print(f"All configuration files created in {self.config_dir}/")
    
    def _create_config_comparison_script(self, configs):
        """Create a script to compare different configurations."""
        script_content = '''#!/usr/bin/env python3
"""
Configuration Comparison Script
This script demonstrates how to compare different NLM configurations.
"""

import json
import sys
import os

def load_configs():
    """Load all configuration files."""
    config_dir = os.path.join(os.path.dirname(__file__), 'configs')
    configs = {}
    
    for filename in os.listdir(config_dir):
        if filename.endswith('.json') and filename != 'default_config.json':
            filepath = os.path.join(config_dir, filename)
            with open(filepath, 'r') as f:
                config_name = filename.replace('_config.json', '').replace('_', ' ')
                configs[config_name] = json.load(f)
    
    return configs

def analyze_config(config, name):
    """Analyze a configuration and print key metrics."""
    brain_config = config.get('brain', {})
    plasticity_config = config.get('plasticity', {})
    neuromod_config = config.get('neuromodulation', {})
    
    print(f"\n{name.upper()} Configuration Analysis:")
    print("-" * 40)
    print(f"Neurons: {brain_config.get('neuron_count', 'N/A')}")
    print(f"Synapse Density: {brain_config.get('synapse_density', 'N/A')}")
    print(f"STDP Enabled: {plasticity_config.get('stdp', {}).get('enable', False)}")
    print(f"Hebbian Enabled: {plasticity_config.get('hebbian', {}).get('enable', False)}")
    print(f"Curiosity Enabled: {neuromod_config.get('curiosity', {}).get('enable', False)}")
    print(f"Novelty Enabled: {neuromod_config.get('novelty', {}).get('enable', False)}")
    
    # Calculate expected characteristics
    expected_neurons = brain_config.get('neuron_count', 0)
    expected_synapses = expected_neurons * expected_neurons * brain_config.get('synapse_density', 0)
    
    print(f"Expected Synapses: {expected_synapses:.0f}")
    
    return {
        'neurons': expected_neurons,
        'synapses': expected_synapses,
        'learning_systems': sum([
            plasticity_config.get('stdp', {}).get('enable', False),
            plasticity_config.get('hebbian', {}).get('enable', False),
            neuromod_config.get('curiosity', {}).get('enable', False),
            neuromod_config.get('novelty', {}).get('enable', False)
        ])
    }

def main():
    configs = load_configs()
    
    print("=== NLM Configuration Comparison ===")
    
    analyses = {}
    for name, config in configs.items():
        analyses[name] = analyze_config(config, name)
    
    # Summary comparison
    print(f"\n=== Configuration Summary ===")
    print(f"{'Configuration':<20} {'Neurons':<10} {'Synapses':<10} {'Learning Systems':<15}")
    print("-" * 60)
    
    for name, analysis in analyses.items():
        print(f"{name:<20} {analysis['neurons']:<10} {analysis['synapses']:<10} {analysis['learning_systems']:<15}")

if __name__ == "__main__":
    main()
'''
        
        script_path = os.path.join(self.config_dir, 'compare_configs.py')
        with open(script_path, 'w') as f:
            f.write(script_content)
        
        print(f"Configuration comparison script created: {script_path}")

# Example configuration management
if __name__ == "__main__":
    print("=== Configuration Management Example ===")
    
    # Create configuration manager
    config_manager = ConfigurationManager()
    
    # Create default configuration
    default_config = config_manager.create_default_config("default_config.json")
    
    # Create configuration from template
    small_network = config_manager.create_config_from_template("small_network")
    medium_network = config_manager.create_config_from_template("medium_network")
    
    print(f"Small network: {small_network['brain']['neuron_count']} neurons")
    print(f"Medium network: {medium_network['brain']['neuron_count']} neurons")
    
    # Save configurations
    config_manager.save_config_to_file(small_network, "my_small_network.json")
    
    # Create complete configuration set
    config_manager.create_complete_config_set()
    
    # Example of loading and modifying configuration
    loaded_config = config_manager.load_config_from_file("configs/default_config.json")
    loaded_config['brain']['neuron_count'] = 3000  # Modify loaded config
    
    print(f"Modified configuration neuron count: {loaded_config['brain']['neuron_count']}")
```

---

## Multi-Agent Systems

### Example 11: Multi-Agent Framework

```python
import pynlm
import numpy as np
import time
import threading
import queue

class MultiAgentSystem:
    def __init__(self, num_agents=3, shared_world=None):
        self.num_agents = num_agents
        self.agents = []
        self.shared_world = shared_world
        self.communication_channel = queue.Queue()
        self.performance_metrics = []
        self.running = False
        
    def create_agent(self, agent_id, brain_config_size=800):
        """Create a single agent with unique ID."""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", brain_config_size)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        if self.shared_world:
            agent = pynlm.createAgentBrain(brain)
            self.shared_world.reset()
            agent.initialize(self.shared_world)
            
            # Enable all learning systems
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            agent.enableDevelopment(True)
        
        agent_instance = {
            'id': agent_id,
            'brain': brain,
            'agent': agent,
            'world': self.shared_world,
            'performance': {
                'total_reward': 0.0,
                'steps_completed': 0,
                'unique_actions': set(),
                'learning_progress': []
            },
            'last_percept': None,
            'last_action': None,
            'memory': []
        }
        
        return agent_instance
    
    def initialize_agents(self):
        """Initialize all agents in the system."""
        print(f"Initializing {self.num_agents} agents...")
        
        for i in range(self.num_agents):
            agent = self.create_agent(i)
            self.agents.append(agent)
        
        print("All agents initialized successfully")
        return self.agents
    
    def inter_agent_communication(self, agent_idx):
        """Simulate inter-agent communication and coordination."""
        agent = self.agents[agent_idx]
        
        # Create communication message
        message = {
            'sender_id': agent['id'],
            'timestamp': time.time(),
            'percept': agent['last_percept'],
            'action': agent['last_action'],
            'performance': agent['performance']
        }
        
        # Send message to communication channel
        self.communication_channel.put(message)
        
        # Process incoming messages
        processed_messages = []
        while not self.communication_channel.empty():
            msg = self.communication_channel.get()
            if msg['sender_id'] != agent['id']:  # Don't process own messages
                processed_messages.append(msg)
        
        return processed_messages
    
    def collaborative_learning(self, agent_idx):
        """Implement collaborative learning between agents."""
        agent = self.agents[agent_idx]
        
        # Collect insights from other agents
        insights = []
        for other_agent in self.agents:
            if other_agent['id'] != agent['id']:
                # Share successful strategies
                if other_agent['performance']['total_reward'] > agent['performance']['total_reward']:
                    insights.append({
                        'agent_id': other_agent['id'],
                        'success_rate': other_agent['performance']['total_reward'] / 
                                      max(other_agent['performance']['steps_completed'], 1),
                        'last_action': other_agent['last_action']
                    })
        
        # Apply collaborative learning
        if insights:
            best_insight = max(insights, key=lambda x: x['success_rate'])
            
            # Modify agent's behavior based on successful strategies
            curiosity_boost = min(1.0, 0.1 * len(insights))
            agent['agent'].applyRewardModulation(curiosity_boost * 10.0, 0.0)
            
            # Record learning from collaboration
            agent['performance']['learning_progress'].append({
                'step': agent['performance']['steps_completed'],
                'collaboration_bonus': curiosity_boost,
                'best_peer_id': best_insight['agent_id']
            })
    
    def run_multi_agent_simulation(self, steps=300):
        """Run the complete multi-agent simulation."""
        print(f"\n=== Multi-Agent Simulation ({self.num_agents} agents) ===")
        
        self.running = True
        simulation_start = time.time()
        
        for step in range(steps):
            step_start = time.time()
            
            # Update shared world if available
            if self.shared_world:
                self.shared_world.update(0.1)
            
            # Each agent takes its turn
            for agent_idx in range(self.num_agents):
                agent = self.agents[agent_idx]
                
                try:
                    # Get current percept
                    if self.shared_world:
                        agent['last_percept'] = self.shared_world.getSensoryPercept()
                    
                    # Process sensory input
                    agent['agent'].processSensoryInput(agent['last_percept'])
                    
                    # Brain computation
                    agent['brain'].step(step)
                    
                    # Decode motor command
                    agent['last_action'] = agent['agent'].decodeMotorCommand()
                    
                    # Apply action to world
                    if self.shared_world:
                        self.shared_world.applyMotorCommand(
                            agent['last_action'], 
                            self.shared_world.getSimulationTime()
                        )
                    
                    # Calculate and apply reward
                    reward = self._calculate_collaborative_reward(agent_idx)
                    agent['performance']['total_reward'] += reward
                    agent['agent'].applyRewardModulation(reward, 0.0)
                    
                    # Agent learning
                    agent['agent'].updateDevelopment(0.1)
                    
                    # Inter-agent communication
                    messages = self.inter_agent_communication(agent_idx)
                    
                    # Collaborative learning
                    self.collaborative_learning(agent_idx)
                    
                    # Update performance metrics
                    agent['performance']['steps_completed'] += 1
                    agent['performance']['unique_actions'].add(str(agent['last_action']))
                    
                    # Log progress
                    if step % 50 == 0 and agent_idx == 0:  # Log every agent at start of each cycle
                        print(f"  Step {step}: Agent {agent_idx} completed")
                
                except Exception as e:
                    print(f"  Error in agent {agent_idx} at step {step}: {e}")
                    continue
            
            # Control simulation speed
            step_time = time.time() - step_start
            sleep_time = max(0, 0.1 - step_time)
            time.sleep(sleep_time)
        
        simulation_time = time.time() - simulation_start
        self.running = False
        
        # Collect final results
        results = self._collect_multi_agent_results()
        
        print(f"\n=== Multi-Agent Simulation Results ===")
        print(f"Total simulation time: {simulation_time:.2f} seconds")
        print(f"Steps per second: {steps / simulation_time:.2f}")
        
        return results
    
    def _calculate_collaborative_reward(self, agent_idx):
        """Calculate reward that considers collaborative effects."""
        agent = self.agents[agent_idx]
        
        # Base reward from agent's own performance
        base_reward = 0.1
        
        # Additional reward for collaboration
        collaboration_bonus = 0.0
        for other_agent in self.agents:
            if other_agent['id'] != agent['id']:
                # Reward if helping other agents succeed
                if (other_agent['performance']['total_reward'] > agent['performance']['total_reward'] and
                    other_agent['performance']['steps_completed'] > agent['performance']['steps_completed']):
                    collaboration_bonus += 0.05
        
        # Novelty bonus for exploring new strategies
        novelty_bonus = 0.05 if len(agent['performance']['unique_actions']) > 2 else 0.0
        
        total_reward = base_reward + collaboration_bonus + novelty_bonus
        return total_reward
    
    def _collect_multi_agent_results(self):
        """Collect comprehensive results from all agents."""
        results = {
            'total_agents': self.num_agents,
            'agent_results': [],
            'collective_metrics': {},
            'comparison_table': []
        }
        
        for agent in self.agents:
            agent_result = {
                'agent_id': agent['id'],
                'total_reward': agent['performance']['total_reward'],
                'steps_completed': agent['performance']['steps_completed'],
                'avg_reward_per_step': (
                    agent['performance']['total_reward'] / 
                    max(agent['performance']['steps_completed'], 1)
                ),
                'unique_actions_count': len(agent['performance']['unique_actions']),
                'final_curiosity': agent['agent'].getCuriosityLevel(),
                'final_novelty': agent['agent'].getNoveltyLevel(),
                'developmental_stage': agent['brain'].getDevelopmentalStage(),
                'collaboration_experience': len(agent['performance']['learning_progress'])
            }
            
            results['agent_results'].append(agent_result)
            
            # Add to comparison table
            results['comparison_table'].append({
                'Agent': f"Agent {agent['id']}",
                'Reward': f"{agent_result['total_reward']:.2f}",
                'Avg Reward': f"{agent_result['avg_reward_per_step']:.4f}",
                'Actions': agent_result['unique_actions_count'],
                'Curiosity': f"{agent_result['final_curiosity']:.3f}",
                'Stage': agent_result['developmental_stage']
            })
        
        # Collective metrics
        total_reward = sum(r['total_reward'] for r in results['agent_results'])
        avg_reward = total_reward / self.num_agents
        
        results['collective_metrics'] = {
            'total_collective_reward': total_reward,
            'average_agent_reward': avg_reward,
            'total_unique_actions': sum(
                r['unique_actions_count'] for r in results['agent_results']
            ),
            'average_curiosity': np.mean([
                r['final_curiosity'] for r in results['agent_results']
            ]),
            'stage_distribution': {
                'initial': sum(1 for r in results['agent_results'] 
                             if r['developmental_stage'] == 'Initial'),
                'critical': sum(1 for r in results['agent_results'] 
                              if r['developmental_stage'] == 'CriticalPeriod'),
                'maturing': sum(1 for r in results['agent_results'] 
                             if r['developmental_stage'] == 'Maturation'),
                'adult': sum(1 for r in results['agent_results'] 
                           if r['developmental_stage'] == 'Adult'),
                'aging': sum(1 for r in results['agent_results'] 
                           if r['developmental_stage'] == 'Aging')
            }
        }
        
        return results
    
    def print_multi_agent_summary(self, results):
        """Print a comprehensive summary of multi-agent results."""
        print(f"\n=== Multi-Agent System Summary ===")
        print(f"\nIndividual Agent Performance:")
        print(f"{'Agent':<8} {'Reward':<10} {'Avg Reward':<12} {'Actions':<8} {'Curiosity':<10} {'Stage':<10}")
        print("-" * 70)
        
        for row in results['comparison_table']:
            print(f"{row['Agent']:<8} {row['Reward']:<10} {row['Avg Reward']:<12} "
                  f"{row['Actions']:<8} {row['Curiosity']:<10} {row['Stage']:<10}")
        
        print(f"\nCollective Metrics:")
        for key, value in results['collective_metrics'].items():
            if isinstance(value, dict):
                print(f"  {key}:")
                for sub_key, sub_value in value.items():
                    print(f"    {sub_key}: {sub_value}")
            else:
                print(f"  {key}: {value:.3f}" if isinstance(value, float) else f"  {key}: {value}")

# Example multi-agent system
if __name__ == "__main__":
    print("=== Multi-Agent System Example ===")
    
    # Create multi-agent system with shared world
    shared_world = pynlm.createSimpleWorld()
    shared_world.configure(width=40, height=40, visionWidth=12, visionHeight=12)
    
    mas = MultiAgentSystem(num_agents=3, shared_world=shared_world)
    mas.initialize_agents()
    
    # Run simulation
    results = mas.run_multi_agent_simulation(steps=300)
    
    # Print results
    mas.print_multi_agent_summary(results)
```

---

## Real-Time Control Systems

### Example 12: Real-Time Control with Neural Network

```python
import pynlm
import numpy as np
import time
import threading
from collections import deque

class RealTimeControlSystem:
    def __init__(self, control_frequency=100.0):
        self.control_frequency = control_frequency  # Hz
        self.control_period = 1.0 / control_frequency  # seconds
        self.target_state = {'position': [0.0, 0.0], 'velocity': [0.0, 0.0]}
        self.current_state = {'position': [0.0, 0.0], 'velocity': [0.0, 0.0]}
        self.control_history = deque(maxlen=1000)
        self.error_history = deque(maxlen=500)
        
        # Initialize control system
        self._initialize_control_hardware()
        self._initialize_neural_controller()
        self._initialize_control_loop()
        
    def _initialize_control_hardware(self):
        """Initialize control hardware interface."""
        self.hardware_interface = {
            'actuators': {
                'motor_forward': 0.0,
                'motor_backward': 0.0,
                'motor_turn_left': 0.0,
                'motor_turn_right': 0.0,
                'motor_interact': 0.0,
                'motor_wait': 0.0
            },
            'sensors': {
                'position': [0.0, 0.0],
                'velocity': [0.0, 0.0],
                'orientation': 0.0,
                'proximity': 0.0
            },
            'control_signals': {
                'enabled': False,
                'emergency_stop': False,
                'calibration_mode': False
            }
        }
        
        print("Control hardware initialized")
    
    def _initialize_neural_controller(self):
        """Initialize neural network for control."""
        # Create neural controller
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1500)
        config.set("brain.synapse_density", 0.08)
        config.set("brain.v_thresh", -50.0)
        
        self.controller_brain = pynlm.createBrain(config)
        self.controller_brain.initialize()
        
        # Create agent controller
        self.controller_agent = pynlm.createAgentBrain(self.controller_brain)
        
        # Create environment for controller training
        self.controller_world = pynlm.createSimpleWorld()
        self.controller_world.configure(
            width=20,
            height=20,
            visionWidth=8,
            visionHeight=8
        )
        
        # Initialize controller agent
        self.controller_world.reset()
        self.controller_agent.initialize(self.controller_world)
        
        # Enable learning systems for adaptation
        self.controller_agent.enableRewardModulation(True)
        self.controller_agent.enableCuriosity(True)
        self.controller_agent.enableDevelopment(True)
        
        # Control state
        self.control_state = {
            'last_action': None,
            'current_error': 0.0,
            'integral_error': 0.0,
            'derivative_error': 0.0,
            'control_output': [0.0, 0.0],
            'training_enabled': True,
            'adaptation_rate': 0.01
        }
        
        print("Neural controller initialized")
    
    def _initialize_control_loop(self):
        """Initialize real-time control loop."""
        self.control_loop_running = True
        self.control_thread = None
        self.performance_metrics = {
            'updates_per_second': 0,
            'control_accuracy': 0.0,
            'computation_time': 0.0,
            'stability_margin': 0.0
        }
        
    def start_control_loop(self):
        """Start the real-time control loop in a separate thread."""
        self.control_thread = threading.Thread(target=self._control_loop_thread)
        self.control_thread.daemon = True
        self.control_thread.start()
        print(f"Real-time control loop started at {self.control_frequency} Hz")
    
    def stop_control_loop(self):
        """Stop the control loop."""
        self.control_loop_running = False
        if self.control_thread:
            self.control_thread.join(timeout=5.0)
        print("Control loop stopped")
    
    def _control_loop_thread(self):
        """Real-time control loop thread."""
        last_time = time.time()
        updates = 0
        
        while self.control_loop_running:
            loop_start = time.time()
            
            # Execute control cycle
            control_output = self._execute_control_cycle()
            
            # Update actuators
            self._update_actuators(control_output)
            
            # Collect performance metrics
            updates += 1
            
            # Sleep to maintain control frequency
            loop_time = time.time() - loop_start
            sleep_time = max(0, self.control_period - loop_time)
            time.sleep(sleep_time)
        
        # Calculate performance metrics
        self.performance_metrics['updates_per_second'] = updates
        print(f"Control loop completed: {updates} updates")
    
    def _execute_control_cycle(self):
        """Execute a single control cycle."""
        cycle_start = time.time()
        
        # Get current state from sensors
        current_state = self._read_sensors()
        
        # Calculate control error
        error = self._calculate_control_error(current_state)
        
        # Compute control action using neural network
        control_action = self._compute_neural_control(error)
        
        # Update control state
        self._update_control_state(error)
        
        # Record performance
        computation_time = time.time() - cycle_start
        self.performance_metrics['computation_time'] = computation_time
        
        return control_action
    
    def _read_sensors(self):
        """Read current state from control sensors."""
        # Simulate sensor readings
        state = {
            'position': np.array([
                self.hardware_interface['sensors']['position'][0],
                self.hardware_interface['sensors']['position'][1]
            ]),
            'velocity': np.array([
                self.hardware_interface['sensors']['velocity'][0],
                self.hardware_interface['sensors']['velocity'][1]
            ]),
            'orientation': self.hardware_interface['sensors']['orientation'],
            'proximity': self.hardware_interface['sensors']['proximity']
        }
        
        # Update current state
        self.current_state = state.copy()
        
        return state
    
    def _calculate_control_error(self, current_state):
        """Calculate control error based on current state."""
        target_pos = np.array(self.target_state['position'])
        current_pos = current_state['position']
        
        # Position error
        position_error = target_pos - current_pos
        position_error_magnitude = np.linalg.norm(position_error)
        
        # Velocity error
        target_vel = np.array(self.target_state['velocity'])
        current_vel = current_state['velocity']
        velocity_error = target_vel - current_vel
        velocity_error_magnitude = np.linalg.norm(velocity_error)
        
        # Combined error
        total_error = np.sqrt(
            position_error_magnitude**2 + velocity_error_magnitude**2
        )
        
        # Store error
        self.control_state['current_error'] = total_error
        
        # Update error history
        self.error_history.append({
            'timestamp': time.time(),
            'error': total_error,
            'position_error': position_error_magnitude,
            'velocity_error': velocity_error_magnitude
        })
        
        return total_error, position_error, velocity_error
    
    def _compute_neural_control(self, errors):
        """Compute control action using neural network."""
        total_error, position_error, velocity_error = errors
        
        # Prepare sensory input for neural controller
        sensory_input = self._prepare_sensory_input(position_error, velocity_error)
        
        # Create percept for controller
        percept = self._create_control_percept(sensory_input)
        
        # Process sensory input in neural controller
        self.controller_agent.processSensoryInput(percept)
        
        # Get brain action
        self.controller_brain.step(time.time())
        
        # Decode control action
        control_action = self.controller_agent.decodeMotorCommand()
        
        # Apply control action to hardware
        self._apply_neural_control(control_action)
        
        return control_action
    
    def _prepare_sensory_input(self, position_error, velocity_error):
        """Prepare sensory input for neural controller."""
        # Normalize errors
        position_error_norm = position_error / 20.0  # Assume max position error of 20 units
        velocity_error_norm = velocity_error / 10.0   # Assume max velocity error of 10 units
        
        # Create sensory representation
        sensory_input = {
            'position_error_x': position_error[0],
            'position_error_y': position_error[1],
            'velocity_error_x': velocity_error[0],
            'velocity_error_y': velocity_error[1],
            'error_magnitude': np.linalg.norm([position_error[0], position_error[1], velocity_error[0], velocity_error[1]]),
            'error_direction': np.arctan2(position_error[1], position_error[0]) if np.linalg.norm(position_error) > 0 else 0.0,
            'timestamp': time.time()
        }
        
        return sensory_input
    
    def _create_control_percept(self, sensory_input):
        """Create a percept object for the control system."""
        # For simplicity, use internal signals to represent control input
        # In a real system, this would use the world percept interface
        
        # Create custom percept-like object
        class ControlPercept:
            def __init__(self, sensory_data):
                self.sensory_data = sensory_data
            
            def getVision(self):
                # Convert position/velocity errors to vision-like data
                vision_size = 16
                vision = np.zeros(vision_size * vision_size)
                
                # Encode position error in first quadrant
                if self.sensory_data['position_error_x'] > 0:
                    x_idx = int((self.sensory_data['position_error_x'] / 20.0) * (vision_size/2))
                else:
                    x_idx = vision_size//2 + int((abs(self.sensory_data['position_error_x']) / 20.0) * (vision_size/2))
                
                if self.sensory_data['position_error_y'] > 0:
                    y_idx = int((self.sensory_data['position_error_y'] / 20.0) * (vision_size/2))
                else:
                    y_idx = vision_size//2 + int((abs(self.sensory_data['position_error_y']) / 20.0) * (vision_size/2))
                
                if 0 <= x_idx < vision_size and 0 <= y_idx < vision_size:
                    vision[y_idx * vision_size + x_idx] = min(1.0, self.sensory_data['error_magnitude'] / 5.0)
                
                return vision.tolist()
            
            def getInternal(self):
                # Return internal signals with error information
                return [
                    self.sensory_data['error_magnitude'],
                    self.sensory_data['error_direction'],
                    self.sensory_data['position_error_x'],
                    self.sensory_data['position_error_y']
                ]
        
        return ControlPercept(sensory_input)
    
    def _update_control_state(self, errors):
        """Update control state with error information."""
        total_error, position_error, velocity_error = errors
        
        # Calculate PID control terms
        dt = self.control_period
        self.control_state['integral_error'] += total_error * dt
        self.control_state['derivative_error'] = (
            total_error - getattr(self.control_state, '_last_error', 0.0)
        ) / dt if hasattr(self.control_state, '_last_error') else 0.0
        self.control_state._last_error = total_error
        
        # Store current error
        self.control_state['current_error'] = total_error
    
    def _apply_neural_control(self, neural_action):
        """Apply neural network control action to hardware."""
        # Convert neural action to control signals
        control_output = self._decode_neural_action(neural_action)
        
        # Apply to hardware interface
        self.hardware_interface['actuators'] = control_output
        self.control_state['control_output'] = control_output
        
        # Update world state
        self._update_world_from_control(control_output)
    
    def _decode_neural_action(self, neural_action):
        """Decode neural action into control signals."""
        # This would depend on the specific neural action encoding
        # For demonstration, create a reasonable control mapping
        
        control_signals = {
            'motor_forward': 0.0,
            'motor_backward': 0.0,
            'motor_turn_left': 0.0,
            'motor_turn_right': 0.0,
            'motor_interact': 0.0,
            'motor_wait': 0.0
        }
        
        # Convert action to control signals based on action encoding
        # This is a simplified example
        if neural_action and hasattr(neural_action, '__getitem__'):
            if len(neural_action) > 0:
                control_signals['motor_forward'] = max(0, min(1, neural_action[0]))
            if len(neural_action) > 1:
                control_signals['motor_backward'] = max(0, min(1, neural_action[1]))
            if len(neural_action) > 2:
                control_signals['motor_turn_left'] = max(0, min(1, neural_action[2]))
            if len(neural_action) > 3:
                control_signals['motor_turn_right'] = max(0, min(1, neural_action[3]))
        
        return control_signals
    
    def _update_world_from_control(self, control_signals):
        """Update world state based on control signals.
        # Simulate the effect of control signals on the physical system
        position = self.current_state['position']
        velocity = self.current_state['velocity']
        
        # Apply control forces
        dt = self.control_period
        
        # Forward/backward motion
        forward_force = control_signals['motor_forward'] - control_signals['motor_backward']
        velocity[0] += forward_force * dt * 0.1
        
        # Turning motion
        turn_force = control_signals['motor_turn_right'] - control_signals['motor_turn_left']
        velocity[1] += turn_force * dt * 0.1
        
        # Update position
        position[0] += velocity[0] * dt
        position[1] += velocity[1] * dt
        
        # Apply boundaries
        position[0] = max(-10, min(10, position[0]))
        position[1] = max(-10, min(10, position[1]))
        
        # Update hardware sensors
        self.hardware_interface['sensors']['position'] = position.tolist()
        self.hardware_interface['sensors']['velocity'] = velocity.tolist()
        
        # Update control history
        self.control_history.append({
            'timestamp': time.time(),
            'position': position.copy(),
            'velocity': velocity.copy(),
            'control_signals': control_signals.copy(),
            'error': self.control_state['current_error']
        })
    
    def set_target_state(self, position, velocity):
        """Set target state for control system."""
        self.target_state['position'] = position
        self.target_state['velocity'] = velocity
        print(f"Target state set: position {position}, velocity {velocity}")
    
    def get_control_performance(self):
        """Get current control performance metrics."""
        if not self.error_history:
            return self.performance_metrics
        
        # Calculate control accuracy
        errors = [entry['error'] for entry in self.error_history]
        self.performance_metrics['control_accuracy'] = 1.0 / (1.0 + np.mean(errors))
        
        # Calculate stability margin
        if len(self.error_history) >= 10:
            recent_errors = [entry['error'] for entry in list(self.error_history)[-10:]]
            self.performance_metrics['stability_margin'] = max(0, 1.0 - np.std(recent_errors) / np.mean(recent_errors))
        
        return self.performance_metrics
    
    def plot_control_performance(self):
        """Plot control system performance."""
        import matplotlib.pyplot as plt
        
        if not self.control_history:
            print("No control history to plot")
            return
        
        fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(12, 10))
        fig.suptitle("Real-Time Control System Performance", fontsize=16)
        
        timestamps = [entry['timestamp'] for entry in self.control_history]
        
        # Position over time
        positions = [entry['position'] for entry in self.control_history]
        position_array = np.array(positions)
        
        ax1.plot(timestamps, position_array[:, 0], label='Position X')
        ax1.plot(timestamps, position_array[:, 1], label='Position Y')
        ax1.plot(timestamps, [self.target_state['position'][0]] * len(timestamps), 
                'r--', alpha=0.5, label='Target X')
        ax1.plot(timestamps, [self.target_state['position'][1]] * len(timestamps), 
                'g--', alpha=0.5, label='Target Y')
        ax1.set_xlabel('Time (s)')
        ax1.set_ylabel('Position')
        ax1.legend()
        ax1.grid(True, alpha=0.3)
        
        # Control error over time
        errors = [entry['error'] for entry in self.error_history]
        error_timestamps = [entry['timestamp'] for entry in self.error_history]
        ax2.plot(error_timestamps, errors, 'r-', alpha=0.7)
        ax2.set_xlabel('Time (s)')
        ax2.set_ylabel('Control Error')
        ax2.set_yscale('log')
        ax2.grid(True, alpha=0.3)
        
        # Control signals over time
        control_signals_history = [entry['control_signals'] for entry in self.control_history]
        if control_signals_history:
            signals_array = np.array(control_signals_history)
            ax3.plot(timestamps, signals_array[:, 0], label='Forward', alpha=0.7)
            ax3.plot(timestamps, signals_array[:, 1], label='Backward', alpha=0.7)
            ax3.plot(timestamps, signals_array[:, 2], label='Turn Left', alpha=0.7)
            ax3.plot(timestamps, signals_array[:, 3], label='Turn Right', alpha=0.7)
            ax3.set_xlabel('Time (s)')
            ax3.set_ylabel('Control Signal')
            ax3.legend()
            ax3.grid(True, alpha=0.3)
        
        # Performance metrics
        metrics = list(self.performance_metrics.keys())
        values = [self.performance_metrics[m] for m in metrics]
        
        ax4.bar(metrics, values, color=['skyblue', 'lightgreen', 'salmon', 'gold'])
        ax4.set_ylabel('Value')
        ax4.set_title('Performance Metrics')
        ax4.grid(True, alpha=0.3)
        
        plt.tight_layout()
        plt.savefig('control_performance.png', dpi=150, bbox_inches='tight')
        plt.show()
        
        print("Control performance plot saved as 'control_performance.png'")

# Example real-time control system
if __name__ == "__main__":
    print("=== Real-Time Control System Example ===")
    
    # Create control system
    control_system = RealTimeControlSystem(control_frequency=50.0)  # 50 Hz
    
    # Set target state
    control_system.set_target_state([5.0, 5.0], [0.0, 0.0])
    
    # Start control loop
    control_system.start_control_loop()
    
    try:
        # Run for 10 seconds
        print("Running control system for 10 seconds...")
        time.sleep(10)
        
        # Stop control loop
        control_system.stop_control_loop()
        
        # Get and display performance metrics
        performance = control_system.get_control_performance()
        print(f"\nFinal Performance Metrics:")
        for key, value in performance.items():
            print(f"  {key}: {value:.4f}")
        
        # Plot performance
        control_system.plot_control_performance()
        
    except KeyboardInterrupt:
        print("\nControl system interrupted by user")
        control_system.stop_control_loop()
```

---

## Experimental Protocol Examples

### Example 13: Scientific Experiment Framework

```python
import pynlm
import numpy as np
import time
import json
from datetime import datetime
from typing import List, Dict, Any

class NLMExperiment:
    def __init__(self, experiment_name: str, description: str):
        self.experiment_name = experiment_name
        self.description = description
        self.results = []
        self.protocol = {}
        self.metadata = {
            'start_time': None,
            'end_time': None,
            'duration': 0.0,
            ' experimenter': 'NLM Research Team',
            'version': '1.0'
        }
        
    def add_protocol_step(self, step_name: str, parameters: Dict[str, Any], 
                         expected_outcomes: Dict[str, Any]):
        """Add a protocol step to the experiment."""
        if 'steps' not in self.protocol:
            self.protocol['steps'] = []
        
        self.protocol['steps'].append({
            'step_name': step_name,
            'parameters': parameters,
            'expected_outcomes': expected_outcomes,
            'actual_outcomes': {},
            'status': 'pending'
        })
    
    def run_step(self, step_index: int, **kwargs) -> Dict[str, Any]:
        """Run a specific protocol step."""
        if 'steps' not in self.protocol or step_index >= len(self.protocol['steps']):
            raise ValueError(f"Invalid step index: {step_index}")
        
        step = self.protocol['steps'][step_index]
        step['status'] = 'running'
        
        try:
            # Run the experiment step
            result = self._execute_protocol_step(step, **kwargs)
            
            # Store actual outcomes
            step['actual_outcomes'] = result
            step['status'] = 'completed'
            
            self.results.append({
                'step_name': step['step_name'],
                'step_index': step_index,
                'result': result,
                'timestamp': datetime.now().isoformat()
            })
            
            return result
            
        except Exception as e:
            step['status'] = 'failed'
            error_result = {
                'error': str(e),
                'error_type': type(e).__name__
            }
            
            self.results.append({
                'step_name': step['step_name'],
                'step_index': step_index,
                'result': error_result,
                'timestamp': datetime.now().isoformat(),
                'failed': True
            })
            
            raise
    
    def _execute_protocol_step(self, step: Dict[str, Any], **kwargs) -> Dict[str, Any]:
        """Execute a single protocol step."""
        step_name = step['step_name']
        parameters = step['parameters']
        
        # Common experiment patterns
        if step_name == 'basic_neural_dynamics':
            return self._run_basic_dynamics_experiment(parameters)
        elif step_name == 'reward_learning':
            return self._run_reward_learning_experiment(parameters)
        elif step_name == 'developmental_process':
            return self._run_developmental_experiment(parameters)
        elif step_name == 'multi_agent_interaction':
            return self._run_multi_agent_experiment(parameters)
        elif step_name == 'plasticity_study':
            return self._run_plasticity_experiment(parameters)
        else:
            raise ValueError(f"Unknown protocol step: {step_name}")
    
    def _run_basic_dynamics_experiment(self, params: Dict[str, Any]) -> Dict[str, Any]:
        """Run basic neural dynamics experiment."""
        config = pynlm.createDefaultConfig()
        
        # Apply parameters
        for key, value in params.items():
            if isinstance(value, dict):
                for nested_key, nested_value in value.items():
                    config.set(f"{key}.{nested_key}", nested_value)
            else:
                config.set(key, value)
        
        # Create and run brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        experiment_results = {
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'spike_traces': [],
            'firing_rates': [],
            'dynamics_metrics': {}
        }
        
        # Run simulation
        for step in range(params.get('simulation_steps', 100)):
            brain.step(step)
            
            experiment_results['spike_traces'].append(brain.getTotalSpikeCount())
            experiment_results['firing_rates'].append(brain.getAverageFiringRate())
        
        # Analyze dynamics
        experiment_results['dynamics_metrics'] = {
            'mean_firing_rate': np.mean(experiment_results['firing_rates']),
            'firing_rate_variance': np.var(experiment_results['firing_rates']),
            'total_spikes': sum(experiment_results['spike_traces']),
            'final_developmental_stage': brain.getDevelopmentalStage()
        }
        
        return experiment_results
    
    def _run_reward_learning_experiment(self, params: Dict[str, Any]) -> Dict[str, Any]:
        """Run reward learning experiment."""
        # Create agent and world
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", params.get('neuron_count', 500))
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        
        world = pynlm.createSimpleWorld()
        world.configure(**params.get('world_config', {}))
        world.reset()
        
        agent.initialize(world)
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run reward learning protocol
        episode_rewards = []
        curiosity_levels = []
        novelty_levels = []
        
        for episode in range(params.get('episodes', 50)):
            episode_reward = 0.0
            
            for step in range(params.get('steps_per_episode', 100)):
                # Environment interaction
                world.update(0.1)
                
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Calculate reward
                agent_pos = world.getAgentBody()
                target_pos = np.array(params.get('target_position', [0, 0]))
                distance = np.linalg.norm(agent_pos - target_pos)
                reward = max(0, 1.0 - distance / params.get('max_distance', 10.0))
                
                agent.applyRewardModulation(reward, 0.0)
                episode_reward += reward
            
            episode_rewards.append(episode_reward)
            curiosity_levels.append(agent.getCuriosityLevel())
            novelty_levels.append(agent.getNoveltyLevel())
        
        return {
            'episode_rewards': episode_rewards,
            'final_curiosity': curiosity_levels[-1],
            'final_novelty': novelty_levels[-1],
            'learning_curve': np.convolve(episode_rewards, 
                                        np.ones(min(10, len(episode_rewards)))/min(10, len(episode_rewards)), 
                                        mode='valid').tolist(),
            'performance_metrics': {
                'mean_reward': np.mean(episode_rewards),
                'reward_variance': np.var(episode_rewards),
                'learning_rate': self._calculate_learning_rate(episode_rewards),
                'convergence_episode': self._find_convergence_episode(episode_rewards)
            }
        }
    
    def _run_developmental_experiment(self, params: Dict[str, Any]) -> Dict[str, Any]:
        """Run developmental process experiment."""
        # Run developmental protocol
        developmental_stages = []
        plasticity_metrics = []
        
        for stage in range(params.get('developmental_stages', 5)):
            # Create brain for each developmental stage
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", params.get('neuron_count', 1000))
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Simulate developmental progression
            stage_results = []
            for step in range(params.get('stage_duration', 100)):
                brain.step(step)
                stage_results.append({
                    'firing_rate': brain.getAverageFiringRate(),
                    'plasticity_rate': np.random.random() * 0.1,  # Simulated plasticity
                    'developmental_progress': stage / params.get('developmental_stages', 5)
                })
            
            developmental_stages.append({
                'stage': stage,
                'results': stage_results,
                'final_firing_rate': brain.getAverageFiringRate(),
                'final_developmental_stage': brain.getDevelopmentalStage()
            })
            
            plasticity_metrics.append(np.mean([r['plasticity_rate'] for r in stage_results]))
        
        return {
            'developmental_stages': developmental_stages,
            'plasticity_trajectory': plasticity_metrics,
            'stage_transitions': self._analyze_stage_transitions(developmental_stages),
            'developmental_metrics': {
                'total_development_time': params.get('developmental_stages', 5) * params.get('stage_duration', 100),
                'average_final_firing_rate': np.mean([s['final_firing_rate'] for s in developmental_stages]),
                'stability_measure': self._calculate_stability(developmental_stages)
            }
        }
    
    def _run_multi_agent_experiment(self, params: Dict[str, Any]) -> Dict[str, Any]:
        """Run multi-agent interaction experiment."""
        num_agents = params.get('num_agents', 3)
        agents = []
        shared_world = None
        
        if params.get('shared_environment', True):
            shared_world = pynlm.createSimpleWorld()
            shared_world.configure(**params.get('world_config', {}))
        
        # Create agents
        for i in range(num_agents):
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", params.get('neuron_count', 500))
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            agent = pynlm.createAgentBrain(brain)
            
            if shared_world:
                shared_world.reset()
                agent.initialize(shared_world)
            
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            
            agents.append({
                'id': i,
                'brain': brain,
                'agent': agent,
                'world': shared_world,
                'performance': []
            })
        
        # Run multi-agent simulation
        for episode in range(params.get('episodes', 50)):
            for agent_data in agents:
                agent = agent_data['agent']
                brain = agent_data['brain']
                
                episode_reward = 0.0
                
                for step in range(params.get('steps_per_episode', 100)):
                    if shared_world:
                        shared_world.update(0.1)
                    
                    percept = shared_world.getSensoryPercept() if shared_world else None
                    if percept:
                        agent.processSensoryInput(percept)
                    
                    brain.step(step)
                    action = agent.decodeMotorCommand()
                    
                    if shared_world:
                        shared_world.applyMotorCommand(action, shared_world.getSimulationTime())
                    
                    # Calculate reward with collaboration effects
                    reward = 0.1  # Base reward
                    agent_pos = shared_world.getAgentBody() if shared_world else None
                    if agent_pos is not None:
                        target_pos = np.array(params.get('target_position', [0, 0]))
                        distance = np.linalg.norm(agent_pos - target_pos)
                        reward += max(0, 1.0 - distance / params.get('max_distance', 10.0))
                    
                    agent.applyRewardModulation(reward, 0.0)
                    episode_reward += reward
                
                agent_data['performance'].append(episode_reward)
        
        # Analyze multi-agent results
        all_rewards = [agent['performance'] for agent in agents]
        
        return {
            'agents': [{
                'agent_id': agent['id'],
                'episodes': agent['performance'],
                'mean_reward': np.mean(agent['performance']),
                'reward_variance': np.var(agent['performance']),
                'final_curiosity': agent['agent'].getCuriosityLevel()
            } for agent in agents],
            'collective_metrics': {
                'total_reward': sum(np.sum(agent['performance']) for agent in agents),
                'average_agent_reward': np.mean([np.mean(agent['performance']) for agent in agents]),
                'collaboration_effect': self._calculate_collaboration_effect(all_rewards),
                'competitive_dynamics': self._analyze_competitive_dynamics(all_rewards)
            }
        }
    
    def _run_plasticity_experiment(self, params: Dict[str, Any]) -> Dict[str, Any]:
        """Run plasticity study experiment."""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", params.get('neuron_count', 1000))
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.structural.enable", True)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Enable all plasticity mechanisms
        agent = pynlm.createAgentBrain(brain)
        agent.enableRewardModulation(True)
        
        plasticity_data = {
            'stdp_changes': [],
            'hebbian_changes': [],
            'structural_changes': [],
            'synapse_growth': [],
            'synapse_pruning': [],
            'learning_curves': []
        }
        
        for step in range(params.get('plasticity_steps', 500)):
            # Record current state
            plasticity_data['stdp_changes'].append({
                'time': step,
                'total_spikes': brain.getTotalSpikeCount(),
                'firing_rate': brain.getAverageFiringRate()
            })
            
            # Run simulation step
            brain.step(step)
            
            # Collect plasticity metrics
            plasticity_data['learning_curves'].append(brain.getAverageFiringRate())
        
        return plasticity_data
    
    def _calculate_learning_rate(self, rewards: List[float]) -> float:
        """Calculate learning rate from reward sequence."""
        if len(rewards) < 10:
            return 0.0
        
        # Calculate slope of last 20% of rewards
        start_idx = int(0.8 * len(rewards))
        early_rewards = rewards[:start_idx]
        late_rewards = rewards[start_idx:]
        
        if np.mean(early_rewards) == 0:
            return 0.0
        
        learning_rate = (np.mean(late_rewards) - np.mean(early_rewards)) / len(late_rewards)
        return max(0, learning_rate)
    
    def _find_convergence_episode(self, rewards: List[float], threshold: float = 0.05) -> int:
        """Find episode where learning converged."""
        if len(rewards) < 20:
            return len(rewards) - 1
        
        for i in range(10, len(rewards)):
            recent_mean = np.mean(rewards[max(0, i-10):i])
            global_mean = np.mean(rewards)
            
            if abs(recent_mean - global_mean) / global_mean < threshold:
                return i
        
        return len(rewards) - 1
    
    def _analyze_stage_transitions(self, stages: List[Dict]) -> Dict[str, Any]:
        """Analyze developmental stage transitions."""
        transitions = []
        
        for i in range(1, len(stages)):
            prev_stage = stages[i-1]['final_developmental_stage']
            curr_stage = stages[i]['final_developmental_stage']
            
            if prev_stage != curr_stage:
                transitions.append({
                    'from_stage': prev_stage,
                    'to_stage': curr_stage,
                    'transition_step': i
                })
        
        return {
            'transitions': transitions,
            'num_transitions': len(transitions),
            'stage_durations': [s['results'][-1]['developmental_progress'] for s in stages]
        }
    
    def _calculate_stability(self, stages: List[Dict]) -> float:
        """Calculate stability measure from developmental stages."""
        firing_rates = [s['final_firing_rate'] for s in stages]
        
        if len(firing_rates) < 2:
            return 0.0
        
        # Calculate coefficient of variation
        cv = np.std(firing_rates) / np.mean(firing_rates)
        return max(0, 1.0 - cv)
    
    def _calculate_collaboration_effect(self, agent_rewards: List[List[float]]) -> float:
        """Calculate collaboration effects in multi-agent system."""
        if not agent_rewards:
            return 0.0
        
        # Calculate average reward per agent
        avg_rewards = [np.mean(rewards) for rewards in agent_rewards]
        
        # Calculate collaboration benefit (sum > individual sum)
        total_reward = sum(sum(rewards) for rewards in agent_rewards)
        individual_sum = sum(avg_rewards) * len(agent_rewards)
        
        collaboration_benefit = (total_reward - individual_sum) / max(individual_sum, 1.0)
        return max(0, collaboration_benefit)
    
    def _analyze_competitive_dynamics(self, agent_rewards: List[List[float]]) -> Dict[str, Any]:
        """Analyze competitive dynamics between agents."""
        if not agent_rewards:
            return {}
        
        performance_trajectories = np.array(agent_rewards)
        
        # Calculate rank correlations over time
        num_time_steps = performance_trajectories.shape[1]
        rank_correlations = []
        
        for t in range(num_time_steps):
            ranks = np.argsort(-performance_trajectories[:, t])  # Negative for descending
            rank_correlations.append(np.corrcoef(ranks, range(len(ranks)))[0, 1])
        
        return {
            'mean_rank_correlation': np.mean(rank_correlations),
            'rank_correlation_variance': np.var(rank_correlations),
            'stability_measure': 1.0 - np.std(rank_correlations)
        }
    
    def run_experiment(self, auto_save: bool = True) -> Dict[str, Any]:
        """Run the complete experiment."""
        self.metadata['start_time'] = datetime.now().isoformat()
        
        print(f"Starting experiment: {self.experiment_name}")
        print(f"Description: {self.description}")
        
        try:
            # Run all protocol steps
            for step_index, step in enumerate(self.protocol['steps']):
                print(f"\nRunning step {step_index + 1}: {step['step_name']}")
                
                if step['status'] in ['completed', 'failed']:
                    print(f"  Skipping (already {step['status']})")
                    continue
                
                try:
                    result = self.run_step(step_index, **step['parameters'])
                    print(f"  Completed successfully")
                except Exception as e:
                    print(f"  Failed: {str(e)}")
                    continue
            
            # Calculate experiment duration
            self.metadata['end_time'] = datetime.now().isoformat()
            start_time = datetime.fromisoformat(self.metadata['start_time'])
            end_time = datetime.fromisoformat(self.metadata['end_time'])
            self.metadata['duration'] = (end_time - start_time).total_seconds()
            
            # Save experiment if requested
            if auto_save:
                self.save_experiment()
            
            print(f"\nExperiment completed in {self.metadata['duration']:.2f} seconds")
            print(f"Total steps executed: {len([r for r in self.results if 'failed' not in r])}")
            print(f"Steps failed: {len([r for r in self.results if 'failed' in r])}")
            
            return self.get_experiment_summary()
            
        except Exception as e:
            print(f"Experiment failed: {str(e)}")
            self.metadata['failed'] = True
            self.metadata['error'] = str(e)
            
            if auto_save:
                self.save_experiment()
            
            raise
    
    def save_experiment(self, filename: str = None) -> str:
        """Save experiment results to file."""
        if filename is None:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            filename = f"nlm_experiment_{self.experiment_name}_{timestamp}.json"
        
        experiment_data = {
            'experiment_name': self.experiment_name,
            'description': self.description,
            'metadata': self.metadata,
            'protocol': self.protocol,
            'results': self.results,
            'summary': self.get_experiment_summary()
        }
        
        with open(filename, 'w') as f:
            json.dump(experiment_data, f, indent=2, default=str)
        
        print(f"Experiment saved to {filename}")
        return filename
    
    def get_experiment_summary(self) -> Dict[str, Any]:
        """Get experiment summary statistics."""
        completed_steps = [r for r in self.results if 'failed' not in r]
        
        summary = {
            'experiment_name': self.experiment_name,
            'total_steps': len(self.protocol['steps']),
            'completed_steps': len(completed_steps),
            'failed_steps': len(self.results) - len(completed_steps),
            'completion_rate': len(completed_steps) / len(self.protocol['steps']) * 100,
            'total_execution_time': self.metadata['duration'],
            'result_types': {},
            'key_metrics': {}
        }
        
        # Analyze result types
        for result in completed_steps:
            result_type = type(result['result']).__name__
            if result_type not in summary['result_types']:
                summary['result_types'][result_type] = 0
            summary['result_types'][result_type] += 1
        
        return summary

# Example scientific experiment
if __name__ == "__main__":
    print("=== Scientific Experiment Framework Example ===")
    
    # Create and run a scientific experiment
    experiment = NLMExperiment(
        "Neural Development and Learning",
        "Comprehensive study of neural development, plasticity, and learning in artificial brains"
    )
    
    # Add experimental protocol steps
    experiment.add_protocol_step(
        "basic_neural_dynamics",
        {
            "brain.neuron_count": 1000,
            "brain.synapse_density": 0.1,
            "simulation_steps": 200
        },
        {
            "expected_spike_count": "> 1000",
            "expected_stable_firing": True
        }
    )
    
    experiment.add_protocol_step(
        "reward_learning",
        {
            "neuron_count": 500,
            "world_config": {"width": 20, "height": 20, "visionWidth": 8, "visionHeight": 8},
            "episodes": 30,
            "steps_per_episode": 100,
            "target_position": [10, 10],
            "max_distance": 15
        },
        {
            "expected_improved_performance": True,
            "expected_learning_curve": "increasing"
        }
    )
    
    experiment.add_protocol_step(
        "developmental_process",
        {
            "neuron_count": 800,
            "developmental_stages": 5,
            "stage_duration": 100
        },
        {
            "expected_developmental_stages": 5,
            "expected_stability_increase": True
        }
    )
    
    experiment.add_protocol_step(
        "multi_agent_interaction",
        {
            "num_agents": 3,
            "neuron_count": 400,
            "episodes": 20,
            "steps_per_episode": 80,
            "shared_environment": True,
            "world_config": {"width": 25, "height": 25, "visionWidth": 10, "visionHeight": 10},
            "target_position": [12.5, 12.5],
            "max_distance": 20
        },
        {
            "expected_collaboration_benefit": "> 0.1",
            "expected_agent_diversity": True
        }
    )
    
    experiment.add_protocol_step(
        "plasticity_study",
        {
            "neuron_count": 1200,
            "plasticity_steps": 300,
            "plasticity": {
                "stdp.enable": True,
                "hebbian.enable": True,
                "structural.enable": True
            }
        },
        {
            "expected_plasticity_effects": True,
            "expected_synaptic_adaptation": True
        }
    )
    
    # Run the experiment
    try:
        results = experiment.run_experiment(auto_save=True)
        
        print(f"\n=== Experiment Results Summary ===")
        print(f"Experiment: {results['experiment_name']}")
        print(f"Completion Rate: {results['completion_rate']:.1f}%")
        print(f"Total Execution Time: {results['total_execution_time']:.2f} seconds")
        print(f"Result Types: {results['result_types']}")
        
    except Exception as e:
        print(f"Experiment failed: {str(e)}")