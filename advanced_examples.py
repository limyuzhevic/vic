#!/usr/bin/env python3
"""
Advanced NLM Examples for Expert Users

This module provides advanced examples demonstrating sophisticated NLM usage
patterns, optimization techniques, and expert-level simulation scenarios.

Author: NLM Expert Team
Version: 2.0.0
License: MIT
"""

import pynlm
import numpy as np
from typing import List, Dict, Optional, Tuple
import json

class AdvancedSimulationSetup:
    """
    Advanced simulation setup class demonstrating expert-level configuration
    and optimization techniques.
    """
    
    def __init__(self):
        self.experiments = []
    
    def create_adaptive_network(self, target_efficiency: float = 0.8) -> pynlm.Config:
        """
        Create an adaptive network configuration optimized for specific
        efficiency targets.
        
        Args:
            target_efficiency: Desired network efficiency (0.0 to 1.0)
            
        Returns:
            Optimized configuration for adaptive networks
        """
        config = pynlm.createAdvancedConfig()
        
        # Adaptive network parameters based on target efficiency
        if target_efficiency > 0.9:
            # High efficiency - smaller, faster network
            config.setInt("brain.neuron_count", 800)
            config.setFloat("brain.connection_probability", 0.03)
            config.setFloat("plasticity.stdp.learning_rate", 0.002)
            config.set("neuromod.dopamine.scale", 0.5)
            
        elif target_efficiency > 0.7:
            # Balanced efficiency - moderate size network
            config.setInt("brain.neuron_count", 1500)
            config.setFloat("brain.connection_probability", 0.05)
            config.setFloat("plasticity.stdp.learning_rate", 0.001)
            config.set("neuromod.dopamine.scale", 1.0)
            
        else:
            # Lower efficiency - larger, more plastic network
            config.setInt("brain.neuron_count", 3000)
            config.setFloat("brain.connection_probability", 0.08)
            config.setFloat("plasticity.stdp.learning_rate", 0.0005)
            config.set("neuromod.dopamine.scale", 2.0)
        
        # Enable all advanced plasticity
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.hebbian.learning_rate", 0.0008)
        config.set("plasticity.structural.enable", True)
        config.set("plasticity.structural.synapse_growth_rate", 0.001)
        
        # Configure neuromodulation for learning
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        config.set("neuromod.prediction.error_gain", 1.5)
        
        # Set development parameters
        config.set("development.enabled", True)
        config.set("development.initial_plasticity", 1.2)
        
        # Optimize logging for performance
        config.set("log_level", "WARN")
        
        return config
    
    def create_distributed_simulation_config(self, num_agents: int = 4) -> Dict:
        """
        Create configuration for distributed multi-agent simulation.
        
        Args:
            num_agents: Number of distributed agents
            
        Returns:
            Dictionary with simulation configurations for each agent
        """
        base_config = pynlm.createAdvancedConfig()
        agent_configs = {}
        
        for agent_id in range(num_agents):
            # Create unique configuration for each agent
            agent_config = pynlm.createAdvancedConfig()
            
            # Agent-specific parameters
            base_neurons = 2000
            neuron_variance = 500
            
            agent_config.setInt("brain.neuron_count", 
                              base_neurons + (agent_id * neuron_variance))
            agent_config.setFloat("brain.connection_probability",
                                0.05 + (agent_id * 0.02))
            
            # Different developmental stages for each agent
            if agent_id == 0:
                agent_config.set("development.stage", "Initial")
            elif agent_id == 1:
                agent_config.set("development.stage", "CriticalPeriod")
            elif agent_id == 2:
                agent_config.set("development.stage", "Maturation")
            else:
                agent_config.set("development.stage", "Adult")
            
            # Unique learning parameters
            agent_config.setFloat("plasticity.stdp.learning_rate",
                                0.001 * (1.0 + (agent_id * 0.5)))
            
            agent_configs[f"agent_{agent_id}"] = agent_config
        
        return agent_configs
    
    def create_memory_efficient_simulation(self, memory_limit_mb: int = 512) -> pynlm.Config:
        """
        Create memory-efficient configuration for resource-constrained environments.
        
        Args:
            memory_limit_mb: Maximum memory usage in MB
            
        Returns:
            Memory-optimized configuration
        """
        config = pynlm.createAdvancedConfig()
        
        # Calculate neuron count based on memory constraint
        # Estimate: ~100 bytes per neuron + 10 bytes per synapse
        # For 512MB: ~5.12M bytes / 110 bytes ≈ 46,545 neurons (with sparse connectivity)
        # But we'll be conservative
        neuron_count = min(10000, int((memory_limit_mb * 1024 * 1024) / 150))
        
        config.setInt("brain.neuron_count", neuron_count)
        
        # Use very sparse connectivity for memory efficiency
        connection_prob = max(0.01, min(0.05, neuron_count / 10000 * 0.05))
        config.setFloat("brain.connection_probability", connection_prob)
        
        # Enable only essential plasticity to reduce memory
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.002)
        config.set("plasticity.hebbian.enable", False)  # Disable to save memory
        config.set("plasticity.structural.enable", False)  # Disable to save memory
        
        # Optimize neuromodulation
        config.set("neuromod.dopamine.scale", 0.5)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        
        # Disable development to save memory
        config.set("development.enabled", False)
        
        # Optimize logging
        config.set("log_level", "ERROR")
        
        return config
