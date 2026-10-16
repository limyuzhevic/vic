# Improved NLM Python API
# Enhanced with more Pythonic interfaces and advanced features

import pynlm
from typing import Optional, List, Dict, Any, Callable, Union
import numpy as np

class AdvancedBrain:
    """Enhanced brain interface with advanced Python features"""
    
    def __init__(self, config=None):
        """Initialize advanced brain with optional configuration"""
        if config is None:
            config = pynlm.createDefaultConfig()
        self._brain = pynlm.createBrain(config)
        self._brain.initialize()
    
    def simulate(self, steps: int, callback: Optional[Callable] = None):
        """Run simulation with optional callback for each step"""
        for step in range(steps):
            self._brain.step(step)
            if callback:
                callback(step, self.get_stats())
    
    def get_stats(self) -> Dict[str, Any]:
        """Get comprehensive brain statistics as Python dict"""
        return {
            'total_neurons': self._brain.getTotalNeuronCount(),
            'total_synapses': self._brain.getTotalSynapseCount(),
            'firing_neurons': self._brain.getFiringNeuronCount(),
            'average_firing_rate': self._brain.getAverageFiringRate(),
            'excitation_inhibition_ratio': self._brain.getExcitationInhibitionRatio(),
            'development_stage': self._brain.getDevelopmentalStage(),
            'region_count': self._brain.getRegionCount()
        }
    
    def enable_learning(self, stdp=True, hebbian=True, structural=True):
        """Enable learning subsystems"""
        if structural:
            self._agent.enableStructuralPlasticity(True)
        # Note: This would need proper agent integration
        return self
    
    def connect_regions(self, source_region: str, target_region: str, 
                       weight: float = 1.0, delay: int = 1):
        """Connect two brain regions"""
        # This would need to be implemented with region management
        return self

class InteractiveAgent:
    """Enhanced agent with interactive features"""
    
    def __init__(self, brain, world):
        self._brain = brain
        self._world = world
        self._agent = pynlm.createAgentBrain(brain)
        self._agent.initialize(world)
        self._observation_history = []
        self._action_history = []
        self._reward_history = []
    
    def step(self, perception=None):
        """Take an agent step with optional external perception"""
        if perception is None:
            perception = self._world.getSensoryPercept()
        
        self._agent.processSensoryInput(perception)
        self._brain.step(len(self._observation_history))
        
        action = self._agent.decodeMotorCommand()
        self._world.applyMotorCommand(action, self._world.getSimulationTime())
        
        # Store history for analysis
        self._observation_history.append(perception)
        self._action_history.append(action)
        
        return action
    
    def apply_reward(self, reward: float, predicted: float = 0.0):
        """Apply reward modulation with optional prediction"""
        self._agent.applyRewardModulation(reward, predicted)
        self._reward_history.append(reward)
    
    def get_experience(self) -> Dict[str, Any]:
        """Get agent's experience as structured data"""
        return {
            'observations': self._observation_history,
            'actions': self._action_history,
            'rewards': self._reward_history,
            'stats': self._brain.get_stats()
        }

class NLMConfigurator:
    """Advanced configuration manager with validation and presets"""
    
    @staticmethod
    def get_presets() -> Dict[str, Dict[str, Any]]:
        """Get built-in configuration presets"""
        return {
            'minimal': {
                'brain.neuron_count': 100,
                'brain.synapse_density': 0.05,
                'plasticity.stdp.enable': True,
                'neuromod.dopamine.scale': 1.0
            },
            'research': {
                'brain.neuron_count': 1000,
                'brain.synapse_density': 0.1,
                'plasticity.stdp.enable': True,
                'plasticity.stdp.learning_rate': 0.01,
                'plasticity.hebbian.enable': True,
                'neuromod.dopamine.scale': 2.0,
                'neuromod.curiosity.enable': True,
                'neuromod.novelty.enable': True
            },
            'production': {
                'brain.neuron_count': 500,
                'brain.synapse_density': 0.08,
                'plasticity.stdp.enable': True,
                'plasticity.hebbian.enable': True,
                'neuromod.curiosity.enable': False,
                'neuromod.novelty.enable': False
            }
        }
    
    @staticmethod
    def create_config(preset: str = 'minimal', **kwargs) -> pynlm.Config:
        """Create configuration with preset and overrides"""
        config = pynlm.createDefaultConfig()
        
        # Apply preset
        if preset in NLMConfigurator.get_presets():
            for key, value in NLMConfigurator.get_presets()[preset].items():
                config.set(key, value)
        
        # Apply overrides
        for key, value in kwargs.items():
            config.set(key, value)
        
        return config

