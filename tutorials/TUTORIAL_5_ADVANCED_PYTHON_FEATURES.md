# Tutorial 5: Advanced Python Features

## Overview

This tutorial introduces you to advanced NLM (Neural Learning Machine) Python features. It covers:

- Async support for high-performance applications
- NumPy and Pandas integration
- JSON serialization and configuration management
- Parallel processing and threading
- Context managers and resource management
- Event system and callbacks
- Custom brain implementations
- Integration with external libraries

This tutorial is designed for users who want to leverage advanced Python features for optimal NLM performance.

## Prerequisites

Before starting this tutorial, you should:

1. **Be familiar with Tutorial 1-4**: Basic brain creation, configuration, batch processing, and profiling
2. **Understand advanced Python concepts**: Async programming, NumPy, Pandas, threading
3. **Have NLM installed**: As per the main installation instructions
4. **Consider system resources**: Ensure adequate memory and CPU for advanced features

## What You'll Learn

By the end of this tutorial, you will:

- Understand advanced Python integration features
- Implement async support for high-performance applications
- Integrate NumPy and Pandas for data analysis
- Use JSON serialization for configuration management
- Implement parallel processing and threading
- Create context managers for resource management
- Use event system and callbacks for advanced functionality
- Create custom brain implementations
- Integrate with external libraries and frameworks
- Build production-ready NLM applications

## Setting Up Your Environment

The same as previous tutorials. Ensure NLM is installed:

```bash
pip install pynlm numpy pandas matplotlib
```

## Advanced Python Features

### 1. Async Support

#### Async Brain Processing
```python
import pynlm
import asyncio

class AsyncNLM:
    """Async NLM implementation for high-performance applications."""
    
    def __init__(self, config=None):
        """Initialize async NLM with configuration."""
        self.config = config or pynlm.createDefaultConfig()
        self.brain = None
        self.world = None
        self.agent = None
    
    async def initialize(self):
        """Initialize NLM components asynchronously."""
        # Create brain with async initialization
        self.brain = pynlm.createBrain(self.config)
        await self._initializeBrainAsync(self.brain)
        
        # Create world
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=20, height=20)
        await self._initializeWorldAsync(self.world)
        
        # Create agent
        self.agent = pynlm.createAgentBrain(self.brain)
        await self._initializeAgentAsync(self.agent)
        
        # Enable learning features
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
    
    async def _initializeBrainAsync(self, brain):
        """Initialize brain asynchronously."""
        await asyncio.get_event_loop().run_in_executor(None, brain.initialize)
    
    async def _initializeWorldAsync(self, world):
        """Initialize world asynchronously."""
        await asyncio.get_event_loop().run_in_executor(None, world.reset)
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    
    async def _initializeAgentAsync(self, agent):
        """Initialize agent asynchronously."""
        await asyncio.get_event_loop().run_in_executor(None, agent.initialize, self.world)
    
    async def processStepAsync(self, step):
        """Process single simulation step asynchronously."""
        # Update world asynchronously
        await asyncio.get_event_loop().run_in_executor(None, self.world.update, 0.1)
        
        # Get sensory input asynchronously
        percept = await asyncio.get_event_loop().run_in_executor(None, self.world.getSensoryPercept)
        
        # Process sensory input asynchronously
        await asyncio.get_event_loop().run_in_executor(None, self.agent.processSensoryInput, percept)
        
        # Run brain step asynchronously
        await asyncio.get_event_loop().run_in_executor(None, self.brain.step, step)
        
        # Get action asynchronously
        action = await asyncio.get_event_loop().run_in_executor(None, self.agent.decodeMotorCommand)
        
        # Apply action asynchronously
        await asyncio.get_event_loop().run_in_executor(None, self.world.applyMotorCommand, action, self.world.getSimulationTime())
        
        return step
    
    async def runSimulationAsync(self, steps):
        """Run simulation asynchronously with multiple concurrent tasks."""
        print(f"Starting async simulation with {steps} steps...")
        
        # Create tasks for parallel processing
        tasks = []
        for step in range(steps):
            task = asyncio.create_task(self.processStepAsync(step))
            tasks.append(task)
            
            # Limit concurrent tasks to avoid overwhelming the system
            if len(tasks) >= 10:
                done, pending = await asyncio.wait(tasks, return_when=asyncio.FIRST_COMPLETED)
                for completed_task in done:
                    tasks.remove(completed_task)
        
        # Wait for all remaining tasks
        if tasks:
            await asyncio.gather(*tasks)
        
        print(f"Async simulation completed!")
    
    async def runWithProgressAsync(self, steps):
        """Run simulation with progress monitoring asynchronously."""
        print(f"Starting async simulation with progress monitoring...")
        
        start_time = time.time()
        
        for step in range(steps):
            await self.processStepAsync(step)
            
            # Update progress
            if step % 100 == 0:
                elapsed = time.time() - start_time
                steps_per_second = step / elapsed if elapsed > 0 else 0
                print(f"Progress: {step}/{steps} steps ({steps_per_second:.0f} steps/s)")
        
        end_time = time.time()
        total_time = end_time - start_time
        
        print(f"Async simulation completed in {total_time:.2f}s")
        print(f"Average speed: {steps / total_time:.0f} steps/s")
```

