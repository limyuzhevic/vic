#!/usr/bin/env bash
# Visualization and Monitoring Tools for NLM
# Real-time visualization, monitoring, and analysis tools for neural simulations

import os
import sys
import json
import time
import threading
import websocket
import socketio
from typing import Dict, Any, List, Optional, Tuple, Callable
from dataclasses import dataclass, field
from enum import Enum
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import plotly.graph_objs as go
import plotly.io as pio
from datetime import datetime
import subprocess
import psutil
import pandas as pd

# Real-time Visualization and Monitoring System

class VisualizationType(Enum):
    NEURAL_NETWORK = "neural_network"
    TIME_SERIES = "time_series"
    HEAT_MAP = "heat_map"
    3D_REPRESENTATION = "3d_representation"
    SPATIAL_DISTRIBUTION = "spatial_distribution"
    FUNCTIONAL_CONNECTIVITY = "functional_connectivity"
    DEVELOPMENTAL_TRACKING = "developmental_tracking"
    NEUROMODULATION = "neuromodulation"
    MEMORY_ACTIVITY = "memory_activity"
    PLASTICITY_DYNAMICS = "plasticity_dynamics"

class MonitoringLevel(Enum):
    BASIC = "basic"
    INTERMEDIATE = "intermediate"
    ADVANCED = "advanced"
    COMPREHENSIVE = "comprehensive"

@dataclass
class NeuralNode:
    """Represents a neuron in visualization"""
    id: int
    type: str  # 'excitatory', 'inhibitory', 'sensory', 'motor', 'internal'
    region: str
    population: str
    x: float
    y: float
    z: float = 0.0
    activity: float
    firing_state: str
    connections: List[int] = field(default_factory=list)
    color: str = "#333366"
    size: float = 5.0
    pulse_effect: bool = False
    glow_intensity: float = 0.0

@dataclass
class NeuralConnection:
    """Represents a synaptic connection in visualization"""
    source_id: int
    target_id: int
    weight: float
    type: str  # 'excitatory', 'inhibitory', 'electrical'
    strength: float
    active: bool
    color: str = "#666666"
    opacity: float = 1.0

@dataclass
class MonitoringData:
    """Real-time monitoring data"""
    timestamp: float
    simulation_step: int
    neurons: List[NeuralNode]
    connections: List[NeuralConnection]
    statistics: Dict[str, Any]
    events: List[Dict[str, Any]]
    memory_usage: Dict[str, float]
    performance_metrics: Dict[str, float]

