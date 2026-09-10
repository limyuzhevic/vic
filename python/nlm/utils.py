"""
Utilities and helper functions for NLM.

Common utilities for NLM Python development.
"""

from typing import List, Dict, Any, Optional, Callable
import time
import statistics
from dataclasses import dataclass
from enum import Enum
class LearningMetrics:
    """Metrics for learning analysis."""
    
    def __init__(self):
        self.rewards = []
        self.firing_rates = []
        self.spike_counts = []
        self.curiosity_levels = []
        self.novelty_levels = []
        self.steps = []
    
    def add_step(self, reward: float, firing_rate: float, spike_count: int,
                 curiosity: float = 0.0, novelty: float = 0.0, step: int = 0):
        """Add step metrics.
        
        Args:
            reward: Reward for step
            firing_rate: Average firing rate for step
            spike_count: Number of spikes for step
            curiosity: Curiosity level for step
            novelty: Novelty level for step
            step: Step number
        """
        self.rewards.append(reward)
        self.firing_rates.append(firing_rate)
        self.spike_counts.append(spike_count)
        self.curiosity_levels.append(curiosity)
        self.novelty_levels.append(novelty)
        self.steps.append(step)
    
    def get_summary(self) -> Dict[str, Any]:
        """Get metrics summary.
        
        Returns:
            Dictionary with metrics summary
        """
        if not self.rewards:
            return {}
            
        return {
            "total_steps": len(self.rewards),
            "total_reward": sum(self.rewards),
            "average_reward": statistics.mean(self.rewards),
            "max_reward": max(self.rewards),
            "min_reward": min(self.rewards),
            "std_reward": statistics.stdev(self.rewards) if len(self.rewards) > 1 else 0.0,
            
            "average_firing_rate": statistics.mean(self.firing_rates),
            "max_firing_rate": max(self.firing_rates),
            "min_firing_rate": min(self.firing_rates),
            
            "total_spikes": sum(self.spike_counts),
            "average_spikes_per_step": statistics.mean(self.spike_counts),
            
            "average_curiosity": statistics.mean(self.curiosity_levels) if self.curiosity_levels else 0.0,
            "average_novelty": statistics.mean(self.novelty_levels) if self.novelty_levels else 0.0,
            
            "reward_efficiency": sum(self.rewards) / len(self.spike_counts) if self.spike_counts else 0.0,
            "learning_rate": self._estimate_learning_rate(),
        }
    
    def _estimate_learning_rate(self) -> float:
        """Estimate learning rate from reward progression.
        
        Returns:
            Estimated learning rate
        """
        if len(self.rewards) < 10:
            return 0.0
            
        # Calculate slope of rewards over time
        early_rewards = self.rewards[:len(self.rewards)//2]
        late_rewards = self.rewards[len(self.rewards)//2:]
        
        if not early_rewards or not late_rewards:
            return 0.0
            
        early_avg = sum(early_rewards) / len(early_rewards)
        late_avg = sum(late_rewards) / len(late_rewards)
        
        return late_avg - early_avg
class EpisodeStats:
    """Statistics for single episode."""
    
    def __init__(self, episode_id: int, start_time: float, end_time: float):
        self.episode_id = episode_id
        self.start_time = start_time
        self.end_time = end_time
        self.duration = end_time - start_time
        self.metrics = LearningMetrics()
        self.action_results = []
        self.collisions = []
        self.interactions = []
    
    def add_step_stats(self, step_data: Dict[str, Any]):
        """Add step statistics.
        
        Args:
            step_data: Step data dictionary
        """
        # Extract metrics
        reward = step_data.get("action", {}).get("reward", 0)
        firing_rate = step_data.get("brain", {}).get("average_firing_rate", 0)
        spike_count = step_data.get("brain", {}).get("total_spikes", 0)
        
        # Extract agent stats
        agent_stats = step_data.get("agent", {})
        curiosity = agent_stats.get("neuromodulation", {}).get("curiosity_level", 0)
        novelty = agent_stats.get("neuromodulation", {}).get("novelty_level", 0)
        
        self.metrics.add_step(reward, firing_rate, spike_count, curiosity, novelty, step_data.get("step", 0))
        
        # Record action results
        action_result = step_data.get("action", {})
        if action_result.get("success", False):
            self.action_results.append(action_result)
            
        # Record interactions
        interaction = step_data.get("world", {}).get("interaction", {})
        if interaction.get("success", False):
            self.interactions.append(interaction)
    
    def get_summary(self) -> Dict[str, Any]:
        """Get episode summary.
        
        Returns:
            Dictionary with episode summary
        """
        return {
            "episode_id": self.episode_id,
            "duration": self.duration,
            "total_steps": self.metrics.total_steps,
            "total_reward": self.metrics.total_reward,
            "average_reward": self.metrics.average_reward,
            "successful_actions": len(self.action_results),
            "total_interactions": len(self.interactions),
            "successful_interactions": len([i for i in self.interactions if i.get("success", False)]),
            "collision_count": len(self.collisions),
            "metrics": self.metrics.get_summary(),
        }
class SimulationLogger:
    """Logger for simulation events."""
    
    def __init__(self, log_file: Optional[str] = None):
        self.log_file = log_file
        self.events = []
        self.start_time = time.time()
    
    def log_event(self, message: str, level: str = "INFO"):
        """Log event.
        
        Args:
            message: Log message
            level: Log level (INFO, WARNING, ERROR)
        """
        event = {
            "timestamp": time.time() - self.start_time,
            "level": level,
            "message": message,
        }
        
        self.events.append(event)
        
        if self.log_file:
            self._write_to_file(event)
    
    def log_episode(self, episode_id: int, episode_stats: Dict[str, Any], duration: float):
        """Log episode results.
        
        Args:
            episode_id: Episode ID
            episode_stats: Episode statistics
            duration: Episode duration
        """
        message = f"Episode {episode_id} completed: "
        message += f"reward={episode_stats.get('total_reward', 0):.2f}, "
        message += f"steps={episode_stats.get('steps', 0)}, "
        message += f"duration={duration:.2f}s"
        
        self.log_event(message, "INFO")
    
    def log_progress(self, current: int, total: int, episode_result: Dict[str, Any]):
        """Log progress.
        
        Args:
            current: Current episode
            total: Total episodes
            episode_result: Episode result
        """
        progress = (current + 1) / total * 100
        message = f"Progress: {progress:.1f}% ({current + 1}/{total}) "
        message += f"reward={episode_result.get('stats', {}).get('total_reward', 0):.2f}"
        
        self.log_event(message, "INFO")
    
    def log_error(self, message: str):
        """Log error.
        
        Args:
            message: Error message
        """
        self.log_event(message, "ERROR")
    
    def get_events(self, level: Optional[str] = None, since: Optional[float] = None) -> List[Dict[str, Any]]:
        """Get logged events.
        
        Args:
            level: Optional log level to filter
            since: Optional time since start to filter
            
        Returns:
            List of events
        """
        events = self.events
        
        if level:
            events = [e for e in events if e["level"] == level]
            
        if since:
            events = [e for e in events if e["timestamp"] >= since]
            
        return events
    
    def _write_to_file(self, event: Dict[str, Any]):
        """Write event to log file.
        
        Args:
            event: Event to write
        """
        try:
            with open(self.log_file, "a") as f:
                import json
                f.write(json.dumps(event) + "\n")
        except Exception:
            pass
class TrainingCurve:
    """Training curve visualization data."""
    
    def __init__(self):
        self.episode_numbers = []
        self.rewards = []
        self.firing_rates = []
        self.curiosity_levels = []
    
    def add_episode(self, episode_num: int, episode_result: Dict[str, Any]):
        """Add episode to curve.
        
        Args:
            episode_num: Episode number
            episode_result: Episode result
        """
        self.episode_numbers.append(episode_num)
        
        stats = episode_result.get("stats", {})
        self.rewards.append(stats.get("total_reward", 0))
        self.firing_rates.append(stats.get("average_firing_rate", 0))
        
        # Extract curiosity from agent stats if available
        agent_stats = episode_result.get("data", [{}])[0].get("agent", {})
        neuromod = agent_stats.get("neuromodulation", {})
        self.curiosity_levels.append(neurom.get("curiosity_level", 0))
    
    def get_curve_data(self) -> Dict[str, List[float]]:
        """Get curve data.
        
        Returns:
            Dictionary with curve data
        """
        return {
            "episodes": self.episode_numbers,
            "rewards": self.rewards,
            "firing_rates": self.firing_rates,
            "curiosity_levels": self.curiosity_levels,
        }
    
    def smooth(self, window_size: int = 5) -> 'TrainingCurve':
        """Apply moving average smoothing.
        
        Args:
            window_size: Smoothing window size
            
        Returns:
            Smoothed TrainingCurve
        """
        smoothed = TrainingCurve()
        
        if len(self.rewards) < window_size:
            return smoothed
            
        for i in range(len(self.rewards)):
            start = max(0, i - window_size // 2)
            end = min(len(self.rewards), i + window_size // 2 + 1)
            
            smoothed_rewards = sum(self.rewards[start:end]) / (end - start)
            smoothed_firing_rates = sum(self.firing_rates[start:end]) / (end - start)
            smoothed_curiosity = sum(self.curiosity_levels[start:end]) / (end - start)
            
            smoothed.episode_numbers.append(self.episode_numbers[i])
            smoothed.rewards.append(smoothed_rewards)
            smoothed.firing_rates.append(smoothed_firing_rates)
            smoothed.curiosity_levels.append(smoothed_curiosity)
            
        return smoothed