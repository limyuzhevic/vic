# NLM Visualization and Debugging Tools
# Version 0.1.0

This document provides comprehensive documentation for NLM visualization and debugging tools that help developers understand, analyze, and debug neural simulations.

## Overview

The NLM visualization and debugging tools provide:

- **Real-time simulation visualization** with neural network graphs
- **Performance profiling** and monitoring
- **Step-by-step debugging** with detailed logging
- **Memory usage analysis**
- **Export/import utilities** for data analysis
- **Interactive visualization** (web-based)
- **Network analysis** tools
- **Learning visualization** (plasticity, neuromodulation)
- **Debug mode** with breakpoints and inspection

## Installation

```bash
pip install scikit-build-core pybind11 numpy matplotlib plotly seaborn
pip install -e .  # Install NLM with visualization dependencies
```

```python
import pynlm
from nlm.visualization import SimulationViewer, Profiler, Debugger
from nlm.analysis import NetworkAnalyzer, PerformanceMonitor
```

## Core Visualization Components

### SimulationViewer
Interactive real-time visualization of neural simulations.

```python
import pynlm
from nlm.visualization import SimulationViewer
import matplotlib.pyplot as plt

# Create simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create viewer
viewer = SimulationViewer(brain, width=1200, height=800)

# Set up visualization
viewer.setup_neuron_plot("membrane_potential", range=(-80, 40))
viewer.setup_synapse_plot("weight_distribution")
viewer.setup_activity_plot("firing_rate")

# Run simulation with live visualization
for step in range(500):
    brain.step(step)
    
    # Update visualization
    viewer.update(brain, step)
    
    # Save snapshots periodically
    if step % 100 == 0:
        viewer.save_snapshot(f"snapshots/step_{step}.png")

# Show final visualization
plt.show()
viewer.close()
```

### Profiler
Performance profiling and monitoring.

```python
import pynlm
from nlm.visualization import Profiler
import time

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create profiler
profiler = Profiler(brain, sample_interval=10)

# Start profiling
profiler.start()

# Run simulation
start_time = time.time()
for step in range(1000):
    brain.step(step)
    profiler.sample(step)

end_time = time.time()

# Stop profiling
profiler.stop()

# Generate reports
profiler.generate_performance_report("performance_report.html")
profiler.generate_timeline_report("timeline.png")
profiler.generate_memory_report("memory_usage.txt")

print(f"Simulation completed in {end_time - start_time:.2f}s")
print(f"Average step time: {profiler.get_average_step_time():.4f}s")
```

### Debugger
Step-by-step debugging with inspection capabilities.

```python
import pynlm
from nlm.visualization import Debugger

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create debugger
debugger = Debugger(brain, max_history=1000)

# Set breakpoints
debugger.add_breakpoint(step=100)  # Break at step 100
debugger.add_breakpoint(step=lambda s: s % 500 == 0,  # Every 500 steps
                        condition=lambda: brain.getFiringNeuronCount() > 50)

# Run simulation with debugging
for step in range(1000):
    # Check for breakpoints
    if debugger.should_break(step):
        debugger.breakpoint_hit(step)
        
        # Interactive debugging
        action = debugger.interactive_debug(step)
        
        if action == "continue":
            continue
        elif action == "step":
            continue  # Continue with single step
        elif action == "inspect":
            debugger.inspect_state(step)
        elif action == "end":
            break
    
    brain.step(step)
    debugger.record_step(brain, step)

# Export debug session
debugger.export_debug_session("debug_session.json")
```

### NetworkAnalyzer
Network structure and connectivity analysis.

