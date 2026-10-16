#!/usr/bin/env python3
"""
Simulation orchestration and helper functions for NLM

This module provides high-level simulation orchestration functions that make it easy
to run complete simulations, manage experiments, and coordinate complex scenarios.
"""

import pynlm_enhanced as nlm
import time
import threading
from typing import Callable, Optional, List, Dict, Any
from dataclasses import dataclass
from enum import Enum

class SimulationStatus(Enum):
    NOT_STARTED = "not_started"
    RUNNING = "running"
    PAUSED = "paused"
    COMPLETED = "completed"
    STOPPED = "stopped"

@dataclass
class SimulationStats:
    """Statistics collected during a simulation"""
    steps_completed: int = 0
    start_time: float = 0.0
    end_time: float = 0.0
    total_neurons: int = 0
    total_synapses: int = 0
    max_energy: float = 0.0
    min_energy: float = 0.0
    avg_firing_rate: float = 0.0
    action_counts: Dict[str, int] = None
    reward_history: List[float] = None
    novelty_history: List[float] = None
    curiosity_history: List[float] = None
    
    def __post_init__(self):
        if self.action_counts is None:
            self.action_counts = {}
        if self.reward_history is None:
            self.reward_history = []
        if self.novelty_history is None:
            self.novelty_history = []
        if self.curiosity_history is None:
            self.curiosity_history = []
        
        self.start_time = time.time()

@dataclass
class SimulationConfig:
    """Configuration for simulation runs"""
    brain_config: Optional[nlm.ConfigWrapper] = None
    world_width: int = 20
    world_height: int = 20
    vision_width: int = 10
    vision_height: int = 10
    timestep: float = 0.05
    max_steps: int = 1000
    seed: Optional[int] = None
    enable_reward_modulation: bool = True
    enable_structural_plasticity: bool = False
    enable_development: bool = True
    enable_curiosity: bool = True
    log_interval: int = 50
    start_position: tuple = (2.0, 2.0)
    action_callback: Optional[Callable] = None
    reward_callback: Optional[Callable] = None
    termination_condition: Optional[Callable] = None

