import pynlm
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from mpl_toolkits.mplot3d import Axes3D
import scipy.signal as signal
import scipy.optimize as optimize
import pandas as pd
import seaborn as sns
from sklearn.model_selection import cross_val_score
from sklearn.linear_model import LinearRegression
from sklearn.ensemble import RandomForestRegressor
import time
import os
import json
from typing import List, Optional, Dict, Any, Tuple
import warnings

# Set default plotting style
plt.style.use('seaborn-v0_8')
sns.set_style("whitegrid")

warnings.filterwarnings('ignore')

__version__ = "0.1.0"

class HighLevelAPIError(Exception):
    """Exception raised for errors in the NLM high-level API."""
    pass

class BrainSimulator:
    """
    High-level interface for NLM brain simulation with preset configurations
    and advanced utilities.
    """
    
    # Preset configurations
    PRESET_CONFIGS = {
        'phase2': {
            'brain.neuron_count': 500,
            'brain.synapse_density': 0.05,
            'plasticity.stdp.enable': True,
            'plasticity.hebbian.enable': True,
            'neuromod.dopamine.scale': 1.0,
            'neuromod.curiosity.enable': True,
            'neuromod.novelty.enable': True,
        },
        'phase3': {
            'brain.neuron_count': 1000,
            'brain.synapse_density': 0.1,
            'brain.initial_weight_mean': 0.5,
            'brain.initial_weight_std': 0.1,
            'plasticity.stdp.enable': True,
            'plasticity.stdp.learning_rate': 0.001,
            'plasticity.hebbian.enable': True,
            'plasticity.structural.enable': True,
            'neuromod.dopamine.scale': 1.0,
            'neuromod.curiosity.enable': True,
            'neuromod.novelty.enable': True,
            'neuromod.prediction_error.scale': 1.0,
            'cognition.planner.enable': True,
            'cognition.attention.enable': True,
        },
        'phase4': {
            'brain.neuron_count': 2000,
            'brain.synapse_density': 0.15,
            'brain.initial_weight_mean': 0.5,
            'brain.initial_weight_std': 0.1,
            'plasticity.stdp.enable': True,
            'plasticity.stdp.learning_rate': 0.001,
            'plasticity.hebbian.enable': True,
            'plasticity.structural.enable': True,
            'neuromod.dopamine.scale': 1.0,
            'neuromod.curiosity.enable': True,
            'neuromod.novelty.enable': True,
            'neuromod.prediction_error.scale': 1.0,
            'cognition.planner.enable': True,
            'cognition.attention.enable': True,
            'cognition.concept.enable': True,
            'memory.working.enable': True,
            'memory.episodic.enable': True,
            'memory.associative.enable': True,
            'prediction.enabled': True,
        },
        'phase5': {
            'brain.neuron_count': 5000,
            'brain.synapse_density': 0.2,
            'brain.initial_weight_mean': 0.5,
            'brain.initial_weight_std': 0.1,
            'plasticity.stdp.enable': True,
            'plasticity.stdp.learning_rate': 0.001,
            'plasticity.hebbian.enable': True,
            'plasticity.structural.enable': True,
            'neuromod.dopamine.scale': 1.0,
            'neuromod.curiosity.enable': True,
            'neuromod.novelty.enable': True,
            'neuromod.prediction_error.scale': 1.0,
            'cognition.planner.enable': True,
            'cognition.attention.enable': True,
            'cognition.concept.enable': True,
            'memory.working.enable': True,
            'memory.episodic.enable': True,
            'memory.associative.enable': True,
            'prediction.enabled': True,
            'optimization.enabled': True,
            'performance.monitoring': True,
        },
        'phase6': {
            'brain.neuron_count': 10000,
            'brain.synapse_density': 0.25,
            'brain.initial_weight_mean': 0.5,
            'brain.initial_weight_std': 0.1,
            'plasticity.stdp.enable': True,
            'plasticity.stdp.learning_rate': 0.001,
            'plasticity.hebbian.enable': True,
            'plasticity.structural.enable': True,
            'neuromod.dopamine.scale': 1.0,
            'neuromod.curiosity.enable': True,
            'neuromod.novelty.enable': True,
            'neuromod.prediction_error.scale': 1.0,
            'cognition.planner.enable': True,
            'cognition.attention.enable': True,
            'cognition.concept.enable': True,
            'memory.working.enable': True,
            'memory.episodic.enable': True,
            'memory.associative.enable': True,
            'prediction.enabled': True,
            'optimization.enabled': True,
            'performance.monitoring': True,
            'visualization.enabled': True,
            'benchmarking.enabled': True,
        }
    }
    
    def __init__(self, preset='phase3'):
        """
        Initialize a BrainSimulator with a preset configuration.
        
        Args:
            preset (str): Preset configuration name ('phase2', 'phase3', 'phase4', 'phase5', 'phase6')
        """
        if preset not in self.PRESET_CONFIGS:
            raise HighLevelAPIError(f"Unknown preset: {preset}. Available presets: {list(self.PRESET_CONFIGS.keys())}")
        
        self.preset = preset
        config = pynlm.createDefaultConfig()
        
        # Apply preset configuration
        for key, value in self.PRESET_CONFIGS[preset].items():
            if isinstance(value, bool):
                config.set(key, value)
            elif isinstance(value, int):
                config.set(key, value)
            elif isinstance(value, float):
                config.set(key, value)
            elif isinstance(value, str):
                config.set(key, value)
            else:
                config.set(key, value)
        
        self.brain = pynlm.createBrain(config)
        self.config = config
        self._initialized = False
    
    def initialize(self):
        """Initialize the brain."""
        if not self._initialized:
            self.brain.initialize()
            self._initialized = True
    
    def step(self, step_num):
        """Perform one simulation step."""
        if not self._initialized:
            self.initialize()
        self.brain.step(step_num)
    
    def run_learning_experiment(self, environment, duration=1000, learning_rate=0.01):
        """
        Run a learning experiment in the given environment.
        
        Args:
            environment: World instance to interact with
            duration (int): Number of simulation steps
            learning_rate (float): Learning rate for neural updates
            
        Returns:
            dict: Results containing metrics over time
        """
        if not self._initialized:
            self.initialize()
        
        results = {
            'step': [],
            'firing_rate': [],
            'curiosity': [],
            'novelty': [],
            'reward': [],
            'action_selected': [],
            'developmental_stage': []
        }
        
        for step in range(duration):
            # Get sensory input from environment
            percept = environment.getSensoryPercept()
            
            # Process sensory input
            if hasattr(environment, 'processSensoryInput'):
                environment.processSensoryInput(percept)
            
            # Run brain step
            self.brain.step(step)
            
            # Get action
            action = self.brain.produceAction()
            
            # Apply action to environment
            result = environment.applyAction(action)
            
            # Record metrics
            results['step'].append(step)
            results['firing_rate'].append(self.brain.getFiringNeuronCount())
            results['curiosity'].append(self.brain.getCuriosityLevel())
            results['novelty'].append(self.brain.getNoveltyLevel())
            results['reward'].append(result.get('reward', 0.0))
            results['action_selected'].append(action.getType() if hasattr(action, 'getType') else str(action))
            results['developmental_stage'].append(self.brain.getDevelopmentalStage())
            
            # Update learning rate
            if learning_rate > 0:
                self.config.set('learning.rate', learning_rate)
        
        return results
    
    def train_agent(self, world, episodes=100, max_steps_per_episode=100, exploration_rate=0.1):
        """
        Train an agent in the world using reinforcement learning.
        
        Args:
            world: World instance for training
            episodes (int): Number of training episodes
            max_steps_per_episode (int): Maximum steps per episode
            exploration_rate (float): Probability of random exploration
            
        Returns:
            dict: Training results and metrics
        """
        training_results = {
            'episode': [],
            'total_reward': [],
            'steps_taken': [],
            'final_curiosity': [],
            'final_novelty': [],
            'developmental_stage': []
        }
        
        for episode in range(episodes):
            world.reset()
            total_reward = 0.0
            steps_taken = 0
            
            for step in range(max_steps_per_episode):
                # Get current state from world
                percept = world.getSensoryPercept()
                
                # Process sensory input with curiosity-driven exploration
                if np.random.random() < exploration_rate:
                    # Random action for exploration
                    random_action = self._get_random_action()
                    action_result = world.applyAction(random_action)
                    reward = action_result.get('reward', 0.0)
                else:
                    # Deliberate action based on brain activity
                    action = self.brain.produceAction()
                    action_result = world.applyAction(action)
                    reward = action_result.get('reward', 0.0)
                
                total_reward += reward
                steps_taken += 1
                
                # Update brain with reward
                if hasattr(self.brain, 'applyRewardModulation'):
                    self.brain.applyRewardModulation(reward, 0.0)
                
                # End episode if world signals termination
                if world.is_episode_complete():
                    break
            
            # Record episode results
            training_results['episode'].append(episode)
            training_results['total_reward'].append(total_reward)
            training_results['steps_taken'].append(steps_taken)
            training_results['final_curiosity'].append(self.brain.getCuriosityLevel())
            training_results['final_novelty'].append(self.brain.getNoveltyLevel())
            training_results['developmental_stage'].append(self.brain.getDevelopmentalStage())
        
        return training_results
    
    def _get_random_action(self):
        """Generate a random action."""
        action_types = [
            pynlm.ActionType.MoveForward,
            pynlm.ActionType.TurnLeft,
            pynlm.ActionType.TurnRight,
            pynlm.ActionType.Interact,
            pynlm.ActionType.Wait
        ]
        
        import random
        action_type = random.choice(action_types)
        return pynlm.Action(action_type)
    
    def benchmark_performance(self, duration=1000, metrics=None):
        """
        Run performance benchmarks on the brain.
        
        Args:
            duration (int): Number of steps to benchmark
            metrics (list): List of metrics to measure
            
        Returns:
            dict: Performance metrics
        """
        if metrics is None:
            metrics = [
                'neuron_count',
                'synapse_count',
                'firing_rate',
                'memory_usage',
                'computation_time',
                'learning_efficiency'
            ]
        
        if not self._initialized:
            self.initialize()
        
        benchmark_results = {}
        
        if 'neuron_count' in metrics:
            benchmark_results['neuron_count'] = self.brain.getTotalNeuronCount()
        
        if 'synapse_count' in metrics:
            benchmark_results['synapse_count'] = self.brain.getTotalSynapseCount()
        
        if 'firing_rate' in metrics:
            benchmark_results['firing_rate'] = []
            for step in range(duration):
                self.brain.step(step)
                benchmark_results['firing_rate'].append(self.brain.getFiringNeuronCount())
        
        if 'computation_time' in metrics:
            benchmark_results['computation_time'] = []
            for step in range(duration):
                start_time = time.time()
                self.brain.step(step)
                end_time = time.time()
                benchmark_results['computation_time'].append(end_time - start_time)
        
        if 'memory_usage' in metrics:
            import psutil
            process = psutil.Process()
            benchmark_results['memory_usage'] = process.memory_info().rss / 1024 / 1024  # MB
        
        if 'learning_efficiency' in metrics:
            benchmark_results['learning_efficiency'] = (
                sum(benchmark_results.get('firing_rate', [0])) / 
                len(benchmark_results.get('firing_rate', [1]))
            )
        
        return benchmark_results


