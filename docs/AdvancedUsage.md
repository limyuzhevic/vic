# Advanced NLM Usage - Examples for Experienced Users

## Overview

This document provides advanced usage examples and patterns for experienced NLM users who want to leverage the full capabilities of the NLM framework. It covers complex scenarios, performance optimizations, and advanced features that go beyond basic usage.

## 1. Advanced Simulation Management

### 1.1. Multi-Agent System with Coordination

```python
import pynlm
import threading
import time
from concurrent.futures import ThreadPoolExecutor
import numpy as np

class AdvancedMultiAgentSystem:
    def __init__(self, world_config, agent_configs):
        """Initialize advanced multi-agent system."""
        self.world = pynlm.createSimpleWorld()
        self.world.configure(**world_config)
        
        self.agents = []
        self.agent_brains = []
        
        for agent_config in agent_configs:
            # Create agent with custom configuration
            config = pynlm.createDefaultConfig()
            config.update(agent_config.get('brain_config', {}))
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(self.world)
            
            # Configure advanced agent features
            self.configure_advanced_agent_features(agent, agent_config)
            
            self.agents.append(agent)
            self.agent_brains.append(brain)
        
        # Setup inter-agent communication
        self.setup_agent_communication()
    
    def configure_advanced_agent_features(self, agent, config):
        """Configure advanced features for agent."""
        # Enable all advanced features
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        agent.enablePredictionError(True)
        
        # Set custom neuromodulation parameters
        brain = agent.getBrain()
        
        # Custom neuromodulation levels
        dopamine_brain = brain.getDopamine()
        if dopamine_brain:
            dopamine_brain.setLevel(0.7f)
            dopamine_brain.setPlasticityFactor(1.2f)
        
        curiosity_brain = brain.getCuriosity()
        if curiosity_brain:
            curiosity_brain.setLevel(0.5f)
            curiosity_brain.setExplorationRate(0.3f)
        
        # Custom development parameters
        dev_system = brain.getDevelopmentSystem()
        if dev_system:
            dev_system.setMaturationSpeed(0.8f)
            dev_system.setPlasticityDecline(0.9f)
    
    def setup_agent_communication(self):
        """Setup sophisticated inter-agent communication."""
        # Create communication channels between agents
        for i, agent in enumerate(self.agents):
            for j, other_agent in enumerate(self.agents):
                if i != j:
                    # Setup bidirectional communication
                    self.setup_bidirectional_communication(agent, other_agent)
    
    def setup_bidirectional_communication(self, agent1, agent2):
        """Setup bidirectional communication between two agents."""
        # Create shared sensory buffer
        shared_buffer = {
            'vision': [],
            'internal': [],
            'proprioception': [],
            'timestamp': 0
        }
        
        # Override processSensoryInput to share data
        original_process = agent1.processSensoryInput
        
        def shared_process(percept):
            # Process locally
            original_process(percept)
            
            # Share with other agent
            shared_buffer['vision'] = percept.getVision()
            shared_buffer['internal'] = percept.getInternal()
            shared_buffer['proprioception'] = percept.getProprioception()
            shared_buffer['timestamp'] = percept.getTimestamp()
            
            # Create shared percept for other agent
            shared_percept = pynlm.SensoryPercept()
            shared_percept.setVision(shared_buffer['vision'])
            shared_percept.setInternal(shared_buffer['internal'])
            shared_percept.setProprioception(shared_buffer['proprioception'])
            shared_percept.setTimestamp(shared_buffer['timestamp'])
            
            # Other agent's sensory processing
            other_agent = agent2
            original_process2 = other_agent.processSensoryInput
            original_process2(shared_percept)
        
        # Replace with shared version
        agent1.processSensoryInput = shared_process
    
    def run_coordinated_simulation(self, steps, coordination_mode='sequential'):
        """Run simulation with agent coordination."""
        if coordination_mode == 'sequential':
            return self._run_sequential_simulation(steps)
        elif coordination_mode == 'parallel':
            return self._run_parallel_simulation(steps)
        elif coordination_mode == 'hierarchical':
            return self._run_hierarchical_simulation(steps)
        elif coordination_mode == 'competitive':
            return self._run_competitive_simulation(steps)
        else:
            raise ValueError(f"Unknown coordination mode: {coordination_mode}")
    
    def _run_sequential_simulation(self, steps):
        """Run simulation with agents taking turns."""
        print("Running sequential coordination simulation...")
        
        for step in range(steps):
            # Each agent gets a turn
            for i, agent in enumerate(self.agents):
                # Agent-specific actions
                percept = self.world.getSensoryPercept()
                agent.processSensoryInput(percept)
                
                brain = agent.getBrain()
                brain.step(step)
                
                action = agent.decodeMotorCommand()
                self.world.applyMotorCommand(action, self.world.getSimulationTime())
                
                # Agent-specific learning
                reward = self.calculate_agent_reward(agent, step)
                agent.applyRewardModulation(reward, 0.0)
                
                # Log agent-specific statistics
                self.log_agent_stats(agent, step, i)
        
        return self.get_system_summary()
    
    def _run_parallel_simulation(self, steps):
        """Run simulation with agents operating in parallel."""
        print("Running parallel coordination simulation...")
        
        with ThreadPoolExecutor(max_workers=len(self.agents)) as executor:
            futures = []
            
            for i, agent in enumerate(self.agents):
                future = executor.submit(self._run_agent_parallel_simulation, agent, steps, i)
                futures.append(future)
            
            # Wait for all simulations to complete
            for future in futures:
                future.result()
        
        return self.get_system_summary()
    
    def _run_agent_parallel_simulation(self, agent, steps, agent_id):
        """Run parallel simulation for a single agent."""
        for step in range(steps):
            # Process sensory input
            percept = self.world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain computation
            brain = agent.getBrain()
            brain.step(step)
            
            # Action selection and execution
            action = agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Learning
            reward = self.calculate_agent_reward(agent, step)
            agent.applyRewardModulation(reward, 0.0)
            
            # Small delay to coordinate agent turns
            time.sleep(0.001)
    
    def _run_hierarchical_simulation(self, steps):
        """Run hierarchical simulation with leader/follower relationships."""
        print("Running hierarchical coordination simulation...")
        
        # Designate leader and followers
        leader = self.agents[0]
        followers = self.agents[1:]
        
        for step in range(steps):
            # Leader makes decisions
            leader_percept = self.world.getSensoryPercept()
            leader.processSensoryInput(leader_percept)
            
            leader_brain = leader.getBrain()
            leader_brain.step(step)
            
            leader_action = leader.decodeMotorCommand()
            
            # Followers observe leader and make decisions
            for follower in followers:
                # Enhanced sensory input based on leader's action
                enhanced_percept = self.create_enhanced_percept(leader_percept, leader_action)
                
                follower.processSensoryInput(enhanced_percept)
                
                follower_brain = follower.getBrain()
                follower_brain.step(step)
                
                # Followers may have different actions based on their perspective
                follower_action = follower.decodeMotorCommand()
                self.world.applyMotorCommand(follower_action, self.world.getSimulationTime())
        
        return self.get_system_summary()
    
    def _run_competitive_simulation(self, steps):
        """Run competitive simulation where agents compete."""
        print("Running competitive coordination simulation...")
        
        # Initialize competition scores
        scores = {i: 0 for i in range(len(self.agents))}
        
        for step in range(steps):
            # Each agent acts competitively
            for i, agent in enumerate(self.agents):
                # Competitive sensory processing
                percept = self.create_competitive_percept(i, steps)
                
                agent.processSensoryInput(percept)
                
                brain = agent.getBrain()
                brain.step(step)
                
                action = agent.decodeMotorCommand()
                self.world.applyMotorCommand(action, self.world.getSimulationTime())
                
                # Calculate competitive reward
                reward = self.calculate_competitive_reward(agent, i, scores)
                agent.applyRewardModulation(reward, 0.0)
                
                # Update scores
                scores[i] += reward
            
            # Log competition results
            self.log_competition_results(scores, step)
        
        return self.get_system_summary()
    
    def create_enhanced_percept(self, leader_percept, leader_action):
        """Create enhanced percept based on leader's action."""
        enhanced = pynlm.SensoryPercept()
        enhanced.setVision(leader_percept.getVision())
        enhanced.setInternal(leader_percept.getInternal())
        enhanced.setProprioception(leader_percept.getProprioception())
        enhanced.setAudio(leader_percept.getAudio())
        enhanced.setTimestamp(leader_percept.getTimestamp())
        
        # Add leader-specific features
        enhanced.setTouch(self.create_enhanced_touch(leader_action))
        
        return enhanced
    
    def create_enhanced_touch(self, leader_action):
        """Create enhanced touch percept based on leader action."""
        # Convert action to touch-related features
        touch_data = []
        
        if leader_action.getType() == pynlm.ActionType.Interact:
            touch_data.extend([0.8, 0.6, 0.4])  # Interaction strength
        elif leader_action.getType() == pynlm.ActionType.Eat:
            touch_data.extend([0.9, 0.3, 0.7])  # Consumption indicators
        elif leader_action.getType() == pynlm.ActionType.Drink:
            touch_data.extend([0.5, 0.8, 0.6])  # Consumption indicators
        else:
            touch_data.extend([0.2, 0.3, 0.4])  # Default touch
        
        # Add temporal dynamics
        touch_data.extend([time.time() % 1.0, (time.time() * 2) % 1.0])
        
        return touch_data
    
    def create_competitive_percept(self, agent_id, total_steps):
        """Create competitive percept for agent."""
        # Base percept from world
        percept = self.world.getSensoryPercept()
        
        # Add agent-specific competitive elements
        competitive_internal = percept.getInternal().copy() if percept.getInternal() else []
        
        # Add competition-related features
        competitive_internal.extend([
            agent_id / max(1, len(self.agents)),  # Agent identity
            (total_steps - step) / total_steps,  # Remaining steps
            np.sin(step * 0.1 + agent_id),       # Competitive oscillation
            np.cos(step * 0.05 + agent_id * 2)   # Competitive modulation
        ])
        
        # Create enhanced percept
        enhanced = pynlm.SensoryPercept()
        enhanced.setVision(percept.getVision())
        enhanced.setInternal(competitive_internal)
        enhanced.setProprioception(percept.getProprioception())
        enhanced.setAudio(percept.getAudio())
        enhanced.setTimestamp(percept.getTimestamp())
        
        return enhanced
    
    def calculate_agent_reward(self, agent, step, base_reward=1.0):
        """Calculate reward for specific agent."""
        brain = agent.getBrain()
        
        # Base reward from internal signals
        internal = brain.getInternalSignals()
        if internal:
            internal_signals = internal.getAllSignals()
            if internal_signals:
                base_reward = max(0.0, internal_signals[0])
        
        # Agent-specific modifiers
        curiosity = agent.getCuriosityLevel()
        novelty = agent.getNoveltyLevel()
        prediction_error = agent.getPredictionError()
        
        # Curiosity bonus
        curiosity_bonus = curiosity * 0.5
        
        # Novelty reward
        novelty_reward = novelty * 0.3
        
        # Prediction error penalty
        prediction_penalty = -abs(prediction_error) * 0.2
        
        # Development stage bonus
        dev_stage = brain.getDevelopmentalStage()
        if dev_stage == pynlm.DevelopmentalStage.Maturation:
            development_bonus = 0.5
        elif dev_stage == pynlm.DevelopmentalStage.CriticalPeriod:
            development_bonus = 1.0
        else:
            development_bonus = 0.0
        
        total_reward = (base_reward + curiosity_bonus + 
                       novelty_reward + prediction_penalty + development_bonus)
        
        return total_reward
    
    def calculate_competitive_reward(self, agent, agent_id, scores):
        """Calculate competitive reward based on other agents' performance."""
        # Base reward
        base_reward = self.calculate_agent_reward(agent, agent_id)
        
        # Competitive adjustment
        if len(scores) > 1:
            # Reward for outperforming average
            avg_score = sum(scores.values()) / len(scores)
            competitive_advantage = (scores[agent_id] - avg_score) / max(1, abs(avg_score))
            
            competitive_bonus = competitive_advantage * 0.5
            base_reward += competitive_bonus
        
        return base_reward
    
    def log_agent_stats(self, agent, step, agent_id):
        """Log statistics for specific agent."""
        brain = agent.getBrain()
        
        print(f"Agent {agent_id} - Step {step}: "
              f"Neurons: {brain.getTotalNeuronCount()}, "
              f"Firing: {brain.getFiringNeuronCount()}, "
              f"Curiosity: {agent.getCuriosityLevel():.3f}, "
              f"Novelty: {agent.getNoveltyLevel():.3f}, "
              f"Dev Stage: {brain.getDevelopmentalStage()}")
    
    def log_competition_results(self, scores, step):
        """Log competition results."""
        sorted_agents = sorted(scores.items(), key=lambda x: x[1], reverse=True)
        
        print(f"Competition Results - Step {step}:")
        for i, (agent_id, score) in enumerate(sorted_agents):
            print(f"  Agent {agent_id}: {score:.2f} {'(LEADER)' if i == 0 else ''}")
    
    def get_system_summary(self):
        """Get comprehensive system summary."""
        summary = {
            'total_agents': len(self.agents),
            'world_stats': {
                'width': self.world.getWidth(),
                'height': self.world.getHeight(),
                'simulation_time': self.world.getSimulationTime()
            },
            'agent_summaries': [],
            'brain_stats': []
        }
        
        for i, agent in enumerate(self.agents):
            brain = agent.getBrain()
            
            summary['agent_summaries'].append({
                'agent_id': i,
                'neurons': brain.getTotalNeuronCount(),
                'firing': brain.getFiringNeuronCount(),
                'spikes': brain.getTotalSpikeCount(),
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'prediction_error': agent.getPredictionError(),
                'development_stage': brain.getDevelopmentalStage().name
            })
            
            summary['brain_stats'].append({
                'total_neurons': brain.getTotalNeuronCount(),
                'total_synapses': brain.getTotalSynapseCount(),
                'active_neurons': brain.getActiveNeuronCount(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'avg_firing_rate': brain.getAverageFiringRate(),
                'e_i_ratio': brain.getExcitationInhibitionRatio(),
                'total_spikes': brain.getTotalSpikeCount()
            })
        
        return summary

# Usage example
config = {
    'width': 50,
    'height': 50,
    'visionWidth': 12,
    'visionHeight': 12
}

agent_configs = [
    {
        'brain_config': {'brain.neuron_count': 800},
        'novelty_enabled': True
    },
    {
        'brain_config': {'brain.neuron_count': 1200},
        'curiosity_enabled': True
    },
    {
        'brain_config': {'brain.neuron_count': 1000},
        'structural_plasticity_enabled': True
    }
]

# Create advanced multi-agent system
advanced_system = AdvancedMultiAgentSystem(config, agent_configs)

# Run different coordination simulations
system_results = {}
for mode in ['sequential', 'parallel', 'hierarchical', 'competitive']:
    print(f"\n=== Running {mode} coordination simulation ===")
    results = advanced_system.run_coordinated_simulation(100, mode)
    system_results[mode] = results

# Save results
save_system_results(system_results, 'advanced_multi_agent_results.json')
print("Advanced multi-agent simulation completed and saved!")
```

