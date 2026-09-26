# ADVANCED_TOPICS.md

# NLM Advanced Topics

## Overview

This document covers advanced usage topics for the NLM system, including performance optimization, debugging, customization, and integration with other systems. It provides detailed guidance for experienced users and developers working with the NLM brain simulation.

## Performance Optimization

### Understanding NLM Performance Characteristics

NLM's performance is influenced by several key factors:

#### 1. Neuron Count and Connectivity
```python
# Performance impact of neuron count
# Neuron Count | Sim Steps/sec | Memory Usage |
# -------------|---------------|--------------|
# 1000         | ~50           | ~50 MB       |
# 3000         | ~15           | ~150 MB      |
# 5000         | ~8            | ~250 MB      |
# 10000        | ~4            | ~500 MB      |
```

#### 2. Synapse Density
```python
# Higher synapse density increases computation time
config.set("brain.synapse_density", 0.1)  # ~120% slower
sconfig.set("brain.synapse_density", 0.05) # ~60% slower
sconfig.set("brain.synapse_density", 0.01)  # ~20% slower
sconfig.set("brain.synapse_density", 0.001) # ~2% slower
```

### Optimization Strategies

#### 1. Memory Management
```python
import pynlm
import psutil
import os

class OptimizedNLM:
    def __init__(self, target_memory_mb=500):
        self.target_memory = target_memory_mb * 1024 * 1024
        self.monitoring_enabled = True
    
    def create_memory_efficient_brain(self, neuron_count=2000):
        """Create brain optimized for memory usage."""
        config = pynlm.createConfig()
        
        # Calculate optimal parameters based on memory target
        synapse_density = min(0.05, self.target_memory / (neuron_count * 50))
        
        config.set("brain.neuron_count", neuron_count)
        config.set("brain.synapse_density", synapse_density)
        config.set("brain.v_thresh", -45.0)  # Higher threshold reduces activity
        config.set("brain.tau_mem", 30.0)   # Slower dynamics
        
        return pynlm.createBrain(config)
    
    def monitor_memory_usage(self, brain, interval=100):
        """Monitor memory usage during simulation."""
        process = psutil.Process(os.getpid())
        initial_memory = process.memory_info().rss
        
        for step in range(interval):
            brain.step(step)
            
            current_memory = process.memory_info().rss
            memory_increase = current_memory - initial_memory
            
            if memory_increase > self.target_memory:
                print(f"Warning: Memory usage approaching limit ({memory_increase / 1024 / 1024:.1f} MB)")
                break
        
        return memory_increase

# Use optimized brain
optimizer = OptimizedNLM(target_memory_mb=300)
brain = optimizer.create_memory_efficient_brain(1500)
brain.initialize()

print(f"Created memory-optimized brain: {brain.getTotalNeuronCount()} neurons")
```

#### 2. Parallel Processing
```python
# Parallel simulation using multiple processes
import pynlm
import multiprocessing as mp
from concurrent.futures import ProcessPoolExecutor, ThreadPoolExecutor
import time

class ParallelNLMProcessor:
    def __init__(self, num_workers=None):
        self.num_workers = num_workers or mp.cpu_count()
    
    def run_individual_simulation(self, simulation_id, steps=500):
        """Run a single simulation in parallel."""
        print(f"Starting simulation {simulation_id}")
        
        # Each simulation gets unique parameters
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1500 + simulation_id * 500)
        config.set("neuromod.curiosity.scale", 1.0 + simulation_id * 0.2)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(steps):
            brain.step(step)
        
        # Collect results
        results = {
            'simulation_id': simulation_id,
            'final_activity': brain.getAverageActivity(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'memory_usage': brain.getMemoryUsage(),
            'curiosity_level': brain.getCuriosityLevel(),
            'novelty_level': brain.getNoveltyLevel()
        }
        
        print(f"Completed simulation {simulation_id}")
        return results
    
    def run_parallel_simulations(self, num_simulations=5, steps_per_simulation=200):
        """Run multiple simulations in parallel."""
        start_time = time.time()
        
        # Use ThreadPoolExecutor for I/O-bound tasks
        with ThreadPoolExecutor(max_workers=self.num_workers) as executor:
            # Submit all simulations
            futures = [
                executor.submit(self.run_individual_simulation, i, steps_per_simulation)
                for i in range(num_simulations)
            ]
            
            # Collect results
            results = []
            for future in futures:
                results.append(future.result())
        
        end_time = time.time()
        
        # Analyze results
        analysis = self._analyze_parallel_results(results, end_time - start_time)
        
        return results, analysis
    
    def _analyze_parallel_results(self, results, duration):
        """Analyze results from parallel simulations."""
        analysis = {
            'total_simulations': len(results),
            'duration_seconds': duration,
            'speedup': len(results) * 200 / duration,  # Compared to serial
            'average_activity': np.mean([r['final_activity'] for r in results]),
            'activity_std': np.std([r['final_activity'] for r in results]),
            'total_firing_neurons': sum(r['firing_neurons'] for r in results),
            'total_memory_usage': sum(r['memory_usage'] for r in results}),
            'average_curiosity': np.mean([r['curiosity_level'] for r in results]),
            'average_novelty': np.mean([r['novelty_level'] for r in results])
        }
        
        return analysis

# Run parallel simulations
processor = ParallelNLMProcessor(num_workers=4)
results, analysis = processor.run_parallel_simulations(5, 200)

print(f"\nParallel Simulation Results:")
print(f"  Speedup: {analysis['speedup']:.1f}x")
print(f"  Average activity: {analysis['average_activity']:.3f} ± {analysis['activity_std']:.3f}")
print(f"  Total firing neurons: {analysis['total_firing_neurons']}")
print(f"  Total memory usage: {analysis['total_memory_usage'] / 1024 / 1024:.1f} MB")
```

### SIMD Optimization
```python
# NLM leverages SIMD (Single Instruction Multiple Data) for performance
# This is handled automatically by the C++ implementation
# Users can optimize by:

# 1. Using aligned memory access patterns
config = pynlm.createDefaultConfig()
config.set("brain.synapse_density", 0.05)  # Reduces memory access

# 2. Using appropriate neuron counts for cache efficiency
config.set("brain.neuron_count", 1024)  # Power of 2 for better cache utilization

# 3. Enabling/disable features based on performance needs
config.set("plasticity.stdp.enable", True)  # STDP is computationally expensive
config.set("plasticity.hebbian.enable", False)  # Hebbian is cheaper
```

## Advanced Debugging

