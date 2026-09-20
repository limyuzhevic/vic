#!/usr/bin/env python3
"""
NLM Advanced Debugging and Visualization Tools

Advanced debugging, profiling, and visualization capabilities for NLM neural simulations.
"""

import argparse
import sys
import os
import json
import time
import threading
from pathlib import Path
from typing import Optional, List, Dict, Any, Callable
import numpy as np
import matplotlib.pyplot as plt
import matplotlib
matplotlib.use('Agg')  # Non-interactive backend

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Warning: pynlm Python bindings not available. Some debugging tools will not work.")

# Version
__version__ = "0.1.0"


class PerformanceMonitor:
    """Real-time performance monitoring for NLM simulations."""
    
    def __init__(self):
        self.start_time = time.time()
        self.samples = []
        self.monitoring = False
        self.monitor_thread = None
    
    def start_monitoring(self, interval: float = 0.1):
        """Start real-time monitoring."""
        self.monitoring = True
        self.monitor_thread = threading.Thread(
            target=self._monitor_loop,
            args=(interval,),
            daemon=True
        )
        self.monitor_thread.start()
    
    def stop_monitoring(self):
        """Stop monitoring."""
        self.monitoring = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=1.0)
    
    def _monitor_loop(self, interval: float):
        """Monitoring loop running in background."""
        while self.monitoring:
            sample = self._collect_sample()
            if sample:
                self.samples.append(sample)
            time.sleep(interval)
    
    def _collect_sample(self):
        """Collect a single performance sample."""
        try:
            import psutil
            import pynlm
            
            # CPU usage
            cpu_percent = psutil.cpu_percent(interval=0.01)
            
            # Memory usage
            memory = psutil.virtual_memory()
            memory_percent = memory.percent
            
            # Process memory
            process = psutil.Process()
            process_memory_mb = process.memory_info().rss / 1024 / 1024
            
            # Get sample from pynlm if available
            brain_info = {}
            if PYNLM_AVAILABLE:
                try:
                    pass
                except:
                    pass
            
            return {
                'timestamp': time.time() - self.start_time,
                'cpu_percent': cpu_percent,
                'memory_percent': memory_percent,
                'process_memory_mb': process_memory_mb,
                'brain_info': brain_info
            }
        except Exception as e:
            print(f"Monitoring error: {e}")
            return None
    
    def get_statistics(self) -> Dict[str, Any]:
        """Get monitoring statistics."""
        if not self.samples:
            return {}
        
        cpu_samples = [s['cpu_percent'] for s in self.samples if 'cpu_percent' in s]
        memory_samples = [s['memory_percent'] for s in self.samples if 'memory_percent' in s]
        process_memory_samples = [s['process_memory_mb'] for s in self.samples if 'process_memory_mb' in s]
        
        return {
            'duration': len(self.samples),
            'cpu': {
                'mean': np.mean(cpu_samples) if cpu_samples else 0,
                'max': np.max(cpu_samples) if cpu_samples else 0,
                'min': np.min(cpu_samples) if cpu_samples else 0,
                'std': np.std(cpu_samples) if cpu_samples else 0
            },
            'memory': {
                'mean': np.mean(memory_samples) if memory_samples else 0,
                'max': np.max(memory_samples) if memory_samples else 0,
                'min': np.min(memory_samples) if memory_samples else 0,
                'std': np.std(memory_samples) if memory_samples else 0
            },
            'process_memory': {
                'mean': np.mean(process_memory_samples) if process_memory_samples else 0,
                'max': np.max(process_memory_samples) if process_memory_samples else 0,
                'min': np.min(process_memory_samples) if process_memory_samples else 0,
                'std': np.std(process_memory_samples) if process_memory_samples else 0
            }
        }
    
    def plot_performance(self, output_file: str):
        """Plot performance statistics."""
        stats = self.get_statistics()
        
        if not stats:
            print("No performance data to plot")
            return
        
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        fig.suptitle("NLM Performance Monitoring", fontsize=16)
        
        if self.samples:
            timestamps = [s['timestamp'] for s in self.samples]
            cpu_values = [s['cpu_percent'] for s in self.samples]
            
            axes[0, 0].plot(timestamps, cpu_values, 'b-', alpha=0.7)
            axes[0, 0].set_xlabel('Time (s)')
            axes[0, 0].set_ylabel('CPU %')
            axes[0, 0].set_title('CPU Usage Over Time')
            axes[0, 0].grid(True, alpha=0.3)
        
        if self.samples:
            timestamps = [s['timestamp'] for s in self.samples]
            memory_values = [s['memory_percent'] for s in self.samples]
            
            axes[0, 1].plot(timestamps, memory_values, 'r-', alpha=0.7)
            axes[0, 1].set_xlabel('Time (s)')
            axes[0, 1].set_ylabel('Memory %')
            axes[0, 1].set_title('Memory Usage Over Time')
            axes[0, 1].grid(True, alpha=0.3)
        
        axes[1, 0].axis('off')
        summary_text = f"""Performance Summary:
        
        Duration: {stats['duration']:.1f} seconds
        
        CPU Statistics:
          Mean: {stats['cpu']['mean']:.1f}%
          Max: {stats['cpu']['max']:.1f}%
          Min: {stats['cpu']['min']:.1f}%
          Std: {stats['cpu']['std']:.1f}%
        
        Memory Statistics:
          Mean: {stats['memory']['mean']:.1f}%
          Max: {stats['memory']['max']:.1f}%
          Min: {stats['memory']['min']:.1f}%
          Std: {stats['memory']['std']:.1f}%
        
        Process Memory (MB):
          Mean: {stats['process_memory']['mean']:.1f}
          Max: {stats['process_memory']['max']:.1f}
          Min: {stats['process_memory']['min']:.1f}
          Std: {stats['process_memory']['std']:.1f}
        """
        axes[1, 0].text(0.1, 0.5, summary_text, fontsize=10,
                      verticalalignment='center', fontfamily='monospace')
        
        plt.tight_layout()
        plt.savefig(output_file, dpi=150, bbox_inches='tight')
        plt.close()