class AdvancedAgentController:
    """
    Advanced agent controller demonstrating sophisticated control strategies
    and coordination techniques.
    """
    
    def __init__(self):
        self.agent_states = {}
        self.coordination_strategies = {}
    
    def create_hierarchical_control_system(self, world: pynlm.SimpleWorld,
                                          agents: List[pynlm.AgentBrain]) -> Dict:
        """
        Create hierarchical control system with multiple levels of coordination.
        
        Args:
            world: The shared world instance
            agents: List of agent instances
            
        Returns:
            Dictionary with control system components
        """
        control_system = {
            'coordination_layer': self._create_coordination_layer(agents),
            'execution_layer': self._create_execution_layer(agents, world),
            'monitoring_layer': self._create_monitoring_layer(agents),
            'adaptation_layer': self._create_adaptation_layer(agents)
        }
        
        return control_system
    
    def _create_coordination_layer(self, agents: List[pynlm.AgentBrain]) -> Dict:
        """Create coordination layer for agent communication and consensus."""
        return {
            'message_bus': self._create_message_bus(agents),
            'consensus_protocol': self._create_consensus_protocol(),
            'resource_allocator': self._create_resource_allocator(agents),
            'decision_maker': self._create_decision_maker(agents)
        }
    
    def _create_message_bus(self, agents: List[pynlm.AgentBrain]) -> Dict:
        """Create message bus for inter-agent communication."""
        return {
            'broadcast_channel': [agent.getBrain().getTotalNeuronCount() for agent in agents],
            'private_channels': {i: 100 for i in range(len(agents))},
            'priority_queue': self._create_priority_queue(),
            'acknowledgment_system': self._create_acknowledgment_system()
        }
    
    def _create_consensus_protocol(self) -> Dict:
        """Create consensus protocol for coordinated decision making."""
        return {
            'voting_mechanism': 'weighted_majority',
            'confidence_threshold': 0.7,
            'timeout_seconds': 5.0,
            'disagreement_resolution': 'hierarchical_override'
        }
    
    def _create_resource_allocator(self, agents: List[pynlm.AgentBrain]) -> Dict:
        """Create resource allocator for shared resource management."""
        return {
            'energy_manager': self._create_energy_manager(agents),
            'memory_allocator': self._create_memory_allocator(),
            'computation_scheduler': self._create_computation_scheduler(agents)
        }
    
    def _create_execution_layer(self, agents: List[pynlm.AgentBrain], 
                               world: pynlm.SimpleWorld) -> Dict:
        """Create execution layer for agent action coordination."""
        return {
            'action_queue': self._create_action_queue(agents),
            'conflict_resolver': self._create_conflict_resolver(),
            'reward_coordinator': self._create_reward_coordinator(agents),
            'experience_sharing': self._create_experience_sharing(agents)
        }
    
    def _create_monitoring_layer(self, agents: List[pynlm.AgentBrain]) -> Dict:
        """Create monitoring layer for system-wide oversight."""
        return {
            'performance_monitor': self._create_performance_monitor(agents),
            'behavior_analyzer': self._create_behavior_analyzer(agents),
            'health_checker': self._create_health_checker(agents),
            'alert_system': self._create_alert_system()
        }
    
    def _create_adaptation_layer(self, agents: List[pynlm.AgentBrain]) -> Dict:
        """Create adaptation layer for dynamic system adjustment."""
        return {
            'learning_optimizer': self._create_learning_optimizer(agents),
            'topology_adapter': self._create_topology_adapter(agents),
            'strategy_selector': self._create_strategy_selector(agents),
            'feedback_loop': self._create_feedback_loop()
        }
