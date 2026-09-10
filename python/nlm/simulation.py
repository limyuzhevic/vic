"""
Simulation management for NLM.

High-level Python API for managing complete simulation sessions and learning experiments.
"""

from typing import List, Optional, Dict, Any, Callable
import time
import threading

from .exceptions import NLMRuntimeError, NLMValidationError
from .agent import Agent
from .simulation import SimulationLogger

class Simulation:
    """Complete NLM simulation controller.
    
    High-level interface for running simulation sessions with episode management,
    learning, and data collection.
    """
    
    def __init__(self, agent: Optional[Agent] = None, timestep: float = 0.1,
                 max_steps: int = 1000, log_file: Optional[str] = None):
        """Initialize simulation.
        
        Args:
            agent: Agent instance (if None, creates default)
            timestep: Simulation time step
            max_steps: Maximum steps per episode
            log_file: Optional log file path
        """
        self.agent = agent or Agent()
        self.timestep = timestep
        self.max_steps = max_steps
        self.logger = SimulationLogger(log_file) if log_file else None
        
        # Episode data
        self.current_episode = 0
        self.episode_data = []
        self.simulation_active = False
        self._thread = None
        self._stop_event = threading.Event()
    
    def run_episode(self, episode_id: Optional[int] = None) -> Dict[str, Any]:
        """Run single episode.
        
        Args:
            episode_id: Optional episode ID
            
        Returns:
            Dictionary with episode results
        """
        if episode_id is None:
            episode_id = self.current_episode
            
        episode_start_time = time.time()
        
        # Reset agent for new episode
        self.agent.agent_brain.reset() if hasattr(self.agent, 'agent_brain') else None
        self.agent.world.reset() if hasattr(self.agent, 'world') else None
        
        # Run episode
        episode_data = self.agent.run_episode(self.max_steps, self.timestep)
        
        episode_duration = time.time() - episode_start_time
        
        # Calculate episode statistics
        episode_stats = self._calculate_episode_stats(episode_data)
        
        # Update current episode
        self.current_episode = episode_id + 1
        self.episode_data.extend(episode_data)
        
        # Log episode results
        if self.logger:
            self.logger.log_episode(episode_id, episode_stats, episode_duration)
            
        return {
            "episode_id": episode_id,
            "duration": episode_duration,
            "steps": len(episode_data),
            "stats": episode_stats,
            "data": episode_data,
        }
    
    def run_learning_session(self, num_episodes: int, save_interval: int = 100) -> List[Dict[str, Any]]:
        """Run complete learning session.
        
        Args:
            num_episodes: Number of episodes to run
            save_interval: Save interval (episodes)
            
        Returns:
            List of episode results
        """
        session_results = []
        
        for episode_id in range(num_episodes):
            # Check for stop signal
            if self._stop_event.is_set():
                break
                
            # Run episode
            episode_result = self.run_episode(episode_id)
            session_results.append(episode_result)
            
            # Save periodically
            if episode_id % save_interval == 0:
                self.save_state(f"checkpoint_episode_{episode_id}")
                
            # Log progress
            if self.logger:
                self.logger.log_progress(episode_id, num_episodes, episode_result)
                
        # Save final state
        self.save_state("checkpoint_final")
        
        return session_results
    
    def run_continuous(self, callback: Optional[Callable] = None,
                      duration: Optional[float] = None) -> None:
        """Run continuous simulation.
        
        Args:
            callback: Optional callback for each step
            duration: Optional maximum duration in seconds
        """
        self.simulation_active = True
        self._stop_event.clear()
        
        start_time = time.time()
        
        try:
            while self.simulation_active:
                # Check duration limit
                if duration and (time.time() - start_time) > duration:
                    break
                    
                # Run single step (if agent supports step-by-step)
                self._run_single_step()
                
                # Call callback
                if callback:
                    callback(time.time() - start_time)
                    
        except KeyboardInterrupt:
            print("Simulation interrupted by user")
        finally:
            self.simulation_active = False
    
    def stop(self):
        """Stop continuous simulation."""
        self.simulation_active = False
        self._stop_event.set()
    
    def save_state(self, filepath: str) -> bool:
        """Save simulation state.
        
        Args:
            filepath: Path to save state
            
        Returns:
            True if successful, False otherwise
        """
        try:
            import json
            
            state = {
                "current_episode": self.current_episode,
                "episode_data": self.episode_data,
                "max_steps": self.max_steps,
                "timestep": self.timestep,
            }
            
            with open(filepath, 'w') as f:
                json.dump(state, f, indent=2)
                
            if self.logger:
                self.logger.log_event(f"Saved state to {filepath}")
                
            return True
            
        except Exception as e:
            if self.logger:
                self.logger.log_error(f"Failed to save state: {e}")
            return False
    
    def load_state(self, filepath: str) -> bool:
        """Load simulation state.
        
        Args:
            filepath: Path to load state from
            
        Returns:
            True if successful, False otherwise
        """
        try:
            import json
            
            with open(filepath, 'r') as f:
                state = json.load(f)
                
            self.current_episode = state.get("current_episode", 0)
            self.episode_data = state.get("episode_data", [])
            self.max_steps = state.get("max_steps", 1000)
            self.timestep = state.get("timestep", 0.1)
                
            if self.logger:
                self.logger.log_event(f"Loaded state from {filepath}")
                
            return True
            
        except Exception as e:
            if self.logger:
                self.logger.log_error(f"Failed to load state: {e}")
            return False
    
    def get_stats(self) -> Dict[str, Any]:
        """Get simulation statistics.
        
        Returns:
            Dictionary with simulation statistics
        """
        return {
            "current_episode": self.current_episode,
            "total_steps": len(self.episode_data),
            "simulation_active": self.simulation_active,
            "timestep": self.timestep,
            "max_steps": self.max_steps,
            "agent_stats": self.agent.get_agent_stats() if hasattr(self.agent, 'get_agent_stats') else {},
        }
    
    def _calculate_episode_stats(self, episode_data: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Calculate episode statistics.
        
        Args:
            episode_data: Episode step data
            
        Returns:
            Dictionary with episode statistics
        """
        if not episode_data:
            return {}
            
        # Extract metrics
        steps = len(episode_data)
        total_reward = sum(step.get("action", {}).get("reward", 0) for step in episode_data)
        avg_firing_rate = sum(step.get("brain", {}).get("average_firing_rate", 0) for step in episode_data) / steps
        total_spikes = sum(step.get("brain", {}).get("total_spikes", 0) for step in episode_data)
        
        # Calculate agent statistics
        agent_stats = {}
        if episode_data and "agent" in episode_data[0]:
            agent_stats = episode_data[0]["agent"]
            
        return {
            "steps": steps,
            "total_reward": total_reward,
            "average_reward": total_reward / steps,
            "average_firing_rate": avg_firing_rate,
            "total_spikes": total_spikes,
            "max_firing_rate": max(step.get("brain", {}).get("average_firing_rate", 0) for step in episode_data),
            "agent_stats": agent_stats,
        }
    
    def _run_single_step(self):
        """Run single simulation step (if agent supports it)."""
        # This would need to be implemented based on agent capabilities
        pass
# Convenience functions
def run_episode(agent: Agent, num_steps: int, timestep: float = 0.1,
                episode_id: Optional[int] = None) -> Dict[str, Any]:
    """Run single episode (convenience function).
    
    Args:
        agent: Agent instance
        num_steps: Number of steps to run
        timestep: Time per step
        episode_id: Optional episode ID
        
    Returns:
        Dictionary with episode results
    """
    simulation = Simulation(agent, timestep, num_steps)
    return simulation.run_episode(episode_id)

def run_learning_session(agent: Agent, num_episodes: int, timestep: float = 0.1,
                        max_steps_per_episode: int = 1000) -> List[Dict[str, Any]]:
    """Run learning session (convenience function).
    
    Args:
        agent: Agent instance
        num_episodes: Number of episodes
        timestep: Time per step
        max_steps_per_episode: Maximum steps per episode
        
    Returns:
        List of episode results
    """
    simulation = Simulation(agent, timestep, max_steps_per_episode)
    return simulation.run_learning_session(num_episodes)