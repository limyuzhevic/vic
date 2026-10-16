#!/usr/bin/env python3
"""
Advanced NLM Python Extensions

This module provides advanced extensions and functionality for NLM simulations,
including multi-agent systems, advanced visualization, and research tools.
"""

import json
import time
import threading
import multiprocessing
from typing import Dict, List, Optional, Tuple, Any, Callable
from pathlib import Path
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import pandas as pd
from scipy import stats
import warnings

# Try to import the NLM bindings
try:
    import pynlm as nlm
    import pynlm_enhanced as nlm_enhanced
    HAS_NLM = True
except ImportError:
    HAS_NLM = False
    nlm = None
    nlm_enhanced = None
class MultiAgentSystem:
    """
    Advanced multi-agent simulation system for NLM.
    
    Supports multiple brains interacting with each other and with a shared world,
    enabling complex social and collective behaviors.
    """
    
    def __init__(self, world=None, agent_count=5):
        """
        Initialize multi-agent system.
        
        Args:
            world: Shared world object
            agent_count: Number of agents in the system
        """
        if not HAS_NLM:
            raise ImportError("NLM bindings not available")
        
        self.world = world or nlm.createSimpleWorld()
        self.agent_count = agent_count
        self.agents = []
        self.brains = []
        self.agent_positions = []
        self.agent_orientations = []
        self.communication_matrix = None
        self.shared_memory = {}
        self.simulation_step = 0
        
        # Initialize agents
        self.initialize_agents()
    
    def initialize_agents(self):
        """Initialize multiple agents with different configurations."""
        # Distribute agents across the world
        world_width = self.world.getWidth()
        world_height = self.world.getHeight()
        
        for i in range(self.agent_count):
            # Different agent types and configurations
            config = nlm.createDefaultConfig()
            
            # Vary agent parameters for diversity
            brain = nlm.createBrain(config)
            brain.initialize()
            
            agent = nlm.createAgentBrain(brain)
            
            # Different starting positions
            x = (i + 1) * world_width / (self.agent_count + 1)
            y = world_height / 2
            self.agent_positions.append((x, y))
            self.agent_orientations.append(0.0)
            
            self.agents.append(agent)
            self.brains.append(brain)
            
            # Initialize agent in world
            agent.initialize(self.world)
            agent.reset()
    
    def set_communication_matrix(self, matrix):
        """
        Set communication matrix between agents.
        
        Args:
            matrix: NxN matrix where entry [i,j] represents communication strength from agent i to j
        """
        if len(matrix) != self.agent_count or any(len(row) != self.agent_count for row in matrix):
            raise ValueError("Communication matrix must be NxN where N is agent count")
        
        self.communication_matrix = np.array(matrix)
    
    def enable_social_learning(self):
        """Enable social learning between agents."""
        self.social_learning_enabled = True
    
    def enable_collaboration(self):
        """Enable collaborative behavior."""
        self.collaboration_enabled = True
    
    def enable_competition(self):
        """Enable competitive behavior."""
        self.competition_enabled = True
    
    def step(self):
        """Run one simulation step for all agents."""
        self.simulation_step += 1
        
        # Update world
        self.world.update(0.1)
        
        # Collect sensory input for all agents
        agent_percepts = []
        for i, agent in enumerate(self.agents):
            # Each agent gets a view of the world centered on its position
            percept = self.get_agent_view(i)
            agent_percepts.append(percept)
        
        # Process sensory input and allow inter-agent communication
        for i, agent in enumerate(self.agents):
            # Process own sensory input
            agent.processSensoryInput(agent_percepts[i])
            
            # Social learning: agents can learn from each other's behavior
            if hasattr(self, 'social_learning_enabled') and self.social_learning_enabled:
                self.apply_social_learning(i, agent_percepts)
            
            # Collaboration: agents can work together
            if hasattr(self, 'collaboration_enabled') and self.collaboration_enabled:
                self.apply_collaboration(i)
            
            # Competition: agents can compete
            if hasattr(self, 'competition_enabled') and self.competition_enabled:
                self.apply_competition(i)
            
            # Brain computation
            self.brains[i].step(self.simulation_step)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Update agent position
            self.update_agent_position(i, action)
    
    def get_agent_view(self, agent_idx, view_distance=5.0):
        """
        Get a simplified view of the world for a specific agent.
        
        Args:
            agent_idx: Index of the agent
            view_distance: How far the agent can see
            
        Returns:
            Simplified sensory percept for the agent
        """
        agent_x, agent_y = self.agent_positions[agent_idx]
        
        # Create a simplified vision map
        vision_width = self.world.getVisionWidth()
        vision_height = self.world.getVisionHeight()
        
        # Get actual percept
        percept = self.world.getSensoryPercept()
        
        # For now, return the standard percept
        # In a full implementation, this would include inter-agent information
        return percept
    
    def apply_social_learning(self, agent_idx, all_percepts):
        """
        Apply social learning between agents.
        
        Args:
            agent_idx: Index of the agent learning
            all_percepts: Percepts from all agents
        """
        if self.communication_matrix is None:
            return
        
        # Get information from other agents based on communication matrix
        agent_info = {}
        for i in range(self.agent_count):
            if i != agent_idx and self.communication_matrix[agent_idx, i] > 0:
                # Receive information from agent i
                agent_info[i] = all_percepts[i]
        
        # Apply social learning (simplified implementation)
        # In practice, this could involve:
        # - Imitation learning
        # - Teaching behaviors
        # - Cultural transmission
        pass
    
    def apply_collaboration(self, agent_idx):
        """
        Apply collaborative behavior.
        
        Args:
            agent_idx: Index of the agent
        """
        # Simple collaboration: agents can help each other
        # Find nearby agents
        agent_x, agent_y = self.agent_positions[agent_idx]
        
        for i in range(self.agent_count):
            if i != agent_idx:
                other_x, other_y = self.agent_positions[i]
                distance = np.sqrt((agent_x - other_x)**2 + (agent_y - other_y)**2)
                
                if distance < 2.0:  # Close proximity
                    # Simple cooperation: share information
                    pass
    
    def apply_competition(self, agent_idx):
        """
        Apply competitive behavior.
        
        Args:
            agent_idx: Index of the agent
        """
        # Simple competition: agents compete for resources
        # This could be implemented with:
        # - Resource competition
        # - Territory marking
        # - Dominance hierarchies
        pass
    
    def update_agent_position(self, agent_idx, action):
        """
        Update agent position based on action.
        
        Args:
            agent_idx: Index of the agent
            action: Action to apply
        """
        agent_x, agent_y = self.agent_positions[agent_idx]
        
        # Update position based on action type
        if action.getType() == nlm.ActionType.MoveForward:
            angle = self.agent_orientations[agent_idx]
            agent_x += np.cos(angle) * 0.5
            agent_y += np.sin(angle) * 0.5
        elif action.getType() == nlm.ActionType.TurnLeft:
            self.agent_orientations[agent_idx] += np.pi / 4
        elif action.getType() == nlm.ActionType.TurnRight:
            self.agent_orientations[agent_idx] -= np.pi / 4
        elif action.getType() == nlm.ActionType.Interact:
            # Interaction could lead to position change or other effects
            pass
        
        # Keep agent in bounds
        agent_x = max(0, min(agent_x, self.world.getWidth()))
        agent_y = max(0, min(agent_y, self.world.getHeight()))
        
        self.agent_positions[agent_idx] = (agent_x, agent_y)
    
    def get_system_status(self):
        """Get comprehensive status of the multi-agent system."""
        status = {
            'simulation_step': self.simulation_step,
            'agent_count': self.agent_count,
            'world_size': {
                'width': self.world.getWidth(),
                'height': self.world.getHeight()
            },
            'agent_statistics': [],
            'social_dynamics': {},
            'system_performance': {}
        }
        
        # Get statistics for each agent
        for i in range(self.agent_count):
            brain = self.brains[i]
            agent = self.agents[i]
            
            agent_stats = {
                'agent_id': i,
                'position': self.agent_positions[i],
                'orientation': self.agent_orientations[i],
                'neurons_firing': brain.getFiringNeuronCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'average_firing_rate': brain.getAverageFiringRate(),
                'development_stage': brain.getDevelopmentalStage(),
                'curiosity_level': agent.getCuriosityLevel(),
                'novelty_level': agent.getNoveltyLevel(),
                'neuromodulation_level': agent.getNeuromodulationLevel()
            }
            
            status['agent_statistics'].append(agent_stats)
        
        # Calculate system-level statistics
        total_spikes = sum(agent_stats['total_spikes'] for agent_stats in status['agent_statistics'])
        total_neurons_firing = sum(agent_stats['neurons_firing'] for agent_stats in status['agent_statistics'])
        
        status['system_performance'] = {
            'total_system_spikes': total_spikes,
            'total_neurons_active': total_neurons_firing,
            'average_agent_activity': total_spikes / self.agent_count,
            'diversity_index': self.calculate_diversity_index(),
            'cohesion_index': self.calculate_cohesion_index()
        }
        
        return status
    
    def calculate_diversity_index(self):
        """Calculate diversity index of agent behaviors."""
        if self.agent_count <= 1:
            return 0.0
        
        # Calculate based on developmental stages
        stages = [agent_stats['development_stage'] for agent_stats in self.get_system_status()['agent_statistics']]
        unique_stages = len(set(stages))
        
        return unique_stages / self.agent_count
    
    def calculate_cohesion_index(self):
        """Calculate cohesion index of agent positions."""
        if self.agent_count <= 1:
            return 1.0
        
        # Calculate average distance between agents
        total_distance = 0.0
        pairs = 0
        
        for i in range(self.agent_count):
            for j in range(i + 1, self.agent_count):
                x1, y1 = self.agent_positions[i]
                x2, y2 = self.agent_positions[j]
                distance = np.sqrt((x1 - x2)**2 + (y1 - y2)**2)
                total_distance += distance
                pairs += 1
        
        if pairs == 0:
            return 0.0
        
        avg_distance = total_distance / pairs
        max_distance = np.sqrt(self.world.getWidth()**2 + self.world.getHeight()**2)
        
        return 1.0 - (avg_distance / max_distance)