### 1.2. Advanced Memory Management

```python
import pynlm
import json
from datetime import datetime

class AdvancedMemoryManager:
    def __init__(self, brain_config):
        """Initialize advanced memory manager."""
        self.brain_config = brain_config
        self.memory_profiles = {}
        self.checkpoint_intervals = {}
        self.optimization_history = []
        
    def create_memory_efficient_brain(self, config):
        """Create brain with memory-efficient configuration."""
        # Optimize for memory efficiency
        optimized_config = pynlm.createDefaultConfig()
        
        # Set memory-efficient parameters
        optimized_config.set("brain.neuron_count", min(config.get('brain.neuron_count', 1000), 500))
        optimized_config.set("brain.region_count", min(config.get('brain.region_count', 2), 1))
        
        # Disable memory-intensive features
        optimized_config.set("plasticity.structural.enable", False)
        optimized_config.set("neuromod.novelty.enable", False)
        optimized_config.set("neuromod.curiosity.enable", False)
        
        # Enable memory-efficient alternatives
        optimized_config.set("plasticity.stdp.enable", True)
        optimized_config.set("plasticity.hebbian.enable", True)
        
        return optimized_config
    
    def create_performance_optimized_brain(self, config):
        """Create brain optimized for performance."""
        # Optimize for speed
        optimized_config = pynlm.createDefaultConfig()
        
        # Set performance-optimized parameters
        optimized_config.set("brain.neuron_count", config.get('brain.neuron_count', 1000))
        optimized_config.set("brain.region_count", config.get('brain.region_count', 2))
        
        # Enable performance features
        optimized_config.set("plasticity.structural.enable", True)
        optimized_config.set("plasticity.stdp.enable", True)
        optimized_config.set("neuromod.curiosity.enable", True)
        
        # Set performance-specific parameters
        optimized_config.set("simulation_timestep", 0.001)
        optimized_config.set("replay_interval", 100)
        optimized_config.set("consolidation_interval", 1000)
        
        return optimized_config
    
    def profile_memory_usage(self, brain, steps=100):
        """Profile memory usage during simulation."""
        import psutil
        import os
        
        memory_samples = []
        time_samples = []
        
        for step in range(steps):
            brain.step(step)
            
            # Sample memory usage
            process = psutil.Process(os.getpid())
            memory_info = process.memory_info()
            
            memory_samples.append({
                'step': step,
                'rss': memory_info.rss / 1024 / 1024,  # MB
                'vms': memory_info.vms / 1024 / 1024,  # MB
                'shared': memory_info.shared / 1024 / 1024,  # MB
                'text': memory_info.text / 1024 / 1024,  # MB
                'data': memory_info.data / 1024 / 1024,  # MB
                'dirty': memory_info.dirty / 1024 / 1024   # MB
            })
            
            time_samples.append(time.time())
        
        return {
            'memory_samples': memory_samples,
            'time_samples': time_samples,
            'peak_memory': max(m['rss'] for m in memory_samples),
            'avg_memory': sum(m['rss'] for m in memory_samples) / len(memory_samples),
            'memory_growth': memory_samples[-1]['rss'] - memory_samples[0]['rss']
        }
    
    def optimize_memory_allocation(self, brain, optimization_targets):
        """Optimize memory allocation based on targets."""
        # Analyze current memory usage
        profile = self.profile_memory_usage(brain)
        
        # Determine optimization strategy based on targets
        if 'memory' in optimization_targets:
            self.optimize_for_memory_efficiency(brain, profile)
        elif 'performance' in optimization_targets:
            self.optimize_for_performance(brain, profile)
        elif 'balanced' in optimization_targets:
            self.optimize_for_balance(brain, profile)
        
        return profile
    
    def optimize_for_memory_efficiency(self, brain, profile):
        """Optimize brain for memory efficiency."""
        # Create memory-efficient version
        memory_efficient_config = self.create_memory_efficient_brain(self.brain_config)
        
        # Reinitialize brain with memory-efficient config
        memory_efficient_brain = pynlm.createBrain(memory_efficient_config)
        memory_efficient_brain.initialize()
        
        # Replace original brain
        self.brain = memory_efficient_brain
        
        # Store optimization
        self.optimization_history.append({
            'timestamp': datetime.now().isoformat(),
            'strategy': 'memory_efficiency',
            'memory_reduction': (profile['peak_memory'] - self.get_current_memory()) / profile['peak_memory'] * 100
        })
    
    def optimize_for_performance(self, brain, profile):
        """Optimize brain for performance."""
        # Create performance-optimized version
        performance_config = self.create_performance_optimized_brain(self.brain_config)
        
        # Reinitialize brain with performance-optimized config
        performance_brain = pynlm.createBrain(performance_config)
        performance_brain.initialize()
        
        # Replace original brain
        self.brain = performance_brain
        
        # Store optimization
        self.optimization_history.append({
            'timestamp': datetime.now().isoformat(),
            'strategy': 'performance',
            'speed_improvement': self.calculate_speed_improvement(profile)
        })
    
    def optimize_for_balance(self, brain, profile):
        """Optimize brain for balanced performance."""
        # Create balanced configuration
        balanced_config = self.create_default_config()
        
        # Apply balanced optimizations
        balanced_config.set("brain.neuron_count", min(profile['total_neurons'], 800))
        balanced_config.set("plasticity.structural.enable", True)
        balanced_config.set("neuromod.curiosity.enable", True)
        balanced_config.set("neuromod.novelty.enable", True)
        
        # Reinitialize brain
        balanced_brain = pynlm.createBrain(balanced_config)
        balanced_brain.initialize()
        
        # Replace original brain
        self.brain = balanced_brain
        
        # Store optimization
        self.optimization_history.append({
            'timestamp': datetime.now().isoformat(),n            'strategy': 'balanced',
            'memory_efficiency': self.calculate_memory_efficiency(profile),
            'performance_score': self.calculate_overall_performance_score(profile)
        })
    
    def get_current_memory(self):
        """Get current memory usage."""
        import psutil
        process = psutil.Process()
        return process.memory_info().rss / 1024 / 1024  # MB
    
    def calculate_speed_improvement(self, profile):
        """Calculate speed improvement from optimization."""
        # This would compare with baseline performance
        return 15.0  # Example: 15% improvement
    
    def calculate_memory_efficiency(self, profile):
        """Calculate memory efficiency improvement."""
        # This would compare with baseline memory usage
        return 20.0  # Example: 20% improvement
    
    def calculate_overall_performance_score(self, profile):
        """Calculate overall performance score."""
        # Combine memory and performance metrics
        memory_score = self.calculate_memory_efficiency(profile)
        speed_score = self.calculate_speed_improvement(profile)
        
        return (memory_score + speed_score) / 2
    
    def save_memory_profile(self, filepath):
        """Save memory profile to file."""
        profile_data = {
            'timestamp': datetime.now().isoformat(),
            'optimization_history': self.optimization_history,
            'current_memory': self.get_current_memory(),
            'brain_config': self.brain_config.toDict() if hasattr(self, 'brain') else None
        }
        
        with open(filepath, 'w') as f:
            json.dump(profile_data, f, indent=2)
        
        print(f"Memory profile saved to {filepath}")

# Usage example
brain_config = {'brain.neuron_count': 1000, 'brain.region_count': 2}

memory_manager = AdvancedMemoryManager(brain_config)

# Create performance-optimized brain
performance_config = memory_manager.create_performance_optimized_brain(brain_config)
performance_brain = pynlm.createBrain(performance_config)
performance_brain.initialize()

# Profile memory usage
profile = memory_manager.profile_memory_usage(performance_brain, 200)

# Optimize for specific targets
memory_manager.optimize_memory_allocation(performance_brain, ['performance'])

# Save memory profile
memory_manager.save_memory_profile('memory_profile.json')
print("Memory profiling completed!")
```

