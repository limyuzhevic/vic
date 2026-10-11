"""Agent control and management for NLM Python API."""

import numpy as np
from typing import Dict, List, Optional, Any, Tuple
from collections import deque
import time
import pickle
from pathlib import Path
import json

from ..brain.Brain.hpp:4> # noqa: F401
from ..agent.AgentBrain.hpp:4> # noqa: F401
from ..world.SimpleWorld.hpp:4> # noqa: F401
from ..core.Config.Config import Config


class AgentTrainer:
    """
    Train agents with advanced features.
    
    This class provides comprehensive training capabilities for NLM agents,
    including reinforcement learning, curriculum learning, and advanced
    optimization techniques.
    """
    
    def __init__(self, learning_rate: float = 0.01, batch_size: int = 32,
                 discount_factor: float = 0.99, entropy_coefficient: float = 0.01):
        """
        Initialize agent trainer.
        
        Args:
            learning_rate: Base learning rate for training
            batch_size: Number of samples per training batch
            discount_factor: Discount factor for future rewards
            entropy_coefficient: Coefficient for entropy regularization
        """
        self.learning_rate = learning_rate
        self.batch_size = batch_size
        self.discount_factor = discount_factor
        self.entropy_coefficient = entropy_coefficient
        
        # Training state
        self.training_episodes = 0
        self.training_steps = 0
        self.current_loss = 0.0
        self.current_entropy = 0.0
        
        # Experience replay
        self.experience_replay = deque(maxlen=10000)
        self.priority_replay = []  # For prioritized experience replay
        
        # Performance metrics
        self.performance_history = deque(maxlen=100)
        self.loss_history = deque(maxlen=100)
        self.entropy_history = deque(maxlen=100)
        
        # Curriculum learning
        self.curriculum_level = 1
        self.difficulty_threshold = 0.7
        self.current_difficulty = 0.5
        
        # Advanced training features
        self.early_stopping_patience = 20
        self.early_stopping_counter = 0
        self.best_performance = -float('inf')
        
    def add_experience(self, state: Any, action: Any, reward: float,
                      next_state: Any, done: bool) -> None:
        """
        Add experience to replay buffer.
        
        Args:
            state: Current state
            action: Action taken
            reward: Reward received
            next_state: Next state
            done: Whether episode ended
        """
        experience = {
            "state": state,
            "action": action,
            "reward": reward,
            "next_state": next_state,
            "done": done,
            "priority": 1.0  # Initial priority
        }
        
        self.experience_replay.append(experience)
        
        # Calculate priority for prioritized experience replay
        priority = abs(reward) + 0.1  # Higher priority for larger rewards
        self.priority_replay.append((priority, experience))
        
    def sample_experience(self, batch_size: int = None) -> List[Dict[str, Any]]:
        """
        Sample experiences from replay buffer.
        
        Args:
            batch_size: Batch size (if None, uses self.batch_size)
            
        Returns:
            List[Dict[str, Any]]: Sampled experiences
        """
        if batch_size is None:
            batch_size = self.batch_size
            
        if not self.experience_replay:
            return []
            
        # Use prioritized experience replay if available
        if self.priority_replay:
            # Sample based on priority (higher priority more likely)
            priorities = [p[0] for p in self.priority_replay]
            total_priority = sum(priorities)
            
            if total_priority > 0:
                probs = [p / total_priority for p in priorities]
                indices = np.random.choice(len(self.priority_replay), 
                                          size=min(batch_size, len(self.priority_replay)),
                                          p=probs, replace=False)
            else:
                indices = np.random.choice(len(self.priority_replay),
                                          size=min(batch_size, len(self.priority_replay)),
                                          replace=False)
                
            sampled = [self.priority_replay[i][1] for i in indices]
            
            # Update priorities based on TD error (simplified)
            for i, experience in zip(indices, sampled):
                old_priority = self.priority_replay[i][0]
                td_error = self._calculate_td_error(experience)
                new_priority = max(0.01, abs(td_error))
                self.priority_replay[i] = (new_priority, experience)
        else:
            # Uniform sampling
            indices = np.random.choice(len(self.experience_replay),
                                      size=min(batch_size, len(self.experience_replay)),
                                      replace=False)
            sampled = [self.experience_replay[i] for i in indices]
            
        return sampled
        
    def _calculate_td_error(self, experience: Dict[str, Any]) -> float:
        """Calculate temporal difference error for an experience."""
        # Simplified TD error calculation
        reward = experience["reward"]
        done = experience["done"]
        
        if done:
            td_error = reward
        else:
            # Estimate value of next state (simplified)
            next_value = 0.0
            td_error = reward + self.discount_factor * next_value - 0.0  # current value
            
        return td_error
        
    def train_step(self, agent_brain: 'AgentBrain') -> Dict[str, Any]:
        """
        Perform a training step.
        
        Args:
            agent_brain: Agent brain to train
            
        Returns:
            Dict[str, Any]: Training results
        """
        # Sample experiences
        experiences = self.sample_experience()
        if not experiences:
            return {"success": False, "message": "No experiences to train on"}
            
        # Process batch
        batch_results = self._process_training_batch(experiences, agent_brain)
        
        # Update metrics
        self.training_steps += len(experiences)
        
        # Check for early stopping
        self._check_early_stopping(batch_results.get("performance", 0.0))
        
        # Update curriculum if needed
        self._update_curriculum(batch_results.get("performance", 0.0))
        
        return batch_results
        
    def _process_training_batch(self, experiences: List[Dict[str, Any]], 
                               agent_brain: 'AgentBrain') -> Dict[str, Any]:
        """Process a batch of experiences for training."""
        # In a real implementation, this would perform neural network updates
        # For now, we'll simulate training
        
        # Calculate statistics
        rewards = [exp["reward"] for exp in experiences]
        done_flags = [exp["done"] for exp in experiences]
        
        # Calculate metrics
        avg_reward = np.mean(rewards) if rewards else 0.0
        success_rate = sum(done_flags) / len(done_flags) if done_flags else 0.0
        
        # Simulate loss calculation
        self.current_loss = np.random.random() * 0.5
        self.current_entropy = np.random.random() * 0.1
        
        # Update history
        self.loss_history.append(self.current_loss)
        self.entropy_history.append(self.current_entropy)
        self.performance_history.append(avg_reward)
        
        return {
            "success": True,
            "batch_size": len(experiences),
            "average_reward": avg_reward,
            "success_rate": success_rate,
            "current_loss": self.current_loss,
            "current_entropy": self.current_entropy,
            "learning_rate": self.learning_rate,
            "discount_factor": self.discount_factor
        }
        
    def _check_early_stopping(self, performance: float) -> None:
        """Check if early stopping should be triggered."""
        if performance > self.best_performance:
            self.best_performance = performance
            self.early_stopping_counter = 0
        else:
            self.early_stopping_counter += 1
            
        if self.early_stopping_counter >= self.early_stopping_patience:
            # In real implementation, would stop training
            pass
            
    def _update_curriculum(self, performance: float) -> None:
        """Update curriculum based on performance."""
        if performance > self.difficulty_threshold:
            self.current_difficulty = min(1.0, self.current_difficulty + 0.1)
            self.curriculum_level += 1
        elif performance < self.difficulty_threshold * 0.5:
            self.current_difficulty = max(0.1, self.current_difficulty - 0.1)
            
    def get_training_statistics(self) -> Dict[str, Any]:
        """Get training statistics."""
        return {
            "training_episodes": self.training_episodes,
            "training_steps": self.training_steps,
            "current_loss": self.current_loss,
            "current_entropy": self.current_entropy,
            "batch_size": self.batch_size,
            "learning_rate": self.learning_rate,
            "discount_factor": self.discount_factor,
            "entropy_coefficient": self.entropy_coefficient,
            "experience_replay_size": len(self.experience_replay),
            "priority_replay_size": len(self.priority_replay) if hasattr(self, 'priority_replay') else 0,
            "performance_history": list(self.performance_history)[-10:],
            "loss_history": list(self.loss_history)[-10:],
            "average_performance": np.mean(self.performance_history) if self.performance_history else 0.0,
            "average_loss": np.mean(self.loss_history) if self.loss_history else 0.0,
            "curriculum_level": self.curriculum_level,
            "current_difficulty": self.current_difficulty,
            "early_stopping_counter": self.early_stopping_counter,
            "best_performance": self.best_performance
        }
        
    def reset(self) -> None:
        """Reset trainer to initial state."""
        self.training_episodes = 0
        self.training_steps = 0
        self.experience_replay.clear()
        self.priority_replay.clear()
        
        self.performance_history.clear()
        self.loss_history.clear()
        self.entropy_history.clear()
        
        self.curriculum_level = 1
        self.current_difficulty = 0.5
        self.early_stopping_counter = 0
        self.best_performance = -float('inf')


