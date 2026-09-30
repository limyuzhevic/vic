# NLM - Visualization and Analysis Tools

**Phase 6: Final Integration with Visualization and Analysis Features**

This document describes the visualization and analysis tools added to NLM (Neural Learning Machine) Phase 6 implementation.
These tools provide comprehensive visualization capabilities and data analysis for neural simulations.

## Overview of Visualization and Analysis Enhancements

### 1. Visualization Interface

The enhanced visualization system provides:

#### 1.1 Basic Visualization

**Core Visualization Features:**
- 2D neural population visualization
- Time series plotting of neural activity
- World view with agent and objects
- Spike raster plots
- Heat maps of neural activity

```python
import pynlm
from nlm_visualization import NeuralVisualizer

# Create visualizer
visualizer = NeuralVisualizer()

# Setup visualization
visualizer.setupBrain(brain)
visualizer.setupWorld(world)
visualizer.setupAgent(agent)

# Run simulation with live visualization
for step in range(100):
    # Run simulation step
    brain.step(step)
    world.update(0.1)
    
    # Get data for visualization
    neural_activity = brain.getAverageFiringRate()
    world_view = world.getSensoryPercept()
    agent_pos = world.getAgentBody()
    
    # Update visualization
    visualizer.update(step, neural_activity, world_view, agent_pos)
    
    # Render (if enabled)
    if visualizer.renderEnabled:
        visualizer.render()
        time.sleep(0.01)  # Control frame rate

# Save visualization
visualizer.saveAnimation("simulation.mp4")
visualizer.saveFrames("frames/")
```

#### 1.2 Advanced Visualization

**Advanced Features:**
- 3D neural volume visualization
- Multiple simultaneous viewports
- Interactive inspection
- Custom rendering pipelines
- Data overlays and annotations

```python
# Advanced 3D visualization
visualizer = NeuralVisualizer()
visualizer.setup3DBrain(brain)
visualizer.enable3DEnabled(True)

# Configure rendering
visualizer.setRenderMode("volume_rendering")
visualizer.setOpacity(0.7)
visualizer.setLighting(True)

# Add annotations
visualizer.addAnnotation("Target", target_position, color=(1,0,0))
visualizer.addAnnotation("Resource", resource_position, color=(0,1,0))

# Custom shader
visualizer.setShader("metallic", {"shininess": 0.8, "metallic": 0.2})

# Interactive controls
visualizer.setupInteractiveControls({
    "rotate": True,
    "zoom": True,
    "pan": True,
    "select": True,
    "measure": True
})
```

### 2. Data Analysis and Processing

#### 2.1 Neural Activity Analysis

**Analysis Functions:**
- Spike detection and analysis
- Frequency spectrum analysis
- Correlation analysis between neurons
- Pattern recognition
- Change detection

```python
import numpy as np
from scipy import signal
from sklearn.cluster import KMeans
from nlm_analysis import NeuralDataAnalyzer

def analyze_neural_activity(brain, window_size=50):
    """Comprehensive neural activity analysis"""
    
    # Get neural activity time series
    activity_history = brain.getActivityHistory()
    firing_history = brain.getFiringHistory()
    
    analyzer = NeuralDataAnalyzer()
    
    # Basic statistics
    stats = analyzer.getBasicStats(activity_history)
    print(f"Activity Statistics:")
    print(f"  Mean: {stats['mean']:.3f}")
    print(f"  Std: {stats['std']:.3f}")
    print(f"  Min: {stats['min']:.3f}")
    print(f"  Max: {stats['max']:.3f}")
    
    # Frequency analysis
    fs = 1.0  # Simulation time step (normalized)
    freqs, psd = signal.periodogram(activity_history, fs)
    
    dominant_freq = freqs[np.argmax(psd)]
    print(f"Dominant frequency: {dominant_freq:.3f} Hz")
    
    # Pattern detection
    patterns = analyzer.detectTemporalPatterns(activity_history, min_duration=10)
    print(f"Detected patterns: {len(patterns)}")
    
    # Change detection
    changes = analyzer.detectAbruptChanges(activity_history, threshold=2.0)
    print(f"Abrupt changes detected: {len(changes)}")
    
    # Clustering analysis
    if len(activity_history) >= 100:
        X = np.array(activity_history[-100:]).reshape(-1, 1)
        kmeans = KMeans(n_clusters=3, random_state=42)
        clusters = kmeans.fit_predict(X)
        
        print(f"Neural activity clusters:")
        for i in range(max(clusters) + 1):
            cluster_size = np.sum(clusters == i)
            print(f"  Cluster {i}: {cluster_size} samples")
    
    return {
        'statistics': stats,
        'dominant_frequency': dominant_freq,
        'patterns': patterns,
        'changes': changes,
        'clusters': clusters if 'clusters' in locals() else None
    }
```

