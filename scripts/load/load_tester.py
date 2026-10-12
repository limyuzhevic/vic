#!/usr/bin/env python3
"""
Load Testing Tool for NLM Brain Simulation

Comprehensive load testing tool for testing brain performance under various load conditions.
Tests parallel processing performance, stress tests memory usage, and generates detailed
load testing reports with performance analysis.

Features:
- Concurrent simulation execution
- Memory stress testing
- Parallel processing performance testing
- Load level testing (low, medium, high)
- Resource utilization tracking
- Performance degradation analysis
- Load curve generation
- Bottleneck identification
- Integration with profiling tools

Usage:
    # Test brain performance under concurrent load
    python load_tester.py --concurrent 50 --duration 300
    
    # Memory stress testing
    python load_tester.py --memory-pressure --peak-requests 1000
    
    # Parallel processing performance
    python load_tester.py --parallel 100 --batch-size 50
    
    # Load level testing
    python load_tester.py --load-levels low medium high --report
    
    # Stress testing with resource monitoring
    python load_tester.py --stress --monitor-cpu --monitor-memory --interval 2.0

Examples:
    >>> from load_tester import LoadTester
    >>> tester = LoadTester()
    >>> results = tester.run_concurrent_test(simulation_func, concurrent_users=50, duration=300)
    
    >>> results = tester.run_memory_stress_test(simulation_func, memory_gb=16, duration=600)
    >>> tester.generate_load_report(results, "load_test_report.html")

See Also:
    performance_profiler.py - For CPU and time profiling
    memory_profiler.py - For memory analysis
    benchmark_suite.py - For baseline performance comparison
    performance_analyzer.py - For bottleneck analysis

API Reference:
    LoadTester - Main load testing class
    run_concurrent_test() - Test concurrent simulation performance
    run_memory_stress_test() - Memory stress testing
    run_parallel_test() - Parallel processing performance
    generate_load_report() - Generate load testing reports

Author: NLM Team
Version: 1.0.0
License: MIT

For support and documentation, visit: https://github.com/your-repo/nlm-tools
"""

import time
import os
import sys
import argparse
from typing import Dict, List, Any, Optional, Callable, Tuple
from pathlib import Path
import threading
import queue
import random
import statistics
import json
from datetime import datetime
import psutil
import numpy as np
import matplotlib.pyplot as plt
from concurrent.futures import ThreadPoolExecutor, as_completed, ProcessPoolExecutor
import gc
import resource
from contextlib import contextmanager

# Set style for matplotlib
plt.style.use('seaborn-v0_8')