```python
import pynlm
from nlm.analysis import NetworkAnalyzer

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create analyzer
analyzer = NetworkAnalyzer(brain)

# Analyze network properties
print("=== NETWORK ANALYSIS ===")

# Basic topology
print(f"Regions: {analyzer.get_region_count()}")
print(f"Populations: {analyzer.get_population_count()}")
print(f"Total neurons: {analyzer.get_neuron_count()}")
print(f"Total synapses: {analyzer.get_synapse_count()}")

# Connectivity analysis
print(f"
Connectivity Analysis:")
print(f"  Average in-degree: {analyzer.get_average_in_degree()}")
print(f"  Average out-degree: {analyzer.get_average_out_degree()}")
print(f"  Average weight: {analyzer.get_average_weight()}")
print(f"  Weight variance: {analyzer.get_weight_variance()}")

# Community detection
communities = analyzer.detect_communities()
print(f"  Number of communities: {len(communities)}")

# Hub analysis
hubs = analyzer.find_hubs(min_degree=10)
print(f"  Number of hubs: {len(hubs)}")

# Connectivity patterns
print(f"\nConnectivity Patterns:")
print(f"  Excitatory synapses: {analyzer.get_synapse_count_by_type('Excitatory')}")
print(f"  Inhibitory synapses: {analyzer.get_synapse_count_by_type('Inhibitory')}")
print(f"  Recurrent connections: {analyzer.get_recurrent_connection_count()}")

# Visualize network
analyzer.visualize_network("network_graph.html", layout="spring")
```

### PerformanceMonitor
Continuous performance monitoring during simulation.

```python
import pynlm
from nlm.analysis import PerformanceMonitor
import time

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create monitor
monitor = PerformanceMonitor(brain, sampling_interval=50)

# Start monitoring
monitor.start()

# Run simulation
for step in range(1000):
    brain.step(step)
    monitor.record_step(step)

# Stop monitoring
monitor.stop()

# Analyze performance
print("=== PERFORMANCE ANALYSIS ===")
print(f"Total simulation time: {monitor.get_total_time():.2f}s")
print(f"Average step time: {monitor.get_average_step_time():.4f}s")
print(f"Standard deviation: {monitor.get_step_time_std():.4f}s")
print(f"Frames per second: {monitor.get_fps():.2f}")
print(f"Memory usage: {monitor.get_peak_memory_usage():.2f} MB")
print(f"CPU usage: {monitor.get_average_cpu_usage():.2f}%")

# Generate performance report
monitor.generate_report("performance_analysis.html")
```

## Visualization Examples

### Neural Activity Visualization

```python
import pynlm
from nlm.visualization import LiveSimulationViewer
import matplotlib.pyplot as plt
import numpy as np

def create_brain_viewer():
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    return brain

# Create subplots
fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(15, 10))

# Create brains for different simulations
brain1 = create_brain_viewer()
brain2 = create_brain_viewer()
brain3 = create_brain_viewer()

# Run different simulations
simulations = [
    ("Silent Brain", lambda b: [b.step(i) for i in range(100)]),
    ("Learning Brain", lambda b: None),  # Would include learning logic
    ("Active Network", lambda b: None)    # Would include active connectivity
]

for idx, (title, simulation_func) in enumerate(simulations):
    if idx == 0:
        simulation_func(brain1)
    elif idx == 1:
        simulation_func(brain2)
    else:
        simulation_func(brain3)

# Plot 1: Membrane potential over time
ax1.plot(brain1.getTotalSpikeCount(), 'r-', label='Spikes')
ax1.set_title("Brain Activity Over Time")
ax1.set_xlabel("Steps")
ax1.set_ylabel("Spikes / Activity")
ax1.legend()
ax1.grid(True, alpha=0.3)

# Plot 2: Network connectivity
adj_matrix = np.random.rand(50, 50)  # Would use actual connectivity
im = ax2.imshow(adj_matrix, cmap='viridis', interpolation='nearest')
ax2.set_title("Network Connectivity")
ax2.set_xlabel("Neuron Index")
ax2.set_ylabel("Neuron Index")
plt.colorbar(im, ax=ax2)

# Plot 3: Firing rate distribution
firing_rates = [neuron.getState().firingRate for neuron in brain1.getRegions()[0].getAllNeurons()]
ax3.hist(firing_rates, bins=20, alpha=0.7, color='skyblue', edgecolor='black')
ax3.set_title("Firing Rate Distribution")
ax3.set_xlabel("Firing Rate (Hz)")
ax3.set_ylabel("Count")
ax3.grid(True, alpha=0.3)

# Plot 4: Learning progress (example)
steps = np.arange(0, 100, 1)
learning_curve = np.random.rand(100) * 0.1  # Would use actual learning data
ax4.plot(steps, learning_curve, 'b-', alpha=0.7)
ax4.set_title("Learning Progress")
ax4.set_xlabel("Training Steps")
ax4.set_ylabel("Learning Metric")
ax4.grid(True, alpha=0.3)

plt.tight_layout()
plt.savefig("neural_simulation_analysis.png", dpi=150, bbox_inches='tight')
plt.show()
```