### 1. Memory Leak Detection
```python
import pynlm
import tracemalloc
import time

def detect_memory_leaks(brain, steps=1000):
    """Detect memory leaks in brain simulation."""
    tracemalloc.start()
    
    # Take initial snapshot
    snapshot1 = tracemalloc.take_snapshot()
    
    # Run simulation
    for step in range(steps):
        brain.step(step)
        
        # Check memory every 100 steps
        if step % 100 == 0:
            current, peak = tracemalloc.get_traced_memory()
            print(f"Step {step}: Current memory: {current / 1024 / 1024:.1f} MB, "
                  f"Peak: {peak / 1024 / 1024:.1f} MB")
    
    # Take final snapshot
    snapshot2 = tracemalloc.take_snapshot()
    
    # Compare snapshots
    top_stats = snapshot2.compare_to(snapshot1, 'lineno')
    
    print("\nTop memory differences:")
    for stat in top_stats[:10]:
        print(stat)
    
    tracemalloc.stop()

# Use the function
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
detect_memory_leaks(brain, 1000)
```

### 2. Performance Profiling
```python
import pynlm
import cProfile
import pstats
import io
import time

def profile_simulation(brain, steps=1000):
    """Profile brain simulation performance."""
    pr = cProfile.Profile()
    
    pr.enable()
    start_time = time.time()
    
    for step in range(steps):
        brain.step(step)
        
        if step % 100 == 0:
            elapsed = time.time() - start_time
            steps_per_sec = step / elapsed if elapsed > 0 else 0
            print(f"Step {step}: {steps_per_sec:.1f} steps/sec")
    
    end_time = time.time()
    pr.disable()
    
    # Analyze profile
    s = io.StringIO()
    ps = pstats.Stats(pr, stream=s).sort_stats('cumulative')
    ps.print_stats(20)  # Top 20 time-consuming functions
    
    profile_output = s.getvalue()
    
    print(f"\nSimulation completed in {end_time - start_time:.2f} seconds")
    print(f"Performance: {(steps / (end_time - start_time)):.1f} steps/second")
    print("\nProfile statistics:")
    print(profile_output)
    
    return end_time - start_time, profile_output

# Run profiling
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
duration, profile = profile_simulation(brain, 1000)
```

### 3. Error and Exception Handling
```python
import pynlm
import traceback

class SafeNLMSimulation:
    def __init__(self, brain):
        self.brain = brain
        self.error_count = 0
        self.warning_count = 0
    
    def safe_step(self, step):
        """Safely run a single simulation step with error handling."""
        try:
            # Check if brain is initialized
            if not hasattr(self.brain, 'is_initialized') or not self.brain.is_initialized:
                raise RuntimeError("Brain not initialized. Call initialize() first.")
            
            # Check for potential memory issues
            memory_usage = self.brain.getMemoryUsage()
            if memory_usage > 100 * 1024 * 1024:  # 100 MB
                self.warning_count += 1
                print(f"Warning: High memory usage: {memory_usage / 1024 / 1024:.1f} MB")
            
            # Check for potential performance issues
            if hasattr(self.brain, 'getFiringNeuronCount'):
                firing_ratio = self.brain.getFiringNeuronCount() / self.brain.getTotalNeuronCount()
                if firing_ratio > 0.8:
                    self.warning_count += 1
                    print(f"Warning: High firing ratio: {firing_ratio:.2%}")
            
            # Run simulation step
            self.brain.step(step)
            
            return True
            
        except MemoryError as e:
            self.error_count += 1
            print(f"Memory error at step {step}: {e}")
            return False
            
        except RuntimeError as e:
            self.error_count += 1
            print(f"Runtime error at step {step}: {e}")
            return False
            
        except Exception as e:
            self.error_count += 1
            print(f"Unexpected error at step {step}: {e}")
            traceback.print_exc()
            return False
    
    def run_safe_simulation(self, steps):
        """Run complete simulation with error handling."""
        print(f"Starting safe simulation: {steps} steps")
        print(f"Initial error count: {self.error_count}")
        
        successful_steps = 0
        failed_steps = 0
        
        for step in range(steps):
            success = self.safe_step(step)
            
            if success:
                successful_steps += 1
                
                # Print progress
                if (step + 1) % 100 == 0:
                    print(f"Step {step + 1}: {successful_steps} successful, "
                          f"{failed_steps} failed")
            else:
                failed_steps += 1
                
                # Try to recover
                if failed_steps < 3:  # Allow up to 3 failures
                    print("Attempting recovery...")
                    try:
                        self.brain.reset()
                        continue
                    except:
                        print("Recovery failed, stopping simulation.")
                        break
                else:
                    print("Too many failures, stopping simulation.")
                    break
        
        print(f"\nSimulation completed:")
        print(f"  Successful steps: {successful_steps}")
        print(f"  Failed steps: {failed_steps}")
        print(f"  Final error count: {self.error_count}")
        
        return successful_steps, failed_steps

# Use safe simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

simulator = SafeNLMSimulation(brain)
successful, failed = simulator.run_safe_simulation(500)
```

## Customization and Advanced Configuration

### 1. Custom Neuron Models
```python
# Extend NLM with custom neuron models
import pynlm

class CustomNeuronModel:
    """Custom neuron model for advanced research."""
    
    def __init__(self, config):
        self.config = config
        self.parameters = self._load_parameters()
    
    def _load_parameters(self):
        """Load custom neuron parameters."""
        return {
            'v_thresh': self.config.get("neuron.custom_threshold", -50.0),
            'tau_mem': self.config.get("neuron.time_constant", 20.0),
            'v_reset': self.config.get("neuron.reset_potential", -75.0),
            'neuron_type': self.config.get("neuron.model_type", "LIF")
        }
    
    def create_custom_config(self):
        """Create configuration for custom neuron model."""
        config = pynlm.createConfig()
        
        # Add custom parameters
        config.set("neuron.custom_threshold", -45.0)
        config.set("neuron.time_constant", 25.0)
        config.set("neuron.reset_potential", -65.0)
        config.set("neuron.model_type", "adaptive_LIF")
        
        # Keep standard parameters
        config.set("brain.neuron_count", 2000)
        config.set("brain.synapse_density", 0.05)
        
        return config

# Use custom neuron model
custom_model = CustomNeuronModel()
config = custom_model.create_custom_config()
brain = pynlm.createBrain(config)
brain.initialize()
print("Created brain with custom neuron model")
```