def create_brain_with_preset(preset='phase3'):
    """
    Create a brain with a predefined configuration.
    
    Args:
        preset (str): Preset configuration name ('phase2', 'phase3', 'phase4', 'phase5', 'phase6')
    
    Returns:
        BrainSimulator: Configured brain simulator instance
    """
    return BrainSimulator(preset)


def run_learning_experiment(brain, environment, duration=1000, learning_rate=0.01):
    """
    High-level function to run learning experiments.
    
    Args:
        brain: Brain or BrainSimulator instance
        environment: World instance to interact with
        duration (int): Number of simulation steps
        learning_rate (float): Learning rate for neural updates
        
    Returns:
        dict: Results containing metrics over time
    """
    if isinstance(brain, BrainSimulator):
        return brain.run_learning_experiment(environment, duration, learning_rate)
    else:
        # Create BrainSimulator from raw brain
        simulator = BrainSimulator('phase3')
        simulator.brain = brain
        simulator._initialized = True
        return simulator.run_learning_experiment(environment, duration, learning_rate)


def train_agent(brain, world, episodes=100, max_steps_per_episode=100, exploration_rate=0.1):
    """
    Train an agent in the world.
    
    Args:
        brain: Brain or BrainSimulator instance
        world: World instance for training
        episodes (int): Number of training episodes
        max_steps_per_episode (int): Maximum steps per episode
        exploration_rate (float): Probability of random exploration
        
    Returns:
        dict: Training results and metrics
    """
    if isinstance(brain, BrainSimulator):
        return brain.train_agent(world, episodes, max_steps_per_episode, exploration_rate)
    else:
        simulator = BrainSimulator('phase3')
        simulator.brain = brain
        simulator._initialized = True
        return simulator.train_agent(world, episodes, max_steps_per_episode, exploration_rate)