### 2. NumPy Integration

#### NumPy-Optimized Brain
```python
import pynlm
import numpy as np

class NumPyOptimizedBrain:
    """NumPy-optimized brain for high-performance neural computation."""
    
    def __init__(self, config):
        """Initialize NumPy-optimized brain."""
        self.config = config
        self.setupNumPyOptimization()
        
        # Create NumPy-optimized data structures
        self.setupNumPyDataStructures()
    
    def setupNumPyOptimization(self):
        """Set up NumPy optimization strategies."""
        # Enable NumPy optimization
        self.numpy_optimization_enabled = True
        
        # Configure NumPy operations
        self.vectorized_operations = True
        self.batched_processing = True
        self.memory_efficiency = True
    
    def setupNumPyDataStructures(self):
        """Set up NumPy-optimized data structures."""
        # Use NumPy arrays for efficient memory usage
        self.membrane_potentials = np.full(self.neuron_count, -70.0, dtype=np.float32)
        self.refractory_counters = np.zeros(self.neuron_count, dtype=np.float32)
        self.spike_counts = np.zeros(self.neuron_count, dtype=np.int32)
        
        # Use NumPy for synapse management
        self.synapse_weights = np.full(self.synapse_count, 0.5, dtype=np.float32)
        self.synapse_delays = np.full(self.synapse_count, 1.0, dtype=np.float32)
        
        # Use NumPy for connectivity
        self.connectivity_matrix = self.createConnectivityMatrix()
    
    def createConnectivityMatrix(self):
        """Create connectivity matrix using NumPy."""
        # Use NumPy for sparse matrix operations
        rows = np.random.randint(0, self.neuron_count, size=self.synapse_count)
        cols = np.random.randint(0, self.neuron_count, size=self.synapse_count)
        
        # Create sparse connectivity matrix
        connectivity = np.zeros((self.neuron_count, self.neuron_count), dtype=np.float32)
        connectivity[rows, cols] = 1.0
        
        return connectivity
    
    def processBatchWithNumPy(self, steps):
        """Process batch using NumPy optimization."""
        # Vectorized neuron processing
        for step in range(steps):
            # Update membrane potentials using NumPy vectorization
            self.updateMembranePotentialsNumPy()
            
            # Process spikes using NumPy vectorization
            self.processSpikesNumPy()
            
            # Update synaptic weights using NumPy vectorization
            self.updateSynapticWeightsNumPy()
            
            # Schedule new events
            self.scheduleNewEventsNumPy()
    
    def updateMembranePotentialsNumPy(self):
        """Update membrane potentials using NumPy vectorization."""
        # Use NumPy for vectorized operations
        dt = 0.1
        tau = 20.0
        v_rest = -70.0
        
        # Vectorized membrane potential update
        dv = (v_rest - self.membrane_potentials) / tau * dt
        self.membrane_potentials += dv
        
        # Detect spikes using NumPy
        spikes = self.membrane_potentials >= -50.0
        spike_indices = np.where(spikes)[0]
        
        # Reset membrane potentials for spiking neurons
        self.membrane_potentials[spike_indices] = -75.0
        
        # Update refractory counters
        self.refractory_counters[spike_indices] = 2.0
    
    def processSpikesNumPy(self):
        """Process spikes using NumPy vectorization."""
        # Use NumPy for spike processing
        spike_indices = np.where(self.refractory_counters > 0)[0]
        
        # Update refractory counters
        self.refractory_counters[spike_counts] -= 0.1
        
        # Update spike counts
        self.spike_counts[spike_indices] += 1
    
    def updateSynapticWeightsNumPy(self):
        """Update synaptic weights using NumPy vectorization."""
        # Use NumPy for weight updates
        pre_spikes = np.where(self.spike_counts > 0)[0]
        
        if len(pre_spikes) > 0:
            # STDP learning rule using NumPy
            for pre_neuron in pre_spikes:
                # Find postsynaptic neurons
                post_neurons = np.where(self.connectivity_matrix[pre_neuron] > 0)[0]
                
                # Update synaptic weights using NumPy
                weight_updates = 0.01 * np.random.randn(len(post_neurons))
                self.synapse_weights[pre_neuron, post_neurons] += weight_updates
                
                # Constrain weights to [0, 1]
                self.synapse_weights = np.clip(self.synapse_weights, 0.0, 1.0)
```

### 3. Pandas Integration