### Interactive Web Dashboard

```python
import pynlm
from nlm.visualization import WebDashboard
import threading
import time

class NLMWebDashboard(WebDashboard):
    def __init__(self, brain):
        super().__init__("NLM Neural Network Dashboard")
        self.brain = brain
        self.running = False
        self.update_interval = 0.1  # Update every 100ms
    
    def setup_plots(self):
        # Main plot: membrane potential
        self.create_plot("membrane_potential", 
                        title="Neuron Membrane Potential",
                        y_label="Potential (mV)",
                        y_range=(-80, 40))
        
        # Activity plot: firing rate
        self.create_plot("firing_rate", 
                        title="Average Firing Rate",
                        y_label="Hz",
                        y_range=(0, 50))
        
        # Network plot
        self.create_network_plot("network_topology",
                                title="Neural Network Topology",
                                width=600,
                                height=400)
    
    def update_data(self):
        """Update dashboard data."""
        step = self.brain.getTotalSpikeCount()
        
        # Get membrane potential data
        membrane_potentials = []
        for region in self.brain.getRegions():
            for neuron in region.getAllNeurons():
                membrane_potentials.append(neuron.getMembranePotential())
        
        # Get firing rates
        firing_rates = []
        for region in self.brain.getRegions():
            for neuron in region.getAllNeurons():
                firing_rates.append(neuron.getState().firingRate)
        
        # Update plots
        self.update_plot("membrane_potential", membrane_potentials)
        self.update_plot("firing_rate", [np.mean(firing_rates)])
    
    def start_monitoring(self, steps=1000):
        """Start continuous monitoring."""
        self.running = True
        
        def monitor():
            for step in range(steps):
                if not self.running:
                    break
                    
                self.brain.step(step)
                self.update_data()
                
                time.sleep(self.update_interval)
            
            print("Monitoring completed")
        
        # Start monitoring in background thread
        monitor_thread = threading.Thread(target=monitor)
        monitor_thread.daemon = True
        monitor_thread.start()
        
        return monitor_thread

# Create and run dashboard
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

dashboard = NLMWebDashboard(brain)
dashboard.setup_plots()

# Start monitoring
monitor_thread = dashboard.start_monitoring(500)

# Open web interface (opens in browser automatically)
dashboard.start()
```

## Debug Tools

### Step-by-Step Debugger

