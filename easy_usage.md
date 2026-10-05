# NLM (熙然) - Neural Learning Machine
# Profiling and Debugging Tools

## Overview

This module provides comprehensive profiling and debugging capabilities for NLM simulations. It includes:

- Performance profiling with timing measurements
- Memory usage tracking
- Event and activity monitoring
- System health checks
- Integration validation
- Advanced debugging utilities

---

## Performance Profiling

### Performance Profiler Class

```python
import pynlm
import time
import psutil
import threading
from typing import Dict, List, Any, Callable
from dataclasses import dataclass, field
from enum import Enum

class MeasurementType(Enum):
    CPU_TIME = "cpu_time"
    WALL_TIME = "wall_time"
    MEMORY_USAGE = "memory_usage"
    EVENT_COUNT = "event_count"
    SYNAPSE_UPDATES = "synapse_updates"
    NEURON_FIRING = "neuron_firing"
    MEMORY_ACTIVITY = "memory_activity"

@dataclass
class Measurement:
    name: str
    type: MeasurementType
    value: float
    timestamp: float
    step: int
    metadata: Dict[str, Any] = field(default_factory=dict)

@dataclass
class PerformanceMetrics:
    total_steps: int = 0
    total_time: float = 0.0
    cpu_time: float = 0.0
    memory_usage: float = 0.0
    event_counts: Dict[str, int] = field(default_factory=dict)
    synapse_updates: int = 0
    neuron_firings: int = 0
    start_time: float = 0.0
    end_time: float = 0.0

class PerformanceProfiler:
    """Comprehensive performance profiler for NLM simulations"""
    
    def __init__(self, brain: pynlm.Brain):
        self.brain = brain
        self.measurements: List[Measurement] = []
        self.metrics = PerformanceMetrics()
        self.sampling_rate = 1.0  # Hz
        self.is_profiling = False
        self.profiling_thread: threading.Thread = None
        self.measurement_callbacks: List[Callable[[Measurement], None]] = []
    
    def start_profiling(self):
        """Start performance profiling"""
        self.is_profiling = True
        self.metrics.start_time = time.time()
        self.measurements.clear()
        
        # Start monitoring thread
        self.profiling_thread = threading.Thread(target=self._profiling_loop)
        self.profiling_thread.daemon = True
        self.profiling_thread.start()
        
        print("Performance profiling started")
    
    def stop_profiling(self):
        """Stop performance profiling"""
        self.is_profiling = False
        
        if self.profiling_thread:
            self.profiling_thread.join(timeout=1.0)
            self.profiling_thread = None
        
        self.metrics.end_time = time.time()
        self.metrics.total_time = self.metrics.end_time - self.metrics.start_time
        
        print(f"Performance profiling stopped. Duration: {self.metrics.total_time:.2f}s")
    
    def add_measurement_callback(self, callback: Callable[[Measurement], None]):
        """Add callback for measurement notifications"""
        self.measurement_callbacks.append(callback)
    
    def set_sampling_rate(self, rate_hz: float):
        """Set sampling rate for measurements (Hz)"""
        self.sampling_rate = rate_hz
    
    def _profiling_loop(self):
        """Background profiling loop"""
        import numpy as np
        
        last_sample_time = time.time()
        step_counter = 0
        
        while self.is_profiling:
            current_time = time.time()
            
            # Sample at specified rate
            if current_time - last_sample_time >= 1.0 / self.sampling_rate:
                self._take_measurement(current_time, step_counter)
                last_sample_time = current_time
                step_counter += 1
            
            time.sleep(0.001)  # Small sleep to prevent busy waiting
    
    def _take_measurement(self, timestamp: float, step: int):
        """Take a single measurement"""
        measurements = []
        
        # CPU time measurement
        process = psutil.Process()
        cpu_time = process.cpu_times().user + process.cpu_times().system
        measurements.append(Measurement(
            name="cpu_time",
            type=MeasurementType.CPU_TIME,
            value=cpu_time,
            timestamp=timestamp,
            step=step
        ))
        
        # Memory usage measurement
        memory_info = process.memory_info()
        memory_mb = memory_info.rss / (1024 * 1024)
        measurements.append(Measurement(
            name="memory_usage",
            type=MeasurementType.MEMORY_USAGE,
            value=memory_mb,
            timestamp=timestamp,
            step=step
        ))
        
        # Neural activity measurements
        measurements.append(Measurement(
            name="neuron_firing_rate",
            type=MeasurementType.NEURON_FIRING,
            value=self.brain.getAverageFiringRate(),
            timestamp=timestamp,
            step=step
        ))
        
        measurements.append(Measurement(
            name="total_spike_count",
            type=MeasurementType.NEURON_FIRING,
            value=self.brain.getTotalSpikeCount(),
            timestamp=timestamp,
            step=step
        ))
        
        # System metrics
        measurements.append(Measurement(
            name="simulation_step",
            type=MeasurementType.WALL_TIME,
            value=float(step),
            timestamp=timestamp,
            step=step
        ))
        
        # Brain statistics
        measurements.append(Measurement(
            name="e_i_ratio",
            type=MeasurementType.EVENT_COUNT,
            value=self.brain.getExcitationInhibitionRatio(),
            timestamp=timestamp,
            step=step
        ))
        
        # Memory system activity
        if self.brain.getWorkingMemory():
            measurements.append(Measurement(
                name="working_memory_traces",
                type=MeasurementType.MEMORY_ACTIVITY,
                value=float(self.brain.getWorkingMemory().getActiveTraces()),
                timestamp=timestamp,
                step=step
            ))
        
        if self.brain.getEpisodicMemory():
            measurements.append(Measurement(
                name="episodic_episodes",
                type=MeasurementType.MEMORY_ACTIVITY,
                value=float(self.brain.getEpisodicMemory().getEpisodeCount()),
                timestamp=timestamp,
                step=step
            ))
        
        # Add all measurements
        for measurement in measurements:
            self.measurements.append(measurement)
            
            # Notify callbacks
            for callback in self.measurement_callbacks:
                try:
                    callback(measurement)
                except Exception as e:
                    print(f"Measurement callback error: {e}")
        
        # Update metrics
        self.metrics.total_steps = step
        self.metrics.cpu_time = cpu_time
        self.metrics.memory_usage = memory_mb
        self.metrics.neuron_firings = self.brain.getTotalSpikeCount()
    
    def get_measurements_by_type(self, measurement_type: MeasurementType) -> List[Measurement]:
        """Get all measurements of a specific type"""
        return [m for m in self.measurements if m.type == measurement_type]
    
    def get_measurements_by_name(self, name: str) -> List[Measurement]:
        """Get all measurements with a specific name"""
        return [m for m in self.measurements if m.name == name]
    
    def get_measurements_for_step_range(self, start_step: int, end_step: int) -> List[Measurement]:
        """Get measurements within a step range"""
        return [m for m in self.measurements 
                if start_step <= m.step <= end_step]
    
    def get_performance_summary(self) -> Dict[str, Any]:
        """Get performance summary"""
        if not self.measurements:
            return {}
        
        cpu_measurements = self.get_measurements_by_type(MeasurementType.CPU_TIME)
        memory_measurements = self.get_measurements_by_type(MeasurementType.MEMORY_USAGE)
        firing_measurements = self.get_measurements_by_type(MeasurementType.NEURON_FIRING)
        
        summary = {
            "total_duration": self.metrics.total_time,
            "total_steps": self.metrics.total_steps,
            "average_cpu_time": np.mean([m.value for m in cpu_measurements]) if cpu_measurements else 0,
            "max_memory_usage": np.max([m.value for m in memory_measurements]) if memory_measurements else 0,
            "min_memory_usage": np.min([m.value for m in memory_measurements]) if memory_measurements else 0,
            "average_firing_rate": np.mean([m.value for m in firing_measurements if m.name == "neuron_firing_rate"]) if firing_measurements else 0,
            "total_spike_count": self.metrics.neuron_firings,
            "measurement_count": len(self.measurements)
        }
        
        return summary
    
    def export_measurements(self, filepath: str):
        """Export measurements to CSV file"""
        import csv
        
        with open(filepath, 'w', newline='') as csvfile:
            fieldnames = ['timestamp', 'step', 'name', 'type', 'value', 'metadata']
            writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
            writer.writeheader()
            
            for measurement in self.measurements:
                writer.writerow({
                    'timestamp': measurement.timestamp,
                    'step': measurement.step,
                    'name': measurement.name,
                    'type': measurement.type.value,
                    'value': measurement.value,
                    'metadata': str(measurement.metadata)
                })
        
        print(f"Measurements exported to {filepath}")
```