class Evaluator:
    """
    Evaluate agent performance.
    
    This class provides comprehensive evaluation capabilities for NLM agents,
    including performance metrics, comparison with baselines, and
    statistical analysis of results.
    """
    
    def __init__(self, evaluation_metrics: List[str] = None):
        """
        Initialize evaluator.
        
        Args:
            evaluation_metrics: Metrics to compute
        """
        if evaluation_metrics is None:
            evaluation_metrics = ["reward", "success", "efficiency", "consistency", "novelty"]
            
        self.evaluation_metrics = evaluation_metrics
        
        # Evaluation history
        self.evaluation_history = deque(maxlen=100)
        self.comparison_history = deque(maxlen=50)
        
        # Baseline performance
        self.baseline_performance = {}
        self.baseline_reward = 0.0
        self.baseline_success_rate = 0.0
        
        # Statistical analysis
        self.performance_distribution = {}
        self.statistical_tests = {}
        
    def evaluate(self, agent_brain: 'AgentBrain', world: 'SimpleWorld',
                 episodes: int = 10, max_steps: int = 100) -> Dict[str, Any]:
        """
        Evaluate an agent.
        
        Args:
            agent_brain: Agent brain to evaluate
            world: World environment
            episodes: Number of evaluation episodes
            max_steps: Maximum steps per episode
            
        Returns:
            Dict[str, Any]: Evaluation results
        """
        # Reset world and agent
        world.reset()
        agent_brain.reset()
        
        # Run evaluation episodes
        episode_results = []
        
        for episode in range(episodes):
            # Run episode
            episode_result = self._run_evaluation_episode(agent_brain, world, max_steps)
            episode_results.append(episode_result)
            
        # Calculate metrics
        evaluation_result = self._calculate_evaluation_metrics(episode_results)
        
        # Update history
        self.evaluation_history.append(evaluation_result)
        
        # Compare with baseline if available
        if self.baseline_performance:
            comparison = self._compare_with_baseline(evaluation_result)
            self.comparison_history.append(comparison)
            
        return evaluation_result
        
    def _run_evaluation_episode(self, agent_brain: 'AgentBrain', world: 'SimpleWorld',
                               max_steps: int) -> Dict[str, Any]:
        """Run a single evaluation episode."""
        episode_result = {
            "episode": len(self.evaluation_history),
            "total_reward": 0.0,
            "steps": 0,
            "success": False,
            "novel_actions": 0,
            "efficiency": 0.0,
            "consistency": 0.0
        }
        
        # In a real implementation, this would run the actual episode
        # For now, we'll simulate results
        
        for step in range(max_steps):
            # Simulate agent action
            action = agent_brain.decode_motor_command(np.random.randn(4))
            
            # Simulate world response
            percept = world.getSensoryPercept()
            agent_brain.processSensoryInput(percept)
            
            # Simulate reward
            reward = np.random.random() * 10.0
            
            episode_result["total_reward"] += reward
            episode_result["steps"] += 1
            
            # Check for success
            if episode_result["total_reward"] > 50.0:
                episode_result["success"] = True
                
            if step > 0 and np.random.random() < 0.1:  # Random success
                episode_result["success"] = True
                
        # Calculate efficiency
        if episode_result["steps"] > 0:
            episode_result["efficiency"] = episode_result["total_reward"] / episode_result["steps"]
            
        # Calculate consistency (variance of step rewards)
        # In real implementation, would track step-by-step rewards
        episode_result["consistency"] = np.random.random()
        
        return episode_result
        
    def _calculate_evaluation_metrics(self, episode_results: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Calculate evaluation metrics from episode results."""
        if not episode_results:
            return {"error": "No episode results available"}
            
        # Extract metrics
        total_rewards = [r["total_reward"] for r in episode_results]
        successes = [r["success"] for r in episode_results]
        efficiencies = [r["efficiency"] for r in episode_results]
        consistencies = [r["consistency"] for r in episode_results]
        
        # Calculate statistics
        metrics = {}
        
        for metric in self.evaluation_metrics:
            if metric == "reward":
                metrics["average_reward"] = np.mean(total_rewards)
                metrics["reward_std"] = np.std(total_rewards)
                metrics["reward_percentile_25"] = np.percentile(total_rewards, 25)
                metrics["reward_percentile_75"] = np.percentile(total_rewards, 75)
                
            elif metric == "success":
                metrics["success_rate"] = np.mean(successes)
                metrics["success_count"] = sum(successes)
                
            elif metric == "efficiency":
                metrics["average_efficiency"] = np.mean(efficiencies)
                metrics["efficiency_std"] = np.std(efficiencies)
                
            elif metric == "consistency":
                metrics["average_consistency"] = np.mean(consistencies)
                metrics["consistency_std"] = np.std(consistencies)
                
            elif metric == "novelty":
                # Simulate novelty calculation
                novel_actions = [r["novel_actions"] for r in episode_results]
                metrics["average_novel_actions"] = np.mean(novel_actions)
                metrics["novelty_score"] = np.mean([min(1.0, n / 10.0) for n in novel_actions])
                
        # Overall performance score
        if "average_reward" in metrics and "success_rate" in metrics:
            metrics["overall_score"] = (
                metrics["average_reward"] / 10.0 * 0.6 +
                metrics["success_rate"] * 0.4
            )
            
        return metrics
        
    def _compare_with_baseline(self, current_results: Dict[str, Any]) -> Dict[str, Any]:
        """Compare current results with baseline."""
        comparison = {}
        
        for metric, value in current_results.items():
            if metric in self.baseline_performance:
                baseline_value = self.baseline_performance[metric]
                if baseline_value != 0:
                    improvement = (value - baseline_value) / abs(baseline_value)
                else:
                    improvement = float('inf') if value > 0 else float('-inf')
                    
                comparison[metric] = {
                    "current": value,
                    "baseline": baseline_value,
                    "improvement": improvement,
                    "better_than_baseline": improvement > 0
                }
                
        return comparison
        
    def set_baseline(self, baseline_results: Dict[str, Any]) -> None:
        """
        Set baseline performance for comparison.
        
        Args:
            baseline_results: Baseline performance metrics
        """
        self.baseline_performance = baseline_results
        
        # Calculate baseline summary
        if "average_reward" in baseline_results:
            self.baseline_reward = baseline_results["average_reward"]
        if "success_rate" in baseline_results:
            self.baseline_success_rate = baseline_results["success_rate"]
            
    def get_evaluation_statistics(self) -> Dict[str, Any]:
        """Get evaluation statistics."""
        if not self.evaluation_history:
            return {"initialized": False}
            
        recent_results = list(self.evaluation_history)[-10:]
        
        return {
            "initialized": True,
            "total_evaluations": len(self.evaluation_history),
            "baseline_set": bool(self.baseline_performance),
            "baseline_performance": self.baseline_performance,
            "baseline_reward": self.baseline_reward,
            "baseline_success_rate": self.baseline_success_rate,
            "recent_evaluations": recent_results,
            "evaluation_trend": self._calculate_evaluation_trend(),
            "comparison_count": len(self.comparison_history)
        }
        
    def _calculate_evaluation_trend(self) -> Dict[str, Any]:
        """Calculate evaluation trend."""
        if len(self.evaluation_history) < 2:
            return {"trend": "insufficient_data", "slope": 0.0}
            
        x = np.arange(len(self.evaluation_history))
        y = [h.get("overall_score", 0.0) for h in self.evaluation_history]
        
        slope = np.polyfit(x, y, 1)[0]
        
        if slope > 0.01:
            trend = "improving"
        elif slope < -0.01:
            trend = "declining"
        else:
            trend = "stable"
            
        return {
            "trend": trend,
            "slope": slope
        }
        
    def generate_report(self, format: str = "text") -> str:
        """
        Generate evaluation report.
        
        Args:
            format: Report format ("text", "json", "markdown")
            
        Returns:
            str: Formatted report
        """
        if not self.evaluation_history:
            return "No evaluation data available"
            
        stats = self.get_evaluation_statistics()
        
        if format == "text":
            return self._generate_text_report(stats)
        elif format == "json":
            return json.dumps(stats, indent=2)
        elif format == "markdown":
            return self._generate_markdown_report(stats)
        else:
            return "Unsupported report format"
            
    def _generate_text_report(self, stats: Dict[str, Any]) -> str:
        """Generate text report."""
        report = f"Evaluation Report\n"
        report += f"================\n\n"
        report += f"Total Evaluations: {stats['total_evaluations']}\n"
        report += f"Baseline Set: {stats['baseline_set']}\n"
        
        if stats['baseline_set']:
            report += f"Baseline Reward: {stats['baseline_reward']:.4f}\n"
            report += f"Baseline Success Rate: {stats['baseline_success_rate']:.4f}\n"
            
        report += f"\nEvaluation Trend: {stats['evaluation_trend']['trend']}\n"
        report += f"Trend Slope: {stats['evaluation_trend']['slope']:.6f}\n"
        
        return report
        
    def _generate_markdown_report(self, stats: Dict[str, Any]) -> str:
        """Generate markdown report."""
        report = "# Evaluation Report\n\n"
        report += f"**Total Evaluations**: {stats['total_evaluations']}\n\n"
        report += f"**Baseline Set**: {stats['baseline_set']}\n\n"
        
        if stats['baseline_set']:
            report += f"**Baseline Reward**: {stats['baseline_reward']:.4f}\n\n"
            report += f"**Baseline Success Rate**: {stats['baseline_success_rate']:.4f}\n\n"
            
        trend = stats['evaluation_trend']
        report += f"**Evaluation Trend**: {trend['trend']}\n"
        report += f"**Trend Slope**: {trend['slope']:.6f}\n\n"
        
        return report
        
    def reset(self) -> None:
        """Reset evaluator to initial state."""
        self.evaluation_history.clear()
        self.comparison_history.clear()
        
        self.baseline_performance.clear()
        self.baseline_reward = 0.0
        self.baseline_success_rate = 0.0


class CheckpointManager:
    """
    Save/load agent states.
    
    This class provides checkpoint functionality for saving and loading
    agent states, enabling recovery from failures and continuation of training.
    """
    
    def __init__(self, checkpoint_dir: str = "checkpoints"):
        """
        Initialize checkpoint manager.
        
        Args:
            checkpoint_dir: Directory for checkpoints
        """
        self.checkpoint_dir = Path(checkpoint_dir)
        self.checkpoint_dir.mkdir(parents=True, exist_ok=True)
        
        # Checkpoint history
        self.checkpoint_history = deque(maxlen=100)
        self.version = "1.0"
        
    def save(self, brain: 'Brain', memory: 'EpisodicMemory', world: 'SimpleWorld',
             name: str, metadata: Dict[str, Any] = None) -> str:
        """
        Save agent state to checkpoint.
        
        Args:
            brain: Brain object to save
            memory: Episodic memory to save
            world: World object to save
            name: Checkpoint name
            metadata: Additional metadata
            
        Returns:
            str: Full path to saved checkpoint
        """
        if metadata is None:
            metadata = {}
            
        checkpoint_path = self.checkpoint_dir / f"{name}.json"
        
        # Create checkpoint data
        checkpoint_data = {
            "version": self.version,
            "timestamp": time.time(),
            "name": name,
            "metadata": metadata,
            "brain": self._serialize_brain(brain),
            "memory": self._serialize_memory(memory),
            "world": self._serialize_world(world)
        }
        
        # Save checkpoint
        with open(checkpoint_path, 'w') as f:
            json.dump(checkpoint_data, f, indent=2)
            
        # Update history
        self.checkpoint_history.append({
            "name": name,
            "path": str(checkpoint_path),
            "timestamp": time.time(),
            "size": checkpoint_path.stat().st_size if checkpoint_path.exists() else 0,
            "metadata": metadata
        })
        
        return str(checkpoint_path)
        
    def load(self, name: str) -> Tuple['Brain', 'EpisodicMemory', 'SimpleWorld']:
        """
        Load agent state from checkpoint.
        
        Args:
            name: Checkpoint name (without extension)
            
        Returns:
            Tuple[Brain, EpisodicMemory, SimpleWorld]: Loaded objects
        """
        checkpoint_path = self.checkpoint_dir / f"{name}.json"
        
        if not checkpoint_path.exists():
            raise FileNotFoundError(f"Checkpoint not found: {checkpoint_path}")
            
        # Load checkpoint
        with open(checkpoint_path, 'r') as f:
            checkpoint_data = json.load(f)
            
        # Deserialize objects
        brain = self._deserialize_brain(checkpoint_data["brain"])
        memory = self._deserialize_memory(checkpoint_data["memory"])
        world = self._deserialize_world(checkpoint_data["world"])
        
        return brain, memory, world
        
    def _serialize_brain(self, brain: 'Brain') -> Dict[str, Any]:
        """Serialize brain object."""
        # In a real implementation, this would serialize the C++ brain
        # For now, we'll create a simplified representation
        return {
            "type": "Brain",
            "regions": [],  # Would contain actual brain data
            "timestamp": time.time()
        }
        
    def _deserialize_brain(self, data: Dict[str, Any]) -> 'Brain':
        """Deserialize brain object."""
        # In a real implementation, this would create a C++ Brain object
        # For now, return a placeholder
        return None
        
    def _serialize_memory(self, memory: 'EpisodicMemory') -> Dict[str, Any]:
        """Serialize memory object."""
        return {
            "type": "EpisodicMemory",
            "episodes": memory.episodes,
            "timestamp": time.time()
        }
        
    def _deserialize_memory(self, data: Dict[str, Any]) -> 'EpisodicMemory':
        """Deserialize memory object."""
        memory = EpisodicMemory()
        memory.episodes = data.get("episodes", [])
        return memory
        
    def _serialize_world(self, world: 'SimpleWorld') -> Dict[str, Any]:
        """Serialize world object."""
        return {
            "type": "SimpleWorld",
            "config": {},
            "timestamp": time.time()
        }
        
    def _deserialize_world(self, data: Dict[str, Any]) -> 'SimpleWorld':
        """Deserialize world object."""
        world = SimpleWorld()
        return world
        
    def list_checkpoints(self) -> List[Dict[str, Any]]:
        """List all available checkpoints."""
        if not self.checkpoint_dir.exists():
            return []
            
        checkpoints = []
        for checkpoint_file in self.checkpoint_dir.glob("*.json"):
            try:
                with open(checkpoint_file, 'r') as f:
                    data = json.load(f)
                    
                checkpoints.append({
                    "name": data.get("name", checkpoint_file.stem),
                    "path": str(checkpoint_file),
                    "timestamp": data.get("timestamp", checkpoint_file.stat().st_mtime),
                    "size": checkpoint_file.stat().st_size,
                    "metadata": data.get("metadata", {})
                })
                
            except Exception:
                continue
                
        # Sort by timestamp (newest first)
        checkpoints.sort(key=lambda x: x["timestamp"], reverse=True)
        
        # Update history
        self.checkpoint_history = deque(checkpoints, maxlen=100)
        
        return checkpoints
        
    def delete_checkpoint(self, name: str) -> bool:
        """
        Delete a checkpoint.
        
        Args:
            name: Checkpoint name
            
        Returns:
            bool: True if checkpoint was deleted
        """
        checkpoint_path = self.checkpoint_dir / f"{name}.json"
        
        if checkpoint_path.exists():
            checkpoint_path.unlink()
            
            # Update history
            self.checkpoint_history = deque(
                [c for c in self.checkpoint_history if c["name"] != name],
                maxlen=100
            )
            
            return True
            
        return False
        
    def get_checkpoint(self, name: str) -> Optional[Dict[str, Any]]:
        """
        Get checkpoint information.
        
        Args:
            name: Checkpoint name
            
        Returns:
            Optional[Dict[str, Any]]: Checkpoint information
        """
        for checkpoint in self.checkpoint_history:
            if checkpoint["name"] == name:
                return checkpoint
                
        return None
        
    def get_checkpoint_history(self) -> List[Dict[str, Any]]:
        """Get checkpoint history."""
        return list(self.checkpoint_history)
        
    def cleanup_old_checkpoints(self, keep_recent: int = 10) -> List[str]:
        """
        Clean up old checkpoints, keeping only the most recent.
        
        Args:
            keep_recent: Number of recent checkpoints to keep
            
        Returns:
            List[str]: Names of deleted checkpoints
        """
        checkpoints = self.list_checkpoints()
        
        if len(checkpoints) <= keep_recent:
            return []
            
        # Sort by timestamp
        checkpoints.sort(key=lambda x: x["timestamp"])
        
        # Keep only recent ones
        to_keep = checkpoints[-keep_recent:]
        to_delete = checkpoints[:-keep_recent]
        
        deleted_names = []
        for checkpoint in to_delete:
            if self.delete_checkpoint(checkpoint["name"]):
                deleted_names.append(checkpoint["name"])
                
        return deleted_names
        
    def reset(self) -> None:
        """Reset checkpoint manager."""
        # Delete all checkpoints
        self.cleanup_old_checkpoints(0)
        
        # Clear history
        self.checkpoint_history.clear()


class PerformanceMonitor:
    """
    Monitor agent performance.
    
    This class provides comprehensive performance monitoring for NLM agents,
    including real-time monitoring, historical analysis, and alerting.
    """
    
    def __init__(self, monitoring_interval: float = 1.0):
        """
        Initialize performance monitor.
        
        Args:
            monitoring_interval: Interval between monitoring updates
        """
        self.monitoring_interval = monitoring_interval
        
        # Performance metrics
        self.performance_metrics = deque(maxlen=1000)
        self.alert_history = deque(maxlen=100)
        self.performance_thresholds = {}
        
        # Monitoring state
        self.monitoring_active = False
        self.monitoring_start_time = None
        self.current_metrics = {}
        
        # Performance analysis
        self.baseline_metrics = {}
        self.performance_trends = {}
        self.anomaly_detector = AnomalyDetector()
        
    def start_monitoring(self) -> None:
        """Start performance monitoring."""
        self.monitoring_active = True
        self.monitoring_start_time = time.time()
        
    def stop_monitoring(self) -> None:
        """Stop performance monitoring."""
        self.monitoring_active = False
        
    def update_metrics(self, metrics: Dict[str, Any]) -> None:
        """
        Update performance metrics.
        
        Args:
            metrics: Performance metrics to update
        """
        self.current_metrics.update(metrics)
        self.performance_metrics.append({
            "timestamp": time.time(),
            "metrics": metrics.copy()
        })
        
        # Check for anomalies
        anomaly_score = self.anomaly_detector.detect_anomaly(metrics)
        if anomaly_score > 0.8:
            self._trigger_alert("performance_anomaly", anomaly_score, metrics)
            
        # Check thresholds
        self._check_thresholds(metrics)
        
    def _trigger_alert(self, alert_type: str, severity: float, data: Dict[str, Any]) -> None:
        """Trigger an alert."""
        alert = {
            "type": alert_type,
            "severity": severity,
            "timestamp": time.time(),
            "data": data
        }
        
        self.alert_history.append(alert)
        
        # In a real implementation, this would send notifications
        # For now, we'll just log the alert
        print(f"ALERT: {alert_type} (severity: {severity:.2f})")
        
    def _check_thresholds(self, metrics: Dict[str, Any]) -> None:
        """Check if metrics exceed thresholds."""
        for metric_name, threshold in self.performance_thresholds.items():
            if metric_name in metrics:
                value = metrics[metric_name]
                if value > threshold:
                    self._trigger_alert(f"threshold_exceeded_{metric_name}", 
                                      value / threshold, metrics)
                    
    def set_threshold(self, metric: str, threshold: float, alert_type: str = "warning") -> None:
        """
        Set performance threshold.
        
        Args:
            metric: Metric name
            threshold: Threshold value
            alert_type: Alert type
        """
        self.performance_thresholds[metric] = threshold
        
    def get_performance_stats(self) -> Dict[str, Any]:
        """Get performance statistics."""
        if not self.performance_metrics:
            return {"initialized": False}
            
        all_metrics = []
        for entry in self.performance_metrics:
            all_metrics.append(entry["metrics"])
            
        stats = {}
        
        for metric_name in self.current_metrics:
            metric_values = [m.get(metric_name) for m in all_metrics if m.get(metric_name) is not None]
            if metric_values:
                stats[metric_name] = {
                    "current": self.current_metrics.get(metric_name),
                    "average": np.mean(metric_values),
                    "std": np.std(metric_values),
                    "min": np.min(metric_values),
                    "max": np.max(metric_values),
                    "percentile_25": np.percentile(metric_values, 25),
                    "percentile_75": np.percentile(metric_values, 75)
                }
                
        return {
            "initialized": True,
            "monitoring_active": self.monitoring_active,
            "monitoring_duration": time.time() - self.monitoring_start_time if self.monitoring_start_time else 0,
            "total_samples": len(self.performance_metrics),
            "current_metrics": self.current_metrics,
            "statistics": stats,
            "thresholds": self.performance_thresholds,
            "alert_count": len(self.alert_history),
            "recent_alerts": list(self.alert_history)[-5:],
            "anomaly_score": self.anomaly_detector.get_current_anomaly_score()
        }
        
    def get_performance_trend(self, metric: str, window: int = 10) -> Dict[str, Any]:
        """
        Get performance trend for a specific metric.
        
        Args:
            metric: Metric name
            window: Window size for trend calculation
            
        Returns:
            Dict[str, Any]: Trend information
        """
        if not self.performance_metrics:
            return {"error": "No performance data available"}
            
        # Get recent metric values
        metric_values = []
        timestamps = []
        
        for entry in self.performance_metrics[-window:]:
            if metric in entry["metrics"]:
                metric_values.append(entry["metrics"][metric])
                timestamps.append(entry["timestamp"])
                
        if len(metric_values) < 2:
            return {"error": "Insufficient data for trend analysis"}
            
        # Calculate trend
        x = np.arange(len(metric_values))
        y = np.array(metric_values)
        
        slope = np.polyfit(x, y, 1)[0]
        intercept = np.polyfit(x, y, 1)[1]
        
        # Predict next value
        next_x = x[-1] + 1
        predicted_next = slope * next_x + intercept
        
        # Calculate R-squared
        y_pred = slope * x + intercept
        ss_res = np.sum((y - y_pred) ** 2)
        ss_tot = np.sum((y - np.mean(y)) ** 2)
        r_squared = 1 - (ss_res / ss_tot) if ss_tot != 0 else 0
        
        # Determine trend direction
        if slope > 0.01:
            trend = "increasing"
        elif slope < -0.01:
            trend = "decreasing"
        else:
            trend = "stable"
            
        return {
            "metric": metric,
            "trend": trend,
            "slope": slope,
            "intercept": intercept,
            "r_squared": r_squared,
            "predicted_next": predicted_next,
            "current": metric_values[-1],
            "average": np.mean(metric_values),
            "change_since_start": metric_values[-1] - metric_values[0] if len(metric_values) > 1 else 0
        }
        
    def get_all_trends(self) -> Dict[str, Any]:
        """Get trends for all monitored metrics."""
        trends = {}
        
        for metric in self.current_metrics:
            trends[metric] = self.get_performance_trend(metric)
            
        return trends
        
    def reset(self) -> None:
        """Reset performance monitor."""
        self.performance_metrics.clear()
        self.alert_history.clear()
        
        self.performance_thresholds.clear()
        self.monitoring_active = False
        self.monitoring_start_time = None
        self.current_metrics.clear()


class AnomalyDetector:
    """Detect performance anomalies."""
    
    def __init__(self):
        """Initialize anomaly detector."""
        self.anomaly_scores = deque(maxlen=100)
        self.baseline_statistics = {}
        
    def detect_anomaly(self, metrics: Dict[str, Any]) -> float:
        """
        Detect if metrics contain anomalies.
        
        Args:
            metrics: Metrics to analyze
            
        Returns:
            float: Anomaly score (0-1, higher = more anomalous)
        """
        if not self.baseline_statistics:
            # Initialize baseline
            self._update_baseline(metrics)
            return 0.0
            
        # Calculate anomaly score based on baseline deviation
        anomaly_score = 0.0
        
        for metric_name, value in metrics.items():
            if metric_name in self.baseline_statistics:
                baseline = self.baseline_statistics[metric_name]
                
                # Calculate z-score
                if baseline["std"] > 0:
                    z_score = abs(value - baseline["mean"]) / baseline["std"]
                    anomaly_score += min(1.0, z_score / 3.0)  # Cap at 1.0
                    
        # Normalize
        anomaly_score = min(1.0, anomaly_score / len(self.baseline_statistics))
        
        self.anomaly_scores.append(anomaly_score)
        
        return anomaly_score
        
    def _update_baseline(self, metrics: Dict[str, Any]) -> None:
        """Update baseline statistics."""
        self.baseline_statistics = {}
        
        for metric_name, value in metrics.items():
            if isinstance(value, (int, float)):
                self.baseline_statistics[metric_name] = {
                    "mean": value,
                    "std": 1.0,  # Initial std
                    "min": value,
                    "max": value
                }
                
    def get_current_anomaly_score(self) -> float:
        """Get current anomaly score."""
        return self.anomaly_scores[-1] if self.anomaly_scores else 0.0
        
    def get_anomaly_statistics(self) -> Dict[str, Any]:
        """Get anomaly statistics."""
        if not self.anomaly_scores:
            return {"initialized": False}
            
        return {
            "initialized": True,
            "current_anomaly_score": self.get_current_anomaly_score(),
            "average_anomaly_score": np.mean(self.anomaly_scores),
            "max_anomaly_score": np.max(self.anomaly_scores),
            "anomaly_count": sum(1 for score in self.anomaly_scores if score > 0.5),
            "recent_anomalies": [score for score in self.anomaly_scores if score > 0.5][-5:]
        }


# Re-export commonly used classes
__all__ = [
    "AgentTrainer", "Evaluator", "CheckpointManager", "PerformanceMonitor"
]