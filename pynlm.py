# pynlm
# Advanced utilities for NLM brain simulation

import pynlm
import json
import csv
import time
from typing import List, Dict, Any, Optional, Callable
from pathlib import Path

class AdvancedUtils:
    """Advanced utilities for working with NLM brains."""
    
    @staticmethod
    def create_advanced_scenario(scenario_type="exploration", 
                                neuron_count=2000, 
                                region_count=3,
                                simulation_steps=1000) -> pynlm.Brain:
        """
        Create an advanced brain configuration for different scenarios.
        
        Args:
            scenario_type: Type of scenario - "exploration", "exploitation", 
                          "development", "stable", "balanced"
            neuron_count: Number of neurons to create
            region_count: Number of brain regions
            simulation_steps: Number of simulation steps
            
        Returns:
            Configured and initialized brain
        """
        config = pynlm.createDefaultConfig()
        
        if scenario_type == "exploration":
            # High novelty, high curiosity, moderate development
            config.set("brain.neuron_count", neuron_count)
            config.set("brain.region_count", region_count)
            config.set("brain.connection_probability", 0.15)
            config.set("neuromod.curiosity.enable", True)
            config.set("neuromod.curiosity.scale", 2.0)
            config.set("neuromod.novelty.enable", True)
            config.set("neuromod.dopamine.scale", 0.5)
            config.set("plasticity.structural.enable", True)
            config.set("development.enable", True)
            config.set("development.critical_period_duration", 500)
            
        elif scenario_type == "exploitation":
            # Low exploration, high stability
            config.set("brain.neuron_count", neuron_count)
            config.set("brain.region_count", region_count)
            config.set("brain.connection_probability", 0.05)
            config.set("neuromod.curiosity.enable", False)
            config.set("neuromod.novelty.enable", False)
            config.set("neuromod.dopamine.scale", 1.5)
            config.set("plasticity.structural.enable", False)
            config.set("development.enable", False)
            config.set("plasticity.stdp.enable", True)
            config.set("plasticity.stdp.learning_rate", 0.002)
            
        elif scenario_type == "development":
            # High plasticity, fast development
            config.set("brain.neuron_count", neuron_count * 1.5)
            config.set("brain.region_count", region_count)
            config.set("brain.connection_probability", 0.1)
            config.set("neuromod.curiosity.enable", True)
            config.set("neuromod.curiosity.scale", 1.0)
            config.set("neuromod.novelty.enable", True)
            config.set("neuromod.dopamine.scale", 1.0)
            config.set("plasticity.structural.enable", True)
            config.set("development.enable", True)
            config.set("development.critical_period_duration", 1000)
            config.set("development.maturation_rate", 0.8)
            
        elif scenario_type == "stable":
            # Very stable, minimal changes
            config.set("brain.neuron_count", neuron_count)
            config.set("brain.region_count", region_count)
            config.set("brain.connection_probability", 0.01)
            config.set("neuromod.curiosity.enable", False)
            config.set("neuromod.novelty.enable", False)
            config.set("neuromod.dopamine.scale", 0.8)
            config.set("plasticity.structural.enable", False)
            config.set("plasticity.stdp.enable", True)
            config.set("plasticity.stdp.learning_rate", 0.0005)
            config.set("development.enable", False)
            
        elif scenario_type == "balanced":
            # Balanced approach
            config.set("brain.neuron_count", neuron_count)
            config.set("brain.region_count", region_count)
            config.set("brain.connection_probability", 0.08)
            config.set("neuromod.curiosity.enable", True)
            config.set("neuromod.curiosity.scale", 1.0)
            config.set("neuromod.novelty.enable", True)
            config.set("neuromod.dopamine.scale", 1.0)
            config.set("plasticity.structural.enable", True)
            config.set("development.enable", True)
            config.set("plasticity.stdp.learning_rate", 0.001)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        return brain
    
    @staticmethod
    def export_brain_data(brain, filename, export_format="csv"):
        """
        Export brain data to file for analysis.
        
        Args:
            brain: Brain instance to export data from
            filename: Output file path
            export_format: Format - "csv", "json", "txt"
        """
        data = {
            "neuron_count": brain.getTotalNeuronCount(),
            "synapse_count": brain.getTotalSynapseCount(),
            "active_neuron_count": brain.getActiveNeuronCount(),
            "firing_neuron_count": brain.getFiringNeuronCount(),
            "average_firing_rate": brain.getAverageFiringRate(),
            "excitation_inhibition_ratio": brain.getExcitationInhibitionRatio(),
            "total_spike_count": brain.getTotalSpikeCount(),
            "region_count": brain.getRegionCount(),
            "developmental_stage": brain.getDevelopmentalStage(),
            "timestamp": time.time()
        }
        
        # Add region-specific data
        region_data = []
        for region_id in brain.getRegionIds():
            region = brain.getRegion(region_id)
            region_data.append({
                "region_id": region_id.index(),
                "name": region.getName(),
                "neuron_count": region.getTotalNeuronCount(),
                "synapse_count": region.getSynapseCount(),
                "average_weight": region.getAverageSynapticWeight()
            })
        
        data["regions"] = region_data
        
        file_path = Path(filename)
        
        if export_format == "csv":
            # Export main data
            with open(file_path, 'w', newline='') as f:
                writer = csv.writer(f)
                writer.writerow(data.keys())
                writer.writerow(data.values())
            
            # Export region data
            region_file = file_path.with_name(f"{file_path.stem}_regions.csv")
            with open(region_file, 'w', newline='') as f:
                if region_data:
                    writer = csv.DictWriter(f, fieldnames=region_data[0].keys())
                    writer.writeheader()
                    writer.writerows(region_data)
                    
        elif export_format == "json":
            export_data = {
                "brain_data": data,
                "region_data": region_data
            }
            with open(file_path, 'w') as f:
                json.dump(export_data, f, indent=2)
                
        elif export_format == "txt":
            with open(file_path, 'w') as f:
                f.write("NLM Brain Data Export\n")
                f.write("=" * 50 + "\n\n")
                for key, value in data.items():
                    if key != "regions":
                        f.write(f"{key}: {value}\n")
                f.write("\nRegion Data:\n")
                for region in region_data:
                    f.write(f"  Region {region['region_id']} ({region['name']}): "
                           f"{region['neuron_count']} neurons, {region['synapse_count']} synapses, "
                           f"avg weight: {region['average_weight']:.4f}\n")
    
    @staticmethod
    def create_batch_processor(num_steps=100, batch_size=10):
        """
        Create a batch processor for efficient simulation.
        
        Args:
            num_steps: Number of simulation steps
            batch_size: Steps per batch
            
        Returns:
            BatchProcessor instance
        """
        return BatchProcessor(num_steps, batch_size)