#### Data Analysis with Pandas
```python
import pynlm
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

class DataAnalysisBrain:
    """Brain with data analysis capabilities using Pandas."""
    
    def __init__(self, config):
        """Initialize data analysis brain."""
        self.config = config
        self.setupDataAnalysis()
        
        # Create data analysis components
        self.setupDataAnalysisComponents()
    
    def setupDataAnalysis(self):
        """Set up data analysis strategies."""
        # Enable data analysis
        self.data_analysis_enabled = True
        
        # Configure data analysis options
        self.time_series_analysis = True
        self.correlation_analysis = True
        self.statistical_analysis = True
    
    def setupDataAnalysisComponents(self):
        """Set up data analysis components."""
        # Create simulation data collection
        self.simulation_data = []
        
        # Create analysis tools
        self.data_analyzer = DataAnalyzer()
        self.correlation_analyzer = CorrelationAnalyzer()
        self.statistical_analyzer = StatisticalAnalyzer()
    
    def runSimulationWithDataAnalysis(self, steps):
        """Run simulation with data analysis."""
        print(f"Running simulation with data analysis...")
        
        for step in range(steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Run brain
            self.brain.step(step)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            
            # Apply action
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Collect simulation data
            self.collectSimulationData(step)
            
            # Print progress
            if step % 100 == 0:
                print(f"Step {step}: Data collected: {len(self.simulation_data)} points")
        
        # Analyze simulation data
        self.analyzeSimulationData()
        
        return pd.DataFrame(self.simulation_data)
    
    def collectSimulationData(self, step):
        """Collect simulation data for analysis."""
        data_point = {
            'step': step,
            'simulation_time': self.world.getSimulationTime(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'total_neurons': self.brain.getTotalNeuronCount(),
            'total_synapses': self.brain.getTotalSynapseCount(),
            'curiosity': self.agent.getCuriosityLevel(),
            'novelty': self.agent.getNoveltyLevel(),
            'prediction_error': self.agent.getPredictionError(),
            'neuromodulation': self.agent.getNeuromodulationLevel()
        }
        
        self.simulation_data.append(data_point)
    
    def analyzeSimulationData(self):
        """Analyze simulation data using Pandas."""
        print("Analyzing simulation data...")
        
        df = pd.DataFrame(self.simulation_data)
        
        # Basic statistics
        print("\n=== Basic Statistics ===")
        print(df.describe())
        
        # Time series analysis
        print("\n=== Time Series Analysis ===")
        print("Firing neurons over time:")
        plt.figure(figsize=(12, 6))
        plt.plot(df['simulation_time'], df['firing_neurons'])
        plt.title('Firing Neurons over Time')
        plt.xlabel('Simulation Time')
        plt.ylabel('Firing Neurons')
        plt.show()
        
        # Correlation analysis
        print("\n=== Correlation Analysis ===")
        correlations = self.data_analyzer.calculateCorrelations(df)
        print("Correlations between variables:")
        for var1, var2, corr in correlations:
            print(f"  {var1} - {var2}: {corr:.3f}")
        
        # Statistical analysis
        print("\n=== Statistical Analysis ===")
        stats = self.statistical_analyzer.analyze(df)
        print("Statistical summaries:")
        for variable, summary in stats.items():
            print(f"  {variable}:")
            print(f"    Mean: {summary['mean']:.2f}")
            print(f"    Std: {summary['std']:.2f}")
            print(f"    Min: {summary['min']:.2f}")
            print(f"    Max: {summary['max']:.2f}")
```

### 4. JSON Integration

