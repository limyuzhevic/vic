#!/usr/bin/env python3
"""
Learning Performance Metrics Module

Tools for measuring learning efficiency and convergence in NLM brain simulations.
Provides comprehensive learning metrics and performance analysis.
"""

import time
import logging
import statistics
import numpy as np
from typing import Dict, List, Optional, Any, Tuple
from dataclasses import dataclass, field
from enum import Enum
from collections import defaultdict
import math

from .base import PerformanceMetrics, BenchmarkResult, ProfilingFramework, OutputFormat

logger = logging.getLogger("nlm_profiling.learning")


class LearningAnalysisMode(Enum):
    """Learning analysis mode enumeration."""
    CONVERGENCE = "convergence"
    EFFICIENCY = "efficiency"
    ADAPTATION = "adaptation"
    STABILITY = "stability"
    OPTIMIZATION = "optimization"


class LearningProgressTracker:
    """Tracks learning progress over time."""
    
    def __init__(self, initial_value: float = 0.0):
        self.initial_value = initial_value
        self.current_value = initial_value
        self.max_value = initial_value
        self.min_value = initial_value
        self.peak_values: List[float] = [initial_value]
        self.step_values: List[float] = []
        self.timestamps: List[float] = []
        self.convergence_threshold = 0.01
        self.learning_rate = 0.0
    
    def update(self, new_value: float, timestamp: float = None) -> None:
        """Update the tracker with a new value."""
        if timestamp is None:
            timestamp = time.time()
        
        self.step_values.append(new_value)
        self.timestamps.append(timestamp)
        
        if new_value > self.max_value:
            self.max_value = new_value
        
        if new_value < self.min_value:
            self.min_value = new_value
        
        self.current_value = new_value
        self.peak_values.append(self.max_value)
    
    def get_convergence_step(self) -> Optional[int]:
        """Get the step where convergence was achieved."""
        if len(self.step_values) < 2:
            return None
        
        # Check if values have converged (stddev < threshold)
        for i in range(len(self.step_values) - 1):
            window = self.step_values[i:i+10]  # Look at last 10 values
            if len(window) >= 2:
                stddev = statistics.stdev(window)
                if stddev < self.convergence_threshold:
                    return i
        
        return None
    
    def calculate_learning_curve_fit(self) -> Tuple[float, float]:
        """Calculate learning curve parameters (slope, R²)."""
        if len(self.step_values) < 3:
            return 0.0, 0.0
        
        # Simple linear regression for learning curve
        steps = list(range(len(self.step_values)))
        values = self.step_values
        
        n = len(steps)
        if n < 2:
            return 0.0, 0.0
        
        # Calculate slope (learning rate)
        x_mean = statistics.mean(steps)
        y_mean = statistics.mean(values)
        
        numerator = sum((x - x_mean) * (y - y_mean) for x, y in zip(steps, values))
        denominator = sum((x - x_mean) ** 2 for x in steps)
        
        if denominator == 0:
            return 0.0, 0.0
        
        slope = numerator / denominator
        
        # Calculate R² (coefficient of determination)
        ss_total = sum((y - y_mean) ** 2 for y in values)
        ss_residual = sum((y - (y_mean + slope * (x - x_mean))) ** 2 for x, y in zip(steps, values))
        
        if ss_total == 0:
            r_squared = 1.0
        else:
            r_squared = 1 - (ss_residual / ss_total)
        
        self.learning_rate = slope
        
        return slope, r_squared
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary."""
        return {
            "initial_value": self.initial_value,
            "current_value": self.current_value,
            "max_value": self.max_value,
            "min_value": self.min_value,
            "step_values": self.step_values,
            "timestamps": self.timestamps,
            "convergence_threshold": self.convergence_threshold,
            "learning_rate": self.learning_rate,
            "convergence_step": self.get_convergence_step(),
            "total_steps": len(self.step_values)
        }


class LearningPerformanceMetricsConfig:
    """Configuration for learning performance metrics."""
    
    def __init__(self, steps: int = 100, analysis_mode: LearningAnalysisMode = LearningAnalysisMode.CONVERGENCE,
                 convergence_threshold: float = 0.01, enable_adaptation_analysis: bool = True,
                 enable_stability_analysis: bool = True):
        self.steps = steps
        self.analysis_mode = analysis_mode
        self.convergence_threshold = convergence_threshold
        self.enable_adaptation_analysis = enable_adaptation_analysis
        self.enable_stability_analysis = enable_stability_analysis
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary."""
        return {
            "steps": self.steps,
            "analysis_mode": self.analysis_mode.value,
            "convergence_threshold": self.convergence_threshold,
            "enable_adaptation_analysis": self.enable_adaptation_analysis,
            "enable_stability_analysis": self.enable_stability_analysis
        }


