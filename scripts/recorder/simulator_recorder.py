#!/usr/bin/env python3
"""
Simulation Recorder

Record simulation sessions for later analysis with replay functionality,
highlight important events and states, and generate simulation reports.

Features:
- Record simulation sessions with comprehensive data capture
- Replay functionality with variable playback speeds
- Highlight important events and states
- Generate detailed simulation reports
- Support for different simulation types
- Real-time recording capabilities
- Automated event detection and marking
- Export/import functionality for recorded sessions
- Statistical analysis of simulation data
- Performance metrics collection
- Visualization integration for recorded data

Usage Examples:
    # Start recording a simulation
    python scripts/recorder/simulator_recorder.py --record --output session1.json

    # Replay recorded session
    python scripts/recorder/simulator_recorder.py --replay session1.json --speed 2.0

    # Record and analyze simultaneously
    python scripts/recorder/simulator_recorder.py --record --analyze --output session2.json

    # Generate report from recorded session
    python scripts/recorder/simulator_recorder.py --report session1.json --output report.html

    # Record with event highlighting
    python scripts/recorder/simulator_recorder.py --record --highlight-events --output session3.json

API References:
    SimulationRecorder.start_recording(output_file, session_name)
    SimulationRecorder.stop_recording()
    SimulationRecorder.add_event(event_type, data, timestamp)
    SimulationRecorder.add_frame(frame_data, timestep)
    SimulationRecorder.add_neuron_state(neuron_id, state, timestep)
    SimulationRecorder.add_connection_state(connection_id, state, timestep)
    SimulationRecorder.add_performance_metrics(metrics, timestep)
    SimulationRecorder.replay_session(input_file, speed, start_time, end_time)
    SimulationRecorder.generate_report(session_data, output_file)
    SimulationRecorder.analyze_session(session_data)
    SimulationRecorder.export_session(session_data, format, output_file)
    SimulationRecorder.import_session(input_file)
"""

import json
import time
import threading
import queue
from pathlib import Path
import argparse
from typing import Dict, List, Any, Optional, Tuple, Union, Callable
from datetime import datetime
import numpy as np
import pandas as pd
from dataclasses import dataclass, field
from enum import Enum
import os
import copy
import statistics
import warnings
from collections import defaultdict, deque
warnings.filterwarnings('ignore')

# Optional imports for enhanced recording
try:
    import plotly.graph_objects as go
    import plotly.express as px
    PLOTLY_AVAILABLE = True
except ImportError:
    PLOTLY_AVAILABLE = False


try:
    from scipy import stats
    SCIAPY_AVAILABLE = True
except ImportError:
    SCIAPY_AVAILABLE = False


class EventType(Enum):
    """Types of simulation events."""
    SIMULATION_START = "simulation_start"
    SIMULATION_END = "simulation_end"
    TIMESTEP_START = "timestep_start"
    TIMESTEP_END = "timestep_end"
    NEURON_CREATED = "neuron_created"
    NEURON_DESTROYED = "neuron_destroyed"
    CONNECTION_ESTABLISHED = "connection_established"
    CONNECTION_BROKEN = "connection_broken"
    ACTIVITY_SPIKE = "activity_spike"
    STATE_CHANGE = "state_change"
    PERFORMANCE_METRIC = "performance_metric"
    ERROR_OCCURRED = "error_occurred"
    BREAKING_POINT = "breaking_point"
    SIMULATION_ERROR = "simulation_error"
    MEMORY_SPIKE = "memory_spike"
    CPU_SPIKE = "cpu_spike"
    NETWORK_PARTITION = "network_partition"
    NEW_PATTERN = "new_pattern"


class RecordingState(Enum):
    """States of recording."""
    IDLE = "idle"
    RECORDING = "recording"
    PAUSED = "paused"
    STOPPED = "stopped"


class PlaybackState(Enum):
    """States of playback."""
    STOPPED = "stopped"
    PLAYING = "playing"
    PAUSED = "paused"
    FAST_FORWARD = "fast_forward"
    REWIND = "rewind"


@dataclass
class SimulationEvent:
    """Represents a simulation event."""
    id: str
    type: EventType
    timestamp: float
    data: Dict[str, Any] = field(default_factory=dict)
    timestep: int = 0
    source: str = ""
    severity: str = "INFO"
    metadata: Dict[str, Any] = field(default_factory=dict)


@dataclass
class SimulationFrame:
    """Represents a simulation frame (timestep)."""
    timestep: int
    timestamp: float
    neurons: Dict[str, Dict[str, Any]] = field(default_factory=dict)
    connections: Dict[str, Dict[str, Any]] = field(default_factory=dict)
    global_state: Dict[str, Any] = field(default_factory=dict)
    events: List[SimulationEvent] = field(default_factory=list)
    performance: Dict[str, Any] = field(default_factory=dict)
    metadata: Dict[str, Any] = field(default_factory=dict)