#### Configuration Management with JSON
```python
import pynlm
import json

class JSONConfigManager:
    """Configuration manager with JSON support."""
    
    def __init__(self, config_file=None):
        """Initialize JSON configuration manager."""
        self.config_file = config_file
        self.config = pynlm.createDefaultConfig()
        
        if config_file:
            self.loadFromJSON(config_file)
    
    def loadFromJSON(self, filename):
        """Load configuration from JSON file."""
        try:
            with open(filename, 'r') as f:
                config_data = json.load(f)
            
            # Convert JSON data to configuration
            for key, value in config_data.items():
                self.setConfigurationValue(key, value)
            
            print(f"Configuration loaded from {filename}")
            return True
            
        except Exception as e:
            print(f"Error loading configuration from {filename}: {e}")
            return False
    
    def saveToJSON(self, filename):
        """Save configuration to JSON file."""
        try:
            # Convert configuration to JSON
            config_data = self.convertConfigurationToJSON()
            
            with open(filename, 'w') as f:
                json.dump(config_data, f, indent=2)
            
            print(f"Configuration saved to {filename}")
            return True
            
        except Exception as e:
            print(f"Error saving configuration to {filename}: {e}")
            return False
    
    def setConfigurationValue(self, key, value):
        """Set configuration value from JSON data."""
        # Map JSON keys to configuration API
        key_mapping = {
            'neuron_count': 'neuron_count',
            'learning_rate': 'learning_rate',
            'enable_curiosity': 'enable_curiosity',
            'synapse_density': 'synapse_density',
            'simulation_steps': 'max_steps',
            'world_width': 'world_width',
            'world_height': 'world_height'
        }
        
        config_key = key_mapping.get(key, key)
        
        if config_key == 'neuron_count':
            self.config.setInt(config_key, value)
        elif config_key == 'learning_rate':
            self.config.setDouble(config_key, value)
        elif config_key == 'enable_curiosity':
            self.config.setBool(config_key, value)
        elif config_key == 'synapse_density':
            self.config.setDouble(config_key, value)
        elif config_key == 'max_steps':
            self.config.setInt(config_key, value)
        elif config_key == 'world_width':
            self.config.setInt(config_key, value)
        elif config_key == 'world_height':
            self.config.setInt(config_key, value)
        else:
            self.config.set(key, value)
    
    def convertConfigurationToJSON(self):
        """Convert configuration to JSON format."""
        # Get all configuration keys and values
        keys = self.config.getKeys()
        config_data = {}
        
        for key in keys:
            # Map configuration API keys to JSON keys
            json_key = key
            if key == 'neuron_count':
                json_key = 'neuron_count'
            elif key == 'learning_rate':
                json_key = 'learning_rate'
            elif key == 'enable_curiosity':
                json_key = 'enable_curiosity'
            elif key == 'synapse_density':
                json_key = 'synapse_density'
            elif key == 'max_steps':
                json_key = 'simulation_steps'
            elif key == 'world_width':
                json_key = 'world_width'
            elif key == 'world_height':
                json_key = 'world_height'
            
            # Get configuration value
            value = self.getConfigurationValue(key)
            config_data[json_key] = value
        
        return config_data
    
    def getConfigurationValue(self, key):
        """Get configuration value."""
        if key == 'neuron_count':
            return self.config.getInt(key)
        elif key == 'learning_rate':
            return self.config.getDouble(key)
        elif key == 'enable_curiosity':
            return self.config.getBool(key)
        elif key == 'synapse_density':
            return self.config.getDouble(key)
        elif key == 'max_steps':
            return self.config.getInt(key)
        elif key == 'world_width':
            return self.config.getInt(key)
        elif key == 'world_height':
            return self.config.getInt(key)
        else:
            return self.config.get(key)
    
    def createBrainFromJSON(self):
        """Create brain from JSON configuration."""
        return pynlm.createBrain(self.config)
```

### 5. Parallel Processing

#### Multithreaded Brain Processing
```python
import pynlm
import threading
import concurrent.futures

class MultithreadedBrain:
    """Multithreaded brain implementation for high-performance processing."""
    
    def __init__(self, config, num_threads=None):
        """Initialize multithreaded brain."""
        self.config = config
        self.num_threads = num_threads or max(1, threading.cpu_count() - 1)
        
        # Create thread pool
        self.thread_pool = concurrent.futures.ThreadPoolExecutor(max_workers=self.num_threads)
        self.task_queue = []
        self.results = []
    
    def processStepInThread(self, step):
        """Process single step in separate thread."""
        # Create brain in thread
        config_copy = pynlm.createDefaultConfig()
        config_copy.setInt("neuron_count", self.config.getInt("neuron_count"))
        config_copy.setDouble("learning_rate", self.config.getDouble("learning_rate"))
        config_copy.setBool("enable_curiosity", self.config.getBool("enable_curiosity"))
        
        brain = pynlm.createBrain(config_copy)
        brain.initialize()
        
        # Process step
        brain.step(step)
        
        # Get statistics
        return {
            'step': step,
            'firing_neurons': brain.getFiringNeuronCount(),
            'total_neurons': brain.getTotalNeuronCount(),
            'total_synapses': brain.getTotalSynapseCount()
        }
    
    def processBatchInParallel(self, steps):
        """Process batch of steps in parallel."""
        print(f"Processing {steps} steps with {self.num_threads} threads...")
        
        # Submit all steps to thread pool
        futures = []
        for step in range(steps):
            future = self.thread_pool.submit(self.processStepInThread, step)
            futures.append(future)
        
        # Collect results
        results = []
        for future in concurrent.futures.as_completed(futures):
            try:
                result = future.result()
                results.append(result)
                
                # Print progress
                if len(results) % 100 == 0:
                    print(f"Processed {len(results)} steps...")
                    
            except Exception as e:
                print(f"Error processing step: {e}")
        
        # Sort results by step
        results.sort(key=lambda x: x['step'])
        
        return pd.DataFrame(results)
    
    def cleanup(self):
        """Clean up thread pool."""
        self.thread_pool.shutdown(wait=True)
```

### 6. Context Managers

