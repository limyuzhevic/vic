# NLM Best Practices Guide

## Overview

This guide provides comprehensive best practices for developing, testing, and deploying NLM (Neural Learning Machine) applications. It covers coding standards, design patterns, performance optimization, and testing strategies to help you build robust, efficient, and maintainable NLM applications.

## Development Best Practices

### 1. Code Quality Standards

#### Naming Conventions
```python
# Follow Python naming conventions
class NeuralBrain:
    """Represents a neural brain with neurons and synapses."""
    
    def __init__(self, config):
        """Initialize brain with configuration."""
        self.neuron_count = config.getInt("neuron_count")
        self.learning_rate = config.getDouble("learning_rate")
    
    def processStep(self, step_number):
        """Process one simulation step."""
        pass
    
    def getFiringNeuronCount(self):
        """Get firing neuron count."""
        return len(self.firing_neurons)
```

#### Documentation Standards
```python
"""Comprehensive documentation with examples.

This module implements the NLM (Neural Learning Machine) brain system.

Features:
- Event-driven spike processing
- Synaptic plasticity mechanisms
- Memory management
- Performance optimization

Example:
    >>> brain = Brain(config)
    >>> brain.initialize()
    >>> brain.step(0)
    Firing neurons: 5
"""
```

#### Error Handling
```python
import pynlm

class NLMError(Exception):
    """Base exception for NLM errors."""
    pass

class BrainError(NLMError):
    """Exception raised for brain errors."""
    pass

class ConfigError(NLMError):
    """Exception raised for configuration errors."""
    pass

def safeBrainOperation(brain, step_count):
    """Perform safe brain operations with proper error handling."""
    try:
        brain.initialize()
        
        for step in range(step_count):
            brain.step(step)
            
        return brain.getFiringNeuronCount()
        
    except BrainError as e:
        print(f"Brain error: {e}")
        raise
    except ConfigError as e:
        print(f"Configuration error: {e}")
        raise
    except Exception as e:
        print(f"Unexpected error: {e}")
        raise
```

### 2. Configuration Management

#### Use Configuration Best Practices
```python
import pynlm

# Best practice: Create configuration with validation
class BrainConfig:
    """Configuration for NLM brain with validation."""
    
    def __init__(self, config_file=None):
        """Initialize configuration from file or defaults."""
        self.config = pynlm.createDefaultConfig()
        
        if config_file:
            self.config.loadFromFile(config_file)
        
        self.validateConfiguration()
    
    def validateConfiguration(self):
        """Validate configuration."""
        # Required configurations
        required_keys = ["neuron_count", "learning_rate"]
        
        for key in required_keys:
            if not self.config.has(key):
                raise ConfigError(f"Required configuration key missing: {key}")
        
        # Validate value ranges
        neuron_count = self.config.getInt("neuron_count")
        if neuron_count < 100 or neuron_count > 1000000:
            raise ConfigError(f"Invalid neuron count: {neuron_count}")
        
        learning_rate = self.config.getDouble("learning_rate")
        if learning_rate < 0.0 or learning_rate > 1.0:
            raise ConfigError(f"Invalid learning rate: {learning_rate}")
        
        # Optional configurations with defaults
        self.config.set("synapse_density", 0.1)  # Default value
        self.config.set("enable_curiosity", True)  # Default value
    
    def getBrain(self):
        """Create brain with validated configuration."""
        return pynlm.createBrain(self.config)
```