class LearningPerformanceMetricsModule(ProfilingFramework):
    """Learning Performance Metrics for measuring learning efficiency and convergence."""
    
    def __init__(self, config: Optional[LearningPerformanceMetricsConfig] = None):
        super().__init__("learning_performance_metrics")
        self.config = config or LearningPerformanceMetricsConfig()
        self.learning_trackers: Dict[str, LearningProgressTracker] = {}
        self.time_series_data: Dict[str, List[float]] = defaultdict(list)
    
    def _profile(self, **kwargs) -> BenchmarkResult:
        """Profile learning performance."""
        try:
            import pynlm
            
            steps = kwargs.get('steps', self.config.steps)
            convergence_threshold = kwargs.get('convergence_threshold', self.config.convergence_threshold)
            
            # Create brain for learning analysis
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            self.logger.info(f"Starting learning performance analysis: {steps} steps")
            
            # Initialize learning trackers
            self._initialize_learning_trackers()
            
            # Collect learning data over time
            for step in range(steps):
                brain.step(step)
                
                # Collect learning-related metrics
                self.time_series_data["step"].append(step)
                self.time_series_data["total_spike_count"].append(brain.getTotalSpikeCount())
                self.time_series_data["average_firing_rate"].append(brain.getAverageFiringRate())
                
                # Calculate learning progress metrics
                self._calculate_learning_metrics(step, brain)
                
                # Log progress
                if step % 100 == 0:
                    self.logger.info(f"Step {step}/{steps} completed")
            
            # Analyze learning performance
            self._analyze_learning_performance()
            
            # Generate metrics
            metrics = self._generate_learning_metrics()
            
            result = BenchmarkResult(
                test_name=self.name,
                parameters=kwargs,
                metrics=metrics,
                duration=steps * 0.001,  # Estimated duration
                success=True
            )
            
            self.results = {
                "time_series": dict(self.time_series_data),
                "learning_trackers": {name: tracker.to_dict() for name, tracker in self.learning_trackers.items()},
                "metrics": [m.to_dict() for m in metrics]
            }
            return result
            
        except Exception as e:
            self.logger.error(f"Error in learning performance metrics: {e}")
            raise
    
    def _initialize_learning_trackers(self) -> None:
        """Initialize learning progress trackers."""
        # Initialize trackers for key learning metrics
        self.learning_trackers["spike_count_growth"] = LearningProgressTracker(0.0)
        self.learning_trackers["firing_rate_efficiency"] = LearningProgressTracker(0.0)
        self.learning_trackers["learning_convergence"] = LearningProgressTracker(0.0)
        self.learning_trackers["adaptation_rate"] = LearningProgressTracker(0.0)
    
    def _calculate_learning_metrics(self, step: int, brain) -> None:
        """Calculate learning metrics for each tracker."""
        try:
            total_spikes = brain.getTotalSpikeCount()
            firing_rate = brain.getAverageFiringRate()
            
            # Calculate spike count growth (learning indicator)
            if step == 0:
                initial_spikes = total_spikes
            else:
                prev_spikes = self.time_series_data["total_spike_count"][step - 1]
                growth = (total_spikes - prev_spikes) / max(prev_spikes, 1)
            
            self.learning_trackers["spike_count_growth"].update(growth if step > 0 else 0)
            
            # Calculate firing rate efficiency (normalized)
            efficiency = firing_rate / max(brain.getTotalNeuronCount(), 1)
            self.learning_trackers["firing_rate_efficiency"].update(efficiency)
            
            # Calculate learning convergence (rate of change)
            if step > 0 and step > 10:
                recent_values = self.learning_trackers["spike_count_growth"].step_values[-10:]
                if len(recent_values) >= 2:
                    adaptation = statistics.mean(recent_values[1:]) - statistics.mean(recent_values[:-1])
                    self.learning_trackers["adaptation_rate"].update(adaptation)
            
            # Calculate learning convergence metric
            convergence = 1.0 / (1.0 + abs(firing_rate - 5.0))  # Target firing rate of 5 Hz
            self.learning_trackers["learning_convergence"].update(convergence)
            
        except Exception as e:
            self.logger.error(f"Error calculating learning metrics: {e}")
    
    def _analyze_learning_performance(self) -> None:
        """Analyze overall learning performance."""
        try:
            learning_analysis = {
                "total_trackers": len(self.learning_trackers),
                "converged_trackers": 0,
                "average_learning_rate": 0.0,
                "overall_efficiency_score": 0.0
            }
            
            total_learning_rate = 0.0
            valid_trackers = 0
            
            for name, tracker in self.learning_trackers.items():
                # Check if tracker has converged
                if tracker.get_convergence_step() is not None:
                    learning_analysis["converged_trackers"] += 1
                
                # Get learning curve parameters
                slope, r_squared = tracker.calculate_learning_curve_fit()
                total_learning_rate += slope
                valid_trackers += 1
                
                # Calculate efficiency score
                if tracker.max_value > tracker.initial_value:
                    efficiency = (tracker.max_value - tracker.initial_value) / tracker.max_value
                else:
                    efficiency = 0.0
                
                learning_analysis[f"{name}_efficiency"] = efficiency
            
            if valid_trackers > 0:
                learning_analysis["average_learning_rate"] = total_learning_rate / valid_trackers
            
            # Calculate overall efficiency score
            efficiencies = [learning_analysis[f"{name}_efficiency"] for name in self.learning_trackers.keys()]
            if efficiencies:
                learning_analysis["overall_efficiency_score"] = statistics.mean(efficiencies)
            
            # Determine learning quality
            efficiency_score = learning_analysis["overall_efficiency_score"]
            convergence_rate = learning_analysis["converged_trackers"] / learning_analysis["total_trackers"]
            
            if efficiency_score > 0.8 and convergence_rate > 0.7:
                learning_analysis["learning_quality"] = "Excellent - Fast and efficient convergence"
            elif efficiency_score > 0.6 and convergence_rate > 0.5:
                learning_analysis["learning_quality"] = "Good - Acceptable learning rate and convergence"
            elif efficiency_score > 0.4 or convergence_rate > 0.3:
                learning_analysis["learning_quality"] = "Fair - Moderate performance, needs improvement"
            else:
                learning_analysis["learning_quality"] = "Poor - Slow or inefficient learning"
            
            self.learning_analysis = learning_analysis
            
        except Exception as e:
            self.logger.error(f"Error analyzing learning performance: {e}")
    
    def _generate_learning_metrics(self) -> List[PerformanceMetrics]:
        """Generate learning-related metrics."""
        metrics = []
        
        # Add time series metrics
        for metric_name, values in self.time_series_data.items():
            if len(values) > 0:
                metrics.append(PerformanceMetrics(
                    timestamp=time.time(),
                    metric_name=f"time_series_{metric_name}",
                    value=float(values[-1]),
                    unit="" if metric_name == "step" else ""
                ))
        
        # Add learning tracker metrics
        if hasattr(self, 'learning_analysis'):
            for key, value in self.learning_analysis.items():
                if isinstance(value, (int, float)):
                    metrics.append(PerformanceMetrics(
                        timestamp=time.time(),
                        metric_name=f"learning_{key}",
                        value=float(value),
                        unit="" if "rate" in key or "score" in key else ""
                    ))
        
        # Add convergence metrics
        for name, tracker in self.learning_trackers.items():
            convergence_step = tracker.get_convergence_step()
            if convergence_step is not None:
                metrics.append(PerformanceMetrics(
                    timestamp=time.time(),
                    metric_name=f"{name}_convergence_step",
                    value=float(convergence_step),
                    unit="steps"
                ))
            
            slope, r_squared = tracker.calculate_learning_curve_fit()
            metrics.append(PerformanceMetrics(
                timestamp=time.time(),
                metric_name=f"{name}_learning_rate",
                value=slope,
                unit="value/step"
            ))
            
            metrics.append(PerformanceMetrics(
                timestamp=time.time(),
                metric_name=f"{name}_curve_fit_r2",
                value=r_squared,
                unit="R²"
            ))
        
        return metrics
    
    def generate_learning_insights(self, result: Dict[str, Any]) -> Dict[str, Any]:
        """Generate insights from learning performance analysis."""
        insights = {}
        
        try:
            if "metrics" not in result:
                return insights
            
            # Extract key learning metrics
            total_spikes = next((m["value"] for m in result["metrics"] 
                               if m["metric_name"] == "time_series_total_spike_count"), 0)
            avg_firing_rate = next((m["value"] for m in result["metrics"] 
                                  if m["metric_name"] == "time_series_average_firing_rate"), 0)
            
            insights["total_spikes"] = total_spikes
            insights["average_firing_rate"] = avg_firing_rate
            
            # Learning analysis
            if hasattr(self, 'learning_analysis'):
                analysis = self.learning_analysis
                
                insights["learning_quality"] = analysis.get("learning_quality", "Unknown")
                insights["overall_efficiency_score"] = analysis.get("overall_efficiency_score", 0)
                insights["average_learning_rate"] = analysis.get("average_learning_rate", 0)
                insights["convergence_rate"] = analysis.get("converged_trackers", 0) / analysis.get("total_trackers", 1)
                
                # Learning recommendations
                if "excellent" in insights["learning_quality"].lower():
                    insights["learning_efficiency"] = "High efficiency with fast convergence"
                elif "good" in insights["learning_quality"].lower():
                    insights["learning_efficiency"] = "Good efficiency with acceptable convergence"
                elif "fair" in insights["learning_quality"].lower():
                    insights["learning_efficiency"] = "Moderate efficiency - room for improvement"
                else:
                    insights["learning_efficiency"] = "Low efficiency - significant improvements needed"
            
            insights["recommendations"] = self._generate_learning_recommendations(insights)
            
        except Exception as e:
            self.logger.error(f"Error generating learning insights: {e}")
            insights["error"] = f"Failed to generate insights: {e}"
        
        return insights
    
    def _generate_learning_recommendations(self, insights: Dict[str, Any]) -> List[str]:
        """Generate recommendations based on learning analysis."""
        recommendations = []
        
        if "learning_efficiency" in insights:
            if "high" in insights["learning_efficiency"].lower():
                recommendations.append("Maintain current learning configuration")
                recommendations.append("Monitor for overlearning or saturation")
            elif "moderate" in insights["learning_efficiency"].lower():
                recommendations.append("Optimize learning parameters for better convergence")
                recommendations.append("Consider adjusting learning rates and thresholds")
            else:
                recommendations.append("Implement learning acceleration techniques")
                recommendations.append("Review and adjust learning parameters")
        
        if "convergence_rate" in insights:
            convergence_rate = insights["convergence_rate"]
            if convergence_rate > 0.8:
                recommendations.append("Fast convergence achieved - consider increasing training complexity")
            elif convergence_rate > 0.5:
                recommendations.append("Good convergence - maintain current approach")
            elif convergence_rate > 0.2:
                recommendations.append("Slow convergence - increase training duration or adjust parameters")
            else:
                recommendations.append("Very slow convergence - significant parameter tuning required")
        
        if "average_learning_rate" in insights:
            learning_rate = insights["average_learning_rate"]
            if learning_rate < 0.01:
                recommendations.append("Increase learning rate or adjust learning dynamics")
            elif learning_rate < 0.1:
                recommendations.append("Current learning rate is appropriate")
            else:
                recommendations.append("Learning rate is high - monitor for instability")
        
        return recommendations

    def export_results(self, format: OutputFormat, filepath: Optional[str] = None) -> str:
        """Export learning performance analysis results."""
        if not self.results:
            return "No results to export"
        
        # Add learning insights to results
        if isinstance(self.results, dict):
            insights = self.generate_learning_insights(self.results)
            self.results["learning_insights"] = insights
        
        return super().export_results(format, filepath)