class BatchProcessor:
    """Efficient batch processor for NLM brain simulation."""
    
    def __init__(self, num_steps=100, batch_size=10):
        self.num_steps = num_steps
        self.batch_size = batch_size
        self.batches = []
        self.current_batch = []
        self.batch_index = 0
        
    def process_batch(self, brain, agent=None, world=None):
        """
        Process a batch of simulation steps.
        
        Args:
            brain: Brain instance
            agent: Optional agent interface
            world: Optional world interface
            
        Returns:
            BatchResults object
        """
        batch_results = BatchResults(self.batch_index)
        
        for i in range(self.batch_size):
            step = self.batch_index * self.batch_size + i
            
            if step >= self.num_steps:
                break
                
            # Get pre-step statistics
            stats_before = {
                "firing_neurons": brain.getFiringNeuronCount(),
                "total_spikes": brain.getTotalSpikeCount(),
                "active_neurons": brain.getActiveNeuronCount()
            }
            
            # Process sensory input if agent and world provided
            if agent and world:
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                
                # Run brain step
                brain.step(step)
                
                # Decode motor command
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Apply reward modulation
                reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
                agent.applyRewardModulation(reward, 0.0)
                
                # Update development
                agent.updateDevelopment(world.getSimulationTime())
            else:
                # Simple brain step without agent/world
                brain.step(step)
            
            # Get post-step statistics
            stats_after = {
                "firing_neurons": brain.getFiringNeuronCount(),
                "total_spikes": brain.getTotalSpikeCount(),
                "active_neurons": brain.getActiveNeuronCount()
            }
            
            batch_results.add_step_result(step, stats_before, stats_after)
        
        self.batch_index += 1
        return batch_results
    
    def get_total_batches(self):
        """Get total number of batches."""
        return (self.num_steps + self.batch_size - 1) // self.batch_size
    
    def reset(self):
        """Reset the batch processor."""
        self.batch_index = 0
        self.current_batch = []
        self.batches = []

