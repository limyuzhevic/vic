"""Advanced simulation features for NLM"""

import time
from typing import List, Callable, Optional, Dict, Any
from dataclasses import dataclass
from enum import Enum
import numpy as np

class SimulationMode(Enum):
    """Simulation modes."""
    
    REAL_TIME = "real_time"
    BATCH = "batch"
    EPISODE = "episode"
    MONITOR = "monitor"
class EpisodeStatus(Enum):
    """Episode status."""
    
    RUNNING = "running"
    COMPLETED = "completed"
    PAUSED = "paused"
    TERMINATED = "terminated"
@dataclass
class EpisodeMetrics:
    """Episode metrics for tracking performance."""
    
    steps: int = 0
    total_spikes: int = 0
    active_neurons: int = 0
    average_firing_rate: float = 0.0
    curiosity_level: float = 0.0
    novelty_level: float = 0.0
    prediction_error: float = 0.0
    reward: float = 0.0
    development_stage: str = "initial"
    start_time: float = 0.0
    end_time: float = 0.0
    
    @property
    def duration(self):
        """Episode duration."""
        return self.end_time - self.start_time
    
    @property
    def spikes_per_step(self):
        """Average spikes per step."""
        return self.total_spikes / max(1, self.steps)
class BatchSimulator:
    """Efficient batch simulation with performance monitoring."""
    
    def __init__(self, brain, world=None, agent=None):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.mode = SimulationMode.BATCH
        self.step_times = []
        self.step_callbacks = []
    
    def run_batch(self, steps: int, timestep: float = 0.001,
                 enable_monitoring: bool = False):
        """Run batch simulation efficiently."""
        if not self.brain.is_initialized:
            raise RuntimeError("Brain must be initialized before running simulation")
        
        start_time = time.time()
        
        for step in range(steps):
            step_start = time.time()
            
            # Update world if available
            if self.world:
                self.world.update(timestep)
            
            # Process sensory input if agent available
            if self.agent and self.world:
                percept = self.world.getSensoryPercept()
                self.agent.processSensoryInput(percept)
            
            # Step brain
            self.brain.step(step, timestep * step)
            
            # Decode action if agent available
            if self.agent:
                action = self.agent.decodeMotorCommand()
                if self.world:
                    self.world.applyMotorCommand(action, timestep * step)
            
            # Record step time
            step_time = time.time() - step_start
            self.step_times.append(step_time)
            
            # Call callbacks
            for callback in self.step_callbacks:
                callback(step, self.brain)
            
            # Monitor performance if enabled
            if enable_monitoring and step % 100 == 0:
                self._log_performance(step)
        
        end_time = time.time()
        total_time = end_time - start_time
        
        return {
            'total_steps': steps,
            'total_time': total_time,
            'average_step_time': total_time / steps if steps > 0 else 0,
            'steps_per_second': steps / total_time if total_time > 0 else 0,
            'max_step_time': max(self.step_times) if self.step_times else 0,
            'min_step_time': min(self.step_times) if self.step_times else 0
        }
    
    def _log_performance(self, step):
        """Log performance metrics."""
        if self.step_times:
            avg_step_time = sum(self.step_times) / len(self.step_times)
            print(f"Step {step}: {1000 * avg_step_time:.2f}ms/step, "
                  f"{len(self.step_times)/sum(self.step_times):.0f} steps/s")
    
    def add_step_callback(self, callback: Callable):
        """Add step callback."""
        self.step_callbacks.append(callback)
        return self
    
    def remove_step_callback(self, callback: Callable):
        """Remove step callback."""
        if callback in self.step_callbacks:
            self.step_callbacks.remove(callback)
        return self
    
    def get_performance_stats(self):
        """Get performance statistics."""
        if not self.step_times:
            return {}
        
        total_steps = len(self.step_times)
        total_time = sum(self.step_times)
        
        return {
            'total_steps': total_steps,
            'total_time': total_time,
            'average_step_time': total_time / total_steps,
            'steps_per_second': total_steps / total_time,
            'max_step_time': max(self.step_times),
            'min_step_time': min(self.step_times),
            'step_time_std': np.std(self.step_times) if len(self.step_times) > 1 else 0.0
        }