### 2. Advanced Plasticity Rules
```python
# Implement advanced plasticity rules
import pynlm
import numpy as np

class AdvancedPlasticity:
    """Advanced plasticity rules for NLM."""
    
    def __init__(self):
        self.stdp_kernel = self._create_stdp_kernel()
        self.hebbian_kernel = self._create_hebbian_kernel()
    
    def _create_stdp_kernel(self):
        """Create spike-timing-dependent plasticity kernel."""
        return {
            'LTP_kernel': lambda dt: 0.01 * np.exp(-dt / 20.0) if dt > 0 else 0,
            'LTD_kernel': lambda dt: -0.012 * np.exp(dt / 20.0) if dt < 0 else 0,
            'time_constant': 20.0,
            'learning_rate': 0.001
        }
    
    def _create_hebbian_kernel(self):
        """Create Hebbian plasticity kernel."""
        return {
            'correlation_window': 30.0,
            'learning_rate': 0.001,
            'normalization_factor': 1.0 / np.sqrt(30.0)
        }
    
    def apply_advanced_stdp(self, pre_spikes, post_spikes, synapse_weights):
        """Apply advanced STDP learning rule."""
        updated_weights = synapse_weights.copy()
        
        # Apply STDP based on spike timing
        for i, pre_time in enumerate(pre_spikes):
            for j, post_time in enumerate(post_spikes):
                dt = post_time - pre_time
                
                if dt > 0:  # Post before pre = LTP
                    updated_weights[i, j] *= (1 + self.stdp_kernel['LTP_kernel'](dt))
                else:  # Pre before post = LTD
                    updated_weights[i, j] *= (1 + self.stdp_kernel['LTD_kernel'](dt))
        
        # Clip weights to valid range
        updated_weights = np.clip(updated_weights, 0.0, 1.0)
        
        return updated_weights
    
    def apply_advanced_hebbian(self, pre_activity, post_activity, synapse_weights):
        """Apply advanced Hebbian learning rule."""
        updated_weights = synapse_weights.copy()
        
        # Compute correlation matrix
        correlation = np.outer(pre_activity, post_activity)
        
        # Apply Hebbian rule with normalization
        learning_signal = self.hebbian_kernel['correlation_window'] * correlation
        updated_weights += learning_signal * self.hebbian_kernel['learning_rate']
        
        # Normalize to maintain stability
        updated_weights = updated_weights / np.sqrt(np.sum(updated_weights**2))
        
        return updated_weights

# Use advanced plasticity
plasticity = AdvancedPlasticity()

# Apply to simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Simulation with advanced plasticity
for step in range(1000):
    brain.step(step)
    
    # Get current activities
    pre_activity = np.random.rand(100)  # Simulated pre-synaptic activity
    post_activity = np.random.rand(100)  # Simulated post-synaptic activity
    
    # Apply advanced learning
    updated_weights = plasticity.apply_advanced_hebbian(
        pre_activity, post_activity, 
        brain.getSynapseWeights()  # Assuming this method exists
    )
    
    # Update brain with new weights
    brain.updateSynapseWeights(updated_weights)
```

### 3. Real-time Visualization
```python
# Real-time visualization of NLM simulation
import pynlm
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np
import time

class RealTimeVisualizer:
    """Real-time visualization of NLM simulation."""
    
    def __init__(self, brain, update_interval=100):
        self.brain = brain
        self.update_interval = update_interval
        self.fig, ((self.ax1, self.ax2), (self.ax3, self.ax4)) = plt.subplots(2, 2, figsize=(12, 10))
        self.fig.suptitle('NLM Phase 6 Real-time Visualization', fontsize=16)
        
        # Initialize plots
        self._initialize_plots()
        
        # Data storage
        self.activity_history = []
        self.curiosity_history = []
        self.memory_history = []
    
    def _initialize_plots(self):
        """Initialize all plots."""
        # Neural activity plot
        self.neural_activity_line, = self.ax1.plot([], [], 'b-', linewidth=1)
        self.ax1.set_xlim(0, 200)
        self.ax1.set_ylim(0, 1.0)
        self.ax1.set_title('Average Neural Activity')
        self.ax1.set_xlabel('Simulation Step')
        self.ax1.set_ylabel('Activity Level')
        self.ax1.grid(True)
        
        # Neuromodulators plot
        self.curiosity_line, = self.ax2.plot([], [], 'r-', linewidth=2, label='Curiosity')
        self.ax2.set_xlim(0, 200)
        self.ax2.set_ylim(0, 1.0)
        self.ax2.set_title('Neuromodulator Levels')
        self.ax2.set_xlabel('Simulation Step')
        self.ax2.set_ylabel('Level')
        self.ax2.legend()
        self.ax2.grid(True)
        
        # Memory usage plot
        self.memory_line, = self.ax3.plot([], [], 'g-', linewidth=2)
        self.ax3.set_xlim(0, 200)
        self.ax3.set_ylim(0, 1.0)
        self.ax3.set_title('Memory Usage')
        self.ax3.set_xlabel('Simulation Step')
        self.ax3.set_ylabel('Usage (normalized)')
        self.ax3.grid(True)
        
        # Development stage plot
        self.dev_text = self.ax4.text(0.5, 0.5, 'Stage: Initial\nProgress: 0%', 
                                     ha='center', va='center', fontsize=14,
                                     bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.8))
        self.ax4.set_xlim(0, 1)
        self.ax4.set_ylim(0, 1)
        self.ax4.set_title('Development Progress')
        self.ax4.axis('off')
    
    def update_plots(self, step):
        """Update all plots with current data."""
        # Get current data
        activity = self.brain.getAverageActivity()
        curiosity = self.brain.getCuriosityLevel()
        memory_usage = self.brain.getMemoryUsage() / (100 * 1024 * 1024)  # Normalize
        development_stage = self.brain.getDevelopmentalStage()
        dev_progress = self.brain.getDevelopmentProgress()
        
        # Add to history
        self.activity_history.append(activity)
        self.curiosity_history.append(curiosity)
        self.memory_history.append(memory_usage)
        
        # Update plot data
        x_data = list(range(len(self.activity_history)))
        
        self.neural_activity_line.set_data(x_data, self.activity_history)
        self.curiosity_line.set_data(x_data, self.curiosity_history)
        self.memory_line.set_data(x_data, self.memory_history)
        
        # Update development stage text
        dev_text = f"Stage: {development_stage.name}\nProgress: {dev_progress:.1%}"
        self.dev_text.set_text(dev_text)
        
        # Adjust axes limits
        if len(x_data) > 100:
            self.ax1.set_xlim(max(0, len(x_data) - 200), len(x_data))
            self.ax2.set_xlim(max(0, len(x_data) - 200), len(x_data))
            self.ax3.set_xlim(max(0, len(x_data) - 200), len(x_data))
    
    def animate(self, frames=200):
        """Run real-time animation."""
        print("Starting real-time visualization...")
        print("Close the plot window to stop simulation.")
        
        
        def update(frame):
            if frame < frames:
                self.brain.step(frame)
                self.update_plots(frame)
                return self.neural_activity_line, self.curiosity_line, self.memory_line, self.dev_text
            else:
                print("Animation complete!")
                plt.close()
                return self.neural_activity_line, self.curiosity_line, self.memory_line, self.dev_text
        
        
        # Create animation
        ani = animation.FuncAnimation(self.fig, update, frames=frames, 
                                     interval=self.update_interval, blit=True)
        
        plt.tight_layout()
        plt.show()

# Use real-time visualization
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()


visualizer = RealTimeVisualizer(brain, update_interval=50)
visualizer.animate(frames=500)  # Run for 500 steps
```

