# NLM Python Learning Module
#
# High-level learning utilities for training NLM brains.
# Provides convenient methods for learning experiments, evaluation, and metrics collection.

from typing import List, Dict, Any, Callable, Optional
from enum import Enum
import json
import time

from .nlm import NLMConfiguration, NLMBrain, NLMWorld, NLMEnvironment

class LearningExperiment:
    """High-level wrapper for NLM learning experiments."""
    
    def __init__(self, environment: NLMEnvironment):
        self.environment = environment
        self.results = []
    
    def run_experiment(self, name: str, num_episodes: int = 10, max_steps: int = 1000) -> Dict[str, Any]:
        """Run a learning experiment with given parameters."""
        print(f"Running experiment: {name}")
        print(f"  Episodes: {num_episodes}, Max steps per episode: {max_steps}")
        
        experiment_start = time.time()
        results = self.environment.run_lifetime_learning(num_episodes, max_steps)
        experiment_end = time.time()
        
        experiment_result = {
            "name": name,
            "start_time": experiment_start,
            "end_time": experiment_end,
            "duration": experiment_end - experiment_start,
            "results": results
        }
        
        self.results.append(experiment_result)
        print(f"Experiment '{name}' completed in {experiment_end - experiment_start:.2f}s")
        return experiment_result
    
    def compare_experiments(self, experiment_configs: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Compare multiple experiments with different configurations."""
        print("Comparing experiments with different configurations...")
        
        comparison_results = {}
        
        for config in experiment_configs:
            name = config["name"]
            print(f"\n--- Running comparison: {name} ---")
            
            # Create fresh environment for each experiment
            env = NLMEnvironment(width=config.get("width", 20), 
                                 height=config.get("height", 20))
            env.create_brain(neuron_count=config.get("neurons", 1000))
            env.create_agent()
            
            # Run experiment
            result = env.run_lifetime_learning(
                num_episodes=config.get("episodes", 5),
                max_steps_per_episode=config.get("max_steps", 500)
            )
            
            comparison_results[name] = {
                "config": config,
                "results": result,
                "environment": env  # Keep reference for analysis
            }
        
        return comparison_results
    
    def analyze_results(self, experiment_name: str) -> Dict[str, Any]:
        """Analyze results from a specific experiment."""
        for experiment in self.results:
            if experiment["name"] == experiment_name:
                results = experiment["results"]
                
                # Compute advanced metrics
                episode_results = results["episode_results"]
                if not episode_results:
                    return {"error": "No episode results to analyze"}
                
                # Learning curve analysis
                rewards = [ep["total_reward"] for ep in episode_results]
                steps = [ep["total_steps"] for ep in episode_results]
                
                learning_curve = {
                    "episode": list(range(len(episode_results))),
                    "reward": rewards,
                    "steps": steps,
                    "avg_reward": sum(rewards) / len(rewards),
                    "reward_improvement": rewards[-1] - rewards[0] if len(rewards) > 1 else 0,
                    "reward_improvement_rate": (rewards[-1] - rewards[0]) / len(rewards) if len(rewards) > 1 else 0,
                }
                
                # Performance metrics
                performance = {
                    "total_episodes": len(episode_results),
                    "total_steps": sum(steps),
                    "total_reward": sum(rewards),
                    "avg_steps_per_episode": sum(steps) / len(steps),
                    "avg_reward_per_episode": sum(rewards) / len(rewards),
                    "reward_variance": self._variance(rewards),
                    "steps_variance": self._variance(steps),
                }
                
                return {
                    "experiment_name": experiment_name,
                    "learning_curve": learning_curve,
                    "performance": performance,
                    "detailed_results": results
                }
        
        return {"error": f"Experiment '{experiment_name}' not found"}
    
    def _variance(self, values: List[float]) -> float:
        """Calculate variance of a list of values."""
        if len(values) < 2:
            return 0.0
        
        mean = sum(values) / len(values)
        squared_diffs = [(x - mean) ** 2 for x in values]
        return sum(squared_diffs) / len(squared_diffs)

class NeuromodulationAnalyzer:
    """Analyze neuromodulation dynamics during learning."""
    
    @staticmethod
    def analyze_dopamine_dynamics(environment: NLMEnvironment) -> Dict[str, Any]:
        """Analyze dopamine level dynamics across an episode."""
        metrics_history = environment.metrics_history
        if not metrics_history:
            return {"error": "No metrics history available"}
        
        dopamine_levels = [m["dopamine_level"] for m in metrics_history]
        curiosity_levels = [m["curiosity_level"] for m in metrics_history]
        novelty_levels = [m["novelty_level"] for m in metrics_history]
        
        analysis = {
            "dopamine": {
                "initial": dopamine_levels[0] if dopamine_levels else 0,
                "final": dopamine_levels[-1] if dopamine_levels else 0,
                "max": max(dopamine_levels) if dopamine_levels else 0,
                "min": min(dopamine_levels) if dopamine_levels else 0,
                "mean": sum(dopamine_levels) / len(dopamine_levels) if dopamine_levels else 0,
                "variance": NeuromodulationAnalyzer._variance(dopamine_levels),
                "trend": "increasing" if len(dopamine_levels) > 1 and dopamine_levels[-1] > dopamine_levels[0] else "decreasing"
            },
            "curiosity": {
                "initial": curiosity_levels[0] if curiosity_levels else 0,
                "final": curiosity_levels[-1] if curiosity_levels else 0,
                "max": max(curiosity_levels) if curiosity_levels else 0,
                "min": min(curiosity_levels) if curiosity_levels else 0,
                "mean": sum(curiosity_levels) / len(curiosity_levels) if curiosity_levels else 0,
                "variance": NeuromodulationAnalyzer._variance(curiosity_levels),
                "trend": "increasing" if len(curiosity_levels) > 1 and curiosity_levels[-1] > curiosity_levels[0] else "decreasing"
            },
            "novelty": {
                "initial": novelty_levels[0] if novelty_levels else 0,
                "final": novelty_levels[-1] if novelty_levels else 0,
                "max": max(novelty_levels) if novelty_levels else 0,
                "min": min(novelty_levels) if novelty_levels else 0,
                "mean": sum(novelty_levels) / len(novelty_levels) if novelty_levels else 0,
                "variance": NeuromodulationAnalyzer._variance(novelty_levels),
                "trend": "increasing" if len(novelty_levels) > 1 and novelty_levels[-1] > novelty_levels[0] else "decreasing"
            },
            "correlation_analysis": NeuromodulationAnalyzer._analyze_correlations(
                dopamine_levels, curiosity_levels, novelty_levels
            ),
            "temporal_patterns": NeuromodulationAnalyzer._detect_temporal_patterns(
                dopamine_levels, curiosity_levels, novelty_levels
            )
        }
        
        return analysis
    
    @staticmethod
    def _variance(values: List[float]) -> float:
        """Calculate variance of a list of values."""
        if len(values) < 2:
            return 0.0
        
        mean = sum(values) / len(values)
        squared_diffs = [(x - mean) ** 2 for x in values]
        return sum(squared_diffs) / len(squared_diffs)
    
    @staticmethod
    def _analyze_correlations(dopamine: List[float], curiosity: List[float], novelty: List[float]) -> Dict[str, float]:
        """Analyze correlations between neuromodulators."""
        if len(dopamine) != len(curiosity) or len(dopamine) != len(novelty):
            return {"error": "Length mismatch in neuromodulator data"}
        
        def correlation(x: List[float], y: List[float]) -> float:
            if len(x) < 2:
                return 0.0
            
            x_mean = sum(x) / len(x)
            y_mean = sum(y) / len(y)
            
            numerator = sum((x[i] - x_mean) * (y[i] - y_mean) for i in range(len(x)))
            denominator_x = sum((xi - x_mean) ** 2 for xi in x)
            denominator_y = sum((yi - y_mean) ** 2 for yi in y)
            
            if denominator_x == 0 or denominator_y == 0:
                return 0.0
            
            return numerator / (denominator_x * denominator_y) ** 0.5
        
        return {
            "dopamine_curiosity": correlation(dopamine, curiosity),
            "dopamine_novelty": correlation(dopamine, novelty),
            "curiosity_novelty": correlation(curiosity, novelty)
        }
    
    @staticmethod
    def _detect_temporal_patterns(dopamine: List[float], curiosity: List[float], novelty: List[float]) -> Dict[str, Any]:
        """Detect temporal patterns in neuromodulator dynamics."""
        if len(dopamine) < 10:
            return {"error": "Insufficient data for pattern detection"}
        
        patterns = {}
        
        # Detect peaks and valleys
        def detect_extrema(values: List[float]) -> Dict[str, List[int]]:
            peaks = []
            valleys = []
            
            for i in range(1, len(values) - 1):
                if values[i] > values[i-1] and values[i] > values[i+1]:
                    peaks.append(i)
                elif values[i] < values[i-1] and values[i] < values[i+1]:
                    valleys.append(i)
            
            return {"peaks": peaks, "valleys": valleys}
        
        patterns["dopamine_extrema"] = detect_extrema(dopamine)
        patterns["curiosity_extrema"] = detect_extrema(curiosity)
        patterns["novelty_extrema"] = detect_extrema(novelty)
        
        # Detect oscillatory patterns (simple heuristic)
        def has_oscillation(values: List[float], window: int = 5) -> bool:
            if len(values) < window * 2:
                return False
            
            # Check for consistent direction changes
            direction_changes = 0
            for i in range(window, len(values)):
                if values[i] > values[i-1] and values[i-window] > values[i-window-1]:
                    direction_changes += 1
                elif values[i] < values[i-1] and values[i-window] < values[i-window-1]:
                    direction_changes += 1
            
            return direction_changes > len(values) * 0.3  # More than 30% direction changes
        
        patterns["oscillatory_patterns"] = {
            "dopamine": has_oscillation(dopamine),
            "curiosity": has_oscillation(curiosity),
            "novelty": has_oscillation(novelty)
        }
        
        return patterns

class VisualizationTools:
    """Tools for visualizing NLM learning results."""
    
    @staticmethod
    def create_learning_curve_plot(analysis: Dict[str, Any], save_path: Optional[str] = None) -> Dict[str, Any]:
        """Create a learning curve visualization from experiment analysis."""
        learning_curve = analysis.get("learning_curve", {})
        
        # In a real implementation, this would use matplotlib to create plots
        # For now, we'll return a structured representation of what the plot would show
        
        plot_data = {
            "title": f"Learning Curve: {analysis.get('experiment_name', 'Experiment')}",
            "x_axis": "Episode",
            "y_axis": "Reward",
            "data_points": [
                {"episode": ep, "reward": reward, "steps": steps}
                for ep, reward, steps in zip(
                    learning_curve.get("episode", []),
                    learning_curve.get("reward", []),
                    learning_curve.get("steps", [])
                )
            ],
            "annotations": []
        }
        
        # Add convergence annotation if appropriate
        if len(learning_curve.get("reward", [])) > 10:
            rewards = learning_curve["reward"]
            recent_rewards = rewards[-5:]  # Last 5 episodes
            avg_recent = sum(recent_rewards) / len(recent_rewards)
            avg_early = sum(rewards[:5]) / 5 if len(rewards) >= 5 else rewards[0]
            
            if avg_recent > avg_early * 1.2:  # 20% improvement
                plot_data["annotations"].append({
                    "text": "Convergence achieved",
                    "position": "top_right"
                })
        
        if save_path:
            try:
                with open(save_path, 'w') as f:
                    json.dump(plot_data, f, indent=2)
                print(f"Plot data saved to {save_path}")
            except Exception as e:
                print(f"Failed to save plot: {e}")
        
        return plot_data
    
    @staticmethod
    def create_neuromodulation_animation_data(analysis: Dict[str, Any]) -> Dict[str, Any]:
        """Create animation data for neuromodulator dynamics."""
        neuromod_data = analysis.get("neuromodulation", {})
        
        # Create time-series data for animation
        animation_data = {
            "title": "Neuromodulator Dynamics During Learning",
            "time_series": [],
            "mediators": ["dopamine", "curiosity", "novelty"],
            "colors": {
                "dopamine": "#FF6B6B",
                "curiosity": "#4ECDC4",
                "novelty": "#45B7D1"
            }
        }
        
        # Use metrics history if available
        if hasattr(analysis, 'environment') and analysis['environment'].metrics_history:
            metrics = analysis['environment'].metrics_history
            for i, metric in enumerate(metrics):
                time_series_point = {
                    "step": metric["step"],
                    "time": i * 0.001,  # Assuming 1ms per step
                    "dopamine": metric.get("dopamine_level", 0),
                    "curiosity": metric.get("curiosity_level", 0),
                    "novelty": metric.get("novelty_level", 0)
                }
                animation_data["time_series"].append(time_series_point)
        
        return animation_data
    
    @staticmethod
    def export_results_for_notebook(analysis: Dict[str, Any], filename: str = "nlm_results.json"):
        """Export results in a format suitable for Jupyter notebooks."""
        export_data = {
            "analysis_metadata": {
                "timestamp": time.time(),
                "experiment_name": analysis.get("experiment_name"),
                "analysis_version": "1.0.0"
            },
            "learning_analysis": analysis.get("learning_curve", {}),
            "performance_metrics": analysis.get("performance", {}),
            "neuromodulation_analysis": analysis.get("neuromodulation", {})
        }
        
        try:
            with open(filename, 'w') as f:
                json.dump(export_data, f, indent=2, default=str)
            
            print(f"Results exported to {filename} for notebook use")
            return True
        except Exception as e:
            print(f"Failed to export results: {e}")
            return False