#### Use Configuration Schema
```python
import pynlm

# Best practice: Use configuration schema for complex validation
class AdvancedBrainConfig:
    """Advanced brain configuration with schema validation."""
    
    @staticmethod
    def createSchema():
        """Create configuration schema."""
        return pynlm.Config::createBuilder()
            .add("neuron_count", pynlm.ConfigSchema("neuron_count", pynlm.ConfigSchema::Type::Integer, true))
                .withRange(100, 1000000)
                .withDefault(1000)
                .withDescription("Number of neurons in the brain")
            .add("learning_rate", pynlm.ConfigSchema("learning_rate", pynlm.ConfigSchema::Type::Double, false))
                .withRange(0.0, 1.0)
                .withDefault(0.01)
                .withDescription("Learning rate for neural network")
            .add("enable_curiosity", pynlm.ConfigSchema("enable_curiosity", pynlm.ConfigSchema::Type::Boolean, false))
                .withDefault(true)
                .withDescription("Enable curiosity-driven exploration")
            .add("synapse_density", pynlm.ConfigSchema("synapse_density", pynlm.ConfigSchema::Type::Double, false))
                .withRange(0.01, 0.5)
                .withDefault(0.1)
                .withDescription("Synapse connectivity density")
            .build()
    
    @staticmethod
    def createBrainWithSchema():
        """Create brain with schema validation."""
        schema = AdvancedBrainConfig.createSchema()
        config = pynlm.createDefaultConfig()
        config.initializeWithSchema(schema)
        return pynlm.createBrain(config)
```

### 3. Brain Design

