pynlm/dashboard.py
Name: Dashboard module
Summary: Real-time monitoring with customizable dashboards

from pynlm.interface import ScriptingInterface, ScriptingEvent, EventType
import time
import threading
from typing import Dict, List, Any, Optional, Callable, Tuple
from dataclasses import dataclass
from enum import Enum
import json


class DashboardWidgetType(Enum):
    """Widget type enumeration"""
    METRICS = "metrics"
    PLOT = "plot"
    TABLE = "table"
    GAUGE = "gauge"
    HISTORICAL = "historical"
    REAL_TIME = "real_time"
    ALARM = "alarm"
    LOG = "log"


class Dashboard:
    """Main dashboard controller for real-time monitoring"""
    
    def __init__(self, interface: ScriptingInterface):
        self.interface = interface
        self.context = interface.context
        self.widgets: List[DashboardWidget] = []
        self.layout = None
        self.update_interval = 1.0
        self.auto_refresh = True
        self.history_length = 1000
        self.event_subscriptions = {}
        self.data_streams = {}
        self.websocket_server = None
        self.web_server = None
    
    def add_widget(self, widget_type: DashboardWidgetType, 
                  title: str, data_source: Callable, **kwargs) -> DashboardWidget:
        """Add a widget to the dashboard"""
        widget_configs = {
            DashboardWidgetType.METRICS: MetricsWidget,
            DashboardWidgetType.PLOT: PlotWidget,
            DashboardWidgetType.TABLE: TableWidget,
            DashboardWidgetType.GAUGE: GaugeWidget,
            DashboardWidgetType.HISTORICAL: HistoricalWidget,
            DashboardWidgetType.REAL_TIME: RealTimeWidget,
            DashboardWidgetType.ALARM: AlarmWidget,
            DashboardWidgetType.LOG: LogWidget
        }
        
        if widget_type not in widget_configs:
            raise ValueError(f"Unsupported widget type: {widget_type}")
        
        widget_class = widget_configs[widget_type]
        widget = widget_class(title, data_source, self.interface, **kwargs)
        self.widgets.append(widget)
        
        logger = __import__('logging').getLogger(__name__)
        logger.info(f"Added {widget_type} widget: {title}")
        
        return widget
    
    def remove_widget(self, widget_id: str) -> bool:
        """Remove a widget from the dashboard"""
        for i, widget in enumerate(self.widgets):
            if widget.id == widget_id:
                del self.widgets[i]
                return True
        return False
    
    def start_dashboard(self, update_interval: float = 1.0,
                       auto_refresh: bool = True) -> str:
        """Start dashboard with specified update interval"""
        self.update_interval = update_interval
        self.auto_refresh = auto_refresh
        
        # Start background thread for dashboard updates
        if self.widgets:
            self._update_thread = threading.Thread(
                target=self._dashboard_loop,
                daemon=True
            )
            self._update_thread.start()
        
        # Create web server if needed
        if any(widget.layout == 'web' for widget in self.widgets):
            self._start_web_server()
        
        logger = __import__('logging').getLogger(__name__)
        logger.info(f"Dashboard started with {len(self.widgets)} widgets, {update_interval}s interval")
        
        return "Dashboard started"
    
    def stop_dashboard(self):
        """Stop dashboard updates"""
        self.auto_refresh = False
        
        if hasattr(self, '_update_thread'):
            self._update_thread.join(timeout=5.0)
        
        if self.websocket_server:
            self.websocket_server.stop()
        
        if self.web_server:
            self.web_server.stop()
        
        logger = __import__('logging').getLogger(__name__)
        logger.info("Dashboard stopped")
    
    def start_websocket_server(self, port: int = 8080) -> str:
        """Start WebSocket server for real-time updates"""
        # In a real implementation, this would start a WebSocket server
        # For this example, we'll return a mock URL
        websocket_url = f"ws://localhost:{port}/dashboard"
        self.websocket_server = {
            'port': port,
            'url': websocket_url,
            'connected_clients': 0
        }
        
        logger = __import__('logging').getLogger(__name__)
        logger.info(f"WebSocket server started on {websocket_url}")
        
        return websocket_url
    
    def start_web_server(self, port: int = 8080):
        """Start web server for dashboard access"""
        # In a real implementation, this would start a web server
        # For this example, we'll return a mock URL
        self.web_server = {
            'port': port,
            'url': f"http://localhost:{port}",
            'widgets': len(self.widgets)
        }
        
        logger = __import__('logging').getLogger(__name__)
        logger.info(f"Web server started on port {port}")
    
    def subscribe_to_events(self, event_types: List[EventType], 
                          callback: Callable, widget_id: Optional[str] = None):
        """Subscribe to specific events"""
        for event_type in event_types:
            if event_type not in self.event_subscriptions:
                self.event_subscriptions[event_type] = []
            
            subscription = {
                'callback': callback,
                'widget_id': widget_id,
                'timestamp': time.time()
            }
            
            self.event_subscriptions[event_type].append(subscription)
    
    def add_data_stream(self, stream_id: str, data_provider: Callable,
                       update_interval: float = 1.0):
        """Add a data stream for real-time monitoring"""
        self.data_streams[stream_id] = {
            'provider': data_provider,
            'last_update': 0,
            'interval': update_interval,
            'data': None,
            'subscribers': []
        }
    
    def start_data_stream(self, stream_id: str):
        """Start a data stream"""
        if stream_id in self.data_streams:
            # Start background thread for data collection
            stream = self.data_streams[stream_id]
            stream_thread = threading.Thread(
                target=self._data_stream_loop,
                args=(stream_id,),
                daemon=True
            )
            stream['thread'] = stream_thread
            stream_thread.start()
    
    def stop_data_stream(self, stream_id: str):
        """Stop a data stream"""
        if stream_id in self.data_streams:
            stream = self.data_streams[stream_id]
            if hasattr(stream, 'thread'):
                stream['thread'].join(timeout=5.0)
            del self.data_streams[stream_id]
    
    def _dashboard_loop(self):
        """Internal dashboard update loop"""
        while self.auto_refresh and self.widgets:
            try:
                # Update all widgets
                for widget in self.widgets:
                    widget.update()
                
                # Check for new events
                for event_type, subscriptions in self.event_subscriptions.items():
                    for subscription in subscriptions[:]:  # Copy list to allow modification
                        if subscription['widget_id']:
                            # Widget-specific subscription
                            pass
                        else:
                            # Global subscription
                            pass
                
                # Update data streams
                for stream_id, stream in self.data_streams.items():
                    if time.time() - stream['last_update'] >= stream['interval']:
                        stream['data'] = stream['provider']()
                        stream['last_update'] = time.time()
                        
                        # Notify subscribers
                        for subscriber in stream['subscribers']:
                            try:
                                subscriber(stream['data'])
                            except Exception:
                                pass
                
                time.sleep(self.update_interval)
                
            except Exception as e:
                logger = __import__('logging').getLogger(__name__)
                logger.error(f"Dashboard loop error: {e}")
                time.sleep(1.0)
    
    def _data_stream_loop(self, stream_id: str):
        """Internal data stream update loop"""
        stream = self.data_streams.get(stream_id)
        if not stream:
            return
        
        while stream_id in self.data_streams:
            try:
                if time.time() - stream['last_update'] >= stream['interval']:
                    stream['data'] = stream['provider']()
                    stream['last_update'] = time.time()
                    
                    # Notify subscribers
                    for subscriber in stream['subscribers']:
                        try:
                            subscriber(stream['data'])
                        except Exception:
                            pass
                
                time.sleep(0.1)
                
            except Exception as e:
                logger = __import__('logging').getLogger(__name__)
                logger.error(f"Data stream error for {stream_id}: {e}")
                time.sleep(1.0)
    
    def get_dashboard_state(self) -> Dict[str, Any]:
        """Get complete dashboard state"""
        return {
            'widgets': [widget.get_state() for widget in self.widgets],
            'data_streams': {k: v['data'] for k, v in self.data_streams.items()},
            'event_subscriptions': len(self.event_subscriptions),
            'auto_refresh': self.auto_refresh,
            'update_interval': self.update_interval,
            'total_widgets': len(self.widgets)
        }
    
    def export_dashboard(self, format: str = 'json', filepath: Optional[str] = None):
        """Export dashboard configuration and state"""
        state = self.get_dashboard_state()
        
        if format.lower() == 'json':
            data = {
                'dashboard_state': state,
                'widgets': [widget.to_dict() for widget in self.widgets],
                'data_streams': {k: {
                    'provider': v['provider'].__name__ if callable(v['provider']) else str(v['provider']),
                    'interval': v['interval']
                } for k, v in self.data_streams.items()}
            }
        elif format.lower() == 'python':
            data = f"dashboard_config = {state}"
        else:
            raise ValueError(f"Unsupported export format: {format}")
        
        if filepath:
            with open(filepath, 'w') as f:
                if format.lower() == 'python':
                    f.write(data)
                else:
                    json.dump(data, f, indent=2, default=str)
        
        logger = __import__('logging').getLogger(__name__)
        logger.info(f"Dashboard exported to {filepath}")
        
        return data