class SimulationRunner:
    """High-level simulation runner with advanced features"""
    
    def __init__(self, config: Optional[SimulationConfig] = None):
        self.config = config or SimulationConfig()
        self.status = SimulationStatus.NOT_STARTED
        self.stats = SimulationStats()
        self.brain = None
        self.world = None
        self.agent = None
        self.thread = None
        self._stop_event = threading.Event()
    
    def _initialize_simulation(self):
        """Initialize simulation components"""
        print(f"Initializing simulation with {self.config.world_width}x{self.config.world_height} world")
        
        # Create configuration
        if self.config.brain_config:
            config = self.config.brain_config
        else:
            config = nlm.createDefaultConfig()
            if self.config.seed:
                config.set("random_seed", self.config.seed)
        
        # Create simulation components
        self.brain = nlm.createBrain(config)
        self.world = nlm.createSimpleWorld()
        self.agent = nlm.createAgentBrain(self.brain)
        
        # Configure world
        self.world.configure(
            self.config.world_width,
            self.config.world_height,
            self.config.vision_width,
            self.config.vision_height
        )
        
        # Set start position
        x, y = self.config.start_position
        self.world.setAgentStart(x, y)
        
        # Initialize agent
        self.agent.initialize(self.world)
        
        # Enable features
        if self.config.enable_reward_modulation:
            self.agent.enableRewardModulation(True)
        
        if self.config.enable_structural_plasticity:
            self.agent.enableStructuralPlasticity(True)
        
        if self.config.enable_development:
            self.agent.enableDevelopment(True)
        
        if self.config.enable_curiosity:
            self.agent.enableCuriosity(True)
        
        # Initialize stats
        self.stats.total_neurons = self.brain.getBrain().getTotalNeuronCount()
        self.stats.total_synapses = self.brain.getBrain().getTotalSynapseCount()
        
        print(f"Simulation initialized: {self.stats.total_neurons} neurons, {self.stats.total_synapses} synapses")
    
    def _update_stats(self, step, action, reward, novelty, curiosity):
        """Update simulation statistics"""
        self.stats.steps_completed = step
        
        # Update brain stats
        brain_obj = self.brain.getBrain()
        self.stats.avg_firing_rate = brain_obj.getAverageFiringRate()
        
        # Update action counts
        action_name = str(action)
        self.stats.action_counts[action_name] = self.stats.action_counts.get(action_name, 0) + 1
        
        # Update history
        self.stats.reward_history.append(reward)
        self.stats.novelty_history.append(novelty)
        self.stats.curiosity_history.append(curiosity)
        
        # Update energy stats
        body = self.world.getAgentBody()
        self.stats.max_energy = max(self.stats.max_energy, body.energy)
        self.stats.min_energy = min(self.stats.min_energy, body.energy)
    
    def _should_terminate(self, step):
        """Check if simulation should terminate"""
        # Check step limit
        if step >= self.config.max_steps:
            return True, "Reached maximum step limit"
        
        # Check custom termination condition
        if self.config.termination_condition:
            should_terminate, reason = self.config.termination_condition(step, self.world, self.agent, self.brain)
            if should_terminate:
                return True, reason
        
        # Check for energy exhaustion
        if self.world.getAgentBody().energy <= 0:
            return True, "Agent energy exhausted"
        
        return False, ""
    
    def run(self) -> SimulationStats:
        """Run the simulation"""
        try:
            self.status = SimulationStatus.RUNNING
            self._initialize_simulation()
            
            print(f"Starting simulation with {self.config.max_steps} maximum steps")
            
            for step in range(self.config.max_steps):
                # Check for termination
                should_terminate, reason = self._should_terminate(step)
                if should_terminate:
                    print(f"Simulation terminated: {reason}")
                    break
                
                # Update world
                self.world.update(self.config.timestep)
                
                # Get sensory input
                percept = self.world.getSensoryPercept()
                self.agent.processSensoryInput(percept)
                
                # Brain step
                self.brain.step(step)
                
                # Get action and apply
                action = self.agent.decodeMotorCommand()
                
                # Call action callback if provided
                if self.config.action_callback:
                    self.config.action_callback(step, action, self.world, self.agent, self.brain)
                
                self.world.applyMotorCommand(action, self.world.getSimulationTime())
                
                # Get current levels for callbacks
                body = self.world.getAgentBody()
                reward = 1.0 if body.energy > 50.0 else 0.0
                novelty = self.agent.getNoveltyLevel()
                curiosity = self.agent.getCuriosityLevel()
                
                # Call reward callback if provided
                if self.config.reward_callback:
                    self.config.reward_callback(step, reward, novelty, curiosity, self.world, self.agent, self.brain)
                
                # Apply reward modulation
                if self.config.enable_reward_modulation:
                    self.agent.applyRewardModulation(reward, 0.0)
                
                # Update development
                if self.config.enable_development:
                    self.agent.updateDevelopment(self.config.timestep)
                
                # Update statistics
                self._update_stats(step, action, reward, novelty, curiosity)
                
                # Log progress
                if self.config.log_interval > 0 and step % self.config.log_interval == 0:
                    body = self.world.getAgentBody()
                    stats_str = nlm.formatStats(self.brain.getBrain())
                    print(f"Step {step}: Pos({body.x:.1f}, {body.y:.1f}), "
                          f"Energy: {body.energy:.1f}, "
                          f"Dev: {self.agent.getDevelopmentalStage()}, "
                          f"Neuromod: {self.agent.getNeuromodulationLevel():.2f}")
                    print(stats_str)
            
            # Simulation completed
            self.status = SimulationStatus.COMPLETED
            self.stats.end_time = time.time()
            
            print(f"Simulation completed successfully!")
            print(f"Total steps: {self.stats.steps_completed}")
            print(f"Total simulation time: {self.stats.end_time - self.stats.start_time:.2f} seconds")
            
            return self.stats
            
        except Exception as e:
            print(f"Simulation error: {type(e).__name__}: {e}")
            self.status = SimulationStatus.STOPPED
            raise
    
    def save_checkpoint(self, filepath) -> bool:
        """Save current simulation state"""
        if self.status != SimulationStatus.RUNNING:
            print(f"Cannot save checkpoint: simulation status is {self.status}")
            return False
        
        print(f"Saving checkpoint to {filepath}...")
        success = self.brain.save(filepath)
        
        if success:
            print("Checkpoint saved successfully!")
            
            # Save metadata
            metadata = {
                "steps_completed": self.stats.steps_completed,
                "status": self.status.value,
                "timestamp": time.time(),
                "neuron_count": self.stats.total_neurons,
                "synapse_count": self.stats.total_synapses,
                "max_energy": self.stats.max_energy,
                "min_energy": self.stats.min_energy
            }
            
            import json
            metadata_path = filepath.with_suffix('.meta.json')
            with open(metadata_path, 'w') as f:
                json.dump(metadata, f, indent=2)
            
            print(f"Metadata saved to {metadata_path}")
        
        return success
    
    def resume_from_checkpoint(self, filepath) -> bool:
        """Resume simulation from checkpoint"""
        print(f"Resuming simulation from {filepath}...")
        
        # Check if files exist
        import os
        if not os.path.exists(filepath):
            print(f"Error: Checkpoint file '{filepath}' not found")
            return False
        
        # Try to load metadata if available
        import json
        metadata_path = filepath.with_suffix('.meta.json')
        if os.path.exists(metadata_path):
            with open(metadata_path, 'r') as f:
                metadata = json.load(f)
            print(f"Loaded metadata: {metadata}")
            
            # Restore simulation stats
            self.stats.steps_completed = metadata.get("steps_completed", 0)
            self.stats.max_energy = metadata.get("max_energy", 0.0)
            self.stats.min_energy = metadata.get("min_energy", 0.0)
        
        # Load brain state
        print("Loading brain state...")
        success = self.brain.load(filepath)
        
        if success:
            print("Brain state loaded successfully!")
            
            # Continue with existing world and agent
            print("Simulation resumed successfully!")
            return True
        else:
            print(f"Failed to load brain state from {filepath}")
            return False

