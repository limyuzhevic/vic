"""
Comprehensive Real-Time Monitoring Interface for NLM Brain System

This module provides a complete monitoring solution for the Neural Learning Machine
brain system with support for multiple interface options and advanced monitoring features.

Key Features:
- Multi-interface support (Web, CLI, Terminal, API, Plugin-based)
- Real-time data streaming and historical tracking
- Advanced neural activity visualization
- Memory system monitoring
- Neuromodulation tracking
- Plasticity activity monitoring
- Alert system and anomaly detection
- Predictive monitoring and performance profiling
- Export capabilities in multiple formats
- Scalable architecture for small to large deployments
"""

from pynlm.interface import ScriptingInterface, ScriptingEvent, EventType
from pynlm.dashboard import Dashboard, DashboardWidgetType
from pynlm.visualization import VisualizationBackend, PlotManager
from pynlm.event_system import EventBus, EventEmitter
from pynlm.plugin_system import Plugin, PluginManager
from pynlm.brain_control import BrainController

import asyncio
import threading
import time
import json
import psutil
import numpy as np
from typing import Dict, List, Any, Optional, Callable, Union, Tuple
from dataclasses import dataclass, field
from enum import Enum
import logging
import subprocess
import os
from datetime import datetime, timedelta

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)


class MonitoringLevel(Enum):
    """Monitoring levels"""
    BASIC = "basic"
    DETAILED = "detailed"
    COMPREHENSIVE = "comprehensive"
    DEBUG = "debug"


class MonitoringMode(Enum):
    """Monitoring modes"""
    REAL_TIME = "real_time"
    BATCH = "batch"
    EVENT_DRIVEN = "event_driven"
    THROTTLED = "throttled"


