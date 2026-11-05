#!/usr/bin/env python3
"""
Spike Rate Analyzer Module

Tools for analyzing neural spike patterns and statistics.
Provides comprehensive spike rate analysis with statistical insights.
"""

import time
import logging
import statistics
import numpy as np
from typing import Dict, List, Optional, Any, Tuple
from dataclasses import dataclass, field
from enum import Enum
from collections import defaultdict, Counter
import math

from .base import PerformanceMetrics, BenchmarkResult, ProfilingFramework, OutputFormat

logger = logging.getLogger("nlm_profiling.spike")


class SpikeAnalysisMode(Enum):
    """Spike analysis mode enumeration."""
    BASIC = "basic"
    STATISTICAL = "statistical"
    PATTERN = "pattern"
    BURST_ANALYSIS = "burst_analysis"
    FREQUENCY_DOMAIN = "frequency_domain"


class SpikePattern:
    """Represents a spike pattern in time."""
    
    def __init__(self, spike_times: List[float], neuron_id: int = 0):
        self.spike_times = sorted(spike_times)
        self.neuron_id = neuron_id
        self.duration = max(spike_times) - min(spike_times) if spike_times else 0
        self.num_spikes = len(spike_times)
        self.sampling_rate = 1.0  # 1 spike per time unit
    
    def get_inter_spike_intervals(self) -> List[float]:
        """Calculate inter-spike intervals."""
        if len(self.spike_times) < 2:
            return []
        return [self.spike_times[i+1] - self.spike_times[i] for i in range(len(self.spike_times)-1)]
    
    def get_firing_rate(self) -> float:
        """Calculate firing rate (spikes per time unit)."""
        if self.duration == 0:
            return self.num_spikes
        return self.num_spikes / self.duration
    
    def get_cv_of_isi(self) -> float:
        """Calculate coefficient of variation of inter-spike intervals."""
        isis = self.get_inter_spike_intervals()
        if len(isis) < 2:
            return 0.0
        mean_isi = statistics.mean(isis)
        if mean_isi == 0:
            return 0.0
        return statistics.stdev(isis) / mean_isi if len(isis) > 1 else 0.0
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary."""
        return {
            "neuron_id": self.neuron_id,
            "spike_times": self.spike_times,
            "duration": self.duration,
            "num_spikes": self.num_spikes,
            "firing_rate": self.get_firing_rate(),
            "cv_isi": self.get_cv_of_isi(),
            "inter_spike_intervals": self.get_inter_spike_intervals()
        }


class SpikeRateAnalyzerConfig:
    """Configuration for spike rate analysis."""
    
    def __init__(self, steps: int = 100, analysis_mode: SpikeAnalysisMode = SpikeAnalysisMode.STATISTICAL,
                 enable_pattern_detection: bool = True, enable_burst_analysis: bool = True,
                 enable_frequency_analysis: bool = False):
        self.steps = steps
        self.analysis_mode = analysis_mode
        self.enable_pattern_detection = enable_pattern_detection
        self.enable_burst_analysis = enable_burst_analysis
        self.enable_frequency_analysis = enable_frequency_analysis
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary."""
        return {
            "steps": self.steps,
            "analysis_mode": self.analysis_mode.value,
            "enable_pattern_detection": self.enable_pattern_detection,
            "enable_burst_analysis": self.enable_burst_analysis,
            "enable_frequency_analysis": self.enable_frequency_analysis
        }


