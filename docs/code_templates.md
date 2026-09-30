# NLM Code Templates Guide

This document provides reusable code templates for NLM development, covering brain configuration, agent-world integration, experiment frameworks, performance monitoring, and debugging techniques.

## Table of Contents

1. [Brain Configuration Templates](#brain-configuration-templates)
2. [Agent-World Integration Templates](#agent-world-integration-templates)
3. [Experiment Framework Templates](#experiment-framework-templates)
4. [Performance Monitoring Templates](#performance-monitoring-templates)
5. [Debugging and Validation Templates](#debugging-and-validation-templates)

---

## 1. Brain Configuration Templates

### Template 1: Basic Brain Configuration

```python
# basic_brain_config.py
import pynlm

def create_basic_brain_config(neuron_count=1000, synapse_density=0.05):
    """Create basic brain configuration for NLM simulation."""
    
    config = pynlm.createDefaultConfig()
    
    # Core brain parameters
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.synapse_density", synapse_density)
    config.set("brain.v_thresh", -40.0)  # Threshold potential
    config.set("brain.tau_mem", 10.0)   # Membrane time constant
    config.set("brain.tau_ref", 2.0)    # Refractory period
    config.set("brain.c_m", 1.0)        # Membrane capacitance
    
    # Neural dynamics
    config.set("neural.dynamics.type", "LIF")  # Leaky Integrate-and-Fire
    config.set("neural.dynamics.reset_v", -65.0)
    config.set("neural.dynamics.spike_threshold", -40.0)
    
    # Synaptic properties
    config.set("synapse.excitation.weight_mean", 0.5)
    config.set("synapse.excitation.weight_std", 0.1)
    config.set("synapse.inhibition.weight_mean", -1.0)
    config.set("synapse.inhibition.weight_std", 0.2)
    
    # Network topology
    config.set("network.topology.type", "random")
    config.set("network.topology.sparse", True)
    config.set("network.topology.connectance", 0.1)
    
    return config

# Usage example
config = create_basic_brain_config(neuron_count=2000, synapse_density=0.08)
brain = pynlm.createBrain(config)
brain.initialize()
```

### Template 2: Advanced Learning Brain Configuration

```python
# advanced_learning_config.py
import pynlm
import numpy as np

def create_advanced_learning_config(neuron_count=5000):
    """Create advanced brain configuration with learning capabilities."""
    
    config = pynlm.createDefaultConfig()
    
    # Core brain parameters
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.synapse_density", 0.06)
    config.set("brain.v_thresh", -45.0)
    config.set("brain.tau_mem", 12.0)
    config.set("brain.tau_ref", 3.0)
    
    # Learning system configuration
    config.set("learning.enabled", True)
    config.set("learning.stdp.enable", True)
    config.set("learning.stdp.a_plus", 0.01)
    config.set("learning.stdp.a_minus", 0.012)
    config.set("learning.stdp.tau_plus", 20.0)
    config.set("learning.stdp.tau_minus", 20.0)
    
    config.set("learning.hebbian.enable", True)
    config.set("learning.hebbian.learning_rate", 0.001)
    
    # Neuromodulation configuration
    config.set("neuromod.dopamine.enable", True)
    config.set("neuromod.dopamine.baseline", 0.1)
    config.set("neuromod.dopamine.learning_rate", 0.05)
    
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.curiosity.sensitivity", 0.8)
    
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.novelty.threshold", 0.3)
    
    # Development system
    config.set("development.enabled", True)
    config.set("development.critical_period", 100.0)
    config.set("development.synaptogenesis_rate", 0.01)
    config.set("development.pruning_rate", 0.005)
    
    # Memory systems
    config.set("memory.working_memory.enabled", True)
    config.set("memory.working_memory.capacity", 1000)
    
    config.set("memory.episodic.enabled", True)
    config.set("memory.episodic.capacity", 5000)
    
    config.set("memory.associative.enabled", True)
    config.set("memory.associative.similarity_threshold", 0.7)
    
    # Prediction system
    config.set("prediction.enabled", True)
    config.set("prediction.horizon", 10)
    config.set("prediction.learning_rate", 0.03)
    
    # Cognitive functions
    config.set("cognition.attention.enabled", True)
    config.set("cognition.attention.competition_strength", 1.0)
    
    config.set("cognition.planning.enabled", True)
    config.set("cognition.planning.horizon", 5)
    
    config.set("cognition.memory.enabled", True)
    config.set("cognition.memory.consolidation_rate", 0.1)
    
    return config

# Usage example
config = create_advanced_learning_config(neuron_count=10000)
brain = pynlm.createBrain(config)
brain.initialize()

# Enable learning features
agent = pynlm.createAgentBrain(brain)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)
```

### Template 3: Performance-Optimized Configuration

```python
# performance_config.py
import pynlm

def create_performance_config(neuron_count=20000):
    """Create performance-optimized brain configuration."""
    
    config = pynlm.createDefaultConfig()
    
    # Optimized for performance
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.synapse_density", 0.02)  # Lower density for performance
    config.set("brain.v_thresh", -50.0)       # Higher threshold for stability
    config.set("brain.tau_mem", 15.0)        # Longer time constant
    config.set("brain.tau_ref", 5.0)         # Longer refractory period
    
    # Simplified learning for performance
    config.set("learning.stdp.enable", False)  # Disable STDP for performance
    config.set("learning.hebbian.enable", False)
    
    # Basic neuromodulation
    config.set("neuromod.dopamine.enable", True)
    config.set("neuromod.dopamine.baseline", 0.1)
    
    config.set("neuromod.curiosity.enable", False)  # Disable curiosity
    
    # Minimal memory systems
    config.set("memory.working_memory.enabled", True)
    config.set("memory.working_memory.capacity", 500)
    
    config.set("memory.episodic.enabled", False)
    config.set("memory.associative.enabled", False)
    
    # Simple prediction
    config.set("prediction.enabled", False)
    
    return config

# Usage example
config = create_performance_config(neuron_count=50000)
brain = pynlm.createBrain(config)
brain.initialize()
```

### Template 4: Developmental Psychology Configuration

```python
# developmental_config.py
import pynlm
import numpy as np

def create_developmental_config(neuron_count=8000):
    """Create brain configuration for developmental psychology research."""
    
    config = pynlm.createDefaultConfig()
    
    # Core parameters
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.synapse_density", 0.07)
    config.set("brain.v_thresh", -42.0)
    config.set("brain.tau_mem", 8.0)
    config.set("brain.tau_ref", 2.5)
    
    # Enhanced developmental systems
    config.set("development.enabled", True)
    config.set("development.critical_period_duration", 200.0)
    config.set("development.synaptogenesis_rate", 0.015)
    config.set("development.pruning_rate", 0.008)
    config.set("development.maturation_rate", 0.001)
    
    # Sensitive periods for different functions
    config.set("development.sensory_sensitive_period", 100.0)
    config.set("development.motor_sensitive_period", 150.0)
    config.set("development.social_sensitive_period", 300.0)
    config.set("development.cognitive_sensitive_period", 500.0)
    
    # Learning systems with developmental modulation
    config.set("learning.stdp.enable", True)
    config.set("learning.stdp.a_plus", 0.015)
    config.set("learning.stdp.a_minus", 0.018)
    
    config.set("learning.hebbian.enable", True)
    config.set("learning.hebbian.learning_rate", 0.002)
    
    # Neuromodulation with developmental changes
    config.set("neuromod.dopamine.enable", True)
    config.set("neuromod.dopamine.development_modulation", True)
    config.set("neuromod.dopamine.baseline_development", 0.05)
    config.set("neuromod.dopamine.baseline_adult", 0.15)
    
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.curiosity.development_modulation", True)
    config.set("neuromod.curiosity.baseline_juvenile", 0.2)
    config.set("neuromod.curiosity.baseline_adult", 0.1)
    
    # Advanced memory systems
    config.set("memory.working_memory.enabled", True)
    config.set("memory.working_memory.capacity", 2000)
    config.set("memory.working_memory.development_modulation", True)
    
    config.set("memory.episodic.enabled", True)
    config.set("memory.episodic.capacity", 10000)
    config.set("memory.episodic.consolidation_rate", 0.2)
    
    config.set("memory.associative.enabled", True)
    config.set("memory.associative.similarity_threshold", 0.8)
    config.set("memory.associative.development_modulation", True)
    
    # Prediction with developmental changes
    config.set("prediction.enabled", True)
    config.set("prediction.horizon", 15)
    config.set("prediction.development_modulation", True)
    config.set("prediction.initial_accuracy", 0.3)
    config.set("prediction.final_accuracy", 0.8)
    
    # Cognitive development
    config.set("cognition.attention.enabled", True)
    config.set("cognition.attention.development_modulation", True)
    config.set("cognition.attention.selectivity_juvenile", 0.6)
    config.set("cognition.attention.selectivity_adult", 0.9)
    
    config.set("cognition.planning.enabled", True)
    config.set("cognition.planning.horizon", 10)
    config.set("cognition.planning.development_modulation", True)
    
    config.set("cognition.memory.enabled", True)
    config.set("cognition.memory.consolidation_rate", 0.15)
    
    return config

# Usage example
config = create_developmental_config(neuron_count=12000)
brain = pynlm.createBrain(config)
brain.initialize()
```

## 2. Agent-World Integration Templates

### Template 1: Basic Agent-World Setup

```python
# basic_agent_world.py
import pynlm
import numpy as np

class BasicAgentWorld:
    """Basic agent-world integration template."""
    
    def __init__(self, brain_config=None):
        # Create brain
        if brain_config is None:
            brain_config = pynlm.createDefaultConfig()
        
        self.brain = pynlm.createBrain(brain_config)
        self.brain.initialize()
        
        # Create agent
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Create world
        self.world = pynlm.createSimpleWorld()
        
        # Initialize agent with world
        self.agent.initialize(self.world)
        
        # Simulation state
        self.step_count = 0
        self.episode_count = 0
        self.total_reward = 0.0
        
    def run_episode(self, max_steps=1000):
        """Run a single episode."""
        print(f"Starting Episode {self.episode_count + 1}")
        
        # Reset world and agent
        self.world.reset()
        self.agent.reset()
        self.total_reward = 0.0
        
        # Episode loop
        for step in range(max_steps):
            # 1. Update world
            self.world.update(0.1)
            
            # 2. Get sensory input
            percept = self.world.getSensoryPercept()
            
            # 3. Process sensory input
            self.agent.processSensoryInput(percept)
            
            # 4. Brain computation
            self.brain.step(step)
            
            # 5. Get action
            action = self.agent.decodeMotorCommand()
            
            # 6. Apply action
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # 7. Calculate reward
            reward = self.calculate_reward(percept, action)
            self.total_reward += reward
            
            # 8. Apply neuromodulation
            self.agent.applyRewardModulation(reward, 0.0)
            
            # 9. Update development
            self.agent.updateDevelopment(0.1)
            
            # Print progress
            if step % 100 == 0:
                self.print_step_status(step, reward)
        
        self.episode_count += 1
        return self.total_reward
    
    def calculate_reward(self, percept, action):
        """Calculate reward for action."""
        # Basic reward calculation
        reward = 0.0
        
        # Reward for successful actions
        if action is not None:
            reward += 0.1
        
        # Reward for world interactions
        if percept is not None:
            reward += np.random.uniform(-0.1, 0.1)
        
        return np.clip(reward, -1.0, 1.0)
    
    def print_step_status(self, step, reward):
        """Print step status."""
        print(f"Step {step:4d}: Reward = {reward:6.2f}, "
              f"Total = {self.total_reward:8.2f}")

# Usage example
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)

agent_world = BasicAgentWorld(config)

# Run multiple episodes
for episode in range(5):
    reward = agent_world.run_episode(max_steps=500)
    print(f"Episode {episode + 1} Reward: {reward:.2f}")
    print("-" * 40)
```

### Template 2: Multi-Agent Coordination

```python
# multi_agent_template.py
import pynlm
import numpy as np

class MultiAgentSystem:
    """Multi-agent coordination template."""
    
    def __init__(self, num_agents=3, shared_world=None):
        self.num_agents = num_agents
        self.agents = []
        self.brains = []
        
        if shared_world is None:
            self.shared_world = pynlm.createSimpleWorld()
            self.shared_world.configure(width=50, height=50, visionWidth=12, visionHeight=12)
            self.shared_world.reset()
        else:
            self.shared_world = shared_world
        
        # Create agents
        for i in range(num_agents):
            # Create agent-specific configuration
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", 2000 + i * 500)
            config.set("brain.synapse_density", 0.06 + i * 0.01)
            config.set("brain.role", f"agent_{i}")
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            agent = pynlm.createAgentBrain(brain)
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            agent.enableDevelopment(True)
            
            self.brains.append(brain)
            self.agents.append(agent)
            
            # Initialize agent with shared world
            agent.initialize(self.shared_world)
        
        # Communication system
        self.communication_system = CommunicationSystem(num_agents)
        
        # Shared resource manager
        self.resource_manager = SharedResourceManager()
        
        # Statistics tracking
        self.statistics = {
            'steps': [],
            'individual_rewards': [],
            'coordination_scores': [],
            'resource_usage': []
        }
        
    def run_coordination_episode(self, num_steps=1000):
        """Run coordination episode."""
        print(f"Starting Multi-Agent Coordination Episode with {self.num_agents} agents")
        
        for step in range(num_steps):
            # Update shared world
            self.shared_world.update(0.1)
            
            # Each agent processes sensory input
            agent_percepts = []
            for i, agent in enumerate(self.agents):
                percept = self.shared_world.getSensoryPercept()
                agent_percepts.append(percept)
                
                # Process sensory input
                agent.processSensoryInput(percept)
                
                # Brain computation
                self.brains[i].step(step)
                
                # Get action
                action = agent.decodeMotorCommand()
                
                # Apply action to shared world
                self.shared_world.applyMotorCommand(action, self.shared_world.getSimulationTime())
                
                # Apply reward
                reward = self.calculate_individual_reward(i, percept, action)
                agent.applyRewardModulation(reward, 0.0)
                
                # Update development
                agent.updateDevelopment(0.1)
            
            # Communication phase
            self.communication_system.update_communications(
                agent_percepts, step
            )
            
            # Resource management
            resource_usage = self.resource_manager.update_resources(
                self.brains, self.shared_world, step
            )
            
            # Collect statistics
            self.collect_coordination_statistics(
                step, agent_percepts, reward, resource_usage
            )
            
            # Print progress
            if step % 100 == 0:
                self.print_coordination_status(step)
        
        return self.statistics
    
    def calculate_individual_reward(self, agent_idx, percept, action):
        """Calculate individual reward for agent."""
        # Base reward
        reward = 0.1
        
        # Reward for action success
        if action is not None:
            reward += 0.1
        
        # Reward for resource acquisition
        if self.resource_manager.has_resource(agent_idx):
            reward += 0.5
        
        # Penalty for conflicts
        conflicts = self.communication_system.get_agent_conflicts(agent_idx)
        reward -= conflicts * 0.3
        
        return np.clip(reward, -1.0, 1.0)
    
    def collect_coordination_statistics(self, step, agent_percepts, reward, resource_usage):
        """Collect coordination statistics."""
        self.statistics['steps'].append(step)
        
        # Individual rewards (simplified)
        individual_rewards = [reward] * self.num_agents
        self.statistics['individual_rewards'].append(individual_rewards)
        
        # Coordination score (simplified)
        coordination_score = np.mean(individual_rewards)
        self.statistics['coordination_scores'].append(coordination_score)
        
        # Resource usage
        self.statistics['resource_usage'].append(resource_usage)
    
    def print_coordination_status(self, step):
        """Print coordination status."""
        if not self.statistics['steps']:
            return
        
        current_step = step
        avg_reward = np.mean(self.statistics['individual_rewards'][current_step - 100:current_step + 1])
        coordination = self.statistics['coordination_scores'][current_step]
        
        print(f"Step {current_step:4d}:")
        print(f"  Average Individual Reward: {avg_reward:6.2f}")
        print(f"  Coordination Score: {coordination:6.2f}")
        print(f"  Resource Usage: {self.statistics['resource_usage'][current_step]:6.2f}")

class CommunicationSystem:
    """Communication system for multi-agent coordination."""
    
    def __init__(self, num_agents):
        self.num_agents = num_agents
        self.communication_channels = []
        self.agent_messages = [[] for _ in range(num_agents)]
        
    def update_communications(self, agent_percepts, step):
        """Update agent communications."""
        # Simple broadcasting model
        for i in range(self.num_agents):
            # Broadcast agent's percept to all agents
            for j in range(self.num_agents):
                if i != j:
                    # Add message to receiving agent's message queue
                    message = {
                        'sender': i,
                        'percept': agent_percepts[i],
                        'step': step,
                        'importance': self.calculate_message_importance(i)
                    }
                    
                    self.agent_messages[j].append(message)
            
            # Process received messages
            self.process_received_messages(i, step)
    
    def calculate_message_importance(self, sender_idx):
        """Calculate message importance based on sender."""
        # Higher importance for agents with higher performance
        return 1.0 + sender_idx * 0.1
    
    def process_received_messages(self, agent_idx, step):
        """Process messages received by agent."""
        messages = self.agent_messages[agent_idx]
        
        # Process recent messages
        recent_messages = [msg for msg in messages if step - msg['step'] <= 10]
        
        if recent_messages:
            # Average message content
            avg_importance = np.mean([msg['importance'] for msg in recent_messages])
            
            # This would affect agent's decision making
            # Placeholder for actual implementation
            pass
        
        # Remove old messages
        self.agent_messages[agent_idx] = [
            msg for msg in messages if step - msg['step'] <= 100
        ]
    
    def get_agent_conflicts(self, agent_idx):
        """Get number of conflicts for agent."""
        conflicts = 0
        
        # Simple conflict detection based on message overlaps
        for message in self.agent_messages[agent_idx]:
            if message['importance'] > 1.5:
                conflicts += 1
        
        return conflicts

class SharedResourceManager:
    """Manager for shared resources in multi-agent system."""
    
    def __init__(self):
        self.resources = {
            'energy': 100.0,
            'information': 50.0,
            'space': 1000.0,  # Area units
            'time': 1000.0   # Time units
        }
        self.agent_resources = {}
        
    def update_resources(self, brains, world, step):
        """Update shared resource state."""
        total_usage = 0.0
        
        # Calculate resource usage by each agent
        for i, brain in enumerate(brains):
            usage = self.calculate_agent_resource_usage(brain, step)
            self.agent_resources[i] = usage
            total_usage += usage
        
        # Update shared resources
        for resource_name in self.resources:
            if resource_name == 'energy':
                # Energy regenerates slowly
                self.resources[resource_name] += 0.1
                # Subtract usage
                self.resources[resource_name] -= total_usage * 0.5
            elif resource_name == 'information':
                # Information accumulates
                self.resources[resource_name] += 1.0
                # Subtract usage
                self.resources[resource_name] -= total_usage * 0.3
            elif resource_name == 'space':
                # Space consumption from world objects
                space_usage = len(world.getObjects()) * 10.0 if hasattr(world, 'getObjects') else 0
                self.resources[resource_name] -= space_usage
            elif resource_name == 'time':
                # Time passes
                self.resources[resource_name] -= 1.0 * len(brains)
        
        # Keep resources within bounds
        for resource_name in self.resources:
            if resource_name in ['energy', 'information']:
                self.resources[resource_name] = np.clip(self.resources[resource_name], 0, 1000)
            else:
                self.resources[resource_name] = np.clip(self.resources[resource_name], 0, 10000)
        
        return total_usage
    
    def calculate_agent_resource_usage(self, brain, step):
        """Calculate resource usage by agent."""
        # Based on brain activity
        firing_rate = brain.getAverageFiringRate()
        
        # Base usage from neural activity
        usage = firing_rate * 0.1
        
        # Additional usage for learning
        if hasattr(brain, 'getLearningRate'):
            learning_rate = brain.getLearningRate()
            usage += learning_rate * 0.5
        
        # Additional usage for memory
        if hasattr(brain, 'getMemoryUsage'):
            memory_usage = brain.getMemoryUsage()
            usage += memory_usage * 0.3
        
        return usage
    
    def has_resource(self, agent_idx):
        """Check if agent has sufficient resources."""
        if agent_idx not in self.agent_resources:
            return False
        
        return self.agent_resources[agent_idx] < 5.0  # Below threshold

# Usage example
if __name__ == "__main__":
    # Create multi-agent system
    multi_agent = MultiAgentSystem(num_agents=3)
    
    # Run coordination episode
    results = multi_agent.run_coordination_episode(num_steps=500)
    
    print("\n" + "=" * 60)
    print("Multi-Agent Coordination Complete!")
    print("=" * 60)
    
    # Print summary
    avg_coordination = np.mean(results['coordination_scores'])
    total_reward = np.sum([np.sum(rewards) for rewards in results['individual_rewards']])
    
    print(f"Average Coordination Score: {avg_coordination:.3f}")
    print(f"Total Individual Rewards: {total_reward:.2f}")
    print(f"Final Resource Usage: {results['resource_usage'][-1]:.2f}")
```

## 3. Experiment Framework Templates

### Template 1: Basic Experiment Framework

```python
# basic_experiment_framework.py
import pynlm
import numpy as np
import time
import json
from datetime import datetime

class BasicExperiment:
    """Basic experiment framework for NLM research."""
    
    def __init__(self, config, experiment_name="default_experiment"):
        self.config = config
        self.experiment_name = experiment_name
        self.timestamp = datetime.now().isoformat()
        
        # Initialize components
        self.setup_components()
        
        # Experiment state
        self.step_count = 0
        self.episode_count = 0
        self.experiment_start_time = time.time()
        
        # Data collection
        self.experiment_data = {
            'metadata': {
                'name': experiment_name,
                'timestamp': self.timestamp,
                'config': config.summary() if hasattr(config, 'summary') else str(config),
                'start_time': self.experiment_start_time
            },
            'metrics': {
                'steps': [],
                'rewards': [],
                'brain_stats': [],
                'agent_stats': [],
                'environmental_state': [],
                'learning_curves': {}
            },
            'results': {},
            'performance': {}
        }
        
    def setup_components(self):
        """Setup experiment components."""
        # Create brain
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        # Create agent
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Create world
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
        
        # Initialize agent with world
        self.agent.initialize(self.world)
        
        # Setup data collectors
        self.setup_data_collectors()
        
    def setup_data_collectors(self):
        """Setup data collection systems."""
        self.data_collectors = {
            'brain': BrainDataCollector(self.brain),
            'agent': AgentDataCollector(self.agent),
            'world': WorldDataCollector(self.world),
            'environment': EnvironmentDataCollector(self.world)
        }
        
    def run_experiment(self, num_steps=1000, num_episodes=1):
        """Run experiment."""
        print(f"Starting Experiment: {self.experiment_name}")
        print(f"Steps per episode: {num_steps}, Episodes: {num_episodes}")
        
        for episode in range(num_episodes):
            episode_start_time = time.time()
            
            print(f"\nEpisode {episode + 1}/{num_episodes}")
            print("=" * 50)
            
            # Run episode
            episode_results = self.run_episode(num_steps, episode)
            
            episode_time = time.time() - episode_start_time
            print(f"Episode {episode + 1} completed in {episode_time:.2f}s")
            print(f"Episode reward: {episode_results['total_reward']:.2f}")
        
        self.finalize_experiment()
        return self.experiment_data
    
    def run_episode(self, num_steps, episode):
        """Run single episode."""
        # Reset for new episode
        self.world.reset()
        self.agent.reset()
        
        episode_data = {
            'episode': episode,
            'steps': [],
            'total_reward': 0.0,
            'step_times': []
        }
        
        for step in range(num_steps):
            step_start_time = time.time()
            
            # 1. Update world
            self.world.update(0.1)
            
            # 2. Get sensory input
            percept = self.world.getSensoryPercept()
            
            # 3. Process sensory input
            self.agent.processSensoryInput(percept)
            
            # 4. Brain computation
            self.brain.step(step)
            
            # 5. Get action
            action = self.agent.decodeMotorCommand()
            
            # 6. Apply action
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # 7. Calculate reward
            reward = self.calculate_reward(percept, action, step)
            episode_data['total_reward'] += reward
            
            # 8. Apply neuromodulation
            self.agent.applyRewardModulation(reward, 0.0)
            
            # 9. Update development
            self.agent.updateDevelopment(0.1)
            
            # 10. Collect data
            step_data = self.collect_step_data(step, percept, action, reward)
            episode_data['steps'].append(step_data)
            
            # 11. Record step time
            step_time = time.time() - step_start_time
            episode_data['step_times'].append(step_time)
            
            # Print progress
            if step % 100 == 0:
                self.print_step_progress(step, reward)
        
        return episode_data
    
    def calculate_reward(self, percept, action, step):
        """Calculate reward for action."""
        # Basic reward calculation
        reward = 0.0
        
        # Reward for action
        if action is not None:
            reward += 0.1
        
        # Reward based on percept
        if percept is not None:
            # Add some variability based on percept
            reward += np.random.uniform(-0.1, 0.1)
        
        # Step-based reward (encourage completion)
        if step > 0:
            reward += 0.01
        
        return np.clip(reward, -1.0, 1.0)
    
    def collect_step_data(self, step, percept, action, reward):
        """Collect data for current step."""
        step_data = {
            'step': step,
            'reward': reward,
            'action': self.serialize_action(action),
            'brain_stats': self.data_collectors['brain'].collect(step),
            'agent_stats': self.data_collectors['agent'].collect(step),
            'environmental_state': self.data_collectors['environment'].collect(step),
            'world_state': self.data_collectors['world'].collect(step)
        }
        
        # Add to experiment data
        self.experiment_data['metrics']['steps'].append(step)
        self.experiment_data['metrics']['rewards'].append(reward)
        self.experiment_data['metrics']['brain_stats'].append(step_data['brain_stats'])
        self.experiment_data['metrics']['agent_stats'].append(step_data['agent_stats'])
        self.experiment_data['metrics']['environmental_state'].append(step_data['environmental_state'])
        
        return step_data
    
    def serialize_action(self, action):
        """Serialize action for storage."""
        if action is None:
            return None
        
        if hasattr(action, 'getType'):
            return {
                'type': action.getType(),
                'parameters': action.getParameters() if hasattr(action, 'getParameters') else []
            }
        
        return str(action)
    
    def print_step_progress(self, step, reward):
        """Print step progress."""
        print(f"Step {step:4d}: Reward = {reward:6.2f}, "
              f"Total = {self.experiment_data['metrics']['rewards'][-1]:8.2f}")
    
    def finalize_experiment(self):
        """Finalize experiment and analyze results."""
        experiment_time = time.time() - self.experiment_start_time
        
        # Calculate performance metrics
        self.calculate_performance_metrics()
        
        # Analyze learning curves
        self.analyze_learning_curves()
        
        # Store results
        self.experiment_data['metadata']['end_time'] = time.time()
        self.experiment_data['metadata']['total_duration'] = experiment_time
        self.experiment_data['metadata']['total_steps'] = sum(
            len(episode['steps']) for episode in self.experiment_data.get('episodes', [])
        )
        
        # Save experiment data
        self.save_experiment_data()
        
        print(f"\nExperiment Complete!")
        print(f"Total Duration: {experiment_time:.2f}s")
        print(f"Total Steps: {self.experiment_data['metadata']['total_steps']}")
    
    def calculate_performance_metrics(self):
        """Calculate performance metrics."""
        rewards = self.experiment_data['metrics']['rewards']
        
        if not rewards:
            return
        
        self.experiment_data['performance'] = {
            'mean_reward': np.mean(rewards),
            'std_reward': np.std(rewards),
            'min_reward': np.min(rewards),
            'max_reward': np.max(rewards),
            'median_reward': np.median(rewards),
            'reward_variance': np.var(rewards),
            'convergence_step': self.find_convergence_step(rewards),
            'stability_score': self.calculate_stability_score(rewards)
        }
        
    def find_convergence_step(self, rewards, window=100, threshold=0.01):
        """Find step where rewards converge."""
        if len(rewards) < window * 2:
            return len(rewards) - 1
        
        for i in range(window, len(rewards)):
            recent_mean = np.mean(rewards[i-window:i])
            overall_mean = np.mean(rewards[:i])
            
            if abs(recent_mean - overall_mean) < threshold:
                return i
        
        return len(rewards) - 1
    
    def calculate_stability_score(self, rewards, window=50):
        """Calculate stability score."""
        if len(rewards) < window * 2:
            return 0.0
        
        # Calculate reward stability (lower variance in recent steps)
        recent_rewards = rewards[-window:]
        overall_rewards = rewards[:-window]
        
        recent_variance = np.var(recent_rewards)
        overall_variance = np.var(overall_rewards)
        
        if overall_variance == 0:
            return 1.0
        
        stability = 1.0 - (recent_variance / overall_variance)
        
        return max(0.0, stability)
    
    def analyze_learning_curves(self):
        """Analyze learning curves."""
        rewards = self.experiment_data['metrics']['rewards']
        
        if len(rewards) < 100:
            return
        
        # Split into segments for learning curve analysis
        segments = {
            'early': rewards[:len(rewards)//4],
            'middle': rewards[len(rewards)//4:len(rewards)//2],
            'late': rewards[len(rewards)//2:]
        }
        
        for segment_name, segment_rewards in segments.items():
            if len(segment_rewards) > 0:
                self.experiment_data['metrics']['learning_curves'][segment_name] = {
                    'mean': np.mean(segment_rewards),
                    'std': np.std(segment_rewards),
                    'min': np.min(segment_rewards),
                    'max': np.max(segment_rewards),
                    'slope': self.calculate_segment_slope(segment_rewards)
                }
    
    def calculate_segment_slope(self, rewards):
        """Calculate slope of reward segment."""
        if len(rewards) < 2:
            return 0.0
        
        x = np.arange(len(rewards))
        y = np.array(rewards)
        
        # Linear regression
        slope = np.polyfit(x, y, 1)[0]
        
        return slope
    
    def save_experiment_data(self):
        """Save experiment data to file."""
        # Create filename
        filename = f"{self.experiment_name}_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json"
        
        # Convert numpy arrays to lists for JSON serialization
        json_data = self.convert_to_json_serializable(self.experiment_data)
        
        # Save to file
        with open(filename, 'w') as f:
            json.dump(json_data, f, indent=2, default=str)
        
        print(f"Experiment data saved to: {filename}")
    
    def convert_to_json_serializable(self, data):
        """Convert data to JSON-serializable format."""
        if isinstance(data, dict):
            return {key: self.convert_to_json_serializable(value) for key, value in data.items()}
        elif isinstance(data, list):
            return [self.convert_to_json_serializable(item) for item in data]
        elif isinstance(data, np.ndarray):
            return data.tolist()
        elif isinstance(data, (np.integer, np.floating)):
            return float(data)
        elif isinstance(data, (int, float)):
            return data
        else:
            return data

class BrainDataCollector:
    """Collect brain-related data."""
    
    def __init__(self, brain):
        self.brain = brain
    
    def collect(self, step):
        """Collect brain data for step."""
        return {
            'step': step,
            'total_neurons': self.brain.getTotalNeuronCount(),
            'total_synapses': self.brain.getTotalSynapseCount(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'excitation_inhibition_ratio': self.brain.getExcitationInhibitionRatio(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'developmental_stage': self.brain.getDevelopmentalStage().value if hasattr(self.brain.getDevelopmentalStage(), 'value') else str(self.brain.getDevelopmentalStage())
        }

class AgentDataCollector:
    """Collect agent-related data."""
    
    def __init__(self, agent):
        self.agent = agent
    
    def collect(self, step):
        """Collect agent data for step."""
        return {
            'step': step,
            'curiosity_level': self.agent.getCuriosityLevel(),
            'novelty_level': self.agent.getNoveltyLevel(),
            'prediction_error': self.agent.getPredictionError(),
            'neuromodulation_level': self.agent.getNeuromodulationLevel(),
            'developmental_stage': self.agent.getDevelopmentalStage().value if hasattr(self.agent.getDevelopmentalStage(), 'value') else str(self.agent.getDevelopmentalStage()),
            'reward_modulation_enabled': self.agent.isRewardModulationEnabled(),
            'curiosity_enabled': self.agent.isCuriosityEnabled(),
            'development_enabled': self.agent.isDevelopmentEnabled()
        }

class WorldDataCollector:
    """Collect world-related data."""
    
    def __init__(self, world):
        self.world = world
    
    def collect(self, step):
        """Collect world data for step."""
        return {
            'step': step,
            'simulation_time': self.world.getSimulationTime(),
            'agent_position': self.get_agent_position(),
            'agent_velocity': self.get_agent_velocity(),
            'world_size': [self.world.getWidth(), self.world.getHeight()],
            'max_energy': self.world.getMaxEnergy()
        }
    
    def get_agent_position(self):
        """Get agent position."""
        agent_body = self.world.getAgentBody()
        if agent_body:
            return [agent_body.x, agent_body.y]
        return [0, 0]
    
    def get_agent_velocity(self):
        """Get agent velocity."""
        agent_body = self.world.getAgentBody()
        if agent_body:
            return [agent_body.velocityX, agent_body.velocityY]
        return [0, 0]

class EnvironmentDataCollector:
    """Collect environmental data."""
    
    def __init__(self, world):
        self.world = world
    
    def collect(self, step):
        """Collect environmental data for step."""
        return {
            'step': step,
            'percept_available': True,  # Would check if percept exists
            'sensory_inputs': self.get_sensory_inputs()
        }
    
    def get_sensory_inputs(self):
        """Get sensory inputs."""
        # Would collect actual sensory input data
        return {
            'vision_available': True,
            'touch_available': True,
            'internal_available': True
        }

# Usage example
if __name__ == "__main__":
    # Create experiment configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    
    # Create and run experiment
    experiment = BasicExperiment(config, "basic_experiment")
    results = experiment.run_experiment(num_steps=1000, num_episodes=3)
    
    # Print summary
    print("\n" + "=" * 60)
    print("Experiment Summary")
    print("=" * 60)
    
    print(f"Mean Reward: {results['performance']['mean_reward']:.3f}")
    print(f"Reward Standard Deviation: {results['performance']['std_reward']:.3f}")
    print(f"Convergence Step: {results['performance']['convergence_step']}")
    print(f"Stability Score: {results['performance']['stability_score']:.3f}")
    
    if 'learning_curves' in results['metrics']:
        print(f"\nLearning Curve Analysis:")
        for segment, data in results['metrics']['learning_curves'].items():
            print(f"  {segment.capitalize()}: mean={data['mean']:.3f}, slope={data['slope']:.5f}")
```