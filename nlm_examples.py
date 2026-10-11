#!/usr/bin/env python3
"""
NLM Python API Examples
=======================

Comprehensive examples demonstrating the enhanced Python API for NLM.

This file provides:
- Basic brain creation and initialization
- Network analysis and visualization
- Learning experiments with metrics
- Agent-based simulations
- Advanced plasticity control
- Performance monitoring
- World interaction examples
"""

import pynlm as nlm
import numpy as np
import json
import matplotlib.pyplot as plt
from typing import Dict, List, Optional
import time

class NLMAnalyzer:
    """High-level interface for analyzing and experimenting with NLM brains."""
    
    def __init__(self, config_name="simple"):
        """Create an analyzer with the specified brain configuration."""
        self.config = nlm.createDefaultConfig()
        
        # Apply preset configurations
        if config_name == "simple":
            self.config.set("neuron_count", 100)
            self.config.set("region_count", 1)
            self.config.set("connection_probability", 0.1)
            self.config.set("simulation_timestep", 0.001)
        elif config_name == "complex":
            self.config.set("neuron_count", 1000)
            self.config.set("region_count", 5)
            self.config.set("connection_probability", 0.15)
            self.config.set("simulation_timestep", 0.0005)
            self.config.set("stdp_ltp_weight", 0.03)
            self.config.set("stdp_ltd_weight", 0.02)
        elif config_name == "high_plasticity":
            self.config.set("neuron_count", 200)
            self.config.set("region_count", 2)
            self.config.set("connection_probability", 0.2)
            self.config.set("stdp_ltp_weight", 0.05)
            self.config.set("stdp_ltd_weight", 0.04)
            self.config.set("synaptogenesis_rate", 0.001)
            self.config.set("pruning_rate", 0.0005)
        
        self.brain = nlm.createBrain(self.config)
        self.world = nlm.createSimpleWorld()
        self.agent_brain = nlm.createAgentBrain(self.brain)
        
        self.initialize()
    
    def initialize(self):
        """Initialize the brain and configure the world."""
        print("Initializing NLM brain...")
        success = self.brain.initialize()
        if not success:
            raise RuntimeError("Brain initialization failed")
        
        # Configure world for testing
        self.world.configure(800, 600, 320, 240)
        self.world.setAgentStart(400, 300)
        self.agent_brain.initialize(self.world)
        
        print("Brain and world initialized successfully")
    
    def get_network_statistics(self) -> Dict:
        """Get comprehensive network statistics."""
        stats = self.brain.get_statistics()
        stats.update(self.get_plasticity_status())
        stats.update(self.get_development_status())
        return stats
    
    def get_plasticity_status(self) -> Dict:
        """Get current plasticity system status."""
        return {
            "stdp_enabled": self.brain.getSTDP() is not None,
            "hebbian_enabled": self.brain.getHebbian() is not None,
            "structural_enabled": self.brain.getStructuralPlasticity() is not None,
        }
    
    def get_development_status(self) -> Dict:
        """Get developmental stage information."""
        stage = self.brain.getDevelopmentalStage()
        return {
            "stage": str(stage),
            "is_developing": stage != nlm.DevelopmentalStage.Adult,
            "stage_name": self._get_stage_name(stage),
        }
    
    def _get_stage_name(self, stage: nlm.DevelopmentalStage) -> str:
        """Convert development stage to readable name."""
        names = {
            nlm.DevelopmentalStage.Initial: "Initial",
            nlm.DevelopmentalStage.CriticalPeriod: "Critical Period",
            nlm.DevelopmentalStage.Maturation: "Maturation",
            nlm.DevelopmentalStage.Adult: "Adult",
            nlm.DevelopmentalStage.Aging: "Aging",
        }
        return names.get(stage, "Unknown")
    
    def run_plasticity_analysis(self, duration: int = 1000) -> Dict:
        """Run a plasticity analysis experiment."""
        print(f"Running plasticity analysis for {duration} steps...")
        
        # Enable all plasticity mechanisms
        self.brain.enable_all_plasticity(True, True, True)
        
        # Record initial state
        initial_stats = self.brain.get_statistics()
        initial_weights = self._get_average_synapse_weights()
        
        # Run simulation with learning patterns
        for step in range(duration):
            # Apply learning patterns
            for i in range(20):
                self.brain.injectCurrentToNeurons(nlm.NeuronType.Sensory, 30.0)
            
            self.brain.step(step, step * 0.001)
            
            # Log progress every 100 steps
            if step % 100 == 0:
                current_stats = self.brain.get_statistics()
                print(f"  Step {step}: Active neurons: {current_stats['active_neurons']}, "
                      f"Spikes: {current_stats['total_spikes']}")
        
        # Record final state
        final_stats = self.brain.get_statistics()
        final_weights = self._get_average_synapse_weights()
        
        # Calculate learning metrics
        results = self._calculate_learning_metrics(initial_weights, final_weights)
        results.update({
            "initial_stats": initial_stats,
            "final_stats": final_stats,
            "duration": duration,
            "plasticity_enabled": True,
        })
        
        return results
    
    def _get_average_synapse_weights(self) -> List[float]:
        """Get average weights from all synapses."""
        weights = []
        for region_id in self.brain.getRegionIds():
            region = self.brain.getRegion(region_id)
            if region:
                for synapse in region.getSynapses():
                    weights.append(synapse.getWeight())
        return weights
    
    def _calculate_learning_metrics(self, initial: List[float], final: List[float]) -> Dict:
        """Calculate learning metrics from weight changes."""
        if not initial or not final:
            return {"error": "No weights recorded"}
        
        min_size = min(len(initial), len(final))
        initial_trunc = initial[:min_size]
        final_trunc = final[:min_size]
        
        results = {
            "weight_change_count": len(initial_trunc),
            "initial_mean_weight": np.mean(initial_trunc),
            "final_mean_weight": np.mean(final_trunc),
            "weight_change_percentage": (np.mean(final_trunc) - np.mean(initial_trunc)) / np.mean(initial_trunc) * 100,
        }
        
        # Count synapse changes
        strengthened = sum(1 for i in range(min_size) if final_trunc[i] - initial_trunc[i] > 0.01)
        weakened = sum(1 for i in range(min_size) if final_trunc[i] - initial_trunc[i] < -0.01)
        unchanged = min_size - strengthened - weakened
        
        results.update({
            "strengthened_synapses": strengthened,
            "weakened_synapses": weakened,
            "unchanged_synapses": unchanged,
            "learning_occurred": (strengthened > 0 or weakened > 0),
        })
        
        return results
    
    def create_learning_experiment(self, experiment_config: Optional[Dict] = None) -> Dict:
        """Run a comprehensive learning experiment."""
        if experiment_config is None:
            experiment_config = {
                "duration": 2000,
                "pattern_size": 30,
                "learning_rate": 0.02,
                "enable_plasticity": True,
            }
        
        print(f"Running learning experiment: {experiment_config}")
        
        # Reset and initialize
        self.brain.reset()
        self.brain.initialize()
        
        # Configure plasticity
        if experiment_config["enable_plasticity"]:
            self.brain.enable_all_plasticity(True, True, False)
        
        # Record initial state
        initial_stats = self.brain.get_statistics()
        
        # Run learning
        duration = experiment_config["duration"]
        pattern_size = experiment_config["pattern_size"]
        learning_rate = experiment_config["learning_rate"]
        
        for step in range(duration):
            # Create input pattern
            for i in range(pattern_size):
                self.brain.injectCurrentToNeurons(nlm.NeuronType.Sensory, learning_rate)
            
            self.brain.step(step, step * 0.001)
            
            # Log progress
            if step % 500 == 0:
                current_stats = self.brain.get_statistics()
                print(f"  Step {step}: Spikes {current_stats['total_spikes']}, "
                      f"Active neurons {current_stats['active_neurons']}")
        
        # Record final state
        final_stats = self.brain.get_statistics()
        
        # Calculate results
        experiment_results = {
            "duration": duration,
            "pattern_size": pattern_size,
            "learning_rate": learning_rate,
            "initial_stats": initial_stats,
            "final_stats": final_stats,
            "total_spikes": final_stats["total_spikes"],
            "learning_effectiveness": self._calculate_learning_effectiveness(initial_stats, final_stats),
        }
        
        return experiment_results
    
    def _calculate_learning_effectiveness(self, initial: Dict, final: Dict) -> float:
        """Calculate a simple learning effectiveness score."""
        if initial["total_spikes"] == 0:
            return 0.0
        
        # Normalize spike count increase
        spike_increase = (final["total_spikes"] - initial["total_spikes"]) / initial["total_spikes"]
        
        # Normalize activity increase
        activity_increase = (final["active_neurons"] - initial["active_neurons"]) / initial["active_neurons"]
        
        # Weighted combination
        effectiveness = (spike_increase + activity_increase) / 2.0
        return max(0.0, min(effectiveness, 10.0))  # Clamp between 0 and 10
    
    def run_agent_simulation(self, steps: int = 500) -> Dict:
        """Run an agent-based simulation with the world."""
        print(f"Running agent simulation for {steps} steps...")
        
        # Reset world and agent
        self.world.reset()
        self.agent_brain.reset()
        
        # Create test environment
        self._create_test_environment()
        
        # Run simulation
        sensory_percept = nlm.SensoryPercept()
        action_results = []
        
        for step in range(steps):
            # Get sensory input
            world_percept = self.world.getSensoryPercept()
            
            # Process through agent brain
            self.agent_brain.processSensoryInput(world_percept)
            
            # Produce action
            action = self.brain.produceAction()
            if action:
                # Apply action in world
                result = self.world.applyMotorCommand(
                    self.agent_brain.decodeMotorCommand(),
                    step * 0.001
                )
                action_results.append({
                    "step": step,
                    "action_type": str(action.getType()),
                    "reward": result.reward,
                    "success": result.success,
                })
            
            # Update world
            self.world.update(0.001)
            
            # Log progress
            if step % 100 == 0:
                stats = self.brain.get_statistics()
                print(f"  Step {step}: Position ({self.world.getAgentBody().x:.1f}, {self.world.getAgentBody().y:.1f}), "
                      f"Energy {self.world.getAgentBody().energy:.1f}")
        
        # Return results
        return {
            "total_steps": steps,
            "actions_taken": len(action_results),
            "successful_actions": sum(1 for a in action_results if a["success"]),
            "average_reward": np.mean([a["reward"] for a in action_results]) if action_results else 0,
            "final_position": {
                "x": self.world.getAgentBody().x,
                "y": self.world.getAgentBody().y,
            },
            "final_energy": self.world.getAgentBody().energy,
            "action_history": action_results,
        }
    
    def _create_test_environment(self):
        """Create a test environment with resources and hazards."""
        width, height = self.world.getWidth(), self.world.getHeight()
        
        # Add resources
        for i in range(8):
            import random
            x = random.uniform(50, width - 50)
            y = random.uniform(50, height - 50)
            obj = nlm.WorldObject(x, y, nlm.WorldObjectType.Resource, 10.0, 0.5)
            self.world.addObject(obj)
        
        # Add hazards
        for i in range(3):
            import random
            x = random.uniform(100, width - 100)
            y = random.uniform(100, height - 100)
            obj = nlm.WorldObject(x, y, nlm.WorldObjectType.Hazard, 0.0, 0.8)
            self.world.addObject(obj)
    
    def get_performance_metrics(self) -> Dict:
        """Get comprehensive performance metrics."""
        metrics = self.brain.get_performance_metrics()
        
        # Add agent-specific metrics
        if self.agent_brain:
            agent_stats = self.agent_brain.get_brain_state()
            metrics["agent_brain_state"] = agent_stats
        
        # Add world-specific metrics
        body = self.world.getAgentBody()
        metrics["agent_state"] = {
            "x": body.x,
            "y": body.y,
            "orientation": body.orientation,
            "energy": body.energy,
            "health": body.health,
            "age": body.age,
            "is_moving": body.isMoving,
            "is_turning": body.isTurning,
        }
        
        return metrics
    
    def print_summary(self):
        """Print a comprehensive summary of the current state."""
        print("\n" + "="*60)
        print("NLM BRAIN ANALYSIS SUMMARY")
        print("="*60)
        
        stats = self.get_network_statistics()
        
        print(f"Network Structure:")
        print(f"  Neurons: {stats['total_neurons']}")
        print(f"  Synapses: {stats['total_synapses']}")
        print(f"  Regions: {stats['region_count']}")
        print(f"  Active Neurons: {stats['active_neurons']}")
        print(f"  Firing Neurons: {stats['firing_neurons']}")
        print(f"  Total Spikes: {stats['total_spikes']}")
        
        print(f"\nPerformance:")
        print(f"  Average Firing Rate: {stats['average_firing_rate']:.2f} Hz")
        print(f"  Excitation/Inhibition Ratio: {stats['excitation_inhibition_ratio']:.2f}")
        print(f"  Pending Spike Events: {stats['pending_spike_events']}")
        
        print(f"\nPlasticity:")
        plasticity = stats['plasticity']
        print(f"  STDP Enabled: {plasticity['stdp_enabled']}")
        print(f"  Hebbian Enabled: {plasticity['hebbian_enabled']}")
        print(f"  Structural Enabled: {plasticity['structural_enabled']}")
        
        print(f"\nDevelopment:")
        dev = stats['development']
        print(f"  Stage: {dev['stage_name']}")
        print(f"  Is Developing: {dev['is_developing']}")
        
        print("="*60)

