#!/usr/bin/env python3
"""
Brain Performance Benchmarker Example
Benchmark and analyze brain performance characteristics.
"""

import sys
import os
import argparse
from pathlib import Path
from typing import List, Dict, Any, Optional
import json
import time
import threading
from datetime import datetime
import statistics
import numpy as np

class BrainPerformanceBenchmarker:
    def __init__(self, args):
        self.args = args
        self.benchmark_results = {}
        self.monitoring_data = []
        
        # Benchmark configuration
        self.benchmark_config = {
            "brain_file": self.args.brain_file,
            "output_dir": self.args.output_dir,
            "duration": self.args.duration or 60,  # seconds
            "threads": self.args.threads or 4,
            "metrics": self.args.metrics or ["all"],
            "benchmark_mode": self.args.benchmark_mode or "comprehensive",
            "warmup_time": self.args.warmup_time or 10,
            "sampling_rate": self.args.sampling_rate or 1.0
        }
        
        # Create output directory
        if self.benchmark_config["output_dir"]:
            Path(self.benchmark_config["output_dir"]).mkdir(parents=True, exist_ok=True)
        
        # Performance thresholds
        self.performance_thresholds = {
            "cpu_usage_max": 80.0,
            "memory_usage_max": 2048.0,  # MB
            "inference_time_max": 100.0,  # ms
            "training_time_max": 50.0,    # ms
            "throughput_min": 5.0,        # Hz
            "accuracy_min": 0.9
        }
        
        print("Brain Performance Benchmarker initialized")
        print(f"Brain file: {self.benchmark_config['brain_file']}")
        print(f"Duration: {self.benchmark_config['duration']} seconds")
        print(f"Threads: {self.benchmark_config['threads']}")
        print(f"Metrics: {self.benchmark_config['metrics']}")
    
    def load_brain_data(self) -> bool:
        """Load brain data for benchmarking."""
        try:
            brain_path = Path(self.benchmark_config["brain_file"])
            if not brain_path.exists():
                print(f"Error: Brain file not found: {self.benchmark_config['brain_file']}")
                return False
            
            print(f"Loading brain data from: {brain_path}")
            
            # Load based on file type
            if brain_path.suffix == ".json":
                with open(brain_path, 'r') as f:
                    self.benchmark_results = json.load(f)
            elif brain_path.suffix == ".txt":
                with open(brain_path, 'r') as f:
                    self.benchmark_results = {"text_data": f.read()}
            else:
                self.benchmark_results = {"file": str(brain_path)}
            
            return True
            
        except Exception as e:
            print(f"Error loading brain data: {e}")
            return False
    
    def run_benchmark(self) -> Dict[str, Any]:
        """Run the comprehensive benchmark."""
        print(f"Starting brain performance benchmark ({self.benchmark_config['duration']} seconds)...")
        
        # Warm-up period
        if self.benchmark_config["warmup_time"] > 0:
            print(f"Warm-up period: {self.benchmark_config['warmup_time']} seconds")
            self.run_warmup()
        
        # Start benchmarking
        benchmark_start = time.time()
        
        try:
            if self.benchmark_config["benchmark_mode"] == "comprehensive":
                results = self.run_comprehensive_benchmark()
            elif self.benchmark_config["benchmark_mode"] == "focused":
                results = self.run_focused_benchmark()
            elif self.benchmark_config["benchmark_mode"] == "stress":
                results = self.run_stress_benchmark()
            else:
                results = self.run_comprehensive_benchmark()
            
            benchmark_end = time.time()
            duration = benchmark_end - benchmark_start
            
            # Add metadata
            results["metadata"] = {
                "brain_file": self.benchmark_config["brain_file"],
                "benchmark_mode": self.benchmark_config["benchmark_mode"],
                "duration": duration,
                "start_time": datetime.fromtimestamp(benchmark_start).isoformat(),
                "end_time": datetime.fromtimestamp(benchmark_end).isoformat(),
                "threads": self.benchmark_config["threads"],
                "warmup_time": self.benchmark_config["warmup_time"],
                "sampling_rate": self.benchmark_config["sampling_rate"]
            }
            
            # Generate reports
            self.generate_reports(results)
            
            return results
            
        except Exception as e:
            print(f"Error during benchmark: {e}")
            return {"error": str(e)}
    
    def run_warmup(self):
        """Run warm-up period."""
        warmup_start = time.time()
        warmup_end = warmup_start + self.benchmark_config["warmup_time"]
        
        while time.time() < warmup_end:
            # Simulate brain operations during warm-up
            time.sleep(0.1)
    
    def run_comprehensive_benchmark(self) -> Dict[str, Any]:
        """Run comprehensive benchmark suite."""
        results = {
            "timestamp": datetime.now().isoformat(),
            "benchmark_type": "comprehensive",
            "performance_metrics": {},
            "system_metrics": {},
            "stability_metrics": {},
            "efficiency_metrics": {}
        }
        
        # Performance metrics
        results["performance_metrics"] = self.benchmark_performance()
        
        # System metrics
        results["system_metrics"] = self.benchmark_system_resources()
        
        # Stability metrics
        results["stability_metrics"] = self.benchmark_stability()
        
        # Efficiency metrics
        results["efficiency_metrics"] = self.benchmark_efficiency()
        
        return results
    
    def run_focused_benchmark(self) -> Dict[str, Any]:
        """Run focused benchmark on specific metrics."""
        results = {
            "timestamp": datetime.now().isoformat(),
            "benchmark_type": "focused",
            "performance_metrics": {},
            "system_metrics": {}
        }
        
        # Focus on specific metrics
        for metric in self.benchmark_config["metrics"]:
            if metric == "inference":
                results["performance_metrics"]["inference"] = self.benchmark_inference()
            elif metric == "memory":
                results["system_metrics"]["memory"] = self.benchmark_memory()
            elif metric == "cpu":
                results["system_metrics"]["cpu"] = self.benchmark_cpu()
            elif metric == "stability":
                results["performance_metrics"]["stability"] = self.benchmark_stability()
            elif metric == "efficiency":
                results["performance_metrics"]["efficiency"] = self.benchmark_efficiency()
        
        return results
    
    def run_stress_benchmark(self) -> Dict[str, Any]:n        """Run stress test benchmark."""
        results = {
            "timestamp": datetime.now().isoformat(),
            "benchmark_type": "stress",
            "performance_metrics": {},
            "system_metrics": {},
            "stress_metrics": {}
        }
        
        # Run stress tests
        print("Running stress tests...")
        
        # High load test
        high_load_metrics = self.benchmark_high_load()
        results["stress_metrics"]["high_load"] = high_load_metrics
        
        # Memory pressure test
        memory_pressure_metrics = self.benchmark_memory_pressure()
        results["stress_metrics"]["memory_pressure"] = memory_pressure_metrics
        
        # CPU intensive test
        cpu_intensive_metrics = self.benchmark_cpu_intensive()
        results["stress_metrics"]["cpu_intensive"] = cpu_intensive_metrics
        
        # System metrics during stress
        results["system_metrics"] = self.benchmark_system_resources()
        
        return results
    
    def benchmark_performance(self) -> Dict[str, Any]:
        """Benchmark core brain performance metrics."""
        print("  Benchmarking performance metrics...")
        
        start_time = time.time()
        
        # Simulate multiple brain operations
        inference_times = []
        training_times = []
        accuracies = []
        
        for i in range(10):  # Run 10 iterations
            # Simulate inference
            inference_time = np.random.normal(50, 15)  # ms
            inference_times.append(max(0, inference_time))
            
            # Simulate training
            training_time = np.random.normal(20, 8)  # ms
            training_times.append(max(0, training_time))
            
            # Simulate accuracy
            accuracy = np.random.normal(0.94, 0.02)
            accuracies.append(min(1.0, max(0, accuracy)))
            
            # Simulate processing time
            time.sleep(0.01)
        
        duration = time.time() - start_time
        
        return {
            "duration_seconds": duration,
            "inference": {
                "average_ms": statistics.mean(inference_times),
                "median_ms": statistics.median(inference_times),
                "std_ms": statistics.stdev(inference_times) if len(inference_times) > 1 else 0,
                "min_ms": min(inference_times),
                "max_ms": max(inference_times),
                "throughput_hz": 1000.0 / statistics.mean(inference_times)  # operations per second
            },
            "training": {
                "average_ms": statistics.mean(training_times),
                "median_ms": statistics.median(training_times),
                "std_ms": statistics.stdev(training_times) if len(training_times) > 1 else 0,
                "min_ms": min(training_times),
                "max_ms": max(training_times)
            },
            "accuracy": {
                "average": statistics.mean(accuracies),
                "median": statistics.median(accuracies),
                "std": statistics.stdev(accuracies) if len(accuracies) > 1 else 0,
                "min": min(accuracies),
                "max": max(accuracies)
            }
        }
    
    def benchmark_system_resources(self) -> Dict[str, Any]:
        """Benchmark system resource usage."""
        print("  Benchmarking system resources...")
        
        start_time = time.time()
        cpu_usage = []
        memory_usage = []
        disk_io = []
        
        for i in range(10):
            # Simulate resource measurements
            cpu_usage.append(np.random.uniform(30, 70))  # %
            memory_usage.append(np.random.uniform(512, 1024))  # MB
            disk_io.append(np.random.uniform(10, 50))  # MB/s
            
            time.sleep(0.01)
        
        duration = time.time() - start_time
        
        return {
            "duration_seconds": duration,
            "cpu": {
                "average_percent": statistics.mean(cpu_usage),
                "median_percent": statistics.median(cpu_usage),
                "std_percent": statistics.stdev(cpu_usage) if len(cpu_usage) > 1 else 0,
                "min_percent": min(cpu_usage),
                "max_percent": max(cpu_usage),
                "utilization": statistics.mean(cpu_usage) / self.performance_thresholds["cpu_usage_max"] * 100
            },
            "memory": {
                "average_mb": statistics.mean(memory_usage),
                "median_mb": statistics.median(memory_usage),
                "std_mb": statistics.stdev(memory_usage) if len(memory_usage) > 1 else 0,
                "min_mb": min(memory_usage),
                "max_mb": max(memory_usage),
                "utilization": statistics.mean(memory_usage) / self.performance_thresholds["memory_usage_max"] * 100
            },
            "disk_io": {
                "average_mb_per_s": statistics.mean(disk_io),
                "median_mb_per_s": statistics.median(disk_io),
                "std_mb_per_s": statistics.stdev(disk_io) if len(disk_io) > 1 else 0
            }
        }
    
    def benchmark_stability(self) -> Dict[str, Any]:
        """Benchmark stability and consistency."""
        print("  Benchmarking stability...")
        
        start_time = time.time()
        
        # Run multiple measurements to check stability
        measurements = []
        for i in range(10):
            # Simulate a measurement
            measurement = {
                "cpu": np.random.uniform(30, 70),
                "memory": np.random.uniform(512, 1024),
                "inference_time": np.random.normal(50, 10),
                "accuracy": np.random.normal(0.94, 0.02)
            }
            measurements.append(measurement)
            time.sleep(0.01)
        
        duration = time.time() - start_time
        
        # Calculate stability metrics
        cpu_values = [m["cpu"] for m in measurements]
        memory_values = [m["memory"] for m in measurements]
        inference_values = [m["inference_time"] for m in measurements]
        accuracy_values = [m["accuracy"] for m in measurements]
        
        return {
            "duration_seconds": duration,
            "stability": {
                "cpu_cv": statistics.stdev(cpu_values) / statistics.mean(cpu_values) if statistics.mean(cpu_values) > 0 else 0,  # Coefficient of variation
                "memory_cv": statistics.stdev(memory_values) / statistics.mean(memory_values) if statistics.mean(memory_values) > 0 else 0,
                "inference_cv": statistics.stdev(inference_values) / statistics.mean(inference_values) if statistics.mean(inference_values) > 0 else 0,
                "accuracy_cv": statistics.stdev(accuracy_values) / statistics.mean(accuracy_values) if statistics.mean(accuracy_values) > 0 else 0,
                "overall_stability": 1.0 - (
                    (statistics.stdev(cpu_values) / statistics.mean(cpu_values) if statistics.mean(cpu_values) > 0 else 0) +
                    (statistics.stdev(memory_values) / statistics.mean(memory_values) if statistics.mean(memory_values) > 0 else 0) +
                    (statistics.stdev(inference_values) / statistics.mean(inference_values) if statistics.mean(inference_values) > 0 else 0) +
                    (statistics.stdev(accuracy_values) / statistics.mean(accuracy_values) if statistics.mean(accuracy_values) > 0 else 0)
                ) / 4.0
            }
        }
    
    def benchmark_efficiency(self) -> Dict[str, Any]:
        """Benchmark efficiency metrics."""
        print("  Benchmarking efficiency...")
        
        # Simulate efficiency calculations
        efficiency = {
            "cpu_efficiency": np.random.uniform(0.8, 0.95),
            "memory_efficiency": np.random.uniform(0.75, 0.90),
            "energy_efficiency": np.random.uniform(0.82, 0.96),
            "overall_efficiency": np.random.uniform(0.85, 0.93)
        }
        
        return efficiency
    
    def benchmark_inference(self) -> Dict[str, Any]:
        """Benchmark inference specifically."""
        print("  Benchmarking inference...")
        
        start_time = time.time()
        
        inference_times = []
        for i in range(20):
            # Simulate inference with varying complexity
            complexity = i / 20.0 + 0.1
            base_time = 50 + complexity * 100  # ms
            inference_time = base_time * np.random.uniform(0.8, 1.2)
            inference_times.append(max(0, inference_time))
            time.sleep(0.005)
        
        duration = time.time() - start_time
        
        return {
            "duration_seconds": duration,
            "inference": {
                "average_ms": statistics.mean(inference_times),
                "median_ms": statistics.median(inference_times),
                "p95_ms": np.percentile(inference_times, 95),
                "p99_ms": np.percentile(inference_times, 99),
                "throughput_hz": 1000.0 / statistics.mean(inference_times),
                "complexity_scaling": self.calculate_complexity_scaling(inference_times)
            }
        }
    
    def benchmark_memory(self) -> Dict[str, Any]:
        """Benchmark memory usage."""
        print("  Benchmarking memory...")
        
        # Simulate memory patterns
        memory_patterns = {
            "working_set_size": np.random.uniform(256, 1024),
            "peak_memory": np.random.uniform(512, 2048),
            "memory_growth": np.random.uniform(0.01, 0.1),
            "cache_hit_rate": np.random.uniform(0.7, 0.95)
        }
        
        return memory_patterns
    
    def benchmark_cpu(self) -> Dict[str, Any]:
        """Benchmark CPU usage."""
        print("  Benchmarking CPU...")
        
        start_time = time.time()
        
        cpu_samples = []
        for i in range(10):
            # Simulate CPU usage with varying load
            base_cpu = 45 + i * 5
            cpu = base_cpu * np.random.uniform(0.8, 1.2)
            cpu_samples.append(max(0, min(100, cpu)))
            time.sleep(0.01)
        
        duration = time.time() - start_time
        
        return {
            "duration_seconds": duration,
            "cpu": {
                "average_percent": statistics.mean(cpu_samples),
                "median_percent": statistics.median(cpu_samples),
                "min_percent": min(cpu_samples),
                "max_percent": max(cpu_samples),
                "load_balance": self.calculate_load_balance(cpu_samples)
            }
        }
    
    def benchmark_high_load(self) -> Dict[str, Any]:
        """Benchmark under high load conditions."""
        print("  Benchmarking high load...")
        
        # Simulate high load
        start_time = time.time()
        
        metrics = {
            "cpu_usage": np.random.uniform(70, 95),
            "memory_usage": np.random.uniform(1024, 1536),
            "inference_time": np.random.normal(80, 20),
            "throughput": 1000.0 / np.random.normal(80, 20),
            "stability": np.random.uniform(0.7, 0.85)
        }
        
        duration = time.time() - start_time
        
        return {
            "duration_seconds": duration,
            **metrics
        }
    
    def benchmark_memory_pressure(self) -> Dict[str, Any]:
        """Benchmark under memory pressure."""
        print("  Benchmarking memory pressure...")
        
        metrics = {
            "memory_usage": np.random.uniform(1536, 2048),
            "swapping": np.random.uniform(0, 0.1),
            "gc_frequency": np.random.uniform(5, 15),  # Hz
            "throughput": np.random.uniform(500, 800),
            "stability": np.random.uniform(0.6, 0.8)
        }
        
        return metrics
    
    def benchmark_cpu_intensive(self) -> Dict[str, Any]:
        """Benchmark CPU intensive operations."""
        print("  Benchmarking CPU intensive...")
        
        start_time = time.time()
        
        # Simulate CPU intensive work
        cpu_load = []
        for i in range(5):
            # High CPU workload
            workload = np.random.uniform(80, 100)
            cpu_load.append(workload)
            time.sleep(0.05)
        
        duration = time.time() - start_time
        
        return {
            "duration_seconds": duration,
            "cpu": {
                "average_percent": statistics.mean(cpu_load),
                "max_percent": max(cpu_load),
                "thermal": np.random.uniform(65, 85),  # Celsius
                "throttling": np.random.uniform(0, 0.05)
            }
        }
    
    def calculate_complexity_scaling(self, inference_times):
        """Calculate complexity scaling characteristics."""
        if len(inference_times) < 2:
            return {"scaling_factor": 1.0}
        
        # Simple scaling analysis (complexity = iteration index / total iterations)
        complexities = [i / len(inference_times) for i in range(len(inference_times))]
        
        # Linear regression to find scaling factor
        sum_x = sum(complexities)
        sum_y = sum(inference_times)
        sum_xx = sum(x*x for x in complexities)
        sum_xy = sum(x*y for x, y in zip(complexities, inference_times))
        
        n = len(complexities)
        denominator = n * sum_xx - sum_x * sum_x
        
        if denominator != 0:
            slope = (n * sum_xy - sum_x * sum_y) / denominator
            intercept = (sum_y - slope * sum_x) / n
        else:
            slope = 0
            intercept = sum_y / n
        
        return {
            "scaling_factor": slope,
            "base_time": intercept,
            "r_squared": 0.0  # Would need to calculate actual R-squared
        }
    
    def calculate_load_balance(self, cpu_samples):
        """Calculate load balance metrics."""
        if len(cpu_samples) < 2:
            return {"balance_factor": 1.0}
        
        # Load balance = 1 - (max - min) / (max + min)
        max_cpu = max(cpu_samples)
        min_cpu = min(cpu_samples)
        
        if max_cpu + min_cpu > 0:
            balance_factor = 1.0 - (max_cpu - min_cpu) / (max_cpu + min_cpu)
        else:
            balance_factor = 1.0
        
        return {
            "balance_factor": balance_factor,
            "min_cpu": min_cpu,
            "max_cpu": max_cpu,
            "avg_cpu": statistics.mean(cpu_samples)
        }
    
    def generate_reports(self, results: Dict[str, Any]):
        """Generate benchmark reports."""
        print("Generating benchmark reports...")
        
        # Generate markdown report
        markdown_report = self.generate_markdown_report(results)
        
        # Generate JSON report
        json_report = self.generate_json_report(results)
        
        # Generate HTML report
        html_report = self.generate_html_report(results)
        
        # Save reports
        if self.benchmark_config["output_dir"]:
            output_dir = Path(self.benchmark_config["output_dir"])
            
            # Save markdown
            with open(output_dir / "benchmark_report.md", 'w') as f:
                f.write(markdown_report)
            
            # Save JSON
            with open(output_dir / "benchmark_report.json", 'w') as f:
                json.dump(results, f, indent=2)
            
            # Save HTML
            with open(output_dir / "benchmark_report.html", 'w') as f:
                f.write(html_report)
            
            self.log_report_generation(output_dir)
    
    def generate_markdown_report(self, results: Dict[str, Any]) -> str:
        """Generate markdown benchmark report."""
        metadata = results.get("metadata", {})
        
        report = f"# Brain Performance Benchmark Report\n"
        report += f"Generated: {metadata.get('start_time', datetime.now().isoformat())}\n"
        report += f"Duration: {metadata.get('duration', 0):.2f} seconds\n\n"
        
        report += f"## Performance Summary\n"
        if "performance_metrics" in results:
            perf = results["performance_metrics"]
            report += f"- Average Inference Time: {perf.get('inference', {}).get('average_ms', 0):.2f} ms\n"
            report += f"- Average Training Time: {perf.get('training', {}).get('average_ms', 0):.2f} ms\n"
            report += f"- Average Accuracy: {perf.get('accuracy', {}).get('average', 0):.4f}\n"
            report += f"- Throughput: {perf.get('inference', {}).get('throughput_hz', 0):.2f} Hz\n\n"
        
        report += f"## System Resources\n"
        if "system_metrics" in results:
            sys = results["system_metrics"]
            if "cpu" in sys:
                report += f"- CPU Usage: {sys['cpu'].get('average_percent', 0):.2f}%\n"
            if "memory" in sys:
                report += f"- Memory Usage: {sys['memory'].get('average_mb', 0):.2f} MB\n\n"
        
        report += f"## Stability Analysis\n"
        if "stability_metrics" in results:
            stability = results["stability_metrics"]
            if "stability" in stability:
                report += f"- Overall Stability: {stability['stability'].get('overall_stability', 0):.4f}\n"
        
        report += f"## Recommendations\n"
        
        # Add recommendations based on thresholds
        if "performance_metrics" in results:
            perf = results["performance_metrics"]
            
            # Check performance against thresholds
            inference_avg = perf.get('inference', {}).get('average_ms', 0)
            if inference_avg > self.performance_thresholds["inference_time_max"]:
                report += f"- ⚠️  Inference time ({inference_avg:.2f} ms) exceeds threshold ({self.performance_thresholds['inference_time_max']} ms)\n"
            
            accuracy_avg = perf.get('accuracy', {}).get('average', 0)
            if accuracy_avg < self.performance_thresholds["accuracy_min"]:
                report += f"- ⚠️  Accuracy ({accuracy_avg:.4f}) below threshold ({self.performance_thresholds['accuracy_min']})\n"
        
        if "system_metrics" in results:
            sys = results["system_metrics"]
            
            # Check system metrics against thresholds
            cpu_avg = sys.get('cpu', {}).get('average_percent', 0)
            if cpu_avg > self.performance_thresholds["cpu_usage_max"]:
                report += f"- ⚠️  CPU usage ({cpu_avg:.2f}%) exceeds threshold ({self.performance_thresholds['cpu_usage_max']}%)\n"
            
            mem_avg = sys.get('memory', {}).get('average_mb', 0)
            if mem_avg > self.performance_thresholds["memory_usage_max"]:
                report += f"- ⚠️  Memory usage ({mem_avg:.2f} MB) exceeds threshold ({self.performance_thresholds['memory_usage_max']} MB)\n"
        
        return report
    
    def generate_json_report(self, results: Dict[str, Any]) -> Dict[str, Any]:
        """Generate JSON benchmark report."""
        # Create a clean version for JSON output
        json_report = {
            "benchmark_type": results.get("benchmark_type", "unknown"),
            "timestamp": results.get("timestamp", datetime.now().isoformat()),
            "duration": results.get("metadata", {}).get("duration", 0),
            "performance": {},
            "system": {},
            "recommendations": []
        }
        
        # Add performance metrics if present
        if "performance_metrics" in results:
            json_report["performance"] = results["performance_metrics"]
        
        # Add system metrics if present
        if "system_metrics" in results:
            json_report["system"] = results["system_metrics"]
        
        return json_report
    
    def generate_html_report(self, results: Dict[str, Any]) -> str:
        """Generate HTML benchmark report."""
        metadata = results.get("metadata", {})
        
        html = f"""<!DOCTYPE html>
<html>
<head>
    <title>Brain Performance Benchmark Report</title>
    <style>
        body {{ font-family: Arial, sans-serif; margin: 40px; }}
        .header {{ background-color: #f0f0f0; padding: 20px; border-radius: 5px; }}
        .section {{ margin: 20px 0; }}
        .metric {{ display: inline-block; margin: 10px; padding: 10px; 
                 background-color: #f9f9f9; border-radius: 3px; }}
        .warning {{ background-color: #fff3cd; border-color: #ffeaa7; }}
        .good {{ background-color: #d4edda; border-color: #c3e6cb; }}
    </style>
</head>
<body>
    <div class="header">
        <h1>Brain Performance Benchmark Report</h1>
        <p>Generated: {metadata.get('start_time', datetime.now().isoformat())}</p>
        <p>Duration: {metadata.get('duration', 0):.2f} seconds</p>
    </div>
    
    <div class="section">
        <h2>Performance Summary</h2>
"""
        
        if "performance_metrics" in results:
            perf = results["performance_metrics"]
            html += f"""
        <div class="metric good">
            <h3>Inference</h3>
            <p>Average: {perf.get('inference', {}).get('average_ms', 0):.2f} ms</p>
            <p>Throughput: {perf.get('inference', {}).get('throughput_hz', 0):.2f} Hz</p>
        </div>
        
        <div class="metric good">
            <h3>Accuracy</h3>
            <p>Average: {perf.get('accuracy', {}).get('average', 0):.4f}</p>
            <p>Std Dev: {perf.get('accuracy', {}).get('std', 0):.4f}</p>
        </div>
"""
        
        html += """
    </div>
    
    <div class="section">
        <h2>System Resources</h2>
"""
        
        if "system_metrics" in results:
            sys = results["system_metrics"]
            if "cpu" in sys:
                cpu_avg = sys["cpu"]["average_percent"]
                cpu_class = "warning" if cpu_avg > self.performance_thresholds["cpu_usage_max"] else "good"
                html += f"""
        <div class="metric {cpu_class}">
            <h3>CPU</h3>
            <p>Usage: {cpu_avg:.2f}%</p>
            <p>Threshold: {self.performance_thresholds["cpu_usage_max"]}%</p>
        </div>
        """
            
            if "memory" in sys:
                mem_avg = sys["memory"]["average_mb"]
                mem_class = "warning" if mem_avg > self.performance_thresholds["memory_usage_max"] else "good"
                html += f"""
        <div class="metric {mem_class}">
            <h3>Memory</h3>
            <p>Usage: {mem_avg:.2f} MB</p>
            <p>Threshold: {self.performance_thresholds["memory_usage_max"]} MB</p>
        </div>
        """
        
        html += """
    </div>
    
    <div class="section">
        <h2>Recommendations</h2>
        <ul>
            <li>Monitor performance regularly</li>
            <li>Optimize inference for better throughput</li>
            <li>Consider memory optimization strategies</li>
        </ul>
    </div>
    
</body>
</html>
"""
        
        return html
    
    def log_report_generation(self, output_dir: Path):
        """Log report generation."""
        report_files = list(output_dir.glob("benchmark_report.*"))
        
        for report_file in report_files:
            print(f"Report generated: {report_file.name}")
        
        print(f"All reports saved to: {output_dir}")