class SpikeAnalyzer:
    """Analyze neural spike patterns and dynamics."""
    
    def __init__(self, spike_threshold: float = 0.5, analysis_window: int = 1000):
        self.spike_threshold = spike_threshold
        self.analysis_window = analysis_window
        self.spike_data = []
        self.spike_windows = []
    
    def add_spike_data(self, spike_times: List[float], neuron_ids: List[int] = None):
        """Add spike data for analysis."""
        if neuron_ids is None:
            neuron_ids = [0] * len(spike_times)
        
        for spike_time, neuron_id in zip(spike_times, neuron_ids):
            self.spike_data.append({
                'time': spike_time,
                'neuron_id': neuron_id
            })
        
        self.spike_data.sort(key=lambda x: x['time'])
    
    def analyze_spike_patterns(self) -> Dict[str, Any]:
        """Analyze spike patterns and return statistics."""
        if not self.spike_data:
            return {}
        
        spike_times = [s['time'] for s in self.spike_data]
        neuron_ids = [s['neuron_id'] for s in self.spike_data]
        
        stats = {
            'total_spikes': len(spike_times),
            'unique_neurons': len(set(neuron_ids)),
            'mean_interval': np.mean(np.diff(spike_times)) if len(spike_times) > 1 else 0,
            'std_interval': np.std(np.diff(spike_times)) if len(spike_times) > 1 else 0,
            'min_interval': np.min(np.diff(spike_times)) if len(spike_times) > 1 else 0,
            'max_interval': np.max(np.diff(spike_times)) if len(spike_times) > 1 else 0,
            'firing_rate': len(spike_times) / max(len(spike_times), 1),
        }
        
        bursts = self._detect_bursts(spike_times)
        stats['burst_count'] = len(bursts)
        stats['burst_durations'] = [len(burst) for burst in bursts]
        stats['mean_burst_duration'] = np.mean(stats['burst_durations']) if bursts else 0
        
        if len(spike_times) > 1:
            intervals = np.diff(spike_times)
            refractory_violations = np.sum(intervals < 0.001)
            stats['refractory_violations'] = refractory_violations
            stats['refractory_violation_rate'] = refractory_violations / len(intervals)
        
        return stats
    
    def _detect_bursts(self, spike_times: List[float], max_interval: float = 0.02) -> List[List[float]]:
        """Detect spike bursts."""
        if len(spike_times) < 2:
            return []
        
        bursts = []
        current_burst = [spike_times[0]]
        
        for i in range(1, len(spike_times)):
            interval = spike_times[i] - spike_times[i-1]
            if interval <= max_interval:
                current_burst.append(spike_times[i])
            else:
                if len(current_burst) > 1:
                    bursts.append(current_burst)
                current_burst = [spike_times[i]]
        
        if len(current_burst) > 1:
            bursts.append(current_burst)
        
        return bursts
    
    def create_spike_raster(self, output_file: str, neuron_ids: List[int] = None):
        """Create a spike raster plot."""
        if not self.spike_data:
            print("No spike data to plot")
            return
        
        if neuron_ids is None:
            neuron_ids = sorted(set(s['neuron_id'] for s in self.spike_data))
        
        fig, ax = plt.subplots(figsize=(12, 8))
        
        for spike in self.spike_data:
            neuron_id = spike['neuron_id']
            if neuron_id in neuron_ids:
                ax.plot(spike['time'], neuron_id, 'k.', markersize=2, alpha=0.7)
        
        ax.set_xlabel('Time (s)')
        ax.set_ylabel('Neuron ID')
        ax.set_title('Spike Raster Plot')
        ax.invert_yaxis()
        ax.grid(True, alpha=0.3)
        
        plt.tight_layout()
        plt.savefig(output_file, dpi=150, bbox_inches='tight')
        plt.close()
    
    def create_spike_histogram(self, output_file: str, bins: int = 100):
        """Create a histogram of spike times."""
        if not self.spike_data:
            print("No spike data to plot")
            return
        
        spike_times = [s['time'] for s in self.spike_data]
        
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        fig.suptitle('Spike Time Distribution Analysis', fontsize=16)
        
        axes[0, 0].hist(spike_times, bins=bins, alpha=0.7, color='blue', edgecolor='black')
        axes[0, 0].set_xlabel('Time (s)')
        axes[0, 0].set_ylabel('Count')
        axes[0, 0].set_title('All Spikes')
        axes[0, 0].grid(True, alpha=0.3)
        
        if len(spike_times) > 1:
            intervals = np.diff(spike_times)
            axes[0, 1].hist(intervals, bins=50, alpha=0.7, color='red', edgecolor='black')
            axes[0, 1].set_xlabel('Interval (s)')
            axes[0, 1].set_ylabel('Count')
            axes[0, 1].set_title('Inter-Spike Intervals')
            axes[0, 1].grid(True, alpha=0.3)
        
        neuron_counts = {}
        for spike in self.spike_data:
            neuron_id = spike['neuron_id']
            neuron_counts[neuron_id] = neuron_counts.get(neuron_id, 0) + 1
        
        neurons = list(neuron_counts.keys())
        counts = list(neuron_counts.values())
        
        axes[1, 0].bar(range(len(neurons)), counts, alpha=0.7, color='green')
        axes[1, 0].set_xlabel('Neuron ID')
        axes[1, 0].set_ylabel('Spike Count')
        axes[1, 0].set_title('Spikes Per Neuron')
        axes[1, 0].grid(True, alpha=0.3)
        
        cumulative = np.cumsum(sorted(counts, reverse=True))
        axes[1, 1].plot(range(1, len(cumulative) + 1), cumulative, 'o-', alpha=0.7)
        axes[1, 1].set_xlabel('Neuron Rank')
        axes[1, 1].set_ylabel('Cumulative Spikes')
        axes[1, 1].set_title('Cumulative Spike Count (Sorted)')
        axes[1, 1].grid(True, alpha=0.3)
        
        plt.tight_layout()
        plt.savefig(output_file, dpi=150, bbox_inches='tight')
        plt.close()


