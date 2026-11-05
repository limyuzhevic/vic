#!/usr/bin/env python3
"""
Benchmark Harness Module

Structured benchmarking system for comparing different brain configurations.
Provides comprehensive benchmarking capabilities with multiple configurations and performance comparisons.
"""

import time
import logging
import statistics
from typing import Dict, List, Optional, Any, Callable
from dataclasses import dataclass
from enum import Enum

from .base import PerformanceMetrics, BenchmarkResult, ProfilingFramework, OutputFormat

logger = logging.getLogger("nlm_profiling.benchmark")


class BenchmarkType(Enum):
    """Benchmark type enumeration."""
    CREATION = "creation"
    STEP_PERFORMANCE = "step_performance"
    CONFIGURATION = "configuration"
    COMPARATIVE = "comparative"


class BenchmarkConfig:
    """Configuration for benchmarking."""
    
    def __init__(self, benchmark_type: BenchmarkType = BenchmarkType.CREATION,
                 steps: int = 100, runs: int = 3, warmup_steps: int = 10,
                 config_file: Optional[str] = None):
        self.benchmark_type = benchmark_type
        self.steps = steps
        self.runs = runs
        self.warmup_steps = warmup_steps
        self.config_file = config_file
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary."""
        return {
            "benchmark_type": self.benchmark_type.value,
            "steps": self.steps,
            "runs": self.runs,
            "warmup_steps": self.warmup_steps,
            "config_file": self.config_file
        }


class Configuration:
    """Brain configuration for benchmarking."""
    
    def __init__(self, name: str, parameters: Dict[str, Any]):
        self.name = name
        self.parameters = parameters
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary."""
        return {
            "name": self.name,
            "parameters": self.parameters
        }


