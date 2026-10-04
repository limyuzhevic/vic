'''NLM Python Visualization Tools

This module provides comprehensive visualization and monitoring capabilities for the Neural Learning Machine (NLM) project.
It integrates with the C++ core NLM system and provides real-time visualization of neural activity, brain state, and system performance.
'''

import asyncio
import json
import time
from datetime import datetime
from typing import Dict, List, Optional, Callable, Any
import numpy as np
from dataclasses import dataclass, asdict
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.gridspec import GridSpec
import matplotlib.dates as mdates
import threading
import queue
import os
import sys

@dataclass
class PerformanceMetrics:
    """Performance metrics for NLM system monitoring"""
    timestamp: datetime
    cpu_usage: float = 0.0
    memory_usage: float = 0.0
    simulation_time: float = 0.0
    rendering_time: float = 0.0
    spike_count: int = 0
    active_neuron_count: int = 0
    average_firing_rate: float = 0.0
    step_count: int = 0
    @classmethod
    def from_cpp_metrics(cls, cpp_metrics: dict) -> 'PerformanceMetrics':
        return cls(
            timestamp=datetime.now(),
            cpu_usage=cpp_metrics.get('cpu_usage', 0.0),
            memory_usage=cpp_metrics.get('memory_usage', 0.0),
            simulation_time=cpp_metrics.get('simulation_time', 0.0),
            rendering_time=cpp_metrics.get('rendering_time', 0.0),
            spike_count=cpp_metrics.get('spike_count', 0),
            active_neuron_count=cpp_metrics.get('active_neuron_count', 0),
            average_firing_rate=cpp_metrics.get('average_firing_rate', 0.0),
            step_count=cpp_metrics.get('step_count', 0)
        )
@dataclass
class SystemAlert:
    """System alert for monitoring issues"""
    level: str
    message: str
    timestamp: datetime
    source: str = ""
    
    @classmethod
    def from_cpp_alert(cls, cpp_alert: dict) -> 'SystemAlert':
        return cls(
            level=cpp_alert.get('level', 'INFO'),
            message=cpp_alert.get('message', ''),
            timestamp=datetime.now(),
            source=cpp_alert.get('source', '')
        )