class MemoryMonitor:
    """Monitor memory usage and system resources."""
    
    def __init__(self):
        self.monitoring = False
        self.monitor_thread = None
        self.memory_samples = []
        self.cpu_samples = []
    
    def start_memory_monitoring(self, interval: float = 0.5):
        """Start memory monitoring."""
        self.monitoring = True
        self.monitor_thread = threading.Thread(
            target=self._memory_monitor_loop,
            args=(interval,),
            daemon=True
        )
        self.monitor_thread.start()
    
    def stop_memory_monitoring(self):
        """Stop memory monitoring."""
        self.monitoring = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=1.0)
    
    def _memory_monitor_loop(self, interval: float):
        """Memory monitoring loop."""
        while self.monitoring:
            sample = self._collect_memory_sample()
            if sample:
                self.memory_samples.append(sample)
            time.sleep(interval)
    
    def _collect_memory_sample(self):
        """Collect memory usage sample."""
        try:
            import psutil
            import pynlm
            
            memory = psutil.virtual_memory()
            process = psutil.Process()
            process_memory = process.memory_info()
            cpu_times = process.cpu_times()
            
            nlm_memory = {}
            if PYNLM_AVAILABLE:
                try:
                    pass
                except:
                    pass
            
            return {
                'timestamp': time.time(),
                'system_memory_total': memory.total,
                'system_memory_available': memory.available,
                'system_memory_percent': memory.percent,
                'process_memory_rss': process_memory.rss,
                'process_memory_vms': process_memory.vms,
                'process_memory_shared': process_memory.shared,
                'cpu_user_time': cpu_times.user,
                'cpu_system_time': cpu_times.system,
                'cpu_nice_time': cpu_times.nice,
                'nlm_memory': nlm_memory
            }
        except Exception as e:
            print(f"Memory monitoring error: {e}")
            return None
    
    def get_memory_statistics(self) -> Dict[str, Any]:
        """Get memory usage statistics."""
        if not self.memory_samples:
            return {}
        
        samples = self.memory_samples
        
        return {
            'samples_count': len(samples),
            'system_memory': {
                'mean_percent': np.mean([s['system_memory_percent'] for s in samples]),
                'max_percent': np.max([s['system_memory_percent'] for s in samples]),
                'min_percent': np.min([s['system_memory_percent'] for s in samples]),
            },
            'process_memory': {
                'mean_rss_mb': np.mean([s['process_memory_rss'] / 1024 / 1024 for s in samples]),
                'max_rss_mb': np.max([s['process_memory_rss'] / 1024 / 1024 for s in samples]),
                'mean_vms_mb': np.mean([s['process_memory_vms'] / 1024 / 1024 for s in samples]),
            },
            'cpu_time': {
                'mean_user': np.mean([s['cpu_user_time'] for s in samples]),
                'mean_system': np.mean([s['cpu_system_time'] for s in samples]),
            }
        }
    
    def plot_memory_usage(self, output_file: str):
        """Plot memory usage over time."""
        stats = self.get_memory_statistics()
        if not stats:
            print("No memory data to plot")
            return
        
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        fig.suptitle("NLM Memory Usage Monitoring", fontsize=16)
        
        if self.memory_samples:
            timestamps = [s['timestamp'] for s in self.memory_samples]
            sys_memory = [s['system_memory_percent'] for s in self.memory_samples]
            proc_memory_rss = [s['process_memory_rss'] / 1024 / 1024 for s in self.memory_samples]
            
            axes[0, 0].plot(timestamps, sys_memory, 'r-', alpha=0.7)
            axes[0, 0].set_xlabel('Time')
            axes[0, 0].set_ylabel('Memory %')
            axes[0, 0].set_title('System Memory Usage Over Time')
            axes[0, 0].grid(True, alpha=0.3)
            
            axes[0, 1].plot(timestamps, proc_memory_rss, 'b-', alpha=0.7)
            axes[0, 1].set_xlabel('Time')
            axes[0, 1].set_ylabel('Memory (MB)')
            axes[0, 1].set_title('Process Memory (RSS) Over Time')
            axes[0, 1].grid(True, alpha=0.3)
        
        axes[1, 0].axis('off')
        summary_text = f"""Memory Usage Summary:
        
        Monitoring Duration: {len(self.memory_samples)} samples
        
        System Memory:
          Mean Usage: {stats['system_memory']['mean_percent']:.1f}%
          Max Usage: {stats['system_memory']['max_percent']:.1f}%
          Min Usage: {stats['system_memory']['min_percent']:.1f}%
        
        Process Memory:
          Mean RSS: {stats['process_memory']['mean_rss_mb']:.1f} MB
          Max RSS: {stats['process_memory']['max_rss_mb']:.1f} MB
          Mean VMS: {stats['process_memory']['mean_vms_mb']:.1f} MB
        """
        axes[1, 0].text(0.1, 0.5, summary_text, fontsize=10,
                      verticalalignment='center', fontfamily='monospace')
        
        plt.tight_layout()
        plt.savefig(output_file, dpi=150, bbox_inches='tight')
        plt.close()