class AdvancedVisualization:
    """
    Advanced visualization tools for NLM simulations.
    
    Includes interactive plots, 3D visualizations, and animation tools.
    """
    
    def __init__(self):
        self.setup_matplotlib()
    
    def setup_matplotlib(self):
        """Setup matplotlib for advanced visualization."""
        plt.style.use('seaborn-v0_8')
        self.figures = {}
        self.animations = {}
    
    def create_interactive_dashboard(self, sim, output_file=None):
        """
        Create an interactive dashboard for simulation monitoring.
        
        Args:
            sim: Simulation object or SimulationManager
            output_file: Optional output file for dashboard
            
        Returns:
            Interactive dashboard
        """
        try:
            import ipywidgets as widgets
            from IPython.display import display
            
            # Create widgets for dashboard controls
            controls = {
                'animation_speed': widgets.IntSlider(
                    value=100, min=10, max=1000, step=10,
                    description='Speed:',
                    continuous_update=True
                ),
                'show_neurons': widgets.Checkbox(
                    value=True, description='Show Neurons'
                ),
                'show_spikes': widgets.Checkbox(
                    value=True, description='Show Spikes'
                ),
                'show_activities': widgets.Checkbox(
                    value=True, description='Show Activities'
                ),
                'theme': widgets.Dropdown(
                    options=['Light', 'Dark', 'Auto'],
                    value='Auto', description='Theme:'
                )
            }
            
            # Create visualization areas
            viz_areas = {
                'neural_activity': widgets.Output(),
                'population_dynamics': widgets.Output(),
                'network_topology': widgets.Output(),
                'performance_metrics': widgets.Output()
            }
            
            # Create main layout
            dashboard_layout = widgets.VBox([
                widgets.HBox([controls['animation_speed'], controls['show_neurons']]),
                widgets.HBox([controls['show_spikes'], controls['show_activities']]),
                widgets.HBox([controls['theme']]),
                widgets.HBox([viz_areas['neural_activity'], viz_areas['population_dynamics']]),
                widgets.HBox([viz_areas['network_topology'], viz_areas['performance_metrics']])
            ])
            
            # Update function
            def update_dashboard(change=None):
                with viz_areas['neural_activity']:
                    plt.clf()
                    if hasattr(sim, 'brain'):
                        self.plot_neural_activity(sim.brain, controls['show_neurons'].value)
                    plt.show()
                
                with viz_areas['population_dynamics']:
                    plt.clf()
                    if hasattr(sim, 'world'):
                        self.plot_population_dynamics(sim.world)
                    plt.show()
                
                with viz_areas['network_topology']:
                    plt.clf()
                    if hasattr(sim, 'brain'):
                        self.plot_network_topology(sim.brain)
                    plt.show()
                
                with viz_areas['performance_metrics']:
                    plt.clf()
                    if hasattr(sim, 'agent'):
                        self.plot_performance_metrics(sim.agent)
                    plt.show()
            
            # Connect controls to update function
            for control in controls.values():
                control.observe(update_dashboard, names='value')
            
            # Initial update
            update_dashboard()
            
            return dashboard_layout
            
        except ImportError:
            print("ipywidgets not available. Installing with: pip install ipywidgets")
            return None
    
    def create_3d_neural_network(self, brain, output_file=None):
        """
        Create 3D visualization of neural network.
        
        Args:
            brain: Brain object
            output_file: Optional output file
            
        Returns:
            3D neural network visualization
        """
        try:
            from mpl_toolkits.mplot3d import Axes3D
            import networkx as nx
            
            # Create figure
            fig = plt.figure(figsize=(12, 10))
            ax = fig.add_subplot(111, projection='3d')
            
            # Get brain regions
            regions = brain.getRegions()
            
            # Create network graph
            G = nx.Graph()
            
            # Add nodes (neurons/regions)
            for i, region in enumerate(regions):
                region_id = region.getId()
                neuron_count = 0  # In a real implementation, get actual neuron count
                
                G.add_node(i, 
                          pos=(i % 10, i // 10, neuron_count / 1000),
                          region_id=region_id,
                          size=neuron_count / 10)
            
            # Add edges (connections)
            for i in range(len(regions)):
                for j in range(i + 1, len(regions)):
                    # Add connection with some probability
                    if np.random.random() < 0.1:
                        G.add_edge(i, j, weight=np.random.random())
            
            # Layout
            pos = nx.spring_layout(G, dim=3, seed=42)
            
            # Draw nodes
            node_sizes = [G.nodes[node]['size'] * 100 for node in G.nodes()]
            node_colors = [G.nodes[node]['size'] / max(node_sizes) for node in G.nodes()]
            
            nx.draw_networkx_nodes(
                G, pos, node_color=node_colors, node_size=node_sizes,
                cmap=plt.cm.viridis, alpha=0.8, ax=ax
            )
            
            # Draw edges
            edges = G.edges()
            for edge in edges:
                nx.draw_networkx_edges(
                    G, pos, edgelist=[edge], alpha=0.3, width=1,
                    edge_color='gray', ax=ax
                )
            
            # Labels
            for node, data in G.nodes(data=True):
                ax.text(
                    pos[node][0], pos[node][1], pos[node][2] + 0.1,
                    f"R{data['region_id']}",
                    fontsize=8, ha='center'
                )
            
            ax.set_title("3D Neural Network Visualization")
            ax.set_xlabel("X")
            ax.set_ylabel("Y")
            ax.set_zlabel("Z")
            
            if output_file:
                plt.savefig(output_file, dpi=150, bbox_inches='tight')
                plt.close()
                return output_file
            else:
                plt.show()
                return fig
                
        except ImportError:
            print("Required packages not available. Installing with: pip install matplotlib networkx")
            return None
    
    def create_activity_animation(self, sim, output_file=None, duration=10):
        """
        Create activity animation.
        
        Args:
            sim: Simulation object
            output_file: Optional output file
            duration: Duration in seconds
            
        Returns:
            Animation object
        """
        try:
            fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 10))
            
            # Setup data storage
            time_data = []
            spike_data = []
            firing_rate_data = []
            
            def animate(frame):
                # Run one step
                if hasattr(sim, 'step'):
                    sim.step()
                
                # Collect data
                step = sim.simulation_step if hasattr(sim, 'simulation_step') else frame
                
                if hasattr(sim, 'brain'):
                    brain = sim.brain
                    time_data.append(step)
                    spike_data.append(brain.getFiringNeuronCount())
                    firing_rate_data.append(brain.getAverageFiringRate())
                
                # Clear plots
                ax1.clear()
                ax2.clear()
                
                # Plot spike counts
                ax1.plot(time_data, spike_data, 'r-', alpha=0.7, label='Firing Neurons')
                ax1.set_xlabel('Step')
                ax1.set_ylabel('Firing Neurons', color='red')
                ax1.set_title('Neural Activity Over Time')
                ax1.legend()
                ax1.grid(True, alpha=0.3)
                
                # Plot firing rates
                ax2.plot(time_data, firing_rate_data, 'b-', alpha=0.7, label='Firing Rate')
                ax2.set_xlabel('Step')
                ax2.set_ylabel('Firing Rate (Hz)', color='blue')
                ax2.legend()
                ax2.grid(True, alpha=0.3)
                
                # Adjust layout
                plt.tight_layout()
            
            # Create animation
            anim = FuncAnimation(
                fig, animate, frames=duration * 10, interval=100, blit=False
            )
            
            if output_file:
                anim.save(output_file, writer='ffmpeg', fps=10)
                plt.close()
                return output_file
            else:
                plt.show()
                return anim
                
        except ImportError:
            print("ffmpeg or matplotlib.animation not available")
            return None