class RealTimePlotter:
    """Real-time plotting and monitoring for NLM"""
    
    def __init__(self, update_interval: float = 1.0):
        self.update_interval = update_interval
        self.metrics_history: List[PerformanceMetrics] = []
        self.alert_history: List[SystemAlert] = []
        self.plot_data = {
            'time': [],
            'cpu': [],
            'memory': [],
            'spike_rate': [],
            'neuron_activity': []
        }
        self.running = False
        self.thread: Optional[threading.Thread] = None
        
    def start_monitoring(self, cpp_callback: Optional[Callable] = None):
        """Start real-time monitoring"""
        self.running = True
        self.thread = threading.Thread(target=self._monitor_loop, args=(cpp_callback,))
        self.thread.daemon = True
        self.thread.start()
    
    def stop_monitoring(self):
        """Stop real-time monitoring"""
        self.running = False
        if self.thread:
            self.thread.join(timeout=5.0)
            self.thread = None
    
    def _monitor_loop(self, cpp_callback: Optional[Callable] = None):
        """Main monitoring loop"""
        while self.running:
            start_time = time.time()
            
            # Get metrics from C++ if callback provided
            if cpp_callback:
                try:
                    metrics_data = cpp_callback()
                    if metrics_data:
                        metrics = PerformanceMetrics.from_cpp_metrics(metrics_data)
                        self.metrics_history.append(metrics)
                        
                        # Update plot data
                        self.plot_data['time'].append(metrics.timestamp)
                        self.plot_data['cpu'].append(metrics.cpu_usage)
                        self.plot_data['memory'].append(metrics.memory_usage)
                        self.plot_data['spike_rate'].append(metrics.average_firing_rate)
                        self.plot_data['neuron_activity'].append(metrics.active_neuron_count)
                        
                        # Keep only last 1000 data points
                        max_points = 1000
                        for key in self.plot_data:
                            if len(self.plot_data[key]) > max_points:
                                self.plot_data[key] = self.plot_data[key][-max_points:]
                except Exception as e:
                    print(f"Error in monitoring loop: {e}")
            
            # Sleep for update interval
            elapsed = time.time() - start_time
            sleep_time = max(0, self.update_interval - elapsed)
            time.sleep(sleep_time)
    
    def create_dashboard_plot(self, save_path: Optional[str] = None):
        """Create a comprehensive dashboard plot"""
        if not self.plot_data['time']:
            print("No data available for dashboard")
            return
        
        fig = plt.figure(figsize=(15, 10))
        gs = GridSpec(3, 3, hspace=0.3, wspace=0.3)
        
        # CPU Usage
        ax1 = fig.add_subplot(gs[0, 0])
        ax1.plot(self.plot_data['time'], self.plot_data['cpu'], 'r-', linewidth=1)
        ax1.set_title('CPU Usage')
        ax1.set_ylabel('%')
        ax1.grid(True, alpha=0.3)
        
        # Memory Usage
        ax2 = fig.add_subplot(gs[0, 1])
        ax2.plot(self.plot_data['time'], self.plot_data['memory'], 'b-', linewidth=1)
        ax2.set_title('Memory Usage')
        ax2.set_ylabel('%')
        ax2.grid(True, alpha=0.3)
        
        # Spike Rate
        ax3 = fig.add_subplot(gs[0, 2])
        ax3.plot(self.plot_data['time'], self.plot_data['spike_rate'], 'g-', linewidth=1)
        ax3.set_title('Spike Rate')
        ax3.set_ylabel('Hz')
        ax3.grid(True, alpha=0.3)
        
        # Neuron Activity
        ax4 = fig.add_subplot(gs[1, :2])
        ax4.plot(self.plot_data['time'], self.plot_data['neuron_activity'], 'm-', linewidth=1)
        ax4.set_title('Active Neuron Count')
        ax4.set_ylabel('Count')
        ax4.grid(True, alpha=0.3)
        
        # All metrics together
        ax5 = fig.add_subplot(gs[1, 2])
        ax5.plot(self.plot_data['time'], self.plot_data['cpu'], 'r-', label='CPU', linewidth=0.5)
        ax5.plot(self.plot_data['time'], self.plot_data['memory'], 'b-', label='Memory', linewidth=0.5)
        ax5.plot(self.plot_data['time'], self.plot_data['spike_rate'], 'g-', label='Spike Rate', linewidth=0.5)
        ax5.set_title('All Metrics')
        ax5.set_ylabel('Value')
        ax5.legend(fontsize='small')
        ax5.grid(True, alpha=0.3)
        
        # System alerts
        ax6 = fig.add_subplot(gs[2, :])
        if self.alert_history:
            levels = [alert.level for alert in self.alert_history]
            times = [alert.timestamp for alert in self.alert_history]
            ax6.scatter(times, range(len(levels)), c=[{'INFO': 'blue', 'WARNING': 'orange', 
                                                      'ERROR': 'red', 'CRITICAL': 'darkred'}[level] 
                                                     for level in levels], s=20, alpha=0.7)
            ax6.set_title('System Alerts')
            ax6.set_xlabel('Time')
            ax6.set_ylabel('Alert Level')
            ax6.set_yticks([])
            ax6.grid(True, alpha=0.3)
        else:
            ax6.text(0.5, 0.5, 'No Alerts', ha='center', va='center', transform=ax6.transAxes)
            ax6.set_title('System Alerts')
        
        # Format time axis
        for ax in fig.get_axes():
            for label in ax.get_xticklabels():
                label.set_rotation(45)
                label.set_ha('right')
        
        plt.suptitle('NLM Real-Time Monitoring Dashboard', fontsize=14, fontweight='bold')
        
        if save_path:
            plt.savefig(save_path, dpi=150, bbox_inches='tight')
            print(f"Dashboard saved to {save_path}")
        
        plt.show()
        plt.close()
    
    def create_spike_raster_plot(self, spikes_data: Optional[List[dict]] = None):
        """Create a spike raster plot"""
        fig, ax = plt.subplots(figsize=(12, 6))
        
        if spikes_data:
            # Group spikes by neuron
            neuron_spikes = {}
            for spike in spikes_data:
                neuron_id = spike.get('neuron_id', 0)
                time = spike.get('time', 0)
                if neuron_id not in neuron_spikes:
                    neuron_spikes[neuron_id] = []
                neuron_spikes[neuron_id].append(time)
            
            # Plot spikes
            for neuron_id, spike_times in neuron_spikes.items():
                ax.scatter(spike_times, [neuron_id] * len(spike_times), 
                          s=1, c='black', alpha=0.6)
        
        ax.set_xlabel('Time (ms)')
        ax.set_ylabel('Neuron ID')
        ax.set_title('Spike Raster Plot')
        ax.grid(True, alpha=0.3)
        
        plt.show()
        plt.close()
    
    def create_region_heatmap(self, region_data: Optional[Dict] = None):
        """Create a brain region activity heatmap"""
        fig, ax = plt.subplots(figsize=(10, 8))
        
        if region_data:
            # Create heatmap data
            regions = list(region_data.keys())
            activities = list(region_data.values())
            
            # Create bar plot
            bars = ax.bar(range(len(regions)), activities, alpha=0.7, 
                         color=plt.cm.viridis(np.linspace(0, 1, len(regions))))
            
            # Add value labels on bars
            for bar in bars:
                height = bar.get_height()
                ax.text(bar.get_x() + bar.get_width()/2., height,
                       f'{height:.2f}', ha='center', va='bottom')
            
            ax.set_xticks(range(len(regions)))
            ax.set_xticklabels(regions)
            ax.set_ylabel('Activity Level')
            ax.set_title('Brain Region Activity Heatmap')
            ax.grid(True, alpha=0.3, axis='y')
        
        plt.tight_layout()
        plt.show()
        plt.close()