def run_basic_examples():
    """Run basic examples demonstrating the enhanced API."""
    print("Running Basic NLM Examples")
    print("="*50)
    
    # Example 1: Basic brain creation and analysis
    print("\n1. Basic Brain Creation and Analysis:")
    analyzer = NLMAnalyzer("simple")
    analyzer.print_summary()
    
    # Example 2: Network topology analysis
    print("\n2. Network Topology Analysis:")
    topology = analyzer.brain.analyze_network_topology()
    print(f"   Regions: {topology['region_count']}")
    print(f"   Overall Stats: {topology['overall_stats']}")
    
    # Example 3: Learning experiment
    print("\n3. Learning Experiment:")
    learning_results = analyzer.create_learning_experiment()
    print(f"   Learning Effectiveness: {learning_results['learning_effectiveness']:.2f}")
    print(f"   Final Spike Count: {learning_results['final_stats']['total_spikes']}")
    
    # Example 4: Agent simulation
    print("\n4. Agent Simulation:")
    agent_results = analyzer.run_agent_simulation(200)
    print(f"   Steps Completed: {agent_results['total_steps']}")
    print(f"   Final Position: ({agent_results['final_position']['x']:.1f}, {agent_results['final_position']['y']:.1f})")
    print(f"   Average Reward: {agent_results['average_reward']:.2f}")
    
    # Example 5: Performance monitoring
    print("\n5. Performance Monitoring:")
    metrics = analyzer.get_performance_metrics()
    print(f"   Neurons: {metrics['neuron_count']}")
    print(f"   Synapses: {metrics['synapse_count']}")
    print(f"   Average Firing Rate: {metrics['average_firing_rate']:.2f} Hz")
    
    return analyzer