class DataExporter:
    """Export NLM data in various formats."""
    
    def export_to_json(self, data: Dict[str, Any], output_file: str):
        """Export data to JSON format."""
        try:
            with open(output_file, 'w') as f:
                json.dump(data, f, indent=2)
            print(f"Data exported to JSON: {output_file}")
        except Exception as e:
            raise Exception(f"Failed to export to JSON: {e}")
    
    def export_to_csv(self, data: Dict[str, List], output_file: str):
        """Export data to CSV format."""
        try:
            import pandas as pd
            
            df_data = {}
            for key, values in data.items():
                df_data[key] = values
            
            df = pd.DataFrame(df_data)
            df.to_csv(output_file, index=False)
            print(f"Data exported to CSV: {output_file}")
        except Exception as e:
            raise Exception(f"Failed to export to CSV: {e}")
    
    def export_to_matlab(self, data: Dict[str, Any], output_file: str):
        """Export data to MATLAB format."""
        try:
            import scipy.io
            
            scipy.io.savemat(output_file, data)
            print(f"Data exported to MATLAB: {output_file}")
        except Exception as e:
            raise Exception(f"Failed to export to MATLAB: {e}")
    
    def export_analysis_report(self, analysis_results: Dict[str, Any], 
                             output_file: str, format: str = "markdown"):
        """Export comprehensive analysis report."""
        try:
            if format == "markdown":
                report = self._generate_markdown_report(analysis_results)
                with open(output_file, 'w') as f:
                    f.write(report)
            elif format == "html":
                report = self._generate_html_report(analysis_results)
                with open(output_file, 'w') as f:
                    f.write(report)
            elif format == "pdf":
                raise NotImplementedError("PDF report generation not yet implemented")
            
            print(f"Analysis report exported: {output_file}")
        except Exception as e:
            raise Exception(f"Failed to export analysis report: {e}")
    
    def _generate_markdown_report(self, results: Dict[str, Any]) -> str:
        """Generate markdown analysis report."""
        report = f"# NLM Analysis Report\n\n"
        report += f"Generated on: {time.ctime()}\n\n"
        
        if 'performance' in results:
            report += "## Performance Metrics\n\n"
            perf = results['performance']
            report += f"| Metric | Value |\n"
            report += f"|--------|-------|\n"
            
            for key, value in perf.items():
                if isinstance(value, (int, float)):
                    report += f"| {key} | {value:.4f} |\n"
                else:
                    report += f"| {key} | {value} |\n"
            
            report += "\n"
        
        if 'spike_analysis' in results:
            report += "## Spike Analysis\n\n"
            spike = results['spike_analysis']
            
            report += "### Statistics\n\n"
            report += f"- Total Spikes: {spike.get('total_spikes', 'N/A')}\n"
            report += f"- Mean Interval: {spike.get('mean_interval', 'N/A'):.4f}\n"
            report += f"- Std Interval: {spike.get('std_interval', 'N/A'):.4f}\n"
            report += f"- Burst Count: {spike.get('burst_count', 'N/A')}\n"
            report += f"- Refractory Violations: {spike.get('refractory_violations', 'N/A')}\n\n"
        
        if 'memory_analysis' in results:
            report += "## Memory Analysis\n\n"
            memory = results['memory_analysis']
            
            report += "### Usage Statistics\n\n"
            if 'system_memory' in memory:
                sys_mem = memory['system_memory']
                report += "#### System Memory\n\n"
                report += f"- Mean Usage: {sys_mem.get('mean_percent', 'N/A'):.1f}%\n"
                report += f"- Max Usage: {sys_mem.get('max_percent', 'N/A'):.1f}%\n"
                report += f"- Min Usage: {sys_mem.get('min_percent', 'N/A'):.1f}%\n\n"
        
        if 'neural_activity' in results:
            report += "## Neural Activity Analysis\n\n"
            neural = results['neural_activity']
            
            if 'brain_stats' in neural:
                stats = neural['brain_stats']
                report += "### Brain Statistics\n\n"
                report += f"- Total Neurons: {stats.get('total_neurons', 'N/A')}\n"
                report += f"- Total Synapses: {stats.get('total_synapses', 'N/A')}\n"
                report += f"- Average Firing Rate: {stats.get('average_firing_rate', 'N/A'):.4f}\n"
                report += f"- E/I Ratio: {stats.get('excitation_inhibition_ratio', 'N/A'):.4f}\n\n"
        
        return report
    
    def _generate_html_report(self, results: Dict[str, Any]) -> str:
        """Generate HTML analysis report."""
        html = f"""<!DOCTYPE html>
<html>
<head>
    <title>NLM Analysis Report</title>
    <style>
        body {{ font-family: Arial, sans-serif; margin: 40px; }}
        h1 {{ color: #2c3e50; }}
        h2 {{ color: #34495e; border-bottom: 2px solid #3498db; padding-bottom: 5px; }}
        table {{ border-collapse: collapse; width: 100%; margin: 20px 0; }}
        th, td {{ border: 1px solid #ddd; padding: 8px; text-align: left; }}
        th {{ background-color: #f2f2f2; }}
        .metric {{ background-color: #f9f9f9; padding: 10px; margin: 10px 0; }}
    </style>
</head>
<body>
    <h1>NLM Analysis Report</h1>
    <p>Generated on: {time.ctime()}</p>
    
"""
        
        if 'performance' in results:
            html += """
    <h2>Performance Metrics</h2>
    <table>
        <tr><th>Metric</th><th>Value</th></tr>
"""
            for key, value in results['performance'].items():
                html += f"        <tr><td>{key}</td><td>{value:.4f if isinstance(value, float) else value}</td></tr>\n"
            html += "    </table>\n"
        
        html += "</body>\n</html>"
        return html


