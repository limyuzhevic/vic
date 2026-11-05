#!/usr/bin/env python3
"""
Timeline Profiler Module

Detailed timeline analysis of brain operations for NLM simulations.
Provides comprehensive timeline analysis and operation profiling.
"""

import time
import logging
import statistics
import pandas as pd
import numpy as np
from typing import Dict, List, Optional, Any, Tuple
from dataclasses import dataclass, field
from enum import Enum
from collections import defaultdict
import json
from datetime import datetime

from .base import PerformanceMetrics, BenchmarkResult, ProfilingFramework, OutputFormat

logger = logging.getLogger("nlm_profiling.timeline")


class TimelineEvent:
    """Represents a timeline event in brain simulation."""
    
    def __init__(self, event_name: str, timestamp: float, step: int = 0,
                 duration: float = 0.0, metadata: Dict[str, Any] = None):
        self.event_name = event_name
        self.timestamp = timestamp
        self.step = step
        self.duration = duration
        self.metadata = metadata or {}
        self.event_id = f"{event_name}_{step}_{int(timestamp * 1000)}"
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary."""
        return {
            "event_id": self.event_id,
            "event_name": self.event_name,
            "timestamp": self.timestamp,
            "step": self.step,
            "duration": self.duration,
            "metadata": self.metadata
        }


class TimelineOperation:
    """Represents a sequence of related operations."""
    
    def __init__(self, operation_name: str, start_time: float, end_time: float = None):
        self.operation_name = operation_name
        self.start_time = start_time
        self.end_time = end_time
        self.duration = end_time - start_time if end_time else 0.0
        self.events: List[TimelineEvent] = []
        self.child_operations: List['TimelineOperation'] = []
    
    def add_event(self, event: TimelineEvent) -> None:
        """Add an event to this operation."""
        self.events.append(event)
    
    def add_child_operation(self, operation: 'TimelineOperation') -> None:
        """Add a child operation."""
        self.child_operations.append(operation)
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary."""
        return {
            "operation_name": self.operation_name,
            "start_time": self.start_time,
            "end_time": self.end_time,
            "duration": self.duration,
            "event_count": len(self.events),
            "child_operation_count": len(self.child_operations),
            "events": [e.to_dict() for e in self.events],
            "child_operations": [op.to_dict() for op in self.child_operations]
        }