#### Use Event-Driven Architecture
```python
import pynlm

class EventDrivenBrain:
    """Event-driven brain implementation."""
    
    def __init__(self, config):
        """Initialize event-driven brain."""
        self.config = config
        self.event_queue = []
        self.neurons = []
        self.synapses = []
        self.memory = {}
    
    def initialize(self):
        """Initialize brain with event-driven architecture."""
        # Create neurons
        for i in range(self.config.getInt("neuron_count")):
            neuron = self.createNeuron(i)
            self.neurons.append(neuron)
        
        # Create synapses
        for i, neuron in enumerate(self.neurons):
            for j in range(i):
                if self.shouldConnect(neuron, self.neurons[j]):
                    synapse = self.createSynapse(neuron, self.neurons[j])
                    self.synapses.append(synapse)
    
    def createNeuron(self, id):
        """Create neuron with event-driven capabilities."""
        return {
            'id': id,
            'type': 'LIF',  # Leaky Integrate-and-Fire
            'membrane_potential': -70.0,  # mV
            'refractory_period': 2.0,  # ms
            'refractory_counter': 0.0,
            'spike_threshold': -50.0,  # mV
            'resting_potential': -70.0,  # mV
            'reset_potential': -75.0,  # mV
            'membrane_time_constant': 20.0,  # ms
            'activity': 0.0,
            'events': []  # Event queue for this neuron
        }
    
    def createSynapse(self, pre_neuron, post_neuron):
        """Create synapse with plasticity support."""
        return {
            'pre_neuron': pre_neuron,
            'post_neuron': post_neuron,
            'weight': 0.5,
            'delay': 1.0,  # ms
            'plasticity_rule': 'STDP',  # Spike-Timing-Dependent Plasticity
            'last_spike_pre': -1000.0,  # Time of last spike
            'last_spike_post': -1000.0,  # Time of last spike
            'trace_pre': 0.0,
            'trace_post': 0.0
        }
    
    def shouldConnect(self, neuron1, neuron2):
        """Determine if neurons should be connected."""
        density = self.config.getDouble("synapse_density")
        return np.random.random() < density
    
    def addEvent(self, neuron_id, event_type, data):
        """Add event to neuron event queue."""
        neuron = next((n for n in self.neurons if n['id'] == neuron_id), None)
        if neuron:
            event = {
                'type': event_type,
                'data': data,
                'time': time.time()
            }
            neuron['events'].append(event)
    
    def processEvents(self, current_time):
        """Process all neuron events."""
        for neuron in self.neurons:
            events = neuron['events']
            neuron['events'] = []  # Clear events after processing
            
            for event in events:
                if event['type'] == 'spike':
                    self.handleSpikeEvent(neuron, event)
                elif event['type'] == 'input':
                    self.handleInputEvent(neuron, event)
                elif event['type'] == 'learning':
                    self.handleLearningEvent(neuron, event)
    
    def handleSpikeEvent(self, neuron, event):
        """Handle spike event."""
        # Update neuron state
        neuron['refractory_counter'] = neuron['refractory_period']
        neuron['membrane_potential'] = neuron['reset_potential']
        
        # Process synaptic connections
        for synapse in self.synapses:
            if synapse['pre_neuron']['id'] == neuron['id']:
                self.scheduleSpikeTransmission(synapse, event['time'])
    
    def scheduleSpikeTransmission(self, synapse, spike_time):
        """Schedule spike transmission with delay."""
        transmission_time = spike_time + synapse['delay']
        
        # Create delayed spike event
        event = {
            'type': 'delayed_spike',
            'data': {
                'synapse': synapse,
                'strength': synapse['weight']
            },
            'time': transmission_time
        }
        
        # Add event to postsynaptic neuron
        post_neuron = synapse['post_neuron']
        post_neuron['events'].append(event)
    
    def step(self, step_number):
        """Process one simulation step with event-driven processing."""
        current_time = step_number * 0.1  # 0.1 ms per step
        
        # Process all events
        self.processEvents(current_time)
        
        # Update neuron states
        self.updateNeuronStates(current_time)
        
        # Process plasticity
        self.processPlasticity(current_time)
        
        # Schedule new events
        self.scheduleNewEvents(current_time)
    
    def updateNeuronStates(self, current_time):
        """Update neuron membrane potentials."""
        for neuron in self.neurons:
            if neuron['refractory_counter'] > 0:
                # Neuron is in refractory period
                neuron['refractory_counter'] -= 0.1
            else:
                # Update membrane potential
                dt = 0.1  # Time step
                tau = neuron['membrane_time_constant']
                v_rest = neuron['resting_potential']
                
                # Leaky integrate-and-fire equation
                dv = (v_rest - neuron['membrane_potential']) / tau * dt
                neuron['membrane_potential'] += dv
                
                # Check for spike
                if neuron['membrane_potential'] >= neuron['spike_threshold']:
                    neuron['membrane_potential'] = neuron['reset_potential']
                    neuron['refractory_counter'] = neuron['refractory_period']
                    
                    # Add spike event
                    self.addEvent(neuron['id'], 'spike', {
                        'time': current_time,
                        'neuron_id': neuron['id']
                    })
    
    def processPlasticity(self, current_time):
        """Process synaptic plasticity."""
        for synapse in self.synapses:
            # Update STDP traces
            synapse['trace_pre'] *= 0.95
            synapse['trace_post'] *= 0.95
            
            # Check for spike-timing dependent plasticity
            if synapse['last_spike_pre'] > -1000 and synapse['last_spike_post'] > -1000:
                dt = synapse['last_spike_post'] - synapse['last_spike_pre']
                
                if abs(dt) < 20:  # Within 20 ms
                    if dt > 0:
                        # Post before pre (LTP)
                        synapse['weight'] += 0.01 * synapse['trace_post']
                    else:
                        # Pre before post (LTD)
                        synapse['weight'] -= 0.01 * synapse['trace_pre']
                
                # Constrain weight
                synapse['weight'] = max(0.0, min(1.0, synapse['weight']))
    
    def scheduleNewEvents(self, current_time):
        """Schedule new events for next step."""
        for neuron in self.neurons:
            # Add random input events
            if np.random.random() < 0.1:  # 10% chance per neuron
                self.addEvent(neuron['id'], 'input', {
                    'current': np.random.normal(0, 1),
                    'time': current_time
                })
            
            # Add learning events
            if np.random.random() < 0.05:  # 5% chance per neuron
                self.addEvent(neuron['id'], 'learning', {
                    'reward': np.random.random(),
                    'time': current_time
                })

# Use event-driven brain
if __name__ == "__main__":
    import numpy as np
    import time
    
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 100)
    
    brain = EventDrivenBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(1000):
        brain.step(step)
        
        if step % 100 == 0:
            firing_count = sum(1 for neuron in brain.neurons 
                             if neuron['refractory_counter'] > 0)
            print(f"Step {step}: {firing_count} firing neurons")
```