class ResearchTools:
    """
    Research-specific tools for NLM experiments.
    
    Includes statistical analysis, experiment management, and reproducibility tools.
    """
    
    def __init__(self):
        self.experiment_db = {}
        self.results_archive = {}
        self.parameters_store = {}
    
    def create_experiment(self, name, description, parameters):
        """
        Create a new experiment.
        
        Args:
            name: Experiment name
            description: Experiment description
            parameters: Experiment parameters
            
        Returns:
            Experiment object
        """
        experiment = {
            'name': name,
            'description': description,
            'parameters': parameters,
            'created_at': time.time(),
            'status': 'pending',
            'results': None,
            'metadata': {}
        }
        
        self.experiment_db[name] = experiment
        return experiment
    
    def run_experiment(self, experiment_name, sim_factory):
        """
        Run an experiment.
        
        Args:
            experiment_name: Name of experiment
            sim_factory: Function that creates simulation
            
        Returns:
            Experiment results
        """
        if experiment_name not in self.experiment_db:
            raise ValueError(f"Experiment '{experiment_name}' not found")
        
        experiment = self.experiment_db[experiment_name]
        experiment['status'] = 'running'
        experiment['start_time'] = time.time()
        
        try:
            # Run simulation
            sim = sim_factory(experiment['parameters'])
            
            # Run episodes
            num_episodes = experiment['parameters'].get('num_episodes', 10)
            max_steps = experiment['parameters'].get('max_steps', 1000)
            
            all_stats = []
            for episode in range(num_episodes):
                stats = sim.run_episode(max_steps, random_seed=episode)
                all_stats.append(stats)
            
            # Calculate experiment results
            results = self.analyze_experiment_results(all_stats, experiment['parameters'])
            
            experiment['results'] = results
            experiment['end_time'] = time.time()
            experiment['status'] = 'completed'
            
            # Archive results
            self.archive_results(experiment_name, experiment)
            
            return results
            
        except Exception as e:
            experiment['status'] = 'failed'
            experiment['error'] = str(e)
            experiment['end_time'] = time.time()
            raise
    
    def analyze_experiment_results(self, episode_stats, parameters):
        """
        Analyze experiment results.
        
        Args:
            episode_stats: List of episode statistics
            parameters: Experiment parameters
            
        Returns:
            Analysis results
        """
        # Convert to DataFrame for analysis
        df = pd.DataFrame(episode_stats)
        
        analysis = {
            'summary_statistics': {
                'mean_reward': df['total_reward'].mean(),
                'std_reward': df['total_reward'].std(),
                'min_reward': df['total_reward'].min(),
                'max_reward': df['total_reward'].max(),
                'mean_steps': df['steps'].mean(),
                'success_rate': df['success'].mean(),
                'mean_objects_collected': df['objects_collected'].mean(),
                'mean_hazards_avoided': df['hazards_avoided'].mean()
            },
            'distributions': {
                'reward_distribution': self.calculate_distribution(df['total_reward']),
                'steps_distribution': self.calculate_distribution(df['steps'])
            },
            'comparative_analysis': self.perform_statistical_tests(df, parameters),
            'learning_curves': self.calculate_learning_curves(df),
            'quality_metrics': self.calculate_quality_metrics(df)
        }
        
        return analysis
    
    def calculate_distribution(self, data):
        """Calculate statistical distribution of data."""
        return {
            'mean': np.mean(data),
            'median': np.median(data),
            'std': np.std(data),
            'min': np.min(data),
            'max': np.max(data),
            'skewness': stats.skew(data),
            'kurtosis': stats.kurtosis(data)
        }
    
    def perform_statistical_tests(self, df, parameters):
        """Perform statistical tests on experiment results."""
        tests = {}
        
        # Test if mean reward is significantly different from zero
        if len(df) > 1:
            t_stat, p_value = stats.ttest_1samp(df['total_reward'], 0)
            tests['reward_vs_zero'] = {
                't_statistic': t_stat,
                'p_value': p_value,
                'significant': p_value < 0.05
            }
        
        # Test success rate
        success_count = df['success'].sum()
        tests['success_rate'] = {
            'observed_rate': success_count / len(df),
            'binomial_ci': self.calculate_binomial_ci(success_count, len(df))
        }
        
        # Compare with baseline if provided
        baseline = parameters.get('baseline_reward', None)
        if baseline is not None:
            t_stat, p_value = stats.ttest_1samp(df['total_reward'], baseline)
            tests['vs_baseline'] = {
                't_statistic': t_stat,
                'p_value': p_value,
                'baseline': baseline,
                'mean_difference': df['total_reward'].mean() - baseline,
                'significant': p_value < 0.05
            }
        
        return tests
    
    def calculate_binomial_ci(self, successes, trials, confidence=0.95):
        """Calculate binomial confidence interval."""
        from scipy.stats import binomtest
        
        result = binomtest(successes, trials, 0.5)
        ci_low, ci_high = result.proportion_confint(confidence_level=confidence)
        
        return {
            'lower_bound': ci_low,
            'upper_bound': ci_high,
            'confidence': confidence
        }
    
    def calculate_learning_curves(self, df):
        """Calculate learning curves from episode data."""
        # Group by episode
        df['episode'] = range(len(df))
        
        # Calculate rolling statistics
        rolling_mean = df['total_reward'].rolling(window=3, min_periods=1).mean()
        rolling_std = df['total_reward'].rolling(window=3, min_periods=1).std()
        
        return {
            'episodes': df['episode'].tolist(),
            'rewards': df['total_reward'].tolist(),
            'rolling_mean': rolling_mean.tolist(),
            'rolling_std': rolling_std.tolist()
        }
    
    def calculate_quality_metrics(self, df):
        """Calculate quality metrics for experiment results."""
        metrics = {}
        
        # Stability metrics
        metrics['reward_stability'] = 1.0 / (1.0 + df['total_reward'].std())
        metrics['success_consistency'] = df['success'].mean()
        
        # Efficiency metrics
        metrics['steps_efficiency'] = 1.0 / (1.0 + df['steps'].mean())
        metrics['reward_per_step'] = df['total_reward'].mean() / df['steps'].mean() if df['steps'].mean() > 0 else 0
        
        # Performance metrics
        metrics['max_performance'] = df['total_reward'].max()
        metrics['min_performance'] = df['total_reward'].min()
        
        return metrics
    
    def archive_results(self, experiment_name, experiment_data):
        """Archive experiment results."""
        timestamp = time.strftime("%Y%m%d_%H%M%S")
        filename = f"experiment_{experiment_name}_{timestamp}.json"
        
        with open(filename, 'w') as f:
            json.dump(experiment_data, f, indent=2)
        
        self.results_archive[experiment_name] = filename
    
    def load_experiment_results(self, experiment_name):
        """Load archived experiment results."""
        if experiment_name not in self.results_archive:
            raise ValueError(f"Experiment '{experiment_name}' not found in archive")
        
        filename = self.results_archive[experiment_name]
        
        with open(filename, 'r') as f:
            return json.load(f)
    
    def create_research_report(self, experiment_name, output_file=None):
        """
        Create a research report for an experiment.
        
        Args:
            experiment_name: Name of experiment
            output_file: Optional output file
            
        Returns:
            Research report
        """
        if experiment_name not in self.experiment_db:
            raise ValueError(f"Experiment '{experiment_name}' not found")
        
        experiment = self.experiment_db[experiment_name]
        
        if experiment['status'] != 'completed':
            raise ValueError(f"Experiment '{experiment_name}' is not completed")
        
        report = {
            'experiment_title': experiment['name'],
            'description': experiment['description'],
            'parameters': experiment['parameters'],
            'execution_time': experiment['end_time'] - experiment['start_time'],
            'results': experiment['results'],
            'created_at': experiment['created_at'],
            'completed_at': experiment['end_time']
        }
        
        if output_file:
            with open(output_file, 'w') as f:
                json.dump(report, f, indent=2)
            
            # Also create a markdown report
            md_file = output_file.replace('.json', '.md')
            with open(md_file, 'w') as f:
                f.write(self.create_markdown_report(report))
            
            return output_file
        else:
            return report
    
    def create_markdown_report(self, report):
        """Create a markdown research report."""
        md = f"# Research Report: {report['experiment_title']}\n\n"
        
        md += f"## Description\n{report['description']}\n\n"
        
        md += f"## Experiment Parameters\n"
        md += "| Parameter | Value |\n"
        md += "|-----------|-------|\n"
        for key, value in report['parameters'].items():
            md += f"| {key} | {value} |\n"
        
        md += f"\n## Results Summary\n"
        results = report['results']
        
        md += f"### Statistical Summary\n"
        summary = results['summary_statistics']
        md += "| Metric | Value |\n"
        md += "|--------|-------|\n"
        for key, value in summary.items():
            md += f"| {key} | {value:.4f} |\n"
        
        md += f"\n### Quality Metrics\n"
        quality = results['quality_metrics']
        md += "| Metric | Value |\n"
        md += "|--------|-------|\n"
        for key, value in quality.items():
            md += f"| {key} | {value:.4f} |\n"
        
        md += f"\n## Analysis\n"
        md += f"- **Execution time**: {report['execution_time']:.2f} seconds\n"
        md += f"- **Success rate**: {summary['success_rate']*100:.1f}%\n"
        md += f"- **Average reward**: {summary['mean_reward']:.3f}\n"
        
        return md
