import pynlm
import json
import os
from pathlib import Path

class BrainAnalyzer:
    """
    Utility class for analyzing brain state and statistics.
    
    This provides common analysis functions that are useful for debugging
    and understanding brain behavior.
    """
    
    def __init__(self, brain):
        self.brain = brain
    
    def get_comprehensive_stats(self):
        """Get comprehensive brain statistics."""
        stats = {
            'neuron_count': self.brain.getTotalNeuronCount(),
            'synapse_count': self.brain.getTotalSynapseCount(),
            'active_neuron_count': self.brain.getActiveNeuronCount(),
            'firing_neuron_count': self.brain.getFiringNeuronCount(),
            'total_spike_count': self.brain.getTotalSpikeCount(),
            'pending_spike_count': self.brain.getPendingSpikeEventCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'excitatory_inhibition_ratio': self.brain.getExcitationInhibitionRatio(),
            'developmental_stage': self.brain.getDevelopmentalStage(),
        }
        return stats
    
    def get_activity_summary(self, num_steps=100):
        """Get activity summary over recent steps (placeholder for future implementation)."""
        # This would track activity over time if we had access to step history
        return {
            'recent_firing_count': self.brain.getFiringNeuronCount(),
            'recent_spike_count': self.brain.getTotalSpikeCount(),
            'current_step': "need_step_counter"
        }
    
    def export_to_csv(self, filename):
        """Export brain statistics to CSV format."""
        stats = self.get_comprehensive_stats()
        
        with open(filename, 'w') as f:
            f.write("metric,value\n")
            for key, value in stats.items():
                f.write(f"{key},{value}\n")
        
        return filename
    
    def export_to_json(self, filename):
        """Export brain statistics to JSON format."""
        stats = self.get_comprehensive_stats()
        
        with open(filename, 'w') as f:
            json.dump(stats, f, indent=2)
        
        return filename
    
    def print_formatted_stats(self):
        """Print formatted brain statistics."""
        stats = self.get_comprehensive_stats()
        
        print("=== Brain Statistics ===")
        print(f"Neurons: {stats['neuron_count']:,}")
        print(f"Synapses: {stats['synapse_count']:,}")
        print(f"Active neurons: {stats['active_neuron_count']:,}")
        print(f"Firing neurons: {stats['firing_neuron_count']:,}")
        print(f"Total spikes: {stats['total_spike_count']:,}")
        print(f"Pending spikes: {stats['pending_spike_count']:,}")
        print(f"Average firing rate: {stats['average_firing_rate']:.2f} Hz")
        print(f"E/I ratio: {stats['excitatory_inhibition_ratio']:.3f}")
        print(f"Developmental stage: {stats['developmental_stage']}")

class ConfigManager:
    """
    Utility class for configuration management.
    
    This provides helper functions for working with NLM configurations.
    """
    
    @staticmethod
    def create_preset_config(preset_name):
        """
        Create a configuration preset.
        
        Args:
            preset_name: Name of preset ('small', 'medium', 'large', 'tiny')
        """
        config = pynlm.createDefaultConfig()
        
        presets = {
            'tiny': {
                'brain.neuron_count': 100,
                'brain.synapse_density': 0.05,
            },
            'small': {
                'brain.neuron_count': 500,
                'brain.synapse_density': 0.1,
            },
            'medium': {
                'brain.neuron_count': 1000,
                'brain.synapse_density': 0.1,
            },
            'large': {
                'brain.neuron_count': 5000,
                'brain.synapse_density': 0.05,
            },
            'debug': {
                'brain.neuron_count': 200,
                'random_seed': 42,
                'simulation_timestep': 0.001,
            }
        }
        
        if preset_name.lower() in presets:
            for key, value in presets[preset_name.lower()].items():
                config.set(key, value)
        else:
            print(f"Warning: Unknown preset '{preset_name}', using default")
        
        return config
    
    @staticmethod
    def config_to_dict(config):
        """Convert config to dictionary."""
        keys = config.getKeys()
        config_dict = {}
        
        for key in keys:
            # Try to get common types
            value = config.get(key)
            config_dict[key] = value
        
        return config_dict
    
    @staticmethod
    def dict_to_config(config_dict, config=None):
        """Create config from dictionary."""
        if config is None:
            config = pynlm.createDefaultConfig()
        
        for key, value in config_dict.items():
            config.set(key, value)
        
        return config
    
    @staticmethod
    def print_config_summary(config):
        """Print formatted config summary."""
        keys = config.getKeys()
        
        print("=== Configuration Summary ===")
        print(f"Total settings: {len(keys)}")
        
        for key in keys:
            print(f"{key}")