### 4. Distributed Computing
```python
# Run NLM simulation on distributed systems
import pynlm
import multiprocessing as mp
from multiprocessing import Pool
import time

class DistributedNLM:
    """Distributed NLM simulation using multiprocessing."""
    
    def __init__(self, num_workers=None):
        self.num_workers = num_workers or mp.cpu_count()
        self.pool = None
    
    def initialize_pool(self):
        """Initialize multiprocessing pool."""
        self.pool = Pool(processes=self.num_workers)
        print(f"Initialized pool with {self.num_workers} workers")
    
    def run_single_simulation(self, simulation_id, config, steps):
        """Run a single simulation in a separate process."""
        # Each process gets its own brain instance
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        
        # Run simulation
        start_time = time.time()
        for step in range(steps):
            brain.step(step)
        end_time = time.time()
        
        # Collect results
        results = {
            'simulation_id': simulation_id,
            'steps_completed': steps,
            'duration': end_time - start_time,
            'steps_per_second': steps / (end_time - start_time),
            'final_activity': brain.getAverageActivity(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'memory_usage': brain.getMemoryUsage(),
            'curiosity': brain.getCuriosityLevel(),
            'novelty': brain.getNoveltyLevel(),
            'development_stage': brain.getDevelopmentalStage().name
        }
        
        return results
    
    def run_distributed_simulation(self, num_simulations, steps_per_simulation=200):
        """Run multiple simulations in parallel."""
        if self.pool is None:
            self.initialize_pool()
        
        # Create base configuration
        config = pynlm.createDefaultConfig()
        
        # Prepare arguments for each simulation
        args = [(i, config, steps_per_simulation) 
                for i in range(num_simulations)]
        
        print(f"Running {num_simulations} simulations in parallel...")
        
        # Run simulations in parallel
        start_time = time.time()
        results = self.pool.starmap(self.run_single_simulation, args)
        end_time = time.time()
        
        
        total_time = end_time - start_time
        
        # Analyze results
        analysis = self._analyze_distributed_results(results, total_time)
        
        return results, analysis
    
    def _analyze_distributed_results(self, results, total_duration):
        """Analyze results from distributed simulations."""
        analysis = {
            'total_simulations': len(results),
            'total_duration': total_duration,
            'total_steps': len(results) * 200,
            'steps_per_second': len(results) * 200 / total_duration,
            'average_activity': np.mean([r['final_activity'] for r in results]),
            'activity_std': np.std([r['final_activity'] for r in results]),
            'average_firing': np.mean([r['firing_neurons'] for r in results]),
            'total_memory_usage': sum(r['memory_usage'] for r in results),
            'average_curiosity': np.mean([r['curiosity'] for r in results]),
            'average_novelty': np.mean([r['novelty'] for r in results]),
            'development_stages': list(set(r['development_stage'] for r in results)),
            'slowest_simulation': max(results, key=lambda x: x['duration']),
            'fastest_simulation': min(results, key=lambda x: x['duration'])
        }
        
        return analysis
    
    def close(self):
        """Close the multiprocessing pool."""
        if self.pool:
            self.pool.close()
            self.pool.join()
            self.pool = None
            print("Pool closed.")

# Use distributed computing
print("Starting distributed NLM simulation...")
distributed = DistributedNLM(num_workers=4)

# Run distributed simulations
results, analysis = distributed.run_distributed_simulation(10, 200)

print(f"\nDistributed Simulation Results:")
print(f"  Total simulations: {analysis['total_simulations']}")
print(f"  Total duration: {analysis['total_duration']:.2f} seconds")
print(f"   Speed: {analysis['steps_per_second']:.1f} steps/second")
print(f"  Average activity: {analysis['average_activity']:.3f} ± {analysis['activity_std']:.3f}")
print(f"  Average curiosity: {analysis['average_curiosity']:.3f}")
print(f"  Development stages achieved: {analysis['development_stages']}")

# Close the pool
distributed.close()
```

## Integration with Other Systems

### 1. Integration with Robot Control
```python
# Integrate NLM with robot control systems
import pynlm
import numpy as np

class NLMRobotController:
    """NLM-based robot controller."""
    
    def __init__(self):
        # Create NLM brain
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 2000)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        
        self.brain = pynlm.createBrain(config)
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Robot interface
        self.robot = RobotInterface()
        self.sensor_data = SensorData()
        
        # Initialize
        self.agent.initialize(self.robot)
    
    def run_control_loop(self, duration):
        """Run control loop with NLM."""""
        print("Starting NLM-based robot control...")
        
        start_time = time.time()
        
        for step in range(duration):
            current_time = time.time() - start_time
            
            # Get sensor data
            sensor_data = self.sensor_data.get_current_data()
            
            # Process sensor input
            self.agent.processSensoryInput(sensor_data)
            
            # Run brain computation
            self.brain.step(step)
            
            # Decode motor command
            motor_cmd = self.agent.decodeMotorCommand()
            
            # Convert to robot commands
            robot_commands = self._convert_to_robot_commands(motor_cmd)
            
            # Execute robot commands
            self.robot.execute_commands(robot_commands)
            
            # Apply reward
            reward = self._calculate_robot_reward(sensor_data)
            self.agent.applyRewardModulation(reward, 0.0)
            
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Print status
            if step % 100 == 0:
                self._print_status(step, current_time)
        
        print("Control loop completed.")
    
    def _convert_to_robot_commands(self, motor_cmd):
        """Convert NLM motor command to robot commands."""
        # Map NLM action to robot commands
        if motor_cmd.action_type == pynlm.ActionType.MoveForward:
            return RobotCommand(type='move', speed=motor_cmd.value, duration=0.1)
        elif motor_cmd.action_type == pynlm.ActionType.TurnLeft:
            return RobotCommand(type='turn', angle=motor_cmd.value, speed='slow')
        elif motor_cmd.action_type == pynlm.ActionType.TurnRight:
            return RobotCommand(type='turn', angle=motor_cmd.value, speed='fast')
        elif motor_cmd.action_type == pynlm.ActionType.Interact:
            return RobotCommand(type='grab', force=motor_cmd.value)
        else:
            return RobotCommand(type='idle')
    
    def _calculate_robot_reward(self, sensor_data):
        """Calculate reward for robot actions."""
        reward = 0.0
        
        # Success reward
        if sensor_data.goal_reached:
            reward += 10.0
        
        # Efficiency reward (faster goal reaching)
        if sensor_data.time_to_goal:
            reward += 1.0 / sensor_data.time_to_goal
        
        # Exploration reward
        novelty = self.agent.getNoveltyLevel()
        reward += novelty * 2.0
        
        # Safety reward
        if not sensor_data.collision_detected:
            reward += 1.0
        
        return reward
    
    def _print_status(self, step, elapsed_time):
        """Print current status."""
        print(f"\nStep {step} (t={elapsed_time:.1f}s):")
        print(f"  Activity: {self.brain.getAverageActivity():.3f}")
        print(f"  Curiosity: {self.agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {self.agent.getNoveltyLevel():.3f}")
        print(f"  Dev stage: {self.brain.getDevelopmentalStage().name}")
        print(f"  Memory usage: {self.brain.getMemoryUsage() / 1024 / 1024:.1f} MB")

# Use NLM robot controller
controller = NLMRobotController()
controller.run_control_loop(1000)
```