#### Use Modular Brain Design
```python
import pynlm

# Define brain components as separate classes
class NeuralLayer:
    """Represents a layer of neurons in the brain."""
    
    def __init__(self, size, layer_type="hidden"):
        self.size = size
        self.layer_type = layer_type
        self.neurons = []
        self.connections = []
    
    def createNeurons(self):
        """Create neurons for this layer."""
        for i in range(self.size):
            neuron = self.createNeuron(i, self.layer_type)
            self.neurons.append(neuron)
    
    def createNeuron(self, id, layer_type):
        """Create single neuron."""
        return {
            'id': id,
            'layer': self.layer_type,
            'type': 'LIF',
            'membrane_potential': -70.0,
            'refractory_period': 2.0,
            'refractory_counter': 0.0,
            'spike_threshold': -50.0,
            'resting_potential': -70.0,
            'reset_potential': -75.0,
            'membrane_time_constant': 20.0
        }

class ConnectionManager:
    """Manages connections between neural layers."""
    
    def __init__(self, config):
        self.config = config
        self.connections = []
    
    def connectLayers(self, pre_layer, post_layer):
        """Connect neurons between layers."""
        density = self.config.getDouble("synapse_density")
        
        for pre_neuron in pre_layer.neurons:
            for post_neuron in post_layer.neurons:
                if np.random.random() < density:
                    connection = self.createConnection(pre_neuron, post_neuron)
                    self.connections.append(connection)
    
    def createConnection(self, pre_neuron, post_neuron):
        """Create connection between neurons."""
        return {
            'pre_neuron': pre_neuron,
            'post_neuron': post_neuron,
            'weight': np.random.normal(0.5, 0.1),
            'delay': 1.0,
            'plasticity': 'STDP'
        }

class PlasticityManager:
    """Manages synaptic plasticity rules."""
    
    def __init__(self):
        self.rules = {
            'STDP': self.stdpPlasticity,
            'Hebbian': self.hebbianPlasticity,
            'structural': self.structuralPlasticity
        }
    
    def applyPlasticity(self, synapse, pre_spike_time, post_spike_time):
        """Apply plasticity rule to synapse."""
        rule = synapse.get('plasticity', 'STDP')
        
        if rule in self.rules:
            self.rules[rule](synapse, pre_spike_time, post_spike_time)
    
    def stdpPlasticity(self, synapse, pre_spike_time, post_spike_time):
        """Apply STDP plasticity rule."""
        dt = post_spike_time - pre_spike_time
        
        if abs(dt) < 20:  # Within 20 ms
            if dt > 0:
                # Post before pre (LTP)
                synapse['weight'] += 0.01
            else:
                # Pre before post (LTD)
                synapse['weight'] -= 0.01
            
            # Constrain weight
            synapse['weight'] = max(0.0, min(1.0, synapse['weight']))
    
    def hebbianPlasticity(self, synapse, pre_spike_time, post_spike_time):
        """Apply Hebbian plasticity rule."""
        # Increase weight if both pre and post spike
        if pre_spike_time > -1000 and post_spike_time > -1000:
            synapse['weight'] += 0.005
            synapse['weight'] = min(1.0, synapse['weight'])
    
    def structuralPlasticity(self, synapse, pre_spike_time, post_spike_time):
        """Apply structural plasticity (synaptogenesis/pruning)."""
        # Probabilistic synaptogenesis
        if np.random.random() < 0.001:
            # Create new synapse
            new_synapse = {
                'pre_neuron': synapse['pre_neuron'],
                'post_neuron': synapse['post_neuron'],
                'weight': np.random.normal(0.5, 0.1),
                'delay': 1.0,
                'plasticity': 'STDP'
            }
            return new_synapse
        
        # Synaptic pruning
        if np.random.random() < 0.001 and synapse['weight'] < 0.1:
            return None  # Remove synapse
        
        return synapse
```

### 4. Performance Optimization

