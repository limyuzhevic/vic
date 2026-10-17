#!/usr/bin/env python3
"""
NLM Memory Analysis and Inspection Tools

This script provides utilities for analyzing and debugging NLM (Neural Learning Machine)
brain states, memory systems, and simulation results. It can be used as a command-line
tool or imported for programmatic access to memory inspection functionality.
"""

import sys
import json
from typing import Dict, List, Any, Optional

# Add pynlm import - assuming it's available after building
try:
    import pynlm
except ImportError:
    print("Error: pynlm module not found. Please install the NLM Python bindings.")
    sys.exit(1)


class MemoryAnalyzer:
    """Analyzes brain memory systems and provides insights."""
    
    def __init__(self, brain):
        self.brain = brain
        self.agent = None
        
    def get_working_memory_analysis(self) -> Dict[str, Any]:
        """Analyze working memory state and statistics."""
        if not self.brain.getWorkingMemory():
            return {"error": "Working memory not available"}
            
        wm = self.brain.getWorkingMemory()
        return {
            "active_traces": wm.getActiveTraces(),
            "capacity": wm.getCapacity(),
            "memory_neuron_count": len(wm.getMemoryNeurons()),
            "memory_activity": wm.getMemoryActivity(),
            "decay_rate": wm.getDecayRate(),
            "average_activation": self._calculate_average_activation(wm)
        }
    
    def get_episodic_memory_analysis(self) -> Dict[str, Any]:
        """Analyze episodic memory state."""
        if not self.brain.getEpisodicMemory():
            return {"error": "Episodic memory not available"}
            
        em = self.brain.getEpisodicMemory()
        return {
            "episode_count": em.getEpisodeCount(),
            "max_episodes": em.getMaxEpisodes(),
            "replay_enabled": em.isReplayEnabled(),
            "recent_episodes": em.getRecentEpisodes(5),
            "average_reward": em.getAverageReward(),
            "memory_efficiency": self._calculate_memory_efficiency(em)
        }
    
    def get_prediction_analysis(self) -> Dict[str, Any]:
        """Analyze prediction system state."""
        if not self.brain.getPredictionSystem():
            return {"error": "Prediction system not available"}
            
        pred = self.brain.getPredictionSystem()
        return {
            "prediction_error": pred.getPredictionError(),
            "confidence": pred.getConfidence(),
            "error_history": pred.getErrorHistory(),
            "prediction_stability": self._calculate_prediction_stability(pred)
        }
    
    def get_cognition_analysis(self) -> Dict[str, Any]:
        """Analyze cognitive systems state."""
        analysis = {}
        
        # Neural planner
        if self.brain.getPlanner():
            planner = self.brain.getPlanner()
            analysis["planner"] = {
                "initialized": True,
                "planning_depth": planner.getPlanningDepth()
            }
        
        # Concept formation
        if self.brain.getConceptFormation():
            analysis["concept_formation"] = {
                "initialized": True
            }
        
        # Attention system
        if self.brain.getAttention():
            attention = self.brain.getAttention()
            analysis["attention"] = {
                "initialized": True,
                "inhibition_strength": attention.getInhibitionStrength(),
                "excitation_strength": attention.getExcitationStrength(),
                "winners_count": len(attention.getWinners())
            }
        
        return analysis
    
    def get_neuromodulation_analysis(self) -> Dict[str, Any]:
        """Analyze neuromodulation systems."""
        analysis = {}
        
        for name, system in [
            ("dopamine", self.brain.getDopamine()),
            ("curiosity", self.brain.getCuriosity()),
            ("novelty", self.brain.getNovelty()),
            ("prediction_error", self.brain.getPredictionErrorSignal())
        ]:
            if system:
                analysis[name] = {
                    "level": system.getLevel(),
                    "initialized": True
                }
        
        return analysis
    
    def get_development_analysis(self) -> Dict[str, Any]:
        """Analyze developmental stage and effects."""
        return {
            "developmental_stage": self.brain.getDevelopmentalStage(),
            "developmental_stage_name": self._get_stage_name(self.brain.getDevelopmentalStage()),
            "developmental_age": self.brain.getDevelopmentSystem().getDevelopmentalAge() if self.brain.getDevelopmentSystem() else 0.0,
            "plasticity_modifier": self.brain.getDevelopmentSystem().getPlasticityModifier() if self.brain.getDevelopmentSystem() else 1.0
        }
    
    def get_network_stats(self) -> Dict[str, Any]:
        """Get overall network statistics."""
        return {
            "neuron_count": self.brain.getTotalNeuronCount(),
            "synapse_count": self.brain.getTotalSynapseCount(),
            "region_count": self.brain.getRegionCount(),
            "active_neuron_count": self.brain.getActiveNeuronCount(),
            "firing_neuron_count": self.brain.getFiringNeuronCount(),
            "average_firing_rate": self.brain.getAverageFiringRate(),
            "excitation_inhibition_ratio": self.brain.getExcitationInhibitionRatio(),
            "pending_spike_events": self.brain.getPendingSpikeEventCount()
        }
    
    def get_comprehensive_report(self) -> Dict[str, Any]:
        """Generate a comprehensive report of brain state."""
        return {
            "timestamp": self._get_current_timestamp(),
            "network_stats": self.get_network_stats(),
            "working_memory": self.get_working_memory_analysis(),
            "episodic_memory": self.get_episodic_memory_analysis(),
            "prediction": self.get_prediction_analysis(),
            "cognition": self.get_cognition_analysis(),
            "neuromodulation": self.get_neuromodulation_analysis(),
            "development": self.get_development_analysis(),
            "integration_score": self._calculate_integration_score()
        }
    
    def _calculate_average_activation(self, wm) -> float:
        """Calculate average activation in working memory."""
        activations = wm.getMemoryNeurons()
        if not activations:
            return 0.0
        return sum(activations) / len(activations)
    
    def _calculate_memory_efficiency(self, em) -> float:
        """Calculate memory efficiency based on episode count vs capacity."""
        capacity = em.getMaxEpisodes()
        if capacity == 0:
            return 0.0
        return static_cast<float>(em.getEpisodeCount()) / capacity
    
    def _calculate_prediction_stability(self, pred) -> float:
        """Calculate prediction stability based on error history."""
        history = pred.getErrorHistory()
        if not history:
            return 0.0
        # Lower variance indicates more stable predictions
        mean_error = sum(history) / len(history)
        if mean_error == 0:
            return 1.0
        variance = sum((x - mean_error) ** 2 for x in history) / len(history)
        return max(0.0, 1.0 - variance / mean_error)
    
    def _get_stage_name(self, stage) -> str:
        """Get human-readable name for developmental stage."""
        stage_names = {
            pynlm.DevelopmentalStage.Initial: "Initial",
            pynlm.DevelopmentalStage.CriticalPeriod: "Critical Period",
            pynlm.DevelopmentalStage.Maturation: "Maturation",
            pynlm.DevelopmentalStage.Adult: "Adult",
            pynlm.DevelopmentalStage.Aging: "Aging"
        }
        return stage_names.get(stage, "Unknown")
    
    def _get_current_timestamp(self) -> str:
        """Get current timestamp for reporting."""
        import datetime
        return datetime.datetime.now().isoformat()
    
    def _calculate_integration_score(self) -> Dict[str, float]:
        """Calculate integration score for different brain systems."""
        scores = {}
        
        # Check which systems are present and active
        if self.brain.getWorkingMemory() and self.brain.getWorkingMemory().getActiveTraces() > 0:
            scores["working_memory"] = 1.0
        else:
            scores["working_memory"] = 0.0
            
        if self.brain.getEpisodicMemory() and self.brain.getEpisodicMemory().getEpisodeCount() > 0:
            scores["episodic_memory"] = 1.0
        else:
            scores["episodic_memory"] = 0.0
            
        if self.brain.getPredictionSystem() and self.brain.getPredictionSystem().getPredictionError() < 0.5:
            scores["prediction_system"] = 1.0
        else:
            scores["prediction_system"] = 0.0
        
        if self.brain.getPlanner():
            scores["planning"] = 1.0
        else:
            scores["planning"] = 0.0
        
        overall = sum(scores.values()) / len(scores) if scores else 0.0
        return {"overall": overall, "details": scores}