class ResourceMonitor:
    """Monitor system resources during load testing."""
    
    def __init__(self, interval: float = 1.0):
        self.interval = interval
        self.cpu_samples = []
        self.memory_samples = []
        self.disk_samples = []
        self.network_samples = []
        self.monitoring = False
        self.monitor_thread = None
        
    def start_monitoring(self):
        """Start resource monitoring."""
        self.monitoring = True
        self.monitor_thread = threading.Thread(target=self._monitor_resources, daemon=True)
        self.monitor_thread.start()
        
    def _monitor_resources(self):
        """Background resource monitoring thread."""
        while self.monitoring:
            # CPU monitoring
            cpu_percent = psutil.cpu_percent(interval=None)
            self.cpu_samples.append({
                'timestamp': time.time(),
                'cpu_percent': cpu_percent,
                'cpu_count': psutil.cpu_count()
            })
            
            # Memory monitoring
            memory = psutil.virtual_memory()
            process = psutil.Process(os.getpid())
            process_memory = process.memory_info()
            
            self.memory_samples.append({
                'timestamp': time.time(),
                'system_memory_percent': memory.percent,
                'system_memory_available_gb': memory.available / 1024 / 1024 / 1024,
                'system_memory_used_gb': memory.used / 1024 / 1024 / 1024,
                'process_memory_rss_gb': process_memory.rss / 1024 / 1024 / 1024,
                'process_memory_vms_gb': process_memory.vms / 1024 / 1024 / 1024,
                'process_memory_percent': (process_memory.rss / psutil.virtual_memory().total) * 100
            })
            
            # Disk monitoring
            disk_io = psutil.disk_io_counters()
            self.disk_samples.append({
                'timestamp': time.time(),
                'disk_read_mb': disk_io.read_bytes / 1024 / 1024 if disk_io else 0,
                'disk_write_mb': disk_io.write_bytes / 1024 / 1024 if disk_io else 0,
                'disk_read_count': disk_io.read_count if disk_io else 0,
                'disk_write_count': disk_io.write_count if disk_io else 0
            })
            
            # Network monitoring
            net_io = psutil.net_io_counters()
            self.network_samples.append({
                'timestamp': time.time(),
                'net_sent_mb': net_io.bytes_sent / 1024 / 1024 if net_io else 0,
                'net_recv_mb': net_io.bytes_recv / 1024 / 1024 if net_io else 0,
                'net_sent_packets': net_io.packets_sent if net_io else 0,
                'net_recv_packets': net_io.packets_recv if net_io else 0
            })
            
            time.sleep(self.interval)
            
    def stop_monitoring(self):
        """Stop resource monitoring."""
        self.monitoring = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=5)
            
    def get_statistics(self) -> Dict[str, Any]:
        """Get resource usage statistics."""
        stats = {}
        
        if self.cpu_samples:
            cpu_percents = [s['cpu_percent'] for s in self.cpu_samples]
            stats['cpu'] = {
                'mean_percent': statistics.mean(cpu_percents),
                'max_percent': max(cpu_percents),
                'min_percent': min(cpu_percents),
                'std_percent': statistics.stdev(cpu_percents) if len(cpu_percents) > 1 else 0,
                'samples': len(cpu_percents)
            }
            
        if self.memory_samples:
            memory_usage = [s['process_memory_percent'] for s in self.memory_samples]
            stats['memory'] = {
                'mean_percent': statistics.mean(memory_usage),
                'max_percent': max(memory_usage),
                'min_percent': min(memory_usage),
                'std_percent': statistics.stdev(memory_usage) if len(memory_usage) > 1 else 0,
                'peak_rss_gb': max(s['process_memory_rss_gb'] for s in self.memory_samples),
                'avg_rss_gb': statistics.mean(s['process_memory_rss_gb'] for s in self.memory_samples)
            }
            
        if self.disk_samples:
            disk_read = [s['disk_read_mb'] for s in self.disk_samples]
            disk_write = [s['disk_write_mb'] for s in self.disk_samples]
            stats['disk'] = {
                'read_mb_per_sec': statistics.mean(disk_read) / self.interval if disk_read else 0,
                'write_mb_per_sec': statistics.mean(disk_write) / self.interval if disk_write else 0,
                'read_ops_per_sec': statistics.mean([s['disk_read_count'] for s in self.disk_samples]) / self.interval if self.disk_samples else 0,
                'write_ops_per_sec': statistics.mean([s['disk_write_count'] for s in self.disk_samples]) / self.interval if self.disk_samples else 0
            }
            
        if self.network_samples:
            net_sent = [s['net_sent_mb'] for s in self.network_samples]
            net_recv = [s['net_recv_mb'] for s in self.network_samples]
            stats['network'] = {
                'sent_mb_per_sec': statistics.mean(net_sent) / self.interval if net_sent else 0,
                'recv_mb_per_sec': statistics.mean(net_recv) / self.interval if net_recv else 0,
                'sent_packets_per_sec': statistics.mean([s['net_sent_packets'] for s in self.network_samples]) / self.interval if self.network_samples else 0,
                'recv_packets_per_sec': statistics.mean([s['net_recv_packets'] for s in self.network_samples]) / self.interval if self.network_samples else 0
            }
            
        return stats


class LoadTestResult:
    """Container for load test results."""
    
    def __init__(self):
        self.start_time = None
        self.end_time = None
        self.test_info = {}
        self.iterations = []
        self.resource_stats = {}
        self.throughput_data = []
        self.latency_data = []
        self.error_data = []
        
    def add_iteration(self, iteration_num: int, success: bool, duration: float,
                     memory_mb: float, cpu_percent: float):
        """Add a test iteration result."""
        self.iterations.append({
            'iteration': iteration_num,
            'success': success,
            'duration': duration,
            'memory_mb': memory_mb,
            'cpu_percent': cpu_percent,
            'timestamp': time.time()
        })
        
        if success:
            self.throughput_data.append(1.0 / duration if duration > 0 else 0)
            self.latency_data.append(duration * 1000)  # Convert to milliseconds
        else:
            self.error_data.append(iteration_num)
            
    def finalize(self, test_info: Dict[str, Any], resource_stats: Dict[str, Any]):
        """Finalize test results."""
        self.test_info = test_info
        self.resource_stats = resource_stats
        
        if self.start_time and self.end_time:
            self.test_info['total_duration'] = self.end_time - self.start_time
            
        # Calculate overall statistics
        if self.throughput_data:
            self.test_info['avg_throughput'] = statistics.mean(self.throughput_data)
            self.test_info['min_throughput'] = min(self.throughput_data)
            self.test_info['max_throughput'] = max(self.throughput_data)
            self.test_info['throughput_std'] = statistics.stdev(self.throughput_data) if len(self.throughput_data) > 1 else 0
            
        if self.latency_data:
            self.test_info['avg_latency_ms'] = statistics.mean(self.latency_data)
            self.test_info['min_latency_ms'] = min(self.latency_data)
            self.test_info['max_latency_ms'] = max(self.latency_data)
            self.test_info['p95_latency_ms'] = np.percentile(self.latency_data, 95) if self.latency_data else 0
            
        self.test_info['success_rate'] = 1 - (len(self.error_data) / len(self.iterations) if self.iterations else 0)
        self.test_info['total_iterations'] = len(self.iterations)
        self.test_info['failed_iterations'] = len(self.error_data)