class PluginSystem:
    """
    Plugin system for extending NLM functionality.
    
    Allows loading and managing plugins that extend NLM capabilities.
    """
    
    def __init__(self):
        self.plugins = {}
        self.plugin_directories = []
    
    def add_plugin_directory(self, directory):
        """Add a plugin directory."""
        self.plugin_directories.append(directory)
    
    def discover_plugins(self):
        """Discover plugins in directories."""
        import importlib.util
        import os
        
        for directory in self.plugin_directories:
            if os.path.exists(directory):
                for filename in os.listdir(directory):
                    if filename.endswith('.py'):
                        module_name = filename[:-3]
                        file_path = os.path.join(directory, filename)
                        
                        try:
                            spec = importlib.util.spec_from_file_location(module_name, file_path)
                            module = importlib.util.module_from_spec(spec)
                            spec.loader.exec_module(module)
                            
                            if hasattr(module, 'Plugin'):
                                plugin_class = module.Plugin
                                plugin = plugin_class()
                                self.plugins[plugin.name] = plugin
                                
                        except Exception as e:
                            print(f"Error loading plugin '{filename}': {e}")
    
    def load_plugin(self, plugin_name):
        """Load a plugin by name."""
        if plugin_name in self.plugins:
            return self.plugins[plugin_name]
        
        # Search for plugin in directories
        for directory in self.plugin_directories:
            file_path = os.path.join(directory, f"{plugin_name}.py")
            if os.path.exists(file_path):
                import importlib.util
                
                spec = importlib.util.spec_from_file_location(plugin_name, file_path)
                module = importlib.util.module_from_spec(spec)
                spec.loader.exec_module(module)
                
                if hasattr(module, 'Plugin'):
                    plugin_class = module.Plugin
                    plugin = plugin_class()
                    self.plugins[plugin_name] = plugin
                    return plugin
        
        raise ValueError(f"Plugin '{plugin_name}' not found")
    
    def unload_plugin(self, plugin_name):
        """Unload a plugin."""
        if plugin_name in self.plugins:
            plugin = self.plugins[plugin_name]
            
            # Call cleanup if available
            if hasattr(plugin, 'cleanup'):
                plugin.cleanup()
            
            # Remove from plugins
            del self.plugins[plugin_name]
    
    def get_plugin(self, plugin_name):
        """Get a plugin by name."""
        return self.plugins.get(plugin_name)
    
    def list_plugins(self):
        """List all loaded plugins."""
        return list(self.plugins.keys())
    
    def enable_plugin(self, plugin_name, sim=None):
        """
        Enable a plugin.
        
        Args:
            plugin_name: Name of plugin
            sim: Simulation object
        """
        plugin = self.get_plugin(plugin_name)
        if plugin:
            if hasattr(plugin, 'initialize'):
                plugin.initialize(sim)
            if hasattr(plugin, 'enable'):
                plugin.enable()
    
    def disable_plugin(self, plugin_name):
        """Disable a plugin."""
        plugin = self.get_plugin(plugin_name)
        if plugin:
            if hasattr(plugin, 'disable'):
                plugin.disable()
