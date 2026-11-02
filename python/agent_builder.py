# AgentBuilder - High-level agent creation and configuration

This module provides tools for building and configuring intelligent agents
with NLM brains, including world integration, sensory processing, and motor output.
"""

import time
from typing import Optional, List, Dict, Any, Callable
from dataclasses import dataclass
import numpy as np

from .enhanced_api import NLMSystem, SimulationStats

@dataclass
class AgentConfig:
    """Configuration for agent creation"""
    brain_config: Any = None
    world_config: Dict[str, Any] = field(default_factory=dict)
    learning_enabled: bool = True
    development_enabled: bool = True
    curiosity_enabled: bool = True
    reward_modulation_enabled: bool = True
    structural_plasticity_enabled: bool = True
    num_sensory_channels: int = 4
    num_motor_channels: int = 6
    timestep: float = 0.1
    max_energy: float = 100.0
    energy_decay_rate: float = 0.01

class AgentBuilder:
    """
    Builder for creating intelligent agents with NLM brains.
    
    This class provides a high-level interface for creating agents that can
    interact with environments, learn from experience, and adapt their behavior.
    
    Examples:
        >>> # Basic agent
        >>> builder = nlm.AgentBuilder()
        >>> agent = builder.create_agent("simple", brain_config)
        >>> 
        >>> # Learning agent with world integration
        >>> learning_builder = nlm.AgentBuilder()
        >>> learning_builder.enable_learning(True)
        >>> learning_builder.enable_development(True)
        >>> learning_builder.enable_curiosity(True)
        >>> agent = learning_builder.create_agent("world")
        >>> 
        >>> # Agent with custom sensory/motor configuration
        >>> custom_builder = nlm.AgentBuilder()
        >>> custom_builder.set_sensory_channels(8, 8, 4, 6)
        >>> custom_builder.set_motor_channels(10)
        >>> agent = custom_builder.create_agent("custom")
    """
    
    def __init__(self, config: Optional[AgentConfig] = None):
        """
        Initialize the agent builder.
        
        Args:
            config: Optional agent configuration
        """
        self.config = config or AgentConfig()
        self.world = None
        self.agent_brain = None
        self.simulation_system = None
        
    def enable_learning(self, enabled: bool = True) -> 'AgentBuilder':
        """Enable or disable learning capabilities."""
        self.config.learning_enabled = enabled
        return self
    
    def enable_development(self, enabled: bool = True) -> 'AgentBuilder':
        """Enable or disable development capabilities."""
        self.config.development_enabled = enabled
        return self
    
    def enable_curiosity(self, enabled: bool = True) -> 'AgentBuilder':
        """Enable or disable curiosity-driven exploration."""
        self.config.curiosity_enabled = enabled
        return self
    
    def enable_reward_modulation(self, enabled: bool = True) -> 'AgentBuilder':
        """Enable or disable reward modulation."""
        self.config.reward_modulation_enabled = enabled
        return self
    
    def enable_structural_plasticity(self, enabled: bool = True) -> 'AgentBuilder':
        """Enable or disable structural plasticity."""
        self.config.structural_plasticity_enabled = enabled
        return self
    
    def set_sensory_channels(self, vision_width: int, vision_height: int, 
                           touch_channels: int = 8, internal_channels: int = 4) -> 'AgentBuilder':
        """
        Set sensory channel configuration.
        
        Args:
            vision_width: Width of vision input
            vision_height: Height of vision input
            touch_channels: Number of touch channels
            internal_channels: Number of internal state channels
        """
        # Calculate total sensory channels
        total_channels = vision_width * vision_height + touch_channels + internal_channels
        self.config.num_sensory_channels = total_channels
        self.config.world_config.update({
            'visionWidth': vision_width,
            'visionHeight': vision_height,
            'touchChannels': touch_channels,
            'internalChannels': internal_channels
        })
        return self
    
    def set_motor_channels(self, motor_channels: int) -> 'AgentBuilder':
        """Set motor channel configuration."""
        self.config.num_motor_channels = motor_channels
        return self
    
    def set_timestep(self, timestep: float) -> 'AgentBuilder':
        """Set simulation timestep."""
        self.config.timestep = timestep
        return self
    
    def set_energy_parameters(self, max_energy: float, decay_rate: float) -> 'AgentBuilder':
        """Set energy system parameters."""
        self.config.max_energy = max_energy
        self.config.energy_decay_rate = decay_rate
        self.config.world_config.update({
            'maxEnergy': max_energy,
            'energyDecayRate': decay_rate
        })
        return self
    
    def create_agent(self, agent_type: str = "basic") -> Dict[str, Any]:
        """
        Create an agent with the configured settings.
        
        Args:
            agent_type: Type of agent to create ('basic', 'learning', 'world', 'custom')
            
        Returns:
            Dictionary containing agent components (brain, world, agent interface)
            
        Raises:
            ConfigurationError: If agent creation fails
        """
        try:
            import pynlm as nlm
            
            # Create brain
            brain_config = self.config.brain_config or nlm.createDefaultConfig()
            brain = nlm.createBrain(brain_config)
            
            # Apply agent-specific configurations
            self._apply_agent_config(brain, agent_type)
            
            # Initialize brain
            brain.initialize()
            
            # Create world if needed
            world = None
            if agent_type in ['world', 'custom']:
                world = self._create_world()
            
            # Create agent brain interface
            agent_brain_interface = nlm.createAgentBrain(brain)
            
            if world:
                agent_brain_interface.initialize(world)
            
            # Create enhanced system
            system = NLMSystem(brain)
            system.initialize()
            
            return {
                'brain': brain,
                'world': world,
                'agent_brain': agent_brain_interface,
                'system': system,
                'config': self.config
            }
            
        except Exception as e:
            raise Exception(f"Failed to create agent: {e}")
    
    def create_agent_session(self, agent_type: str, num_steps: int) -> Dict[str, Any]:
        """
        Create and run an agent simulation session.
        
        Args:
            agent_type: Type of agent to create
            num_steps: Number of simulation steps to run
            
        Returns:
            Dictionary containing session results
        """
        try:
            # Create agent
            agent = self.create_agent(agent_type)
            
            # Run simulation
            world = agent['world']
            system = agent['system']
            agent_brain = agent['agent_brain']
            
            session_results = {
                'agent': agent,
                'steps_completed': 0,
                'total_reward': 0.0,
                'learning_curve': [],
                'behavior_trace': [],
                'performance_metrics': {}
            }
            
            for step in range(num_steps):
                # Update world
                if world:
                    world.update(self.config.timestep)
                
                # Get sensory input
                percept = world.getSensoryPercept() if world else None
                
                # Process sensory input
                if percept and agent_brain:
                    agent_brain.processSensoryInput(percept)
                
                # Run brain simulation
                stats = system.step()
                
                # Record metrics
                session_results['steps_completed'] += 1
                session_results['learning_curve'].append({
                    'step': step,
                    'firing_rate': stats.firing_rate,
                    'learning_efficiency': stats.learning_rate
                })
                
                # Decode motor command
                if agent_brain:
                    action = agent_brain.decodeMotorCommand()
                    session_results['behavior_trace'].append({
                        'step': step,
                        'action': str(action),
                        'novelty_level': agent_brain.getNoveltyLevel(),
                        'curiosity_level': agent_brain.getCuriosityLevel()
                    })
                
                # Apply reward if world exists
                if world:
                    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
                    session_results['total_reward'] += reward
                    if agent_brain:
                        agent_brain.applyRewardModulation(reward, 0.0)
            
            # Calculate performance metrics
            session_results['performance_metrics'] = {
                'average_firing_rate': np.mean([s['firing_rate'] for s in session_results['learning_curve']]),
                'final_firing_rate': session_results['learning_curve'][-1]['firing_rate'] if session_results['learning_curve'] else 0,
                'total_reward_per_step': session_results['total_reward'] / max(1, num_steps),
                'adaptation_score': calculate_adaptation_score(session_results['learning_curve'])
            }
            
            return session_results
            
        except Exception as e:
            raise Exception(f"Failed to create agent session: {e}")
    
    def _create_world(self):
        """Create and configure the world for the agent."""
        try:
            import pynlm as nlm
            
            world = nlm.createSimpleWorld()
            
            # Apply world configuration
            if self.config.world_config:
                world.configure(
                    width=self.config.world_config.get('width', 20),
                    height=self.config.world_config.get('height', 20),
                    visionWidth=self.config.world_config.get('visionWidth', 8),
                    visionHeight=self.config.world_config.get('visionHeight', 8)
                )
                
                if 'maxEnergy' in self.config.world_config:
                    world.setMaxEnergy(self.config.world_config['maxEnergy'])
                if 'energyDecayRate' in self.config.world_config:
                    world.setEnergyDecayRate(self.config.world_config['energyDecayRate'])
            
            return world
            
        except Exception as e:
            raise Exception(f"Failed to create world: {e}")
    
    def _apply_agent_config(self, brain: Any, agent_type: str):
        """Apply agent-specific configuration to brain."""
        try:
            # Enable/disable subsystems based on agent type and config
            if not self.config.learning_enabled:
                # Disable learning components
                pass
                
            if not self.config.development_enabled:
                # Disable development components
                pass
                
            if not self.config.curiosity_enabled:
                # Disable curiosity components
                pass
                
        except Exception as e:
            # Log but don't fail if config application fails
            pass
    
    def get_agent_presets() -> Dict[str, Dict[str, Any]]:
        """
        Get predefined agent configuration presets.
        
        Returns:
            Dictionary of agent presets
        """
        return {
            'simple': {
                'learning_enabled': False,
                'development_enabled': False,
                'curiosity_enabled': False,
                'reward_modulation_enabled': True,
                'structural_plasticity_enabled': False,
                'brain_config': {
                    'brain.neuron_count': 200,
                    'brain.synapse_density': 0.05,
                    'learning.plasticity_rate': 0.01
                }
            },
            'learning': {
                'learning_enabled': True,
                'development_enabled': True,
                'curiosity_enabled': True,
                'reward_modulation_enabled': True,
                'structural_plasticity_enabled': True,
                'brain_config': {
                    'brain.neuron_count': 500,
                    'brain.synapse_density': 0.1,
                    'learning.plasticity_rate': 0.1
                }
            },
            'exploratory': {
                'learning_enabled': True,
                'development_enabled': True,
                'curiosity_enabled': True,
                'reward_modulation_enabled': True,
                'structural_plasticity_enabled': True,
                'brain_config': {
                    'brain.neuron_count': 300,
                    'brain.synapse_density': 0.08,
                    'learning.plasticity_rate': 0.05,
                    'neuromodulation.curiosity_weight': 1.0
                }
            }
        }