# Advanced Example 2: Real-Time Visualization and Monitoring

## Overview

This advanced example demonstrates real-time visualization and monitoring capabilities for NLM simulations. It showcases how to integrate visualization systems to monitor neural activity, agent behavior, and simulation metrics in real-time.

## Learning Objectives

After completing this example, you will be able to:

1. **Implement real-time visualization systems** for neural activity
2. **Monitor simulation metrics** in real-time
3. **Create interactive visualizations** for agent behavior
4. **Set up visualization backends** (OpenGL, WebGL, etc.)
5. **Integrate monitoring systems** for performance tracking

## What This Example Demonstrates

- **Real-time neural visualization**: Live display of neuron activity and firing patterns
- **Agent monitoring**: Track agent position, energy, curiosity, and behavior
- **Performance monitoring**: CPU, memory, and simulation speed monitoring
- **Interactive visualization**: User-interactive visualizations for exploration
- **Multi-visualization setup**: Concurrent displays of different data types

## System Requirements

For real-time visualization, you'll need:

- **Visualization libraries**: OpenGL/Vulkan, GLFW, Dear ImGui, etc.
- **Rendering backend**: GPU acceleration recommended for smooth visualization
- **Display system**: Monitor or windowing system
- **Performance**: Real-time processing requires dedicated CPU/GPU resources
- **Dependencies**: Visualization libraries (OpenGL, matplotlib, plotly, etc.)

## Technical Architecture

### 1. Visualization System Architecture

```cpp
#include "NLM/Visualization.hpp"
#include "NLM/Brain.hpp"
#include "NLM/World.hpp"

class RealTimeVisualizer {
private:
    // Different visualization components
    NeuralActivityRenderer neuralRenderer;
    AgentBehaviorRenderer agentRenderer;
    PerformanceMonitor performanceMonitor;
    InteractionHandler interactionHandler;
    
    // Data buffers for real-time updates
    std::vector<NeuronState> neuralDataBuffer;
    std::vector<AgentState> agentDataBuffer;
    std::vector<PerformanceMetrics> perfDataBuffer;
    
    // Rendering configuration
    VisualizationConfig config;
    
public:
    RealTimeVisualizer() {
        initializeRenderers();
        setupDataBuffers();
        configureRenderingPipeline();
    }
    
    void initializeRenderers() {
        // Initialize different renderers
        neuralRenderer = NeuralActivityRenderer(config.neuralRendering);
        agentRenderer = AgentBehaviorRenderer(config.agentRendering);
        performanceMonitor = PerformanceMonitor(config.monitoring);
        interactionHandler = InteractionHandler(config.interaction);
    }
    
    void setupDataBuffers() {
        // Pre-allocate buffers for performance
        neuralDataBuffer.reserve(MAX_NEURONS * 2);
        agentDataBuffer.reserve(MAX_AGENTS * 2);
        perfDataBuffer.reserve(MAX_PERF_SAMPLES * 2);
    }
    
    void renderFrame() {
        // Capture current simulation state
        captureNeuralState();
        captureAgentState();
        capturePerformanceMetrics();
        
        // Render all visualization components
        neuralRenderer.render(neuralDataBuffer);
        agentRenderer.render(agentDataBuffer);
        performanceMonitor.render(perfDataBuffer);
        
        // Handle user interactions
        interactionHandler.processInput();
        
        // Swap buffers for display
        swapBuffers();
    }
    
    void captureNeuralState() {
        // Capture neural activity data
        neuralDataBuffer.clear();
        
        // Sample neural activity at high frequency
        int sampleCount = std::min((int)brain->getTotalNeuronCount(), MAX_SAMPLES_PER_FRAME);
        
        for (int i = 0; i < sampleCount; ++i) {
            NeuronState state;
            state.neuronId = i;
            state.position = brain->getNeuronPosition(i);
            state.firingRate = brain->getNeuronFiringRate(i);
            state.membranePotential = brain->getNeuronMembranePotential(i);
            state.spikeTime = brain->getLastSpikeTime(i);
            
            // Quantize for performance
            state.firingRate = quantizeFloat(state.firingRate, NEURAL_QUANTIZATION);
            state.membranePotential = quantizeFloat(state.membranePotential, POTENTIAL_QUANTIZATION);
            
            neuralDataBuffer.push_back(state);
        }
    }
    
    void captureAgentState() {
        // Capture agent behavior and state
        agentDataBuffer.clear();
        
        for (int agentId = 0; agentId < world->getAgentCount(); ++agentId) {
            AgentState state;
            state.agentId = agentId;
            state.position = world->getAgentPosition(agentId);
            state.orientation = world->getAgentOrientation(agentId);
            state.energy = world->getAgentEnergy(agentId);
            state.maxEnergy = world->getAgentMaxEnergy(agentId);
            state.curiosity = agent->getCuriosityLevel(agentId);
            state.novelty = agent->getNoveltyLevel(agentId);
            state.predictionError = agent->getPredictionError(agentId);
            
            // Track behavioral metrics
            state.actionHistory = agent->getActionHistory(agentId, HISTORY_LENGTH);
            state.perceptionQuality = agent->getPerceptionQuality(agentId);
            
            agentDataBuffer.push_back(state);
        }
    }
    
    void capturePerformanceMetrics() {
        // Capture performance monitoring data
        perfDataBuffer.clear();
        
        PerformanceMetrics metrics;
        metrics.timestamp = getCurrentTime();
        metrics.cpuUsage = getCPUUsage();
        metrics.memoryUsage = getMemoryUsage();
        metrics.simTimePerStep = getSimulationTimePerStep();
        metrics.realTimeFactor = getRealTimeFactor();
        metrics.frameTime = getFrameTime();
        metrics.droppedFrames = getDroppedFrames();
        
        perfDataBuffer.push_back(metrics);
    }
};
```

### 2. Neural Activity Visualization