def main():
    parser = argparse.ArgumentParser(
        description="Brain Performance Benchmarker",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Run comprehensive benchmark
  brain_performance_benchmarker.py --brain brain_data.json --output-dir ./results
  
  # Run focused benchmark on specific metrics
  brain_performance_benchmarker.py --brain brain_data.json --metrics inference memory cpu
  
  # Run stress test
  brain_performance_benchmarker.py --brain brain_data.json --benchmark-mode stress
  
  # Monitor brain for specific duration
  brain_performance_benchmarker.py --brain brain_data.json --duration 120 --output-dir ./monitoring
"""
    )
    
    parser.add_argument(
        "--brain-file",
        required=True,
        help="Path to brain data file to benchmark"
    )
    parser.add_argument(
        "--output-dir",
        help="Output directory for benchmark reports"
    )
    parser.add_argument(
        "--duration",
        type=int,
        help="Benchmark duration in seconds (default: 60)"
    )
    parser.add_argument(
        "--threads",
        type=int,
        default=4,
        help="Number of threads for parallel operations (default: 4)"
    )
    parser.add_argument(
        "--metrics",
        nargs="+",
        choices=["inference", "memory", "cpu", "stability", "efficiency", "all"],
        default=["all"],
        help="Performance metrics to benchmark (default: all)"
    )
    parser.add_argument(
        "--benchmark-mode",
        choices=["comprehensive", "focused", "stress"],
        default="comprehensive",
        help="Benchmark mode (default: comprehensive)"
    )
    parser.add_argument(
        "--warmup-time",
        type=int,
        default=10,
        help="Warm-up period in seconds (default: 10)"
    )
    parser.add_argument(
        "--sampling-rate",
        type=float,
        default=1.0,
        help="Sampling rate for metrics collection (default: 1.0)"
    )
    
    args = parser.parse_args()
    
    # Convert metrics list
    metrics = ["all"] if "all" in args.metrics else args.metrics
    
    benchmarker = BrainPerformanceBenchmarker(args)
    
    # Load brain data
    if not benchmarker.load_brain_data():
        print("Failed to load brain data. Exiting.")
        return 1
    
    # Run benchmark
    results = benchmarker.run_benchmark()
    
    if "error" in results:
        print(f"Benchmark failed: {results['error']}")
        return 1
    
    print("Benchmark completed successfully!")
    return 0

if __name__ == "__main__":
    sys.exit(main())"