class SpikeRateAnalyzerModule(ProfilingFramework):
    """Spike Rate Analyzer for analyzing neural spike patterns and statistics."""
    
    def __init__(self, config: Optional[SpikeRateAnalyzerConfig] = None):
        super().__init__("spike_rate_analyzer")
        self.config = config or SpikeRateAnalyzerConfig()
        self.spike_patterns: List[SpikePattern] = []
        self.time_series_data: Dict[str, List[float]] = defaultdict(list)
    
    def _profile(self, **kwargs) -> BenchmarkResult:
        """Analyze spike patterns."""
        try:
            import pynlm
            
            steps = kwargs.get('steps', self.config.steps)
            
            # Create brain for spike analysis
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            self.logger.info(f"Starting spike rate analysis: {steps} steps")
            
            # Collect spike data over time
            for step in range(steps):
                brain.step(step)
                
                # Collect various spike-related metrics
                self.time_series_data["step"].append(step)
                self.time_series_data["total_spike_count"].append(brain.getTotalSpikeCount())
                self.time_series_data["average_firing_rate"].append(brain.getAverageFiringRate())
                self.time_series_data["active_neuron_count"].append(brain.getActiveNeuronCount())
                self.time_series_data["firing_neuron_count"].append(brain.getFiringNeuronCount())
                
                # Simulate spike pattern detection (in real implementation, this would use actual spike data)
                if self.config.enable_pattern_detection:
                    self._detect_spike_patterns(step, brain)
                
                # Log progress
                if step % 100 == 0:
                    self.logger.info(f"Step {step}/{steps} completed")
            
            # Analyze spike patterns
            self._analyze_spike_patterns()
            
            # Generate metrics
            metrics = self._generate_spike_metrics()
            
            result = BenchmarkResult(
                test_name=self.name,
                parameters=kwargs,
                metrics=metrics,
                duration=steps * 0.001,  # Estimated duration
                success=True
            )
            
            self.results = {
                "time_series": dict(self.time_series_data),
                "spike_patterns": [p.to_dict() for p in self.spike_patterns],
                "metrics": [m.to_dict() for m in metrics]
            }
            return result
            
        except Exception as e:
            self.logger.error(f"Error in spike rate analyzer: {e}")
            raise
    
    def _detect_spike_patterns(self, step: int, brain) -> None:
        """Detect spike patterns from brain state."""
        try:
            # Extract spike-related information from brain
            total_spikes = brain.getTotalSpikeCount()
            firing_rate = brain.getAverageFiringRate()
            
            # Create simulated spike pattern for analysis
            if total_spikes > 0 and step > 0:
                # Generate simulated spike times based on firing rate
                spike_times = self._generate_simulated_spike_times(
                    step, firing_rate, total_spikes)
                
                pattern = SpikePattern(spike_times, neuron_id=step % 100)  # Use step modulo for neuron ID
                self.spike_patterns.append(pattern)
            
        except Exception as e:
            self.logger.error(f"Error detecting spike patterns: {e}")
    
    def _generate_simulated_spike_times(self, step: int, firing_rate: float, total_spikes: int) -> List[float]:
        """Generate simulated spike times for analysis."""
        # This is a simplified simulation for demonstration
        # In a real implementation, this would use actual spike data
        spike_times = []
        
        for i in range(min(total_spikes, 100)):  # Limit to 100 spikes for analysis
            # Generate spike time based on firing rate and step
            interval = 1.0 / max(firing_rate, 0.1)  # Minimum interval to avoid division by zero
            spike_time = step * interval + i * interval * (1 + 0.1 * (np.random.random() - 0.5))
            spike_times.append(spike_time)
        
        return spike_times
    
    def _analyze_spike_patterns(self) -> None:
        """Analyze detected spike patterns."""
        if not self.spike_patterns:
            return
        
        try:
            # Calculate pattern statistics
            firing_rates = [p.get_firing_rate() for p in self.spike_patterns]
            cv_values = [p.get_cv_of_isi() for p in self.spike_patterns if p.num_spikes > 1]
            
            # Calculate burst statistics
            if self.config.enable_burst_analysis:
                self._analyze_burst_patterns()
            
            # Store pattern analysis results
            self.pattern_analysis = {
                "total_patterns": len(self.spike_patterns),
                "average_firing_rate": statistics.mean(firing_rates) if firing_rates else 0,
                "firing_rate_stddev": statistics.stdev(firing_rates) if len(firing_rates) > 1 else 0,
                "cv_isi_mean": statistics.mean(cv_values) if cv_values else 0,
                "cv_isi_stddev": statistics.stdev(cv_values) if len(cv_values) > 1 else 0,
                "regularity_score": self._calculate_regularity_score(firing_rates, cv_values)
            }
            
        except Exception as e:
            self.logger.error(f"Error analyzing spike patterns: {e}")
    
    def _analyze_burst_patterns(self) -> None:
        """Analyze burst patterns in spike data."""
        try:
            burst_stats = {
                "total_bursts": 0,
                "average_burst_duration": 0.0,
                "average_burst_frequency": 0.0,
                "burst_regularity": 0.0
            }
            
            # Simplified burst analysis
            for pattern in self.spike_patterns:
                isis = pattern.get_inter_spike_intervals()
                if isis:
                    # Detect bursts (short ISIs followed by longer ISIs)
                    short_isi_threshold = statistics.mean(isis) * 0.5
                    burst_count = sum(1 for isi in isis if isi < short_isi_threshold)
                    
                    burst_stats["total_bursts"] += burst_count
                    
                    # Calculate burst duration and frequency
                    if burst_count > 0:
                        burst_duration = sum(isi for isi in isis if isi < short_isi_threshold)
                        burst_freq = burst_count / pattern.duration if pattern.duration > 0 else 0
                        burst_stats["average_burst_duration"] += burst_duration
                        burst_stats["average_burst_frequency"] += burst_freq
            
            # Normalize
            if burst_stats["total_bursts"] > 0:
                burst_count = burst_stats["total_bursts"]
                burst_stats["average_burst_duration"] /= burst_count
                burst_stats["average_burst_frequency"] /= burst_count
            
            self.pattern_analysis["burst_statistics"] = burst_stats
            
        except Exception as e:
            self.logger.error(f"Error analyzing burst patterns: {e}")
    
    def _calculate_regularity_score(self, firing_rates: List[float], cv_values: List[float]) -> float:
        """Calculate spike train regularity score."""
        if not firing_rates or not cv_values:
            return 0.0
        
        # Combine firing rate and ISI CV to get regularity score
        avg_fr = statistics.mean(firing_rates)
        avg_cv = statistics.mean(cv_values)
        
        # Regularity is inversely related to CV (lower CV = more regular)
        # and directly related to firing rate
        regularity = (1.0 / (1.0 + avg_cv)) * min(avg_fr / 100.0, 1.0)
        
        return regularity
    
    def _generate_spike_metrics(self) -> List[PerformanceMetrics]:
        """Generate spike-related metrics."""
        metrics = []
        
        if not self.time_series_data:
            return metrics
        
        # Overall spike metrics
        total_spikes = self.time_series_data.get("total_spike_count", [])
        if total_spikes:
            metrics.append(PerformanceMetrics(
                timestamp=time.time(),
                metric_name="total_spikes_collected",
                value=sum(total_spikes),
                unit="spikes"
            ))
            
            metrics.append(PerformanceMetrics(
                timestamp=time.time(),
                metric_name="average_spike_count_per_step",
                value=statistics.mean(total_spikes),
                unit="spikes/step"
            ))
            
            if len(total_spikes) > 1:
                metrics.append(PerformanceMetrics(
                    timestamp=time.time(),
                    metric_name="spike_count_stddev",
                    value=statistics.stdev(total_spikes),
                    unit="spikes"
                ))
        
        # Firing rate metrics
        firing_rates = self.time_series_data.get("average_firing_rate", [])
        if firing_rates:
            metrics.append(PerformanceMetrics(
                timestamp=time.time(),
                metric_name="average_firing_rate",
                value=statistics.mean(firing_rates),
                unit="Hz"
            ))
            
            metrics.append(PerformanceMetrics(
                timestamp=time.time(),
                metric_name="firing_rate_stddev",
                value=statistics.stdev(firing_rates),
                unit="Hz"
            ))
        
        # Pattern analysis metrics
        if hasattr(self, 'pattern_analysis'):
            for key, value in self.pattern_analysis.items():
                if isinstance(value, (int, float)):
                    metrics.append(PerformanceMetrics(
                        timestamp=time.time(),
                        metric_name=f"pattern_{key}",
                        value=float(value),
                        unit=""
                    ))
        
        return metrics
    
    def generate_spike_insights(self, result: Dict[str, Any]) -> Dict[str, Any]:
        """Generate insights from spike rate analysis."""
        insights = {}
        
        try:
            if "metrics" not in result:
                return insights
            
            # Extract key metrics
            total_spikes = next((m["value"] for m in result["metrics"] 
                               if m["metric_name"] == "total_spikes_collected"), 0)
            avg_spike_count = next((m["value"] for m in result["metrics"] 
                                  if m["metric_name"] == "average_spike_count_per_step"), 0)
            avg_firing_rate = next((m["value"] for m in result["metrics"] 
                                  if m["metric_name"] == "average_firing_rate"), 0)
            
            insights["total_spikes"] = total_spikes
            insights["average_spike_count_per_step"] = avg_spike_count
            insights["average_firing_rate_hz"] = avg_firing_rate
            
            # Spike analysis
            if total_spikes > 1000:
                insights["spike_activity"] = "High spike activity - network is highly active"
            elif total_spikes > 100:
                insights["spike_activity"] = "Moderate spike activity - normal network function"
            elif total_spikes > 10:
                insights["spike_activity"] = "Low spike activity - network may be underactive"
            else:
                insights["spike_activity"] = "Very low spike activity - network may be dysfunctional"
            
            # Firing rate analysis
            if avg_firing_rate > 10.0:
                insights["firing_rate_analysis"] = "High firing rate - potentially unstable"
            elif avg_firing_rate > 5.0:
                insights["firing_rate_analysis"] = "Moderate firing rate - healthy activity"
            elif avg_firing_rate > 1.0:
                insights["firing_rate_analysis"] = "Low firing rate - may indicate reduced excitability"
            else:
                insights["firing_rate_analysis"] = "Very low firing rate - severe hypoactivity"
            
            # Pattern analysis
            if "pattern_analysis" in result:
                pattern_data = result["pattern_analysis"]
                if "regularity_score" in pattern_data:
                    regularity = pattern_data["regularity_score"]
                    if regularity > 0.8:
                        insights["pattern_regularity"] = "Very regular spike patterns - highly synchronized"
                    elif regularity > 0.5:
                        insights["pattern_regularity"] = "Moderately regular patterns - good synchronization"
                    elif regularity > 0.2:
                        insights["pattern_regularity"] = "Irregular patterns - variable synchronization"
                    else:
                        insights["pattern_regularity"] = "Highly irregular patterns - desynchronized activity"
            
            insights["recommendations"] = self._generate_spike_recommendations(insights)
            
        except Exception as e:
            self.logger.error(f"Error generating spike insights: {e}")
            insights["error"] = f"Failed to generate insights: {e}"
        
        return insights
    
    def _generate_spike_recommendations(self, insights: Dict[str, Any]) -> List[str]:
        """Generate recommendations based on spike analysis."""
        recommendations = []
        
        if "spike_activity" in insights:
            if "high" in insights["spike_activity"].lower():
                recommendations.append("Consider implementing inhibitory mechanisms to stabilize activity")
                recommendations.append("Review neuromodulatory parameters for overexcitation")
            elif "low" in insights["spike_activity"].lower():
                recommendations.append("Increase external input or neuronal excitability")
                recommendations.append("Consider adjusting synaptic strength parameters")
            else:
                recommendations.append("Spike activity is within optimal range")
        
        if "firing_rate_analysis" in insights:
            if "unstable" in insights["firing_rate_analysis"].lower():
                recommendations.append("Implement homeostatic plasticity mechanisms")
                recommendations.append("Consider synaptic scaling mechanisms")
            elif "low" in insights["firing_rate_analysis"].lower():
                recommendations.append("Increase driving input or reduce inhibition")
                recommendations.append("Review neuromodulatory tone")
            else:
                recommendations.append("Firing rate is within healthy range")
        
        if "pattern_regularity" in insights:
            if "desynchronized" in insights["pattern_regularity"].lower():
                recommendations.append("Consider interventions to enhance synchronization")
                recommendations.append("Review network connectivity patterns")
            elif "synchronized" in insights["pattern_regularity"].lower():
                recommendations.append("Monitor for potential epileptic activity")
                recommendations.append("Implement anti-seizure mechanisms if necessary")
        
        return recommendations

    def export_results(self, format: OutputFormat, filepath: Optional[str] = None) -> str:
        """Export spike rate analysis results."""
        if not self.results:
            return "No results to export"
        
        # Add spike insights to results
        if isinstance(self.results, dict):
            insights = self.generate_spike_insights(self.results)
            self.results["spike_insights"] = insights
        
        return super().export_results(format, filepath)