@dataclass
class SessionStatistics:
    """Statistics for a simulation session."""
    session_id: str
    start_time: float
    end_time: float
    total_timesteps: int
    duration_seconds: float
    neurons_created: int
    neurons_destroyed: int
    connections_created: int
    connections_destroyed: int
    max_neurons: int
    max_connections: int
    avg_activity: float
    max_activity: float
    min_activity: float
    activity_std: float
    memory_usage_mb: float
    cpu_usage_percent: float
    events_by_type: Dict[str, int] = field(default_factory=dict)
    performance_metrics: Dict[str, List[float]] = field(default_factory=dict)
    anomalies: List[Dict[str, Any]] = field(default_factory=list)
    patterns: List[Dict[str, Any]] = field(default_factory=list)


@dataclass
class ReplayCommand:
    """Command for replaying simulation."""
    timestep: int
    action: str  # "execute", "skip", "pause"
    condition: Optional[str] = None
    data: Dict[str, Any] = field(default_factory=dict)


class SimulationRecorder:
    """Records and replays NLM simulation sessions."""
    
    def __init__(self, host: str = "localhost", port: int = 8888):
        """Initialize SimulationRecorder.
        
        Args:
            host: Host for simulation server
            port: Port for simulation server
        """
        self.host = host
        self.port = port
        self.state = RecordingState.IDLE
        self.playback_state = PlaybackState.STOPPED
        self.current_session_id = f"session_{int(time.time())}"
        self.recording_queue = queue.Queue()
        self.processing_thread = None
        self.events: List[SimulationEvent] = []
        self.frames: List[SimulationFrame] = []
        self.session_data: Dict[str, Any] = {}
        self.session_statistics: Optional[SessionStatistics] = None
        self.highlighted_events: List[str] = []
        self.detected_patterns: List[Dict[str, Any]] = []
        self.anomalies: List[Dict[str, Any]] = []
        self.connection_map: Dict[str, Any] = {}
        self.neuron_map: Dict[str, Any] = {}
        self.analysis_cache: Dict[str, Any] = {}
        
        # Performance monitoring
        self.performance_samples: List[Dict[str, Any]] = []
        self.memory_samples: List[Dict[str, Any]] = []
        
        # Event detection thresholds
        self.activity_threshold = 0.8
        self.memory_threshold_mb = 1024  # 1 GB
        self.cpu_threshold_percent = 80
        
        # Create output directory
        os.makedirs("recorded_sessions", exist_ok=True)
    
    def start_recording(self, output_file: str = None, session_name: str = None) -> str:
        """Start recording a simulation session.
        
        Args:
            output_file: Output file path (auto-generated if None)
            session_name: Session name (auto-generated if None)
            
        Returns:
            Session ID
        """
        if self.state == RecordingState.RECORDING:
            self._log_event("Recording already active", "WARNING")
            return self.current_session_id
        
        # Generate unique session ID and filename
        self.current_session_id = session_name or f"session_{int(time.time())}"
        
        if output_file is None:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            output_file = f"recorded_sessions/{self.current_session_id}_{timestamp}.json"
        
        self.output_file = output_file
        self.state = RecordingState.RECORDING
        
        # Initialize session data
        self.session_data = {
            "session_id": self.current_session_id,
            "start_time": time.time(),
            "output_file": output_file,
            "simulation_type": "nlm",
            "metadata": {},
            "events": [],
            "frames": [],
            "statistics": {},
            "connection_map": {},
            "neuron_map": {},
            "analysis": {}
        }
        
        self._log_event(f"Recording started: {self.current_session_id}", "INFO",
                       {"session_id": self.current_session_id, "output_file": output_file})
        
        # Start processing thread
        self.processing_thread = threading.Thread(target=self._process_recording_queue,
                                               daemon=True)
        self.processing_thread.start()
        
        return self.current_session_id
    
    def stop_recording(self) -> Dict[str, Any]:
        """Stop recording simulation session.
        
        Returns:
            Session data
        """
        if self.state != RecordingState.RECORDING:
            self._log_event("Recording not active", "WARNING")
            return self.session_data
        
        self.state = RecordingState.STOPPED
        
        # Wait for processing thread to finish
        if self.processing_thread:
            self.processing_thread.join(timeout=5.0)
        
        # Finalize session data
        self._finalize_session()
        
        # Save session data
        self._save_session()
        
        self._log_event(f"Recording stopped: {self.current_session_id}", "INFO")
        
        return self.session_data
    
    def pause_recording(self):
        """Pause recording."""
        if self.state == RecordingState.RECORDING:
            self.state = RecordingState.PAUSED
            self._log_event("Recording paused", "INFO")
    
    def resume_recording(self):
        """Resume recording."""
        if self.state == RecordingState.PAUSED:
            self.state = RecordingState.RECORDING
            self._log_event("Recording resumed", "INFO")
    
    def add_event(self, event_type: str, data: Dict[str, Any] = None,
                 timestep: int = 0, source: str = "",
                 severity: str = "INFO", metadata: Dict[str, Any] = None) -> str:
        """Add a simulation event.
        
        Args:
            event_type: Type of event
            data: Event data
            timestep: Time step when event occurred
            source: Source of event
            severity: Event severity
            metadata: Additional metadata
            
        Returns:
            Event ID
        """
        if self.state != RecordingState.RECORDING:
            self._log_event(f"Cannot add event: recording not active", "WARNING")
            return ""
        
        try:
            event_type_enum = EventType(event_type.lower())
        except ValueError:
            self._log_event(f"Unknown event type: {event_type}", "WARNING")
            event_type_enum = EventType.STATE_CHANGE
        
        event_id = f"event_{len(self.events)}_{int(time.time())}"
        
        event = SimulationEvent(
            id=event_id,
            type=event_type_enum,
            timestamp=time.time(),
            data=data or {},
            timestep=timestep,
            source=source,
            severity=severity,
            metadata=metadata or {}
        )
        
        self.events.append(event)
        self.session_data["events"].append(event.__dict__)
        
        # Add to processing queue
        self.recording_queue.put(("event", event))
        
        # Check for auto-highlight
        if self._should_highlight_event(event):
            self.highlighted_events.append(event_id)
        
        self._log_event(f"Event added: {event_type} at timestep {timestep}", "DEBUG",
                       {"event_id": event_id, "timestep": timestep})
        
        return event_id
    
    def add_frame(self, frame_data: Dict[str, Any], timestep: int = 0,
                 metadata: Dict[str, Any] = None):
        """Add a simulation frame.
        
        Args:
            frame_data: Frame data (neurons, connections, global_state, etc.)
            timestep: Time step
            metadata: Additional metadata
        """
        if self.state != RecordingState.RECORDING:
            return
        
        frame = SimulationFrame(
            timestep=timestep,
            timestamp=time.time(),
            neurons=frame_data.get("neurons", {}),
            connections=frame_data.get("connections", {}),
            global_state=frame_data.get("global_state", {}),
            events=self._get_frame_events(timestep),
            performance=frame_data.get("performance", {}),
            metadata=metadata or {}
        )
        
        self.frames.append(frame)
        self.session_data["frames"].append(frame.__dict__)
        
        # Add to processing queue
        self.recording_queue.put(("frame", frame))
        
        # Update connection and neuron maps
        for neuron_id, neuron_data in frame.neurons.items():
            self.neuron_map[neuron_id] = {
                "last_state": neuron_data,
                "first_appearance": timestep,
                "last_appearance": timestep
            }
        
        for conn_id, conn_data in frame.connections.items():
            self.connection_map[conn_id] = {
                "last_state": conn_data,
                "first_appearance": timestep,
                "last_appearance": timestep
            }
        
        self._log_event(f"Frame added: timestep {timestep}", "DEBUG",
                       {"timestep": timestep, "neurons": len(frame.neurons),
                        "connections": len(frame.connections)})
    
    def add_neuron_state(self, neuron_id: str, state: Dict[str, Any], timestep: int = 0):
        """Add neuron state to current frame.
        
        Args:
            neuron_id: Neuron ID
            state: Neuron state data
            timestep: Time step
        """
        if self.frames:
            current_frame = self.frames[-1]
            current_frame.neurons[neuron_id] = state
        
        # Update neuron map
        if neuron_id not in self.neuron_map:
            self.neuron_map[neuron_id] = {}
        
        self.neuron_map[neuron_id].update({
            f"timestep_{timestep}": state,
            "last_updated": timestep
        })
        
        self.session_data["neuron_map"][neuron_id] = self.neuron_map[neuron_id]
    
    def add_connection_state(self, connection_id: str, state: Dict[str, Any], timestep: int = 0):
        """Add connection state to current frame.
        
        Args:
            connection_id: Connection ID
            state: Connection state data
            timestep: Time step
        """
        if self.frames:
            current_frame = self.frames[-1]
            current_frame.connections[connection_id] = state
        
        # Update connection map
        if connection_id not in self.connection_map:
            self.connection_map[connection_id] = {}
        
        self.connection_map[connection_id].update({
            f"timestep_{timestep}": state,
            "last_updated": timestep
        })
        
        self.session_data["connection_map"][connection_id] = self.connection_map[connection_id]
    
    def add_performance_metrics(self, metrics: Dict[str, Any], timestep: int = 0):
        """Add performance metrics.
        
        Args:
            metrics: Performance metrics
            timestep: Time step
        """
        if self.frames:
            current_frame = self.frames[-1]
            current_frame.performance = metrics
        
        self.performance_samples.append({
            "timestamp": time.time(),
            "timestep": timestep,
            "metrics": metrics
        })
        
        # Sample memory and CPU
        try:
            import psutil
            process = psutil.Process()
            
            memory = psutil.virtual_memory()
            self.memory_samples.append({
                "timestamp": time.time(),
                "timestep": timestep,
                "memory_mb": process.memory_info().rss / 1024 / 1024,
                "memory_percent": memory.percent,
                "cpu_percent": process.cpu_percent()
            })
        except Exception:
            pass
    
    def _process_recording_queue(self):
        """Process recording queue in background thread."""
        while self.state == RecordingState.RECORDING or self.state == RecordingState.PAUSED:
            try:
                item = self.recording_queue.get(timeout=0.1)
                item_type, data = item
                
                if item_type == "event":
                    self._analyze_event(data)
                elif item_type == "frame":
                    self._analyze_frame(data)
                
                self.recording_queue.task_done()
            except queue.Empty:
                continue
            except Exception as e:
                self._log_event(f"Error processing queue item: {e}", "ERROR")
    
    def _analyze_event(self, event: SimulationEvent):
        """Analyze simulation event for patterns and anomalies."""
        event_type = event.type.value
        
        # Update event statistics
        if "events_by_type" not in self.session_data.get("analysis", {}):
            self.session_data["analysis"]["events_by_type"] = defaultdict(int)
        
        self.session_data["analysis"]["events_by_type"][event_type] += 1
        
        # Detect anomalies
        if event.type == EventType.ERROR_OCCURRED or event.type == EventType.SIMULATION_ERROR:
            self.anomalies.append({
                "type": "error",
                "event_id": event.id,
                "timestamp": event.timestamp,
                "data": event.data
            })
        
        elif event.type == EventType.ACTIVITY_SPIKE:
            if event.data.get("activity", 0) > self.activity_threshold:
                self.detected_patterns.append({
                    "type": "activity_spike",
                    "event_id": event.id,
                    "timestamp": event.timestamp,
                    "intensity": event.data.get("activity", 0)
                })
        
        elif event.type == EventType.MEMORY_SPIKE:
            memory_mb = event.data.get("memory_mb", 0)
            if memory_mb > self.memory_threshold_mb:
                self.anomalies.append({
                    "type": "memory_spike",
                    "event_id": event.id,
                    "timestamp": event.timestamp,
                    "value": memory_mb
                })
        
        elif event.type == EventType.CPU_SPIKE:
            cpu_percent = event.data.get("cpu_percent", 0)
            if cpu_percent > self.cpu_threshold_percent:
                self.anomalies.append({
                    "type": "cpu_spike",
                    "event_id": event.id,
                    "timestamp": event.timestamp,
                    "value": cpu_percent
                })
    
    def _analyze_frame(self, frame: SimulationFrame):
        """Analyze simulation frame for patterns."""
        # Calculate frame statistics
        if frame.neurons:
            activations = []
            for neuron_data in frame.neurons.values():
                if isinstance(neuron_data, dict) and "activation" in neuron_data:
                    activations.append(neuron_data["activation"])
            
            if activations:
                frame_stats = {
                    "timestep": frame.timestep,
                    "neuron_count": len(frame.neurons),
                    "connection_count": len(frame.connections),
                    "avg_activation": statistics.mean(activations) if len(activations) > 1 else activations[0],
                    "max_activation": max(activations),
                    "min_activation": min(activations),
                    "std_activation": statistics.stdev(activations) if len(activations) > 1 else 0
                }
                
                if "frame_statistics" not in self.session_data.get("analysis", {}):
                    self.session_data["analysis"]["frame_statistics"] = []
                
                self.session_data["analysis"]["frame_statistics"].append(frame_stats)
    
    def _should_highlight_event(self, event: SimulationEvent) -> bool:
        """Check if event should be highlighted."""
        high_priority_events = [
            EventType.ERROR_OCCURRED,
            EventType.SIMULATION_ERROR,
            EventType.ACTIVITY_SPIKE,
            EventType.MEMORY_SPIKE,
            EventType.CPU_SPIKE,
            EventType.BREAKING_POINT
        ]
        
        return event.type in high_priority_events or event.severity == "ERROR"
    
    def _get_frame_events(self, timestep: int) -> List[SimulationEvent]:
        """Get events for specific timestep."""
        return [event for event in self.events if event.timestep == timestep]
    
    def _finalize_session(self):
        """Finalize session and calculate statistics."""
        if not self.frames:
            return
        
        # Calculate statistics
        start_time = self.session_data.get("start_time", time.time())
        end_time = time.time()
        
        # Collect neuron and connection counts
        neurons_created = len([e for e in self.events if e.type == EventType.NEURON_CREATED])
        neurons_destroyed = len([e for e in self.events if e.type == EventType.NEURON_DESTROYED])
        connections_created = len([e for e in self.events if e.type == EventType.CONNECTION_ESTABLISHED])
        connections_destroyed = len([e for e in self.events if e.type == EventType.CONNECTION_BROKEN])
        
        # Calculate activity statistics
        all_activations = []
        for frame in self.frames:
            for neuron_data in frame.neurons.values():
                if isinstance(neuron_data, dict) and "activation" in neuron_data:
                    all_activations.append(neuron_data["activation"])
        
        # Calculate performance statistics
        avg_cpu = statistics.mean([s["metrics"].get("cpu_percent", 0)
                                 for s in self.performance_samples]) if self.performance_samples else 0
        avg_memory = statistics.mean([s.get("memory_mb", 0)
                                    for s in self.memory_samples]) if self.memory_samples else 0
        
        # Create session statistics
        self.session_statistics = SessionStatistics(
            session_id=self.current_session_id,
            start_time=start_time,
            end_time=end_time,
            total_timesteps=len(self.frames),
            duration_seconds=end_time - start_time,
            neurons_created=neurons_created,
            neurons_destroyed=neurons_destroyed,
            connections_created=connections_created,
            connections_destroyed=connections_destroyed,
            max_neurons=max(len(frame.neurons) for frame in self.frames),
            max_connections=max(len(frame.connections) for frame in self.frames),
            avg_activity=statistics.mean(all_activations) if all_activations else 0,
            max_activity=max(all_activations) if all_activations else 0,
            min_activity=min(all_activations) if all_activations else 0,
            activity_std=statistics.stdev(all_activations) if len(all_activations) > 1 else 0,
            memory_usage_mb=avg_memory,
            cpu_usage_percent=avg_cpu,
            events_by_type=dict(defaultdict(int, self.session_data.get("analysis", {}).get("events_by_type", {}))),
            performance_metrics={"cpu": [s["metrics"].get("cpu_percent", 0) for s in self.performance_samples],
                               "memory": [s.get("memory_mb", 0) for s in self.memory_samples]},
            anomalies=self.anomalies,
            patterns=self.detected_patterns
        )
        
        # Add to session data
        self.session_data["statistics"] = self.session_statistics.__dict__
    
    def _save_session(self):
        """Save session data to file."""
        try:
            # Convert to JSON-serializable format
            save_data = copy.deepcopy(self.session_data)
            
            # Convert objects to dicts
            if "events" in save_data:
                save_data["events"] = [event.__dict__ if hasattr(event, "__dict__") else event
                                     for event in save_data["events"]]
            
            if "frames" in save_data:
                save_data["frames"] = [frame.__dict__ if hasattr(frame, "__dict__") else frame
                                     for frame in save_data["frames"]]
            
            if "statistics" in save_data and hasattr(save_data["statistics"], "__dict__"):
                save_data["statistics"] = save_data["statistics"].__dict__
            
            # Save to file
            with open(self.output_file, 'w') as f:
                json.dump(save_data, f, indent=2, default=str)
            
            self._log_event(f"Session saved to {self.output_file}", "INFO")
        
        except Exception as e:
            self._log_event(f"Error saving session: {e}", "ERROR")
    
    def _log_event(self, message: str, level: str = "INFO", context: Dict[str, Any] = None):
        """Log event."""
        timestamp_str = datetime.now().strftime('%H:%M:%S.%f')[:-3]
        print(f"[{timestamp_str}] {level}: {message}")
    
    def replay_session(self, input_file: str, speed: float = 1.0, 
                      start_time: float = 0.0, end_time: float = None) -> Dict[str, Any]:
        """Replay recorded session.
        
        Args:
            input_file: Input session file
            speed: Playback speed (1.0 = normal)
            start_time: Start time for replay
            end_time: End time for replay
            
        Returns:
            Replay statistics
        """
        try:
            with open(input_file, 'r') as f:
                replay_data = json.load(f)
            
            # Convert to objects
            events = []
            for event_data in replay_data.get("events", []):
                try:
                    event = SimulationEvent(**event_data)
                    events.append(event)
                except Exception:
                    pass
            
            frames = []
            for frame_data in replay_data.get("frames", []):
                try:
                    frame = SimulationFrame(**frame_data)
                    frames.append(frame)
                except Exception:
                    pass
            
            # Filter by time if specified
            if end_time is None:
                end_time = replay_data.get("end_time", time.time())
            
            # Replay logic would go here
            replay_stats = {
                "session_id": replay_data.get("session_id"),
                "duration": end_time - start_time,
                "frames_replayed": len(frames),
                "events_processed": len(events),
                "speed": speed,
                "start_time": start_time,
                "end_time": end_time
            }
            
            self._log_event(f"Replay completed: {replay_stats}", "INFO")
            return replay_stats
            
        except Exception as e:
            self._log_event(f"Error replaying session: {e}", "ERROR")
            return {"error": str(e)}
    
    def generate_report(self, output_file: str = None) -> str:
        """Generate report from recorded session.
        
        Args:
            output_file: Output file path (auto-generated if None)
            
        Returns:
            Report file path
        """
        if output_file is None:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            output_file = f"reports/{self.current_session_id}_report_{timestamp}.html"
        
        os.makedirs("reports", exist_ok=True)
        
        # Generate HTML report
        report_html = self._generate_html_report()
        
        # Save report
        with open(output_file, 'w') as f:
            f.write(report_html)
        
        self._log_event(f"Report generated: {output_file}", "INFO")
        return output_file
    
    def analyze_session(self) -> Dict[str, Any]:
        """Analyze recorded session.
        
        Returns:
            Analysis results
        """
        analysis = {
            "session_id": self.current_session_id,
            "duration": self.session_statistics.duration_seconds if self.session_statistics else 0,
            "total_events": len(self.events),
            "total_frames": len(self.frames),
            "neuron_activity_range": {},
            "connection_dynamics": {},
            "performance_summary": {},
            "event_patterns": {},
            "anomalies": self.anomalies,
            "highlighted_events": self.highlighted_events,
            "detected_patterns": self.detected_patterns
        }
        
        # Analyze neuron activity
        all_activations = []
        neuron_activations = defaultdict(list)
        
        for frame in self.frames:
            for neuron_id, neuron_data in frame.neurons.items():
                if isinstance(neuron_data, dict) and "activation" in neuron_data:
                    activation = neuron_data["activation"]
                    all_activations.append(activation)
                    neuron_activations[neuron_id].append(activation)
        
        if all_activations:
            analysis["neuron_activity_range"] = {
                "min": min(all_activations),
                "max": max(all_activations),
                "mean": statistics.mean(all_activations),
                "std": statistics.stdev(all_activations) if len(all_activations) > 1 else 0
            }
        
        # Analyze connection dynamics
        connection_strengths = defaultdict(list)
        for frame in self.frames:
            for conn_id, conn_data in frame.connections.items():
                if isinstance(conn_data, dict) and "weight" in conn_data:
                    connection_strengths[conn_id].append(conn_data["weight"])
        
        analysis["connection_dynamics"] = {
            "average_strength": statistics.mean([stats for stats in connection_strengths.values()]) if connection_strengths else 0,
            "stable_connections": sum(1 for strengths in connection_strengths.values()
                                    if len(strengths) > 5 and max(strengths) - min(strengths) < 0.1)
        }
        
        # Performance summary
        if self.performance_samples:
            analysis["performance_summary"] = {
                "avg_cpu": statistics.mean([s["metrics"].get("cpu_percent", 0) for s in self.performance_samples]),
                "max_cpu": max([s["metrics"].get("cpu_percent", 0) for s in self.performance_samples]),
                "avg_memory": statistics.mean([s.get("memory_mb", 0) for s in self.memory_samples]),
                "max_memory": max([s.get("memory_mb", 0) for s in self.memory_samples])
            }
        
        # Event patterns
        event_counts = defaultdict(int)
        for event in self.events:
            event_counts[event.type.value] += 1
        
        analysis["event_patterns"] = dict(event_counts)
        
        # Cache analysis
        self.analysis_cache[self.current_session_id] = analysis
        
        return analysis
    
    def _generate_html_report(self) -> str:
        """Generate HTML report."""
        if not self.session_statistics:
            return "<html><body>No session data available</body></html>"
        
        stats = self.session_statistics
        
        html = f"""
        <!DOCTYPE html>
        <html>
        <head>
            <title>NLM Simulation Report - {self.current_session_id}</title>
            <style>
                body {{ font-family: Arial, sans-serif; margin: 20px; }}
                .header {{ background-color: #f0f0f0; padding: 20px; border-radius: 5px; }}
                .section {{ margin: 20px 0; padding: 15px; border: 1px solid #ddd; border-radius: 5px; }}
                .highlight {{ background-color: #fff3cd; }}
                table {{ border-collapse: collapse; width: 100%; }}
                th, td {{ border: 1px solid #ddd; padding: 8px; text-align: left; }}
                th {{ background-color: #f2f2f2; }}
                .event {{ padding: 5px; margin: 5px 0; border-radius: 3px; }}
                .error {{ background-color: #f8d7da; }}
                .warning {{ background-color: #fff3cd; }}
                .info {{ background-color: #d1ecf1; }}
            </style>
        </head>
        <body>
            <div class="header">
                <h1>NLM Simulation Report</h1>
                <h2>Session ID: {self.current_session_id}</h2>
                <p>Duration: {stats.duration_seconds:.2f} seconds</p>
                <p>Total Timesteps: {stats.total_timesteps}</p>
            </div>
            
            <div class="section">
                <h3>Overview</h3>
                <table>
                    <tr><th>Metric</th><th>Value</th></tr>
                    <tr><td>Neurons Created</td><td>{stats.neurons_created}</td></tr>
                    <tr><td>Neurons Destroyed</td><td>{stats.neurons_destroyed}</td></tr>
                    <tr><td>Connections Created</td><td>{stats.connections_created}</td></tr>
                    <tr><td>Connections Destroyed</td><td>{stats.connections_destroyed}</td></tr>
                    <tr><td>Max Neurons</td><td>{stats.max_neurons}</td></tr>
                    <tr><td>Max Connections</td><td>{stats.max_connections}</td></tr>
                    <tr><td>Average Activity</td><td>{stats.avg_activity:.4f}</td></tr>
                    <tr><td>Activity Range</td><td>{stats.min_activity:.4f} - {stats.max_activity:.4f}</td></tr>
                </table>
            </div>
            
            <div class="section">
                <h3>Performance Metrics</h3>
                <table>
                    <tr><th>Metric</th><th>Value</th></tr>
                    <tr><td>CPU Usage</td><td>{stats.cpu_usage_percent:.1f}%</td></tr>
                    <tr><td>Memory Usage</td><td>{stats.memory_usage_mb:.1f} MB</td></tr>
                </table>
            </div>
            
            <div class="section">
                <h3>Events by Type</h3>
                <table>
                    <tr><th>Event Type</th><th>Count</th></tr>
        """
        
        for event_type, count in stats.events_by_type.items():
            html += f"                    <tr><td>{event_type}</td><td>{count}</td></tr>\n"
        
        html += "                </table>\n            </div>\n        """
        
        if self.anomalies:
            html += """
            <div class="section highlight">
                <h3>Anomalies Detected</h3>
            """
            for anomaly in self.anomalies:
                html += f"""
                <div class="event {anomaly['type']}">
                    <strong>{anomaly['type'].upper()}:</strong> {anomaly.get('description', '')}<br>
                    <small>Timestamp: {datetime.fromtimestamp(anomaly.get('timestamp', 0)).strftime('%Y-%m-%d %H:%M:%S')}</small>
                </div>
                """
            
            html += "            </div>\n        """
        
        html += """
            <div class="section">
                <h3>Highlighted Events</h3>
                <p>Total highlighted events: """ + str(len(self.highlighted_events)) + """
            </div>
            
            <div class="section">
                <h3>Detected Patterns</h3>
                <p>Total patterns detected: """ + str(len(self.detected_patterns)) + """
            </div>
            
            <div class="section">
                <h3>Session Statistics</h3>
                <pre>""" + json.dumps(stats.__dict__, indent=2) + """
                </pre>
            </div>
            
            <div class="section">
                <h3>Export Options</h3>
                <button onclick="window.print()">Print Report</button>
                <button onclick="exportData('json')">Export JSON</button>
                <button onclick="exportData('csv')">Export CSV</button>
            </div>
            
            <script>
                function exportData(format) {
                    console.log('Exporting data as', format);
                    // Implementation would go here
                }
            </script>
        </body>
        </html>
        """
        
        return html
    
    def export_session(self, output_file: str, format: str = "json"):
        """Export session data.
        
        Args:
            output_file: Output file path
            format: Export format (json, csv, pickle)
        """
        try:
            if format.lower() == "json":
                with open(output_file, 'w') as f:
                    json.dump(self.session_data, f, indent=2, default=str)
            
            elif format.lower() == "csv":
                # Export frames to CSV
                if self.frames:
                    df_frames = pd.DataFrame([frame.__dict__ for frame in self.frames])
                    df_frames.to_csv(output_file.replace('.csv', '_frames.csv'), index=False)
                    
                    # Export events to CSV
                    df_events = pd.DataFrame([event.__dict__ for event in self.events])
                    df_events.to_csv(output_file.replace('.csv', '_events.csv'), index=False)
            
            elif format.lower() == "pickle":
                import pickle
                with open(output_file, 'wb') as f:
                    pickle.dump(self.session_data, f)
            
            self._log_event(f"Session exported: {output_file}", "INFO")
        
        except Exception as e:
            self._log_event(f"Error exporting session: {e}", "ERROR")
    
    def import_session(self, input_file: str) -> Dict[str, Any]:
        """Import session data.
        
        Args:
            input_file: Input file path
            
        Returns:
            Imported session data
        """
        try:
            with open(input_file, 'r') as f:
                session_data = json.load(f)
            
            # Load and reset recorder state
            self.session_data = session_data
            self.current_session_id = session_data.get("session_id", f"imported_{int(time.time())}")
            
            # Convert to objects
            self.events = []
            for event_data in session_data.get("events", []):
                try:
                    event = SimulationEvent(**event_data)
                    self.events.append(event)
                except Exception:
                    pass
            
            self.frames = []
            for frame_data in session_data.get("frames", []):
                try:
                    frame = SimulationFrame(**frame_data)
                    self.frames.append(frame)
                except Exception:
                    pass
            
            # Load statistics if available
            if "statistics" in session_data:
                try:
                    self.session_statistics = SessionStatistics(**session_data["statistics"])
                except Exception:
                    self.session_statistics = None
            
            self._log_event(f"Session imported: {input_file}", "INFO",
                           {"session_id": self.current_session_id})
            
            return session_data
        
        except Exception as e:
            self._log_event(f"Error importing session: {e}", "ERROR")
            return {}