### Usage Example

```python
import pynlm
from nlm_profiling import PerformanceProfiler

# Create brain and start profiling
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

profiler = PerformanceProfiler(brain)
profiler.start_profiling()

# Run simulation
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step} complete")

profiler.stop_profiling()

# Get performance summary
summary = profiler.get_performance_summary()
print(f"Performance summary: {summary}")

# Export measurements for analysis
profiler.export_measurements("brain_performance.csv")
```

---

## Memory Usage Monitor

### Memory Usage Monitor Class

```python
import pynlm
import psutil
import time
from typing import Dict, Callable
from dataclasses import dataclass
from enum import Enum

class MemoryRegion(Enum):
    BRAIN = "brain"
    WORKING_MEMORY = "working_memory"
    EPISODIC_MEMORY = "episodic_memory"
    AGENT = "agent"
    WORLD = "world"

@dataclass
class MemorySnapshot:
    timestamp: float
    region: MemoryRegion
    usage_bytes: int
    usage_percentage: float
    peak_usage_bytes: int
    object_count: int

class MemoryUsageMonitor:
    """Monitor memory usage of NLM components"""
    
    def __init__(self, brain: pynlm.Brain, agent: pynlm.AgentBrain, world: pynlm.SimpleWorld):
        self.brain = brain
        self.agent = agent
        self.world = world
        self.snapshots: List[MemorySnapshot] = []
        self.is_monitoring = False
        self.monitor_thread: threading.Thread = None
        self.threshold_mb = 100  # Memory threshold in MB
        self.alert_callback: Callable[[MemorySnapshot], None] = None
    
    def start_monitoring(self, sampling_interval: float = 1.0):
        """Start memory monitoring"""
        self.is_monitoring = True
        self.monitor_thread = threading.Thread(target=self._monitoring_loop, 
                                             args=(sampling_interval,))
        self.monitor_thread.daemon = True
        self.monitor_thread.start()
        
        print("Memory monitoring started")
    
    def stop_monitoring(self):
        """Stop memory monitoring"""
        self.is_monitoring = False
        
        if self.monitor_thread:
            self.monitor_thread.join(timeout=1.0)
            self.monitor_thread = None
        
        print("Memory monitoring stopped")
    
    def set_alert_callback(self, callback: Callable[[MemorySnapshot], None]):
        """Set callback for memory threshold alerts"""
        self.alert_callback = callback
    
    def set_memory_threshold(self, threshold_mb: float):
        """Set memory usage threshold in MB"""
        self.threshold_mb = threshold_mb
    
    def _monitoring_loop(self, interval: float):
        """Background memory monitoring loop"""
        process = psutil.Process()
        
        while self.is_monitoring:
            try:
                self._take_memory_snapshot(process)
                time.sleep(interval)
            except Exception as e:
                print(f"Memory monitoring error: {e}")
                time.sleep(interval)
    
    def _take_memory_snapshot(self, process: psutil.Process):
        """Take a memory snapshot of all NLM components"""
        timestamp = time.time()
        
        # Get process memory info
        memory_info = process.memory_info()
        total_memory_mb = memory_info.rss / (1024 * 1024)
        
        # Component memory estimates (simplified)
        brain_memory = self._estimate_brain_memory()
        working_memory = self._estimate_working_memory()
        episodic_memory = self._estimate_episodic_memory()
        agent_memory = self._estimate_agent_memory()
        world_memory = self._estimate_world_memory()
        
        # Create snapshots
        snapshots = [
            MemorySnapshot(timestamp, MemoryRegion.BRAIN, brain_memory, 
                         (brain_memory / total_memory_mb) * 100, brain_memory, 
                         self.brain.getTotalNeuronCount()),
            MemorySnapshot(timestamp, MemoryRegion.WORKING_MEMORY, working_memory,
                         (working_memory / total_memory_mb) * 100, working_memory,
                         self.brain.getWorkingMemory().getActiveTraces() if self.brain.getWorkingMemory() else 0),
            MemorySnapshot(timestamp, MemoryRegion.EPISODIC_MEMORY, episodic_memory,
                         (episodic_memory / total_memory_mb) * 100, episodic_memory,
                         self.brain.getEpisodicMemory().getEpisodeCount() if self.brain.getEpisodicMemory() else 0),
            MemorySnapshot(timestamp, MemoryRegion.AGENT, agent_memory,
                         (agent_memory / total_memory_mb) * 100, agent_memory, 1),
            MemorySnapshot(timestamp, MemoryRegion.WORLD, world_memory,
                         (world_memory / total_memory_mb) * 100, world_memory, 1)
        ]
        
        # Add to snapshots list
        self.snapshots.extend(snapshots)
        
        # Check thresholds and alert
        for snapshot in snapshots:
            if snapshot.usage_mb > self.threshold_mb:
                if self.alert_callback:
                    try:
                        self.alert_callback(snapshot)
                    except Exception as e:
                        print(f"Alert callback error: {e}")
    
    def _estimate_brain_memory(self) -> int:
        """Estimate brain memory usage in bytes"""
        # Rough estimate based on neuron and synapse counts
        neurons = self.brain.getTotalNeuronCount()
        synapses = self.brain.getTotalSynapseCount()
        
        # Estimate: ~100 bytes per neuron, ~50 bytes per synapse
        return (neurons * 100 + synapses * 50) * 1024  # Convert to KB
    
    def _estimate_working_memory(self) -> int:
        """Estimate working memory usage in bytes"""
        if self.brain.getWorkingMemory():
            traces = self.brain.getWorkingMemory().getActiveTraces()
            return traces * 1024  # Rough estimate
        return 0
    
    def _estimate_episodic_memory(self) -> int:
        """Estimate episodic memory usage in bytes"""
        if self.brain.getEpisodicMemory():
            episodes = self.brain.getEpisodicMemory().getEpisodeCount()
            # Estimate: ~1 KB per episode
            return episodes * 1024
        return 0
    
    def _estimate_agent_memory(self) -> int:
        """Estimate agent memory usage in bytes"""
        # Simplified estimate
        return 50 * 1024
    
    def _estimate_world_memory(self) -> int:
        """Estimate world memory usage in bytes"""
        # Simplified estimate based on grid size
        width = self.world.getWidth()
        height = self.world.getHeight()
        return (width * height * 100) * 1024  # Rough estimate
    
    def get_memory_history(self, region: MemoryRegion = None) -> List[MemorySnapshot]:
        """Get memory history for a specific region or all regions"""
        if region:
            return [s for s in self.snapshots if s.region == region]
        return self.snapshots
    
    def get_peak_memory_usage(self, region: MemoryRegion = None) -> float:
        """Get peak memory usage for a region"""
        history = self.get_memory_history(region)
        if not history:
            return 0
        
        return max(s.peak_usage_bytes for s in history) / (1024 * 1024)  # Convert to MB
    
    def get_average_memory_usage(self, region: MemoryRegion = None) -> float:
        """Get average memory usage for a region"""
        history = self.get_memory_history(region)
        if not history:
            return 0
        
        return sum(s.usage_bytes for s in history) / len(history) / (1024 * 1024)  # Convert to MB
    
    def export_memory_data(self, filepath: str):
        """Export memory data to CSV file"""
        import csv
        
        with open(filepath, 'w', newline='') as csvfile:
            fieldnames = ['timestamp', 'region', 'usage_mb', 'usage_percentage', 'peak_mb']
            writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
            writer.writeheader()
            
            for snapshot in self.snapshots:
                writer.writerow({
                    'timestamp': snapshot.timestamp,
                    'region': snapshot.region.value,
                    'usage_mb': snapshot.usage_mb,
                    'usage_percentage': snapshot.usage_percentage,
                    'peak_mb': snapshot.peak_mb
                })
        
        print(f"Memory data exported to {filepath}")
    
    def analyze_memory_patterns(self) -> Dict[str, Any]:
        """Analyze memory usage patterns"""
        if not self.snapshots:
            return {}
        
        # Group by region
        region_data = {}
        for region in MemoryRegion:
            region_snapshots = self.get_memory_history(region)
            if region_snapshots:
                usage_mb = [s.usage_mb for s in region_snapshots]
                region_data[region.value] = {
                    'peak_mb': max(usage_mb),
                    'average_mb': sum(usage_mb) / len(usage_mb),
                    'min_mb': min(usage_mb),
                    'std_dev': sum((x - sum(usage_mb)/len(usage_mb))**2 for x in usage_mb) / len(usage_mb),
                    'total_samples': len(usage_mb)
                }
        
        return {
            'total_duration': self.snapshots[-1].timestamp - self.snapshots[0].timestamp if len(self.snapshots) > 1 else 0,
            'regions': region_data,
            'total_peak_mb': max(region_data.values(), key=lambda x: x['peak_mb'])['peak_mb'] if region_data else 0
        }
```