class AdvancedExperimentFramework:
    """
    Advanced experiment framework for conducting systematic research
    and testing optimization strategies.
    """
    
    def __init__(self):
        self.experiment_results = {}
        self.parameter_space = {}
    
    def create_parameter_search_space(self) -> Dict:
        """
        Create comprehensive parameter search space for systematic experiments.
        
        Returns:
            Dictionary with parameter definitions and ranges
        """
        return {
            'network_parameters': {
                'neuron_count': {'type': 'int', 'range': [500, 5000], 'step': 500},
                'connection_probability': {'type': 'float', 'range': [0.01, 0.2], 'step': 0.01},
                'initial_weight_mean': {'type': 'float', 'range': [0.1, 1.0], 'step': 0.1},
                'plasticity_learning_rate': {'type': 'float', 'range': [0.0001, 0.01], 'step': 0.001}
            },
            'neuromodulation_parameters': {
                'dopamine_scale': {'type': 'float', 'range': [0.1, 3.0], 'step': 0.1},
                'curiosity_enable': {'type': 'bool'},
                'novelty_enable': {'type': 'bool'},
                'prediction_error_gain': {'type': 'float', 'range': [0.5, 2.0], 'step': 0.1}
            },
            'development_parameters': {
                'development_enabled': {'type': 'bool'},
                'initial_plasticity': {'type': 'float', 'range': [0.5, 2.0], 'step': 0.1},
                'maturation_rate': {'type': 'float', 'range': [0.001, 0.1], 'step': 0.01}
            }
        }
    
    def run_parameter_sweep(self, parameter_space: Dict, num_trials: int = 20) -> Dict:
        """
        Run parameter sweep experiment across defined parameter space.
        
        Args:
            parameter_space: Parameter space definition
            num_trials: Number of parameter combinations to test
            
        Returns:
            Dictionary with experiment results
        """
        import random
        
        results = []
        for trial in range(num_trials):
            # Randomly sample parameters
            params = self._sample_parameters(parameter_space)
            
            # Run simulation with sampled parameters
            result = self._run_single_trial(params)
            result['trial'] = trial
            results.append(result)
        
        return {
            'results': results,
            'best_configuration': self._find_best_configuration(results),
            'parameter_analysis': self._analyze_parameters(results)
        }
    
    def _sample_parameters(self, parameter_space: Dict) -> Dict:
        """Randomly sample parameters from parameter space."""
        import random
        
        sampled = {}
        for category, params in parameter_space.items():
            sampled[category] = {}
            for param_name, param_def in params.items():
                if param_def['type'] == 'int':
                    sampled[category][param_name] = random.randint(
                        param_def['range'][0], param_def['range'][1])
                elif param_def['type'] == 'float':
                    sampled[category][param_name] = random.uniform(
                        param_def['range'][0], param_def['range'][1])
                elif param_def['type'] == 'bool':
                    sampled[category][param_name] = random.choice([True, False])
        
        return sampled
    
    def _run_single_trial(self, params: Dict) -> Dict:
        """Run single simulation trial with given parameters."""
        # Convert parameters to NLM config
        config = pynlm.createAdvancedConfig()
        
        # Apply network parameters
        if 'neuron_count' in params.get('network_parameters', {}):
            config.setInt("brain.neuron_count", params['network_parameters']['neuron_count'])
        
        if 'connection_probability' in params.get('network_parameters', {}):
            config.setFloat("brain.connection_probability", 
                          params['network_parameters']['connection_probability'])
        
        # Apply neuromodulation parameters
        if 'dopamine_scale' in params.get('neuromodulation_parameters', {}):
            config.setFloat("neuromod.dopamine.scale", 
                          params['neuromodulation_parameters']['dopamine_scale'])
        
        if 'curiosity_enable' in params.get('neuromodulation_parameters', {}):
            config.set("neuromod.curiosity.enable", 
                      params['neuromodulation_parameters']['curiosity_enable'])
        
        if 'novelty_enable' in params.get('neuromodulation_parameters', {}):
            config.set("neuromod.novelty.enable", 
                      params['neuromodulation_parameters']['novelty_enable'])
        
        # Apply development parameters
        if 'development_enabled' in params.get('development_parameters', {}):
            config.set("development.enabled", 
                      params['development_parameters']['development_enabled'])
        
        # Run simulation
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run for shorter duration for parameter sweep
        for step in range(200):
            brain.step(step)
        
        # Record results
        return {
            'final_spikes': brain.getTotalSpikeCount(),
            'final_firing_rate': brain.getAverageFiringRate(),
            'final_curiosity': 0.0,  # Would need agent to get this
            'final_novelty': 0.0,    # Would need agent to get this
            'learned_preference': brain.getLearnedPreference()
        }
    
    def _find_best_configuration(self, results: List[Dict]) -> Dict:
        """Find best performing configuration from results."""
        if not results:
            return {}
        
        # Rank by total spikes (higher is better for exploration)
        best_result = max(results, key=lambda x: x['final_spikes'])
        
        return {
            'parameters': best_result,
            'metrics': {
                'spike_count': best_result['final_spikes'],
                'firing_rate': best_result['final_firing_rate'],
                'learning_effectiveness': best_result['final_spikes'] / 200.0
            }
        }
    
    def _analyze_parameters(self, results: List[Dict]) -> Dict:
        """Analyze parameter effectiveness across trials."""
        analysis = {
            'parameter_correlations': {},
            'performance_distribution': {},
            'best_practices': []
        }
        
        # Extract parameter values and performance metrics
        param_values = []
        performance_values = []
        
        for result in results:
            param_values.append(result.get('parameters', {}))
            performance_values.append(result.get('final_spikes', 0))
        
        # Simple correlation analysis
        if len(param_values) > 1:
            analysis['parameter_correlations'] = {
                'neuron_count_vs_performance': self._calculate_correlation(
                    [p.get('neuron_count', 0) for p in param_values],
                    performance_values),
                'connection_probability_vs_performance': self._calculate_correlation(
                    [p.get('connection_probability', 0) for p in param_values],
                    performance_values)
            }
        
        # Calculate performance statistics
        analysis['performance_distribution'] = {
            'mean_spikes': sum(performance_values) / len(performance_values),
            'std_spikes': statistics.stdev(performance_values) if len(performance_values) > 1 else 0,
            'min_spikes': min(performance_values),
            'max_spikes': max(performance_values)
        }
        
        # Identify best practices
        analysis['best_practices'] = self._identify_best_practices(analysis['parameter_correlations'])
        
        return analysis
    
    def _calculate_correlation(self, x: List[float], y: List[float]) -> float:
        """Calculate Pearson correlation coefficient."""
        if len(x) != len(y) or len(x) < 2:
            return 0.0
        
        x_mean = sum(x) / len(x)
        y_mean = sum(y) / len(y)
        
        numerator = sum((xi - x_mean) * (yi - y_mean) for xi, yi in zip(x, y))
        denominator = (sum((xi - x_mean) ** 2 for xi in x) * 
                       sum((yi - y_mean) ** 2 for yi in y)) ** 0.5
        
        return numerator / denominator if denominator != 0 else 0.0
    
    def _identify_best_practices(self, correlations: Dict) -> List[str]:
        """Identify best practices based on correlation analysis."""
        best_practices = []
        
        if correlations.get('neuron_count_vs_performance', 0) > 0.5:
            best_practices.append("Higher neuron counts tend to produce more exploration")
        
        if correlations.get('connection_probability_vs_performance', 0) > 0.3:
            best_practices.append("Moderate connection probabilities often yield optimal learning")
        
        best_practices.append("Balance between exploration (high spikes) and exploitation")
        best_practices.append("Consider developmental stage effects on learning efficiency")
        
        return best_practices
