# Phase 6 Performance Monitoring Tool
# Advanced performance monitoring for NLM Phase 6 neural simulations

import time
import threading
import sys
from typing import Dict, Any, List, Optional
from dataclasses import dataclass, field
from enum import Enum
import json
from pathlib import Path
from datetime import datetime
import os

class PerformanceMetric(Enum):
    CPU_USAGE = "cpu_usage"
    MEMORY_USAGE = "memory_usage"
    FIRING_RATE = "firing_rate"
    PLASTICITY_RATE = "plasticity_rate"
    SIMULATION_TIME = "simulation_time"
    INTEGRATION_SCORE = "integration_score"
    REWARD_PROGRESS = "reward_progress"
    NOVELTY_LEVEL = "novelty_level"
    DOPAMINE_LEVEL = "dopamine_level"

@dataclass
class PerformanceDataPoint:
    """A single data point in performance monitoring."""
    timestamp: float
    simulation_step: int
    metrics: Dict[PerformanceMetric, float] = field(default_factory=dict)
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            "timestamp": self.timestamp,
            "simulation_step": self.simulation_step,
            "metrics": {metric.value: value for metric, value in self.metrics.items()}
        }
    
    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> 'PerformanceDataPoint':
        point = cls(
            timestamp=data["timestamp"],
            simulation_step=data["simulation_step"]
        )
        point.metrics = {
            PerformanceMetric(metric_name): value 
            for metric_name, value in data.get("metrics", {}).items()
        }
        return point

@dataclass
class PerformanceThresholds:
    """Thresholds for triggering alerts."""
    high_cpu_threshold: float = 80.0
    high_memory_threshold: float = 90.0
    high_firing_threshold: float = 10.0
    low_reward_threshold: float = 0.0
    integration_failure_threshold: float = 0.5
    
    def to_dict(self) -> Dict[str, float]:
        return {
            "high_cpu_threshold": self.high_cpu_threshold,
            "high_memory_threshold": self.high_memory_threshold,
            "high_firing_threshold": self.high_firing_threshold,
            "low_reward_threshold": self.low_reward_threshold,
            "integration_failure_threshold": self.integration_failure_threshold
        }
    
    @classmethod
    def from_dict(cls, data: Dict[str, float]) -> 'PerformanceThresholds':
        return cls(**data)

@dataclass
class PerformanceAlert:
    """Performance alert/notification."""
    alert_type: str
    severity: str  # "INFO", "WARNING", "ERROR", "CRITICAL"
    message: str
    timestamp: float
    simulation_step: int
    metrics: Dict[PerformanceMetric, float] = field(default_factory=dict)
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            "alert_type": self.alert_type,
            "severity": self.severity,
            "message": self.message,
            "timestamp": self.timestamp,
            "simulation_step": self.simulation_step,
            "metrics": {metric.value: value for metric, value in self.metrics.items()}
        }