```python
import pynlm
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import time

class NeuralActivityVisualizer:
    """Real-time visualization of neural activity"""
    
    def __init__(self, brain, max_neurons=1000):
        self.brain = brain
        self.max_neurons = max_neurons
        self.neural_data = []
        self.spike_history = []
        self.firing_rates = []
        
        # Visualization configuration
        self.fig, self.axes = plt.subplots(2, 2, figsize=(14, 10))
        self.fig.suptitle('Real-Time Neural Activity Visualization', fontsize=16)
        
        # Setup subplots
        self.setup_plots()
        
        # Animation parameters
        self.animation_data = []
        self.update_interval = 100  # ms
        self.last_update_time = time.time()
    
    def setup_plots(self):
        """Setup matplotlib subplots for neural visualization"""
        
        # Plot 1: Neural activity heatmap
        self.ax_heatmap = self.axes[0, 0]
        self.heatmap_data = np.zeros((50, 50))
        self.heatmap_im = self.ax_heatmap.imshow(self.heatmap_data, 
                                                cmap='hot', interpolation='nearest',
                                                vmin=0, vmax=1, aspect='auto')
        self.ax_heatmap.set_title('Neural Activity Heatmap')
        self.ax_heatmap.set_xlabel('Neural Population')
        self.ax_heatmap.set_ylabel('Time Steps')
        
        # Plot 2: Firing rate distribution
        self.ax_firing_dist = self.axes[0, 1]\n        self.firing_dist_data, = self.ax_firing_dist.plot([], [], 'b-', alpha=0.7, linewidth=2)
        self.ax_firing_dist.set_title('Firing Rate Distribution')
        self.ax_firing_dist.set_xlabel('Neuron Index')
        self.ax_firing_dist.set_ylabel('Firing Rate (Hz)')
        self.ax_firing_dist.set_ylim(0, 100)
        
        # Plot 3: Spike raster plot
        self.ax_raster = self.axes[1, 0]\n        self.raster_data = []
        self.raster_im = self.ax_raster.imshow(self.raster_data, 
                                              cmap='binary', aspect='auto',
                                              vmin=0, vmax=1)
        self.ax_raster.set_title('Spike Raster Plot')
        self.ax_raster.set_xlabel('Time Steps')
        self.ax_raster.set_ylabel('Neuron Index')
        
        # Plot 4: Population activity
        self.ax_population = self.axes[1, 1]\n        self.population_line, = self.ax_population.plot([], [], 'r-', linewidth=2)
        self.ax_population.set_title('Population Activity')
        self.ax_population.set_xlabel('Time Steps')
        self.ax_population.set_ylabel('Active Neurons')
        self.ax_population.set_ylim(0, self.brain.getTotalNeuronCount() if hasattr(self.brain, 'getTotalNeuronCount') else 100)
    
    def update_neural_visualization(self, frame):
        """Update neural activity visualization"""
        
        # Get current neural state
        current_time = time.time()
        if current_time - self.last_update_time < self.update_interval / 1000:
            return
        
        self.last_update_time = current_time
        
        # Sample neural activity (for performance, sample from large networks)
        total_neurons = self.brain.getTotalNeuronCount() if hasattr(self.brain, 'getTotalNeuronCount') else 100
        sample_step = max(1, total_neurons // self.max_neurons)
        
        active_neurons = []
        firing_rates = []
        spike_times = []
        
        # Collect neural data
        for i in range(0, total_neurons, sample_step):
            firing_rate = self.brain.getNeuronFiringRate(i) if hasattr(self.brain, 'getNeuronFiringRate') else 0
            membrane_potential = self.brain.getNeuronMembranePotential(i) if hasattr(self.brain, 'getNeuronMembranePotential') else -70
            last_spike = self.brain.getLastSpikeTime(i) if hasattr(self.brain, 'getLastSpikeTime') else -1
            
            firing_rates.append(firing_rate)
            active_neurons.append(1 if firing_rate > 0 else 0)
            
            if last_spike >= 0:
                spike_times.append((frame, i))
        
        # Update heatmap (simplified 2D representation)
        self.heatmap_data = self.heatmap_data[1:, :]  # Shift up
        self.heatmap_data[-1, :len(firing_rates)] = firing_rates
        self.heatmap_im.set_data(self.heatmap_data)
        
        # Update firing rate distribution
        self.firing_dist_data.set_data(list(range(len(firing_rates))), firing_rates)
        
        # Update spike raster
        self.raster_data.append([0] * len(active_neurons))
        if spike_times:
            for time_step, neuron_idx in spike_times:
                if neuron_idx < len(self.raster_data[-1]):
                    self.raster_data[-1][neuron_idx] = 1
        
        # Limit raster data size
        if len(self.raster_data) > 200:
            self.raster_data.pop(0)
        
        self.raster_im.set_data(self.raster_data)
        
        # Update population activity
        current_active = sum(active_neurons)
        self.population_line.set_data(range(len(self.raster_data)), 
                                      [sum(row) for row in self.raster_data])
        
        # Update titles with metrics
        avg_firing_rate = np.mean(firing_rates) if firing_rates else 0
        active_percentage = (current_active / total_neurons * 100) if total_neurons > 0 else 0
        
        self.ax_heatmap.set_title(f'Neural Activity Heatmap (Avg: {avg_firing_rate:.1f} Hz)')
        self.ax_raster.set_title(f'Spike Raster Plot ({active_percentage:.1f}% Active)')
        
        # Adjust plot limits
        self.ax_raster.set_xlim(0, len(self.raster_data))
        self.ax_raster.set_ylim(0, self.max_neurons)
        
        return [self.heatmap_im, self.firing_dist_data, self.raster_im, self.population_line]
    
    def animate(self, num_frames=500):
        """Start real-time animation"""
        
        print("Starting real-time neural activity visualization...")
        print("Visualization shows:")
        print("  - Heatmap: Neural activity distribution over time")
        print("  - Firing Rate: Distribution of firing rates across neurons")
        print("  - Spike Raster: Individual spike events")
        print("  - Population: Overall network activity")
        
        # Create animation
        ani = FuncAnimation(self.fig, self.update_neural_visualization, 
                           frames=num_frames, interval=self.update_interval,
                           blit=True, repeat=True)
        
        plt.tight_layout()
        plt.show()
        
        return ani
```

### 3. Agent Behavior Visualization