### Usage Example

```python
import pynlm
from nlm_memory_monitor import MemoryUsageMonitor

# Create components
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Start memory monitoring
monitor = MemoryUsageMonitor(brain, agent, world)
monitor.start_monitoring(sampling_interval=0.5)  # Sample every 0.5 seconds

# Set memory threshold and alert callback
def memory_alert(snapshot):
    print(f"MEMORY ALERT: {snapshot.region.value} usage exceeded {monitor.threshold_mb}MB")
    print(f"  Current usage: {snapshot.usage_mb:.2f}MB")

monitor.set_alert_callback(memory_alert)
monitor.set_memory_threshold(500)  # 500MB threshold

# Run simulation with memory monitoring
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 100 == 0:
        print(f"Step {step} complete")

# Stop monitoring
monitor.stop_monitoring()

# Analyze memory patterns
analysis = monitor.analyze_memory_patterns()
print(f"Memory analysis: {analysis}")

# Export memory data
monitor.export_memory_data("memory_usage.csv")
```

---

## Event Tracer

### Event Tracer Class

```python
import pynlm
import time
from typing import List, Callable, Dict, Any
from enum import Enum
from dataclasses import dataclass
from datetime import datetime

class EventType(Enum):
    NEURON_SPIKE = "neuron_spike"
    SYNAPSE_STRENGTH_CHANGE = "synapse_strength_change"
    MEMORY_STORE = "memory_store"
    MEMORY_REPLAY = "memory_replay"
    DEVELOPMENT_STAGE_CHANGE = "development_stage_change"
    NEUROMODULATION_UPDATE = "neuromodulation_update"
    ACTION_SELECTION = "action_selection"
    SENSORY_INPUT = "sensory_input"
    EPISODE_STORAGE = "episode_storage"

@dataclass
class SystemEvent:
    event_id: str
    event_type: EventType
    timestamp: float
    step: int
    source: str
    target: str
    data: Dict[str, Any]
    description: str

class EventTracer:
    """Trace and analyze events in NLM simulations"""
    
    def __init__(self, brain: pynlm.Brain, agent: pynlm.AgentBrain, world: pynlm.SimpleWorld):
        self.brain = brain
        self.agent = agent
        self.world = world
        self.events: List[SystemEvent] = []
        self.event_filters: List[Callable[[SystemEvent], bool]] = []
        self.event_handlers: Dict[EventType, List[Callable[[SystemEvent], None]]] = {}
        self.is_tracing = False
        self.trace_thread: threading.Thread = None
    
    def start_tracing(self):
        """Start event tracing"""
        self.is_tracing = True
        self.events.clear()
        
        # Set up default event handlers
        self._setup_default_handlers()
        
        print("Event tracing started")
    
    def stop_tracing(self):
        """Stop event tracing"""
        self.is_tracing = False
        
        if self.trace_thread:
            self.trace_thread.join(timeout=1.0)
            self.trace_thread = None
        
        print(f"Event tracing stopped. Traced {len(self.events)} events")
    
    def add_event_filter(self, filter_func: Callable[[SystemEvent], bool]):
        """Add event filter function"""
        self.event_filters.append(filter_func)
    
    def add_event_handler(self, event_type: EventType, handler: Callable[[SystemEvent], None]):
        """Add event handler for specific event type"""
        if event_type not in self.event_handlers:
            self.event_handlers[event_type] = []
        self.event_handlers[event_type].append(handler)
    
    def _setup_default_handlers(self):
        """Set up default event handlers"""
        # Neuron spike handler
        self.add_event_handler(EventType.NEURON_SPIKE, self._handle_neuron_spike)
        
        # Synapse change handler
        self.add_event_handler(EventType.SYNAPSE_STRENGTH_CHANGE, self._handle_synapse_change)
        
        # Memory handlers
        self.add_event_handler(EventType.MEMORY_STORE, self._handle_memory_store)
        self.add_event_handler(EventType.MEMORY_REPLAY, self._handle_memory_replay)
        
        # Development handler
        self.add_event_handler(EventType.DEVELOPMENT_STAGE_CHANGE, self._handle_development_change)
        
        # Neuromodulation handler
        self.add_event_handler(EventType.NEUROMODULATION_UPDATE, self._handle_neuromodulation)
        
        # Action handler
        self.add_event_handler(EventType.ACTION_SELECTION, self._handle_action_selection)
        
        # Sensory input handler
        self.add_event_handler(EventType.SENSORY_INPUT, self._handle_sensory_input)
        
        # Episode storage handler
        self.add_event_handler(EventType.EPISODE_STORAGE, self._handle_episode_storage)
    
    def _handle_neuron_spike(self, event: SystemEvent):
        """Handle neuron spike events"""
        neuron_id = event.data.get('neuron_id')
        potential = event.data.get('potential', 0.0)
        
        # Log spike information
        print(f"[NEURON SPIKE] Neuron {neuron_id}: V={potential:.3f} at step {event.step}")
    
    def _handle_synapse_change(self, event: SystemEvent):
        """Handle synapse strength change events"""
        synapse_id = event.data.get('synapse_id')
        source_neuron = event.data.get('source_neuron')
        target_neuron = event.data.get('target_neuron')
        old_weight = event.data.get('old_weight', 0.0)
        new_weight = event.data.get('new_weight', 0.0)
        delta = event.data.get('delta', 0.0)
        
        print(f"[SYNAPSE CHANGE] Synapse {synapse_id} ({source_neuron} -> {target_neuron}): "
              f"{old_weight:.4f} -> {new_weight:.4f} (Δ={delta:.4f}) at step {event.step}")
    
    def _handle_memory_store(self, event: SystemEvent):
        """Handle memory store events"""
        memory_type = event.data.get('memory_type')
        item_id = event.data.get('item_id')
        activation = event.data.get('activation', 0.0)
        
        print(f"[MEMORY STORE] {memory_type} item {item_id}: activation={activation:.3f} at step {event.step}")
    
    def _handle_memory_replay(self, event: SystemEvent):
        """Handle memory replay events"""
        episode_id = event.data.get('episode_id')
        replay_strength = event.data.get('replay_strength', 1.0)
        
        print(f"[MEMORY REPLAY] Episode {episode_id}: strength={replay_strength:.3f} at step {event.step}")
    
    def _handle_development_change(self, event: SystemEvent):
        """Handle development stage change events"""
        old_stage = event.data.get('old_stage')
        new_stage = event.data.get('new_stage')
        
        print(f"[DEVELOPMENT CHANGE] Stage changed from {old_stage} to {new_stage} at step {event.step}")
    
    def _handle_neuromodulation(self, event: SystemEvent):
        """Handle neuromodulation update events"""
        neuromodulator_type = event.data.get('neuromodulator_type')
        level = event.data.get('level', 0.0)
        effect = event.data.get('effect', '')
        
        print(f"[NEUROMODULATION] {neuromodulator_type}: level={level:.3f}, effect={effect} at step {event.step}")
    
    def _handle_action_selection(self, event: SystemEvent):
        """Handle action selection events"""
        action_type = event.data.get('action_type')
        confidence = event.data.get('confidence', 0.0)
        source = event.data.get('source', 'unknown')
        
        print(f"[ACTION SELECTION] {action_type}: confidence={confidence:.3f} (source: {source}) at step {event.step}")
    
    def _handle_sensory_input(self, event: SystemEvent):
        """Handle sensory input events"""
        modality = event.data.get('modality', 'unknown')
        data_summary = str(event.data.get('data', []))[:50]  # First 50 chars
        
        print(f"[SENSORY INPUT] {modality}: {data_summary}... at step {event.step}")
    
    def _handle_episode_storage(self, event: SystemEvent):
        """Handle episode storage events"""
        episode_id = event.data.get('episode_id')
        episode_length = event.data.get('episode_length', 0)
        
        print(f"[EPISODE STORAGE] Episode {episode_id}: length={episode_length} steps at step {event.step}")
    
    def trace_event(self, event_type: EventType, source: str, target: str, 
                    data: Dict[str, Any], description: str = ""):
        """Trace a single event"""
        event = SystemEvent(
            event_id=f"event_{len(self.events)}_{int(time.time() * 1000)}",
            event_type=event_type,
            timestamp=time.time(),
            step=self.brain.getTotalSpikeCount(),
            source=source,
            target=target,
            data=data,
            description=description
        )
        
        # Apply filters
        if self.event_filters:
            if not any(filter_func(event) for filter_func in self.event_filters):
                return
        
        # Add to events list
        self.events.append(event)
        
        # Notify handlers
        if event_type in self.event_handlers:
            for handler in self.event_handlers[event_type]:
                try:
                    handler(event)
                except Exception as e:
                    print(f"Event handler error: {e}")
    
    def get_events_by_type(self, event_type: EventType) -> List[SystemEvent]:
        """Get events of a specific type"""
        return [e for e in self.events if e.event_type == event_type]
    
    def get_events_for_step_range(self, start_step: int, end_step: int) -> List[SystemEvent]:
        """Get events within a step range"""
        return [e for e in self.events if start_step <= e.step <= end_step]
    
    def get_events_for_time_range(self, start_time: float, end_time: float) -> List[SystemEvent]:
        """Get events within a time range"""
        return [e for e in self.events if start_time <= e.timestamp <= end_time]
    
    def search_events(self, **filters) -> List[SystemEvent]:
        """Search events with specific filters"""
        results = self.events
        
        for key, value in filters.items():
            if hasattr(SystemEvent, key):
                results = [e for e in results if getattr(e, key) == value]
            elif key == 'event_type':
                if isinstance(value, EventType):
                    results = [e for e in results if e.event_type == value]
                else:
                    results = [e for e in results if e.event_type.value == value]
            elif key == 'step_range':
                start, end = value
                results = [e for e in results if start <= e.step <= end]
            elif key == 'time_range':
                start, end = value
                results = [e for e in results if start <= e.timestamp <= end]
        
        return results
    
    def export_events(self, filepath: str):
        """Export events to JSON file"""
        import json
        
        events_data = []
        for event in self.events:
            events_data.append({
                'event_id': event.event_id,
                'event_type': event.event_type.value,
                'timestamp': event.timestamp,
                'step': event.step,
                'source': event.source,
                'target': event.target,
                'data': event.data,
                'description': event.description
            })
        
        with open(filepath, 'w') as f:
            json.dump(events_data, f, indent=2)
        
        print(f"Events exported to {filepath}")
    
    def analyze_event_patterns(self) -> Dict[str, Any]:
        """Analyze event patterns and statistics"""
        if not self.events:
            return {}
        
        # Count by event type
        event_counts = {}
        for event_type in EventType:
            event_counts[event_type.value] = len(self.get_events_by_type(event_type))
        
        # Calculate average events per step
        max_step = max(e.step for e in self.events)
        min_step = min(e.step for e in self.events)
        total_steps = max_step - min_step + 1 if max_step >= min_step else 1
        
        events_per_step = len(self.events) / total_steps
        
        # Time-based analysis
        event_times = [e.timestamp for e in self.events]
        time_span = max(event_times) - min(event_times) if event_times else 0
        
        # Source-target analysis
        sources = {}
        targets = {}
        for event in self.events:
            sources[event.source] = sources.get(event.source, 0) + 1
            targets[event.target] = targets.get(event.target, 0) + 1
        
        return {
            'total_events': len(self.events),
            'time_span': time_span,
            'events_per_step': events_per_step,
            'event_counts': event_counts,
            'sources': sources,
            'targets': targets,
            'unique_event_types': list(event_counts.keys()),
            'most_active_source': max(sources.items(), key=lambda x: x[1])[0] if sources else None,
            'most_active_target': max(targets.items(), key=lambda x: x[1])[0] if targets else None
        }
```

