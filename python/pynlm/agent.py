"""
Agent brain module for NLM Python bindings.
"""

import time
import math
from typing import Dict, List, Any, Optional, Tuple, Callable
from enum import Enum

try:
    from pynlm.bindings import AgentBrain, createAgentBrain, Brain, createBrain
    from pynlm.bindings import createSimpleWorld, createDefaultConfig
    from pynlm.bindings import Vision, Audio, InternalSignals
    from pynlm.bindings import Action, ActionType, MotorCommand
    BINDINGS_AVAILABLE = True
except ImportError:
    BINDINGS_AVAILABLE = False
    print("Warning: C++ bindings not available. Agent module limited.")

# Import exceptions and utilities
from .exceptions import NLMInitializationError, NLMExecutionError, NLMConfigurationError
from .core import configure_default, calculate_statistics, format_number


class AgentStatus(Enum):
    """Agent status states."""
    IDLE = "idle"
    RUNNING = "running"
    PAUSED = "paused"
    LEARNING = "learning"
    EXPLORATION = "exploration"
    RESTING = "resting"
    ERROR = "error"


class AgentSession:
    """Context manager for agent training/testing sessions."""
    
    def __init__(self, brain=None, world=None, agent=None, max_steps=1000):
        """
        Initialize agent session.
        
        Args:
            brain: Brain object (if None, will be created)
            world: World object (if None, will be created)
            agent: AgentBrain object (if None, will be created)
            max_steps: Maximum number of simulation steps
        """
        self.brain = brain
        self.world = world
        self.agent = agent
        self.max_steps = max_steps
        self.current_step = 0
        self.is_running = False
        self.status = AgentStatus.IDLE
        self.session_stats = {
            'steps_completed': 0,
            'total_rewards': 0.0,
            'start_time': 0.0,
            'end_time': 0.0,
            'curiosity_scores': [],
            'novelty_scores': [],
            'prediction_errors': [],
        }
        
        if not BINDINGS_AVAILABLE:
            raise NLMInitializationError("C++ bindings not available")
        
        # Create components if not provided
        if brain is None:
            config = configure_default()
            self.brain = createBrain(config)
            self.brain.initialize()
        
        if world is None:
            self.world = createSimpleWorld()
            self.world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            self.world.reset()
        
        if agent is None:
            self.agent = createAgentBrain(self.brain)
            self.agent.initialize(self.world)
    
    def __enter__(self):
        self.start()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        self.stop()
    
    def start(self, enable_learning=True, enable_curiosity=True, enable_novelty=True):
        """
        Start agent session.
        
        Args:
            enable_learning: Enable reward-based learning
            enable_curiosity: Enable curiosity-driven exploration
            enable_novelty: Enable novelty detection
        """
        if self.is_running:
            return
        
        self.is_running = True
        self.status = AgentStatus.RUNNING if enable_learning and enable_curiosity else AgentStatus.EXPLORATION
        self.current_step = 0
        self.session_stats['steps_completed'] = 0
        self.session_stats['start_time'] = time.time()
        self.session_stats['total_rewards'] = 0.0
        
        # Enable subsystems
        if enable_learning:
            self.agent.enableRewardModulation(True)
        if enable_curiosity:
            self.agent.enableCuriosity(True)
        if enable_novelty:
            # Novelty is usually enabled by default
            pass
        
        self._trigger_event('session_started', {
            'enable_learning': enable_learning,
            'enable_curiosity': enable_curiosity,
        })
    
    def step(self, step_number: int = 0, timestep: float = 0.1):
        """
        Execute one agent step.
        
        Args:
            step_number: Step number
            timestep: Time duration for this step
        """
        if not self.is_running:
            raise NLMExecutionError("Agent session not running. Call start() first.")
        
        try:
            # Update world
            self.world.update(timestep)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Run brain step
            self.brain.step(step_number)
            
            # Decode motor command
            motor_cmd = self.agent.decodeMotorCommand()
            
            # Apply motor command
            self.world.applyMotorCommand(motor_cmd, self.world.getSimulationTime())
            
            # Apply reward modulation
            reward = 0.0
            if percept.getInternal():
                reward = percept.getInternal()[0]
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(timestep)
            
            # Record statistics
            self.session_stats['steps_completed'] += 1
            self.session_stats['total_rewards'] += reward
            
            # Record agent scores
            if self.agent.isCuriosityEnabled():
                self.session_stats['curiosity_scores'].append(
                    self.agent.getCuriosityLevel()
                )
            if self.agent.isCuriosityEnabled():  # Novelty is often tied to curiosity
                self.session_stats['novelty_scores'].append(
                    self.agent.getNoveltyLevel()
                )
            self.session_stats['prediction_errors'].append(
                self.agent.getPredictionError()
            )
            
            self.current_step += 1
            
            self._trigger_event('step_completed', {
                'step': self.current_step,
                'reward': reward,
                'curiosity': self.agent.getCuriosityLevel(),
                'novelty': self.agent.getNoveltyLevel(),
                'prediction_error': self.agent.getPredictionError(),
            })
            
        except Exception as e:
            self.status = AgentStatus.ERROR
            self._trigger_event('session_error', str(e))
            raise NLMExecutionError(f"Agent step {self.current_step} failed: {e}")
    
    def run(self, steps: int = 100, timestep: float = 0.1):
        """
        Run agent session for specified number of steps.
        
        Args:
            steps: Number of steps to run
            timestep: Time per step
        """
        if self.current_step + steps > self.max_steps:
            raise NLMExecutionError(f"Would exceed maximum steps ({self.max_steps})")
        
        for i in range(steps):
            step_num = self.current_step + i
            self.step(step_num, timestep)
    
    def stop(self):
        """Stop agent session."""
        if self.is_running:
            self.is_running = False
            self.status = AgentStatus.IDLE
            self.session_stats['end_time'] = time.time()
            self._trigger_event('session_stopped', self.session_stats)
    
    def get_session_stats(self) -> Dict[str, Any]:
        """Get session statistics."""
        stats = self.session_stats.copy()
        
        # Add brain statistics
        if self.brain:
            stats['brain'] = {
                'total_neurons': self.brain.getTotalNeuronCount(),
                'total_synapses': self.brain.getTotalSynapseCount(),
                'active_neurons': self.brain.getActiveNeuronCount(),
                'firing_neurons': self.brain.getFiringNeuronCount(),
                'average_firing_rate': self.brain.getAverageFiringRate(),
                'total_spikes': self.brain.getTotalSpikeCount(),
                'development_stage': str(self.brain.getDevelopmentalStage()),
            }
        
        # Add agent statistics
        if self.agent:
            stats['agent'] = {
                'neuromodulation_level': self.agent.getNeuromodulationLevel(),
                'curiosity_level': self.agent.getCuriosityLevel(),
                'novelty_level': self.agent.getNoveltyLevel(),
                'prediction_error': self.agent.getPredictionError(),
                'development_stage': str(self.agent.getDevelopmentalStage()),
                'reward_modulation_enabled': self.agent.isRewardModulationEnabled(),
                'structural_plasticity_enabled': self.agent.isStructuralPlasticityEnabled(),
                'development_enabled': self.agent.isDevelopmentEnabled(),
                'curiosity_enabled': self.agent.isCuriosityEnabled(),
            }
        
        # Add world statistics
        if self.world:
            body = self.world.getAgentBody()
            stats['world'] = {
                'width': self.world.getWidth(),
                'height': self.world.getHeight(),
                'simulation_time': self.world.getSimulationTime(),
                'agent_x': body.x,
                'agent_y': body.y,
                'agent_energy': body.energy,
                'agent_health': body.health,
            }
        
        # Calculate derived statistics
        if stats['start_time'] and stats['end_time']:
            duration = stats['end_time'] - stats['start_time']
            stats['duration'] = duration
            if duration > 0:
                stats['steps_per_second'] = stats['steps_completed'] / duration
                stats['average_reward_per_step'] = stats['total_rewards'] / stats['steps_completed']
        
        # Calculate summary statistics for agent scores
        if stats['curiosity_scores']:
            stats['curiosity_summary'] = calculate_statistics(stats['curiosity_scores'])
        if stats['novelty_scores']:
            stats['novelty_summary'] = calculate_statistics(stats['novelty_scores'])
        if stats['prediction_errors']:
            stats['prediction_error_summary'] = calculate_statistics(stats['prediction_errors'])
        
        return stats
    
    def save_session(self, filepath: str):
        """Save agent session data to file."""
        import json
        import pickle
        
        session_data = {
            'stats': self.get_session_stats(),
            'config': self._get_config_dict(),
            'brain_state': self._save_brain(),
            'timestamp': time.time(),
        }
        
        with open(filepath, 'w') as f:
            json.dump(session_data, f, indent=2)
    
    def load_session(self, filepath: str):
        """Load agent session from file."""
        import json
        
        with open(filepath, 'r') as f:
            session_data = json.load(f)
        
        self._load_config_dict(session_data.get('config', {}))
        self._load_brain_state(session_data.get('brain_state', {}))
    
    def add_event_handler(self, event_type: str, handler: Callable):
        """Add event handler for specific event type."""
        if event_type not in self.event_handlers:
            self.event_handlers[event_type] = []
        self.event_handlers[event_type].append(handler)
    
    def remove_event_handler(self, event_type: str, handler: Callable):
        """Remove event handler."""
        if event_type in self.event_handlers:
            try:
                self.event_handlers[event_type].remove(handler)
            except ValueError:
                pass
    
    def _trigger_event(self, event_type: str, data: Any = None):
        """Trigger event handlers."""
        from pynlm.core import NLMExecutionError
        
        event_data = {
            'type': event_type,
            'timestamp': time.time(),
            'data': data,
        }
        
        if event_type in self.event_handlers:
            for handler in self.event_handlers[event_type]:
                try:
                    handler(event_data)
                except Exception as e:
                    print(f"Event handler error for {event_type}: {e}")
    
    def _get_config_dict(self) -> Dict[str, Any]:
        """Get current configuration as dictionary."""
        # This would need to be implemented to extract config from brain/world
        return {}
    
    def _load_config_dict(self, config_dict: Dict[str, Any]):
        """Load configuration from dictionary."""
        # This would need to be implemented
        pass
    
    def _save_brain(self) -> Dict[str, Any]:
        """Save brain state (placeholder)."""
        return {}
    
    def _load_brain_state(self, brain_state: Dict[str, Any]):
        """Load brain state (placeholder)."""
        # This would need to be implemented
        pass
    
    def __repr__(self) -> str:
        return f"AgentSession(steps={self.current_step}, status={self.status})"


