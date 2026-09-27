"""
Real-time Monitoring Examples for NLM

This module provides examples of using the NLM performance monitoring
and optimization tools for real-time monitoring of neural simulations.
"""

import time
import sys
import os
from pathlib import Path
from typing import Dict, List, Optional, Callable
import json
import threading
import signal
from datetime import datetime

# Add the parent directory to the path so we can import NLM modules
sys.path.insert(0, str(Path(__file__).parent.parent))

try:
    from nlm.performance import (
        PerformanceMonitor, MemoryTracker, CPUMonitor, StepProfiler,
        ResourceOptimizer, PerformanceReporter, PerformanceProfiler
    )
    NLM_AVAILABLE = True
except ImportError:
    NLM_AVAILABLE = False
    print("NLM modules not available. Running in demonstration mode.")

class RealTimeMonitor:
    """Real-time monitoring class for NLM simulations."""
    
    def __init__(self, simulation_name: str):
        self.simulation_name = simulation_name
        self.is_running = False
        self.monitoring_thread = None
        self.update_interval = 1.0  # Update interval in seconds
        self.metrics_history = []
        self.alerts = []
        
        # Initialize NLM performance monitoring components if available
        if NLM_AVAILABLE:
            self.performance_monitor = PerformanceMonitor()
            self.memory_tracker = MemoryTracker()
            self.cpu_monitor = CPUMonitor()
            self.step_profiler = StepProfiler()
            self.resource_optimizer = ResourceOptimizer()
            self.performance_reporter = PerformanceReporter()
            self.performance_profiler = PerformanceProfiler()
            
            # Start monitoring
            self.performance_monitor.start()
            self.memory_tracker.start()
            self.cpu_monitor.start()
            self.step_profiler.start()
            self.resource_optimizer.start()
            
            # Add callbacks for real-time updates
            self.performance_monitor.addMetricCallback(self._on_metric_update)
            self.performance_monitor.addAlertCallback(self._on_alert)
            self.memory_tracker.addMemoryAlertCallback(self._on_memory_alert)
            self.cpu_monitor.addCPUAlertCallback(self._on_cpu_alert)
            self.resource_optimizer.addOptimizationCallback(self._on_optimization_suggestion)
    
    def start_monitoring(self):
        """Start real-time monitoring."""
        if self.is_running:
            return
        
        self.is_running = True
        self.monitoring_thread = threading.Thread(target=self._monitoring_loop, daemon=True)
        self.monitoring_thread.start()
        
        print(f"Starting real-time monitoring for simulation: {self.simulation_name}")
        print(f"Update interval: {self.update_interval} seconds")
    
    def stop_monitoring(self):
        """Stop real-time monitoring."""
        if not self.is_running:
            return
        
        self.is_running = False
        if self.monitoring_thread:
            self.monitoring_thread.join(timeout=5.0)
        
        # Stop monitoring components
        if NLM_AVAILABLE:
            self.performance_monitor.stop()
            self.memory_tracker.stop()
            self.cpu_monitor.stop()
            self.step_profiler.stop()
            self.resource_optimizer.stop()
        
        print(f"\nReal-time monitoring stopped for simulation: {self.simulation_name}")
    
    def _monitoring_loop(self):
        """Main monitoring loop."""
        while self.is_running:
            try:
                # Record metrics
                self._record_current_metrics()
                
                # Print dashboard
                self._print_dashboard()
                
                # Sleep for update interval
                time.sleep(self.update_interval)
                
            except Exception as e:
                print(f"Monitoring error: {e}")
                time.sleep(1.0)
    
    def _record_current_metrics(self):
        """Record current metrics for monitoring."""
        timestamp = datetime.now()
        
        # Get current metrics from NLM components if available
        if NLM_AVAILABLE:
            try:
                resource_allocation = self.performance_monitor.getResourceAllocation()
                throughput_metrics = self.performance_monitor.getThroughputMetrics()
                memory_stats = self.memory_tracker.getCurrentMemoryStats()
                cpu_usage = self.cpu_monitor.getOverallCPUUsage()
                
                # Get performance statistics
                memory_stats_metric = self.performance_monitor.getPerformanceStats("memory_usage")
                cpu_stats_metric = self.performance_monitor.getPerformanceStats("cpu_usage")
                step_stats_metric = self.performance_monitor.getPerformanceStats("step_time")
                
                # Create metrics record
                metrics = {
                    'timestamp': timestamp,
                    'memory_allocated': resource_allocation.memoryAllocated,
                    'memory_used': resource_allocation.memoryUsed,
                    'memory_peak': resource_allocation.memoryPeak,
                    'cpu_usage_percentage': resource_allocation.cpuUsagePercentage,
                    'cpu_cores_used': resource_allocation.cpuCoresUsed,
                    'spikes_per_second': throughput_metrics.spikesPerSecond,
                    'neurons_per_second': throughput_metrics.neuronsPerSecond,
                    'real_time_factor': throughput_metrics.realTimeFactor,
                    'total_steps': throughput_metrics.totalSteps,
                    'total_spikes': throughput_metrics.totalSpikes,
                    'memory_usage_mean': memory_stats_metric.mean,
                    'memory_usage_stddev': memory_stats_metric.stddev,
                    'cpu_usage_mean': cpu_stats_metric.mean,
                    'cpu_usage_stddev': cpu_stats_metric.stddev,
                    'step_time_mean': step_stats_metric.mean,
                    'step_time_stddev': step_stats_metric.stddev,
                    'current_step': self.performance_monitor.getCurrentStep(),
                    'progress_percentage': self.performance_monitor.getProgressPercentage()
                }
                
                self.metrics_history.append(metrics)
                
                # Keep only last hour of history
                cutoff_time = timestamp.replace(hour=timestamp.hour - 1)
                self.metrics_history = [m for m in self.metrics_history 
                                      if m['timestamp'] >= cutoff_time]
                
            except Exception as e:
                print(f"Error recording metrics: {e}")
    
    def _print_dashboard(self):
        """Print real-time dashboard."""
        clear_screen = "cls" if sys.platform == "win32" else "clear"
        os.system(clear_screen)
        
        print(f"=" * 60)
        print(f"NLM Real-Time Monitoring Dashboard")
        print(f"Simulation: {self.simulation_name}")
        print(f"Monitoring since: {datetime.now()}")
        print(f"Update interval: {self.update_interval} seconds")
        print(f"Metrics history: {len(self.metrics_history)} data points")
        print(f"Alerts: {len(self.alerts)}")
        print(f"=" * 60)
        
        if not self.metrics_history:
            print("\nNo metrics data available. Start the simulation to monitor.")
            return
        
        # Get latest metrics
        latest = self.metrics_history[-1]
        
        print(f"\nCURRENT METRICS:")
        print(f"  Step: {latest.get('current_step', 'N/A')}")
        print(f"  Progress: {latest.get('progress_percentage', 0):.1f}%")
        print(f"  Memory Usage: {latest.get('memory_used', 0)/(1024*1024):.1f} MB")
        print(f"  Memory Peak: {latest.get('memory_peak', 0)/(1024*1024):.1f} MB")
        print(f"  CPU Usage: {latest.get('cpu_usage_percentage', 0):.1f}%")
        print(f"  CPU Cores: {latest.get('cpu_cores_used', 0)}")
        print(f"  Spikes/sec: {latest.get('spikes_per_second', 0)}")
        print(f"  Neurons/sec: {latest.get('neurons_per_second', 0)}")
        print(f"  Real-time Factor: {latest.get('real_time_factor', 0):.2f}")
        print(f"  Step Time (mean): {latest.get('step_time_mean', 0):.4f} sec")
        
        # Show alerts if any
        if self.alerts:
            print(f"\nRECENT ALERTS ({len(self.alerts)}):")
            for alert in self.alerts[-5:]:  # Show last 5 alerts
                print(f"  [{alert['timestamp']}] {alert['type']}: {alert['message']}")
        
        # Show performance trends
        if len(self.metrics_history) >= 10:
            print(f"\nPERFORMANCE TRENDS:")
            
            # Calculate trends over last 5 minutes
            recent_metrics = [m for m in self.metrics_history 
                             if (datetime.now() - m['timestamp']).total_seconds() <= 300]
            
            if len(recent_metrics) >= 2:
                # Memory trend
                memory_trend = recent_metrics[-1]['memory_used'] - recent_metrics[0]['memory_used']
                if memory_trend > 10 * 1024 * 1024:  # 10 MB
                    print(f"  Memory usage is increasing ({memory_trend/(1024*1024):.1f} MB)")
                
                # CPU trend
                cpu_trend = recent_metrics[-1]['cpu_usage_percentage'] - recent_metrics[0]['cpu_usage_percentage']
                if cpu_trend > 5.0:
                    print(f"  CPU usage is increasing ({cpu_trend:.1f}%)")
                
                # Throughput trend
                spikes_trend = recent_metrics[-1]['spikes_per_second'] - recent_metrics[0]['spikes_per_second']
                if spikes_trend < -10:
                    print(f"  Throughput is decreasing ({spikes_trend:.1f} spikes/sec)")
    
    def _on_metric_update(self, metric):
        """Callback for metric updates."""
        # This is called when a new metric is recorded
        pass
    
    def _on_alert(self, alert):
        """Callback for performance alerts."""
        self.alerts.append({
            'timestamp': datetime.now(),
            'type': 'performance',
            'message': f"{alert.level}: {alert.name} - {alert.description}"
        })
    
    def _on_memory_alert(self, alert):
        """Callback for memory alerts."""
        self.alerts.append({
            'timestamp': datetime.now(),
            'type': 'memory',
            'message': f"{alert.severity}: {alert.description}"
        })
    
    def _on_cpu_alert(self, alert):
        """Callback for CPU alerts."""
        self.alerts.append({
            'timestamp': datetime.now(),
            'type': 'cpu',
            'message': f"{alert.severity}: {alert.description}"
        })
    
    def _on_optimization_suggestion(self, suggestion):
        """Callback for optimization suggestions."""
        self.alerts.append({
            'timestamp': datetime.now(),
            'type': 'optimization',
            'message': f"Suggestion: {suggestion.description}"
        })
    
    def get_dashboard_data(self) -> Dict:
        """Get current dashboard data for external visualization."""
        if not self.metrics_history:
            return {}
        
        latest = self.metrics_history[-1]
        
        return {
            'timestamp': latest['timestamp'],
            'metrics': {
                'memory_usage': {
                    'allocated': latest['memory_allocated'],
                    'used': latest['memory_used'],
                    'peak': latest['memory_peak']
                },
                'cpu': {
                    'usage_percentage': latest['cpu_usage_percentage'],
                    'cores_used': latest['cpu_cores_used']
                },
                'throughput': {
                    'spikes_per_second': latest['spikes_per_second'],
                    'neurons_per_second': latest['neurons_per_second'],
                    'real_time_factor': latest['real_time_factor']
                },
                'performance': {
                    'step_time_mean': latest['step_time_mean'],
                    'step_time_stddev': latest['step_time_stddev']
                },
                'simulation': {
                    'current_step': latest['current_step'],
                    'progress_percentage': latest['progress_percentage']
                }
            },
            'history': {
                'timestamps': [m['timestamp'] for m in self.metrics_history],
                'memory_used': [m['memory_used'] for m in self.metrics_history],
                'cpu_usage': [m['cpu_usage_percentage'] for m in self.metrics_history],
                'spikes_per_second': [m['spikes_per_second'] for m in self.metrics_history]
            }
        }
    
    def save_monitoring_data(self, filename: str):
        """Save monitoring data to file."""
        data = self.get_dashboard_data()
        
        if filename.endswith('.json'):
            with open(filename, 'w') as f:
                json.dump(data, f, indent=2, default=str)
        elif filename.endswith('.csv'):
            import csv
            with open(filename, 'w', newline='') as f:
                if self.metrics_history:
                    # Write header
                    header = ['timestamp', 'memory_allocated', 'memory_used', 'memory_peak',
                             'cpu_usage_percentage', 'cpu_cores_used', 'spikes_per_second',
                             'neurons_per_second', 'real_time_factor', 'step_time_mean',
                             'step_time_stddev', 'current_step', 'progress_percentage']
                    writer = csv.DictWriter(f, fieldnames=header)
                    writer.writeheader()
                    
                    # Write data
                    for metrics in self.metrics_history:
                        row = {
                            'timestamp': metrics['timestamp'],
                            'memory_allocated': metrics['memory_allocated'],
                            'memory_used': metrics['memory_used'],
                            'memory_peak': metrics['memory_peak'],
                            'cpu_usage_percentage': metrics['cpu_usage_percentage'],
                            'cpu_cores_used': metrics['cpu_cores_used'],
                            'spikes_per_second': metrics['spikes_per_second'],
                            'neurons_per_second': metrics['neurons_per_second'],
                            'real_time_factor': metrics['real_time_factor'],
                            'step_time_mean': metrics['step_time_mean'],
                            'step_time_stddev': metrics['step_time_stddev'],
                            'current_step': metrics['current_step'],
                            'progress_percentage': metrics['progress_percentage']
                        }
                        writer.writerow(row)
        
        print(f"Monitoring data saved to: {filename}")
    
    def generate_visualization_data(self, output_dir: str):
        """Generate visualization data for external tools."""
        data = self.get_dashboard_data()
        
        # Create JSON visualization data
        viz_data = {
            'title': f"NLM Real-Time Monitoring - {self.simulation_name}",
            'description': "Real-time monitoring data for neural simulation",
            'data': data,
            'metadata': {
                'generated_at': datetime.now(),
                'update_interval': self.update_interval,
                'duration_seconds': len(self.metrics_history) * self.update_interval
            }
        }
        
        viz_file = Path(output_dir) / f"visualization_{self.simulation_name}_{int(time.time())}.json"
        with open(viz_file, 'w') as f:
            json.dump(viz_data, f, indent=2, default=str)
        
        print(f"Visualization data saved to: {viz_file}")