class RealTimeVisualizer:
    def __init__(self):
        self.current_data: Optional[MonitoringData] = None
        self.visualization_type: VisualizationType = VisualizationType.NEURAL_NETWORK
        self.monitoring_level: MonitoringLevel = MonitoringLevel.INTERMEDIATE
        self.running: bool = False
        self.clients: List[Any] = []  # WebSocket clients
        self.sio = socketio.Server(async_mode='threading')
        self.thread: Optional[threading.Thread] = None
        self.websocket_thread: Optional[threading.Thread] = None
        
        # Data buffers for time series
        self.activity_buffer: List[float] = []
        self.spike_buffer: List[float] = []
        self.memory_buffer: List[float] = []
        self.plasticity_buffer: List[float] = []
        
        # Performance tracking
        self.frame_times: List[float] = []
        self.update_times: List[float] = []
    
    def start_server(self, host: str = 'localhost', port: int = 8080) -> bool:
        """Start the real-time visualization server"""
        try:
            @self.sio.on('connect')
            def connect(sid, environ):
                print(f"Client connected: {sid}")
                self.clients.append(sid)
                
                # Send current data to new client
                if self.current_data:
                    self.sio.emit('neural_data', {
                        'data': self._serialize_monitoring_data(self.current_data),
                        'type': self.visualization_type.value
                    }, sid=sid)
            
            @self.sio.on('disconnect')
            def disconnect(sid):
                print(f"Client disconnected: {sid}")
                if sid in self.clients:
                    self.clients.remove(sid)
            
            @self.sio.on('update_visualization')
            def update_visualization(sid, data):
                self.visualization_type = VisualizationType(data.get('type', 'neural_network'))
                self.monitoring_level = MonitoringLevel(data.get('level', 'intermediate'))
                print(f"Client {sid} updated visualization to {self.visualization_type.value}")
            
            @self.sio.on('request_data')
            def request_data(sid, data):
                if self.current_data:
                    self.sio.emit('neural_data', {
                        'data': self._serialize_monitoring_data(self.current_data),
                        'type': self.visualization_type.value
                    }, sid=sid)
            
            # Start Socket.IO server in a separate thread
            self.running = True
            self.thread = threading.Thread(target=self._run_socketio, args=(host, port), daemon=True)
            self.thread.start()
            
            # Start WebSocket server for real-time updates
            self.websocket_thread = threading.Thread(target=self._run_websocket, args=(host, port + 1), daemon=True)
            self.websocket_thread.start()
            
            print(f"Real-time visualization server started on {host}:{port}")
            return True
        except Exception as e:
            print(f"Error starting visualization server: {e}")
            return False
    
    def _run_socketio(self, host: str, port: int) -> None:
        """Run Socket.IO server"""
        try:
            from eventlet import wsgi
            wsgi.server(self.sio, host, port)
        except ImportError:
            # Fallback to simple WSGI if eventlet not available
            try:
                from wsgiref.simple_server import make_server
                app = self.sio
                httpd = make_server(host, port, app)
                print(f"Socket.IO server running on http://{host}:{port}")
                httpd.serve_forever()
            except ImportError:
                print("Warning: Cannot start visualization server (required dependencies missing)")
    
    def _run_websocket(self, host: str, port: int) -> None:
        """Run WebSocket server for real-time updates"""
        try:
            import asyncio
            from websockets.server import serve
            
            async def websocket_handler(websocket, path):
                print(f"WebSocket client connected: {websocket.remote_address}")
                self.clients.append(websocket)
                
                try:
                    async for message in websocket:
                        data = json.loads(message)
                        if data.get('type') == 'update':
                            self._process_client_update(data)
                except websockets.exceptions.ConnectionClosed:
                    pass
                finally:
                    if websocket in self.clients:
                        self.clients.remove(websocket)
                        print(f"WebSocket client disconnected: {websocket.remote_address}")
            
            async def start_server():
                async with serve(websocket_handler, host, port):
                    await asyncio.Future()  # Run forever
            
            asyncio.run(start_server())
        except ImportError:
            print("Warning: Cannot start WebSocket server (websockets library not available)")
    
    def update_data(self, monitoring_data: MonitoringData) -> None:
        """Update visualization data"""
        self.current_data = monitoring_data
        
        # Update data buffers
        self._update_data_buffers(monitoring_data)
        
        # Broadcast to all connected clients
        if self.current_data:
            serialized_data = self._serialize_monitoring_data(self.current_data)
            
            # Socket.IO broadcast
            for sid in self.clients:
                try:
                    self.sio.emit('neural_data', {
                        'data': serialized_data,
                        'type': self.visualization_type.value
                    }, sid=sid)
                except Exception as e:
                    print(f"Error broadcasting to client {sid}: {e}")
            
            # WebSocket broadcast
            for client in self.clients:
                try:
                    if hasattr(client, 'send'):  # WebSocket client
                        import asyncio
                        asyncio.create_task(client.send(json.dumps({
                            'type': 'neural_data',
                            'data': serialized_data,
                            'visualization_type': self.visualization_type.value
                        }))
                except Exception as e:
                    print(f"Error broadcasting to WebSocket client: {e}")
    
    def _update_data_buffers(self, monitoring_data: MonitoringData) -> None:
        """Update internal data buffers for time series analysis"""
        current_time = monitoring_data.timestamp
        
        # Update activity buffer
        if monitoring_data.statistics:
            total_activity = monitoring_data.statistics.get('total_activity', 0)
            self.activity_buffer.append(total_activity)
            if len(self.activity_buffer) > 1000:
                self.activity_buffer.pop(0)
        
        # Update spike buffer
        spike_count = monitoring_data.statistics.get('spike_count', 0)
        self.spike_buffer.append(spike_count)
        if len(self.spike_buffer) > 1000:
            self.spike_buffer.pop(0)
        
        # Update memory buffer
        if monitoring_data.memory_usage:
            working_memory = monitoring_data.memory_usage.get('working_memory', 0)
            self.memory_buffer.append(working_memory)
            if len(self.memory_buffer) > 1000:
                self.memory_buffer.pop(0)
        
        # Update plasticity buffer
        if monitoring_data.statistics:
            plasticity_change = monitoring_data.statistics.get('plasticity_change', 0)
            self.plasticity_buffer.append(plasticity_change)
            if len(self.plasticity_buffer) > 1000:
                self.plasticity_buffer.pop(0)
    
    def _serialize_monitoring_data(self, monitoring_data: MonitoringData) -> Dict[str, Any]:
        """Serialize monitoring data for transmission"""
        return {
            'timestamp': monitoring_data.timestamp,
            'simulation_step': monitoring_data.simulation_step,
            'statistics': monitoring_data.statistics,
            'events': monitoring_data.events,
            'memory_usage': monitoring_data.memory_usage,
            'performance_metrics': monitoring_data.performance_metrics,
            'visualization_config': {
                'type': self.visualization_type.value,
                'level': self.monitoring_level.value,
                'update_rate': getattr(monitoring_data, 'update_rate', 1000)
            }
        }
    
    def _process_client_update(self, data: Dict[str, Any]) -> None:
        """Process client visualization update request"""
        if 'visualization_type' in data:
            try:
                self.visualization_type = VisualizationType(data['visualization_type'])
            except ValueError:
                print(f"Unknown visualization type: {data['visualization_type']}")
        
        if 'monitoring_level' in data:
            try:
                self.monitoring_level = MonitoringLevel(data['monitoring_level'])
            except ValueError:
                print(f"Unknown monitoring level: {data['monitoring_level']}")
        
        if 'filters' in data:
            # Apply visualization filters
            pass
    
    def generate_static_visualization(self, output_file: str, format: str = 'html') -> bool:
        """Generate static visualization file"""
        try:
            if not self.current_data:
                print("No data available for visualization")
                return False
            
            if format == 'html':
                return self._generate_html_visualization(output_file)
            elif format == 'png':
                return self._generate_png_visualization(output_file)
            elif format == 'json':
                return self._generate_json_visualization(output_file)
            else:
                print(f"Unsupported visualization format: {format}")
                return False
        except Exception as e:
            print(f"Error generating visualization: {e}")
            return False
    
    def _generate_html_visualization(self, output_file: str) -> bool:
        """Generate HTML visualization"""
        html_template = f"""
<!DOCTYPE html>
<html>
<head>
    <title>NLM Real-time Visualization</title>
    <script src="https://cdn.jsdelivr.net/npm/three@0.132.2/build/three.min.js"></script>
    <script src="https://cdn.jsdelivr.net/npm/three@0.132.2/examples/js/controls/OrbitControls.min.js"></script>
    <script src="https://cdn.jsdelivr.net/npm/three@0.132.2/examples/js/loaders/GLTFLoader.min.js"></script>
    <style>
        body {{ margin: 0; overflow: hidden; background-color: #1a1a1a; }}
        #canvas {{ display: block; }}
        #info {{ position: absolute; top: 10px; left: 10px; color: white; font-family: Arial, sans-serif; }}
        #stats {{ position: absolute; bottom: 10px; right: 10px; color: white; background: rgba(0,0,0,0.7); padding: 10px; border-radius: 5px; }}
        .neuron {{ position: absolute; border-radius: 50%; transition: all 0.3s ease; }}
        .connection {{ position: absolute; background: rgba(255,255,255,0.3); transform-origin: 0 0; }}
    </style>
</head>
<body>
    <div id="info">
        <h1>NLM Neural Network Visualization</h1>
        <p>Real-time neural activity simulation</p>
        <div id="controls">
            <label>Visualization Type:</label>
            <select id="vizType">
                <option value="neural_network">Neural Network</option>
                <option value="time_series">Time Series</option>
                <option value="heat_map">Heat Map</option>
                <option value="3d_representation">3D Representation</option>
            </select>
            <label>Update Rate:</label>
            <select id="updateRate">
                <option value="100">Real-time</option>
                <option value="1000">1s</option>
                <option value="5000">5s</option>
                </select>
        </div>
    </div>
    
    <canvas id="canvas"></canvas>
    
    <div id="stats">
        <h3>Statistics</h3>
        <div id="statContent"></div>
    </div>
    
    <script>
        // Three.js visualization setup
        const scene = new THREE.Scene();
        const camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 0.1, 1000);
        const renderer = new THREE.WebGLRenderer({{ antialias: true }});
        renderer.setSize(window.innerWidth, window.innerHeight);
        renderer.setPixelRatio(window.devicePixelRatio);
        document.body.appendChild(renderer.domElement);
        
        // Lighting
        const ambientLight = new THREE.AmbientLight(0x404040);
        scene.add(ambientLight);
        const directionalLight = new THREE.DirectionalLight(0xffffff, 0.8);
        directionalLight.position.set(10, 10, 5);
        scene.add(directionalLight);
        
        // Neural network visualization
        const neurons = [];
        const connections = [];
        const neuronGeometry = new THREE.SphereGeometry(1, 32, 32);
        const neuronMaterial = new THREE.MeshBasicMaterial({{ color: 0x333366 }});
        
        // Load current visualization data
        const neuralData = {data: {}};
        
        function updateVisualization() {{
            // Update Three.js scene based on neural data
            // This would be populated with actual neural network data
        }}
        
        function animate() {{
            requestAnimationFrame(animate);
            
            const startTime = performance.now();
            
            updateVisualization();
            
            const endTime = performance.now();
            const frameTime = endTime - startTime;
            
            // Update stats
            document.getElementById('statContent').innerHTML = `
                <div>Frame time: ${{frameTime.toFixed(2)}}ms</div>
                <div>FPS: ${{Math.round(1000 / frameTime)}}</div>
                <div>Neurons: ${{neurons.length}}</div>
                <div>Connections: ${{connections.length}}</div>
            `;
        }}
        
        // Handle window resize
        window.addEventListener('resize', () => {{
            camera.aspect = window.innerWidth / window.innerHeight;
            camera.updateProjectionMatrix();
            renderer.setSize(window.innerWidth, window.innerHeight);
        }});
        
        // Start animation
        animate();
        
        // WebSocket connection for real-time updates
        const socket = new WebSocket('ws://localhost:8081');
        
        socket.addEventListener('message', (event) => {{
            const data = JSON.parse(event.data);
            if (data.type === 'neural_data') {{
                neuralData.data = data.data;
                updateVisualization();
            }}
        }});
        
        // Control panel event listeners
        document.getElementById('vizType').addEventListener('change', (e) => {{
            socket.send(JSON.stringify({{ type: 'update', visualization_type: e.target.value }}));
        }});
        
        document.getElementById('updateRate').addEventListener('change', (e) => {{
            socket.send(JSON.stringify({{ type: 'update', update_rate: parseInt(e.target.value) }}));
        }});
    </script>
</body>
</html>
"""
            
            with open(output_file, 'w') as f:
                f.write(html_template)
            
            print(f"HTML visualization generated: {output_file}")
            return True
        except Exception as e:
            print(f"Error generating HTML visualization: {e}")
            return False
    
    def _generate_png_visualization(self, output_file: str) -> bool:
        """Generate PNG visualization using matplotlib"""
        try:
            import matplotlib.pyplot as plt
            import matplotlib.image as mpimg
            
            # Create a simple visualization based on current data
            fig, axes = plt.subplots(2, 2, figsize=(12, 10))
            fig.suptitle('NLM Neural Network Analysis', fontsize=16)
            
            # Plot 1: Neural activity over time
            if self.activity_buffer:
                axes[0, 0].plot(self.activity_buffer)
                axes[0, 0].set_title('Total Neural Activity')
                axes[0, 0].set_xlabel('Time Step')
                axes[0, 0].set_ylabel('Activity')
            
            # Plot 2: Spike rate over time
            if self.spike_buffer:
                axes[0, 1].plot(self.spike_buffer)
                axes[0, 1].set_title('Spike Rate')
                axes[0, 1].set_xlabel('Time Step')
                axes[0, 1].set_ylabel('Spikes')
            
            # Plot 3: Memory usage over time
            if self.memory_buffer:
                axes[1, 0].plot(self.memory_buffer)
                axes[1, 0].set_title('Memory Usage')
                axes[1, 0].set_xlabel('Time Step')
                axes[1, 0].set_ylabel('Memory Units')
            
            # Plot 4: Plasticity changes over time
            if self.plasticity_buffer:
                axes[1, 1].plot(self.plasticity_buffer)
                axes[1, 1].set_title('Plasticity Changes')
                axes[1, 1].set_xlabel('Time Step')
                axes[1, 1].set_ylabel('Change')
            
            plt.tight_layout()
            plt.savefig(output_file, dpi=150, bbox_inches='tight')
            plt.close()
            
            print(f"PNG visualization generated: {output_file}")
            return True
        except Exception as e:
            print(f"Error generating PNG visualization: {e}")
            return False
    
    def _generate_json_visualization(self, output_file: str) -> bool:
        """Generate JSON visualization data"""
        try:
            if not self.current_data:
                return False
            
            viz_data = {
                'metadata': {
                    'timestamp': time.time(),
                    'version': '1.0',
                    'visualization_type': self.visualization_type.value,
                    'monitoring_level': self.monitoring_level.value
                },
                'neural_network': {
                    'neurons': [self._serialize_neuron(neuron) for neuron in self.current_data.neurons],
                    'connections': [self._serialize_connection(conn) for conn in self.current_data.connections]
                },
                'statistics': self.current_data.statistics,
                'time_series': {
                    'activity': self.activity_buffer,
                    'spikes': self.spike_buffer,
                    'memory': self.memory_buffer,
                    'plasticity': self.plasticity_buffer
                }
            }
            
            with open(output_file, 'w') as f:
                json.dump(viz_data, f, indent=2, default=str)
            
            print(f"JSON visualization generated: {output_file}")
            return True
        except Exception as e:
            print(f"Error generating JSON visualization: {e}")
            return False
    
    def _serialize_neuron(self, neuron: NeuralNode) -> Dict[str, Any]:
        """Serialize neuron for visualization"""
        return {
            'id': neuron.id,
            'type': neuron.type,
            'region': neuron.region,
            'population': neuron.population,
            'position': {'x': neuron.x, 'y': neuron.y, 'z': neuron.z},
            'activity': neuron.activity,
            'firing_state': neuron.firing_state,
            'connections': neuron.connections,
            'color': neuron.color,
            'size': neuron.size,
            'pulse_effect': neuron.pulse_effect,
            'glow_intensity': neuron.glow_intensity
        }
    
    def _serialize_connection(self, connection: NeuralConnection) -> Dict[str, Any]:
        """Serialize connection for visualization"""
        return {
            'source_id': connection.source_id,
            'target_id': connection.target_id,
            'weight': connection.weight,
            'type': connection.type,
            'strength': connection.strength,
            'active': connection.active,
            'color': connection.color,
            'opacity': connection.opacity
        }
    
    def stop(self) -> None:
        """Stop the visualization server"""
        self.running = False
        if self.thread:
            self.thread.join(timeout=5)
        if self.websocket_thread:
            self.websocket_thread.join(timeout=5)
        print("Visualization server stopped")

class PerformanceMonitor:
    def __init__(self):
        self.monitoring_data: List[MonitoringData] = []
        self.performance_thresholds: Dict[str, float] = {
            'max_cpu_usage': 80.0,
            'max_memory_usage': 90.0,
            'max_frame_time': 16.67,  # 60 FPS
            'min_simulation_speed': 10.0  # steps per second
        }
        self.alert_handlers: List[Callable] = []
        self.performance_history: Dict[str, List[float]] = {
            'cpu_usage': [],
            'memory_usage': [],
            'frame_time': [],
            'simulation_speed': []
        }
    
    def record_performance_metrics(self, metrics: Dict[str, Any]) -> None:
        """Record performance metrics"""
        current_time = time.time()
        
        # Get system metrics
        cpu_percent = psutil.cpu_percent(interval=None)
        memory_percent = psutil.virtual_memory().percent
        
        # Calculate derived metrics
        frame_time = 1000.0 / metrics.get('frame_rate', 60) if metrics.get('frame_rate') else 0.0
        simulation_speed = metrics.get('simulation_steps_per_second', 0.0)
        
        # Store metrics
        self.performance_history['cpu_usage'].append(cpu_percent)
        self.performance_history['memory_usage'].append(memory_percent)
        self.performance_history['frame_time'].append(frame_time)
        self.performance_history['simulation_speed'].append(simulation_speed)
        
        # Keep only recent data (last hour)
        current_time_cutoff = current_time - 3600
        
        # Check for performance issues
        self._check_performance_issues(cpu_percent, memory_percent, frame_time, simulation_speed)
        
        # Generate alert if needed
        alert = self._generate_performance_alert(cpu_percent, memory_percent, frame_time, simulation_speed)
        if alert:
            self._trigger_alert(alert)
    
    def _check_performance_issues(self, cpu_percent: float, memory_percent: float, 
                                 frame_time: float, simulation_speed: float) -> None:
        """Check for performance issues"""
        issues = []
        
        if cpu_percent > self.performance_thresholds['max_cpu_usage']:
            issues.append({
                'type': 'high_cpu_usage',
                'value': cpu_percent,
                'threshold': self.performance_thresholds['max_cpu_usage'],
                'severity': 'medium' if cpu_percent < 90 else 'high'
            })
        
        if memory_percent > self.performance_thresholds['max_memory_usage']:
            issues.append({
                'type': 'high_memory_usage',
                'value': memory_percent,
                'threshold': self.performance_thresholds['max_memory_usage'],
                'severity': 'medium' if memory_percent < 95 else 'high'
            })
        
        if frame_time > self.performance_thresholds['max_frame_time']:
            issues.append({
                'type': 'high_frame_time',
                'value': frame_time,
                'threshold': self.performance_thresholds['max_frame_time'],
                'severity': 'high' if frame_time > 33 else 'medium'
            })
        
        if simulation_speed > 0 and simulation_speed < self.performance_thresholds['min_simulation_speed']:
            issues.append({
                'type': 'low_simulation_speed',
                'value': simulation_speed,
                'threshold': self.performance_thresholds['min_simulation_speed'],
                'severity': 'medium'
            })
        
        # Trigger alerts for issues
        for issue in issues:
            alert = {
                'type': issue['type'],
                'message': f"{issue['type'].replace('_', ' ').title()} detected: {issue['value']:.1f}%",
                'severity': issue['severity'],
                'value': issue['value'],
                'threshold': issue['threshold'],
                'timestamp': time.time()
            }
            self._trigger_alert(alert)
    
    def _generate_performance_alert(self, cpu_percent: float, memory_percent: float, 
                                    frame_time: float, simulation_speed: float) -> Optional[Dict[str, Any]]:
        """Generate performance alert"""
        # Check for critical issues
        if cpu_percent > 95 or memory_percent > 95:
            return {
                'type': 'critical_performance_issue',
                'message': f"Critical performance issue detected",
                'severity': 'critical',
                'cpu_percent': cpu_percent,
                'memory_percent': memory_percent,
                'timestamp': time.time()
            }
        
        # Check for simulation performance degradation
        if len(self.performance_history['simulation_speed']) > 10:
            recent_avg = np.mean(self.performance_history['simulation_speed'][-10:])
            overall_avg = np.mean(self.performance_history['simulation_speed'])
            
            if overall_avg > 0 and (recent_avg / overall_avg) < 0.5:
                return {
                    'type': 'simulation_performance_degradation',
                    'message': f"Simulation performance degraded",
                    'severity': 'medium',
                    'recent_speed': recent_avg,
                    'overall_speed': overall_avg,
                    'timestamp': time.time()
                }
        
        return None
    
    def _trigger_alert(self, alert: Dict[str, Any]) -> None:
        """Trigger performance alert"""
        print(f"PERFORMANCE ALERT: {alert['message']} (Severity: {alert['severity']})")
        
        # Call alert handlers
        for handler in self.alert_handlers:
            try:
                handler(alert)
            except Exception as e:
                print(f"Error in alert handler: {e}")
    
    def add_alert_handler(self, handler: Callable) -> None:
        """Add alert handler"""
        self.alert_handlers.append(handler)
    
    def generate_performance_report(self) -> Dict[str, Any]:
        """Generate comprehensive performance report"""
        if not self.performance_history['cpu_usage']:
            return {'error': 'No performance data available'}
        
        report = {
            'summary': {
                'total_samples': len(self.performance_history['cpu_usage']),
                'average_cpu_usage': np.mean(self.performance_history['cpu_usage']),
                'average_memory_usage': np.mean(self.performance_history['memory_usage']),
                'average_frame_time': np.mean(self.performance_history['frame_time']),
                'average_simulation_speed': np.mean(self.performance_history['simulation_speed']),
                'peak_cpu_usage': np.max(self.performance_history['cpu_usage']),
                'peak_memory_usage': np.max(self.performance_history['memory_usage'])
            },
            'trends': self._calculate_performance_trends(),
            'anomalies': self._detect_performance_anomalies(),
            'recommendations': self._generate_performance_recommendations()
        }
        
        return report
    
    def _calculate_performance_trends(self) -> Dict[str, Any]:
        """Calculate performance trends"""
        if len(self.performance_history['cpu_usage']) < 2:
            return {}
        
        trends = {}
        
        for metric_name, data in self.performance_history.items():
            if len(data) < 2:
                continue
            
            # Calculate linear trend
            x = np.arange(len(data))
            y = np.array(data)
            
            slope = np.polyfit(x, y, 1)[0]
            correlation = np.corrcoef(x, y)[0, 1]
            
            trends[metric_name] = {
                'slope': slope,
                'correlation': correlation,
                'direction': 'increasing' if slope > 0 else 'decreasing',
                'rate_of_change': abs(slope)
            }
        
        return trends
    
    def _detect_performance_anomalies(self) -> List[Dict[str, Any]]:
        """Detect performance anomalies"""
        anomalies = []
        
        for metric_name, data in self.performance_history.items():
            if len(data) < 3:
                continue
            
            data_array = np.array(data)
            mean = np.mean(data_array)
            std = np.std(data_array)
            
            # Detect outliers using Z-score
            z_scores = np.abs((data_array - mean) / std) if std > 0 else np.zeros(len(data_array))
            
            for i, z_score in enumerate(z_scores):
                if z_score > 2.0:  # 2-sigma threshold
                    anomalies.append({
                        'metric': metric_name,
                        'value': data_array[i],
                        'mean': mean,
                        'z_score': z_score,
                        'timestamp': time.time() - (len(data) - i) * 60  # Approximate timestamp
                    })
        
        return anomalies
    
    def _generate_performance_recommendations(self) -> List[str]:
        """Generate performance optimization recommendations"""
        recommendations = []
        
        trends = self._calculate_performance_trends()
        
        # Check CPU trends
        if 'cpu_usage' in trends:
            cpu_trend = trends['cpu_usage']
            if cpu_trend['direction'] == 'increasing' and cpu_trend['rate_of_change'] > 0.1:
                recommendations.append(
                    "CPU usage is increasing over time. Consider optimizing computational load or adding parallel processing."
                )
        
        # Check memory trends
        if 'memory_usage' in trends:
            memory_trend = trends['memory_usage']
            if memory_trend['direction'] == 'increasing' and memory_trend['rate_of_change'] > 0.05:
                recommendations.append(
                    "Memory usage is increasing. Check for memory leaks or optimize data structures."
                )
        
        # Check simulation speed
        if 'simulation_speed' in trends:
            speed_trend = trends['simulation_speed']
            if speed_trend['direction'] == 'decreasing' and speed_trend['rate_of_change'] > 0.05:
                recommendations.append(
                    "Simulation speed is decreasing. Consider performance optimizations or hardware upgrades."
                )
        
        # Check frame time
        if 'frame_time' in trends:
            frame_trend = trends['frame_time']
            if frame_trend['direction'] == 'increasing':
                recommendations.append(
                    "Frame time is increasing. Consider reducing visual complexity or optimizing rendering pipeline."
                )
        
        return recommendations

# Main command-line interface
def main():
    import argparse
    
    parser = argparse.ArgumentParser(description='NLM Visualization and Monitoring Tools')
    parser.add_argument('--mode', choices=['server', 'visualize', 'monitor', 'analyze'], default='server',
                       help='Operating mode')
    parser.add_argument('--host', default='localhost', help='Host for server')
    parser.add_argument('--port', type=int, default=8080, help='Port for server')
    parser.add_argument('--output', '-o', help='Output file for static visualization')
    parser.add_argument('--format', choices=['html', 'png', 'json'], default='html',
                       help='Output format for static visualization')
    parser.add_argument('--level', choices=['basic', 'intermediate', 'advanced', 'comprehensive'],
                       default='intermediate', help='Monitoring level')
    parser.add_argument('--duration', type=int, default=3600, help='Duration in seconds for monitoring')
    parser.add_argument('--analysis-file', help='Analysis file to load for visualization')
    
    args = parser.parse_args()
    
    print("NLM Visualization and Monitoring Tools")
    print("=" * 60)
    
    if args.mode == 'server':
        print(f"Starting real-time visualization server on {args.host}:{args.port}")
        
        # Initialize visualizer
        visualizer = RealTimeVisualizer()
        visualizer.monitoring_level = MonitoringLevel(args.level)
        
        # Start server
        if visualizer.start_server(args.host, args.port):
            print("Server started successfully")
            print("Access visualization at: http://{args.host}:{args.port}")
            
            # Keep server running
            try:
                while visualizer.running:
                    time.sleep(1)
            except KeyboardInterrupt:
                print("\nShutting down server...")
                visualizer.stop()
        else:
            print("Failed to start server")
            return 1
    
    elif args.mode == 'visualize':
        print(f"Generating static visualization to {args.output}")
        
        # Initialize visualizer
        visualizer = RealTimeVisualizer()
        
        if args.analysis_file:
            # Load analysis data
            with open(args.analysis_file, 'r') as f:
                analysis_data = json.load(f)
            
            # Create mock monitoring data for visualization
            from unittest.mock import Mock
            monitoring_data = Mock()
            monitoring_data.timestamp = time.time()
            monitoring_data.simulation_step = analysis_data.get('simulation_step', 0)
            monitoring_data.neurons = []
            monitoring_data.connections = []
            monitoring_data.statistics = analysis_data.get('statistics', {})
            monitoring_data.events = analysis_data.get('events', [])
            monitoring_data.memory_usage = analysis_data.get('memory_usage', {})
            monitoring_data.performance_metrics = analysis_data.get('performance_metrics', {})
            
            visualizer.current_data = monitoring_data
        
        # Generate visualization
        if visualizer.generate_static_visualization(args.output, args.format):
            print(f"Static visualization generated: {args.output}")
        else:
            print("Failed to generate static visualization")
            return 1
    
    elif args.mode == 'monitor':
        print(f"Starting performance monitoring for {args.duration} seconds")
        
        # Initialize monitor
        monitor = PerformanceMonitor()
        
        # Add alert handler for notifications
        def alert_handler(alert):
            print(f"ALERT: {alert['message']} ({alert['severity']})")
        
        monitor.add_alert_handler(alert_handler)
        
        # Start monitoring (simplified - would integrate with actual NLM)
        start_time = time.time()
        end_time = start_time + args.duration
        
        while time.time() < end_time:
            # Simulate performance metrics
            metrics = {
                'frame_rate': 60.0,
                'simulation_steps_per_second': 100.0
            }
            
            monitor.record_performance_metrics(metrics)
            time.sleep(1.0)  # Update every second
        
        # Generate performance report
        report = monitor.generate_performance_report()
        
        if args.output:
            with open(args.output, 'w') as f:
                json.dump(report, f, indent=2, default=str)
            print(f"Performance report saved to: {args.output}")
        
        print(f"Monitoring completed. Generated report with {len(report.get('anomalies', []))} anomalies.")
    
    elif args.mode == 'analyze':
        print("Starting analysis mode")
        if args.analysis_file:
            print(f"Loading analysis data from: {args.analysis_file}")
            
            with open(args.analysis_file, 'r') as f:
                analysis_data = json.load(f)
            
            print("Analysis data loaded successfully")
            # Would perform detailed analysis here
            print("Analysis would include:")
            print("  - Neural activity pattern analysis")
            print("  - Performance trend analysis")
            print("  - Anomaly detection")
            print("  - Optimization recommendations")
        else:
            print("No analysis file provided")
            return 1
    
    print("\nVisualization and monitoring tools completed successfully!")
    return 0

if __name__ == '__main__':
    exit(main())