def benchmark_performance(brain, duration=1000, metrics=None):
    """
    Run performance benchmarks on the brain.
    
    Args:
        brain: Brain or BrainSimulator instance
        duration (int): Number of steps to benchmark
        metrics (list): List of metrics to measure
        
    Returns:
        dict: Performance metrics
    """
    if isinstance(brain, BrainSimulator):
        return brain.benchmark_performance(duration, metrics)
    else:
        simulator = BrainSimulator('phase3')
        simulator.brain = brain
        simulator._initialized = True
        return simulator.benchmark_performance(duration, metrics)


def plot_brain_network(brain, save_path=None):
    """
    Plot neural connectivity.
    
    Args:
        brain: Brain instance
        save_path (str): Path to save the plot
        
    Returns:
        matplotlib.figure.Figure: The generated figure
    """
    import networkx as nx
    
    # Create a network graph of neural regions
    G = nx.Graph()
    
    # Add nodes for regions
    region_ids = brain.getRegionIds()
    for i, region_id in enumerate(region_ids):
        region = brain.getRegion(region_id)
        G.add_node(i, label=f"Region {region_id.value}")
    
    # Add edges based on inter-region connections
    regions = brain.getRegions()
    for region in regions:
        for connection in region.getInterRegionConnections():
            source_idx = region_ids.index(connection.sourceRegion)
            target_idx = region_ids.index(connection.targetRegion)
            G.add_edge(source_idx, target_idx, weight=connection.weight)
    
    # Plot
    fig, ax = plt.subplots(figsize=(12, 10))
    
    # Use a force-directed layout
    pos = nx.spring_layout(G, seed=42)
    
    # Draw nodes
    nx.draw_networkx_nodes(G, pos, ax=ax, node_color='lightblue', 
                          node_size=700, alpha=0.8)
    
    # Draw edges
    edge_weights = [G[u][v]['weight'] for u, v in G.edges()]
    max_weight = max(edge_weights) if edge_weights else 1
    width = [2 * w / max_weight for w in edge_weights]
    nx.draw_networkx_edges(G, pos, ax=ax, width=width, alpha=0.5, edge_color='gray')
    
    # Draw labels
    labels = {i: G.nodes[i]['label'] for i in G.nodes()}
    nx.draw_networkx_labels(G, pos, labels, ax=ax)
    
    ax.set_title("Neural Connectivity Network")
    ax.axis('off')
    
    if save_path:
        plt.savefig(save_path, dpi=300, bbox_inches='tight')
    
    return fig