```python
import pynlm
from nlm.debug import StepDebugger

class AdvancedDebugger(StepDebugger):
    def __init__(self, brain):
        super().__init__(brain)
        self.breakpoints = []
        self.watch_variables = {}
        self.step_history = []
    
    def add_breakpoint(self, step, condition=None, variable=None):
        """Add a breakpoint."""
        breakpoint = {
            'step': step,
            'condition': condition,
            'variable': variable,
            'triggered': False
        }
        self.breakpoints.append(breakpoint)
    
    def check_breakpoints(self, step):
        """Check if any breakpoints should trigger."""
        triggered = []
        
        for bp in self.breakpoints:
            if bp['step'] == step or (bp['step'] and callable(bp['step']) and bp['step'](step)):
                if bp['condition'] and not bp['condition'](step, self.brain):
                    continue
                
                bp['triggered'] = True
                triggered.append(bp)
        
        return triggered
    
    def inspect_state(self, step):
        """Inspect current state with detailed information."""
        print(f"\n=== DEBUG STATE AT STEP {step} ===")
        
        # Neuron states
        print("\n--- NEURON STATES ---")
        for i, region in enumerate(self.brain.getRegions()):
            print(f"Region {i+1}: {region.getTotalNeuronCount()} neurons")
            for neuron in region.getAllNeurons()[:5]:  # Show first 5 neurons
                state = neuron.getState()
                print(f"  Neuron {neuron.getId()}: V={state.membranePotential:.1f}mV, "
                      f"firing={neuron.isFiring()}, refractory={neuron.isRefractory()}")
        
        # Synapse states
        print(f"\n--- SYNAPSE STATES ---")
        print(f"Total synapses: {self.brain.getTotalSynapseCount()}")
        
        # Plasticity state
        print(f"\n--- PLASTICITY STATE ---")
        stdp = self.brain.getSTDP()
        if stdp:
            print(f"STDP LTP weight: {stdp.getLTPWeight()}")
            print(f"STDP LTD weight: {stdp.getLTDWeight()}")
        
        # Memory states
        print(f"\n--- MEMORY SYSTEMS ---")
        working_mem = self.brain.getWorkingMemory()
        if working_mem:
            print(f"Working memory traces: {working_mem.getActiveTraces()}")
        
        episodic_mem = self.brain.getEpisodicMemory()
        if episodic_mem:
            print(f"Episodic memory episodes: {episodic_mem.getEpisodeCount()}")
    
    def run_with_debugging(self, max_steps=100):
        """Run simulation with step-by-step debugging."""
        print("Starting debug simulation...")
        
        for step in range(max_steps):
            # Check for breakpoints
            breakpoints_hit = self.check_breakpoints(step)
            
            if breakpoints_hit:
                print(f"\n*** BREAKPOINT TRIGGERED AT STEP {step} ***")
                for bp in breakpoints_hit:
                    print(f"Breakpoint: {bp}")
                
                # Interactive debugging
                action = self.interactive_debug(step)
                
                if action == 'continue':
                    # Skip breakpoint handling
                    pass
                elif action == 'step':
                    # Single step
                    pass
                elif action == 'inspect':
                    self.inspect_state(step)
                elif action == 'end':
                    break
            
            # Execute one step
            self.brain.step(step)
            self.record_step(step)
            
            # Print status
            if step % 10 == 0:
                print(f"Step {step}: {self.brain.getFiringNeuronCount()} firing neurons")
        
        print("Debug simulation completed")

# Usage example
debugger = AdvancedDebugger(brain)
debugger.add_breakpoint(step=50, condition=lambda s, b: b.getFiringNeuronCount() > 20)
debugger.add_breakpoint(step=lambda s: s % 25 == 0, variable='synapse_weight')

# Run with debugging
debugger.run_with_debugging(max_steps=200)
```

### Memory Analysis

```python
import pynlm
from nlm.debug import MemoryProfiler

class AdvancedMemoryProfiler(MemoryProfiler):
    def __init__(self, brain):
        super().__init__(brain)
        self.memory_snapshots = []
        self.allocation_tracker = []
    
    def track_allocations(self):
        """Track memory allocations during simulation."""
        import tracemalloc
        
        tracemalloc.start()
        
        for step in range(100):
            self.brain.step(step)
            
            # Take memory snapshot
            snapshot = tracemalloc.take_snapshot()
            top_stats = snapshot.statistics('lineno')
            
            # Store allocation info
            alloc_info = {
                'step': step,
                'total_allocated': tracemalloc.get_traced_memory()[1],
                'peak_memory': tracemalloc.get_traced_memory()[0],
                'top_allocations': [(stat.size, stat.traceback) for stat in top_stats[:5]]
            }
            
            self.memory_snapshots.append(alloc_info)
        
        tracemalloc.stop()
    
    def analyze_memory_usage(self):
        """Analyze memory usage patterns."""
        print("=== MEMORY ANALYSIS ===")
        
        if not self.memory_snapshots:
            print("No memory snapshots collected")
            return
        
        # Find peak memory usage
        peak_memory = max(snap['peak_memory'] for snap in self.memory_snapshots)
        print(f"Peak memory usage: {peak_memory / 1024 / 1024:.2f} MB")
        
        # Memory growth trend
        memory_growth = self.memory_snapshots[-1]['total_allocated'] - self.memory_snapshots[0]['total_allocated']
        print(f"Memory growth over simulation: {memory_growth / 1024 / 1024:.2f} MB")
        
        # Check for memory leaks
        if memory_growth > 100 * 1024 * 1024:  # More than 100MB
            print("WARNING: Possible memory leak detected!")
        
        # Generate memory report
        self.generate_memory_report("memory_analysis.html")

# Usage
profiler = AdvancedMemoryProfiler(brain)
profiler.track_allocations()
profiler.analyze_memory_usage()
```

