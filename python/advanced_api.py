"""
Advanced NLM Python API for Power Users

This module provides advanced functionality for experienced users who want
to customize and extend the NLM brain simulation framework.
"""

import pynlm
from typing import List, Optional, Dict, Any, Callable
import json
import os

class AdvancedBrainManager:
    """Advanced brain configuration and management."""
    
    def __init__(self, brain=None):
        """Initialize with existing brain or create new one."""
        if brain is not None:
            self.brain = brain
        else:
            self.brain = pynlm.createBrain(pynlm.createDefaultConfig())
            self.brain.initialize()
    
    def configure_neurons(self, count: int, neuron_type=pynlm.NeuronType.Excitatory):
        """Configure brain with specific neuron count and type."""
        config = pynlm.createDefaultConfig()
        
        # Set neuron count
        config.set("brain.neuron_count", count)
        
        # Clear and recreate brain
        self.brain.reset()
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        return self
    
    def add_custom_connections(self, 
                              source_region_id: pynlm.RegionId,
                              target_region_id: pynlm.RegionId,
                              weight: float = 0.5,
                              delay: int = 1):
        """Add custom inter-region connections."""
        self.brain.addInterRegionConnection(
            source_region_id, target_region_id, weight, delay)
        return self
    
    def enable_advanced_features(self, reward_modulation=True,
                                structural_plasticity=True,
                                development=True,
                                curiosity=True,
                                novelty=True,
                                prediction=True):
        """Enable all advanced brain subsystems."""
        agent = pynlm.createAgentBrain(self.brain)
        agent.initialize(pynlm.createSimpleWorld())
        
        agent.enableRewardModulation(reward_modulation)
        agent.enableStructuralPlasticity(structural_plasticity)
        agent.enableDevelopment(development)
        agent.enableCuriosity(curiosity)
        
        return self
    
    def benchmark_simulation(self, steps: int, world_config=None):
        """Run advanced benchmark simulation with detailed metrics."""
        import time
        
        world = pynlm.createSimpleWorld()
        if world_config:
            world.configure(**world_config)
        
        agent = pynlm.createAgentBrain(self.brain)
        agent.initialize(world)
        
        start_time = time.time()
        metrics = []
        
        for step in range(steps):
            # Update world
            world.update(0.1)
            
            # Process sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain simulation
            self.brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Record metrics
            if step % 100 == 0:
                metric = {
                    'step': step,
                    'firing_rate': self.brain.getAverageFiringRate(),
                    'total_spikes': self.brain.getTotalSpikeCount(),
                    'curiosity': agent.getCuriosityLevel(),
                    'novelty': agent.getNoveltyLevel(),
                    'reward': agent.getNeuromodulationLevel(),
                    'development_stage': self.brain.getDevelopmentalStage()
                }
                metrics.append(metric)
        
        end_time = time.time()
        
        return {
            'metrics': metrics,
            'steps': steps,
            'total_time': end_time - start_time,
            'steps_per_second': steps / (end_time - start_time)
        }
    
    def save_advanced_checkpoint(self, path: str, metadata: Dict[str, Any] = None):
        """Save brain state with custom metadata."""
        # Save brain state
        success = self.brain.save(path)
        
        if success and metadata:
            # Save metadata as JSON
            metadata_path = path.replace('.bin', '_meta.json')
            with open(metadata_path, 'w') as f:
                json.dump(metadata, f, indent=2)
        
        return success

class ExperimentRunner:
    """Run complex experimental protocols."""
    
    @staticmethod
    def run_learning_curve(brain_config, world_config, 
                         environment_changes: List[Dict],
                         num_episodes: int = 100):
        """Run learning curve with changing environment parameters."""
        results = []
        
        for change in environment_changes:
            print(f"Running phase: {change.get('phase', 'Unknown')}")
            
            # Create configuration for this phase
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", brain_config.get('neuron_count', 1000))
            config.set("region_count", brain_config.get('region_count', 1))
            config.set("connection_probability", brain_config.get('connection_probability', 0.1))
            
            # Create brain and world
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            world.configure(**world_config)
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Enable subsystems
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            
            # Run simulation for this phase
            phase_results = []
            for episode in range(num_episodes):
                # Run one episode
                for step in range(change.get('steps_per_episode', 200)):
                    world.update(0.1)
                    agent.processSensoryInput(world.getSensoryPercept())
                    brain.step(step)
                    
                    action = agent.decodeMotorCommand()
                    world.applyMotorCommand(action, world.getSimulationTime())
                    
                    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
                    agent.applyRewardModulation(reward, 0.0)
                
                phase_results.append({
                    'episode': episode,
                    'final_reward': agent.getNeuromodulationLevel(),
                    'curiosity': agent.getCuriosityLevel(),
                    'firing_rate': brain.getAverageFiringRate()
                })
            
            results.append({
                'phase': change.get('phase'),
                'environment_params': change,
                'results': phase_results,
                'brain_config': brain_config,
                'world_config': world_config
            })
        
        return results