```python
class AgentBehaviorVisualizer:
    """Real-time visualization of agent behavior and interactions"""
    
    def __init__(self, world, agent, brain):
        self.world = world
        self.agent = agent
        self.brain = brain
        
        # Behavioral tracking
        self.position_history = []
        self.energy_history = []
        self.curiosity_history = []
        self.novelty_history = []
        self.action_history = []
        self.perception_history = []
        
        # Visualization setup
        self.fig, self.axes = plt.subplots(3, 2, figsize=(16, 12))
        self.fig.suptitle('Agent Behavior Visualization', fontsize=16)
        
        self.setup_agent_plots()
        
        # Environmental setup
        self.setup_environment()
    
    def setup_agent_plots(self):
        """Setup plots for agent behavior monitoring"""
        
        # Plot 1: Agent trajectory
        self.ax_trajectory = self.axes[0, 0]
        self.path_line, = self.ax_trajectory.plot([], [], 'b-', alpha=0.7, linewidth=2, label='Path')
        self.current_pos_marker = self.ax_trajectory.plot([], [], 'ro', markersize=10, label='Current Position')[0]
        self.target_marker = self.ax_trajectory.plot([], [], 'go', markersize=8, label='Target')[0]
        
        self.ax_trajectory.set_title('Agent Trajectory')
        self.ax_trajectory.set_xlabel('X Position')
        self.ax_trajectory.set_ylabel('Y Position')
        self.ax_trajectory.legend()
        self.ax_trajectory.set_aspect('equal')
        self.ax_trajectory.grid(True, alpha=0.3)
        
        # Plot 2: Energy and motivation
        self.ax_energy = self.axes[0, 1]
        self.energy_line, = self.ax_energy.plot([], [], 'g-', linewidth=2, label='Energy')
        self.curiosity_line, = self.ax_energy.plot([], [], 'r-', linewidth=2, label='Curiosity')
        self.novelty_line, = self.ax_energy.plot([], [], 'b-', linewidth=2, label='Novelty')
        
        self.ax_energy.set_title('Energy and Motivation')
        self.ax_energy.set_xlabel('Time Steps')
        self.ax_energy.set_ylabel('Level')
        self.ax_energy.set_ylim(0, 100)
        self.ax_energy.legend()
        self.ax_energy.grid(True, alpha=0.3)
        
        # Plot 3: Action selection
        self.ax_actions = self.axes[1, 0]\n        self.action_bars = []
        action_types = ['Move', 'Turn', 'Interact', 'Eat', 'Sleep', 'Explore']
        colors = ['skyblue', 'lightgreen', 'lightcoral', 'gold', 'plum', 'orange']
        
        for i, (action, color) in enumerate(zip(action_types, colors)):
            bar = self.ax_actions.bar(i, 0, color=color, alpha=0.7, label=action)
            self.action_bars.append(bar)
        
        self.ax_actions.set_title('Action Selection Distribution')
        self.ax_actions.set_xlabel('Action Type')
        self.ax_actions.set_ylabel('Frequency')
        self.ax_actions.set_xticks(range(len(action_types)))
        self.ax_actions.set_xticklabels(action_types, rotation=45)
        self.ax_actions.legend()
        self.ax_actions.grid(True, alpha=0.3)
        
        # Plot 4: Sensory input
        self.ax_sensory = self.axes[1, 1]\n        self.sensory_bars = []
        sensory_features = ['Vision', ' smell', 'Touch', 'Hearing', 'Balance']
        
        for i, feature in enumerate(sensory_features):
            bar = self.ax_sensory.bar(i, 0, color='cyan', alpha=0.7, label=feature)
            self.sensory_bars.append(bar)
        
        self.ax_sensory.set_title('Sensory Input Levels')
        self.ax_sensory.set_xlabel('Sensory Modality')
        self.ax_sensory.set_ylabel('Strength')
        self.ax_sensory.set_xticks(range(len(sensory_features)))
        self.ax_sensory.set_xticklabels(sensory_features, rotation=45)
        self.ax_sensory.legend()
        self.ax_sensory.set_ylim(0, 1)
        self.ax_sensory.grid(True, alpha=0.3)
        
        # Plot 5: Learning curves
        self.ax_learning = self.axes[2, 0]\n        self.prediction_error_line, = self.ax_learning.plot([], [], 'r-', linewidth=2, label='Prediction Error')
        self.curiosity_curve, = self.ax_learning.plot([], [], 'g-', linewidth=2, label='Curiosity Level')
        self.development_line, = self.ax_learning.plot([], [], 'b-', linewidth=2, label='Development Stage')
        
        self.ax_learning.set_title('Learning Progress')
        self.ax_learning.set_xlabel('Time Steps')
        self.ax_learning.set_ylabel('Level')
        self.ax_learning.legend()
        self.ax_learning.grid(True, alpha=0.3)
        
        # Plot 6: Environmental interaction
        self.ax_environment = self.axes[2, 1]
        self.env_objects = []
        self.agent_in_env = None
    
    def setup_environment(self):
        """Setup environment visualization elements"""
        
        # Create environment map background
        self.env_map = np.zeros((50, 50, 3), dtype=np.uint8)
        
        # Add environmental features
        self.add_environment_features()
    
    def add_environment_features(self):
        """Add environmental features to the map"""
        
        # Add resources (green)
        for obj in self.world.getObjects(pynlm.WorldObjectType.Resource):
            x, y = int(obj.getX()), int(obj.getY())
            if 0 <= x < 50 and 0 <= y < 50:
                self.env_map[y, x] = [0, 255, 0]  # Green
        
        # Add hazards (red)
        for obj in self.world.getObjects(pynlm.WorldObjectType.Hazard):
            x, y = int(obj.getX()), int(obj.getY())
            if 0 <= x < 50 and 0 <= y < 50:
                self.env_map[y, x] = [255, 0, 0]  # Red
        
        # Add walls (gray)
        for obj in self.world.getObjects(pynlm.WorldObjectType.Wall):
            x, y = int(obj.getX()), int(obj.getY())
            if 0 <= x < 50 and 0 <= y < 50:
                self.env_map[y, x] = [128, 128, 128]  # Gray
        
        # Add markers (blue)
        for obj in self.world.getObjects(pynlm.WorldObjectType.Marker):
            x, y = int(obj.getX()), int(obj.getY())
            if 0 <= x < 50 and 0 <= y < 50:
                self.env_map[y, x] = [0, 0, 255]  # Blue
    
    def update_visualization(self, frame):
        """Update all agent behavior visualizations"""
        
        # Update world state
        self.world.update(0.1)
        
        # Get agent state
        body = self.world.getAgentBody()
        percept = self.world.getSensoryPercept()
        
        # Update behavioral tracking
        self.position_history.append((body.getX(), body.getY()))
        self.energy_history.append(body.getEnergy())
        self.curiosity_history.append(self.agent.getCuriosityLevel())
        self.novelty_history.append(self.agent.getNoveltyLevel())
        
        # Get action and learning data
        action = self.agent.decodeMotorCommand()
        prediction_error = self.agent.getPredictionError()
        development_stage = self.brain.getDevelopmentalStage()
        
        # Update trajectory
        if len(self.position_history) > 1:
            x_vals, y_vals = zip(*self.position_history[-100:])  # Last 100 positions
            self.path_line.set_data(x_vals, y_vals)
            
            self.current_pos_marker.set_data([body.getX()], [body.getY()])
            
            # Update plot limits
            x_min, x_max = min(x_vals), max(x_vals)
            y_min, y_max = min(y_vals), max(y_vals)
            
            # Add padding
            x_pad = (x_max - x_min) * 0.1 if x_max > x_min else 10
            y_pad = (y_max - y_min) * 0.1 if y_max > y_min else 10
            
            self.ax_trajectory.set_xlim(x_min - x_pad, x_max + x_pad)
            self.ax_trajectory.set_ylim(y_min - y_pad, y_max + y_pad)
        
        # Update energy and motivation
        max_steps = 100
        energy_x = list(range(len(self.energy_history[-max_steps:])))
        
        self.energy_line.set_data(energy_x, self.energy_history[-max_steps:])
        self.curiosity_line.set_data(energy_x, self.curiosity_history[-max_steps:])
        self.novelty_line.set_data(energy_x, self.novelty_history[-max_steps:])
        
        self.ax_energy.set_xlim(0, len(self.energy_history))
        
        # Update action selection
        self.update_action_selection(action)
        
        # Update sensory input
        self.update_sensory_input(percept)
        
        # Update learning curves
        learning_x = list(range(len(self.prediction_error_history[-max_steps:])))
        
        self.prediction_error_line.set_data(learning_x, self.prediction_error_history[-max_steps:])
        self.curiosity_curve.set_data(learning_x, self.curiosity_history[-max_steps:])
        self.development_line.set_data(learning_x, self.development_stages[-max_steps:])
        
        self.ax_learning.set_xlim(0, len(self.prediction_error_history))
        
        # Update environment visualization
        self.update_environment()
        
        # Record data for analysis
        self.record_behavioral_data(action, prediction_error, development_stage)
        
        # Return artists for animation
        artists = [self.path_line, self.current_pos_marker, self.energy_line, 
                  self.curiosity_line, self.novelty_line]
        artists.extend(self.action_bars)
        artists.extend(self.sensory_bars)
        artists.extend([self.prediction_error_line, self.curiosity_curve, self.development_line])
        
        return artists
    
    def update_action_selection(self, action):
        """Update action selection visualization"""
        
        # Track action frequencies
        if not hasattr(self, 'action_counts'):
            self.action_counts = {'Move': 0, 'Turn': 0, 'Interact': 0, 
                                 'Eat': 0, 'Sleep': 0, 'Explore': 0}
        
        action_name = action.name if hasattr(action, 'name') else str(action)
        if action_name in self.action_counts:
            self.action_counts[action_name] += 1
        
        # Update bars
        for i, action_type in enumerate(self.action_counts.keys()):
            count = self.action_counts[action_type]
            max_count = max(self.action_counts.values()) if self.action_counts.values() else 1
            bar_height = (count / max_count) * 10  # Max height of 10
            
            self.action_bars[i].set_height(bar_height)
    
    def update_sensory_input(self, percept):
        """Update sensory input visualization"""
        
        # Get sensory features
        sensory_features = percept.getInternal() if percept.getInternal() else []
        
        # Map to visualization features (simplified)
        feature_mapping = {
            0: 'Vision',
            1: 'smell', 
            2: 'Touch',
            3: 'Hearing',
            4: 'Balance'
        }
        
        for i, feature in enumerate(feature_mapping.keys()):
            if i < len(sensory_features):
                strength = min(1.0, max(0.0, sensory_features[i]))
                self.sensory_bars[i].set_height(strength)
    
    def update_environment(self):
        """Update environment visualization"""
        
        # Create environment background
        env_bg = np.ones((50, 50, 3), dtype=np.uint8) * 255
        
        # Update with current world state
        self.add_environment_features()
        
        # Draw environment (simplified)
        self.ax_environment.clear()
        self.ax_environment.imshow(self.env_map)
        
        # Draw agent
        body = self.world.getAgentBody()
        self.ax_environment.plot(body.getX(), body.getY(), 'ro', markersize=12)
        
        self.ax_environment.set_title('Environmental Context')
        self.ax_environment.set_xlabel('X Position')
        self.ax_environment.set_ylabel('Y Position')
        self.ax_environment.set_aspect('equal')
    
    def animate(self, num_frames=300):
        """Start agent behavior animation"""
        
        print("Starting agent behavior visualization...")
        print("Visualization shows:")
        print("  - Trajectory: Agent's movement path through environment")
        print("  - Energy/Motivation: Energy, curiosity, and novelty levels")
        print("  - Action Selection: Distribution of chosen actions")
        print("  - Sensory Input: Current sensory input strengths")
        print("  - Learning Progress: Prediction error and development")
        print("  - Environment: Current environmental context")
        
        # Initialize tracking arrays
        self.prediction_error_history = []
        self.development_stages = []
        
        # Create animation
        ani = FuncAnimation(self.fig, self.update_visualization,
                           frames=num_frames, interval=100,
                           blit=False, repeat=True)
        
        plt.tight_layout()
        plt.show()
        
        return ani
    
    def record_behavioral_data(self, action, prediction_error, development_stage):
        """Record behavioral data for analysis"""
        
        self.prediction_error_history.append(prediction_error)
        self.development_stages.append(development_stage)
        self.action_history.append(action)
        self.perception_history.append(self.world.getSensoryPercept())
        
        # Keep history lengths manageable
        max_history = 1000
        if len(self.prediction_error_history) > max_history:
            self.prediction_error_history.pop(0)
            self.development_stages.pop(0)
            self.action_history.pop(0)
            self.perception_history.pop(0)
```