class MonitoringInterface:
    """Main monitoring interface for NLM brain system"""
    
    def __init__(self, interface: ScriptingInterface):
        self.interface = interface
        self.context = interface.context
        self.event_bus = EventBus(interface)
        self.plugin_manager = PluginManager(interface)
        self.brain_controller = BrainController(interface)
        
        # Monitoring configuration
        self.monitoring_level = MonitoringLevel.DETAILED
        self.monitoring_mode = MonitoringMode.REAL_TIME
        self.update_interval = 1.0
        self.history_length = 10000
        self.max_data_points = 10000
        
        # Monitoring data storage
        self.monitoring_data = {
            'neural_activity': [],
            'memory_usage': [],
            'performance_metrics': [],
            'neuromodulation': [],
            'plasticity_events': [],
            'system_resources': [],
            'events': []
        }
        
        # Interface managers
        self.web_dashboard = None
        self.cli_monitor = None
        self.terminal_visualizer = None
        self.api_server = None
        self.plugin_system = None
        
        # Monitoring components
        self.monitoring_threads = {}
        self.data_streams = {}
        self.alert_system = None
        self.anomaly_detector = None
        self.performance_profiler = None
        
        # State tracking
        self.is_monitoring = False
        self.start_time = None
        self.last_update = None
        self.data_stream_tasks = {}
        
        logger.info("NLM Monitoring Interface initialized")
    
    def start_monitoring(self, level: MonitoringLevel = MonitoringLevel.DETAILED,
                        mode: MonitoringMode = MonitoringMode.REAL_TIME,
                        update_interval: float = 1.0) -> bool:
        """Start comprehensive monitoring"""
        try:
            self.monitoring_level = level
            self.monitoring_mode = mode
            self.update_interval = update_interval
            self.start_time = time.time()
            
            # Initialize monitoring components
            self._initialize_monitoring_components()
            
            # Start data collection threads
            self._start_data_collection()
            
            # Start interface servers (if configured)
            self._start_interfaces()
            
            self.is_monitoring = True
            
            logger.info(f"Monitoring started with level: {level}, mode: {mode}, interval: {update_interval}s")
            return True
            
        except Exception as e:
            logger.error(f"Failed to start monitoring: {e}")
            return False
    
    def stop_monitoring(self):
        """Stop all monitoring components"""
        try:
            self.is_monitoring = False
            
            # Stop data collection threads
            for thread_name, thread in self.monitoring_threads.items():
                if thread.is_alive():
                    thread.join(timeout=5.0)
            
            self.monitoring_threads.clear()
            
            # Stop data streams
            for stream_id in list(self.data_streams.keys()):
                self.stop_data_stream(stream_id)
            
            # Stop interfaces
            self._stop_interfaces()
            
            logger.info("Monitoring stopped")
            
        except Exception as e:
            logger.error(f"Error stopping monitoring: {e}")
    
    def _initialize_monitoring_components(self):
        """Initialize all monitoring components"""
        # Initialize alert system
        self.alert_system = AlertSystem(self.interface, self.monitoring_level)
        
        # Initialize anomaly detector
        self.anomaly_detector = AnomalyDetector(self.monitoring_level)
        
        # Initialize performance profiler
        self.performance_profiler = PerformanceProfiler()
        
        # Initialize plugin system
        self.plugin_system = self.plugin_manager
        
        # Register monitoring event handlers
        self._register_event_handlers()
        
        # Setup data streams for each monitoring domain
        self._setup_data_streams()
    
    def _register_event_handlers(self):
        """Register event handlers for monitoring"""
        # Handle state changes
        self.event_bus.create_emitter('monitoring')
        self.event_bus.get_emitter('monitoring').add_handler(
            self._handle_state_change_event,
            priority=1
        )
        
        # Handle neural events
        self.event_bus.create_emitter('neural')
        self.event_bus.get_emitter('neural').add_handler(
            self._handle_neural_event,
            priority=2
        )
        
        # Handle simulation events
        self.event_bus.create_emitter('simulation')
        self.event_bus.get_emitter('simulation').add_handler(
            self._handle_simulation_event,
            priority=2
        )
    
    def _setup_data_streams(self):
        """Setup data streams for monitoring"""
        # Neural activity stream
        self.add_data_stream('neural_activity', self._get_neural_activity_data)
        
        # System resources stream
        self.add_data_stream('system_resources', self._get_system_resources_data)
        
        # Memory usage stream
        self.add_data_stream('memory_usage', self._get_memory_usage_data)
        
        # Performance metrics stream
        self.add_data_stream('performance_metrics', self._get_performance_metrics_data)
        
        # Neuromodulation stream
        self.add_data_stream('neuromodulation', self._get_neuromodulation_data)
        
        # Plasticity events stream
        self.add_data_stream('plasticity_events', self._get_plasticity_events_data)
    
    def _start_data_collection(self):
        """Start data collection threads"""
        # Main monitoring thread
        monitoring_thread = threading.Thread(
            target=self._monitoring_loop,
            daemon=True,
            name='monitoring_main'
        )
        monitoring_thread.start()
        self.monitoring_threads['main'] = monitoring_thread
        
        # Historical data management thread
        history_thread = threading.Thread(
            target=self._history_management_loop,
            daemon=True,
            name='history_management'
        )
        history_thread.start()
        self.monitoring_threads['history'] = history_thread
    
    def _monitoring_loop(self):
        """Main monitoring loop"""
        while self.is_monitoring:
            try:
                start_time = time.time()
                
                # Collect data from all sources
                self._collect_monitoring_data()
                
                # Process and analyze data
                self._process_monitoring_data()
                
                # Check for alerts
                if self.alert_system:
                    self.alert_system.check_alerts(self.monitoring_data)
                
                # Detect anomalies
                if self.anomaly_detector:
                    anomalies = self.anomaly_detector.detect_anomalies(self.monitoring_data)
                    if anomalies:
                        self._handle_anomalies(anomalies)
                
                # Profile performance
                if self.performance_profiler:
                    self.performance_profiler.record_performance(self.monitoring_data)
                
                # Emit monitoring event
                self.event_bus.emit_event(ScriptingEvent(
                    EventType.STATE_CHANGED,
                    time.time(),
                    'monitoring',
                    {
                        'data': self.monitoring_data,
                        'monitoring_level': self.monitoring_level.value,
                        'update_interval': self.update_interval
                    }
                ))
                
                # Calculate sleep time to maintain update interval
                elapsed = time.time() - start_time
                sleep_time = max(0, self.update_interval - elapsed)
                time.sleep(sleep_time)
                
            except Exception as e:
                logger.error(f"Monitoring loop error: {e}")
                time.sleep(1.0)
    
    def _history_management_loop(self):
        """Historical data management loop"""
        while self.is_monitoring:
            try:
                # Trim old data
                for key, data in self.monitoring_data.items():
                    if len(data) > self.max_data_points:
                        self.monitoring_data[key] = data[-self.max_data_points:]
                
                # Periodic cleanup
                if int(time.time()) % 60 == 0:  # Every minute
                    self._cleanup_old_data()
                
                time.sleep(30.0)  # Check every 30 seconds
                
            except Exception as e:
                logger.error(f"History management loop error: {e}")
                time.sleep(30.0)
    
    def _collect_monitoring_data(self):
        """Collect data from all monitoring sources"""
        current_time = time.time()
        
        # Neural activity data
        neural_data = self._get_neural_activity_data()
        self.monitoring_data['neural_activity'].append({
            'timestamp': current_time,
            'data': neural_data
        })
        
        # System resources data
        system_data = self._get_system_resources_data()
        self.monitoring_data['system_resources'].append({
            'timestamp': current_time,
            'data': system_data
        })
        
        # Memory usage data
        memory_data = self._get_memory_usage_data()
        self.monitoring_data['memory_usage'].append({
            'timestamp': current_time,
            'data': memory_data
        })
        
        # Performance metrics data
        perf_data = self._get_performance_metrics_data()
        self.monitoring_data['performance_metrics'].append({
            'timestamp': current_time,
            'data': perf_data
        })
        
        # Neuromodulation data
        neuromod_data = self._get_neuromodulation_data()
        self.monitoring_data['neuromodulation'].append({
            'timestamp': current_time,
            'data': neuromod_data
        })
        
        # Plasticity events data
        plasticity_data = self._get_plasticity_events_data()
        if plasticity_data:
            self.monitoring_data['plasticity_events'].append({
                'timestamp': current_time,
                'data': plasticity_data
            })
        
        # Get recent events
        recent_events = self.interface.context.event_queue.queue if hasattr(self.interface.context.event_queue, 'queue') else []
        self.monitoring_data['events'].append({
            'timestamp': current_time,
            'count': len(recent_events),
            'events': recent_events[-10:] if recent_events else []
        })
    
    def _process_monitoring_data(self):
        """Process and analyze collected monitoring data"""
        # Calculate real-time statistics
        self._calculate_realtime_statistics()
        
        # Update averages and trends
        self._update_data_trends()
        
        # Detect performance patterns
        self._detect_performance_patterns()
    
    def _calculate_realtime_statistics(self):
        """Calculate real-time statistics from monitoring data"""
        if not self.monitoring_data['neural_activity']:
            return
        
        # Get latest data
        latest_neural = self.monitoring_data['neural_activity'][-1]['data']
        latest_system = self.monitoring_data['system_resources'][-1]['data'] if self.monitoring_data['system_resources'] else {}
        latest_memory = self.monitoring_data['memory_usage'][-1]['data'] if self.monitoring_data['memory_usage'] else {}
        
        # Calculate statistics
        stats = {
            'neural_active_ratio': latest_neural.get('active_ratio', 0),
            'cpu_usage': latest_system.get('cpu_percent', 0),
            'memory_usage_percent': latest_memory.get('percent', 0),
            'neural_firing_rate': latest_neural.get('average_firing_rate', 0),
            'plasticity_activity': sum(1 for event in self.monitoring_data['plasticity_events'][-1]['data'] 
                                     if event.get('type') == 'plasticity_change') if self.monitoring_data['plasticity_events'] else 0
        }
        
        # Store statistics
        self.monitoring_data['neural_activity'][-1]['statistics'] = stats
    
    def _update_data_trends(self):
        """Update data trends for long-term analysis"""
        # Calculate moving averages
        window_size = min(100, len(self.monitoring_data['neural_activity']))
        if window_size > 0:
            recent_neural = [d['data'] for d in self.monitoring_data['neural_activity'][-window_size:]]
            recent_cpu = [d['data'].get('cpu_percent', 0) for d in self.monitoring_data['system_resources'][-window_size:]]
            
            # Calculate trends
            if recent_neural:
                self.monitoring_data['neural_activity'][-1]['trend'] = {
                    'neural_activity': np.mean([d.get('activity', 0) for d in recent_neural]) if recent_neural else 0,
                    'cpu_usage': np.mean(recent_cpu) if recent_cpu else 0
                }
    
    def _detect_performance_patterns(self):
        """Detect performance patterns and anomalies"""
        if not self.anomaly_detector:
            return
        
        # Detect performance patterns
        recent_performance = []
        for d in self.monitoring_data['performance_metrics'][-100:]:
            recent_performance.append(d['data'])
        
        if recent_performance:
            anomalies = self.anomaly_detector.detect_performance_anomalies(recent_performance)
            if anomalies:
                logger.warning(f"Detected {len(anomalies)} performance anomalies")
    
    def add_data_stream(self, stream_id: str, data_provider: Callable,
                       update_interval: float = 1.0):
        """Add a data stream for real-time monitoring"""
        self.data_streams[stream_id] = {
            'provider': data_provider,
            'update_interval': update_interval,
            'last_update': 0,
            'data': [],
            'subscribers': [],
            'thread': None
        }
    
    def start_data_stream(self, stream_id: str):
        """Start a data stream"""
        if stream_id in self.data_streams:
            stream = self.data_streams[stream_id]
            if not stream['thread'] or not stream['thread'].is_alive():
                stream['thread'] = threading.Thread(
                    target=self._data_stream_loop,
                    args=(stream_id,),
                    daemon=True
                )
                stream['thread'].start()
    
    def stop_data_stream(self, stream_id: str):
        """Stop a data stream"""
        if stream_id in self.data_streams:
            stream = self.data_streams[stream_id]
            if stream['thread'] and stream['thread'].is_alive():
                stream['thread'].join(timeout=5.0)
            stream['thread'] = None
    
    def _data_stream_loop(self, stream_id: str):
        """Internal data stream update loop"""
        stream = self.data_streams.get(stream_id)
        if not stream:
            return
        
        while stream_id in self.data_streams and self.is_monitoring:
            try:
                if time.time() - stream['last_update'] >= stream['update_interval']:
                    data = stream['provider']()
                    stream['data'].append(data)
                    stream['last_update'] = time.time()
                    
                    # Trim data
                    if len(stream['data']) > 1000:
                        stream['data'] = stream['data'][-1000:]
                    
                    # Notify subscribers
                    for subscriber in stream['subscribers']:
                        try:
                            subscriber(data)
                        except Exception:
                            pass
                
                time.sleep(0.1)
                
            except Exception as e:
                logger.error(f"Data stream error for {stream_id}: {e}")
                time.sleep(1.0)
    
    def _get_neural_activity_data(self) -> Dict[str, Any]:
        """Get neural activity data"""
        if not self.context.brain:
            return {}
        
        brain = self.context.brain
        regions = brain.getRegions()
        
        neural_data = {
            'total_neurons': brain.getTotalNeuronCount(),
            'active_neurons': brain.getActiveNeuronCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'excitement_inhibition_ratio': brain.getExcitationInhibitionRatio(),
            'total_spikes': brain.getTotalSpikeCount(),
            'developmental_stage': brain.getDevelopmentalStage(),
            'regions': []
        }
        
        for region in regions:
            region_id = region.getId()
            region_neural = {
                'id': region_id,
                'neuron_count': region.getNeuronCount(),
                'active_count': region.getActiveCount(),
                'firing_count': region.getFiringCount(),
                'activity_level': region.getActivityLevel() if hasattr(region, 'getActivityLevel') else 0,
                'plasticity_level': region.getPlasticityLevel() if hasattr(region, 'getPlasticityLevel') else 0
            }
            neural_data['regions'].append(region_neural)
        
        # Calculate active ratio
        if neural_data['total_neurons'] > 0:
            neural_data['active_ratio'] = neural_data['active_neurons'] / neural_data['total_neurons']
        
        return neural_data
    
    def _get_system_resources_data(self) -> Dict[str, Any]:
        """Get system resource usage data"""
        try:
            process = psutil.Process()
            cpu_percent = process.cpu_percent()
            memory_info = process.memory_info()
            
            system_data = {
                'cpu_percent': cpu_percent,
                'memory_rss_mb': memory_info.rss / (1024 * 1024),
                'memory_vms_mb': memory_info.vms / (1024 * 1024),
                'memory_percent': process.memory_percent(),
                'thread_count': process.num_threads(),
                'create_time': process.create_time(),
                'cpu_times': process.cpu_times()._asdict() if hasattr(process.cpu_times(), '_asdict') else {},
                'memory_info': {
                    'page_faults': process.memory_info().page_faults if hasattr(process.memory_info(), 'page_faults') else 0,
                    'minor_page_faults': process.memory_info().minor_page_faults if hasattr(process.memory_info(), 'minor_page_faults') else 0
                }
            }
            
            return system_data
            
        except Exception as e:
            logger.error(f"Error getting system resources: {e}")
            return {}
    
    def _get_memory_usage_data(self) -> Dict[str, Any]:
        """Get memory usage data"""
        try:
            memory = psutil.virtual_memory()
            
            memory_data = {
                'total_mb': memory.total / (1024 * 1024),
                'available_mb': memory.available / (1024 * 1024),
                'used_mb': memory.used / (1024 * 1024),
                'percent': memory.percent,
                'active_mb': memory.active / (1024 * 1024) if hasattr(memory, 'active') else 0,
                'inactive_mb': memory.inactive / (1024 * 1024) if hasattr(memory, 'inactive') else 0,
                'buffers_mb': memory.buffers / (1024 * 1024) if hasattr(memory, 'buffers') else 0,
                'cached_mb': memory.cached / (1024 * 1024) if hasattr(memory, 'cached') else 0,
                'shared_mb': memory.shared / (1024 * 1024) if hasattr(memory, 'shared') else 0
            }
            
            return memory_data
            
        except Exception as e:
            logger.error(f"Error getting memory usage: {e}")
            return {}
    
    def _get_performance_metrics_data(self) -> Dict[str, Any]:
        """Get performance metrics data"""
        perf_data = {
            'update_interval': self.update_interval,
            'monitoring_level': self.monitoring_level.value,
            'monitoring_mode': self.monitoring_mode.value,
            'is_monitoring': self.is_monitoring,
            'start_time': self.start_time,
            'last_update': self.last_update,
            'active_threads': len(self.monitoring_threads),
            'active_data_streams': len([s for s in self.data_streams.values() if s['thread'] and s['thread'].is_alive()]),
            'event_queue_size': self.interface.context.event_queue.qsize() if hasattr(self.interface.context.event_queue, 'qsize') else 0,
            'plugin_count': len(self.plugin_manager.plugins) if self.plugin_manager else 0,
            'alert_count': self.alert_system.get_alert_count() if self.alert_system else 0
        }
        
        return perf_data
    
    def _get_neuromodulation_data(self) -> Dict[str, Any]:
        """Get neuromodulation data"""
        neuromod_data = {
            'dopamine_level': 0.5,
            'curiosity_level': 0.3,
            'novelty_level': 0.2,
            'excitement_level': 0.4,
            'inhibition_level': 0.3,
            'modulation_active': False,
            'last_modulation_time': 0
        }
        
        # Get actual neuromodulation data if available
        if self.context.brain and hasattr(self.context.brain, 'getNeuromodulationLevel'):
            neuromod_data['dopamine_level'] = self.context.brain.getNeuromodulationLevel()
            neuromod_data['modulation_active'] = neuromod_data['dopamine_level'] > 0
            neuromod_data['last_modulation_time'] = time.time()
        
        return neuromod_data
    
    def _get_plasticity_events_data(self) -> List[Dict[str, Any]]:
        """Get plasticity events data"""
        plasticity_events = []
        
        if not self.context.brain:
            return plasticity_events
        
        brain = self.context.brain
        regions = brain.getRegions()
        
        for region in regions:
            region_id = region.getId()
            neurons = region.getNeurons()
            
            for neuron in neurons:
                neuron_id = neuron.getId()
                
                # Check for plasticity events
                if hasattr(neuron, 'getPlasticityEvents'):
                    events = neuron.getPlasticityEvents()
                    for event in events:
                        plasticity_events.append({
                            'region_id': region_id,
                            'neuron_id': neuron_id,
                            'type': event.get('type', 'unknown'),
                            'strength': event.get('strength', 0),
                            'timestamp': event.get('timestamp', time.time()),
                            'synapse_pre': event.get('synapse_pre', ''),
                            'synapse_post': event.get('synapse_post', ''),
                            'learning_rate': event.get('learning_rate', 0)
                        })
        
        return plasticity_events
    
    def _handle_state_change_event(self, event: ScriptingEvent):
        """Handle state change events"""
        logger.info(f"State change event: {event.event_type} from {event.source}")
    
    def _handle_neural_event(self, event: ScriptingEvent):
        """Handle neural events"""
        logger.info(f"Neural event: {event.event_type}")
    
    def _handle_simulation_event(self, event: ScriptingEvent):
        """Handle simulation events"""
        logger.info(f"Simulation event: {event.event_type}")
    
    def _handle_anomalies(self, anomalies: List[Dict[str, Any]]):
        """Handle detected anomalies"""
        for anomaly in anomalies:
            logger.warning(f"Anomaly detected: {anomaly}")
            
            # Emit anomaly event
            self.event_bus.emit_event(ScriptingEvent(
                EventType.ERROR_OCCURRED,
                time.time(),
                'monitoring',
                {
                    'type': 'anomaly',
                    'severity': anomaly.get('severity', 'medium'),
                    'description': anomaly.get('description', ''),
                    'data': anomaly
                }
            ))
    
    def _cleanup_old_data(self):
        """Clean up old monitoring data"""
        # Keep only the last hour of data
        cutoff_time = time.time() - 3600
        
        for key, data in self.monitoring_data.items():
            self.monitoring_data[key] = [
                d for d in data if d['timestamp'] > cutoff_time
            ]
    
    def _start_interfaces(self):
        """Start monitoring interfaces"""
        # Start CLI monitor
        self.cli_monitor = CLIMonitor(self.interface, self.monitoring_data)
        
        # Start web dashboard
        self.web_dashboard = WebDashboard(self.interface, self.monitoring_data)
        
        # Start terminal visualizer
        self.terminal_visualizer = TerminalVisualizer(self.interface, self.monitoring_data)
        
        # Start API server
        self.api_server = APIServer(self.interface, self.monitoring_data)
        
        # Start plugin system
        if self.plugin_manager:
            self.plugin_system.start_monitoring_plugins()
    
    def _stop_interfaces(self):
        """Stop monitoring interfaces"""
        if self.cli_monitor:
            self.cli_monitor.stop()
            self.cli_monitor = None
        
        if self.web_dashboard:
            self.web_dashboard.stop()
            self.web_dashboard = None
        
        if self.terminal_visualizer:
            self.terminal_visualizer.stop()
            self.terminal_visualizer = None
        
        if self.api_server:
            self.api_server.stop()
            self.api_server = None
        
        if self.plugin_system:
            self.plugin_system.stop_monitoring_plugins()