class BenchmarkHarnessModule(ProfilingFramework):
    """Benchmark Harness for comparing different brain configurations."""
    
    def __init__(self, config: Optional[BenchmarkConfig] = None):
        super().__init__("benchmark_harness")
        self.config = config or BenchmarkConfig()
        self.benchmark_results = []
    
    def add_configuration(self, configuration: Configuration) -> None:
        """Add a configuration to benchmark."""
        self.benchmark_results.append(configuration)
    
    def remove_configuration(self, name: str) -> bool:
        """Remove a configuration by name."""
        for i, config in enumerate(self.benchmark_results):
            if config.name == name:
                del self.benchmark_results[i]
                return True
        return False
    
    def clear_configurations(self) -> None:
        """Clear all configurations."""
        self.benchmark_results.clear()
    
    def get_configurations(self) -> List[Configuration]:
        """Get all configurations."""
        return self.benchmark_results.copy()
    
    def _profile(self, **kwargs) -> BenchmarkResult:
        """Run benchmark tests."""
        try:
            import pynlm
            
            # Extract benchmark parameters
            brain_configs = kwargs.get('brain_configs', self.benchmark_results)
            steps = kwargs.get('steps', self.config.steps)
            runs = kwargs.get('runs', self.config.runs)
            warmup_steps = kwargs.get('warmup_steps', self.config.warmup_steps)
            
            self.logger.info(f"Starting benchmark with {len(brain_configs)} configurations, {runs} runs each")
            
            benchmark_suite = []
            
            for config_idx, config in enumerate(brain_configs):
                config_name = config.name if hasattr(config, 'name') else f"Config_{config_idx}"
                config_params = config.parameters if hasattr(config, 'parameters') else config
                
                self.logger.info(f"Benchmarking configuration: {config_name}")
                
                config_metrics = []
                
                for run in range(runs):
                    self.logger.info(f"  Run {run + 1}/{runs}")
                    
                    # Create brain with configuration
                    try:
                        if isinstance(config_params, dict):
                            # Create config from parameters
                            brain = pynlm.createBrain(pynlm.Config(**config_params))
                        else:
                            brain = config_params
                        
                        brain.initialize()
                        
                        # Warm-up steps
                        for j in range(warmup_steps):
                            brain.step(j)
                        
                        # Benchmark step performance
                        run_start = time.time()
                        step_times = []
                        
                        for step in range(steps):
                            step_start = time.time()
                            brain.step(step + warmup_steps)
                            step_duration = time.time() - step_start
                            step_times.append(step_duration)
                        
                        run_duration = time.time() - run_start
                        
                        # Calculate performance metrics
                        steps_per_second = steps / run_duration if run_duration > 0 else 0
                        avg_step_time = statistics.mean(step_times) if step_times else 0
                        min_step_time = min(step_times) if step_times else 0
                        max_step_time = max(step_times) if step_times else 0
                        stddev_step_time = statistics.stdev(step_times) if len(step_times) > 1 else 0
                        
                        # Create run metrics
                        run_metrics = [
                            PerformanceMetrics(
                                timestamp=time.time(),
                                metric_name="steps_per_second",
                                value=steps_per_second,
                                unit="steps/sec"
                            ),
                            PerformanceMetrics(
                                timestamp=time.time(),
                                metric_name="average_step_time",
                                value=avg_step_time,
                                unit="seconds"
                            ),
                            PerformanceMetrics(
                                timestamp=time.time(),
                                metric_name="min_step_time",
                                value=min_step_time,
                                unit="seconds"
                            ),
                            PerformanceMetrics(
                                timestamp=time.time(),
                                metric_name="max_step_time",
                                value=max_step_time,
                                unit="seconds"
                            ),
                            PerformanceMetrics(
                                timestamp=time.time(),
                                metric_name="step_time_stddev",
                                value=stddev_step_time,
                                unit="seconds"
                            ),
                            PerformanceMetrics(
                                timestamp=time.time(),
                                metric_name="run_duration",
                                value=run_duration,
                                unit="seconds"
                            )
                        ]
                        
                        config_metrics.extend(run_metrics)
                        
                    except Exception as e:
                        self.logger.error(f"Error in configuration {config_name}, run {run + 1}: {e}")
                        # Add error metrics
                        config_metrics.append(PerformanceMetrics(
                            timestamp=time.time(),
                            metric_name="run_error",
                            value=1.0,
                            unit="error"
                        ))
                
                # Calculate configuration statistics
                if config_metrics:
                    valid_metrics = [m for m in config_metrics if m.metric_name != "run_error"]
                    
                    if valid_metrics:
                        # Extract configuration-specific metrics
                        steps_per_second_values = [m.value for m in config_metrics 
                                                 if m.metric_name == "steps_per_second"]
                        if steps_per_second_values:
                            avg_steps_per_second = statistics.mean(steps_per_second_values)
                            stddev_steps_per_second = statistics.stdev(steps_per_second_values) if len(steps_per_second_values) > 1 else 0
                            
                            config_summary = {
                                "configuration": config_name,
                                "parameters": config_params,
                                "runs": len(valid_metrics) // 6,  # Approximate from metrics
                                "average_steps_per_second": avg_steps_per_second,
                                "steps_per_second_stddev": stddev_steps_per_second,
                                "all_metrics": config_metrics
                            }
                        
                        benchmark_suite.append(config_summary)
            
            # Create final benchmark result
            metrics = []
            
            # Add overall benchmark metrics
            if benchmark_suite:
                all_spss = [config["average_steps_per_second"] for config in benchmark_suite 
                           if "average_steps_per_second" in config]
                
                if all_spss:
                    metrics.append(PerformanceMetrics(
                        timestamp=time.time(),
                        metric_name="configurations_benchmarked",
                        value=len(benchmark_suite),
                        unit="configurations"
                    ))
                    
                    metrics.append(PerformanceMetrics(
                        timestamp=time.time(),
                        metric_name="average_steps_per_second",
                        value=statistics.mean(all_spss),
                        unit="steps/sec"
                    ))
                    
                    metrics.append(PerformanceMetrics(
                        timestamp=time.time(),
                        metric_name="best_configuration",
                        value=max(all_spss),
                        unit="steps/sec"
                    ))
                    
                    metrics.append(PerformanceMetrics(
                        timestamp=time.time(),
                        metric_name="total_benchmark_time",
                        value=sum(config.get("duration", 0) for config in benchmark_suite),
                        unit="seconds"
                    ))
            
            result = BenchmarkResult(
                test_name=self.name,
                parameters=kwargs,
                metrics=metrics,
                duration=sum(config.get("duration", 0) for config in benchmark_suite),
                success=True
            )
            
            self.results = result
            return result
            
        except Exception as e:
            self.logger.error(f"Error in benchmark harness: {e}")
            raise
    
    def generate_benchmark_insights(self, result: BenchmarkResult) -> Dict[str, Any]:
        """Generate insights from benchmark results."""
        insights = {}
        
        try:
            # Extract key metrics
            config_count = next((m.value for m in result.metrics 
                               if m.metric_name == "configurations_benchmarked"), 0)
            avg_spss = next((m.value for m in result.metrics 
                           if m.metric_name == "average_steps_per_second"), 0)
            best_spss = next((m.value for m in result.metrics 
                            if m.metric_name == "best_configuration"), 0)
            
            insights["configurations_benchmarked"] = config_count
            insights["average_performance"] = avg_spss
            insights["best_configuration_performance"] = best_spss
            
            if config_count > 0 and avg_spss > 0:
                performance_improvement = (best_spss - avg_spss) / avg_spss * 100 if avg_spss > 0 else 0
                insights["performance_improvement_percentage"] = performance_improvement
                
                if performance_improvement > 20:
                    insights["performance_analysis"] = "Excellent performance variation - significant optimizations possible"
                elif performance_improvement > 10:
                    insights["performance_analysis"] = "Good performance variation - moderate optimization opportunities"
                elif performance_improvement > 5:
                    insights["performance_analysis"] = "Acceptable performance variation - minor optimizations available"
                else:
                    insights["performance_analysis"] = "Low performance variation - configurations are similar"
            
            insights["recommendations"] = self._generate_benchmark_recommendations(insights)
            
        except Exception as e:
            self.logger.error(f"Error generating benchmark insights: {e}")
            insights["error"] = f"Failed to generate insights: {e}"
        
        return insights
    
    def _generate_benchmark_recommendations(self, insights: Dict[str, Any]) -> List[str]:
        """Generate recommendations based on benchmark results."""
        recommendations = []
        
        if "performance_analysis" in insights:
            if "significant optimizations" in insights["performance_analysis"].lower():
                recommendations.append("Investigate best configuration for key optimization parameters")
                recommendations.append("Consider parameter sweep to find optimal settings")
                recommendations.append("Document best practices for configuration")
            elif "moderate" in insights["performance_analysis"].lower():
                recommendations.append("Profile best configuration to identify optimization points")
                recommendations.append("Compare parameters between configurations")
            elif "minor" in insights["performance_analysis"].lower():
                recommendations.append("Current configurations show good similarity")
                recommendations.append("Focus on consistency and reproducibility")
            else:
                recommendations.append("Configurations perform similarly - focus on stability")
        
        if "performance_improvement_percentage" in insights:
            improvement = insights["performance_improvement_percentage"]
            if improvement > 20:
                recommendations.append("Significant performance differences detected - prioritize best configurations")
            elif improvement > 10:
                recommendations.append("Moderate performance differences - analyze top performers")
            else:
                recommendations.append("Performance differences are minor - consider other factors like stability")
        
        return recommendations

    def generate_comparative_report(self, results: List[BenchmarkResult]) -> Dict[str, Any]:
        """Generate a comparative report across multiple benchmark runs."""
        if not results:
            return {}
        
        report = {
            "total_benchmarks": len(results),
            "successful_benchmarks": sum(1 for r in results if r.success),
            "failed_benchmarks": sum(1 for r in results if not r.success),
            "average_duration": sum(r.duration for r in results) / len(results)
        }
        
        # Aggregate metrics across all runs
        all_metrics = {}
        for result in results:
            for metric in result.metrics:
                metric_name = metric.metric_name
                if metric_name not in all_metrics:
                    all_metrics[metric_name] = []
                all_metrics[metric_name].append(metric.value)
        
        # Calculate averages and ranges for each metric
        for metric_name, values in all_metrics.items():
            if values:
                report[f"{metric_name}_average"] = statistics.mean(values)
                report[f"{metric_name}_min"] = min(values)
                report[f"{metric_name}_max"] = max(values)
                report[f"{metric_name}_stddev"] = statistics.stdev(values) if len(values) > 1 else 0
        
        return report

    def export_results(self, format: OutputFormat, filepath: Optional[str] = None) -> str:
        """Export results in specified format with benchmark insights."""
        if not self.results:
            return "No results to export"
        
        # Add benchmark insights to results
        if hasattr(self.results, 'to_dict'):
            insights = self.generate_benchmark_insights(self.results)
            result_dict = self.results.to_dict()
            result_dict["benchmark_insights"] = insights
        
        return super().export_results(format, filepath)


