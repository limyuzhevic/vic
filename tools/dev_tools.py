#!/usr/bin/env python3
"""
Development tools for NLM Python bindings.
Provides helper scripts and utilities for development, testing, and benchmarking.
"""

import sys
import os
import time
import json
import argparse
from pathlib import Path
from typing import Callable, Optional, Tuple
import threading

# Add the current directory to Python path for local imports
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Import pynlm if available
try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Warning: pynlm not available. Some tools may not work.")


def create_development_config(neuron_count: int = 1000, region_count: int = 2, 
                            preset: str = "balanced") -> pynlm.Config:
    """Create a development configuration with common settings."""
    if not PYNLM_AVAILABLE:
        raise RuntimeError("pynlm not available")
    
    config = pynlm.createDefaultConfig()
    
    # Set basic parameters
    config.set("neuron_count", neuron_count)
    config.set("region_count", region_count)
    config.set("connection_probability", 0.1)
    config.set("random_seed", 42)
    config.set("simulation_timestep", 0.001)
    
    # Set preset-specific parameters
    if preset == "minimal":
        config.set("neuron_count", 50)
        config.set("region_count", 1)
        config.set("connection_probability", 0.05)
    
    elif preset == "balanced":
        config.set("neuron_count", 500)
        config.set("region_count", 3)
        config.set("connection_probability", 0.1)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
    
    elif preset == "high_learning":
        config.set("neuron_count", 2000)
        config.set("region_count", 4)
        config.set("connection_probability", 0.15)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.structural.enable", True)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
    
    elif preset == "phase6_full":
        # Full Phase 6 integration configuration
        config.set("neuron_count", 1000)
        config.set("region_count", 2)
        config.set("connection_probability", 0.1)
        config.set("random_seed", 42)
        config.set("simulation_timestep", 0.001)
        config.set("replay_interval", 100)
        config.set("consolidation_interval", 1000)
        
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.001)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.structural.enable", True)
        
        config.set("neuromod.dopamine.scale", 1.0)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        
        config.set("sensory.input_size", 24)
        config.set("motor.output_size", 8)
    
    return config


def create_agent_simulation(world_type: str = "simple") -> Tuple[pynlm.Brain, pynlm.AgentBrain, pynlm.SimpleWorld]:
    """Create a complete agent simulation with brain, agent, and world."""
    if not PYNLM_AVAILABLE:
        raise RuntimeError("pynlm not available")
    
    # Create configuration
    config = create_development_config(preset="balanced")
    
    # Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Initialize agent with world
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    return brain, agent, world