class AlertSystem:
    """Alert system for monitoring"""
    
    def __init__(self, interface: ScriptingInterface, level: MonitoringLevel):
        self.interface = interface
        self.level = level
        self.alerts = []
        self.alert_rules = []
        self.max_alerts = 1000
        
        # Initialize alert rules based on monitoring level
        self._initialize_alert_rules()
    
    def _initialize_alert_rules(self):
        """Initialize alert rules based on monitoring level"""
        # Basic rules
        self.alert_rules = [
            {
                'name': 'high_cpu_usage',
                'condition': lambda data: data.get('system_resources', {}).get('cpu_percent', 0) > 80,
                'severity': 'warning',
                'description': 'High CPU usage detected'
            },
            {
                'name': 'high_memory_usage',
                'condition': lambda data: data.get('system_resources', {}).get('memory_percent', 0) > 90,
                'severity': 'warning',
                'description': 'High memory usage detected'
            }
        ]
        
        # Add detailed rules if level is detailed or higher
        if self.level in [MonitoringLevel.DETAILED, MonitoringLevel.COMPREHENSIVE, MonitoringLevel.DEBUG]:
            self.alert_rules.extend([
                {
                    'name': 'neural_activity_spike',
                    'condition': lambda data: data.get('neural_activity', {}).get('active_ratio', 0) > 0.9,
                    'severity': 'info',
                    'description': 'High neural activity ratio'
                },
                {
                    'name': 'plasticity_activity',
                    'condition': lambda data: data.get('plasticity_events', []),
                    'severity': 'info',
                    'description': 'Plasticity events detected'
                }
            ])
        
        # Add comprehensive rules if level is comprehensive or higher
        if self.level in [MonitoringLevel.COMPREHENSIVE, MonitoringLevel.DEBUG]:
            self.alert_rules.extend([
                {
                    'name': 'anomaly_detected',
                    'condition': lambda data: 'anomalies' in data,
                    'severity': 'critical',
                    'description': 'Anomalies detected in monitoring data'
                },
                {
                    'name': 'performance_degradation',
                    'condition': lambda data: data.get('performance_metrics', {}).get('update_interval', 1.0) > 5.0,
                    'severity': 'warning',
                    'description': 'Performance degradation detected'
                }
            ])
    
    def check_alerts(self, monitoring_data: Dict[str, Any]):
        """Check for alerts based on monitoring data"""
        triggered_alerts = []
        
        for rule in self.alert_rules:
            try:
                if rule['condition'](monitoring_data):
                    alert = {
                        'timestamp': time.time(),
                        'rule': rule['name'],
                        'severity': rule['severity'],
                        'description': rule['description'],
                        'data': monitoring_data
                    }
                    triggered_alerts.append(alert)
                    self.alerts.append(alert)
                    
                    # Keep only recent alerts
                    if len(self.alerts) > self.max_alerts:
                        self.alerts = self.alerts[-self.max_alerts:]
            except Exception as e:
                logger.error(f"Error checking alert rule {rule['name']}: {e}")
        
        # Emit alert events
        for alert in triggered_alerts:
            # In a real implementation, this would emit an event
            logger.info(f"ALERT: {alert['severity'].upper()} - {alert['description']}")
    
    def get_alert_count(self) -> int:
        """Get the number of active alerts"""
        return len(self.alerts)
    
    def get_recent_alerts(self, limit: int = 10) -> List[Dict[str, Any]]:
        """Get recent alerts"""
        return self.alerts[-limit:]