def run_scenario(name: str, config: SimulationConfig) -> SimulationStats:
    """Run a named scenario"""
    print(f"\n=== Running Scenario: {name} ===")
    
    runner = SimulationRunner(config)
    
    try:
        stats = runner.run()
        print(f"Scenario '{name}' completed successfully!")
        return stats
    except Exception as e:
        print(f"Scenario '{name}' failed: {type(e).__name__}: {e}")
        raise

def run_reproduction_experiment(base_config: SimulationConfig, 
                                num_repetitions: int = 10,
                                save_results: bool = False) -> List[SimulationStats]:
    """Run a reproduction experiment with multiple repetitions"""
    print(f"\n=== Running Reproduction Experiment ({num_repetitions} repetitions) ===")
    
    results = []
    
    for i in range(num_repetitions):
        print(f"\n--- Repetition {i + 1}/{num_repetitions} ---")
        
        # Create unique config for this repetition
        config = SimulationConfig(
            brain_config=base_config.brain_config,
            world_width=base_config.world_width,
            world_height=base_config.world_height,
            vision_width=base_config.vision_width,
            vision_height=base_config.vision_height,
            timestep=base_config.timestep,
            max_steps=base_config.max_steps,
            seed=base_config.seed + i if base_config.seed else None,  # Different seed each time
            enable_reward_modulation=base_config.enable_reward_modulation,
            enable_structural_plasticity=base_config.enable_structural_plasticity,
            enable_development=base_config.enable_development,
            enable_curiosity=base_config.enable_curiosity,
            log_interval=base_config.log_interval,
            start_position=base_config.start_position
        )
        
        runner = SimulationRunner(config)
        
        try:
            stats = runner.run()
            results.append(stats)
            
            # Save results if requested
            if save_results:
                filename = f"experiment_{i+1:03d}_steps_{stats.steps_completed}.json"
                save_experiment_results(stats, filename)
                
        except Exception as e:
            print(f"Repetition {i + 1} failed: {type(e).__name__}: {e}")
            # Continue with next repetition
    
    # Print summary
    print(f"\n=== Experiment Summary ===")
    print(f"Total repetitions: {len(results)}")
    
    if results:
        avg_steps = sum(r.steps_completed for r in results) / len(results)
        avg_neurons = sum(r.total_neurons for r in results) / len(results)
        avg_energy = sum((r.max_energy + r.min_energy) / 2 for r in results) / len(results)
        
        print(f"Average steps completed: {avg_steps:.1f}")
        print(f"Average neuron count: {avg_neurons:.1f}")
        print(f"Average final energy: {avg_energy:.2f}")
    
    return results

