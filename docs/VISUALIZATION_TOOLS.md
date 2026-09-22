# NLM Advanced Visualization and Profiling Tools

## Overview

NLM includes comprehensive visualization and profiling capabilities to help developers understand neural system behavior, optimize performance, and analyze simulation results. These tools provide real-time monitoring, post-simulation analysis, and detailed visualization of neural dynamics.

---

## 1. Real-Time Visualization Interface

### 1.1 Live Brain Monitor

```python
import pynlm
import time
import threading
from typing import Callable, Optional

class RealTimeBrainMonitor:
    """Real-time monitor for brain simulation with live plotting"""
    
    def __init__(self, brain: pynlm.Brain, update_interval: float = 0.1):
        self.brain = brain
        self.update_interval = update_interval
        self.running = False
        self.monitor_thread: Optional[threading.Thread] = None
        self.plot_thread: Optional[threading.Thread] = None
        self.data_points = []
        self.subscribers: list[Callable] = []
    
    def start_monitoring(self, plot_backend: str = "matplotlib"):
        """Start real-time monitoring with live plotting"""
        self.running = True
        self.monitor_thread = threading.Thread(target=self._monitor_loop)
        self.monitor_thread.daemon = True
        self.monitor_thread.start()
        
        if plot_backend == "matplotlib":
            self._setup_matplotlib_plot()
            self.plot_thread = threading.Thread(target=self._matplotlib_loop)
        elif plot_backend == "plotly":
            self._setup_plotly_plot()
        elif plot_backend == "custom":
            self._setup_custom_plot()
        
        if self.plot_thread:
            self.plot_thread.daemon = True
            self.plot_thread.start()
    
    def stop_monitoring(self):
        """Stop monitoring and close plots"""
        self.running = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=1.0)
        if self.plot_thread:
            self.plot_thread.join(timeout=1.0)
    
    def add_subscriber(self, callback: Callable):
        """Add a callback to be notified of new data points"""
        self.subscribers.append(callback)
    
    def _monitor_loop(self):
        """Background loop collecting brain data"""
        step = 0
        while self.running:
            data_point = {
                "step": step,
                "time": step * self.brain.getConfig().getOr("simulation_timestep", 0.001),
                "total_spikes": self.brain.getTotalSpikeCount(),
                "firing_rate": self.brain.getAverageFiringRate(),
                "active_neurons": self.brain.getActiveNeuronCount(),
                "firing_neurons": self.brain.getFiringNeuronCount(),
                "ei_ratio": self.brain.getExcitationInhibitionRatio(),
                "developmental_stage": int(self.brain.getDevelopmentalStage()),
                "dopamine_level": self.brain.getDopamine().getLevel() if self.brain.getDopamine() else 0.0,
                "curiosity_level": self.brain.getCuriosity().getLevel() if self.brain.getCuriosity() else 0.0,
            }
            
            self.data_points.append(data_point)
            
            # Notify subscribers
            for subscriber in self.subscribers:
                try:
                    subscriber(data_point)
                except Exception as e:
                    print(f"Subscriber error: {e}")
            
            step += 1
            time.sleep(self.update_interval)
    
    def _setup_matplotlib_plot(self):
        """Setup matplotlib for live plotting"""
        try:
            import matplotlib.pyplot as plt
            import matplotlib.animation as animation
            
            self.fig, ((self.ax1, self.ax2), (self.ax3, self.ax4)) = plt.subplots(2, 2, figsize=(15, 10))
            plt.ion()  # Interactive mode
            
            # Setup plots
            self.line_spikes, = self.ax1.plot([], [], 'b-', label='Total Spikes')
            self.line_rate, = self.ax2.plot([], [], 'r-', label='Firing Rate')
            self.line_ei, = self.ax3.plot([], [], 'g-', label='E/I Ratio')
            self.line_neurons, = self.ax4.plot([], [], 'm-', label='Active Neurons')
            
            for ax in [self.ax1, self.ax2, self.ax3, self.ax4]:
                ax.set_xlabel('Step')
                ax.set_xlim(0, 1000)
                ax.grid(True, alpha=0.3)
            
            self.ax1.set_ylabel('Count')
            self.ax2.set_ylabel('Hz')
            self.ax3.set_ylabel('Ratio')
            self.ax4.set_ylabel('Count')
            
            self.ax1.set_title('Spike Activity')
            self.ax2.set_title('Firing Rate')
            self.ax3.set_title('E/I Balance')
            self.ax4.set_title('Active Neurons')
            
            self.fig.legend(loc='upper right')
            self.fig.tight_layout()
            
        except ImportError:
            print("Matplotlib not available - using simplified monitoring")
    
    def _matplotlib_loop(self):
        """Update matplotlib plots with new data"""
        try:
            import matplotlib.pyplot as plt
            
            max_points = 1000  # Keep only recent data
            while self.running:
                if len(self.data_points) > max_points:
                    self.data_points = self.data_points[-max_points:]
                
                steps = [dp["step"] for dp in self.data_points]
                
                self.line_spikes.set_data(steps, 
                    [dp["total_spikes"] for dp in self.data_points])
                self.line_rate.set_data(steps, 
                    [dp["firing_rate"] for dp in self.data_points])
                self.line_ei.set_data(steps, 
                    [dp["ei_ratio"] for dp in self.data_points])
                self.line_neurons.set_data(steps, 
                    [dp["active_neurons"] for dp in self.data_points])
                
                # Adjust axes
                if steps:
                    self.ax1.set_xlim(min(steps), max(steps) + 100)
                    self.ax1.set_ylim(0, max(dp["total_spikes"] for dp in self.data_points) * 1.1)
                    
                    self.ax2.set_ylim(0, max(dp["firing_rate"] for dp in self.data_points) * 1.1)
                    
                    self.ax3.set_ylim(0, max(dp["ei_ratio"] for dp in self.data_points) * 1.1)
                    
                    self.ax4.set_ylim(0, max(dp["active_neurons"] for dp in self.data_points) * 1.1)
                
                plt.pause(0.01)
                
        except Exception as e:
            print(f"Matplotlib plot error: {e}")
    
    def _setup_plotly_plot(self):
        """Setup plotly for interactive web-based visualization"""
        try:
            import plotly.graph_objects as go
            from plotly.subplots import make_subplots
            
            self.fig = make_subplots(
                rows=2, cols=2,
                subplot_titles=('Spike Activity', 'Firing Rate', 'E/I Balance', 'Active Neurons'),
                specs=[[{'secondary_y': False}, {'secondary_y': False}],
                       [{'secondary_y': False}, {'secondary_y': False}]]
            )
            
            # Initialize traces
            self.trace_spikes = go.Scatter(x=[], y=[], mode='lines', name='Total Spikes', line=dict(color='blue'))
            self.trace_rate = go.Scatter(x=[], y=[], mode='lines', name='Firing Rate', line=dict(color='red'))
            self.trace_ei = go.Scatter(x=[], y=[], mode='lines', name='E/I Ratio', line=dict(color='green'))
            self.trace_neurons = go.Scatter(x=[], y=[], mode='lines', name='Active Neurons', line=dict(color='magenta'))
            
            self.fig.add_trace(self.trace_spikes, row=1, col=1)
            self.fig.add_trace(self.trace_rate, row=1, col=2)
            self.fig.add_trace(self.trace_ei, row=2, col=1)
            self.fig.add_trace(self.trace_neurons, row=2, col=2)
            
            # Setup layout
            self.fig.update_layout(
                title='NLM Real-Time Brain Monitor',
                showlegend=True,
                height=800,
                xaxis=dict(title='Step'),
                xaxis2=dict(title='Step'),
                xaxis3=dict(title='Step'),
                xaxis4=dict(title='Step')
            )
            
        except ImportError:
            print("Plotly not available - falling back to matplotlib")
    
    def get_data_summary(self) -> dict:
        """Get summary of collected data"""
        if not self.data_points:
            return {}
        
        spikes = [dp["total_spikes"] for dp in self.data_points]
        rates = [dp["firing_rate"] for dp in self.data_points]
        neurons = [dp["active_neurons"] for dp in self.data_points]
        
        return {
            "steps": len(self.data_points),
            "total_spikes": spikes[-1] if spikes else 0,
            "max_spikes_per_step": max(spikes) - min(spikes) if len(spikes) > 1 else 0,
            "avg_firing_rate": sum(rates) / len(rates) if rates else 0,
            "max_firing_rate": max(rates) if rates else 0,
            "max_active_neurons": max(neurons) if neurons else 0,
            "simulation_time": self.data_points[-1]["time"] if self.data_points else 0
        }

# Example usage
if __name__ == "__main__":
    # Create brain and world
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Setup monitoring
    monitor = RealTimeBrainMonitor(brain, update_interval=0.05)
    
    # Add custom data processor
    def data_processor(data):
        """Process data for custom analysis"""
        if data["step"] % 100 == 0:  # Every 100 steps
            spike_rate = data["total_spikes"] / (data["step"] + 1)
            print(f"Step {data['step']}: {data['firing_rate']:.3f} Hz, "
                  f"Spike rate: {spike_rate:.6f}/step")
    
    monitor.add_subscriber(data_processor)
    
    print("Starting real-time monitoring...")
    print("Close the plot window to stop monitoring")
    
    # Start monitoring
    monitor.start_monitoring(plot_backend="matplotlib")
    
    # Run simulation with monitoring
    try:
        for step in range(1000):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            if step % 10 == 0:  # Print status every 10 steps
                print(f"\rStep {step}/1000 - Firing: {brain.getFiringNeuronCount()} "
                      f"| Rate: {brain.getAverageFiringRate():.3f} Hz", end="")
    except KeyboardInterrupt:
        print("\nSimulation interrupted by user")
    
    # Stop monitoring and show summary
    monitor.stop_monitoring()
    print("\n\n" + "="*50)
    print("Monitoring Summary:")
    summary = monitor.get_data_summary()
    for key, value in summary.items():
        print(f"  {key}: {value}")
    print("="*50)
```