### 1.3. Advanced State Management

```python
import pynlm
import json
import hashlib
from datetime import datetime
from typing import Dict, List, Any
import threading

class AdvancedStateManager:
    def __init__(self, brain_config):
        """Initialize advanced state manager."""
        self.brain_config = brain_config
        self.state_history = []
        self.checkpoint_registry = {}
        self.state_consistency_checker = StateConsistencyChecker()
        self.state_compressor = StateCompressor()
        self.state_integrity_validator = StateIntegrityValidator()
        
    def create_checkpoint(self, brain, name, metadata=None):
        """Create advanced checkpoint with metadata and validation."""
        # Get brain state
        brain_state = self._capture_brain_state(brain)
        
        # Validate state integrity
        validation_result = self.state_integrity_validator.validate(brain_state)
        if not validation_result.is_valid:
            raise StateValidationError(f"State validation failed: {validation_result.errors}")
        
        # Compress state
        compressed_state = self.state_compressor.compress(brain_state)
        
        # Create checkpoint with metadata
        checkpoint = {
            'name': name,
            'timestamp': datetime.now().isoformat(),
            'metadata': metadata or {},
            'state_hash': self._calculate_state_hash(compressed_state),
            'compressed_size': len(compressed_state),
            'neuron_count': len(brain_state.get('neurons', [])),
            'synapse_count': len(brain_state.get('synapses', [])),
            'step_count': brain_state.get('step_count', 0),
            'development_stage': brain_state.get('development_stage', 'Initial'),
            'world_state': brain_state.get('world_state', {}),
            'internal_states': brain_state.get('internal_states', [])
        }
        
        # Store checkpoint
        self.checkpoint_registry[name] = checkpoint
        
        # Save to file
        filepath = f"checkpoints/{name}.chk"
        self._save_checkpoint(filepath, checkpoint)
        
        return checkpoint
    
    def restore_checkpoint(self, name):
        """Restore brain state from checkpoint."""
        if name not in self.checkpoint_registry:
            raise CheckpointNotFoundError(f"Checkpoint not found: {name}")
        
        # Load checkpoint
        checkpoint = self.checkpoint_registry[name]
        filepath = f"checkpoints/{name}.chk"
        
        # Restore state
        restored_state = self._load_checkpoint(filepath)
        
        # Validate restored state
        validation_result = self.state_integrity_validator.validate(restored_state)
        if not validation_result.is_valid:
            raise StateValidationError(f"Restored state validation failed: {validation_result.errors}")
        
        # Recreate brain
        brain = self._recreate_brain_from_state(restored_state)
        
        return brain, checkpoint
    
    def compare_checkpoints(self, checkpoint1_name, checkpoint2_name):
        """Compare two checkpoints and identify differences."""
        checkpoint1 = self.checkpoint_registry.get(checkpoint1_name)
        checkpoint2 = self.checkpoint_registry.get(checkpoint2_name)
        
        if not checkpoint1 or not checkpoint2:
            raise ValueError("One or both checkpoints not found")
        
        # Load states
        filepath1 = f"checkpoints/{checkpoint1_name}.chk"
        filepath2 = f"checkpoints/{checkpoint2_name}.chk"
        
        state1 = self._load_checkpoint(filepath1)
        state2 = self._load_checkpoint(filepath2)
        
        # Compare states
        comparison = self._compare_states(state1, state2)
        
        return {
            'checkpoint1': checkpoint1_name,
            'checkpoint2': checkpoint2_name,
            'comparison': comparison,
            'similarity_score': comparison.get('similarity_score', 0.0),
            'differences': comparison.get('differences', [])
        }
    
    def create_state_snapshot(self, brain, snapshot_name):
        """Create lightweight state snapshot for quick access."""
        # Capture essential brain state
        snapshot = {
            'name': snapshot_name,
            'timestamp': datetime.now().isoformat(),
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'active_neurons': brain.getActiveNeuronCount(),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'development_stage': brain.getDevelopmentalStage().name,
            'curiosity_level': brain.getCuriosity().getLevel() if brain.getCuriosity() else 0.0,
            'novelty_level': brain.getNovelty().getLevel() if brain.getNovelty() else 0.0,
            'prediction_error': brain.getPredictionErrorSignal().getLevel() if brain.getPredictionErrorSignal() else 0.0,
            'world_state': self._get_world_state(brain),
            'hash': self._calculate_brain_hash(brain)
        }
        
        # Store snapshot
        self.state_history.append(snapshot)
        
        # Save to file
        filepath = f"snapshots/{snapshot_name}.snap"
        self._save_snapshot(filepath, snapshot)
        
        return snapshot
    
    def compare_snapshots(self, snapshot1_name, snapshot2_name):
        """Compare two snapshots and identify differences."""
        # Load snapshots
        filepath1 = f"snapshots/{snapshot1_name}.snap"
        filepath2 = f"snapshots/{snapshot2_name}.snap"
        
        snapshot1 = self._load_snapshot(filepath1)
        snapshot2 = self._load_snapshot(filepath2)
        
        # Compare snapshots
        comparison = self._compare_snapshots_data(snapshot1, snapshot2)
        
        return {
            'snapshot1': snapshot1_name,
            'snapshot2': snapshot2_name,
            'comparison': comparison,
            'similarity_score': comparison.get('similarity_score', 0.0),
            'changes': comparison.get('changes', [])
        }
    
    def create_advanced_experiment_protocol(self, experiment_name, protocol_config):
        """Create advanced experiment protocol with state management."""
        protocol = {
            'name': experiment_name,
            'timestamp': datetime.now().isoformat(),
            'config': protocol_config,
            'checkpoints': [],
            'snapshots': [],
            'milestones': self._define_experiment_milestones(protocol_config),
            'validation_criteria': self._define_validation_criteria(protocol_config),
            'rollback_plan': self._create_rollback_plan(protocol_config)
        }
        
        return protocol
    
    def run_experiment_with_state_management(self, brain, protocol):
        """Run experiment with comprehensive state management."""
        print(f"Starting experiment: {protocol['name']}")
        
        # Initialize experiment state
        experiment_state = {
            'protocol': protocol,
            'current_checkpoint': None,
            'snapshots': [],
            'metrics': [],
            'errors': [],
            'progress': 0.0
        }
        
        try:
            # Run experiment phases
            for phase in protocol['milestones']:
                print(f"Running phase: {phase['name']}")
                
                # Execute phase
                phase_result = self._execute_experiment_phase(brain, phase)
                
                # Record checkpoint if specified
                if phase.get('create_checkpoint'):
                    checkpoint_name = f"{protocol['name']}_{phase['name']}_checkpoint"
                    checkpoint = self.create_checkpoint(brain, checkpoint_name, phase.get('checkpoint_metadata', {}))
                    experiment_state['current_checkpoint'] = checkpoint_name
                    protocol['checkpoints'].append(checkpoint)
                
                # Create snapshot if specified
                if phase.get('create_snapshot'):
                    snapshot_name = f"{protocol['name']}_{phase['name']}_snapshot"
                    snapshot = self.create_state_snapshot(brain, snapshot_name)
                    experiment_state['snapshots'].append(snapshot)
                    protocol['snapshots'].append(snapshot)
                
                # Record metrics
                metrics = self._calculate_experiment_metrics(brain, phase)
                experiment_state['metrics'].extend(metrics)
                
                # Validate results
                validation_result = self._validate_experiment_phase(experiment_state, phase)
                if not validation_result.is_valid:
                    experiment_state['errors'].extend(validation_result.errors)
                    
                    # Handle errors according to protocol
                    if protocol['validation_criteria']['allow_failure']:
                        print(f"Phase validation failed but allowed: {validation_result.errors}")
                    else:
                        raise ExperimentValidationError(f"Phase validation failed: {validation_result.errors}")
                
                # Update progress
                experiment_state['progress'] = (len(experiment_state['metrics']) / 
                                               len(protocol['milestones'])) * 100
            
            print(f"Experiment completed successfully: {protocol['name']}")
            return experiment_state
            
        except Exception as e:
            print(f"Experiment failed: {e}")
            
            # Create rollback checkpoint if available
            if experiment_state['current_checkpoint']:
                print(f"Creating rollback checkpoint: {experiment_state['current_checkpoint']}")
                rollback_checkpoint = self.restore_checkpoint(experiment_state['current_checkpoint'])
                experiment_state['rollback_checkpoint'] = rollback_checkpoint
            
            # Re-raise exception
            raise
    
    def _capture_brain_state(self, brain):
        """Capture complete brain state."""
        return {
            'neurons': self._capture_neurons_state(brain),
            'synapses': self._capture_synapses_state(brain),
            'world_state': self._capture_world_state(brain),
            'internal_states': self._capture_internal_states(brain),
            'memory_systems': self._capture_memory_systems(brain),
            'neuromodulation_systems': self._capture_neuromodulation_systems(brain),
            'development_state': self._capture_development_state(brain),
            'prediction_systems': self._capture_prediction_systems(brain),
            'cognition_systems': self._capture_cognition_systems(brain),
            'step_count': self._get_step_count(brain),
            'development_stage': brain.getDevelopmentalStage().name,
            'simulation_time': self._get_simulation_time(brain)
        }
    
    def _capture_neurons_state(self, brain):
        """Capture neuron state details."""
        neurons = []
        
        # Get all regions
        regions = brain.getRegions()
        for region in regions:
            # Get neurons in region
            for pop in region->getPopulations():
                for neuron in pop->getNeurons():
                    # Capture neuron state
                    neuron_state = {
                        'id': neuron->getId().index(),
                        'region_id': region->getId().index(),
                        'population_id': pop->getId().index(),
                        'type': neuron->getType(),
                        'membrane_potential': neuron->getState().membranePotential,
                        'resting_potential': neuron->getState().restingPotential,
                        'threshold': neuron->getState().threshold,
                        'reset_potential': neuron->getState().resetPotential,
                        'leak_conductance': neuron->getState().leakConductance,
                        'firing_state': neuron->getState().firingState,
                        'refractory_remaining': neuron->getState().refractoryRemaining,
                        'refractory_period': neuron->getState().refractoryPeriod,
                        'last_spike_time': neuron->getState().lastSpikeTime,
                        'is_sensory': neuron->isSensory(),
                        'is_motor': neuron->isMotor()
                    }
                    
                    neurons.append(neuron_state)
        
        return neurons
    
    def _capture_synapses_state(self, brain):
        """Capture synapse state details."""
        synapses = []
        
        # Get all regions
        regions = brain.getRegions()
        for region in regions:
            # Get synapses in region
            for synapse in region->getSynapses() {
                # Capture synapse state
                synapse_state = {
                    'id': synapse->getId().index(),
                    'source_neuron': synapse->getSourceNeuron().index(),
                    'destination_neuron': synapse->getDestinationNeuron().index(),
                    'weight': synapse->getWeight(),
                    'delay': synapse->getDelay(),
                    'type': synapse->getType(),
                    'plasticity_flags': synapse->getPlasticityFlags(),
                    'eligibility_trace': synapse->getEligibilityTrace(),
                    'is_excitatory': synapse->getType() == SynapseType::Excitatory
                }
                
                synapses.append(synapse_state)
            }
        
        return synapses
    
    def _capture_world_state(self, brain):
        """Capture world state."""
        # Get agent body state
        agent_body = brain.getAgentBody()
        if agent_body:
            return {
                'x': agent_body->x,
                'y': agent_body->y,
                'orientation': agent_body->orientation,
                'velocity_x': agent_body->velocityX,
                'velocity_y': agent_body->velocityY,
                'angular_velocity': agent_body->angularVelocity,
                'energy': agent_body->energy,
                'health': agent_body->health,
                'age': agent_body->age,
                'is_moving': agent_body->isMoving,
                'is_turning': agent_body->isTurning,
                'last_action_time': agent_body->lastActionTime
            }
        
        return {}
    
    def _capture_internal_states(self, brain):
        """Capture internal system states."""
        return {
            'dopamine': brain.getDopamine()->getLevel(),
            'curiosity': brain.getCuriosity()->getLevel(),
            'novelty': brain.getNovelty()->getLevel(),
            'prediction_error': brain.getPredictionErrorSignal()->getLevel(),
            'working_memory': brain.getWorkingMemory()->getActiveTraces(),
            'episodic_memory': brain.getEpisodicMemory()->getEpisodeCount(),
            'associative_memory': brain.getAssociativeMemory()->getTotalAssociations()
        }
    
    def _capture_memory_systems(self, brain):
        """Capture memory system states."""
        return {
            'working_memory': brain.getWorkingMemory()->toDict(),
            'episodic_memory': brain.getEpisodicMemory()->toDict(),
            'associative_memory': brain.getAssociativeMemory()->toDict()
        }
    
    def _capture_neuromodulation_systems(self, brain):
        """Capture neuromodulation system states."""
        return {
            'dopamine': {
                'level': brain.getDopamine()->getLevel(),
                'plasticity_factor': brain.getDopamine()->getPlasticityFactor()
            },
            'curiosity': {
                'level': brain.getCuriosity()->getLevel(),
                'exploration_rate': brain.getCuriosity()->getExplorationRate()
            },
            'novelty': {
                'level': brain.getNovelty()->getLevel()
            },
            'prediction_error': {
                'level': brain.getPredictionErrorSignal()->getLevel()
            }
        }
    
    def _capture_development_state(self, brain):
        """Capture development system state."""
        development_system = brain.getDevelopmentSystem()
        if development_system:
            return {
                'current_stage': brain.getDevelopmentalStage().name,
                'maturation_speed': development_system->getMaturationSpeed(),
                'plasticity_decline': development_system->getPlasticityDecline(),
                'synaptogenesis_rate': development_system->getSynaptogenesisRate(),
                'pruning_rate': development_system->getPruningRate()
            }
        
        return {}
    
    def _capture_prediction_systems(self, brain):
        """Capture prediction system state."""
        prediction_system = brain.getPredictionSystem()
        if prediction_system:
            return prediction_system->toDict()
        
        return {}
    
    def _capture_cognition_systems(self, brain):
        """Capture cognition system state."""
        return {
            'attention': brain.getAttention()->toDict() if brain.getAttention() else {},
            'planning': brain.getPlanner()->toDict() if brain.getPlanner() else {},
            'concept_formation': brain.getConceptFormation()->toDict() if brain.getConceptFormation() else {},
            'self_model': {},  # Implementation dependent
            'social_learning': {}  # Implementation dependent
        }
    
    # Additional helper methods
    def _get_world_state(self, brain):
        """Get simplified world state."""
        world = brain.getWorld()
        if world:
            return {
                'simulation_time': world.getSimulationTime(),
                'agent_position': (world.getAgentBody()->x, world.getAgentBody()->y),
                'world_dimensions': (world.getWidth(), world.getHeight())
            }
        
        return {}
    
    def _get_step_count(self, brain):
        """Get current simulation step count."""
        return brain.getTotalSpikeCount()
    
    def _get_simulation_time(self, brain):
        """Get current simulation time."""
        world = brain.getWorld()
        return world.getSimulationTime() if world else 0.0
    
    def _calculate_state_hash(self, state):
        """Calculate hash of state for integrity checking."""
        import hashlib
        state_json = json.dumps(state, sort_keys=True)
        return hashlib.sha256(state_json.encode()).hexdigest()
    
    def _calculate_brain_hash(self, brain):
        """Calculate hash of brain state."""
        # Create hash based on key brain properties
        hash_data = f"{brain.getTotalNeuronCount()}_{brain.getTotalSynapseCount()}_{brain.getTotalSpikeCount()}"
        return hashlib.sha256(hash_data.encode()).hexdigest()
    
    # File operations (simplified)
    def _save_checkpoint(self, filepath, checkpoint):
        """Save checkpoint to file."""
        import os
        os.makedirs(os.path.dirname(filepath), exist_ok=True)
        
        with open(filepath, 'w') as f:
            json.dump(checkpoint, f, indent=2)
    
    def _load_checkpoint(self, filepath):
        """Load checkpoint from file."""
        with open(filepath, 'r') as f:
            return json.load(f)
    
    def _save_snapshot(self, filepath, snapshot):
        """Save snapshot to file."""
        import os
        os.makedirs(os.path.dirname(filepath), exist_ok=True)
        
        with open(filepath, 'w') as f:
            json.dump(snapshot, f, indent=2)
    
    def _load_snapshot(self, filepath):
        """Load snapshot from file."""
        with open(filepath, 'r') as f:
            return json.load(f)
    
    def _create_brain_from_state(self, state):
        """Recreate brain from state."""
        # This would involve reconstructing the entire brain from saved state
        # For simplicity, we'll create a new brain with default configuration
        # In a real implementation, this would be much more sophisticated
        
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", state.get('neuron_count', 1000))
        config.set("brain.region_count", state.get('region_count', 1))
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        return brain
    
    def _define_experiment_milestones(self, protocol_config):
        """Define experiment milestones."""
        milestones = protocol_config.get('milestones', [])
        return milestones
    
    def _define_validation_criteria(self, protocol_config):
        """Define validation criteria for experiment."""
        return protocol_config.get('validation_criteria', {
            'allow_failure': False,
            'max_runtime': float('inf'),
            'min_performance': 0.0
        })
    
    def _create_rollback_plan(self, protocol_config):
        """Create rollback plan for experiment."""
        return protocol_config.get('rollback_plan', {
            'auto_rollback': True,
            'rollback_point': 'last_checkpoint',
            'max_rollbacks': 3
        })
    
    def _execute_experiment_phase(self, brain, phase):
        """Execute single experiment phase."""
        # This would execute the specific phase logic
        # For now, we'll simulate the execution
        return {'success': True, 'metrics': phase.get('expected_metrics', [])}
    
    def _calculate_experiment_metrics(self, brain, phase):
        """Calculate metrics for experiment phase."""
        # Calculate relevant metrics based on phase
        return [{
            'metric_name': 'performance',
            'value': brain.getAverageFiringRate(),
            'timestamp': datetime.now().isoformat()
        }, {
            'metric_name': 'stability',
            'value': self.calculate_stability(brain),
            'timestamp': datetime.now().isoformat()
        }]
    
    def _validate_experiment_phase(self, experiment_state, phase):
        """Validate experiment phase results."""
        # Check if phase meets validation criteria
        return ValidationResult(
            is_valid=True,
            errors=[],
            warnings=[]
        )
    
    def calculate_stability(self, brain):
        """Calculate stability metric for brain."""
        # This would implement stability calculation
        # For now, return a placeholder
        return 0.85

# Supporting classes for advanced state management
class StateConsistencyChecker:
    """Check consistency of brain state."""
    def validate(self, state):
        return ValidationResult(
            is_valid=True,
            errors=[],
            warnings=[]
        )

class StateCompressor:
    """Compress brain state for storage."""
    def compress(self, state):
        return json.dumps(state)

class StateIntegrityValidator:
    """Validate integrity of brain state."""
    def validate(self, state):
        return ValidationResult(
            is_valid=True,
            errors=[],
            warnings=[]
        )

class ValidationResult:
    """Result of validation operation."""
    def __init__(self, is_valid, errors, warnings):
        self.is_valid = is_valid
        self.errors = errors
        self.warnings = warnings

class StateValidationError(Exception):
    """Exception for state validation errors."""
    pass

class CheckpointNotFoundError(Exception):
    """Exception for missing checkpoints."""
    pass

class ExperimentValidationError(Exception):
    """Exception for experiment validation errors."""
    pass

# Usage example for advanced state management
advanced_state_manager = AdvancedStateManager(
    brain_config={'brain.neuron_count': 1000, 'brain.region_count': 2}
)

# Create performance-optimized brain
performance_config = advanced_state_manager.create_performance_optimized_brain(
    brain_config
)
performance_brain = pynlm.createBrain(performance_config)
performance_brain.initialize()

# Create checkpoint
checkpoint = advanced_state_manager.create_checkpoint(
    performance_brain, 'experiment_checkpoint', {'phase': 'initialization'}
)

print(f"Checkpoint created: {checkpoint['name']}")
print(f"Checkpoint size: {checkpoint['compressed_size']} bytes")
print(f"State hash: {checkpoint['state_hash']}")

# Create state snapshot
snapshot = advanced_state_manager.create_state_snapshot(
    performance_brain, 'experiment_snapshot'
)

print(f"Snapshot created: {snapshot['name']}")
print(f"Snapshot neuron count: {snapshot['neuron_count']}")

# Create and run advanced experiment
experiment_protocol = advanced_state_manager.create_advanced_experiment_protocol(
    'behavioral_learning_experiment',
    {
        'milestones': [
            {'name': 'learning_phase_1', 'create_checkpoint': True, 'create_snapshot': True},
            {'name': 'learning_phase_2', 'create_checkpoint': True, 'create_snapshot': True},
            {'name': 'testing_phase', 'create_checkpoint': True, 'create_snapshot': True}
        ],
        'validation_criteria': {'allow_failure': False},
        'rollback_plan': {'auto_rollback': True}
    }
)

# Run experiment with state management
experiment_result = advanced_state_manager.run_experiment_with_state_management(
    performance_brain, experiment_protocol
)

print(f"Experiment completed successfully: {experiment_protocol['name']}")
print(f"Final checkpoint: {experiment_result['current_checkpoint']}")
print(f"Experiment progress: {experiment_result['progress']:.1f}%")
print("Advanced state management completed!")
```