def plot_activity_raster(brain, save_path=None):
    """
    Plot spike rasters.
    
    Args:
        brain: Brain instance
        save_path (str): Path to save the plot
        
    Returns:
        matplotlib.figure.Figure: The generated figure
    """
    spike_data = brain.getSpikeData()
    
    fig, ax = plt.subplots(figsize=(12, 8))
    
    # Plot spike raster
    for spike_time, neuron_id in spike_data:
        ax.plot(spike_time, neuron_id, 'k.', markersize=1)
    
    ax.set_xlabel('Time (ms)')
    ax.set_ylabel('Neuron ID')
    ax.set_title('Spike Raster Plot')
    ax.set_xlim([0, max([t for t, _ in spike_data] + [0])] if spike_data else [0, 100])
    ax.set_ylim([-1, max([n for _, n in spike_data] + [0])] if spike_data else [-1, 100])
    
    # Add firing rate histogram
    ax2 = ax.twinx()
    spike_times = [t for t, _ in spike_data]
    if spike_times:
        hist, bins = np.histogram(spike_times, bins=50)
        bin_centers = (bins[:-1] + bins[1:]) / 2
        ax2.plot(bin_centers, hist, 'r-', alpha=0.7)
        ax2.set_ylabel('Firing Rate Density')
        ax2.set_ylim([0, max(hist) * 1.1])
    
    if save_path:
        plt.savefig(save_path, dpi=300, bbox_inches='tight')
    
    return fig