class NLMProfiler:
    """Comprehensive profiling for NLM applications."""
    
    def __init__(self):
        self.profiling = False
        self.profile_data = {
            'function_calls': [],
            'memory_usage': [],
            'time_points': []
        }
    
    def start_profiling(self, interval: float = 0.1):
        """Start profiling."""
        self.profiling = True
        self.profile_thread = threading.Thread(
            target=self._profiling_loop,
            args=(interval,),
            daemon=True
        )
        self.profile_thread.start()
    
    def stop_profiling(self):
        """Stop profiling."""
        self.profiling = False
        if hasattr(self, 'profile_thread'):
            self.profile_thread.join(timeout=1.0)
    
    def _profiling_loop(self, interval: float):
        """Profiling loop."""
        while self.profiling:
            sample = self._collect_profile_sample()
            if sample:
                for key, value in sample.items():
                    self.profile_data[key].append(value)
            time.sleep(interval)
    
    def _collect_profile_sample(self):
        """Collect profiling sample."""
        try:
            import psutil
            import pynlm
            
            cpu_percent = psutil.cpu_percent(interval=0.01)
            memory = psutil.virtual_memory()
            
            nlm_metrics = {}
            if PYNLM_AVAILABLE:
                try:
                    pass
                except:
                    pass
            
            return {
                'timestamp': time.time(),
                'cpu_percent': cpu_percent,
                'memory_percent': memory.percent,
                'nlm_metrics': nlm_metrics
            }
        except Exception as e:
            print(f"Profiling error: {e}")
            return None
    
    def get_profile_statistics(self) -> Dict[str, Any]:
        """Get profiling statistics."""
        stats = {}
        
        if self.profile_data['cpu_percent']:
            cpu_samples = self.profile_data['cpu_percent']
            stats['cpu'] = {
                'mean': np.mean(cpu_samples),
                'max': np.max(cpu_samples),
                'min': np.min(cpu_samples),
                'std': np.std(cpu_samples)
            }
        
        if self.profile_data['memory_percent']:
            memory_samples = self.profile_data['memory_percent']
            stats['memory'] = {
                'mean': np.mean(memory_samples),
                'max': np.max(memory_samples),
                'min': np.min(memory_samples),
                'std': np.std(memory_samples)
            }
        
        return stats
    
    def create_profile_visualization(self, output_file: str):
        """Create profile visualization."""
        stats = self.get_profile_statistics()
        if not stats:
            print("No profile data to visualize")
            return
        
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        fig.suptitle("NLM Profiling Results", fontsize=16)
        
        if self.profile_data['timestamp']:
            timestamps = self.profile_data['timestamp']
            
            if self.profile_data['cpu_percent']:
                axes[0, 0].plot(timestamps, self.profile_data['cpu_percent'], 'b-', alpha=0.7)
                axes[0, 0].set_xlabel('Time')
                axes[0, 0].set_ylabel('CPU %')
                axes[0, 0].set_title('CPU Usage Over Time')
                axes[0, 0].grid(True, alpha=0.3)
            
            if self.profile_data['memory_percent']:
                axes[0, 1].plot(timestamps, self.profile_data['memory_percent'], 'r-', alpha=0.7)
                axes[0, 1].set_xlabel('Time')
                axes[0, 1].set_ylabel('Memory %')
                axes[0, 1].set_title('Memory Usage Over Time')
                axes[0, 1].grid(True, alpha=0.3)
        
        axes[1, 0].axis('off')
        summary_text = f"""Profiling Summary:
        
        Total Samples: {len(self.profile_data['timestamp']) if self.profile_data['timestamp'] else 0}
        
        CPU Statistics:
          Mean: {stats.get('cpu', {}).get('mean', 0):.1f}%
          Max: {stats.get('cpu', {}).get('max', 0):.1f}%
          Min: {stats.get('cpu', {}).get('min', 0):.1f}%
          Std: {stats.get('cpu', {}).get('std', 0):.1f}%
        
        Memory Statistics:
          Mean: {stats.get('memory', {}).get('mean', 0):.1f}%
          Max: {stats.get('memory', {}).get('max', 0):.1f}%
          Min: {stats.get('memory', {}).get('min', 0):.1f}%
          Std: {stats.get('memory', {}).get('std', 0):.1f}%
        """
        axes[1, 0].text(0.1, 0.5, summary_text, fontsize=10,
                      verticalalignment='center', fontfamily='monospace')
        
        plt.tight_layout()
        plt.savefig(output_file, dpi=150, bbox_inches='tight')
        plt.close()