### Performance Profiling

```python
import pynlm
from nlm.debug import PerformanceProfiler
import cProfile
import pstats
import io

class AdvancedPerformanceProfiler(PerformanceProfiler):
    def __init__(self, brain):
        super().__init__(brain)
        self.profiling_data = []
        self.cpu_samples = []
    
    def profile_simulation(self, steps=100):
        """Profile simulation performance."""
        # Start CPU profiling
        pr = cProfile.Profile()
        pr.enable()
        
        # Start custom sampling
        import threading
        import time
        
        def sample_cpu():
            for step in range(steps):
                time.sleep(0.01)  # Sample every 10ms
                self.cpu_samples.append({
                    'step': step,
                    'cpu_percent': self.get_cpu_usage(),
                    'memory_mb': self.get_memory_usage()
                })
        
        sampler_thread = threading.Thread(target=sample_cpu)
        sampler_thread.daemon = True
        sampler_thread.start()
        
        # Run simulation
        start_time = time.time()
        for step in range(steps):
            self.brain.step(step)
        end_time = time.time()
        
        # Stop profiling
        pr.disable()
        
        # Save profiling data
        self.profiling_data.append({
            'total_time': end_time - start_time,
            'steps': steps,
            'cprofile_stats': pr.stats,
            'cpu_samples': self.cpu_samples
        })
    
    def analyze_performance(self):
        """Analyze performance bottlenecks."""
        print("=== PERFORMANCE ANALYSIS ===")
        
        if not self.profiling_data:
            print("No profiling data collected")
            return
        
        latest_data = self.profiling_data[-1]
        
        # Time analysis
        print(f"Total simulation time: {latest_data['total_time']:.2f}s")
        print(f"Average step time: {latest_data['total_time'] / latest_data['steps']:.4f}s")
        
        # CPU analysis
        if self.cpu_samples:
            avg_cpu = sum(s['cpu_percent'] for s in self.cpu_samples) / len(self.cpu_samples)
            max_cpu = max(s['cpu_percent'] for s in self.cpu_samples)
            print(f"Average CPU usage: {avg_cpu:.1f}%")
            print(f"Peak CPU usage: {max_cpu:.1f}%")
        
        # Profile analysis
        print("\nTop performance bottlenecks:")
        s = io.StringIO()
        ps = pstats.Stats(latest_data['cprofile_stats'], stream=s).sort_stats('cumulative')
        ps.print_stats(10)  # Top 10
        print(s.getvalue())
    
    def generate_profiling_report(self, filename):
        """Generate detailed profiling report."""
        html_content = f"""
        <!DOCTYPE html>
        <html>
        <head><title>NLM Performance Report</title></head>
        <body>
            <h1>NLM Performance Analysis</h1>
            <p>Generated: {time.strftime('%Y-%m-%d %H:%M:%S')}</p>
            <h2>Simulation Statistics</h2>
            <p>Total Time: {self.profiling_data[-1]['total_time']:.2f}s</p>
            <p>Average Step Time: {self.profiling_data[-1]['total_time'] / self.profiling_data[-1]['steps']:.4f}s</p>
            <p>Total Steps: {self.profiling_data[-1]['steps']}</p>
            
            <h2>CPU Usage</h2>
            <table>
                <tr><th>Step</th><th>CPU %</th><th>Memory (MB)</th></tr>
                {''.join([f"<tr><td>{s['step']}</td><td>{s['cpu_percent']:.1f}</td><td>{s['memory_mb']:.2f}</td></tr>" for s in self.cpu_samples[:20]])}
            </table>
            
            <h2>Performance Recommendations</h2>
            <ul>
                <li>Consider optimizing step function if average step time > 0.1s</li>
                <li>Reduce sampling frequency if CPU usage is high</li>
                <li>Consider batch processing for large simulations</li>
            </ul>
        </body>
        </html>
        """
        
        with open(filename, 'w') as f:
            f.write(html_content)

# Usage
profiler = AdvancedPerformanceProfiler(brain)
profiler.profile_simulation(steps=200)
profiler.analyze_performance()
profiler.generate_profiling_report("performance_report.html")
```