class AdvancedAPIManager:
    """
    Advanced API manager for NLM.
    
    Provides enhanced API management, documentation, and tool integration.
    """
    
    def __init__(self):
        self.apis = {}
        self.documentation = {}
        self.tools = {}
        self.examples = {}
    
    def register_api(self, name, api_class, documentation=None):
        """Register an API."""
        self.apis[name] = {
            'class': api_class,
            'documentation': documentation,
            'version': '1.0.0',
            'last_updated': time.time()
        }
    
    def get_api(self, name):
        """Get an API by name."""
        api_info = self.apis.get(name)
        if api_info:
            return api_info['class']
        return None
    
    def list_apis(self):
        """List all registered APIs."""
        return list(self.apis.keys())
    
    def register_tool(self, name, tool_class, description=None):
        """Register a tool."""
        self.tools[name] = {
            'class': tool_class,
            'description': description,
            'version': '1.0.0'
        }
    
    def get_tool(self, name):
        """Get a tool by name."""
        tool_info = self.tools.get(name)
        if tool_info:
            return tool_info['class']
        return None
    
    def register_example(self, name, example_code, description=None):
        """Register an example."""
        self.examples[name] = {
            'code': example_code,
            'description': description,
            'category': 'general'
        }
    
    def get_example(self, name):
        """Get an example by name."""
        example_info = self.examples.get(name)
        if example_info:
            return example_info
        return None
    
    def create_research_session(self, session_name, api_configs=None):
        """
        Create a research session with multiple APIs.
        
        Args:
            session_name: Name of research session
            api_configs: Configuration for APIs
        """
        session = {
            'name': session_name,
            'created_at': time.time(),
            'apis': {},
            'tools': {},
            'results': {}
        }
        
        if api_configs:
            for api_name, config in api_configs.items():
                api_class = self.get_api(api_name)
                if api_class:
                    session['apis'][api_name] = api_class(config)
                else:
                    warnings.warn(f"API '{api_name}' not found")
        
        return session
    
    def run_research_session(self, session, experiment_factory):
        """
        Run a research session.
        
        Args:
            session: Research session object
            experiment_factory: Factory function to create experiments
        """
        session['start_time'] = time.time()
        
        try:
            # Run experiments
            results = {}
            for experiment_name in session['apis']:
                api = session['apis'][experiment_name]
                
                # Create and run experiment
                experiment = experiment_factory(api, session)
                results[experiment_name] = experiment
                
                # Store results
                session['results'][experiment_name] = results[experiment_name]
            
            session['end_time'] = time.time()
            session['status'] = 'completed'
            
            return results
            
        except Exception as e:
            session['end_time'] = time.time()
            session['status'] = 'failed'
            session['error'] = str(e)
            raise
    
    def export_session(self, session, output_file):
        """Export research session."""
        with open(output_file, 'w') as f:
            json.dump(session, f, indent=2, default=str)
        
        return output_file
    
    def import_session(self, input_file):
        """Import research session."""
        with open(input_file, 'r') as f:
            return json.load(f)