class ConcurrentTestExecutor:
    """Execute concurrent load tests."""
    
    def __init__(self, simulation_func: Callable, max_workers: int = 10):
        self.simulation_func = simulation_func
        self.max_workers = max_workers
        self.results = []
        
    def run_concurrent_test(self, concurrent_users: int, duration: int) -> LoadTestResult:
        """Run concurrent test with specified number of users."""
        result = LoadTestResult()
        result.start_time = time.time()
        result.test_info = {
            'test_type': 'concurrent',
            'concurrent_users': concurrent_users,
            'duration': duration
        }
        
        # Create queue for tasks
        task_queue = queue.Queue()
        for i in range(concurrent_users):
            task_queue.put(i)
            
        def worker(worker_id: int) -> Dict[str, Any]:
            """Worker function for concurrent execution."""
            local_result = LoadTestResult()
            local_result.test_info = {
                'worker_id': worker_id,
                'start_time': time.time()
            }
            
            while not task_queue.empty():
                try:
                    iteration = task_queue.get_nowait()
                except queue.Empty:
                    break
                    
                iteration_start = time.time()
                success = False
                duration_ms = 0
                memory_mb = 0
                cpu_percent = 0
                
                try:
                    # Run simulation
                    sim_start = time.time()
                    simulation_result = self.simulation_func()
                    sim_duration = time.time() - sim_start
                    
                    # Collect metrics
                    process = psutil.Process(os.getpid())
                    memory_info = process.memory_info()
                    memory_mb = memory_info.rss / 1024 / 1024
                    cpu_percent = process.cpu_percent()
                    
                    success = True
                    duration_ms = sim_duration * 1000
                    
                    local_result.add_iteration(iteration, success, sim_duration, memory_mb, cpu_percent)
                    
                except Exception as e:
                    # Record failure
                    local_result.add_iteration(iteration, False, 0, 0, 0)
                    
            local_result.end_time = time.time()
            return local_result
            
        # Execute concurrent tests
        with ThreadPoolExecutor(max_workers=self.max_workers) as executor:
            # Submit all tasks
            future_to_worker = {
                executor.submit(worker, i): i 
                for i in range(concurrent_users)
            }
            
            # Collect results
            for future in as_completed(future_to_worker):
                try:
                    worker_result = future.result()
                    self.results.append(worker_result)
                    
                    # Merge worker results into main result
                    for iteration in worker_result.iterations:
                        result.add_iteration(
                            iteration['iteration'],
                            iteration['success'],
                            iteration['duration'],
                            iteration['memory_mb'],
                            iteration['cpu_percent']
                        )
                        
                except Exception as e:
                    print(f"Worker error: {e}")
                    
        result.end_time = time.time()
        return result