class BrainSimulator:
    """High-level simulation controller with built-in analysis."""
    
    def __init__(self, config=None):
        self.config = config
        self.brain = None
        self.agent = None
        self.world = None
        
    def run_episode(self, num_steps: int, enable_analysis: bool = True) -> Dict[str, Any]:
        """Run a complete simulation episode with analysis."""
        if not self.brain:
            self.brain = pynlm.createBrain(self.config)
            self.brain.initialize()
        
        results = {
            "steps": num_steps,
            "start_time": self._get_current_timestamp(),
            "final_stats": {},
            "memory_analysis": {},
            "learning_metrics": {}
        }
        
        if self.agent and self.world:
            for step in range(num_steps):
                # Update world
                self.world.update(0.1)
                
                # Get sensory input and process
                percept = self.world.getSensoryPercept()
                self.agent.processSensoryInput(percept)
                
                # Run brain step
                self.brain.step(step)
                
                # Get action and apply
                motor_cmd = self.agent.decodeMotorCommand()
                self.world.applyMotorCommand(motor_cmd, self.world.getSimulationTime())
                
                # Apply reward modulation
                reward = self._get_reward_from_percept(percept)
                self.agent.applyRewardModulation(reward, 0.0)
                
                # Update development
                self.agent.updateDevelopment(0.1)
                
                # Analyze every 100 steps
                if enable_analysis and step % 100 == 0:
                    results["memory_analysis"][step] = self._get_memory_snapshot()
        
        results["end_time"] = self._get_current_timestamp()
        results["final_stats"] = self.get_brain_stats()
        
        return results
    
    def get_brain_stats(self) -> Dict[str, Any]:
        """Get comprehensive brain statistics."""
        analyzer = MemoryAnalyzer(self.brain)
        return {
            "network": analyzer.get_network_stats(),
            "memory": {
                "working": analyzer.get_working_memory_analysis(),
                "episodic": analyzer.get_episodic_memory_analysis()
            },
            "cognition": analyzer.get_cognition_analysis(),
            "neuromodulation": analyzer.get_neuromodulation_analysis(),
            "development": analyzer.get_development_analysis(),
            "integration_score": analyzer._calculate_integration_score()
        }
    
    def _get_reward_from_percept(self, percept) -> float:
        """Extract reward from sensory percept."""
        internal = percept.getInternal()
        if internal and len(internal) > 0:
            return internal[0]
        return 0.0
    
    def _get_memory_snapshot(self) -> Dict[str, Any]:
        """Get current memory state snapshot."""
        analyzer = MemoryAnalyzer(self.brain)
        return {
            "working_memory": analyzer.get_working_memory_analysis(),
            "episodic_memory": analyzer.get_episodic_memory_analysis(),
            "prediction": analyzer.get_prediction_analysis()
        }
    
    def _get_current_timestamp(self) -> str:
        """Get current timestamp."""
        import datetime
        return datetime.datetime.now().isoformat()