class VisualizationHelper:
    """Helper functions for visualizing brain state and metrics."""
    
    @staticmethod
    def generate_brain_summary(brain) -> Dict[str, Any]:
        """Generate comprehensive brain state summary."""
        return {
            'basic_stats': {
                'total_neurons': brain.getTotalNeuronCount(),
                'total_synapses': brain.getTotalSynapseCount(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'average_firing_rate': brain.getAverageFiringRate(),
                'excitatory_inhibition_ratio': brain.getExcitationInhibitionRatio(),
                'developmental_stage': str(brain.getDevelopmentalStage())
            },
            'memory_systems': {
                'working_memory_active_traces': brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 0,
                'episodic_memory_episode_count': brain.getEpisodicMemory().getEpisodeCount() if brain.getEpisodicMemory() else 0,
                'associative_memory_strength': 0.0  # Would need to query from memory system
            },
            'neuromodulation': {
                'dopamine_level': 0.0,  # Would need to query from agent
                'curiosity_level': 0.0,  # Would need to query from agent
                'novelty_level': 0.0,  # Would need to query from agent
                'prediction_error': 0.0  # Would need to query from agent
            }
        }
    
    @staticmethod
    def format_metrics_readable(metrics: Dict[str, Any]) -> str:
        """Format metrics dictionary in human-readable format."""
        lines = ["NLM Brain State Summary", "=" * 50]
        
        lines.append(f"Basic Statistics:")
        lines.append(f"  Neurons: {metrics['basic_stats']['total_neurons']}")
        lines.append(f"  Synapses: {metrics['basic_stats']['total_synapses']}")
        lines.append(f"  Firing Neurons: {metrics['basic_stats']['firing_neurons']}")
        lines.append(f"  Avg Firing Rate: {metrics['basic_stats']['average_firing_rate']:.2f} Hz")
        lines.append(f"  E/I Balance: {metrics['basic_stats']['excitatory_inhibition_ratio']:.3f}")
        lines.append(f"  Development Stage: {metrics['basic_stats']['developmental_stage']}")
        
        lines.append(f"\nMemory Systems:")
        lines.append(f"  Working Memory Traces: {metrics['memory_systems']['working_memory_active_traces']}")
        lines.append(f"  Episodic Memory Episodes: {metrics['memory_systems']['episodic_memory_episode_count']}")
        
        lines.append(f"\nNeuromodulation:")
        lines.append(f"  Dopamine Level: {metrics['neuromodulation']['dopamine_level']:.3f}")
        lines.append(f"  Curiosity Level: {metrics['neuromodulation']['curiosity_level']:.3f}")
        lines.append(f"  Novelty Level: {metrics['neuromodulation']['novelty_level']:.3f}")
        lines.append(f"  Prediction Error: {metrics['neuromodulation']['prediction_error']:.3f}")
        
        return "\n".join(lines)

# Power user convenience functions

def quick_test_simulation(steps: int = 100, neurons: int = 500):
    """Quick test simulation for development/debugging."""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neurons)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(steps):
        brain.step(step)
        
        if step % 50 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    return brain

def benchmark_comparison(brain_configs: List[Dict], world_config, 
                       steps: int = 1000):
    """Compare performance of different brain configurations."""
    results = []
    
    for i, config_dict in enumerate(brain_configs):
        print(f"Running benchmark {i+1}/{len(brain_configs)}...")
        
        config = pynlm.createDefaultConfig()
        for key, value in config_dict.items():
            config.set(key, value)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        total_firing_rate = 0.0
        for step in range(steps):
            brain.step(step)
            total_firing_rate += brain.getAverageFiringRate()
        
        avg_firing = total_firing_rate / steps
        
        results.append({
            'config': config_dict,
            'average_firing_rate': avg_firing,
            'total_neurons': brain.getTotalNeuronCount(),
            'total_synapses': brain.getTotalSynapseCount()
        })
    
    return results

__all__ = [
    'AdvancedBrainManager',
    'ExperimentRunner', 
    'VisualizationHelper',
    'quick_test_simulation',
    'benchmark_comparison'
]