#!/usr/bin/env python3
"""
NLM Python Bindings Demonstration Script

This script demonstrates the basic functionality of NLM Python bindings,
showing both standard and enhanced features.

Usage:
    python demo_nlm_bindings.py [--enhanced] [--verbose]

This script requires:
    - pynlm or pynlm_enhanced Python bindings
    - NLM C++ library built and installed

The script demonstrates:
1. Basic configuration creation
2. Brain initialization and operations
3. Agent and world creation
4. Simulation execution
5. Error handling
6. Enhanced features (if using enhanced bindings)
"""

import sys
import os
import json
import argparse
import time

# Add current directory to Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Try to import bindings
try:
    import pynlm
    PYNL_available = True
except ImportError:
    PYNL_available = False
    print("Warning: pynlm bindings not available")

try:
    import pynlm_enhanced as pynlm_enhanced
    PYNL_ENHANCED_available = True
except ImportError:
    PYNL_ENHANCED_available = False
    print("Warning: pynlm_enhanced bindings not available")

def demo_basic_usage():
    """Demonstrate basic NLM Python bindings usage"""
    print("=" * 80)
    print("NLM Python Bindings Basic Usage Demonstration")
    print("=" * 80)
    
    # Determine which bindings to use
    use_enhanced = False
    if not PYNL_available and PYNL_ENHANCED_available:
        use_enhanced = True
        print("Using enhanced bindings (pynlm_enhanced)")
    elif PYNL_available:
        print("Using standard bindings (pynlm)")
    else:
        print("ERROR: No NLM Python bindings available")
        print("Please install NLM Python bindings:")
        print("  pip install pynlm")
        print("  or pip install pynlm_enhanced")
        return
    
    # Import the chosen bindings
    if use_enhanced:
        bindings = pynlm_enhanced
    else:
        bindings = pynlm
    
    print(f"Bindings module: {bindings.__name__}")
    print(f"Bindings version: {getattr(bindings, '__version__', 'unknown')}")
    
    # Example 1: Basic configuration
    print("\n1. Basic Configuration Example")
    print("-" * 50)
    
    config = bindings.createDefaultConfig()
    config.set("neuron_count", 500)
    config.set("region_count", 1)
    config.set("connection_probability", 0.1)
    
    print(f"Created configuration with {config.getKeys().count('neuron_count')} neurons")
    print(f"Configuration summary: {config.summary()}")
    
    # Example 2: Brain creation and initialization
    print("\n2. Brain Creation and Initialization")
    print("-" * 50)
    
    brain = bindings.createBrain(config)
    brain.initialize()
    
    print(f"Brain initialized successfully")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Active neurons: {brain.getActiveNeuronCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.3f} Hz")
    
    # Example 3: Simulation steps
    print("\n3. Simulation Steps")
    print("-" * 50)
    
    print("Running simulation steps...")
    start_time = time.time()
    
    for step in range(10):
        brain.step(step)
        if step % 5 == 0:
            print(f"  Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    end_time = time.time()
    print(f"Simulation completed in {end_time - start_time:.3f} seconds")
    
    # Example 4: Agent creation and initialization
    print("\n4. Agent Creation and Initialization")
    print("-" * 50)
    
    agent = bindings.createAgentBrain(brain)
    
    print(f"Agent created successfully")
    print(f"Agent brain pointer: {agent.getBrain()}")
    
    # Example 5: World creation and configuration
    print("\n5. World Creation and Configuration")
    print("-" * 50)
    
    world = bindings.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.reset()
    
    print(f"World configured: {world.getWidth()}x{world.getHeight()} pixels")
    print(f"Vision: {world.getSensoryPercept().getVisionWidth()}x{world.getSensoryPercept().getVisionHeight()}")
    print(f"Agent start position: ({world.getAgentBody().x}, {world.getAgentBody().y})")
    print(f"Agent energy: {world.getAgentBody().energy}/{world.getMaxEnergy()}")
    
    # Example 6: Enhanced features (if available)
    print("\n6. Enhanced Features (if available)")
    print("-" * 50)
    
    if hasattr(bindings, 'createDevelopmentConfig'):
        print("Enhanced configuration helpers available")
        enhanced_config = bindings.createDevelopmentConfig()
        print(f"Enhanced config created with {enhanced_config.getOr[int64_t]('neuron_count', 0)} neurons")
    else:
        print("Enhanced configuration helpers not available")
    
    if hasattr(bindings, 'getBrainStatistics'):
        print("Enhanced brain statistics available")
        brain_stats = bindings.getBrainStatistics(brain, ["basic", "connectivity"])
        print("Brain statistics:")
        print(brain_stats)
    else:
        print("Enhanced brain statistics not available")
    
    if hasattr(bindings, 'runValidatedSimulation'):
        print("Enhanced simulation runner available")
        
        # Initialize agent with world
        agent.initialize(world)
        agent.enableRewardModulation(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Run enhanced simulation
        result = bindings.runValidatedSimulation(agent, world, 50, verbose=False)
        print("Enhanced simulation results:")
        print(result)
    else:
        print("Enhanced simulation runner not available")
    
    # Example 7: Error handling demonstration
    print("\n7. Error Handling Demonstration")
    print("-" * 50)
    
    try:
        # Try to create configuration with validation
        if hasattr(bindings, 'validateRequiredConfigKeys'):
            required_keys = ["neuron_count", "region_count"]
            if bindings.validateRequiredConfigKeys(config, required_keys):
                print("✓ Configuration validation successful")
            else:
                print("✗ Configuration validation failed")
        else:
            print("Configuration validation not available")
            
        # Try to validate brain state
        if hasattr(bindings, 'validateBrainState'):
            bindings.validateBrainState(brain)
            print("✓ Brain state validation successful")
        else:
            print("Brain state validation not available")
            
    except Exception as e:
        print(f"Error during validation: {e}")
    
    # Example 8: Performance statistics
    print("\n8. Performance Statistics")
    print("-" * 50)
    
    if hasattr(bindings, 'getPerformanceStats'):
        performance_stats = bindings.getPerformanceStats(brain)
        print("Performance statistics:")
        print(performance_stats)
    else:
        print("Performance statistics not available")
    
    print("\n" + "=" * 80)
    print("DEMONSTRATION COMPLETE")
    print("=" * 80)
    print("This demonstration showed the basic functionality of NLM Python bindings.")
    print("For advanced features, refer to the enhanced bindings documentation.")
    print("For comprehensive examples, run the advanced examples script.")
    print("For testing, run the test suite with: python test_nlm_bindings.py")
    print("=" * 80)

if __name__ == "__main__":
    # Parse command line arguments
    parser = argparse.ArgumentParser(
        description="NLM Python Bindings Demonstration",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Usage:
  python demo_nlm_bindings.py [--enhanced] [--verbose]

Examples:
  python demo_nlm_bindings.py
  python demo_nlm_bindings.py --enhanced
  python demo_nlm_bindings.py --verbose
        """
    )
    
    parser.add_argument(
        '--enhanced', action='store_true',
        help='Use enhanced Python bindings (pynlm_enhanced)'
    )
    parser.add_argument(
        '--verbose', action='store_true',
        help='Enable verbose output'
    )
    
    args = parser.parse_args()
    
    # Override enhanced flag if needed
    if not PYNL_AVAILABLE and PYNL_ENHANCED_available:
        args.enhanced = True
    
    # Run demonstration
    demo_basic_usage()
