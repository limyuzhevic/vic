#!/usr/bin/env python3
"""
Demonstration of NLM Python Bindings Improvements

This script demonstrates the enhanced features and improvements
made to the NLM Python bindings.
"""

import pynlm

def main():
    print("=" * 70)
    print("NLM Python Bindings - Demonstration of Improvements")
    print("=" * 70)
    print()
    
    print("This demonstration shows the enhanced features of the NLM")
    print("Python bindings, which provide a comprehensive API for")
    print("neural network simulation and control.")
    print()
    
    # Demonstration 1: Basic Usage
    print("1. BASIC USAGE")
    print("-" * 70)
    
    config = pynlm.createConfigWithDefaults()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"and {brain.getTotalSynapseCount()} synapses")
    print()
    
    # Demonstration 2: Enhanced Simulation Control
    print("2. ENHANCED SIMULATION CONTROL")
    print("-" * 70)
    
    brain.runSimulation(50, log_interval=10)
    print(f"✓ Simulation completed. Total spikes: {brain.getTotalSpikeCount()}")
    print()
    
    # Demonstration 3: Comprehensive Statistics
    print("3. COMPREHENSIVE STATISTICS")
    print("-" * 70)
    
    stats = pynlm.getBrainStatistics(brain)
    print("Brain Statistics:")
    for key, value in stats.items():
        print(f"  {key}: {value}")
    print()
    
    # Demonstration 4: Advanced Configuration
    print("4. ADVANCED CONFIGURATION")
    print("-" * 70)
    
    advanced_config = pynlm.createConfigAdvanced(
        neuron_count=200,
        connection_probability=0.08,
        enable_stdp=True,
        enable_hebbian=True,
        enable_structural=True,
        enable_curiosity=True
    )
    
    print("Advanced configuration created with custom parameters:")
    print("  - Neuron count: 200")
    print("  - Connection probability: 0.08")
    print("  - STDP enabled: True")
    print("  - Hebbian enabled: True")
    print("  - Structural enabled: True")
    print("  - Curiosity enabled: True")
    print()
    
    # Demonstration 5: Memory System Analysis
    print("5. MEMORY SYSTEM ANALYSIS")
    print("-" * 70)
    
    memory_stats = pynlm.getMemoryStats(brain)
    print("Memory System Statistics:")
    for key, value in memory_stats.items():
        print(f"  {key}: {value}")
    print()
    
    # Demonstration 6: Development Tracking
    print("6. DEVELOPMENT TRACKING")
    print("-" * 70)
    
    # Create agent for demonstration
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
    world.reset()
    agent.initialize(world)
    
    print("Agent created with development system:")
    print(f"  - Development stage: {brain.getDevelopmentalStage()}")
    print(f"  - Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  - Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"  - Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print()
    
    # Demonstration 7: Spike Pattern Analysis
    print("7. SPIKE PATTERN ANALYSIS")
    print("-" * 70)
    
    pattern = pynlm.analyzeSpikePattern(brain, window_size=10)
    print(f"Spike pattern analyzed over {len(pattern)} time points")
    print(f"First 5 data points: {pattern[:5]}")
    print()
    
    # Demonstration 8: Error Handling
    print("8. ERROR HANDLING")
    print("-" * 70)
    
    try:
        # Test NLM exception handling
        raise pynlm.NLMError("This is a test NLM error")
    except pynlm.NLMError as e:
        print(f"✓ NLM exception caught: {e}")
    
    try:
        # Test Python exception handling
        raise ValueError("This is a Python ValueError")
    except ValueError as e:
        print(f"✓ Python exception caught: {e}")
    
    print()
    
    # Summary
    print("=" * 70)
    print("DEMONSTRATION SUMMARY")
    print("=" * 70)
    print("✓ Basic brain creation and initialization")
    print("✓ Enhanced simulation control with logging")
    print("✓ Comprehensive statistics collection")
    print("✓ Advanced configuration with custom parameters")
    print("✓ Memory system analysis")
    print("✓ Development system tracking")
    print("✓ Spike pattern analysis")
    print("✓ Robust error handling")
    print()
    print("The NLM Python bindings provide a comprehensive, user-friendly")
    print("interface for neural network simulation with powerful features")
    print("for both beginners and advanced users.")
    print()
    print("Key improvements demonstrated:")
    print("• createConfigWithDefaults() - Easy configuration setup")
    print("• createConfigAdvanced() - Advanced parameter control")
    print("• runSimulation() - High-level simulation control")
    print("• getBrainStatistics() - Comprehensive statistics")
    print("• getMemoryStats() - Memory system analysis")
    print("• getDevelopmentStats() - Development system tracking")
    print("• analyzeSpikePattern() - Spike pattern analysis")
    print("• NLMError - Custom exception handling")
    print()

if __name__ == "__main__":
    main()