class Phase6PerformanceMonitor:
    """Advanced performance monitoring for Phase 6 neural simulations."""
    
    def __init__(self, config: Dict[str, Any] = None):
        self.config = config or {}
        self.monitoring_enabled = self.config.get("enable_monitoring", True)
        self.interval = self.config.get("monitoring_interval", 1000)
        self.thresholds = PerformanceThresholds.from_dict(
            self.config.get("thresholds", {})
        )
        
        self.data_points: List[PerformanceDataPoint] = []
        self.alerts: List[PerformanceAlert] = []
        self.monitoring_thread: Optional[threading.Thread] = None
        self.stop_monitoring = False
        
        # Performance tracking
        self.start_time = time.time()
        self.total_steps = 0
        self.current_step = 0
        self.performance_history: List[Dict[str, float]] = []
        
    def start_monitoring(self, brain_simulation_func):
        """Start background performance monitoring."""
        if not self.monitoring_enabled:
            return
        
        self.stop_monitoring = False
        self.monitoring_thread = threading.Thread(
            target=self._monitoring_loop,
            args=(brain_simulation_func,),
            daemon=True
        )
        self.monitoring_thread.start()
    
    def stop_monitoring(self):
        """Stop performance monitoring."""
        self.stop_monitoring = True
        if self.monitoring_thread:
            self.monitoring_thread.join(timeout=5.0)
            self.monitoring_thread = None
    
    def _monitoring_loop(self, brain_simulation_func):
        """Main monitoring loop."""
        while not self.stop_monitoring:
            time.sleep(0.1)  # Check every 100ms
            
            if self.current_step % self.interval == 0 and self.current_step > 0:
                self._collect_performance_data(brain_simulation_func)
    
    def _collect_performance_data(self, brain_simulation_func):
        """Collect performance data at current simulation step."""
        try:
            # Get system metrics
            system_metrics = self._get_system_metrics()
            
            # Get simulation-specific metrics
            simulation_metrics = self._get_simulation_metrics(brain_simulation_func)
            
            # Combine metrics
            all_metrics = {**system_metrics, **simulation_metrics}
            
            # Create data point
            data_point = PerformanceDataPoint(
                timestamp=time.time(),
                simulation_step=self.current_step,
                metrics=all_metrics
            )
            
            self.data_points.append(data_point)
            
            # Check for alerts
            self._check_alerts(data_point)
            
            # Trim old data to prevent memory issues
            if len(self.data_points) > 10000:
                self.data_points = self.data_points[-5000:]
                
        except Exception as e:
            # Log error but don't crash monitoring
            print(f"Performance monitoring error: {e}")
    
    def _get_system_metrics(self) -> Dict[PerformanceMetric, float]:
        """Get system-level performance metrics."""
        metrics = {}
        
        try:
            # CPU usage (simplified - on real systems would use psutil)
            import os
            metrics[PerformanceMetric.CPU_USAGE] = os.getloadavg()[0] * 100 / os.cpu_count()
            
            # Memory usage (simplified - on real systems would use psutil)
            import resource
            memory_usage = resource.getrusage(resource.RUSAGE_SELF).ru_maxrss
            metrics[PerformanceMetric.MEMORY_USAGE] = (memory_usage / 1024 / 1024) * 100  # Percentage
            
        except ImportError:
            # Fallback values if psutil not available
            metrics[PerformanceMetric.CPU_USAGE] = 0.0
            metrics[PerformanceMetric.MEMORY_USAGE] = 0.0
        
        return metrics
    
    def _get_simulation_metrics(self, brain_simulation_func) -> Dict[PerformanceMetric, float]:
        """Get simulation-specific performance metrics."""
        metrics = {}
        
        try:
            # Simulation time
            elapsed_time = time.time() - self.start_time
            metrics[PerformanceMetric.SIMULATION_TIME] = elapsed_time
            
            # Get simulation-specific metrics via callback
            if callable(brain_simulation_func):
                sim_metrics = brain_simulation_func()
                if isinstance(sim_metrics, dict):
                    for key, value in sim_metrics.items():
                        try:
                            metric_enum = PerformanceMetric(key)
                            metrics[metric_enum] = value
                        except ValueError:
                            pass  # Unknown metric, ignore
            
        except Exception as e:
            print(f"Error collecting simulation metrics: {e}")
        
        return metrics
    
    def _check_alerts(self, data_point: PerformanceDataPoint):
        """Check if any performance alerts should be triggered."""
        alerts_to_check = []
        
        # Check CPU threshold
        if PerformanceMetric.CPU_USAGE in data_point.metrics:
            cpu_usage = data_point.metrics[PerformanceMetric.CPU_USAGE]
            if cpu_usage > self.thresholds.high_cpu_threshold:
                alerts_to_check.append((
                    "HIGH_CPU_USAGE",
                    "WARNING" if cpu_usage < 95 else "ERROR",
                    f"CPU usage is high: {cpu_usage:.1f}%",
                    data_point.timestamp,
                    data_point.simulation_step,
                    {PerformanceMetric.CPU_USAGE: cpu_usage}
                ))
        
        # Check memory threshold
        if PerformanceMetric.MEMORY_USAGE in data_point.metrics:
            memory_usage = data_point.metrics[PerformanceMetric.MEMORY_USAGE]
            if memory_usage > self.thresholds.high_memory_threshold:
                alerts_to_check.append((
                    "HIGH_MEMORY_USAGE",
                    "WARNING" if memory_usage < 98 else "ERROR",
                    f"Memory usage is high: {memory_usage:.1f}%",
                    data_point.timestamp,
                    data_point.simulation_step,
                    {PerformanceMetric.MEMORY_USAGE: memory_usage}
                ))
        
        # Check firing rate threshold
        if PerformanceMetric.FIRING_RATE in data_point.metrics:
            firing_rate = data_point.metrics[PerformanceMetric.FIRING_RATE]
            if firing_rate > self.thresholds.high_firing_threshold:
                alerts_to_check.append((
                    "HIGH_FIRING_RATE",
                    "INFO",
                    f"Firing rate is high: {firing_rate:.1f}%",
                    data_point.timestamp,
                    data_point.simulation_step,
                    {PerformanceMetric.FIRING_RATE: firing_rate}
                ))
        
        # Create alert objects
        for alert_data in alerts_to_check:
            alert = PerformanceAlert(
                alert_type=alert_data[0],
                severity=alert_data[1],
                message=alert_data[2],
                timestamp=alert_data[3],
                simulation_step=alert_data[4],
                metrics=alert_data[5]
            )
            self.alerts.append(alert)
            
            # Print alert for immediate visibility
            print(f"[{alert.severity}] {alert.alert_type}: {alert.message} (Step {alert.simulation_step})")
        
        # Trim old alerts
        if len(self.alerts) > 1000:
            self.alerts = self.alerts[-500:]
    
    def update_simulation_step(self, step: int):
        """Update current simulation step."""
        self.current_step = step
    
    def get_performance_summary(self) -> Dict[str, Any]:
        """Get performance summary statistics."""
        if not self.data_points:
            return {}
        
        summary = {
            "total_data_points": len(self.data_points),
            "total_simulation_steps": self.current_step,
            "monitoring_duration": time.time() - self.start_time,
            "average_metrics": {},
            "peak_metrics": {},
            "alert_count": len(self.alerts)
        }
        
        # Calculate averages for each metric
        metric_sums: Dict[PerformanceMetric, List[float]] = {}
        
        for point in self.data_points:
            for metric, value in point.metrics.items():
                if metric not in metric_sums:
                    metric_sums[metric] = []
                metric_sums[metric].append(value)
        
        # Calculate averages
        for metric, values in metric_sums.items():
            if values:
                summary["average_metrics"][metric.value] = sum(values) / len(values)
                summary["peak_metrics"][metric.value] = max(values)
        
        return summary
    
    def export_performance_data(self, output_path: str):
        """Export performance data to JSON file."""
        data = {
            "data_points": [point.to_dict() for point in self.data_points],
            "alerts": [alert.to_dict() for alert in self.alerts],
            "summary": self.get_performance_summary(),
            "config": self.config
        }
        
        with open(output_path, 'w') as f:
            json.dump(data, f, indent=4)
        
        return output_path
    
    def import_performance_data(self, input_path: str):
        """Import performance data from JSON file."""
        with open(input_path, 'r') as f:
            data = json.load(f)
        
        self.data_points = [
            PerformanceDataPoint.from_dict(point) 
            for point in data.get("data_points", [])
        ]
        
        self.alerts = [
            PerformanceAlert(
                alert_type=alert["alert_type"],
                severity=alert["severity"],
                message=alert["message"],
                timestamp=alert["timestamp"],
                simulation_step=alert["simulation_step"],
                metrics={PerformanceMetric(k): v for k, v in alert.get("metrics", {}).items()}
            )
            for alert in data.get("alerts", [])
        ]
        
        return len(self.data_points), len(self.alerts)
    
    def generate_performance_report(self) -> str:
        """Generate human-readable performance report."""
        summary = self.get_performance_summary()
        
        report = f"""NLM Phase 6 Performance Report
Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}

Simulation Summary:
- Total Steps: {summary.get('total_simulation_steps', 0)}
- Duration: {summary.get('monitoring_duration', 0):.2f} seconds
- Data Points Collected: {summary.get('total_data_points', 0)}

Performance Averages:
"""
        
        for metric_name, value in summary.get("average_metrics", {}).items():
            report += f"- {metric_name}: {value:.3f}\n"
        
        report += f"\nPeak Performance:\n"
        for metric_name, value in summary.get("peak_metrics", {}).items():
            report += f"- {metric_name}: {value:.3f}\n"
        
        report += f"\nAlerts: {summary.get('alert_count', 0)}\n"
        
        if self.alerts:
            report += "\nRecent Alerts:\n"
            for alert in self.alerts[-5:]:  # Show last 5 alerts
                report += f"- [{alert.severity}] Step {alert.simulation_step}: {alert.message}\n"
        
        return report

# Global performance monitor instance
performance_monitor = Phase6PerformanceMonitor()

# Convenience functions
def start_performance_monitoring(config: Dict[str, Any] = None, brain_simulation_func=None):
    """Start performance monitoring."""
    if config:
        performance_monitor.config.update(config)
        performance_monitor = Phase6PerformanceMonitor(config)
    
    performance_monitor.start_monitoring(brain_simulation_func)

def stop_performance_monitoring():
    """Stop performance monitoring."""
    performance_monitor.stop_monitoring()

def update_simulation_step(step: int):
    """Update simulation step for monitoring."""
    performance_monitor.update_simulation_step(step)

def get_performance_summary() -> Dict[str, Any]:
    """Get performance summary."""
    return performance_monitor.get_performance_summary()

def export_performance_data(output_path: str) -> str:
    """Export performance data."""
    return performance_monitor.export_performance_data(output_path)

def generate_performance_report() -> str:
    """Generate performance report."""
    return performance_monitor.generate_performance_report()