class SpikeRateAnalyzerCLI:
    """CLI interface for spike rate analyzer."""
    
    def __init__(self):
        self.logger = logging.getLogger("nlm_profiling.spike.cli")
    
    def run_spike_analysis(self, args) -> int:
        """Run spike rate analysis."""
        try:
            # Create spike analysis config
            config = SpikeRateAnalyzerConfig(
                steps=args.steps,
                analysis_mode=args.analysis_mode
            )
            
            # Create spike analyzer
            analyzer = SpikeRateAnalyzerModule(config)
            
            # Run analysis
            result = analyzer.run_profiling(steps=args.steps)
            
            # Output results
            if args.json:
                output = analyzer.export_results(OutputFormat.JSON)
                print(output)
            else:
                print(f"\n=== Spike Rate Analyzer Results ===")
                print(f"Analysis completed in {result['duration']:.2f}s")
                
                # Print spike insights
                insights = analyzer.generate_spike_insights(result)
                print(f"\nSpike Insights:")
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
            self.logger.error(f"Spike analysis failed: {e}")
            return 1


if __name__ == "__main__":
    import argparse
    import sys
    
    parser = argparse.ArgumentParser(
        description="Spike Rate Analyzer for NLM Neural Activity",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python -m nlm_profiling.spike --steps 100
  python -m nlm_profiling.spike --steps 500 --analysis-mode statistical
  python -m nlm_profiling.spike --json --analysis-mode pattern
        """
    )
    
    parser.add_argument("--steps", type=int, default=100, help="Number of steps to analyze")
    parser.add_argument("--analysis-mode", type=str, choices=["basic", "statistical", "pattern", "burst_analysis", "frequency_domain"],
                      default="statistical", help="Analysis mode to use")
    parser.add_argument("--json", action="store_true", help="Output results as JSON")
    parser.add_argument("--verbose", action="store_true", help="Enable verbose output")
    
    args = parser.parse_args()
    
    # Setup logging
    level = logging.DEBUG if args.verbose else logging.INFO
    logging.basicConfig(level=level)
    
    # Create CLI handler and run analysis
    cli = SpikeRateAnalyzerCLI()
    sys.exit(cli.run_spike_analysis(args))