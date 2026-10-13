"""
Resource monitoring for NLM neural simulation framework.

This module provides system resource monitoring for memory, CPU, and I/O tracking
to help identify resource bottlenecks and optimize NLM simulation performance.
"""

import time
import psutil
import os
from typing import Dict, List, Any, Optional
import threading
from datetime import datetime

from .enhanced_api import NLMError

class ResourceSnapshot:
    """Container for system resource snapshot"""
    
    def __init__(self):
        self.timestamp = datetime.now()
        self.memory_info: Dict[str, Any] = {}
        self.cpu_info: Dict[str, Any] = {}
        self.disk_info: Dict[str, Any] = {}
        self.network_info: Dict[str, Any] = {}
        self.process_info: Dict[str, Any] = {}
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert resource snapshot to dictionary"""
        return {
            'timestamp': self.timestamp.isoformat(),
            'memory_info': self.memory_info,
            'cpu_info': self.cpu_info,
            'disk_info': self.disk_info,
            'network_info': self.network_info,
            'process_info': self.process_info,
        }

class ResourceMonitor:
    """
    System resource monitoring for NLM simulations.
    
    Tracks and analyzes system resources including memory, CPU, disk I/O, and
    network usage to help identify resource bottlenecks and optimize performance.
    """
    
    def __init__(self, brain=None):
        """Initialize resource monitor with optional brain reference"""
        self.brain = brain
        self.snapshots: List[ResourceSnapshot] = []
        self.monitoring = False
        self.monitor_thread = None
        self.collection_interval = 1.0  # seconds
        self.max_snapshots = 10000
        
        # Resource tracking
        self.memory_history: List[Dict[str, Any]] = []
        self.cpu_history: List[Dict[str, Any]] = []
        self.disk_history: List[Dict[str, Any]] = []
        self.network_history: List[Dict[str, Any]] = []
    
    def configure(self, collection_interval: float = 1.0, max_snapshots: int = 10000):
        """Configure resource monitoring settings"""
        self.collection_interval = collection_interval
        self.max_snapshots = max_snapshots
        return self
    
    def start_monitoring(self):
        """Start continuous resource monitoring"""
        if self.monitoring:
            return
        
        self.monitoring = True
        self.monitor_thread = threading.Thread(target=self._monitoring_loop, daemon=True)
        self.monitor_thread.start()
        
        print(f"Resource monitoring started (interval: {self.collection_interval}s)")
        return self
    
    def stop_monitoring(self):
        """Stop continuous resource monitoring"""
        self.monitoring = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=5.0)
            self.monitor_thread = None
        
        print("Resource monitoring stopped")
        return self
    
    def _monitoring_loop(self):
        """Continuous monitoring loop"""
        while self.monitoring:
            try:
                self._collect_resources()
                time.sleep(self.collection_interval)
            except Exception as e:
                print(f"Warning: Resource monitoring error: {e}")
                time.sleep(self.collection_interval)
    
    def _collect_resources(self):
        """Collect current system resource usage"""
        snapshot = ResourceSnapshot()
        
        # Collect memory information
        try:
            memory = psutil.virtual_memory()
            swap = psutil.swap_memory()
            
            snapshot.memory_info = {
                'total_memory_gb': memory.total / (1024**3),
                'available_memory_gb': memory.available / (1024**3),
                'used_memory_gb': memory.used / (1024**3),
                'memory_usage_percent': memory.percent,
                'total_swap_gb': swap.total / (1024**3),
                'used_swap_gb': swap.used / (1024**3),
                'swap_usage_percent': swap.percent,
            }
        except Exception as e:
            snapshot.memory_info['error'] = str(e)
        
        # Collect CPU information
        try:
            cpu_percent = psutil.cpu_percent(interval=0)
            cpu_freq = psutil.cpu_freq()
            
            snapshot.cpu_info = {
                'cpu_percent': cpu_percent,
                'cpu_freq_mhz': cpu_freq.current if cpu_freq else 0,
                'cpu_count': psutil.cpu_count(),
                'cpu_count_logical': psutil.cpu_count(logical=True),
                'load_avg_1min': psutil.getloadavg()[0] if hasattr(psutil, 'getloadavg') else 0,
                'load_avg_5min': psutil.getloadavg()[1] if hasattr(psutil, 'getloadavg') else 0,
                'load_avg_15min': psutil.getloadavg()[2] if hasattr(psutil, 'getloadavg') else 0,
            }
        except Exception as e:
            snapshot.cpu_info['error'] = str(e)
        
        # Collect disk information
        try:
            disk_usage = psutil.disk_usage('/')
            disk_io = psutil.disk_io_counters() if psutil.disk_io_counters() else {}
            
            snapshot.disk_info = {
                'total_disk_gb': disk_usage.total / (1024**3),
                'used_disk_gb': disk_usage.used / (1024**3),
                'free_disk_gb': disk_usage.free / (1024**3),
                'disk_usage_percent': disk_usage.percent,
                'disk_read_bytes': getattr(disk_io, 'read_bytes', 0),
                'disk_write_bytes': getattr(disk_io, 'write_bytes', 0),
                'disk_read_count': getattr(disk_io, 'read_count', 0),
                'disk_write_count': getattr(disk_io, 'write_count', 0),
            }
        except Exception as e:
            snapshot.disk_info['error'] = str(e)
        
        # Collect network information
        try:
            net_io = psutil.net_io_counters() if psutil.net_io_counters() else {}
            net_connections = len(psutil.net_connections()) if psutil.net_connections() else 0
            
            snapshot.network_info = {
                'bytes_sent': getattr(net_io, 'bytes_sent', 0),
                'bytes_recv': getattr(net_io, 'bytes_recv', 0),
                'packets_sent': getattr(net_io, 'packets_sent', 0),
                'packets_recv': getattr(net_io, 'packets_recv', 0),
                'num_connections': net_connections,
                'error_in': getattr(net_io, 'errin', 0),
                'error_out': getattr(net_io, 'errout', 0),
            }
        except Exception as e:
            snapshot.network_info['error'] = str(e)
        
        # Collect process information
        try:
            process = psutil.Process()
            memory_info = process.memory_info()
            cpu_times = process.cpu_times()
            
            snapshot.process_info = {
                'process_id': process.pid,
                'process_name': process.name(),
                'memory_rss_mb': memory_info.rss / (1024**2),
                'memory_vms_mb': memory_info.vms / (1024**2),
                'cpu_time_user': cpu_times.user,
                'cpu_time_system': cpu_times.system,
                'num_threads': process.num_threads(),
                'num_fds': process.num_fds() if hasattr(process, 'num_fds') else 0,
                'create_time': process.create_time(),
                'cpu_percent': process.cpu_percent(interval=None),
            }
        except Exception as e:
            snapshot.process_info['error'] = str(e)
        
        # Add to snapshots list
        self.snapshots.append(snapshot)
        
        # Limit snapshot count
        if len(self.snapshots) > self.max_snapshots:
            self.snapshots = self.snapshots[-self.max_snapshots:]
    
    def track_resource_usage(self):
        """Track resource usage for current simulation step"""
        if not self.snapshots:  # Collect one snapshot if none exists
            self._collect_resources()
        
        # Update history
        if self.snapshots:
            latest = self.snapshots[-1]
            
            if latest.memory_info:
                self.memory_history.append({
                    'timestamp': latest.timestamp,
                    'memory_gb': latest.memory_info.get('used_memory_gb', 0),
                    'memory_percent': latest.memory_info.get('memory_usage_percent', 0),
                })
            
            if latest.cpu_info:
                self.cpu_history.append({
                    'timestamp': latest.timestamp,
                    'cpu_percent': latest.cpu_info.get('cpu_percent', 0),
                })
            
            if latest.disk_info:
                self.disk_history.append({
                    'timestamp': latest.timestamp,
                    'disk_gb': latest.disk_info.get('used_disk_gb', 0),
                    'disk_percent': latest.disk_info.get('disk_usage_percent', 0),
                })
            
            if latest.network_info:
                self.network_history.append({
                    'timestamp': latest.timestamp,
                    'bytes_sent': latest.network_info.get('bytes_sent', 0),
                    'bytes_recv': latest.network_info.get('bytes_recv', 0),
                })
    
    def get_memory_usage(self) -> Dict[str, Any]:
        """Get current memory usage statistics"""
        if not self.snapshots:
            return {'error': 'No resource snapshots available'}
        
        latest = self.snapshots[-1]
        return latest.memory_info
    
    def get_cpu_usage(self) -> Dict[str, Any]:
        """Get current CPU usage statistics"""
        if not self.snapshots:
            return {'error': 'No resource snapshots available'}
        
        latest = self.snapshots[-1]
        return latest.cpu_info
    
    def get_iow_usage(self) -> Dict[str, Any]:
        """Get I/O usage statistics"""
        if not self.snapshots:
            return {'error': 'No resource snapshots available'}
        
        latest = self.snapshots[-1]
        return {
            'disk': latest.disk_info,
            'network': latest.network_info,
            'process': latest.process_info,
        }
    
    def get_resource_analysis(self) -> Dict[str, Any]:
        """Get comprehensive resource usage analysis"""
        if not self.snapshots:
            return {'error': 'No resource data available'}
        
        # Calculate statistics
        memory_stats = self._calculate_memory_stats()
        cpu_stats = self._calculate_cpu_stats()
        disk_stats = self._calculate_disk_stats()
        network_stats = self._calculate_network_stats()
        
        # Identify bottlenecks
        bottlenecks = self._identify_resource_bottlenecks()
        
        # Calculate efficiency metrics
        efficiency_metrics = self._calculate_efficiency_metrics()
        
        return {
            'time_series': {
                'memory': self.memory_history,
                'cpu': self.cpu_history,
                'disk': self.disk_history,
                'network': self.network_history,
            },
            'statistics': {
                'memory': memory_stats,
                'cpu': cpu_stats,
                'disk': disk_stats,
                'network': network_stats,
            },
            'bottlenecks': bottlenecks,
            'efficiency_metrics': efficiency_metrics,
            'recommendations': self._generate_resource_recommendations(),
        }
    
    def _calculate_memory_stats(self) -> Dict[str, Any]:
        """Calculate memory usage statistics"""
        if not self.memory_history:
            return {}
        
        memory_gb = [m['memory_gb'] for m in self.memory_history]
        memory_percent = [m['memory_percent'] for m in self.memory_history]
        
        return {
            'total_samples': len(self.memory_history),
            'avg_memory_gb': sum(memory_gb) / len(memory_gb),
            'min_memory_gb': min(memory_gb),
            'max_memory_gb': max(memory_gb),
            'avg_memory_percent': sum(memory_percent) / len(memory_percent),
            'memory_trend': self._calculate_trend([m['memory_gb'] for m in self.memory_history]),
            'memory_efficiency': self._calculate_memory_efficiency(),
        }
    
    def _calculate_cpu_stats(self) -> Dict[str, Any]:
        """Calculate CPU usage statistics"""
        if not self.cpu_history:
            return {}
        
        cpu_values = [c['cpu_percent'] for c in self.cpu_history]
        
        return {
            'total_samples': len(self.cpu_history),
            'avg_cpu_percent': sum(cpu_values) / len(cpu_values),
            'max_cpu_percent': max(cpu_values),
            'min_cpu_percent': min(cpu_values),
            'cpu_utilization_trend': self._calculate_trend(cpu_values),
            'cpu_efficiency': self._calculate_cpu_efficiency(),
        }
    
    def _calculate_disk_stats(self) -> Dict[str, Any]:
        """Calculate disk usage statistics"""
        if not self.disk_history:
            return {}
        
        disk_gb = [d['disk_gb'] for d in self.disk_history]
        
        return {
            'total_samples': len(self.disk_history),
            'avg_disk_gb': sum(disk_gb) / len(disk_gb),
            'min_disk_gb': min(disk_gb),
            'max_disk_gb': max(disk_gb),
            'disk_growth_gb': max(disk_gb) - min(disk_gb),
            'disk_trend': self._calculate_trend(disk_gb),
        }
    
    def _calculate_network_stats(self) -> Dict[str, Any]:
        """Calculate network usage statistics"""
        if not self.network_history:
            return {}
        
        bytes_sent = [n['bytes_sent'] for n in self.network_history]
        bytes_recv = [n['bytes_recv'] for n in self.network_history]
        
        return {
            'total_samples': len(self.network_history),
            'total_bytes_sent': max(bytes_sent) - min(bytes_sent),
            'total_bytes_recv': max(bytes_recv) - min(bytes_recv),
            'bandwidth_sent_mbps': (max(bytes_sent) - min(bytes_sent)) / (len(self.network_history) * self.collection_interval * 8 / (1024**2)),
            'bandwidth_recv_mbps': (max(bytes_recv) - min(bytes_recv)) / (len(self.network_history) * self.collection_interval * 8 / (1024**2)),
        }
    
    def _calculate_trend(self, values: List[float]) -> str:
        """Calculate trend of a time series"""
        if len(values) < 2:
            return "stable"
        
        # Simple linear trend detection
        n = len(values)
        x = list(range(n))
        y = values
        
        mean_x = sum(x) / n
        mean_y = sum(y) / n
        
        numerator = sum((x[i] - mean_x) * (y[i] - mean_y) for i in range(n))
        denominator = sum((x[i] - mean_x) ** 2 for i in range(n))
        
        if denominator == 0:
            return "stable"
        
        slope = numerator / denominator
        
        if abs(slope) < 0.01:
            return "stable"
        elif slope > 0:
            return "increasing"
        else:
            return "decreasing"
    
    def _calculate_memory_efficiency(self) -> float:
        """Calculate memory efficiency score"""
        if not self.memory_history:
            return 0.0
        
        memory_gb = [m['memory_gb'] for m in self.memory_history]
        max_memory_gb = max(memory_gb) if memory_gb else 1
        
        # Efficiency score based on memory usage relative to maximum
        avg_memory_gb = sum(memory_gb) / len(memory_gb)
        efficiency = 1.0 - (avg_memory_gb / max_memory_gb)
        
        return max(0.0, min(1.0, efficiency))
    
    def _calculate_cpu_efficiency(self) -> float:
        """Calculate CPU efficiency score"""
        if not self.cpu_history:
            return 0.0
        
        cpu_values = [c['cpu_percent'] for c in self.cpu_history]
        avg_cpu = sum(cpu_values) / len(cpu_values)
        
        # Efficiency score based on CPU utilization
        efficiency = 1.0 - (avg_cpu / 100.0)
        
        return max(0.0, min(1.0, efficiency))
    
    def _identify_resource_bottlenecks(self) -> List[Dict[str, Any]]:
        """Identify resource bottlenecks"""
        bottlenecks = []
        
        memory_stats = self._calculate_memory_stats()
        cpu_stats = self._calculate_cpu_stats()
        
        # Memory bottleneck
        if memory_stats.get('avg_memory_percent', 0) > 80:
            bottlenecks.append({
                'type': 'memory',
                'severity': 'high',
                'description': f'Average memory usage ({memory_stats.get("avg_memory_percent", 0):.1f}%) is high',
                'recommendation': 'Consider reducing memory allocation or implementing memory pools',
            })
        
        # CPU bottleneck
        if cpu_stats.get('avg_cpu_percent', 0) > 80:
            bottlenecks.append({
                'type': 'cpu',
                'severity': 'high',
                'description': f'Average CPU utilization ({cpu_stats.get("avg_cpu_percent", 0):.1f}%) is high',
                'recommendation': 'Consider optimizing computational complexity or enabling parallel processing',
            })
        
        # Memory trend bottleneck
        if memory_stats.get('memory_trend') == 'increasing':
            bottlenecks.append({
                'type': 'memory_growth',
                'severity': 'medium',
                'description': 'Memory usage is increasing over time',
                'recommendation': 'Check for memory leaks and implement proper memory management',
            })
        
        # CPU trend bottleneck
        if cpu_stats.get('cpu_utilization_trend') == 'increasing':
            bottlenecks.append({
                'type': 'cpu_growth',
                'severity': 'medium',
                'description': 'CPU utilization is increasing over time',
                'recommendation': 'Consider optimizing algorithms or reducing computational complexity',
            })
        
        return bottlenecks
    
    def _calculate_efficiency_metrics(self) -> Dict[str, Any]:
        """Calculate resource efficiency metrics"""
        memory_stats = self._calculate_memory_stats()
        cpu_stats = self._calculate_cpu_stats()
        
        return {
            'memory_efficiency': memory_stats.get('memory_efficiency', 0.0),
            'cpu_efficiency': cpu_stats.get('cpu_efficiency', 0.0),
            'overall_efficiency': (memory_stats.get('memory_efficiency', 0.0) + 
                                  cpu_stats.get('cpu_efficiency', 0.0)) / 2,
            'resource_utilization': (memory_stats.get('avg_memory_percent', 0) +
                                   cpu_stats.get('avg_cpu_percent', 0)) / 2,
        }
    
    def _generate_resource_recommendations(self) -> List[str]:
        """Generate resource optimization recommendations"""
        recommendations = []
        
        bottlenecks = self._identify_resource_bottlenecks()
        for bottleneck in bottlenecks:
            recommendations.append(bottleneck['recommendation'])
        
        if not recommendations:
            recommendations.append("Resource usage is optimal. Consider further optimizations for specific workloads.")
        
        return recommendations
    
    def export_resource_data(self, filepath: str):
        """Export resource data to file"""
        try:
            data = self.get_resource_analysis()
            
            with open(filepath, 'w') as f:
                json.dump(data, f, indent=2, default=str)
            
            print(f"Resource data exported to {filepath}")
            
        except Exception as e:
            raise NLMError(f"Error exporting resource data: {e}")
