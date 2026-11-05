#!/usr/bin/env python3
"""
Performance Measurement Framework Module

Core utilities for measuring and reporting brain performance metrics.
Provides detailed performance measurements with multiple output formats.
"""

import time
import logging
from typing import Dict, List, Optional, Any
from dataclasses import dataclass

from .base import PerformanceMetrics, BenchmarkResult, ProfilingFramework, OutputFormat

logger = logging.getLogger("nlm_profiling.performance_measurement")


class PerformanceMeasurementConfig:
    """Configuration for performance measurement."""
    
    def __init__(self, steps: int = 100, metric_interval: int = 1, enable_continuity: bool = True):
        self.steps = steps
        self.metric_interval = metric_interval
        self.enable_continuity = enable_continuity
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary."""
        return {
            "steps": self.steps,
            "metric_interval": self.metric_interval,
            "enable_continuity": self.enable_continuity
        }


class PerformanceMeasurementModule(ProfilingFramework):
    """Performance Measurement Framework for measuring brain performance metrics."""
    
    def __init__(self, config: Optional[PerformanceMeasurementConfig] = None):
        super().__init__("performance_measurement")
        self.config = config or PerformanceMeasurementConfig()
    
    def _profile(self, **kwargs) -> BenchmarkResult:
        """Profile brain performance metrics."""
        try:
            import pynlm
            
            # Get optional brain parameter
            brain = kwargs.get('brain')
            steps = self.config.steps
            
            if not brain:
                # Create a default brain if none provided
                config = pynlm.createDefaultConfig()
                brain = pynlm.createBrain(config)
                brain.initialize()
                self.logger.info("Created default brain for performance measurement")
            
            metrics = []
            total_duration = 0.0
            
            # Profile over specified steps
            for step in range(steps):
                step_start = time.time()
                
                # Collect performance metrics
                metrics_data = PerformanceMetrics(
                    timestamp=time.time(),
                    metric_name="total_spike_count",
                    value=float(brain.getTotalSpikeCount()),
                    unit="spikes"
                )
                metrics.append(metrics_data)
                
                metrics_data = PerformanceMetrics(
                    timestamp=time.time(),
                    metric_name="average_firing_rate",
                    value=brain.getAverageFiringRate(),
                    unit="Hz"
                )
                metrics.append(metrics_data)
                
                metrics_data = PerformanceMetrics(
                    timestamp=time.time(),
                    metric_name="total_neuron_count",
                    value=float(brain.getTotalNeuronCount()),
                    unit="neurons"
                )
                metrics.append(metrics_data)
                
                metrics_data = PerformanceMetrics(
                    timestamp=time.time(),
                    metric_name="total_synapse_count",
                    value=float(brain.getTotalSynapseCount()),
                    unit="synapses"
                )
                metrics.append(metrics_data)
                
                metrics_data = PerformanceMetrics(
                    timestamp=time.time(),
                    metric_name="active_neuron_count",
                    value=float(brain.getActiveNeuronCount()),
                    unit="neurons"
                )
                metrics.append(metrics_data)
                
                metrics_data = PerformanceMetrics(
                    timestamp=time.time(),
                    metric_name="firing_neuron_count",
                    value=float(brain.getFiringNeuronCount()),
                    unit="neurons"
                )
                metrics.append(metrics_data)
                
                metrics_data = PerformanceMetrics(
                    timestamp=time.time(),
                    metric_name="excitatory_inhibitory_ratio",
                    value=brain.getExcitationInhibitionRatio(),
                    unit="ratio"
                )
                metrics.append(metrics_data)
                
                # Perform brain step
                brain.step(step)
                
                step_duration = time.time() - step_start
                total_duration += step_duration
                
                # Log progress at intervals
                if step % 50 == 0 and step > 0:
                    self.logger.info(f"Step {step}/{steps} completed")
            
            # Calculate summary statistics
            spike_metrics = [m for m in metrics if m.metric_name == "total_spike_count"]
            firing_rate_metrics = [m for m in metrics if m.metric_name == "average_firing_rate"]
            
            # Add performance summary metrics
            if spike_metrics:
                final_spikes = spike_metrics[-1].value
                initial_spikes = spike_metrics[0].value
                metrics.append(PerformanceMetrics(
                    timestamp=time.time(),
                    metric_name="spike_count_growth",
                    value=final_spikes - initial_spikes,
                    unit="spikes"
                ))
            
            if firing_rate_metrics:
                avg_firing_rate = sum(m.value for m in firing_rate_metrics) / len(firing_rate_metrics)
                metrics.append(PerformanceMetrics(
                    timestamp=time.time(),
                    metric_name="average_firing_rate_over_time",
                    value=avg_firing_rate,
                    unit="Hz"
                ))
            
            result = BenchmarkResult(
                test_name=self.name,
                parameters=kwargs,
                metrics=metrics,
                duration=total_duration,
                success=True
            )
            
            self.results = result
            return result
            
        except Exception as e:
            self.logger.error(f"Error in performance measurement: {e}")
            raise
    
    def generate_performance_insights(self, result: BenchmarkResult) -> Dict[str, Any]:
        """Generate actionable performance insights from results."""
        insights = {}
        
        try:
            # Extract key metrics from results
            spike_metrics = [m for m in result.metrics if m.metric_name == "total_spike_count"]
            firing_rate_metrics = [m for m in result.metrics if m.metric_name == "average_firing_rate"]
            duration_metrics = [m for m in result.metrics if m.metric_name == "spike_count_growth"]
            
            if spike_metrics:
                final_spikes = spike_metrics[-1].value
                initial_spikes = spike_metrics[0].value
                spike_growth_rate = (final_spikes - initial_spikes) / result.duration if result.duration > 0 else 0
                insights["spike_growth_rate"] = spike_growth_rate
                
                if spike_growth_rate < 0.1:
                    insights["spike_analysis"] = "Low spike growth detected - consider increasing neural excitability or external input"
                elif spike_growth_rate > 1.0:
                    insights["spike_analysis"] = "High spike growth rate - potential instability detected"
                else:
                    insights["spike_analysis"] = "Normal spike growth rate detected"
            
            if firing_rate_metrics:
                avg_firing_rate = sum(m.value for m in firing_rate_metrics) / len(firing_rate_metrics)
                insights["average_firing_rate"] = avg_firing_rate
                
                if avg_firing_rate < 1.0:
                    insights["firing_rate_analysis"] = "Low average firing rate - network may be underactive"
                elif avg_firing_rate > 20.0:
                    insights["firing_rate_analysis"] = "High average firing rate - network may be overactive"
                else:
                    insights["firing_rate_analysis"] = "Normal firing rate range detected"
            
            if duration_metrics:
                growth_efficiency = duration_metrics[0].value / result.duration if result.duration > 0 else 0
                insights["growth_efficiency"] = growth_efficiency
                insights["performance_score"] = min(growth_efficiency * 10, 10.0)
            
            insights["recommendations"] = self._generate_recommendations(insights)
            
        except Exception as e:
            self.logger.error(f"Error generating performance insights: {e}")
            insights["error"] = f"Failed to generate insights: {e}"
        
        return insights
    
    def _generate_recommendations(self, insights: Dict[str, Any]) -> List[str]:nlm_profiling/performance_measurement.py
"""Performance measurement module implementation."""

import time
import logging
from typing import Dict, List, Optional, Any
from dataclasses import dataclass

from .base import PerformanceMetrics, BenchmarkResult, ProfilingFramework, OutputFormat

logger = logging.getLogger("nlm_profiling.performance_measurement")


class PerformanceMeasurementConfig:
    """Configuration for performance measurement."""
    
    def __init__(self, steps: int = 100, metric_interval: int = 1, enable_continuity: bool = True):
        self.steps = steps
        self.metric_interval = metric_interval
        self.enable_continuity = enable_continuity
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary."""
        return {
            "steps": self.steps,
            "metric_interval": self.metric_interval,
            "enable_continuity": self.enable_continuity
        }