#### 2.2 Behavioral Analysis

**Behavioral Analysis Functions:**
- Action sequence analysis
- Decision pattern analysis
- Learning curve analysis
- Reward prediction error analysis
- Exploration vs. exploitation analysis

```python
from nlm_analysis import BehavioralAnalyzer

def analyze_behavioral_patterns(agent, world, num_steps=500):
    """Analyze agent behavioral patterns"""
    
    analyzer = BehavioralAnalyzer()
    
    # Collect behavioral data
    actions = []
    rewards = []
    curiosity_levels = []
    novelty_levels = []
    
    for step in range(num_steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        action = agent.decodeMotorCommand()
        
        # Apply action and get reward
        world.applyMotorCommand(action, step * 0.1)
        reward = compute_environment_reward(world, agent)
        
        actions.append(action)
        rewards.append(reward)
        curiosity_levels.append(agent.getCuriosityLevel())
        novelty_levels.append(agent.getNoveltyLevel())
    
    # Action sequence analysis
    action_sequences = analyzer.extractActionSequences(actions, max_length=10)
    print(f"Action sequences detected: {len(action_sequences)}")
    
    # Decision patterns
    decision_patterns = analyzer.analyzeDecisionPatterns(actions, rewards)
    print(f"Decision patterns: {len(decision_patterns)}")
    
    # Learning curves
    learning_curves = analyzer.analyzeLearningCurves(
        curiosity_levels, novelty_levels, rewards
    )
    
    # Exploration vs. exploitation
    exploration_ratio = analyzer.calculateExplorationExploitationRatio(
        actions, curiosity_levels
    )
    print(f"Exploration ratio: {exploration_ratio:.3f}")
    
    return {
        'action_sequences': action_sequences,
        'decision_patterns': decision_patterns,
        'learning_curves': learning_curves,
        'exploration_ratio': exploration_ratio,
        'action_distribution': analyzer.getActionDistribution(actions)
    }
```

#### 2.3 Memory Analysis

**Memory Analysis Functions:**
- Working memory capacity analysis
- Episodic memory effectiveness
- Memory consolidation analysis
- Memory replay analysis
- Memory decay analysis

```python
from nlm_analysis import MemoryAnalyzer

def analyze_memory_systems(brain, num_steps=200):
    """Analyze memory system performance"""
    
    analyzer = MemoryAnalyzer()
    
    # Memory system analysis
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    
    if working_memory:
        wm_capacity = analyzer.analyzeWorkingMemoryCapacity(working_memory)
        print(f"Working memory capacity: {wm_capacity:.3f}")
    
    if episodic_memory:
        # Run some steps to store episodes
        for step in range(num_steps):
            brain.step(step, step * 0.001)
        
        episodic_stats = analyzer.analyzeEpisodicMemory(e episodic_memory)
        print(f"Episodic memory episodes: {episodic_stats['episode_count']}")
        print(f"Episodic memory effectiveness: {episodic_stats['retrieval_accuracy']:.3f}")
        
        # Memory replay analysis
        replay_events = analyzer.analyzeMemoryReplay(brain)
        print(f"Memory replay events: {len(replay_events)}")
    
    # Memory consolidation
    consolidation = analyzer.analyzeMemoryConsolidation(brain)
    print(f"Memory consolidation strength: {consolidation['consolidation_strength']:.3f}")
    
    return {
        'working_memory_capacity': wm_capacity if working_memory else None,
        'episodic_memory_stats': episodic_stats if episodic_memory else None,
        'replay_events': replay_events,
        'consolidation': consolidation
    }
```

