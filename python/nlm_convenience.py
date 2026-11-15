# NLM Python Convenience Module
"""
Advanced convenience functions for NLM Python users.

This module provides higher-level functions for common operations,
easing the use of the NLM brain system for both beginners and advanced users.
"""

import pynlm
from typing import List, Dict, Optional, Tuple, Any
import json
import os

class NLMBuilder:
    """Builder class for creating complete NLM simulations."""
    
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
        self.brain = None
        self.world = None
        self.agent = None
    
    def set_brain_size(self, neuron_count: int, region_count: int = 1) -> 'NLMBuilder':
        """Set brain size and configuration."""
        self.config.set("neuron_count", neuron_count)
        self.config.set("region_count", region_count)
        return self
    
    def set_plasticity(self, enable: bool = True, hebbian: bool = True, 
                      stdp: bool = True, structural: bool = True) -> 'NLMBuilder':
        """Enable plasticity mechanisms."""
        self.config.set("plasticity.hebbian.enable", hebbian)
        self.config.set("plasticity.stdp.enable", stdp)
        self.config.set("plasticity.structural.enable", structural)
        return self
    
    def set_neuromodulation(self, dopamine: bool = True, curiosity: bool = True,
                           novelty: bool = True, reward: bool = True) -> 'NLMBuilder':
        """Enable neuromodulation systems."""
        self.config.set("neuromod.dopamine.scale", 1.0 if dopamine else 0.0)
        self.config.set("neuromod.curiosity.enable", curiosity)
        self.config.set("neuromod.novelty.enable", novelty)
        return self
    
    def set_developmental(self, enable: bool = True) -> 'NLMBuilder':
        """Enable developmental system."""
        self.config.set("development.enable", enable)
        return self
    
    def set_world(self, width: int = 20, height: int = 20, 
                  vision_width: int = 8, vision_height: int = 8) -> 'NLMBuilder':
        """Configure the world environment."""
        self.world_config = {
            'width': width,
            'height': height,
            'vision_width': vision_width,
            'vision_height': vision_height
        }
        return self
    
    def build(self):
        """Build and return the complete NLM system."""
        # Create brain
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        # Create agent
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Create world
        if hasattr(self, 'world_config'):
            self.world = pynlm.createSimpleWorld()
            self.world.configure(
                self.world_config['width'],
                self.world_config['height'],
                self.world_config['vision_width'],
                self.world_config['vision_height']
            )
            self.world.reset()
            
            # Initialize agent with world
            self.agent.initialize(self.world)
        
        return self
    
    def enable_all_features(self):
        """Enable all advanced features for maximum learning capacity."""
        self.set_plasticity(True, True, True, True)
        self.set_neuromodulation(True, True, True, True)
        self.set_developmental(True)
        return self
class SimulationRunner:
    """Advanced simulation runner with logging and metrics."""
    
    def __init__(self):
        self.step_history = []
        self.reward_history = []
        self.metrics = {}
    
    def run_episode(self, brain, world, agent, max_steps: int = 1000,
                   enable_learning: bool = True) -> Dict[str, Any]:
        """Run a complete simulation episode with detailed tracking."""
        initial_spikes = brain.getTotalSpikeCount()
        initial_firing = brain.getFiringNeuronCount()
        
        for step in range(max_steps):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action to world
            action_result = world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation if enabled
            if enable_learning:
                reward = action_result.reward
                agent.applyRewardModulation(reward, 0.0)
                agent.updateDevelopment(0.1)
            
            # Record metrics
            self.step_history.append({
                'step': step,
                'spikes': brain.getTotalSpikeCount() - initial_spikes,
                'firing_neutrons': brain.getFiringNeuronCount(),
                'action': str(action),
                'reward': action_result.reward if enable_learning else 0.0
            })
            
            if enable_learning:
                self.reward_history.append(action_result.reward)
        
        # Calculate final metrics
        total_spikes = brain.getTotalSpikeCount() - initial_spikes
        final_firing = brain.getFiringNeuronCount()
        
        metrics = {
            'total_spikes': total_spikes,
            'initial_firing': initial_firing,
            'final_firing': final_firing,
            'average_firing': sum(h['firing_neutrons'] for h in self.step_history) / len(self.step_history) if self.step_history else 0,
            'total_reward': sum(h['reward'] for h in self.step_history),
            'steps_completed': max_steps
        }
        
        if enable_learning:
            metrics['average_reward'] = sum(self.reward_history) / len(self.reward_history) if self.reward_history else 0
        
        self.metrics = metrics
        return metrics
    
    def get_step_stats(self) -> Dict[str, float]:
        """Get statistics about the simulation steps."""
        if not self.step_history:
            return {}
        
        spikes = [h['spikes'] for h in self.step_history]
        rewards = [h['reward'] for h in self.step_history if h['reward'] > 0]
        
        return {
            'max_spikes_per_step': max(spikes) if spikes else 0,
            'min_spikes_per_step': min(spikes) if spikes else 0,
            'avg_spikes_per_step': sum(spikes) / len(spikes) if spikes else 0,
            'positive_reward_steps': len(rewards),
            'total_positive_reward': sum(rewards)
        }