class AdvancedVisualizationTools:
    """
    Advanced visualization tools for analyzing and presenting NLM simulation data.
    """
    
    def __init__(self):
        self.visualizations = {}
    
    def create_performance_dashboard(self, monitor_data) -> Dict:
        """
        Create comprehensive performance dashboard.
        
        Args:
            monitor_data: Performance monitoring data
            
        Returns:
            Dictionary with visualization components
        """
        return {
            'time_series_plots': self._create_time_series_visualizations(monitor_data),
            'performance_metrics': self._create_performance_metrics_charts(monitor_data),
            'learning_curves': self._create_learning_curve_visualizations(monitor_data),
            'statistical_analysis': self._create_statistical_analysis(monitor_data)
        }
    
    def create_brain_activity_heatmap(self, brain_data: Dict) -> Dict:
        """
        Create heatmap visualization of brain activity patterns.
        
        Args:
            brain_data: Brain activity data
            
        Returns:
            Heatmap visualization data
        """
        return {
            'spike_heatmap': self._generate_spike_heatmap(brain_data),
            'firing_rate_map': self._generate_firing_rate_map(brain_data),
            'connectivity_heatmap': self._generate_connectivity_heatmap(brain_data),
            'activity_clusters': self._identify_activity_clusters(brain_data)
        }
    
    def create_learning_trajectory_analysis(self, learning_data: Dict) -> Dict:
        """
        Create analysis of learning trajectories over time.
        
        Args:
            learning_data: Learning trajectory data
            
        Returns:
            Learning trajectory analysis
        """
        return {
            'trajectory_plots': self._create_trajectory_plots(learning_data),
            'convergence_analysis': self._analyze_convergence(learning_data),
            'stability_assessment': self._assess_stability(learning_data),
            'optimal_stopping': self._determine_optimal_stopping(learning_data)
        }
    
    def _create_time_series_visualizations(self, monitor_data) -> Dict:
        """Create time series visualizations."""
        return {
            'step_time_series': {
                'data': [[sample.step, sample.step_time_ms] for sample in monitor_data.samples],
                'type': 'line',
                'title': 'Step Time Over Time',
                'xaxis': 'Step',
                'yaxis': 'Time (ms)'
            },
            'spike_count_time_series': {
                'data': [[sample.step, sample.total_spikes] for sample in monitor_data.samples],
                'type': 'scatter',
                'title': 'Total Spikes Over Time',
                'xaxis': 'Step',
                'yaxis': 'Total Spikes'
            },
            'neuron_activity_time_series': {
                'data': [[sample.step, sample.neurons_firing] for sample in monitor_data.samples],
                'type': 'area',
                'title': 'Neuron Activity Over Time',
                'xaxis': 'Step',
                'yaxis': 'Firing Neurons'
            }
        }
    
    def _create_performance_metrics_charts(self, monitor_data) -> Dict:
        """Create performance metrics visualizations."""
        stats = monitor_data.get_performance_summary()
        
        return {
            'metric_distribution': {
                'step_time_histogram': {
                    'data': [sample.step_time_ms for sample in monitor_data.samples],
                    'bins': 20,
                    'title': 'Step Time Distribution'
                },
                'spike_count_histogram': {
                    'data': [sample.total_spikes for sample in monitor_data.samples],
                    'bins': 20,
                    'title': 'Spike Count Distribution'
                }
            },
            'correlation_matrix': {
                'correlation_data': self._calculate_correlation_matrix(monitor_data),
                'title': 'Performance Metric Correlations'
            }
        }
    
    def _create_learning_curve_visualizations(self, monitor_data) -> Dict:
        """Create learning curve visualizations."""
        return {
            'learning_curve': {
                'data': [(i, monitor_data.stats.learning_curve[i]) 
                        for i in range(min(len(monitor_data.stats.learning_curve), 100))],
                'type': 'line',
                'title': 'Learning Rate Over Time'
            },
            'convergence_plot': {
                'data': self._calculate_convergence_points(monitor_data),
                'title': 'Learning Convergence Analysis'
            }
        }
    
    def _create_statistical_analysis(self, monitor_data) -> Dict:
        """Create statistical analysis visualizations."""
        return {
            'statistical_summary': {
                'mean_values': {
                    'step_time_ms': monitor_data.stats.avg_step_time_ms,
                    'spike_count': monitor_data.stats.avg_spikes_per_second,
                    'neurons_per_second': monitor_data.stats.avg_neurons_per_second
                },
                'confidence_intervals': self._calculate_confidence_intervals(monitor_data),
                'outlier_detection': self._detect_outliers(monitor_data)
            }
        }
    
    def _generate_spike_heatmap(self, brain_data: Dict) -> Dict:
        """Generate spike heatmap visualization."""
        # This would create a 2D heatmap showing spike density over time
        return {
            'heatmap_data': [],
            'color_scale': 'viridis',
            'title': 'Spike Density Heatmap'
        }
    
    def _generate_firing_rate_map(self, brain_data: Dict) -> Dict:
        """Generate firing rate map visualization."""
        return {
            'firing_rate_map': [],
            'color_scale': 'plasma',
            'title': 'Firing Rate Map'
        }
    
    def _generate_connectivity_heatmap(self, brain_data: Dict) -> Dict:
        """Generate connectivity heatmap visualization."""
        return {
            'connectivity_heatmap': [],
            'color_scale': 'coolwarm',
            'title': 'Connectivity Heatmap'
        }
    
    def _identify_activity_clusters(self, brain_data: Dict) -> Dict:
        """Identify and visualize activity clusters."""
        return {
            'cluster_data': [],
            'cluster_labels': [],
            'title': 'Activity Clusters'
        }
    
    def _create_trajectory_plots(self, learning_data: Dict) -> Dict:
        """Create learning trajectory plots."""
        return {
            'trajectory_plot': [],
            'title': 'Learning Trajectories'
        }
    
    def _analyze_convergence(self, learning_data: Dict) -> Dict:
        """Analyze learning convergence."""
        return {
            'convergence_points': [],
            'plateau_detection': [],
            'analysis': 'Convergence analysis would be performed here'
        }
    
    def _assess_stability(self, learning_data: Dict) -> Dict:
        """Assess learning stability."""
        return {
            'stability_metrics': [],
            'volatility_analysis': [],
            'recommendation': 'Stability assessment would be performed here'
        }
    
    def _determine_optimal_stopping(self, learning_data: Dict) -> Dict:
        """Determine optimal stopping criteria."""
        return {
            'stopping_criteria': [],
            'performance_thresholds': [],
            'recommendation': 'Optimal stopping analysis would be performed here'
        }
    
    def _calculate_correlation_matrix(self, monitor_data) -> Dict:
        """Calculate correlation matrix for performance metrics."""
        # This would calculate correlations between different metrics
        return {}
    
    def _calculate_convergence_points(self, monitor_data) -> List:
        """Calculate convergence points in learning curve."""
        # This would identify when learning plateaus
        return []
    
    def _calculate_confidence_intervals(self, monitor_data) -> Dict:
        """Calculate confidence intervals for performance metrics."""
        # This would calculate statistical confidence intervals
        return {}
    
    def _detect_outliers(self, monitor_data) -> Dict:
        """Detect outliers in performance data."""
        # This would identify anomalous performance patterns
        return {}

