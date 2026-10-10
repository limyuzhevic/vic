# Visualization and Debugging Tools for NLM

This directory contains visualization and debugging tools for NLM, including:
- Network visualization tools
- Activity monitoring
- Debug logging and profiling
- Performance analysis
- Experimental visualization

## Overview

NLM includes several visualization and debugging tools to help users:
1. **Visualize neural network structures and activity**
2. **Monitor simulation performance and statistics**
3. **Debug simulation issues and bottlenecks**
4. **Analyze network connectivity and dynamics**
5. **Create experimental visualizations**

## Tools Included

### 1. Network Visualizer (`NetworkVisualizer`)

```python
import pynlm
from visualization import NetworkVisualizer

def visualize_network(brain, output_file="network_viz.png"):
    """Create a visualization of the neural network."""
    
    visualizer = NetworkVisualizer(brain)
    
    # Create different types of visualizations
    visualizer.create_connectivity_matrix(output_file="connectivity.png")
    visualizer.create_spike_raster(output_file="spike_raster.png")
    visualizer.create_activity_heatmap(output_file="activity_heatmap.png")
    visualizer.create_weight_distribution(output_file="weight_distribution.png")
    
    print(f"Network visualizations saved to directory")

# Usage
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
viz = NetworkVisualizer(brain)
viz.create_network_diagram("neural_network.png")
```

### 2. Activity Monitor (`ActivityMonitor`)

```python
import pynlm
from visualization import ActivityMonitor
import matplotlib.pyplot as plt
import time

class NLMPerformanceMonitor:
    """Monitor and analyze NLM simulation performance."""
    
    def __init__(self, brain):
        self.brain = brain
        self.activity_data = []
        self.performance_data = []
        self.start_time = time.time()
    
    def start_monitoring(self):
        """Start monitoring brain activity."""
        self.monitoring = True
        self.activity_data.clear()
        self.performance_data.clear()
        self.start_time = time.time()
    
    def stop_monitoring(self):
        """Stop monitoring."""
        self.monitoring = False
    
    def record_activity(self):
        """Record current brain activity."""
        if self.monitoring:
            activity = {
                'timestamp': time.time() - self.start_time,
                'firing_neurons': self.brain.getFiringNeuronCount(),
                'total_spikes': self.brain.getTotalSpikeCount(),
                'average_firing_rate': self.brain.getAverageFiringRate(),
                'e_i_ratio': self.brain.getExcitationInhibitionRatio()
            }
            self.activity_data.append(activity)
    
    def create_activity_plots(self, output_prefix="activity"):
        """Create plots from recorded activity data."""
        if not self.activity_data:
            print("No activity data to plot")
            return
        
        timestamps = [d['timestamp'] for d in self.activity_data]
        firing_rates = [d['average_firing_rate'] for d in self.activity_data]
        e_i_ratios = [d['e_i_ratio'] for d in self.activity_data]
        spike_counts = [d['total_spikes'] for d in self.activity_data]
        
        fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(12, 10))
        
        # Firing rate over time
        ax1.plot(timestamps, firing_rates, 'b-')
        ax1.set_xlabel('Time (s)')
        ax1.set_ylabel('Average Firing Rate (Hz)')
        ax1.set_title('Firing Rate Over Time')
        ax1.grid(True)
        
        # E/I ratio over time
        ax2.plot(timestamps, e_i_ratios, 'r-')
        ax2.set_xlabel('Time (s)')
        ax2.set_ylabel('E/I Ratio')
        ax2.set_title('Excitation/Inhibition Balance Over Time')
        ax2.set_ylim(0, 2)
        ax2.grid(True)
        
        # Spike count over time
        ax3.plot(timestamps, spike_counts, 'g-')
        ax3.set_xlabel('Time (s)')
        ax3.set_ylabel('Total Spikes')
        ax3.set_title('Cumulative Spike Count Over Time')
        ax3.grid(True)
        
        # Firing neuron count over time
        firing_counts = [d['firing_neurons'] for d in self.activity_data]
        ax4.plot(timestamps, firing_counts, 'orange')
        ax4.set_xlabel('Time (s)')
        ax4.set_ylabel('Firing Neurons')
        ax4.set_title('Active Neurons Over Time')
        ax4.grid(True)
        
        plt.tight_layout()
        plt.savefig(f"{output_prefix}_analysis.png")
        plt.show()
        
        print(f"Activity plots saved to {output_prefix}_analysis.png")

# Usage example
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

monitor = NLMPerformanceMonitor(brain)
monitor.start_monitoring()

# Run simulation while monitoring
for step in range(100):
    brain.step(step)
    monitor.record_activity()

monitor.stop_monitoring()
monitor.create_activity_plots()
```

