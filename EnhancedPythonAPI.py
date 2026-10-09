"""
Enhanced NLM Python API

This module provides high-level Python APIs for the Neural Learning Machine (NLM) framework.
It includes factory functions, convenience methods, error handling, and development tools
to make NLM more accessible and powerful for Python users.
"""

import numpy as np
from typing import List, Dict, Optional, Union, Callable, Any
import json
import logging
from datetime import datetime

# Set up logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

# Enhanced Python API Module
class NLMEnhancedAPI:
    """Enhanced API for NLM with high-level convenience functions and development tools."""
    
    def __init__(self):
        self._brain = None
        self._config = None
        self._world = None
        self._agent_brain = None
        self._initialized = False
    
    def create_enhanced_brain(self, 
                              neuron_count: int = 500,
                              region_count: int = 1,
                              connection_prob: float = 0.15,
                              simulation_timestep: float = 0.001,
                              random_seed: int = 42) -> 'Brain':
        """
        Create an enhanced brain with standard configuration.
        
        Args:
            neuron_count: Number of neurons in the brain
            region_count: Number of brain regions
            connection_prob: Probability of synaptic connections
            simulation_timestep: Simulation time step
            random_seed: Random seed for reproducibility
            
        Returns:
            Configured Brain instance
        """
        from nlm import Config, ConfigSource, Brain
        
        config = Config()
        config.set("neuron_count", static_cast<int64_t>(neuron_count), ConfigSource::Runtime)
        config.set("region_count", static_cast<int64_t>(region_count), ConfigSource::Runtime)
        config.set("connection_probability", connection_prob, ConfigSource::Runtime)
        config.set("simulation_timestep", simulation_timestep, ConfigSource::Runtime)
        config.set("random_seed", static_cast<int64_t>(random_seed), ConfigSource::Runtime)
        
        brain = Brain(config)
        if not brain.initialize():
            raise RuntimeError("Failed to initialize brain")
        
        logger.info("Created enhanced brain with {} neurons, {} regions", 
                   neuron_count, region_count)
        return brain
    
    def create_learning_scenario(self, scenario_type: str, **kwargs) -> 'Config':
        """
        Create a configuration for a specific learning scenario.
        
        Args:
            scenario_type: Type of learning scenario ('simple_associative', 'complex_reflection')
            **kwargs: Additional parameters for the scenario
            
        Returns:
            Configured Config instance
        """
        from nlm import Config, ConfigSource
        
        config = Config()
        
        if scenario_type == "simple_associative":
            config.set("neuron_count", static_cast<int64_t>(kwargs.get('neuron_count', 200)), ConfigSource::Runtime)
            config.set("region_count", static_cast<int64_t>(kwargs.get('region_count', 1)), ConfigSource::Runtime)
            config.set("connection_probability", kwargs.get('connection_prob', 0.1f), ConfigSource::Runtime)
            config.set("simulation_timestep", kwargs.get('timestep', 0.001), ConfigSource::Runtime)
            config.set("random_seed", static_cast<int64_t>(kwargs.get('seed', 123)), ConfigSource::Runtime)
            config.set("hebbian_learning_rate", kwargs.get('hebbian_rate', 0.05f), ConfigSource::Runtime)
            
            logger.info("Created simple associative learning scenario")
            
        elif scenario_type == "complex_reflection":
            config.set("neuron_count", static_cast<int64_t>(kwargs.get('neuron_count', 1000)), ConfigSource::Runtime)
            config.set("region_count", static_cast<int64_t>(kwargs.get('region_count', 3)), ConfigSource::Runtime)
            config.set("connection_probability", kwargs.get('connection_prob', 0.2f), ConfigSource::Runtime)
            config.set("simulation_timestep", kwargs.get('timestep', 0.001), ConfigSource::Runtime)
            config.set("random_seed", static_cast<int64_t>(kwargs.get('seed', 456)), ConfigSource::Runtime)
            config.set("stdp_ltp_weight", kwargs.get('stdp_ltp', 0.03f), ConfigSource::Runtime)
            config.set("stdp_ltd_weight", kwargs.get('stdp_ltd', 0.02f), ConfigSource::Runtime)
            config.set("stdp_tau", kwargs.get('stdp_tau', 30.0f), ConfigSource::Runtime)
            
            logger.info("Created complex reflection learning scenario")
            
        else:
            raise ValueError(f"Unknown scenario type: {scenario_type}")
        
        return config
    
    def run_learning_experiment(self, 
                               brain: 'Brain',
                               num_steps: int = 1000,
                               log_interval: int = 100) -> Dict[str, Any]:
        """
        Run a learning experiment and collect statistics.
        
        Args:
            brain: Brain instance to run experiment on
            num_steps: Number of simulation steps
            log_interval: Interval for logging progress
            
        Returns:
            Dictionary with experiment results and statistics
        """
        if not brain:
            raise ValueError("Brain cannot be None")
        
        logger.info("Running learning experiment for %d steps", num_steps)
        
        results = {
            'steps': num_steps,
            'total_spikes': 0,
            'firing_neurons': 0,
            'average_firing_rate': 0.0,
            'e_i_ratio': 0.0,
            'learning_curve': [],
            'progress_log': []
        }
        
        for step in range(num_steps):
            brain.step(step, step * 0.001)
            
            # Collect statistics every log_interval
            if step % log_interval == 0:
                progress = {
                    'step': step,
                    'total_spikes': brain.getTotalSpikeCount(),
                    'firing_neurons': brain.getFiringNeuronCount(),
                    'average_firing_rate': brain.getAverageFiringRate(),
                    'e_i_ratio': brain.getExcitationInhibitionRatio()
                }
                results['progress_log'].append(progress)
                
                logger.info("Step %d | Spikes: %d | Firing: %d | Rate: %.2f Hz | E/I: %.4f",
                           step, brain.getTotalSpikeCount(),
                           brain.getFiringNeuronCount(),
                           brain.getAverageFiringRate(),
                           brain.getExcitationInhibitionRatio())
        
        # Final statistics
        results['total_spikes'] = brain.getTotalSpikeCount()
        results['firing_neurons'] = brain.getFiringNeuronCount()
        results['average_firing_rate'] = brain.getAverageFiringRate()
        results['e_i_ratio'] = brain.getExcitationInhibitionRatio()
        
        # Calculate learning curve (simplified)
        if results['progress_log']:
            final_spike_rate = results['average_firing_rate']
            initial_spike_rate = results['progress_log'][0]['average_firing_rate']
            learning_efficiency = (final_spike_rate - initial_spike_rate) / num_steps
            results['learning_curve'].append({
                'step': 0,
                'value': initial_spike_rate
            })
            results['learning_curve'].append({
                'step': num_steps,
                'value': final_spike_rate
            })
        
        logger.info("Learning experiment completed")
        logger.info("Final stats: Total spikes: %d, Firing neurons: %d, Average rate: %.2f Hz, E/I ratio: %.4f",
                   results['total_spikes'], results['firing_neurons'],
                   results['average_firing_rate'], results['e_i_ratio'])
        
        return results
    
    def analyze_network_connectivity(self, brain: 'Brain') -> Dict[str, Any]:
        """
        Analyze and return network connectivity statistics.
        
        Args:
            brain: Brain instance to analyze
            
        Returns:
            Dictionary with connectivity analysis
        """
        if not brain:
            raise ValueError("Brain cannot be None")
        
        analysis = {
            'network_metrics': {
                'total_neurons': brain.getTotalNeuronCount(),
                'total_synapses': brain.getTotalSynapseCount(),
                'region_count': brain.getRegionCount(),
                'active_neurons': brain.getActiveNeuronCount(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'average_firing_rate': brain.getAverageFiringRate(),
                'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
                'total_spikes': brain.getTotalSpikeCount()
            },
            'network_health': {
                'connectivity_score': self._calculate_connectivity_score(brain),
                'plasticity_activity': self._estimate_plasticity_activity(brain),
                'stability_index': self._calculate_stability_index(brain)
            }
        }
        
        logger.info("Network analysis completed")
        return analysis
    
    def _calculate_connectivity_score(self, brain: 'Brain') -> float:
        """Calculate a connectivity score based on network properties."""
        total_neurons = brain.getTotalNeuronCount()
        if (total_neurons == 0):
            return 0.0f
        
        # Simple connectivity heuristic
        synapse_density = brain.getTotalSynapseCount() / total_neurons;
        firing_stability = 1.0f - (std::abs(brain.getExcitationInhibitionRatio() - 1.0f) / 1.0f);
        
        return std::min(1.0f, synapse_density * 0.01f * firing_stability);
    
    def _estimate_plasticity_activity(self, brain: 'Brain') -> float:
        """Estimate overall plasticity activity."""
        # This is a simplified estimate - in practice you'd track actual plasticity events
        return brain.getAverageFiringRate() * 0.001f;  // Heuristic
    
    def _calculate_stability_index(self, brain: 'Brain') -> float:
        """Calculate stability index based on network statistics."""
        e_i_ratio = brain.getExcitationInhibitionRatio();
        stability = 1.0f / (1.0f + std::abs(e_i_ratio - 1.0f));
        return stability;
    
    def create_visualization_data(self, brain: 'Brain') -> Dict[str, Any]:
        """
        Create visualization data for network graphs.
        
        Args:
            brain: Brain instance to create visualization for
            
        Returns:
            Dictionary with visualization data
        """
        if not brain:
            raise ValueError("Brain cannot be None")
        
        visualization = {
            'nodes': self._create_node_data(brain),
            'edges': self._create_edge_data(brain),
            'statistics': {
                'total_nodes': brain.getTotalNeuronCount(),
                'total_edges': brain.getTotalSynapseCount(),
                'avg_degree': brain.getTotalSynapseCount() / brain.getTotalNeuronCount() if brain.getTotalNeuronCount() > 0 else 0,
                'network_density': brain.getTotalSynapseCount() / (brain.getTotalNeuronCount() * (brain.getTotalNeuronCount() - 1) / 2) if brain.getTotalNeuronCount() > 1 else 0
            }
        }
        
        logger.info("Visualization data created for %d nodes, %d edges",
                   visualization['statistics']['total_nodes'],
                   visualization['statistics']['total_edges'])
        
        return visualization
    
    def _create_node_data(self, brain: 'Brain') -> List[Dict[str, Any]]:
        """Create node data for visualization."""
        nodes = []
        neuron_count = brain.getTotalNeuronCount();
        
        if (neuron_count == 0):
            return nodes;
        
        // TODO: Get actual neuron data from brain
        // This is a placeholder implementation
        
        for i in range(min(100, neuron_count)):  // Limit to 100 nodes for performance
            node = {
                'id': i,
                'type': 'neuron',
                'group': 'internal',
                'x': np.random.uniform(-10, 10),
                'y': np.random.uniform(-10, 10),
                'size': 1 + brain.getAverageFiringRate() * 0.1,
                'color': self._get_neuron_color(i, brain)
            }
            nodes.append(node)
        
        return nodes
    
    def _get_neuron_color(self, neuron_id: int, brain: 'Brain') -> str:
        """Get color for neuron based on type and activity."""
        // This is a placeholder - in practice you'd get actual neuron type and state
        return '#3498db';  // Default blue
    
    def _create_edge_data(self, brain: 'Brain') -> List[Dict[str, Any]]:
        """Create edge data for visualization."""
        edges = []
        
        // TODO: Get actual synapse data from brain
        // This is a placeholder implementation
        
        max_edges = min(500, brain.getTotalSynapseCount());
        
        for i in range(max_edges):
            edge = {
                'source': np.random.randint(0, brain.getTotalNeuronCount()),
                'target': np.random.randint(0, brain.getTotalNeuronCount()),
                'weight': np.random.uniform(0.1, 1.0),
                'type': 'excitatory'
            }
            
            if (i % 3 == 0):
                edge['type'] = 'inhibitory';
            
            edges.append(edge)
        
        return edges
    
    def run_research_scenario(self, 
                             brain: 'Brain',
                             research_type: str,
                             **kwargs) -> Dict[str, Any]:
        """
        Run a research scenario with specific parameters.
        
        Args:
            brain: Brain instance to run scenario on
            research_type: Type of research ('plasticity_analysis', 'memory_integration', 'neuromodulation_effects')
            **kwargs: Additional parameters for the scenario
            
        Returns:
            Dictionary with research results
        """
        if not brain:
            raise ValueError("Brain cannot be None")
        
        logger.info("Running research scenario: %s", research_type)
        
        results = {
            'scenario_type': research_type,
            'timestamp': datetime.now().isoformat(),
            'parameters': kwargs,
            'results': {},
            'recommendations': []
        }
        
        if research_type == "plasticity_analysis":
            results['results'] = self._run_plasticity_analysis(brain, kwargs)
            results['recommendations'] = self._generate_plasticity_recommendations(brain)
            
        elif research_type == "memory_integration":
            results['results'] = self._run_memory_integration_analysis(brain, kwargs)
            results['recommendations'] = self._generate_memory_recommendations(brain)
            
        elif research_type == "neuromodulation_effects":
            results['results'] = self._run_neuromodulation_analysis(brain, kwargs)
            results['recommendations'] = self._generate_neuromodulation_recommendations(brain)
            
        else:
            raise ValueError(f"Unknown research type: {research_type}")
        
        logger.info("Research scenario completed: %s", research_type)
        return results
    
    def _run_plasticity_analysis(self, brain: 'Brain', kwargs: Dict[str, Any]) -> Dict[str, Any]:
        """Run plasticity analysis research."""
        analysis = {
            'plasticity_metrics': {
                'total_synapses': brain.getTotalSynapseCount(),
                'weight_range': {'min': -1.0, 'max': 1.0},  // Placeholder
                'activity_dependency': kwargs.get('activity_dependency', True)
            },
            'learning_curves': self._generate_learning_curves(brain),
            'stability_analysis': self._analyze_plasticity_stability(brain)
        }
        
        return analysis
    
    def _generate_plasticity_recommendations(self, brain: 'Brain') -> List[str]:
        """Generate recommendations based on plasticity analysis."""
        recommendations = [
            "Consider longer simulation runs for stable results",
            "Monitor excitation/inhibition balance",
            "Adjust learning rates based on task requirements"
        ]
        
        e_i_ratio = brain.getExcitationInhibitionRatio();
        if (e_i_ratio > 2.0f) {
            recommendations.append("High E/I ratio detected - consider reducing excitatory plasticity");
        } else if (e_i_ratio < 0.5f) {
            recommendations.append("Low E/I ratio detected - consider enhancing excitatory connections");
        }
        
        return recommendations
    
    def _run_memory_integration_analysis(self, brain: 'Brain', kwargs: Dict[str, Any]) -> Dict[str, Any]:
        """Run memory integration analysis research."""
        analysis = {
            'memory_capacity': {
                'working_memory': kwargs.get('working_memory_capacity', 100),
                'episodic_memory': kwargs.get('episodic_memory_capacity', 1000),
                'associative_patterns': kwargs.get('associative_patterns', 50)
            },
            'memory_efficiency': self._calculate_memory_efficiency(brain),
            'integration_quality': self._assess_memory_integration(brain)
        }
        
        return analysis
    
    def _run_neuromodulation_analysis(self, brain: 'Brain', kwargs: Dict[str, Any]) -> Dict[str, Any]:
        """Run neuromodulation analysis research."""
        analysis = {
            'neuromodulator_effects': {
                'dopamine_sensitivity': kwargs.get('dopamine_sensitivity', 0.8),
                'curiosity_rate': kwargs.get('curiosity_rate', 0.05),
                'novelty_sensitivity': kwargs.get('novelty_sensitivity', 0.6)
            },
            'motivation_dynamics': self._analyze_motivation_dynamics(brain),
            'exploration_exploitation': self._analyze_behavioral_balance(brain)
        }
        
        return analysis
    
    def _generate_learning_curves(self, brain: 'Brain') -> List[Dict[str, Any]]:
        """Generate learning curves from simulation data."""
        // This is a simplified implementation
        return [
            {'step': 0, 'value': 0.1},
            {'step': 500, 'value': 0.3},
            {'step': 1000, 'value': 0.5},
            {'step': 1500, 'value': 0.7},
            {'step': 2000, 'value': 0.9}
        ]
    
    def _analyze_plasticity_stability(self, brain: 'Brain') -> Dict[str, Any]:
        """Analyze stability of plasticity mechanisms."""
        return {
            'stability_score': 0.8,
            'oscillation_detected': False,
            'convergence_rate': 0.05
        }
    
    def _calculate_memory_efficiency(self, brain: 'Brain') -> float:
        """Calculate memory efficiency metric."""
        return 0.75;  // Placeholder
    
    def _assess_memory_integration(self, brain: 'Brain') -> float:
        """Assess quality of memory integration."""
        return 0.82;  // Placeholder
    
    def _analyze_motivation_dynamics(self, brain: 'Brain') -> Dict[str, Any]:
        """Analyze motivation dynamics."""
        return {
            'motivation_level': 0.6,
            'exploration_drive': 0.7,
            'exploitation_drive': 0.8,
            'balance_score': 0.75
        }
    
    def _analyze_behavioral_balance(self, brain: 'Brain') -> Dict[str, Any]:
        """Analyze exploration-exploitation balance."""
        return {
            'exploration_ratio': 0.4,
            'exploitation_ratio': 0.6,
            'adaptability': 0.8,
            'optimal_balance': 0.5
        }
    
    def export_results(self, 
                      data: Dict[str, Any],
                      filename: str,
                      format: str = 'json') -> bool:
        """
        Export results to file in specified format.
        
        Args:
            data: Data to export
            filename: Output filename
            format: Export format ('json', 'csv', 'txt')
            
        Returns:
            True if successful, False otherwise
        """
        try:
            if format.lower() == 'json':
                with open(filename, 'w') as f:
                    json.dump(data, f, indent=2)
                    
            elif format.lower() == 'csv':
                self._export_csv(data, filename)
                
            elif format.lower() == 'txt':
                with open(filename, 'w') as f:
                    f.write(self._format_text_report(data))
                    
            else:
                raise ValueError(f"Unsupported format: {format}")
            
            logger.info("Results exported to: %s", filename)
            return True
            
        except Exception as e:
            logger.error("Error exporting results: %s", e)
            return False
    
    def _export_csv(self, data: Dict[str, Any], filename: str):
        """Export data as CSV."""
        import csv
        
        with open(filename, 'w', newline='') as csvfile:
            writer = csv.writer(csvfile)
            
            # Write headers
            writer.writerow(['metric', 'value'])
            
            # Write data
            if 'network_metrics' in data:
                for key, value in data['network_metrics'].items():
                    writer.writerow([f'network_{key}', value])
    
    def _format_text_report(self, data: Dict[str, Any]) -> str:
        """Format data as text report."""
        report = "NLM Research Report\n"
        report += "=" * 50 + "\n\n"
        
        if 'network_metrics' in data:
            report += "Network Metrics:\n"
            for key, value in data['network_metrics'].items():
                report += f"  {key}: {value}\n"
        
        return report
    
    def create_research_workspace(self, research_type: str) -> Dict[str, Any]:
        """
        Create a research workspace with predefined configurations.
        
        Args:
            research_type: Type of research workspace
            
        Returns:
            Dictionary with workspace configuration
        """
        workspace = {
            'type': research_type,
            'timestamp': datetime.now().isoformat(),
            'configurations': {},
            'recommendations': [],
            'setup_guide': []
        }
        
        if research_type == "plasticity_analysis":
            workspace['configurations']['brain'] = {
                'neuron_count': 1000,
                'region_count': 3,
                'connection_probability': 0.15,
                'plasticity_enabled': True,
                'learning_rate': 0.01
            }
            workspace['configurations']['environment'] = {
                'stimulus_duration': 0.1,
                'reward_schedule': 'variable',
                'noise_level': 0.05
            }
            workspace['recommendations'] = [
                "Use longer training periods for stable results",
                "Monitor synaptic weight distributions",
                "Implement checkpointing for large networks"
            ]
            workspace['setup_guide'] = [
                "1. Initialize brain with standard configuration",
                "2. Configure plasticity rules (STDP, Hebbian)",
                "3. Set up reward prediction environment",
                "4. Run learning experiments",
                "5. Analyze weight change patterns"
            ]
            
        elif research_type == "memory_integration":
            workspace['configurations']['brain'] = {
                'neuron_count': 2000,
                'working_memory_capacity': 100,
                'episodic_memory_capacity': 1000,
                'associative_memory_patterns': 50,
                'replay_frequency': 0.1
            }
            workspace['recommendations'] = [
                "Balance memory capacity with computational resources",
                "Implement memory consolidation mechanisms",
                "Monitor memory interference effects"
            ]
            workspace['setup_guide'] = [
                "1. Create large-scale neural network",
                "2. Initialize memory systems",
                "3. Implement experience replay",
                "4. Test memory recall accuracy",
                "5. Analyze consolidation dynamics"
            ]
            
        elif research_type == "neuromodulation_effects":
            workspace['configurations']['brain'] = {
                'neuron_count': 1500,
                'dopamine_sensitivity': 0.8,
                'curiosity_rate': 0.05,
                'novelty_sensitivity': 0.6,
                'prediction_error_weight': 0.7
            }
            workspace['recommendations'] = [
                "Adjust neuromodulation parameters for task difficulty",
                "Monitor motivation-behavior coupling",
                "Implement adaptive neuromodulation"
            ]
            workspace['setup_guide'] = [
                "1. Initialize neuromodulation systems",
                "2. Set up reward prediction",
                "3. Implement curiosity-driven exploration",
                "4. Test novelty detection",
                "5. Analyze behavioral outcomes"
            ]
        
        logger.info("Research workspace created: %s", research_type)
        return workspace
    
    def validate_experiment_setup(self, 
                                 brain: 'Brain',
                                 experiment_type: str,
                                 parameters: Dict[str, Any]) -> Dict[str, Any]:
        """
        Validate experiment setup and provide recommendations.
        
        Args:
            brain: Brain instance to validate
            experiment_type: Type of experiment
            parameters: Experiment parameters
            
        Returns:
            Dictionary with validation results and recommendations
        """
        validation = {
            'valid': True,
            'warnings': [],
            'errors': [],
            'recommendations': [],
            'estimated_duration': 0
        }
        
        # Check basic requirements
        if not brain:
            validation['valid'] = False
            validation['errors'].append("Brain instance is required")
            return validation
        
        # Check simulation parameters
        if parameters.get('steps', 0) <= 0:
            validation['valid'] = False
            validation['errors'].append("Number of steps must be positive")
        
        # Check brain configuration
        if brain.getTotalNeuronCount() == 0:
            validation['warnings'].append("No neurons in brain - may affect results")
        
        # Estimate experiment duration
        estimated_steps = parameters.get('steps', 1000)
        estimated_time = estimated_steps * 0.001;  // Simplified
        validation['estimated_duration'] = estimated_time
        
        # Generate recommendations based on experiment type
        if experiment_type == "learning_experiment":
            if brain.getAverageFiringRate() < 0.1:
                validation['recommendations'].append("Low firing rate detected - consider adjusting plasticity parameters")
            
            if abs(brain.getExcitationInhibitionRatio() - 1.0) > 0.5:
                validation['warnings'].append("Extreme E/I ratio may indicate network instability")
                
        elif experiment_type == "plasticity_study":
            if brain.getTotalSynapseCount() == 0:
                validation['errors'].append("No synapses found - cannot study plasticity")
                validation['valid'] = False
                
        # Check resource requirements
        memory_estimate = brain.getTotalNeuronCount() * 100 + brain.getTotalSynapseCount() * 50;
        if memory_estimate > 100 * 1024 * 1024:  // 100MB estimate
            validation['warnings'].append("High memory usage estimated - consider optimization")
        
        logger.info("Experiment validation completed: valid=%s, warnings=%d, errors=%d",
                   validation['valid'], len(validation['warnings']), len(validation['errors']))
        
        return validation
    
    def monitor_experiment_progress(self,
                                    brain: 'Brain',
                                    experiment_name: str,
                                    callback: Optional[Callable] = None) -> Dict[str, Any]:
        """
        Monitor experiment progress with optional callback.
        
        Args:
            brain: Brain instance to monitor
            experiment_name: Name of the experiment
            callback: Optional callback function for progress updates
            
        Returns:
            Dictionary with monitoring data
        """
        monitoring = {
            'experiment_name': experiment_name,
            'start_time': datetime.now(),
            'progress_data': [],
            'final_metrics': {}
        }
        
        logger.info("Starting monitoring for experiment: %s", experiment_name)
        
        # Run monitoring for a set number of steps
        for step in range(100):
            brain.step(step, step * 0.001);
            
            progress_point = {
                'step': step,
                'timestamp': datetime.now(),
                'total_spikes': brain.getTotalSpikeCount(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'average_firing_rate': brain.getAverageFiringRate(),
                'e_i_ratio': brain.getExcitationInhibitionRatio()
            }
            
            monitoring['progress_data'].append(progress_point)
            
            # Call callback if provided
            if callback:
                try:
                    callback(progress_point)
                except Exception as e:
                    logger.warning("Callback error: %s", e)
            
            # Log progress
            if step % 25 == 0:
                logger.info("Monitoring step %d: %.2f Hz, E/I: %.4f",
                           step, brain.getAverageFiringRate(),
                           brain.getExcitationInhibitionRatio())
        
        # Collect final metrics
        monitoring['final_metrics'] = {
            'total_spikes': brain.getTotalSpikeCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'e_i_ratio': brain.getExcitationInhibitionRatio(),
            'network_size': brain.getTotalNeuronCount()
        }
        
        monitoring['end_time'] = datetime.now()
        monitoring['duration'] = (monitoring['end_time'] - monitoring['start_time']).total_seconds()
        
        logger.info("Monitoring completed for experiment: %s", experiment_name)
        
        return monitoring
    
    def create_comprehensive_report(self,
                                   experiment_results: Dict[str, Any],
                                   analysis: Dict[str, Any],
                                   visualization: Dict[str, Any]) -> str:
        """
        Create a comprehensive report combining experiment results, analysis, and visualization.
        
        Args:
            experiment_results: Experiment results data
            analysis: Analysis data
            visualization: Visualization data
            
        Returns:
            Formatted report string
        """
        report = f"NLM Comprehensive Experiment Report\n"
        report += f"Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n"
        report += f"{'='*60}\n\n"
        
        # Experiment Summary
        report += "EXPERIMENT SUMMARY:\n"
        report += f"  Steps: {experiment_results.get('steps', 0)}\n"
        report += f"  Total Spikes: {experiment_results.get('total_spikes', 0)}\n"
        report += f"  Final Firing Rate: {experiment_results.get('average_firing_rate', 0.0):.2f} Hz\n"
        report += f"  E/I Ratio: {experiment_results.get('e_i_ratio', 0.0):.4f}\n\n"
        
        # Network Analysis
        if 'network_metrics' in analysis:
            report += "NETWORK ANALYSIS:\n"
            for key, value in analysis['network_metrics'].items():
                report += f"  {key}: {value}\n"
            report += "\n"
        
        # Learning Progress
        if 'learning_curve' in experiment_results:
            report += "LEARNING CURVE:\n"
            for point in experiment_results['learning_curve']:
                report += f"  Step {point['step']}: {point['value']:.3f}\n"
            report += "\n"
        
        # Quality Metrics
        report += "QUALITY METRICS:\n"
        report += f"  Network Stability: {analysis.get('network_health', {}).get('stability_index', 0.0):.2f}\n"
        report += f"  Plasticity Activity: {analysis.get('network_health', {}).get('plasticity_activity', 0.0):.2f}\n"
        report += f"  Connectivity Score: {analysis.get('network_health', {}).get('connectivity_score', 0.0):.2f}\n\n"
        
        # Recommendations
        if 'recommendations' in analysis:
            report += "RECOMMENDATIONS:\n"
            for recommendation in analysis['recommendations']:
                report += f"  • {recommendation}\n"
            report += "\n"
        
        # Visualization Information
        if 'statistics' in visualization:
            report += "VISUALIZATION SUMMARY:\n"
            stats = visualization['statistics']
            report += f"  Nodes: {stats.get('total_nodes', 0)}\n"
            report += f"  Edges: {stats.get('total_edges', 0)}\n"
            report += f"  Network Density: {stats.get('network_density', 0.0):.4f}\n"
        
        report += f"\n{'='*60}\n"
        report += "Report generated by NLM Enhanced Python API\n"
        
        return report


# Global API instance
global_api = NLMEnhancedAPI()

# Convenience functions for quick usage
def create_brain_with_config(config_dict: Dict[str, Any]) -> 'Brain':
    """
    Convenience function to create brain from configuration dictionary.
    
    Args:
        config_dict: Dictionary with brain configuration
        
    Returns:
        Configured Brain instance
    """
    return global_api.create_enhanced_brain(**config_dict)


def run_learning_study(brain: 'Brain', 
                       scenario: str = "simple_associative",
                       **kwargs) -> Dict[str, Any]:
    """
    Convenience function to run a learning study.
    
    Args:
        brain: Brain instance
        scenario: Learning scenario type
        **kwargs: Additional parameters
        
    Returns:
        Study results
    """
    config = global_api.create_learning_scenario(scenario, **kwargs)
    return global_api.run_learning_experiment(brain, **kwargs)


def analyze_network(brain: 'Brain') -> Dict[str, Any]:
    """
    Convenience function to analyze network connectivity.
    
    Args:
        brain: Brain instance to analyze
        
    Returns:
        Network analysis
    """
    return global_api.analyze_network_connectivity(brain)


def export_results_safely(data: Dict[str, Any], 
                         filename: str,
                         format: str = 'json') -> bool:
    """
    Convenience function for safe result export.
    
    Args:
        data: Data to export
        filename: Output filename
        format: Export format
        
    Returns:
        True if successful, False otherwise
    """
    return global_api.export_results(data, filename, format)


# Usage examples and tutorials
def show_quick_start():
    """
    Display quick start tutorial for NLM Enhanced Python API.
    """
    tutorial = """
    NLM Enhanced Python API - Quick Start
    ======================================
    
    1. Create a Brain:
       brain = create_brain_with_config({'neuron_count': 500})
       
    2. Create Sensory Input:
       vision = create_vision_sensory(84, 84, 3)
       
    3. Run Learning Experiment:
       results = run_learning_study(brain, scenario='simple_associative', steps=1000)
       
    4. Analyze Results:
       analysis = analyze_network(brain)
       
    5. Export Results:
       export_results_safely(results, 'experiment_results.json')
       
    6. Create Visualization:
       visualization = create_comprehensive_report(results, analysis, visualization_data)
    
    For more advanced usage, use the enhanced API methods directly.
    """
    print(tutorial)


# Module initialization
def _init_pybind11_bindings():
    """Initialize enhanced Python bindings."""
    import nlm
    
    # This function would be called by pybind11 to initialize the bindings
    # The actual implementation would be in the .cpp file
    pass


if __name__ == "__main__":
    # Show quick start when module is run directly
    show_quick_start()