def demonstrate_advanced_example_1():
    """
    Demonstrate advanced example 1: Adaptive network configuration.
    """
    print("=== Advanced Example 1: Adaptive Network Configuration ===")
    
    setup = AdvancedSimulationSetup()
    
    # Create adaptive network for different efficiency targets
    for efficiency in [0.5, 0.8, 0.95]:
        config = setup.create_adaptive_network(efficiency)
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print(f"Efficiency target {efficiency}: {brain.getTotalNeuronCount()} neurons")
        
        # Run simulation
        for step in range(100):
            brain.step(step)
        
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Firing rate: {brain.getAverageFiringRate():.2f} Hz")
        print()

def demonstrate_advanced_example_2():
    """
    Demonstrate advanced example 2: Hierarchical agent control system.
    """
    print("=== Advanced Example 2: Hierarchical Agent Control System ===")
    
    controller = AdvancedAgentController()
    
    # Create multiple agents
    agents = []
    for i in range(3):
        config = pynlm.createAdvancedConfig(1000, 0.05)
        brain = pynlm.createBrain(config)
        brain.initialize()
        agent = pynlm.createAgentBrain(brain)
        agents.append(agent)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Initialize agents
    for agent in agents:
        agent.initialize(world)
    
    # Create hierarchical control system
    control_system = controller.create_hierarchical_control_system(world, agents)
    
    print(f"Created hierarchical control system with {len(agents)} agents")
    print(f"Control layers: {list(control_system.keys())}")
    print()