### 4. Performance Monitoring Visualization

```python
import time
import psutil
import numpy as np
from datetime import datetime

class PerformanceMonitor:
    """Real-time performance monitoring and visualization"""
    
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        
        # Performance metrics tracking
        self.cpu_usage = []
        self.memory_usage = []
        self.sim_time_history = []
        self.real_time_history = []
        self.frame_times = []
        self.dropped_frames = 0
        self.total_frames = 0
        
        # Setup performance dashboard
        self.fig, self.axes = plt.subplots(3, 2, figsize=(16, 10))
        self.fig.suptitle('Real-Time Performance Monitoring', fontsize=16)
        
        self.setup_performance_plots()
        
        # Performance thresholds
        self.cpu_threshold = 80.0  # %
        self.memory_threshold = 1024  # MB
        self.sim_threshold = 0.1  # seconds per step
        self.fps_threshold = 30.0  # frames per second
        
        # Alert system
        self.alerts = []
    
    def setup_performance_plots(self):
        """Setup performance monitoring plots"""
        
        # Plot 1: CPU and Memory Usage
        self.ax_resources = self.axes[0, 0]
        self.cpu_line, = self.ax_resources.plot([], [], 'r-', linewidth=2, label='CPU Usage')
        self.memory_line, = self.ax_resources.plot([], [], 'b-', linewidth=2, label='Memory Usage')
        
        self.ax_resources.set_title('Resource Usage')
        self.ax_resources.set_xlabel('Time Steps')
        self.ax_resources.set_ylabel('Usage (%)')
        self.ax_resources.set_ylim(0, 100)
        self.ax_resources.legend()
        self.ax_resources.grid(True, alpha=0.3)
        
        # Plot 2: Simulation Performance
        self.ax_simulation = self.axes[0, 1]
        self.sim_time_line, = self.ax_simulation.plot([], [], 'g-', linewidth=2, label='Sim Time/step')
        self.real_time_line, = self.ax_simulation.plot([], [], 'orange', linewidth=2, label='Real-time Factor')
        
        self.ax_simulation.set_title('Simulation Performance')
        self.ax_simulation.set_xlabel('Time Steps')
        self.ax_simulation.set_ylabel('Time (s) / Factor')
        self.ax_simulation.legend()
        self.ax_simulation.grid(True, alpha=0.3)
        
        # Plot 3: Frame Rate Monitor
        self.ax_fps = self.axes[1, 0]
        self.fps_line, = self.ax_fps.plot([], [], 'purple', linewidth=2, label='FPS')
        self.target_fps_line = self.ax_fps.axhline(y=self.fps_threshold, 
                                                 color='red', linestyle='--', alpha=0.7, label='Target FPS')
        
        self.ax_fps.set_title('Frame Rate Monitor')
        self.ax_fps.set_xlabel('Time Steps')
        self.ax_fps.set_ylabel('Frames Per Second')
        self.ax_fps.set_ylim(0, max(60, self.fps_threshold * 1.5))
        self.ax_fps.legend()
        self.ax_fps.grid(True, alpha=0.3)
        
        # Plot 4: Neural Activity vs Performance
        self.ax_neural_perf = self.axes[1, 1]\n        self.spike_rate_line, = self.ax_neural_perf.plot([], [], 'cyan', linewidth=2, label='Spike Rate')
        self.cpu_neural_line, = self.ax_neural_perf.plot([], [], 'red', linewidth=1, alpha=0.7, label='CPU (Neural)')
        
        self.ax_neural_perf.set_title('Neural Activity vs CPU')
        self.ax_neural_perf.set_xlabel('Time Steps')
        self.ax_neural_perf.set_ylabel('Rate / CPU (%)')
        self.ax_neural_perf.legend()
        self.ax_neural_perf.grid(True, alpha=0.3)
        
        # Plot 5: Bottleneck Detection
        self.ax_bottlenecks = self.axes[2, 0]
        self.bottleneck_data = {'memory': [], 'cpu': [], 'sim': [], 'fps': []}
        
        # Create subplots for different bottleneck indicators
        self.memory_indicator, = self.ax_bottlenecks.plot([], [], 'g-', linewidth=2, label='Memory Bottleneck')
        self.cpu_indicator, = self.ax_bottlenecks.plot([], [], 'r-', linewidth=2, label='CPU Bottleneck')
        self.sim_indicator, = self.ax_bottlenecks.plot([], [], 'orange', linewidth=2, label='Sim Bottleneck')
        
        self.ax_bottlenecks.set_title('Bottleneck Indicators')
        self.ax_bottlenecks.set_xlabel('Time Steps')
        self.ax_bottlenecks.set_ylabel('Bottleneck Level (0-1)')
        self.ax_bottlenecks.set_ylim(0, 1)
        self.ax_bottlenecks.legend()
        self.ax_bottlenecks.grid(True, alpha=0.3)
        
        # Plot 6: System Health
        self.ax_health = self.axes[2, 1]
        self.health_gauge = self.ax_health.bar(0, 0.5, color='green', alpha=0.7, label='System Health')
        self.health_text = self.ax_health.text(0, 0.6, '100%', ha='center', va='bottom')
        
        self.ax_health.set_title('System Health Score')
        self.ax_health.set_xlim(-1, 1)
        self.ax_health.set_ylim(0, 1)
        self.ax_health.set_xticks([])
        self.ax_health.set_yticks([])
        self.ax_health.legend()
        self.ax_health.grid(True, alpha=0.3)
    
    def update_monitoring(self, frame):
        """Update performance monitoring visualization"""
        
        start_time = time.time()
        
        # Collect performance metrics
        self.collect_metrics(frame)
        
        # Update all plots
        self.update_resource_plot()
        self.update_simulation_plot()
        self.update_fps_plot()
        self.update_neural_perf_plot()
        self.update_bottleneck_plot()
        self.update_health_plot()
        
        # Check for alerts
        self.check_for_alerts()
        
        # Draw alerts if any
        if self.alerts:
            self.draw_alerts()
        
        return [self.cpu_line, self.memory_line, self.sim_time_line, 
               self.real_time_line, self.fps_line, self.spike_rate_line,
               self.cpu_neural_line, self.memory_indicator, self.cpu_indicator,
               self.sim_indicator, list(self.health_gauge)[0]]
    
    def collect_metrics(self, frame):
        """Collect system and simulation performance metrics"""
        
        # System metrics
        process = psutil.Process()
        
        self.cpu_usage.append(psutil.cpu_percent(interval=None))
        self.memory_usage.append(process.memory_info().rss / (1024 * 1024))  # MB
        
        # Simulation metrics
        sim_start = time.time()
        
        # Update simulation components
        self.world.update(0.1)
        percept = self.world.getSensoryPercept()
        self.agent.processSensoryInput(percept)
        self.brain.step(frame)
        
        sim_time = time.time() - sim_start
        self.sim_time_history.append(sim_time)
        
        # Real-time factor
        real_time_factor = sim_time / 0.1  # Simulated real-time (0.1s per step)
        self.real_time_history.append(real_time_factor)
        
        # Frame timing
        frame_time = time.time() - start_time
        self.frame_times.append(frame_time)
        self.total_frames += 1
        
        # Neural activity metrics
        spike_rate = self.brain.getAverageFiringRate()
        
        # Track for analysis
        if not hasattr(self, 'spike_rate_history'):
            self.spike_rate_history = []
            self.cpu_neural_history = []
        
        self.spike_rate_history.append(spike_rate)
        self.cpu_neural_history.append(self.cpu_usage[-1] if self.cpu_usage else 0)
        
        # Limit history sizes
        max_history = 500
        for history in [self.cpu_usage, self.memory_usage, self.sim_time_history,
                       self.real_time_history, self.frame_times, self.spike_rate_history,
                       self.cpu_neural_history]:
            if len(history) > max_history:
                history.pop(0)
    
    def update_resource_plot(self):
        """Update resource usage plot"""
        
        steps = list(range(len(self.cpu_usage)))
        
        self.cpu_line.set_data(steps, self.cpu_usage)
        self.memory_line.set_data(steps, self.memory_usage)
        
        # Update y-axis limits
        if self.cpu_usage:
            self.ax_resources.set_xlim(0, len(self.cpu_usage))
            self.ax_resources.set_ylim(0, 100)
    
    def update_simulation_plot(self):
        """Update simulation performance plot"""
        
        steps = list(range(len(self.sim_time_history)))
        
        self.sim_time_line.set_data(steps, self.sim_time_history)
        self.real_time_line.set_data(steps, self.real_time_history)
        
        # Update y-axis limits
        if self.sim_time_history:
            max_sim_time = max(self.sim_time_history) if self.sim_time_history else 0
            max_factor = max(self.real_time_history) if self.real_time_history else 0
            self.ax_simulation.set_ylim(0, max(max_sim_time * 2, max_factor * 1.5))
    
    def update_fps_plot(self):
        """Update frame rate plot"""
        
        if not self.frame_times:
            return
        
        steps = list(range(len(self.frame_times)))
        fps_values = [1.0 / ft if ft > 0 else 0 for ft in self.frame_times]
        
        self.fps_line.set_data(steps, fps_values)
        
        # Update y-axis limits
        if fps_values:
            max_fps = max(fps_values) if fps_values else 0
            self.ax_fps.set_ylim(0, max(60, max_fps * 1.2))
    
    def update_neural_perf_plot(self):
        """Update neural activity vs performance plot"""
        
        steps = list(range(len(self.spike_rate_history)))
        
        self.spike_rate_line.set_data(steps, self.spike_rate_history)
        self.cpu_neural_line.set_data(steps, self.cpu_neural_history)
        
        # Update y-axis limits
        if self.spike_rate_history:
            max_spike_rate = max(self.spike_rate_history) if self.spike_rate_history else 0
            max_cpu = max(self.cpu_neural_history) if self.cpu_neural_history else 0
            self.ax_neural_perf.set_ylim(0, max(max_spike_rate * 1.1, max_cpu * 1.2))
    
    def update_bottleneck_plot(self):
        """Update bottleneck detection indicators"""
        
        steps = list(range(len(self.sim_time_history)))
        
        # Calculate bottleneck indicators (normalized 0-1)
        memory_bottleneck = []
        cpu_bottleneck = []
        sim_bottleneck = []
        fps_bottleneck = []
        
        for i in range(len(self.sim_time_history)):
            # Memory bottleneck
            memory_bottleneck.append(min(1.0, self.memory_usage[i] / self.memory_threshold))
            
            # CPU bottleneck
            cpu_bottleneck.append(min(1.0, self.cpu_usage[i] / self.cpu_threshold))
            
            # Simulation bottleneck
            sim_bottleneck.append(min(1.0, self.sim_time_history[i] / self.sim_threshold))
            
            # FPS bottleneck
            if i < len(self.frame_times) and self.frame_times[i] > 0:
                fps = 1.0 / self.frame_times[i]
                fps_bottleneck.append(min(1.0, fps / self.fps_threshold))
            else:
                fps_bottleneck.append(0)
        
        # Update bottleneck indicators
        self.memory_indicator.set_data(steps, memory_bottleneck)
        self.cpu_indicator.set_data(steps, cpu_bottleneck)
        self.sim_indicator.set_data(steps, sim_bottleneck)
        
        # Update y-axis
        self.ax_bottlenecks.set_xlim(0, len(self.sim_time_history))
        self.ax_bottlenecks.set_ylim(0, 1)
    
    def update_health_plot(self):
        """Update system health gauge"""
        
        if not self.frame_times:
            return
        
        # Calculate health score (inverse of bottleneck indicators)
        health_score = 100.0
        
        # Deduct for bottlenecks
        if self.cpu_usage and self.cpu_usage[-1] > self.cpu_threshold:
            health_score -= (self.cpu_usage[-1] - self.cpu_threshold) * 2
        
        if self.memory_usage and self.memory_usage[-1] > self.memory_threshold:
            health_score -= (self.memory_usage[-1] - self.memory_threshold) / 10
        
        if self.sim_time_history and self.sim_time_history[-1] > self.sim_threshold:
            health_score -= (self.sim_time_history[-1] - self.sim_threshold) * 10
        
        if self.frame_times and self.frame_times[-1] > 0:
            fps = 1.0 / self.frame_times[-1]
            if fps < self.fps_threshold:
                health_score -= (self.fps_threshold - fps) * 10
        
        # Ensure health score is within bounds
        health_score = max(0, min(100, health_score))
        
        # Update health bar
        bar_height = health_score / 100.0
        self.health_gauge[0].set_height(bar_height)
        
        # Update health color
        if health_score >= 80:
            color = 'green'
        elif health_score >= 60:
            color = 'yellow'
        else:
            color = 'red'
        
        self.health_gauge[0].set_color(color)
        
        # Update health text
        self.health_text.set_text(f'{int(health_score)}%')
        self.health_text.set_color('black' if health_score >= 80 else 'white')
    
    def check_for_alerts(self):
        """Check for performance issues and generate alerts"""
        
        current_time = datetime.now().strftime('%H:%M:%S')
        new_alerts = []
        
        # Check CPU threshold
        if self.cpu_usage and self.cpu_usage[-1] > self.cpu_threshold:
            new_alerts.append({
                'time': current_time,
                'type': 'High CPU Usage',
                'value': f"{self.cpu_usage[-1]:.1f}%",
                'severity': 'warning',
                'message': 'CPU usage is above normal threshold. Consider reducing parallel processing.'
            })
        
        # Check memory threshold
        if self.memory_usage and self.memory_usage[-1] > self.memory_threshold:
            new_alerts.append({
                'time': current_time,
                'type': 'High Memory Usage',
                'value': f"{self.memory_usage[-1]:.0f} MB",
                'severity': 'warning',
                'message': 'Memory usage is above normal threshold. Optimize memory pools or reduce simulation size.'
            })
        
        # Check simulation time
        if self.sim_time_history and self.sim_time_history[-1] > self.sim_threshold:
            new_alerts.append({
                'time': current_time,
                'type': 'Slow Simulation',
                'value': f"{self.sim_time_history[-1]:.3f}s/step",
                'severity': 'error',
                'message': 'Simulation step time is above optimal threshold. Investigate bottlenecks.'
            })
        
        # Check FPS
        if self.frame_times and self.frame_times[-1] > 0:
            fps = 1.0 / self.frame_times[-1]
            if fps < self.fps_threshold:
                new_alerts.append({
                    'time': current_time,
                    'type': 'Low Frame Rate',
                    'value': f"{fps:.1f} FPS",
                    'severity': 'warning',
                    'message': 'Frame rate is below optimal threshold. Consider reducing visualization detail.'
                })
        
        # Add new alerts
        self.alerts.extend(new_alerts)
        
        # Keep only recent alerts (last 5)
        if len(self.alerts) > 5:
            self.alerts = self.alerts[-5:]
    
    def draw_alerts(self):
        """Draw alert information on the plot"""
        
        alert_text = '\n'.join([f"[{alert['time']}] {alert['type']}: {alert['value']}"
                               for alert in self.alerts])
        
        self.ax_health.text(0, 1.2, alert_text, fontsize=8, 
                           bbox=dict(boxstyle='round', facecolor='red', alpha=0.7),
                           verticalalignment='top')
    
    def animate(self, num_frames=200):
        """Start real-time performance monitoring animation"""
        
        print("Starting real-time performance monitoring...")
        print("Monitoring shows:")
        print("  - Resource Usage: CPU and memory consumption")
        print("  - Simulation Performance: Time per step and real-time factor")
        print("  - Frame Rate Monitor: Current FPS vs target")
        print("  - Neural Activity vs CPU: Relationship between spikes and CPU usage")
        print("  - Bottleneck Indicators: Detection of performance bottlenecks")
        print("  - System Health: Overall system health score")
        
        # Create animation
        ani = FuncAnimation(self.fig, self.update_monitoring,
                           frames=num_frames, interval=100,
                           blit=True, repeat=True)
        
        plt.tight_layout()
        plt.show()
        
        return ani
```