def plot_weight_matrix(brain, save_path=None):
    """
    Plot synaptic weight matrix.
    
    Args:
        brain: Brain instance
        save_path (str): Path to save the plot
        
    Returns:
        matplotlib.figure.Figure: The generated figure
    """
    # Get all neurons and create a simplified weight matrix
    neurons = brain.getAllNeurons()
    if len(neurons) < 2:
        raise ValueError("Need at least 2 neurons to plot weight matrix")
    
    # Create adjacency matrix
    n = len(neurons)
    weight_matrix = np.zeros((n, n))
    
    for i, neuron1 in enumerate(neurons):
        for j, neuron2 in enumerate(neurons):
            if i != j:
                # Get synaptic weight from neuron1 to neuron2
                synapse = brain.getSynapseBetween(neuron1, neuron2)
                if synapse:
                    weight_matrix[i, j] = synapse.getWeight()
    
    fig, axes = plt.subplots(1, 3, figsize=(18, 6))
    
    # Plot full weight matrix
    im1 = axes[0].imshow(weight_matrix, cmap='RdBu_r', vmin=-1, vmax=1)
    axes[0].set_title('Full Weight Matrix')
    plt.colorbar(im1, ax=axes[0])
    
    # Plot upper triangular part (positive weights only)
    upper_tri = np.triu(weight_matrix, k=1)
    im2 = axes[1].imshow(upper_tri, cmap='Blues', vmin=0, vmax=upper_tri.max())
    axes[1].set_title('Positive Connections Only')
    plt.colorbar(im2, ax=axes[1])
    
    # Plot weight distribution
    axes[2].hist(weight_matrix.flatten(), bins=50, alpha=0.7, edgecolor='black')
    axes[2].set_title('Weight Distribution')
    axes[2].set_xlabel('Weight')
    axes[2].set_ylabel('Frequency')
    
    plt.tight_layout()
    
    if save_path:
        plt.savefig(save_path, dpi=300, bbox_inches='tight')
    
    return fig


def create_video(brain, output_path, duration=10, fps=30):
    """
    Create animation of brain activity.
    
    Args:
        brain: Brain instance
        output_path (str): Path to save the video
        duration (int): Duration of video in seconds
        fps (int): Frames per second
        
    Returns:
        matplotlib.animation.Animation: The animation object
    """
    fig, ax = plt.subplots(figsize=(12, 8))
    
    # Get data for animation
    spike_data = brain.getSpikeData()
    time_steps = np.arange(0, duration * 1000, 1000 / fps)  # Convert to ms
    
    # Initialize plot elements
    scatter = ax.scatter([], [], s=10, c='red', alpha=0.7)
    
    # Set plot limits
    neuron_ids = [n for _, n in spike_data] if spike_data else []
    max_neuron = max(neuron_ids) if neuron_ids else 100
    ax.set_xlim([0, duration * 1000])
    ax.set_ylim([-1, max_neuron])
    ax.set_xlabel('Time (ms)')
    ax.set_ylabel('Neuron ID')
    ax.set_title('Brain Activity Animation')
    
    def update(frame):
        # Get spikes for this time frame
        current_spikes = [(t, n) for t, n in spike_data if abs(t - frame) < 1]
        if current_spikes:
            times, neuron_ids = zip(*current_spikes)
            scatter.set_offsets(list(zip(times, neuron_ids)))
        return scatter,
    
    frames = list(time_steps)
    anim = animation.FuncAnimation(
        fig, update, frames=frames, interval=1000/fps, blit=True
    )
    
    # Save video
    anim.save(output_path, writer='ffmpeg', fps=fps, dpi=150)
    plt.close(fig)
    
    return anim


def train_predictive_model(brain, data, model_type='linear'):
    """
    Train prediction models.
    
    Args:
        brain: Brain instance
        data (dict): Training data
        model_type (str): Type of model ('linear', 'random_forest', 'neural')
        
    Returns:
        dict: Trained model and predictions
    """
    # Prepare data
    X = np.array(data['features'])
    y = np.array(data['targets'])
    
    if model_type == 'linear':
        model = LinearRegression()
    elif model_type == 'random_forest':
        model = RandomForestRegressor(n_estimators=100, random_state=42)
    elif model_type == 'neural':
        # For neural networks, we'll use a simple MLP
        from sklearn.neural_network import MLPRegressor
        model = MLPRegressor(hidden_layer_sizes=(100, 50), max_iter=1000, random_state=42)
    else:
        raise ValueError(f"Unknown model type: {model_type}")
    
    # Train model
    model.fit(X, y)
    
    # Make predictions
    predictions = model.predict(X)
    
    return {
        'model': model,
        'predictions': predictions,
        'r2_score': model.score(X, y)
    }