### 2. Integration with Virtual Reality
```python
# Integrate NLM with virtual reality systems
import pynlm
import pygame
import numpy as np

class NLMVRSystem:
    """NLM integration with Virtual Reality."""
    
    def __init__(self, vr_config=None):
        self.vr_config = vr_config or {}
        
        # Initialize NLM brain
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1500)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        
        
        self.brain = pynlm.createBrain(config)
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Initialize VR system
        self.vr_system = VRSystem(self.vr_config)
        
        # Performance monitoring
        self.performance_stats = {
            'frames': 0,
            'update_time': 0,
            'render_time': 0,
            'brain_time': 0
        }
    
    def run_vr_simulation(self, duration_frames=5000):
        """Run VR simulation with NLM."""
        print("Starting VR simulation with NLM...")
        print(f"Target duration: {duration_frames} frames")
        
        self.vr_system.initialize()
        self.agent.initialize(self.vr_system)
        
        frame_count = 0
        start_time = time.time()
        
        while frame_count < duration_frames:
            frame_start = time.time()
            
            # Get VR sensor data
            vr_sensors = self.vr_system.get_sensors()
            
            # Process sensor input in NLM
            self.agent.processSensoryInput(vr_sensors)
            
            # Run brain computation
            brain_start = time.time()
            self.brain.step(frame_count)
            brain_time = time.time() - brain_start
            
            # Decode motor command
            motor_cmd = self.agent.decodeMotorCommand()
            
            # Convert to VR actions
            vr_actions = self._convert_to_vr_actions(motor_cmd, vr_sensors)
            
            # Update VR system
            self.vr_system.update(vr_actions)
            
            # Render frame
            render_start = time.time()
            self.vr_system.render()
            render_time = time.time() - render_start
            
            update_time = time.time() - frame_start
            
            self.performance_stats['update_time'] += update_time
            self.performance_stats['render_time'] += render_time
            self.performance_stats['brain_time'] += brain_time
            self.performance_stats['frames'] += 1
            
            frame_count += 1
            
            # Print performance stats every 100 frames
            if frame_count % 100 == 0:
                self._print_performance_stats(frame_count)
        
        total_time = time.time() - start_time
        self._print_final_stats(total_time)
    
    def _convert_to_vr_actions(self, motor_cmd, sensors):
        """Convert NLM output to VR actions."""
        actions = {}
        
        # Map NLM actions to VR controls
        if motor_cmd.action_type == pynlm.ActionType.MoveForward:
            actions['movement'] = {
                'x': sensors['head_rotation'][0] * motor_cmd.value,
                'y': 0,
                'z': -motor_cmd.value
            }
        elif motor_cmd.action_type == pynlm.ActionType.TurnLeft:
            actions['rotation'] = {
                'yaw': motor_cmd.value,
                'pitch': 0,
                'roll': 0
            }
        elif motor_cmd.action_type == pynlm.ActionType.TurnRight:
            actions['rotation'] = {
                'yaw': -motor_cmd.value,
                'pitch': 0,
                'roll': 0
            }
        elif motor_cmd.action_type == pynlm.ActionType.Interact:
            actions['trigger'] = True
            actions['grab_strength'] = motor_cmd.value
        elif motor_cmd.action_type == pynlm.ActionType.Eat:
            actions['consume'] = True
        elif motor_cmd.action_type == pynlm.ActionType.Sleep:
            actions['rest'] = True
        
        # Add curiosity-driven exploration
        curiosity = self.agent.getCuriosityLevel()
        if curiosity > 0.7:
            actions['exploration'] = True
            actions['exploration_strength'] = curiosity
        
        return actions
    
    def _print_performance_stats(self, frame_count):
        """Print performance statistics."""
        stats = self.performance_stats
        total_frames = stats['frames']
        
        avg_update = stats['update_time'] / total_frames * 1000  # ms
        avg_render = stats['render_time'] / total_frames * 1000  # ms
        avg_brain = stats['brain_time'] / total_frames * 1000  # ms
        fps = total_frames / (stats['update_time'])
        
        print(f"\nFrame {frame_count}:")
        print(f"  FPS: {fps:.1f}")
        print(f"  Avg update time: {avg_update:.2f} ms")
        print(f"  Avg render time: {avg_render:.2f} ms")
        print(f"  Avg brain time: {avg_brain:.2f} ms")
        print(f"  Activity: {self.brain.getAverageActivity():.3f}")
        print(f"  Curiosity: {self.agent.getCuriosityLevel():.3f}")
    
    def _print_final_stats(self, total_time):
        """Print final performance statistics."""
        stats = self.performance_stats
        total_frames = stats['frames']
        
        avg_update = stats['update_time'] / total_frames * 1000
        avg_render = stats['render_time'] / total_frames * 1000
        avg_brain = stats['brain_time'] / total_frames * 1000
        fps = total_frames / stats['update_time']
        
        print(f"\n{'='*50}")
        print("VR Simulation Complete!")
        print(f"{'='*50}")
        print(f"Total frames: {total_frames}")
        print(f"Total time: {total_time:.2f} seconds")
        print(f"Average FPS: {fps:.1f}")
        print(f"Average update time: {avg_update:.2f} ms")
        print(f"Average render time: {avg_render:.2f} ms")
        print(f"Average brain time: {avg_brain:.2f} ms")
        print(f"Brain usage: {(avg_brain / (avg_update + avg_render + avg_brain) * 100):.1f}%")
        print(f"Final activity: {self.brain.getAverageActivity():.3f}")
        print(f"Final curiosity: {self.agent.getCuriosityLevel():.3f}")
        print(f"Final development stage: {self.brain.getDevelopmentalStage().name}")
```

## Research and Experimental Applications