def run_advanced_examples():
    """Run advanced examples showcasing advanced features."""
    print("\nRunning Advanced NLM Examples")
    print("="*50)
    
    # Example 1: Complex network with high plasticity
    print("\n1. High Plasticity Network:")
    advanced_analyzer = NLMAnalyzer("high_plasticity")
    
    # Enable enhanced plasticity control
    advanced_analyzer.brain.set_plasticity_parameters({
        "stdp_ltp_weight": 0.05,
        "stdp_ltd_weight": 0.04,
        "enable_hebbian": True,
        "enable_stdp": True,
        "enable_structural": True,
    })
    
    advanced_analyzer.print_summary()
    
    # Run plasticity analysis
    plasticity_results = advanced_analyzer.run_plasticity_analysis(500)
    print(f"   Plasticity Analysis: {plasticity_results['learning_occurred']}")
    print(f"   Synaptic Changes: {plasticity_results['strengthened_synapses']} strengthened, "
          f"{plasticity_results['weakened_synapses']} weakened")
    
    # Example 2: Complex world simulation
    print("\n2. Complex World Simulation:")
    world_analyzer = NLMAnalyzer("complex")
    
    # Run longer agent simulation
    world_results = world_analyzer.run_agent_simulation(500)
    print(f"   Simulation Completed: {world_results['total_steps']} steps")
    print(f"   Success Rate: {world_results['successful_actions']}/{world_results['actions_taken']} "
          f"({world_results['successful_actions']/max(1, world_results['actions_taken'])*100:.1f}%)")
    
    # Example 3: Network report generation
    print("\n3. Network Report Generation:")
    network_report = world_analyzer.brain.generate_network_report()
    print(f"   Total Neurons: {network_report['total_neurons']}")
    print(f"   Network Efficiency: {network_report['average_neurons_per_region']:.1f} avg neurons/region")
    print(f"   Plasticity Status: {network_report['plasticity']}")
    
    return advanced_analyzer, world_analyzer