### 3. Debug Logger (`DebugLogger`)

```python
import pynlm
from debugging import DebugLogger
import logging
import time

class NLMDebugger:
    """Comprehensive debugging tool for NLM simulations."""
    
    def __init__(self, log_file="nlm_debug.log"):
        self.logger = logging.getLogger('nlm_debugger')
        self.logger.setLevel(logging.DEBUG)
        
        # Create file handler
        fh = logging.FileHandler(log_file)
        fh.setLevel(logging.DEBUG)
        
        # Create console handler
        ch = logging.StreamHandler()
        ch.setLevel(logging.INFO)
        
        # Create formatter
        formatter = logging.Formatter(
            '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
        )
        fh.setFormatter(formatter)
        ch.setFormatter(formatter)
        
        # Add handlers to logger
        self.logger.addHandler(fh)
        self.logger.addHandler(ch)
        
        self.log_file = log_file
        self.simulation_start_time = time.time()
        self.step_count = 0
        self.debug_info = {}
    
    def log_initialization(self, brain):
        """Log brain initialization details."""
        self.logger.info("=== Brain Initialization ===")
        self.logger.info(f"Total neurons: {brain.getTotalNeuronCount()}")
        self.logger.info(f"Total synapses: {brain.getTotalSynapseCount()}")
        self.logger.info(f"Regions: {brain.getRegionCount()}")
        self.logger.info(f"Initial firing rate: {brain.getAverageFiringRate():.3f} Hz")
        self.logger.info(f"E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
        
        # Store debug info
        self.debug_info['initialization'] = {
            'neurons': brain.getTotalNeuronCount(),
            'synapses': brain.getTotalSynapseCount(),
            'regions': brain.getRegionCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'e_i_ratio': brain.getExcitationInhibitionRatio()
        }
    
    def log_step(self, step, brain, step_time):
        """Log information about a simulation step."""
        self.step_count += 1
        
        # Performance logging
        if step % 10 == 0:
            self.logger.info(
                f"Step {step}: "
                f"Firing: {brain.getFiringNeuronCount()}, "
                f"Spikes: {brain.getTotalSpikeCount()}, "
                f"Rate: {brain.getAverageFiringRate():.3f} Hz, "
                f"Time: {step_time:.6f}s"
            )
        
        # Debug logging for first few steps
        if step < 5:
            self.logger.debug(
                f"Step {step}: "
                f"Neuron potential range: "
                f"Min: {self.get_min_potential(brain)}, "
                f"Max: {self.get_max_potential(brain)}"
            )
        
        # Store step info
        if 'steps' not in self.debug_info:
            self.debug_info['steps'] = []
        self.debug_info['steps'].append({
            'step': step,
            'firing_neurons': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'e_i_ratio': brain.getExcitationInhibitionRatio(),
            'step_time': step_time
        })
    
    def get_min_potential(self, brain):
        """Get minimum membrane potential (placeholder)."""
        return -75.0  # Placeholder - would need actual neuron access
    
    def get_max_potential(self, brain):
        """Get maximum membrane potential (placeholder)."""
        return -50.0  # Placeholder - would need actual neuron access
    
    def log_simulation_summary(self, brain):
        """Log simulation summary."""
        self.logger.info("=== Simulation Summary ===")
        self.logger.info(f"Total steps completed: {self.step_count}")
        self.logger.info(f"Total simulation time: {time.time() - self.simulation_start_time:.2f}s")
        self.logger.info(f"Final firing rate: {brain.getAverageFiringRate():.3f} Hz")
        self.logger.info(f"Final E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
        self.logger.info(f"Final spike count: {brain.getTotalSpikeCount()}")
        
        # Calculate performance metrics
        if self.step_count > 0:
            avg_step_time = (time.time() - self.simulation_start_time) / self.step_count
            self.logger.info(f"Average step time: {avg_step_time:.6f}s")
            self.logger.info(f"Steps per second: {1.0 / avg_step_time:.2f}")
        
        # Save debug info to file
        self.save_debug_info()
    
    def save_debug_info(self):
        """Save debug information to JSON file."""
        import json
        
        debug_data = {
            'simulation_start': self.simulation_start_time,
            'total_steps': self.step_count,
            'log_file': self.log_file,
            'debug_info': self.debug_info
        }
        
        with open(f"{self.log_file.replace('.log', '.json')}", 'w') as f:
            json.dump(debug_data, f, indent=2)
        
        self.logger.info(f"Debug information saved to {self.log_file.replace('.log', '.json')}")
    
    def create_debug_plots(self, output_file="debug_plots.png"):
        """Create debug plots from logged data."""
        import matplotlib.pyplot as plt
        
        if 'steps' not in self.debug_info:
            print("No debug data to plot")
            return
        
        steps = [d['step'] for d in self.debug_info['steps']]
        firing_rates = [d['firing_rate'] for d in self.debug_info['steps']]
        e_i_ratios = [d['e_i_ratio'] for d in self.debug_info['steps']]
        step_times = [d['step_time'] for d in self.debug_info['steps']]
        
        fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(12, 10))
        
        # Firing rate over time
        ax1.plot(steps, firing_rates, 'b-')
        ax1.set_xlabel('Step')
        ax1.set_ylabel('Firing Rate (Hz)')
        ax1.set_title('Firing Rate Over Time')
        ax1.grid(True)
        
        # E/I ratio over time
        ax2.plot(steps, e_i_ratios, 'r-')
        ax2.set_xlabel('Step')
        ax2.set_ylabel('E/I Ratio')
        ax2.set_title('E/I Ratio Over Time')
        ax2.set_ylim(0, 2)
        ax2.grid(True)
        
        # Step time over time
        ax3.plot(steps, step_times, 'g-')
        ax3.set_xlabel('Step')
        ax3.set_ylabel('Step Time (s)')
        ax3.set_title('Step Time Over Time')
        ax3.grid(True)
        
        # Cumulative spikes over time
        cumulative_spikes = []
        cumulative = 0
        for d in self.debug_info['steps']:
            cumulative += d['total_spikes']
            cumulative_spikes.append(cumulative)
        
        ax4.plot(steps, cumulative_spikes, 'purple')
        ax4.set_xlabel('Step')
        ax4.set_ylabel('Cumulative Spikes')
        ax4.set_title('Cumulative Spikes Over Time')
        ax4.grid(True)
        
        plt.tight_layout()
        plt.savefig(output_file)
        plt.show()
        
        print(f"Debug plots saved to {output_file}")

# Usage example
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

debugger = NLMDebugger("nlm_debug.log")
debugger.log_initialization(brain)

# Run simulation with debugging
for step in range(100):
    import time
    step_start = time.time()
    
    brain.step(step)
    
    step_end = time.time()
    debugger.log_step(step, brain, step_end - step_start)

debugger.log_simulation_summary(brain)
debugger.create_debug_plots("nlm_debug_analysis.png")
```