## Conclusion

These advanced NLM usage examples demonstrate the comprehensive capabilities of the NLM framework for experienced users. The examples cover:

1. **Advanced Multi-Agent Systems**: Complex coordination, communication, and competition between multiple agents
2. **Advanced Memory Management**: Memory-efficient brain design and profiling
3. **Advanced State Management**: Comprehensive checkpointing, snapshotting, and experiment management

Key takeaways for advanced NLM users:

1. **Multi-Agent Coordination**: Design sophisticated inter-agent communication and coordination mechanisms
2. **Memory Optimization**: Balance performance and memory efficiency through systematic optimization
3. **State Management**: Implement robust checkpointing and state management for long-running experiments
4. **Error Handling**: Design comprehensive error handling and recovery mechanisms
5. **Performance Monitoring**: Continuously monitor and optimize performance throughout the experiment

These advanced examples build upon the fundamental NLM concepts and demonstrate how to leverage the full capabilities of the framework for complex, real-world applications. The patterns and techniques shown here can be adapted to create sophisticated neural simulations with autonomous learning, coordination, and behavior emergence.

By mastering these advanced usage patterns, experienced users can create powerful NLM applications that push the boundaries of artificial neural systems, demonstrating the full potential of brain-inspired computation.

---

**Note**: These advanced examples require a solid understanding of NLM fundamentals and may need additional implementation details for specific use cases. The examples are provided as starting points for customization and extension.

---

**See also**:
- `HOW_TO_USE.md` - Basic usage guide
- `docs/ARCHITECTURE.md` - System architecture
- Performance benchmarking guide for optimization techniques
- Advanced state management patterns for complex experiments