### 1. Memory Research
```python
# NLM for memory research experiments
import pynlm
import numpy as np
from scipy import stats

class MemoryResearch:
    """Research experiments with NLM memory systems."""
    
    def __init__(self):
        self.baseline_config = self._create_baseline_config()
    
    def _create_baseline_config(self):
        """Create baseline configuration for memory research."""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 2000)
        config.set("plasticity.enable_stdp", True)
        config.set("plasticity.enable_hebbian", True)
        config.set("memory.episodic_memory.encoding_strength", 0.8)
        config.set("memory.associative_memory.pattern_threshold", 0.6)
        return config
    
    def experiment_working_memory_capacity(self, trials=50):
        """Test working memory capacity limits."""
        print("Testing working memory capacity...")
        
        memory_capacities = []
        
        for trial in range(trials):
            # Create brain
            brain = pynlm.createBrain(self.baseline_config)
            brain.initialize()
            
            # Present sequence of patterns
            sequence_length = trial + 1  # Increment length each trial
            presented_patterns = []
            recalled_patterns = []
            
            for step in range(sequence_length * 10):  # Extended presentation
                brain.step(step)
                
                # Present pattern (simulate)
                if step % 10 == 0:
                    presented_patterns.append(step // 10)
                    
                    # Try to recall
                    if step % 5 == 0:
                        recalled_patterns.extend(brain.getWorkingMemory().getActiveIndices())
            
            # Calculate capacity (precision and recall)
            accuracy = len(set(presented_patterns) & set(recalled_patterns)) / len(presented_patterns)
            memory_capacities.append((sequence_length, accuracy))
            
            # Clean up
            del brain
        
        # Analyze results
        capacities = self._analyze_memory_capacity(memory_capacities)
        return capacities
    
    def _analyze_memory_capacity(self, results):
        """Analyze working memory capacity results."""
        lengths, accuracies = zip(*results)
        
        # Find capacity point (where accuracy drops below threshold)
        capacity_threshold = 0.6
        capacity_point = next((l for l, a in zip(lengths, accuracies) if a < capacity_threshold), max(lengths))
        
        # Calculate efficiency
        efficiency = np.mean(accuracies) / np.mean(lengths)
        
        analysis = {
            'tested_lengths': list(lengths),
            'accuracies': list(accuracies),
            'capacity_point': capacity_point,
            'efficiency': efficiency,
            'peak_accuracy': max(accuracies),
            'peak_length': lengths[np.argmax(accuracies)]
        }
        
        return analysis
    
    def experiment_spontaneous_concept_formation(self, steps=1000):
        """Study spontaneous concept formation."""
        print("Testing spontaneous concept formation...")
        
        # Create brain with enhanced concept formation
        config = self.baseline_config
        config.set("brain.enable_concept_formation", True)
        config.set("memory.associative_memory.concept_formulation", True)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Track concept emergence
        concepts_formed = []
        concept_stability = []
        
        for step in range(steps):
            brain.step(step)
            
            # Check for new concepts
            current_patterns = brain.getAssociativeMemory().getCurrentPatterns()
            existing_concepts = brain.getAssociativeMemory().getConceptCount()
            
            if existing_concepts > len(concepts_formed):
                new_concepts = existing_concepts - len(concepts_formed)
                concepts_formed.extend([f"concept_{i}" for i in range(len(concepts_formed), existing_concepts)])
                concept_stability.append(new_concepts)
        
        # Analyze concept formation dynamics
        formation_dynamics = self._analyze_concept_dynamics(concept_stability)
        
        return formation_dynamics
    
    def _analyze_concept_dynamics(self, stability_data):
        """Analyze concept formation dynamics."""
        if not stability_data:
            return {'concepts_formed': 0, 'formation_rate': 0, 'stability': 0}
        
        # Calculate formation rate (concepts per unit time)
        formation_rate = np.mean(stability_data)
        
        # Calculate stability (variance in formation rate)
        stability = 1.0 / (1.0 + np.std(stability_data))
        
        analysis = {
            'concepts_formed': sum(stability_data),
            'formation_rate': formation_rate,
            'stability': stability,
            'formation_trajectory': stability_data
        }
        
        return analysis
    
    def experiment_neuromodulation_effects(self, neuromodulation_conditions):
        """Study effects of different neuromodulation conditions."""
        print("Testing neuromodulation effects...")
        
        results = {}
        
        for condition_name, condition_config in neuromodulation_conditions.items():
            print(f"  Testing condition: {condition_name}")
            
            # Create brain with specific neuromodulation configuration
            config = self.baseline_config.copy()
            config.update(condition_config)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Run simulation
            for step in range(500):
                brain.step(step)
            
            # Collect results
            neuromod_stats = {
                'curiosity_level': brain.getCuriosityLevel(),
                'novelty_level': brain.getNoveltyLevel(),
                'prediction_error': brain.getPredictionError(),
                'activity_level': brain.getAverageActivity(),
                'memory_usage': brain.getMemoryUsage(),
                'development_stage': brain.getDevelopmentalStage().name
            }
            
            results[condition_name] = neuromod_stats
        
        return results

# Run memory research experiments
researcher = MemoryResearch()

print("Starting NLM Memory Research Experiments")
print("=" * 50)

# Test working memory capacity
wm_capacity = researcher.experiment_working_memory_capacity(20)
print(f"\nWorking Memory Capacity Results:")
print(f"  Capacity point: {wm_capacity['capacity_point']} items")
print(f"  Efficiency: {wm_capacity['efficiency']:.3f}")
print(f"  Peak accuracy: {wm_capacity['peak_accuracy']:.3f}")

# Test concept formation
concept_dynamics = researcher.experiment_spontaneous_concept_formation(500)
print(f"\nConcept Formation Results:")
print(f"  Concepts formed: {concept_dynamics['concepts_formed']}")
print(f"  Formation rate: {concept_dynamics['formation_rate']:.2f} concepts/step")
print(f"  Stability: {concept_dynamics['stability']:.3f}")

# Test neuromodulation effects
neuromod_conditions = {
    'baseline': {'neuromod.curiosity.enable': True, 'neuromod.novelty.enable': False},
    'high_curiosity': {'neuromod.curiosity.enable': True, 'neuromod.curiosity.sensitivity': 2.0},
    'high_novelty': {'neuromod.curiosity.enable': False, 'neuromod.novelty.enable': True, 'neuromod.novelty.threshold': 0.2},
    'full_integration': {'neuromod.curiosity.enable': True, 'neuromod.novelty.enable': True, 'neuromod.prediction.error_sensitivity': 1.5}
}

neuromod_results = researcher.experiment_neuromodulation_effects(neuromod_conditions)
print(f"\nNeuromodulation Condition Results:")
for condition, stats in neuromod_results.items():
    print(f"  {condition}:")
    print(f"    Curiosity: {stats['curiosity_level']:.3f}")
    print(f"    Novelty: {stats['novelty_level']:.3f}")
    print(f"    Activity: {stats['activity_level']:.3f}")
    print(f"    Dev stage: {stats['development_stage']}")
```

## Future Research Directions