def demonstrate_advanced_example_3():
    """
    Demonstrate advanced example 3: Parameter sweep experiment.
    """
    print("=== Advanced Example 3: Parameter Sweep Experiment ===")
    
    framework = AdvancedExperimentFramework()
    
    # Create parameter space
    parameter_space = framework.create_parameter_search_space()
    
    print(f"Created parameter space with {len(parameter_space)} categories")
    for category in parameter_space:
        print(f"  {category}: {len(parameter_space[category])} parameters")
    
    # Run parameter sweep (fewer trials for demonstration)
    results = framework.run_parameter_sweep(parameter_space, num_trials=5)
    
    print(f"Ran {len(results['results'])} simulation trials")
    print(f"Best configuration: {results['best_configuration']}")
    print()

def demonstrate_advanced_example_4():
    """
    Demonstrate advanced example 4: Performance visualization tools.
    """
    print("=== Advanced Example 4: Performance Visualization Tools ===")
    
    visualization = AdvancedVisualizationTools()
    
    # Create sample performance data (for demonstration)
    class SampleMonitor:
        def __init__(self):
            self.samples = []
            self.stats = pynlm.PerformanceStats()
        
        def get_performance_summary(self):
            return {
                'avg_step_time_ms': 1.5,
                'avg_spikes_per_second': 100,
                'avg_neurons_per_second': 500
            }
    
    monitor_data = SampleMonitor()
    
    # Create performance dashboard
    dashboard = visualization.create_performance_dashboard(monitor_data)
    
    print("Created performance dashboard with components:")
    for component in dashboard:
        print(f"  {component}")
    
    print("Performance dashboard created successfully!")
    print()

def main():
    """
    Run all advanced examples.
    """
    print("Advanced NLM Examples for Expert Users")
    print("=" * 50)
    print()
    
    # Run advanced examples
    demonstrate_advanced_example_1()
    demonstrate_advanced_example_2()
    demonstrate_advanced_example_3()
    demonstrate_advanced_example_4()
    
    print("All advanced examples completed successfully!")

if __name__ == "__main__":
    main()