#### Resource Management with Context Managers
```python
import pynlm

class BrainContextManager:
    """Context manager for NLM brain resources."""
    
    def __init__(self, config=None):
        """Initialize brain context manager."""
        self.config = config or pynlm.createDefaultConfig()
        self.brain = None
        self.world = None
        self.agent = None
    
    def __enter__(self):
        """Enter context manager."""
        print("Initializing NLM components...")
        
        # Create brain
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        # Create world
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=20, height=20)
        self.world.reset()
        self.world.setAgentStart(10.0, 10.0)
        
        # Create agent
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable learning
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        
        print("NLM components initialized successfully")
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Exit context manager."""
        print("Cleaning up NLM resources...")
        
        # Clean up components
        if self.agent:
            del self.agent
        if self.world:
            del self.world
        if self.brain:
            del self.brain
        
        # Force garbage collection
        import gc
        gc.collect()
        
        print("NLM resources cleaned up")
    
    def runSimulation(self, steps):
        """Run simulation using context manager."""
        for step in range(steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Run brain
            self.brain.step(step)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            
            # Apply action
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Print progress
            if step % 100 == 0:
                print(f"Step {step}: {self.brain.getFiringNeuronCount()} firing neurons")
        
        return True

# Use context manager
def runWithContextManager(steps):
    """Run simulation using context manager."""
    with BrainContextManager() as nlm:
        return nlm.runSimulation(steps)
```

### 7. Event System

#### Event-Driven Brain Processing
```python
import pynlm
from typing import Callable, Dict, Any

class Event:
    """Event for event-driven processing."""
    
    def __init__(self, event_type: str, data: Dict[str, Any]):
        self.type = event_type
        self.data = data
        self.timestamp = time.time()
    
    def __str__(self):
        return f"Event(type={self.type}, data={self.data}, timestamp={self.timestamp})"
class EventManager:
    """Event manager for event-driven processing."""
    
    def __init__(self):
        self.listeners = {}
    
    def add_listener(self, event_type: str, callback: Callable):
        """Add event listener."""
        if event_type not in self.listeners:
            self.listeners[event_type] = []
        self.listeners[event_type].append(callback)
    
    def remove_listener(self, event_type: str, callback: Callable):
        """Remove event listener."""
        if event_type in self.listeners:
            self.listeners[event_type].remove(callback)
    
    def emit(self, event: Event):
        """Emit event to listeners."""
        if event.type in self.listeners:
            for callback in self.listeners[event.type]:
                callback(event)
class EventDrivenBrain:
    """Event-driven brain implementation."""
    
    def __init__(self, config):
        """Initialize event-driven brain."""
        self.config = config
        self.event_manager = EventManager()
        
        # Register event listeners
        self.event_manager.add_listener('spike', self.handleSpikeEvent)
        self.event_manager.add_listener('input', self.handleInputEvent)
        self.event_manager.add_listener('learning', self.handleLearningEvent)
        self.event_manager.add_listener('memory', self.handleMemoryEvent)
    
    def handleSpikeEvent(self, event):
        """Handle spike event."""
        neuron_id = event.data.get('neuron_id')
        print(f"Spike event: Neuron {neuron_id} spiked at time {event.timestamp}")
        
        # Update neuron state
        self.updateNeuronState(neuron_id)
    
    def handleInputEvent(self, event):
        """Handle input event."""
        neuron_id = event.data.get('neuron_id')
        input_current = event.data.get('current', 0.0)
        print(f"Input event: Neuron {neuron_id} received input {input_current}")
        
        # Update neuron state
        self.updateNeuronState(neuron_id, input_current)
    
    def handleLearningEvent(self, event):
        """Handle learning event."""
        reward = event.data.get('reward', 0.0)
        prediction = event.data.get('prediction', 0.0)
        print(f"Learning event: Reward {reward}, Prediction {prediction}")
        
        # Update learning state
        self.updateLearningState(reward, prediction)
    
    def handleMemoryEvent(self, event):
        """Handle memory event."""
        memory_type = event.data.get('type', 'working')
        memory_data = event.data.get('data', {})
        print(f"Memory event: {memory_type} memory operation")
        
        # Update memory state
        self.updateMemoryState(memory_type, memory_data)
    
    def addEvent(self, event_type: str, data: Dict[str, Any]):
        """Add event to event manager."""
        event = Event(event_type, data)
        self.event_manager.emit(event)
    
    def step(self, step_number):
        """Process one simulation step with event-driven processing."""
        current_time = step_number * 0.1
        
        # Schedule new events
        self.scheduleNewEvents(current_time)
        
        # Process events
        self.processEvents(current_time)
        
        # Update brain state
        self.updateBrainState(current_time)
    
    def scheduleNewEvents(self, current_time):
        """Schedule new events for next step."""
        # Random spike generation
        num_spikes = np.random.poisson(10)
        for i in range(num_spikes):
            self.addEvent('spike', {
                'neuron_id': np.random.randint(0, 100),
                'time': current_time
            })
        
        # Random input generation
        num_inputs = np.random.poisson(5)
        for i in range(num_inputs):
            self.addEvent('input', {
                'neuron_id': np.random.randint(0, 100),
                'current': np.random.normal(0, 1),
                'time': current_time
            })
        
        # Random learning events
        num_learning = np.random.poisson(2)
        for i in range(num_learning):
            self.addEvent('learning', {
                'reward': np.random.random(),
                'prediction': np.random.random(),
                'time': current_time
            })
    
    def processEvents(self, current_time):
        """Process events from event queue."""
        # This would normally have a separate event queue
        # For simplicity, we process events directly
        pass
    
    def updateNeuronState(self, neuron_id, input_current=0.0):
        """Update neuron state."""
        # Update neuron membrane potential
        # Apply input current
        # Check for spike
        # Update refractory state
        pass
    
    def updateLearningState(self, reward, prediction):
        """Update learning state."""
        # Update learning parameters
        # Apply reward modulation
        pass
    
    def updateMemoryState(self, memory_type, memory_data):
        """Update memory state."""
        # Update memory state
        # Store memory data
        pass
```

