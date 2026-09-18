import pynlm
from typing import Optional, List, Dict, Any
import json

class NLM:
    """
    High-level interface for NLM brain simulation with enhanced usability.
    Provides improved API over raw pynlm with better defaults and convenience methods.
    
    Features:
    - Simplified configuration
    - Built-in experiment management
    - Enhanced memory tracking
    - Development lifecycle management
    - Advanced visualization utilities
    - Performance monitoring
    """
    
    def __init__(self, config: Optional[Dict[str, Any]] = None):
        """
        Initialize NLM with improved defaults.
        
        Args:
            config: Configuration dictionary. If None, uses optimized defaults
                   for general-purpose simulation.
        """
        import pynlm
        
        if config is None:
            config = self._get_default_config()
        
        self.config = pynlm.createDefaultConfig()
        for key, value in config.items():
            if isinstance(value, dict):
                for subkey, subvalue in value.items():
                    self.config.set(f"{key}.{subkey}", subvalue)
            else:
                self.config.set(key, value)
        
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.experiment_data = {
            'steps': [],
            'stats': [],
            'memories': [],
            'rewards': []
        }
        
        self.development_stage = pynlm.DevelopmentalStage.Initial
        self.episode_start_time = 0.0
        
    def _get_default_config(self) -> Dict[str, Any]:
        """Optimized defaults for general-purpose simulation."""
        return {
            'brain': {
                'neuron_count': 2000,
                'region_count': 2,
                'connection_probability': 0.1,
                'v_thresh': -50.0,
                'v_reset': -75.0,
            },
            'plasticity': {
                'stdp': {
                    'enable': True,
                    'learning_rate': 0.001,
                },
                'hebbian': {
                    'enable': True,
                },
            },
            'neuromod': {
                'dopamine': {
                    'scale': 1.0,
                },
                'curiosity': {
                    'enable': True,
                },
                'novelty': {
                    'enable': True,
                },
            },
            'memory': {
                'working_memory': {
                    'capacity': 200,
                },
                'episodic': {
                    'max_episodes': 1000,
                },
            },
            'development': {
                'maturation_rate': 0.1,
            },
        }
    
    def step(self, time_step: float = 0.1) -> Dict[str, Any]:
        """
        Execute one simulation step with enhanced logging.
        
        Returns:
            Dictionary with step statistics and performance metrics.
        """
        step_data = {
            'time': self.experiment_data['steps'][-1] + time_step if self.experiment_data['steps'] else time_step,
            'firing_rate': self.brain.getFiringNeuronCount(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'memory_usage': self._get_memory_stats(),
            'development': self._get_development_stats(),
            'neuromodulation': self._get_neuromodulation_stats(),
        }
        
        self.experiment_data['steps'].append(step_data['time'])
        self.experiment_data['stats'].append(step_data)
        
        self.brain.step(len(self.experiment_data['steps']) - 1)
        
        return step_data
    
    def run_episode(self, num_steps: int, world=None, agent=None) -> List[Dict[str, Any]]:
        """
        Run a complete episode with automatic memory management.
        
        Args:
            num_steps: Number of steps to run
            world: Optional world for embodied agents
            agent: Optional agent for world interaction
            
        Returns:
            List of step data
        """
        episode_start = len(self.experiment_data['steps'])
        
        for i in range(num_steps):
            if world and agent:
                # Embodied agent simulation
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                self.brain.step(i)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
                agent.applyRewardModulation(reward, 0.0)
                agent.updateDevelopment(0.1)
                
                self.experiment_data['rewards'].append(reward)
                
                # Store episode memory
                self._store_episode_memory(world, agent)
            else:
                # Pure brain simulation
                step_data = self.step(0.1)
            
            # Update development
            if hasattr(self.brain, 'updateDevelopment'):
                self.brain.updateDevelopment(0.1)
        
        return self.experiment_data['stats'][episode_start:]
    
    def enable_advanced_features(self):
        """Enable advanced features for research and experimentation."""
        # Enable all subsystems
        if hasattr(self.brain, 'getWorkingMemory') and self.brain.getWorkingMemory():
            self.brain.getWorkingMemory().setCapacity(500)
        
        if hasattr(self.brain, 'getEpisodicMemory') and self.brain.getEpisodicMemory():
            self.brain.getEpisodicMemory().setMaxEpisodes(5000)
        
        # Enable development
        self.brain.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)
        
        # Enable all neuromodulation
        for mod in ['dopamine', 'curiosity', 'novelty']:
            if hasattr(self.brain, f'get{mod.title()}') and getattr(self.brain, f'get{mod.title()}')():
                getattr(self.brain, f'get{mod.title()}')().setLevel(1.0)
    
    def save_brain_state(self, filepath: str) -> bool:
        """Save brain state with improved error handling."""
        try:
            return self.brain.save(filepath)
        except Exception as e:
            print(f"Error saving brain state: {e}")
            return False
    
    def load_brain_state(self, filepath: str) -> bool:
        """Load brain state with improved error handling."""
        try:
            return self.brain.load(filepath)
        except Exception as e:
            print(f"Error loading brain state: {e}")
            return False
    
    def get_system_status(self) -> Dict[str, Any]:
        """Get comprehensive system status."""
        status = {
            'brain': {
                'neurons': self.brain.getTotalNeuronCount(),
                'synapses': self.brain.getTotalSynapseCount(),
                'firing': self.brain.getFiringNeuronCount(),
                'spikes': self.brain.getTotalSpikeCount(),
                'development': str(self.brain.getDevelopmentalStage()),
            },
            'memory': self._get_memory_stats(),
            'performance': self._get_performance_stats(),
        }
        
        return status
    
    def _get_memory_stats(self) -> Dict[str, Any]:
        """Get memory system statistics."""
        stats = {}
        
        if self.brain.getWorkingMemory():
            stats['working_memory'] = {
                'active_traces': self.brain.getWorkingMemory().getActiveTraces(),
                'capacity': self.brain.getWorkingMemory().getCapacity(),
            }
        
        if self.brain.getEpisodicMemory():
            stats['episodic_memory'] = {
                'episodes': self.brain.getEpisodicMemory().getEpisodeCount(),
                'max_episodes': self.brain.getEpisodicMemory().getMaxEpisodes(),
            }
        
        if self.brain.getPredictionSystem():
            stats['prediction_system'] = {
                'active': True,
                'error_history': len(self.experiment_data['rewards']),
            }
        
        return stats
    
    def _get_development_stats(self) -> Dict[str, Any]:
        """Get development system statistics."""
        return {
            'stage': str(self.development_stage),
            'age': getattr(self, 'developmental_age', 0.0),
            'plasticity_modifier': getattr(self, 'plasticity_modifier', 1.0),
        }
    
    def _get_neuromodulation_stats(self) -> Dict[str, Any]:
        """Get neuromodulation statistics."""
        stats = {}
        
        if self.brain.getDopamine():
            stats['dopamine'] = self.brain.getDopamine().getLevel()
        
        if self.brain.getCuriosity():
            stats['curiosity'] = self.brain.getCuriosity().getLevel()
        
        if self.brain.getNovelty():
            stats['novelty'] = self.brain.getNovelty().getLevel()
        
        return stats
    
    def _get_performance_stats(self) -> Dict[str, Any]:
        """Get performance statistics."""
        return {
            'avg_firing_rate': self.brain.getAverageFiringRate(),
            'e_i_ratio': self.brain.getExcitationInhibitionRatio(),
            'memory_efficiency': self._calculate_memory_efficiency(),
        }
    
    def _calculate_memory_efficiency(self) -> float:
        """Calculate memory efficiency metric."""
        if not self.experiment_data['steps']:
            return 0.0
        
        total_steps = len(self.experiment_data['steps'])
        firing_efficiency = self.brain.getFiringNeuronCount() / max(1, self.brain.getTotalNeuronCount())
        
        return firing_efficiency * 0.7 + (1.0 / max(1, total_steps)) * 0.3
    
    def _store_episode_memory(self, world, agent):
        """Store current episode in episodic memory."""
        if not self.brain.getEpisodicMemory():
            return
        
        episode = {
            'time': len(self.experiment_data['steps']),
            'world_state': self._get_world_state(world),
            'brain_state': self._get_brain_state(),
            'action': self._get_last_action(agent),
            'reward': self.experiment_data['rewards'][-1] if self.experiment_data['rewards'] else 0.0,
        }
        
        self.experiment_data['memories'].append(episode)
    
    def _get_world_state(self, world) -> Dict[str, Any]:
        """Get simplified world state."""
        return {
            'position': getattr(world, 'getAgentPosition', lambda: (0, 0))(),
            'vision': getattr(world, 'getVision', lambda: [])(),
        }
    
    def _get_brain_state(self) -> Dict[str, Any]:
        """Get simplified brain state."""
        return {
            'active_neurons': self.brain.getFiringNeuronCount(),
            'development': str(self.brain.getDevelopmentalStage()),
            'spike_count': self.brain.getTotalSpikeCount(),
        }
    
    def _get_last_action(self, agent) -> Dict[str, Any]:
        """Get last action from agent."""
        return {
            'type': getattr(agent, 'getLastAction', lambda: 'None')(),
            'value': getattr(agent, 'getLastActionValue', lambda: 0.0)(),
        }
    
    def reset_experiment(self):
        """Reset all experiment data."""
        self.experiment_data = {
            'steps': [],
            'stats': [],
            'memories': [],
            'rewards': []
        }
        self.brain.reset()
    
    def __enter__(self):
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        self.reset_experiment()