class SimulationMonitor:
    """Simulation monitor that integrates with NLM performance monitoring."""
    
    def __init__(self, simulation_name: str):
        self.simulation_name = simulation_name
        self.monitor = None
        
        # Set up signal handlers
        signal.signal(signal.SIGINT, self._signal_handler)
        signal.signal(signal.SIGTERM, self._signal_handler)
    
    def start_monitoring(self, update_interval: float = 1.0):
        """Start monitoring the simulation."""
        self.monitor = RealTimeMonitor(self.simulation_name)
        self.monitor.update_interval = update_interval
        self.monitor.start_monitoring()
    
    def stop_monitoring(self):
        """Stop monitoring the simulation."""
        if self.monitor:
            self.monitor.stop_monitoring()
    
    def _signal_handler(self, signum, frame):
        """Signal handler for graceful shutdown."""
        print(f"\nReceived signal {signum}, stopping monitoring...")
        if self.monitor:
            self.monitor.stop_monitoring()
        sys.exit(0)

def simulate_neural_process(process_id: int, duration: float):
    """
    Simulate a neural process for testing real-time monitoring.
    
    This simulates a neural simulation process that generates
    performance metrics for monitoring.
    """
    print(f"Starting neural process {process_id} for {duration} seconds")
    
    start_time = time.time()
    step_count = 0
    
    while time.time() - start_time < duration:
        # Simulate neural computation
        step_count += 1
        
        # Simulate metrics generation
        step_time = 0.001 + (step_count % 10) * 0.001  # Varying step times
        spikes_count = step_count * 10  # Increasing spikes
        neurons_updated = step_count * 1000  # Increasing neurons
        
        # Simulate memory usage
        memory_usage = 100 * 1024 * 1024 + step_count * 1024 * 1024  # Increasing memory
        
        # Simulate CPU usage
        cpu_usage = 10.0 + (step_count % 20) * 5.0  # Varying CPU usage
        
        # Create metrics for monitoring
        metrics = {
            'timestamp': datetime.now(),
            'step_time': step_time,
            'spikes_count': spikes_count,
            'neurons_updated': neurons_updated,
            'memory_usage': memory_usage,
            'cpu_usage': cpu_usage,
            'simulation_id': process_id,
            'step_count': step_count
        }
        
        # Simulate processing time
        time.sleep(0.01)  # 10ms per step
    
    print(f"Completed neural process {process_id}: {step_count} steps")
    return step_count

def main():
    """Main function for real-time monitoring examples."""
    print("NLM Real-Time Monitoring Examples")
    print("=" * 50)
    
    # Create simulation monitor
    monitor = SimulationMonitor("Neural Simulation")
    
    # Start monitoring
    monitor.start_monitoring(update_interval=1.0)
    
    print(f"\nMonitoring neural simulation...")
    print(f"Press Ctrl+C to stop monitoring")
    
    # Start simulation processes
    processes = []
    for i in range(1, 4):
        process = threading.Thread(target=simulate_neural_process, args=(i, 30.0))
        processes.append(process)
        process.start()
        time.sleep(0.5)  # Stagger process start times
    
    # Wait for processes to complete
    for process in processes:
        process.join()
    
    # Stop monitoring
    monitor.stop_monitoring()
    
    print(f"\n{'='*50}")
    print("Real-time monitoring examples completed!")
    
    # Generate visualization data
    output_dir = "monitoring_output"
    os.makedirs(output_dir, exist_ok=True)
    
    if monitor.monitor:
        monitor.monitor.save_monitoring_data(f"{output_dir}/monitoring_data.csv")
        monitor.monitor.generate_visualization_data(output_dir)

if __name__ == "__main__":
    main()