class LearningPerformanceMetricsCLI:
    """CLI interface for learning performance metrics."""
    
    def __init__(self):
        self.logger = logging.getLogger("nlm_profiling.learning.cli")
    
    def run_learning_analysis(self, args) -> int:
        """Run learning performance analysis."""
        try:
            # Create learning analysis config
            config = LearningPerformanceMetricsConfig(
                steps=args.steps,
                analysis_mode=args.analysis_mode
            )
            
            # Create learning analyzer
            analyzer = LearningPerformanceMetricsModule(config)
            
            # Run analysis
            result = analyzer.run_profiling(steps=args.steps)
            
            # Output results
            if args.json:
                output = analyzer.export_results(OutputFormat.JSON)
                print(output)
            else:
                print(f"\n=== Learning Performance Metrics Results ===")
                print(f"Analysis completed in {result['duration']:.2f}s")
                
                # Print learning insights
                insights = analyzer.generate_learning_insights(result)
                print(f"\nLearning Insights:")
                for key, value in insights.items():
                    if key != "recommendations":
                        print(f"  {key}: {value}")
                
                print(f"\nRecommendations:")
                for recommendation in insights.get("recommendations", []):
                    print(f"  - {recommendation}")
                
                # Print key metrics
                print(f"\nKey Metrics:")
                for metric in result.get("metrics", []):
                    print(f"  {metric['metric_name']}: {metric['value']} {metric.get('unit', '')}")
            
            return 0
            
        except Exception as e:
            self.logger.error(f"Learning analysis failed: {e}")
            return 1


if __name__ == "__main__":
    import argparse
    import sys
    
    parser = argparse.ArgumentParser(
        description="Learning Performance Metrics for NLM Brain Learning",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python -m nlm_profiling.learning --steps 100
  python -m nlm_profiling.learning --steps 500 --analysis-mode convergence
  python -m nlm_profiling.learning --json --analysis-mode efficiency
        """
    )
    
    parser.add_argument("--steps", type=int, default=100, help="Number of steps to analyze")
    parser.add_argument("--analysis-mode", type=str, choices=["convergence", "efficiency", "adaptation", "stability", "optimization"],
                      default="convergence", help="Analysis mode to use")
    parser.add_argument("--json", action="store_true", help="Output results as JSON")
    parser.add_argument("--verbose", action="store_true", help="Enable verbose output")
    
    args = parser.parse_args()
    
    # Setup logging
    level = logging.DEBUG if args.verbose else logging.INFO
    logging.basicConfig(level=level)
    
    # Create CLI handler and run analysis
    cli = LearningPerformanceMetricsCLI()
    sys.exit(cli.run_learning_analysis(args))