class TimelineAnalysisConfig:
    """Configuration for timeline analysis."""
    
    def __init__(self, steps: int = 100, event_sampling_rate: float = 1.0,
                 enable_operation_profiling: bool = True, enable_step_analysis: bool = True,
                 analysis_depth: str = "detailed"):
        self.steps = steps
        self.event_sampling_rate = event_sampling_rate  # events per second
        self.enable_operation_profiling = enable_operation_profiling
        self.enable_step_analysis = enable_step_analysis
        self.analysis_depth = analysis_depth  # "detailed", "summary", "deep"
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary."""
        return {
            "steps": self.steps,
            "event_sampling_rate": self.event_sampling_rate,
            "enable_operation_profiling": self.enable_operation_profiling,
            "enable_step_analysis": self.enable_step_analysis,
            "analysis_depth": self.analysis_depth
        }


class TimelineProfilerModule(ProfilingFramework):
    """Timeline Profiler for detailed timeline analysis of brain operations."""
    
    def __init__(self, config: Optional[TimelineAnalysisConfig] = None):
        super().__init__("timeline_profiler")
        self.config = config or TimelineAnalysisConfig()
        self.timeline_events: List[TimelineEvent] = []
        self.timeline_operations: List[TimelineOperation] = []
        self.step_timeline: Dict[int, List[TimelineEvent]] = defaultdict(list)
        self.operation_durations: Dict[str, List[float]] = defaultdict(list)
    
    def _profile(self, **kwargs) -> BenchmarkResult:
        """Profile timeline operations."""
        try:
            import pynlm
            
            steps = kwargs.get('steps', self.config.steps)
            sampling_interval = 1.0 / self.config.event_sampling_rate if self.config.event_sampling_rate > 0 else 1.0
            
            # Create brain for timeline analysis
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            self.logger.info(f"Starting timeline profiling: {steps} steps, sampling interval: {sampling_interval}s")
            
            # Initialize timeline
            self._initialize_timeline()
            
            # Profile over specified steps
            for step in range(steps):
                step_start_time = time.time()
                
                # Record main operation
                main_operation = TimelineOperation("step_execution", step_start_time)
                
                # Collect brain metrics
                brain_metrics = self._collect_brain_metrics(brain, step)
                
                # Record key events
                self._record_key_events(step, brain, main_operation)
                
                # Perform brain step
                step_operation = TimelineOperation("brain_step", step_start_time)
                brain.step(step)
                step_end_time = time.time()
                
                step_operation.end_time = step_end_time
                main_operation.end_time = step_end_time
                
                # Add step operation to timeline
                self.timeline_operations.append(step_operation)
                
                # Store step-specific timeline
                self.step_timeline[step] = list(main_operation.events)
                
                # Record operation duration
                duration = step_operation.duration
                self.operation_durations["step_execution"].append(duration)
                
                # Record individual operation types
                for metric_name, value in brain_metrics.items():
                    if metric_name != "step":
                        self.operation_durations[metric_name].append(duration)
                
                # Log progress
                if step % 100 == 0:
                    self.logger.info(f"Step {step}/{steps} completed")
            
            # Finalize timeline
            self._finalize_timeline(steps)
            
            # Analyze timeline
            self._analyze_timeline(steps)
            
            # Generate metrics
            metrics = self._generate_timeline_metrics()
            
            result = BenchmarkResult(
                test_name=self.name,
                parameters=kwargs,
                metrics=metrics,
                duration=steps * 0.001,  # Estimated duration
                success=True
            )
            
            self.results = {
                "timeline_events": [e.to_dict() for e in self.timeline_events],
                "timeline_operations": [op.to_dict() for op in self.timeline_operations],
                "step_timeline": dict(self.step_timeline),
                "metrics": [m.to_dict() for m in metrics]
            }
            return result
            
        except Exception as e:
            self.logger.error(f"Error in timeline profiler: {e}")
            raise
    
    def _initialize_timeline(self) -> None:
        """Initialize timeline with global operations."""
        start_time = time.time()
        
        global_operations = [
            TimelineOperation("initialization", start_time),
            TimelineOperation("configuration_setup", start_time),
            TimelineOperation("memory_allocation", start_time)
        ]
        
        self.timeline_operations.extend(global_operations)
        
        # Add global events
        for operation in global_operations:
            operation.end_time = time.time()  # Simplified - end immediately
    
    def _collect_brain_metrics(self, brain, step: int) -> Dict[str, float]:
        """Collect brain metrics for timeline."""
        metrics = {
            "step": step,
            "timestamp": time.time()
        }
        
        # Collect various brain metrics
        metrics["neuron_count"] = brain.getTotalNeuronCount()
        metrics["synapse_count"] = brain.getTotalSynapseCount()
        metrics["spike_count"] = brain.getTotalSpikeCount()
        metrics["firing_rate"] = brain.getAverageFiringRate()
        metrics["active_neurons"] = brain.getActiveNeuronCount()
        metrics["firing_neurons"] = brain.getFiringNeuronCount()
        metrics["ei_ratio"] = brain.getExcitationInhibitionRatio()
        metrics["developmental_stage"] = brain.getDevelopmentalStage()
        
        return metrics
    
    def _record_key_events(self, step: int, brain, operation: TimelineOperation) -> None:
        """Record key events for the current step."""
        timestamp = time.time()
        
        # Record brain state events
        brain_metrics = self._collect_brain_metrics(brain, step)
        
        for metric_name, value in brain_metrics.items():
            if metric_name != "step" and metric_name != "timestamp":
                event = TimelineEvent(
                    f"metric_{metric_name}",
                    timestamp,
                    step,
                    0.0,  # Duration is 0 for metric events
                    {"value": value}
                )
                operation.add_event(event)
                self.timeline_events.append(event)
        
        # Record operation completion event
        completion_event = TimelineEvent(
            "step_completion",
            timestamp,
            step,
            0.0,
            {"operation": "brain_step"}
        )
        operation.add_event(completion_event)
        self.timeline_events.append(completion_event)
    
    def _finalize_timeline(self, total_steps: int) -> None:
        """Finalize timeline with summary operations."""
        end_time = time.time()
        
        # Create summary operation
        summary_operation = TimelineOperation("timeline_profiling_summary", start_time=0.0, end_time=end_time)
        
        # Add summary events
        summary_event = TimelineEvent(
            "profiling_complete",
            end_time,
            total_steps,
            0.0,
            {
                "total_steps": total_steps,
                "total_events": len(self.timeline_events),
                "total_operations": len(self.timeline_operations)
            }
        )
        summary_operation.add_event(summary_event)
        
        self.timeline_operations.append(summary_operation)
    
    def _analyze_timeline(self, total_steps: int) -> None:
        """Analyze timeline data."""
        try:
            timeline_analysis = {
                "total_events": len(self.timeline_events),
                "total_operations": len(self.timeline_operations),
                "total_steps": total_steps,
                "event_types": self._count_event_types(),
                "operation_durations": self._analyze_operation_durations(),
                "step_durations": self._analyze_step_durations(),
                "performance_metrics": self._calculate_performance_metrics()
            }
            
            # Calculate bottleneck operations
            bottleneck_operations = self._identify_bottleneck_operations()
            timeline_analysis["bottleneck_operations"] = bottleneck_operations
            
            # Calculate efficiency metrics
            efficiency_metrics = self._calculate_efficiency_metrics()
            timeline_analysis["efficiency_metrics"] = efficiency_metrics
            
            # Determine timeline quality
            quality_metrics = self._calculate_timeline_quality(timeline_analysis)
            timeline_analysis["timeline_quality"] = quality_metrics
            
            self.timeline_analysis = timeline_analysis
            
        except Exception as e:
            self.logger.error(f"Error analyzing timeline: {e}")
    
    def _count_event_types(self) -> Dict[str, int]:
        """Count events by type."""
        event_counts = defaultdict(int)
        for event in self.timeline_events:
            event_counts[event.event_name] += 1
        return dict(event_counts)
    
    def _analyze_operation_durations(self) -> Dict[str, Dict[str, float]]:
        """Analyze operation durations."""
        analysis = {}
        
        for operation_name, durations in self.operation_durations.items():
            if durations:
                analysis[operation_name] = {
                    "mean": statistics.mean(durations),
                    "median": statistics.median(durations),
                    "min": min(durations),
                    "max": max(durations),
                    "stddev": statistics.stdev(durations) if len(durations) > 1 else 0.0,
                    "count": len(durations)
                }
        
        return analysis
    
    def _analyze_step_durations(self) -> Dict[str, Any]:
        """Analyze step durations."""
        durations = self.operation_durations.get("step_execution", [])
        
        if not durations:
            return {}
        
        return {
            "mean": statistics.mean(durations),
            "median": statistics.median(durations),
            "min": min(durations),
            "max": max(durations),
            "stddev": statistics.stdev(durations) if len(durations) > 1 else 0.0,
            "cv": statistics.stdev(durations) / statistics.mean(durations) if statistics.mean(durations) > 0 else 0.0,
            "trend": self._calculate_step_trend(durations)
        }
    
    def _calculate_step_trend(self, durations: List[float]) -> str:
        """Calculate trend in step durations."""
        if len(durations) < 3:
            return "insufficient_data"
        
        # Simple linear regression
        steps = list(range(len(durations)))
        n = len(steps)
        
        x_mean = statistics.mean(steps)
        y_mean = statistics.mean(durations)
        
        numerator = sum((x - x_mean) * (y - y_mean) for x, y in zip(steps, durations))
        denominator = sum((x - x_mean) ** 2 for x in steps)
        
        if denominator == 0:
            return "stable"
        
        slope = numerator / denominator
        
        if abs(slope) < 0.001:
            return "stable"
        elif slope > 0.001:
            return "increasing"
        else:
            return "decreasing"
    
    def _calculate_performance_metrics(self) -> Dict[str, float]:
        """Calculate performance metrics from timeline."""
        metrics = {}
        
        step_durations = self.operation_durations.get("step_execution", [])
        if not step_durations:
            return metrics
        
        metrics["steps_per_second"] = 1.0 / statistics.mean(step_durations) if statistics.mean(step_durations) > 0 else 0
        metrics["total_simulation_time"] = sum(step_durations)
        metrics["time_consistency"] = 1.0 - (statistics.stdev(step_durations) / statistics.mean(step_durations) 
                                            if statistics.mean(step_durations) > 0 else 0)
        
        return metrics
    
    def _identify_bottleneck_operations(self) -> List[Dict[str, Any]]:
        """Identify bottleneck operations."""
        bottlenecks = []
        
        duration_analysis = self._analyze_operation_durations()
        
        for operation_name, analysis in duration_analysis.items():
            cv = analysis.get("cv", 0)
            mean_duration = analysis.get("mean", 0)
            
            if cv > 0.5 and mean_duration > 0.01:  # High variability and long duration
                bottlenecks.append({
                    "operation_name": operation_name,
                    "mean_duration": mean_duration,
                    "coefficient_of_variation": cv,
                    "bottleneck_score": cv * mean_duration
                })
        
        # Sort by bottleneck score
        bottlenecks.sort(key=lambda x: x["bottleneck_score"], reverse=True)
        return bottlenecks[:5]  # Return top 5 bottlenecks
    
    def _calculate_efficiency_metrics(self) -> Dict[str, float]:
        """Calculate efficiency metrics."""
        metrics = {}
        
        step_durations = self.operation_durations.get("step_execution", [])
        if not step_durations:
            return metrics
        
        mean_duration = statistics.mean(step_durations)
        
        # Calculate efficiency based on consistency and speed
        time_consistency = 1.0 - (statistics.stdev(step_durations) / mean_duration if mean_duration > 0 else 0)
        steps_per_second = 1.0 / mean_duration if mean_duration > 0 else 0
        
        metrics["time_consistency_score"] = time_consistency
        metrics["throughput_score"] = min(steps_per_second * 10, 10.0)  # Normalized
        metrics["overall_efficiency_score"] = (time_consistency + metrics["throughput_score"]) / 2
        
        return metrics
    
    def _calculate_timeline_quality(self, analysis: Dict[str, Any]) -> Dict[str, str]:
        """Calculate overall timeline quality."""
        quality = {}
        
        efficiency_score = analysis.get("efficiency_metrics", {}).get("overall_efficiency_score", 0)
        
        if efficiency_score > 8.0:
            quality["overall"] = "Excellent - Highly efficient and consistent timeline"
        elif efficiency_score > 6.0:
            quality["overall"] = "Good - Efficient timeline with minor issues"
        elif efficiency_score > 4.0:
            quality["overall"] = "Fair - Timeline has some inefficiencies"
        else:
            quality["overall"] = "Poor - Significant timeline inefficiencies detected"
        
        # Step duration quality
        step_analysis = analysis.get("step_durations", {})
        cv = step_analysis.get("cv", 0)
        if cv < 0.1:
            quality["step_consistency"] = "Excellent - Very consistent step durations"
        elif cv < 0.2:
            quality["step_consistency"] = "Good - Consistent step durations"
        elif cv < 0.5:
            quality["step_consistency"] = "Acceptable - Some variability in step durations"
        else:
            quality["step_consistency"] = "Poor - High variability in step durations"
        
        return quality
    
    def _generate_timeline_metrics(self) -> List[PerformanceMetrics]:
        """Generate timeline metrics."""
        metrics = []
        
        if not hasattr(self, 'timeline_analysis'):
            return metrics
        
        analysis = self.timeline_analysis
        
        # Add analysis metrics
        metrics.append(PerformanceMetrics(
            timestamp=time.time(),
            metric_name="total_timeline_events",
            value=analysis.get("total_events", 0),
            unit="events"
        ))
        
        metrics.append(PerformanceMetrics(
            timestamp=time.time(),
            metric_name="total_timeline_operations",
            value=analysis.get("total_operations", 0),
            unit="operations"
        ))
        
        metrics.append(PerformanceMetrics(
            timestamp=time.time(),
            metric_name="step_count",
            value=analysis.get("total_steps", 0),
            unit="steps"
        ))
        
        # Performance metrics
        perf_metrics = analysis.get("performance_metrics", {})
        for metric_name, value in perf_metrics.items():
            metrics.append(PerformanceMetrics(
                timestamp=time.time(),
                metric_name=f"timeline_{metric_name}",
                value=float(value),
                unit="" if "time" in metric_name else ""
            ))
        
        # Bottleneck metrics
        bottlenecks = analysis.get("bottleneck_operations", [])
        for i, bottleneck in enumerate(bottlenecks[:3]):  # Top 3 bottlenecks
            metrics.append(PerformanceMetrics(
                timestamp=time.time(),
                metric_name=f"bottleneck_{i+1}_operation",
                value=bottleneck.get("bottleneck_score", 0),
                unit="score"
            ))
        
        return metrics
    
    def generate_timeline_insights(self, result: Dict[str, Any]) -> Dict[str, Any]:
        """Generate insights from timeline analysis."""
        insights = {}
        
        try:
            if "metrics" not in result:
                return insights
            
            # Extract key timeline metrics
            total_events = next((m["value"] for m in result["metrics"] 
                               if m["metric_name"] == "total_timeline_events"), 0)
            total_operations = next((m["value"] for m in result["metrics"] 
                                   if m["metric_name"] == "total_timeline_operations"), 0)
            steps = next((m["value"] for m in result["metrics"] 
                         if m["metric_name"] == "step_count"), 0)
            
            insights["total_timeline_events"] = total_events
            insights["total_timeline_operations"] = total_operations
            insights["simulation_steps"] = steps
            
            if hasattr(self, 'timeline_analysis'):
                analysis = self.timeline_analysis
                
                insights["timeline_quality"] = analysis.get("timeline_quality", {}).get("overall", "Unknown")
                insights["step_consistency"] = analysis.get("timeline_quality", {}).get("step_consistency", "Unknown")
                
                # Performance insights
                perf_metrics = analysis.get("performance_metrics", {})
                if "steps_per_second" in perf_metrics:
                    steps_per_sec = perf_metrics["steps_per_second"]
                    insights["simulation_speed"] = f"{steps_per_sec:.2f} steps/sec"
                    
                    if steps_per_sec > 100:
                        insights["speed_analysis"] = "Very fast simulation - excellent performance"
                    elif steps_per_sec > 50:
                        insights["speed_analysis"] = "Good simulation speed - acceptable performance"
                    elif steps_per_sec > 10:
                        insights["speed_analysis"] = "Moderate simulation speed - room for improvement"
                    else:
                        insights["speed_analysis"] = "Slow simulation - performance optimization needed"
                
                # Bottleneck insights
                bottlenecks = analysis.get("bottleneck_operations", [])
                if bottlenecks:
                    top_bottleneck = bottlenecks[0]
                    insights["main_bottleneck"] = top_bottleneck["operation_name"]
                    insights["bottleneck_score"] = top_bottleneck["bottleneck_score"]
            
            insights["recommendations"] = self._generate_timeline_recommendations(insights)
            
        except Exception as e:
            self.logger.error(f"Error generating timeline insights: {e}")
            insights["error"] = f"Failed to generate insights: {e}"
        
        return insights
    
    def _generate_timeline_recommendations(self, insights: Dict[str, Any]) -> List[str]:
        """Generate recommendations based on timeline analysis."""
        recommendations = []
        
        if "speed_analysis" in insights:
            if "slow" in insights["speed_analysis"].lower():
                recommendations.append("Optimize simulation step execution time")
                recommendations.append("Consider using performance-optimized data structures")
                recommendations.append("Review and optimize brain component performance")
            elif "improve" in insights["speed_analysis"].lower():
                recommendations.append("Profile specific operations for bottlenecks")
                recommendations.append("Consider parallelization opportunities")
        
        if "main_bottleneck" in insights:
            bottleneck = insights["main_bottleneck"]
            recommendations.append(f"Focus on optimizing: {bottleneck}")
            recommendations.append("Review implementation of bottleneck operation")
        
        if "timeline_quality" in insights:
            if "poor" in insights["timeline_quality"].lower():
                recommendations.append("Implement performance monitoring and optimization")
                recommendations.append("Review simulation architecture for inefficiencies")
                recommendations.append("Consider profiling tools for detailed performance analysis")
            elif "excellent" in insights["timeline_quality"].lower():
                recommendations.append("Maintain current optimization approach")
                recommendations.append("Continue monitoring for performance degradation")
        
        return recommendations

    def export_results(self, format: OutputFormat, filepath: Optional[str] = None) -> str:
        """Export timeline profiling results."""
        if not self.results:
            return "No results to export"
        
        # Add timeline insights to results
        if isinstance(self.results, dict):
            insights = self.generate_timeline_insights(self.results)
            self.results["timeline_insights"] = insights
        
        return super().export_results(format, filepath)


class TimelineProfilerCLI:
    """CLI interface for timeline profiler."""
    
    def __init__(self):
        self.logger = logging.getLogger("nlm_profiling.timeline.cli")
    
    def run_timeline_profiling(self, args) -> int:
        """Run timeline profiling."""
        try:
            # Create timeline analysis config
            config = TimelineAnalysisConfig(
                steps=args.steps,
                event_sampling_rate=args.sampling_rate,
                analysis_depth=args.analysis_depth
            )
            
            # Create timeline profiler
            profiler = TimelineProfilerModule(config)
            
            # Run profiling
            result = profiler.run_profiling(steps=args.steps)
            
            # Output results
            if args.json:
                output = profiler.export_results(OutputFormat.JSON)
                print(output)
            else:
                print(f"\n=== Timeline Profiler Results ===")
                print(f"Profiling completed in {result['duration']:.2f}s")
                
                # Print timeline insights
                insights = profiler.generate_timeline_insights(result)
                print(f"\nTimeline Insights:")
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
            self.logger.error(f"Timeline profiling failed: {e}")
            return 1


if __name__ == "__main__":
    import argparse
    import sys
    
    parser = argparse.ArgumentParser(
        description="Timeline Profiler for NLM Brain Simulation",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python -m nlm_profiling.timeline --steps 100
  python -m nlm_profiling.timeline --steps 500 --sampling-rate 10
  python -m nlm_profiling.timeline --json --analysis-depth detailed
        """
    )
    
    parser.add_argument("--steps", type=int, default=100, help="Number of steps to profile")
    parser.add_argument("--sampling-rate", type=float, default=1.0, help="Event sampling rate (events per second)")
    parser.add_argument("--analysis-depth", type=str, choices=["summary", "detailed", "deep"], default="detailed",
                      help="Depth of analysis to perform")
    parser.add_argument("--json", action="store_true", help="Output results as JSON")
    parser.add_argument("--verbose", action="store_true", help="Enable verbose output")
    
    args = parser.parse_args()
    
    # Setup logging
    level = logging.DEBUG if args.verbose else logging.INFO
    logging.basicConfig(level=level)
    
    # Create CLI handler and run profiling
    cli = TimelineProfilerCLI()
    sys.exit(cli.run_timeline_profiling(args))