#### Use Batch Processing
```python
import pynlm
from performance import BatchBrain

class OptimizedBrain:
    """Optimized brain implementation with batch processing."""
    
    def __init__(self, config):
        """Initialize optimized brain."""
        self.config = config
        self.brain = BatchBrain(config)
        self.batch_processor = BatchProcessor(config)
    
    def processBatch(self, steps):
        """Process multiple steps efficiently."""
        return self.batch_processor.processBatch(steps)
    
    def getPerformanceMetrics(self):
        """Get performance metrics."""
        return self.batch_processor.getMetrics()

class BatchProcessor:
    """Batch processor for efficient neural computation."""
    
    def __init__(self, config):
        self.config = config
        self.setupBatchProcessor()
    
    def setupBatchProcessor(self):
        """Set up batch processor."""
        # Configure for optimal performance
        self.batch_size = 1000
        self.enable_SIMD = True
        self.enable_multithreading = True
        self.enable_memory_pool = True
        self.early_termination_threshold = 0.95
    
    def processBatch(self, start_step, end_step):
        """Process batch of steps."""
        # Configure batch processor
        batch_config = {
            'batch_size': self.batch_size,
            'enable_SIMD': self.enable_SIMD,
            'enable_multithreading': self.enable_multithreading,
            'enable_memory_pool': self.enable_memory_pool,
            'early_termination_threshold': self.early_termination_threshold
        }
        
        # Process batch
        return self.runBatch(start_step, end_step, batch_config)
    
    def runBatch(self, start_step, end_step, batch_config):
        """Run batch processing."""
        # Implementation would use BatchStepProcessor
        # This is a simplified example
        steps_processed = end_step - start_step + 1
        processing_time = steps_processed * 0.001  # Simulated
        
        return {
            'steps_processed': steps_processed,
            'processing_time': processing_time,
            'throughput': steps_processed / processing_time,
            'batch_config': batch_config
        }
    
    def getMetrics(self):
        """Get performance metrics."""
        return {
            'batch_size': self.batch_size,
            'SIMD_enabled': self.enable_SIMD,
            'multithreading_enabled': self.enable_multithreading,
            'memory_pool_enabled': self.enable_memory_pool,
            'early_termination_threshold': self.early_termination_threshold
        }
```