### Usage Example

```python
import pynlm
from nlm_event_tracer import EventTracer

# Create components
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Start event tracing
tracer = EventTracer(brain, agent, world)
tracer.start_tracing()

# Set up event handlers
def spike_handler(event):
    print(f"IMPORTANT SPIKE: Neuron activity detected at step {event.step}")

tracer.add_event_handler(EventType.NEURON_SPIKE, spike_handler)

def memory_handler(event):
    print(f"MEMORY EVENT: {event.data.get('memory_type', 'unknown')} at step {event.step}")

tracer.add_event_handler(EventType.MEMORY_STORE, memory_handler)

# Set up event filters
def spike_filter(event):
    return event.event_type == EventType.NEURON_SPIKE and event.step % 10 == 0

tracer.add_event_filter(spike_filter)

# Run simulation with event tracing
for step in range(100):
    world.update(0.1)
    
    # Manually trace sensory input
    tracer.trace_event(
        event_type=EventType.SENSORY_INPUT,
        source="world",
        target="agent",
        data={"modality": "vision", "data": list(world.getSensoryPercept().getVision())},
        description="Agent receives visual input from world"
    )
    
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    # Manually trace action selection
    action = agent.decodeMotorCommand()
    tracer.trace_event(
        event_type=EventType.ACTION_SELECTION,
        source="agent",
        target="world",
        data={"action_type": str(action.getType()), "confidence": 0.8},
        description="Agent selects motor action"
    )
    
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 50 == 0:
        print(f"Step {step} complete")

# Stop tracing
tracer.stop_tracing()

# Get event statistics
stats = tracer.analyze_event_patterns()
print(f"Event statistics: {stats}")

# Search for specific events
spike_events = tracer.get_events_by_type(EventType.NEURON_SPIKE)
print(f"Total spike events: {len(spike_events)}")

# Export events for analysis
tracer.export_events("simulation_events.json")
```

---

## System Health Monitor

### System Health Monitor Class

