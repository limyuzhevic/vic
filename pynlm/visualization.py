'''Visualization utilities for pynlm'''

from __future__ import annotations

from typing import List, Optional, Any, Dict, Tuple
import math
import json

from .core.types import (
    NeuronId, SynapseId, RegionId, PopulationId, SimulationStep, Timestamp
)
from .core.enums import (
    NeuronType, SynapseType, DevelopmentalStage, FiringState,
    ActionType, MotorCommand, WorldObjectType
)
from .brain import Brain, NeuralRegion
from .world import SimpleWorld
from .agent import AgentBrain


class VisualizationUtils:
    """Visualization utilities for NLM system.
    
    The VisualizationUtils class provides tools for visualizing neural networks,
    simulations, and agent behavior. It includes functionality for generating
    plots, creating HTML visualizations, and analyzing simulation data.
    """
    
    def __init__(self):
        """Initialize VisualizationUtils instance."""
        self.plotting_backend: str = "matplotlib"
        self.html_template: str = ""
        self.default_colors = {
            'excitatory_neuron': '#FF6B6B',
            'inhibitory_neuron': '#4ECDC4',
            'modulatory_neuron': '#45B7D1',
            'sensory_neuron': '#96CEB4',
            'motor_neuron': '#FFEAA7',
            'internal_neuron': '#DDA0DD',
            'active_neuron': '#FF5722',
            'resting_neuron': '#E0E0E0',
            'firing_neuron': '#FF9800',
            'synapse_excitatory': '#FF6B6B',
            'synapse_inhibitory': '#4ECDC4',
            'synapse_modulatory': '#45B7D1',
            'region_background': '#F8F9FA',
            'agent': '#9B59B6',
            'resource': '#27AE60',
            'hazard': '#E74C3C',
            'wall': '#34495E',
            'marker': '#F39C12',
        }
    
    def create_neural_network_plot(self, brain: Brain, 
                                   output_file: str = "neural_network.png"):
        """Create a visualization of the neural network structure.
        
        Args:
            brain: Brain instance to visualize.
            output_file: Path to save the visualization.
        """
        # In a real implementation, this would create a network graph plot
        # For now, it creates a text-based summary
        
        plot_data = {
            'type': 'neural_network',
            'brain_id': brain.brain_id,
            'region_count': brain.getRegionCount(),
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'regions': self._create_region_data(brain),
            'statistics': self._calculate_network_statistics(brain),
        }
        
        self._save_plot_data(plot_data, output_file)
        return plot_data
    
    def create_simulation_animation(self, brain: Brain, world: SimpleWorld,
                                     steps: int, output_file: str = "simulation.gif"):
        """Create an animation of simulation over time.
        
        Args:
            brain: Brain instance to simulate.
            world: World instance to simulate.
            steps: Number of simulation steps to animate.
            output_file: Path to save the animation.
        """
        animation_data = {
            'type': 'simulation_animation',
            'steps': steps,
            'world_dimensions': (world.getWidth(), world.getHeight()),
            'frames': self._create_animation_frames(brain, world, steps),
            'statistics': self._calculate_simulation_statistics(brain, world, steps),
        }
        
        self._save_animation_data(animation_data, output_file)
        return animation_data
    
    def create_behavior_plot(self, agent_brain: AgentBrain, 
                             output_file: str = "behavior.png"):
        """Create a visualization of agent behavior over time.
        
        Args:
            agent_brain: AgentBrain instance to visualize.
            output_file: Path to save the visualization.
        """
        behavior_data = {
            'type': 'behavior_plot',
            'total_steps': agent_brain.step_count,
            'total_actions': agent_brain.action_count,
            'developmental_stage': agent_brain.getDevelopmentalStage().name,
            'neuromodulation_level': agent_brain.getNeuromodulationLevel(),
            'curiosity_level': agent_brain.getCuriosityLevel(),
            'novelty_level': agent_brain.getNoveltyLevel(),
            'prediction_error': agent_brain.getPredictionError(),
            'time_series': self._create_behavior_time_series(agent_brain),
            'action_distribution': self._calculate_action_distribution(agent_brain),
        }
        
        self._save_plot_data(behavior_data, output_file)
        return behavior_data
    
    def create_html_visualization(self, brain: Brain, world: SimpleWorld,
                                   agent_brain: Optional[AgentBrain] = None,
                                   output_file: str = "visualization.html"):
        """Create an HTML visualization of the NLM system.
        
        Args:
            brain: Brain instance to visualize.
            world: World instance to visualize.
            agent_brain: Optional AgentBrain instance to visualize.
            output_file: Path to save the HTML file.
        """
        html_content = self._generate_html_template(
            brain, world, agent_brain
        )
        
        with open(output_file, 'w') as f:
            f.write(html_content)
        
        return html_content
    
    def create_performance_report(self, brain: Brain, world: SimpleWorld,
                                   agent_brain: Optional[AgentBrain] = None,
                                   steps: int = 100) -> Dict[str, Any]:
        """Create a comprehensive performance report.
        
        Args:
            brain: Brain instance to analyze.
            world: World instance to analyze.
            agent_brain: Optional AgentBrain instance to analyze.
            steps: Number of steps to analyze.
            
        Returns:
            Performance report as a dictionary.
        """
        report = {
            'network_analysis': self._analyze_network(brain),
            'simulation_analysis': self._analyze_simulation(world, steps),
            'behavior_analysis': self._analyze_behavior(agent_brain) if agent_brain else {},
            'recommendations': self._generate_recommendations(brain, world, agent_brain),
        }
        
        return report
    
    def _create_region_data(self, brain: Brain) -> List[Dict[str, Any]]:
        """Create data for region visualization.
        
        Args:
            brain: Brain instance.
            
        Returns:
            List of region data dictionaries.
        """
        region_data = []
        for region_id, region in brain.regions.items():
            region_info = {
                'id': region_id.index(),
                'name': region.name,
                'neuron_count': region.get_neuron_count(),
                'synapse_count': region.get_synapse_count(),
                'neuron_types': self._count_neuron_types(region),
                'developmental_stage': region.developmental_stage.name,
                'plasticity_flags': region.plasticity_flags,
            }
            region_data.append(region_info)
        
        return region_data
    
    def _calculate_network_statistics(self, brain: Brain) -> Dict[str, Any]:
        """Calculate network-wide statistics.
        
        Args:
            brain: Brain instance.
            
        Returns:
            Dictionary of network statistics.
        """
        stats = {
            'total_neurons': brain.getTotalNeuronCount(),
            'total_synapses': brain.getTotalSynapseCount(),
            'active_neurons': brain.getActiveNeuronCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
            'total_spikes': brain.getTotalSpikeCount(),
            'simulation_step': brain.simulation_step,
            'simulation_time': brain.simulation_time,
        }
        
        return stats
    
    def _create_animation_frames(self, brain: Brain, world: SimpleWorld,
                                 steps: int) -> List[Dict[str, Any]]:
        """Create frames for animation.
        
        Args:
            brain: Brain instance.
            world: World instance.
            steps: Number of frames to create.
            
        Returns:
            List of frame data dictionaries.
        """
        frames = []
        
        for step in range(steps):
            # Simulate a step
            brain.step(step)
            world.update(0.1)  # Assuming 0.1 timestep
            
            frame = {
                'step': step,
                'time': brain.simulation_time,
                'neuron_counts': [r.get_neuron_count() for r in brain.regions.values()],
                'synapse_counts': [r.get_synapse_count() for r in brain.regions.values()],
                'agent_position': (world.agent.x, world.agent.y) if world.agent else None,
                'agent_energy': world.agent.energy if world.agent else None,
                'world_objects': self._create_world_object_data(world),
            }
            
            frames.append(frame)
        
        return frames
    
    def _create_behavior_time_series(self, agent_brain: AgentBrain) -> Dict[str, List]:
        """Create time series data for behavior analysis.
        
        Args:
            agent_brain: AgentBrain instance.
            
        Returns:
            Dictionary of time series data.
        """
        # In a real implementation, this would track behavior over time
        time_series = {
            'steps': list(range(agent_brain.step_count)),
            'prediction_errors': [agent_brain.prediction_error] * agent_brain.step_count,
            'neuromodulation_levels': [agent_brain.neuromodulation_level] * agent_brain.step_count,
            'action_counts': [agent_brain.action_count] * agent_brain.step_count,
        }
        
        return time_series
    
    def _calculate_action_distribution(self, agent_brain: AgentBrain) -> Dict[str, Any]:
        """Calculate action distribution.
        
        Args:
            agent_brain: AgentBrain instance.
            
        Returns:
            Dictionary of action distribution.
        """
        # In a real implementation, this would track actual actions
        distribution = {
            'total_actions': agent_brain.action_count,
            'wait_actions': agent_brain.action_count,  # Default
            'move_actions': 0,
            'turn_actions': 0,
            'interact_actions': 0,
            'explore_actions': 0,
        }
        
        return distribution
    
    def _generate_html_template(self, brain: Brain, world: SimpleWorld,
                                 agent_brain: Optional[AgentBrain] = None) -> str:
        """Generate HTML visualization template.
        
        Args:
            brain: Brain instance.
            world: World instance.
            agent_brain: Optional AgentBrain instance.
            
        Returns:
            HTML template as string.
        """
        html_template = f"""
<!DOCTYPE html>
<html>
<head>
    <title>NLM Visualization</title>
    <style>
        body {{ font-family: Arial, sans-serif; margin: 0; padding: 20px; }}
        .container {{ max-width: 1200px; margin: 0 auto; }}
        .header {{ text-align: center; margin-bottom: 30px; }}
        .section {{ margin: 20px 0; padding: 20px; border: 1px solid #ddd; }}
        .stats-grid {{ display: grid; grid-template-columns: 4; gap: 20px; }}
        .stat-item {{ text-align: center; padding: 10px; border: 1px solid #ccc; }}
        .neuron {{ display: inline-block; width: 20px; height: 20px; margin: 2px; }}
        .synapse {{ display: inline-block; width: 10px; height: 2px; margin: 4px; }}
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>Neural Learning Machine (NLM) Visualization</h1>
            <p>Real-time simulation and visualization of neural networks</p>
        </div>
        
        <div class="section">
            <h2>Brain Network</h2>
            <div class="stats-grid">
                <div class="stat-item">
                    <h3>Total Neurons</h3>
                    <p>{brain.getTotalNeuronCount()}</p>
                </div>
                <div class="stat-item">
                    <h3>Total Synapses</h3>
                    <p>{brain.getTotalSynapseCount()}</p>
                </div>
                <div class="stat-item">
                    <h3>Active Neurons</h3>
                    <p>{brain.getActiveNeuronCount()}</p>
                </div>
                <div class="stat-item">
                    <h3>Average Firing Rate</h3>
                    <p>{brain.getAverageFiringRate():.2f} Hz</p>
                </div>
            </div>
        </div>
        
        <div class="section">
            <h2>World State</h2>
            <p>World dimensions: {world.getWidth()} x {world.getHeight()}</p>
            <p>Simulation time: {world.getSimulationTime():.2f}s</p>
            <p>World step: {world.simulation_step}</p>
        </div>
        
        {agent_brain_html}
    </div>
</body>
</html>
"""
        
        agent_brain_html = ""
        if agent_brain:
            agent_brain_html = f"""
        <div class="section">
            <h2>Agent Behavior</h2>
            <p>Total steps: {agent_brain.step_count}</p>
            <p>Total actions: {agent_brain.action_count}</p>
            <p>Developmental stage: {agent_brain.getDevelopmentalStage().name}</p>
            <p>Neuromodulation level: {agent_brain.getNeuromodulationLevel():.3f}</p>
            <p>Curiosity level: {agent_brain.getCuriosityLevel():.3f}</p>
            <p>Novelty level: {agent_brain.getNoveltyLevel():.3f}</p>
            <p>Prediction error: {agent_brain.getPredictionError():.3f}</p>
        </div>
        """
        
        return html_template
    
    def _analyze_network(self, brain: Brain) -> Dict[str, Any]:
        """Analyze network properties.
        
        Args:
            brain: Brain instance.
            
        Returns:
            Network analysis results.
        """
        analysis = {
            'region_count': brain.getRegionCount(),
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'connectivity_ratio': brain.getTotalSynapseCount() / max(brain.getTotalNeuronCount(), 1),
            'firing_rate': brain.getAverageFiringRate(),
            'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
        }
        
        return analysis
    
    def _analyze_simulation(self, world: SimpleWorld, steps: int) -> Dict[str, Any]:
        """Analyze simulation performance.
        
        Args:
            world: World instance.
            steps: Number of steps.
            
        Returns:
            Simulation analysis results.
        """
        analysis = {
            'world_dimensions': (world.getWidth(), world.getHeight()),
            'simulation_time': world.getSimulationTime(),
            'total_steps': world.total_steps,
            'agent_position': (world.agent.x, world.agent.y) if world.agent else None,
            'agent_energy': world.agent.energy if world.agent else None,
        }
        
        return analysis
    
    def _analyze_behavior(self, agent_brain: AgentBrain) -> Dict[str, Any]:
        """Analyze agent behavior.
        
        Args:
            agent_brain: AgentBrain instance.
            
        Returns:
            Behavior analysis results.
        """
        analysis = {
            'total_steps': agent_brain.step_count,
            'total_actions': agent_brain.action_count,
            'developmental_stage': agent_brain.getDevelopmentalStage().name,
            'neuromodulation_level': agent_brain.getNeuromodulationLevel(),
            'curiosity_level': agent_brain.getCuriosityLevel(),
            'novelty_level': agent_brain.getNoveltyLevel(),
            'prediction_error': agent_brain.getPredictionError(),
        }
        
        return analysis
    
    def _generate_recommendations(self, brain: Brain, world: SimpleWorld,
                                   agent_brain: Optional[AgentBrain] = None) -> List[str]:
        """Generate recommendations based on analysis.
        
        Args:
            brain: Brain instance.
            world: World instance.
            agent_brain: Optional AgentBrain instance.
            
        Returns:
            List of recommendations.
        """
        recommendations = []
        
        # Network recommendations
        if brain.getTotalSynapseCount() == 0:
            recommendations.append("No synapses found. Consider adding neurons and synapses.")
        
        # Simulation recommendations
        if world.agent and world.agent.energy <= 0:
            recommendations.append("Agent energy is low or depleted. Consider adding food resources.")
        
        # Developmental recommendations
        if agent_brain and agent_brain.getDevelopmentalStage().name == "INITIAL":
            recommendations.append("Brain is in INITIAL stage. Allow more simulation time for maturation.")
        
        # General recommendations
        recommendations.append("Consider increasing simulation timestep for smoother visualization.")
        recommendations.append("Add more neurons and synapses for richer network dynamics.")
        
        return recommendations
    
    def _create_world_object_data(self, world: SimpleWorld) -> List[Dict[str, Any]]:
        """Create data for world objects.
        
        Args:
            world: World instance.
            
        Returns:
            List of world object data.
        """
        objects = []
        for obj_id, obj in world.objects.items():
            if obj.get('active', True):
                objects.append({
                    'id': obj_id,
                    'x': obj.get('x', 0.0),
                    'y': obj.get('y', 0.0),
                    'type': obj.get('type', WorldObjectType.EMPTY).name,
                    'value': obj.get('value', 0.0),
                    'radius': obj.get('radius', 0.5),
                })
        
        return objects
    
    def _count_neuron_types(self, region: NeuralRegion) -> Dict[str, int]:
        """Count neurons by type in a region.
        
        Args:
            region: NeuralRegion instance.
            
        Returns:
            Dictionary of neuron type counts.
        """
        counts = {}
        for neuron in region.neurons.values():
            neuron_type = neuron['type']
            counts[neuron_type.name] = counts.get(neuron_type.name, 0) + 1
        
        return counts
    
    def _save_plot_data(self, data: Dict[str, Any], filepath: str):
        """Save plot data to file.
        
        Args:
            data: Plot data to save.
            filepath: Output file path.
        """
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2)
    
    def _save_animation_data(self, data: Dict[str, Any], filepath: str):
        """Save animation data to file.
        
        Args:
            data: Animation data to save.
            filepath: Output file path.
        """
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2)


__all__ = ['VisualizationUtils']