#### Use Memory Optimization
```python
import pynlm

class MemoryOptimizedBrain:
    """Memory-optimized brain implementation."""
    
    def __init__(self, config):
        """Initialize memory-optimized brain."""
        self.config = config
        self.setupMemoryOptimization()
        
        # Create optimized data structures
        self.setupOptimizedDataStructures()
    
    def setupMemoryOptimization(self):
        """Set up memory optimization strategies."""
        # Enable memory pooling
        self.memory_pool_enabled = True
        self.slab_allocation_enabled = True
        self.cache_friendly_layout = True
        
        # Configure memory sizes
        self.neuron_pool_size = 1000000
        self.synapse_pool_size = 10000000
        self.event_queue_capacity = 16384
    
    def setupOptimizedDataStructures(self):
        """Set up optimized data structures."""
        # Use Structure-of-Arrays (SoA) for better cache performance
        self.setupSoALayout()
        
        # Use memory pools for efficient allocation
        self.setupMemoryPools()
        
        # Use event queues for spike processing
        self.setupEventQueues()
    
    def setupSoALayout(self):
        """Set up Structure-of-Arrays layout."""
        # Separate arrays for each neuron state component
        self.membrane_potentials = np.zeros(self.neuron_pool_size)
        self.refractory_counters = np.zeros(self.neuron_pool_size)
        self.spike_counts = np.zeros(self.neuron_pool_size)
        self.synaptic_weights = np.zeros(self.synapse_pool_size)
        self.synapse_delays = np.zeros(self.synapse_pool_size)
        
        # Initialize neuron state
        for i in range(self.neuron_pool_size):
            self.membrane_potentials[i] = -70.0
            self.refractory_counters[i] = 0.0
            self.spike_counts[i] = 0
    
    def setupMemoryPools(self):
        """Set up memory pools."""
        # Create memory pools for neurons and synapses
        self.neuron_pool = MemoryPool('neuron', self.neuron_pool_size)
        self.synapse_pool = MemoryPool('synapse', self.synapse_pool_size)
        
        # Fill pools
        for i in range(self.neuron_pool_size):
            self.neuron_pool.allocate()
        
        for i in range(self.synapse_pool_size):
            self.synapse_pool.allocate()
    
    def setupEventQueues(self):
        """Set up event queues for spike processing."""
        # Create ring buffer for spike events
        self.spike_queue = RingBuffer(16384)
        
        # Create time-bucketed delayed spike queue
        self.delayed_spike_queue = TimeBucketedQueue(1024, 100)
    
    def step(self, step_number):
        """Process one simulation step with memory optimization."""
        current_time = step_number * 0.1
        
        # Process spike events from queue
        self.processSpikeEvents(current_time)
        
        # Update neuron states using SoA
        self.updateNeuronStatesSoA(current_time)
        
        # Process plasticity using optimized data structures
        self.processPlasticityOptimized(current_time)
        
        # Schedule new events
        self.scheduleNewEvents(current_time)
    
    def processSpikeEvents(self, current_time):
        """Process spike events from event queues."""
        while self.spike_queue.hasEvents():
            spike_event = self.spike_queue.pop()
            self.handleSpikeEvent(spike_event)
        
        # Process delayed spike events
        self.delayed_spike_queue.processDelayedSpikes(current_time)
    
    def handleSpikeEvent(self, spike_event):
        """Handle spike event."""
        neuron_id = spike_event['neuron_id']
        synapse_strength = spike_event['synapse_strength']
        
        # Update postsynaptic neurons
        self.updatePostsynapticNeurons(neuron_id, synapse_strength, current_time)
    
    def updateNeuronStatesSoA(self, current_time):
        """Update neuron states using Structure-of-Arrays."""
        # Process neurons in batches for cache efficiency
        batch_size = 1024
        num_neurons = len(self.neurons)
        
        for batch_start in range(0, num_neurons, batch_size):
            batch_end = min(batch_start + batch_size, num_neurons)
            self.processNeuronBatch(batch_start, batch_end, current_time)
    
    def processNeuronBatch(self, start_idx, end_idx, current_time):
        """Process batch of neurons."""
        for i in range(start_idx, end_idx):
            if self.refractory_counters[i] > 0:
                # Neuron is in refractory period
                self.refractory_counters[i] -= 0.1
            else:
                # Update membrane potential
                dt = 0.1
                tau = 20.0
                v_rest = -70.0
                
                dv = (v_rest - self.membrane_potentials[i]) / tau * dt
                self.membrane_potentials[i] += dv
                
                # Check for spike
                if self.membrane_potentials[i] >= -50.0:
                    self.membrane_potentials[i] = -75.0
                    self.refractory_counters[i] = 2.0
                    self.spike_counts[i] += 1
                    
                    # Add to spike queue
                    self.spike_queue.push({
                        'neuron_id': i,
                        'synapse_strength': self.synaptic_weights[i],
                        'time': current_time
                    })
    
    def scheduleNewEvents(self, current_time):
        """Schedule new events for next step."""
        # Random spike generation
        num_spikes = np.random.poisson(100)  # Poisson distribution for spike generation
        
        for _ in range(num_spikes):
            neuron_id = np.random.randint(0, len(self.neurons))
            self.spike_queue.push({
                'neuron_id': neuron_id,
                'synapse_strength': self.synaptic_weights[neuron_id],
                'time': current_time
            })
```

### 5. Testing