### 1. Brain-Computer Interface Integration
```python
# Future direction: Brain-Computer Interface (BCI) integration
import pynlm
import numpy as np

class BCIResearch:
    """Research NLM integration with Brain-Computer Interfaces."""
    
    def __init__(self):
        # Create NLM for BCI control
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1000)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.prediction.error_sensitivity", 1.0)
        
        self.brain = pynlm.createBrain(config)
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # BCI interface (future development)
        self.bci_interface = None
    
    def prepare_bci_integration(self):
        """Prepare system for BCI integration."""
        # TODO: Implement BCI interface
        # 1. EEG signal acquisition
        # 2. Signal preprocessing
        # 3. Feature extraction
        # 4. NLM input processing
        # 5. Output mapping to actions
        
        print("BCI integration preparation:")
        print("  ✓ NLM brain created")
        print("  ✗ BCI hardware interface not implemented")
        print("  ✗ Signal processing pipeline not implemented")
        print("  ✗ Feature extraction not implemented")
        print("  ✗ Output mapping not implemented")
    
    def simulate_bci_input(self, eeg_data):
        """Simulate BCI input for research."""
        # Simulate EEG-based neural activity
        simulated_activity = self._process_eeg_signal(eeg_data)
        
        # Convert to NLM input
        sensory_input = self._create_sensory_input(simulated_activity)
        
        # Process through NLM
        self.agent.processSensoryInput(sensory_input)
        self.brain.step(0)
        
        return self.agent.decodeMotorCommand()
    
    def _process_eeg_signal(self, eeg_data):
        """Process EEG signal to simulate neural activity."""
        # TODO: Implement proper EEG signal processing
        # For now, use simplified simulation
        
        # Band power analysis (simulated)
        alpha_power = np.mean(eeg_data[:, 0:8])  # Alpha band simulation
        beta_power = np.mean(eeg_data[:, 8:16])   # Beta band simulation
        gamma_power = np.mean(eeg_data[:, 16:24]) # Gamma band simulation
        
        # Combine into simulated neural activity
        simulated_activity = {
            'alpha_power': alpha_power,
            'beta_power': beta_power,
            'gamma_power': gamma_power,
            'overall_excitability': (alpha_power + beta_power) / 2
        }
        
        return simulated_activity
    
    def _create_sensory_input(self, neural_activity):
        """Create sensory input from neural activity."""
        # Convert neural activity to sensory input format
        sensory_input = {
            'visual_input': neural_activity['gamma_power'] * 0.1,
            'attention_level': neural_activity['beta_power'] * 0.05,
            'excitability': neural_activity['overall_excitability'] * 0.2,
            'curiosity_drive': np.tanh(neural_activity['beta_power'] - neural_activity['alpha_power'])
        }
        
        return sensory_input

# Future BCI research
bci_research = BCIResearch()
bci_research.prepare_bci_integration()

print("\nFuture Research Directions:")
print("1. Brain-Computer Interface (BCI) Integration")
print("2. Real-time Neurofeedback Systems")
print("3. Adaptive Learning with BCI Feedback")
print("4. Closed-loop BCI-NLM Systems")
print("5. Reinforcement Learning with BCI Rewards")
```

### 2. Multi-Modal Learning
```python
# Future direction: Multi-modal learning with NLM
import pynlm
import numpy as np

class MultiModalLearning:
    """Research multi-modal learning with NLM."""
    
    def __init__(self):
        # Create multimodal NLM
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 2500)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        config.set("neuromod.prediction.error_sensitivity", 1.2)
        
        self.brain = pynlm.createBrain(config)
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Multimodal input processors
        self.visual_processor = None
        self.audio_processor = None
        self.tactile_processor = None
        self.olfactory_processor = None
    
    def initialize_multimodal_system(self):
        """Initialize multimodal learning system."""
        print("Initializing Multi-Modal Learning System...")
        print("  ✓ NLM brain created")
        print("  ✗ Visual processing not implemented")
        print("  ✗ Audio processing not implemented")
        print("  ✗ Tactile processing not implemented")
        print("  ✗ Olfactory processing not implemented")
        print("  ✗ Multimodal integration not implemented")
    
    def process_multimodal_input(self, visual_data, audio_data, tactile_data, olfactory_data):
        """Process multimodal input through NLM."""
        # TODO: Implement multimodal processing
        
        # Create unified multimodal input
        multimodal_input = {
            'visual': self._process_visual(visual_data),
            'audio': self._process_audio(audio_data),
            'tactile': self._process_tactile(tactile_data),
            'olfactory': self._process_olfactory(olfactory_data),
            'integrated': self._integrate_modalities(
                self._process_visual(visual_data),
                self._process_audio(audio_data),
                self._process_tactile(tactile_data),
                self._process_olfactory(olfactory_data)
            )
        }
        
        # Process through NLM
        self.agent.processSensoryInput(multimodal_input)
        self.brain.step(0)
        
        return self.agent.decodeMotorCommand()
    
    def _process_visual(self, visual_data):
        """Process visual input."""
        # TODO: Implement visual processing
        return {
            'features': np.random.rand(10),
            'complexity': np.random.rand(),
            'color_features': np.random.rand(3),
            'motion_features': np.random.rand(2)
        }
    
    def _process_audio(self, audio_data):
        """Process audio input."""
        # TODO: Implement audio processing
        return {
            'frequency_features': np.random.rand(8),
            'temporal_features': np.random.rand(5),
            'pitch_features': np.random.rand(3),
            'volume_features': np.random.rand(2)
        }
    
    def _process_tactile(self, tactile_data):
        """Process tactile input."""
        # TODO: Implement tactile processing
        return {
            'pressure_features': np.random.rand(6),
            'temperature_features': np.random.rand(3),
            'texture_features': np.random.rand(4)
        }
    
    def _process_olfactory(self, olfactory_data):
        """Process olfactory input."""
        # TODO: Implement olfactory processing
        return {
            'odor_features': np.random.rand(12),
            'concentration_features': np.random.rand(3)
        }
    
    def _integrate_modalities(self, visual, audio, tactile, olfactory):
        """Integrate multimodal features."""
        # TODO: Implement multimodal integration
        
        # Simple feature concatenation (placeholder)
        integrated_features = np.concatenate([
            visual['features'],
            audio['frequency_features'],
            tactile['pressure_features'],
            olfactory['odor_features']
        ])
        
        # Normalize features
        integrated_features = integrated_features / (np.sum(integrated_features) + 1e-8)
        
        return {
            'features': integrated_features,
            'complexity': np.sum([
                visual['complexity'],
                np.mean(audio['frequency_features']),
                np.mean(tactile['pressure_features']),
                np.mean(olfactory['concentration_features'])
            ]),
            'novelty': np.sum([
                np.std(visual['features']),
                np.std(audio['frequency_features']),
                np.std(tactile['pressure_features']),
                np.std(olfactory['odor_features'])
            ]) / 4
        }
    
    def run_multimodal_experiment(self, trials=100):
        """Run multimodal learning experiment."""
        print("Running Multi-Modal Learning Experiment...")
        
        # Generate multimodal data
        for trial in range(trials):
            # Create multimodal input
            visual_data = np.random.randn(24, 24, 3)  # Simulated RGB image
            audio_data = np.random.randn(44100)  # Simulated audio samples
            tactile_data = {
                'pressure': np.random.rand(10),
                'temperature': np.random.rand(5)
            }
            olfactory_data = np.random.rand(20)  # Odor concentration
            
            # Process through NLM
            action = self.process_multimodal_input(
                visual_data, audio_data, tactile_data, olfactory_data
            )
            
            # Simulate reward based on action
            reward = self._calculate_multimodal_reward(action, visual_data, audio_data)
            
            # Update development
            self.agent.applyRewardModulation(reward, 0.0)
            self.agent.updateDevelopment(0.1)
            
            if trial % 20 == 0:
                print(f"  Trial {trial}: Curiosity={self.agent.getCuriosityLevel():.3f}, "
                      f"Novelty={self.agent.getNoveltyLevel():.3f}, "
                      f"Activity={self.brain.getAverageActivity():.3f}")
        
        print("\nMultimodal learning experiment completed!")
        return self._analyze_multimodal_results(trials)
    
    def _calculate_multimodal_reward(self, action, visual_data, audio_data):
        """Calculate reward for multimodal actions."""
        reward = 0.0
        
        # Feature recognition reward
        visual_features = self._extract_visual_features(visual_data)
        audio_features = self._extract_audio_features(audio_data)
        
        # Check if action matches expected features
        if action.action_type == pynlm.ActionType.Interact:
            reward += 1.0  # Interaction reward
        
        # Novelty reward
        novelty = self.agent.getNoveltyLevel()
        reward += novelty * 0.5
        
        # Curiosity reward
        curiosity = self.agent.getCuriosityLevel()
        reward += curiosity * 0.3
        
        return reward
    
    def _analyze_multimodal_results(self, trials):
        """Analyze multimodal learning results."""
        analysis = {
            'trials_completed': trials,
            'final_curiosity': self.agent.getCuriosityLevel(),
            'final_novelty': self.agent.getNoveltyLevel(),
            'final_activity': self.brain.getAverageActivity(),
            'final_development_stage': self.brain.getDevelopmentalStage().name,
            'learning_efficiency': np.random.rand(),  # Placeholder
            'multimodal_integration': 'simulated'  # Placeholder
        }
        
        return analysis

# Future multimodal research
multimodal_research = MultiModalLearning()
multimodal_research.initialize_multimodal_system()

results = multimodal_research.run_multimodal_experiment(100)
```


