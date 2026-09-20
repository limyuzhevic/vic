#!/usr/bin/env python3
"""
System Management Commands for NLM CLI

Handles system resource monitoring, status checks, plugin management, and alerts.
"""

import json
import time
import psutil
import sys
import platform
from pathlib import Path
from typing import Dict, Any, Optional, List
from datetime import datetime

import click
from rich.console import Console
from rich.table import Table
from rich.syntax import Syntax
from rich.progress import Progress, SpinnerColumn, TextColumn, BarColumn

from nlm_cli import NLMCLI

console = Console()

class SystemCommands:
    """Handle system management commands."""
    
    def __init__(self, nlm_cli: NLMCLI):
        self.nlm_cli = nlm_cli
    
    def handle(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Handle system commands."""
        command = args.get('command')
        
        handlers = {
            'status': self.get_system_status,
            'resources': self.get_system_resources,
            'processes': self.get_process_info,
            'disk': self.get_disk_info,
            'network': self.get_network_info,
            'memory': self.get_memory_info,
            'cpu': self.get_cpu_info,
            'alerts': self.get_system_alerts,
            'setup': self.setup_monitoring,
            'check': self.check_system_health,
            'dependencies': self.check_dependencies,
            'install': self.install_dependencies,
            'plugins': self.manage_plugins,
            'logs': self.get_system_logs,
        }
        
        if command in handlers:
            return handlers[command](args)
        else:
            raise click.ClickException(f"Unknown system command: {command}")
    
    def get_system_status(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Get overall system status."""
        status_info = {
            'timestamp': datetime.now().isoformat(),
            'overall_status': 'healthy',
            'components': {}
        }
        
        # Check each system component
        status_info['components']['cpu'] = self._check_cpu_status()
        status_info['components']['memory'] = self._check_memory_status()
        status_info['components']['disk'] = self._check_disk_status()
        status_info['components']['network'] = self._check_network_status()
        status_info['components']['processes'] = self._check_process_status()
        
        # Determine overall status
        for component, status in status_info['components'].items():
            if status.get('status') in ['error', 'critical']:
                status_info['overall_status'] = 'unhealthy'
                break
            elif status.get('status') == 'warning' and status_info['overall_status'] == 'healthy':
                status_info['overall_status'] = 'warning'
        
        # Display status
        self._display_system_status(status_info)
        
        return {
            'success': True,
            'message': f"System status retrieved. Overall status: {status_info['overall_status']}",
            'system_status': status_info
        }
    
    def get_system_resources(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Get current system resource usage."""
        resources = {
            'timestamp': datetime.now().isoformat(),
            'cpu': self._get_cpu_resources(),
            'memory': self._get_memory_resources(),
            'disk': self._get_disk_resources(),
            'network': self._get_network_resources(),
            'processes': self._get_process_resources()
        }
        
        # Display resources
        self._display_system_resources(resources)
        
        return {
            'success': True,
            'message': "System resources retrieved.",
            'system_resources': resources
        }
    
    def get_process_info(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Get process information."""
        process_name = args.get('name')
        include_children = args.get('include_children', False)
        
        processes = []
        
        if process_name:
            # Filter by name
            for proc in psutil.process_iter(['pid', 'name', 'cpu_percent', 'memory_percent', 'status']):
                try:
                    if proc.info['name'] and process_name.lower() in proc.info['name'].lower():
                        process_info = self._get_process_details(proc, include_children)
                        processes.append(process_info)
                except (psutil.NoSuchProcess, psutil.AccessDenied):
                    continue
        else:
            # Get all processes (top N)
            top_n = args.get('top_n', 10)
            for proc in psutil.process_iter(['pid', 'name', 'cpu_percent', 'memory_percent', 'status']):
                try:
                    process_info = self._get_process_details(proc, include_children)
                    processes.append(process_info)
                except (psutil.NoSuchProcess, psutil.AccessDenied):
                    continue
            
            # Sort by CPU usage and take top N
            processes.sort(key=lambda x: x.get('cpu_percent', 0), reverse=True)
            processes = processes[:top_n]
        
        return {
            'success': True,
            'message': f"Found {len(processes)} processes.",
            'processes': processes
        }
    
    def get_disk_info(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Get disk information."""
        disks = []
        
        for partition in psutil.disk_partitions():
            try:
                usage = psutil.disk_usage(partition.mountpoint)
                disk_info = {
                    'device': partition.device,
                    'mountpoint': partition.mountpoint,
                    'filesystem': partition.fstype,
                    'total_gb': usage.total / (1024**3),
                    'used_gb': usage.used / (1024**3),
                    'free_gb': usage.free / (1024**3),
                    'used_percent': usage.percent,
                    'read_bytes_per_sec': 0,
                    'write_bytes_per_sec': 0
                }
                
                # Get I/O stats if available
                try:
                    io_counters = psutil.disk_io_counters(partition.device)
                    if io_counters:
                        disk_info['read_bytes_per_sec'] = io_counters.read_bytes
                        disk_info['write_bytes_per_sec'] = io_counters.write_bytes
                except (psutil.AccessDenied, FileNotFoundError):
                    pass
                
                disks.append(disk_info)
            except (psutil.AccessDenied, FileNotFoundError):
                continue
        
        # Display disk info
        self._display_disk_info(disks)
        
        return {
            'success': True,
            'message': f"Found {len(disks)} disk partitions.",
            'disks': disks
        }
    
    def get_network_info(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Get network information."""
        network_info = {
            'timestamp': datetime.now().isoformat(),
            'interfaces': [],
            'io_counters': psutil.net_io_counters()
        }
        
        for interface_name, interface in psutil.net_if_addrs().items():
            try:
                io = psutil.net_io_counters(interface_name)
                if interface:
                    interface_info = {
                        'name': interface_name,
                        'family': str(interface[0].family),
                        'type': str(interface[0].type),
                        'address': interface[0].address,
                        'netmask': interface[0].netmask if interface[0].netmask else None,
                        'broadcast': interface[0].broadcast if interface[0].broadcast else None,
                        'bytes_sent': io.bytes_sent,
                        'bytes_recv': io.bytes_recv,
                        'packets_sent': io.packets_sent,
                        'packets_recv': io.packets_recv,
                        'err_in': io.errin,
                        'err_out': io.errout,
                        'drop_in': io.dropin,
                        'drop_out': io.dropout
                    }
                    network_info['interfaces'].append(interface_info)
            except (psutil.AccessDenied, FileNotFoundError):
                continue
        
        # Display network info
        self._display_network_info(network_info)
        
        return {
            'success': True,
            'message': "Network information retrieved.",
            'network_info': network_info
        }
    
    def get_memory_info(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Get memory information."""
        memory_info = psutil.virtual_memory()
        swap_info = psutil.swap_memory()
        
        # Get memory by process
        process_memory = []
        for proc in psutil.process_iter(['pid', 'name', 'memory_info']):
            try:
                mem_info = proc.memory_info()
                process_memory.append({
                    'pid': proc.info['pid'],
                    'name': proc.info['name'],
                    'rss_mb': mem_info.rss / (1024**2),
                    'vms_mb': mem_info.vms / (1024**2)
                })
            except (psutil.NoSuchProcess, psutil.AccessDenied):
                continue
        
        # Sort by memory usage
        process_memory.sort(key=lambda x: x['rss_mb'], reverse=True)
        top_processes = process_memory[:10]
        
        memory_data = {
            'timestamp': datetime.now().isoformat(),
            'system_memory': {
                'total_gb': memory_info.total / (1024**3),
                'available_gb': memory_info.available / (1024**3),
                'used_gb': memory_info.used / (1024**3),
                'free_gb': memory_info.free / (1024**3),
                'used_percent': memory_info.percent
            },
            'swap_memory': {
                'total_gb': swap_info.total / (1024**3),
                'used_gb': swap_info.used / (1024**3),
                'free_gb': swap_info.free / (1024**3),
                'used_percent': swap_info.percent
            },
            'top_processes': top_processes
        }
        
        # Display memory info
        self._display_memory_info(memory_data)
        
        return {
            'success': True,
            'message': "Memory information retrieved.",
            'memory_info': memory_data
        }
    
    def get_cpu_info(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Get CPU information."""
        cpu_info = {
            'timestamp': datetime.now().isoformat(),
            'physical_cores': psutil.cpu_count(logical=False),
            'logical_cores': psutil.cpu_count(logical=True),
            'cpu_percent': psutil.cpu_percent(interval=1),
            'cpu_times': psutil.cpu_times(),
            'cpu_freq': psutil.cpu_freq(),
            'load_average': psutil.getloadavg() if hasattr(psutil, 'getloadavg') else None,
            'per_cpu_percent': psutil.cpu_percent(interval=1, percpu=True)
        }
        
        # Display CPU info
        self._display_cpu_info(cpu_info)
        
        return {
            'success': True,
            'message': "CPU information retrieved.",
            'cpu_info': cpu_info
        }
    
    def get_system_alerts(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Get system alerts and notifications."""
        alerts = []
        
        # Check for various alert conditions
        memory_info = psutil.virtual_memory()
        if memory_info.percent > 90:
            alerts.append({
                'type': 'memory',
                'severity': 'critical',
                'message': f"High memory usage: {memory_info.percent}%"
            })
        
        cpu_percent = psutil.cpu_percent(interval=0.1)
        if cpu_percent > 90:
            alerts.append({
                'type': 'cpu',
                'severity': 'warning',
                'message': f"High CPU usage: {cpu_percent}%"
            })
        
        # Check disk space
        for partition in psutil.disk_partitions():
            try:
                usage = psutil.disk_usage(partition.mountpoint)
                if usage.percent > 90:
                    alerts.append({
                        'type': 'disk',
                        'severity': 'warning',
                        'message': f"Low disk space on {partition.mountpoint}: {usage.percent}% used"
                    })
            except (psutil.AccessDenied, FileNotFoundError):
                continue
        
        # Check for processes with high memory usage
        for proc in psutil.process_iter(['pid', 'name', 'memory_percent']):
            try:
                if proc.info['memory_percent'] and proc.info['memory_percent'] > 50:
                    alerts.append({
                        'type': 'process',
                        'severity': 'warning',
                        'message': f"High memory usage by process {proc.info['name']} (PID: {proc.info['pid']})"
                    })
            except (psutil.NoSuchProcess, psutil.AccessDenied):
                continue
        
        # Display alerts
        self._display_alerts(alerts)
        
        return {
            'success': True,
            'message': f"Found {len(alerts)} system alerts.",
            'alerts': alerts
        }
    
    def setup_monitoring(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Set up system monitoring."""
        duration = args.get('duration', 3600)  # 1 hour default
        interval = args.get('interval', 60)   # 1 minute default
        
        monitoring_config = {
            'monitoring_enabled': True,
            'duration_seconds': duration,
            'interval_seconds': interval,
            'metrics_to_track': args.get('metrics', ['cpu', 'memory', 'disk']),
            'alert_thresholds': args.get('alert_thresholds', {
                'cpu': 90,
                'memory': 90,
                'disk': 90
            }),
            'setup_time': datetime.now().isoformat()
        }
        
        # Save monitoring configuration
        config_file = Path.home() / '.nlm' / 'monitoring_config.json'
        with open(config_file, 'w') as f:
            json.dump(monitoring_config, f, indent=2, default=str)
        
        # Start monitoring in background
        monitoring_thread = threading.Thread(
            target=self._monitor_system_resources,
            args=(duration, interval, monitoring_config),
            daemon=True
        )
        monitoring_thread.start()
        
        # Store monitoring thread
        if 'monitoring_threads' not in self.nlm_cli.context:
            self.nlm_cli.context['monitoring_threads'] = []
        
        self.nlm_cli.context['monitoring_threads'].append(monitoring_thread)
        
        return {
            'success': True,
            'message': f"System monitoring setup completed for {duration} seconds with {interval}-second intervals.",
            'monitoring_config': monitoring_config,
            'monitoring_thread': monitoring_thread
        }
    
    def check_system_health(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Check system health."""
        health_report = {
            'timestamp': datetime.now().isoformat(),
            'health_score': 100,
            'components': {}
        }
        
        # Check each component
        cpu_health = self._check_cpu_health()
        memory_health = self._check_memory_health()
        disk_health = self._check_disk_health()
        process_health = self._check_process_health()
        
        health_report['components'] = {
            'cpu': cpu_health,
            'memory': memory_health,
            'disk': disk_health,
            'processes': process_health
        }
        
        # Calculate overall health score
        scores = []
        for component, health in health_report['components'].items():
            scores.append(health.get('score', 0))
        
        if scores:
            health_report['health_score'] = sum(scores) / len(scores)
        
        # Determine health status
        if health_report['health_score'] >= 90:
            health_report['status'] = 'healthy'
        elif health_report['health_score'] >= 70:
            health_report['status'] = 'warning'
        else:
            health_report['status'] = 'unhealthy'
        
        # Display health report
        self._display_health_report(health_report)
        
        return {
            'success': True,
            'message': f"System health check completed. Overall health score: {health_report['health_score']:.1f}",
            'health_report': health_report
        }
    
    def check_dependencies(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Check system dependencies."""
        dependencies = [
            'psutil',
            'numpy',
            'matplotlib',
            'scipy',
            'scikit-learn',
            'networkx',
            'rich',
            'click'
        ]
        
        dependency_status = []
        
        for dep in dependencies:
            try:
                __import__(dep)
                dependency_status.append({
                    'name': dep,
                    'status': 'installed',
                    'version': self._get_package_version(dep)
                })
            except ImportError:
                dependency_status.append({
                    'name': dep,
                    'status': 'not_installed',
                    'version': None
                })
        
        # Display dependency status
        self._display_dependency_status(dependency_status)
        
        return {
            'success': True,
            'message': f"Checked {len(dependencies)} dependencies.",
            'dependency_status': dependency_status
        }
    
    def install_dependencies(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Install system dependencies."""
        dependencies = args.get('dependencies', ['psutil', 'numpy', 'matplotlib'])
        
        installed = []
        failed = []
        
        for dep in dependencies:
            try:
                # Try to import first
                __import__(dep)
                installed.append({
                    'name': dep,
                    'status': 'already_installed',
                    'action': 'skipped'
                })
            except ImportError:
                try:
                    # Try to install
                    subprocess.run([sys.executable, '-m', 'pip', 'install', dep], 
                                 check=True, capture_output=True, text=True)
                    installed.append({
                        'name': dep,
                        'status': 'installed',
                        'action': 'installed'
                    })
                except subprocess.CalledProcessError as e:
                    failed.append({
                        'name': dep,
                        'status': 'failed',
                        'error': e.stderr
                    })
        
        return {
            'success': len(failed) == 0,
            'message': f"Installed {len(installed)} dependencies, failed to install {len(failed)}.",
            'installed': installed,
            'failed': failed
        }
    
    def manage_plugins(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Manage system plugins."""
        action = args.get('action', 'list')
        
        if action == 'list':
            return self._list_system_plugins()
        elif action == 'update':
            return self._update_system_plugins()
        elif action == 'install':
            return self._install_system_plugins(args)
        else:
            raise click.ClickException(f"Unknown plugin action: {action}")
    
    def get_system_logs(self, args: Dict[str, Any]) -> Dict[str, Any]:n:
        """Get system logs."""
        log_file = args.get('log_file')
        lines = args.get('lines', 100)
        
        if log_file:
            # Read specific log file
            log_path = Path(log_file)
            if not log_path.exists():
                raise click.ClickException(f"Log file not found: {log_file}")
            
            with open(log_path, 'r') as f:
                log_content = f.readlines()[-lines:]
        else:
            # Get logs from NLM
            nlm_log_path = Path.home() / '.nlm' / 'logs'
            log_content = []
            
            if nlm_log_path.exists():
                for log_file in nlm_log_path.glob('*.log'):
                    with open(log_file, 'r') as f:
                        log_content.extend(f.readlines()[-lines:])
        
        return {
            'success': True,
            'message': f"Retrieved {len(log_content)} log lines.",
            'log_content': ''.join(log_content[-lines:]) if log_content else ''
        }
    
    # Helper methods
    
    def _check_cpu_status(self) -> Dict[str, Any]:
        """Check CPU status."""
        cpu_percent = psutil.cpu_percent(interval=0.1)
        
        if cpu_percent > 90:
            status = 'error'
            message = f"Critical CPU usage: {cpu_percent}%"
        elif cpu_percent > 70:
            status = 'warning'
            message = f"High CPU usage: {cpu_percent}%"
        else:
            status = 'ok'
            message = f"Normal CPU usage: {cpu_percent}%"
        
        return {
            'status': status,
            'cpu_percent': cpu_percent,
            'message': message
        }
    
    def _check_memory_status(self) -> Dict[str, Any]:
        """Check memory status."""
        memory = psutil.virtual_memory()
        
        if memory.percent > 90:
            status = 'error'
            message = f"Critical memory usage: {memory.percent}%"
        elif memory.percent > 70:
            status = 'warning'
            message = f"High memory usage: {memory.percent}%"
        else:
            status = 'ok'
            message = f"Normal memory usage: {memory.percent}%"
        
        return {
            'status': status,
            'memory_percent': memory.percent,
            'used_gb': memory.used / (1024**3),
            'message': message
        }
    
    def _check_disk_status(self) -> Dict[str, Any]:
        """Check disk status."""
        disks_status = []
        overall_status = 'ok'
        
        for partition in psutil.disk_partitions():
            try:
                usage = psutil.disk_usage(partition.mountpoint)
                
                if usage.percent > 90:
                    disk_status = 'error'
                    overall_status = 'error'
                elif usage.percent > 70:
                    disk_status = 'warning'
                    if overall_status == 'ok':
                        overall_status = 'warning'
                else:
                    disk_status = 'ok'
                
                disks_status.append({
                    'mountpoint': partition.mountpoint,
                    'status': disk_status,
                    'usage_percent': usage.percent
                })
            except (psutil.AccessDenied, FileNotFoundError):
                continue
        
        message = f"Checked {len(disks_status)} disks. Overall status: {overall_status}"
        
        return {
            'status': overall_status,
            'disks': disks_status,
            'message': message
        }
    
    def _check_network_status(self) -> Dict[str, Any]:
        """Check network status."""
        try:
            net_io = psutil.net_io_counters()
            
            # Check for network errors
            if net_io.errin > 100 or net_io.errout > 100:
                status = 'warning'
                message = f"Network errors: {net_io.errin} received, {net_io.errout} transmitted"
            else:
                status = 'ok'
                message = "Network status: OK"
        except (psutil.AccessDenied, FileNotFoundError):
            status = 'error'
            message = "Cannot access network statistics"
        
        return {
            'status': status,
            'message': message
        }
    
    def _check_process_status(self) -> Dict[str, Any]:
        """Check process status."""
        critical_processes = []
        
        # Check for processes with extremely high resource usage
        for proc in psutil.process_iter(['pid', 'name', 'cpu_percent', 'memory_percent']):
            try:
                if proc.info['cpu_percent'] and proc.info['cpu_percent'] > 95:
                    critical_processes.append({
                        'name': proc.info['name'],
                        'pid': proc.info['pid'],
                        'cpu_percent': proc.info['cpu_percent'],
                        'reason': 'High CPU usage'
                    })
                
                if proc.info['memory_percent'] and proc.info['memory_percent'] > 80:
                    critical_processes.append({
                        'name': proc.info['name'],
                        'pid': proc.info['pid'],
                        'memory_percent': proc.info['memory_percent'],
                        'reason': 'High memory usage'
                    })
            except (psutil.NoSuchProcess, psutil.AccessDenied):
                continue
        
        if critical_processes:
            status = 'error'
            message = f"Found {len(critical_processes)} processes with high resource usage"
        else:
            status = 'ok'
            message = "All processes are running normally"
        
        return {
            'status': status,
            'critical_processes': critical_processes,
            'message': message
        }
    
    def _get_cpu_resources(self) -> Dict[str, Any]:
        """Get CPU resources."""
        return {
            'cpu_percent': psutil.cpu_percent(interval=0.1),
            'cpu_freq': psutil.cpu_freq().current if psutil.cpu_freq() else None,
            'load_avg': psutil.getloadavg() if hasattr(psutil, 'getloadavg') else None
        }
    
    def _get_memory_resources(self) -> Dict[str, Any]:
        """Get memory resources."""
        memory = psutil.virtual_memory()
        swap = psutil.swap_memory()
        
        return {
            'total_gb': memory.total / (1024**3),
            'used_gb': memory.used / (1024**3),
            'available_gb': memory.available / (1024**3),
            'percent': memory.percent,
            'swap_total_gb': swap.total / (1024**3),
            'swap_used_gb': swap.used / (1024**3),
            'swap_percent': swap.percent
        }
    
    def _get_disk_resources(self) -> Dict[str, Any]:
        """Get disk resources."""
        disks = []
        
        for partition in psutil.disk_partitions():
            try:
                usage = psutil.disk_usage(partition.mountpoint)
                disks.append({
                    'mountpoint': partition.mountpoint,
                    'total_gb': usage.total / (1024**3),
                    'used_gb': usage.used / (1024**3),
                    'free_gb': usage.free / (1024**3),
                    'percent': usage.percent
                })
            except (psutil.AccessDenied, FileNotFoundError):
                continue
        
        return disks
    
    def _get_network_resources(self) -> Dict[str, Any]:
        """Get network resources."""
        return {
            'bytes_sent': psutil.net_io_counters().bytes_sent,
            'bytes_recv': psutil.net_io_counters().bytes_recv,
            'packets_sent': psutil.net_io_counters().packets_sent,
            'packets_recv': psutil.net_io_counters().packets_recv,
            'errin': psutil.net_io_counters().errin,
            'errout': psutil.net_io_counters().errout,
            'dropin': psutil.net_io_counters().dropin,
            'dropout': psutil.net_io_counters().dropout
        }
    
    def _get_process_resources(self) -> List[Dict[str, Any]]:
        """Get process resources."""
        processes = []
        
        for proc in psutil.process_iter(['pid', 'name', 'cpu_percent', 'memory_percent']):
            try:
                processes.append({
                    'pid': proc.info['pid'],
                    'name': proc.info['name'],
                    'cpu_percent': proc.info['cpu_percent'],
                    'memory_percent': proc.info['memory_percent']
                })
            except (psutil.NoSuchProcess, psutil.AccessDenied):
                continue
        
        # Sort by CPU usage
        processes.sort(key=lambda x: x.get('cpu_percent', 0), reverse=True)
        
        return processes[:10]  # Top 10
    
    def _get_process_details(self, proc, include_children=False) -> Dict[str, Any]:
        """Get detailed information about a process."""
        details = {
            'pid': proc.info['pid'],
            'name': proc.info['name'],
            'status': proc.status(),
            'cpu_percent': proc.cpu_percent(interval=0.1),
            'memory_percent': proc.memory_percent(),
            'memory_info': proc.memory_info()._asdict() if hasattr(proc, 'memory_info') else None,
            'create_time': datetime.fromtimestamp(proc.create_time()).isoformat() if hasattr(proc, 'create_time') else None,
            'num_threads': proc.num_threads() if hasattr(proc, 'num_threads') else None,
            'children': []
        }
        
        if include_children:
            try:
                for child in proc.children(recursive=True):
                    child_details = {
                        'pid': child.pid,
                        'name': child.name() if hasattr(child, 'name') else 'Unknown',
                        'parent_pid': proc.info['pid']
                    }
                    details['children'].append(child_details)
            except (psutil.NoSuchProcess, psutil.AccessDenied):
                pass
        
        return details
    
    def _get_cpu_health(self) -> Dict[str, Any]:
        """Get CPU health score."""
        cpu_percent = psutil.cpu_percent(interval=0.1)
        
        if cpu_percent < 30:
            score = 100
            status = 'healthy'
        elif cpu_percent < 60:
            score = 100 - (cpu_percent - 30) * 0.5
            status = 'warning'
        elif cpu_percent < 80:
            score = 85 - (cpu_percent - 60) * 0.5
            status = 'warning'
        elif cpu_percent < 95:
            score = 70 - (cpu_percent - 80) * 0.5
            status = 'critical'
        else:
            score = max(0, 60 - (cpu_percent - 95) * 0.2)
            status = 'critical'
        
        return {
            'score': score,
            'status': status,
            'cpu_percent': cpu_percent
        }
    
    def _get_memory_health(self) -> Dict[str, Any]:
        """Get memory health score."""
        memory = psutil.virtual_memory()
        
        if memory.percent < 50:
            score = 100
            status = 'healthy'
        elif memory.percent < 70:
            score = 100 - (memory.percent - 50) * 0.5
            status = 'warning'
        elif memory.percent < 85:
            score = 85 - (memory.percent - 70) * 0.5
            status = 'warning'
        elif memory.percent < 95:
            score = 70 - (memory.percent - 85) * 0.5
            status = 'critical'
        else:
            score = max(0, 60 - (memory.percent - 95) * 0.2)
            status = 'critical'
        
        return {
            'score': score,
            'status': status,
            'memory_percent': memory.percent
        }
    
    def _get_disk_health(self) -> Dict[str, Any]:
        """Get disk health score."""
        scores = []
        
        for partition in psutil.disk_partitions():
            try:
                usage = psutil.disk_usage(partition.mountpoint)
                
                if usage.percent < 50:
                    score = 100
                    status = 'healthy'
                elif usage.percent < 70:
                    score = 100 - (usage.percent - 50) * 0.5
                    status = 'warning'
                elif usage.percent < 85:
                    score = 85 - (usage.percent - 70) * 0.5
                    status = 'warning'
                elif usage.percent < 95:
                    score = 70 - (usage.percent - 85) * 0.5
                    status = 'critical'
                else:
                    score = max(0, 60 - (usage.percent - 95) * 0.2)
                    status = 'critical'
                
                scores.append(score)
            except (psutil.AccessDenied, FileNotFoundError):
                continue
        
        if not scores:
            return {
                'score': 0,
                'status': 'unknown',
                'message': 'No accessible disk partitions'
            }
        
        overall_score = sum(scores) / len(scores)
        
        return {
            'score': overall_score,
            'status': 'healthy' if overall_score >= 90 else 'warning' if overall_score >= 70 else 'critical',
            'disk_usage_scores': scores,
            'total_disks_checked': len(scores)
        }
    
    def _get_process_health(self) -> Dict[str, Any]:
        """Get process health score."""
        scores = []
        
        for proc in psutil.process_iter(['pid', 'cpu_percent', 'memory_percent']):
            try:
                cpu_score = 100 if proc.info['cpu_percent'] < 30 else \
                        90 - (proc.info['cpu_percent'] - 30) if proc.info['cpu_percent'] < 60 else \
                        max(0, 70 - (proc.info['cpu_percent'] - 60) * 0.5)
                
                mem_score = 100 if proc.info['memory_percent'] < 30 else \
                        90 - (proc.info['memory_percent'] - 30) if proc.info['memory_percent'] < 60 else \
                        max(0, 70 - (proc.info['memory_percent'] - 60) * 0.5)
                
                process_score = (cpu_score + mem_score) / 2
                scores.append(process_score)
            except (psutil.NoSuchProcess, psutil.AccessDenied):
                continue
        
        if not scores:
            return {
                'score': 0,
                'status': 'unknown',
                'message': 'No accessible processes'
            }
        
        overall_score = sum(scores) / len(scores)
        
        return {
            'score': overall_score,
            'status': 'healthy' if overall_score >= 90 else 'warning' if overall_score >= 70 else 'critical',
            'total_processes': len(scores),
            'average_cpu_usage': sum(p['cpu_percent'] for p in psutil.process_iter(['cpu_percent']) if p['cpu_percent']) / len(list(psutil.process_iter(['cpu_percent']))) if list(psutil.process_iter(['cpu_percent'])) else 0
        }
    
    def _display_system_status(self, status_info: Dict[str, Any]):
        """Display system status information."""
        console.print(f"System Status: {status_info['timestamp']}")
        console.print("=" * 50)
        console.print(f"Overall Status: [{self._get_status_color(status_info['overall_status'])}]{status_info['overall_status']}[/{self._get_status_color(status_info['overall_status'])}]")
        console.print()
        
        for component, status in status_info['components'].items():
            color = self._get_status_color(status['status'])
            console.print(f"{component.capitalize()}: [{color}]{status['status']}[/{color}] - {status['message']}")
    
    def _display_system_resources(self, resources: Dict[str, Any]):
        """Display system resources information."""
        console.print(f"System Resources: {resources['timestamp']}")
        console.print("=" * 50)
        
        # CPU
        cpu = resources['cpu']
        console.print(f"CPU Usage: {cpu['cpu_percent']:.1f}%")
        if cpu['cpu_freq']:
            console.print(f"CPU Frequency: {cpu['cpu_freq']:.0f} MHz")
        
        # Memory
        memory = resources['memory']
        console.print(f"Memory: {memory['used_gb']:.1f} GB used / {memory['total_gb']:.1f} GB total ({memory['percent']:.1f}%)")
        
        # Disk
        console.print("Disk Usage:")
        for disk in resources['disk']:
            console.print(f"  {disk['mountpoint']}: {disk['used_gb']:.1f} GB / {disk['total_gb']:.1f} GB ({disk['percent']:.1f}%)")
        
        # Network
        network = resources['network']
        console.print(f"Network: {network['bytes_sent'] / (1024**2):.1f} MB sent, {network['bytes_recv'] / (1024**2):.1f} MB received")
        
        # Top processes
        console.print("Top Processes by CPU Usage:")
        for proc in resources['processes']:
            console.print(f"  {proc['name']} (PID: {proc['pid']}): {proc['cpu_percent']:.1f}% CPU, {proc['memory_percent']:.1f}% Memory")
    
    def _display_disk_info(self, disks: List[Dict[str, Any]]):
        """Display disk information."""
        console.print("Disk Information:")
        console.print("=" * 50)
        
        table = Table(title="Disks")
        table.add_column("Device", style="cyan")
        table.add_column("Mountpoint", style="green")
        table.add_column("Filesystem", style="yellow")
        table.add_column("Total (GB)", style="magenta")
        table.add_column("Used (GB)", style="red")
        table.add_column("Free (GB)", style="blue")
        table.add_column("Usage %", style="cyan")
        
        for disk in disks:
            table.add_row(
                disk['device'],
                disk['mountpoint'],
                disk['filesystem'],
                f"{disk['total_gb']:.1f}",
                f"{disk['used_gb']:.1f}",
                f"{disk['free_gb']:.1f}",
                f"{disk['used_percent']:.1f}%"
            )
        
        console.print(table)
    
    def _display_network_info(self, network_info: Dict[str, Any]):
        """Display network information."""
        console.print("Network Information:")
        console.print("=" * 50)
        
        io = network_info['io_counters']
        console.print(f"Bytes Sent: {io.bytes_sent / (1024**2):.1f} MB")
        console.print(f"Bytes Received: {io.bytes_recv / (1024**2):.1f} MB")
        console.print(f"Packets Sent: {io.packets_sent}")
        console.print(f"Packets Received: {io.packets_recv}")
        console.print(f"Errors In: {io.errin}")
        console.print(f"Errors Out: {io.errout}")
        
        if network_info['interfaces']:
            console.print("\nInterfaces:")
            for interface in network_info['interfaces']:
                console.print(f"  {interface['name']}:")
                console.print(f"    Address: {interface['address']}")
                console.print(f"    Netmask: {interface['netmask'] or 'N/A'}")
                console.print(f"    Broadcast: {interface['broadcast'] or 'N/A'}")
                console.print(f"    Bytes Sent: {interface['bytes_sent']}")
                console.print(f"    Bytes Received: {interface['bytes_recv']}")
    
    def _display_memory_info(self, memory_info: Dict[str, Any]):
        """Display memory information."""
        console.print("Memory Information:")
        console.print("=" * 50)
        
        system_mem = memory_info['system_memory']
        console.print(f"Total Memory: {system_mem['total_gb']:.1f} GB")
        console.print(f"Used Memory: {system_mem['used_gb']:.1f} GB ({system_mem['used_percent']:.1f}%)")
        console.print(f"Available Memory: {system_mem['available_gb']:.1f} GB")
        console.print(f"Free Memory: {system_mem['free_gb']:.1f} GB")
        
        swap = memory_info['swap_memory']
        console.print(f"\nSwap Memory: {swap['total_gb']:.1f} GB total, {swap['used_gb']:.1f} GB used ({swap['used_percent']:.1f}%)")
        
        if memory_info['top_processes']:
            console.print("\nTop Processes by Memory Usage:")
            for proc in memory_info['top_processes']:
                console.print(f"  {proc['name']} (PID: {proc['pid']}): {proc['rss_mb']:.1f} MB RSS, {proc['vms_mb']:.1f} MB VMS")
    
    def _display_cpu_info(self, cpu_info: Dict[str, Any]):
        """Display CPU information."""
        console.print("CPU Information:")
        console.print("=" * 50)
        
        console.print(f"Physical Cores: {cpu_info['physical_cores']}")
        console.print(f"Logical Cores: {cpu_info['logical_cores']}")
        console.print(f"Current CPU Usage: {cpu_info['cpu_percent']:.1f}%")
        
        if cpu_info['cpu_freq']:
            freq = cpu_info['cpu_freq']
            console.print(f"CPU Frequency: Min: {freq.min:.0f} MHz, Current: {freq.current:.0f} MHz, Max: {freq.max:.0f} MHz")
        
        if cpu_info['load_average']:
            load = cpu_info['load_average']
            console.print(f"Load Average: {load[0]:.2f}, {load[1]:.2f}, {load[2]:.2f}")
        
        console.print("\nPer-Core Usage:")
        for i, usage in enumerate(cpu_info['per_cpu_percent']):
            console.print(f"  Core {i}: {usage:.1f}%")
    
    def _display_alerts(self, alerts: List[Dict[str, Any]]):
        """Display system alerts."""
        if not alerts:
            console.print("No alerts - system is healthy!")
            return
        
        console.print("System Alerts:")
        console.print("=" * 50)
        
        for alert in alerts:
            severity = alert['severity']
            if severity == 'critical':
                color = 'red'
                icon = '✗'
            elif severity == 'warning':
                color = 'yellow'
                icon = '!'
            else:
                color = 'green'
                icon = '✓'
            
            console.print(f"[{color}]{icon} {alert['severity'].upper()}: {alert['message']}[/{color}]")
    
    def _display_health_report(self, health_report: Dict[str, Any]):
        """Display health report."""
        console.print(f"System Health Report: {health_report['timestamp']}")
        console.print("=" * 50)
        console.print(f"Overall Health Score: {health_report['health_score']:.1f}")
        console.print(f"Status: [{self._get_status_color(health_report['status'])}]{health_report['status'][0].upper() + health_report['status'][1:]}[/{self._get_status_color(health_report['status'])}]")
        console.print()
        
        for component, health in health_report['components'].items():
            console.print(f"{component.capitalize()}:")
            console.print(f"  Score: {health['score']:.1f}")
            console.print(f"  Status: [{self._get_status_color(health['status'])}]{health['status'][0].upper() + health['status'][1:]}[/{self._get_status_color(health['status'])}]")
    
    def _display_dependency_status(self, dependency_status: List[Dict[str, Any]]):
        """Display dependency status."""
        console.print("Dependency Status:")
        console.print("=" * 50)
        
        installed = [d for d in dependency_status if d['status'] == 'installed']
        not_installed = [d for d in dependency_status if d['status'] == 'not_installed']
        
        console.print(f"Installed: {len(installed)}")
        for dep in installed:
            console.print(f"  ✓ {dep['name']} (version: {dep['version']})")
        
        console.print(f"\nNot Installed: {len(not_installed)}")
        for dep in not_installed:
            console.print(f"  ✗ {dep['name']}")
    
    def _get_status_color(self, status: str) -> str:
        """Get color for status."""
        if status == 'healthy' or status == 'ok':
            return 'green'
        elif status == 'warning':
            return 'yellow'
        elif status == 'critical' or status == 'error':
            return 'red'
        else:
            return 'white'
    
    def _monitor_system_resources(self, duration: int, interval: int, config: Dict[str, Any]):
        """Monitor system resources in background."""
        end_time = time.time() + duration
        monitoring_data = []
        
        while time.time() < end_time:
            sample = {
                'timestamp': datetime.now().isoformat(),
                'cpu': psutil.cpu_percent(interval=0.1),
                'memory': psutil.virtual_memory().percent,
                'disk': [usage.percent for usage in [psutil.disk_usage(part.mountpoint) for part in psutil.disk_partitions() if part.mountpoint.startswith('/')]][0] if psutil.disk_partitions() else 0,
                'processes': len(psutil.process_iter())
            }
            
            monitoring_data.append(sample)
            
            # Check for alerts
            self._check_monitoring_alerts(sample, config.get('alert_thresholds', {}))
            
            time.sleep(interval)
        
        # Save monitoring data
        if len(monitoring_data) > 0:
            data_file = Path.home() / '.nlm' / 'monitoring_data.json'
            
            # Load existing data
            if data_file.exists():
                with open(data_file, 'r') as f:
                    existing_data = json.load(f)
            else:
                existing_data = []
            
            existing_data.extend(monitoring_data)
            
            # Save back
            with open(data_file, 'w') as f:
                json.dump(existing_data, f, indent=2, default=str)
    
    def _check_monitoring_alerts(self, sample: Dict[str, Any], alert_thresholds: Dict[str, Any]):
        """Check for monitoring alerts."""
        alerts = []
        
        # Check CPU
        if sample['cpu'] > alert_thresholds.get('cpu', 90):
            alerts.append({
                'type': 'cpu',
                'severity': 'warning',
                'value': sample['cpu'],
                'threshold': alert_thresholds.get('cpu', 90),
                'timestamp': sample['timestamp']
            })
        
        # Check memory
        if sample['memory'] > alert_thresholds.get('memory', 90):
            alerts.append({
                'type': 'memory',
                'severity': 'warning',
                'value': sample['memory'],
                'threshold': alert_thresholds.get('memory', 90),
                'timestamp': sample['timestamp']
            })
        
        # Check disk
        if sample['disk'] > alert_thresholds.get('disk', 90):
            alerts.append({
                'type': 'disk',
                'severity': 'warning',
                'value': sample['disk'],
                'threshold': alert_thresholds.get('disk', 90),
                'timestamp': sample['timestamp']
            })
        
        # Save alerts if any
        if alerts:
            alerts_file = Path.home() / '.nlm' / 'monitoring_alerts.json'
            
            # Load existing alerts
            if alerts_file.exists():
                with open(alerts_file, 'r') as f:
                    existing_alerts = json.load(f)
            else:
                existing_alerts = []
            
            existing_alerts.extend(alerts)
            
            # Save back
            with open(alerts_file, 'w') as f:
                json.dump(existing_alerts, f, indent=2, default=str)
    
    def _list_system_plugins(self) -> Dict[str, Any]:
        """List system plugins."""
        plugins = []
        
        # Check for plugins directory
        plugins_dir = Path('/usr/local/lib/python3.8/dist-packages')  # Placeholder for system plugins
        
        if plugins_dir.exists():
            for plugin_dir in plugins_dir.iterdir():
                if plugin_dir.is_dir():
                    plugin_info = {
                        'name': plugin_dir.name,
                        'path': str(plugin_dir),
                        'type': 'system_plugin'
                    }
                    plugins.append(plugin_info)
        
        return {
            'success': True,
            'message': f"Found {len(plugins)} system plugins.",
            'plugins': plugins
        }
    
    def _update_system_plugins(self) -> Dict[str, Any]:
        """Update system plugins."""
        # This would typically update system packages
        # For now, return a placeholder
        updated = []
        
        return {
            'success': True,
            'message': "System plugins updated (placeholder implementation).",
            'updated_plugins': updated
        }
    
    def _install_system_plugins(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Install system plugins."""
        packages = args.get('packages', [])
        
        installed = []
        failed = []
        
        for package in packages:
            try:
                subprocess.run([sys.executable, '-m', 'pip', 'install', '--system', package],
                             check=True, capture_output=True, text=True)
                installed.append({
                    'name': package,
                    'status': 'installed',
                    'action': 'system_install'
                })
            except subprocess.CalledProcessError as e:
                failed.append({
                    'name': package,
                    'status': 'failed',
                    'error': e.stderr
                })
        
        return {
            'success': len(failed) == 0,
            'message': f"Installed {len(installed)} system packages, failed to install {len(failed)}.",
            'installed': installed,
            'failed': failed
        }
    
    def _get_package_version(self, package_name: str) -> Optional[str]:
        """Get package version."""
        try:
            # Try to import the package
            module = __import__(package_name)
            
            # Try to get version attribute
            if hasattr(module, '__version__'):
                return module.__version__
            elif hasattr(module, 'version'):
                return module.version
            elif hasattr(module, '__file__'):
                # Try to extract version from file path
                path = Path(module.__file__)
                if 'dist-packages' in str(path):
                    # Try to find version file
                    version_file = path.parent / 'version.txt'
                    if version_file.exists():
                        with open(version_file, 'r') as f:
                            return f.read().strip()
                
                return "Unknown"
            else:
                return "Unknown"
        except ImportError:
            return None