### 5. Interactive Visualization Interface

```python
import pygame
import OpenGL
from OpenGL.GL import *
from OpenGL.GLU import *
import numpy as np

class InteractiveVisualizer:
    """Interactive 3D visualization for NLM simulations"""
    
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        
        # Pygame and OpenGL setup
        pygame.init()
        pygame.display.set_caption('NLM Interactive Visualizer')
        
        # Create OpenGL context
        self.screen = pygame.display.set_mode((1280, 720), pygame.OPENGL | pygame.DOUBLEBUF)
        
        # Initialize OpenGL
        self.setup_opengl()
        
        # Visualization data
        self.neural_positions = []
        self.agent_path = []
        self.environment_objects = []
        
        # Interaction state
        self.camera_angle = 0
        self.camera_distance = 20
        self.selected_neuron = None
        self.interaction_mode = 'navigation'  # navigation, inspection, analysis
        
        # Performance tracking
        self.frame_count = 0
        self.last_fps_update = time.time()
        self.current_fps = 0
    
    def setup_opengl(self):
        """Setup OpenGL rendering context"""
        
        # Set background color
        glClearColor(0.1, 0.1, 0.15, 1.0)
        
        # Enable depth testing
        glEnable(GL_DEPTH_TEST)
        glDepthFunc(GL_LESS)
        
        # Enable blending
        glEnable(GL_BLEND)
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
        
        # Enable lighting
        glEnable(GL_LIGHTING)
        glEnable(GL_LIGHT0)
        
        # Setup lighting
        light_pos = [10.0, 10.0, 10.0, 1.0]
        glLightfv(GL_LIGHT0, GL_POSITION, light_pos)
        
        # Set material properties
        glMaterialfv(GL_FRONT, GL_AMBIENT, [0.2, 0.2, 0.2, 1.0])
        glMaterialfv(GL_FRONT, GL_DIFFUSE, [0.8, 0.8, 0.8, 1.0])
        glMaterialfv(GL_FRONT, GL_SPECULAR, [1.0, 1.0, 1.0, 1.0])
        glMaterialf(GL_FRONT, GL_SHININESS, 50.0)
        
        # Setup camera
        glMatrixMode(GL_PROJECTION)
        gluPerspective(45, 1280/720, 0.1, 1000.0)
        
        glMatrixMode(GL_MODELVIEW)
        gluLookAt(0, 10, 30, 0, 0, 0, 0, 1, 0)
    
    def update_visualization_data(self):
        """Update visualization data from simulation"""
        
        # Update world
        self.world.update(0.1)
        
        # Get current state
        body = self.world.getAgentBody()
        percept = self.world.getSensoryPercept()
        
        # Update agent path
        self.agent_path.append((body.getX(), body.getY(), body.getOrientation()))
        if len(self.agent_path) > 100:
            self.agent_path.pop(0)
        
        # Update neural positions (sample from large network)
        self.neural_positions = []
        total_neurons = self.brain.getTotalNeuronCount()
        sample_step = max(1, total_neurons // 1000)  # Sample 1000 neurons
        
        for i in range(0, min(total_neurons, 1000), sample_step):
            firing_rate = self.brain.getNeuronFiringRate(i)
            if firing_rate > 0.1:  # Only show actively firing neurons
                # Convert to 3D coordinates (simplified mapping)
                x = (i % 100) * 0.1 - 5
                y = (i // 100) * 0.1 - 5
                z = firing_rate * 2
                
                self.neural_positions.append((x, y, z, firing_rate))
        
        # Update environment objects
        self.environment_objects = []
        
        # Add resources
        for obj in self.world.getObjects(pynlm.WorldObjectType.Resource):
            self.environment_objects.append({
                'type': 'resource',
                'position': (obj.getX(), obj.getY(), 0),
                'value': obj.getValue(),
                'color': (0.0, 1.0, 0.0, 0.7)  # Green
            })
        
        # Add hazards
        for obj in self.world.getObjects(pynlm.WorldObjectType.Hazard):
            self.environment_objects.append({
                'type': 'hazard',
                'position': (obj.getX(), obj.getY(), 0),
                'value': obj.getValue(),
                'color': (1.0, 0.0, 0.0, 0.7)  # Red
            })
        
        # Add walls
        for obj in self.world.getObjects(pynlm.WorldObjectType.Wall):
            self.environment_objects.append({
                'type': 'wall',
                'position': (obj.getX(), obj.getY(), 0),
                'value': obj.getValue(),
                'color': (0.5, 0.5, 0.5, 0.8)  # Gray
            })
    
    def render_3d_scene(self):
        """Render 3D scene"""
        
        # Clear buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        
        # Update camera
        self.update_camera()
        
        # Draw environment
        self.draw_environment()
        
        # Draw neural network
        self.draw_neural_network()
        
        # Draw agent
        self.draw_agent()
        
        # Draw UI elements
        self.draw_ui()
        
        # Swap buffers
        pygame.display.flip()
    
    def update_camera(self):
        """Update camera position and orientation"""
        
        # Get agent position for following camera
        body = self.world.getAgentBody()
        
        # Calculate camera position
        agent_x, agent_y, agent_angle = body.getX(), body.getY(), body.getOrientation()
        
        camera_x = agent_x + self.camera_distance * np.cos(np.radians(agent_angle))
        camera_y = agent_y + self.camera_distance * np.sin(np.radians(agent_angle))
        camera_z = 10
        
        # Look at agent
        gluLookAt(camera_x, camera_y, camera_z,
                 agent_x, agent_y, 0,
                 0, 1, 0)
        
        # Rotate for better viewing
        glRotatef(self.camera_angle, 0, 1, 0)
    
    def draw_environment(self):
        """Draw environment objects"""
        
        glPushMatrix()
        
        # Draw environment objects
        for obj in self.environment_objects:
            glPushMatrix()
            
            # Set color
            glColor4f(*obj['color'])
            
            # Draw based on type
            if obj['type'] == 'resource':
                self.draw_resource(obj['position'])
            elif obj['type'] == 'hazard':
                self.draw_hazard(obj['position'])
            elif obj['type'] == 'wall':
                self.draw_wall(obj['position'])
            
            glPopMatrix()
        
        glPopMatrix()
    
    def draw_neural_network(self):
        """Draw neural network as 3D connections"""
        
        glPushMatrix()
        
        # Enable lighting for neural connections
        glEnable(GL_LIGHTING)
        
        # Draw neural connections (simplified)
        for i, neuron in enumerate(self.neural_positions):
            if i < len(self.neural_positions) - 1:
                # Draw connection to next neuron (simplified)
                glBegin(GL_LINES)
                
                x1, y1, z1, rate1 = neuron
                x2, y2, z2, rate2 = self.neural_positions[i + 1]
                
                # Set color based on firing rate
                glColor3f(rate1, rate1, rate1)
                glVertex3f(x1, y1, z1)
                
                glColor3f(rate2, rate2, rate2)
                glVertex3f(x2, y2, z2)
                
                glEnd()
        
        # Draw neurons as spheres
        glDisable(GL_LIGHTING)
        glPointSize(5.0)
        
        glBegin(GL_POINTS)
        for neuron in self.neural_positions:
            x, y, z, rate = neuron
            
            # Set color based on firing rate
            glColor3f(rate, rate, rate)
            glVertex3f(x, y, z)
        
        glEnd()
        
        glPopMatrix()
    
    def draw_agent(self):
        """Draw agent and its sensory input"""
        
        glPushMatrix()
        
        # Get agent state
        body = self.world.getAgentBody()
        agent_x, agent_y, agent_angle = body.getX(), body.getY(), body.getOrientation()
        
        # Draw agent body
        glColor3f(0.0, 0.5, 1.0)  # Blue agent
        glPushMatrix()
        glTranslatef(agent_x, agent_y, 0)
        glRotatef(agent_angle, 0, 0, 1)
        
        # Draw agent as triangle
        glBegin(GL_TRIANGLES)
        glVertex3f(0, 0.5, 0)
        glVertex3f(-0.3, -0.3, 0)
        glVertex3f(0.3, -0.3, 0)
        glEnd()
        
        glPopMatrix()
        
        # Draw agent's field of view
        self.draw_fov(agent_x, agent_y, agent_angle)
        
        glPopMatrix()
    
    def draw_fov(self, agent_x, agent_y, agent_angle):
        """Draw agent's field of view cone"""
        
        glPushMatrix()
        
        # Get percept for FOV
        percept = self.world.getSensoryPercept()
        fov_width = percept.getFOVWidth() if hasattr(percept, 'getFOVWidth') else 8
        fov_height = percept.getFOVHeight() if hasattr(percept, 'getFOVHeight') else 8
        
        # Draw FOV rectangle
        glColor4f(0.0, 1.0, 1.0, 0.3)  # Cyan, semi-transparent
        
        glBegin(GL_QUADS)
        half_width = fov_width / 2
        half_height = fov_height / 2
        
        # Rotate to agent's orientation
        glRotatef(agent_angle, 0, 0, 1)
        
        glVertex3f(agent_x, agent_y, 0)
        glVertex3f(agent_x + half_width, agent_y + half_height, 0)
        glVertex3f(agent_x - half_width, agent_y + half_height, 0)
        glVertex3f(agent_x - half_width, agent_y - half_height, 0)
        
        glEnd()
        
        glPopMatrix()
    
    def draw_ui(self):
        """Draw user interface elements"""
        
        glDisable(GL_DEPTH_TEST)
        glMatrixMode(GL_PROJECTION)
        glPushMatrix()
        
        # Set orthographic projection for UI
        glLoadIdentity()
        gluOrtho2D(0, 1280, 0, 720)
        
        glMatrixMode(GL_MODELVIEW)
        glPushMatrix()
        glLoadIdentity()
        
        # Draw FPS counter
        fps_text = f"FPS: {self.current_fps:.1f}"
        self.draw_text(10, 700, fps_text, (1.0, 1.0, 1.0, 1.0))
        
        # Draw neuron count
        neuron_count = self.brain.getTotalNeuronCount() if hasattr(self.brain, 'getTotalNeuronCount') else 0
        neurons_text = f"Neurons: {neuron_count:,}"
        self.draw_text(10, 670, neurons_text, (1.0, 1.0, 1.0, 1.0))
        
        # Draw interaction help
        help_text = "WASD: Move | R: Rotate | Q: Zoom | TAB: Switch Mode"
        self.draw_text(10, 640, help_text, (0.8, 0.8, 0.8, 1.0))
        
        glPopMatrix()
        glMatrixMode(GL_PROJECTION)
        glPopMatrix()
        glMatrixMode(GL_MODELVIEW)
        glEnable(GL_DEPTH_TEST)
    
    def draw_text(self, x, y, text, color):
        """Draw text on screen (simplified)"""
        
        # This is a simplified version - in practice you'd use a proper text rendering system
        pass
    
    def draw_resource(self, position):
        """Draw resource object"""
        
        x, y, _ = position
        glPushMatrix()
        glTranslatef(x, y, 0)
        
        # Draw cube for resource
        glBegin(GL_QUADS)
        
        # Front face
        glColor3f(0.0, 1.0, 0.0)
        glVertex3f(-0.3, -0.3, 0)
        glVertex3f(0.3, -0.3, 0)
        glVertex3f(0.3, 0.3, 0)
        glVertex3f(-0.3, 0.3, 0)
        
        # Other faces...
        
        glEnd()
        
        glPopMatrix()
    
    def draw_hazard(self, position):
        """Draw hazard object"""
        
        x, y, _ = position
        glPushMatrix()
        glTranslatef(x, y, 0)
        
        # Draw triangle for hazard
        glColor3f(1.0, 0.0, 0.0)
        glBegin(GL_TRIANGLES)
        glVertex3f(0, 0.5, 0)
        glVertex3f(-0.3, -0.3, 0)
        glVertex3f(0.3, -0.3, 0)
        glEnd()
        
        glPopMatrix()
    
    def draw_wall(self, position):
        """Draw wall object"""
        
        x, y, _ = position
        glPushMatrix()
        glTranslatef(x, y, 0)
        
        # Draw box for wall
        glColor3f(0.5, 0.5, 0.5)
        glBegin(GL_QUADS)
        
        # Front face
        glVertex3f(-0.4, -0.4, 0)
        glVertex3f(0.4, -0.4, 0)
        glVertex3f(0.4, 0.4, 0)
        glVertex3f(-0.4, 0.4, 0)
        
        # Other faces...
        
        glEnd()
        
        glPopMatrix()
    
    def run_interactive(self):
        """Run interactive visualization loop"""
        
        print("Starting interactive NLM visualization...")
        print("Controls:")
        print("  WASD: Move camera")
        print("  R: Rotate view")
        print("  Q/E: Zoom in/out")
        print("  TAB: Switch interaction mode")
        print("  ESC: Exit")
        
        clock = pygame.time.Clock()
        running = True
        
        while running:
            # Handle events
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    running = False
                elif event.type == pygame.KEYDOWN:
                    self.handle_key_input(event.key)
            
            # Update visualization data
            self.update_visualization_data()
            
            # Render scene
            self.render_3d_scene()
            
            # Update FPS counter
            self.frame_count += 1
            if time.time() - self.last_fps_update > 1.0:
                self.current_fps = self.frame_count / (time.time() - self.last_fps_update)
                self.frame_count = 0
                self.last_fps_update = time.time()
            
            # Cap frame rate
            clock.tick(60)
        
        # Cleanup
        pygame.quit()
    
    def handle_key_input(self, key):
        """Handle keyboard input"""
        
        if key == pygame.K_ESCAPE:
            pygame.quit()
            exit()
        
        elif key == pygame.K_w:
            self.camera_distance *= 0.9  # Zoom in
        elif key == pygame.K_s:
            self.camera_distance *= 1.1  # Zoom out
        elif key == pygame.K_a:
            self.camera_angle += 5  # Rotate left
        elif key == pygame.K_d:
            self.camera_angle -= 5  # Rotate right
        elif key == pygame.K_TAB:
            # Switch interaction mode
            self.interaction_mode = ('navigation', 'inspection', 'analysis')[(
                ['navigation', 'inspection', 'analysis'].index(self.interaction_mode) + 1) % 3]
        elif key == pygame.K_1:
            # Inspection mode - select neuron
            self.interaction_mode = 'inspection'
        elif key == pygame.K_2:
            # Analysis mode - show analytics
            self.interaction_mode = 'analysis'
```