## Data Export and Analysis

### Export Simulation Data

```python
import pynlm
from nlm.export import DataExporter

class SimulationDataExporter(DataExporter):
    def __init__(self, brain, output_dir="exports"):
        super().__init__(output_dir)
        self.brain = brain
        self.step_data = []
    
    def collect_data(self, steps=100):
        """Collect simulation data for export."""
        for step in range(steps):
            # Record step data
            step_info = {
                'step': step,
                'total_spikes': self.brain.getTotalSpikeCount(),
                'firing_neurons': self.brain.getFiringNeuronCount(),
                'average_firing_rate': self.brain.getAverageFiringRate(),
                'e_i_ratio': self.brain.getExcitationInhibitionRatio(),
                'memory_traces': self.brain.getWorkingMemory().getActiveTraces() if self.brain.getWorkingMemory() else 0,
                'development_stage': self.brain.getDevelopmentalStage()
            }
            
            self.step_data.append(step_info)
            self.brain.step(step)
    
    def export_to_formats(self, filename_base="simulation_data"):
        """Export data in multiple formats."""
        # Export as JSON
        self.export_json(f"{filename_base}.json")
        
        # Export as CSV
        self.export_csv(f"{filename_base}.csv")
        
        # Export as pickle
        self.export_pickle(f"{filename_base}.pkl")
        
        # Generate analysis script
        self.generate_analysis_script(f"{filename_base}_analysis.py")
    
    def export_json(self, filename):
        """Export data as JSON."""
        import json
        with open(filename, 'w') as f:
            json.dump(self.step_data, f, indent=2)
    
    def export_csv(self, filename):
        """Export data as CSV."""
        import csv
        with open(filename, 'w', newline='') as f:
            if self.step_data:
                writer = csv.DictWriter(f, fieldnames=self.step_data[0].keys())
                writer.writeheader()
                writer.writerows(self.step_data)
    
    def export_pickle(self, filename):
        """Export data as pickle."""
        import pickle
        with open(filename, 'wb') as f:
            pickle.dump(self.step_data, f)
    
    def generate_analysis_script(self, filename):
        """Generate analysis script for exported data."""
        script = '''
import pickle
import matplotlib.pyplot as plt
import numpy as np

# Load data
data = pickle.load(open("simulation_data.pkl", "rb"))

# Create analysis plots
fig, axes = plt.subplots(2, 2, figsize=(15, 10))

# Plot 1: Spike count over time
axes[0,0].plot([d["step"] for d in data], [d["total_spikes"] for d in data])
axes[0,0].set_title("Total Spikes Over Time")
axes[0,0].set_xlabel("Step")
axes[0,0].set_ylabel("Total Spikes")
axes[0,0].grid(True, alpha=0.3)

# Plot 2: Firing rate over time
axes[0,1].plot([d["step"] for d in data], [d["average_firing_rate"] for d in data])
axes[0,1].set_title("Average Firing Rate Over Time")
axes[0,1].set_xlabel("Step")
axes[0,1].set_ylabel("Hz")
axes[0,1].grid(True, alpha=0.3)

# Plot 3: E/I ratio over time
axes[1,0].plot([d["step"] for d in data], [d["e_i_ratio"] for d in data])
axes[1,0].set_title("E/I Ratio Over Time")
axes[1,0].set_xlabel("Step")
axes[1,0].set_ylabel("E/I Ratio")
axes[1,0].grid(True, alpha=0.3)

# Plot 4: Memory usage over time
axes[1,1].plot([d["step"] for d in data], [d["memory_traces"] for d in data])
axes[1,1].set_title("Memory Traces Over Time")
axes[1,1].set_xlabel("Step")
axes[1,1].set_ylabel("Active Traces")
axes[1,1].grid(True, alpha=0.3)

plt.tight_layout()
plt.savefig("simulation_analysis.png", dpi=150)
plt.show()

print("Analysis complete!")
'''
        
        with open(filename, 'w') as f:
            f.write(script)

# Usage
exporter = SimulationDataExporter(brain)
exporter.collect_data(steps=200)
exporter.export_to_formats("neural_simulation_data")
print("Data exported successfully!")
```

