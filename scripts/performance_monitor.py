#!/usr/bin/env python3
"""
Performance monitoring and profiling tools for NLM Phase 6 brain simulation.
This script provides comprehensive performance analysis and monitoring capabilities.
"""

import subprocess
import time
import psutil
import json
import argparse
import os
from pathlib import Path
from typing import Dict, List, Optional
import threading
import signal
from dataclasses import dataclass
from datetime import datetime

@dataclass
class PerformanceMetrics:
    """Container for performance metrics."""
    timestamp: float
    cpu_usage: float
    memory_usage: float
    memory_peak: float
    spike_rate: float
    neuron_count: int
    synapse_count: int
    simulation_steps: int
    simulation_time: float
    wall_clock_time: float
    frames_per_second: float

class NLMPerformanceMonitor:
    """Performance monitoring for NLM brain simulation."""
    
    def __init__(self, process_name="nlm_phase6_demo"):
        self.process_name = process_name
        self.metrics_history: List[PerformanceMetrics] = []
        self.monitoring = False
        self.monitor_thread: Optional[threading.Thread] = None
        self.process: Optional[subprocess.Popen] = None
        
    def start_monitoring(self, simulation_time: int = 300, output_file: str = "performance.json"):
        """Start monitoring an NLM simulation process."""
        print(f"Starting performance monitoring for {simulation_time} seconds...")
        
        # Find and start the NLM process
        self.process = self._find_and_start_nlm_process()
        
        if not self.process:
            print("Error: Could not find or start NLM process")
            return False
            
        self.monitoring = True
        self.monitor_thread = threading.Thread(target=self._monitor_loop, 
                                              args=(simulation_time, output_file))
        self.monitor_thread.daemon = True
        self.monitor_thread.start()
        
        print("Performance monitoring started successfully")
        return True
        
    def _find_and_start_nlm_process(self) -> Optional[subprocess.Popen]:
        """Find existing NLM process or start a new one."""
        # First try to find existing process
        for proc in psutil.process_iter(['pid', 'name', 'cmdline']):
            try:
                if proc.info['cmdline'] and any('nlm_phase6_demo' in cmd for cmd in proc.info['cmdline']):
                    print(f"Found existing NLM process (PID: {proc.info['pid']})")
                    return psutil.Process(proc.info['pid'])
            except (psutil.NoSuchProcess, psutil.AccessDenied):
                continue
                
        # If no existing process, start a new one
        print("Starting new NLM process...")
        script_dir = Path(__file__).parent
        project_root = script_dir.parent
        
        # Build command with performance-friendly settings
        cmd = [
            "python3", str(project_root / "src" / "main.py"),
            "--max-steps", "1000",
            "--output-dir", str(project_root / "experiments" / "performance_test"),
            "--verbose"
        ]
        
        # Check if the binary exists and use it if available
        binary_path = str(project_root / "build" / "bin" / "nlm_phase6_demo")
        if os.path.exists(binary_path):
            cmd = [binary_path] + cmd[1:]
        
        return subprocess.Popen(cmd, 
                               stdout=subprocess.PIPE, 
                               stderr=subprocess.PIPE,
                               preexec_fn=os.setsid)
    
    def _monitor_loop(self, duration: int, output_file: str):
        """Continuous monitoring loop."""
        start_time = time.time()
        last_step_count = 0
        
        while self.monitoring and (time.time() - start_time) < duration:
            try:
                if self.process:
                    # Get process info
                    cpu_percent = self.process.cpu_percent()
                    memory_info = self.process.memory_info()
                    
                    # Get brain metrics from the process (simplified approach)
                    metrics = PerformanceMetrics(
                        timestamp=time.time(),
                        cpu_usage=cpu_percent,
                        memory_usage=memory_info.rss / (1024 * 1024),  # MB
                        memory_peak=memory_info.vms / (1024 * 1024),  # MB
                        spike_rate=0.0,  # Would need integration with brain metrics
                        neuron_count=0,  # Would need integration with brain metrics
                        synapse_count=0,  # Would need integration with brain metrics
                        simulation_steps=0,  # Would need integration with brain metrics
                        simulation_time=time.time() - start_time,
                        wall_clock_time=time.time() - start_time,
                        frames_per_second=0.0  # Would need simulation timing
                    )
                    
                    self.metrics_history.append(metrics)
                    
                    # Log current metrics
                    self._log_metrics(metrics)
                    
            except (psutil.NoSuchProcess, psutil.AccessDenied):
                print("NLM process terminated")
                break
            except Exception as e:
                print(f"Monitoring error: {e}")
                
            time.sleep(0.5)  # Monitor every 500ms
            
        # Stop monitoring and save results
        self.monitoring = False
        self._save_performance_data(output_file)
        
    def _log_metrics(self, metrics: PerformanceMetrics):
        """Log current performance metrics."""
        cpu_str = f"{metrics.cpu_usage:.1f}%"
        memory_str = f"{metrics.memory_usage:.1f}MB"
        print(f"[{datetime.now().strftime('%H:%M:%S')}] CPU: {cpu_str}, Memory: {memory_str}, "
              f"Time: {metrics.simulation_time:.1f}s, Steps: {metrics.simulation_steps}")
              
    def _save_performance_data(self, output_file: str):
        """Save performance metrics to JSON file."""
        if not self.metrics_history:
            print("No performance data to save")
            return
            
        # Convert to serializable format
        data = []
        for metrics in self.metrics_history:
            data.append({
                'timestamp': metrics.timestamp,
                'cpu_usage': metrics.cpu_usage,
                'memory_usage': metrics.memory_usage,
                'memory_peak': metrics.memory_peak,
                'spike_rate': metrics.spike_rate,
                'neuron_count': metrics.neuron_count,
                'synapse_count': metrics.synapse_count,
                'simulation_steps': metrics.simulation_steps,
                'simulation_time': metrics.simulation_time,
                'wall_clock_time': metrics.wall_clock_time,
                'frames_per_second': metrics.frames_per_second
            })
            
        # Save to file
        with open(output_file, 'w') as f:
            json.dump(data, f, indent=2)
            
        print(f"Performance data saved to {output_file}")
        
    def generate_report(self, output_file: str = "performance_report.md"):
        """Generate human-readable performance report."""
        if not self.metrics_history:
            print("No performance data available for report generation")
            return
            
        report_lines = [
            "# NLM Phase 6 Performance Report",
            f"Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}",
            "",
            "## Summary Statistics",
            "",
            f"Total monitoring duration: {self.metrics_history[-1].wall_clock_time:.1f} seconds",
            f"Peak CPU usage: {max(m.cpu_usage for m in self.metrics_history):.1f}%",
            f"Average CPU usage: {sum(m.cpu_usage for m in self.metrics_history) / len(self.metrics_history):.1f}%",
            f"Peak memory usage: {max(m.memory_usage for m in self.metrics_history):.1f} MB",
            f"Average memory usage: {sum(m.memory_usage for m in self.metrics_history) / len(self.metrics_history):.1f} MB",
            "",
            "## Performance Timeline",
            "",
            "| Time (s) | CPU (%) | Memory (MB) | Steps |",
            "|----------|---------|-------------|-------|",
        ]
        
        for metrics in self.metrics_history[::5]:  # Sample every 5th data point
            report_lines.append(
                f"| {metrics.wall_clock_time:.1f} | {metrics.cpu_usage:.1f} | "
                f"{metrics.memory_usage:.1f} | {metrics.simulation_steps} |"
            )
            
        # Write report
        with open(output_file, 'w') as f:
            f.write("\n".join(report_lines))
            
        print(f"Performance report generated: {output_file}")

def main():
    parser = argparse.ArgumentParser(description="NLM Performance Monitoring Tool")
    parser.add_argument("--start", action="store_true", help="Start monitoring an NLM process")
    parser.add_argument("--duration", type=int, default=300, help="Monitoring duration in seconds")
    parser.add_argument("--output", default="performance.json", help="Output file for performance data")
    parser.add_argument("--process", default="nlm_phase6_demo", help="Process name to monitor")
    parser.add_argument("--report", default="performance_report.md", help="Generate human-readable report")
    
    args = parser.parse_args()
    
    if args.start:
        monitor = NLMPerformanceMonitor(args.process)
        if monitor.start_monitoring(args.duration, args.output):
            print(f"Monitoring will run for {args.duration} seconds")
            print("Press Ctrl+C to stop early")
            
            # Keep main thread alive
            try:
                while monitor.monitoring:
                    time.sleep(1)
            except KeyboardInterrupt:
                print("\nMonitoring stopped by user")
                monitor.monitoring = False
                
        if args.report:
            monitor.generate_report(args.report)
    else:
        print("Use --start to begin monitoring")
        parser.print_help()

if __name__ == "__main__":
    main()