def optimize_weights(brain, optimization_method='stochastic', learning_rate=0.01, iterations=1000):
    """
    Optimize synaptic weights.
    
    Args:
        brain: Brain instance
        optimization_method (str): Optimization method
        learning_rate (float): Learning rate for optimization
        iterations (int): Number of optimization iterations
        
    Returns:
        dict: Optimization results
    """
    results = {
        'iteration': [],
        'objective_value': [],
        'learning_rate': []
    }
    
    for iteration in range(iterations):
        # Get current weights
        current_weights = brain.getAllWeights()
        
        # Compute gradient (placeholder for actual optimization logic)
        gradient = optimize_gradients(current_weights)
        
        # Update weights
        if optimization_method == 'stochastic':
            new_weights = [w - learning_rate * g for w, g in zip(current_weights, gradient)]
        elif optimization_method == 'adam':
            new_weights = adam_update(current_weights, gradient, learning_rate, iteration)
        else:
            new_weights = [w - learning_rate * g for w, g in zip(current_weights, gradient)]
        
        # Apply weights to brain
        brain.setAllWeights(new_weights)
        
        # Record results
        results['iteration'].append(iteration)
        results['objective_value'].append(compute_objective(current_weights))
        results['learning_rate'].append(learning_rate)
    
    return results


def optimize_gradients(weights):
    """Compute gradients for weight optimization (placeholder)."""
    # Placeholder for actual gradient computation
    return np.random.randn(*weights.shape) * 0.01


def compute_objective(weights):
    """Compute objective function for weight optimization (placeholder)."""
    # Placeholder for actual objective function
    return np.sum(weights**2) * 0.5


def adam_update(weights, gradient, learning_rate, iteration, beta1=0.9, beta2=0.999, epsilon=1e-8):
    """Adam optimizer update (placeholder)."""
    # Placeholder for Adam optimizer
    m = np.zeros_like(gradient)
    v = np.zeros_like(gradient)
    
    m = beta1 * m + (1 - beta1) * gradient
    v = beta2 * v + (1 - beta2) * (gradient ** 2)
    
    m_hat = m / (1 - beta1 ** (iteration + 1))
    v_hat = v / (1 - beta2 ** (iteration + 1))
    
    return weights - learning_rate * m_hat / (np.sqrt(v_hat) + epsilon)


def run_reinforcement_learning(brain, environment, episodes=1000, gamma=0.99, alpha=0.01):
    """
    Run reinforcement learning training.
    
    Args:
        brain: Brain instance
        environment: World instance
        episodes (int): Number of episodes
        gamma (float): Discount factor
        alpha (float): Learning rate
        
    Returns:
        dict: Training history and Q-values
    """
    # Q-table placeholder (for tabular RL)
    # In a real implementation, this would be replaced by neural network
    q_table = {}
    
    history = {
        'episode': [],
        'reward': [],
        'epsilon': [],
        'q_value': []
    }
    
    for episode in range(episodes):
        # Reset environment
        state = environment.reset()
        epsilon = max(0.01, 0.1 * (1 - episode / episodes))  # Decay epsilon
        
        for step in range(100):  # Max steps per episode
            # Choose action (epsilon-greedy)
            if np.random.random() < epsilon:
                action = environment.sample_action()
            else:
                # Get action from brain
                action = brain.produceAction()
            
            # Execute action and get next state and reward
            next_state, reward, done = environment.step(action)
            
            # Store transition (placeholder)
            q_table[(state, action)] = reward
            
            # Update Q-value (placeholder)
            current_q = q_table.get((state, action), 0)
            next_max_q = max([q_table.get((next_state, a), 0) for a in environment.get_possible_actions()])
            new_q = current_q + alpha * (reward + gamma * next_max_q - current_q)
            q_table[(state, action)] = new_q
            
            # Update brain with reward
            if hasattr(brain, 'applyRewardModulation'):
                brain.applyRewardModulation(reward, 0.0)
            
            # Record history
            history['episode'].append(episode)
            history['reward'].append(reward)
            history['epsilon'].append(epsilon)
            history['q_value'].append(new_q)
            
            state = next_state
            if done:
                break
    
    return history