```python
import pynlm
import time
from typing import Dict, List, Callable
from enum import Enum
from dataclasses import dataclass
from datetime import datetime, timedelta

class HealthStatus(Enum):
    HEALTHY = "healthy"
    WARNING = "warning"
    CRITICAL = "critical"
    UNKNOWN = "unknown"

class ComponentHealth(Enum):
    BRAIN = "brain"
    AGENT = "agent"
    WORLD = "world"
    MEMORY = "memory"
    NEUROMODULATION = "neuromodulation"
    DEVELOPMENT = "development"

@dataclass
class HealthCheck:
    component: ComponentHealth
    status: HealthStatus
    message: str
    timestamp: float
    details: Dict[str, Any] = None

class SystemHealthMonitor:
    """Monitor system health and perform health checks"""
    
    def __init__(self, brain: pynlm.Brain, agent: pynlm.AgentBrain, world: pynlm.SimpleWorld):
        self.brain = brain
        self.agent = agent
        self.world = world
        self.health_history: List[HealthCheck] = []
        self.alert_callbacks: List[Callable[[HealthCheck], None]] = []
        self.check_intervals = {
            ComponentHealth.BRAIN: 1.0,      # Check every 1 second
            ComponentHealth.AGENT: 1.0,     # Check every 1 second
            ComponentHealth.WORLD: 2.0,     # Check every 2 seconds
            ComponentHealth.MEMORY: 2.0,    # Check every 2 seconds
            ComponentHealth.NEUROMODULATION: 1.0,  # Check every 1 second
            ComponentHealth.DEVELOPMENT: 5.0    # Check every 5 seconds
        }
        self.is_monitoring = False
        self.monitor_thread: threading.Thread = None
    
    def start_monitoring(self):
        """Start system health monitoring"""
        self.is_monitoring = True
        self.health_history.clear()
        
        self.monitor_thread = threading.Thread(target=self._monitoring_loop)
        self.monitor_thread.daemon = True
        self.monitor_thread.start()
        
        print("System health monitoring started")
    
    def stop_monitoring(self):
        """Stop system health monitoring"""
        self.is_monitoring = False
        
        if self.monitor_thread:
            self.monitor_thread.join(timeout=1.0)
            self.monitor_thread = None
        
        print("System health monitoring stopped")
    
    def add_alert_callback(self, callback: Callable[[HealthCheck], None]):
        """Add alert callback for health check alerts"""
        self.alert_callbacks.append(callback)
    
    def set_check_interval(self, component: ComponentHealth, interval: float):
        """Set check interval for a component"""
        self.check_intervals[component] = interval
    
    def _monitoring_loop(self):
        """Background health monitoring loop"""
        last_checks = {component: 0.0 for component in ComponentHealth}
        
        while self.is_monitoring:
            current_time = time.time()
            
            # Perform health checks for each component
            for component in ComponentHealth:
                if current_time - last_checks[component] >= self.check_intervals[component]:
                    self._perform_health_check(component)
                    last_checks[component] = current_time
            
            time.sleep(0.1)  # Small sleep to prevent busy waiting
    
    def _perform_health_check(self, component: ComponentHealth) -> HealthCheck:
        """Perform health check for a component"""
        timestamp = time.time()
        health_check = None
        
        try:
            if component == ComponentHealth.BRAIN:
                health_check = self._check_brain_health()
            elif component == ComponentHealth.AGENT:
                health_check = self._check_agent_health()
            elif component == ComponentHealth.WORLD:
                health_check = self._check_world_health()
            elif component == ComponentHealth.MEMORY:
                health_check = self._check_memory_health()
            elif component == ComponentHealth.NEUROMODULATION:
                health_check = self._check_neuromodulation_health()
            elif component == ComponentHealth.DEVELOPMENT:
                health_check = self._check_development_health()
            
            if health_check:
                self.health_history.append(health_check)
                
                # Notify alert callbacks for critical issues
                if health_check.status == HealthStatus.CRITICAL:
                    for callback in self.alert_callbacks:
                        try:
                            callback(health_check)
                        except Exception as e:
                            print(f"Alert callback error: {e}")
                
                # Log the health check
                status_icon = "✓" if health_check.status == HealthStatus.HEALTHY else "⚠" if health_check.status == HealthStatus.WARNING else "✗"
                print(f"{status_icon} {component.value}: {health_check.message}")
            
        except Exception as e:
            # Create error health check
            health_check = HealthCheck(
                component=component,
                status=HealthStatus.CRITICAL,
                message=f"Health check failed: {e}",
                timestamp=timestamp,
                details={"error": str(e)}
            )
            self.health_history.append(health_check)
        
        return health_check
    
    def _check_brain_health(self) -> HealthCheck:
        """Check brain system health"""
        issues = []
        
        # Check basic brain stats
        if self.brain.getTotalNeuronCount() == 0:
            issues.append("No neurons in brain")
        
        if self.brain.getTotalSynapseCount() == 0:
            issues.append("No synapses in brain")
        
        # Check firing rate
        firing_rate = self.brain.getAverageFiringRate()
        if firing_rate < 0.1:
            issues.append(f"Very low firing rate: {firing_rate:.3f}")
        elif firing_rate > 100:
            issues.append(f"Very high firing rate: {firing_rate:.3f}")
        
        # Check E/I ratio
        ei_ratio = self.brain.getExcitationInhibitionRatio()
        if ei_ratio < 0.1 or (not isinstance(ei_ratio, float) and ei_ratio != float('inf')):
            issues.append(f"Abnormal E/I ratio: {ei_ratio}")
        
        # Check memory systems
        if self.brain.getWorkingMemory() is None:
            issues.append("Working memory not integrated")
        
        if self.brain.getEpisodicMemory() is None:
            issues.append("Episodic memory not integrated")
        
        # Check prediction system
        if self.brain.getPredictionSystem() is None:
            issues.append("Prediction system not integrated")
        
        if issues:
            if len(issues) == 1:
                return HealthCheck(
                    component=ComponentHealth.BRAIN,
                    status=HealthStatus.WARNING,
                    message=f"Brain has issues: {issues[0]}",
                    timestamp=time.time(),
                    details={"issues": issues}
                )
            else:
                return HealthCheck(
                    component=ComponentHealth.BRAIN,
                    status=HealthStatus.CRITICAL,
                    message=f"Brain has multiple issues: {'; '.join(issues)}",
                    timestamp=time.time(),
                    details={"issues": issues}
                )
        else:
            return HealthCheck(
                component=ComponentHealth.BRAIN,
                status=HealthStatus.HEALTHY,
                message="Brain system is healthy",
                timestamp=time.time(),
                details={"neuron_count": self.brain.getTotalNeuronCount(),
                        "synapse_count": self.brain.getTotalSynapseCount(),
                        "firing_rate": firing_rate,
                        "e_i_ratio": ei_ratio}
            )
    
    def _check_agent_health(self) -> HealthCheck:
        """Check agent system health"""
        issues = []
        
        # Check agent basic functionality
        if self.agent.getBrain() is None:
            issues.append("Agent has no brain reference")
        
        # Check neuromodulation systems
        if not self.agent.isRewardModulationEnabled():
            issues.append("Reward modulation not enabled")
        
        if not self.agent.isCuriosityEnabled():
            issues.append("Curiosity not enabled")
        
        # Check development
        if not self.agent.isDevelopmentEnabled():
            issues.append("Development not enabled")
        
        # Check neuromodulation levels
        if self.agent.getNeuromodulationLevel() < 0.0 or self.agent.getNeuromodulationLevel() > 2.0:
            issues.append(f"Abnormal neuromodulation level: {self.agent.getNeuromodulationLevel():.3f}")
        
        if issues:
            if len(issues) == 1:
                return HealthCheck(
                    component=ComponentHealth.AGENT,
                    status=HealthStatus.WARNING,
                    message=f"Agent has issues: {issues[0]}",
                    timestamp=time.time(),
                    details={"issues": issues}
                )
            else:
                return HealthCheck(
                    component=ComponentHealth.AGENT,
                    status=HealthStatus.CRITICAL,
                    message=f"Agent has multiple issues: {'; '.join(issues)}",
                    timestamp=time.time(),
                    details={"issues": issues}
                )
        else:
            return HealthCheck(
                component=ComponentHealth.AGENT,
                status=HealthStatus.HEALTHY,
                message="Agent system is healthy",
                timestamp=time.time(),
                details={"reward_enabled": self.agent.isRewardModulationEnabled(),
                        "curiosity_enabled": self.agent.isCuriosityEnabled(),
                        "development_enabled": self.agent.isDevelopmentEnabled(),
                        "neuromodulation_level": self.agent.getNeuromodulationLevel()}
            )
    
    def _check_world_health(self) -> HealthCheck:
        """Check world system health"""
        issues = []
        
        # Check world dimensions
        if self.world.getWidth() == 0 or self.world.getHeight() == 0:
            issues.append("Invalid world dimensions")
        
        # Check agent position
        body = self.world.getAgentBody()
        if body is None:
            issues.append("Agent body not available")
        else:
            if body.x < 0 or body.x > self.world.getWidth() or body.y < 0 or body.y > self.world.getHeight():
                issues.append("Agent position outside world bounds")
        
        if issues:
            if len(issues) == 1:
                return HealthCheck(
                    component=ComponentHealth.WORLD,
                    status=HealthStatus.WARNING,
                    message=f"World has issues: {issues[0]}",
                    timestamp=time.time(),
                    details={"issues": issues}
                )
            else:
                return HealthCheck(
                    component=ComponentHealth.WORLD,
                    status=HealthStatus.CRITICAL,
                    message=f"World has multiple issues: {'; '.join(issues)}",
                    timestamp=time.time(),
                    details={"issues": issues}
                )
        else:
            return HealthCheck(
                component=ComponentHealth.WORLD,
                status=HealthStatus.HEALTHY,
                message="World system is healthy",
                timestamp=time.time(),
                details={"width": self.world.getWidth(),
                        "height": self.world.getHeight(),
                        "time": self.world.getSimulationTime()}
            )
    
    def _check_memory_health(self) -> HealthCheck:
        """Check memory system health"""
        issues = []
        
        # Check working memory
        if self.brain.getWorkingMemory():
            traces = self.brain.getWorkingMemory().getActiveTraces()
            if traces > 1000:
                issues.append(f"High working memory load: {traces} traces")
            elif traces == 0:
                issues.append("No working memory traces")
        else:
            issues.append("Working memory not available")
        
        # Check episodic memory
        if self.brain.getEpisodicMemory():
            episodes = self.brain.getEpisodicMemory().getEpisodeCount()
            if episodes > 10000:
                issues.append(f"High episodic memory load: {episodes} episodes")
            elif episodes == 0:
                issues.append("No episodic episodes stored")
        else:
            issues.append("Episodic memory not available")
        
        if issues:
            if len(issues) == 1:
                return HealthCheck(
                    component=ComponentHealth.MEMORY,
                    status=HealthStatus.WARNING,
                    message=f"Memory system has issues: {issues[0]}",
                    timestamp=time.time(),
                    details={"issues": issues}
                )
            else:
                return HealthCheck(
                    component=ComponentHealth.MEMORY,
                    status=HealthStatus.CRITICAL,
                    message=f"Memory system has multiple issues: {'; '.join(issues)}",
                    timestamp=time.time(),
                    details={"issues": issues}
                )
        else:
            return HealthCheck(
                component=ComponentHealth.MEMORY,
                status=HealthStatus.HEALTHY,
                message="Memory system is healthy",
                timestamp=time.time(),
                details={"working_memory_traces": self.brain.getWorkingMemory().getActiveTraces() if self.brain.getWorkingMemory() else 0,
                        "episodic_episodes": self.brain.getEpisodicMemory().getEpisodeCount() if self.brain.getEpisodicMemory() else 0}
            )
    
    def _check_neuromodulation_health(self) -> HealthCheck:
        """Check neuromodulation system health"""
        issues = []
        
        # Check dopamine level
        if self.brain.getDopamine():
            dopamine_level = self.brain.getDopamine().getLevel()
            if dopamine_level < 0.0 or dopamine_level > 2.0:
                issues.append(f"Abnormal dopamine level: {dopamine_level:.3f}")
        else:
            issues.append("Dopamine system not available")
        
        # Check curiosity level
        if self.brain.getCuriosity():
            curiosity_level = self.brain.getCuriosity().getLevel()
            if curiosity_level < 0.0 or curiosity_level > 1.0:
                issues.append(f"Abnormal curiosity level: {curiosity_level:.3f}")
        else:
            issues.append("Curiosity system not available")
        
        # Check novelty level
        if self.brain.getNovelty():
            novelty_level = self.brain.getNovelty().getLevel()
            if novelty_level < 0.0 or novelty_level > 1.0:
                issues.append(f"Abnormal novelty level: {novelty_level:.3f}")
        else:
            issues.append("Novelty system not available")
        
        if issues:
            if len(issues) == 1:
                return HealthCheck(
                    component=ComponentHealth.NEUROMODULATION,
                    status=HealthStatus.WARNING,
                    message=f"Neuromodulation system has issues: {issues[0]}",
                    timestamp=time.time(),
                    details={"issues": issues}
                )
            else:
                return HealthCheck(
                    component=ComponentHealth.NEUROMODULATION,
                    status=HealthStatus.CRITICAL,
                    message=f"Neuromodulation system has multiple issues: {'; '.join(issues)}",
                    timestamp=time.time(),
                    details={"issues": issues}
                )
        else:
            return HealthCheck(
                component=ComponentHealth.NEUROMODULATION,
                status=HealthStatus.HEALTHY,
                message="Neuromodulation system is healthy",
                timestamp=time.time(),
                details={"dopamine_level": self.brain.getDopamine().getLevel() if self.brain.getDopamine() else 0,
                        "curiosity_level": self.brain.getCuriosity().getLevel() if self.brain.getCuriosity() else 0,
                        "novelty_level": self.brain.getNovelty().getLevel() if self.brain.getNovelty() else 0}
            )
    
    def _check_development_health(self) -> HealthCheck:
        """Check development system health"""
        issues = []
        
        # Check developmental stage
        stage = self.brain.getDevelopmentalStage()
        if stage is None:
            issues.append("Developmental stage not available")
        
        # Check development system
        if self.brain.getDevelopmentSystem() is None:
            issues.append("Development system not available")
        
        if issues:
            if len(issues) == 1:
                return HealthCheck(
                    component=ComponentHealth.DEVELOPMENT,
                    status=HealthStatus.WARNING,
                    message=f"Development system has issues: {issues[0]}",
                    timestamp=time.time(),
                    details={"issues": issues}
                )
            else:
                return HealthCheck(
                    component=ComponentHealth.DEVELOPMENT,
                    status=HealthStatus.CRITICAL,
                    message=f"Development system has multiple issues: {'; '.join(issues)}",
                    timestamp=time.time(),
                    details={"issues": issues}
                )
        else:
            return HealthCheck(
                component=ComponentHealth.DEVELOPMENT,
                status=HealthStatus.HEALTHY,
                message=f"Development system is healthy (stage: {stage})",
                timestamp=time.time(),
                details={"developmental_stage": stage}
            )
    
    def get_current_health(self) -> List[HealthCheck]:
        """Get current health status of all components"""
        health_checks = []
        
        for component in ComponentHealth:
            # Perform a quick check for each component
            if component == ComponentHealth.BRAIN:
                health_checks.append(self._check_brain_health())
            elif component == ComponentHealth.AGENT:
                health_checks.append(self._check_agent_health())
            elif component == ComponentHealth.WORLD:
                health_checks.append(self._check_world_health())
            elif component == ComponentHealth.MEMORY:
                health_checks.append(self._check_memory_health())
            elif component == ComponentHealth.NEUROMODULATION:
                health_checks.append(self._check_neuromodulation_health())
            elif component == ComponentHealth.DEVELOPMENT:
                health_checks.append(self._check_development_health())
        
        return health_checks
    
    def get_health_history(self, component: ComponentHealth = None, time_range: timedelta = None) -> List[HealthCheck]:
        """Get health history for a component or all components"""
        if component:
            history = [h for h in self.health_history if h.component == component]
        else:
            history = self.health_history
        
        if time_range:
            cutoff_time = time.time() - time_range.total_seconds()
            history = [h for h in history if h.timestamp >= cutoff_time]
        
        return history
    
    def get_overall_health(self) -> HealthCheck:
        """Get overall system health status"""
        health_checks = self.get_current_health()
        
        if not health_checks:
            return HealthCheck(
                component=ComponentHealth.BRAIN,  # Default to brain
                status=HealthStatus.UNKNOWN,
                message="No health data available",
                timestamp=time.time()
            )
        
        # Determine overall status
        status = HealthStatus.HEALTHY
        critical_issues = []
        warning_issues = []
        
        for check in health_checks:
            if check.status == HealthStatus.CRITICAL:
                status = HealthStatus.CRITICAL
                critical_issues.append(f"{check.component.value}: {check.message}")
            elif check.status == HealthStatus.WARNING and status == HealthStatus.HEALTHY:
                status = HealthStatus.WARNING
                warning_issues.append(f"{check.component.value}: {check.message}")
        
        if status == HealthStatus.HEALTHY:
            message = "All systems healthy"
        elif status == HealthStatus.WARNING:
            message = f"System warnings: {'; '.join(warning_issues)}"
        else:
            message = f"System critical: {'; '.join(critical_issues)}"
        
        return HealthCheck(
            component=ComponentHealth.BRAIN,  # System-wide check
            status=status,
            message=message,
            timestamp=time.time(),
            details={
                "component_checks": {check.component.value: check.status.value for check in health_checks},
                "critical_issues": critical_issues,
                "warning_issues": warning_issues
            }
        )
    
    def export_health_data(self, filepath: str):
        """Export health data to JSON file"""
        import json
        
        health_data = []
        for check in self.health_history:
            health_data.append({
                'component': check.component.value,
                'status': check.status.value,
                'message': check.message,
                'timestamp': check.timestamp,
                'details': check.details
            })
        
        with open(filepath, 'w') as f:
            json.dump(health_data, f, indent=2)
        
        print(f"Health data exported to {filepath}")
    
    def generate_health_report(self) -> str:
        """Generate comprehensive health report"""
        current_health = self.get_current_health()
        overall_health = self.get_overall_health()
        
        report = f"=== NLM SYSTEM HEALTH REPORT ===\n"
        report += f"Generated at: {datetime.fromtimestamp(time.time()).isoformat()}\n\n"
        
        report += f"OVERALL STATUS: {overall_health.status.value.upper()}\n"
        report += f"Message: {overall_health.message}\n\n"
        
        report += "COMPONENT STATUS:\n"
        for check in current_health:
            status_icon = "✓" if check.status == HealthStatus.HEALTHY else "⚠" if check.status == HealthStatus.WARNING else "✗"
            report += f"  {status_icon} {check.component.value}: {check.message}\n"
        
        report += f"\nHISTORY:\n"
        report += f"Total checks performed: {len(self.health_history)}\n"
        report += f"Healthy checks: {len([h for h in self.health_history if h.status == HealthStatus.HEALTHY])}\n"
        report += f"Warning checks: {len([h for h in self.health_history if h.status == HealthStatus.WARNING])}\n"
        report += f"Critical checks: {len([h for h in self.health_history if h.status == HealthStatus.CRITICAL])}\n"
        
        return report
```