# Main debugging and visualization tools

class NLMDebugTools:
    """Main interface for NLM debugging and visualization tools."""
    
    def __init__(self):
        self.performance_monitor = PerformanceMonitor()
        self.spike_analyzer = SpikeAnalyzer()
        self.memory_monitor = MemoryMonitor()
        self.data_exporter = DataExporter()
        self.profiler = NLMProfiler()
        self.current_brain = None
    
    def set_brain(self, brain):
        """Set the current brain for analysis."""
        self.current_brain = brain
    
    def collect_spike_data(self, spike_threshold: float = 0.5, analysis_window: int = 1000):
        """Collect spike data from current brain."""
        if not self.current_brain:
            raise Exception("No brain set. Use set_brain() first.")
        
        spike_times = []
        neuron_ids = []
        
        if spike_times and neuron_ids:
            self.spike_analyzer.add_spike_data(spike_times, neuron_ids)
            print(f"Collected {len(spike_times)} spikes from brain")
        else:
            print("No spikes collected")
    
    def run_performance_profiling(self, duration: float = 30.0):
        """Run performance profiling."""
        print(f"Starting performance profiling for {duration} seconds...")
        self.performance_monitor.start_monitoring(0.1)
        time.sleep(duration)
        self.performance_monitor.stop_monitoring()
        
        print("Profiling completed")
        return self.performance_monitor.get_statistics()
    
    def analyze_spike_patterns(self):
        """Analyze spike patterns from collected data."""
        return self.spike_analyzer.analyze_spike_patterns()
    
    def start_memory_monitoring(self, duration: float = 30.0):
        """Start memory monitoring."""
        print(f"Starting memory monitoring for {duration} seconds...")
        self.memory_monitor.start_memory_monitoring(0.5)
        time.sleep(duration)
        self.memory_monitor.stop_memory_monitoring()
        
        print("Memory monitoring completed")
        return self.memory_monitor.get_memory_statistics()
    
    def create_spike_raster(self, output_file: str):
        """Create spike raster plot."""
        self.spike_analyzer.create_spike_raster(output_file)
    
    def create_spike_histogram(self, output_file: str):
        """Create spike histogram."""
        self.spike_analyzer.create_spike_histogram(output_file)
    
    def create_performance_plot(self, output_file: str):
        """Create performance monitoring plot."""
        self.performance_monitor.plot_performance(output_file)
    
    def create_memory_plot(self, output_file: str):
        """Create memory usage plot."""
        self.memory_monitor.plot_memory_usage(output_file)
    
    def export_results(self, results: Dict[str, Any], output_file: str, format: str = "json"):
        """Export analysis results."""
        if format == "json":
            self.data_exporter.export_to_json(results, output_file)
        elif format == "csv":
            self.data_exporter.export_to_csv(results, output_file)
        elif format == "matlab":
            self.data_exporter.export_to_matlab(results, output_file)
        elif format == "markdown":
            self.data_exporter.export_analysis_report(results, output_file, format)
        elif format == "html":
            self.data_exporter.export_analysis_report(results, output_file, format)
    
    def create_comprehensive_report(self, output_file: str = "nlm_analysis_report.html"):
        """Create comprehensive analysis report."""
        report_data = {
            'timestamp': time.ctime(),
            'performance': self.performance_monitor.get_statistics(),
            'spike_analysis': self.analyze_spike_patterns(),
            'memory_analysis': self.memory_monitor.get_memory_statistics(),
            'neural_activity': self._get_neural_activity_stats() if self.current_brain else {}
        }
        
        self.data_exporter.export_analysis_report(report_data, output_file, "html")
        print(f"Comprehensive report saved to: {output_file}")
    
    def _get_neural_activity_stats(self):
        """Get neural activity statistics from current brain."""
        if not self.current_brain:
            return {}
        
        stats = {
            'brain_stats': {
                'total_neurons': self.current_brain.getTotalNeuronCount(),
                'total_synapses': self.current_brain.getTotalSynapseCount(),
                'average_firing_rate': self.current_brain.getAverageFiringRate(),
                'excitation_inhibition_ratio': self.current_brain.getExcitationInhibitionRatio(),
            }
        }
        
        if self.current_brain.getWorkingMemory():
            stats['working_memory'] = {
                'active_traces': self.current_brain.getWorkingMemory().getActiveTraces()
            }
        
        if self.current_brain.getEpisodicMemory():
            stats['episodic_memory'] = {
                'episode_count': self.current_brain.getEpisodicMemory().getEpisodeCount()
            }
        
        return stats