def analyze_brain(brain) -> MemoryAnalyzer:
    """Create a memory analyzer for the given brain."""
    return MemoryAnalyzer(brain)


def run_simulation_with_analysis(config, num_steps: int, verbose: bool = True) -> Dict[str, Any]:
    """Run a simulation with built-in analysis and reporting."""
    simulator = BrainSimulator(config)
    results = simulator.run_episode(num_steps, enable_analysis=True)
    
    if verbose:
        print_simulation_results(results)
    
    return results


def print_simulation_results(results: Dict[str, Any]) -> None:
    """Print formatted simulation results."""
    print("=== NLM Simulation Results ===\n")
    
    final = results["final_stats"]
    print("Network Statistics:")
    print(f"  Total Neurons: {final['network']['neuron_count']}")
    print(f"  Total Synapses: {final['network']['synapse_count']}")
    print(f"  Active Neurons: {final['network']['active_neuron_count']}")
    print(f"  Firing Rate: {final['network']['average_firing_rate']:.3f}")
    print(f"  E/I Ratio: {final['network']['excitation_inhibition_ratio']:.2f}")
    print()
    
    integration = final["integration_score"]["overall"]
    print(f"Overall Integration Score: {integration:.2%}")
    
    if "details" in final["integration_score"]:
        print("  System Details:")
        for system, score in final["integration_score"]["details"].items():
            print(f"    {system}: {score:.0%}")
    print()
    
    # Memory analysis summary
    if "working_memory" in final["memory"]:
        wm = final["memory"]["working_memory"]
        if "error" not in wm:
            print(f"Working Memory: {wm.get('active_traces', 0)} active traces")
    
    if "episodic_memory" in final["memory"]:
        em = final["memory"]["episodic_memory"]
        if "error" not in em:
            print(f"Episodic Memory: {em.get('episode_count', 0)} episodes")
    
    print(f"\nSimulation completed in {results['steps']} steps")
    print(f"Started: {results['start_time']}")
    print(f"Ended: {results['end_time']}")