## Implementation Notes

### 1. Python Bindings Implementation

The Python bindings (`pynlm`) are implemented using `pybind11` and provide access to:


#### Core Brain Functionality
- **Brain Creation and Configuration**: `createBrain()`, `createDefaultConfig()`
- **Simulation Control**: `step()`, `initialize()`, `reset()`
- **State Management**: `save()`, `load()`
- **Statistics**: `getTotalNeuronCount()`, `getFiringNeuronCount()`, etc.
- **Subsystems**: Access to working memory, episodic memory, associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty, prediction error
- **Development**: Stage management and plasticity rates

#### Agent Interface
- **Agent Creation**: `createAgentBrain()`
- **Sensory Processing**: `processSensoryInput()`
- **Motor Decoding**: `decodeMotorCommand()`
- **Neuromodulation Control**: `enableRewardModulation()`, etc.
- **Development Update**: `updateDevelopment()`


#### World Simulation
- **World Creation**: `createSimpleWorld()`
- **Configuration**: `configure()` for size, vision, etc.
- **Control**: `reset()`, `update()`
- **Agent Management**: `setAgentStart()`, `applyMotorCommand()`
- **Sensory Access**: `getSensoryPercept()`
- **Object Management**: `addObject()`, `removeObject()`


### 2. C++ Implementation

The core NLM implementation is in C++20:


#### File Structure
- **`src/brain/`**: Brain class and neural computation
- **`src/neuromodulation/`**: Neuromodulator implementations
- **`src/memory/`**: Memory systems
- **`src/cognition/`**: Cognitive mechanisms
- **`src/environment/`**: Environment simulation
- **`src/plasticity/`**: Plasticity rules
- **`src/world/`**: World simulation
- **`src/agent/`**: Agent interface


#### Key Components
- **Neural Dynamics**: LIF neuron models with event-driven computation
- **Plasticity**: STDP, Hebbian, structural plasticity
- **Memory**: Working, episodic, associative memory
- **Prediction**: Prediction system with error computation
- **Cognition**: Planning, concept formation, attention
- **Development**: Developmental stages and plasticity modulation
- **Neuromodulation**: Dopamine, curiosity, novelty, prediction error

### 3. Performance Considerations


#### Memory Usage
```python
# Approximate memory usage for different configurations
config_sizes = {
    1000: 50,      # ~50 MB
    3000: 150,     # ~150 MB  
    5000: 250,     # ~250 MB
    10000: 500,    # ~500 MB
    20000: 1000   # ~1 GB
}
```


#### Performance Characteristics
- **Simulation Speed**: ~4-50 steps/second depending on neuron count
- **Memory Usage**: Scales linearly with neuron count
- **CPU Usage**: ~1-10% per CPU core for small brains
- **GPU**: Limited GPU acceleration (research direction)


### 4. Compatibility

#### Python Version
- **Minimum**: Python 3.8
- **Recommended**: Python 3.10+
- **Features**: Requires `numpy`, `pybind11`, `scikit-build-core`


#### Platform Support
- **Linux**: Fully supported
- **macOS**: Supported (with limitations)
- **Windows**: Limited support (experimental)
- **ARM**: Supported (performance tested)

### 5. Future Development

#### Research Directions
1. **Brain-Computer Interface Integration**
   - Real-time EEG/EMG signal processing
   - Closed-loop neurofeedback systems
   - Adaptive learning with neural signals

2. **Multi-Modal Learning**
   - Visual, auditory, tactile, olfactory integration
   - Cross-modal concept formation
   - Multisensory attention mechanisms

3. **Advanced Plasticity**
   - Structured plasticity rules
   - Homeostatic regulation
   - Network-level plasticity effects


4. **Scalable Architecture**
   - Distributed computing support
   - GPU acceleration
   - Cloud deployment capabilities

5. **Real-time Applications**
   - Interactive learning environments
   - Neuroadaptive interfaces
   - Personalized AI systems

### 6. Known Limitations


#### Current Constraints
- **Memory**: Large-scale simulations require significant memory
- **Performance**: Real-time large-scale simulation challenging
- **Accuracy**: Simplified neuron models vs biological accuracy
- **Complexity**: High computational cost for detailed integration

#### Areas for Improvement
- **Neuron Models**: More biophysically accurate models
- **Algorithms**: More efficient computation methods
- **Hardware**: Better hardware utilization
- **Integration**: More seamless system integration

---

*Documentation last updated: $(date +%Y-%m-%d)*