class BatchResults:
    """Results from batch processing."""
    
    def __init__(self, batch_index):
        self.batch_index = batch_index
        self.step_results = []
        
    def add_step_result(self, step, stats_before, stats_after):
        """Add results for a single step."""
        self.step_results.append({
            "step": step,
            "stats_before": stats_before,
            "stats_after": stats_after,
            "spike_increase": stats_after["total_spikes"] - stats_before["total_spikes"],
            "firing_change": stats_after["firing_neurons"] - stats_before["firing_neurons"]
        })
        
    def get_summary(self):
        """Get summary statistics for the batch."""
        if not self.step_results:
            return {}
            
        total_spike_increase = sum(r["spike_increase"] for r in self.step_results)
        avg_firing_change = sum(r["firing_change"] for r in self.step_results) / len(self.step_results)
        
        return {
            "batch_index": self.batch_index,
            "steps_processed": len(self.step_results),
            "total_spike_increase": total_spike_increase,
            "average_firing_change": avg_firing_change,
            "max_firing_change": max(r["firing_change"] for r in self.step_results),
            "min_firing_change": min(r["firing_change"] for r in self.step_results)
        }

class DebugTools:
    """Debugging and validation tools for NLM brains."""
    
    @staticmethod
    def validate_brain_state(brain):
        """
        Validate brain state for consistency.
        
        Args:
            brain: Brain instance to validate
            
        Returns:
            ValidationResult object
        """
        result = ValidationResult()
        
        # Check basic properties
        if brain.getTotalNeuronCount() == 0:
            result.add_error("Brain has no neurons")
        
        if brain.getTotalSynapseCount() < 0:
            result.add_error("Brain has negative synapse count")
            
        # Check region consistency
        region_count = brain.getRegionCount()
        if region_count > 0:
            region_ids = brain.getRegionIds()
            if len(region_ids) != region_count:
                result.add_warning("Region count mismatch")
                
            # Check each region
            for region_id in region_ids:
                region = brain.getRegion(region_id)
                if region is None:
                    result.add_error(f"Region {region_id} not found")
                elif region.getTotalNeuronCount() == 0:
                    result.add_warning(f"Region {region_id} has no neurons")
        
        # Check developmental stage consistency
        stage = brain.getDevelopmentalStage()
        if stage not in [pynlm.DevelopmentalStage.Initial,
                       pynlm.DevelopmentalStage.CriticalPeriod,
                       pynlm.DevelopmentalStage.Maturation,
                       pynlm.DevelopmentalStage.Adult,
                       pynlm.DevelopmentalStage.Aging]:
            result.add_warning(f"Unknown developmental stage: {stage}")
        
        return result

class ValidationResult:
    """Result of brain state validation."""
    
    def __init__(self):
        self.errors = []
        self.warnings = []
        
    def add_error(self, message):
        """Add an error message."""
        self.errors.append(message)
        
    def add_warning(self, message):
        """Add a warning message."""
        self.warnings.append(message)
        
    def is_valid(self):
        """Check if validation passed (no errors)."""
        return len(self.errors) == 0
        
    def get_report(self):
        """Get a report string."""
        lines = []
        if self.errors:
            lines.append("ERRORS:")
            for error in self.errors:
                lines.append(f"  - {error}")
                
        if self.warnings:
            lines.append("WARNINGS:")
            for warning in self.warnings:
                lines.append(f"  - {warning}")
                
        if not lines:
            lines.append("Validation passed with no errors or warnings.")
            
        return "\n".join(lines)