def create_brain_with_config(config_file: str = None) -> pynlm.Brain:
    """Create a brain with optional configuration file."""
    if config_file:
        config = pynlm.createDefaultConfig()
        try:
            config.loadFromFile(config_file)
        except:
            print(f"Warning: Could not load config file {config_file}, using defaults")
    else:
        config = pynlm.createDefaultConfig()
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    return brain


if __name__ == "__main__":
    # Command-line interface
    import argparse
    
    parser = argparse.ArgumentParser(
        description="NLM Memory Analysis and Inspection Tools",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Analyze a running brain
  python nlm_memory_tools.py analyze --brain-file checkpoint.bin
  
  # Run simulation with analysis
  python nlm_memory_tools.py simulate --steps 1000 --verbose
  
  # Quick memory snapshot
  python nlm_memory_tools.py snapshot
        ""
    )
    
    parser.add_argument(
        "action", choices=["analyze", "simulate", "snapshot"],
        help="Action to perform"
    )
    
    parser.add_argument(
        "--steps", type=int, default=100,
        help="Number of simulation steps (for simulate action)"
    )
    
    parser.add_argument(
        "--brain-file", help="Brain checkpoint file (for analyze action)"
    )
    
    parser.add_argument(
        "--verbose", action="store_true", default=True,
        help="Verbose output"
    )
    
    args = parser.parse_args()
    
    try:
        if args.action == "analyze":
            if args.brain_file:
                # Load and analyze brain from checkpoint
                brain = pynlm.Brain()  # Note: This assumes createLoad method exists
                brain.load(args.brain_file)
                analyzer = analyze_brain(brain)
                report = analyzer.get_comprehensive_report()
                print(json.dumps(report, indent=2))
            else:
                print("Error: --brain-file required for analyze action")
                sys.exit(1)
                
        elif args.action == "simulate":
            # Run simulation with analysis
            results = run_simulation_with_analysis(
                config=None, 
                num_steps=args.steps, 
                verbose=args.verbose
            )
            
        elif args.action == "snapshot":
            # Create brain and take snapshot
            brain = create_brain_with_config()
            analyzer = analyze_brain(brain)
            
            print("=== NLM Brain Snapshot ===\n")
            print("Network Stats:", analyzer.get_network_stats())
            print("\nWorking Memory:", analyzer.get_working_memory_analysis())
            print("\nEpisodic Memory:", analyzer.get_episodic_memory_analysis())
            
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)