class DashboardWidget:
    """Base class for dashboard widgets"""
    
    def __init__(self, title: str, data_source: Callable, 
                 interface: ScriptingInterface, **kwargs):
        self.id = f"{title.lower().replace(' ', '_')}_{int(time.time())}"
        self.title = title
        self.data_source = data_source
        self.interface = interface
        self.context = interface.context
        self.layout = kwargs.get('layout', 'console')
        self.refresh_interval = kwargs.get('refresh_interval', 1.0)
        self.position = kwargs.get('position', {'x': 0, 'y': 0, 'w': 4, 'h': 3})
        self.last_update = 0
        self.data = None
        self.error_count = 0
        self.max_errors = 3
    
    def update(self):
        """Update widget data"""
        if time.time() - self.last_update < self.refresh_interval:
            return
        
        try:
            self.data = self.data_source()
            self.error_count = 0
        except Exception as e:
            self.error_count += 1
            self.data = {'error': str(e)}
            logger = __import__('logging').getLogger(__name__)
            logger.error(f"Widget {self.title} update error: {e}")
        
        self.last_update = time.time()
    
    def get_state(self) -> Dict[str, Any]:
        """Get widget state"""
        return {
            'id': self.id,
            'title': self.title,
            'type': self.__class__.__name__,
            'layout': self.layout,
            'position': self.position,
            'last_update': self.last_update,
            'refresh_interval': self.refresh_interval,
            'data': self.data,
            'error_count': self.error_count,
            'is_error': self.error_count > 0
        }
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert widget to dictionary"""
        return self.get_state()


class MetricsWidget(DashboardWidget):
    """Widget for displaying key metrics"""
    
    def __init__(self, title: str, data_source: Callable, 
                 interface: ScriptingInterface, **kwargs):
        super().__init__(title, data_source, interface, **kwargs)
        self.metrics = kwargs.get('metrics', [])
    
    def update(self):
        """Update metrics widget"""
        super().update()
        
        if self.data is None or 'error' in self.data:
            return
        
        # Calculate summary statistics
        if isinstance(self.data, dict):
            for key, value in self.data.items():
                if isinstance(value, (list, tuple)):
                    self.data[f"{key}_min"] = min(value)
                    self.data[f"{key}_max"] = max(value)
                    self.data[f"{key}_avg"] = sum(value) / len(value)
                    self.data[f"{key}_count"] = len(value)


class PlotWidget(DashboardWidget):
    """Widget for displaying plots"""
    
    def __init__(self, title: str, data_source: Callable, 
                 interface: ScriptingInterface, **kwargs):
        super().__init__(title, data_source, interface, **kwargs)
        self.plot_type = kwargs.get('plot_type', 'line')
        self.x_axis = kwargs.get('x_axis', 'time')
        self.y_axis = kwargs.get('y_axis', 'value')
        self.color_scheme = kwargs.get('color_scheme', 'default')
    
    def update(self):
        """Update plot widget"""
        super().update()
        
        if self.data and 'error' not in self.data:
            # In a real implementation, this would generate plots
            pass


class TableWidget(DashboardWidget):
    """Widget for displaying tabular data"""
    
    def __init__(self, title: str, data_source: Callable, 
                 interface: ScriptingInterface, **kwargs):
        super().__init__(title, data_source, interface, **kwargs)
        self.columns = kwargs.get('columns', [])
        self.sort_by = kwargs.get('sort_by', '')
        self.sort_desc = kwargs.get('sort_desc', False)


class GaugeWidget(DashboardWidget):
    """Widget for displaying gauge indicators"""
    
    def __init__(self, title: str, data_source: Callable, 
                 interface: ScriptingInterface, **kwargs):
        super().__init__(title, data_source, interface, **kwargs)
        self.min_value = kwargs.get('min_value', 0)
        self.max_value = kwargs.get('max_value', 100)
        self.thresholds = kwargs.get('thresholds', [])


class HistoricalWidget(DashboardWidget):
    """Widget for displaying historical data"""
    
    def __init__(self, title: str, data_source: Callable, 
                 interface: ScriptingInterface, **kwargs):
        super().__init__(title, data_source, interface, **kwargs)
        self.history_length = kwargs.get('history_length', 100)
        self.history_data = []
    
    def update(self):
        """Update historical widget"""
        super().update()
        
        if self.data:
            self.history_data.append({
                'timestamp': time.time(),
                'data': self.data
            })
            
            # Trim history
            if len(self.history_data) > self.history_length:
                self.history_data = self.history_data[-self.history_length:]


class RealTimeWidget(DashboardWidget):
    """Widget for real-time data display"""
    
    def __init__(self, title: str, data_source: Callable, 
                 interface: ScriptingInterface, **kwargs):
        super().__init__(title, data_source, interface, **kwargs)
        self.refresh_interval = kwargs.get('refresh_interval', 0.5)
        self.animation_enabled = kwargs.get('animation_enabled', True)
        self.smoothing = kwargs.get('smoothing', 0.1)


class AlarmWidget(DashboardWidget):
    """Widget for displaying alarms and alerts"""
    
    def __init__(self, title: str, data_source: Callable, 
                 interface: ScriptingInterface, **kwargs):
        super().__init__(title, data_source, interface, **kwargs)
        self.alert_rules = kwargs.get('alert_rules', [])
        self.active_alarms = []
        self.acknowledged_alarms = []


class LogWidget(DashboardWidget):
    """Widget for displaying log messages"""
    
    def __init__(self, title: str, data_source: Callable, 
                 interface: ScriptingInterface, **kwargs):
        super().__init__(title, data_source, interface, **kwargs)
        self.log_level = kwargs.get('log_level', 'INFO')
        self.max_entries = kwargs.get('max_entries', 1000)
        self.log_entries = []
        self.filter_keywords = kwargs.get('filter_keywords', [])
    
    def add_log_entry(self, message: str, level: str = 'INFO', source: str = ''):
        """Add log entry"""
        if (level not in ['DEBUG', 'INFO', 'WARNING', 'ERROR', 'CRITICAL'] or
            (self.filter_keywords and not any(keyword.lower() in message.lower() 
                                             for keyword in self.filter_keywords))):
            return
        
        self.log_entries.append({
            'timestamp': time.time(),
            'message': message,
            'level': level,
            'source': source
        })
        
        # Trim log entries
        if len(self.log_entries) > self.max_entries:
            self.log_entries = self.log_entries[-self.max_entries:]
    
    def update(self):
        """Update log widget"""
        super().update()
        
        if self.data and 'error' not in self.data:
            # Process log data
            if isinstance(self.data, dict) and 'logs' in self.data:
                for log_entry in self.data['logs']:
                    self.add_log_entry(**log_entry)


class WebDashboard:
    """Web-based dashboard for NLM system"""
    
    def __init__(self, interface: ScriptingInterface):
        self.interface = interface
        self.context = interface.context
        self.server_url = "http://localhost:8080"
        self.websocket_url = "ws://localhost:8080"
        self.widgets = []
        self.data_sources = {}
        self.simulation_data = []
    
    def start(self, port: int = 8080, host: str = 'localhost'):
        """Start web dashboard server"""
        self.server_url = f"http://{host}:{port}"
        self.websocket_url = f"ws://{host}:{port}"
        
        # In a real implementation, this would start a web server
        # For this example, we'll return the URL
        
        logger = __import__('logging').getLogger(__name__)
        logger.info(f"Web dashboard started on {self.server_url}")
        
        return self.server_url
    
    def add_neural_monitor_widget(self, widget_id: str, region_id: int = 0,
                                 metrics: Optional[List[str]] = None):
        """Add neural monitor widget"""
        if metrics is None:
            metrics = ['activity', 'firing_rate', 'potential', 'synaptic_weight']
        
        widget = {
            'id': widget_id,
            'type': 'neural_monitor',
            'region_id': region_id,
            'metrics': metrics,
            'last_update': time.time(),
            'data': {}
        }
        
        self.widgets.append(widget)
        
        # Add data source
        self.data_sources[widget_id] = lambda: self._get_neural_data(region_id, metrics)
        
        return widget
    
    def add_simulation_widget(self, widget_id: str, 
                             plot_type: str = 'activity',
                             x_axis: str = 'time',
                             y_axis: str = 'value'):
        """Add simulation widget"""
        widget = {
            'id': widget_id,
            'type': 'simulation',
            'plot_type': plot_type,
            'x_axis': x_axis,
            'y_axis': y_axis,
            'last_update': time.time(),
            'data': {'times': [], 'values': []}
        }
        
        self.widgets.append(widget)
        
        # Add data source
        self.data_sources[widget_id] = lambda: self._get_simulation_data(plot_type, x_axis, y_axis)
        
        return widget
    
    def add_control_panel(self, widget_id: str, 
                         controls: Optional[List[Dict]] = None):
        """Add control panel widget"""
        if controls is None:
            controls = [
                {'type': 'button', 'id': 'pause', 'label': 'Pause', 'action': 'pause'},
                {'type': 'button', 'id': 'resume', 'label': 'Resume', 'action': 'resume'},
                {'type': 'button', 'id': 'step', 'label': 'Step', 'action': 'step'},
                {'type': 'slider', 'id': 'speed', 'label': 'Speed', 'min': 0.1, 'max': 10.0, 'value': 1.0}
            ]
        
        widget = {
            'id': widget_id,
            'type': 'control_panel',
            'controls': controls,
            'last_update': time.time(),
            'data': {}
        }
        
        self.widgets.append(widget)
        
        return widget
    
    def add_event_monitor_widget(self, widget_id: str,
                                event_types: Optional[List[str]] = None,
                                filter_source: Optional[str] = None):
        """Add event monitor widget"""
        if event_types is None:
            event_types = ['neuron_fired', 'state_changed', 'action_produced']
        
        widget = {
            'id': widget_id,
            'type': 'event_monitor',
            'event_types': event_types,
            'filter_source': filter_source,
            'last_update': time.time(),
            'data': {'events': []}
        }
        
        self.widgets.append(widget)
        
        return widget
    
    def update_widget(self, widget_id: str, data: Dict[str, Any]):
        """Update widget data"""
        for widget in self.widgets:
            if widget['id'] == widget_id:
                widget['data'].update(data)
                widget['last_update'] = time.time()
                break
    
    def _get_neural_data(self, region_id: int, metrics: List[str]) -> Dict[str, Any]:
        """Get neural data for monitoring"""
        if not self.interface.context.brain:
            return {}
        
        brain = self.interface.context.brain
        regions = brain.getRegions()
        
        if region_id >= len(regions):
            return {}
        
        region = regions[region_id]
        neurons = region.getNeurons()
        
        data = {}
        
        for metric in metrics:
            if metric == 'activity':
                data['activity'] = [n.getFiringState() for n in neurons]
            elif metric == 'firing_rate':
                data['firing_rate'] = [n.getFiringRate() for n in neurons]
            elif metric == 'potential':
                data['potential'] = [n.getMembranePotential() for n in neurons]
            elif metric == 'synaptic_weight':
                weights = []
                for neuron in neurons:
                    total_weight = 0
                    for synapse in neuron.getSynapses():
                        total_weight += synapse.getStrength()
                    weights.append(total_weight)
                data['synaptic_weight'] = weights
        
        return data
    
    def _get_simulation_data(self, plot_type: str, x_axis: str, y_axis: str) -> Dict[str, Any]:
        """Get simulation data for plotting"""
        # Get current state
        state = self.interface.get_state() if hasattr(self.interface, 'get_state') else {}
        
        data = {'times': [], 'values': []}
        
        # Add current state
        data['times'].append(time.time())
        
        if plot_type == 'activity':
            value = state.get('average_firing_rate', 0)
        elif plot_type == 'neurons':
            value = state.get('active_neurons', 0)
        elif plot_type == 'spikes':
            value = state.get('total_spikes', 0)
        else:
            value = 0
        
        data['values'].append(value)
        
        return data
    
    def get_dashboard_config(self) -> Dict[str, Any]:
        """Get complete dashboard configuration"""
        return {
            'server_url': self.server_url,
            'websocket_url': self.websocket_url,
            'widgets': self.widgets,
            'data_sources': {k: v.__name__ if callable(v) else str(v) for k, v in self.data_sources.items()},
            'simulation_data': self.simulation_data
        }
    
    def export_dashboard(self, filepath: str, format: str = 'json'):
        """Export dashboard configuration"""
        config = self.get_dashboard_config()
        
        if format.lower() == 'json':
            with open(filepath, 'w') as f:
                json.dump(config, f, indent=2, default=str)
        elif format.lower() == 'python':
            with open(filepath, 'w') as f:
                f.write(f"dashboard_config = {config}")
        else:
            raise ValueError(f"Unsupported format: {format}")
        
        logger = __import__('logging').getLogger(__name__)
        logger.info(f"Dashboard exported to {filepath}")


# Export key classes and functions
__all__ = [
    'Dashboard',
    'DashboardWidget',
    'DashboardWidgetType',
    'MetricsWidget',
    'PlotWidget',
    'TableWidget',
    'GaugeWidget',
    'HistoricalWidget',
    'RealTimeWidget',
    'AlarmWidget',
    'LogWidget',
    'WebDashboard'
]