### 4. Performance Analyzer (`PerformanceAnalyzer`)

```python
import pynlm
from analysis import PerformanceAnalyzer
import time
import statistics

class NLMPerformanceProfiler:
    """Analyze NLM simulation performance characteristics."""
    
    def __init__(self, brain):
        self.brain = brain
        self.timing_data = []
        self.memory_samples = []
        self.step_count = 0
    
    def profile_step(self, step):
        """Profile a single simulation step."""
        start_time = time.time()
        
        # Run the step
        self.brain.step(step)
        
        end_time = time.time()
        step_time = end_time - start_time
        
        self.timing_data.append(step_time)
        self.step_count += 1
        
        return step_time
    
    def analyze_performance(self):
        """Analyze performance metrics."""
        if not self.timing_data:
            print("No performance data to analyze")
            return
        
        # Calculate timing statistics
        mean_time = statistics.mean(self.timing_data)
        median_time = statistics.median(self.timing_data)
        std_time = statistics.stdev(self.timing_data) if len(self.timing_data) > 1 else 0.0
        min_time = min(self.timing_data)
        max_time = max(self.timing_data)
        
        # Calculate performance metrics
        total_time = sum(self.timing_data)
        steps_per_second = self.step_count / total_time
        
        print("=== Performance Analysis ===")
        print(f"Total steps: {self.step_count}")
        print(f"Total simulation time: {total_time:.3f}s")
        print(f"Average step time: {mean_time:.6f}s")
        print(f"Median step time: {median_time:.6f}s")
        print(f"Std deviation: {std_time:.6f}s")
        print(f"Min step time: {min_time:.6f}s")
        print(f"Max step time: {max_time:.6f}s")
        print(f"Steps per second: {steps_per_second:.2f}")
        
        # Performance classification
        if mean_time < 0.001:
            performance_class = "Excellent"
        elif mean_time < 0.01:
            performance_class = "Good"
        elif mean_time < 0.1:
            performance_class = "Acceptable"
        else:
            performance_class = "Poor"
        
        print(f"Performance class: {performance_class}")
        
        return {
            'mean_time': mean_time,
            'median_time': median_time,
            'std_time': std_time,
            'min_time': min_time,
            'max_time': max_time,
            'steps_per_second': steps_per_second,
            'performance_class': performance_class
        }
    
    def create_performance_report(self, output_file="performance_report.txt"):
        """Create a detailed performance report."""
        analysis = self.analyze_performance()
        
        with open(output_file, 'w') as f:
            f.write("NLM Performance Report\n")
            f.write("=" * 50 + "\n\n")
            
            f.write(f"Total steps analyzed: {self.step_count}\n")
            f.write(f"Total simulation time: {sum(self.timing_data):.3f}s\n")
            f.write(f"Average step time: {analysis['mean_time']:.6f}s\n")
            f.write(f"Median step time: {analysis['median_time']:.6f}s\n")
            f.write(f"Standard deviation: {analysis['std_time']:.6f}s\n")
            f.write(f"Minimum step time: {analysis['min_time']:.6f}s\n")
            f.write(f"Maximum step time: {analysis['max_time']:.6f}s\n")
            f.write(f"Steps per second: {analysis['steps_per_second']:.2f}\n")
            f.write(f"Performance class: {analysis['performance_class']}\n\n")
            
            # Step time distribution
            f.write("Step Time Distribution:\n")
            f.write("Less than 0.001s: {} steps\n".format(
                sum(1 for t in self.timing_data if t < 0.001)))
            f.write("0.001s - 0.01s: {} steps\n".format(
                sum(1 for t in self.timing_data if 0.001 <= t < 0.01)))
            f.write("0.01s - 0.1s: {} steps\n".format(
                sum(1 for t in self.timing_data if 0.01 <= t < 0.1)))
            f.write("Greater than 0.1s: {} steps\n".format(
                sum(1 for t in self.timing_data if t >= 0.1)))
        
        print(f"Performance report saved to {output_file}")

# Usage example
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

profiler = NLMPerformanceProfiler(brain)

# Run simulation with profiling
for step in range(100):
    profiler.profile_step(step)

profiler.create_performance_report("nlm_performance_report.txt")
```