class ScenarioManager:
    """Manager for running and comparing different scenarios."""
    
    @staticmethod
    def run_scenario_comparison(scenario_types, common_config=None):
        """
        Run and compare multiple scenarios.
        
        Args:
            scenario_types: List of scenario type names
            common_config: Common configuration parameters
            
        Returns:
            Dictionary with scenario results
        """
        if common_config is None:
            common_config = {
                "neuron_count": 1000,
                "region_count": 2,
                "simulation_steps": 500
            }
        
        results = {}
        
        for scenario_type in scenario_types:
            print(f"Running scenario: {scenario_type}")
            
            brain = AdvancedUtils.create_advanced_scenario(
                scenario_type=scenario_type,
                neuron_count=common_config["neuron_count"],
                region_count=common_config["region_count"],
                simulation_steps=common_config["simulation_steps"]
            )
            
            # Record initial state
            initial_data = {
                "neuron_count": brain.getTotalNeuronCount(),
                "synapse_count": brain.getTotalSynapseCount(),
                "developmental_stage": brain.getDevelopmentalStage(),
                "timestamp": time.time()
            }
            
            # Run simulation
            for step in range(common_config["simulation_steps"]):
                brain.step(step)
                
            # Record final state
            final_data = {
                "neuron_count": brain.getTotalNeuronCount(),
                "synapse_count": brain.getTotalSynapseCount(),
                "firing_neurons": brain.getFiringNeuronCount(),
                "total_spikes": brain.getTotalSpikeCount(),
                "average_firing_rate": brain.getAverageFiringRate(),
                "excitation_inhibition_ratio": brain.getExcitationInhibitionRatio(),
                "developmental_stage": brain.getDevelopmentalStage(),
                "timestamp": time.time()
            }
            
            results[scenario_type] = {
                "initial": initial_data,
                "final": final_data,
                "brain": brain
            }
            
            print(f"  Completed scenario: {scenario_type}")
        
        return results
    
    @staticmethod
    def compare_scenarios(results, output_file="scenario_comparison.json"):
        """
        Compare multiple scenario results and save to file.
        
        Args:
            results: Dictionary of scenario results
            output_file: Output file path
        """
        comparison = {
            "scenarios": list(results.keys()),
            "summary": {}
        }
        
        for scenario_name, data in results.items():
            scenario_summary = {
                "neuron_growth": data["final"]["neuron_count"] - data["initial"]["neuron_count"],
                "synapse_growth": data["final"]["synapse_count"] - data["initial"]["synapse_count"],
                "spike_count": data["final"]["total_spikes"],
                "avg_firing_rate": data["final"]["average_firing_rate"],
                "final_stage": data["final"]["developmental_stage"],
                "simulation_time": data["final"]["timestamp"] - data["initial"]["timestamp"]
            }
            
            comparison["summary"][scenario_name] = scenario_summary
        
        # Find best performing scenario
        best_scenario = max(comparison["summary"].keys(), 
                          key=lambda x: comparison["summary"][x]["spike_count"])
        comparison["best_scenario"] = best_scenario
        
        # Find most stable scenario (smallest synapse growth relative to initial)
        most_stable = min(comparison["summary"].keys(),
                         key=lambda x: comparison["summary"][x]["synapse_growth"] / 
                                     max(1, comparison["summary"][x]["initial_synapses"]))
        comparison["most_stable_scenario"] = most_stable
        
        # Save to file
        with open(output_file, 'w') as f:
            json.dump(comparison, f, indent=2)
        
        return comparison

# Export all advanced utilities
__all__ = [
    'AdvancedUtils',
    'BatchProcessor', 
    'BatchResults',
    'DebugTools',
    'ValidationResult',
    'ScenarioManager'
]