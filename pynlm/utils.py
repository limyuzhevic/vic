"""
NLM (Neural Learning Machine) Python Bindings Utilities

This module provides enhanced utilities for working with NLM Python bindings,
including configuration management, benchmarking tools, and example usage patterns.
"""

import sys
import json
import time
import inspect
from typing import Dict, Any, List, Optional
from pathlib import Path

# Try to import NLM bindings
try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False


class NLMUtils:
    """Utility functions for NLM Python bindings users."""
    
    @staticmethod
    def print_info():
        """Print NLM Python bindings information and system details."""
        print("=== NLM Python Bindings Information ===")
        print(f"Version: 0.1.0")
        print(f"Python Bindings: {'Available' if PYNLM_AVAILABLE else 'Not Available'}")
        
        if PYNLM_AVAILABLE:
            print("\nAvailable Classes:")
            print("  - NeuronId")
            print("  - SynapseId") 
            print("  - RegionId")
            print("  - PopulationId")
            print("  - Config")
            print("  - SensoryInput (base class)")
            print("    - Vision")
            print("    - Audio")
            print("    - InternalSignals")
            print("  - Brain")
            print("  - AgentBrain")
            print("  - SimpleWorld")
            print("  - Action")
            print("  - SensoryPercept")
            print("  - WorldObject")
            print("  - AgentBody")
            print("  - ActionResult")
        
        print("\nGetting Started Examples:")
        print("1. Create a default configuration:")
        print("   >>> config = pynlm.createDefaultConfig()")
        print("2. Create a brain from config:")
        print("   >>> brain = pynlm.createBrain(config)")
        print("3. Create a world:")
        print("   >>> world = pynlm.createSimpleWorld()")
        print("4. Create an agent brain:")
        print("   >>> agent_brain = pynlm.createAgentBrain(brain)")
    
    @staticmethod
    def show_config(config: 'pynlm.Config') -> str:
        """Display configuration in a user-friendly format."""
        if not PYNLM_AVAILABLE:
            return "NLM Python bindings not available"
        
        keys = config.getKeys()
        result = "NLM Configuration:\n"
        
        for key in sorted(keys):
            if config.has(key):
                # Get the value as string
                if key == "neuron_count" or key == "region_count":
                    value = config.get(key)
                    if value:
                        result += f"  {key}: {value} neurons\n"
                elif key == "simulation_timestep":
                    value = config.get(key)
                    if value:
                        result += f"  {key}: {value}s\n"
                elif key == "connection_probability":
                    value = config.get(key)
                    if value:
                        result += f"  {key}: {value:.3f}\n"
                elif key == "stdp_ltp_weight" or key == "stdp_ltd_weight":
                    value = config.get(key)
                    if value:
                        result += f"  {key}: {value:.5f}\n"
                elif key == "environment_width" and "environment_height" in keys:
                    width = config.get("environment_width").value if config.has("environment_width") else 0
                    height = config.get("environment_height").value if config.has("environment_height") else 0
                    result += f"  environment: {width}x{height} grid\n"
                else:
                    result += f"  {key}: {config.get(key).value}\n"
        
        return result
    
    @staticmethod
    def create_example_brain(neuron_count: int = 100) -> 'pynlm.Brain':
        """Create a brain with example configuration for learning."""
        if not PYNLM_AVAILABLE:
            raise ImportError("NLM Python bindings not available")
        
        config = pynlm.createDefaultConfig()
        
        # Configure for example learning scenario
        config.set("neuron_count", neuron_count)
        config.set("region_count", 2)
        config.set("connection_probability", 0.15)
        config.set("random_seed", 42)
        
        # Enable plasticity for learning
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        return brain
    
    @staticmethod
    def create_example_world(width: int = 20, height: int = 20) -> 'pynlm.SimpleWorld':
        """Create a world with example setup."""
        if not PYNLM_AVAILABLE:
            raise ImportError("NLM Python bindings not available")
        
        world = pynlm.createSimpleWorld()
        world.configure(width, height, 5, 5)  # width, height, vision_width, vision_height
        
        # Place an agent in the center
        world.setAgentStart(width // 2, height // 2)
        
        return world
    
    @staticmethod
    def create_example_agent_brain(brain: 'pynlm.Brain', world: 'pynlm.SimpleWorld') -> 'pynlm.AgentBrain':
        """Create an agent brain with example setup."""
        if not PYNLM_AVAILABLE:
            raise ImportError("NLM Python bindings not available")
        
        agent_brain = pynlm.createAgentBrain(brain)
        agent_brain.initialize(world)
        
        return agent_brain


def print_info():
    """Print NLM Python bindings information and system details."""
    NLMUtils.print_info()


def show_config(config):
    """Display configuration in a user-friendly format."""
    return NLMUtils.show_config(config)


def create_example_brain(neuron_count: int = 100):
    """Create a brain with example configuration for learning."""
    return NLMUtils.create_example_brain(neuron_count)


def create_example_world(width: int = 20, height: int = 20):
    """Create a world with example setup."""
    return NLMUtils.create_example_world(width, height)


def create_example_agent_brain(brain, world):
    """Create an agent brain with example setup."""
    return NLMUtils.create_example_agent_brain(brain, world)


class BenchmarkRunner:
    """Run performance benchmarks for NLM components."""
    
    @staticmethod
    def run_benchmarks(brain: 'pynlm.Brain', world: 'pynlm.SimpleWorld', 
                      duration: float = 10.0, verbose: bool = True) -> Dict[str, Any]:
        """Run comprehensive benchmarks on brain and world performance."""
        if not PYNLM_AVAILABLE:
            raise ImportError("NLM Python bindings not available")
        
        start_time = time.time()
        
        # Initialize benchmark results
        results = {
            'total_time': 0,
            'brain_steps': 0,
            'spikes_generated': 0,
            'active_neurons': 0,
            'firing_rate': 0,
            'memory_usage': {},
            'configurations_tested': []
        }
        
        # Run simulation for specified duration
        steps_per_second = 100  # Approximate
        total_steps = int(duration * steps_per_second)
        
        for step in range(total_steps):
            # Simulate world step
            current_time = step / steps_per_second
            world.update(current_time)
            
            # Inject random sensory input occasionally
            if step % 50 == 0:
                world.applyMotorCommand(pynlm.MotorCommand.MoveForward, current_time)
            
            # Step brain
            brain.step(step, current_time)
            
            # Collect statistics
            results['brain_steps'] += 1
            results['spikes_generated'] += brain.getTotalSpikeCount()
            
            if verbose and step % (steps_per_second * 2) == 0:
                print(f"  Step {step}: "
                      f"Total spikes: {brain.getTotalSpikeCount()}, "
                      f"Active neurons: {brain.getActiveNeuronCount()}")
        
        end_time = time.time()
        results['total_time'] = end_time - start_time
        
        # Calculate derived metrics
        if results['total_time'] > 0:
            results['firing_rate'] = results['spikes_generated'] / results['total_time']
        
        # Test different configurations if requested
        if hasattr(brain, 'getConfig'):
            config = brain.getConfig()
            results['configurations_tested'] = config.getKeys()
        
        if verbose:
            print(f"\nBenchmark Results:")
            print(f"  Total simulation time: {results['total_time']:.2f}s")
            print(f"  Steps completed: {results['brain_steps']}")
            print(f"  Total spikes generated: {results['spikes_generated']}")
            print(f"  Average firing rate: {results['firing_rate']:.2f} spikes/second")
        
        return results


def run_benchmarks(brain, world, duration: float = 10.0, verbose: bool = True):
    """Run comprehensive benchmarks on brain and world performance."""
    return BenchmarkRunner.run_benchmarks(brain, world, duration, verbose)