## Installation and Setup

### Installing Visualization Tools

```bash
# Install the visualization tools
pip install -e .[visualization]

# Install additional dependencies for plotting
pip install matplotlib seaborn plotly
```

### Running Visualization Examples

```bash
# Run visualization examples
python examples/network_visualization.py
python examples/activity_monitoring.py
python examples/debug_logging.py
python examples/performance_profiling.py
```

## Files in This Directory

### visualization/
directory
- `__init__.py` - Package initialization
- `network_visualizer.py` - Network visualization tools
- `activity_monitor.py` - Activity monitoring tools
- `visualization_utils.py` - Common visualization utilities

### debugging/
directory
- `__init__.py` - Package initialization
- `debug_logger.py` - Debug logging tools
- `step_tracker.py` - Step tracking and debugging
- `error_handler.py` - Error handling and reporting

### analysis/
directory
- `__init__.py` - Package initialization
- `performance_analyzer.py` - Performance analysis tools
- `memory_analyzer.py` - Memory usage analysis
- `connectivity_analyzer.py` - Network connectivity analysis

### examples/
directory
- `network_visualization.py` - Network visualization examples
- `activity_monitoring.py` - Activity monitoring examples
- `debug_logging.py` - Debug logging examples
- `performance_profiling.py` - Performance profiling examples
- `experimental_visualization.py` - Experimental visualization examples