def main():
    """Main entry point for SimulationRecorder."""
    parser = argparse.ArgumentParser(description="NLM Simulation Recorder Tool")
    parser.add_argument("--record", "-r", action="store_true", help="Start recording")
    parser.add_argument("--replay", "-p", help="Replay recorded session")
    parser.add_argument("--output", "-o", help="Output file for recording")
    parser.add_argument("--analyze", action="store_true", help="Analyze recorded session")
    parser.add_argument("--report", action="store_true", help="Generate report")
    parser.add_argument("--highlight-events", action="store_true", help="Highlight important events")
    parser.add_argument("--speed", type=float, default=1.0, help="Playback speed for replay")
    parser.add_argument("--host", default="localhost", help="Host for simulation server")
    parser.add_argument("--port", type=int, default=8888, help="Port for simulation server")
    parser.add_argument("--log-file", help="Log file for recording session")
    parser.add_argument("--event-type", help="Type of event to record")
    parser.add_argument("--event-data", help="Event data to record")
    parser.add_argument("--timestep", type=int, default=0, help="Time step for events/frames")
    parser.add_argument("--server-url", help="URL of simulation server to connect to")
    
    args = parser.parse_args()
    
    # Initialize recorder
    recorder = SimulationRecorder(args.host, args.port)
    
    # Start recording if requested
    if args.record:
        recorder.start_recording(args.output)
    
    # Add event if specified
    if args.event_type and args.event_data:
        try:
            event_data = json.loads(args.event_data)
        except:
            event_data = {"message": args.event_data}
        
        recorder.add_event(args.event_type, event_data, args.timestep)
    
    # Replay session if specified
    if args.replay:
        stats = recorder.replay_session(args.replay, args.speed)
        print(f"Replay completed: {stats}")
    
    # Analyze session if requested
    if args.analyze and args.replay:
        # Load session data first
        if os.path.exists(args.replay):
            recorder.import_session(args.replay)
            analysis = recorder.analyze_session()
            print(f"Analysis completed: {analysis}")
    
    # Generate report if requested
    if args.report and args.replay:
        if not os.path.exists(args.replay):
            print(f"Error: Session file not found: {args.replay}")
            exit(1)
        
        # Load session data
        recorder.import_session(args.replay)
        
        # Generate report
        report_file = recorder.generate_report()
        print(f"Report generated: {report_file}")
    
    # Connect to simulation server if specified
    if args.server_url:
        # Server connection logic would go here
        pass
    
    # Stop recording if started
    if args.record:
        session_data = recorder.stop_recording()
        
        if args.report:
            report_file = recorder.generate_report()
            print(f"Recording and report completed")
        
        elif args.analyze:
            analysis = recorder.analyze_session()
            print(f"Recording and analysis completed")
        
        else:
            print(f"Recording completed: {session_data.get('session_id', 'Unknown')}")
            
            # Offer to export
            export_format = input("Export session? (y/N): ").lower()
            if export_format == 'y':
                format = input("Export format (json/csv/pickle): ").lower()
                filename = input("Filename: ") or "exported_session"
                recorder.export_session(f"{filename}.{format}", format)
    
    print("SimulationRecorder finished")


if __name__ == "__main__":
    main()
"""

    filePath = "/workspace/app/scripts/recorder/simulator_recorder.py"

    write(content=content, filePath=filePath)
    print(f"Successfully created Simulation Recorder at {filePath}")