def create_training_session(**kwargs):
    """Create training session with common configuration."""
    session = AgentSession()
    
    # Apply custom settings
    for key, value in kwargs.items():
        if key == 'max_steps':
            session.max_steps = value
        elif key == 'world_width':
            session.world.configure(width=value)
        elif key == 'world_height':
            session.world.configure(height=value)
        elif key == 'enable_learning':
            session.agent.enableRewardModulation(value)
        elif key == 'enable_curiosity':
            session.agent.enableCuriosity(value)
        elif key == 'enable_novelty':
            # Novelty is usually enabled by default
            pass
    
    return session


def create_agent_brain_with_config(config_file: str = None):
    """Create agent brain with configuration file."""
    if config_file:
        config = createDefaultConfig()
        config.loadFromFile(config_file)
    else:
        config = configure_default()
    
    brain = createBrain(config)
    brain.initialize()
    
    world = createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = createAgentBrain(brain)
    agent.initialize(world)
    
    return brain, world, agent


def create_exploration_session(brain, world, agent, exploration_episodes: int = 10):
    """Create exploration session for curiosity-driven learning."""
    from pynlm.core import NLMExecutionError
    
    if not agent.isCuriosityEnabled():
        raise NLMConfigurationError("Curiosity must be enabled for exploration session")
    
    # Track exploration metrics
    exploration_data = {
        'episodes': exploration_episodes,
        'novelty_scores': [],
        'curiosity_levels': [],
        'total_rewards': [],
    }
    
    for episode in range(exploration_episodes):
        # Reset environment
        world.reset()
        
        # Run episode
        for step in range(100):  # Standard episode length
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            
            # Explore (random actions)
            import random
            action_type = random.choice(list(ActionType))
            action = Action(action_type)
            
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Record metrics
            exploration_data['novelty_scores'].append(agent.getNoveltyLevel())
            exploration_data['curiosity_levels'].append(agent.getCuriosityLevel())
            exploration_data['total_rewards'].append(agent.getNeuromodulationLevel())
            
            agent.applyRewardModulation(0.0, 0.0)
        
        print(f"Exploration episode {episode + 1}/{exploration_episodes} completed")
    
    return exploration_data