### docs/
directory
- `visualization_guide.md` - Visualization guide
- `debugging_guide.md` - Debugging guide
- `performance_analysis.md` - Performance analysis guide
- `examples_guide.md` - Examples guide

## Usage Examples

### Basic Network Visualization

```python
from visualization import NetworkVisualizer

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

viz = NetworkVisualizer(brain)
viz.create_network_diagram("network.png")
viz.create_spike_raster("spike_raster.png")
viz.create_activity_heatmap("activity_heatmap.png")
```

### Advanced Activity Monitoring

```python
from visualization import ActivityMonitor

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

monitor = ActivityMonitor(brain)
monitor.start_monitoring()

for step in range(100):
    brain.step(step)
    monitor.record_activity()

monitor.stop_monitoring()
monitor.create_plots("activity_report.png")
```

### Comprehensive Debugging

```python
from debugging import DebugLogger

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

debugger = DebugLogger("simulation_debug.log")
debugger.log_initialization(brain)

for step in range(100):
    brain.step(step)
    debugger.log_step(step, brain)

debugger.log_simulation_summary(brain)
debugger.create_debug_plots("debug_analysis.png")
```

### Performance Analysis

```python
from analysis import PerformanceAnalyzer

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

profiler = PerformanceAnalyzer(brain)

for step in range(100):
    profiler.profile_step(step)

profiler.create_performance_report("perf_report.txt")
```

## Configuration

### Configuration Options

```python
config = pynlm.createDefaultConfig()

# Visualization settings
config.set("visualization.enabled", True)
config.set("visualization.output_format", "png")
config.set("visualization.output_directory", "./visualizations")

# Debugging settings
config.set("debug.logging_level", "DEBUG")
config.set("debug.log_file", "nlm_debug.log")
config.set("debug.enable_step_tracking", True)
config.set("debug.enable_performance_monitoring", True)

# Performance settings
config.set("performance.enable_profiling", True)
config.set("performance.memory_sampling", True)
config.set("performance.sampling_interval", 10)
```

### Example Configuration File

```json
{
    "visualization": {
        "enabled": true,
        "output_format": "png",
        "output_directory": "./visualizations",
        "network_visualization": true,
        "activity_monitoring": true,
        "heatmap_resolution": 100
    },
    "debugging": {
        "logging_level": "DEBUG",
        "log_file": "nlm_debug.log",
        "enable_step_tracking": true,
        "enable_performance_monitoring": true,
        "error_reporting": true
    },
    "performance": {
        "enable_profiling": true,
        "memory_sampling": true,
        "sampling_interval": 10,
        "benchmark_mode": false
    }
}
```

## Advanced Features

### Custom Visualizations

```python
from visualization import NetworkVisualizer

class CustomVisualizer(NetworkVisualizer):
    def create_custom_network_view(self, brain, output_file="custom_network.png"):
        """Create a custom network visualization."""
        
        # Get neuron data
        neurons = brain.get_neurons()
        synapses = brain.get_synapses()
        
        # Create custom visualization
        # Your custom visualization code here
        
        plt.savefig(output_file)
        plt.close()
```

### Automated Experiment Tracking