### 3. Interactive Analysis Tools

#### 3.1 Jupyter Notebook Integration

**Notebook Tools:**
- Interactive simulation control
- Real-time data visualization
- Parameter tuning
- Experiment comparison
- Machine learning integration

```python
# Example Jupyter notebook integration
# Create interactive widgets
import ipywidgets as widgets
from IPython.display import display

def create_interactive_analysis():
    # Control widgets
    neuron_slider = widgets.IntSlider(value=100, min=50, max=1000, description='Neurons')
    steps_slider = widgets.IntSlider(value=100, min=10, max=1000, description='Steps')
    curiosity_toggle = widgets.Checkbox(value=True, description='Enable Curiosity')
    
    # Display controls
    display(widgets.VBox([neuron_slider, steps_slider, curiosity_toggle]))
    
    # Analysis function
    def run_analysis():
        # Setup simulation with selected parameters
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", neuron_slider.value)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 8, 8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        if curiosity_toggle.value:
            agent.enableCuriosity(True)
        
        # Run simulation
        for step in range(steps_slider.value):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, step * 0.1)
        
        # Return analysis results
        return {
            'avg_firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount(),
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel()
        }
    
    # Connect button
    analyze_button = widgets.Button(description='Run Analysis')
    analyze_button.on_click(lambda b: display(run_analysis()))
    
    display(analyze_button)
```

#### 3.2 Web Dashboard

**Web Interface Features:**
- Real-time simulation monitoring
- Live data visualization
- Performance metrics dashboard
- Experiment results comparison
- Export capabilities

```python
from flask import Flask, render_template, jsonify
import threading
import time

app = Flask(__name__)
simulation_data = []
simulation_running = False

def simulation_thread():
    global simulation_running, simulation_data
    simulation_running = True
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 200)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(15, 15, 8, 8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    for step in range(1000):
        if not simulation_running:
            break
            
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, step * 0.1)
        
        # Record data
        simulation_data.append({
            'step': step,
            'firing_rate': brain.getAverageFiringRate(),
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'reward': get_recent_reward(world, agent)
        })
        
        time.sleep(0.01)  # Control update rate
    
    simulation_running = False

@app.route('/')
def dashboard():
    return render_template('dashboard.html', data=simulation_data[-100:])

@app.route('/api/data')
def get_data():
    return jsonify(simulation_data[-100:])

@app.route('/start', methods=['POST'])
def start_simulation():
    global simulation_data
    simulation_data = []
    thread = threading.Thread(target=simulation_thread)
    thread.start()
    return jsonify({'status': 'started'})

@app.route('/stop', methods=['POST'])
def stop_simulation():
    global simulation_running
    simulation_running = False
    return jsonify({'status': 'stopped'})

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8080, debug=True)
```

### 4. Export and Analysis Tools

#### 4.1 Data Export

**Export Formats:**
- CSV for spreadsheet analysis
- JSON for programmatic access
- MATLAB for scientific computing
- HDF5 for large datasets
- Custom formats for specific applications

```python
from nlm_export import DataExporter

def export_simulation_results(brain, agent, world, filename_prefix="simulation"):
    """Export simulation results to multiple formats"""
    
    exporter = DataExporter()
    
    # Collect data
    data = {
        'brain': {
            'firing_history': brain.getFiringHistory(),
            'activity_history': brain.getActivityHistory(),
            'total_spikes': brain.getTotalSpikeCount(),
            'average_firing_rate': brain.getAverageFiringRate()
        },
        'agent': {
            'curiosity_history': agent.getCuriosityHistory(),
            'novelty_history': agent.getNoveltyHistory(),
            'reward_history': agent.getRewardHistory(),
            'neuromodulation_level': agent.getNeuromodulationLevel()
        },
        'world': {
            'position_history': get_world_position_history(world),
            'reward_history': get_world_reward_history(world)
        }
    }
    
    # Export to different formats
    exporter.export_csv(data, f"{filename_prefix}.csv")
    exporter.export_json(data, f"{filename_prefix}.json")
    
    # For large datasets, use HDF5
    if len(brain.getFiringHistory()) > 10000:
        exporter.export_hdf5(data, f"{filename_prefix}.h5")
    
    return f"Exported to: {filename_prefix}.*"
```