def run_simulation(brain: pynlm.Brain, agent: pynlm.AgentBrain, world: pynlm.SimpleWorld, 
                   num_steps: int = 1000, progress_callback: Optional[Callable[[int, float], None]] = None) -> Tuple[int, float]:
    """Run a complete simulation with progress tracking."""
    if not PYNLM_AVAILABLE:
        raise RuntimeError("pynlm not available")
    
    print(f"Starting simulation for {num_steps} steps...")
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process in brain
        agent.processSensoryInput(percept)
        
        # Run brain simulation
        brain.step(step)
        
        # Get motor command
        action = agent.decodeMotorCommand()
        
        # Apply to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation based on sensory internal signals
        internal_signals = percept.getInternal()
        reward = internal_signals[0] if internal_signals else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Report progress
        if progress_callback:
            progress = step / num_steps
            progress_callback(step, progress)
        
        # Occasionally print status
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons, "
                  f"{agent.getCuriosityLevel():.3f} curiosity, "
                  f"{agent.getNoveltyLevel():.3f} novelty")
    
    print("Simulation complete!")
    
    total_spikes = brain.getTotalSpikeCount()
    avg_firing_rate = brain.getAverageFiringRate()
    
    print(f"Final stats: {total_spikes} total spikes, {avg_firing_rate:.2f} avg firing rate")
    print(f"Development stage: {brain.getDevelopmentalStage()}")
    print(f"Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    
    return total_spikes, avg_firing_rate


def validate_config(config: pynlm.Config) -> dict:
    """Validate and fix common configuration issues."""
    if not PYNLM_AVAILABLE:
        raise RuntimeError("pynlm not available")
    
    issues_found = []
    fixes_applied = []
    
    # Check neuron count
    if config.has("neuron_count"):
        neuron_count = config.getOr[int64_t]("neuron_count", 0)
        if neuron_count < 10:
            issues_found.append(f"Neuron count too low: {neuron_count}")
            config.set("neuron_count", 100)
            fixes_applied.append(f"Set neuron count to 100")
    
    # Check region count
    if config.has("region_count"):
        region_count = config.getOr[int64_t]("region_count", 0)
        if region_count < 1:
            issues_found.append(f"Region count too low: {region_count}")
            config.set("region_count", 2)
            fixes_applied.append(f"Set region count to 2")
    
    # Check connection probability
    if config.has("connection_probability"):
        conn_prob = config.getOr[float]("connection_probability", 0.0)
        if conn_prob <= 0.0 or conn_prob > 1.0:
            issues_found.append(f"Invalid connection probability: {conn_prob}")
            config.set("connection_probability", 0.1)
            fixes_applied.append(f"Set connection probability to 0.1")
    
    # Check random seed
    if not config.has("random_seed"):
        issues_found.append("Missing random seed")
        config.set("random_seed", 42)
        fixes_applied.append(f"Set random seed to 42")
    
    return {
        "issues_found": issues_found,
        "fixes_applied": fixes_applied,
        "config_valid": len(issues_found) == 0
    }


def get_config_summary(config: pynlm.Config) -> dict:
    """Get a comprehensive summary of configuration."""
    if not PYNLM_AVAILABLE:
        raise RuntimeError("pynlm not available")
    
    summary = {}
    
    # Basic parameters
    if config.has("neuron_count"):
        summary["neuron_count"] = config.getOr[int64_t]("neuron_count", 0)
    if config.has("region_count"):
        summary["region_count"] = config.getOr[int64_t]("region_count", 0)
    if config.has("connection_probability"):
        summary["connection_probability"] = config.getOr[float]("connection_probability", 0.0)
    if config.has("random_seed"):
        summary["random_seed"] = config.getOr[int64_t]("random_seed", 0)
    if config.has("simulation_timestep"):
        summary["simulation_timestep"] = config.getOr[float]("simulation_timestep", 0.0)
    
    # Plasticity settings
    if config.has("plasticity.stdp.enable"):
        summary["stdp_enabled"] = config.getOr[bool]("plasticity.stdp.enable", False)
    if config.has("plasticity.hebbian.enable"):
        summary["hebbian_enabled"] = config.getOr[bool]("plasticity.hebbian.enable", False)
    if config.has("plasticity.structural.enable"):
        summary["structural_enabled"] = config.getOr[bool]("plasticity.structural.enable", False)
    
    # Neuromodulation settings
    if config.has("neuromod.dopamine.scale"):
        summary["dopamine_scale"] = config.getOr[float]("neuromod.dopamine.scale", 0.0)
    if config.has("neuromod.curiosity.enable"):
        summary["curiosity_enabled"] = config.getOr[bool]("neuromod.curiosity.enable", False)
    if config.has("neuromod.novelty.enable"):
        summary["novelty_enabled"] = config.getOr[bool]("neuromod.novelty.enable", False)
    
    return summary


def compare_configs(config1: pynlm.Config, config2: pynlm.Config) -> dict:
    """Compare two configurations and show differences."""
    if not PYNLM_AVAILABLE:
        raise RuntimeError("pynlm not available")
    
    keys1 = config1.getKeys()
    keys2 = config2.getKeys()
    
    all_keys = set(keys1) | set(keys2)
    differences = {}
    
    for key in all_keys:
        val1 = config1.getOr[str](key, "") if config1.has(key) else "(missing)"
        val2 = config2.getOr[str](key, "") if config2.has(key) else "(missing)"
        
        # For numeric values, use actual comparison
        try:
            if config1.has(key) and config2.has(key):
                # Try to get as appropriate type
                if isinstance(config1.getOr[int64_t](key, 0), int64_t):
                    val1 = config1.getOr[int64_t](key, 0)
                    val2 = config2.getOr[int64_t](key, 0)
                elif isinstance(config1.getOr[float](key, 0.0), float):
                    val1 = config1.getOr[float](key, 0.0)
                    val2 = config2.getOr[float](key, 0.0)
                elif isinstance(config1.getOr[bool](key, False), bool):
                    val1 = config1.getOr[bool](key, False)
                    val2 = config2.getOr[bool](key, False)
        except:
            pass
        
        if val1 != val2:
            differences[key] = {"config1": val1, "config2": val2}
    
    return differences


def export_config(config: pynlm.Config, filepath: str):
    """Export configuration to JSON file."""
    if not PYNLM_AVAILABLE:
        raise RuntimeError("pynlm not available")
    
    summary = get_config_summary(config)
    
    with open(filepath, 'w') as f:
        json.dump(summary, f, indent=2)
    
    print(f"Configuration exported to {filepath}")


def import_config(filepath: str) -> pynlm.Config:
    """Import configuration from JSON file."""
    if not PYNLM_AVAILABLE:
        raise RuntimeError("pynlm not available")
    
    with open(filepath, 'r') as f:
        data = json.load(f)
    
    config = pynlm.createDefaultConfig()
    
    # Import all settings
    for key, value in data.items():
        if isinstance(value, bool):
            config.set(key, value)
        elif isinstance(value, int):
            config.set(key, int(value))
        elif isinstance(value, float):
            config.set(key, value)
        else:
            config.set(key, str(value))
    
    return config


def print_help():
    """Print help information."""
    help_text = """
NLM Development Tools
=====================

Available functions:

1. create_development_config(neuron_count=1000, region_count=2, preset="balanced")
   Create a development configuration with preset settings.

2. create_agent_simulation(world_type="simple")
   Create a complete agent simulation with brain, agent, and world.

3. run_simulation(brain, agent, world, num_steps=1000, progress_callback=None)
   Run a complete simulation with progress tracking.

4. validate_config(config)
   Validate and fix common configuration issues.

5. get_config_summary(config)
   Get a comprehensive summary of configuration.

6. compare_configs(config1, config2)
   Compare two configurations and show differences.

7. export_config(config, filepath)
   Export configuration to JSON file.

8. import_config(filepath)
   Import configuration from JSON file.

9. preset_configs
   Dictionary of predefined preset configurations.

10. examples
    Dictionary of example code snippets.

11. help()
    Show this help information.
"""
    print(help_text)


def main():
    """Main entry point for command line tools."""
    parser = argparse.ArgumentParser(description="NLM Development Tools")
    parser.add_argument("--tool", choices=["create_config", "create_simulation", "run", "validate", 
                                           "compare", "export", "import", "help"], 
                       help="Tool to run")
    parser.add_argument("--config", help="Configuration file path")
    parser.add_argument("--steps", type=int, default=1000, help="Number of simulation steps")
    parser.add_argument("--preset", default="balanced", 
                       choices=["minimal", "balanced", "high_learning", "phase6_full"],
                       help="Preset configuration to use")
    parser.add_argument("--output", help="Output file path")
    
    args = parser.parse_args()
    
    if not PYNLM_AVAILABLE:
        print("Error: pynlm not available. Please install the module first.")
        sys.exit(1)
    
    if args.tool == "create_config":
        config = create_development_config(preset=args.preset)
        print("Configuration created successfully!")
        summary = get_config_summary(config)
        print(json.dumps(summary, indent=2))
        
        if args.output:
            export_config(config, args.output)
    
    elif args.tool == "create_simulation":
        brain, agent, world = create_agent_simulation()
        print("Agent simulation created successfully!")
        print(f"Brain: {brain.getTotalNeuronCount()} neurons")
        print(f"World: {world.getWidth()}x{world.getHeight()} meters")
    
    elif args.tool == "run":
        brain, agent, world = create_agent_simulation()
        
        def progress_callback(step, progress):
            bar_length = 20
            filled = int(bar_length * progress)
            bar = "=" * filled + ">" + " " * (bar_length - filled)
            print(f"\rProgress: [{bar}] {progress*100:.1f}% Step {step}/{args.steps}", end="")
        
        total_spikes, avg_rate = run_simulation(brain, agent, world, args.steps, progress_callback)
        print(f"\nSimulation complete: {total_spikes} spikes, {avg_rate:.2f} avg rate")
    
    elif args.tool == "validate":
        if not args.config:
            print("Error: --config argument required for validation")
            sys.exit(1)
        
        # For now, just show help
        print_help()
    
    elif args.tool == "compare":
        print("Config comparison tool requires two configurations.")
        print("Use --config for each configuration to compare.")
    
    elif args.tool == "export":
        if not args.config:
            print("Error: --config argument required for export")
            sys.exit(1)
        
        if not args.output:
            print("Error: --output argument required for export")
            sys.exit(1)
        
        # For now, just show help
        print_help()
    
    elif args.tool == "import":
        if not args.config:
            print("Error: --config argument required for import")
            sys.exit(1)
        
        # For now, just show help
        print_help()
    
    elif args.tool == "help":
        print_help()


if __name__ == "__main__":
    main()