### Usage Example

```python
import pynlm
from nlm_health_monitor import SystemHealthMonitor

# Create components
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Start health monitoring
monitor = SystemHealthMonitor(brain, agent, world)
monitor.start_monitoring()

# Set up alert callbacks
def critical_alert(check):
    print(f"🚨 CRITICAL ALERT: {check.component.value} - {check.message}")

def warning_alert(check):
    print(f"⚠️ WARNING ALERT: {check.component.value} - {check.message}")

monitor.add_alert_callback(critical_alert)
monitor.add_alert_callback(warning_alert)

# Run simulation with health monitoring
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step} complete")

# Stop monitoring
monitor.stop_monitoring()

# Generate health report
report = monitor.generate_health_report()
print(report)

# Get current health
overall_health = monitor.get_overall_health()
print(f"Overall health: {overall_health.status.value}")
print(f"Overall message: {overall_health.message}")

# Export health data
monitor.export_health_data("health_report.json")
```

---

## Integration and Usage

### Running the Profiling Tools

```bash
# Install dependencies for profiling tools
pip install psutil numpy matplotlib pandas

# Create profiling scripts
mkdir -p nlm/profiling
mkdir -p nlm/debugging

# Basic profiling example
cat > nlm/profiling/basic_profiling.py << 'EOF'
import pynlm
from nlm_profiling import PerformanceProfiler, MemoryUsageMonitor, EventTracer

# Create components
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Start profiling
profiler = PerformanceProfiler(brain)
memory_monitor = MemoryUsageMonitor(brain, agent, world)
event_tracer = EventTracer(brain, agent, world)

profiler.start_profiling()
memory_monitor.start_monitoring()
event_tracer.start_tracing()

# Run simulation
for step in range(200):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Stop profiling
profiler.stop_profiling()
memory_monitor.stop_monitoring()
event_tracer.stop_tracing()

# Export data
profiler.export_measurements("performance_data.csv")
memory_monitor.export_memory_data("memory_data.csv")
event_tracer.export_events("event_data.json")

print("Profiling complete!")
EOF

# Run the profiling script
python nlm/profiling/basic_profiling.py
```