#### 4.2 Analysis Reports

**Automated Report Generation:**
- Performance summaries
- Statistical analysis
- Visualization summaries
- Comparative analysis
- Statistical significance testing

```python
from nlm_reports import ReportGenerator

def generate_analysis_report(brain, agent, world, experiment_name):
    """Generate comprehensive analysis report"""
    
    report = ReportGenerator()
    
    # Basic statistics
    report.add_section("Basic Statistics")
    report.add_metric("Average Firing Rate", brain.getAverageFiringRate())
    report.add_metric("Total Spikes", brain.getTotalSpikeCount())
    report.add_metric("Final Curiosity Level", agent.getCuriosityLevel())
    report.add_metric("Final Novelty Level", agent.getNoveltyLevel())
    
    # Performance analysis
    report.add_section("Performance Analysis")
    if brain.getActivityHistory().size > 0:
        activity_stats = report.calculateStatistics(brain.getActivityHistory())
        report.add_statistic("Activity Mean", activity_stats['mean'])
        report.add_statistic("Activity Std Dev", activity_stats['std'])
        report.add_statistic("Activity Range", activity_stats['max'] - activity_stats['min'])
    
    # Behavioral analysis
    report.add_section("Behavioral Analysis")
    behavioral_stats = analyze_behavioral_patterns(agent, world)
    report.add_metric("Exploration Ratio", behavioral_stats['exploration_ratio'])
    report.add_metric("Learning Rate", behavioral_stats['learning_curves']['rate'])
    
    # Memory analysis
    report.add_section("Memory Analysis")
    if brain.getEpisodicMemory():
        memory_stats = brain.getEpisodicMemory().getStatistics()
        report.add_metric("Memory Episodes", memory_stats['episode_count'])
        report.add_metric("Memory Effectiveness", memory_stats['retrieval_accuracy'])
    
    # Save report
    report_filename = f"{experiment_name}_report.pdf"
    report.save(report_filename)
    
    # Also save as HTML for web viewing
    html_filename = f"{experiment_name}_report.html"
    report.save_html(html_filename)
    
    return report_filename
```

### 5. Machine Learning Integration

#### 5.1 Neural Network Training

**ML Features:**
- Neural activity pattern recognition
- Behavioral prediction models
- Reinforcement learning integration
- Transfer learning capabilities
- Model-based planning

```python
from sklearn.neural_network import MLPClassifier
from sklearn.preprocessing import StandardScaler
import numpy as np

def train_behavioral_predictor(agent, num_samples=1000):
    """Train a neural network to predict behavior"""
    
    # Collect training data
    X = []
    y = []
    
    for step in range(num_samples):
        # Get agent state features
        features = extract_agent_features(agent)
        X.append(features)
        
        # Get target (next action)
        target_action = predict_next_action(agent)
        y.append(target_action)
    
    X = np.array(X)
    y = np.array(y)
    
    # Scale features
    scaler = StandardScaler()
    X_scaled = scaler.fit_transform(X)
    
    # Train neural network
    model = MLPClassifier(hidden_layer_sizes=(100, 50), max_iter=1000, random_state=42)
    model.fit(X_scaled, y)
    
    # Evaluate
    accuracy = model.score(X_scaled, y)
    print(f"Model accuracy: {accuracy:.3f}")
    
    return model, scaler

def extract_agent_features(agent):
    """Extract features from agent state"""
    return [
        agent.getCuriosityLevel(),
        agent.getNoveltyLevel(),
        agent.getPredictionError(),
        agent.getNeuromodulationLevel(),
        agent.getDevelopmentalStage().value if hasattr(agent.getDevelopmentalStage(), 'value') else 0
    ]
```

#### 5.2 Reinforcement Learning

**RL Integration:**
- Q-learning from neural activity
- Policy gradient methods
- Actor-critic algorithms
- Intrinsic motivation
- Curiosity-driven exploration