def analyze_agent_performance(agent_session: AgentSession) -> Dict[str, Any]:
    """Analyze agent performance from session statistics."""
    stats = agent_session.get_session_stats()
    
    analysis = {
        'efficiency': {},
        'learning_trajectory': {},
        'behavior_patterns': {},
    }
    
    # Calculate efficiency metrics
    if stats['steps_completed'] > 0:
        analysis['efficiency'] = {
            'steps_per_second': stats.get('steps_per_second', 0),
            'average_reward_per_step': stats.get('average_reward_per_step', 0),
            'total_energy_used': 0,  # Would need to track this
        }
    
    # Analyze learning trajectory
    if 'curiosity_summary' in stats:
        analysis['learning_trajectory'] = {
            'curiosity_growth': stats['curiosity_summary'],
            'novelty_adaptation': stats.get('novelty_summary', {}),
            'prediction_error_reduction': stats.get('prediction_error_summary', {}),
        }
    
    # Behavior patterns
    if stats['world']:
        analysis['behavior_patterns'] = {
            'final_position': (stats['world']['agent_x'], stats['world']['agent_y']),
            'final_energy': stats['world']['agent_energy'],
            'exploration_rate': stats['curiosity_summary']['mean'] if 'curiosity_summary' in stats else 0,
        }
    
    return analysis