### 1.2 Neural Activity Heatmaps

```python
import pynlm
import numpy as np
from typing import List, Optional

class NeuralActivityVisualizer:
    """Visualizes neural activity patterns using heatmaps and connectivity graphs"""
    
    def __init__(self, brain: pynlm.Brain):
        self.brain = brain
        self.neuron_activations = {}
        self.synapse_activations = {}
        self.region_activity = {}
    
    def capture_activity_snapshot(self, step: int, neurons: List[pynlm.Neuron*]) -> dict:
        """Capture current neural activity snapshot"""
        snapshot = {
            "step": step,
            "timestamp": step * self.brain.getConfig().getOr("simulation_timestep", 0.001),
            "region_activity": {},
            "neuron_activations": {},
            "synapse_weights": {},
            "spike_events": []
        }
        
        # Capture region activity
        for region in self.brain.getRegions():
            region_id = region.getId().index()
            region_neuron_count = region.getTotalNeuronCount()
            active_count = 0
            total_potential = 0.0
            
            for neuron : neurons
                if neuron.getRegionId() == region.getId():
                    if neuron.isFiring() or neuron.getState().membranePotential > -40:
                        active_count += 1
                    total_potential += neuron.getState().membranePotential
            
            region_activity = {
                "region_id": region_id,
                "region_name": region.getName(),
                "neuron_count": region_neuron_count,
                "active_neurons": active_count,
                "average_potential": total_potential / region_neuron_count if region_neuron_count > 0 else 0.0,
                "activity_percentage": (active_count / region_neuron_count * 100) if region_neuron_count > 0 else 0.0
            }
            
            snapshot["region_activity"][region_id] = region_activity
        
        # Capture individual neuron activations
        for neuron in neurons:
            neuron_id = neuron.getId().index()
            activation = {
                "neuron_id": neuron_id,
                "region_id": neuron.getRegionId().index(),
                "population": neuron.getPopulationId().index(),
                "type": neuron.getType(),
                "membrane_potential": neuron.getState().membranePotential,
                "is_firing": neuron.isFiring(),
                "is_refractory": neuron.isRefractory(),
                "firing_rate_contribution": neuron.getState().firingRate,
                "last_spike_time": neuron.getLastSpikeTime(),
                "excitatory_input": neuron.getState().membranePotential > 0,
                "inhibitory_input": neuron.getState().membranePotential < -55
            }
            
            snapshot["neuron_activations"][neuron_id] = activation
        
        # Capture synapse weights
        for region in self.brain.getRegions():
            for synapse in region.getSynapses():
                synapse_id = synapse.getId().index()
                weight = synapse.getWeight()
                is_active = (
                    (synapse.getSourceNeuron() < neurons[-1].getId()) and  # safety check
                    weight > 0.001 and weight < 1.0
                )
                
                synapse_data = {
                    "synapse_id": synapse_id,
                    "source_neuron": synapse.getSourceNeuron().index(),
                    "destination_neuron": synapse.getDestinationNeuron().index(),
                    "weight": weight,
                    "type": synapse.getType(),
                    "is_active": is_active,
                    "delay": synapse.getDelay(),
                    "plasticity_flags": {
                        "hebbian": synapse.getPlasticityFlags().hebbian,
                        "stdp": synapse.getPlasticityFlags().stdp,
                        "reward_modulated": synapse.getPlasticityFlags().reward_modulated
                    }
                }
                
                snapshot["synapse_weights"][synapse_id] = synapse_data
        
        # Capture recent spike events
        spike_events = []
        for region in self.brain.getRegions():
            for neuron : neurons
                if neuron.getState().lastSpikeTime > 0:
                    # Check if spike is recent (within last 10 steps)
                    spike_age = (step * self.brain.getConfig().getOr("simulation_timestep", 0.001) - 
                               neuron.getState().lastSpikeTime)
                    
                    if spike_age < 0.01:  # Spike in last 10ms
                        spike_events.append({
                            "neuron_id": neuron.getId().index(),
                            "spike_time": neuron.getState().lastSpikeTime,
                            "spike_age_ms": spike_age * 1000,
                            "region_id": neuron.getRegionId().index(),
                            "population": neuron.getPopulationId().index()
                        })
        
        snapshot["spike_events"] = spike_events
        self.neuron_activations[step] = snapshot
        
        return snapshot
    
    def generate_activity_heatmap(self, neurons: List[pynlm.Neuron*], step: int) -> np.ndarray:
        """Generate a heatmap of neural activity"""
        # Create activity matrix based on region and population
        matrix_size = 10  # 10x10 heatmap
        activity_matrix = np.zeros((matrix_size, matrix_size))
        
        for region in self.brain.getRegions():
            for pop in region.getPopulations():
                # Calculate region activity
                region_active = 0
                region_total = 0
                
                for neuron in neurons:
                    if neuron.getRegionId() == region.getId() and neuron.getPopulationId() == pop.getId():
                        region_total += 1
                        if neuron.isFiring() or neuron.getState().membranePotential > -40:
                            region_active += 1
                
                if region_total > 0:
                    activity = region_active / region_total
                    # Map region to heatmap coordinates
                    region_x = region.getId().index() % matrix_size
                    region_y = region.getId().index() // matrix_size
                    
                    if region_x < matrix_size and region_y < matrix_size:
                        activity_matrix[region_y, region_x] = activity
        
        return activity_matrix
    
    def create_connectivity_heatmap(self, step: int) -> np.ndarray:
        """Create a connectivity heatmap showing active pathways"""
        # Count active connections between regions
        connectivity_matrix = np.zeros((10, 10))
        
        for region_a in self.brain.getRegions():
            for region_b in self.brain.getRegions():
                if region_a.getId().index() != region_b.getId().index():
                    # Count active synapses between regions
                    active_connections = 0
                    total_connections = 0
                    
                    for synapse in region_a.getSynapses():
                        if synapse.getDestinationNeuron().index() == region_b.getId().index():
                            total_connections += 1
                            if synapse.getWeight() > 0.1:  # Strong connection
                                active_connections += 1
                    
                    if total_connections > 0:
                        connection_strength = active_connections / total_connections
                        connectivity_matrix[region_a.getId().index() % 10, 
                                         region_b.getId().index() % 10] = connection_strength
        
        return connectivity_matrix
    
    def visualize_activity_sequence(self, start_step: int, end_step: int, 
                                  neurons: List[pynlm.Neuron*]) -> dict:
        """Create a time series of activity visualizations"""
        animation_frames = []
        
        for step in range(start_step, end_step + 1, 10):  # Sample every 10 steps
            # Capture current activity
            snapshot = self.capture_activity_snapshot(step, neurons)
            
            # Generate visualizations
            activity_heatmap = self.generate_activity_heatmap(neurons, step)
            connectivity_heatmap = self.create_connectivity_heatmap(step)
            
            # Create frame data
            frame = {
                "step": step,
                "timestamp": snapshot["timestamp"],
                "activity_heatmap": activity_heatmap,
                "connectivity_heatmap": connectivity_heatmap,
                "total_active_neurons": sum(
                    dp["active_neurons"] for dp in snapshot["region_activity"].values()
                ),
                "average_potential": np.mean([
                    dp["average_potential"] for dp in snapshot["region_activity"].values()
                ]),
                "spike_count": len(snapshot["spike_events"])
            }
            
            animation_frames.append(frame)
        
        return {
            "frames": animation_frames,
            "start_step": start_step,
            "end_step": end_step,
            "total_frames": len(animation_frames)
        }

# Example usage
if __name__ == "__main__":
    # Create brain and get neurons
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Collect all neurons
    all_neurons = []
    for region in brain.getRegions():
        for pop in region.getPopulations():
            for neuron in pop.getNeurons():
                all_neurons.append(neuron)
    
    print(f"Total neurons collected: {len(all_neurons)}")
    
    # Initialize visualizer
    visualizer = NeuralActivityVisualizer(brain)
    
    # Run simulation and capture activity
    print("Running simulation and capturing activity...")
    for step in range(500):
        brain.step(step)
        
        # Capture snapshot every 50 steps
        if step % 50 == 0:
            snapshot = visualizer.capture_activity_snapshot(step, all_neurons)
            print(f"Step {step}: {snapshot['total_active_neurons']} active neurons, "
                  f"{snapshot['spike_count']} spikes recorded")
    
    # Generate visualizations
    print("\nGenerating visualizations...")
    activity_sequence = visualizer.visualize_activity_sequence(0, 500, all_neurons)
    
    # Create animated heatmap
    try:
        import matplotlib.pyplot as plt
        import matplotlib.animation as animation
        
        fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(15, 6))
        
        # Create heatmap displays
        im1 = ax1.imshow(np.zeros((10, 10)), cmap='hot', interpolation='nearest', vmin=0, vmax=1)
        im2 = ax2.imshow(np.zeros((10, 10)), cmap='viridis', interpolation='nearest', vmin=0, vmax=1)
        
        ax1.set_title('Neural Activity Heatmap')
        ax2.set_title('Connectivity Heatmap')
        
        step_text = ax1.text(0.02, 0.95, '', transform=ax1.transAxes, fontsize=12,
                           bbox=dict(boxstyle='round', facecolor='white', alpha=0.8))
        
        def animate(frame):
            im1.set_data(activity_sequence["frames"][frame]["activity_heatmap"])
            im2.set_data(activity_sequence["frames"][frame]["connectivity_heatmap"])
            step_text.set_text(f"Step: {activity_sequence['frames'][frame]['step']}")
            return im1, im2, step_text
        
        anim = animation.FuncAnimation(fig, animate, frames=len(activity_sequence["frames"]),
                                    interval=100, blit=True)
        
        plt.tight_layout()
        plt.show()
        
    except ImportError:
        print("Matplotlib not available - skipping animation")
        
        # Save data for external visualization
        import json
        with open("activity_sequence.json", 'w') as f:
            # Convert numpy arrays to lists for JSON serialization
            frames_json = []
            for frame in activity_sequence["frames"]:
                frame_copy = frame.copy()
                frame_copy["activity_heatmap"] = frame["activity_heatmap"].tolist()
                frame_copy["connectivity_heatmap"] = frame["connectivity_heatmap"].tolist()
                frames_json.append(frame_copy)
            
            json.dump(frames_json, f, indent=2)
        
        print("Activity data saved to activity_sequence.json")
```