```python
from stable_baselines3 import PPO
import gym

def train_rl_agent_with_nlm(neural_brain, world, env):
    """Train RL agent using NLM as feature extractor"""
    
    # Create environment wrapper
    class NLMEnvironment(gym.Env):
        def __init__(self, brain, world):
            self.brain = brain
            self.world = world
            self.action_space = gym.spaces.Discrete(6)  # Basic actions
            self.observation_space = gym.spaces.Box(low=-1, high=1, shape=(12,))
        
        def step(self, action):
            # Apply action
            action_type = list(self.action_space.n)[action]
            # Convert to MotorCommand
            cmd = convert_action_type_to_motor_command(action_type)
            
            # Update world
            self.world.applyMotorCommand(cmd, self.world.getSimulationTime())
            
            # Get next state
            percept = self.world.getSensoryPercept()
            self.brain.receiveSensoryInput(percept)
            
            # Extract features from brain
            state = extract_brain_features(self.brain)
            
            # Get reward
            reward = compute_environment_reward(self.world, percept)
            
            # Check termination
            done = self.world.getSimulationTime() > 1000.0
            
            return state, reward, done, {}
        
        def reset(self):
            self.world.reset()
            return extract_brain_features(self.brain)
    
    # Create RL model
    model = PPO('MlpPolicy', env, verbose=1, tensorboard_log="./nlm_tensorboard/")
    
    # Train model
    model.learn(total_timesteps=10000)
    
    return model
```

### 6. Advanced Visualization Types

#### 6.1 3D Brain Visualization

**3D Features:**
- Volume rendering of neural activity
- Surface reconstruction
- Interactive 3D navigation
- Multi-region visualization
- Dynamic connectivity visualization

```python
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import numpy as np

def visualize_3d_brain(brain):
    """Create 3D visualization of brain activity"""
    
    fig = plt.figure(figsize=(12, 8))
    ax = fig.add_subplot(111, projection='3d')
    
    # Get brain regions
    regions = brain.getRegions()
    
    for i, region in enumerate(regions):
        # Get neurons in region
        neurons = region->getNeurons()
        
        # Extract 3D positions (simplified)
        x = np.random.randn(len(neurons)) * 10
        y = np.random.randn(len(neurons)) * 10
        z = np.random.randn(len(neurons)) * 10
        
        # Get activity levels
        activities = np.array([n->getState().membranePotential for n in neurons])
        
        # Normalize activities
        if activities.max() > activities.min():
            activities = (activities - activities.min()) / (activities.max() - activities.min())
        
        # Plot neurons
        scatter = ax.scatter(x, y, z, c=activities, 
                           cmap='hot', alpha=0.6, s=activities*1000)
        
        # Add region label
        ax.text(x.mean(), y.mean(), z.mean(), 
               f"Region {i}", fontsize=10)
    
    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')
    ax.set_title('3D Brain Activity Visualization')
    
    plt.colorbar(scatter, ax=ax, label='Activity Level')
    plt.show()
    
    return fig
```

#### 6.2 Neural Network Diagrams

**Network Visualization:**
- Synaptic connectivity visualization
- Neural population mapping
- Activity propagation diagrams
- Plasticity event visualization
- Dynamic network evolution

```python
import networkx as nx
import matplotlib.pyplot as plt

def visualize_neural_network(brain):
    """Create interactive neural network visualization"""
    
    G = nx.Graph()
    
    # Add neurons as nodes
    regions = brain.getRegions()
    for region_idx, region in enumerate(regions):
        populations = region->getPopulations()
        
        for pop_idx, pop in enumerate(populations):
            neurons = pop->getNeurons()
            
            for neuron_idx, neuron in enumerate(neurons):
                node_id = f"neuron_{region_idx}_{pop_idx}_{neuron_idx}"
                G.add_node(node_id)
                
                # Add neuron attributes
                neuron_type = pop->getNeuronType()
                G.nodes[node_id]['type'] = neuron_type
                G.nodes[node_id]['region'] = region_idx
                G.nodes[node_id]['population'] = pop_idx
    
    # Add synapses as edges
    synapses = brain.getSynapses()
    for synapse in synapses:
        pre_neuron = synapse->getPreNeuron()
        post_neuron = synapse->getPostNeuron()
        
        # Find neuron IDs
        pre_id = find_neuron_id(brain, pre_neuron)
        post_id = find_neuron_id(brain, post_neuron)
        
        if pre_id and post_id:
            G.add_edge(pre_id, post_id)
    
    # Create layout
    pos = nx.spring_layout(G, k=1, iterations=50)
    
    # Draw network
    fig, ax = plt.subplots(figsize=(15, 10))
    
    # Draw nodes by type
    neuron_types = [G.nodes[node]['type'] for node in G.nodes()]
    colors = {'excitatory': 'red', 'inhibitory': 'blue', 
              'sensory': 'green', 'motor': 'orange', 
              'modulatory': 'purple', 'internal': 'gray'}
    
    node_colors = [colors.get(str(t), 'black') for t in neuron_types]
    
    nx.draw_networkx_nodes(G, pos, node_color=node_colors, 
                          node_size=100, alpha=0.7)
    
    nx.draw_networkx_edges(G, pos, alpha=0.3, edge_color='gray')
    nx.draw_networkx_labels(G, pos, font_size=8, alpha=0.5)
    
    ax.set_title("Neural Network Connectivity Visualization")
    ax.axis('off')
    
    plt.show()
    
    return fig, G
```