class NLMExperiment:
    """Experiment management with systematic testing"""
    
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.results = []
    
    def run_episode(self, max_steps: int, target_objective: Optional[Callable] = None) -> Dict[str, Any]:
        """Run a single episode with objective tracking"""
        for step in range(max_steps):
            self.world.update(0.1)
            action = self.agent.step()
            
            if target_objective:
                objective = target_objective(self)
                if objective is not None:
                    self.agent.apply_reward(objective)
        
        episode_result = {
            'steps': len(range(max_steps)),
            'brain_stats': self.brain.get_stats(),
            'experience': self.agent.get_experience()
        }
        
        self.results.append(episode_result)
        return episode_result
    
    def run_experiment(self, episodes: int, steps_per_episode: int) -> Dict[str, Any]:
        """Run multiple episodes with aggregate results"""
        for episode in range(episodes):
            self.run_episode(steps_per_episode)
            print(f"Episode {episode + 1}/{episodes} completed")
        
        # Aggregate results
        aggregated = {
            'total_episodes': episodes,
            'average_neurons': np.mean([r['brain_stats']['total_neurons'] for r in self.results]),
            'average_firing_rate': np.mean([r['brain_stats']['average_firing_rate'] for r in self.results]),
            'total_spikes': sum([r['brain_stats']['total_synapses'] for r in self.results])
        }
        
        return aggregated

def create_advanced_brain(config=None, **kwargs) -> AdvancedBrain:
    """Factory function for advanced brain creation"""
    return AdvancedBrain(config)

def create_interactive_agent(brain, world) -> InteractiveAgent:
    """Factory function for interactive agent creation"""
    return InteractiveAgent(brain, world)

def run_research_experiment(config_preset: str = 'research', episodes: int = 10):
    """High-level function for running research experiments"""
    # Create components
    config = NLMConfigurator.create_config(config_preset)
    brain = AdvancedBrain(config)
    
    # Create world and agent (simplified)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = InteractiveAgent(brain, world)
    
    # Run experiment
    experiment = NLMExperiment(brain, world, agent)
    results = experiment.run_experiment(episodes, 100)
    
    print(f"Research experiment completed: {results}")
    return experiment

# Utility functions

def validate_brain_state(brain) -> List[str]:
    """Validate brain state and return list of issues"""
    issues = []
    
    stats = brain.get_stats()
    if stats['total_neurons'] < 10:
        issues.append("Brain has too few neurons for meaningful simulation")
    
    if stats['average_firing_rate'] < 0.01:
        issues.append("Brain firing rate is extremely low - may need more input")
    
    return issues

def benchmark_brain(brain, steps: int = 1000) -> Dict[str, float]:
    """Benchmark brain performance"""
    import time
    
    start_time = time.time()
    brain.simulate(steps)
    end_time = time.time()
    
    stats = brain.get_stats()
    
    return {
        'simulation_time': end_time - start_time,
        'steps_per_second': steps / (end_time - start_time),
        'neurons_per_second': stats['total_neurons'] * steps / (end_time - start_time),
        'final_firing_rate': stats['average_firing_rate']
    }

# Example usage
if __name__ == "__main__":
    print("=== NLM Advanced API Demo ===")
    
    # Create advanced brain
    brain = AdvancedBrain()
    print(f"Created brain with {brain.get_stats()['total_neurons']} neurons")
    
    # Run simulation with callback
    def step_callback(step, stats):
        if step % 100 == 0:
            print(f"Step {step}: {stats['firing_neurons']} neurons firing")
    
    brain.simulate(500, step_callback)
    
    print("=== Advanced API Demo Complete ===")