class PerformanceMeasurementModule(ProfilingFramework):
    """Performance Measurement Framework for measuring brain performance metrics."""
    
    def __init__(self, config: Optional[PerformanceMeasurementConfig] = None):
        super().__init__("performance_measurement")
        self.config = config or PerformanceMeasurementConfig()
    
    def _profile(self, **kwargs) -> BenchmarkResult:
        """Profile brain performance metrics."""
        try:
            import pynlm
            
            # Get optional brain parameter
            brain = kwargs.get('brain')
            steps = self.config.steps
            
            if not brain:
                # Create a default brain if none provided
                config = pynlm.createDefaultConfig()
                brain = pynlm.createBrain(config)
                brain.initialize()
                self.logger.info("Created default brain for performance measurement")
            
            metrics = []
            total_duration = 0.0
            
            # Profile over specified steps
            for step in range(steps):
                step_start = time.time()
                
                # Collect performance metrics
                metrics_data = PerformanceMetrics(
                    timestamp=time.time(),
                    metric_name="total_spike_count",
                    value=float(brain.getTotalSpikeCount()),
                    unit="spikes"
                )
                metrics.append(metrics_data)
                
                metrics_data = PerformanceMetrics(
                    timestamp=time.time(),
                    metric_name="average_firing_rate",
                    value=brain.getAverageFiringRate(),
                    unit="Hz"
                )
                metrics.append(metrics_data)
                
                metrics_data = PerformanceMetrics(
                    timestamp=time.time(),
                    metric_name="total_neuron_count",
                    value=float(brain.getTotalNeuronCount()),
                    unit="neurons"
                )
                metrics.append(metrics_data)
                
                metrics_data = PerformanceMetrics(
                    timestamp=time.time(),
                    metric_name="total_synapse_count",
                    value=float(brain.getTotalSynapseCount()),
                    unit="synapses"
                )
                metrics.append(metrics_data)
                
                metrics_data = PerformanceMetrics(
                    timestamp=time.time(),
                    metric_name="active_neuron_count",
                    value=float(brain.getActiveNeuronCount()),
                    unit="neurons"
                )
                metrics.append(metrics_data)
                
                metrics_data = PerformanceMetrics(
                    timestamp=time.time(),
                    metric_name="firing_neuron_count",
                    value=float(brain.getFiringNeuronCount()),
                    unit="neurons"
                )
                metrics.append(metrics_data)
                
                metrics_data = PerformanceMetrics(
                    timestamp=time.time(),
                    metric_name="excitatory_inhibitory_ratio",
                    value=brain.getExcitationInhibitionRatio(),
                    unit="ratio"
                )
                metrics.append(metrics_data)
                
                # Perform brain step
                brain.step(step)
                
                step_duration = time.time() - step_start
                total_duration += step_duration
                
                # Log progress at intervals
                if step % 50 == 0 and step > 0:
                    self.logger.info(f"Step {step}/{steps} completed")
            
            # Calculate summary statistics
            spike_metrics = [m for m in metrics if m.metric_name == "total_spike_count"]
            firing_rate_metrics = [m for m in metrics if m.metric_name == "average_firing_rate"]
            
            # Add performance summary metrics
            if spike_metrics:
                final_spikes = spike_metrics[-1].value
                initial_spikes = spike_metrics[0].value
                metrics.append(PerformanceMetrics(
                    timestamp=time.time(),
                    metric_name="spike_count_growth",
                    value=final_spikes - initial_spikes,
                    unit="spikes"
                ))
            
            if firing_rate_metrics:
                avg_firing_rate = sum(m.value for m in firing_rate_metrics) / len(firing_rate_metrics)
                metrics.append(PerformanceMetrics(
                    timestamp=time.time(),
                    metric_name="average_firing_rate_over_time",
                    value=avg_firing_rate,
                    unit="Hz"
                ))
            
            result = BenchmarkResult(
                test_name=self.name,
                parameters=kwargs,
                metrics=metrics,
                duration=total_duration,
                success=True
            )
            
            self.results = result
            return result
            
        except Exception as e:
            self.logger.error(f"Error in performance measurement: {e}")
            raise
    
    def generate_performance_insights(self, result: BenchmarkResult) -> Dict[str, Any]:
        """Generate actionable performance insights from results."""
        insights = {}
        
        try:
            # Extract key metrics from results
            spike_metrics = [m for m in result.metrics if m.metric_name == "total_spike_count"]
            firing_rate_metrics = [m for m in result.metrics if m.metric_name == "average_firing_rate"]
            duration_metrics = [m for m in result.metrics if m.metric_name == "spike_count_growth"]
            
            if spike_metrics:
                final_spikes = spike_metrics[-1].value
                initial_spikes = spike_metrics[0].value
                spike_growth_rate = (final_spikes - initial_spikes) / result.duration if result.duration > 0 else 0
                insights["spike_growth_rate"] = spike_growth_rate
                
                if spike_growth_rate < 0.1:
                    insights["spike_analysis"] = "Low spike growth detected - consider increasing neural excitability or external input"
                elif spike_growth_rate > 1.0:
                    insights["spike_analysis"] = "High spike growth rate - potential instability detected"
                else:
                    insights["spike_analysis"] = "Normal spike growth rate detected"
            
            if firing_rate_metrics:
                avg_firing_rate = sum(m.value for m in firing_rate_metrics) / len(firing_rate_metrics)
                insights["average_firing_rate"] = avg_firing_rate
                
                if avg_firing_rate < 1.0:
                    insights["firing_rate_analysis"] = "Low average firing rate - network may be underactive"
                elif avg_firing_rate > 20.0:
                    insights["firing_rate_analysis"] = "High average firing rate - network may be overactive"
                else:
                    insights["firing_rate_analysis"] = "Normal firing rate range detected"
            
            if duration_metrics:
                growth_efficiency = duration_metrics[0].value / result.duration if result.duration > 0 else 0
                insights["growth_efficiency"] = growth_efficiency
                insights["performance_score"] = min(growth_efficiency * 10, 10.0)
            
            insights["recommendations"] = self._generate_recommendations(insights)
            
        except Exception as e:
            self.logger.error(f"Error generating performance insights: {e}")
            insights["error"] = f"Failed to generate insights: {e}"
        
        return insights
    
    def _generate_recommendations(self, insights: Dict[str, Any]) -> List[str]:
        """Generate actionable recommendations based on insights."""
        recommendations = []
        
        if "spike_analysis" in insights:
            if "instability" in insights["spike_analysis"].lower():
                recommendations.append("Consider adjusting synaptic weights to improve stability")
                recommendations.append("Implement homeostatic mechanisms to prevent runaway excitation")
            elif "low" in insights["spike_analysis"].lower():
                recommendations.append("Increase external input or neuronal excitability")
                recommendations.append("Consider adjusting threshold parameters")
            else:
                recommendations.append("Current spike dynamics appear healthy")
        
        if "firing_rate_analysis" in insights:
            if "underactive" in insights["firing_rate_analysis"].lower():
                recommendations.append("Increase driving input or reduce inhibition")
                recommendations.append("Consider adjusting neuromodulatory parameters")
            elif "overactive" in insights["firing_rate_analysis"].lower():
                recommendations.append("Increase inhibitory connectivity")
                recommendations.append("Implement spike frequency adaptation mechanisms")
            else:
                recommendations.append("Firing rate is within optimal range")
        
        if "performance_score" in insights:
            if insights["performance_score"] < 5.0:
                recommendations.append("Performance score is below optimal - consider neural architecture optimization")
            elif insights["performance_score"] < 7.0:
                recommendations.append("Performance is acceptable but could be improved")
            else:
                recommendations.append("Excellent performance - maintain current configuration")
        
        return recommendations