class ActivityMonitor:
    """Activity monitoring and analysis for NLM"""
    
    def __init__(self):
        self.activity_history: List[Dict] = []
        self.current_activity: Dict = {}
        self.analysis_window: int = 100
    
    def update_activity(self, brain_data: Dict):
        """Update activity monitoring data"""
        self.current_activity = brain_data
        self.activity_history.append({
            'timestamp': datetime.now(),
            'data': brain_data
        })
        
        # Keep only recent data
        if len(self.activity_history) > self.analysis_window:
            self.activity_history = self.activity_history[-self.analysis_window:]
    
    def get_activity_summary(self) -> Dict:
        """Get activity summary statistics"""
        if not self.activity_history:
            return {}
        
        latest = self.activity_history[-1]['data']
        summary = {
            'timestamp': latest.get('timestamp'),
            'total_neurons': latest.get('neuron_count', 0),
            'active_neurons': latest.get('active_neuron_count', 0),
            'firing_rate': latest.get('firing_rate', 0.0),
            'synapse_count': latest.get('synapse_count', 0),
            'development_stage': latest.get('development_stage', 'Unknown'),
            'plasticity_level': latest.get('plasticity_level', 0.0)
        }
        
        return summary
    
    def detect_anomalies(self) -> List[str]:
        """Detect potential anomalies in activity"""
        anomalies = []
        latest = self.get_activity_summary()
        
        # Check for high activity levels
        if latest.get('firing_rate', 0.0) > 100.0:
            anomalies.append(f"High firing rate detected: {latest['firing_rate']:.1f} Hz")
        
        # Check for low neuron activity
        if latest.get('active_neurons', 0) == 0:
            anomalies.append("No active neurons detected")
        
        # Check for development stage issues
        if latest.get('development_stage') == 'Unknown':
            anomalies.append("Development stage not reported")
        
        return anomalies