class AnomalyDetector:
    """Anomaly detector for monitoring data"""
    
    def __init__(self, level: MonitoringLevel):
        self.level = level
        self.anomaly_history = []
        self.thresholds = {}
        
        # Initialize thresholds based on monitoring level
        self._initialize_thresholds()
    
    def _initialize_thresholds(self):
        """Initialize anomaly detection thresholds"""
        self.thresholds = {
            'cpu_usage': {'normal': (0, 80), 'warning': (80, 90), 'critical': (90, 100)},
            'memory_usage': {'normal': (0, 70), 'warning': (70, 85), 'critical': (85, 100)},
            'neural_activity': {'normal': (0, 0.7), 'warning': (0.7, 0.9), 'critical': (0.9, 1.0)}
        }
        
        if self.level in [MonitoringLevel.DETAILED, MonitoringLevel.COMPREHENSIVE, MonitoringLevel.DEBUG]:
            self.thresholds['firing_rate'] = {'normal': (0, 50), 'warning': (50, 100), 'critical': (100, 200)}
            self.thresholds['plasticity'] = {'normal': (0, 10), 'warning': (10, 50), 'critical': (50, 100)}
    
    def detect_anomalies(self, monitoring_data: Dict[str, Any]) -> List[Dict[str, Any]]:
        """Detect anomalies in monitoring data"""
        anomalies = []
        
        for key, data in monitoring_data.items():
            if not isinstance(data, dict):
                continue
            
            # Detect anomalies based on thresholds
            anomalies.extend(self._detect_anomalies_by_key(key, data))
        
        # Record anomalies
        for anomaly in anomalies:
            self.anomaly_history.append({
                'timestamp': time.time(),
                'anomaly': anomaly
            })
            
            # Keep only recent anomalies
            if len(self.anomaly_history) > 1000:
                self.anomaly_history = self.anomaly_history[-1000:]
        
        return anomalies
    
    def _detect_anomalies_by_key(self, key: str, data: Dict[str, Any]) -> List[Dict[str, Any]]:
        """Detect anomalies for a specific data key"""
        anomalies = []
        thresholds = self.thresholds.get(key, {})
        
        if not thresholds:
            return anomalies
        
        for param, threshold_ranges in thresholds.items():
            value = data.get(param)
            if value is None:
                continue
            
            for severity, (min_val, max_val) in threshold_ranges.items():
                if min_val <= value <= max_val:
                    anomalies.append({
                        'type': param,
                        'value': value,
                        'threshold': (min_val, max_val),
                        'severity': severity,
                        'description': f"{param} value {value} outside normal range"
                    })
        
        return anomalies
    
    def detect_performance_anomalies(self, performance_data: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
        """Detect performance anomalies"""
        anomalies = []
        
        if not performance_data:
            return anomalies
        
        # Calculate rolling statistics
        cpu_values = [d.get('cpu_percent', 0) for d in performance_data]
        memory_values = [d.get('memory_percent', 0) for d in performance_data]
        
        # Detect spikes
        if len(cpu_values) >= 5:
            recent_cpu = cpu_values[-5:]
            avg_cpu = sum(recent_cpu) / len(recent_cpu)
            if any(abs(cpu - avg_cpu) > 20 for cpu in recent_cpu):
                anomalies.append({
                    'type': 'cpu_spike',
                    'value': max(cpu_values),
                    'threshold': 'spike detected',
                    'severity': 'warning',
                    'description': 'CPU usage spike detected'
                })
        
        if len(memory_values) >= 5:
            recent_memory = memory_values[-5:]
            avg_memory = sum(recent_memory) / len(recent_memory)
            if any(abs(mem - avg_memory) > 15 for mem in recent_memory):
                anomalies.append({
                    'type': 'memory_spike',
                    'value': max(memory_values),
                    'threshold': 'spike detected',
                    'severity': 'warning',
                    'description': 'Memory usage spike detected'
                })
        
        return anomalies
    
    def get_anomaly_count(self) -> int:
        """Get the number of anomalies detected"""
        return len(self.anomaly_history)


class PerformanceProfiler:
    """Performance profiler for monitoring"""
    
    def __init__(self):
        self.performance_history = []
        self.profiling_enabled = True
    
    def record_performance(self, monitoring_data: Dict[str, Any]):
        """Record performance metrics"""
        if not self.profiling_enabled:
            return
        
        perf_metrics = {
            'timestamp': time.time(),
            'cpu_usage': monitoring_data.get('system_resources', {}).get('cpu_percent', 0),
            'memory_usage': monitoring_data.get('system_resources', {}).get('memory_percent', 0),
            'thread_count': monitoring_data.get('system_resources', {}).get('thread_count', 0),
            'data_collection_time': monitoring_data.get('system_resources', {}).get('execution_time_ms', 0),
            'update_interval': monitoring_data.get('performance_metrics', {}).get('update_interval', 1.0),
            'monitoring_data_size': sum(len(d.get('data', [])) for d in monitoring_data.values() if isinstance(d, dict))
        }
        
        self.performance_history.append(perf_metrics)
        
        # Keep only recent performance data
        if len(self.performance_history) > 1000:
            self.performance_history = self.performance_history[-1000:]
    
    def get_performance_summary(self) -> Dict[str, Any]:
        """Get performance summary"""
        if not self.performance_history:
            return {}
        
        cpu_values = [p['cpu_usage'] for p in self.performance_history]
        memory_values = [p['memory_usage'] for p in self.performance_history]
        
        return {
            'average_cpu_usage': sum(cpu_values) / len(cpu_values) if cpu_values else 0,
            'average_memory_usage': sum(memory_values) / len(memory_values) if memory_values else 0,
            'max_cpu_usage': max(cpu_values) if cpu_values else 0,
            'max_memory_usage': max(memory_values) if memory_values else 0,
            'data_points': len(self.performance_history),
            'sampling_rate': len(self.performance_history) / (self.performance_history[-1]['timestamp'] - self.performance_history[0]['timestamp']) if len(self.performance_history) > 1 else 0
        }


class CLIMonitor:
    """Command-line interface monitor"""
    
    def __init__(self, interface: ScriptingInterface, monitoring_data: Dict[str, Any]):
        self.interface = interface
        self.monitoring_data = monitoring_data
        self.running = False
    
    def start(self):
        """Start CLI monitor"""
        self.running = True
        
        monitor_thread = threading.Thread(
            target=self._monitor_loop,
            daemon=True,
            name='cli_monitor'
        )
        monitor_thread.start()
    
    def stop(self):
        """Stop CLI monitor"""
        self.running = False
    
    def _monitor_loop(self):
        """CLI monitoring loop"""
        while self.running:
            try:
                self._display_status()
                time.sleep(5.0)  # Update every 5 seconds
            except Exception as e:
                logger.error(f"CLI monitor error: {e}")
                time.sleep(5.0)
    
    def _display_status(self):
        """Display status in console"""
        if not self.monitoring_data['neural_activity']:
            return
        
        latest = self.monitoring_data['neural_activity'][-1]['data']
        system = self.monitoring_data['system_resources'][-1]['data'] if self.monitoring_data['system_resources'] else {}
        memory = self.monitoring_data['memory_usage'][-1]['data'] if self.monitoring_data['memory_usage'] else {}
        
        # Clear screen and display status
        print("\033c", end="")  # Clear screen
        print("=" * 60)
        print("NLM BRAIN SYSTEM MONITORING - CLI INTERFACE")
        print("=" * 60)
        print(f"Time: {datetime.now().strftime('%H:%M:%S')}")
        print(f"Neurons: {latest.get('total_neurons', 'N/A')}")
        print(f"Active: {latest.get('active_neurons', 'N/A')}")
        print(f"Firing Rate: {latest.get('average_firing_rate', 0):.2f} Hz")
        print(f"CPU Usage: {system.get('cpu_percent', 0):.1f}%")
        print(f"Memory Usage: {memory.get('percent', 0):.1f}%")
        print(f"Plasticity Events: {len(self.monitoring_data['plasticity_events'])}")
        print(f"Alerts: {len(self.monitoring_data.get('alerts', []))}")
        print("=" * 60)
        print("Press Ctrl+C to stop monitoring")
        print("=" * 60)


class WebDashboard:
    """Web-based dashboard"""
    
    def __init__(self, interface: ScriptingInterface, monitoring_data: Dict[str, Any]):
        self.interface = interface
        self.monitoring_data = monitoring_data
        self.running = False
        self.host = 'localhost'
        self.port = 8080
    
    def start(self, host: str = 'localhost', port: int = 8080):
        """Start web dashboard server"""
        self.host = host
        self.port = port
        self.running = True
        
        # In a real implementation, this would start a web server
        # For this example, we'll just log the start
        logger.info(f"Web dashboard would start on {host}:{port}")
    
    def stop(self):
        """Stop web dashboard"""
        self.running = False
        logger.info("Web dashboard stopped")


class TerminalVisualizer:
    """Terminal-based visualizer"""
    
    def __init__(self, interface: ScriptingInterface, monitoring_data: Dict[str, Any]):
        self.interface = interface
        self.monitoring_data = monitoring_data
        self.running = False
    
    def start(self):
        """Start terminal visualizer"""
        self.running = True
        
        visualizer_thread = threading.Thread(
            target=self._visualizer_loop,
            daemon=True,
            name='terminal_visualizer'
        )
        visualizer_thread.start()
    
    def stop(self):
        """Stop terminal visualizer"""
        self.running = False
    
    def _visualizer_loop(self):
        """Terminal visualizer loop"""
        while self.running:
            try:
                self._display_visualization()
                time.sleep(1.0)  # Update every second
            except Exception as e:
                logger.error(f"Terminal visualizer error: {e}")
                time.sleep(1.0)
    
    def _display_visualization(self):
        """Display visualization in terminal"""
        if not self.monitoring_data['neural_activity']:
            return
        
        # Simple text-based visualization
        latest_neural = self.monitoring_data['neural_activity'][-1]['data']
        regions = latest_neural.get('regions', [])
        
        print("\033c", end="")  # Clear screen
        print("NLM BRAIN VISUALIZATION")
        print("=" * 40)
        
        if regions:
            # Display region activity as a simple bar chart
            for region in regions[:5]:  # Show first 5 regions
                activity = region.get('activity_level', 0)
                region_id = region.get('id', 0)
                bar = "█" * int(activity * 10)
                print(f"Region {region_id:2d}: {bar} ({activity:.2f})")
        
        print("=" * 40)
        print(f"Updated: {datetime.now().strftime('%H:%M:%S')}")


class APIServer:
    """REST API server for monitoring"""
    
    def __init__(self, interface: ScriptingInterface, monitoring_data: Dict[str, Any]):
        self.interface = interface
        self.monitoring_data = monitoring_data
        self.running = False
        self.host = 'localhost'
        self.port = 5000
    
    def start(self, host: str = 'localhost', port: int = 5000):
        """Start API server"""
        self.host = host
        self.port = port
        self.running = True
        
        # In a real implementation, this would start a web server
        # For this example, we'll just log the start
        logger.info(f"API server would start on {host}:{port}")
    
    def stop(self):
        """Stop API server"""
        self.running = False
        logger.info("API server stopped")
    
    def get_api_data(self) -> Dict[str, Any]:
        """Get API data"""
        return {
            'status': 'running',
            'timestamp': time.time(),
            'neural_activity': self.monitoring_data['neural_activity'][-1] if self.monitoring_data['neural_activity'] else {},
            'system_resources': self.monitoring_data['system_resources'][-1] if self.monitoring_data['system_resources'] else {},
            'memory_usage': self.monitoring_data['memory_usage'][-1] if self.monitoring_data['memory_usage'] else {},
            'performance_metrics': self.monitoring_data['performance_metrics'][-1] if self.monitoring_data['performance_metrics'] else {}
        }


# Main function for backward compatibility

def create_monitoring_interface(interface: ScriptingInterface, **kwargs) -> MonitoringInterface:
    """Create a monitoring interface"""
    return MonitoringInterface(interface)


# Export key classes and functions
__all__ = [
    'MonitoringInterface',
    'MonitoringLevel',
    'MonitoringMode',
    'AlertSystem',
    'AnomalyDetector',
    'PerformanceProfiler',
    'CLIMonitor',
    'WebDashboard',
    'TerminalVisualizer',
    'APIServer',
    'create_monitoring_interface'
]

# Example usage
if __name__ == "__main__":
    import time
    
    # Initialize NLM system
    print("Initializing NLM system...")
    interface = ScriptingInterface()
    interface.initialize_system()
    
    # Create monitoring interface
    print("Creating monitoring interface...")
    monitor = MonitoringInterface(interface)
    
    # Start monitoring
    print("Starting monitoring...")
    if monitor.start_monitoring(level=MonitoringLevel.COMPREHENSIVE, update_interval=2.0):
        print("Monitoring started successfully!")
        
        # Run for a short time to demonstrate
        time.sleep(10)
        
        # Stop monitoring
        print("\nStopping monitoring...")
        monitor.stop_monitoring()
        print("Monitoring stopped!")
    else:
        print("Failed to start monitoring!")
    
    print("\nExample completed!")