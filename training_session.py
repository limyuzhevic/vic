# Enhanced NLM Python Training Module
"""
Enhanced training utilities for NLM (Neural Learning Machine) simulations.
Provides advanced training session management, monitoring, and performance analysis.
"""

import time
import threading
from typing import List, Dict, Any, Optional, Callable
from pathlib import Path
import json

# Import NLM modules
import pynlm
class AdvancedTrainingSession:
    """Advanced training session with comprehensive monitoring and control."""
    
    def __init__(self, brain, world, agent, name: str = "advanced_session"):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.name = name
        self.step_count = 0
        self.start_time = time.time()
        self.performance_data = []
        self.monitoring_active = False
        self.monitoring_thread = None
        self.checkpoint_dir = Path(f".nlm_advanced_checkpoints/{name}")
        self.checkpoint_dir.mkdir(parents=True, exist_ok=True)
    
    def run_training_episode(self, steps: int, verbose: bool = True) -> Dict[str, Any]:
        """Run a complete training episode with comprehensive metrics collection."""
        print(f"Starting training episode: {self.name} ({steps} steps)")
        
        episode_start_step = self.step_count
        episode_start_time = time.time()
        
        # Initialize monitoring data for this episode
        episode_metrics = {
            'episode': len(self.performance_data),
            'name': self.name,
            'steps': steps,
            'start_step': episode_start_step,
            'start_time': episode_start_time,
            'data_points': []
        }
        
        # Run simulation steps
        for step in range(steps):
            # Update world state
            self.world.update(0.1)
            
            # Get and process sensory input
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Brain computation
            self.brain.step(self.step_count)
            
            # Get motor action
            action = self.agent.decodeMotorCommand()
            
            # Apply action to world
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Extract reward and apply learning
            reward = self._extract_training_reward(percept)
            prediction = self.agent.getPredictionError()
            self.agent.applyRewardModulation(reward, prediction)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Record metrics
            metrics_point = self._collect_metrics_point(step)
            episode_metrics['data_points'].append(metrics_point)
            
            self.step_count += 1
            
            if verbose and step % 100 == 0:
                self._log_progress(step, metrics_point)
        
        episode_end_time = time.time()
        episode_metrics['end_time'] = episode_end_time
        episode_metrics['duration'] = episode_end_time - episode_start_time
        episode_metrics['end_step'] = self.step_count
        
        # Calculate episode-level statistics
        episode_summary = self._compute_episode_summary(episode_metrics)
        episode_metrics['summary'] = episode_summary
        
        # Add to performance history
        self.performance_data.append(episode_metrics)
        
        if verbose:
            print(f"Episode complete: {episode_summary}")
        
        return episode_summary
    
    def _extract_training_reward(self, percept) -> float:
        """Extract training reward from percept with adaptive weighting."""
        # Multiple reward signals from different percept components
        reward_signals = []
        
        # Reward from curiosity (exploration motivation)
        curiosity = self.agent.getCuriosityLevel()
        if curiosity > 0.5:
            reward_signals.append(curiosity * 0.5)
        
        # Reward from prediction accuracy
        prediction_error = self.agent.getPredictionError()
        if prediction_error < 0.3:
            reward_signals.append((1.0 - prediction_error) * 0.3)
        
        # Reward from developmental progress
        stage = self.brain.getDevelopmentalStage()
        if stage > 1:  # Not initial stage
            reward_signals.append(0.2)
        
        # Combine rewards with adaptive weights
        total_reward = sum(reward_signals) / len(reward_signals) if reward_signals else 0.外观
        return total_reward
    
    def _collect_metrics_point(self, step: int) -> Dict[str, Any]:
        """Collect detailed metrics at current simulation step."""
        return {
            'step': self.step_count,
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'active_neurons': self.brain.getActiveNeuronCount(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'excitatory_inhibition_ratio': self.brain.getExcitationInhibitionRatio(),
            'curiosity': self.agent.getCuriosityLevel(),
            'novelty': self.agent.getNoveltyLevel(),
            'prediction_error': self.agent.getPredictionError(),
            'developmental_stage': self.brain.getDevelopmentalStage(),
            'world_time': self.world.getSimulationTime(),
            'timestamp': time.time()
        }
    
    def _compute_episode_summary(self, episode_metrics: Dict[str, Any]) -> Dict[str, Any]:
        """Compute episode-level summary statistics."""
        data_points = episode_metrics['data_points']
        
        if not data_points:
            return {'error': 'No data points collected'}
        
        firing_rates = [dp['average_firing_rate'] for dp in data_points]
        curiosities = [dp['curiosity'] for dp in data_points]
        novelty_levels = [dp['novelty'] for dp in data_points]
        prediction_errors = [dp['prediction_error'] for dp in data_points]
        
        return {
            'steps_completed': episode_metrics['steps'],
            'duration_seconds': episode_metrics['duration'],
            'final_firing_rate': firing_rates[-1] if firing_rates else 0,
            'average_firing_rate': sum(firing_rates) / len(firing_rates),
            'min_firing_rate': min(firing_rates) if firing_rates else 0,
            'max_firing_rate': max(firing_rates) if firing_rates else 0,
            'final_curiosity': curiosities[-1] if curiosities else 0,
            'average_curiosity': sum(curiosities) / len(curiosities),
            'final_novelty': novelty_levels[-1] if novelty_levels else 0,
            'average_novelty': sum(novelty_levels) / len(novelty_levels),
            'final_prediction_error': prediction_errors[-1] if prediction_errors else 0,
            'average_prediction_error': sum(prediction_errors) / len(prediction_errors),
            'stability_index': self._compute_stability_index(data_points)
        }
    
    def _compute_stability_index(self, data_points: List[Dict[str, Any]]) -> float:
        """Compute stability index based on variance of key metrics."""
        if len(data_points) < 10:
            return 0.0
        
        # Compute coefficient of variation for key metrics
        firing_rates = [dp['average_firing_rate'] for dp in data_points]
        curiosities = [dp['curiosity'] for dp in data_points]
        
        def coefficient_of_variation(data):
            if not data or sum(data) == 0:
                return 0.0
            mean = sum(data) / len(data)
            variance = sum((x - mean) ** 2 for x in data) / len(data)
            std_dev = variance ** 0.5
            return std_dev / mean if mean > 0 else 0.0
        
        cv_firing = coefficient_of_variation(firing_rates)
        cv_curiosity = coefficient_of_variation(curiosities)
        
        # Lower CV means more stable (higher stability index)
        stability = 1.0 / (1.0 + cv_firing + cv_curiosity)
        return stability
    
    def _log_progress(self, step: int, metrics: Dict[str, Any]):
        """Log training progress with formatted output."""
        firing = metrics['firing_neurons']
        rate = metrics['average_firing_rate']
        curiosity = metrics['curiosity']
        novelty = metrics['novelty']
        error = metrics['prediction_error']
        
        print(f"  Step {step:4d}: {firing:4d} neurons firing, "
              f"rate {rate:6.2f}, curiosity {curiosity:5.3f}, "
              f"novelty {novelty:5.3f}, error {error:5.3f}")
    
    def save_episode_checkpoint(self, episode: int = None) -> str:
        """Save current brain state as a checkpoint."""
        if episode is None:
            episode = len(self.performance_data)
        
        checkpoint_path = self.checkpoint_dir / f"episode_{episode}_checkpoint_{self.step_count}.bin"
        self.brain.save(str(checkpoint_path))
        return str(checkpoint_path)
    
    def load_checkpoint(self, checkpoint_path: str) -> bool:
        """Load brain state from checkpoint file."""
        return self.brain.load(checkpoint_path)
    
    def get_training_statistics(self) -> Dict[str, Any]:
        """Get comprehensive training statistics."""
        if not self.performance_data:
            return {'error': 'No training data available'}
        
        all_data = []
        for episode in self.performance_data:
            all_data.extend(episode['data_points'])
        
        if not all_data:
            return {'error': 'No data points collected'}
        
        # Compute aggregate statistics
        firing_rates = [dp['average_firing_rate'] for dp in all_data]
        curiosities = [dp['curiosity'] for dp in all_data]
        novelty_levels = [dp['novelty'] for dp in all_data]
        prediction_errors = [dp['prediction_error'] for dp in all_data]
        
        return {
            'total_training_steps': self.step_count,
            'total_episodes': len(self.performance_data),
            'total_duration': time.time() - self.start_time,
            'aggregate_metrics': {
                'average_firing_rate': sum(firing_rates) / len(firing_rates),
                'average_curiosity': sum(curiosities) / len(curiosities),
                'average_novelty': sum(novelty_levels) / len(novelty_levels),
                'average_prediction_error': sum(prediction_errors) / len(prediction_errors),
                'firing_rate_stability': self._compute_stability_index(all_data)
            },
            'final_state': {
                'firing_neurons': self.brain.getFiringNeuronCount(),
                'developmental_stage': self.brain.getDevelopmentalStage(),
                'total_spikes': self.brain.getTotalSpikeCount()
            }
        }
    
    def start_monitoring(self, interval: float = 1.0) -> threading.Thread:
        """Start continuous monitoring in a background thread."""
        self.monitoring_active = True
        
        def monitor_loop():
            while self.monitoring_active:
                self._log_monitoring_data()
                time.sleep(interval)
        
        self.monitoring_thread = threading.Thread(target=monitor_loop, daemon=True)
        self.monitoring_thread.start()
        return self.monitoring_thread
    
    def _log_monitoring_data(self):
        """Log monitoring data for current state."""
        metrics = {
            'timestamp': time.time(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'curiosity': self.agent.getCuriosityLevel(),
            'novelty': self.agent.getNoveltyLevel(),
            'prediction_error': self.agent.getPredictionError()
        }
        
        # Print monitoring line
        print(f"[MONITOR] Firing: {metrics['firing_neurons']}, "
              f"Curiosity: {metrics['curiosity']:.3f}, "
              f"Novelty: {metrics['novelty']:.3f}")
    
    def stop_monitoring(self):
        """Stop the monitoring thread."""
        self.monitoring_active = False
        if self.monitoring_thread:
            self.monitoring_thread.join(timeout=5.0)
            self.monitoring_thread = None
    
    def export_training_data(self, filepath: str = None) -> str:
        """Export training data to JSON file."""
        if filepath is None:
            filepath = f".nlm_training_export_{self.name}_{int(time.time())}.json"
        
        export_data = {
            'training_session': self.name,
            'start_time': self.start_time,
            'total_steps': self.step_count,
            'episodes': self.performance_data
        }
        
        with open(filepath, 'w') as f:
            json.dump(export_data, f, indent=2, default=str)
        
        return filepath
    
    def import_training_data(self, filepath: str) -> bool:
        """Import training data from JSON file."""
        try:
            with open(filepath, 'r') as f:
                data = json.load(f)
            
            self.performance_data = data.get('episodes', [])
            # Note: This doesn't restore brain state, just performance data
            return True
        except Exception as e:
            print(f"Error importing training data: {e}")
            return False
    
    def compare_with_baseline(self, baseline_data: List[Dict]) -> Dict[str, float]:
        """Compare current training performance with baseline."""
        if not baseline_data or not self.performance_data:
            return {'error': 'Insufficient data for comparison'}
        
        # Calculate performance improvements
        baseline_firing = sum(ep.get('summary', {}).get('average_firing_rate', 0) 
                             for ep in baseline_data) / len(baseline_data)
        
        current_firing = sum(ep.get('summary', {}).get('average_firing_rate', 0)
                           for ep in self.performance_data) / len(self.performance_data)
        
        improvement = (current_firing - baseline_firing) / baseline_firing if baseline_firing > 0 else 0
        
        return {
            'firing_rate_improvement': improvement,
            'baseline_firing_rate': baseline_firing,
            'current_firing_rate': current_firing,
            'episodes_completed': len(self.performance_data)
        }
    
    def __enter__(self):
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        # Cleanup resources
        self.stop_monitoring()
        self.world.reset()
        self.brain.reset()
        self.agent.reset()