#### Unit Testing
```python
import unittest
import pynlm
import numpy as np

class TestBrain(unittest.TestCase):
    """Test brain functionality."""
    
    def setUp(self):
        """Set up test."""
        self.config = pynlm.createDefaultConfig()
        self.config.set("neuron_count", 100)
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
    
    def tearDown(self):
        """Clean up test."""
        del self.brain
    
    def testBrainCreation(self):
        """Test brain creation."""
        self.assertIsNotNone(self.brain)
        self.assertEqual(self.brain.getTotalNeuronCount(), 100)
    
    def testBrainInitialization(self):
        """Test brain initialization."""
        self.assertTrue(self.brain.getTotalNeuronCount() > 0)
    
    def testBrainStep(self):
        """Test brain step."""
        initial_firing = self.brain.getFiringNeuronCount()
        
        for step in range(10):
            self.brain.step(step)
        
        final_firing = self.brain.getFiringNeuronCount()
        self.assertGreaterEqual(final_firing, initial_firing)
    
    def testBrainStats(self):
        """Test brain statistics."""
        neuron_count = self.brain.getTotalNeuronCount()
        synapse_count = self.brain.getTotalSynapseCount()
        
        self.assertGreater(neuron_count, 0)
        self.assertGreater(synapse_count, 0)
        self.assertLess(synapse_count, neuron_count * 10)

class TestConfig(unittest.TestCase):
    """Test configuration functionality."""
    
    def testConfigCreation(self):
        """Test configuration creation."""
        config = pynlm.createDefaultConfig()
        self.assertIsNotNone(config)
    
    def testConfigSetGet(self):
        """Test configuration set/get."""
        config = pynlm.createDefaultConfig()
        config.set("test_key", "test_value")
        self.assertEqual(config.get("test_key"), "test_value")
    
    def testConfigValidation(self):
        """Test configuration validation."""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 1000)
        
        self.assertTrue(config.validateAgainstSchema())
    
    def testConfigFileIO(self):
        """Test configuration file I/O."""
        import tempfile
        import os
        
        config = pynlm.createDefaultConfig()
        config.set("test_key", "test_value")
        
        # Write to file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            config.saveToFile(f.name)
            temp_file = f.name
        
        # Read from file
        new_config = pynlm.createDefaultConfig()
        self.assertTrue(new_config.loadFromFile(temp_file))
        self.assertEqual(new_config.get("test_key"), "test_value")
        
        # Clean up
        os.unlink(temp_file)
```

#### Integration Testing
```python
import unittest
import pynlm

class TestBrainWorldIntegration(unittest.TestCase):
    """Test brain-world integration."""
    
    def setUp(self):
        """Set up test."""
        # Create brain
        config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        # Create world
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=20, height=20)
        
        # Create agent
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
    
    def tearDown(self):
        """Clean up test."""
        del self.agent, self.brain, self.world
    
    def testAgentWorldInteraction(self):
        """Test agent-world interaction."""
        # Run simulation
        for step in range(50):
            self.world.update(0.1)
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        # Verify interaction
        self.assertTrue(self.world.getSimulationTime() > 0)
        self.assertGreater(self.brain.getFiringNeuronCount(), 0)
    
    def testAgentLearning(self):
        """Test agent learning capabilities."""
        # Enable learning
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        
        # Run simulation
        for step in range(100):
            self.world.update(0.1)
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        # Verify learning
        self.assertGreater(self.agent.getCuriosityLevel(), 0)
        self.assertGreater(self.agent.getNoveltyLevel(), 0)
    
    def testBrainWorldPersistence(self):
        """Test brain-world persistence."""
        # Run simulation
        for step in range(10):
            self.world.update(0.1)
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        # Save brain state
        import tempfile
        import os
        
        with tempfile.NamedTemporaryFile(delete=False) as f:
            temp_file = f.name
        
        self.brain.save(temp_file)
        
        # Create new brain and load state
        new_config = pynlm.createDefaultConfig()
        new_brain = pynlm.createBrain(new_config)
        new_brain.load(temp_file)
        
        # Verify persistence
        self.assertEqual(new_brain.getTotalNeuronCount(), self.brain.getTotalNeuronCount())
        
        # Clean up
        os.unlink(temp_file)
```