def cross_validate(model, X, y, cv=5):
    """
    Cross-validation for models.
    
    Args:
        model: Model to validate
        X: Features
        y: Targets
        cv (int): Number of cross-validation folds
        
    Returns:
        dict: Cross-validation results
    """
    scores = cross_val_score(model, X, y, cv=cv, scoring='r2')
    
    return {
        'cv_scores': scores,
        'mean_score': np.mean(scores),
        'std_score': np.std(scores),
        'min_score': np.min(scores),
        'max_score': np.max(scores)
    }


def monitor_performance(brain, duration=1000, sampling_rate=100):
    """
    Monitor system performance.
    
    Args:
        brain: Brain instance
        duration (int): Number of steps to monitor
        sampling_rate (int): Steps between measurements
        
    Returns:
        dict: Performance monitoring data
    """
    monitoring_data = {
        'time': [],
        'neuron_count': [],
        'synapse_count': [],
        'firing_rate': [],
        'memory_usage': [],
        'cpu_usage': [],
        'computation_time': []
    }
    
    for step in range(0, duration, sampling_rate):
        start_time = time.time()
        brain.step(step)
        end_time = time.time()
        
        monitoring_data['time'].append(step)
        monitoring_data['neuron_count'].append(brain.getTotalNeuronCount())
        monitoring_data['synapse_count'].append(brain.getTotalSynapseCount())
        monitoring_data['firing_rate'].append(brain.getFiringNeuronCount())
        
        # Get memory usage
        import psutil
        process = psutil.Process()
        monitoring_data['memory_usage'].append(process.memory_info().rss / 1024 / 1024)
        
        # Get CPU usage
        monitoring_data['cpu_usage'].append(process.cpu_percent())
        
        # Get computation time
        monitoring_data['computation_time'].append(end_time - start_time)
    
    return monitoring_data


def measure_compatibility(system1, system2):
    """
    Measure compatibility with different systems.
    
    Args:
        system1: First system (brain or agent)
        system2: Second system (brain or agent)
        
    Returns:
        dict: Compatibility metrics
    """
    # Placeholder for actual compatibility measurement
    # This could measure shared interfaces, API compatibility, etc.
    
    compatibility_metrics = {
        'interface_compatibility': 0.95,
        'data_compatibility': 0.90,
        'performance_compatibility': 0.85,
        'protocol_compatibility': 0.92
    }
    
    return compatibility_metrics


def profile_activity(brain, duration=100, profile_type='spiking'):
    """
    Profile neural activity.
    
    Args:
        brain: Brain instance
        duration (int): Duration of profiling
        profile_type (str): Type of profiling ('spiking', 'plasticity', 'development')
        
    Returns:
        dict: Activity profile
    """
    profile_data = {
        'timestamp': [],
        'spike_count': [],
        'firing_rate': [],
        'plasticity_changes': [],
        'development_stage': []
    }
    
    for step in range(duration):
        brain.step(step)
        
        profile_data['timestamp'].append(step)
        profile_data['spike_count'].append(brain.getTotalSpikeCount())
        profile_data['firing_rate'].append(bbrain.getFiringNeuronCount())
        
        # Get plasticity changes (placeholder)
        profile_data['plasticity_changes'].append(0.01)  # Placeholder
        
        # Get developmental stage
        profile_data['development_stage'].append(brain.getDevelopmentalStage())
    
    return profile_data


def export_for_debugging(brain, output_path):
    """
    Export brain state for debugging.
    
    Args:
        brain: Brain instance
        output_path (str): Path to save debug data
        
    Returns:
        str: Path to saved debug file
    """
    debug_data = {
        'brain_state': brain.getBrainState(),  # This should be implemented
        'config': brain.getConfig(),
        'timestamp': time.time(),
        'version': __version__
    }
    
    # Save to JSON file
    with open(output_path, 'w') as f:
        json.dump(debug_data, f, indent=2, default=str)
    
    return output_path


def import_from_checkpoint(filepath):
    """
    Import saved brain states.
    
    Args:
        filepath (str): Path to checkpoint file
        
    Returns:
        object: Loaded brain state
    """
    # Load checkpoint data
    with open(filepath, 'r') as f:
        checkpoint_data = json.load(f)
    
    # Reconstruct brain (placeholder)
    # In a real implementation, this would restore the brain state
    
    return checkpoint_data