class BrainProfiler:
    """Tools for profiling and analyzing brain performance."""
    
    @staticmethod
    def analyze_spike_patterns(brain, window_size: int = 100) -> Dict[str, Any]:
        """Analyze spike patterns and dynamics."""
        if brain.getTotalNeuronCount() == 0:
            return {'error': 'No neurons in brain'}
        
        # Get neuron firing rates
        firing_rates = []
        spike_counts = []
        
        for step in range(window_size):
            current_spikes = brain.getTotalSpikeCount()
            spike_counts.append(current_spikes)
            
            # Simulate one step (for profiling)
            brain.step(step)
            
            firing_rate = brain.getAverageFiringRate()
            firing_rates.append(firing_rate)
        
        # Calculate statistics
        avg_spike_rate = sum(spike_counts) / len(spike_counts) if spike_counts else 0
        avg_firing_rate = sum(firing_rates) / len(firing_rates) if firing_rates else 0
        
        # Detect burst patterns (consecutive high firing)
        bursts = 0
        for i in range(1, len(firing_rates)):
            if firing_rates[i] > 0.5 and firing_rates[i-1] > 0.5:
                bursts += 1
        
        return {
            'average_spike_rate': avg_spike_rate,
            'average_firing_rate': avg_firing_rate,
            'burst_count': bursts,
            'firing_rate_variance': sum((fr - avg_firing_rate) ** 2 for fr in firing_rates) / len(firing_rates) if firing_rates else 0,
            'excitatory_inhibition_ratio': brain.getExcitationInhibitionRatio(),
            'total_neurons': brain.getTotalNeuronCount(),
            'total_synapses': brain.getTotalSynapseCount()
        }
    
    @staticmethod
    def profile_memory_usage(brain) -> Dict[str, Any]:
        """Profile memory system usage."""
        profile = {}
        
        # Working memory
        if hasattr(brain, 'getWorkingMemory') and brain.getWorkingMemory():
            wm = brain.getWorkingMemory()
            profile['working_memory_traces'] = wm.getActiveTraces()
            profile['working_memory_capacity'] = wm.getCapacity()
        
        # Episodic memory
        if hasattr(brain, 'getEpisodicMemory') and brain.getEpisodicMemory():
            em = brain.getEpisodicMemory()
            profile['episodic_memory_episodes'] = em.getEpisodeCount()
            profile['episodic_memory_max_episodes'] = em.getMaxEpisodes()
        
        return profile
    
    @staticmethod
    def compare_brain_states(brain1, brain2, similarity_threshold: float = 0.8) -> Dict[str, Any]:
        """Compare two brain states for similarity."""
        comparison = {
            'neuron_count_match': brain1.getTotalNeuronCount() == brain2.getTotalNeuronCount(),
            'synapse_count_match': brain1.getTotalSynapseCount() == brain2.getTotalSynapseCount(),
            'firing_rate_similarity': 0.0,
            'structural_similarity': 0.0
        }
        
        # Simple similarity metrics
        firing1 = brain1.getAverageFiringRate()
        firing2 = brain2.getAverageFiringRate()
        
        if firing1 > 0 or firing2 > 0:
            comparison['firing_rate_similarity'] = 1.0 - abs(firing1 - firing2) / max(firing1, firing2)
        
        return comparison
class NLMTools:
    """Collection of utility functions for NLM users."""
    
    @staticmethod
    def export_brain_state(brain, filepath: str):
        """Export brain state to file."""
        try:
            brain.save(filepath)
            return True
        except Exception as e:
            print(f"Error exporting brain: {e}")
            return False
    
    @staticmethod
    def import_brain_state(filepath: str, config=None):
        """Import brain state from file."""
        try:
            if config is None:
                config = pynlm.createDefaultConfig()
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            if brain.load(filepath):
                return brain
            else:
                print(f"Error loading brain from {filepath}")
                return None
        except Exception as e:
            print(f"Error importing brain: {e}")
            return None
    
    @staticmethod
    def create_pretrained_brain(model_path: str = None) -> pynlm.Brain:
        """Create a pre-trained brain from a model file."""
        if model_path and os.path.exists(model_path):
            config = pynlm.createDefaultConfig()
            brain = NLMTools.import_brain_state(model_path, config)
            if brain:
                return brain
        
        # Create and train a simple brain if no model available
        print("Creating new brain (no pretrained model available)")
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 500)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.stdp.enable", True)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Brief training
        for i in range(100):
            brain.step(i)
        
        return brain
    
    @staticmethod
    def benchmark_configuration(config, steps: int = 1000) -> Dict[str, Any]:
        """Benchmark a configuration and return performance metrics."""
        import time
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        start_time = time.time()
        
        for i in range(steps):
            brain.step(i)
        
        end_time = time.time()
        
        elapsed = end_time - start_time
        spikes = brain.getTotalSpikeCount()
        
        return {
            'steps_per_second': steps / elapsed,
            'total_spikes': spikes,
            'average_firing_rate': brain.getAverageFiringRate(),
            'excitatory_inhibition_ratio': brain.getExcitationInhibitionRatio(),
            'configuration': config.summary() if hasattr(config, 'summary') else str(config)
        }
    
    @staticmethod
    def validate_configuration(config) -> Tuple[bool, List[str]]:
        """Validate a configuration and return errors if any."""
        errors = []
        
        # Check required keys
        required_keys = ['neuron_count']
        for key in required_keys:
            if not config.has(key):
                errors.append(f"Missing required configuration key: {key}")
        
        # Check value ranges
        if config.has('neuron_count'):
            neuron_count = config.getOr<int64_t>('neuron_count', 0)
            if neuron_count <= 0:
                errors.append(f"Invalid neuron_count: {neuron_count} (must be positive)")
            elif neuron_count > 100000:
                errors.append(f"neuron_count too large: {neuron_count} (recommended max: 100000)")
        
        if config.has('connection_probability'):
            conn_prob = config.getOr<float>('connection_probability', 0.0)
            if conn_prob < 0.0 or conn_prob > 1.0:
                errors.append(f"Invalid connection_probability: {conn_prob} (must be 0.0-1.0)")
        
        return len(errors) == 0, errors