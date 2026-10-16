#!/usr/bin/env python3
"""
NLM Quick Start Guide
Demonstrates the improved NLM APIs and features

This script shows how to use the enhanced NLM Python bindings,
configuration system, and new command-line tools.
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '../python'))

try:
    from pynlm import (
        createDefaultConfig, createDemoConfig, createResearchConfig,
        createBrain, createSimpleWorld, createAgentBrain,
        create_brain_for_learning, run_demo, benchmark_brain_creation
    )
    PYTHON_BINDINGS_AVAILABLE = True
except ImportError as e:
    PYTHON_BINDINGS_AVAILABLE = False
    print(f"Warning: Python bindings not available: {e}")

def demo_basic_usage():
    """Demonstrate basic NLM usage with improved APIs"""
    print("=" * 60)
    print("NLM Basic Usage Demo")
    print("=" * 60)
    
    if not PYTHON_BINDINGS_AVAILABLE:
        print("Python bindings not available. Skipping demo.")
        return
    
    # Example 1: Using convenience functions
    print("\n1. Creating a brain using convenience function:")
    brain = create_brain_for_learning(neuron_count=100, enable_plasticity=True)
    print(f"   Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"   Plasticity enabled: {brain.getConfig().has('enable_hebbian')}")
    
    # Example 2: Running simulation
    print("\n2. Running simulation steps:")
    brain.runSteps(50, verbose=False)
    print(f"   Completed 50 steps")
    print(f"   Final firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"   Active neurons: {brain.getFiringNeuronCount()}")
    
    # Example 3: Configuration presets
    print("\n3. Configuration presets:")
    demo_config = createDemoConfig()
    print(f"   Demo config: {demo_config.getOr('neuron_count', 'N/A')} neurons")
    
    research_config = createResearchConfig()
    print(f"   Research config: {research_config.getOr('neuron_count', 'N/A')} neurons")
    
    # Example 4: Creating complete agent system
    print("\n4. Creating complete agent system:")
    world = createSimpleWorld()
    world.configure(16, 16, 8, 8)
    world.setAgentStart(8, 8)
    
    agent = createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print(f"   Created agent with sensory input size: {agent.getSensoryInputSize()}")
    print(f"   Motor output size: {agent.getMotorOutputSize()}")
    
    # Example 5: Running training episode
    print("\n5. Running training episode:")
    agent.trainEpisode(world, steps=20, verbose=False)
    print("   Training completed successfully")
    
    print("\n" + "=" * 60)
    print("Basic usage demo completed!")
    print("=" * 60)

def demo_configuration_system():
    """Demonstrate the improved configuration system"""
    print("\n" + "=" * 60)
    print("Configuration System Demo")
    print("=" * 60)
    
    if not PYTHON_BINDINGS_AVAILABLE:
        print("Python bindings not available. Skipping demo.")
        return
    
    # Example 1: Creating default config
    print("\n1. Default configuration:")
    config = createDefaultConfig()
    print(f"   Config keys: {len(config.getKeys())}")
    print(f"   Has neuron_count: {config.has('neuron_count')}")
    
    # Example 2: Setting values
    print("\n2. Setting configuration values:")
    config.set("neuron_count", 250)
    config.set("region_count", 2)
    config.set("connection_probability", 0.2f)
    config.set("simulation_timestep", 0.002)
    print(f"   Set custom values successfully")
    
    # Example 3: Getting values with defaults
    print("\n3. Getting values with defaults:")
    neurons = config.getOr("neuron_count", 100)
    regions = config.getOr("region_count", 1)
    timestep = config.getOr("simulation_timestep", 0.001)
    print(f"   Neurons: {neurons}, Regions: {regions}, Timestep: {timestep}")
    
    # Example 4: Configuration summary
    print("\n4. Configuration summary:")
    summary = config.summary()
    lines = summary.split('\n')
    for line in lines[:10]:  # Show first 10 lines
        if line.strip():
            print(f"   {line}")
    
    print("\n" + "=" * 60)
    print("Configuration system demo completed!")
    print("=" * 60)

def demo_command_line_usage():
    """Show examples of command-line usage"""
    print("\n" + "=" * 60)
    print("Command-Line Usage Examples")
    print("=" * 60)
    
    examples = [
        ("Quick Demo", "nlm_tool --mode demo --preset realtime --steps 100"),
        ("Research Simulation", "nlm_tool --mode research --neurons 5000 --steps 5000 --verbose"),
        ("Real-Time Demo", "nlm_tool --mode realtime --preset realtime"),
        ("Benchmark", "nlm_tool --benchmark 1000"),
        ("Export Config", "nlm_tool --mode export --output settings.json"),
        ("Import Config", "nlm_tool --mode import --config settings.json"),
        ("Custom Preset", "nlm_tool --preset demo --neurons 300 --steps 200"),
        ("Full Training", "nlm_tool --mode training --neurons 1000 --steps 10000 --enable-curiosity --enable-development")
    ]
    
    print("\nCommand-line tool usage examples:")
    for description, command in examples:
        print(f"\n{description}:")
        print(f"  {command}")
    
    print("\n" + "=" * 60)
    print("Command-line usage examples shown!")
    print("=" * 60)

def demo_advanced_features():
    """Demonstrate advanced features and APIs"""
    print("\n" + "=" * 60)
    print("Advanced Features Demo")
    print("=" * 60)
    
    if not PYTHON_BINDINGS_AVAILABLE:
        print("Python bindings not available. Skipping demo.")
        return
    
    # Example 1: High-performance benchmark
    print("\n1. Performance benchmarking:")
    print("   (Run: nlm_tool --benchmark 1000)")
    print("   This measures brain creation time and shows optimization effectiveness")
    
    # Example 2: Competitive training
    print("\n2. Competitive training simulation:")
    print("   (Run: nlm_tool --mode competitive --preset competitive)")
    print("   Creates multiple brains that compete for resources")
    
    # Example 3: Real-time interaction
    print("\n3. Real-time interaction:")
    print("   (Run: nlm_tool --mode realtime --preset realtime)")
    print("   Small, fast network suitable for interactive applications")
    
    # Example 4: Configuration export/import
    print("\n4. Configuration persistence:")
    print("   (Commands shown in command-line section)")
    print("   Save and load complex configurations between sessions")
    
    # Example 5: Research-grade simulation
    print("\n5. Research-grade simulation:")
    print("   (Run: nlm_tool --mode research --preset research)")
    print("   Large network with comprehensive plasticity and neuromodulation")
    
    print("\n" + "=" * 60)
    print("Advanced features demo completed!")
    print("=" * 60)

def main():
    """Main demonstration function"""
    print("\n" + "=" * 60)
    print("NLM Quick Start Guide - Improved Codebase Features")
    print("=" * 60)
    
    print("\nThis script demonstrates the improvements made to the NLM codebase:")
    print("• Modular Python bindings with convenience methods")
    print("• Enhanced configuration system with JSON support")
    print("• Command-line tool for running simulations")
    print("• Advanced features for research and production use")
    print("• Better error handling and user experience")
    
    # Run all demos
    demo_basic_usage()
    demo_configuration_system()
    demo_command_line_usage()
    demo_advanced_features()
    
    print("\n" + "=" * 60)
    print("SUMMARY OF IMPROVEMENTS")
    print("=" * 60)
    
    improvements = [
        "✅ Modular Python bindings (separate modules for types, config, sensory, action, world, agent, brain)",
        "✅ Convenience methods and Pythonic APIs for common operations",
        "✅ Enhanced configuration system with JSON support",
        "✅ Configuration presets (demo, research, realtime, competitive)",
        "✅ Command-line tool (nlm_tool) with multiple modes",
        "✅ Better error handling and user-friendly messages",
        "✅ Advanced features for power users and researchers",
        "✅ Improved documentation and examples",
        "✅ Backward compatibility maintained",
        "✅ Better separation of concerns and code organization"
    ]
    
    for improvement in improvements:
        print(f"  {improvement}")
    
    print(f"\n{'=' * 60}")
    print("Quick start completed! Use 'nlm_tool --help' for command-line options")
    print("Use Python examples in the docs/ directory for programmatic access")
    print("=" * 60)

if __name__ == "__main__":
    main()