# Main execution functions for CLI

def main():
    """Main entry point for NLM debugging and visualization tools."""
    parser = argparse.ArgumentParser(
        description="NLM Advanced Debugging and Visualization Tools",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm-debug --spike-analysis --output spike_analysis.html
  nlm-debug --performance-profile --duration 60 --output profile.png
  nlm-debug --memory-monitor --duration 30 --output memory_plot.png
  nlm-debug --spike-raster --output raster.png
  nlm-debug --spike-histogram --output histogram.png
  nlm-debug --export-results --format json --output results.json
  nlm-debug --comprehensive-report --output full_report.html
        """
    )
    
    parser.add_argument(
        "--config",
        default="nlm_config.json",
        help="Configuration file path (default: nlm_config.json)"
    )
    parser.add_argument(
        "--verbose",
        action="store_true",
        help="Enable verbose output"
    )
    parser.add_argument(
        "--quiet",
        action="store_true",
        help="Suppress non-essential output"
    )
    parser.add_argument(
        "--log-level",
        choices=["trace", "debug", "info", "warn", "error"],
        default="info",
        help="Set logging level (default: info)"
    )
    parser.add_argument(
        "--version",
        action="version",
        version="NLM Debug Tools 0.1.0"
    )
    
    subparsers = parser.add_subparsers(dest="command", help="Available commands")
    
    setup_debug_subparsers(subparsers)
    
    args = parser.parse_args()
    
    setup_logging(args.log_level)
    
    debug_tools = NLMDebugTools()
    
    if args.command == "spike-analysis":
        print("Spike analysis command (stub)")
    
    elif args.command == "performance-profile":
        stats = debug_tools.run_performance_profiling()
        print("Performance profiling completed")
        print(f"Statistics: {stats}")
    
    elif args.command == "memory-monitor":
        stats = debug_tools.start_memory_monitoring()
        print("Memory monitoring completed")
        print(f"Statistics: {stats}")
    
    elif args.command == "spike-raster":
        debug_tools.create_spike_raster("spike_raster.png")
    
    elif args.command == "spike-histogram":
        debug_tools.create_spike_histogram("spike_histogram.png")
    
    elif args.command == "performance-plot":
        debug_tools.create_performance_plot("performance_plot.png")
    
    elif args.command == "memory-plot":
        debug_tools.create_memory_plot("memory_plot.png")
    
    elif args.command == "export-results":
        results = {
            'timestamp': time.ctime(),
            'performance': debug_tools.performance_monitor.get_statistics(),
            'spike_analysis': debug_tools.analyze_spike_patterns(),
            'memory_analysis': debug_tools.memory_monitor.get_memory_statistics()
        }
        debug_tools.export_results(results, args.output, args.format)
    
    elif args.command == "comprehensive-report":
        debug_tools.create_comprehensive_report("nlm_analysis_report.html")
    
    else:
        parser.error("No command specified or invalid command")


def setup_debug_subparsers(subparsers):
    """Setup debugging and visualization subparsers."""
    
    spike_parser = subparsers.add_parser(
        "spike-analysis",
        help="Analyze neural spike patterns"
    )
    spike_parser.add_argument("--spike-threshold", type=float, default=0.5, help="Spike detection threshold")
    spike_parser.add_argument("--analysis-window", type=int, default=1000, help="Analysis window size")
    spike_parser.add_argument("--output", help="Output file path")
    
    perf_parser = subparsers.add_parser(
        "performance-profile",
        help="Run performance profiling"
    )
    perf_parser.add_argument("--duration", type=float, default=30.0, help="Profiling duration in seconds")
    perf_parser.add_argument("--output", help="Output file path")
    
    mem_parser = subparsers.add_parser(
        "memory-monitor",
        help="Monitor memory usage"
    )
    mem_parser.add_argument("--duration", type=float, default=30.0, help="Monitoring duration in seconds")
    mem_parser.add_argument("--output", help="Output file path")
    
    raster_parser = subparsers.add_parser(
        "spike-raster",
        help="Create spike raster plot"
    )
    raster_parser.add_argument("--output", default="spike_raster.png", help="Output file path")
    
    hist_parser = subparsers.add_parser(
        "spike-histogram",
        help="Create spike histogram"
    )
    hist_parser.add_argument("--output", default="spike_histogram.png", help="Output file path")
    
    perf_plot_parser = subparsers.add_parser(
        "performance-plot",
        help="Create performance plot"
    )
    perf_plot_parser.add_argument("--output", default="performance_plot.png", help="Output file path")
    
    mem_plot_parser = subparsers.add_parser(
        "memory-plot",
        help="Create memory usage plot"
    )
    mem_plot_parser.add_argument("--output", default="memory_plot.png", help="Output file path")
    
    export_parser = subparsers.add_parser(
        "export-results",
        help="Export analysis results"
    )
    export_parser.add_argument("--output", required=True, help="Output file path")
    export_parser.add_argument("--format", choices=["json", "csv", "matlab", "markdown", "html"], 
                             default="json", help="Export format")
    
    report_parser = subparsers.add_parser(
        "comprehensive-report",
        help="Create comprehensive analysis report"
    )
    report_parser.add_argument("--output", default="nlm_analysis_report.html", help="Output file path")


def setup_logging(log_level: str = "info"):
    """Setup logging configuration."""
    import logging
    
    level_map = {
        'trace': logging.DEBUG,
        'debug': logging.DEBUG,
        'info': logging.INFO,
        'warn': logging.WARNING,
        'error': logging.ERROR
    }
    
    level = level_map.get(log_level.lower(), logging.INFO)
    
    logging.basicConfig(
        level=level,
        format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
        handlers=[logging.StreamHandler(sys.stdout)]
    )


def main():
    """Main entry point for NLM debugging and visualization tools."""
    parser = argparse.ArgumentParser(
        description="NLM Advanced Debugging and Visualization Tools",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm-debug --spike-analysis --output spike_analysis.html
  nlm-debug --performance-profile --duration 60 --output profile.png
  nlm-debug --memory-monitor --duration 30 --output memory_plot.png
  nlm-debug --spike-raster --output raster.png
  nlm-debug --spike-histogram --output histogram.png
  nlm-debug --export-results --format json --output results.json
  nlm-debug --comprehensive-report --output full_report.html
        """
    )
    
    parser.add_argument(
        "--config",
        default="nlm_config.json",
        help="Configuration file path (default: nlm_config.json)"
    )
    parser.add_argument(
        "--verbose",
        action="store_true",
        help="Enable verbose output"
    )
    parser.add_argument(
        "--quiet",
        action="store_true",
        help="Suppress non-essential output"
    )
    parser.add_argument(
        "--log-level",
        choices=["trace", "debug", "info", "warn", "error"],
        default="info",
        help="Set logging level (default: info)"
    )
    parser.add_argument(
        "--version",
        action="version",
        version="NLM Debug Tools 0.1.0"
    )
    
    subparsers = parser.add_subparsers(dest="command", help="Available commands")
    
    setup_debug_subparsers(subparsers)
    
    args = parser.parse_args()
    
    setup_logging(args.log_level)
    
    debug_tools = NLMDebugTools()
    
    if args.command == "spike-analysis":
        print("Spike analysis command (stub)")
    
    elif args.command == "performance-profile":
        stats = debug_tools.run_performance_profiling()
        print("Performance profiling completed")
        print(f"Statistics: {stats}")
    
    elif args.command == "memory-monitor":
        stats = debug_tools.start_memory_monitoring()
        print("Memory monitoring completed")
        print(f"Statistics: {stats}")
    
    elif args.command == "spike-raster":
        debug_tools.create_spike_raster("spike_raster.png")
    
    elif args.command == "spike-histogram":
        debug_tools.create_spike_histogram("spike_histogram.png")
    
    elif args.command == "performance-plot":
        debug_tools.create_performance_plot("performance_plot.png")
    
    elif args.command == "memory-plot":
        debug_tools.create_memory_plot("memory_plot.png")
    
    elif args.command == "export-results":
        results = {
            'timestamp': time.ctime(),
            'performance': debug_tools.performance_monitor.get_statistics(),
            'spike_analysis': debug_tools.analyze_spike_patterns(),
            'memory_analysis': debug_tools.memory_monitor.get_memory_statistics()
        }
        debug_tools.export_results(results, args.output, args.format)
    
    elif args.command == "comprehensive-report":
        debug_tools.create_comprehensive_report("nlm_analysis_report.html")
    
    else:
        parser.error("No command specified or invalid command")