def run_experiments():
    """Run a series of systematic experiments."""
    print("\nRunning Systematic Experiments")
    print("="*50)
    
    experiments = []
    
    # Experiment 1: Effect of network size on learning
    print("\nExperiment 1: Network Size vs Learning")
    sizes = [50, 100, 200]
    for size in sizes:
        print(f"  Testing size: {size} neurons")
        analyzer = NLMAnalyzer("simple")
        # Temporarily override neuron count
        analyzer.config.set("neuron_count", size)
        analyzer.brain.reset()
        analyzer.brain.initialize()
        
        # Run learning experiment
        results = analyzer.create_learning_experiment({"duration": 1000})
        experiments.append({
            "type": "size_vs_learning",
            "neuron_count": size,
            "learning_effectiveness": results["learning_effectiveness"],
            "final_spikes": results["final_stats"]["total_spikes"],
        })
    
    # Experiment 2: Effect of plasticity parameters
    print("\nExperiment 2: Plasticity Parameters")
    plasticity_configs = [
        {"stdp_ltp_weight": 0.01, "stdp_ltd_weight": 0.005},
        {"stdp_ltp_weight": 0.03, "stdp_ltd_weight": 0.02},
        {"stdp_ltp_weight": 0.05, "stdp_ltd_weight": 0.04},
    ]
    
    for i, params in enumerate(plasticity_configs):
        print(f"  Configuration {i+1}: LTP={params['stdp_ltp_weight']}, LTD={params['stdp_ltd_weight']}")
        analyzer = NLMAnalyzer("simple")
        analyzer.brain.set_plasticity_parameters(params)
        
        results = analyzer.run_plasticity_analysis(500)
        experiments.append({
            "type": "plasticity_params",
            "config_id": i,
            "ltp_weight": params["stdp_ltp_weight"],
            "ltd_weight": params["stdp_ltd_weight"],
            "learning_occurred": results["learning_occurred"],
            "synaptic_changes": results["strengthened_synapses"] + results["weakened_synapses"],
        })
    
    # Summarize experiments
    print("\n" + "="*50)
    print("EXPERIMENT SUMMARY")
    print("="*50)
    
    for exp in experiments:
        print(f"\n{exp['type']}: {exp}")
    
    return experiments

if __name__ == "__main__":
    print("NLM Python API Examples")
    print("="*60)
    
    try:
        # Run basic examples
        basic_analyzer = run_basic_examples()
        
        # Run advanced examples
        advanced_analyzer, world_analyzer = run_advanced_examples()
        
        # Run systematic experiments
        experiment_results = run_experiments()
        
        print("\n" + "="*60)
        print("ALL EXAMPLES COMPLETED SUCCESSFULLY")
        print("="*60)
        
        # Save results to files
        with open("basic_examples_results.json", "w") as f:
            import json
            json.dump({
                "basic_analyzer_stats": basic_analyzer.get_network_statistics(),
                "advanced_analyzer_stats": advanced_analyzer.get_network_statistics(),
                "world_analyzer_stats": world_analyzer.get_network_statistics(),
            }, f, indent=2)
        
        print("Results saved to 'basic_examples_results.json'")
        
    except Exception as e:
        print(f"\nERROR: {e}")
        import traceback
        traceback.print_exc()
        print("\nPlease check your NLM installation and try again.")