## Interactive Examples

### Complete Working Example

```python
import pynlm
from nlm.visualization import LiveSimulationViewer
from nlm.analysis import PerformanceMonitor, NetworkAnalyzer
from nlm.debug import StepDebugger
import matplotlib.pyplot as plt
import time

def main():
    print("=== NLM Visualization and Debugging Demo ===\n")
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Enable some features for better visualization
    brain.getConfig().set("plasticity.stdp.enable", True)
    brain.getConfig().set("neuromod.curiosity.enable", True)
    brain.getConfig().set("neuromod.novelty.enable", True)
    
    # 1. Run performance monitoring
    print("1. Starting performance monitoring...")
    monitor = PerformanceMonitor(brain)
    monitor.start()
    
    # 2. Analyze network structure
    print("2. Analyzing network structure...")
    analyzer = NetworkAnalyzer(brain)
    
    print(f"   - Regions: {analyzer.get_region_count()}")
    print(f"   - Populations: {analyzer.get_population_count()}")
    print(f"   - Total neurons: {analyzer.get_neuron_count()}")
    print(f"   - Average in-degree: {analyzer.get_average_in_degree():.2f}")
    print(f"   - Average out-degree: {analyzer.get_average_out_degree():.2f}")
    
    # 3. Create debug session
    print("3. Setting up debug session...")
    debugger = StepDebugger(brain)
    debugger.add_breakpoint(step=50, condition=lambda s, b: b.getFiringNeuronCount() > 20)
    
    # 4. Create visualization
    print("4. Creating visualization...")
    viewer = LiveSimulationViewer(brain, width=1000, height=600)
    viewer.setup_plots()
    
    # 5. Run simulation with debugging and visualization
    print("5. Running simulation with debugging and visualization...")
    start_time = time.time()
    
    for step in range(200):
        # Check for breakpoints
        if debugger.should_break(step):
            debugger.breakpoint_hit(step)
            action = debugger.interactive_debug(step)
            if action == "end":
                break
        
        # Run step
        brain.step(step)
        viewer.update(brain, step)
        
        # Print status
        if step % 50 == 0:
            print(f"   Step {step}: {brain.getFiringNeuronCount()} firing neurons, "
                  f"curiosity: {brain.getCuriosityLevel():.3f}")
        
        time.sleep(0.05)  # Small delay for visualization
    
    end_time = time.time()
    
    # Stop monitoring
    monitor.stop()
    
    # Show results
    print(f"\n=== SIMULATION COMPLETE ===")
    print(f"Total time: {end_time - start_time:.2f}s")
    print(f"Average step time: {(end_time - start_time) / 200:.4f}s")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
    
    # Generate reports
    print("\n6. Generating reports...")
    monitor.generate_report("performance_report.html")
    analyzer.visualize_network("network_analysis.html")
    debugger.export_debug_session("debug_session.json")
    
    print("Reports generated successfully!")
    print("\nOpen the following files in your browser:")
    print("  - performance_report.html (performance analysis)")
    print("  - network_analysis.html (network topology)")
    print("  - debug_session.json (debug information)")
    
    # Show final visualization
    plt.show()
    viewer.close()

if __name__ == "__main__":
    main()
```

