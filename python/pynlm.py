# pynlm - Python Bindings for NLM
"""
Python bindings for the Neural Learning Machine (NLM) project.

This module provides a Python interface to the NLM C++ neural simulation framework,
allowing users to interact with the artificial brain system through Python.
"""

import sys
from pathlib import Path

# Try to import the C++ extension
try:
    from pynlm._pynlm import (
        Brain,
        Neuron,
        Synapse,
        NeuralRegion,
        RegionId,
        NeuronId,
        NeuronType,
        SimulationStep,
        MembranePotential,
        Delay,
        Timestamp,
        PlasticityFlags,
        MotorCommand,
        DevelopmentalStage,
        SensoryPercept,
        Action,
        SimpleWorld,
        Config,
        Logger,
        SimulationClock,
        RandomGenerator,
    )
    print("✓ NLM C++ extension loaded successfully")
except ImportError as e:
    print(f"✗ Failed to import NLM C++ extension: {e}")
    print("You may need to build the extension first using:")
    print("  python setup.py build_ext --inplace")
    print("or using:")
    print("  pip install -e .")
    sys.exit(1)

__version__ = "0.1.0"
__author__ = "NLM Authors"

# Re-export the main classes and functions for convenience
__all__ = [
    # Core types
    "Brain",
    "Neuron", 
    "Synapse",
    "NeuralRegion",
    "RegionId",
    "NeuronId",
    "NeuronType",
    "SimulationStep",
    "MembranePotential",
    "Delay",
    "Timestamp",
    "PlasticityFlags",
    "MotorCommand",
    "DevelopmentalStage",
    "SensoryPercept",
    "Action",
    "SimpleWorld",
    "Config",
    "Logger",
    "SimulationClock",
    "RandomGenerator",
    
    # Agent brain
    "AgentBrain",
    
    # Memory systems
    "WorkingMemory",
    "EpisodicMemory", 
    "AssociativeMemory",
    
    # Neuromodulation
    "Dopamine",
    "Curiosity",
    "Novelty",
    "PredictionError",
    
    # Prediction and cognition
    "PredictionSystem",
    "NeuralPlanner",
    "ConceptFormation",
    "AttentionalSelection",
    
    # Development
    "DevelopmentSystem",
    
    # Environment
    "Environment",
    "SensoryInput",
    "Vision",
    "Audio",
    "InternalSignals",
    
    # Motor system
    "MotorSystem",
    
    # Experiments
    "Experiment",
    "ExperimentRunner",
    "Phase6IntegratedExperiment",
    
    # Testing and utilities
    "testConnectivity",
    "testPlasticity",
    "testSTDP",
]

# Simple convenience functions for quick setup

def create_brain(neuron_count=500, timestep=0.001, random_seed=42):
    """
    Create a new NLM brain with the specified parameters.
    
    Args:
        neuron_count: Number of neurons in the brain
        timestep: Simulation time step in seconds
        random_seed: Random seed for reproducible results
        
    Returns:
        Brain: An initialized Brain object
    """
    config = Config()
    config.set("neuron_count", neuron_count)
    config.set("simulation_timestep", timestep)
    config.set("random_seed", random_seed)
    
    brain = Brain(config)
    if brain.initialize():
        print(f"✓ Brain created with {neuron_count} neurons")
        return brain
    else:
        print("✗ Failed to initialize brain")
        return None

def run_phase6_demo(brain=None, max_steps=2000):
    """
    Run the Phase 6 integration demo.
    
    Args:
        brain: Optional existing brain to use
        max_steps: Maximum number of simulation steps
        
    Returns:
        dict: Experiment results including performance metrics
    """
    if brain is None:
        brain = create_brain()
        if brain is None:
            return None
    
    from experiments import Phase6IntegratedExperiment
    
    experiment = Phase6IntegratedExperiment(brain)
    config = Phase6Config()
    config.maxSteps = max_steps
    config.enableCheckpointing = True
    config.enableReplay = True
    config.enableDevelopment = True
    
    result = experiment.run(config)
    
    print("\n=== Phase 6 Demo Results ===")
    print(f"Total Reward: {result.totalReward}")
    print(f"Average Firing Rate: {result.avgFiringRate}")
    print(f"Memory Episodes Stored: {result.memoryEpisodesStored}")
    print(f"Dopamine Level: {result.dopamineLevel}")
    
    return result

def test_learning(brain=None, steps=1000):
    """
    Run a learning experiment to test synaptic plasticity.
    
    Args:
        brain: Optional existing brain to use
        steps: Number of simulation steps
        
    Returns:
        dict: Learning statistics
    """
    if brain is None:
        brain = create_brain()
        if brain is None:
            return None
    
    # Run basic test sequence
    brain.reset()
    brain.initialize()
    
    # Inject some input patterns
    for step in range(steps):
        # Create random input patterns
        for region in brain.getRegions():
            for neuron in region.getAllNeurons():
                if neuron.getNeuronType() == NeuronType.Sensory:
                    neuron.injectCurrent(30.0f)
        
        brain.step(step, step * 0.001)
    
    # Collect statistics
    stats = {
        "total_spikes": brain.getTotalSpikeCount(),
        "total_neurons": brain.getTotalNeuronCount(),
        "total_synapses": brain.getTotalSynapseCount(),
        "active_neurons": brain.getActiveNeuronCount(),
        "firing_neurons": brain.getFiringNeuronCount(),
        "avg_firing_rate": brain.getAverageFiringRate(),
        "excitation_inhibition_ratio": brain.getExcitationInhibitionRatio(),
    }
    
    print(f"\n=== Learning Test Results ===")
    print(f"Total Spikes: {stats['total_spikes']}")
    print(f"Total Neurons: {stats['total_neurons']}")
    print(f"Total Synapses: {stats['total_synapses']}")
    print(f"Active Neurons: {stats['active_neurons']}")
    print(f"Firing Neurons: {stats['firing_neurons']}")
    print(f"Average Firing Rate: {stats['avg_firing_rate']}")
    print(f"E/I Ratio: {stats['excitation_inhibition_ratio']}")
    
    return stats

if __name__ == "__main__":
    # Simple CLI for quick testing
    import argparse
    
    parser = argparse.ArgumentParser(description="NLM Python Interface CLI")
    parser.add_argument("command", choices=["demo", "test-learning", "help"], default="help")
    parser.add_argument("--steps", type=int, default=2000, help="Number of simulation steps")
    parser.add_argument("--neurons", type=int, default=500, help="Number of neurons")
    
    args = parser.parse_args()
    
    if args.command == "demo":
        run_phase6_demo()
    elif args.command == "test-learning":
        test_learning()
    elif args.command == "help":
        print("NLM Python Interface")
        print("Commands:")
        print("  python -m pynlm demo      # Run Phase 6 integration demo")
        print("  python -m pynlm test-learning  # Test learning and plasticity")
        print("  python -m pynlm help      # Show this help")