class DevelopmentTracker:
    """Development tracking and visualization for NLM"""
    
    def __init__(self):
        self.development_history: List[Dict] = []
        self.current_stage: str = "Unknown"
        self.stage_transitions: List[Dict] = []
    
    def update_stage(self, stage_info: Dict):
        """Update development stage information"""
        old_stage = self.current_stage
        self.current_stage = stage_info.get('stage', 'Unknown')
        
        if old_stage != self.current_stage:
            self.stage_transitions.append({
                'timestamp': datetime.now(),
                'from_stage': old_stage,
                'to_stage': self.current_stage
            })
        
        self.development_history.append({
            'timestamp': datetime.now(),
            'stage': self.current_stage,
            'info': stage_info
        })
        
        # Keep only recent data
        if len(self.development_history) > 1000:
            self.development_history = self.development_history[-1000:]
    
    def create_development_timeline(self, save_path: Optional[str] = None):
        """Create a development timeline visualization"""
        if not self.development_history:
            print("No development data available")
            return
        
        fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 8))
        
        # Stage timeline
        timestamps = [entry['timestamp'] for entry in self.development_history]
        stages = [entry['stage'] for entry in self.development_history]
        
        # Map stages to numeric values for plotting
        stage_values = {}
        unique_stages = list(set(stages))
        for i, stage in enumerate(unique_stages):
            stage_values[stage] = i
        
        stage_numeric = [stage_values[stage] for stage in stages]
        
        ax1.plot(timestamps, stage_numeric, 'b-', linewidth=2, marker='o', markersize=3)
        ax1.set_yticks(range(len(unique_stages)))
        ax1.set_yticklabels(unique_stages)
        ax1.set_xlabel('Time')
        ax1.set_ylabel('Development Stage')
        ax1.set_title('Development Stage Timeline')
        ax1.grid(True, alpha=0.3)
        
        # Transitions
        if self.stage_transitions:
            trans_times = [t['timestamp'] for t in self.stage_transitions]
            trans_from = [t['from_stage'] for t in self.stage_transitions]
            trans_to = [t['to_stage'] for t in self.stage_transitions]
            
            ax2.scatter(trans_times, [stage_values[f] for f in trans_from], 
                       c='red', s=50, marker='o', label='From Stage')
            ax2.scatter(trans_times, [stage_values[t] for t in trans_to], 
                       c='green', s=50, marker='s', label='To Stage')
            ax2.set_yticks(range(len(unique_stages)))
            ax2.set_yticklabels(unique_stages)
            ax2.set_xlabel('Time')
            ax2.set_ylabel('Stage')
            ax2.set_title('Development Stage Transitions')
            ax2.legend()
            ax2.grid(True, alpha=0.3)
        
        # Format time axis
        for ax in fig.get_axes():
            for label in ax.get_xticklabels():
                label.set_rotation(45)
                label.set_ha('right')
        
        plt.suptitle('NLM Development Timeline', fontsize=14, fontweight='bold')
        
        if save_path:
            plt.savefig(save_path, dpi=150, bbox_inches='tight')
            print(f"Development timeline saved to {save_path}")
        
        plt.show()
        plt.close()
class SystemMonitor:
    """System monitoring and health checks for NLM"""
    
    def __init__(self):
        self.alerts: List[SystemAlert] = []
        self.health_status: Dict = {}
        self.monitoring_rules = [
            {'name': 'High CPU Usage', 'condition': lambda m: m.get('cpu_usage', 0) > 90, 'severity': 'WARNING'},
            {'name': 'Low Memory', 'condition': lambda m: m.get('memory_usage', 0) > 95, 'severity': 'ERROR'},
            {'name': 'No Activity', 'condition': lambda m: m.get('spike_count', 0) == 0, 'severity': 'INFO'},
            {'name': 'Critical Error', 'condition': lambda m: m.get('error', False), 'severity': 'CRITICAL'}
        ]
    
    def update_health(self, health_data: Dict):
        """Update system health status"""
        self.health_status = health_data
        
        # Check monitoring rules
        for rule in self.monitoring_rules:
            if rule['condition'](health_data):
                alert = SystemAlert(
                    level=rule['severity'],
                    message=f"{rule['name']}: {health_data.get('message', '')}",
                    timestamp=datetime.now(),
                    source=rule['name']
                )
                self.add_alert(alert)
    
    def add_alert(self, alert: SystemAlert):
        """Add a system alert"""
        self.alerts.append(alert)
        
        # Keep only recent alerts
        if len(self.alerts) > 100:
            self.alerts = self.alerts[-100:]
    
    def get_alert_summary(self) -> Dict:
        """Get alert summary statistics"""
        levels = ['INFO', 'WARNING', 'ERROR', 'CRITICAL']
        counts = {level: 0 for level in levels}
        
        for alert in self.alerts:
            if alert.level in counts:
                counts[alert.level] += 1
        
        return {
            'total_alerts': len(self.alerts),
            'alerts_by_level': counts,
            'latest_alert': self.alerts[-1].message if self.alerts else None,
            'latest_alert_time': self.alerts[-1].timestamp if self.alerts else None
        }
    
    def check_system_health(self) -> Dict:
        """Check overall system health"""
        health_score = 100.0
        issues = []
        
        # Check various health metrics
        if self.health_status.get('cpu_usage', 0) > 80:
            health_score -= 10
            issues.append("High CPU usage")
        
        if self.health_status.get('memory_usage', 0) > 90:
            health_score -= 20
            issues.append("Low memory")
        
        if self.health_status.get('simulation_time', 0) > 1000:
            health_score -= 15
            issues.append("Slow simulation")
        
        status = "HEALTHY" if health_score > 90 else "WARNING" if health_score > 70 else "CRITICAL"
        
        return {
            'status': status,
            'health_score': health_score,
            'issues': issues,
            'timestamp': datetime.now()
        }