### Advanced Debugging Script

```python
# Create comprehensive debugging script
cat > nlm/debugging/comprehensive_debugger.py << 'EOF'
import pynlm
from nlm_debugger import ComprehensiveDebugger

# Create components
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(25, 25, 10, 10)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Configure debugger
debugger = ComprehensiveDebugger(brain, agent, world)
debugger.enable_performance_profiling(True)
debugger.enable_memory_monitoring(True)
debugger.enable_event_tracing(True)
debugger.enable_system_health_monitoring(True)

# Set up alerts and callbacks
def on_performance_alert(measurement):
    if measurement.name == "memory_usage" and measurement.value > 500:  # MB
        print(f"🚨 HIGH MEMORY USAGE: {measurement.value:.2f}MB")

def on_health_alert(check):
    if check.status == check.status.CRITICAL:
        print(f"🚨 HEALTH ALERT: {check.component.value} - {check.message}")

debugger.performance_profiler.add_measurement_callback(on_performance_alert)
debugger.health_monitor.add_alert_callback(on_health_alert)

# Start comprehensive debugging
debugger.start_debugging()

# Run complex simulation with debugging
for day in range(10):  # 10 days of debugging
    print(f"\n=== Day {day + 1} ===")
    
    # Run simulation for the day
    for step in range(500):
        world.update(0.1)
        
        # Add custom events
        event_data = {
            "day": day + 1,
            "step": step,
            "time": world.getSimulationTime(),
            "agent_x": world.getAgentBody().x if world.getAgentBody() else 0,
            "agent_y": world.getAgentBody().y if world.getAgentBody() else 0
        }
        
        debugger.event_tracer.trace_event(
            event_type="simulation_step",
            source="simulation",
            target="all",
            data=event_data,
            description=f"Simulation step {step} on day {day + 1}"
        )
        
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Check for learning events
        if step % 100 == 0:
            debugger.check_learning_milestones()
    
    # End of day diagnostics
    debugger.end_of_day_diagnostics(day + 1)

# Stop debugging
debugger.stop_debugging()

# Generate comprehensive report
report = debugger.generate_comprehensive_report()
print(report)

# Export all debugging data
debugger.export_all_debug_data("debugging_session")

print("\n=== Comprehensive debugging complete! ===")
EOF

# Run the comprehensive debugger
python nlm/debugging/comprehensive_debugger.py
```

---

## API Reference

### Performance Profiler Methods

```python
# Core methods
profiler.start_profiling()  # Start performance profiling
profiler.stop_profiling()   # Stop performance profiling
profiler.set_sampling_rate(rate_hz)  # Set sampling rate
profiler.add_measurement_callback(callback)  # Add measurement callback

# Data access
profiler.get_measurements_by_type(measurement_type)  # Get measurements by type
profiler.get_measurements_by_name(name)  # Get measurements by name
profiler.get_measurements_for_step_range(start_step, end_step)  # Get measurements in step range
profiler.get_performance_summary()  # Get performance summary
profiler.export_measurements(filepath)  # Export measurements to CSV
```

### Memory Usage Monitor Methods

```python
# Core methods
monitor.start_monitoring(sampling_interval)  # Start memory monitoring
monitor.stop_monitoring()  # Stop memory monitoring
monitor.set_alert_callback(callback)  # Set memory alert callback
monitor.set_memory_threshold(threshold_mb)  # Set memory threshold

# Data access
monitor.get_memory_history(region)  # Get memory history
monitor.get_peak_memory_usage(region)  # Get peak memory usage
monitor.get_average_memory_usage(region)  # Get average memory usage
monitor.analyze_memory_patterns()  # Analyze memory patterns
monitor.export_memory_data(filepath)  # Export memory data
```

### Event Tracer Methods

```python
# Core methods
tracer.start_tracing()  # Start event tracing
tracer.stop_tracing()  # Stop event tracing
tracer.add_event_filter(filter_func)  # Add event filter
tracer.add_event_handler(event_type, handler)  # Add event handler

# Data access
tracer.get_events_by_type(event_type)  # Get events by type
tracer.get_events_for_step_range(start_step, end_step)  # Get events in step range
tracer.search_events(**filters)  # Search events with filters
tracer.export_events(filepath)  # Export events to JSON
tracer.analyze_event_patterns()  # Analyze event patterns
```

### System Health Monitor Methods

```python
# Core methods
monitor.start_monitoring()  # Start health monitoring
monitor.stop_monitoring()  # Stop health monitoring
monitor.add_alert_callback(callback)  # Add health alert callback

# Data access
monitor.get_current_health()  # Get current health status
monitor.get_health_history(component, time_range)  # Get health history
monitor.get_overall_health()  # Get overall system health
monitor.export_health_data(filepath)  # Export health data
monitor.generate_health_report()  # Generate health report
```

---

## Installation and Setup

### Quick Installation

```bash
# Clone the NLM repository
cd /path/to/nlm

# Install Python dependencies
pip install pynlm psutil numpy matplotlib pandas

# Build C++ components (if needed)
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Create Custom Profiling Scripts

```bash
# Create custom profiling scripts
mkdir -p ~/nlm/scripts

cat > ~/nlm/scripts/start_profiling.sh << 'EOF'
#!/bin/bash

cd /path/to/nlm

# Create profiling directories
mkdir -p profiling_data debug_data health_data

# Start profiling with custom settings
python3 << 'PYTHON_END'
import pynlm
from nlm_profiling import PerformanceProfiler, MemoryUsageMonitor, EventTracer
from nlm_health_monitor import SystemHealthMonitor

# Custom configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(30, 30, 12, 12)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Initialize monitoring tools with custom settings
profiler = PerformanceProfiler(brain)
profiler.set_sampling_rate(5.0)  # 5 Hz sampling

memory_monitor = MemoryUsageMonitor(brain, agent, world)
memory_monitor.set_memory_threshold(800)  # 800MB threshold

event_tracer = EventTracer(brain, agent, world)

health_monitor = SystemHealthMonitor(brain, agent, world)
health_monitor.set_check_interval(health_monitor.ComponentHealth.WORLD, 3.0)

# Start all monitoring tools
profiler.start_profiling()
memory_monitor.start_monitoring(0.5)  # Sample every 0.5 seconds
event_tracer.start_tracing()
health_monitor.start_monitoring()

