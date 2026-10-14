# Phase 6 Advanced API
# High-level APIs and utilities for Phase 6 neural simulations

import json
import time
import threading
from typing import Dict, Any, List, Optional, Callable
from dataclasses import dataclass, field
from pathlib import Path
import os
from enum import Enum

class LearningMode(Enum):
    EXPLORATION = "exploration"
    EXPLOITATION = "exploitation"
    BALANCED = "balanced"
    CURIOUS = "curious"
    ADAPTIVE = "adaptive"

class DevelopmentPhase(Enum):
    INITIAL = "initial"
    CRITICAL_PERIOD = "critical_period"
    MATURATION = "maturation"
    ADULT = "adult"
    AGING = "aging"

@dataclass
class AdvancedBrainConfig:
    """Advanced configuration for Phase 6 brain simulation."""
    # Core brain parameters
    neuron_count: int = 1000
    region_count: int = 2
    connection_probability: float = 0.1
    simulation_timestep: float = 0.001
    random_seed: int = 42
    
    # Learning parameters
    learning_mode: LearningMode = LearningMode.BALANCED
    exploration_rate: float = 0.1
    exploitation_rate: float = 0.8
    curiosity_strength: float = 0.5
    novelty_sensitivity: float = 0.3
    
    # Development parameters
    developmental_phase: DevelopmentPhase = DevelopmentPhase.INITIAL
    plasticity_decay_rate: float = 0.01
    enable_critical_periods: bool = True
    enable_maturation: bool = True
    enable_aging: bool = False
    
    # Memory parameters
    working_memory_capacity: int = 1000
    episodic_memory_capacity: int = 5000
    memory_replay_enabled: bool = True
    memory_replay_interval: int = 100
    memory_consolidation_enabled: bool = True
    
    # Neuromodulation parameters
    dopamine_enabled: bool = True
    dopamine_baseline: float = 0.1
    dopamine_range: float = 1.0
    serotonin_enabled: bool = True
    norepinephrine_enabled: bool = True
    acetylcholine_enabled: bool = True
    
    # Performance and monitoring
    enable_performance_monitoring: bool = True
    performance_monitoring_interval: int = 1000
    enable_profiling: bool = False
    profiling_interval: int = 100
    
    # Environment interaction
    environment_name: str = "GridWorld"
    environment_width: int = 10
    environment_height: int = 10
    max_energy: float = 100.0
    energy_decay_rate: float = 0.01
    
    # Goals and objectives
    target_reward: float = 100.0
    max_episodes_before_terminate: int = 1000
    minimum_firing_rate: float = 0.1
    minimum_memory_efficiency: float = 0.8
    target_plasticity_rate: float = 0.05
    
    # Visualization and output
    enable_visualization: bool = False
    visualization_update_rate: float = 30.0
    enable_detailed_logging: bool = True
    log_to_file: bool = True
    log_filename: str = "nlm_advanced.log"
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert config to dictionary."""
        config_dict = {
            "neuron_count": self.neuron_count,
            "region_count": self.region_count,
            "connection_probability": self.connection_probability,
            "simulation_timestep": self.simulation_timestep,
            "random_seed": self.random_seed,
            "learning_mode": self.learning_mode.value,
            "exploration_rate": self.exploration_rate,
            "exploitation_rate": self.exploitation_rate,
            "curiosity_strength": self.curiosity_strength,
            "novelty_sensitivity": self.novelty_sensitivity,
            "developmental_phase": self.developmental_phase.value,
            "plasticity_decay_rate": self.plasticity_decay_rate,
            "enable_critical_periods": self.enable_critical_periods,
            "enable_maturation": self.enable_maturation,
            "enable_aging": self.enable_aging,
            "working_memory_capacity": self.working_memory_capacity,
            "episodic_memory_capacity": self.episodic_memory_capacity,
            "memory_replay_enabled": self.memory_replay_enabled,
            "memory_replay_interval": self.memory_replay_interval,
            "memory_consolidation_enabled": self.memory_consolidation_enabled,
            "dopamine_enabled": self.dopamine_enabled,
            "dopamine_baseline": self.dopamine_baseline,
            "dopamine_range": self.dopamine_range,
            "serotonin_enabled": self.serotonin_enabled,
            "norepinephrine_enabled": self.norepinephrine_enabled,
            "acetylcholine_enabled": self.acetylcholine_enabled,
            "enable_performance_monitoring": self.enable_performance_monitoring,
            "performance_monitoring_interval": self.performance_monitoring_interval,
            "enable_profiling": self.enable_profiling,
            "profiling_interval": self.profiling_interval,
            "environment_name": self.environment_name,
            "environment_width": self.environment_width,
            "environment_height": self.environment_height,
            "max_energy": self.max_energy,
            "energy_decay_rate": self.energy_decay_rate,
            "target_reward": self.target_reward,
            "max_episodes_before_terminate": self.max_episodes_before_terminate,
            "minimum_firing_rate": self.minimum_firing_rate,
            "minimum_memory_efficiency": self.minimum_memory_efficiency,
            "target_plasticity_rate": self.target_plasticity_rate,
            "enable_visualization": self.enable_visualization,
            "visualization_update_rate": self.visualization_update_rate,
            "enable_detailed_logging": self.enable_detailed_logging,
            "log_to_file": self.log_to_file,
            "log_filename": self.log_filename
        }
        return config_dict
    
    @classmethod
    def from_dict(cls, config_dict: Dict[str, Any]) -> 'AdvancedBrainConfig':
        """Create config from dictionary."""
        # Convert enum strings to enum values
        if "learning_mode" in config_dict:
            config_dict["learning_mode"] = LearningMode(config_dict["learning_mode"])
        
        if "developmental_phase" in config_dict:
            config_dict["developmental_phase"] = DevelopmentPhase(config_dict["developmental_phase"])
        
        return cls(**config_dict)

class Phase6BrainSimulation:
    """High-level brain simulation API for Phase 6."""
    
    def __init__(self, config: AdvancedBrainConfig = None):
        self.config = config or AdvancedBrainConfig()
        self.brain = None
        self.agent = None
        self.world = None
        self.monitoring_enabled = False
        self.simulation_running = False
        self.simulation_step = 0
        self.episode_count = 0
        self.total_reward = 0.0
        
        # Performance tracking
        self.start_time = time.time()
        self.step_times: List[float] = []
        self.reward_history: List[float] = []
        self.firing_rate_history: List[float] = []
        
        # Event callbacks
        self.on_step_callback: Optional[Callable[[int, Dict[str, Any]], None]] = None
        self.on_episode_end_callback: Optional[Callable[[int, Dict[str, Any]], None]] = None
        self.on_learning_callback: Optional[Callable[[Dict[str, Any]], None]] = None
        self.on_performance_alert_callback: Optional[Callable[[Dict[str, Any]], None]] = None
        
        # Learning state
        self.learning_mode = LearningMode.BALANCED
        self.exploration_rate = 0.1
        self.exploitation_rate = 0.8
        self.curiosity_accumulator = 0.0
        
    def initialize(self):
        """Initialize the brain simulation with advanced configuration."""
        try:
            # Import here to avoid circular dependencies
            from src.brain.Brain import Brain
            from src.agent.AgentBrain import AgentBrain
            from src.world.SimpleWorld import SimpleWorld
            from src.core.Config.Config import Config
            
            # Create configuration
            brain_config = Config()
            brain_config.set("neuron_count", self.config.neuron_count)
            brain_config.set("region_count", self.config.region_count)
            brain_config.set("connection_probability", self.config.connection_probability)
            brain_config.set("random_seed", self.config.random_seed)
            brain_config.set("simulation_timestep", self.config.simulation_timestep)
            
            # Set learning-related config
            brain_config.set("curiosity_strength", self.config.curiosity_strength)
            brain_config.set("novelty_sensitivity", self.config.novelty_sensitivity)
            brain_config.set("plasticity_decay_rate", self.config.plasticity_decay_rate)
            
            # Initialize components
            self.brain = Brain(brain_config)
            self.world = SimpleWorld()
            self.agent = AgentBrain(self.brain)
            
            # Configure agent with advanced settings
            self.agent.initialize(self.world)
            
            # Set learning mode and parameters
            self._setup_learning_behavior()
            
            # Set development stage
            self._setup_development_behavior()
            
            # Initialize memory systems
            self._setup_memory_systems()
            
            # Configure neuromodulation
            self._setup_neuromodulation()
            
            self.monitoring_enabled = self.config.enable_performance_monitoring
            self.simulation_running = True
            
            print(f"Phase 6 Brain Simulation initialized:")
            print(f"  - Neurons: {self.config.neuron_count}")
            print(f"  - Regions: {self.config.region_count}")
            print(f"  - Learning Mode: {self.config.learning_mode.value}")
            print(f"  - Developmental Phase: {self.config.developmental_phase.value}")
            print(f"  - Curiosity Strength: {self.config.curiosity_strength}")
            print(f"  - Memory Replay: {'Enabled' if self.config.memory_replay_enabled else 'Disabled'}")
            
            return True
            
        except Exception as e:
            print(f"Error initializing Phase 6 Brain Simulation: {e}")
            return False
    
    def _setup_learning_behavior(self):
        """Setup learning behavior based on learning mode."""
        if self.config.learning_mode == LearningMode.EXPLORATION:
            self.exploration_rate = 0.7
            self.exploitation_rate = 0.2
            self.curiosity_accumulator = 1.0
            
        elif self.config.learning_mode == LearningMode.EXPLOITATION:
            self.exploration_rate = 0.1
            self.exploitation_rate = 0.9
            self.curiosity_accumulator = 0.0
            
        elif self.config.learning_mode == LearningMode.CURIOUS:
            self.exploration_rate = 0.3
            self.exploitation_rate = 0.6
            self.curiosity_accumulator = 0.8
            
        elif self.config.learning_mode == LearningMode.ADAPTIVE:
            self.exploration_rate = 0.2
            self.exploitation_rate = 0.7
            self.curiosity_accumulator = 0.5
        
        # Configure agent with learning parameters
        self.agent.enableCuriosity(self.config.curiosity_enabled)
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(self.config.memory_replay_enabled)
        self.agent.enableDevelopment(self.config.development_enabled)
        
        # Set exploration parameters
        if hasattr(self.agent, 'setExplorationRate'):
            self.agent.setExplorationRate(self.exploration_rate)
        if hasattr(self.agent, 'setExploitationRate'):
            self.agent.setExploitationRate(self.exploitation_rate)
    
    def _setup_development_behavior(self):
        """Setup development behavior based on phase."""
        if hasattr(self.agent, 'setDevelopmentStage'):
            self.agent.setDevelopmentStage(self.config.developmental_phase.value)
        
        # Configure critical periods
        if self.config.development_enabled and self.config.enable_critical_periods:
            # Set up critical period effects
            pass
        
        # Configure maturation
        if self.config.development_enabled and self.config.enable_maturation:
            # Set up maturation effects
            pass
    
    def _setup_memory_systems(self):
        """Setup memory systems with advanced configuration."""
        if self.config.memory_replay_enabled:
            # Configure replay system
            if hasattr(self.agent, 'setReplayInterval'):
                self.agent.setReplayInterval(self.config.memory_replay_interval)
            
            if hasattr(self.agent, 'setReplayStrength'):
                self.agent.setReplayStrength(self.config.replay_strength)
        
        if self.config.memory_consolidation_enabled:
            # Configure memory consolidation
            pass
        
        # Set memory capacities
        if hasattr(self.brain, 'setWorkingMemoryCapacity'):
            self.brain.setWorkingMemoryCapacity(self.config.working_memory_capacity)
        
        if hasattr(self.brain, 'setEpisodicMemoryCapacity'):
            self.brain.setEpisodicMemoryCapacity(self.config.episodic_memory_capacity)
    
    def _setup_neuromodulation(self):
        """Setup neuromodulation systems."""
        # Configure dopamine system
        if self.config.dopamine_enabled:
            if hasattr(self.agent, 'setDopamineBaseline'):
                self.agent.setDopamineBaseline(self.config.dopamine_baseline)
            if hasattr(self.agent, 'setDopamineRange'):
                self.agent.setDopamineRange(self.config.dopamine_range)
        
        # Configure other neuromodulators
        if hasattr(self.agent, 'setSerotoninEnabled'):
            self.agent.setSerotoninEnabled(self.config.serotonin_enabled)
        
        if hasattr(self.agent, 'setNorepinephrineEnabled'):
            self.agent.setNorepinephrineEnabled(self.config.norepinephrine_enabled)
        
        if hasattr(self.agent, 'setAcetylcholineEnabled'):
            self.agent.setAcetylcholineEnabled(self.config.acetylcholine_enabled)
    
    def run_episode(self, max_steps: int = None) -> Dict[str, Any]:
        """Run a single episode with advanced features."""
        if not self.simulation_running:
            print("Simulation not initialized. Call initialize() first.")
            return {}
        
        episode_start_time = time.time()
        episode_reward = 0.0
        episode_steps = max_steps or self.config.max_episodes_before_terminate
        
        # Reset state
        self.brain.reset()
        self.brain.initialize()
        self.agent.reset()
        self.world.reset()
        
        self.simulation_step = 0
        
        print(f"Starting episode {self.episode_count + 1}...")
        
        for step in range(episode_steps):
            try:
                # Record step start time for performance profiling
                step_start_time = time.time()
                
                # Get sensory input from world
                if hasattr(self.world, 'observe'):
                    observation = self.world.observe(self.agent)
                else:
                    observation = None
                
                # Process sensory input
                if observation is not None:
                    self.agent.processSensoryInput(observation)
                
                # Run brain simulation
                self.brain.step(step, step * self.config.simulation_timestep)
                
                # Update development if enabled
                if self.config.development_enabled:
                    self.agent.updateDevelopment(step * self.config.simulation_timestep)
                
                # Get motor command from agent
                if hasattr(self.agent, 'decodeMotorCommand'):
                    motor_command = self.agent.decodeMotorCommand()
                else:
                    motor_command = None
                
                # Apply action to world
                if motor_command is not None and hasattr(self.world, 'applyAction'):
                    action_result = self.world.applyAction(self.agent, motor_command)
                    
                    # Apply reward modulation
                    if hasattr(self.agent, 'applyRewardModulation'):
                        reward = action_result.get('reward', 0.0) if isinstance(action_result, dict) else 0.0
                        self.agent.applyRewardModulation(reward, step * self.config.simulation_timestep)
                    
                    episode_reward += reward
                    self.total_reward += reward
                    
                    # Track reward for learning
                    self.reward_history.append(reward)
                
                # Record firing rate
                if hasattr(self.brain, 'getAverageFiringRate'):
                    firing_rate = self.brain.getAverageFiringRate()
                    self.firing_rate_history.append(firing_rate)
                
                # Record step time
                step_time = time.time() - step_start_time
                self.step_times.append(step_time)
                
                # Update step
                self.simulation_step = step + 1
                
                # Call step callback if provided
                if self.on_step_callback:
                    step_data = {
                        'step': step,
                        'reward': episode_reward,
                        'firing_rate': firing_rate if 'firing_rate' in locals() else 0.0,
                        'time': step_time
                    }
                    self.on_step_callback(step, step_data)
                
                # Check if episode should end
                if hasattr(self.agent, 'getDevelopmentStage'):
                    stage = self.agent.getDevelopmentStage()
                    if stage == DevelopmentPhase.AGING and step > self.config.max_episodes_before_terminate * 0.8:
                        print(f"Episode {self.episode_count + 1} ended due to aging phase.")
                        break
                
                # Adaptive learning based on performance
                if self.config.learning_mode == LearningMode.ADAPTIVE:
                    self._adapt_learning_parameters(step, episode_reward)
                
            except Exception as e:
                print(f"Error during episode step {step}: {e}")
                break
        
        # Episode ended
        episode_duration = time.time() - episode_start_time
        episode_end_data = {
            'episode': self.episode_count,
            'steps': self.simulation_step,
            'reward': episode_reward,
            'total_reward': self.total_reward,
            'duration': episode_duration,
            'average_firing_rate': sum(self.firing_rate_history) / len(self.firing_rate_history) if self.firing_rate_history else 0.0,
            'average_step_time': sum(self.step_times) / len(self.step_times) if self.step_times else 0.0,
            'learning_mode': self.learning_mode.value,
            'development_stage': self.config.developmental_phase.value
        }
        
        print(f"Episode {self.episode_count + 1} completed: {episode_reward:.2f} reward over {self.simulation_step} steps")
        
        # Call episode end callback
        if self.on_episode_end_callback:
            self.on_episode_end_callback(self.episode_count, episode_end_data)
        
        # Increment episode count
        self.episode_count += 1
        
        return episode_end_data
    
    def _adapt_learning_parameters(self, step: int, reward: float):
        """Adapt learning parameters based on performance."""
        # Adjust exploration rate based on reward
        if reward > self.config.target_reward * 0.8:
            # High reward, reduce exploration
            self.exploration_rate = max(0.05, self.exploration_rate * 0.9)
            self.exploitation_rate = min(0.95, 1.0 - self.exploration_rate)
        elif reward < self.config.target_reward * 0.2:
            # Low reward, increase exploration
            self.exploration_rate = min(0.5, self.exploration_rate * 1.2)
            self.exploitation_rate = 1.0 - self.exploration_rate
        
        # Update learning mode based on performance
        if len(self.reward_history) >= 10:
            recent_rewards = self.reward_history[-10:]
            avg_reward = sum(recent_rewards) / len(recent_rewards)
            
            if avg_reward > self.config.target_reward * 0.9:
                self.learning_mode = LearningMode.EXPLOITATION
            elif avg_reward < self.config.target_reward * 0.2:
                self.learning_mode = LearningMode.EXPLORATION
            else:
                self.learning_mode = LearningMode.BALANCED
    
    def run_simulation(self, max_episodes: int = None) -> Dict[str, Any]:
        """Run complete simulation with multiple episodes."""
        max_episodes = max_episodes or self.config.max_episodes_before_terminate
        
        print(f"Starting Phase 6 Brain Simulation with {max_episodes} episodes...")
        
        simulation_start_time = time.time()
        episode_results = []
        
        for episode in range(max_episodes):
            # Run single episode
            episode_result = self.run_episode()
            episode_results.append(episode_result)
            
            # Call learning callback if provided
            if self.on_learning_callback:
                learning_data = {
                    'episode': episode,
                    'reward': episode_result['reward'],
                    'learning_mode': self.learning_mode.value,
                    'development_stage': self.config.developmental_phase.value
                }
                self.on_learning_callback(learning_data)
            
            # Check if simulation should end
            if episode_result['reward'] >= self.config.target_reward:
                print(f"Target reward reached! Simulation stopping early.")
                break
            
            # Small delay between episodes to prevent overwhelming
            time.sleep(0.1)
        
        simulation_duration = time.time() - simulation_start_time
        
        # Generate final report
        final_report = {
            'total_episodes': self.episode_count,
            'total_steps': self.simulation_step,
            'total_reward': self.total_reward,
            'simulation_duration': simulation_duration,
            'average_reward_per_episode': sum(r['reward'] for r in episode_results) / len(episode_results) if episode_results else 0.0,
            'average_steps_per_episode': sum(r['steps'] for r in episode_results) / len(episode_results) if episode_results else 0.0,
            'learning_mode': self.learning_mode.value,
            'development_stage': self.config.developmental_phase.value,
            'episode_results': episode_results
        }
        
        print(f"\nPhase 6 Brain Simulation Complete!")
        print(f"  - Episodes: {self.episode_count}")
        print(f"  - Total Steps: {self.simulation_step}")
        print(f"  - Total Reward: {self.total_reward:.2f}")
        print(f"  - Duration: {simulation_duration:.2f} seconds")
        print(f"  - Average Reward/Episode: {final_report['average_reward_per_episode']:.2f}")
        
        return final_report
    
    def export_results(self, output_path: str):
        """Export simulation results to file."""
        results = {
            'config': self.config.to_dict(),
            'simulation_stats': {
                'total_episodes': self.episode_count,
                'total_steps': self.simulation_step,
                'total_reward': self.total_reward,
                'start_time': self.start_time,
                'end_time': time.time(),
                'reward_history': self.reward_history,
                'firing_rate_history': self.firing_rate_history,
                'step_times': self.step_times
            },
            'learning_state': {
                'learning_mode': self.learning_mode.value,
                'exploration_rate': self.exploration_rate,
                'exploitation_rate': self.exploitation_rate,
                'curiosity_accumulator': self.curiosity_accumulator
            }
        }
        
        # Ensure output directory exists
        output_dir = Path(output_path).parent
        output_dir.mkdir(parents=True, exist_ok=True)
        
        with open(output_path, 'w') as f:
            json.dump(results, f, indent=4)
        
        print(f"Results exported to: {output_path}")
        return output_path
    
    def import_config(self, config_path: str):
        """Import configuration from file."""
        try:
            with open(config_path, 'r') as f:
                config_dict = json.load(f)
            
            # Convert to AdvancedBrainConfig
            config = AdvancedBrainConfig.from_dict(config_dict)
            self.config = config
            
            print(f"Configuration imported from: {config_path}")
            return True
            
        except Exception as e:
            print(f"Error importing configuration: {e}")
            return False
    
    def get_performance_stats(self) -> Dict[str, Any]:
        """Get performance statistics."""
        stats = {
            'simulation_step': self.simulation_step,
            'episode_count': self.episode_count,
            'total_reward': self.total_reward,
            'current_learning_mode': self.learning_mode.value,
            'current_development_stage': self.config.developmental_phase.value,
            'average_firing_rate': sum(self.firing_rate_history) / len(self.firing_rate_history) if self.firing_rate_history else 0.0,
            'average_step_time': sum(self.step_times) / len(self.step_times) if self.step_times else 0.0,
            'reward_history': self.reward_history,
            'step_times': self.step_times
        }
        
        return stats
    
    def stop(self):
        """Stop simulation and cleanup."""
        self.simulation_running = False
        print("Phase 6 Brain Simulation stopped.")

# Global advanced brain simulation instance
advanced_brain_simulation = Phase6BrainSimulation()

# Convenience functions
def create_advanced_brain(config: AdvancedBrainConfig = None) -> Phase6BrainSimulation:
    """Create advanced brain simulation with configuration."""
    return Phase6BrainSimulation(config)

def run_advanced_simulation(max_episodes: int = None, config: AdvancedBrainConfig = None) -> Dict[str, Any]:
    """Run advanced simulation with given parameters."""
    brain_sim = Phase6BrainSimulation(config)
    if brain_sim.initialize():
        return brain_sim.run_simulation(max_episodes)
    return {}

def export_config_as_json(config: AdvancedBrainConfig, output_path: str):
    """Export configuration as JSON."""
    with open(output_path, 'w') as f:
        json.dump(config.to_dict(), f, indent=4)
    return output_path