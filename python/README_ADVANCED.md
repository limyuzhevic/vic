# NLM Python Bindings - Advanced Examples

## Advanced Learning Examples

This document contains advanced examples for the NLM Python bindings, focusing on complex learning scenarios, research applications, and performance optimization.

## Table of Contents

1. [Multi-Agent Cooperation](#multi-agent-cooperation)
2. [Complex Learning Environments](#complex-learning-environments)
3. [Performance Optimization Examples](#performance-optimization-examples)
4. [Research Methodology Examples](#research-methodology-examples)
5. [Advanced Debugging and Validation](#advanced-debugging-and-validation)
6. [Custom Extension Examples](#custom-extension-examples)
7. [Production Deployment Examples](#production-deployment-examples)

## Multi-Agent Cooperation

Example showing multiple agents learning to cooperate in a shared environment.

```python
import pynlm
import numpy as np
from multiprocessing import Pool

class MultiAgentSystem:
    def __init__(self, num_agents=3):
        self.num_agents = num_agents
        self.agents = []
        self.shared_world = None
        self.coordination_mechanisms = []
    
    def create_cooperative_agents(self):
        """Create agents with specialized roles"""
        # Agent 1: Explorer
        explorer_config = pynlm.createDefaultConfig()
        explorer_config.set("neuron_count", 800)
        explorer_config.set("enable_curiosity", True)
        explorer = pynlm.createAgentBrain(pynlm.createBrain(explorer_config))
        
        # Agent 2: Collector
        collector_config = pynlm.createDefaultConfig()
        collector_config.set("neuron_count", 600)
        collector_config.set("enable_reward_modulation", True)
        collector = pynlm.createAgentBrain(pynlm.createBrain(collector_config))
        
        # Agent 3: Defender
        defender_config = pynlm.createDefaultConfig()
        defender_config.set("neuron_count", 500)
        defender_config.set("enable_structural_plasticity", True)
        defender = pynlm.createAgentBrain(pynlm.createBrain(defender_config))
        
        self.agents = [explorer, collector, defender]
        
        # Create shared world
        self.shared_world = pynlm.createSimpleWorld()
        self.shared_world.configure(width=100, height=100, visionWidth=16, visionHeight=16)
        
        # Initialize all agents
        for agent in self.agents:
            agent.initialize(self.shared_world)
        
        # Enable coordination mechanisms
        for i, agent in enumerate(self.agents):
            if i == 0:  # Explorer coordinates with others
                agent.enableCuriosity(True)
                agent.setCommunicationRange(30.0)
            elif i == 1:  # Collector optimizes resource gathering
                agent.enableRewardModulation(True)
                agent.setCollectionEfficiency(1.2)
            else:  # Defender protects the group
                agent.enableDevelopment(True)
                agent.setDefenseStrength(2.0)
    
    def run_cooperative_simulation(self, num_steps=1000):
        """Run simulation with agent cooperation"""
        print(f"Starting multi-agent cooperative simulation with {self.num_agents} agents")
        
        for step in range(num_steps):
            # Each agent acts based on its role
            actions = []
            rewards = []
            
            for i, agent in enumerate(self.agents):
                # Agent-specific behavior
                if i == 0:  # Explorer: search for new resources
                    action = self.explore_action(agent, step)
                elif i == 1:  # Collector: gather resources
                    action = self.collect_action(agent, step)
                else:  # Defender: protect agent 0 and 1
                    action = self.defend_action(agent, step)
                
                actions.append(action)
                
                # Apply actions in shared world
                result = self.shared_world.applyMotorCommand(action, self.shared_world.getSimulationTime())
                rewards.append(result.reward)
                
                # Process sensory input
                agent.processSensoryInput(self.shared_world.getSensoryPercept())
                agent.applyRewardModulation(reward, 0.0)  # Simplified reward
                
                # World update
                self.shared_world.update(0.1)
            
            # Calculate group performance
            group_reward = sum(rewards)
            exploration_bonus = self.calculate_exploration_bonus()
            
            if step % 100 == 0:
                print(f"Step {step}: Group reward = {group_reward:.2f}, "
                      f"Exploration bonus = {exploration_bonus:.2f}")
        
        return self.collect_group_metrics()
    
    def explore_action(self, agent, step):
        """Explorer's action: search for new resources"""
        # Use curiosity to explore new areas
        if agent.getCuriosityLevel() > 0.7:
            return self.random_exploration_action()
        else:
            return self.targeted_search_action(agent, step)
    
    def collect_action(self, agent, step):
        """Collector's action: optimize resource collection"""
        # Learn optimal collection paths
        if agent.getNeuromodulationLevel() > 0.5:
            return self.optimized_collection_action(agent, step)
        else:
            return self.basic_collection_action(agent, step)
    
    def defend_action(self, agent, step):
        """Defender's action: protect the group"""
        # Use development stage to adjust defense strategy
        stage = agent.getDevelopmentalStage()
        if stage == "Adult":
            return self.adaptive_defense_action(agent, step)
        else:
            return self.basic_defense_action(agent, step)
    
    def calculate_exploration_bonus(self):
        """Calculate bonus for exploration behavior"""
        total_curiosity = sum(agent.getCuriosityLevel() for agent in self.agents)
        return total_curiosity * 0.5
    
    def collect_group_metrics(self):
        """Collect metrics for group performance"""
        return {
            "total_rewards": [agent.getNeuromodulationLevel() for agent in self.agents],
            "curiosity_levels": [agent.getCuriosityLevel() for agent in self.agents],
            "novelty_levels": [agent.getNoveltyLevel() for agent in self.agents],
            "developmental_stages": [agent.getDevelopmentalStage() for agent in self.agents]
        }

# Usage example
if __name__ == "__main__":
    system = MultiAgentSystem(num_agents=3)
    system.create_cooperative_agents()
    results = system.run_cooperative_simulation(500)
    
    print("Multi-agent cooperative simulation completed!")
    print(f"Final metrics: {results}")
```

## Complex Learning Environments

Example showing NLM agents in complex, dynamic environments.

```python
import pynlm
import numpy as np
import random

class DynamicEnvironment:
    def __init__(self):
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=50, height=50, visionWidth=12, visionHeight=12)
        self.world.setRandomSeed(42)
        
        # Initialize with dynamic elements
        self.resources = []
        self.hazards = []
        self.moving_hazards = []
        self.resource_patterns = []
        
        self.initialize_dynamic_environment()
    
    def initialize_dynamic_environment(self):
        """Create a dynamic environment with changing conditions"""
        # Fixed resources
        for i in range(8):
            x = random.uniform(5, 45)
            y = random.uniform(5, 45)
            self.resources.append({
                'x': x, 'y': y, 'value': random.uniform(10, 30),
                'decay_rate': random.uniform(0.01, 0.05),
                'respawn_time': random.uniform(100, 300)
            })
        
        # Static hazards
        for i in range(3):
            x = random.uniform(10, 40)
            y = random.uniform(10, 40)
            self.hazards.append({
                'x': x, 'y': y, 'damage': random.uniform(5, 15),
                'radius': random.uniform(0.8, 1.5)
            })
        
        # Moving hazards (will be updated each step)
        self.moving_hazard_types = [
            self.simple_walker,
            self.cyclical_mover,
            self.random_walker
        ]
    
    def simple_walker(self, hazard, step):
        """Hazard that moves in a simple pattern"""
        pattern = step % 200
        if pattern < 50:
            hazard['x'] += 1.0
        elif pattern < 100:
            hazard['y'] += 1.0
        elif pattern < 150:
            hazard['x'] -= 1.0
        else:
            hazard['y'] -= 1.0
        
        # Wrap around world boundaries
        hazard['x'] = max(1.0, min(49.0, hazard['x']))
        hazard['y'] = max(1.0, min(49.0, hazard['y']))
    
    def cyclical_mover(self, hazard, step):
        """Hazard that moves in cycles"""
        angle = (step * 0.05) % (2 * 3.14159)
        hazard['x'] = 25 + 20 * np.cos(angle)
        hazard['y'] = 25 + 20 * np.sin(angle)
    
    def random_walker(self, hazard, step):
        """Hazard that moves randomly"""
        if random.random() < 0.3:
            dx = random.uniform(-2.0, 2.0)
            dy = random.uniform(-2.0, 2.0)
            hazard['x'] += dx
            hazard['y'] += dy
            
            hazard['x'] = max(1.0, min(49.0, hazard['x']))
            hazard['y'] = max(1.0, min(49.0, hazard['y']))
    
    def update(self, step):
        """Update environment state"""
        # Update resources (decay)
        for resource in self.resources:
            resource['value'] *= (1.0 - resource['decay_rate'])
            if resource['value'] <= 0.1:
                # Resource depleted, respawn after delay
                if step % resource['respawn_time'] == 0:
                    resource['value'] = random.uniform(10, 30)
        
        # Update moving hazards
        if step % 10 == 0:  # Update less frequently
            for hazard in self.moving_hazards:
                hazard_type = hazard['type']
                hazard_type(hazard, step)
        
        # Check for agent-environment interactions
        return self.check_interactions()
    
    def check_interactions(self):
        """Check for agent-environment interactions"""
        results = {'resources': [], 'hazards': []}
        
        # Check resources (simplified - would integrate with agent body)
        for resource in self.resources:
            if resource['value'] > 0.1:
                results['resources'].append(resource)
        
        # Check hazards (simplified)
        for hazard in self.hazards:
            results['hazards'].append(hazard)
        
        return results

class AdaptiveLearningInDynamicEnvironment:
    def __init__(self):
        self.environment = DynamicEnvironment()
        self.config = pynlm.createDefaultConfig()
        self.config.set("neuron_count", 1200)
        self.config.set("enable_structural_plasticity", True)
        self.config.set("enable_curiosity", True)
        self.config.set("enable_development", True)
        
        self.brain = pynlm.createBrain(self.config)
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.environment.world)
        
        # Enable learning features
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
    
    def run_adaptive_learning_study(self, num_steps=2000):
        """Run adaptive learning in dynamic environment"""
        print("Starting adaptive learning in dynamic environment...")
        
        learning_metrics = {
            'rewards': [],
            'curiosity': [],
            'novelty': [],
            'energy': [],
            'environmental_changes': []
        }
        
        for step in range(num_steps):
            # Get environment state
            env_state = self.environment.update(step)
            environmental_changes = len(env_state['resources']) + len(env_state['hazards'])
            
            # Agent acts based on current environment
            action = self.agent.decodeMotorCommand()
            
            # Apply action (simplified - would integrate with real world)
            result = self.agent.applyMotorCommand(action, self.agent.getBrain()->getTotalSpikeCount())
            reward = result.reward
            
            # Process sensory input
            self.agent.processSensoryInput(self.agent.getSensoryInput())
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Brain updates
            self.brain.step(step)
            
            # Record metrics
            learning_metrics['rewards'].append(reward)
            learning_metrics['curiosity'].append(self.agent.getCuriosityLevel())
            learning_metrics['novelty'].append(self.agent.getNoveltyLevel())
            learning_metrics['energy'].append(self.agent.getBrain()->getTotalSpikeCount())
            learning_metrics['environmental_changes'].append(environmental_changes)
            
            if step % 200 == 0:
                print(f"Step {step}: Reward={reward:.2f}, "
                      f"Curiosity={self.agent.getCuriosityLevel():.3f}, "
                      f"Novelty={self.agent.getNoveltyLevel():.3f}, "
                      f"Env_changes={environmental_changes}")
        
        return learning_metrics
    
    def analyze_adaptive_learning(self, metrics):
        """Analyze adaptive learning results"""
        import matplotlib.pyplot as plt
        
        fig, axes = plt.subplots(2, 3, figsize=(15, 10))
        
        # Reward over time
        axes[0, 0].plot(metrics['rewards'])
        axes[0, 0].set_title('Reward over Time')
        axes[0, 0].set_xlabel('Step')
        axes[0, 0].set_ylabel('Reward')
        
        # Curiosity over time
        axes[0, 1].plot(metrics['curiosity'])
        axes[0, 1].set_title('Curiosity over Time')
        axes[0, 1].set_xlabel('Step')
        axes[0, 1].set_ylabel('Curiosity Level')
        
        # Novelty over time
        axes[0, 2].plot(metrics['novelty'])
        axes[0, 2].set_title('Novelty over Time')
        axes[0, 2].set_xlabel('Step')
        axes[0, 2].set_ylabel('Novelty Level')
        
        # Environmental changes over time
        axes[1, 0].bar(range(0, len(metrics['environmental_changes']), 50),
                      metrics['environmental_changes'][::50])
        axes[1, 0].set_title('Environmental Changes')
        axes[1, 0].set_xlabel('Step (sampled)')
        axes[1, 0].set_ylabel('Number of Changes')
        
        # Learning curve (moving average)
        window = 50
        moving_avg = np.convolve(metrics['rewards'], np.ones(window)/window, mode='valid')
        axes[1, 1].plot(moving_avg)
        axes[1, 1].set_title('Learning Curve (Moving Average)')
        axes[1, 1].set_xlabel('Step')
        axes[1, 1].set_ylabel('Average Reward')
        
        # Curiosity vs Novelty relationship
        axes[1, 2].scatter(metrics['curiosity'], metrics['novelty'], alpha=0.5, s=1)
        axes[1, 2].set_title('Curiosity vs Novelty')
        axes[1, 2].set_xlabel('Curiosity Level')
        axes[1, 2].set_ylabel('Novelty Level')
        
        plt.tight_layout()
        plt.show()
        
        return self.summarize_adaptive_learning(metrics)
    
    def summarize_adaptive_learning(self, metrics):
        """Summarize adaptive learning results"""
        return {
            'total_reward': sum(metrics['rewards']),
            'mean_reward': np.mean(metrics['rewards']),
            'reward_variance': np.var(metrics['rewards']),
            'final_curiosity': metrics['curiosity'][-1],
            'final_novelty': metrics['novelty'][-1],
            'environmental_stability': 1.0 / (1.0 + np.var(metrics['environmental_changes'])),
            'learning_efficiency': (metrics['rewards'][-1] - metrics['rewards'][0]) / (metrics['rewards'][0] + 0.01)
        }

# Usage example
if __name__ == "__main__":
    study = AdaptiveLearningInDynamicEnvironment()
    metrics = study.run_adaptive_learning_study(1000)
    results = study.analyze_adaptive_learning(metrics)
    
    print("\nAdaptive Learning Study Results:")
    for key, value in results.items():
        print(f"  {key}: {value:.4f}")
```

## Performance Optimization Examples

Examples demonstrating performance optimization techniques.

```python
import pynlm
import time
import psutil
import threading
from concurrent.futures import ThreadPoolExecutor

class PerformanceOptimizer:
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
        self.base_config = self.config.copy() if hasattr(self.config, 'copy') else self.config
        
        # Performance tracking
        self.performance_metrics = {
            'memory_usage': [],
            'cpu_usage': [],
            'simulation_time': [],
            'learning_speed': []
        }
    
    def run_performance_test(self, config_name, num_steps=500, num_runs=5):
        """Run performance test with specific configuration"""
        print(f"Running performance test: {config_name}")
        
        total_time = 0
        peak_memory = 0
        
        for run in range(num_runs):
            # Apply configuration
            self.apply_configuration(config_name)
            
            # Create fresh brain for each run
            brain = pynlm.createBrain(self.config)
            brain.initialize()
            
            # Monitor system resources
            process = psutil.Process()
            initial_memory = process.memory_info().rss / 1024 / 1024  # MB
            
            # Run simulation
            start_time = time.time()
            for step in range(num_steps):
                brain.step(step)
                # Add some work to measure performance
                if step % 100 == 0:
                    _ = brain.getTotalSpikeCount()
            
            end_time = time.time()
            
            # Measure performance
            run_time = end_time - start_time
            final_memory = process.memory_info().rss / 1024 / 1024  # MB
            memory_increase = final_memory - initial_memory
            
            total_time += run_time
            peak_memory = max(peak_memory, memory_increase)
            
            self.performance_metrics['simulation_time'].append(run_time)
            self.performance_metrics['memory_usage'].append(memory_increase)
            self.performance_metrics['cpu_usage'].append(process.cpu_percent())
            self.performance_metrics['learning_speed'].append(
                brain.getTotalSpikeCount() / run_time
            )
            
            print(f"  Run {run + 1}: Time={run_time:.2f}s, "
                  f"Memory={memory_increase:.1f}MB, "
                  f"Learning Speed={brain.getTotalSpikeCount() / run_time:.0f} spikes/s")
        
        return self.calculate_performance_summary()
    
    def apply_configuration(self, config_name):
        """Apply specific configuration"""
        if config_name == "minimal":
            self.config.set("neuron_count", 500)
            self.config.set("connection_probability", 0.05)
            self.config.set("enable_structural_plasticity", False)
            self.config.set("enable_curiosity", False)
        elif config_name == "balanced":
            self.config = self.base_config.copy() if hasattr(self.base_config, 'copy') else self.base_config
        elif config_name == "optimized":
            self.config.set("neuron_count", 2000)
            self.config.set("connection_probability", 0.15)
            self.config.set("enable_structural_plasticity", True)
            self.config.set("enable_curiosity", True)
            self.config.set("enable_development", True)
        elif config_name == "research":
            self.config.set("neuron_count", 5000)
            self.config.set("connection_probability", 0.2)
            self.config.set("enable_structural_plasticity", True)
            self.config.set("enable_curiosity", True)
            self.config.set("enable_development", True)
    
    def calculate_performance_summary(self):
        """Calculate performance summary statistics"""
        return {
            'avg_time': np.mean(self.performance_metrics['simulation_time']),
            'std_time': np.std(self.performance_metrics['simulation_time']),
            'avg_memory': np.mean(self.performance_metrics['memory_usage']),
            'std_memory': np.std(self.performance_metrics['memory_usage']),
            'avg_cpu': np.mean(self.performance_metrics['cpu_usage']),
            'avg_learning_speed': np.mean(self.performance_metrics['learning_speed']),
            'speedups': {
                'optimized_vs_minimal': (self.calculate_efficiency_score('optimized') / 
                                       self.calculate_efficiency_score('minimal')),
                'research_vs_minimal': (self.calculate_efficiency_score('research') / 
                                      self.calculate_efficiency_score('minimal'))
            }
        }
    
    def calculate_efficiency_score(self, config_name):
        """Calculate efficiency score for a configuration"""
        # Efficiency = (learning output) / (resource cost)
        learning_output = self.performance_metrics['learning_speed'][-1] if self.performance_metrics['learning_speed'] else 0
        resource_cost = (np.mean(self.performance_metrics['memory_usage']) + 
                        np.mean(self.performance_metrics['simulation_time']))
        
        return learning_output / (resource_cost + 0.01)
    
    def run_optimization_study(self):
        """Run comprehensive optimization study"""
        configurations = ['minimal', 'balanced', 'optimized', 'research']
        
        results = {}
        for config in configurations:
            results[config] = self.run_performance_test(config, num_steps=300, num_runs=3)
        
        # Print comparison
        print("\nPerformance Optimization Study Results:")
        print("Configuration | Avg Time (s) | Avg Memory (MB) | Learning Speed (spikes/s)")
        print("-" * 80)
        for config, result in results.items():
            print(f"{config:12} | {result['avg_time']:11.2f} | {result['avg_memory']:15.1f} | {result['avg_learning_speed']:24.0f}")
        
        # Find best configuration
        best_config = max(results.keys(), key=lambda k: results[k]['avg_learning_speed'] / 
                         (results[k]['avg_time'] * results[k]['avg_memory']))
        
        print(f"\nBest performing configuration: {best_config}")
        print(f"Performance score: {results[best_config]['avg_learning_speed'] / (results[best_config]['avg_time'] * results[best_config]['avg_memory']):.4f}")
        
        return results

# Usage example
if __name__ == "__main__":
    optimizer = PerformanceOptimizer()
    results = optimizer.run_optimization_study()
```

## Research Methodology Examples

Examples demonstrating research methodologies and experimental designs.

```python
import pynlm
import numpy as np
import pandas as pd
from scipy import stats
import matplotlib.pyplot as plt

class NeuralDevelopmentStudy:
    def __init__(self):
        self.data_repository = {}
        self.experimental_conditions = {}
    
    def setup_development_experiment(self, num_subjects=20, duration_steps=1000):
        """Setup experimental study on neural development"""
        print(f"Setting up development study with {num_subjects} subjects, {duration_steps} steps each")
        
        subjects = []
        for subject_id in range(num_subjects):
            # Create subject-specific configuration
            config = pynlm.createDefaultConfig()
            
            # Vary parameters to study effects
            config.set("neuron_count", np.random.choice([500, 1000, 1500, 2000]))
            config.set("connection_probability", np.random.uniform(0.05, 0.2))
            config.set("enable_structural_plasticity", subject_id % 2 == 0)
            config.set("enable_curiosity", subject_id % 3 == 0)
            config.set("enable_development", subject_id % 2 == 0)
            
            # Set subject seed for reproducibility
            config.set("random_seed", subject_id + 42)
            
            # Create brain
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            subject = {
                'id': subject_id,
                'config': config,
                'brain': brain,
                'development_stages': [],
                'learning_metrics': [],
                'performance_metrics': []
            }
            
            subjects.append(subject)
        
        self.data_repository['development_study'] = subjects
        return subjects
    
    def run_development_phases(self, subjects, phases_config):
        """Run developmental phases for all subjects"""
        print(f"Running {len(phases_config)} developmental phases")
        
        for phase_name, phase_duration in phases_config.items():
            print(f"\n--- Running {phase_name} phase ---")
            
            for subject in subjects:
                # Implement phase-specific dynamics
                if phase_name == "initial":
                    self.run_initial_phase(subject, phase_duration)
                elif phase_name == "critical_period":
                    self.run_critical_period_phase(subject, phase_duration)
                elif phase_name == "maturation":
                    self.run_maturation_phase(subject, phase_duration)
                elif phase_name == "adult":
                    self.run_adult_phase(subject, phase_duration)
                
                # Record developmental stage
                subject['development_stages'].append({
                    'phase': phase_name,
                    'duration': phase_duration,
                    'developmental_stage': subject['brain'].getDevelopmentalStage(),
                    'total_neurons': subject['brain'].getTotalNeuronCount(),
                    'total_synapses': subject['brain'].getTotalSynapseCount()
                })
    
    def run_initial_phase(self, subject, duration):
        """Run initial development phase"""
        initial_plasticity = 1.0
        
        for step in range(duration):
            # Initial phase: high plasticity, exploration
            subject['brain'].step(step)
            
            # Record metrics
            metrics = self.record_subject_metrics(subject, step, phase="initial")
            subject['learning_metrics'].append(metrics)
    
    def run_critical_period_phase(self, subject, duration):
        """Run critical period phase"""
        critical_plasticity = 0.8
        
        for step in range(duration):
            # Critical period: continued learning, sensitive periods
            subject['brain'].step(step)
            
            # Apply phase-specific modulation
            if step % 100 == 0:
                # Simulate experience-dependent plasticity
                subject['brain'].getSTDP()->setLTPWeight(0.015)
                subject['brain'].getSTDP()->setLTDWeight(0.018)
            
            metrics = self.record_subject_metrics(subject, step, phase="critical_period")
            subject['learning_metrics'].append(metrics)
    
    def run_maturation_phase(self, subject, duration):
        """Run maturation phase"""
        maturation_plasticity = 0.5
        
        for step in range(duration):
            # Maturation: refined connections, specialization
            subject['brain'].step(step)
            
            # Apply phase-specific modulation
            if step % 100 == 0:
                # Simulate activity-dependent refinement
                subject['brain'].getSTDP()->setLTPWeight(0.01)
                subject['brain'].getSTDP()->setLTDWeight(0.012)
            
            metrics = self.record_subject_metrics(subject, step, phase="maturation")
            subject['learning_metrics'].append(metrics)
    
    def run_adult_phase(self, subject, duration):
        """Run adult phase"""
        adult_plasticity = 0.2
        
        for step in range(duration):
            # Adult: stable, maintenance, some flexibility
            subject['brain'].step(step)
            
            # Apply phase-specific modulation
            if step % 100 == 0:
                # Simulate adult maintenance
                subject['brain'].getSTDP()->setLTPWeight(0.005)
                subject['brain'].getSTDP()->setLTDWeight(0.006)
            
            metrics = self.record_subject_metrics(subject, step, phase="adult")
            subject['learning_metrics'].append(metrics)
    
    def record_subject_metrics(self, subject, step, phase):
        """Record metrics for a subject at a given step"""
        return {
            'step': step,
            'phase': phase,
            'developmental_stage': subject['brain'].getDevelopmentalStage(),
            'total_neurons': subject['brain'].getTotalNeuronCount(),
            'total_synapses': subject['brain'].getTotalSynapseCount(),
            'active_neurons': subject['brain'].getActiveNeuronCount(),
            'firing_neurons': subject['brain'].getFiringNeuronCount(),
            'average_firing_rate': subject['brain'].getAverageFiringRate(),
            'excitatory_inhibition_ratio': subject['brain'].getExcitationInhibitionRatio(),
            'total_spikes': subject['brain'].getTotalSpikeCount(),
            'memory_traces': len(subject['brain'].getWorkingMemory().getActiveTraces()) if subject['brain'].getWorkingMemory() else 0,
            'episodes_stored': subject['brain'].getEpisodicMemory().getEpisodeCount() if subject['brain'].getEpisodicMemory() else 0
        }
    
    def analyze_development_data(self, subjects):
        """Analyze developmental data across subjects"""
        # Create analysis DataFrame
        analysis_data = []
        
        for subject in subjects:
            for metric in subject['learning_metrics']:
                analysis_data.append({
                    'subject_id': subject['id'],
                    'step': metric['step'],
                    'phase': metric['phase'],
                    'developmental_stage': metric['developmental_stage'],
                    'total_neurons': metric['total_neurons'],
                    'total_synapses': metric['total_synapses'],
                    'average_firing_rate': metric['average_firing_rate'],
                    'excitatory_inhibition_ratio': metric['excitatory_inhibition_ratio'],
                    'total_spikes': metric['total_spikes'],
                    'memory_traces': metric['memory_traces'],
                    'episodes_stored': metric['episodes_stored']
                })
        
        df = pd.DataFrame(analysis_data)
        
        # Statistical analysis
        development_stats = {
            'subject_variance': df.groupby('subject_id')['total_synapses'].std(),
            'phase_effects': df.groupby('phase')['average_firing_rate'].agg(['mean', 'std', 'count']),
            'developmental_progression': self.calculate_developmental_progression(df),
            'learning_curves': self.fit_learning_curves(df)
        }
        
        return development_stats, df
    
    def calculate_developmental_progression(self, df):
        """Calculate developmental progression metrics"""
        progression_metrics = {}
        
        for subject_id in df['subject_id'].unique():
            subject_data = df[df['subject_id'] == subject_id].sort_values('step')
            
            # Calculate rates of change
            neuron_growth_rate = (subject_data['total_neurons'].iloc[-1] - 
                                subject_data['total_neurons'].iloc[0]) / len(subject_data)
            
            synapse_growth_rate = (subject_data['total_synapses'].iloc[-1] - 
                                 subject_data['total_synapses'].iloc[0]) / len(subject_data)
            
            learning_rate = (subject_data['average_firing_rate'].iloc[-1] - 
                           subject_data['average_firing_rate'].iloc[0]) / len(subject_data)
            
            progression_metrics[subject_id] = {
                'neuron_growth_rate': neuron_growth_rate,
                'synapse_growth_rate': synapse_growth_rate,
                'learning_rate': learning_rate,
                'stability_score': 1.0 / (1.0 + df[df['subject_id'] == subject_id]['total_synapses'].std())
            }
        
        return progression_metrics
    
    def fit_learning_curves(self, df):
        """Fit learning curves to developmental data"""
        learning_curves = {}
        
        for subject_id in df['subject_id'].unique():
            subject_data = df[df['subject_id'] == subject_id]
            
            # Fit exponential growth model
            X = subject_data['step'].values
            y = subject_data['total_synapses'].values
            
            # Simple linear regression for learning rate
            coeffs = np.polyfit(X, y, 1)
            slope = coeffs[0]  # Learning rate (synapses per step)
            intercept = coeffs[1]  # Initial synapse count
            
            learning_curves[subject_id] = {
                'slope': slope,
                'intercept': intercept,
                'r_squared': self.calculate_r_squared(X, y, slope, intercept),
                'predicted_final': slope * X.max() + intercept
            }
        
        return learning_curves
    
    def calculate_r_squared(self, X, y, slope, intercept):
        """Calculate R-squared for linear regression"""
        y_pred = slope * X + intercept
        ss_res = np.sum((y - y_pred) ** 2)
        ss_tot = np.sum((y - np.mean(y)) ** 2)
        return 1 - (ss_res / ss_tot) if ss_tot > 0 else 0
    
    def run_research_study(self, experimental_design):
        """Run comprehensive research study"""
        print(f"Running research study with {len(experimental_design)} experimental conditions")
        
        # Run all experimental conditions
        results = {}
        for condition_name, condition_params in experimental_design.items():
            print(f"\n--- Running condition: {condition_name} ---")
            
            # Setup experiment
            subjects = self.setup_development_experiment(
                num_subjects=condition_params['num_subjects'],
                duration_steps=condition_params['duration_steps']
            )
            
            # Run developmental phases
            self.run_development_phases(subjects, condition_params['developmental_phases'])
            
            # Analyze results
            stats, df = self.analyze_development_data(subjects)
            
            results[condition_name] = {
                'subjects': subjects,
                'statistics': stats,
                'dataframe': df,
                'parameters': condition_params
            }
            
            print(f"Completed condition: {condition_name}")
        
        # Compare conditions
        self.compare_experimental_conditions(results)
        
        return results
    
    def compare_experimental_conditions(self, results):
        """Compare results across experimental conditions"""
        print("\n=== Experimental Condition Comparison ===")
        
        comparison_data = {}
        for condition_name, result in results.items():
            df = result['dataframe']
            
            comparison_data[condition_name] = {
                'mean_final_synapses': df.groupby('subject_id')['total_synapses'].last().mean(),
                'std_final_synapses': df.groupby('subject_id')['total_synapses'].last().std(),
                'mean_final_firing_rate': df.groupby('subject_id')['average_firing_rate'].last().mean(),
                'mean_learning_efficiency': self.calculate_learning_efficiency(df),
                'num_subjects': df['subject_id'].nunique()
            }
        
        # Print comparison table
        print(f"{'Condition':<20} {'Final Synapses':<15} {'Std Dev':<10} {'Firing Rate':<15} {'Learning Eff':<12} {'Subjects':<8}")
        print("-" * 90)
        
        for condition, data in comparison_data.items():
            print(f"{condition:<20} {data['mean_final_synapses']:<15.1f} "
                  f"{data['std_final_synapses']:<10.1f} {data['mean_final_firing_rate']:<15.3f} "
                  f"{data['mean_learning_efficiency']:<12.4f} {data['num_subjects']:<8}")
        
        # Statistical comparison
        print(f"\nStatistical Comparisons:")
        
        conditions = list(comparison_data.keys())
        if len(conditions) >= 2:
            # Compare specific conditions
            condition_a = conditions[0]
            condition_b = conditions[1]
            
            synapses_a = df[df['condition'] == condition_a]['total_synapses']
            synapses_b = df[df['condition'] == condition_b]['total_synapses']
            
            if len(synapses_a) > 1 and len(synapses_b) > 1:
                t_stat, p_value = stats.ttest_ind(synapses_a, synapses_b)
                print(f"  Synapses difference between {condition_a} and {condition_b}: t={t_stat:.3f}, p={p_value:.4f}")
    
    def calculate_learning_efficiency(self, df):
        """Calculate learning efficiency metric"""
        # Efficiency = (final performance) / (initial performance + time cost)
        subject_performance = df.groupby('subject_id').agg({
            'total_synapses': ['first', 'last'],
            'average_firing_rate': ['first', 'last'],
            'step': ['min', 'max']
        })
        
        efficiency_scores = []
        for subject_id in df['subject_id'].unique():
            subject_data = df[df['subject_id'] == subject_id]
            
            initial_synapses = subject_data['total_synapses'].iloc[0]
            final_synapses = subject_data['total_synapses'].iloc[-1]
            initial_firing_rate = subject_data['average_firing_rate'].iloc[0]
            final_firing_rate = subject_data['average_firing_rate'].iloc[-1]
            duration_steps = subject_data['step'].max() - subject_data['step'].min()
            
            # Calculate efficiency
            synapse_efficiency = (final_synapses - initial_synapses) / (initial_synapses + 0.01)
            firing_efficiency = (final_firing_rate - initial_firing_rate) / (initial_firing_rate + 0.01)
            time_efficiency = 1.0 / (duration_steps + 1.0)
            
            efficiency = (synapse_efficiency + firing_efficiency + time_efficiency) / 3.0
            efficiency_scores.append(efficiency)
        
        return np.mean(efficiency_scores)

# Usage example
if __name__ == "__main__":
    # Define experimental design
    experimental_design = {
        'baseline': {
            'num_subjects': 10,
            'duration_steps': 500,
            'developmental_phases': {
                'initial': 100,
                'critical_period': 200,
                'maturation': 150,
                'adult': 50
            }
        },
        'enhanced': {
            'num_subjects': 10,
            'duration_steps': 500,
            'developmental_phases': {
                'initial': 100,
                'critical_period': 200,
                'maturation': 150,
                'adult': 50
            }
        },
        'restricted': {
            'num_subjects': 10,
            'duration_steps': 500,
            'developmental_phases': {
                'initial': 100,
                'critical_period': 200,
                'maturation': 150,
                'adult': 50
            }
        }
    }
    
    # Modify restricted condition for comparison
    experimental_design['restricted']['enable_structural_plasticity'] = False
    experimental_design['restricted']['enable_curiosity'] = False
    experimental_design['restricted']['enable_development'] = False
    
    # Run research study
    study = NeuralDevelopmentStudy()
    results = study.run_research_study(experimental_design)
    
    print("\nResearch study completed!")
    print(f"Total experimental conditions: {len(results)}")
    print(f"Total subjects: {sum(len(r['subjects']) for r in results.values())}")
```

## Advanced Debugging and Validation

Examples demonstrating advanced debugging and validation techniques.

```python
import pynlm
import numpy as np
import traceback
import json
from datetime import datetime

class BrainDebugger:
    def __init__(self):
        self.debug_log = []
        self.validation_results = {}
        self.performance_benchmarks = {}
    
    def log_debug_info(self, message, level="INFO", details=None):
        """Log debug information"""
        timestamp = datetime.now().isoformat()
        log_entry = {
            'timestamp': timestamp,
            'level': level,
            'message': message,
            'details': details or {},
            'stack_trace': traceback.format_stack() if level == "ERROR" else None
        }
        self.debug_log.append(log_entry)
        
        if level == "ERROR":
            print(f"[ERROR] {timestamp}: {message}")
            if details:
                print(f"  Details: {json.dumps(details, indent=2)}")
        elif level == "WARNING":
            print(f"[WARNING] {timestamp}: {message}")
        else:
            print(f"[INFO] {timestamp}: {message}")
    
    def validate_brain_integrity(self, brain):
        """Validate brain state integrity"""
        validation_results = {
            'is_valid': True,
            'issues': [],
            'warnings': [],
            'checks': {}
        }
        
        # Check 1: Basic connectivity
        try:
            neuron_count = brain.getTotalNeuronCount()
            synapse_count = brain.getTotalSynapseCount()
            region_count = brain.getRegionCount()
            
            if neuron_count <= 0:
                validation_results['issues'].append("No neurons in brain")
                validation_results['is_valid'] = False
            
            if synapse_count < neuron_count * 0.1:  # Should have at least minimal connectivity
                validation_results['warnings'].append(f"Low connectivity: {synapse_count} synapses for {neuron_count} neurons")
            
            validation_results['checks']['connectivity'] = {
                'neurons': neuron_count,
                'synapses': synapse_count,
                'regions': region_count,
                'synapse_per_neuron': synapse_count / neuron_count if neuron_count > 0 else 0
            }
            
        except Exception as e:
            validation_results['issues'].append(f"Connectivity check failed: {str(e)}")
            validation_results['is_valid'] = False
        
        # Check 2: Plasticity systems
        try:
            # Check STDP
            stdp = brain.getSTDP()
            if stdp:
                ltp_weight = stdp.getLTPWeight()
                ltd_weight = stdp.getLTDWeight()
                if ltp_weight <= 0 or ltd_weight <= 0:
                    validation_results['warnings'].append("STDP weights are non-positive")
            else:
                validation_results['issues'].append("STDP system not available")
                validation_results['is_valid'] = False
            
            # Check Hebbian
            hebbian = brain.getHebbian()
            if hebbian:
                validation_results['checks']['plasticity'] = {
                    'stdp_available': stdp is not None,
                    'hebbian_available': hebbian is not None,
                    'structural_plasticity': brain.getStructuralPlasticity() is not None
                }
            else:
                validation_results['issues'].append("Hebbian system not available")
                validation_results['is_valid'] = False
            
        except Exception as e:
            validation_results['issues'].append(f"Plasticity check failed: {str(e)}")
            validation_results['is_valid'] = False
        
        # Check 3: Memory systems
        try:
            working_memory = brain.getWorkingMemory()
            episodic_memory = brain.getEpisodicMemory()
            associative_memory = brain.getAssociativeMemory()
            
            validation_results['checks']['memory'] = {
                'working_memory': working_memory is not None,
                'episodic_memory': episodic_memory is not None,
                'associative_memory': associative_memory is not None,
                'working_memory_capacity': working_memory.getCapacity() if working_memory else 0,
                'episodic_memory_limit': episodic_memory.getMaxEpisodes() if episodic_memory else 0
            }
            
            if working_memory is None:
                validation_results['warnings'].append("Working memory not available")
            
            if episodic_memory is None:
                validation_results['warnings'].append("Episodic memory not available")
            
        except Exception as e:
            validation_results['issues'].append(f"Memory check failed: {str(e)}")
            validation_results['is_valid'] = False
        
        # Check 4: Neuromodulation systems
        try:
            dopamine = brain.getDopamine()
            curiosity = brain.getCuriosity()
            novelty = brain.getNovelty()
            prediction_error = brain.getPredictionErrorSignal()
            
            validation_results['checks']['neuromodulation'] = {
                'dopamine': dopamine is not None,
                'curiosity': curiosity is not None,
                'novelty': novelty is not None,
                'prediction_error': prediction_error is not None
            }
            
        except Exception as e:
            validation_results['issues'].append(f"Neuromodulation check failed: {str(e)}")
            validation_results['is_valid'] = False
        
        # Check 5: Consistency checks
        try:
            # Check for consistency between components
            developmental_stage = brain.getDevelopmentalStage()
            total_spikes = brain.getTotalSpikeCount()
            average_firing_rate = brain.getAverageFiringRate()
            
            validation_results['checks']['consistency'] = {
                'developmental_stage': developmental_stage,
                'total_spikes': total_spikes,
                'average_firing_rate': average_firing_rate,
                'spike_rate_reasonable': 0 <= average_firing_rate <= 1000
            }
            
            if total_spikes < 0:
                validation_results['issues'].append("Negative total spike count")
                validation_results['is_valid'] = False
            
        except Exception as e:
            validation_results['issues'].append(f"Consistency check failed: {str(e)}")
            validation_results['is_valid'] = False
        
        self.validation_results['brain_integrity'] = validation_results
        
        return validation_results
    
    def benchmark_brain_performance(self, brain, num_steps=100, num_runs=5):
        """Benchmark brain performance"""
        benchmark_results = {
            'steps_per_second': [],
            'memory_usage': [],
            'cpu_usage': [],
            'learning_speed': [],
            'stability': []
        }
        
        print("Starting brain performance benchmark...")
        
        for run in range(num_runs):
            import time
            import psutil
            import gc
            
            # Force garbage collection
            gc.collect()
            
            # Measure system resources
            process = psutil.Process()
            initial_memory = process.memory_info().rss / 1024 / 1024  # MB
            initial_cpu = process.cpu_percent()
            
            # Measure brain performance
            start_time = time.time()
            
            for step in range(num_steps):
                brain.step(step)
                
                # Record performance metrics
                if step % 10 == 0:
                    current_memory = process.memory_info().rss / 1024 / 1024  # MB
                    current_cpu = process.cpu_percent()
                    
                    benchmark_results['memory_usage'].append(current_memory - initial_memory)
                    benchmark_results['cpu_usage'].append(current_cpu)
                    benchmark_results['learning_speed'].append(
                        brain.getTotalSpikeCount() / (time.time() - start_time)
                    )
            
            end_time = time.time()
            final_memory = process.memory_info().rss / 1024 / 1024  # MB
            final_cpu = process.cpu_percent()
            
            # Calculate metrics
            run_time = end_time - start_time
            steps_per_second = num_steps / run_time
            memory_increase = final_memory - initial_memory
            
            benchmark_results['steps_per_second'].append(steps_per_second)
            benchmark_results['memory_usage'].append(memory_increase)
            benchmark_results['cpu_usage'].append((initial_cpu + final_cpu) / 2)
            
            print(f"  Run {run + 1}: {steps_per_second:.1f} steps/s, "
                  f"{memory_increase:.1f}MB memory, {(initial_cpu + final_cpu)/2:.1f}% CPU")
        
        # Calculate stability metrics
        stability_metrics = self.calculate_stability_metrics(benchmark_results)
        benchmark_results['stability'] = stability_metrics
        
        self.performance_benchmarks['brain_performance'] = benchmark_results
        
        return benchmark_results
    
    def calculate_stability_metrics(self, benchmark_results):
        """Calculate stability metrics from benchmark results"""
        stability = {}
        
        if benchmark_results['steps_per_second']:
            stability['steps_per_second_stability'] = 1.0 / (1.0 + np.std(benchmark_results['steps_per_second']))
        
        if benchmark_results['learning_speed']:
            stability['learning_stability'] = 1.0 / (1.0 + np.std(benchmark_results['learning_speed']))
        
        if benchmark_results['memory_usage']:
            stability['memory_stability'] = 1.0 / (1.0 + np.std(benchmark_results['memory_usage']))
        
        if benchmark_results['cpu_usage']:
            stability['cpu_stability'] = 1.0 / (1.0 + np.std(benchmark_results['cpu_usage']))
        
        # Overall stability (weighted average)
        stability_scores = [v for k, v in stability.items() if k.endswith('_stability')]
        stability['overall_stability'] = np.mean(stability_scores) if stability_scores else 0.0
        
        return stability
    
    def run_comprehensive_validation(self, brain):
        """Run comprehensive validation and debugging"""
        print("Starting comprehensive brain validation...")
        
        validation_report = {
            'timestamp': datetime.now().isoformat(),
            'validation_results': {},
            'benchmark_results': {},
            'debug_log': self.debug_log[-100:],  # Last 100 entries
            'summary': {}
        }
        
        # Run validation checks
        validation_report['validation_results'] = self.validate_brain_integrity(brain)
        
        # Run performance benchmarks
        validation_report['benchmark_results'] = self.benchmark_brain_performance(brain)
        
        # Generate summary
        validation_report['summary'] = {
            'is_valid': validation_report['validation_results']['brain_integrity']['is_valid'],
            'issues_count': len(validation_report['validation_results']['brain_integrity']['issues']),
            'warnings_count': len(validation_report['validation_results']['brain_integrity']['warnings']),
            'performance_score': validation_report['benchmark_results']['brain_performance']['stability']['overall_stability'],
            'recommendations': self.generate_recommendations(validation_report)
        }
        
        # Log validation results
        self.log_debug_info(
            "Comprehensive validation completed",
            "INFO",
            {
                'is_valid': validation_report['summary']['is_valid'],
                'issues_count': validation_report['summary']['issues_count'],
                'performance_score': validation_report['summary']['performance_score']
            }
        )
        
        return validation_report
    
    def generate_recommendations(self, validation_report):
        """Generate recommendations based on validation results"""
        recommendations = []
        
        if not validation_report['validation_results']['brain_integrity']['is_valid']:
            recommendations.append("Fix critical issues before proceeding with simulations")
        
        issues = validation_report['validation_results']['brain_integrity']['issues']
        if issues:
            recommendations.extend([f"Address issue: {issue}" for issue in issues])
        
        warnings = validation_report['validation_results']['brain_integrity']['warnings']
        if warnings:
            recommendations.extend([f"Consider fixing warning: {warning}" for warning in warnings])
        
        performance_score = validation_report['benchmark_results']['brain_performance']['stability']['overall_stability']
        if performance_score < 0.5:
            recommendations.append("Performance is low. Consider optimization settings or hardware")
        
        if not recommendations:
            recommendations.append("All checks passed. Ready for use.")
        
        return recommendations

# Usage example
if __name__ == "__main__":
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Setup debugger
    debugger = BrainDebugger()
    debugger.log_debug_info("Brain creation completed", "INFO")
    
    # Run comprehensive validation
    validation_report = debugger.run_comprehensive_validation(brain)
    
    # Display results
    print("\n=== Validation Report ===")
    print(f"Overall validity: {validation_report['summary']['is_valid']}")
    print(f"Issues: {validation_report['summary']['issues_count']}")
    print(f"Warnings: {validation_report['summary']['warnings_count']}")
    print(f"Performance score: {validation_report['summary']['performance_score']:.3f}")
    
    if validation_report['summary']['recommendations']:
        print("\nRecommendations:")
        for recommendation in validation_report['summary']['recommendations']:
            print(f"  - {recommendation}")
    
    # Save validation report
    with open('validation_report.json', 'w') as f:
        json.dump(validation_report, f, indent=2, default=str)
    
    print(f"\nValidation report saved to validation_report.json")
```

## Custom Extension Examples

Examples demonstrating custom extensions and plugin systems.

```python
import pynlm
from abc import ABC, abstractmethod

class BrainExtension(ABC):
    """Abstract base class for brain extensions"""
    
    @abstractmethod
    def initialize(self, brain):
        """Initialize the extension with a brain"""
        pass
    
    @abstractmethod
    def process_step(self, step, brain):
        """Process a single step"""
        pass
    
    def cleanup(self):
        """Cleanup extension resources"""
        pass

class CustomLearningExtension(BrainExtension):
    """Custom extension that modifies learning rules"""
    
    def __init__(self):
        self.custom_learning_rules = {}
        self.learning_history = []
    
    def initialize(self, brain):
        """Initialize with brain and setup custom rules"""
        self.brain = brain
        
        # Override STDP parameters
        stdp = brain.getSTDP()
        if stdp:
            # Store original values
            self.original_ltp_weight = stdp.getLTPWeight()
            self.original_ltd_weight = stdp.getLTDWeight()
            
            # Apply custom rules
            stdp.setLTPWeight(0.02)  # Enhanced LTP
            stdp.setLTDWeight(0.015)  # Standard LTD
        
        # Setup monitoring
        self.monitoring_active = True
        self.monitoring_interval = 100
    
    def process_step(self, step, brain):
        """Process step with custom learning rules"""
        # Apply custom learning rules based on step
        if step % self.monitoring_interval == 0 and self.monitoring_active:
            self.record_learning_metrics(step, brain)
        
        # Apply custom modifications
        self.apply_custom_modifications(step, brain)
        
        # Log step information
        self.log_custom_step(step, brain)
    
    def apply_custom_modifications(self, step, brain):
        """Apply custom modifications based on learning stage"""
        # Get current developmental stage
        stage = brain.getDevelopmentalStage()
        
        # Apply stage-specific modifications
        if stage == "Initial":
            # High learning rate in initial stage
            self.enhance_plasticity(brain, factor=2.0)
        elif stage == "CriticalPeriod":
            # Sensitive period modifications
            self.modify_critical_period_rules(brain)
        elif stage == "Maturation":
            # Refinement phase
            self.refine_connections(brain)
        elif stage == "Adult":
            # Stable learning with occasional exploration
            self.stable_learning_mode(brain)
    
    def enhance_plasticity(self, brain, factor):
        """Enhance plasticity during initial stage"""
        stdp = brain.getSTDP()
        if stdp:
            current_ltp = stdp.getLTPWeight()
            current_ltd = stdp.getLTDWeight()
            
            # Store in learning history
            self.learning_history.append({
                'step': brain.getTotalSpikeCount(),
                'stage': "Initial",
                'ltp_weight': current_ltp,
                'ltd_weight': current_ltd,
                'enhancement_factor': factor
            })
            
            # Apply enhancement
            stdp.setLTPWeight(current_ltp * factor)
            stdp.setLTDWeight(current_ltd * factor)
    
    def modify_critical_period_rules(self, brain):
        """Modify rules during critical period"""
        stdp = brain.getSTDP()
        if stdp:
            # Make learning more sensitive to timing
            current_ltp = stdp.getLTPWeight()
            current_ltd = stdp.getLTDWeight()
            
            # Increase LTP weight for stronger potentiation
            stdp.setLTPWeight(current_ltp * 1.5)
            stdp.setLTDWeight(current_ltd * 1.2)  # Moderate LTD increase
            
            self.learning_history.append({
                'step': brain.getTotalSpikeCount(),
                'stage': "CriticalPeriod",
                'modification_type': "sensitive_period_enhancement"
            })
    
    def refine_connections(self, brain):
        """Refine connections during maturation"""
        stdp = brain.getSTDP()
        if stdp:
            # Refine synaptic weights
            current_ltp = stdp.getLTPWeight()
            current_ltd = stdp.getLTDWeight()
            
            # Reduce learning rates for refinement
            stdp.setLTPWeight(current_ltp * 0.7)
            stdp.setLTDWeight(current_ltd * 0.7)
            
            self.learning_history.append({
                'step': brain.getTotalSpikeCount(),
                'stage': "Maturation",
                'refinement_factor': 0.7
            })
    
    def stable_learning_mode(self, brain):
        """Enable stable learning mode in adult stage"""
        stdp = brain.getSTDP()
        if stdp:
            # Use original STDP weights for stability
            stdp.setLTPWeight(self.original_ltp_weight)
            stdp.setLTDWeight(self.original_ltd_weight)
            
            self.learning_history.append({
                'step': brain.getTotalSpikeCount(),
                'stage': "Adult",
                'mode': "stable_learning"
            })
    
    def record_learning_metrics(self, step, brain):
        """Record learning metrics for analysis"""
        metrics = {
            'step': step,
            'developmental_stage': brain.getDevelopmentalStage(),
            'total_neurons': brain.getTotalNeuronCount(),
            'total_synapses': brain.getTotalSynapseCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount(),
            'working_memory_traces': len(brain.getWorkingMemory().getActiveTraces()) if brain.getWorkingMemory() else 0
        }
        
        self.learning_history.append(metrics)
    
    def log_custom_step(self, step, brain):
        """Log custom step information"""
        # Log extension-specific information
        if step % 500 == 0:
            print(f"CustomExtension: Step {step}, "
                  f"Stage: {brain.getDevelopmentalStage()}, "
                  f"Neurons: {brain.getTotalNeuronCount()}, "
                  f"Synapses: {brain.getTotalSynapseCount()}")
    
    def get_learning_analysis(self):
        """Get analysis of learning patterns"""
        if not self.learning_history:
            return None
        
        # Analyze learning patterns
        analysis = {
            'total_steps_monitored': len(self.learning_history),
            'developmental_stages': list(set(item.get('stage', '') for item in self.learning_history if 'stage' in item)),
            'plasticity_changes': len([item for item in self.learning_history if 'enhancement_factor' in item]),
            'critical_period_modifications': len([item for item in self.learning_history if 'critical_period_modifications' in item]),
            'refinement_phases': len([item for item in self.learning_history if 'refinement_factor' in item]),
            'stable_learning_phases': len([item for item in self.learning_history if 'stable_learning' in item]),
            'average_firing_rate_trend': self.calculate_firing_rate_trend(),
            'synaptic_weight_evolution': self.calculate_synaptic_weight_evolution()
        }
        
        return analysis
    
    def calculate_firing_rate_trend(self):
        """Calculate firing rate trend over time"""
        firing_rates = [item['average_firing_rate'] for item in self.learning_history if 'average_firing_rate' in item]
        if len(firing_rates) < 2:
            return 0.0
        
        # Calculate linear trend
        x = list(range(len(firing_rates)))
        slope = np.polyfit(x, firing_rates, 1)[0]
        
        return slope
    
    def calculate_synaptic_weight_evolution(self):
        """Calculate synaptic weight evolution"""
        weights = []
        for item in self.learning_history:
            if 'ltp_weight' in item and 'ltd_weight' in item:
                weights.append((item['ltp_weight'], item['ltd_weight']))
        
        if not weights:
            return None
        
        ltp_weights = [w[0] for w in weights]
        ltd_weights = [w[1] for w in weights]
        
        return {
            'ltp_trend': np.polyfit(range(len(ltp_weights)), ltp_weights, 1)[0],
            'ltd_trend': np.polyfit(range(len(ltd_weights)), ltd_weights, 1)[0],
            'final_ltp': ltp_weights[-1],
            'final_ltd': ltd_weights[-1]
        }
    
    def export_learning_data(self):
        """Export learning data for analysis"""
        return {
            'learning_history': self.learning_history,
            'original_stdp_weights': {
                'ltp': self.original_ltp_weight,
                'ltd': self.original_ltd_weight
            }
        }

class MultiExtensionBrain:
    """Brain class that supports multiple extensions"""
    
    def __init__(self):
        self.brain = pynlm.createBrain(pynlm.createDefaultConfig())
        self.extensions = []
        self.extension_data = {}
    
    def add_extension(self, extension):
        """Add an extension to the brain"""
        self.extensions.append(extension)
        extension.initialize(self.brain)
        self.extension_data[extension.__class__.__name__] = {
            'active': True,
            'initialized': True,
            'history': []
        }
    
    def remove_extension(self, extension_name):
        """Remove an extension"""
        for extension in self.extensions:
            if extension.__class__.__name__ == extension_name:
                extension.cleanup()
                self.extensions.remove(extension)
                self.extension_data[extension_name]['active'] = False
                break
    
    def run_simulation(self, num_steps):
        """Run simulation with all extensions"""
        print(f"Running simulation with {len(self.extensions)} extensions")
        
        for step in range(num_steps):
            # Process each extension
            for extension in self.extensions:
                extension.process_step(step, self.brain)
            
            # Run brain simulation
            self.brain.step(step)
            
            # Log extension status
            for extension_name, extension_data in self.extension_data.items():
                if extension_data['active']:
                    extension_data['history'].append({
                        'step': step,
                        'brain_state': {
                            'total_synapses': self.brain.getTotalSynapseCount(),
                            'total_spikes': self.brain.getTotalSpikeCount(),
                            'developmental_stage': self.brain.getDevelopmentalStage()
                        }
                    })
        
        return self.get_simulation_results()
    
    def get_simulation_results(self):
        """Get simulation results from all extensions"""
        results = {}
        
        for extension_name, extension_data in self.extension_data.items():
            # Find the corresponding extension
            extension = None
            for ext in self.extensions:
                if ext.__class__.__name__ == extension_name:
                    extension = ext
                    break
            
            if extension:
                # Get extension-specific results
                if hasattr(extension, 'get_learning_analysis'):
                    results[extension_name] = extension.get_learning_analysis()
                else:
                    results[extension_name] = extension_data
        
        return results
    
    def cleanup(self):
        """Cleanup all extensions"""
        for extension in self.extensions:
            extension.cleanup()
        self.extensions.clear()
        self.extension_data.clear()

# Usage example
if __name__ == "__main__":
    # Create brain with custom extensions
    multi_brain = MultiExtensionBrain()
    
    # Add custom learning extension
    custom_learning = CustomLearningExtension()
    multi_brain.add_extension(custom_learning)
    
    # Add another extension (example)
    # memory_extension = MemoryMonitoringExtension()
    # multi_brain.add_extension(memory_extension)
    
    # Run simulation
    results = multi_brain.run_simulation(1000)
    
    # Analyze results
    print("\nExtension Analysis Results:")
    for extension_name, result in results.items():
        print(f"\n{extension_name}:")
        if isinstance(result, dict) and 'learning_history' in result:
            print(f"  Steps monitored: {len(result['learning_history'])}")
            print(f"  Developmental stages: {result.get('developmental_stages', [])}")
    
    # Cleanup
    multi_brain.cleanup()
```

## Production Deployment Examples

Examples for deploying NLM in production environments.

```python
import pynlm
import time
import logging
from multiprocessing import Pool
import signal
import sys

class ProductionNLMSimulator:
    """Production-ready NLM simulator with monitoring and fault tolerance"""
    
    def __init__(self, config, simulation_settings):
        self.config = config
        self.simulation_settings = simulation_settings
        self.setup_logging()
        self.setup_signal_handlers()
        self.performance_monitor = None
        self.error_handler = None
        
        # Initialize brain
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        # Setup monitoring
        self.setup_monitoring()
        
        # State management
        self.current_step = 0
        self.is_running = False
        self.should_stop = False
        
        logging.info("Production NLM simulator initialized")
    
    def setup_logging(self):
        """Setup logging configuration"""
        logging.basicConfig(
            level=logging.INFO,
            format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
            handlers=[
                logging.FileHandler('nlm_simulation.log'),
                logging.StreamHandler()
            ]
        )
        self.logger = logging.getLogger(__name__)
    
    def setup_signal_handlers(self):
        """Setup signal handlers for graceful shutdown"""
        signal.signal(signal.SIGINT, self.signal_handler)
        signal.signal(signal.SIGTERM, self.signal_handler)
    
    def signal_handler(self, signum, frame):
        """Handle shutdown signals"""
        self.logger.info(f"Received signal {signum}, shutting down gracefully...")
        self.should_stop = True
    
    def setup_monitoring(self):
        """Setup performance monitoring"""
        import psutil
        import time
        
        self.performance_monitor = {
            'memory_samples': [],
            'cpu_samples': [],
            'simulation_times': [],
            'learning_rates': [],
            'start_time': time.time()
        }
        
        self.logger.info("Performance monitoring setup complete")
    
    def start_simulation(self, num_steps):
        """Start simulation with monitoring"""
        self.logger.info(f"Starting simulation with {num_steps} steps")
        self.is_running = True
        self.should_stop = False
        self.current_step = 0
        
        # Record start time
        simulation_start = time.time()
        
        try:
            # Run simulation
            for step in range(num_steps):
                if self.should_stop:
                    self.logger.info("Simulation stopped by user request")
                    break
                
                # Measure performance before step
                process = psutil.Process()
                memory_before = process.memory_info().rss / 1024 / 1024  # MB
                cpu_before = process.cpu_percent()
                
                step_start = time.time()
                
                # Execute simulation step
                self.brain.step(step)
                
                step_time = time.time() - step_start
                
                # Record metrics
                self.performance_monitor['simulation_times'].append(step_time)
                self.performance_monitor['memory_samples'].append(
                    process.memory_info().rss / 1024 / 1024 - memory_before
                )
                self.performance_monitor['cpu_samples'].append(
                    (cpu_before + process.cpu_percent()) / 2
                )
                self.performance_monitor['learning_rates'].append(
                    self.brain.getTotalSpikeCount() / (step + 1)
                )
                
                self.current_step = step
                
                # Log progress periodically
                if step % 100 == 0:
                    self.logger.info(
                        f"Step {step}/{num_steps}: "
                        f"Time={step_time:.3f}s, "
                        f"Memory={self.performance_monitor['memory_samples'][-1]:.1f}MB, "
                        f"Learning Rate={self.performance_monitor['learning_rates'][-1]:.1f} spikes/s"
                    )
                
                # Check for errors
                if self.check_for_errors():
                    self.logger.error("Error detected, simulation stopped")
                    break
        
        except Exception as e:
            self.logger.error(f"Simulation error: {str(e)}")
            self.error_handler.handle_error(e) if self.error_handler else None
            raise
        
        finally:
            simulation_end = time.time()
            total_time = simulation_end - simulation_start
            
            self.is_running = False
            
            # Calculate final metrics
            final_metrics = self.calculate_performance_metrics()
            
            self.logger.info(
                f"Simulation completed: {self.current_step + 1} steps in {total_time:.2f}s"
            )
            self.logger.info(f"Performance metrics: {final_metrics}")
            
            # Save results
            self.save_simulation_results(final_metrics)
            
            return final_metrics
    
    def check_for_errors(self):
        """Check for simulation errors"""
        # Check for memory issues
        process = psutil.Process()
        memory_usage = process.memory_info().rss / 1024 / 1024  # MB
        
        if memory_usage > 2000:  # 2GB limit
            self.logger.warning(f"High memory usage: {memory_usage:.1f} MB")
            return True
        
        # Check brain health
        try:
            neuron_count = self.brain.getTotalNeuronCount()
            synapse_count = self.brain.getTotalSynapseCount()
            
            if neuron_count <= 0 or synapse_count < neuron_count * 0.1:
                self.logger.error(f"Brain integrity compromised: {neuron_count} neurons, {synapse_count} synapses")
                return True
            
        except Exception as e:
            self.logger.error(f"Brain health check failed: {str(e)}")
            return True
        
        return False
    
    def calculate_performance_metrics(self):
        """Calculate performance metrics"""
        if not self.performance_monitor['simulation_times']:
            return {}
        
        simulation_times = self.performance_monitor['simulation_times']
        memory_usage = self.performance_monitor['memory_samples']
        cpu_usage = self.performance_monitor['cpu_samples']
        learning_rates = self.performance_monitor['learning_rates']
        
        metrics = {
            'total_steps': self.current_step + 1,
            'total_time': sum(simulation_times),
            'average_step_time': np.mean(simulation_times),
            'std_step_time': np.std(simulation_times),
            'min_step_time': np.min(simulation_times),
            'max_step_time': np.max(simulation_times),
            'average_memory_increase': np.mean(memory_usage) if memory_usage else 0,
            'std_memory_increase': np.std(memory_usage) if memory_usage else 0,
            'average_cpu_usage': np.mean(cpu_usage) if cpu_usage else 0,
            'average_learning_rate': np.mean(learning_rates) if learning_rates else 0,
            'final_total_spikes': self.brain.getTotalSpikeCount(),
            'final_developmental_stage': self.brain.getDevelopmentalStage(),
            'simulation_efficiency': self.calculate_simulation_efficiency()
        }
        
        return metrics
    
    def calculate_simulation_efficiency(self):
        """Calculate simulation efficiency score"""
        if not self.performance_monitor['simulation_times']:
            return 0.0
        
        # Efficiency = (total learning output) / (total time * memory cost)
        total_learning = self.brain.getTotalSpikeCount()
        total_time = sum(self.performance_monitor['simulation_times'])
        total_memory = sum(self.performance_monitor['memory_samples'])
        
        if total_time <= 0 or total_memory <= 0:
            return 0.0
        
        return total_learning / (total_time * total_memory)
    
    def save_simulation_results(self, metrics):
        """Save simulation results"""
        import json
        from datetime import datetime
        
        results = {
            'timestamp': datetime.now().isoformat(),
            'simulation_settings': self.simulation_settings,
            'config': self.config.getOr({}, 'config'),
            'metrics': metrics,
            'performance_data': {
                'simulation_times': self.performance_monitor['simulation_times'],
                'memory_usage': self.performance_monitor['memory_samples'],
                'cpu_usage': self.performance_monitor['cpu_samples'],
                'learning_rates': self.performance_monitor['learning_rates']
            }
        }
        
        # Save to file
        filename = f"simulation_results_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json"
        with open(filename, 'w') as f:
            json.dump(results, f, indent=2, default=str)
        
        self.logger.info(f"Simulation results saved to {filename}")
    
    def get_current_status(self):
        """Get current simulation status"""
        return {
            'is_running': self.is_running,
            'current_step': self.current_step,
            'total_steps': self.simulation_settings.get('num_steps', 0),
            'progress_percentage': (self.current_step / self.simulation_settings.get('num_steps', 1)) * 100 if self.simulation_settings.get('num_steps', 0) > 0 else 0,
            'brain_state': {
                'total_neurons': self.brain.getTotalNeuronCount(),
                'total_synapses': self.brain.getTotalSynapseCount(),
                'total_spikes': self.brain.getTotalSpikeCount(),
                'developmental_stage': self.brain.getDevelopmentalStage()
            }
        }

# Usage example
if __name__ == "__main__":
    # Configure production simulation
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 2000)
    config.set("enable_structural_plasticity", True)
    config.set("enable_curiosity", True)
    config.set("enable_development", True)
    
    simulation_settings = {
        'num_steps': 10000,
        'log_level': 'INFO',
        'checkpoint_interval': 1000,
        'max_memory_usage': 4000  # MB
    }
    
    # Create and run production simulator
    simulator = ProductionNLMSimulator(config, simulation_settings)
    
    try:
        # Run simulation
        results = simulator.start_simulation(simulation_settings['num_steps'])
        
        # Print final results
        print("\n=== Simulation Results ===")
        print(f"Total steps completed: {results['total_steps']}")
        print(f"Total time: {results['total_time']:.2f}s")
        print(f"Average step time: {results['average_step_time']:.3f}s")
        print(f"Final developmental stage: {results['final_developmental_stage']}")
        print(f"Simulation efficiency: {results['simulation_efficiency']:.6f}")
        
    except KeyboardInterrupt:
        print("\nSimulation interrupted by user")
    except Exception as e:
        print(f"\nSimulation failed with error: {str(e)}")
        import traceback
        traceback.print_exc()
```

## Advanced Debugging Example

```python
import pynlm
import numpy as np
from IPython.display import display
import pandas as pd

class AdvancedBrainDebugger:
    """Advanced debugging tools for brain simulations"""
    
    def __init__(self, brain):
        self.brain = brain
        self.debug_history = []
        self.performance_profiler = None
        self.memory_profiler = None
        
        # Setup advanced profiling
        self.setup_profiling()
    
    def setup_profiling(self):
        """Setup advanced profiling tools"""
        import time
        import cProfile
        import pstats
        
        self.performance_profiler = {
            'profiler': cProfile.Profile(),
            'stats': None,
            'start_time': None,
            'end_time': None
        }
        
        self.memory_profiler = {
            'memory_samples': [],
            'peak_memory': 0,
            'start_memory': 0
        }
    
    def profile_simulation_step(self, step, func):
        """Profile a single simulation step"""
        import time
        import cProfile
        import pstats
        import io
        
        # Start memory tracking
        process = psutil.Process()
        self.memory_profiler['start_memory'] = process.memory_info().rss / 1024 / 1024  # MB
        
        # Start time tracking
        step_start = time.time()
        
        # Profile execution
        prof = cProfile.Profile()
        prof.enable()
        
        # Execute function
        result = func(step)
        
        # Stop profiling
        prof.disable()
        step_time = time.time() - step_start
        
        # Stop memory tracking
        self.memory_profiler['peak_memory'] = max(
            self.memory_profiler['peak_memory'],
            process.memory_info().rss / 1024 / 1024 - self.memory_profiler['start_memory']
        )
        self.memory_profiler['memory_samples'].append(
            process.memory_info().rss / 1024 / 1024 - self.memory_profiler['start_memory']
        )
        
        # Store profile data
        self.performance_profiler['profiler'] = prof
        self.performance_profiler['start_time'] = step_start
        self.performance_profiler['end_time'] = time.time()
        
        # Generate stats
        s = io.StringIO()
        ps = pstats.Stats(prof, stream=s).sort_stats('cumulative')
        ps.print_stats(20)  # Top 20 functions
        self.performance_profiler['stats'] = s.getvalue()
        
        # Log step information
        self.log_step_debug(step, step_time, result)
        
        return result
    
    def log_step_debug(self, step, step_time, result):
        """Log detailed step information"""
        step_debug = {
            'step': step,
            'timestamp': time.time(),
            'step_time': step_time,
            'brain_state': {
                'total_neurons': self.brain.getTotalNeuronCount(),
                'total_synapses': self.brain.getTotalSynapseCount(),
                'total_spikes': self.brain.getTotalSpikeCount(),
                'active_neurons': self.brain.getActiveNeuronCount(),
                'firing_neurons': self.brain.getFiringNeuronCount(),
                'average_firing_rate': self.brain.getAverageFiringRate(),
                'developmental_stage': self.brain.getDevelopmentalStage()
            },
            'result': result,
            'performance_metrics': {
                'steps_per_second': 1.0 / step_time if step_time > 0 else 0,
                'memory_usage': self.memory_profiler['peak_memory'],
                'cpu_usage': psutil.cpu_percent()
            }
        }
        
        self.debug_history.append(step_debug)
        
        # Log at appropriate levels
        if step_time > 0.1:  # Slow step
            self.log_debug(f"Slow step {step}: {step_time:.3f}s", "WARNING")
        elif step_time < 0.001:  # Very fast step
            self.log_debug(f"Fast step {step}: {step_time:.3f}s", "DEBUG")
        else:
            self.log_debug(f"Step {step} completed: {step_time:.3f}s", "INFO")
    
    def generate_detailed_report(self):
        """Generate detailed analysis report"""
        if not self.debug_history:
            return None
        
        report = {
            'summary': self.generate_summary(),
            'step_analysis': self.analyze_steps(),
            'performance_analysis': self.analyze_performance(),
            'memory_analysis': self.analyze_memory_usage(),
            'bottleneck_analysis': self.identify_bottlenecks(),
            'optimization_recommendations': self.generate_optimization_recommendations()
        }
        
        return report
    
    def generate_summary(self):
        """Generate summary of debugging session"""
        if not self.debug_history:
            return {}
        
        steps = len(self.debug_history)
        time_span = self.debug_history[-1]['timestamp'] - self.debug_history[0]['timestamp']
        
        return {
            'total_steps': steps,
            'time_span': time_span,
            'average_step_time': np.mean([d['step_time'] for d in self.debug_history]),
            'min_step_time': np.min([d['step_time'] for d in self.debug_history]),
            'max_step_time': np.max([d['step_time'] for d in self.debug_history]),
            'final_total_spikes': self.debug_history[-1]['brain_state']['total_spikes'],
            'final_developmental_stage': self.debug_history[-1]['brain_state']['developmental_stage'],
            'total_memory_usage': sum(d['performance_metrics']['memory_usage'] for d in self.debug_history),
            'peak_memory_usage': max(d['performance_metrics']['memory_usage'] for d in self.debug_history)
        }
    
    def analyze_steps(self):
        """Analyze step-by-step performance"""
        if not self.debug_history:
            return {}
        
        steps = [d['step'] for d in self.debug_history]
        step_times = [d['step_time'] for d in self.debug_history]
        spike_counts = [d['brain_state']['total_spikes'] for d in self.debug_history]
        
        # Calculate rates of change
        spike_rate = np.diff(spike_counts)
        step_time_change = np.diff(step_times)
        
        return {
            'step_times': step_times,
            'spike_counts': spike_counts,
            'spike_rate': spike_rate,
            'step_time_change': step_time_change,
            'spike_rate_mean': np.mean(spike_rate),
            'spike_rate_std': np.std(spike_rate),
            'step_time_mean': np.mean(step_times),
            'step_time_std': np.std(step_times)
        }
    
    def analyze_performance(self):
        """Analyze overall performance"""
        if not self.debug_history:
            return {}
        
        performance_metrics = [d['performance_metrics'] for d in self.debug_history]
        
        return {
            'memory_usage': [m['memory_usage'] for m in performance_metrics],
            'cpu_usage': [m['cpu_usage'] for m in performance_metrics],
            'steps_per_second': [1.0 / d['step_time'] for d in self.debug_history],
            'memory_efficiency': self.calculate_memory_efficiency(performance_metrics),
            'cpu_efficiency': self.calculate_cpu_efficiency(performance_metrics)
        }
    
    def analyze_memory_usage(self):
        """Analyze memory usage patterns"""
        memory_usage = [d['performance_metrics']['memory_usage'] for d in self.debug_history]
        
        if not memory_usage:
            return {}
        
        return {
            'total_memory_increase': sum(memory_usage),
            'peak_memory_increase': max(memory_usage),
            'memory_growth_rate': np.polyfit(range(len(memory_usage)), memory_usage, 1)[0],
            'memory_stability': 1.0 / (1.0 + np.std(memory_usage))
        }
    
    def identify_bottlenecks(self):
        """Identify performance bottlenecks"""
        bottlenecks = []
        
        # Check for slow steps
        slow_steps = [(d['step'], d['step_time']) for d in self.debug_history 
                     if d['step_time'] > 0.1]
        if slow_steps:
            bottlenecks.extend([
                f"Slow steps: {[s[1]:.3f}s for step {s[0]}]"
            ])
        
        # Check for memory growth
        memory_growth_rate = np.polyfit(range(len(self.debug_history)), 
                                       [d['performance_metrics']['memory_usage'] for d in self.debug_history], 
                                       1)[0]
        if memory_growth_rate > 0.1:  # 10% memory growth per step
            bottlenecks.append(f"High memory growth rate: {memory_growth_rate:.3f} MB/step")
        
        # Check for spike rate fluctuations
        spike_rates = [d['performance_metrics']['cpu_usage'] for d in self.debug_history]
        if np.std(spike_rates) / np.mean(spike_rates) > 0.5:  # 50% variation
            bottlenecks.append("High CPU usage variation indicating uneven computation")
        
        return bottlenecks
    
    def generate_optimization_recommendations(self):
        """Generate optimization recommendations"""
        recommendations = []
        
        # Get analysis data
        step_analysis = self.analyze_steps()
        memory_analysis = self.analyze_memory_usage()
        
        # Check for slow steps
        if 'step_times' in step_analysis:
            slow_step_percentage = len([t for t in step_analysis['step_times'] if t > 0.1]) / len(step_analysis['step_times'])
            if slow_step_percentage > 0.1:
                recommendations.append("Consider optimizing slow steps (>100ms)")
        
        # Check memory usage
        if memory_analysis:
            if memory_analysis.get('memory_growth_rate', 0) > 0.05:
                recommendations.append("Implement memory optimization strategies")
            if memory_analysis.get('memory_stability', 1.0) < 0.8:
                recommendations.append("Add memory pooling and reuse strategies")
        
        # Check for performance bottlenecks
        bottlenecks = self.identify_bottlenecks()
        if bottlenecks:
            recommendations.extend([f"Address bottleneck: {b}" for b in bottlenecks])
        
        if not recommendations:
            recommendations.append("Performance is optimal")
        
        return recommendations
    
    def calculate_memory_efficiency(self, performance_metrics):
        """Calculate memory efficiency"""
        memory_usage = [m['memory_usage'] for m in performance_metrics]
        if not memory_usage or sum(memory_usage) == 0:
            return 0.0
        
        return 1.0 / (1.0 + np.mean(memory_usage))
    
    def calculate_cpu_efficiency(self, performance_metrics):
        """Calculate CPU efficiency"""
        cpu_usage = [m['cpu_usage'] for m in performance_metrics]
        if not cpu_usage:
            return 0.0
        
        # Efficiency = (useful work) / (CPU time)
        return 1.0 / (1.0 + np.mean(cpu_usage))
    
    def save_debug_data(self, filename):
        """Save debug data to file"""
        import json
        
        debug_data = {
            'debug_history': self.debug_history,
            'performance_profiler': self.performance_profiler,
            'memory_profiler': self.memory_profiler
        }
        
        with open(filename, 'w') as f:
            json.dump(debug_data, f, indent=2, default=str)
        
        print(f"Debug data saved to {filename}")
    
    def plot_debug_analysis(self):
        """Plot debug analysis results"""
        import matplotlib.pyplot as plt
        import seaborn as sns
        
        if not self.debug_history:
            print("No debug data available")
            return
        
        fig, axes = plt.subplots(2, 3, figsize=(18, 12))
        fig.suptitle('Brain Simulation Debug Analysis', fontsize=16)
        
        # Step times over time
        steps = [d['step'] for d in self.debug_history]
        step_times = [d['step_time'] for d in self.debug_history]
        
        axes[0, 0].plot(steps, step_times, alpha=0.7)
        axes[0, 0].set_title('Step Times Over Time')
        axes[0, 0].set_xlabel('Step')
        axes[0, 0].set_ylabel('Step Time (s)')
        axes[0, 0].axhline(y=0.1, color='r', linestyle='--', alpha=0.5)
        axes[0, 0].set_yscale('log')
        
        # Memory usage over time
        memory_usage = [d['performance_metrics']['memory_usage'] for d in self.debug_history]
        axes[0, 1].plot(steps, memory_usage, color='g', alpha=0.7)
        axes[0, 1].set_title('Memory Usage Over Time')
        axes[0, 1].set_xlabel('Step')
        axes[0, 1].set_ylabel('Memory Usage (MB)')
        
        # Spike count over time
        spike_counts = [d['brain_state']['total_spikes'] for d in self.debug_history]
        axes[0, 2].plot(steps, spike_counts, color='b', alpha=0.7)
        axes[0, 2].set_title('Total Spike Count Over Time')
        axes[0, 2].set_xlabel('Step')
        axes[0, 2].set_ylabel('Total Spikes')
        
        # Performance metrics heatmap
        performance_matrix = np.column_stack([
            step_times,
            [d['performance_metrics']['memory_usage'] for d in self.debug_history],
            [d['performance_metrics']['cpu_usage'] for d in self.debug_history],
            [d['brain_state']['average_firing_rate'] for d in self.debug_history]
        ])
        
        sns.heatmap(performance_matrix.T, ax=axes[1, 0], cmap='RdYlBu_r', 
                   cbar_kws={'label': 'Normalized Value'})
        axes[1, 0].set_title('Performance Metrics Heatmap')
        axes[1, 0].set_xticklabels(['Time', 'Memory', 'CPU', 'Firing Rate'], rotation=45)
        
        # Step time distribution
        axes[1, 1].hist(step_times, bins=50, alpha=0.7, density=True)
        axes[1, 1].set_title('Step Time Distribution')
        axes[1, 1].set_xlabel('Step Time (s)')
        axes[1, 1].set_ylabel('Density')
        
        # Memory usage distribution
        axes[1, 2].hist(memory_usage, bins=50, alpha=0.7, density=True)
        axes[1, 2].set_title('Memory Usage Distribution')
        axes[1, 2].set_xlabel('Memory Usage (MB)')
        axes[1, 2].set_ylabel('Density')
        
        plt.tight_layout()
        plt.show()
    
    def run_comprehensive_debugging(self, num_steps):
        """Run comprehensive debugging session"""
        print(f"Starting comprehensive debugging session with {num_steps} steps")
        
        # Start profiling
        self.performance_profiler['profiler'].enable()
        self.performance_profiler['start_time'] = time.time()
        
        try:
            for step in range(num_steps):
                if step % 100 == 0:
                    print(f"Step {step}/{num_steps}")
                
                # Profile step execution
                def execute_step(s):
                    return self.brain.step(s)
                
                self.profile_simulation_step(step, execute_step)
            
        finally:
            # Stop profiling
            self.performance_profiler['profiler'].disable()
            self.performance_profiler['end_time'] = time.time()
        
        # Generate comprehensive report
        report = self.generate_detailed_report()
        
        # Display report
        self.display_report(report)
        
        # Plot analysis
        self.plot_debug_analysis()
        
        return report
    
    def display_report(self, report):
        """Display debugging report"""
        if not report:
            print("No report generated")
            return
        
        print("\n" + "="*80)
        print("COMPREHENSIVE DEBUGGING REPORT")
        print("="*80)
        
        print(f"\nSimulation Summary:")
        print(f"  Total Steps: {report['summary']['total_steps']}")
        print(f"  Time Span: {report['summary']['time_span']:.2f} seconds")
        print(f"  Average Step Time: {report['summary']['average_step_time']:.4f} seconds")
        print(f"  Final Developmental Stage: {report['summary']['final_developmental_stage']}")
        print(f"  Total Memory Usage: {report['summary']['total_memory_usage']:.1f} MB")
        print(f"  Peak Memory Usage: {report['summary']['peak_memory_usage']:.1f} MB")
        
        print(f"\nPerformance Analysis:")
        perf = report['performance_analysis']
        print(f"  Average Steps per Second: {np.mean(perf['steps_per_second']):.1f}")
        print(f"  Memory Efficiency: {perf.get('memory_efficiency', 0):.3f}")
        print(f"  CPU Efficiency: {perf.get('cpu_efficiency', 0):.3f}")
        
        print(f"\nMemory Usage Analysis:")
        mem = report['memory_analysis']
        print(f"  Total Memory Increase: {mem.get('total_memory_increase', 0):.1f} MB")
        print(f"  Memory Growth Rate: {mem.get('memory_growth_rate', 0):.3f} MB/step")
        print(f"  Memory Stability: {mem.get('memory_stability', 0):.3f}")
        
        print(f"\nBottlenecks Identified:")
        for bottleneck in report['bottleneck_analysis']:
            print(f"  - {bottleneck}")
        
        print(f"\nOptimization Recommendations:")
        for recommendation in report['optimization_recommendations']:
            print(f"  - {recommendation}")
        
        print("\n" + "="*80)

# Usage example
if __name__ == "__main__":
    # Create brain for debugging
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 1000)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create advanced debugger
    debugger = AdvancedBrainDebugger(brain)
    
    # Run comprehensive debugging session
    report = debugger.run_comprehensive_debugging(500)
    
    # Save debug data
    debugger.save_debug_data("advanced_debug_data.json")
    
    print("\nAdvanced debugging session completed!")
    print(f"Debug data saved to advanced_debug_data.json")
```

This comprehensive example demonstrates advanced debugging and validation techniques for NLM simulations, including performance profiling, memory analysis, bottleneck identification, and optimization recommendations.