### 6. Complete Advanced Example Integration

```python
def main():
    """Complete real-time visualization example"""
    
    print("=== NLM Real-Time Visualization Example ===")
    print("This example demonstrates comprehensive real-time visualization")
    print("and monitoring capabilities for NLM simulations.\n")
    
    # 1. Setup simulation environment
    print("1. Setting up simulation environment...")
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    
    # Create brain and agent
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    print("   ✓ Simulation environment created")
    print("   ✓ Brain and agent initialized")
    print("   ✓ All subsystems enabled")
    
    # 2. Create visualization components
    print("\n2. Creating visualization components...")
    
    # Create different visualization systems
    neural_visualizer = NeuralActivityVisualizer(brain)
    agent_behavior_visualizer = AgentBehaviorVisualizer(world, agent, brain)
    performance_monitor = PerformanceMonitor(brain, world, agent)
    interactive_visualizer = InteractiveVisualizer(brain, world, agent)
    
    print("   ✓ Neural activity visualizer")
    print("   ✓ Agent behavior visualizer")
    print("   ✓ Performance monitor")
    print("   ✓ Interactive 3D visualizer")
    
    # 3. Run real-time demonstrations
    print("\n3. Running real-time demonstrations...")
    
    # Option 1: Run all visualizations in sequence
    print("   Running neural activity visualization...")
    neural_anim = neural_visualizer.animate(300)
    plt.show()
    
    print("   Running agent behavior visualization...")
    agent_anim = agent_behavior_visualizer.animate(300)
    plt.show()
    
    print("   Running performance monitoring...")
    perf_anim = performance_monitor.animate(300)
    plt.show()
    
    # 4. Run interactive 3D visualization
    print("   Starting interactive 3D visualization...")
    print("   Note: This will start a separate window. Close it to continue.")
    
    # Uncomment to run interactive visualization
    # interactive_visualizer.run_interactive()
    
    print("   ✓ Interactive visualization ready (commented out for web)")
    
    # 5. Save final state
    print("\n4. Saving final visualization state...")
    
    brain.save("realtime_visualization_brain.bin")
    
    # Create visualization summary
    create_visualization_summary(brain, world, agent)
    
    print("   ✓ Brain state saved")
    print("   ✓ Visualization summary created")
    
    print("\n=== Real-Time Visualization Example Complete ===")
    print("\nVisualization capabilities demonstrated:")
    print("✓ Real-time neural activity monitoring")
    print("✓ Agent behavior tracking and visualization")
    print("✓ Performance monitoring and bottleneck detection")
    print("✓ Interactive 3D visualization")
    print("✓ Multi-modal data representation")
    print("✓ Alert system for performance issues")
    
    print("\nKey benefits:")
    print("• Immediate feedback on simulation behavior")
    print("• Deep insight into neural dynamics")
    print("• Early detection of performance issues")
    print("• Enhanced debugging capabilities")
    print("• Better understanding of agent-environment interaction")
    
    print("\nNext steps:")
    print("1. Explore the generated visualization files")
    print("2. Customize visualization parameters")
    print("3. Add custom visualization components")
    print("4. Integrate with your own analysis tools")

def create_visualization_summary(brain, world, agent):
    """Create summary of visualization capabilities"""
    
    print(f"\n=== Visualization Summary ===")
    
    # Capture final state
    body = world.getAgentBody()
    
    print(f"Simulation State:")
    print(f"   Total neurons: {brain.getTotalNeuronCount():,}")
    print(f"   Agent position: ({body.getX():.1f}, {body.getY():.1f})")
    print(f"   Agent energy: {body.getEnergy():.1f}")
    print(f"   Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"   Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"   Development stage: {brain.getDevelopmentalStage()}")
    
    print(f"\nVisualization Features:")
    print(f"   ✓ 2D neural activity plots")
    print(f"   ✓ Agent trajectory tracking")
    print(f"   ✓ Performance monitoring")
    print(f"   ✓ Interactive 3D environment")
    print(f"   ✓ Real-time alert system")
    print(f"   ✓ Multi-scale visualization")
    
    print(f"\nData Capture:")
    print(f"   ✓ Neural firing rates")
    print(f"   ✓ Agent behavior metrics")
    print(f"   ✓ Environmental interactions")
    print(f"   • System performance indicators")
    print(f"   • Learning progress tracking")

if __name__ == "__main__":
    main()
```