### 8. Custom Brain Implementation

#### Custom Brain Class
```python
import pynlm

class CustomBrain(pynlm.Brain):
    """Custom brain implementation with extended functionality."""
    
    def __init__(self, config):
        """Initialize custom brain."""
        super().__init__(config)
        
        # Custom brain parameters
        self.custom_neurons = []
        self.custom_plasticity = True
        self.custom_learning = True
    
    def initialize(self):
        """Initialize custom brain."""
        super().initialize()
        
        # Add custom neurons
        self.setupCustomNeurons()
        
        # Configure custom plasticity
        self.configureCustomPlasticity()
    
    def setupCustomNeurons(self):
        """Setup custom neurons."""
        for i in range(self.getTotalNeuronCount()):
            if i % 10 == 0:  # Every 10th neuron is custom
                self.custom_neurons.append({
                    'id': i,
                    'type': 'custom',
                    'properties': {
                        'plasticity': self.custom_plasticity,
                        'learning': self.custom_learning,
                        'special_function': self.getCustomFunction(i)
                    }
                })
    
    def getCustomFunction(self, neuron_id):
        """Get custom function for neuron."""
        # Implement custom neuron function
        # This could be spiking behavior, plasticity rules, etc.
        return lambda: self.customNeuronFunction(neuron_id)
    
    def customNeuronFunction(self, neuron_id):
        """Custom neuron function."""
        # Implement custom neuron behavior
        # This could be different from standard NLM neuron dynamics
        return np.tanh(neuron_id / 10.0)
    
    def processStep(self, step_number):
        """Process custom simulation step."""
        super().processStep(step_number)
        
        # Apply custom processing
        self.processCustomNeurons(step_number)
        self.applyCustomLearning(step_number)
    
    def processCustomNeurons(self, step_number):
        """Process custom neurons."""
        for neuron in self.custom_neurons:
            if neuron['type'] == 'custom':
                # Apply custom dynamics
                self.customDynamics(neuron, step_number)
    
    def customDynamics(self, neuron, step_number):
        """Apply custom neural dynamics."""
        # Implement custom neural dynamics
        # This could be different from standard NLM dynamics
        neuron['activity'] = np.tanh(neuron['activity'])
        neuron['plasticity'] *= 1.0 - neuron['activity']
    
    def applyCustomLearning(self, step_number):
        """Apply custom learning."""
        # Apply custom learning rules
        if step_number % 100 == 0:
            # Periodic learning update
            self.updateCustomPlasticity()
    
    def updateCustomPlasticity(self):
        """Update custom plasticity."""
        for neuron in self.custom_neurons:
            neuron['plasticity'] *= 0.95
```

### 9. Integration with External Libraries

#### Integration with Machine Learning Libraries
```python
import pynlm
import numpy as np
import pandas as pd
from sklearn.preprocessing import StandardScaler
from sklearn.model_selection import train_test_split
from sklearn.neural_network import MLPClassifier

class IntegrationBrain:
    """Brain with integration capabilities for external libraries."""
    
    def __init__(self, config):
        """Initialize integration brain."""
        self.config = config
        self.setupIntegration()
        
        # Create integration components
        self.setupIntegrationComponents()
    
    def setupIntegration(self):
        """Set up integration features."""
        # Enable integration with external libraries
        self.numpy_integration = True
        self.sklearn_integration = True
        self.pandas_integration = True
    
    def setupIntegrationComponents(self):
        """Set up integration components."""
        # Create data collection for external libraries
        self.simulation_data = []
        
        # Create sklearn models
        self.scaler = StandardScaler()
        self.mlp_classifier = MLPClassifier(hidden_layer_sizes=(100,), max_iter=1000)
        
        # Create pandas DataFrames for analysis
        self.simulation_df = pd.DataFrame()
    
    def runSimulationWithIntegration(self, steps):
        """Run simulation with integration to external libraries."""
        print(f"Running simulation with external library integration...")
        
        for step in range(steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Run brain
            self.brain.step(step)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            
            # Apply action
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Collect simulation data
            self.collectSimulationData(step)
            
            # Train ML model with simulation data
            if step >= 100:
                self.trainMLModel()
            
            # Print progress
            if step % 100 == 0:
                print(f"Step {step:5d}: Data collected: {len(self.simulation_data):5d} points")
        
        return self.simulation_df
    
    def collectSimulationData(self, step):
        """Collect simulation data for external library integration."""
        data_point = {
            'step': step,
            'simulation_time': self.world.getSimulationTime(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'total_neurons': self.brain.getTotalNeuronCount(),
            'total_synapses': self.brain.getTotalSynapseCount(),
            'curiosity': self.agent.getCuriosityLevel(),
            'novelty': self.agent.getNoveltyLevel(),
            'prediction_error': self.agent.getPredictionError(),
            'neuromodulation': self.agent.getNeuromodulationLevel()
        }
        
        self.simulation_data.append(data_point)
        self.simulation_df = pd.DataFrame(self.simulation_data)
    
    def trainMLModel(self):
        """Train machine learning model with simulation data."""
        # Prepare data for ML training
        if len(self.simulation_df) >= 100:
            # Select features
            features = ['firing_neurons', 'curiosity', 'novelty', 'prediction_error', 'neuromodulation']
            target = 'total_neurons'  # Example target
            
            X = self.simulation_df[features]
            y = self.simulation_df[target]
            
            # Split data
            X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)
            
            # Scale features
            X_train_scaled = self.scaler.fit_transform(X_train)
            X_test_scaled = self.scaler.transform(X_test)
            
            # Train model
            self.mlp_classifier.fit(X_train_scaled, y_train)
            
            # Evaluate model
            accuracy = self.mlp_classifier.score(X_test_scaled, y_test)
            print(f"    ML model accuracy: {accuracy:.3f}")
```