def save_experiment_results(stats: SimulationStats, filename: str):
    """Save experiment results to file"""
    import json
    import datetime
    
    results = {
        "timestamp": datetime.datetime.now().isoformat(),
        "steps_completed": stats.steps_completed,
        "total_simulation_time": stats.end_time - stats.start_time,
        "total_neurons": stats.total_neurons,
        "total_synapses": stats.total_synapses,
        "max_energy": stats.max_energy,
        "min_energy": stats.min_energy,
        "avg_firing_rate": stats.avg_firing_rate,
        "action_counts": stats.action_counts,
        "final_reward": stats.reward_history[-1] if stats.reward_history else 0.0,
        "final_novelty": stats.novelty_history[-1] if stats.novelty_history else 0.0,
        "final_curiosity": stats.curiosity_history[-1] if stats.curiosity_history else 0.0,
        "total_rewards": sum(stats.reward_history),
        "total_novelty": sum(stats.novelty_history),
        "total_curiosity": sum(stats.curiosity_history)
    }
    
    with open(filename, 'w') as f:
        json.dump(results, f, indent=2)
    
    print(f"Results saved to {filename}")

def create_comparison_config(base_config: SimulationConfig, 
                            variations: Dict[str, Any]) -> List[tuple]:
    """Create multiple configurations for comparison"""
    configs = []
    
    for name, variation in variations.items():
        config = SimulationConfig(
            brain_config=base_config.brain_config,
            world_width=variation.get('world_width', base_config.world_width),
            world_height=variation.get('world_height', base_config.world_height),
            vision_width=variation.get('vision_width', base_config.vision_width),
            vision_height=variation.get('vision_height', base_config.vision_height),
            timestep=variation.get('timestep', base_config.timestep),
            max_steps=variation.get('max_steps', base_config.max_steps),
            seed=variation.get('seed', base_config.seed),
            enable_reward_modulation=variation.get('enable_reward_modulation', base_config.enable_reward_modulation),
            enable_structural_plasticity=variation.get('enable_structural_plasticity', base_config.enable_structural_plasticity),
            enable_development=variation.get('enable_development', base_config.enable_development),
            enable_curiosity=variation.get('enable_curiosity', base_config.enable_curiosity),
            log_interval=variation.get('log_interval', base_config.log_interval),
            start_position=variation.get('start_position', base_config.start_position)
        )
        configs.append((name, config))
    
    return configs

def run_comparison_experiment(base_config: SimulationConfig, 
                             variations: Dict[str, Any]) -> Dict[str, SimulationStats]:
    """Run a comparison experiment with multiple configurations"""
    print(f"\n=== Running Comparison Experiment ===")
    
    # Create configurations
    configs = create_comparison_config(base_config, variations)
    
    results = {}
    
    for name, config in configs:
        print(f"\n--- Running {name} ---")
        runner = SimulationRunner(config)
        
        try:
            stats = runner.run()
            results[name] = stats
            print(f"{name} completed: {stats.steps_completed} steps")
        except Exception as e:
            print(f"{name} failed: {type(e).__name__}: {e}")
            results[name] = None
    
    # Print comparison summary
    print(f"\n=== Comparison Summary ===")
    print(f"{'Configuration':<20} {'Steps':<10} {'Neurons':<10} {'Energy':<10} {'Firing Rate':<12}")
    print("-" * 70)
    
    for name, stats in results.items():
        if stats:
            avg_energy = (stats.max_energy + stats.min_energy) / 2
            print(f"{name:<20} {stats.steps_completed:<10} {stats.total_neurons:<10} {avg_energy:<10.2f} {stats.avg_firing_rate:<12.2f}")
        else:
            print(f"{name:<20} {'FAILED':<10} {'N/A':<10} {'N/A':<10} {'N/A':<12}")
    
    return results