class WorldUtils:
    """
    Utility class for working with worlds.
    """
    
    @staticmethod
    def create_test_scenario(width=20, height=20, vision_width=8, vision_height=8):
        """Create a standard test scenario world."""
        world = pynlm.createSimpleWorld()
        world.configure(width, height, vision_width, vision_height)
        world.reset()
        
        # Add some interesting objects for exploration
        center_x, center_y = width // 2, height // 2
        
        # Add a resource in the middle
        world.addObject(pynlm.WorldObject(
            center_x, center_y, 
            pynlm.WorldObjectType.Resource, 
            value=10.0, 
            radius=1.0
        ))
        
        # Add a hazard somewhere else
        world.addObject(pynlm.WorldObject(
            center_x + 5, center_y + 5,
            pynlm.WorldObjectType.Hazard,
            value=-5.0,
            radius=0.8
        ))
        
        # Add a marker
        world.addObject(pynlm.WorldObject(
            center_x - 5, center_y - 5,
            pynlm.WorldObjectType.Marker,
            value=0.0,
            radius=0.5
        ))
        
        return world
    
    @staticmethod
    def simulate_agent_with_world(world, agent, brain, num_steps=100):
        """
        Run agent simulation in world.
        
        This is a helper function that combines world, agent, and brain operations.
        """
        for step in range(num_steps):
            # Update world
            world.update(0.1)
            
            # Get percept and process it
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain thinks
            brain.step(step)
            
            # Get action and apply it
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        return {
            'final_brain_stats': {
                'firing_neurons': brain.getFiringNeuronCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'firing_rate': brain.getAverageFiringRate()
            },
            'final_world_time': world.getSimulationTime()
        }

# Convenience functions
def quick_brain_test(num_neurons=100, num_steps=100):
    """Quick test function for creating and running a brain."""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", num_neurons)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(num_steps):
        brain.step(step)
    
    return brain

def create_simple_agent_world(world_size=10, vision_size=4, num_steps=50):
    """Simple agent-world setup for testing."""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 200)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(world_size, world_size, vision_size, vision_size)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
    
    return brain, world, agent

# Batch processing utilities
def batch_brain_creation(num_brains, config=None, steps=50):
    """
    Create and run multiple brains in batch.
    
    Useful for parameter sweeps and comparison studies.
    """
    if config is None:
        config = pynlm.createDefaultConfig()
    
    brains = []
    
    for i in range(num_brains):
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(steps):
            brain.step(step)
        
        brains.append(brain)
    
    return brains

# Experiment utilities
class ExperimentRunner:
    """
    Utility class for running and managing experiments.
    """
    
    def __init__(self, experiment_name):
        self.experiment_name = experiment_name
        self.results = []
    
    def run_trial(self, brain, world=None, agent=None, steps=100):
        """
        Run a single trial with optional world and agent.
        """
        trial_result = {
            'brain_stats': {
                'neuron_count': brain.getTotalNeuronCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'firing_rate': brain.getAverageFiringRate()
            },
            'trial_success': True
        }
        
        if world and agent:
            for step in range(steps):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        
        self.results.append(trial_result)
        return trial_result
    
    def get_summary(self):
        """Get experiment summary statistics."""
        if not self.results:
            return {}
        
        total_spikes = sum(r['brain_stats']['total_spikes'] for r in self.results)
        avg_firing_rate = sum(r['brain_stats']['firing_rate'] for r in self.results) / len(self.results)
        
        return {
            'experiment_name': self.experiment_name,
            'num_trials': len(self.results),
            'total_spikes': total_spikes,
            'average_firing_rate': avg_firing_rate,
            'success_rate': sum(1 for r in self.results if r['trial_success']) / len(self.results)
        }

# Error handling utilities
class NLMError(Exception):
    """Base exception class for NLM errors."""
    pass

class BrainError(NLMError):
    """Exception for brain-related errors."""
    pass

class WorldError(NLMError):
    """Exception for world-related errors."""
    pass

class ConfigError(NLMError):
    """Exception for configuration errors."""
    pass

# Main execution example
if __name__ == "__main__":
    print("NLM Utility Module - Example Usage")
    print("=" * 40)
    
    # Example 1: Quick brain test
    print("\n1. Quick Brain Test")
    brain = quick_brain_test(num_neurons=200, num_steps=100)
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    
    # Example 2: Use brain analyzer
    print("\n2. Brain Analysis")
    analyzer = BrainAnalyzer(brain)
    analyzer.print_formatted_stats()
    
    # Example 3: Configuration management
    print("\n3. Configuration Management")
    config = ConfigManager.create_preset_config("debug")
    print("Created debug configuration")
    ConfigManager.print_config_summary(config)
    
    # Example 4: Create test world
    print("\n4. World Utilities")
    world = WorldUtils.create_test_scenario()
    print(f"Created world {world.getWidth()}x{world.getHeight()} with vision {world.getVisionWidth()}x{world.getVisionHeight()}")
    
    print("\n" + "=" * 40)
    print("Utility module examples completed successfully!")