class EpisodeRunner:
    """Episode-based simulation runner for testing and training."""
    
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.episode_history: List[EpisodeMetrics] = []
        self.max_steps_per_episode = 1000
        self.automatic_reset = True
    
    def run_episode(self, max_steps: Optional[int] = None,
                   enable_learning: bool = True) -> EpisodeMetrics:
        """Run a single episode and return metrics."""
        if max_steps is None:
            max_steps = self.max_steps_per_episode
        
        metrics = EpisodeMetrics()
        metrics.start_time = time.time()
        
        if not self.brain.is_initialized:
            raise RuntimeError("Brain must be initialized before running episode")
        
        for step in range(max_steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory percept
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Step brain
            self.brain.step(step)
            
            # Decode and apply action
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply reward modulation if learning enabled
            if enable_learning:
                # Get reward from world (example implementation)
                reward = 0.0
                if percept.getInternal():
                    reward = percept.getInternal()[0]
                
                self.agent.applyRewardModulation(reward, 0.0)
                metrics.reward += reward
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Update metrics
            metrics.steps = step + 1
            metrics.total_spikes = self.brain.getTotalSpikeCount()
            metrics.active_neurons = self.brain.getActiveNeuronCount()
            metrics.average_firing_rate = self.brain.getAverageFiringRate()
            metrics.curiosity_level = self.agent.getCuriosityLevel()
            metrics.novelty_level = self.agent.getNoveltyLevel()
            metrics.prediction_error = self.agent.getPredictionError()
            metrics.development_stage = self.brain.getDevelopmentalStage()
            
            # Check for early termination
            if self._should_terminate_episode(step, metrics):
                metrics.end_time = time.time()
                break
        
        metrics.end_time = time.time()
        self.episode_history.append(metrics)
        
        # Reset for next episode if automatic reset enabled
        if self.automatic_reset:
            self.reset_episode()
        
        return metrics
    
    def _should_terminate_episode(self, step: int, metrics: EpisodeMetrics) -> bool:
        """Determine if episode should terminate early."""
        # Check for maximum steps
        if step >= self.max_steps_per_episode - 1:
            return True
        
        # Check for developmental stage completion
        if metrics.development_stage == "Adult":
            # Run until stable firing rate for 100 steps
            if step >= 100 and self._is_stable(metrics):
                return True
        
        return False
    
    def _is_stable(self, metrics: EpisodeMetrics, window: int = 100) -> bool:
        """Check if metrics indicate stable state."""
        if len(self.episode_history) < window + 1:
            return False
        
        recent_firing_rates = [ep.average_firing_rate for ep in self.episode_history[-window:]]
        return np.max(recent_firing_rates) - np.min(recent_firing_rates) < 0.1
    
    def reset_episode(self):
        """Reset for next episode."""
        self.brain.reset()
        self.world.reset()
        self.agent.reset()
    
    def get_episode_statistics(self) -> Dict[str, Any]:
        """Get statistics across all episodes."""
        if not self.episode_history:
            return {}
        
        steps = [ep.steps for ep in self.episode_history]
        firing_rates = [ep.average_firing_rate for ep in self.episode_history]
        curiosity_levels = [ep.curiosity_level for ep in self.episode_history]
        
        return {
            'episodes': len(self.episode_history),
            'total_steps': sum(steps),
            'average_steps_per_episode': sum(steps) / len(steps),
            'average_firing_rate': np.mean(firing_rates) if firing_rates else 0,
            'average_curiosity': np.mean(curiosity_levels) if curiosity_levels else 0,
            'max_steps': max(steps),
            'min_steps': min(steps),
            'episode_range': {
                'min_spikes': min(ep.total_spikes for ep in self.episode_history),
                'max_spikes': max(ep.total_spikes for ep in self.episode_history),
                'min_reward': min(ep.reward for ep in self.episode_history),
                'max_reward': max(ep.reward for ep in self.episode_history)
            }
        }
class EnvironmentInterface:
    """Environment interface for simulation control and monitoring."""
    
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.monitoring_enabled = False
        self.checkpoint_manager = CheckpointManager()
    
    def run_step(self, step: int, timestamp: float = 0.0,
                 apply_actions: bool = True) -> Dict[str, Any]:
        """Run a single simulation step with comprehensive monitoring."""
        start_time = time.time()
        
        step_data = {
            'step': step,
            'timestamp': timestamp,
            'world_time': timestamp,
            'actions_applied': False
        }
        
        try:
            # Update world
            self.world.update(timestamp)
            step_data['world_updated'] = True
            
            # Get sensory percept
            percept = self.world.getSensoryPercept()
            step_data['sensory_data'] = percept.to_dict() if hasattr(percept, 'to_dict') else None
            
            # Process sensory input
            if self.agent:
                self.agent.processSensoryInput(percept)
            
            # Step brain
            self.brain.step(step, timestamp)
            step_data['brain_step'] = True
            
            # Decode and apply action
            if apply_actions and self.agent:
                action = self.agent.decodeMotorCommand()
                self.world.applyMotorCommand(action, timestamp)
                step_data['action'] = action
                step_data['actions_applied'] = True
                
                # Apply reward modulation if available
                if hasattr(self.agent, 'applyRewardModulation'):
                    # Get reward from world
                    reward = 0.0
                    if hasattr(percept, 'getInternal') and percept.getInternal():
                        reward = percept.getInternal()[0]
                    self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            if self.agent:
                self.agent.updateDevelopment(timestamp)
        
            except Exception as e:
                step_data['error'] = str(e)
                step_data['brain_step'] = False
                step_data['actions_applied'] = False
        
            # Calculate metrics
            step_data['metrics'] = {
                'total_neurons': self.brain.getTotalNeuronCount(),
                'total_synapses': self.brain.getTotalSynapseCount(),
                'active_neurons': self.brain.getActiveNeuronCount(),
                'firing_neurons': self.brain.getFiringNeuronCount(),
                'total_spikes': self.brain.getTotalSpikeCount(),
                'average_firing_rate': self.brain.getAverageFiringRate(),
                'e_i_ratio': self.brain.getExcitationInhibitionRatio()
            }
            
            # Add neuromodulation metrics
            if self.agent:
                step_data['metrics'].update({
                    'curiosity_level': self.agent.getCuriosityLevel(),
                    'novelty_level': self.agent.getNoveltyLevel(),
                    'prediction_error': self.agent.getPredictionError(),
                    'development_stage': self.brain.getDevelopmentalStage()
                })
            
            # Record step time
            step_time = time.time() - start_time
            step_data['step_time'] = step_time
            
            # Monitor if enabled
            if self.monitoring_enabled:
                self._monitor_step(step_data)
            
            return step_data
        
        except Exception as e:
            return {'error': str(e), 'step': step}
    
    def _monitor_step(self, step_data: Dict[str, Any]):
        """Monitor step execution."""
        # Log performance
        if step_data['step'] % 100 == 0:
            print(f"Step {step_data['step']}: "
                  f"{step_data['metrics']['firing_neurons']} neurons firing, "
                  f"{step_data['metrics']['average_firing_rate']:.3f} firing rate")
        
        # Check for anomalies
        metrics = step_data['metrics']
        if metrics['firing_neurons'] > metrics['total_neurons'] * 0.8:
            print(f"Warning: High firing rate at step {step_data['step']}: "
                  f"{metrics['firing_neurons']}/{metrics['total_neurons']} neurons")
    
    def save_checkpoint(self, filepath: str):
        """Save current simulation state."""
        return self.checkpoint_manager.save(
            filepath, self.brain, self.world, self.agent
        )
    
    def load_checkpoint(self, filepath: str):
        """Load simulation state from checkpoint."""
        return self.checkpoint_manager.load(
            filepath, self.brain, self.world, self.agent
        )
class CheckpointManager:
    """Manage checkpoint saving and loading."""
    
    def __init__(self):
        self.checkpoints = []
    
    def save(self, filepath: str, brain, world, agent) -> bool:
        """Save simulation state to file."""
        try:
            # Save brain state
            brain.save(filepath.replace('.bin', '_brain.bin'))
            
            # Save world state
            if hasattr(world, 'save'):
                world.save(filepath.replace('.bin', '_world.bin'))
            
            # Save agent state
            if hasattr(agent, 'save'):
                agent.save(filepath.replace('.bin', '_agent.bin'))
            
            # Record checkpoint
            self.checkpoints.append({
                'filepath': filepath,
                'timestamp': time.time(),
                'brain_state': 'saved',
                'world_state': 'saved' if hasattr(world, 'save') else 'none',
                'agent_state': 'saved' if hasattr(agent, 'save') else 'none'
            })
            
            return True
        
        except Exception as e:
            print(f"Error saving checkpoint: {e}")
            return False
    
    def load(self, filepath: str, brain, world, agent) -> bool:
        """Load simulation state from file."""
        try:
            # Load brain state
            brain.load(filepath.replace('.bin', '_brain.bin'))
            
            # Load world state
            if hasattr(world, 'load'):
                world.load(filepath.replace('.bin', '_world.bin'))
            
            # Load agent state
            if hasattr(agent, 'load'):
                agent.load(filepath.replace('.bin', '_agent.bin'))
            
            # Record checkpoint load
            self.checkpoints.append({
                'filepath': filepath,
                'timestamp': time.time(),
                'brain_state': 'loaded',
                'world_state': 'loaded' if hasattr(world, 'load') else 'none',
                'agent_state': 'loaded' if hasattr(agent, 'load') else 'none'
            })
            
            return True
        
        except Exception as e:
            print(f"Error loading checkpoint: {e}")
            return False
    
    def list_checkpoints(self) -> List[Dict[str, Any]]:
        """List all checkpoints."""
        return self.checkpoints.copy()
    
    def delete_checkpoint(self, filepath: str) -> bool:
        """Delete checkpoint."""
        try:
            # Note: This is a simplified implementation
            # In practice, you would delete the actual files
            self.checkpoints = [cp for cp in self.checkpoints if cp['filepath'] != filepath]
            return True
        
        except Exception:
            return False