### 7. Performance and Memory Optimization

#### 7.1 Efficient Data Structures

**Optimization Techniques:**
- Sparse matrix representations for connectivity
- Compressed sparse row formats
- Memory-mapped files
- Memory pooling
- Just-in-time compilation

```python
import numpy as np
import scipy.sparse as sp
from numba import jit

class OptimizedNeuralData:
    """Optimized data structures for neural simulation"""
    
    def __init__(self, neuron_count, max_synapses_per_neuron=10):
        self.neuron_count = neuron_count
        self.max_synapses_per_neuron = max_synapses_per_neuron
        
        # Sparse connectivity matrix
        self.connectivity = sp.lil_matrix((neuron_count, neuron_count))
        
        # Optimized activity arrays
        self.activity_buffer = np.zeros(neuron_count, dtype=np.float32)
        self.spike_times = []
        
        # Memory-efficient storage
        self.spike_matrix = sp.csr_matrix((neuron_count, 1000), dtype=np.uint8)
    
    @jit(nopython=True)
    def update_neural_dynamics(self, time_step, dt):
        """Optimized neural dynamics update"""
        # Numba-optimized dynamics update
        for i in range(self.neuron_count):
            # Simple LIF dynamics
            self.activity_buffer[i] += dt * (-self.activity_buffer[i] + np.random.randn())
            
            if self.activity_buffer[i] > 1.0:  # Threshold
                self.spike_times.append(time_step)
                self.activity_buffer[i] = 0.0
    
    def add_synapse(self, pre_neuron, post_neuron, weight):
        """Add synaptic connection"""
        self.connectivity[pre_neuron, post_neuron] = weight
    
    def get_connected_neurons(self, neuron_id):
        """Get all neurons connected to given neuron"""
        row = self.connectivity.getrow(neuron_id)
        return np.where(row.toarray()[0] != 0)[1][0]
```

#### 7.2 Performance Monitoring

**Monitoring Features:**
- CPU and memory usage tracking
- Simulation performance metrics
- I/O performance analysis
- Network latency measurement
- Resource utilization tracking

```python
import psutil
import time
from dataclasses import dataclass
from typing import List

@dataclass
class PerformanceMetrics:
    cpu_usage: float
    memory_usage: float
    simulation_time: float
    wall_time_per_step: float
    steps_completed: int
    iops: float
    network_latency: float
    custom_metrics: dict

class PerformanceProfiler:
    """Comprehensive performance profiling"""
    
    def __init__(self):
        self.start_time = time.time()
        self.step_times = []
        self.metrics_history = []
    
    def record_step(self, step_time):
        """Record step performance"""
        self.step_times.append(step_time)
        
        # Get system metrics
        process = psutil.Process()
        cpu_percent = process.cpu_percent()
        memory_info = process.memory_info()
        
        metrics = PerformanceMetrics(
            cpu_usage=cpu_percent,
            memory_usage=memory_info.rss / 1024 / 1024,  # MB
            simulation_time=time.time() - self.start_time,
            wall_time_per_step=step_time,
            steps_completed=len(self.step_times),
            iops=self.calculate_iops(),
            network_latency=self.get_network_latency(),
            custom_metrics={}
        )
        
        self.metrics_history.append(metrics)
    
    def calculate_iops(self):
        """Calculate I/O operations per second"""
        # Implementation would depend on specific I/O operations
        return 0.0
    
    def get_network_latency(self):
        """Get network latency metrics"""
        # Implementation would measure network performance
        return 0.0
    
    def get_summary(self):
        """Get performance summary"""
        if not self.step_times:
            return None
        
        avg_step_time = np.mean(self.step_times)
        std_step_time = np.std(self.step_times)
        steps_per_second = 1.0 / avg_step_time
        
        return {
            'average_step_time_ms': avg_step_time * 1000,
            'step_time_std_ms': std_step_time * 1000,
            'steps_per_second': steps_per_second,
            'total_simulation_time': self.metrics_history[-1].simulation_time,
            'peak_cpu_usage': max(m.cpu_usage for m in self.metrics_history),
            'peak_memory_usage': max(m.memory_usage for m in self.metrics_history),
            'total_steps': len(self.step_times)
        }
```