class NLMEnhanced:
    """
    Main enhanced API class for NLM.
    
    Provides a unified interface for all enhanced NLM functionality.
    """
    
    def __init__(self):
        self.api_manager = AdvancedAPIManager()
        self.plugin_system = PluginSystem()
        self.setup_enhanced_apis()
        self.setup_enhanced_tools()
        self.setup_enhanced_examples()
    
    def setup_enhanced_apis(self):
        """Setup enhanced APIs."""
        # Register MultiAgentSystem
        self.api_manager.register_api(
            'multi_agent',
            MultiAgentSystem,
            "Advanced multi-agent simulation system"
        )
        
        # Register AdvancedVisualization
        self.api_manager.register_api(
            'visualization',
            AdvancedVisualization,
            "Advanced visualization tools"
        )
        
        # Register ResearchTools
        self.api_manager.register_api(
            'research',
            ResearchTools,
            "Research-specific tools and analysis"
        )
    
    def setup_enhanced_tools(self):
        """Setup enhanced tools."""
        # Register plugin system tool
        self.api_manager.register_tool(
            'plugins',
            self.plugin_system,
            "Plugin system for extending NLM functionality"
        )
        
        # Register API manager tool
        self.api_manager.register_tool(
            'api_manager',
            self.api_manager,
            "Advanced API management"
        )
    
    def setup_enhanced_examples(self):
        """Setup enhanced examples."""
        # Register research workflow example
        self.api_manager.register_example(
            'research_workflow',
            self._get_research_workflow_example(),
            "Complete research workflow with multiple APIs"
        )
        
        # Register multi-agent example
        self.api_manager.register_example(
            'multi_agent_system',
            self._get_multi_agent_example(),
            "Multi-agent system with social learning"
        )
        
        # Register advanced visualization example
        self.api_manager.register_example(
            'advanced_visualization',
            self._get_visualization_example(),
            "Advanced visualization techniques"
        )
    
    def _get_research_workflow_example(self):
        """Get research workflow example."""
        return '''
import nlm_enhanced as nlm

# Create enhanced research session
session = nlm.api_manager.create_research_session(
    "neural_network_research",
    api_configs={
        "multi_agent": {"agent_count": 10},
        "visualization": {},
        "research": {}
    }
)

# Define experiment factory
def experiment_factory(api, session):
    if api.__class__.__name__ == "MultiAgentSystem":
        # Create multi-agent experiment
        sim = api(world=nlm.createSimpleWorld())
        sim.set_communication_matrix([[0.1]*10 for _ in range(10)])
        sim.enable_social_learning()
        
        # Run simulation
        for step in range(100):
            sim.step()
        
        return {
            "type": "multi_agent",
            "status": "completed",
            "final_status": sim.get_system_status()
        }
    
    return {"type": "unknown", "status": "not_run"}

# Run research session
results = nlm.api_manager.run_research_session(session, experiment_factory)

# Export results
nlm.api_manager.export_session(session, "research_session.json")
print("Research session completed and exported!")
'''
    
    def _get_multi_agent_example(self):
        """Get multi-agent example."""
        return '''
import nlm_enhanced as nlm
from nlm_enhanced import MultiAgentSystem

# Create multi-agent system
world = nlm.createSimpleWorld()
system = MultiAgentSystem(world, agent_count=5)

# Setup communication and behaviors
system.set_communication_matrix([
    [0.0, 0.5, 0.3, 0.1, 0.1],
    [0.3, 0.0, 0.4, 0.2, 0.1],
    [0.2, 0.3, 0.0, 0.5, 0.0],
    [0.1, 0.2, 0.3, 0.0, 0.4],
    [0.1, 0.1, 0.0, 0.3, 0.0]
])

system.enable_social_learning()
system.enable_collaboration()

# Run simulation
for step in range(200):
    system.step()
    
    # Monitor every 20 steps
    if step % 20 == 0:
        status = system.get_system_status()
        print(f"Step {step}: {status['system_performance']['total_system_spikes']} total spikes")

# Analyze results
final_status = system.get_system_status()
print(f"Diversity index: {final_status['social_dynamics']['diversity_index']:.3f}")
print(f"Cohesion index: {final_status['social_dynamics']['cohesion_index']:.3f}")
'''
    
    def _get_visualization_example(self):
        """Get visualization example."""
        return '''
import nlm_enhanced as nlm
from nlm_enhanced import AdvancedVisualization

# Create visualization system
viz = AdvancedVisualization()

# Create simulation
world = nlm.createSimpleWorld()
sim = nlm.create_simulation("advanced", 100, 100)

# Create 3D neural network visualization
viz.create_3d_neural_network(sim.brain, "neural_network_3d.png")

# Create activity animation
viz.create_activity_animation(sim, "neural_activity.gif", duration=5)

# Create interactive dashboard
viz.create_interactive_dashboard(sim, "dashboard.html")

print("Advanced visualizations created!")
print("Files: neural_network_3d.png, neural_activity.gif, dashboard.html")
'''
    
    def create_research_session(self, session_name, api_configs=None):
        """Create a research session."""
        return self.api_manager.create_research_session(session_name, api_configs)
    
    def run_research_session(self, session, experiment_factory):
        """Run a research session."""
        return self.api_manager.run_research_session(session, experiment_factory)
    
    def get_example(self, example_name):
        """Get an example by name."""
        return self.api_manager.get_example(example_name)
    
    def list_examples(self):
        """List all available examples."""
        return list(self.api_manager.examples.keys())
    
    def list_apis(self):
        """List all available APIs."""
        return self.api_manager.list_apis()
    
    def list_tools(self):
        """List all available tools."""
        return list(self.api_manager.tools.keys())