## Best Practices for Real-Time Visualization

1. **Start with 2D visualizations**: Begin with simple 2D plots before adding 3D complexity
2. **Sample data for performance**: For large networks, sample subsets of neurons
3. **Use efficient rendering**: Optimize OpenGL calls for real-time performance
4. **Monitor frame rates**: Ensure consistent 30+ FPS for smooth experience
5. **Implement progressive updates**: Update visualizations at appropriate intervals
6. **Use color coding wisely**: Choose colors that highlight important patterns
7. **Provide interaction modes**: Allow users to zoom, pan, and inspect data
8. **Handle data overflow**: Implement circular buffers for continuous data
9. **Test on target hardware**: Verify performance on intended display hardware
10. **Document interactions**: Provide clear documentation for all controls

## Extensions and Variations

1. **Multi-agent visualization**: Track multiple agents simultaneously
2. **Network topology visualization**: Show connectivity patterns
3. **Learning timeline visualization**: Track learning progress over time
4. **Environmental change visualization**: Show world state evolution
5. **Custom metric visualization**: Add domain-specific visualizations
6. **Collaboration tools**: Share visualizations across multiple users
7. **Export capabilities**: Save visualizations in various formats
8. **Web integration**: Deploy visualizations to web browsers
9. **Mobile optimization**: Adapt for mobile and tablet devices
10. **AR/VR integration**: Extend to augmented and virtual reality

## Common Pitfalls and Solutions

| Pitfall | Solution |
|---------|----------|
| Poor performance with large networks | Sample neurons, use efficient rendering |
| Overly complex visualizations | Start simple, add complexity gradually |
| Difficult interaction | Provide clear documentation and hints |
| Memory leaks in visualizations | Use proper cleanup and resource management |
| Disorientation in 3D | Provide clear coordinate reference systems |
| Slow update rates | Optimize data capture and rendering pipelines |
| Inconsistent frame rates | Implement adaptive rendering based on system load |
| Color blindness issues | Use color-blind friendly palettes |

## Further Reading

- [Performance Documentation](./docs/PERFORMANCE.md)
- [Architecture Overview](./docs/ARCHITECTURE.md)
- [Phase 6 Integration](./docs/PHASE6_ARCHITECTURE.md)
- [Visualization Research Papers](https://arxiv.org/search/?q=neural+network+visualization)

This example provides a comprehensive foundation for real-time visualization and monitoring of NLM simulations, enabling deep insights into neural dynamics and agent behavior.