### 10. Production-Ready Application

#### Production-Ready NLM Application
```python
import pynlm
import time
import logging
from typing import Dict, Any, Callable

class ProductionReadyNLM:
    """Production-ready NLM application with comprehensive features."""
    
    def __init__(self, config=None):
        """Initialize production-ready NLM."""
        self.config = config or pynlm.createDefaultConfig()
        self.setupProductionFeatures()
        
        # Initialize logging
        self.setupLogging()
        
        # Initialize monitoring
        self.setupMonitoring()
        
        # Initialize error handling
        self.setupErrorHandling()
    
    def setupProductionFeatures(self):
        """Set up production features."""
        # Enable production features
        self.production_enabled = True
        self.error_handling_enabled = True
        self.monitoring_enabled = True
        self.logging_enabled = True
        
        # Configure production settings
        self.max_steps = 100000
        self.checkpoint_interval = 1000
        self.performance_threshold = 0.8
    
    def setupLogging(self):
        """Set up logging for production."""
        # Configure logging
        logging.basicConfig(
            level=logging.INFO,
            format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
            handlers=[
                logging.FileHandler('nlm_production.log'),
                logging.StreamHandler()
            ]
        )
        
        self.logger = logging.getLogger('nlm_production')
    
    def setupMonitoring(self):
        """Set up monitoring for production."""
        self.metrics = {
            'steps_processed': 0,
            'total_time': 0.0,
            'steps_per_second': 0.0,
            'memory_usage': 0.0,
            'cpu_usage': 0.0,
            'errors': 0,
            'warnings': 0
        }
    
    def setupErrorHandling(self):
        """Set up error handling for production."""
        self.error_handler = ErrorHandler()
        self.error_handler.setErrorCallback(self.handleError)
    
    def handleError(self, error):
        """Handle errors in production."""
        self.logger.error(f"Error: {error}")
        self.metrics['errors'] += 1
        
        # Take appropriate action based on error type
        if 'critical' in str(error).lower():
            self.logger.critical(f"Critical error: {error}")
            # Take emergency action
            self.emergencyAction(error)
    
    def emergencyAction(self, error):
        """Take emergency action for critical errors."""
        # Implement emergency shutdown or recovery
        self.logger.error("Taking emergency action due to critical error")
        # This could include saving current state, shutting down gracefully, etc.
    
    def runProductionSimulation(self, steps=None):
        """Run production simulation with comprehensive monitoring."""
        if steps is None:
            steps = self.max_steps
        
        print(f"Starting production simulation with {steps} steps...")
        
        try:
            # Initialize components
            self.brain = pynlm.createBrain(self.config)
            self.brain.initialize()
            
            self.world = pynlm.createSimpleWorld()
            self.world.configure(width=20, height=20)
            self.world.reset()
            self.world.setAgentStart(10.0, 10.0)
            
            self.agent = pynlm.createAgentBrain(self.brain)
            self.agent.initialize(self.world)
            
            # Enable all learning features
            self.agent.enableRewardModulation(True)
            self.agent.enableCuriosity(True)
            self.agent.enableDevelopment(True)
            
            # Start monitoring
            start_time = time.time()
            
            # Run simulation
            for step in range(steps):
                # Update world
                self.world.update(0.1)
                
                # Get sensory input
                percept = self.world.getSensoryPercept()
                
                # Process sensory input
                self.agent.processSensoryInput(percept)
                
                # Run brain
                self.brain.step(step)
                
                # Get action
                action = self.agent.decodeMotorCommand()
                
                # Apply action
                self.world.applyMotorCommand(action, self.world.getSimulationTime())
                
                # Update metrics
                self.updateMetrics(step)
                
                # Print progress
                if step % 1000 == 0:
                    self.printProgress(step)
                
                # Take checkpoint
                if step % self.checkpoint_interval == 0:
                    self.takeCheckpoint(step)
            
            # Calculate final metrics
            end_time = time.time()
            total_time = end_time - start_time
            self.metrics['total_time'] = total_time
            self.metrics['steps_per_second'] = steps / total_time if total_time > 0 else 0
            
            print(f"Production simulation completed in {total_time:.2f}s")
            print(f"Performance: {self.metrics['steps_per_second']:.0f} steps/s")
            
            return True
            
        except Exception as e:
            self.handleError(f"Production simulation failed: {e}")
            return False
    
    def updateMetrics(self, step):
        """Update performance metrics."""
        self.metrics['steps_processed'] = step + 1
        
        # Get system resource usage
        import psutil
        process = psutil.Process()
        
        # Get memory usage
        self.metrics['memory_usage'] = process.memory_info().rss / 1024 / 1024  # MB
        
        # Get CPU usage
        self.metrics['cpu_usage'] = psutil.cpu_percent()
    
    def printProgress(self, step):
        """Print progress information."""
        print(f"Step {step:6d}/{self.max_steps}: "
              f"Progress: {step/self.max_steps*100:5.1f}% | "
              f"Speed: {self.metrics['steps_per_second']:.0f} steps/s | "
              f"Memory: {self.metrics['memory_usage']:.1f} MB | "
              f"CPU: {self.metrics['cpu_usage']:.1f}%")
    
    def takeCheckpoint(self, step):
        """Take checkpoint for recovery."""
        checkpoint_file = f"checkpoint_step_{step}.bin"
        self.brain.save(checkpoint_file)
        print(f"Checkpoint saved: {checkpoint_file}")
    
    def getPerformanceReport(self):
        """Get performance report."""
        report = {
            'total_steps': self.max_steps,
            'steps_processed': self.metrics['steps_processed'],
            'total_time': self.metrics['total_time'],
            'steps_per_second': self.metrics['steps_per_second'],
            'memory_usage': self.metrics['memory_usage'],
            'cpu_usage': self.metrics['cpu_usage'],
            'errors': self.metrics['errors'],
            'warnings': self.metrics['warnings'],
            'performance_rating': self.calculatePerformanceRating()
        }
        
        return report
    
    def calculatePerformanceRating(self):
        """Calculate overall performance rating."""
        steps_per_second_rating = min(100, self.metrics['steps_per_second'] / 100)
        memory_rating = max(0, 100 - self.metrics['memory_usage'] / 10)
        cpu_rating = max(0, 100 - self.metrics['cpu_usage'])
        
        return (steps_per_second_rating + memory_rating + cpu_rating) / 3
```