# Global enhanced API instance
_enhanced_api = None

def get_enhanced_api():
    """Get the global enhanced API instance."""
    global _enhanced_api
    if _enhanced_api is None:
        _enhanced_api = NLMEnhanced()
    return _enhanced_api

# Convenience functions
def create_research_session(session_name, api_configs=None):
    """Create a research session using the enhanced API."""
    return get_enhanced_api().create_research_session(session_name, api_configs)

def run_research_session(session, experiment_factory):
    """Run a research session using the enhanced API."""
    return get_enhanced_api().run_research_session(session, experiment_factory)

def get_example(example_name):
    """Get an example using the enhanced API."""
    return get_enhanced_api().get_example(example_name)

# Register enhanced API with the module
if HAS_NLM:
    # Make the enhanced API available
    import sys
    if 'pynlm_enhanced' not in sys.modules:
        # Create module wrapper
        import types
        enhanced_module = types.ModuleType('pynlm_enhanced')
        enhanced_module.__doc__ = __doc__
        
        # Add all classes and functions from NLMEnhanced
        enhanced_module.NLMEnhanced = NLMEnhanced
        enhanced_module.MultiAgentSystem = MultiAgentSystem
        enhanced_module.AdvancedVisualization = AdvancedVisualization
        enhanced_module.ResearchTools = ResearchTools
        enhanced_module.PluginSystem = PluginSystem
        
        # Add convenience functions
        enhanced_module.get_enhanced_api = get_enhanced_api
        enhanced_module.create_research_session = create_research_session
        enhanced_module.run_research_session = run_research_session
        enhanced_module.get_example = get_example
        
        # Add reference to original NLM
        enhanced_module.nlm = nlm
        
        # Register in sys.modules
        sys.modules['pynlm_enhanced'] = enhanced_module