### 8. Integration and Deployment

#### 8.1 Production Deployment

**Deployment Features:**
- Containerized deployment (Docker)
- Orchestrated deployment (Kubernetes)
- Cloud deployment options
- Configuration management
- Monitoring and alerting

```yaml
# Docker Compose for NLM deployment
services:
  nlm_simulation:
    build: ./docker
    ports:
      - "8080:8080"
    environment:
      - SIMULATION_CONFIG=/app/config/production.yaml
      - MONITORING_ENABLED=true
    volumes:
      - ./data:/app/data
      - ./logs:/app/logs
    restart: unless-stopped
    
  nlm_visualization:
    build: ./visualization
    ports:
      - "3000:3000"
    depends_on:
      - nlm_simulation
    restart: unless-stopped

  nlm_monitoring:
    image: prom/prometheus
    ports:
      - "9090:9090"
    volumes:
      - ./monitoring/prometheus.yml:/etc/prometheus/prometheus.yml
      - ./monitoring/data:/prometheus
    restart: unless-stopped
```

#### 8.2 API Integration

**API Features:**
- RESTful API for simulation control
- GraphQL for complex queries
- WebSocket for real-time updates
- Authentication and authorization
- Rate limiting and throttling

```python
from fastapi import FastAPI, WebSocket
from fastapi.middleware.cors import CORSMiddleware
import json

app = FastAPI(title="NLM API")

# Enable CORS
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# Simulation control API
@app.post("/api/simulations")
async def create_simulation(config: SimulationConfig):
    """Create and start a new simulation"""
    simulation = await simulation_manager.create_simulation(config)
    return {"simulation_id": simulation.id, "status": "running"}

@app.get("/api/simulations/{simulation_id}")
async def get_simulation_status(simulation_id: str):
    """Get simulation status"""
    simulation = await simulation_manager.get_simulation(simulation_id)
    return {
        "id": simulation.id,
        "status": simulation.status,
        "progress": simulation.progress,
        "metrics": simulation.metrics
    }

@app.websocket("/api/simulations/{simulation_id}/stream")
async def websocket_endpoint(websocket: WebSocket, simulation_id: str):
    """WebSocket endpoint for real-time updates"""
    await websocket.accept()
    
    simulation = await simulation_manager.get_simulation(simulation_id)
    
    try:
        while True:
            # Send real-time updates
            update = await simulation.get_next_update()
            await websocket.send_json(update)
            
            # Wait for next update
            await asyncio.sleep(0.1)
    except websockets.exceptions.ConnectionClosed:
        pass
    finally:
        await simulation_manager.cleanup_simulation(simulation_id)
```

### 9. Development and Testing

#### 9.1 Continuous Integration

**CI/CD Pipeline:**
- Automated testing
- Performance regression detection
- Integration testing
- Security scanning
- Documentation validation