def monitor_simulation(runner: SimulationRunner, 
                      interval: float = 1.0,
                      callbacks: Optional[Dict[str, Callable]] = None) -> threading.Thread:
    """Monitor simulation progress in a separate thread"""
    if callbacks is None:
        callbacks = {}
    
    def monitor():
        while runner.status == SimulationStatus.RUNNING and not runner._stop_event.is_set():
            try:
                body = runner.world.getAgentBody()
                brain_obj = runner.brain.getBrain()
                
                # Call custom callbacks
                if 'on_step' in callbacks:
                    callbacks['on_step'](runner.stats.steps_completed, body.x, body.y, body.energy)
                
                if 'on_stats' in callbacks:
                    stats = {
                        'steps': runner.stats.steps_completed,
                        'position': (body.x, body.y),
                        'energy': body.energy,
                        'neurons': brain_obj.getTotalNeuronCount(),
                        'firing_rate': brain_obj.getAverageFiringRate()
                    }
                    callbacks['on_stats'](stats)
                
                time.sleep(interval)
                
            except Exception as e:
                print(f"Monitor error: {type(e).__name__}: {e}")
                break
    
    thread = threading.Thread(target=monitor, daemon=True)
    thread.start()
    return thread

# Example usage functions
def create_phase3_config():
    """Create configuration for Phase 3 experiment"""
    config = SimulationConfig()
    config.max_steps = 500
    config.world_width = 30
    config.world_height = 30
    config.vision_width = 8
    config.vision_height = 8
    config.enable_reward_modulation = True
    config.enable_development = True
    config.enable_curiosity = False  # Focus on basic learning
    config.log_interval = 100
    return config

def create_phase4_config():
    """Create configuration for Phase 4 experiment"""
    config = SimulationConfig()
    config.max_steps = 800
    config.world_width = 40
    config.world_height = 40
    config.vision_width = 12
    config.vision_height = 12
    config.enable_reward_modulation = True
    config.enable_structural_plasticity = True
    config.enable_development = True
    config.enable_curiosity = True
    config.log_interval = 100
    return config

def create_phase6_config():
    """Create configuration for Phase 6 experiment"""
    config = SimulationConfig()
    config.max_steps = 1200
    config.world_width = 50
    config.world_height = 50
    config.vision_width = 15
    config.vision_height = 15
    config.timestep = 0.03
    config.enable_reward_modulation = True
    config.enable_structural_plasticity = True
    config.enable_development = True
    config.enable_curiosity = True
    config.log_interval = 100
    return config

def run_phase3_demo():
    """Run Phase 3 demonstration"""
    print("Running Phase 3 Demonstration")
    config = create_phase3_config()
    return run_scenario("Phase 3 Demo", config)

def run_phase4_demo():
    """Run Phase 4 demonstration"""
    print("Running Phase 4 Demonstration")
    config = create_phase4_config()
    return run_scenario("Phase 4 Demo", config)

def run_phase6_demo():
    """Run Phase 6 demonstration"""
    print("Running Phase 6 Demonstration")
    config = create_phase6_config()
    return run_scenario("Phase 6 Demo", config)

# Main example function
def main_example():
    """Example showing how to use the simulation helpers"""
    print("=== NLM Simulation Helpers Example ===")
    
    # Create a base configuration
    base_config = SimulationConfig()
    base_config.max_steps = 200
    base_config.world_width = 25
    base_config.world_height = 25
    base_config.vision_width = 8
    base_config.vision_height = 8
    base_config.enable_reward_modulation = True
    base_config.enable_development = True
    base_config.log_interval = 50
    
    # Define variations for comparison
    variations = {
        "Basic": {},
        "Enhanced": {
            "enable_curiosity": True,
            "enable_structural_plasticity": True
        },
        "Development": {
            "enable_development": True,
            "enable_curiosity": True,
            "enable_structural_plasticity": True
        }
    }
    
    # Run comparison experiment
    results = run_comparison_experiment(base_config, variations)
    
    print(f"\n=== Example Complete ===")
    print("The simulation helpers provide:")
    print("1. Easy-to-use SimulationRunner class")
    print("2. Configuration management")
    print("3. Statistics collection and monitoring")
    print("4. Checkpoint save/load functionality")
    print("5. Experiment orchestration and comparison")
    print("6. Reproduction experiments")
    print("7. Thread-based monitoring")

if __name__ == "__main__":
    main_example()