class BenchmarkCLInterface:
    """CLI interface for benchmark harness."""
    
    def __init__(self):
        self.logger = logging.getLogger("nlm_profiling.benchmark.cli")
    
    def create_sample_configuration(self, config_type: str = "default") -> Configuration:
        """Create a sample configuration for benchmarking."""
        configs = {
            "default": Configuration("default", {
                "neuron_count": 100,
                "region_count": 2,
                "plasticity": True,
                "learning_rate": 0.01
            }),
            "high_performance": Configuration("high_performance", {
                "neuron_count": 500,
                "region_count": 3,
                "plasticity": True,
                "learning_rate": 0.001,
                "simd_enabled": True
            }),
            "minimal": Configuration("minimal", {
                "neuron_count": 50,
                "region_count": 1,
                "plasticity": False,
                "learning_rate": 0.1
            }),
            "balanced": Configuration("balanced", {
                "neuron_count": 200,
                "region_count": 2,
                "plasticity": True,
                "learning_rate": 0.05,
                "learning_enabled": True
            })
        }
        
        return configs.get(config_type, configs["default"])
    
    def run_benchmark(self, args) -> int:
        """Run benchmark suite."""
        try:
            # Create benchmark config
            config = BenchmarkConfig(
                steps=args.steps,
                runs=args.runs,
                warmup_steps=args.warmup_steps
            )
            
            # Create benchmark harness
            harness = BenchmarkHarnessModule(config)
            
            # Add configurations based on arguments
            if hasattr(args, 'config_type') and args.config_type:
                config_obj = self.create_sample_configuration(args.config_type)
                harness.add_configuration(config_obj)
            
            if hasattr(args, 'custom_configs') and args.custom_configs:
                for config_data in args.custom_configs:
                    harness.add_configuration(Configuration(**config_data))
            
            # Run benchmark
            kwargs = {}
            if hasattr(args, 'brain_configs') and args.brain_configs:
                kwargs['brain_configs'] = args.brain_configs
            
            result = harness.run_profiling(**kwargs)
            
            # Output results
            if args.json:
                output = harness.export_results(OutputFormat.JSON)
                print(output)
            else:
                print(f"\n=== Benchmark Harness Results ===")
                print(f"Test completed in {result.duration:.2f}s")
                print(f"Success: {result.success}")
                
                # Print benchmark insights
                insights = harness.generate_benchmark_insights(result)
                print(f"\nBenchmark Insights:")
                for key, value in insights.items():
                    if key != "recommendations":
                        print(f"  {key}: {value}")
                
                print(f"\nRecommendations:")
                for recommendation in insights.get("recommendations", []):
                    print(f"  - {recommendation}")
                
                # Print key metrics
                print(f"\nKey Metrics:")
                for metric in result.metrics:
                    print(f"  {metric.to_text()}")
            
            return 0
            
        except Exception as e:
            self.logger.error(f"Benchmark failed: {e}")
            return 1