### 1.3 Performance Profiler

```python
import pynlm
import time
import threading
import psutil
import os
from typing import Dict, List, Callable
from collections import deque

class NLMPerformanceProfiler:
    """Comprehensive performance profiler for NLM simulations"""
    
    def __init__(self, brain: pynlm.Brain, sampling_interval: float = 0.1):
        self.brain = brain
        self.sampling_interval = sampling_interval
        self.is_profiling = False
        self.profile_thread: Optional[threading.Thread] = None
        
        # Performance metrics
        self.timing_metrics = {
            "brain_step_time": deque(maxlen=1000),
            "total_simulation_time": 0.0,
            "step_count": 0,
            "start_time": None,
            "end_time": None
        }
        
        # Resource metrics
        self.resource_metrics = {
            "cpu_usage": deque(maxlen=1000),
            "memory_usage": deque(maxlen=1000),
            "thread_count": deque(maxlen=1000),
            "file_handles": deque(maxlen=1000)
        }
        
        # Detailed operation metrics
        self.operation_metrics = {
            "neuron_updates": 0,
            "synapse_updates": 0,
            "spike_events": 0,
            "memory_operations": 0,
            "plasticity_operations": 0,
            "sensory_injections": 0,
            "action_decisions": 0
        }
        
        # Callbacks for events
        self.event_callbacks: List[Callable] = []
    
    def start_profiling(self):
        """Start comprehensive performance profiling"""
        self.is_profiling = True
        self.timing_metrics["start_time"] = time.time()
        
        # Start profiling threads
        self.profile_thread = threading.Thread(target=self._profiling_loop)
        self.profile_thread.daemon = True
        self.profile_thread.start()
        
        # Start system monitoring thread
        self.system_monitor_thread = threading.Thread(target=self._system_monitor_loop)
        self.system_monitor_thread.daemon = True
        self.system_monitor_thread.start()
        
        print("Performance profiling started...")
    
    def stop_profiling(self):
        """Stop profiling and generate reports"""
        self.is_profiling = False
        
        if self.profile_thread:
            self.profile_thread.join(timeout=2.0)
        
        if self.system_monitor_thread:
            self.system_monitor_thread.join(timeout=2.0)
        
        self.timing_metrics["end_time"] = time.time()
        self.timing_metrics["total_simulation_time"] = (
            self.timing_metrics["end_time"] - self.timing_metrics["start_time"]
        )
        
        print(f"Performance profiling stopped. Total time: {self.timing_metrics['total_simulation_time']:.2f}s")
    
    def add_event_callback(self, callback: Callable, event_type: str):
        """Add callback for specific events"""
        def wrapped_callback(*args, **kwargs):
            callback(*args, **kwargs, event_type=event_type)
        self.event_callbacks.append(wrapped_callback)
    
    def _profiling_loop(self):
        """Main profiling loop tracking NLM operations"""
        step = 0
        last_time = time.time()
        
        while self.is_profiling:
            iteration_start = time.time()
            
            # Record brain step timing
            brain_step_start = time.time()
            self.brain.step(step)
            brain_step_time = time.time() - brain_step_start
            
            self.timing_metrics["brain_step_time"].append(brain_step_time)
            step += 1
            self.timing_metrics["step_count"] = step
            
            # Record operation metrics (these would be populated during simulation)
            self.operation_metrics["neuron_updates"] += self.brain.getTotalNeuronCount()
            self.operation_metrics["synapse_updates"] += self.brain.getTotalSynapseCount()
            
            iteration_time = time.time() - iteration_start
            sleep_time = self.sampling_interval - iteration_time
            if sleep_time > 0:
                time.sleep(sleep_time)
            
            # Notify callbacks
            for callback in self.event_callbacks:
                try:
                    callback(step=step, iteration_time=iteration_time)
                except Exception as e:
                    print(f"Event callback error: {e}")
    
    def _system_monitor_loop(self):
        """Monitor system resources"""
        process = psutil.Process(os.getpid())
        
        while self.is_profiling:
            # CPU usage
            cpu_percent = process.cpu_percent()
            self.resource_metrics["cpu_usage"].append(cpu_percent)
            
            # Memory usage
            memory_info = process.memory_info()
            memory_mb = memory_info.rss / 1024 / 1024
            self.resource_metrics["memory_usage"].append(memory_mb)
            
            # Thread count
            thread_count = process.num_threads()
            self.resource_metrics["thread_count"].append(thread_count)
            
            # File handles (Unix only)
            if hasattr(process, 'num_fds'):
                file_handles = process.num_fds()
                self.resource_metrics["file_handles"].append(file_handles)
            
            time.sleep(self.sampling_interval)
    
    def run_profiled_experiment(self, simulation_func, *args, **kwargs) -> dict:
        """Run an experiment with comprehensive profiling"""
        print("Starting profiled experiment...")
        self.start_profiling()
        
        try:
            # Run the simulation
            start_time = time.time()
            result = simulation_func(*args, **kwargs)
            end_time = time.time()
            
            # Wait a moment for final metrics
            time.sleep(1.0)
            
        except Exception as e:
            print(f"Experiment error: {e}")
            result = None
        finally:
            self.stop_profiling()
        
        # Generate performance report
        report = self.generate_performance_report()
        
        print(f"\nExperiment completed in {end_time - start_time:.2f}s")
        print(f"Performance profiling results saved to report")
        
        return result, report
    
    def generate_performance_report(self) -> dict:
        """Generate comprehensive performance report"""
        if self.timing_metrics["step_count"] == 0:
            return {"error": "No data collected"}
        
        # Calculate timing statistics
        brain_step_times = list(self.timing_metrics["brain_step_time"])
        if brain_step_times:
            avg_brain_step = sum(brain_step_times) / len(brain_step_times)
            min_brain_step = min(brain_step_times)
            max_brain_step = max(brain_step_times)
            std_brain_step = (sum((x - avg_brain_step) ** 2 for x in brain_step_times) / len(brain_step_times)) ** 0.5
        else:
            avg_brain_step = min_brain_step = max_brain_step = std_brain_step = 0.0
        
        # Calculate resource statistics
        def calculate_stats(data):
            if not data:
                return {"mean": 0, "std": 0, "min": 0, "max": 0, "p95": 0}
            
            return {
                "mean": sum(data) / len(data),
                "std": (sum((x - sum(data) / len(data)) ** 2 for x in data) / len(data)) ** 0.5,
                "min": min(data),
                "max": max(data),
                "p95": sorted(data)[int(len(data) * 0.95)] if data else 0
            }
        
        cpu_stats = calculate_stats(list(self.resource_metrics["cpu_usage"]))
        memory_stats = calculate_stats(list(self.resource_metrics["memory_usage"]))
        thread_stats = calculate_stats(list(self.resource_metrics["thread_count"]))
        
        # Calculate performance metrics
        total_steps = self.timing_metrics["step_count"]
        total_time = self.timing_metrics["total_simulation_time"]
        steps_per_second = total_steps / total_time if total_time > 0 else 0
        
        # Operation throughput
        neuron_updates_per_step = (
            self.operation_metrics["neuron_updates"] / total_steps 
            if total_steps > 0 else 0
        )
        synapse_updates_per_step = (
            self.operation_metrics["synapse_updates"] / total_steps 
            if total_steps > 0 else 0
        )
        
        # Memory efficiency
        memory_efficiency = (
            total_steps / memory_stats["mean"] if memory_stats["mean"] > 0 else 0
        )
        
        # CPU efficiency
        cpu_efficiency = (
            steps_per_second / cpu_stats["mean"] if cpu_stats["mean"] > 0 else 0
        )
        
        report = {
            "experiment_summary": {
                "total_steps": total_steps,
                "total_time_seconds": total_time,
                "steps_per_second": steps_per_second,
                "simulation_efficiency": steps_per_second / (1.0 / self.sampling_interval)  # Ideal performance
            },
            "timing_analysis": {
                "brain_step": {
                    "average_ms": avg_brain_step * 1000,
                    "min_ms": min_brain_step * 1000,
                    "max_ms": max_brain_step * 1000,
                    "std_ms": std_brain_step * 1000,
                    "p95_ms": sorted(brain_step_times)[int(len(brain_step_times) * 0.95)] * 1000 if brain_step_times else 0
                },
                "simulation_speed": steps_per_second
            },
            "resource_usage": {
                "cpu": cpu_stats,
                "memory_mb": memory_stats,
                "threads": thread_stats
            },
            "operation_throughput": {
                "neuron_updates_per_step": neuron_updates_per_step,
                "synapse_updates_per_step": synapse_updates_per_step,
                "total_operations": (
                    self.operation_metrics["neuron_updates"] + 
                    self.operation_metrics["synapse_updates"]
                )
            },
            "efficiency_metrics": {
                "memory_efficiency": memory_efficiency,
                "cpu_efficiency": cpu_efficiency,
                "operation_density": (
                    (self.operation_metrics["neuron_updates"] + self.operation_metrics["synapse_updates"]) /
                    (memory_stats["mean"] * 1024 * 1024) if memory_stats["mean"] > 0 else 0
                )
            },
            "bottleneck_analysis": self._identify_bottlenecks(),
            "optimization_recommendations": self._generate_recommendations()
        }
        
        return report
    
    def _identify_bottlenecks(self) -> dict:
        """Identify performance bottlenecks"""
        bottlenecks = {}
        
        brain_step_times = list(self.timing_metrics["brain_step_time"])
        if brain_step_times and len(brain_step_times) > 10:
            # Check for step time spikes
            recent_times = brain_step_times[-100:] if len(brain_step_times) > 100 else brain_step_times
            avg_time = sum(recent_times) / len(recent_times)
            
            if avg_time > 0.01:  # Step takes more than 10ms
                bottlenecks["brain_computation"] = {
                    "severity": "high" if avg_time > 0.05 else "medium",
                    "average_time_ms": avg_time * 1000,
                    "recommendation": "Optimize brain step computation, check for inefficient operations"
                }
            
            # Check for increasing step times (potential memory leaks)
            if len(recent_times) >= 20:
                first_half = recent_times[:10]
                second_half = recent_times[10:]
                first_avg = sum(first_half) / len(first_half)
                second_avg = sum(second_half) / len(second_half)
                
                if second_avg > first_avg * 1.5:  # 50% increase
                    bottlenecks["memory_growth"] = {
                        "severity": "high",
                        "growth_rate": (second_avg - first_avg) / first_avg,
                        "recommendation": "Potential memory leak detected. Check for accumulating data structures."
                    }
        
        # Check resource usage patterns
        cpu_data = list(self.resource_metrics["cpu_usage"])
        if cpu_data and max(cpu_data) > 80:
            bottlenecks["high_cpu_usage"] = {
                "severity": "medium",
                "max_cpu_percent": max(cpu_data),
                "recommendation": "Reduce CPU overhead, check for busy loops or inefficient algorithms."
            }
        
        memory_data = list(self.resource_metrics["memory_usage"])
        if memory_data and memory_data[-1] > memory_data[0] * 2:
            bottlenecks["memory_growth"] = {
                "severity": "high",
                "memory_growth_factor": memory_data[-1] / memory_data[0] if memory_data[0] > 0 else float('inf'),
                "recommendation": "Memory usage is growing rapidly. Check for accumulating data."
            }
        
        return bottlenecks
    
    def _generate_recommendations(self) -> list:
        """Generate optimization recommendations"""
        recommendations = []
        
        brain_step_times = list(self.timing_metrics["brain_step_time"])
        if brain_step_times and sum(brain_step_times) / len(brain_step_times) > 0.02:
            recommendations.append(
                "Brain step computation is slow. Consider: "
                "(1) Reducing neuron count for testing, "
                "(2) Optimizing plasticity calculations, "
                "(3) Using event-driven computation more effectively"
            )
        
        memory_data = list(self.resource_metrics["memory_usage"])
        if memory_data and memory_data[-1] > 500:  # More than 500MB
            recommendations.append(
                "High memory usage detected. Recommendations: "
                "(1) Use memory pools, (2) Clear unused data structures, "
                "(3) Implement sparse representations"
            )
        
        cpu_data = list(self.resource_metrics["cpu_usage"])
        if cpu_data and max(cpu_data) > 90:
            recommendations.append(
                "Very high CPU usage. Consider: "
                "(1) Parallelizing independent operations, "
                "(2) Reducing sampling frequency, "
                "(3) Optimizing hot code paths"
            )
        
        if not recommendations:
            recommendations.append(
                "Performance looks good! Consider: "
                "(1) Scaling up for larger simulations, "
                "(2) Adding more detailed profiling, "
                "(3) Running stress tests"
            )
        
        return recommendations

# Example usage
if __name__ == "__main__":
    # Create brain and world
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Setup performance profiler
    profiler = NLMPerformanceProfiler(brain, sampling_interval=0.05)
    
    # Add event callbacks for specific operations
    def step_callback(step, iteration_time, event_type="step"):
        if step % 100 == 0:
            print(f"\rStep {step}: {iteration_time*1000:.2f}ms per step", end="")
    
    profiler.add_event_callback(step_callback, "step_timing")
    
    # Define simulation function
    def run_simulation():
        """Run simulation with profiling"""
        for step in range(500):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Simulate some work
            if step % 50 == 0:
                print(f"  Progress: {step}/500 steps")
        
        return {
            "final_spikes": brain.getTotalSpikeCount(),
            "final_firing_rate": brain.getAverageFiringRate(),
            "total_neurons": brain.getTotalNeuronCount(),
            "total_synapses": brain.getTotalSynapseCount()
        }
    
    # Run profiled experiment
    print("Running NLM simulation with comprehensive profiling...")
    print("This will take approximately 30-60 seconds depending on system resources.")
    print("Close any plots that may appear during the simulation.")
    
    results, performance_report = profiler.run_profiled_experiment(run_simulation)
    
    # Print performance report
    print("\n" + "="*60)
    print("PERFORMANCE PROFILING REPORT")
    print("="*60)
    
    exp_summary = performance_report["experiment_summary"]
    print(f"\nExperiment Summary:")
    print(f"  Total Steps: {exp_summary['total_steps']}")
    print(f"  Total Time: {exp_summary['total_time_seconds']:.2f}s")
    print(f"  Steps per Second: {exp_summary['steps_per_second']:.1f}")
    print(f"  Simulation Efficiency: {exp_summary['simulation_efficiency']:.1f}")
    
    timing = performance_report["timing_analysis"]
    print(f"\nTiming Analysis:")
    print(f"  Average Brain Step: {timing['brain_step']['average_ms']:.2f}ms")
    print(f"  Min Brain Step: {timing['brain_step']['min_ms']:.2f}ms")
    print(f"  Max Brain Step: {timing['brain_step']['max_ms']:.2f}ms")
    print(f"  95th Percentile: {timing['brain_step']['p95_ms']:.2f}ms")
    
    resource = performance_report["resource_usage"]
    print(f"\nResource Usage:")
    print(f"  CPU: Mean {resource['cpu']['mean']:.1f}%, Max {resource['cpu']['max']:.1f}%")
    print(f"  Memory: Mean {resource['memory_mb']['mean']:.1f}MB, Max {resource['memory_mb']['max']:.1f}MB")
    print(f"  Threads: Mean {resource['threads']['mean']:.1f}, Max {resource['threads']['max']:.1f}")
    
    throughput = performance_report["operation_throughput"]
    print(f"\nOperation Throughput:")
    print(f"  Neuron Updates per Step: {throughput['neuron_updates_per_step']:.0f}")
    print(f"  Synapse Updates per Step: {throughput['synapse_updates_per_step']:.0f}")
    print(f"  Total Operations: {throughput['total_operations']}")
    
    # Save detailed report
    import json
    with open("performance_report.json", 'w') as f:
        json.dump(performance_report, f, indent=2, default=str)
    
    print(f"\nDetailed performance report saved to 'performance_report.json'")
    
    # Print bottlenecks and recommendations
    bottlenecks = performance_report["bottleneck_analysis"]
    if bottlenecks:
        print(f"\nPerformance Bottlenecks Identified:")
        for bottleneck, details in bottlenecks.items():
            print(f"  {bottleneck}: {details['severity'].upper()} - {details['recommendation']}")
    
    recommendations = performance_report["optimization_recommendations"]
    if recommendations:
        print(f"\nOptimization Recommendations:")
        for i, rec in enumerate(recommendations, 1):
            print(f"  {i}. {rec}")
    
    print("\n" + "="*60)
```