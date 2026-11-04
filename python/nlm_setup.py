#!/usr/bin/env python3
"""
NLM Python Environment Setup Script

This script provides convenient setup and utility functions for working with
NLM (Neural Learning Machine) Python bindings.
"""

import sys
import os
import time
import json
from typing import Dict, List, Optional, Any

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError as e:
    PYNLM_AVAILABLE = False
    print(f"Warning: pynlm not available: {e}")

def check_environment() -> Dict[str, Any]:
    """Check the current NLM environment and return status."""
    status = {
        "pynlm_available": PYNLM_AVAILABLE,
        "python_version": sys.version,
        "version_info": "NLM Python Bindings v2.0 (Improved)",
        "features": []
    }
    
    if PYNLM_AVAILABLE:
        status["features"] = [
            "Basic Brain Operations",
            "Configuration Management", 
            "Agent-World Integration",
            "Sensory Processing",
            "Motor Control",
            "Neuromodulation",
            "Memory Systems",
            "Development Systems",
            "Prediction Systems",
            "Cognition Systems",
            "Text Processing",
            "Simulation Utilities",
            "Debug Tools",
            "Performance Monitoring"
        ]
    
    return status

def print_environment_info():
    """Print comprehensive environment information."""
    status = check_environment()
    
    print("=" * 60)
    print("NLM Python Environment Status")
    print("=" * 60)
    
    if status["pynlm_available"]:
        print(f"✓ pynlm module: Available")
        print(f"  Version: {status['version_info']}")
        print(f"  Python: {status['python_version'].split()[0]}")
        
        print(f"\nAvailable Features ({len(status['features'])}):")
        for i, feature in enumerate(status['features'], 1):
            print(f"  {i:2d}. {feature}")
    
    else:
        print("✗ pynlm module: Not available")
        print("  Please ensure NLM is properly installed:")
        print("  - Build the C++ library")
        print("  - Build Python bindings")
        print("  - Install Python dependencies")

def create_test_brain(config_dict: Optional[Dict] = None) -> 'pynlm.Brain':
    """Create a test brain with optional custom configuration."""
    if not PYNLM_AVAILABLE:
        raise RuntimeError("pynlm is not available")
    
    # Create default brain
    config = pynlm.createDefaultConfig()
    
    # Apply custom configuration if provided
    if config_dict:
        # This would require implementing Config::from_dict in C++
        pass
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    return brain

def run_test_simulation(steps: int = 50) -> Dict[str, Any]:
    """Run a simple test simulation and return results."""
    if not PYNLM_AVAILABLE:
        raise RuntimeError("pynlm is not available")
    
    # Use the built-in simulation utility
    brain, initial_spikes, final_spikes, initial_neurons, final_neurons, actual_steps = \
        pynlm.runBasicSimulation(steps)
    
    stats = pynlm.getBrainStatistics(brain)
    
    return {
        "brain": brain,
        "initial_spikes": initial_spikes,
        "final_spikes": final_spikes,
        "initial_neurons": initial_neurons,
        "final_neurons": final_neurons,
        "steps_requested": steps,
        "steps_executed": actual_steps,
        "statistics": stats,
        "formatted_results": pynlm.formatSimulationResults(stats)
    }

def benchmark_brain_creation(iterations: int = 10) -> Dict[str, Any]:
    """Benchmark brain creation performance."""
    if not PYNLM_AVAILABLE:
        raise RuntimeError("pynlm is not available")
    
    start_time = time.time()
    
    for _ in range(iterations):
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
    
    end_time = time.time()
    total_time = end_time - start_time
    avg_time = total_time / iterations
    
    return {
        "iterations": iterations,
        "total_time": total_time,
        "average_time_per_brain": avg_time,
        "brains_per_second": iterations / total_time
    }

def validate_brain_state(brain: 'pynlm.Brain') -> List[str]:
    """Validate brain state and return any issues found."""
    if not PYNLM_AVAILABLE:
        return ["pynlm module not available"]
    
    issues = []
    
    try:
        if not brain:
            issues.append("Brain object is null")
            return issues
        
        # Check neuron counts
        neuron_count = brain.getTotalNeuronCount()
        if neuron_count == 0:
            issues.append("Brain has no neurons")
        elif neuron_count > 10000:
            issues.append(f"Large neuron count ({neuron_count}) - may impact performance")
        
        # Check synapse count
        synapse_count = brain.getTotalSynapseCount()
        if synapse_count == 0:
            issues.append("Brain has no synapses")
        
        # Check spike count
        spike_count = brain.getTotalSpikeCount()
        if spike_count == 0:
            issues.append("Brain has no spikes - may be inactive")
        
        # Check firing activity
        firing_count = brain.getFiringNeuronCount()
        if firing_count == 0 and neuron_count > 0:
            issues.append("No neurons are firing - may need input")
        
        # Check developmental stage
        stage = brain.getDevelopmentalStage()
        if stage.name == "Initial":
            issues.append("Brain is in Initial developmental stage")
        
    except Exception as e:
        issues.append(f"Error during validation: {str(e)}")
    
    return issues

def create_example_config() -> Dict[str, Any]:
    """Create an example configuration dictionary."""
    return {
        "brain_parameters": {
            "max_neurons": 500,
            "max_synapses": 5000,
            "initial_exc_neurons": 400,
            "initial_inh_neurons": 100,
            "learning_rate": 0.005,
        },
        "plasticity_settings": {
            "stdp_enabled": True,
            "hebbian_enabled": True,
            "structural_plasticity_enabled": True,
            "plasticity_rate": 1.0,
        },
        "development_settings": {
            "development_enabled": True,
            "plasticity_modifier": 1.0,
        },
        "neuromodulation_settings": {
            "reward_modulation_enabled": True,
            "curiosity_enabled": True,
            "novelty_sensitivity": 0.5,
        },
        "simulation_settings": {
            "timestep": 0.1,
            "max_steps": 1000,
        }
    }

def main():
    """Main function for command-line usage."""
    import argparse
    
    parser = argparse.ArgumentParser(description="NLM Python Environment Setup and Utilities")
    parser.add_argument("action", nargs="?", default="info", 
                       help="Action to perform: info, test, benchmark")
    parser.add_argument("--steps", type=int, default=50,
                       help="Number of steps for test simulation")
    parser.add_argument("--iterations", type=int, default=10,
                       help="Number of iterations for benchmark")
    
    args = parser.parse_args()
    
    try:
        if args.action == "info":
            print_environment_info()
            
        elif args.action == "test":
            print(f"\nRunning test simulation ({args.steps} steps)...")
            results = run_test_simulation(args.steps)
            print(f"✓ Simulation completed")
            print(f"  Spikes: {results['initial_spikes']} -> {results['final_spikes']}")
            print(f"  Neurons: {results['initial_neurons']} -> {results['final_neurons']}")
            print(f"\nResults:\n{results['formatted_results']}")
            
        elif args.action == "benchmark":
            print(f"\nBenchmarking brain creation ({args.iterations} iterations)...")
            results = benchmark_brain_creation(args.iterations)
            print(f"✓ Benchmark completed")
            print(f"  Average time per brain: {results['average_time_per_brain']:.4f}s")
            print(f"  Brains per second: {results['brains_per_second']:.2f}")
            
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()