def validate_configuration(config):
    """
    Validate configuration files.
    
    Args:
        config: Configuration object
        
    Returns:
        dict: Validation results
    """
    validation_results = {
        'is_valid': True,
        'errors': [],
        'warnings': [],
        'missing_required_fields': []
    }
    
    # Check for required fields
    required_fields = ['brain.neuron_count', 'brain.synapse_density']
    for field in required_fields:
        if not config.has(field):
            validation_results['missing_required_fields'].append(field)
            validation_results['is_valid'] = False
    
    # Check field types
    if config.has('brain.neuron_count'):
        try:
            count = config.get('brain.neuron_count', int)
            if count <= 0:
                validation_results['warnings'].append('Neuron count should be positive')
        except:
            validation_results['errors'].append('Invalid neuron count type')
            validation_results['is_valid'] = False
    
    return validation_results


def generate_config_template(output_path=None):
    """
    Generate config templates.
    
    Args:
        output_path (str): Path to save template
        
    Returns:
        dict: Configuration template
    """
    config_template = {
        'brain': {
            'neuron_count': 1000,
            'synapse_density': 0.1,
            'connection_probability': 0.05,
            'initial_weight_mean': 0.5,
            'initial_weight_std': 0.1,
            'v_thresh': -50.0,
            'v_rest': -70.0,
            'v_reset': -75.0,
            'tau_mem': 20.0,
            'tau_ref': 2.0
        },
        'plasticity': {
            'stdp': {
                'enable': True,
                'learning_rate': 0.001,
                'tau_plus': 20.0,
                'tau_minus': 20.0
            },
            'hebbian': {
                'enable': True,
                'learning_rate': 0.001
            },
            'structural': {
                'enable': True,
                'growth_rate': 0.001,
                'pruning_rate': 0.0001
            }
        },
        'neuromod': {
            'dopamine': {
                'scale': 1.0
            },
            'curiosity': {
                'enable': True,
                'scale': 1.0
            },
            'novelty': {
                'enable': True,
                'threshold': 0.5
            },
            'prediction_error': {
                'scale': 1.0
            }
        },
        'cognition': {
            'planner': {
                'enable': True,
                'planning_horizon': 10
            },
            'attention': {
                'enable': True,
                'selection_strength': 1.0
            },
            'concept': {
                'enable': True,
                'formation_rate': 0.01
            }
        },
        'memory': {
            'working': {
                'enable': True,
                'capacity': 100
            },
            'episodic': {
                'enable': True,
                'replay_rate': 0.01
            },
            'associative': {
                'enable': True,
                'association_strength': 0.5
            }
        },
        'prediction': {
            'enabled': True,
            'prediction_horizon': 5
        }
    }
    
    if output_path:
        with open(output_path, 'w') as f:
            json.dump(config_template, f, indent=2)
    
    return config_template


# Create a version file for the Python module
with open(os.path.join(os.path.dirname(__file__), '__init__.py'), 'w') as f:
    f.write(f'''__version__ = "{__version__}"

from .high_level_api import (
    create_brain_with_preset,
    run_learning_experiment,
    train_agent,
    benchmark_performance,
    plot_brain_network,
    plot_activity_raster,
    plot_weight_matrix,
    create_video,
    train_predictive_model,
    optimize_weights,
    run_reinforcement_learning,
    cross_validate,
    monitor_performance,
    measure_compatibility,
    profile_activity,
    export_for_debugging,
    import_from_checkpoint,
    validate_configuration,
    generate_config_template,
    BrainSimulator,
    HighLevelAPIError
)

__all__ = [
    'create_brain_with_preset',
    'run_learning_experiment',
    'train_agent',
    'benchmark_performance',
    'plot_brain_network',
    'plot_activity_raster',
    'plot_weight_matrix',
    'create_video',
    'train_predictive_model',
    'optimize_weights',
    'run_reinforcement_learning',
    'cross_validate',
    'monitor_performance',
    'measure_compatibility',
    'profile_activity',
    'export_for_debugging',
    'import_from_checkpoint',
    'validate_configuration',
    'generate_config_template',
    'BrainSimulator',
    'HighLevelAPIError',
    '__version__'
]
''')