# Command-line visualization tools
class CommandLineVisualizer:
    """Command-line visualization tools for NLM"""
    
    @staticmethod
    def print_neural_activity_summary(brain_data: Dict):
        """Print a summary of neural activity to command line"""
        print("\n" + "="*60)
        print("NLM NEURAL ACTIVITY SUMMARY")
        print("="*60)
        
        neurons = brain_data.get('neuron_count', 0)
        active = brain_data.get('active_neuron_count', 0)
        firing = brain_data.get('firing_neuron_count', 0)
        firing_rate = brain_data.get('firing_rate', 0.0)
        synapses = brain_data.get('synapse_count', 0)
        
        print(f"Total Neurons:          {neurons:,}")
        print(f"Active Neurons:         {active:,} ({active/max(neurons,1)*100:.1f}%)")
        print(f"Firing Neurons:         {firing:,}")
        print(f"Average Firing Rate:    {firing_rate:.2f} Hz")
        print(f"Total Synapses:         {synapses:,}")
        print(f"Excitatory/Inhibitory Ratio: {brain_data.get('exc_inh_ratio', 0.0):.2f}")
        
        # Activity indicators
        activity_level = "LOW"
        if firing_rate > 10:
            activity_level = "HIGH"
        elif firing_rate > 5:
            activity_level = "MEDIUM"
        
        print(f"\nActivity Level:         {activity_level}")
        print(f"Development Stage:      {brain_data.get('development_stage', 'Unknown')}")
        print(f"Plasticity Level:       {brain_data.get('plasticity_level', 0.0):.3f}")
        
        print("="*60 + "\n")
    
    @staticmethod
    def print_system_status(health_data: Dict, alerts: List[SystemAlert]):
        """Print system status to command line"""
        print("\n" + "="*60)
        print("NLM SYSTEM STATUS")
        print("="*60)
        
        # System health
        health = SystemMonitor().check_system_health()
        print(f"Overall Health:         {health['status']} ({health['health_score']:.1f}%)")
        
        # Resource usage
        print(f"\nResource Usage:")
        print(f"  CPU Usage:             {health_data.get('cpu_usage', 0.0):.1f}%")
        print(f"  Memory Usage:          {health_data.get('memory_usage', 0.0):.1f}%")
        print(f"  Simulation Time:       {health_data.get('simulation_time', 0.0):.2f} ms")
        
        # Alert summary
        if alerts:
            print(f"\nAlerts ({len(alerts)} total):")
            for alert in alerts[-5:]:  # Show last 5 alerts
                level_icon = {"INFO": "ℹ", "WARNING": "⚠", "ERROR": "✗", "CRITICAL": "⚡"}[alert.level]
                time_str = alert.timestamp.strftime("%H:%M:%S")
                print(f"  {time_str} {level_icon} {alert.level}: {alert.message}")
        else:
            print("\nAlerts: No active alerts")
        
        print("="*60 + "\n")
    
    @staticmethod
    def create_progress_bar(current: int, total: int, width: int = 50):
        """Create a progress bar for command-line display"""
        progress = current / total
        filled = int(width * progress)
        bar = "█" * filled + "░" * (width - filled)
        
        percent = progress * 100
        print(f"\r[{bar}] {percent:.1f}% ({current}/{total})", end='')
        
        if current >= total:
            print("\n")
        
        return progress
    
    @staticmethod
    def print_visualization_help():
        """Print help information for visualization tools"""
        print("\n" + "="*60)
        print("NLM VISUALIZATION TOOLS HELP")
        print("="*60)
        print("\nAvailable Commands:")
        print("  --monitor          Start real-time monitoring")
        print("  --plot-dashboard   Create dashboard visualization")
        print("  --plot-spikes      Create spike raster plot")
        print("  --plot-regions     Create region heatmap")
        print("  --plot-development  Create development timeline")
        print("  --status           Show system status")
        print("  --help             Show this help")
        print("\nExamples:")
        print("  nlm --monitor --plot-dashboard --interval 1.0")
        print("  nlm --plot-spikes --output spikes.png")
        print("  nlm --status --verbose")
        print("\n" + "="*60 + "\n")