if __name__ == "__main__":
    import argparse
    import sys
    
    parser = argparse.ArgumentParser(
        description="Benchmark Harness for NLM Performance Testing",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python -m nlm_profiling.benchmark --steps 100 --runs 3
  python -m nlm_profiling.benchmark --config-type high_performance
  python -m nlm_profiling.benchmark --custom-configs '[{"name": "test", "parameters": {"neuron_count": 50}}]'
        """
    )
    
    parser.add_argument("--steps", type=int, default=100, help="Number of steps to benchmark")
    parser.add_argument("--runs", type=int, default=3, help="Number of benchmark runs")
    parser.add_argument("--warmup-steps", type=int, default=10, help="Warm-up steps for each run")
    parser.add_argument("--json", action="store_true", help="Output results as JSON")
    parser.add_argument("--verbose", action="store_true", help="Enable verbose output")
    
    # Configuration options
    parser.add_argument("--config-type", type=str, choices=["default", "high_performance", "minimal", "balanced"],
                      help="Predefined configuration type")
    
    parser.add_argument("--custom-configs", type=str, help="JSON string of custom configurations")
    
    args = parser.parse_args()
    
    # Setup logging
    level = logging.DEBUG if args.verbose else logging.INFO
    logging.basicConfig(level=level)
    
    # Create CLI handler and run benchmark
    cli = BenchmarkCLInterface()
    sys.exit(cli.run_benchmark(args))