# Use production-ready NLM
if __name__ == "__main__":
    # Create production-ready NLM
    nlm = ProductionReadyNLM()
    
    # Run production simulation
    success = nlm.runProductionSimulation(10000)
    
    if success:
        # Get performance report
        report = nlm.getPerformanceReport()
        
        print("\n=== Production Performance Report ===")
        for key, value in report.items():
            print(f"{key.replace('_', ' ').title()}: {value:.2f if isinstance(value, float) else value}")
        
        print("\n=== Production Simulation Complete! ===")
    else:
        print("\n=== Production Simulation Failed ===")
```

## Conclusion

This tutorial has covered advanced NLM Python features, including:

1. **Async Support**: High-performance async processing for large-scale simulations
2. **NumPy Integration**: NumPy-optimized neural computation
3. **Pandas Integration**: Data analysis with Pandas
4. **JSON Integration**: Configuration management with JSON
5. **Parallel Processing**: Multithreaded processing for performance
6. **Context Managers**: Resource management for production
7. **Event System**: Event-driven processing with callbacks
8. **Custom Brain Implementation**: Custom brain extensions
9. **External Library Integration**: Integration with machine learning libraries
10. **Production-Ready Application**: Comprehensive production-ready NLM application

By following these advanced Python integration features, you can:

- Build high-performance NLM applications
- Integrate with external data science libraries
- Implement event-driven processing
- Create custom brain implementations
- Build production-ready NLM systems
- Achieve maximum performance and efficiency

Next steps:
- **Tutorial 6**: Learn real-world application development
- **Documentation**: Explore comprehensive documentation for advanced topics
- **Testing**: Test your NLM applications thoroughly

The NLM Python bindings provide comprehensive integration capabilities for building high-performance, production-ready NLM applications. By leveraging these advanced features, you can create sophisticated neural simulation systems that integrate seamlessly with modern data science workflows.

Remember:
- Use async support for high-performance applications
- Integrate with external libraries for advanced functionality
- Implement context managers for resource management
- Use event-driven processing for complex interactions
- Build custom brain implementations for specialized tasks
- Create production-ready applications with comprehensive error handling

Happy coding!