#### Performance Testing
```python
import unittest
import pynlm
import time

class TestPerformance(unittest.TestCase):
    """Test performance characteristics."""
    
    def testBrainPerformance(self):
        """Test brain performance."""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 1000)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Time simulation
        start_time = time.time()
        for step in range(1000):
            brain.step(step)
        end_time = time.time()
        
        # Calculate performance
        steps_per_second = 1000 / (end_time - start_time)
        
        # Assert performance
        self.assertGreater(steps_per_second, 100)  # At least 100 steps/second
    
    def testMemoryUsage(self):
        """Test memory usage."""
        import psutil
        import os
        
        process = psutil.Process(os.getpid())
        
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 100)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Get initial memory
        initial_memory = process.memory_info().rss
        
        # Run simulation
        for step in range(1000):
            brain.step(step)
        
        # Get final memory
        final_memory = process.memory_info().rss
        
        # Calculate memory usage
        memory_increase = (final_memory - initial_memory) / 1024 / 1024  # MB
        
        # Assert memory usage
        self.assertLess(memory_increase, 50)  # Less than 50 MB increase
```

### 6. Documentation

#### Code Documentation
```python
"""NLM (Neural Learning Machine) Brain Implementation.

This module implements the NLM (Neural Learning Machine) brain system,
an event-driven spiking neural network simulator written in C++20.

Features:
- Leaky Integrate-and-Fire (LIF) neurons
- Event-driven spike propagation
- Synaptic delays
- Multiple plasticity mechanisms (STDP, Hebbian, structural)
- Memory management and optimization
- Performance optimization for large-scale simulations

Classes:
- Brain: Main brain class
- Config: Configuration management
- AgentBrain: Agent interface for brain-world interaction
- SimpleWorld: Simple world for agent simulation

Functions:
- createBrain: Create brain with configuration
- createDefaultConfig: Create default configuration
- createAgentBrain: Create agent brain interface
- createSimpleWorld: Create simple world

Example:
    >>> import pynlm
    >>> config = pynlm.createDefaultConfig()
    >>> config.set("brain.neuron_count", 1000)
    >>> config.set("brain.learning_rate", 0.01)
    >>> brain = pynlm.createBrain(config)
    >>> brain.initialize()
    >>> brain.step(0)
    Firing neurons: 5
"""
```

#### API Documentation
```python
# Complete API documentation for all public classes and functions

import pynlm

# Core Classes
pynlm.Brain
pynlm.Config
pynlm.AgentBrain
pynlm.SimpleWorld

# Core Functions
pynlm.createBrain
pynlm.createDefaultConfig
pynlm.createAgentBrain
pynlm.createSimpleWorld

# Enumerations
pynlm.NeuronType
pynlm.DevelopmentalStage
pynlm.ActionType
pynlm.WorldObjectType

# Constants
pynlm.VERSION
pynlm.LICENSE
```

#### Examples
```python
# Example 1: Basic brain usage
import pynlm

config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 100)
config.set("brain.learning_rate", 0.01)

brain = pynlm.createBrain(config)
brain.initialize()

for step in range(100):
    brain.step(step)
    print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")

# Example 2: Complete agent-world simulation
import pynlm

def runSimulation():
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run simulation
    for step in range(1000):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    return brain, world, agent

brain, world, agent = runSimulation()
print(f"Final curiosity: {agent.getCuriosityLevel():.3f}")
print(f"Final novelty: {agent.getNoveltyLevel():.3f}")
```

## Best Practices Summary

### 1. Code Quality
- Follow Python naming conventions
- Write comprehensive documentation
- Handle errors gracefully
- Use type hints for better IDE support

### 2. Configuration Management
- Use configuration schemas for validation
- Provide sensible defaults
- Support configuration files (JSON, YAML, etc.)
- Validate configuration at runtime

### 3. Brain Design
- Use event-driven architecture
- Implement modular brain components
- Support multiple plasticity mechanisms
- Optimize for memory efficiency

### 4. Performance Optimization
- Use batch processing for large simulations
- Implement memory optimization strategies
- Profile and benchmark regularly
- Optimize for cache efficiency

### 5. Testing
- Write comprehensive unit tests
- Test integration thoroughly
- Profile performance
- Monitor memory usage

### 6. Documentation
- Document all public APIs
- Provide examples and use cases
- Keep documentation up-to-date
- Use consistent formatting

By following these best practices, you can create robust, efficient, and maintainable NLM applications that leverage all the features of the NLM system.