```python
import pynlm
from visualization import ActivityMonitor
from debugging import DebugLogger

class ExperimentTracker:
    """Track and analyze NLM experiments."""
    
    def __init__(self, experiment_name):
        self.experiment_name = experiment_name
        self.start_time = time.time()
        self.brain = None
        self.monitor = None
        self.debugger = None
    
    def run_experiment(self, config, steps):
        """Run an experiment with full tracking."""
        
        # Initialize components
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        self.monitor = ActivityMonitor(self.brain)
        self.debugger = DebugLogger(f"{self.experiment_name}_debug.log")
        
        # Start tracking
        self.monitor.start_monitoring()
        self.debugger.log_initialization(self.brain)
        
        # Run experiment
        for step in range(steps):
            self.brain.step(step)
            self.monitor.record_activity()
            self.debugger.log_step(step, self.brain)
        
        # Stop tracking and save results
        self.monitor.stop_monitoring()
        self.debugger.log_simulation_summary(self.brain)
        
        # Save experiment data
        self.save_experiment_data()
        
        return self.brain
    
    def save_experiment_data(self):
        """Save experiment data and visualizations."""
        
        # Save brain state
        timestamp = time.strftime("%Y%m%d_%H%M%S")
        brain_file = f"{self.experiment_name}_brain_{timestamp}.bin"
        self.brain.save(brain_file)
        
        # Save visualizations
        self.monitor.create_plots(f"{self.experiment_name}_activity_{timestamp}.png")
        self.debugger.create_debug_plots(f"{self.experiment_name}_debug_{timestamp}.png")
        
        # Save configuration
        config = self.brain.get_config()
        config_file = f"{self.experiment_name}_config_{timestamp}.json"
        config.saveToFile(config_file)
        
        print(f"Experiment data saved:")
        print(f"  Brain state: {brain_file}")
        print(f"  Activity plots: {self.experiment_name}_activity_{timestamp}.png")
        print(f"  Debug plots: {self.experiment_name}_debug_{timestamp}.png")
        print(f"  Config: {config_file}")
```

## Best Practices

### 1. Performance Considerations

- Use sampling for long simulations to reduce I/O
- Limit visualization output to key milestones
- Use memory-efficient data structures for large networks
- Implement incremental saving to prevent data loss

### 2. Debugging Strategies

- Log at appropriate levels (DEBUG for development, INFO for production)
- Use structured logging with consistent formats
- Implement automated test failures for critical errors
- Create reproducible debugging scripts

### 3. Visualization Guidelines

- Use color coding to represent different neural properties
- Provide zoom and pan capabilities for large networks
- Include comprehensive legends and annotations
- Support multiple output formats for different use cases

### 4. Data Management

- Implement automatic cleanup of temporary files
- Use compression for large data files
- Create metadata files for experiment tracking
- Implement versioning for different analysis outputs

## Troubleshooting

### Common Issues and Solutions

#### Visualization Not Working
```
# Ensure matplotlib is installed
pip install matplotlib

# Check for display issues on headless systems
import matplotlib
matplotlib.use('Agg')  # Use non-interactive backend
```

#### Debug Log Not Created
```
# Check file permissions
import os
os.chmod("nlm_debug.log", 0o644)

# Verify log directory exists
log_dir = os.path.dirname("nlm_debug.log")
if not os.path.exists(log_dir):
    os.makedirs(log_dir)
```

#### Performance Profiling Slow
```
# Reduce sampling frequency for large simulations
config.set("performance.sampling_interval", 50)

# Use efficient data structures
import numpy as np
```

### Getting Help

If you encounter issues with the visualization or debugging tools:

1. **Check the documentation** in the `docs/` directory
2. **Review example scripts** in the `examples/` directory
3. **Use the built-in help system**:
   ```python
   import pynlm
   help(pynlm.visualization)
   help(pynlm.debugging)
   help(pynlm.analysis)
   ```
4. **Create an issue** on the GitHub repository with:
   - Description of the problem
   - Code that reproduces the issue
   - Log files and error messages
   - System information and NLM version

## Conclusion

The NLM visualization and debugging tools provide comprehensive support for:

- **Network visualization** and activity monitoring
- **Debug logging** and error tracking
- **Performance analysis** and profiling
- **Experimental tracking** and reproducibility

These tools help users:
- Understand and validate their NLM simulations
- Identify performance bottlenecks and issues
- Create reproducible and well-documented experiments
- Analyze complex neural dynamics and behaviors

The modular design allows users to customize and extend the tools for their specific research needs, making NLM more accessible and easier to use for both new and experienced users.