class MemoryStressTester:
    """Test memory stress scenarios."""
    
    def __init__(self, simulation_func: Callable):
        self.simulation_func = simulation_func
        self.results = []
        
    def run_memory_stress_test(self, memory_gb: float, duration: int, 
                              target_requests: Optional[int] = None) -> LoadTestResult:
        """Run memory stress test."""
        result = LoadTestResult()
        result.start_time = time.time()
        result.test_info = {
            'test_type': 'memory_stress',
            'memory_gb': memory_gb,
            'duration': duration,
            'target_requests': target_requests
        }
        
        # Simulate memory pressure
        allocated_objects = []
        
        iteration = 0
        while (time.time() - result.start_time) < duration:
            iteration_start = time.time()
            success = False
            duration_ms = 0
            memory_mb = 0
            cpu_percent = 0
            
            try:
                # Allocate memory to simulate pressure
                if memory_gb > 0:
                    # Allocate approximately memory_gb amount of memory
                    target_bytes = int(memory_gb * 1024 * 1024 * 1024)
                    allocated_chunk = []
                    
                    # Fill with objects until target is reached
                    while len(str(allocated_chunk)) * 50 < target_bytes and (time.time() - iteration_start) < 0.1:
                        allocated_chunk.append([random.random() for _ in range(1000)])
                        
                    allocated_objects.append(allocated_chunk)
                    
                    # Trigger garbage collection occasionally
                    if iteration % 50 == 0:
                        gc.collect()
                        
                # Run simulation
                sim_start = time.time()
                simulation_result = self.simulation_func()
                sim_duration = time.time() - sim_start
                
                # Collect metrics
                process = psutil.Process(os.getpid())
                memory_info = process.memory_info()
                memory_mb = memory_info.rss / 1024 / 1024
                cpu_percent = process.cpu_percent()
                
                success = True
                duration_ms = sim_duration * 1000
                
                result.add_iteration(iteration, success, sim_duration, memory_mb, cpu_percent)
                
                iteration += 1
                
                # Check target requests
                if target_requests and iteration >= target_requests:
                    break
                    
            except Exception as e:
                print(f"Iteration {iteration} error: {e}")
                result.add_iteration(iteration, False, 0, 0, 0)
                iteration += 1
                
        result.end_time = time.time()
        return result


class ParallelProcessingTester:
    """Test parallel processing performance."""
    
    def __init__(self, simulation_func: Callable):
        self.simulation_func = simulation_func
        self.results = []
        
    def run_parallel_test(self, parallel_workers: int, batch_size: int) -> LoadTestResult:
        """Run parallel processing test."""
        result = LoadTestResult()
        result.start_time = time.time()
        result.test_info = {
            'test_type': 'parallel_processing',
            'parallel_workers': parallel_workers,
            'batch_size': batch_size
        }
        
        # Create batches
        batches = []
        for i in range(0, parallel_workers * batch_size, batch_size):
            batches.append(list(range(i, min(i + batch_size, parallel_workers * batch_size))))
            
        def process_batch(batch_id: int, batch: List[int]) -> Dict[str, Any]:
            """Process a batch of tasks."""
            local_result = LoadTestResult()
            local_result.test_info = {
                'batch_id': batch_id,
                'batch_size': len(batch)
            }
            
            for item in batch:
                iteration_start = time.time()
                success = False
                duration_ms = 0
                memory_mb = 0
                cpu_percent = 0
                
                try:
                    # Simulate parallel work
                    time.sleep(random.uniform(0.001, 0.01))
                    
                    # Run simulation
                    sim_start = time.time()
                    simulation_result = self.simulation_func(item)
                    sim_duration = time.time() - sim_start
                    
                    # Collect metrics
                    process = psutil.Process(os.getpid())
                    memory_info = process.memory_info()
                    memory_mb = memory_info.rss / 1024 / 1024
                    cpu_percent = process.cpu_percent()
                    
                    success = True
                    duration_ms = sim_duration * 1000
                    
                    local_result.add_iteration(item, success, sim_duration, memory_mb, cpu_percent)
                    
                except Exception as e:
                    local_result.add_iteration(item, False, 0, 0, 0)
                    
            local_result.end_time = time.time()
            return local_result
            
        # Execute parallel tests
        with ProcessPoolExecutor(max_workers=parallel_workers) as executor:
            # Submit all batches
            future_to_batch = {
                executor.submit(process_batch, i, batch): (i, batch)
                for i, batch in enumerate(batches)
            }
            
            # Collect results
            for future in as_completed(future_to_batch):
                try:
                    batch_id, batch = future_to_batch[future]
                    batch_result = future.result()
                    self.results.append(batch_result)
                    
                    # Merge batch results
                    for iteration in batch_result.iterations:
                        result.add_iteration(
                            iteration['iteration'],
                            iteration['success'],
                            iteration['duration'],
                            iteration['memory_mb'],
                            iteration['cpu_percent']
                        )
                        
                except Exception as e:
                    print(f"Batch error: {e}")
                    
        result.end_time = time.time()
        return result