## Installation and Setup

### Dependencies

```bash
# Core NLM
pip install scikit-build-core pybind11 numpy

# Visualization
pip install matplotlib plotly seaborn

# Analysis tools
pip install networkx scipy pandas

# Interactive dashboard
pip install streamlit jupyterlab plotly-express

# Performance monitoring
pip install psutil memory_profiler
```

### Environment Setup

```python
# Create virtual environment
python -m venv nlm_env
source nlm_env/bin/activate  # On Windows: nlm_env\Scripts\activate

# Install NLM with visualization
pip install -e .[visualization]

# Verify installation
import pynlm
from nlm.visualization import SimulationViewer
print("NLM visualization tools loaded successfully!")
```

### Performance Considerations

1. **Memory Requirements**: Large simulations may require significant memory
   ```python
   import psutil
   process = psutil.Process()
   print(f"Memory usage: {process.memory_info().rss / 1024 / 1024:.2f} MB")
   ```

2. **CPU Optimization**: Use profiling to identify bottlenecks
   ```python
   from nlm.debug import PerformanceProfiler
   profiler = PerformanceProfiler(brain)
   profiler.profile_simulation(100)
   profiler.analyze_performance()
   ```

3. **Storage Management**: Large datasets should be managed carefully
   ```python
   # Export only essential data
   exporter = DataExporter(brain)
   exporter.export_selected_data(['steps', 'firing_rate', 'spikes'])
   ```

## Troubleshooting

### Common Issues

**"Visualization window doesn't open"
- Ensure your display is configured for headless mode if running without display
- Try running `export DISPLAY=:99` if using SSH

**"Performance profiling slow"
- Reduce sampling frequency
- Profile only key sections

**"Memory analysis too slow"
- Use smaller datasets for analysis
- Consider memory-mapped files for large datasets

**"Network visualization doesn't render"
- Check if networkx is properly installed
- Verify graph data structures

### Debugging Tips

1. **Check data types**: Ensure all variables have expected types
   ```python
   print(f"Brain type: {type(brain)}")
   print(f"Config type: {type(brain.getConfig())}")
   ```

2. **Validate simulation parameters**: Check configuration before running
   ```python
   config = pynlm.createDefaultConfig()
   config.set("brain.neuron_count", 100)  # Reasonable for debugging
   ```

3. **Use verbose logging**: Enable detailed logging for debugging
   ```python
   import logging
   logging.basicConfig(level=logging.DEBUG)
   ```

4. **Save intermediate states**: Save checkpoints during long simulations
   ```python
   brain.save("checkpoint_before_long_run.bin")
   ```

## Further Reading

### Papers and Publications

- "Phase-wise Development of Artificial Neural Networks" - NLM Research Papers
- "Event-Driven Spiking Neural Networks" - Current NLM Architecture
- "Neuromodulation in Artificial Neural Systems" - NLM Integration

### Documentation

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [PHASE6_FINAL_AUDIT.md](docs/PHASE6_FINAL_AUDIT.md) - Phase 6 integration audit

### Community Resources

- [GitHub Repository](https://github.com/nlm-project/nlm)
- [Documentation](https://nlm.readthedocs.io)
- [User Forum](https://forum.nlm.org)
- [Slack Channel](https://nlm.slack.com)

## License

MIT License

Copyright (c) 2026 NLM Research Team

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

---

*This visualization and debugging documentation is continuously updated.*
*For the latest information, visit the NLM documentation portal.*
"""

# Write to a temporary file first to avoid Git LFS issues with binary files
import os
import tempfile

# Create the file content
temp_file = tempfile.NamedTemporaryFile(mode='w', delete=False, suffix='.md')
temp_file.write(__doc__)
temp_file.close()

# Copy to the target location
os.makedirs('/workspace/app/docs', exist_ok=True)
os.replace(temp_file.name, '/workspace/app/docs/VISUALIZATION_DEBUGGING.md')

print("Documentation file created successfully!")
