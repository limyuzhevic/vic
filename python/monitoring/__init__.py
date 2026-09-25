# NLM Monitoring - Analysis and monitoring utilities

This module provides monitoring and analysis utilities for NLM simulations,
enabling comprehensive analysis of simulation results and performance metrics.
"""

import pynlm
import json
from typing import Dict, Any, List
from pathlib import Path


class Monitoring:
    """Monitoring and analysis utilities for NLM simulations."""
    
    @staticmethod
    def analyze_brain_activity(brain: pynlm.Brain) -> Dict[str, Any]:
        """
        Analyze brain activity patterns.
        
        Args:
            brain: The brain to analyze.
            
        Returns:
            Dictionary with activity analysis.
        """
        stats = brain.get_statistics()
        analysis = {
            "activity_level": "low",
            "complexity": "simple",
            "stability": "unstable",
            "recommendations": []
        }
        
        # Determine activity level
        if stats.get("firing_neurons", 0) > 0.3 * stats.get("total_neurons", 1):
            analysis["activity_level"] = "high"
        elif stats.get("firing_neurons", 0) > 0.1 * stats.get("total_neurons", 1):
            analysis["activity_level"] = "medium"
        
        # Determine complexity
        if stats.get("total_synapses", 0) > 10 * stats.get("total_neurons", 1):
            analysis["complexity"] = "complex"
        
        # Determine stability
        if stats.get("average_firing_rate", 0) > 0 and stats.get("average_firing_rate", 0) < 10:
            analysis["stability"] = "stable"
        elif stats.get("average_firing_rate", 0) == 0:
            analysis["stability"] = "inactive"
        
        # Generate recommendations
        if analysis["activity_level"] == "low":
            analysis["recommendations"].append("Increase sensory input to activate neurons")
        
        if analysis["complexity"] == "simple":
            analysis["recommendations"].append("Increase synapse density for more complex behavior")
        
        if analysis["stability"] == "unstable":
            analysis["recommendations"].append("Adjust neuromodulation parameters")
        
        return analysis
    
    @staticmethod
    def create_visualization_data(brain: pynlm.Brain, agent: pynlm.AgentBrain,
                                  world: pynlm.SimpleWorld) -> Dict[str, Any]:
        """
        Create visualization data for brain-world interaction.
        
        Args:
            brain: The brain to visualize.
            agent: The agent for context.
            world: The world for spatial data.
            
        Returns:
            Dictionary with visualization data.
        """
        visualization_data = {
            "brain_neurons": brain.getTotalNeuronCount(),
            "brain_synapses": brain.getTotalSynapseCount(),
            "firing_neuros": brain.getFiringNeuronCount(),
            "active_neurons": brain.getActiveNeuronCount(),
            "average_firing_rate": brain.getAverageFiringRate(),
            "excitation_inhibition_ratio": brain.getExcitationInhibitionRatio(),
            "developmental_stage": str(brain.getDevelopmentalStage()),
            "agent_stats": {
                "curiosity_level": agent.getCuriosityLevel(),
                "novelty_level": agent.getNoveltyLevel(),
                "neuromodulation_level": agent.getNeuromodulationLevel(),
                "prediction_error": agent.getPredictionError(),
            },
            "world_state": {
                "agent_position": {
                    "x": world.getAgentBody().x,
                    "y": world.getAgentBody().y,
                    "orientation": world.getAgentBody().orientation,
                },
                "world_dimensions": {
                    "width": world.getWidth(),
                    "height": world.getHeight(),
                },
                "simulation_time": world.getSimulationTime(),
            }
        }
        
        return visualization_data
    
    @staticmethod
    def export_for_analysis(brain: pynlm.Brain, agent: pynlm.AgentBrain,
                            world: pynlm.SimpleWorld, output_dir: str = "."):
        """
        Export simulation data for external analysis.
        
        Args:
            brain: The brain to export.
            agent: The agent for context.
            world: The world for spatial data.
            output_dir: Directory to save analysis files.
        """
        Path(output_dir).mkdir(parents=True, exist_ok=True)
        
        # Export brain statistics
        brain_stats = brain.get_statistics()
        with open(Path(output_dir) / "brain_stats.json", 'w') as f:
            json.dump(brain_stats, f, indent=2)
        
        # Export agent status
        agent_status = agent.get_agent_status()
        with open(Path(output_dir) / "agent_status.json", 'w') as f:
            json.dump(agent_status, f, indent=2)
        
        # Export world state
        world_state = world.get_world_state()
        with open(Path(output_dir) / "world_state.json", 'w') as f:
            json.dump(world_state, f, indent=2)
        
        # Export visualization data
        viz_data = Monitoring.create_visualization_data(brain, agent, world)
        with open(Path(output_dir) / "visualization_data.json", 'w') as f:
            json.dump(viz_data, f, indent=2)
        
        print(f"Analysis data exported to {output_dir}")
        return output_dir
    
    @staticmethod
    def generate_performance_report(brain: pynlm.Brain, steps: int) -> Dict[str, Any]:
        """
        Generate a performance report for a brain simulation.
        
        Args:
            brain: The brain to analyze.
            steps: Number of steps that were simulated.
            
        Returns:
            Dictionary with performance report.
        """
        import time
        
        report = {
            "simulation_parameters": {
                "steps": steps,
                "final_firing_neurons": brain.getFiringNeuronCount(),
                "total_spikes": brain.getTotalSpikeCount(),
                "total_neurons": brain.getTotalNeuronCount(),
                "total_synapses": brain.getTotalSynapseCount(),
            },
            "performance_metrics": {
                "average_firing_rate": brain.getAverageFiringRate(),
                "excitation_inhibition_ratio": brain.getExcitationInhibitionRatio(),
                "development_stage": str(brain.getDevelopmentalStage()),
            },
            "efficiency_metrics": {
                "spike_per_neuron_ratio": (
                    brain.getTotalSpikeCount() / brain.getTotalNeuronCount()
                    if brain.getTotalNeuronCount() > 0 else 0
                ),
                "synapse_per_neuron_ratio": (
                    brain.getTotalSynapseCount() / brain.getTotalNeuronCount()
                    if brain.getTotalNeuronCount() > 0 else 0
                ),
            },
            "analysis": Monitoring.analyze_brain_activity(brain),
        }
        
        return report
    
    @staticmethod
    def create_summary_statistics(brain: pynlm.Brain, agent: pynlm.AgentBrain,
                                  world: pynlm.SimpleWorld) -> Dict[str, Any]:
        """
        Create a concise summary of simulation results.
        
        Args:
            brain: The brain that was used.
            agent: The agent that was used.
            world: The world that was used.
            
        Returns:
            Dictionary with concise summary statistics.
        """
        brain_stats = brain.get_statistics()
        agent_status = agent.get_agent_status()
        
        summary = {
            "simulation_summary": {
                "total_neurons": brain_stats["total_neurons"],
                "total_synapses": brain_stats["total_synapses"],
                "firing_neurons": brain_stats["firing_neurons"],
                "average_firing_rate": brain_stats["average_firing_rate"],
                "total_spikes": brain_stats["total_spikes"],
            },
            "agent_summary": {
                "curiosity_level": agent_status["curiosity_level"],
                "novelty_level": agent_status["novelty_level"],
                "neuromodulation_level": agent_status["neuromodulation_level"],
                "prediction_error": agent_status["prediction_error"],
                "developmental_stage": agent_status["developmental_stage"],
            },
            "world_summary": {
                "world_width": world.getWidth(),
                "world_height": world.getHeight(),
                "simulation_time": world.getSimulationTime(),
                "agent_position": {
                    "x": world.getAgentBody().x,
                    "y": world.getAgentBody().y,
                },
            },
            "overall_health": {
                "activity_level": "healthy" if brain_stats["firing_neurons"] > 0 else "inactive",
                "learning_progress": "active" if agent_status["curiosity_level"] > 0.1 else "minimal",
                "stability": "stable" if agent_status["prediction_error"] < 0.5 else "unstable",
            },
        }
        
        return summary


# Quick analysis functions
def quick_brain_analysis(brain: pynlm.Brain) -> Dict[str, Any]:
    """
    Perform quick analysis on a brain.
    
    Args:
        brain: The brain to analyze.
        
    Returns:
        Dictionary with quick analysis results.
    """
    return Monitoring.analyze_brain_activity(brain)


def quick_performance_analysis(brain: pynlm.Brain, steps: int) -> Dict[str, Any]:
    """
    Perform quick performance analysis.
    
    Args:
        brain: The brain to analyze.
        steps: Number of simulation steps that were run.
        
    Returns:
        Dictionary with performance analysis.
    """
    return Monitoring.generate_performance_report(brain, steps)


def export_results_for_research(brain: pynlm.Brain, agent: pynlm.AgentBrain,
                                 world: pynlm.SimpleWorld, output_dir: str):
    """
    Export simulation results in a format suitable for research.
    
    Args:
        brain: The brain used in the simulation.
        agent: The agent used in the simulation.
        world: The world used in the simulation.
        output_dir: Directory to save research data.
    """
    # Create summary statistics
    summary = Monitoring.create_summary_statistics(brain, agent, world)
    
    # Create full visualization data
    viz_data = Monitoring.create_visualization_data(brain, agent, world)
    
    # Create performance report
    performance = Monitoring.generate_performance_report(brain, 0)
    
    # Save all data
    Path(output_dir).mkdir(parents=True, exist_ok=True)
    
    with open(Path(output_dir) / "summary.json", 'w') as f:
        json.dump(summary, f, indent=2)
    
    with open(Path(output_dir) / "visualization.json", 'w') as f:
        json.dump(viz_data, f, indent=2)
    
    with open(Path(output_dir) / "performance.json", 'w') as f:
        json.dump(performance, f, indent=2)
    
    print(f"Research data exported to {output_dir}")
    print(f"  - summary.json: High-level simulation results")
    print(f"  - visualization.json: Detailed brain-world interaction data")
    print(f"  - performance.json: Performance metrics and analysis")
    
    return output_dir


# Package metadata
__version__ = "0.1.0"
__all__ = [
    "Monitoring",
    "quick_brain_analysis",
    "quick_performance_analysis",
    "export_results_for_research",
]