class LoadTester:
    """Main load testing tool for NLM brain simulations."""
    
    def __init__(self):
        self.resource_monitor = ResourceMonitor()
        
    def run_concurrent_test(self, simulation_func: Callable, concurrent_users: int, 
                          duration: int, monitor: bool = True) -> LoadTestResult:
        """Run concurrent load test.
        
        Args:
            simulation_func: Simulation function to test
            concurrent_users: Number of concurrent users/requests
            duration: Test duration in seconds
            monitor: Enable resource monitoring
            
        Returns:
            Load test results
        """
        print(f"Starting concurrent test with {concurrent_users} users for {duration} seconds")
        
        # Start resource monitoring if enabled
        if monitor:
            self.resource_monitor.start_monitoring()
            
        # Run concurrent test
        executor = ConcurrentTestExecutor(simulation_func)
        result = executor.run_concurrent_test(concurrent_users, duration)
        
        # Stop resource monitoring
        if monitor:
            self.resource_monitor.stop_monitoring()
            result.resource_stats = self.resource_monitor.get_statistics()
            
        result.finalize(result.test_info, result.resource_stats)
        return result
        
    def run_memory_stress_test(self, simulation_func: Callable, memory_gb: float, 
                             duration: int, target_requests: Optional[int] = None,
                             monitor: bool = True) -> LoadTestResult:
        """Run memory stress test.
        
        Args:
            simulation_func: Simulation function to test
            memory_gb: Memory pressure in GB
            duration: Test duration in seconds
            target_requests: Target number of requests (optional)
            monitor: Enable resource monitoring
            
        Returns:
            Load test results
        """
        print(f"Starting memory stress test with {memory_gb}GB pressure for {duration} seconds")
        
        # Start resource monitoring if enabled
        if monitor:
            self.resource_monitor.start_monitoring()
            
        # Run memory stress test
        tester = MemoryStressTester(simulation_func)
        result = tester.run_memory_stress_test(memory_gb, duration, target_requests)
        
        # Stop resource monitoring
        if monitor:
            self.resource_monitor.stop_monitoring()
            result.resource_stats = self.resource_monitor.get_statistics()
            
        result.finalize(result.test_info, result.resource_stats)
        return result
        
    def run_parallel_test(self, simulation_func: Callable, parallel_workers: int, 
                         batch_size: int, monitor: bool = True) -> LoadTestResult:
        """Run parallel processing test.
        
        Args:
            simulation_func: Simulation function to test
            parallel_workers: Number of parallel workers
            batch_size: Batch size for processing
            monitor: Enable resource monitoring
            
        Returns:
            Load test results
        """
        print(f"Starting parallel test with {parallel_workers} workers, batch size {batch_size}")
        
        # Start resource monitoring if enabled
        if monitor:
            self.resource_monitor.start_monitoring()
            
        # Run parallel test
        tester = ParallelProcessingTester(simulation_func)
        result = tester.run_parallel_test(parallel_workers, batch_size)
        
        # Stop resource monitoring
        if monitor:
            self.resource_monitor.stop_monitoring()
            result.resource_stats = self.resource_monitor.get_statistics()
            
        result.finalize(result.test_info, result.resource_stats)
        return result
        
    def generate_load_report(self, result: LoadTestResult, output_path: str = None,
                            report_type: str = "html") -> str:
        """Generate load testing report.
        
        Args:
            result: Load test results
            output_path: Output file path. If None, auto-generates.
            report_type: Report format ('html', 'json', 'csv')
            
        Returns:
            Path to generated report
        """
        if output_path is None:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            output_path = f"load_test_report_{timestamp}.{report_type}"
            
        if report_type == "html":
            self._generate_html_report(result, output_path)
        elif report_type == "json":
            self._generate_json_report(result, output_path)
        elif report_type == "csv":
            self._generate_csv_report(result, output_path)
            
        return output_path
        
    def _generate_html_report(self, result: LoadTestResult, output_path: str):
        """Generate HTML load testing report."""
        html_content = f"""
<!DOCTYPE html>
<html>
<head>
    <title>NLM Load Testing Report</title>
    <style>
        body {{ font-family: Arial, sans-serif; margin: 40px; line-height: 1.6; }}
        .header {{ background-color: #ff6b6b; color: white; padding: 30px; border-radius: 5px; }}
        .section {{ margin: 30px 0; padding: 20px; border: 1px solid #ddd; border-radius: 5px; }}
        .metric-grid {{ display: grid; grid-template-columns: repeat(auto-fit, minmax(250px, 1fr)); gap: 20px; }}
        .metric-card {{ background: #f8f9fa; padding: 15px; border-radius: 5px; border-left: 4px solid #ff6b6b; }}
        .metric-value {{ font-size: 24px; font-weight: bold; color: #ff6b6b; }}
        .metric-label {{ color: #6c757d; font-size: 14px; }}
        .warning {{ background-color: #fff3cd; border: 1px solid #ffeaa7; padding: 15px; border-radius: 5px; margin: 10px 0; }}
        .success {{ background-color: #d4edda; border: 1px solid #c3e6cb; }}
        table {{ width: 100%; border-collapse: collapse; margin: 20px 0; }}
        th, td {{ border: 1px solid #ddd; padding: 12px; text-align: left; }}
        th {{ background-color: #f2f2f2; }}
        canvas {{ max-width: 100%; height: auto; }}
    </style>
</head>
<body>
    <div class="header">
        <h1>NLM Load Testing Report</h1>
        <p>Generated: {datetime.now().isoformat()}</p>
        <p>Test Type: {result.test_info.get('test_type', 'unknown')}</p>
        <p>Duration: {result.test_info.get('duration', 0)} seconds</p>
    </div>
    
    <div class="section">
        <h2>Performance Summary</h2>
        <div class="metric-grid">
            <div class="metric-card">
                <div class="metric-value">{result.test_info.get('total_iterations', 0)}</div>
                <div class="metric-label">Total Iterations</div>
            </div>
            <div class="metric-card">
                <div class="metric-value">{result.test_info.get('success_rate', 0):.1%}</div>
                <div class="metric-label">Success Rate</div>
            </div>
            <div class="metric-card">
                <div class="metric-value">{result.test_info.get('avg_throughput', 0):.2f}</div>
                <div class="metric-label">Avg Throughput (req/sec)</div>
            </div>
            <div class="metric-card">
                <div class="metric-value">{result.test_info.get('avg_latency_ms', 0):.2f}</div>
                <div class="metric-label">Avg Latency (ms)</div>
            </div>
        </div>
    </div>
    
    <div class="section">
        <h2>Detailed Metrics</h2>
        <table>
            <tr>
                <th>Metric</th>
                <th>Value</th>
                <th>Threshold</th>
                <th>Status</th>
            </tr>
            <tr>
                <td>Success Rate</td>
                <td>{result.test_info.get('success_rate', 0):.1%}</td>
                <td>> 95%</td>
                <td>{'✅' if result.test_info.get('success_rate', 0) > 0.95 else '⚠️'}</td>
            </tr>
            <tr>
                <td>Average Throughput</td>
                <td>{result.test_info.get('avg_throughput', 0):.2f} req/sec</td>
                <td>> 10 req/sec</td>
                <td>{'✅' if result.test_info.get('avg_throughput', 0) > 10 else '⚠️'}</td>
            </tr>
            <tr>
                <td>Average Latency</td>
                <td>{result.test_info.get('avg_latency_ms', 0):.2f} ms</td>
                <td>< 100ms</td>
                <td>{'✅' if result.test_info.get('avg_latency_ms', 0) < 100 else '⚠️'}</td>
            </tr>
        </table>
    </div>
    
    <div class="section">
        <h2>Resource Utilization</h2>
    """
        
        if result.resource_stats:
            cpu_stats = result.resource_stats.get('cpu', {})
            memory_stats = result.resource_stats.get('memory', {})
            
            html_content += f"""
        <div class="metric-grid">
            <div class="metric-card">
                <div class="metric-value">{cpu_stats.get('max_percent', 0):.1f}%</div>
                <div class="metric-label">Max CPU Usage</div>
            </div>
            <div class="metric-card">
                <div class="metric-value">{memory_stats.get('max_percent', 0):.1f}%</div>
                <div class="metric-label">Max Memory Usage</div>
            </div>
            <div class="metric-card">
                <div class="metric-value">{memory_stats.get('peak_rss_gb', 0):.1f} GB</div>
                <div class="metric-label">Peak RSS Memory</div>
            </div>
            <div class="metric-card">
                <div class="metric-value">{cpu_stats.get('samples', 0)}</div>
                <div class="metric-label">Resource Samples</div>
            </div>
        </div>
            """
            
        html_content += """
    </div>
    
    <div class="section">
        <h2>Performance Charts</h2>
        <canvas id="throughputChart" width="800" height="300"></canvas>
        <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
        <script>
            const throughputData = {
                labels: ["""
        ]
        
        # Add throughput data points
        if result.throughput_data:
            for i, throughput in enumerate(result.throughput_data):
                html_content += f"'{datetime.fromtimestamp(result.iterations[i][\"timestamp\"] if i < len(result.iterations) else time.time()).strftime('%H:%M:%S')}', "
                
        html_content = html_content[:-2] + """]',
                    datasets: [{
                        label: 'Throughput (req/sec)',
                        data: ["""
        ]
        
        # Add throughput values
        for throughput in result.throughput_data:
            html_content += f"{throughput}, "
            
        html_content = html_content[:-2] + """],
                        backgroundColor: 'rgba(255, 99, 132, 0.2)',
                        borderColor: 'rgba(255, 99, 132, 1)',
                        borderWidth: 1
                    }]
                };
                
                const latencyData = {
                    labels: ["""
        ]
        
        # Add latency data points
        if result.latency_data:
            for i, latency in enumerate(result.latency_data):
                html_content += f"'{datetime.fromtimestamp(result.iterations[i][\"timestamp\"] if i < len(result.iterations) else time.time()).strftime('%H:%M:%S')}', "
                
        html_content = html_content[:-2] + """]',
                    datasets: [{
                        label: 'Latency (ms)',
                        data: ["""
        ]
        
        # Add latency values
        for latency in result.latency_data:
            html_content += f"{latency}, "
            
        html_content = html_content[:-2] + """],
                        backgroundColor: 'rgba(54, 162, 235, 0.2)',
                        borderColor: 'rgba(54, 162, 235, 1)',
                        borderWidth: 1
                    }]
                };
                
                function createChart(canvasId, data, title) {
                    const ctx = document.getElementById(canvasId).getContext('2d');
                    new Chart(ctx, {
                        type: 'line',
                        data: data,
                        options: {
                            responsive: true,
                            plugins: {
                                title: { display: true, text: title }
                            },
                            scales: {
                                y: { beginAtZero: true, title: { display: true, text: 'Value' } },
                                x: { title: { display: true, text: 'Time' } }
                            }
                        }
                    });
                }
                
                createChart('throughputChart', throughputData, 'Throughput Over Time');
                createChart('latencyChart', latencyData, 'Latency Over Time');
        </script>
        <canvas id="latencyChart" width="800" height="300"></canvas>
    </div>
    
</body>
</html>
        """
        
        with open(output_path, 'w') as f:
            f.write(html_content)
            
        print(f"Load testing report generated: {output_path}")
        
    def _generate_json_report(self, result: LoadTestResult, output_path: str):
        """Generate JSON load testing report."""
        report_data = {
            'timestamp': datetime.now().isoformat(),
            'test_info': result.test_info,
            'iterations': result.iterations,
            'resource_stats': result.resource_stats,
            'throughput_data': result.throughput_data,
            'latency_data': result.latency_data,
            'error_data': result.error_data
        }
        
        with open(output_path, 'w') as f:
            json.dump(report_data, f, indent=2, default=str)
            
        print(f"Load testing JSON report generated: {output_path}")
        
    def _generate_csv_report(self, result: LoadTestResult, output_path: str):
        """Generate CSV load testing report."""
        import csv
        
        with open(output_path, 'w', newline='') as f:
            writer = csv.writer(f)
            writer.writerow(['Iteration', 'Success', 'Duration', 'Memory_MB', 'CPU_Percent'])
            
            for iteration in result.iterations:
                writer.writerow([
                    iteration['iteration'],
                    iteration['success'],
                    iteration['duration'],
                    iteration['memory_mb'],
                    iteration['cpu_percent']
                ])
                
        print(f"Load testing CSV report generated: {output_path}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="NLM Brain Load Testing Tool")
    parser.add_argument("--concurrent", type=int, help="Concurrent users for load testing")
    parser.add_argument("--duration", type=int, help="Test duration in seconds")
    parser.add_argument("--memory-pressure", type=float, help="Memory pressure in GB for stress testing")
    parser.add_argument("--peak-requests", type=int, help="Target number of requests")
    parser.add_argument("--parallel", type=int, help="Number of parallel workers")
    parser.add_argument("--batch-size", type=int, help="Batch size for parallel processing")
    parser.add_argument("--load-levels", nargs="+", choices=["low", "medium", "high"], help="Load levels to test")n    parser.add_argument("--stress", action="store_true", help="Enable stress testing")
    parser.add_argument("--monitor-cpu", action="store_true", help="Enable CPU monitoring")
    parser.add_argument("--monitor-memory", action="store_true", help="Enable memory monitoring")
    parser.add_argument("--interval", type=float, default=1.0, help="Monitoring interval in seconds")
    parser.add_argument("--output", help="Output report file path")
    parser.add_argument("--format", choices=["html", "json", "csv"], default="html", help="Report format")
    
    args = parser.parse_args()
    
    print("Starting NLM Load Testing...")
    
    # Create mock simulation function for demonstration
    def mock_simulation_func(item=None):
        """Mock simulation function."""
        import time
        import random
        
        # Simulate variable workload
        base_time = random.uniform(0.01, 0.1)
        time.sleep(base_time)
        
        return {
            'item': item,
            'timestamp': datetime.now().isoformat(),
            'completed': True
        }
    
    # Create load tester
    tester = LoadTester()
    
    # Run tests based on arguments
    if args.concurrent and args.duration:
        print(f"\nRunning concurrent test with {args.concurrent} users for {args.duration} seconds")
        result = tester.run_concurrent_test(
            mock_simulation_func, 
            concurrent_users=args.concurrent, 
            duration=args.duration,
            monitor=args.monitor_cpu or args.monitor_memory
        )
        output_file = tester.generate_load_report(result, args.output, args.format)
        print(f"Concurrent test complete. Report saved to: {output_file}")
        
    elif args.memory_pressure:
        print(f"\nRunning memory stress test with {args.memory_pressure}GB pressure")
        result = tester.run_memory_stress_test(
            mock_simulation_func,
            memory_gb=args.memory_pressure,
            duration=args.duration or 300,
            target_requests=args.peak_requests,
            monitor=args.monitor_cpu or args.monitor_memory
        )
        output_file = tester.generate_load_report(result, args.output, args.format)
        print(f"Memory stress test complete. Report saved to: {output_file}")
        
    elif args.parallel and args.batch_size:
        print(f"\nRunning parallel processing test with {args.parallel} workers, batch size {args.batch_size}")
        result = tester.run_parallel_test(
            mock_simulation_func,
            parallel_workers=args.parallel,
            batch_size=args.batch_size,
            monitor=args.monitor_cpu or args.monitor_memory
        )
        output_file = tester.generate_load_report(result, args.output, args.format)
        print(f"Parallel test complete. Report saved to: {output_file}")
        
    elif args.load_levels:
        print(f"\nRunning load level tests: {args.load_levels}")
        all_results = []
        
        for level in args.load_levels:
            print(f"  Testing {level} load level...")
            
            # Set parameters based on load level
            if level == "low":
                users = 10
                duration = 60
            elif level == "medium":
                users = 50
                duration = 120
            elif level == "high":
                users = 100
                duration = 180
                
            result = tester.run_concurrent_test(
                mock_simulation_func,
                concurrent_users=users,
                duration=duration,
                monitor=args.monitor_cpu or args.monitor_memory
            )
            
            result.test_info['load_level'] = level
            all_results.append(result)
            
        # Generate combined report
        combined_output = f"combined_load_test_report_{datetime.now().strftime('%Y%m%d_%H%M%S')}.html"
        print(f"\nAll load level tests complete. Combined report saved to: {combined_output}")
        
    elif args.stress:
        print(f"\nRunning comprehensive stress test")
        
        # Run multiple test types
        stress_results = {}
        
        print("  Running concurrent stress test...")
        stress_results['concurrent'] = tester.run_concurrent_test(
            mock_simulation_func,
            concurrent_users=50,
            duration=180,
            monitor=True
        )
        
        print("  Running memory stress test...")
        stress_results['memory'] = tester.run_memory_stress_test(
            mock_simulation_func,
            memory_gb=8,
            duration=300,
            monitor=True
        )
        
        print("  Running parallel processing test...")
        stress_results['parallel'] = tester.run_parallel_test(
            mock_simulation_func,
            parallel_workers=20,
            batch_size=10,
            monitor=True
        )
        
        print("\nStress testing complete!")
        
    else:
        print("Load testing requires one of the following:")
        print("  --concurrent <users> --duration <seconds>")
        print("  --memory-pressure <GB> [--duration <seconds>]")
        print("  --parallel <workers> --batch-size <size>")
        print("  --load-levels low medium high")
        print("  --stress (runs all test types)")
        print("\nExample:")
        print("  python load_tester.py --concurrent 50 --duration 300")
        print("  python load_tester.py --memory-pressure 8")
        print("  python load_tester.py --stress")
        
    print("\nLoad testing complete!")