```yaml
# GitHub Actions workflow
name: NLM CI/CD

on:
  push:
    branches: [main, develop]
  pull_request:
    branches: [main]

jobs:
  test:
    runs-on: ubuntu-latest
    strategy:
      matrix:
        python-version: [3.8, 3.9, 3.10, 3.11]
    
    steps:
    - uses: actions/checkout@v3
    - name: Set up Python ${{ matrix.python-version }}
      uses: actions/setup-python@v3
      with:
        python-version: ${{ matrix.python-version }}
    
    - name: Install dependencies
      run: |
        pip install -r requirements.txt
        pip install -r requirements-dev.txt
    
    - name: Run tests
      run: |
        pytest tests/ -v --cov=src --cov-report=xml
    
    - name: Run performance tests
      run: |
        python tests/test_performance.py
    
    - name: Upload coverage
      uses: codecov/codecov-action@v3
      if: matrix.python-version == '3.10'
  
  build:
    needs: test
    runs-on: ubuntu-latest
    if: github.ref == 'refs/heads/main'
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Build Docker image
      run: |
        docker build -t nlm:latest .
        docker push your-registry/nlm:latest
```

#### 9.2 Automated Testing

**Test Framework:**
- Unit tests for all components
- Integration tests for system interactions
- Performance regression tests
- Visualization tests
- Machine learning integration tests

```python
import pytest
import asyncio
import aiohttp
from unittest.mock import patch, MagicMock

class TestEnhancedVisualization:
    """Test enhanced visualization features"""
    
    @pytest.fixture
    def visualizer(self):
        return NeuralVisualizer()
    
    def test_3d_visualization_setup(self, visualizer):
        """Test 3D visualization setup"""
        brain = MagicMock()
        visualizer.setup3DBrain(brain)
        
        assert visualizer.is3DEnabled()
        assert visualizer.renderMode == "volume_rendering"
    
    def test_data_export(self, visualizer):
        """Test data export functionality"""
        brain = MagicMock()
        visualizer.setupBrain(brain)
        
        # Mock data
        visualizer.setData([1, 2, 3, 4, 5])
        
        # Test CSV export
        csv_data = visualizer.export_csv()
        assert "data" in csv_data
        assert len(csv_data["data"]) == 5
    
    @pytest.mark.asyncio
    async def test_real_time_streaming(self, visualizer):
        """Test real-time streaming functionality"""
        # Mock WebSocket connection
        with patch('websockets.serve') as mock_serve:
            visualizer.start_streaming()
            
            # Verify streaming started
            assert visualizer.is_streaming()
            
            # Simulate WebSocket connection
            mock_connection = MagicMock()
            mock_serve.return_value = mock_connection
            
            # Test connection handling
            visualizer.handle_connection(mock_connection)
            
            assert mock_connection.send.called

class TestPerformanceMonitoring:
    """Test performance monitoring features"""
    
    @pytest.fixture
    def profiler(self):
        return PerformanceProfiler()
    
    def test_step_recording(self, profiler):
        """Test step recording"""
        profiler.record_step(0.1)
        profiler.record_step(0.05)
        
        assert len(profiler.step_times) == 2
        assert profiler.step_times[0] == 0.1
        assert profiler.step_times[1] == 0.05
    
    def test_performance_summary(self, profiler):
        """Test performance summary generation"""
        profiler.record_step(0.1)
        profiler.record_step(0.2)
        profiler.record_step(0.15)
        
        summary = profiler.get_summary()
        
        assert summary['average_step_time_ms'] == (0.1 + 0.2 + 0.15) * 1000 / 3
        assert summary['steps_per_second'] == 3 / (0.1 + 0.2 + 0.15)
        assert summary['total_steps'] == 3

if __name__ == "__main__":
    pytest.main([__file__, "-v"])
```

## Conclusion

The enhanced NLM visualization and analysis tools provide comprehensive capabilities for:

1. **Visual Representation**: 2D and 3D visualization of neural activity and connectivity
2. **Data Analysis**: Comprehensive analysis of neural, behavioral, and memory systems
3. **Performance Optimization**: Efficient data structures and performance monitoring
4. **Integration**: Machine learning, API integration, and deployment capabilities
5. **Development Tools**: Testing, CI/CD, and debugging utilities

These tools enable researchers and developers to:

- Visualize complex neural interactions
- Analyze simulation results quantitatively
- Optimize performance for large-scale simulations
- Integrate NLM with other systems
- Deploy and maintain production systems

The visualization and analysis enhancements significantly expand the capabilities of NLM for advanced neural simulation research and applications.