# Run custom simulation
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Stop monitoring
profiler.stop_profiling()
memory_monitor.stop_monitoring()
event_tracer.stop_tracing()
health_monitor.stop_monitoring()

# Export data with custom names
profiler.export_measurements("profiling_data/custom_performance.csv")
memory_monitor.export_memory_data("profiling_data/custom_memory.csv")
event_tracer.export_events("profiling_data/custom_events.json")
health_monitor.export_health_data("profiling_data/custom_health.json")

print("Custom profiling complete!")
PYTHON_END
EOF

# Make script executable
chmod +x ~/nlm/scripts/start_profiling.sh

# Run the custom profiling script
~/nlm/scripts/start_profiling.sh
```

### Monitoring Dashboards

Create monitoring dashboards to visualize profiling data:

```python
# Create real-time monitoring dashboard
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from nlm_profiling import PerformanceProfiler, MemoryUsageMonitor

# Initialize monitoring tools
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
profiler = PerformanceProfiler(brain)
memory_monitor = MemoryUsageMonitor(brain, None, None)

profiler.start_profiling()
memory_monitor.start_monitoring()

# Create dashboard
fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(15, 10))

# Performance plot
line1, = ax1.plot([], [])
ax1.set_title('Performance Metrics')
ax1.set_xlabel('Step')
ax1.set_ylabel('Value')
ax1.grid(True)

# Memory plot
line2, = ax2.plot([], [])
ax2.set_title('Memory Usage')
ax2.set_xlabel('Time (s)')
ax2.set_ylabel('Memory (MB)')
ax2.grid(True)

# Firing rate plot
line3, = ax3.plot([], [])
ax3.set_title('Neuron Firing Rate')
ax3.set_xlabel('Step')
ax3.set_ylabel('Firing Rate')
ax3.grid(True)

# Event count plot
line4, = ax4.plot([], [])
ax4.set_title('Event Counts')
ax4.set_xlabel('Step')
ax4.set_ylabel('Count')
ax4.grid(True)

# Animation function
def update(frame):
    # Get current data
    performance = profiler.get_performance_summary()
    memory_data = memory_monitor.get_memory_history()
    
    # Update plots (implementation details omitted for brevity)
    pass

# Create animation
ani = animation.FuncAnimation(fig, update, interval=100)
plt.tight_layout()
plt.show()
```

---

## Troubleshooting

### Common Issues and Solutions

1. **"Profiling data not being collected"**
   - Ensure profiling tools are started before simulation
   - Check for errors in measurement callbacks
   - Verify sampling intervals are reasonable

2. **"High memory usage during profiling"**
   - Reduce sampling frequency
   - Use more specific event filters
   - Monitor memory usage and stop profiling if needed

3. **"Event tracer capturing too many events"**
   - Use event filters to limit captured events
   - Optimize event handlers for performance
   - Use selective tracing

4. **"Health monitoring not detecting issues"**
   - Ensure health checks are running
   - Verify alert callbacks are properly set up
   - Check for component initialization issues

5. **"Data export failing"**
   - Check file permissions
   - Ensure directories exist
   - Verify data format compatibility

### Debugging Scripts

```python
# Create comprehensive debugging script
import pynlm
from nlm_profiling import PerformanceProfiler, MemoryUsageMonitor, EventTracer
from nlm_health_monitor import SystemHealthMonitor

def debug_nlm_system():
    """Comprehensive debugging of NLM system"""
    
    # Create components
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Initialize all debugging tools
    profiler = PerformanceProfiler(brain)
    memory_monitor = MemoryUsageMonitor(brain, agent, world)
    event_tracer = EventTracer(brain, agent, world)
    health_monitor = SystemHealthMonitor(brain, agent, world)
    
    # Start debugging
    profiler.start_profiling()
    memory_monitor.start_monitoring()
    event_tracer.start_tracing()
    health_monitor.start_monitoring()
    
    # Set up comprehensive monitoring
    def comprehensive_alert(check):
        print(f"🚨 ALERT [{check.component.value}]: {check.message}")
    
    def performance_alert(measurement):
        print(f"📊 MEASUREMENT [{measurement.name}]: {measurement.value:.3f}")
    
    health_monitor.add_alert_callback(comprehensive_alert)
    profiler.add_measurement_callback(performance_alert)
    
    # Run debug simulation
    print("Starting comprehensive debugging session...")
    
    for step in range(300):
        world.update(0.1)
        
        # Debug current system state
        if step % 50 == 0:
            print(f"\n=== DEBUG CHECK at step {step} ===")
            debugger = ComprehensiveDebugger(brain, agent, world)
            current_health = debugger.get_current_health()
            for health_check in current_health:
                status_icon = "✓" if health_check.status.value == "healthy" else "⚠" if health_check.status.value == "warning" else "✗"
                print(f"{status_icon} {health_check.component.value}: {health_check.message}")
        
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 100 == 0:
            print(f"Step {step} complete")
    
    # Stop debugging
    profiler.stop_profiling()
    memory_monitor.stop_monitoring()
    event_tracer.stop_tracing()
    health_monitor.stop_monitoring()
    
    # Generate comprehensive report
    print("\n=== COMPREHENSIVE DEBUG REPORT ===")
    health_report = health_monitor.generate_health_report()
    print(health_report)
    
    performance_summary = profiler.get_performance_summary()
    print(f"\nPerformance Summary: {performance_summary}")
    
    # Export all data
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    profiler.export_measurements(f"debug_data/profiling_{timestamp}.csv")
    memory_monitor.export_memory_data(f"debug_data/memory_{timestamp}.csv")
    event_tracer.export_events(f"debug_data/events_{timestamp}.json")
    health_monitor.export_health_data(f"debug_data/health_{timestamp}.json")
    
    print(f"\nDebug data exported with timestamp: {timestamp}")

# Run debugging
if __name__ == "__main__":
    debug_nlm_system()
```

---

## Conclusion

The NLM profiling and debugging tools provide comprehensive monitoring capabilities for NLM simulations:

1. **Performance Profiler**: Detailed timing and performance metrics
2. **Memory Usage Monitor**: Real-time memory usage tracking
3. **Event Tracer**: Comprehensive event logging and analysis
4. **System Health Monitor**: Component health monitoring and alerts

These tools enable users to:

- Identify performance bottlenecks
- Monitor memory usage and prevent leaks
- Debug complex simulation issues
- Validate system integration
- Optimize simulation parameters
- Generate comprehensive diagnostic reports

The profiling and debugging infrastructure is essential for developing robust, high-performance NLM applications and for troubleshooting issues during development and deployment.

---

## API Reference Summary

### Classes

- **PerformanceProfiler**: Performance measurement and analysis
- **MemoryUsageMonitor**: Memory usage tracking and monitoring
- **EventTracer**: Event logging and analysis
- **SystemHealthMonitor**: System health monitoring and alerting

### Key Methods

- **start() / stop()**: Start and stop monitoring
- **get_measurements() / get_history()**: Retrieve monitoring data
- **export_data()**: Export data to files
- **generate_report()**: Generate comprehensive reports

### Event Types

- **NEURON_SPIKE**: Neuron firing events
- **SYNAPSE_STRENGTH_CHANGE**: Synaptic weight changes
- **MEMORY_STORE**: Memory storage events
- **MEMORY_REPLAY**: Memory replay events
- **DEVELOPMENT_STAGE_CHANGE**: Developmental stage transitions
- **NEUROMODULATION_UPDATE**: Neuromodulation level changes
- **ACTION_SELECTION**: Motor action selection
- **SENSORY_INPUT**: Sensory input processing
- **EPISODE_STORAGE**: Episodic memory storage

### Health Components

- **BRAIN**: Neural core system health
- **AGENT**: Agent interface health
- **WORLD**: Environment system health
- **MEMORY**: Memory systems health
- **NEUROMODULATION**: Neuromodulation systems health
- **DEVELOPMENT**: Development systems health

---

This comprehensive profiling and debugging toolkit provides everything needed to effectively monitor, debug, and optimize NLM simulations for both development and production use cases.

---

*Documentation generated by NLM Development Team*
*For support and questions, contact: nlm-dev@nlmp.org*

---

Note: This module requires additional dependencies for full functionality:
- `psutil` for system monitoring
- `numpy` for numerical analysis
- `matplotlib` for visualization
- `pandas` for data export

Install with: `pip install psutil numpy matplotlib pandas`