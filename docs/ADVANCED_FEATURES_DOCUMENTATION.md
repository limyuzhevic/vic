# Advanced Features for Power Users
# 
# This documentation covers advanced features, plugins, and custom extensions
# for experienced NLM users and researchers.

## Table of Contents

1. [Plugin Architecture](#plugin-architecture)
2. [Custom Neural Mechanisms](#custom-neural-mechanims)
3. [Advanced Optimization](#advanced-optimization)
4. [Debugging and Profiling Tools](#debugging-and-profiling-tools)
5. [Performance Tuning](#performance-tuning)
6. [Custom World Environments](#custom-world-environments)
7. [Advanced Agent Behavior](#advanced-agent-behavior)
8. [Research Extensions](#research-extensions)
9. [Migration Guide](#migration-guide)

---

# Plugin Architecture

The enhanced NLM framework supports a comprehensive plugin architecture for custom extensions and research applications.

## Plugin System Overview

```python
# Plugin base class
class NLMPlugin:
    def __init__(self, brain, world):
        self.brain = brain
        self.world = world
    
    def initialize(self):
        """Called when plugin is initialized."""
        pass
    
    def update(self, timestep):
        """Called every simulation step."""
        pass
    
    def shutdown(self):
        """Called when plugin is shut down."""
        pass
    
    def get_name(self):
        """Return plugin name."""
        return "Base Plugin"
```

## Core Plugin Types

### 1. Sensor Plugin

```python
class CustomSensorPlugin(NLMPlugin):
    def __init__(self, brain, world, sensor_type):
        super().__init__(brain, world)
        self.sensor_type = sensor_type
        self.data_buffer = []
    
    def update(self, timestep):
        # Custom sensor implementation
        if self.sensor_type == "custom_vision":
            data = self._get_custom_vision_data()
            self.data_buffer.append(data)
            # Inject into brain
            self.brain._brain.receiveSensoryInput(data)
    
    def _get_custom_vision_data(self):
        # Custom vision sensor logic
        return np.random.rand(16, 16).astype(np.float32)
```

### 2. Learning Plugin

```python
class CustomLearningPlugin(NLMPlugin):
    def __nitialize(self):
        self.learning_rate = 0.001
        self.plasticity_rules = []
    
    def update(self, timestep):
        # Apply custom learning rules
        for neuron in self.brain._brain.get_neurons():
            self._apply_custom_plasticity(neuron, timestep)
    
    def _apply_custom_plasticity(self, neuron, timestep):
        # Custom plasticity implementation
        # This could modify synaptic weights based on custom rules
        pass
```

### 3 the Reward System Plugin

```python
class CustomRewardPlugin(NLMPlugin):
    def __init__(self, brain, world, reward_function):
        super().__init__(brain, world)
        self.reward_function = reward_function
        self.reward_history = []
    
    def update(self, timestep):
        # Calculate reward based on custom function
        reward = self.reward_function(self.brain, self.world, timestep)
        self.reward_history.append(reward)
        
        # Apply reward to agent
        if hasattr(self, 'agent'):
            self.agent.apply_reward_modulation(reward, 0.0)
```

## Plugin Management

```python
class PluginManager:
    def __init__(self):
        self.plugins = {}
    
    def register_plugin(self, plugin):
        """Register a new plugin."""
        self.plugins[plugin.get_name()] = plugin
    
    def initialize_plugins(self):
        """Initialize all registered plugins."""
        for plugin in self.plugins.values():
            plugin.initialize()
    
    def update_plugins(self, timestep):
        """Update all plugins."""
        for plugin in self.plugins.values():
            plugin.update(timestep)
    
    def shutdown_plugins(self):
        """Shutdown all plugins."""
        for plugin in self.plugins.values():
            plugin.shutdown()
    
    def get_plugin(self, name):
        """Get plugin by name."""
        return self.plugins.get(name)
    
    def remove_plugin(self, name):
        """Remove plugin by name."""
        if name in self.plugins:
            del self.plugins[name]
```

## Plugin Lifecycle

```python
# Example plugin usage
plugin_manager = PluginManager()

# Create plugins
sensor_plugin = CustomSensorPlugin(brain, world, "custom_vision")
learning_plugin = CustomLearningPlugin(brain, world)
reward_plugin = CustomRewardPlugin(brain, world, custom_reward_function)

# Register plugins
plugin_manager.register_plugin(sensor_plugin)
plugin_manager.register_plugin(learning_plugin)
plugin_manager.register_plugin(reward_plugin)

# Initialize all plugins
plugin_manager.initialize_plugins()

# Run simulation
brain.run_simulation(100, callback=plugin_manager.update_plugins)

# Cleanup
plugin_manager.shutdown_plugins()
```

---

# Custom Neural Mechanisms

## Custom Neuron Types

### 1. Adaptive Neuron

```python
class AdaptiveNeuron(NLMNeuron):
    def __init__(self, neuron_id):
        super().__init__(neuron_id)
        self.adaptation_rate = 0.01
        self.homeostatic_target = -70.0
        self.adaptation_threshold = 5.0
    
    def step(self, timestep):
        # Standard LIF dynamics
        super().step(timestep)
        
        # Adaptive mechanisms
        if self.fired_recently:
            self.adjust_homeostasis()
            self.modify_refractoriness()
        
        # Activity-dependent plasticity
        if self.get_activity_level() > self.adaptation_threshold:
            self.adapt_synaptic_strength()
    
    def adjust_homeostasis(self):
        # Homeostatic regulation
        self.resting_potential += self.adaptation_rate * \
            (self.homeostatic_target - self.resting_potential)
    
    def adapt_synaptic_strength(self):
        # Activity-dependent synaptic scaling
        for synapse in self.get_outgoing_synapses():
            synapse.adapt_strength(self.adaptation_rate)
```

### 2. Bursting Neuron

```python
class BurstingNeuron(NLMNeuron):
    def __init__(self, neuron_id):
        super().__init__(neuron_id)
        self.burst_threshold = 2.0
        self.burst_duration = 10
        self.current_burst_step = 0
        self.in_burst = False
    
    def step(self, timestep):
        super().step(timestep)
        
        if self.should_start_burst():
            self.start_burst()
        elif self.in_burst:
            self.maintain_burst(timestep)
    
    def should_start_burst(self):
        return (self.get_membrane_potential() > self.burst_threshold and 
                not self.in_burst and self.get_activity_level() > 2.0)
    
    def start_burst(self):
        self.in_burst = True
        self.current_burst_step = 0
        # Increase firing rate during burst
        self.set_refractory_period(1)  # Very short refractory
    
    def maintain_burst(self, timestep):
        self.current_burst_step += 1
        if self.current_burst_step >= self.burst_duration:
            self.in_burst = False
```

### 3. Plastic Synapse

```python
class PlasticSynapse(NLMSynapse):
    def __init__(self, synapse_id, source, destination):
        super().__init__(synapse_id, source, destination)
        self.long_term_plasticity = 0.0
        self.short_term_facilitation = 1.0
        self.short_term_depression = 1.0
        self.plasticity_decay_rate = 0.01
    
    def adapt_strength(self, rate, context=None):
        # Context-dependent plasticity
        if context == "learning":
            self.long_term_plasticity += rate * self.short_term_facilitation
        elif context == "forgetting":
            self.long_term_plasticity -= rate * self.short_term_depression
        
        # Constrain weight
        self.set_weight(self.constrain_weight(self.long_term_plasticity))
    
    def decay_plasticity(self):
        # Decay plasticity over time
        self.long_term_plasticity *= (1.0 - self.plasticity_decay_rate)
    
    def constrain_weight(self, weight):
        # Constrain weight to biologically plausible range
        return max(-1.0, min(1.0, weight))
```

## Custom Brain Regions

### 1. Hierarchical Neural Network

```python
class HierarchicalBrain(NLMBrain):
    def __init__(self, config):
        super().__init__(config)
        self.layers = {}
        self.connections = {}
    
    def add_layer(self, layer_id, neuron_count, layer_type="dense"):
        # Create neural population
        layer = NeuralPopulation(layer_id, neuron_count, layer_type)
        
        # Add to brain
        self.add_region(layer_id)
        
        # Store layer
        self.layers[layer_id] = layer
        
        # Create connections
        self._create_layer_connections(layer_id)
    
    def _create_layer_connections(self, layer_id):
        # Create feedforward connections
        if layer_id in self.layers:
            layer = self.layers[layer_id]
            
            for target_layer in self._get_next_layers(layer_id):
                self._create_feedforward_connections(layer, self.layers[target_layer])
    
    def _get_next_layers(self, layer_id):
        # Get layers that come after this one
        layer_ids = list(self.layers.keys())
        idx = layer_ids.index(layer_id)
        return layer_ids[idx + 1:] if idx + 1 < len(layer_ids) else []
    
    def _create_feedforward_connections(self, source_layer, target_layer):
        # Create random feedforward connections
        connection_probability = 0.1
        
        for source_neuron in source_layer.get_neurons():
            for target_neuron in target_layer.get_neurons():
                if np.random.random() < connection_probability:
                    # Create synapse with custom properties
                    synapse = PlasticSynapse(
                        len(self.connections),
                        source_neuron.get_id(),
                        target_neuron.get_id()
                    )
                    self.add_synapse(synapse)
                    self.connections[synapse.get_id()] = synapse
```

### 2. Sparse Connectivity

```python
class SparseConnectivityBrain(NLMBrain):
    def __init__(self, config, connection_probability=0.05):
        super().__init__(config)
        self.connection_probability = connection_probability
    
    def initialize(self):
        super().initialize()
        
        # Create sparse connections
        self.create_sparse_connections()
    
    def create_sparse_connections(self):
        # Get all neuron pairs
        all_neurons = []
        for region in self.get_regions():
            for population in region.get_populations():
                all_neurons.extend(population.get_neuron_ids())
        
        # Create random sparse connections
        for i, source_id in enumerate(all_neurons):
            for j, target_id in enumerate(all_neurons):
                if i != j and np.random.random() < self.connection_probability:
                    # Create synapse with sparse-specific properties
                    synapse = self.create_sparse_synapse(source_id, target_id)
                    self.add_synapse(synapse)
    
    def create_sparse_synapse(self, source_id, target_id):
        # Create synapse with sparse-specific properties
        return SparseSynapse(
            len(self.get_synapses()),
            source_id,
            target_id
        )
```

---

# Advanced Optimization

## 1. Memory Optimization

### Memory-Efficient Configuration

```python
class MemoryEfficientConfig:
    def __init__(self):
        self.compact_format = True
        self.lazy_loading = True
        self.compression_enabled = True
    
    def save_compact(self, brain, filepath):
        # Save brain state in compact format
        with open(filepath, 'wb') as f:
            # Write compact binary format
            self._write_compact_header(f, brain)
            self._write_neuron_data(f, brain)
            self._write_synapse_data(f, brain)
            self._write_plasticity_data(f, brain)
    
    def load_compact(self, filepath, brain):
        # Load brain state from compact format
        with open(filepath, 'rb') as f:
            self._read_compact_header(f, brain)
            self._read_neuron_data(f, brain)
            self._read_synapse_data(f, brain)
            self._read_plasticity_data(f, brain)
    
    def compress_data(self, data):
        # Compress neural data using custom algorithms
        import zlib
        return zlib.compress(data)
    
    def decompress_data(self, compressed_data):
        # Decompress neural data
        import zlib
        return zlib.decompress(compressed_data)
```

### Memory Usage Monitoring

```python
class MemoryMonitor:
    def __init__(self):
        self.memory_samples = []
        self.max_memory_usage = 0
        self.peak_memory_usage = 0
    
    def sample_memory_usage(self):
        # Get current memory usage
        import psutil
        process = psutil.Process()
        memory_info = process.memory_info()
        
        # Record usage
        usage = memory_info.rss  # Resident Set Size
        self.memory_samples.append(usage)
        
        # Update peak
        if usage > self.peak_memory_usage:
            self.peak_memory_usage = usage
        
        # Update max for current window
        if len(self.memory_samples) > 100:
            self.max_memory_usage = max(self.memory_samples[-100:])
    
    def get_memory_stats(self):
        return {
            'current_usage': self.memory_samples[-1] if self.memory_samples else 0,
            'peak_usage': self.peak_memory_usage,
            'average_usage': np.mean(self.memory_samples) if self.memory_samples else 0,
            'max_window_usage': self.max_memory_usage,
            'usage_samples': len(self.memory_samples)
        }
```

## 2. CPU Optimization

### Thread-Safe Neural Processing

```python
import threading
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor

class ParallelNeuralProcessor:
    def __init__(self, num_threads=4):
        self.num_threads = num_threads
        self.thread_pool = ThreadPoolExecutor(max_workers=num_threads)
        self.lock = threading.Lock()
    
    def process_neurons_parallel(self, neurons, timestep):
        # Process neurons in parallel
        futures = []
        
        for neuron in neurons:
            future = self.thread_pool.submit(self._process_single_neuron, neuron, timestep)
            futures.append(future)
        
        # Wait for all to complete
        results = [future.result() for future in futures]
        
        return results
    
    def _process_single_neuron(self, neuron, timestep):
        # Process single neuron with thread safety
        with self.lock:
            # Neuron processing logic
            return self._compute_neuron_output(neuron, timestep)
    
    def process_synapses_parallel(self, synapses, timestep):
        # Process synapses in parallel
        with ThreadPoolExecutor(max_workers=self.num_threads) as executor:
            futures = [executor.submit(self._process_single_synapse, syn, timestep) 
                      for syn in synapses]
            
            return [future.result() for future in futures]
    
    def _process_single_synapse(self, synapse, timestep):
        # Process single synapse with thread safety
        with self.lock:
            # Synapse processing logic
            return self._compute_synapse_output(synapse, timestep)
```

### SIMD Optimization

```python
import numpy as np
import struct

class SIMDNeuralProcessor:
    def __init__(self):
        self.vectorized_operations = True
    
    def vectorized_membrane_potential_update(self, neurons, weights, input_currents):
        # Vectorized membrane potential update
        if self.vectorized_operations:
            # Use numpy for vectorized operations
            V = np.array([n.get_membrane_potential() for n in neurons])
            I_syn = np.array([syn.get_current() for syn in self.get_all_synapses()])
            
            # LIF update formula
            dV = (1.0 / 20.0) * (V - (-70.0)) + I_syn / 1.0
            V += dV * 0.001
            
            # Update neurons
            for i, neuron in enumerate(neurons):
                neuron.set_membrane_potential(V[i])
            
            return V
        else:
            # Fallback to scalar operations
            return self.scalar_membrane_potential_update(neurons, weights, input_currents)
    
    def vectorized_spike_detection(self, neurons):
        # Vectorized spike detection
        if self.vectorized_operations:
            V = np.array([n.get_membrane_potential() for n in neurons])
            thresholds = np.array([n.get_threshold() for n in neurons])
            
            # Detect spikes
            spikes = V >= thresholds
            
            # Reset spiked neurons
            for i, neuron in enumerate(neurons):
                if spikes[i]:
                    neuron.set_membrane_potential(neuron.get_reset_potential())
            
            return spikes
        else:
            # Fallback to scalar operations
            return self.scalar_spike_detection(neurons)
```

---

# Debugging and Profiling Tools

## 1. Brain State Debugger

### State Inspector

```python
class BrainStateInspector:
    def __init__(self, brain):
        self.brain = brain
    
    def inspect_neuron(self, neuron_id):
        """Inspect specific neuron state."""
        neuron = self.brain.get_neuron(neuron_id)
        if neuron is None:
            print(f"Neuron {neuron_id} not found")
            return
        
        print(f"=== Neuron {neuron_id} ===")
        print(f"  Type: {neuron.get_type()}")
        print(f"  Membrane Potential: {neuron.get_membrane_potential()} mV")
        print(f"  Resting Potential: {neuron.get_rest_potential()} mV")
        print(f"  Threshold: {neuron.get_threshold()} mV")
        print(f"  Reset Potential: {neuron.get_reset_potential()} mV")
        print(f"  Refractory Remaining: {neuron.get_refractory_remaining()}")
        print(f"  Firing State: {neuron.get_firing_state()}")
        print(f"  Firing Rate: {neuron.get_firing_rate()} Hz")
        print(f"  Incoming Synapses: {len(neuron.get_incoming_synapses())}")
        print(f"  Outgoing Synapses: {len(neuron.get_outgoing_synapses())}")
    
    def inspect_synapse(self, synapse_id):
        """Inspect specific synapse state."""
        synapse = self.brain.get_synapse(synapse_id)
        if synapse is None:
            print(f"Synapse {synapse_id} not found")
            return
        
        print(f"=== Synapse {synapse_id} ===")
        print(f"  Source: {synapse.get_source_neuron()}")
        print(f"  Target: {synapse.get_target_neuron()}")
        print(f"  Weight: {synapse.get_weight()}")
        print(f"  Delay: {synapse.get_delay()}")
        print(f"  Type: {synapse.get_type()}")
        print(f"  Plasticity Flags: {synapse.get_plasticity_flags()}")
    
    def inspect_region(self, region_id):
        """Inspect brain region."""
        region = self.brain.get_region(region_id)
        if region is None:
            print(f"Region {region_id} not found")
            return
        
        print(f"=== Region {region_id} ===")
        print(f"  Population count: {region.get_population_count()}")
        print(f"  Total neurons: {region.get_total_neuron_count()}")
        print(f"  Total synapses: {region.get_total_synapse_count()}")
    
    def print_brain_summary(self):
        """Print complete brain summary."""
        print("=== Brain State Summary ===")
        print(f"  Regions: {self.brain.get_region_count()}")
        print(f"  Total Neurons: {self.brain.get_total_neuron_count()}")
        print(f"  Total Synapses: {self.brain.get_total_synapse_count()}")
        print(f"  Firing Neurons: {self.brain.get_firing_neuron_count()}")
        print(f"  Total Spikes: {self.brain.get_total_spike_count()}")
        print(f"  Development Stage: {self.brain.get_developmental_stage()}")
        print(f"  Average Firing Rate: {self.brain.get_average_firing_rate():.2f} Hz")
        print(f"  Excitation/Inhibition Ratio: {self.brain.get_excitation_inhibition_ratio():.3f}")
```

### Event Tracer

```python
class BrainEventTracer:
    def __init__(self, brain, max_events=1000):
        self.brain = brain
        self.events = []
        self.max_events = max_events
    
    def start_tracing(self):
        """Start tracing brain events."""
        self.events = []
    
    def trace_event(self, event_type, data=None):
        """Trace an event."""
        event = {
            'type': event_type,
            'timestamp': self.brain.get_simulation_time(),
            'data': data
        }
        self.events.append(event)
        
        # Limit event storage
        if len(self.events) > self.max_events:
            self.events = self.events[-self.max_events:]
    
    def analyze_events(self, event_type):
        """Analyze events of specific type."""
        matching_events = [e for e in self.events if e['type'] == event_type]
        
        if not matching_events:
            print(f"No events of type '{event_type}' found")
            return
        
        print(f"=== Events of Type: {event_type} ===")
        print(f"Total events: {len(matching_events)}")
        
        # Time analysis
        timestamps = [e['timestamp'] for e in matching_events]
        if len(timestamps) > 1:
            intervals = np.diff(timestamps)
            print(f"Time between events: mean={np.mean(intervals):.3f}, std={np.std(intervals):.3f}")
        
        # Data analysis
        for i, event in enumerate(matching_events[:10]):  # Show first 10 events
            print(f"Event {i+1}: {event}")
        
        if len(matching_events) > 10:
            print(f"... and {len(matching_events) - 10} more events")
    
    def save_events(self, filepath):
        """Save traced events to file."""
        import json
        with open(filepath, 'w') as f:
            json.dump(self.events, f, indent=2)
    
    def load_events(self, filepath):
        """Load traced events from file."""
        import json
        with open(filepath, 'r') as f:
            self.events = json.load(f)
```

## 2. Performance Profiler

### Timing Profiler

```python
class BrainTimingProfiler:
    def __init__(self):
        self.timings = {}
        self.start_times = {}
    
    def start_timing(self, operation_name):
        """Start timing an operation."""
        import time
        self.start_times[operation_name] = time.time()
    
    def stop_timing(self, operation_name):
        """Stop timing an operation and record result."""
        if operation_name in self.start_times:
            import time
            end_time = time.time()
            duration = end_time - self.start_times[operation_name]
            
            if operation_name not in self.timings:
                self.timings[operation_name] = []
            
            self.timings[operation_name].append(duration)
            
            del self.start_times[operation_name]
        else:
            print(f"Warning: No start time recorded for '{operation_name}'")
    
    def get_statistics(self, operation_name):
        """Get timing statistics for an operation."""
        if operation_name not in self.timings:
            return None
        
        times = self.timings[operation_name]
        return {
            'count': len(times),
            'mean': np.mean(times),
            'std': np.std(times),
            'min': np.min(times),
            'max': np.max(times),
            'median': np.median(times)
        }
    
    def print_summary(self):
        """Print timing summary."""
        print("=== Brain Timing Profile ===")
        for operation_name, times in self.timings.items():
            stats = self.get_statistics(operation_name)
            if stats:
                print(f"{operation_name}:")
                print(f"  Count: {stats['count']}")
                print(f"  Mean: {stats['mean']:.6f}s")
                print(f"  Std: {stats['std']:.6f}s")
                print(f"  Min: {stats['min']:.6f}s")
                print(f"  Max: {stats['max']:.6f}s")
                print()
    
    def reset(self):
        """Reset timing data."""
        self.timings = {}
        self.start_times = {}
```

### Memory Profiler

```python
class BrainMemoryProfiler:
    def __init__(self):
        self.memory_samples = []
        self.memory_peaks = {}
    
    def sample_memory_usage(self, operation_name=None):
        """Sample current memory usage."""
        try:
            import psutil
            process = psutil.Process()
            memory_info = process.memory_info()
            
            usage = memory_info.rss  # Bytes
            sample = {
                'timestamp': time.time(),
                'usage': usage,
                'operation': operation_name,
                'vms': memory_info.vms,
                'rss': memory_info.rss,
                'shared': memory_info.shared,
                'text': memory_info.text,
                'data': memory_info.data,
                'dirty': memory_info.dirty
            }
            
            self.memory_samples.append(sample)
            
            # Update peak memory for operation
            if operation_name:
                if operation_name not in self.memory_peaks:
                    self.memory_peaks[operation_name] = usage
                else:
                    self.memory_peaks[operation_name] = max(self.memory_peaks[operation_name], usage)
        
        except ImportError:
            print("psutil not available for memory profiling")
    
    def get_memory_statistics(self, operation_name=None):
        """Get memory usage statistics."""
        if operation_name:
            samples = [s for s in self.memory_samples if s['operation'] == operation_name]
        else:
            samples = self.memory_samples
        
        if not samples:
            return None
        
        usages = [s['usage'] for s in samples]
        
        return {
            'count': len(samples),
            'mean_usage': np.mean(usages),
            'std_usage': np.std(usages),
            'min_usage': np.min(usages),
            'max_usage': np.max(usages),
            'peak_usage': max(usages),
            'total_samples': len(self.memory_samples)
        }
    
    def generate_memory_report(self):
        """Generate memory usage report."""
        print("=== Brain Memory Profile ===")
        
        if operation_name:
            print(f"For operation: {operation_name}")
            stats = self.get_memory_statistics(operation_name)
        else:
            print("For all operations:")
            stats = self.get_memory_statistics()
        
        if stats:
            print(f"Total samples: {stats['count']}")
            print(f"Average memory usage: {stats['mean_usage']/1024/1024:.2f} MB")
            print(f"Peak memory usage: {stats['peak_usage']/1024/1024:.2f} MB")
            print(f"Memory usage range: {stats['min_usage']/1024/1024:.2f} - {stats['max_usage']/1024/1024:.2f} MB")
    
    def create_memory_graph(self, output_file=None):
        """Create memory usage graph."""
        try:
            import matplotlib.pyplot as plt
            
            timestamps = [s['timestamp'] for s in self.memory_samples]
            usages = [s['usage'] for s in self.memory_samples]
            
            plt.figure(figsize=(12, 6))
            plt.plot(timestamps, np.array(usages)/1024/1024, 'b-')
            plt.xlabel('Time (seconds)')
            plt.ylabel('Memory Usage (MB)')
            plt.title('Brain Memory Usage Over Time')
            plt.grid(True)
            
            if output_file:
                plt.savefig(output_file)
                print(f"Memory graph saved to: {output_file}")
            else:
                plt.show()
                
        except ImportError:
            print("matplotlib not available for memory graphing")
```

---

# Performance Tuning

## 1. Configuration Optimization

### Optimal Configuration Generator

```python
class ConfigurationOptimizer:
    def __init__(self):
        self.optimization_strategies = []
    
    def add_strategy(self, strategy):
        """Add optimization strategy."""
        self.optimization_strategies.append(strategy)
    
    def optimize_for_performance(self, target_performance, resource_constraints):
        """Optimize configuration for target performance."""
        optimal_config = {}
        
        # Neural size optimization
        if target_performance.get("memory_footprint"):
            optimal_config['brain.neuron_count'] = self._optimize_neuron_count(
                target_performance["memory_footprint"],
                resource_constraints
            )
        
        # Time optimization
        if target_performance.get("simulation_speed"):
            optimal_config['simulation.timestep'] = self._optimize_timestep(
                target_performance["simulation_speed"],
                resource_constraints
            )
        
        # Learning optimization
        if target_performance.get("learning_efficiency"):
            optimal_config.update(self._optimize_learning_config(
                target_performance["learning_efficiency"],
                resource_constraints
            ))
        
        return optimal_config
    
    def _optimize_neuron_count(self, target_memory, constraints):
        # Calculate optimal neuron count based on memory constraints
        # Rule of thumb: ~1KB per neuron
        max_neurons = int(target_memory * 1024 / 1.0)
        
        # Apply constraints
        max_neurons = min(max_neurons, 100000)  # Maximum neurons
        max_neurons = max(max_neurons, 100)   # Minimum neurons
        
        # Choose power of 2 for efficiency
        optimal = 2 ** int(np.log2(max_neurons))
        
        return optimal
    
    def _optimize_timestep(self, target_speed, constraints):
        # Optimize timestep for target simulation speed
        base_timestep = 0.001
        target_steps = target_speed.get("steps_per_second", 1000)
        
        # Smaller timesteps increase accuracy but decrease speed
        if target_steps > 10000:
            return 0.0005  # Faster simulation
        elif target_steps > 1000:
            return 0.001  # Balanced
        else:
            return 0.002  # Slower simulation
    
    def _optimize_learning_config(self, target_efficiency, constraints):
        # Optimize learning parameters
        if constraints.get("power_efficiency"):
            return {
                'plasticity.stdp.learning_rate': 0.001,
                'plasticity.hebbian.learning_rate': 0.01,
                'neuromod.dopamine.scale': 0.5
            }
        else:
            return {
                'plasticity.stdp.learning_rate': 0.01,
                'plasticity.hebbian.learning_rate': 0.1,
                'neuromod.dopamine.scale': 1.0
            }
    
    def benchmark_configurations(self, configurations, benchmark_function):
        """Benchmark multiple configurations."""
        results = {}
        
        for config_name, config_params in configurations.items():
            print(f"Benchmarking {config_name}...")
            
            # Create brain with configuration
            brain = EnhancedBrain(config_params)
            brain.initialize()
            
            # Run benchmark
            start_time = time.time()
            brain.run_simulation(100, verbose=False)
            end_time = time.time()
            
            # Record results
            duration = end_time - start_time
            results[config_name] = {
                'time': duration,
                'steps_per_second': 100 / duration,
                'neurons_per_second': (config_params.get('brain.neuron_count', 1000) * 100) / duration
            }
        
        return results
```

### Performance Comparison Tool

```python
class PerformanceComparator:
    def __init__(self):
        self.baseline_performance = None
        self.comparison_results = {}
    
    def set_baseline(self, config_name, performance_metrics):
        """Set baseline performance metrics."""
        self.baseline_performance = {
            'name': config_name,
            'metrics': performance_metrics,
            'timestamp': time.time()
        }
    
    def compare_with_baseline(self, config_name, performance_metrics):
        """Compare current performance with baseline."""
        if self.baseline_performance is None:
            print("No baseline set. Set baseline first.")
            return
        
        comparison = {
            'config_name': config_name,
            'baseline_name': self.baseline_performance['name'],
            'improvement': {},
            'regression': {}
        }
        
        # Compare each metric
        for metric_name in self.baseline_performance['metrics']:
            if metric_name in performance_metrics:
                baseline_value = self.baseline_performance['metrics'][metric_name]
                current_value = performance_metrics[metric_name]
                
                # Calculate improvement/regression
                if baseline_value != 0:
                    improvement = (current_value - baseline_value) / baseline_value * 100
                    if improvement > 0:
                        comparison['improvement'][metric_name] = improvement
                    else:
                        comparison['regression'][-metric_name[1:]] = -improvement
                else:
                    comparison['improvement'][metric_name] = float('inf') if current_value > 0 else 0
        
        self.comparison_results[config_name] = comparison
        
        # Print comparison results
        print(f"\n=== Performance Comparison: {config_name} vs {self.baseline_performance['name']} ===")
        if comparison['improvement']:
            print("Improvements:")
            for metric, improvement in comparison['improvement'].items():
                print(f"  {metric}: {improvement:.1f}%")
        
        if comparison['regression']:
            print("Regressions:")
            for metric, regression in comparison['regression'].items():
                print(f"  {metric}: {regression:.1f}%")
    
    def generate_performance_report(self):
        """Generate comprehensive performance report."""
        print("\n" + "="*60)
        print("PERFORMANCE ANALYSIS REPORT")
        print("="*60)
        
        if self.baseline_performance:
            print(f"Baseline: {self.baseline_performance['name']} (at {self.baseline_performance['timestamp']})")
        
        for config_name, comparison in self.comparison_results.items():
            print(f"\n{config_name}:")
            if comparison['improvement']:
                print("  Improvements:")
                for metric, improvement in comparison['improvement'].items():
                    print(f"    {metric}: {improvement:.1f}%")
            
            if comparison['regression']:
                print("  Regressions:")
                for metric, regression in comparison['regression'].items():
                    print(f"    {metric}: {regression:.1f}%")
    
    def save_report(self, filepath):
        """Save performance report to file."""
        with open(filepath, 'w') as f:
            f.write("Performance Analysis Report\n")
            f.write("="*60 + "\n")
            
            if self.baseline_performance:
                f.write(f"Baseline: {self.baseline_performance['name']}\n")
            
            for config_name, comparison in self.comparison_results.items():
                f.write(f"\n{config_name}:\n")
                
                if comparison['improvement']:
                    f.write("  Improvements:\n")
                    for metric, improvement in comparison['improvement'].items():
                        f.write(f"    {metric}: {improvement:.1f}%\n")
                
                if comparison['regression']:
                    f.write("  Regressions:\n")
                    for metric, regression in comparison['regression'].items():
                        f.write(f"    {metric}: {regression:.1f}%\n")
```

## 2. Resource Management

### Memory Pool Manager

```python
class MemoryPoolManager:
    def __init__(self, total_memory_mb=1024):
        self.total_memory = total_memory_mb * 1024 * 1024  # Bytes
        self.available_memory = self.total_memory
        self.memory_pools = {}
    
    def allocate_memory(self, size_mb, pool_name="default"):
        """Allocate memory from pool."""
        required_memory = size_mb * 1024 * 1024
        
        if required_memory > self.available_memory:
            raise MemoryError(f"Not enough memory available: need {size_mb}MB, have {self.available_memory/1024/1024:.1f}MB")
        
        self.available_memory -= required_memory
        
        if pool_name not in self.memory_pools:
            self.memory_pools[pool_name] = []
        
        pool_info = {
            'size': required_memory,
            'address': id(self),  # Simplified
            'timestamp': time.time()
        }
        
        self.memory_pools[pool_name].append(pool_info)
        
        return pool_info
    
    def deallocate_memory(self, pool_name, memory_id):
        """Deallocate memory from pool."""
        if pool_name in self.memory_pools:
            for allocation in self.memory_pools[pool_name]:
                if allocation['address'] == memory_id:
                    self.available_memory += allocation['size']
                    self.memory_pools[pool_name].remove(allocation)
                    return True
        
        return False
    
    def get_memory_status(self):
        """Get memory pool status."""
        return {
            'total_memory': self.total_memory,
            'available_memory': self.available_memory,
            'used_memory': self.total_memory - self.available_memory,
            'pools': {name: len(pool) for name, pool in self.memory_pools.items()}
        }
```

### CPU Scheduler

```python
class NeuralCPUScheduler:
    def __init__(self, num_cores=4):
        self.num_cores = num_cores
        self.core_load = [0.0] * num_cores
        self.current_task = None
        self.current_core = -1
    
    def submit_task(self, task, estimated_cores=1):
        """Submit task for execution."""
        # Find core with lowest load
        min_load_core = min(range(self.num_cores), key=lambda i: self.core_load[i])
        
        if self.core_load[min_load_core] + estimated_cores <= 1.0:
            # Assign task
            self.current_task = task
            self.current_core = min_load_core
            self.core_load[min_load_core] += estimated_cores
            
            # Execute task
            return self._execute_task(task, estimated_cores)
        else:
            # Task too large for any core
            raise ResourceError(f"Task requires {estimated_cores} cores but max available is {self.num_cores}")
    
    def _execute_task(self, task, cores):
        """Execute task on assigned core."""
        # Simulate task execution
        import time
        time.sleep(0.001 * cores)  # Simplified execution time
        
        # Update core load
        self.core_load[self.current_core] -= cores
        self.current_core = -1
        self.current_task = None
        
        return f"Task executed on core {self.current_core}"
    
    def get_load_balance(self):
        """Get current CPU load balance."""
        if not self.core_load:
            return 0.0
        
        max_load = max(self.core_load)
        min_load = min(self.core_load)
        
        # Balance score (0-1, where 1 is perfectly balanced)
        balance_score = 1.0 - (max_load - min_load)
        
        return balance_score
```

---

# Custom World Environments

## 1. Complex World Generator

```python
class ComplexWorldGenerator:
    def __init__(self, width, height):
        self.width = width
        self.height = height
        self.world = EnhancedWorld(width, height)
    
    def create_random_environment(self, resource_density=0.3, hazard_density=0.1):
        """Create random environment with resources and hazards."""
        total_cells = self.width * self.height
        
        # Add resources randomly
        resource_count = int(total_cells * resource_density)
        resource_positions = self._select_random_positions(resource_count)
        
        for x, y in resource_positions:
            value = np.random.uniform(0.5, 3.0)
            self.world.add_resource(x, y, value)
        
        # Add hazards randomly
        hazard_count = int(total_cells * hazard_density)
        hazard_positions = self._select_random_positions(hazard_count)
        
        for x, y in hazard_positions:
            value = np.random.uniform(-1.0, -0.2)
            self.world.add_hazard(x, y, value)
        
        return self.world
    
    def create_structured_environment(self, structure_type="grid"):
        """Create structured environment with specific patterns."""
        if structure_type == "grid":
            self._create_grid_environment()
        elif structure_type == "clustered":
            self._create_clustered_environment()
        elif structure_type == "corridor":
            self._create_corridor_environment()
        elif structure_type == "maze":
            self._create_maze_environment()
        
        return self.world
    
    def _create_grid_environment(self):
        """Create grid-like environment with resources at intersections."""
        grid_size = 5
        cell_size = self.width // grid_size
        
        for row in range(grid_size):
            for col in range(grid_size):
                x = col * cell_size + cell_size // 2
                y = row * cell_size + cell_size // 2
                
                if (row + col) % 2 == 0:  # Alternating pattern
                    self.world.add_resource(x, y, 2.0)
                else:
                    self.world.add_hazard(x, y, -0.5)
    
    def _create_clustered_environment(self):
        """Create clustered resources and hazards."""
        # Create resource clusters
        for _ in range(5):
            cluster_x = np.random.randint(5, self.width - 5)
            cluster_y = np.random.randint(5, self.height - 5)
            
            # Create resource cluster
            for dx in range(-3, 4):
                for dy in range(-3, 4):
                    if np.random.random() < 0.3:  # Cluster density
                        x = cluster_x + dx
                        y = cluster_y + dy
                        if 0 <= x < self.width and 0 <= y < self.height:
                            self.world.add_resource(x, y, np.random.uniform(0.5, 2.0))
            
            # Create hazard cluster
            hazard_x = np.random.randint(5, self.width - 5)
            hazard_y = np.random.randint(5, self.height - 5)
            
            for dx in range(-2, 3):
                for dy in range(-2, 3):
                    if np.random.random() < 0.4:
                        x = hazard_x + dx
                        y = hazard_y + dy
                        if 0 <= x < self.width and 0 <= y < self.height:
                            self.world.add_hazard(x, y, np.random.uniform(-0.5, -0.1))
    
    def _create_corridor_environment(self):
        """Create corridor-like environment."""
        # Create horizontal corridors
        for y in [5, 10, 15, 20, 25]:
            for x in range(5, self.width - 5):
                if np.random.random() < 0.7:  # Corridor density
                    self.world.add_resource(x, y, 1.0)
        
        # Create vertical corridors
        for x in [5, 10, 15, 20, 25]:
            for y in range(5, self.height - 5):
                if np.random.random() < 0.7:  # Corridor density
                    self.world.add_resource(x, y, 1.0)
    
    def _create_maze_environment(self):
        """Create maze-like environment."""
        # Create maze walls
        wall_positions = set()
        
        # Generate random maze
        for x in range(1, self.width - 1):
            for y in range(1, self.height - 1):
                if np.random.random() < 0.3:  # Wall probability
                    wall_positions.add((x, y))
        
        # Create open spaces for movement
        for x in range(1, self.width - 1):
            for y in range(1, self.height - 1):
                if (x, y) not in wall_positions and np.random.random() < 0.8:
                    self.world.add_resource(x, y, 1.0)
        
        # Add hazards in open areas
        for x in range(1, self.width - 1):
            for y in range(1, self.height - 1):
                if (x, y) not in wall_positions and np.random.random() < 0.1:
                    self.world.add_hazard(x, y, -0.3)
    
    def _select_random_positions(self, count):
        """Select random positions."""
        positions = set()
        while len(positions) < count:
            x = np.random.randint(0, self.width)
            y = np.random.randint(0, self.height)
            positions.add((x, y))
        
        return list(positions)
    
    def save_world_config(self, filepath):
        """Save world configuration to file."""
        import json
        
        config = {
            'width': self.width,
            'height': self.height,
            'resources': [],
            'hazards': []
        }
        
        # Extract resource and hazard information
        # This would require additional API to query world contents
        
        with open(filepath, 'w') as f:
            json.dump(config, f, indent=2)
    
    def load_world_config(self, filepath):
        """Load world configuration from file."""
        import json
        
        with open(filepath, 'r') as f:
            config = json.load(f)
        
        # Create new world with loaded configuration
        new_world = ComplexWorldGenerator(config['width'], config['height']).world
        
        return new_world
```

## 2. Dynamic World Evolution

```python
class EvolvingWorld:
    def __init__(self, initial_world):
        self.world = initial_world
        self.evolution_rules = []
        self.current_step = 0
    
    def add_evolution_rule(self, rule):
        """Add evolution rule."""
        self.evolution_rules.append(rule)
    
    def evolve(self, timestep=0.1):
        """Evolve world according to rules."""
        self.current_step += 1
        
        # Apply evolution rules
        for rule in self.evolution_rules:
            rule.apply(self.world, self.current_step)
        
        # Update world state
        self.world.update(timestep)
    
    def add_dynamic_resource(self, x, y, growth_rate=0.01, decay_rate=0.001):
        """Add resource that grows and decays dynamically."""
        class ResourceEvolutionRule:
            def __init__(self, x, y, growth_rate, decay_rate):
                self.x = x
                self.y = y
                self.growth_rate = growth_rate
                self.decay_rate = decay_rate
                self.value = 1.0
            
            def apply(self, world, step):
                # Calculate net change
                growth = self.growth_rate * step
                decay = self.decay_rate * step
                self.value += growth - decay
                
                # Keep value within reasonable bounds
                self.value = max(0.1, min(10.0, self.value))
                
                # Update world with new value
                # This would require API to update existing resources
                pass
        
        rule = ResourceEvolutionRule(x, y, growth_rate, decay_rate)
        self.add_evolution_rule(rule)
    
    def add_dynamic_hazard(self, x, y, spread_rate=0.005, decay_rate=0.002):
        """Add hazard that spreads and decays dynamically."""
        class HazardEvolutionRule:
            def __init__(self, x, y, spread_rate, decay_rate):
                self.x = x
                self.y = y
                self.spread_rate = spread_rate
                self.decay_rate = decay_rate
                self.intensity = 0.5
            
            def apply(self, world, step):
                # Spread hazard to nearby cells
                for dx in [-1, 0, 1]:
                    for dy in [-1, 0, 1]:
                        nx, ny = self.x + dx, self.y + dy
                        if 0 <= nx < world.get_width() and 0 <= ny < world.get_height():
                            if np.random.random() < self.spread_rate:
                                # Add hazard to neighboring cell
                                hazard_intensity = self.intensity * self.decay_rate * step
                                world.add_hazard(nx, ny, -hazard_intensity)
        
        rule = HazardEvolutionRule(x, y, spread_rate, decay_rate)
        self.add_evolution_rule(rule)
    
    def run_evolving_simulation(self, steps, brain=None, agent=None):
        """Run simulation with evolving world."""
        for step in range(steps):
            self.evolve(0.1)
            
            # Run brain/world interaction if provided
            if brain and agent:
                # This would require implementing agent-world interaction
                # with the evolving world
                pass
    
    def get_world_snapshot(self):
        """Get current world state snapshot."""
        return {
            'step': self.current_step,
            'width': self.world.get_width(),
            'height': self.world.get_height(),
            'timestamp': time.time()
        }
```

---

# Advanced Agent Behavior

## 1. Custom Agent Architecture

```python
class AdvancedAgent:
    def __init__(self, brain, world):
        self.brain = brain
        self.world = world
        self.behavior_strategies = {}
        self.emotional_state = {
            'curiosity': 0.0,
            'novelty': 0.0,
            'reward_prediction_error': 0.0,
            'neuromodulation': 0.0
        }
        self.action_history = []
        self.experience_memory = []
    
    def add_behavior_strategy(self, name, strategy):
        """Add behavior strategy."""
        self.behavior_strategies[name] = strategy
    
    def update_emotional_state(self, timestep):
        """Update agent's emotional state."""
        # Update based on brain state
        self.emotional_state['curiosity'] = self.brain.get_curiosity_level()
        self.emotional_state['novelty'] = self.brain.get_novelty_level()
        self.emotional_state['reward_prediction_error'] = self.brain.get_prediction_error()
        self.emotional_state['neuromodulation'] = self.brain.get_neuromodulation_level()
    
    def select_behavior(self, available_strategies):
        """Select behavior based on emotional state and context."""
        # Implement strategy selection logic
        selected_strategy = None
        max_score = -1
        
        for name, strategy in self.behavior_strategies.items():
            if available_strategies and name not in available_strategies:
                continue
            
            # Calculate strategy score based on emotional state
            score = self._calculate_strategy_score(strategy, name)
            
            if score > max_score:
                max_score = score
                selected_strategy = strategy
        
        return selected_strategy
    
    def _calculate_strategy_score(self, strategy, name):
        """Calculate score for a strategy based on emotional state."""
        base_score = 0.0
        
        # Curiosity-based exploration
        if 'curiosity' in strategy.get('triggers', []):
            base_score += self.emotional_state['curiosity'] * strategy.get('curiosity_weight', 1.0)
        
        # Novelty-based exploration
        if 'novelty' in strategy.get('triggers', []):
            base_score += self.emotional_state['novelty'] * strategy.get('novelty_weight', 1.0)
        
        # Reward-based approach
        if 'reward' in strategy.get('triggers', []):
            base_score += max(0, self.emotional_state['reward_prediction_error']) * strategy.get('reward_weight', 1.0)
        
        # Avoid high prediction errors
        if 'avoid_error' in strategy.get('triggers', []):
            base_score += max(0, 1.0 - abs(self.emotional_state['reward_prediction_error'])) * strategy.get('error_weight', 1.0)
        
        return base_score
    
    def execute_strategy(self, strategy, percept):
        """Execute selected strategy."""
        if not strategy:
            return None
        
        # Execute strategy actions
        actions = strategy.get('actions', [])
        
        for action in actions:
            action_type = action.get('type')
            action_params = action.get('params', {})
            
            if action_type == 'sensory_processing':
                self._process_sensory(action_params, percept)
            elif action_type == 'motor_control':
                self._control_motor(action_params)
            elif action_type == 'learning':
                self._initiate_learning(action_params)
            elif action_type == 'memory':
                self._update_memory(action_params)
            elif action_type == 'social':
                self._engage_social(action_params)
            elif action_type == 'exploration':
                self._initiate_exploration(action_params)
        
        # Record action
        self.action_history.append({
            'timestamp': time.time(),
            'strategy': strategy.get('name', 'unknown'),
            'actions': actions,
            'emotional_state': self.emotional_state.copy()
        })
    
    def _process_sensory(self, params, percept):
        """Process sensory input with custom strategy."""
        # Custom sensory processing logic
        pass
    
    def _control_motor(self, params):
        """Control motor output with custom strategy."""
        # Custom motor control logic
        pass
    
    def _initiate_learning(self, params):
        """Initiate learning with custom parameters."""
        # Custom learning logic
        pass
    
    def _update_memory(self, params):
        """Update memory with custom strategy."""
        # Custom memory logic
        pass
    
    def _engage_social(self, params):
        """Engage social behavior with custom strategy."""
        # Custom social logic
        pass
    
    def _initiate_exploration(self, params):
        """Initiate exploration with custom strategy."""
        # Custom exploration logic
        pass
```

## 2. Reinforcement Learning Integration

```python
def custom_reward_function(brain, world, timestep):
    """Custom reward function for reinforcement learning."""
    # Get agent's position
    agent_body = world.get_agent_body()
    
    # Calculate reward based on agent's position and world state
    reward = 0.0
    
    # Reward for approaching resources
    resources = world.get_resources()  # This would require API
    for resource in resources:
        distance = np.sqrt((agent_body.x - resource.x)**2 + (agent_body.y - resource.y)**2)
        if distance < 5.0:  # Within reward range
            reward += 1.0 / (distance + 1.0)
    
    # Penalty for hazards
    hazards = world.get_hazards()  # This would require API
    for hazard in hazards:
        distance = np.sqrt((agent_body.x - hazard.x)**2 + (agent_body.y - hazard.y)**2)
        if distance < 3.0:  # Within hazard range
            reward -= hazard.value / (distance + 1.0)
    
    # Bonus for exploration
    curiosity = brain.get_curiosity_level()
    reward += curiosity * 0.1
    
    # Penalty for staying in same place too long
    if timestep > 10 and timestep < 50:
        reward -= 0.1
    
    return reward
class ReinforcementLearningAgent(AdvancedAgent):
    def __init__(self, brain, world, learning_rate=0.01):
        super().__init__(brain, world)
        self.learning_rate = learning_rate
        self.q_table = {}  # State-action value table
        self.epsilon = 0.1  # Exploration rate
        self.epsilon_decay = 0.995
        self.epsilon_min = 0.01
    
    def state_representation(self, percept):
        """Create state representation for Q-learning."""
        # Convert percept to state vector
        vision = world.get_vision() if hasattr(world, 'get_vision') else []
        internal = percept.get_internal() if hasattr(percept, 'get_internal') else []
        
        # Simple state representation
        state = tuple(vision + internal)
        
        if state not in self.q_table:
            self.q_table[state] = 0.0
        
        return state
    
    def choose_action(self, state, available_actions):
        """Choose action using epsilon-greedy policy."""
        if np.random.random() < self.epsilon:
            # Explore: choose random action
            return np.random.choice(available_actions)
        else:
            # Exploit: choose best known action
            state_actions = [(state, action) for action in available_actions]
            best_state_action = max(state_actions, key=lambda sa: self.q_table.get(sa[0], 0.0))
            return best_state_action[1]
    
    def learn_from_experience(self, state, action, reward, next_state):
        """Update Q-values based on experience."""
        # Q-learning update
        current_q = self.q_table.get(state, {}).get(action, 0.0)
        max_next_q = max(self.q_table.get(next_state, {}).values()) if self.q_table.get(next_state) else 0.0
        
        new_q = current_q + self.learning_rate * (reward + 0.99 * max_next_q - current_q)
        
        if state not in self.q_table:
            self.q_table[state] = {}
        
        self.q_table[state][action] = new_q
        
        # Decay exploration rate
        self.epsilon = max(self.epsilon_min, self.epsilon * self.epsilon_decay)
    
    def process_sensory_input(self, percept):
        """Process sensory input with Q-learning."""
        # Get current state
        state = self.state_representation(percept)
        
        # Choose action
        available_actions = self.get_available_actions()
        action = self.choose_action(state, available_actions)
        
        # Execute action
        self._execute_action(action)
        
        # Get next state
        next_state = self.state_representation(self.world.get_sensory_percept())
        
        # Learn from experience
        self.learn_from_experience(state, action, self.get_last_reward(), next_state)
    
    def get_available_actions(self):
        """Get available actions for the agent."""
        return [
            pynlm.MotorCommand.MoveForward,
            pynlm.MotorCommand.TurnLeft,
            pynlm.MotorCommand.TurnRight,
            pynlm.MotorCommand.Interact,
            pynlm.MotorCommand.Wait
        ]
    
    def get_last_reward(self):
        """Get last reward received."""
        # This would track rewards over time
        return 0.0
    
    def _execute_action(self, action):
        """Execute chosen action."""
        # Convert action to world command
        if action == pynlm.MotorCommand.MoveForward:
            # Move forward
            self.world.apply_motor_command(action, self.world.get_simulation_time())
        elif action == pynlm.MotorCommand.TurnLeft:
            # Turn left
            self.world.apply_motor_command(action, self.world.get_simulation_time())
        elif action == pynlm.MotorCommand.TurnRight:
            # Turn right
            self.world.apply_motor_command(action, self.world.get_simulation_time())
        elif action == pynlm.MotorCommand.Interact:
            # Interact with environment
            self.world.apply_motor_command(action, self.world.get_simulation_time())
        # Wait action requires no command
```

---

# Research Extensions

## 1. Custom Neural Network Architectures

```python
class CustomNeuralNetwork:
    def __init__(self, architecture, input_size, output_size):
        self.architecture = architecture
        self.layers = self._build_layers(architecture, input_size, output_size)
        self.weights = self._initialize_weights(architecture)
        self.biases = self._initialize_biases(architecture)
    
    def _build_layers(self, architecture, input_size, output_size):
        """Build neural network layers."""
        layers = []
        
        # Input layer
        layers.append({
            'type': 'input',
            'size': input_size,
            'activation': 'linear'
        })
        
        # Hidden layers
        for i, hidden_size in enumerate(architecture):
            layers.append({
                'type': 'hidden',
                'size': hidden_size,
                'activation': 'relu',  # Can be customized
                'layer_index': i
            })
        
        # Output layer
        layers.append({
            'type': 'output',
            'size': output_size,
            'activation': 'softmax'  # Can be customized
        })
        
        return layers
    
    def _initialize_weights(self, architecture):
        """Initialize network weights."""
        weights = []
        
        for i in range(len(self.layers) - 1):
            input_size = self.layers[i]['size']
            output_size = self.layers[i + 1]['size']
            
            # Xavier initialization
            limit = np.sqrt(6.0 / (input_size + output_size))
            layer_weights = np.random.uniform(-limit, limit,
                                            (input_size, output_size))
            
            weights.append(layer_weights)
        
        return weights
    
    def _initialize_biases(self, architecture):
        """Initialize network biases."""
        biases = []
        
        for i in range(1, len(self.layers)):
            layer_size = self.layers[i]['size']
            biases.append(np.zeros(layer_size))
        
        return biases
    
    def forward(self, input_data):
        """Forward pass through network."""
        activations = [input_data]
        
        for i, layer in enumerate(self.layers[1:]):  # Skip input layer
            weights = self.weights[i]
            bias = self.biases[i]
            
            # Linear transformation
            z = np.dot(activations[-1], weights) + bias
            
            # Activation function
            activation = layer['activation']
            if activation == 'relu':
                a = np.maximum(0, z)
            elif activation == 'sigmoid':
                a = 1.0 / (1.0 + np.exp(-z))
            elif activation == 'tanh':
                a = np.tanh(z)
            elif activation == 'linear':
                a = z
            else:
                raise ValueError(f"Unknown activation: {activation}")
            
            activations.append(a)
        
        return activations[-1]
    
    def backpropagate(self, input_data, target, learning_rate):
        """Backpropagation through the network."""
        # Forward pass
        activations = self.forward(input_data)
        
        # Calculate loss
        loss = self._calculate_loss(activations[-1], target)
        
        # Backpropagation
        self._backpropagate_layered(loss, learning_rate)
        
        return loss
    
    def _calculate_loss(self, output, target):
        """Calculate loss function."""
        # Mean squared error
        error = output - target
        loss = np.mean(error ** 2)
        
        return loss
    
    def _backpropagate_layered(self, loss, learning_rate):
        """Backpropagation for each layer."""
        # This is a simplified implementation
        # A real implementation would need proper gradient calculations
        
        # For now, just update weights with random noise for demonstration
        for i, (weights, bias) in enumerate(zip(self.weights, self.biases)):
            # Random update (placeholder)
            weights += np.random.randn(*weights.shape) * learning_rate
            bias += np.random.randn(bias.shape[0]) * learning_rate
```

## 2. Cognitive Architecture Extensions

```python
class CognitiveArchitecture:
    def __init__(self, config):
        self.config = config
        self.modules = {}
        self.connections = {}
        self.working_memory = WorkingMemoryModule()
        self.long_term_memory = LongTermMemoryModule()
        self.attention_mechanism = AttentionMechanism()
        self.reasoning_engine = ReasoningEngine()
    
    def add_module(self, name, module):
        """Add cognitive module."""
        self.modules[name] = module
    
    def connect_modules(self, source_module, target_module, connection_type="feedforward"):
        """Connect two modules."""
        if source_module not in self.modules or target_module not in self.modules:
            raise ValueError(f"Module not found: {source_module} or {target_module}")
        
        self.connections[(source_module, target_module)] = {
            'type': connection_type,
            'strength': 1.0,
            'weight': np.random.random()
        }
    
    def process_input(self, input_data):
        """Process input through cognitive architecture."""
        # Input processing
        processed_input = self._preprocess_input(input_data)
        
        # Pass through attention mechanism
        attended_input = self.attention_mechanism.process(processed_input)
        
        # Process through working memory
        working_memory_output = self.working_memory.process(attended_input)
        
        # Process through modules
        module_outputs = {}
        for name, module in self.modules.items():
            # Check incoming connections
            incoming_connections = self._get_incoming_connections(name)
            
            if incoming_connections:
                # Weighted sum of incoming connections
                weighted_input = 0.0
                for source, conn in incoming_connections.items():
                    source_output = module_outputs.get(source, 0.0)
                    weighted_input += source_output * conn['weight']
                
                # Module processing
                module_output = module.process(weighted_input)
                module_outputs[name] = module_output
            else:
                # No incoming connections, use raw input
                module_output = module.process(processed_input)
                module_outputs[name] = module_output
        
        # Integrate outputs from reasoning engine
        reasoning_output = self.reasoning_engine.integrate(module_outputs)
        
        return reasoning_output
    
    def _get_incoming_connections(self, module_name):
        """Get incoming connections for a module."""
        connections = {}
        
        for (source, target), conn in self.connections.items():
            if target == module_name:
                connections[source] = conn
        
        return connections
    
    def update_working_memory(self, new_content):
        """Update working memory with new content."""
        self.working_memory.update(new_content)
    
    def consolidate_long_term_memory(self):
        """Consolidate working memory into long-term memory."""
        working_content = self.working_memory.get_content()
        self.long_term_memory.consolidate(working_content)
    
    def get_cognitive_state(self):
        """Get current cognitive state."""
        return {
            'working_memory': self.working_memory.get_state(),
            'attention_focus': self.attention_mechanism.get_focus(),
            'reasoning_output': self.reasoning_engine.get_output(),
            'module_outputs': {name: module.get_state() for name, module in self.modules.items()}
        }
```

### Module Example: Memory Module

```python
class WorkingMemoryModule:
    def __init__(self, capacity=100):
        self.capacity = capacity
        self.content = []
        self.activation_history = []
    
    def process(self, input_data):
        """Process input through working memory."""
        # Add to working memory
        self.content.append(input_data)
        
        # Apply capacity constraints
        if len(self.content) > self.capacity:
            # Remove oldest items
            self.content = self.content[-self.capacity:]
        
        # Calculate activation based on input
        activation = min(1.0, len(self.content) / self.capacity)
        
        # Return processed output
        return activation * input_data
    
    def get_content(self):
        """Get current working memory content."""
        return self.content
    
    def get_state(self):
        """Get module state."""
        return {
            'content': self.content,
            'activation_history': self.activation_history,
            'capacity': self.capacity,
            'current_load': len(self.content) / self.capacity
        }
```

## 3. Neuroevolution and Genetic Algorithms

```python
import random
import math

class NeuroevolutionSystem:
    def __init__(self, population_size=50, mutation_rate=0.1, elite_size=5):
        self.population_size = population_size
        self.mutation_rate = mutation_rate
        self.elite_size = elite_size
        self.population = []
        self.generation = 0
    
    def create_initial_population(self, brain_factory):
        """Create initial population of brains."""
        self.population = []
        
        for i in range(self.population_size):
            # Create brain with random parameters
            brain = brain_factory.create_random_brain()
            
            # Add to population
            self.population.append({
                'id': i,
                'brain': brain,
                'fitness': 0.0,
                'generation': 0
            })
    
    def evaluate_fitness(self, brain, environment):
        """Evaluate brain fitness in environment."""
        # Run brain in environment
        fitness = 0.0
        
        # Calculate fitness based on performance
        # This would involve running the brain and measuring its performance
        fitness += brain.get_total_spike_count() * 0.01  # Neural activity
        fitness += brain.get_average_firing_rate() * 0.1  # Firing rate
        fitness += brain.get_total_synapse_count() * 0.001  # Network size
        
        return fitness
    
    def evolve_population(self):
        """Evolve population using genetic algorithms."""
        # Evaluate fitness for all individuals
        for individual in self.population:
            fitness = self.evaluate_fitness(individual['brain'], None)
            individual['fitness'] = fitness
        
        # Sort by fitness (descending)
        self.population.sort(key=lambda x: x['fitness'], reverse=True)
        
        # Create new population
        new_population = []
        
        # Keep elite individuals
        for i in range(self.elite_size):
            new_population.append(self.population[i].copy())
        
        # Generate offspring through crossover and mutation
        while len(new_population) < self.population_size:
            parent1 = self._select_parent()
            parent2 = self._select_parent()
            
            child = self._crossover(parent1, parent2)
            child = self._mutate(child)
            child['generation'] = self.generation + 1
            
            new_population.append(child)
        
        self.population = new_population
        self.generation += 1
    
    def _select_parent(self):
        """Select parent using tournament selection."""
        tournament_size = 5
        candidates = random.sample(self.population, min(tournament_size, len(self.population)))
        return max(candidates, key=lambda x: x['fitness'])
    
    def _crossover(self, parent1, parent2):
        """Perform crossover between two parents."""
        # Create child by randomly mixing parents' parameters
        child = {
            'id': len(self.population),
            'brain': self._mix_brains(parent1['brain'], parent2['brain']),
            'fitness': 0.0,
            'generation': self.generation + 1
        }
        
        return child
    
    def _mix_brains(self, brain1, brain2):
        """Mix two brains to create child brain."""
        # This would involve creating a new brain by combining parameters
        # from both parents
        mixed_brain = EnhancedBrain()
        
        # Randomly mix configuration parameters
        for key in brain1._config.get_all_keys():
            if random.random() < 0.5:  # 50% chance to take from parent1
                value = brain1._config.get(key)
            else:  # Otherwise take from parent2
                value = brain2._config.get(key)
            
            mixed_brain._config.set(key, value)
        
        return mixed_brain
    
    def _mutate(self, individual):
        """Apply mutation to individual."""
        brain = individual['brain']
        
        # Randomly mutate configuration parameters
        for key in brain._config.get_all_keys():
            if random.random() < self.mutation_rate:
                # Apply mutation
                value = brain._config.get(key)
                if isinstance(value, (int, float)):
                    value += random.uniform(-0.1, 0.1) * value
                    # Constrain to reasonable range
                    value = max(0, min(1000000, value))
                
                brain._config.set(key, value)
        
        return individual
    
    def get_best_individual(self):
        """Get best performing individual."""
        if not self.population:
            return None
        
        return max(self.population, key=lambda x: x['fitness'])
    
    def run_evolution(self, generations, fitness_function=None):
        """Run evolution for specified generations."""
        print(f"Starting neuroevolution: {generations} generations")
        print(f"Population size: {self.population_size}, Mutation rate: {self.mutation_rate}")
        
        for generation in range(generations):
            print(f"\nGeneration {generation + 1}:")
            
            # Evolve population
            self.evolve_population()
            
            # Get best fitness
            best = self.get_best_individual()
            if best:
                print(f"  Best fitness: {best['fitness']:.3f}")
            
            # Save progress periodically
            if (generation + 1) % 10 == 0:
                self.save_progress(f"neat_progress_gen_{generation + 1}.bin")
        
        return self.get_best_individual()
    
    def save_progress(self, filepath):
        """Save evolution progress."""
        # Implementation for saving progress
        pass
```

---

# Migration Guide

## Upgrading from Basic NLM to Enhanced NLM

### 1. Installation Changes

**Basic NLM:**
```bash
pip install pynlm
```

**Enhanced NLM:**
```bash
pip install pynlm-enhanced
```

### 2. Import Changes

**Basic NLM:**
```python
import pynlm

brain = pynlm.createBrain(config)
brain.initialize()
```

**Enhanced NLM:**
```python
import pynlm_enhanced

brain = pynlm_enhanced.create_brain()
brain.initialize()
```

### 3. Configuration Changes

**Basic NLM:**
```python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
```

**Enhanced NLM:**
```python
# Enhanced configuration with validation
brain = pynlm_enhanced.create_brain(
    neuron_count=1000,
    timestep=0.001,
    max_steps=10000
)
```

### 4. Brain Interface Changes

**Basic NLM:**
```python
# Basic methods only
brain.step(step)
brain.get_total_spike_count()
```

**Enhanced NLM:**
```python
# Enhanced methods with more functionality
brain.initialize()
brain.run_simulation(100, verbose=False)
brain.get_stats()
brain.save_state("checkpoint.bin")
```

### 5. World Interface Changes

**Basic NLM:**
```python
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)
```

**Enhanced NLM:**
```python
world = pynlm_enhanced.create_world(20, 20, 8, 8)
world.add_resource(5, 5, 1.0)
world.add_hazard(15, 15, -0.5)
```

### 6. Agent Interface Changes

**Basic NLM:**
```python
agent = pynlm.createAgentBrain(brain)
agent.processSensoryInput(percept)
```

**Enhanced NLM:**
```python
agent = pynlm_enhanced.create_agent(brain, world)
agent.enable_all_subsystems()
agent.process_sensory_input(percept)
```

### 7. Migration Example

**From Basic to Enhanced:**

```python
# OLD (Basic NLM)
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Initialize
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

# Get stats
stats = {
    'spikes': brain.get_total_spike_count(),
    'firing': brain.get_firing_neuron_count(),
    'rate': brain.get_average_firing_rate()
}

# NEW (Enhanced NLM)
import pynlm_enhanced

# Create brain with enhanced features
brain = pynlm_enhanced.create_brain(neuron_count=1000)

# Initialize with validation
brain.initialize()

# Run simulation with monitoring
brain.run_simulation(100, verbose=False)

# Get comprehensive stats
stats = brain.get_stats()
print(f"Firing neurons: {stats['firing_neurons']}")
print(f"Total spikes: {stats['total_spikes']}")
print(f"Average firing rate: {stats['average_firing_rate']:.2f} Hz")
print(f"Development stage: {stats['development_stage']}")

# Save progress
brain.save_state("enhanced_brain.bin")
```

### 8. Feature Comparison

| Feature | Basic NLM | Enhanced NLM |
|---------|-----------|--------------|
| Configuration Validation | ❌ | ✅ |
| Error Handling | Basic | Comprehensive |
| Progress Monitoring | ❌ | ✅ |
| State Saving/Loading | Basic | Enhanced |
| Memory Management | Basic | Advanced |
| Plugin Support | ❌ | ✅ |
| Performance Profiling | ❌ | ✅ |
| Advanced Tutorials | ❌ | ✅ |

### 9. Migration Checklist

- [ ] Update imports from `pynlm` to `pynlm_enhanced`
- [ ] Replace `createBrain` with `create_brain`
- [ ] Add configuration validation
- [ ] Implement error handling
- [ ] Add progress monitoring
- [ ] Save/load enhanced brain states
- [ ] Update world and agent creation
- [ ] Add plugin system
- [ ] Test compatibility with existing code
- [ ] Update documentation
- [ ] Benchmark performance

---

# Conclusion

The enhanced NLM framework provides a comprehensive set of improvements over the basic NLM:

1. **Robust configuration validation** with helpful error messages
2. **Comprehensive error handling** for better debugging
3. **Progress monitoring** with verbose logging and callbacks
4. **Enhanced memory management** with better resource tracking
5. **Plugin architecture** for custom extensions
6. **Performance profiling** tools for optimization
7. **Advanced tutorials** for all skill levels
8. **Neuroscientific accuracy** with biologically inspired mechanisms

These improvements make the enhanced NLM framework suitable for:

- **Research**: Advanced neuroscience simulations
- **Education**: Teaching neural network concepts
- **Development**: Building custom neural architectures
- **Production**: Deployable neural simulation systems
- **Plugin Development**: Extending NLM functionality

The migration path is straightforward, and the enhanced features provide significant value while maintaining backward compatibility.

---

*This documentation is continuously updated. For the latest version, visit the NLM documentation portal.*

*Acknowledgments: This enhanced documentation was developed as part of the NLM Phase 6 integration effort.*