if __name__ == "__main__":
    # Test the enhanced module
    print("NLM Enhanced Python Extensions")
    print("=" * 50)
    
    try:
        import pynlm_enhanced as nlm_enhanced
        
        print("Testing enhanced functionality...")
        
        # Test example
        print("\n1. Testing research workflow example...")
        example = nlm_enhanced.get_example("research_workflow")
        if example:
            print("✓ Research workflow example available")
        
        print("\n2. Testing multi-agent example...")
        example = nlm_enhanced.get_example("multi_agent_system")
        if example:
            print("✓ Multi-agent example available")
        
        print("\n3. Testing visualization example...")
        example = nlm_enhanced.get_example("advanced_visualization")
        if example:
            print("✓ Visualization example available")
        
        print("\n4. Listing available APIs...")
        apis = nlm_enhanced.list_apis()
        print(f"✓ Available APIs: {', '.join(apis)}")
        
        print("\n5. Listing available tools...")
        tools = nlm_enhanced.list_tools()
        print(f"✓ Available tools: {', '.join(tools)}")
        
        print("\nEnhanced NLM Python Extensions loaded successfully!")
        print("\nKey features:")
        print("- Multi-Agent Systems")
        print("- Advanced Visualization")
        print("- Research Tools")
        print("- Plugin System")
        print("- Research Session Management")
        
    except ImportError as e:
        print(f"Import error: {e}")
        print("Please install required dependencies:")
        print("pip install pynlm_enhanced")
    except Exception as e:
        print(f"Error: {e}")
        import traceback
        traceback.print_exc()