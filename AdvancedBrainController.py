#!/usr/bin/env python3
"""
Advanced NLM Brain Control Commands

This script provides enhanced control and debugging commands for the NLM brain.
It's designed for power users who need deeper control over the neural simulation.
"""

import json
import time
from typing import Dict, List, Any, Optional
import pynlm

class NLMBrainController:
    """Enhanced controller for NLM brain with advanced functionality."""
    
    def __init__(self, config_file: str = None):
        self.config = pynlm.createDefaultConfig()
        if config_file:
            self.config.loadFromFile(config_file)
        self.brain = None
        self.agent = None
        self.world = None
    
    def create_brain(self, neuron_count: int = None, region_count: int = None):
        """Create a brain with custom configuration."""
        if neuron_count is not None:
            self.config.set("brain.neuron_count", neuron_count)
        if region_count is not None:
            self.config.set("brain.region_count", region_count)
        
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        return self.brain
    
    def create_agent_with_world(self, width: int = 20, height: int = 20):
        """Create a complete agent setup with world."""
        self.create_brain()
        self.agent = pynlm.createAgentBrain(self.brain)
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width, height, 8, 8)
        self.world.reset()
        self.agent.initialize(self.world)
        
        return self.brain, self.agent, self.world
    
    def brain_statistics(self) -> Dict[str, Any]:
        """Get comprehensive brain statistics."""
        if not self.brain:
            return {"error": "No brain created"}
        
        stats = {
            "basic_info": {
                "total_neurons": self.brain.getTotalNeuronCount(),
                "total_synapses": self.brain.getTotalSynapseCount(),
                "firing_neuron_count": self.brain.getFiringNeuronCount(),
                "active_neuron_count": self.brain.getActiveNeuronCount(),
                "average_firing_rate": self.brain.getAverageFiringRate(),
                "excitatory_inhibitory_ratio": self.brain.getExcitationInhibitionRatio(),
                "total_spikes": self.brain.getTotalSpikeCount(),
                "pending_spikes": self.brain.getPendingSpikeEventCount(),
            },
            "memory_systems": {},
            "development": {
                "stage": self.brain.getDevelopmentalStage(),
            }
        }
        
        # Add memory system stats if available
        if self.agent:
            if self.agent.getBrain():
                working_mem = self.agent.getBrain().getWorkingMemory()
                if working_mem:
                    stats["memory_systems"]["working_memory"] = {
                        "capacity": working_mem.getCapacity(),
                        "active_traces": working_mem.getActiveTraces(),
                    }
                
                episodic_mem = self.agent.getBrain().getEpisodicMemory()
                if episodic_mem:
                    stats["memory_systems"]["episodic_memory"] = {
                        "episode_count": episodic_mem.getEpisodeCount(),
                        "replay_enabled": episodic_mem.isReplayEnabled(),
                    }
        
        return stats
    
    def run_advanced_simulation(
        self, 
        steps: int, 
        enable_learning: bool = True,
        enable_development: bool = True,
        enable_curiosity: bool = True,
        enable_structural_plasticity: bool = True
    ) -> Dict[str, Any]:
        """Run a simulation with advanced features."""
        if not self.agent:
            return {"error": "No agent created"}
        
        # Enable features
        if enable_learning:
            self.agent.enableRewardModulation(True)
        if enable_development:
            self.agent.enableDevelopment(True)
        if enable_curiosity:
            self.agent.enableCuriosity(True)
        if enable_structural_plasticity:
            self.agent.enableStructuralPlasticity(True)
        
        simulation_start = time.time()
        stats = {
            "steps_completed": steps,
            "start_time": simulation_start,
            "events": [],
            "memory_growth": [],
            "neuromodulation_levels": [],
        }
        
        # Run simulation with detailed tracking
        for step in range(steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Run brain
            self.brain.step(step)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            
            # Apply action to world
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Get reward and apply neuromodulation
            reward = self.world.getSensoryPercept().getInternal()[0] if self.world.getSensoryPercept().getInternal() else 0.0
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Record stats every 100 steps
            if step % 100 == 0:
                step_stats = {
                    "step": step,
                    "time": self.world.getSimulationTime(),
                    "neurons_firing": self.brain.getFiringNeuronCount(),
                    "reward": reward,
                    "curiosity": self.agent.getCuriosityLevel(),
                    "novelty": self.agent.getNoveltyLevel(),
                    "prediction_error": self.agent.getPredictionError(),
                    "development_stage": self.brain.getDevelopmentalStage(),
                }
                stats["events"].append(step_stats)
                
                # Memory growth tracking
                if self.brain.getEpisodicMemory():
                    stats["memory_growth"].append(self.brain.getEpisodicMemory().getEpisodeCount())
                
                # Neuromodulation levels
                stats["neuromodulation_levels"].append({
                    "dopamine": self.agent.getNeuromodulationLevel(),
                    "curiosity": self.agent.getCuriosityLevel(),
                    "novelty": self.agent.getNoveltyLevel(),
                    "prediction_error": self.agent.getPredictionError(),
                })
        
        simulation_end = time.time()
        stats["end_time"] = simulation_end
        stats["total_time"] = simulation_end - simulation_start
        stats["steps_per_second"] = steps / stats["total_time"] if stats["total_time"] > 0 else 0
        
        return stats
    
    def explore_memory(self) -> Dict[str, Any]:
        """Explore and analyze memory contents."""
        if not self.brain:
            return {"error": "No brain created"}
        
        exploration = {
            "working_memory": {},
            "episodic_memory": {},
            "associative_memory": {},
        }
        
        # Explore working memory
        working_mem = self.brain.getWorkingMemory()
        if working_mem:
            exploration["working_memory"] = {
                "capacity": working_mem.getCapacity(),
                "current_size": working_mem.getCurrentSize(),
                "memory_activity": working_mem.getMemoryActivity(),
                "active_traces": working_mem.getActiveTraces(),
            }
        
        # Explore episodic memory
        episodic_mem = self.brain.getEpisodicMemory()
        if episodic_mem:
            exploration["episodic_memory"] = {
                "total_episodes": episodic_mem.getEpisodeCount(),
                "recent_episodes": episodic_mem.getRecentEpisodes(5),
                "replay_enabled": episodic_mem.isReplayEnabled(),
                "average_reward": episodic_mem.getAverageReward(),
            }
        
        # Explore associative memory
        associative_mem = self.brain.getAssociativeMemory()
        if associative_mem:
            exploration["associative_memory"] = {
                "association_count": associative_mem.getAssociationCount(),
                "patterns": len(associative_mem.getPatternNeurons()) if hasattr(associative_mem, 'getPatternNeurons') else 0,
            }
        
        return exploration
    
    def optimize_brain(self, optimization_target: str = "performance") -> Dict[str, Any]:
        """Apply optimization strategies to the brain."""
        if not self.brain:
            return {"error": "No brain created"}
        
        optimization = {
            "target": optimization_target,
            "applied_optimizations": [],
            "results": {},
        }
        
        # Apply optimizations based on target
        if optimization_target == "performance":
            # Performance optimizations
            self.config.set("brain.synapse_density", 0.05)  # Lower density for speed
            self.config.set("brain.connection_probability", 0.02)
            optimization["applied_optimizations"].extend([
                "reduced_synapse_density",
                "reduced_connection_probability"
            ])
            
            # Recreate brain with optimized config
            self.brain = pynlm.createBrain(self.config)
            self.brain.initialize()
            
        elif optimization_target == "plasticity":
            # Plasticity optimizations
            self.config.set("plasticity.stdp.enable", True)
            self.config.set("plasticity.stdp.learning_rate", 0.001)
            self.config.set("plasticity.hebbian.enable", True)
            self.config.set("plasticity.structural.enable", True)
            optimization["applied_optimizations"].extend([
                "enabled_stdp",
                "configured_hebbian",
                "enabled_structural_plasticity"
            ])
            
        elif optimization_target == "development":
            # Development optimizations
            self.config.set("neuromod.curiosity.enable", True)
            self.config.set("neuromod.novelty.enable", True)
            optimization["applied_optimizations"].extend([
                "enabled_curiosity",
                "enabled_novelty_detection"
            ])
        
        optimization["results"]["new_config"] = self.config.summary()
        
        return optimization
    
    def save_brain_state(self, filepath: str) -> bool:
        """Save the current brain state to a file."""
        if not self.brain:
            return False
        
        # TODO: Implement brain saving
        # self.brain.save(filepath)
        
        # For now, save basic state
        state = {
            "brain_stats": self.brain_statistics(),
            "config": self.config.summary(),
        }
        
        with open(filepath, 'w') as f:
            json.dump(state, f, indent=2)
        
        return True
    
    def load_brain_state(self, filepath: str) -> bool:
        """Load brain state from a file."""
        try:
            with open(filepath, 'r') as f:
                state = json.load(f)
            
            # TODO: Implement brain loading
            # self.brain.load(filepath)
            
            return True
        except Exception as e:
            print(f"Error loading brain state: {e}")
            return False


# Example usage functions
def create_advanced_brain_demo():
    """Demonstrate advanced brain creation with custom features."""
    print("=== Advanced NLM Brain Demo ===")
    
    # Create controller
    controller = NLMBrainController()
    
    # Create advanced brain with custom settings
    brain = controller.create_brain(neuron_count=2000, region_count=2)
    
    # Initialize brain
    brain.initialize()
    
    # Print statistics
    stats = controller.brain_statistics()
    print(f"\nBrain Statistics:")
    print(f"  Total Neurons: {stats['basic_info']['total_neurons']}")
    print(f"  Total Synapses: {stats['basic_info']['total_synapses']}")
    print(f"  Average Firing Rate: {stats['basic_info']['average_firing_rate']:.2f}")
    print(f"  E/I Ratio: {stats['basic_info']['excitatory_inhibitory_ratio']:.2f}")
    
    # Run simulation
    print(f"\nRunning simulation for 500 steps...")
    sim_stats = controller.run_advanced_simulation(
        steps=500,
        enable_learning=True,
        enable_development=True,
        enable_curiosity=True,
        enable_structural_plasticity=True
    )
    
    print(f"\nSimulation Complete:")
    print(f"  Steps per second: {sim_stats['steps_per_second']:.1f}")
    print(f"  Total time: {sim_stats['total_time']:.2f}s")
    
    # Explore memory
    memory_explore = controller.explore_memory()
    print(f"\nMemory Exploration:")
    for mem_type, mem_data in memory_explore.items():
        print(f"  {mem_type}: {mem_data}")
    
    print("\n=== Demo Complete ===")
    return controller

if __name__ == "__main__":
    # Run demo
    controller = create_advanced_brain_demo()
    
    # Save brain state
    controller.save_brain_state("nlm_brain_state.json")
    print("\nBrain state saved to nlm_brain_state.json")

# Command line interface for power users
def cli():
    """Simple command line interface for advanced users."""
    import argparse
    
    parser = argparse.ArgumentParser(description="Advanced NLM Brain Controller")
    parser.add_argument("--config", help="Configuration file path")
    parser.add_argument("--create", action="store_true", help="Create brain with custom config")
    parser.add_argument("--neurons", type=int, help="Number of neurons to create")
    parser.add_argument("--regions", type=int, help="Number of brain regions to create")
    parser.add_argument("--simulate", type=int, help="Run simulation for N steps")
    parser.add_argument("--stats", action="store_true", help="Print brain statistics")
    parser.add_argument("--save", help="Save brain state to file")
    
    args = parser.parse_args()
    
    # Initialize controller
    controller = NLMBrainController(args.config)
    
    # Create brain if requested
    if args.create or args.neurons or args.regions:
        brain = controller.create_brain(args.neurons, args.regions)
    
    # Run simulation if requested
    if args.simulate:
        print(f"Running simulation for {args.simulate} steps...")
        stats = controller.run_advanced_simulation(args.simulate)
        print(f"Simulation complete. Steps per second: {stats['steps_per_second']:.1f}")
    
    # Print stats if requested
    if args.stats:
        stats = controller.brain_statistics()
        print(json.dumps(stats, indent=2))
    
    # Save if requested
    if args.save:
        if controller.save_brain_state(args.save):
            print(f"Brain state saved to {args.save}")
        else:
            print("Failed to save brain state")

if __name__ == "__main__":
    cli()