# Main visualization integration class
class NLMVisualizer:
    """Main integration class for all NLM visualization tools"""
    
    def __init__(self, cpp_interface = None):
        self.cpp_interface = cpp_interface
        self.plotter = RealTimePlotter()
        self.monitor = ActivityMonitor()
        self.development_tracker = DevelopmentTracker()
        self.system_monitor = SystemMonitor()
        self.running = False
        
    def start_visualization(self, config: Optional[Dict] = None):
        """Start all visualization systems"""
        config = config or {}
        
        # Start real-time plotting
        self.plotter.start_monitoring(self._get_cpp_metrics)
        
        # Start command-line updates
        self.running = True
        self._update_loop()
        
        print("NLM Visualization started successfully")
    
    def stop_visualization(self):
        """Stop all visualization systems"""
        self.running = False
        self.plotter.stop_monitoring()
        print("NLM Visualization stopped")
    
    def create_all_visualizations(self, output_dir: str = "."):
        """Create all available visualizations"""
        os.makedirs(output_dir, exist_ok=True)
        
        # Dashboard
        self.plotter.create_dashboard_plot(os.path.join(output_dir, "dashboard.png"))
        
        # Spike raster plot
        self.plotter.create_spike_raster_plot()
        
        # Region heatmap
        self.plotter.create_region_heatmap()
        
        # Development timeline
        self.development_tracker.create_development_timeline(
            os.path.join(output_dir, "development_timeline.png")
        )
        
        print(f"All visualizations created in {output_dir}")
    
    def _get_cpp_metrics(self):
        """Get metrics from C++ interface"""
        if self.cpp_interface:
            return self.cpp_interface.get_performance_metrics()
        return None
    
    def _update_loop(self):
        """Main update loop for visualization"""
        import time
        while self.running:
            try:
                # Update monitoring
                if self.cpp_interface:
                    brain_data = self.cpp_interface.get_brain_state()
                    self.monitor.update_activity(brain_data)
                    self.development_tracker.update_stage(brain_data.get('development', {}))
                    self.system_monitor.update_health(brain_data.get('system_health', {}))
                
                # Print status periodically
                if int(time.time()) % 30 == 0:  # Every 30 seconds
                    CommandLineVisualizer.print_system_status(
                        self.system_monitor.health_status,
                        self.system_monitor.alerts
                    )
                
                time.sleep(1.0)  # Update every second
            except Exception as e:
                print(f"Error in visualization update loop: {e}")
                time.sleep(5.0)  # Wait longer on error
# Python bindings for C++ visualization
if __name__ == "__main__":
    # Example usage
    visualizer = NLMVisualizer()
    
    # Start visualization
    visualizer.start_visualization()
    
    # Create visualizations
    visualizer.create_all_visualizations("nlm_visualizations")
    
    # Stop visualization
    visualizer.stop_visualization()

__all__ = [
    'PerformanceMetrics',
    'SystemAlert',
    'RealTimePlotter',
    'ActivityMonitor',
    'DevelopmentTracker',
    'SystemMonitor',
    'CommandLineVisualizer',
    'NLMVisualizer'
]
