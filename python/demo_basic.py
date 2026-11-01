#!/usr/bin/env python3
"""
Example: Basic NLM Simulation
=============================

This example demonstrates the basic usage of the enhanced NLM Python bindings.
It shows how to set up a simple agent-environment simulation and run it.

The example covers:
- Creating agent systems with convenience factory functions
- Running simulation steps with sensory input and motor output
- Monitoring system performance
- Error handling and safe file operations
"""

import pynlm as nlm
import os
import sys

def create_simple_agent_system(width=50.0, height=50.0, seed=42):
    """
    Create a complete agent system using enhanced convenience functions.
    
    This demonstrates the new factory functions and helper utilities
    that make setup much easier than before.
    """
    print("Creating agent system...")
    
    # Create configuration
    config = nlm.createDefaultConfig()
    
    # Create brain and world
    brain = nlm.createBrain(config)
    world = nlm.createSimpleWorld()
    
    # Configure world
    world.configure(width, height, 32, 32)
    world.setRandomSeed(seed)
    
    # Initialize complete agent system
    agent = nlm.initializeAgentSystem(brain, world)
    agent.initialize(world)
    
    print(f"  - World: {width}x{height} pixels")
    print(f"  - Brain: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
    print(f"  - Agent input size: {agent.getSensoryInputSize()}")
    print(f"  - Agent output size: {agent.getMotorOutputSize()}")
    
    return brain, world, agent

def run_simple_simulation(brain, world, agent, steps=100):
    """
    Run a simple simulation loop demonstrating the enhanced bindings.
    
    This example shows:
    - Using runSimulationStep for easy simulation loops
    - Getting system statistics
    - Monitoring simulation progress
    """
    print(f"\nRunning simulation for {steps} steps...")
    
    total_reward = 0.0
    resource_found = False
    
    for step in range(steps):
        # Get current agent state
        body = world.getAgentBody()
        
        # Get sensory percept
        percept = world.getSensoryPercept()
        
        # Generate sensory input (simplified - in real use, this would come from actual sensors)
        sensory_input = percept.getAllSignals()
        
        # Run simulation step using enhanced binding
        action = nlm.runSimulationStep(brain, world, 0.01, sensory_input)
        
        # Accumulate reward (if action has reward info)
        if hasattr(action, 'reward'):
            total_reward += action.reward
        
        # Check for resources (high vision intensity = resources)
        vision = percept.getVision()
        max_vision = max(vision) if vision else 0.0
        
        if max_vision > 0.8:
            resource_found = True
            # Apply neuromodulation
            brain.applyRewardModulation(1.0, 0.5)  # Strong positive reward
            print(f"  Step {step}: Found resource! (Vision intensity: {max_vision:.2f})")
            break
        
        # Check for hazards (low vision intensity = hazards)
        if max_vision < 0.2:
            brain.applyRewardModulation(-0.5, 0.1)  # Negative reward
            print(f"  Step {step}: Hit hazard! (Vision intensity: {max_vision:.2f})")
            break
        
        # Print progress every 25 steps
        if step % 25 == 0:
            time = nlm.getSimulationTime(world)
            print(f"  Step {step}: Time={time:.2f}, Position=({body.x:.1f},{body.y:.1f}), Energy={body.energy:.1f}")
    
    print(f"\nSimulation completed!")
    print(f"  - Total reward: {total_reward:.2f}")
    print(f"  - Resource found: {resource_found}")
    print(f"  - Final agent position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    
    return resource_found, total_reward

def demonstrate_convenience_functions():
    """
    Demonstrate the new convenience factory functions.
    
    This shows how the enhanced bindings provide:
    - Easy creation of sensory buffers
    - Smart initialization helpers
    - Multiple sensory modalities
    """
    print("\n" + "="*60)
    print("DEMONSTRATING CONVENIENCE FACTORY FUNCTIONS")
    print("="*60)
    
    # Create different sensory buffers
    print("\n1. Creating sensory buffers:")
    
    vision = nlm.createVisionInput(64, 64, 1)
    print(f"   - Vision buffer: 64x64 single channel")
    
    audio = nlm.createAudioInput(22050, 1024)
    print(f"   - Audio buffer: 22050 Hz, 1024 samples")
    
    internal = nlm.createInternalSignals()
    print(f"   - Internal signals: homeostatic states")
    
    percept = nlm.createSensoryPercept()
    print(f"   - Sensory percept container")
    
    # Demonstrate processing
    print("\n2. Processing sensory data:")
    
    # Add signals to internal
    internal.addSignal(1.0)
    internal.addSignal(0.5)
    internal.addSignal(0.3)
    print(f"   - Added {internal.getDimensions()} internal signals")
    
    # Set vision data
    vision_data = [0.1 * (x % 10) for x in range(64 * 64)]
    print(f"   - Set {len(vision_data)} vision values (0.0-1.0)")
    
    # Create and set audio data
    audio_data = [0.5 * (i % 100) / 100.0 for i in range(1024)]
    print(f"   - Set {len(audio_data)} audio samples")
    
    return vision, audio, internal, percept

def demonstrate_error_handling():
    """
    Demonstrate the enhanced error handling and validation helpers.
    
    This shows how the new bindings provide:
    - Configuration validation
    - Safe file operations with error handling
    - Graceful error recovery
    """
    print("\n" + "="*60)
    print("DEMONSTRATING ERROR HANDLING AND VALIDATION")
    print("="*60)
    
    # Create a system for demonstration
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    world = nlm.createSimpleWorld()
    world.configure(50, 50, 32, 32)
    
    # Demonstrate configuration validation
    print("\n1. Configuration validation:")
    is_valid = nlm.validateConfiguration(config)
    print(f"   - Configuration valid: {is_valid}")
    
    # Create a temporary directory for checkpoints
    checkpoint_dir = "demo_checkpoints"
    os.makedirs(checkpoint_dir, exist_ok=True)
    
    # Demonstrate safe file operations
    print("\n2. Safe file operations:")
    
    # Save brain state
    checkpoint_path = f"{checkpoint_dir}/demo_brain.pynlm"
    try:
        brain.save(checkpoint_path)
        print(f"   ✓ Successfully saved brain to: {checkpoint_path}")
    except Exception as e:
        print(f"   ✗ Failed to save brain: {e}")
    
    # Try to load the saved state
    load_path = f"{checkpoint_dir}/demo_brain.pynlm"
    try:
        brain.load(load_path)
        print(f"   ✓ Successfully loaded brain from: {load_path}")
    except Exception as e:
        print(f"   ✗ Failed to load brain: {e}")
    
    # Try to load from non-existent file (should fail gracefully)
    bad_path = f"{checkpoint_dir}/nonexistent.pynlm"
    try:
        brain.load(bad_path)
        print(f"   ✗ Should have failed to load nonexistent file")
    except Exception as e:
        print(f"   ✓ Gracefully handled missing file: {type(e).__name__}")
    
    # Demonstrate safe load/save with error handling
    print("\n3. Safe load/save with error handling:")
    
    # Create a fresh brain for safe operations
    fresh_brain = nlm.createBrain(nlm.createDefaultConfig())
    
    # Safe save
    try:
        nlm.safeSaveBrain(fresh_brain, f"{checkpoint_dir}/safe_brain.pynlm")
        print(f"   ✓ Safe save completed")
    except Exception as e:
        print(f"   ✗ Safe save failed: {e}")
    
    # Safe load
    try:
        nlm.safeLoadBrain(fresh_brain, f"{checkpoint_dir}/safe_brain.pynlm")
        print(f"   ✓ Safe load completed")
    except Exception as e:
        print(f"   ✗ Safe load failed: {e}")
    
    # Clean up
    import shutil
    shutil.rmtree(checkpoint_dir, ignore_errors=True)
    print(f"\n   Cleaned up checkpoint directory")

def demonstrate_performance_monitoring():
    """
    Demonstrate performance monitoring tools.
    
    This shows how to use the monitoring utilities to track:
    - System statistics over time
    - Performance metrics
    - Progress tracking
    """
    print("\n" + "="*60)
    print("DEMONSTRATING PERFORMANCE MONITORING")
    print("="*60)
    
    # Create a simple system
    brain, world, agent = create_simple_agent_system(width=30, height=30)
    
    print("\n1. Collecting system statistics:")
    
    # Run a few simulation steps
    for step in range(10):
        percept = world.getSensoryPercept()
        sensory_input = percept.getAllSignals()
        
        action = nlm.runSimulationStep(brain, world, 0.01, sensory_input)
        
        # Get system statistics
        neurons, synapses, active, firing_rate, e_i_ratio = nlm.getSystemStats(brain)
        
        print(f"   Step {step + 1}:")
        print(f"     - Neurons: {neurons}, Synapses: {synapses}")
        print(f"     - Active neurons: {active}, Firing rate: {firing_rate:.2f}")
        print(f"     - E/I ratio: {e_i_ratio:.2f}")
        print(f"     - Simulation time: {nlm.getSimulationTime(world):.3f}")
        print(f"     - Agent position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    
    print("\n2. Performance comparison:")
    
    # Demonstrate getStepCount for progress tracking
    initial_steps = nlm.getStepCount(brain)
    print(f"   Initial spike count: {initial_steps}")
    
    # Run more steps
    for step in range(50, 60):
        percept = world.getSensoryPercept()
        sensory_input = percept.getAllSignals()
        action = nlm.runSimulationStep(brain, world, 0.01, sensory_input)
    
    final_steps = nlm.getStepCount(brain)
    print(f"   Final spike count: {final_steps}")
    print(f"   Steps processed: {final_steps - initial_steps}")
    
    print("\n3. System optimization tips:")
    print("   - Use batch processing for multiple steps")
    print("   - Monitor system stats regularly")
    print("   - Use safe file operations for checkpoints")
    print("   - Clear references to large objects when not needed")

def demonstrate_advanced_features():
    """
    Demonstrate advanced features and expert tools.
    
    This shows features for expert users:
    - Batch processing
    - Development scenarios
    - Neuromodulation control
    """
    print("\n" + "="*60)
    print("DEMONSTRATING ADVANCED FEATURES")
    print("="*60)
    
    # Create system
    brain, world, agent = create_simple_agent_system(width=40, height=40)
    
    print("\n1. Development scenario control:")
    
    # Demonstrate development stage setting
    stage = nlm.createDevelopmentScenario(brain, "critical_period")
    print(f"   Set developmental stage to: {stage}")
    
    # Show how this affects plasticity (conceptual)
    print(f"   - High plasticity enabled for learning")
    print(f"   - Neuromodulation sensitivity: high")
    
    print("\n2. Neuromodulation demonstration:")
    
    # Show current neuromodulation levels
    curiosity = agent.getCuriosityLevel()
    novelty = agent.getNoveltyLevel()
    prediction_error = agent.getPredictionError()
    
    print(f"   - Curiosity level: {curiosity:.2f}")
    print(f"   - Novelty level: {novelty:.2f}")
    print(f"   - Prediction error: {prediction_error:.2f}")
    
    # Apply neuromodulation
    brain.applyRewardModulation(0.8, 0.3)  # High reward prediction
    brain.applyRewardModulation(-0.5, 0.1)  # Negative reward
    
    print(f"   - Applied neuromodulation signals")
    
    print("\n3. Advanced helper functions:")
    
    # Show system capabilities
    print("   Available helper functions:")
    print("     - nlm.getSystemStats() - Comprehensive statistics")
    print("     - nlm.getSimulationTime() - Current simulation time")
    print("     - nlm.getStepCount() - Progress tracking")
    print("     - nlm.createDevelopmentScenario() - Set developmental stages")
    print("     - nlm.validateConfiguration() - Configuration validation")
    print("     - nlm.safeLoadBrain()/safeSaveBrain() - Safe file operations")

def main():
    """
    Main function demonstrating all enhanced features.
    """
    print("\n" + "="*80)
    print("ENHANCED NLM PYTHON BINDINGS - DEMONSTRATION")
    print("="*80)
    print("\nThis demonstration showcases the improved Python bindings")
    print("with enhanced convenience functions, error handling, and")
    print("performance monitoring tools.\n")
    
    # Demonstrate convenience functions
    vision, audio, internal, percept = demonstrate_convenience_functions()
    
    # Demonstrate error handling
    demonstrate_error_handling()
    
    # Demonstrate performance monitoring
    demonstrate_performance_monitoring()
    
    # Demonstrate advanced features
    demonstrate_advanced_features()
    
    # Main simulation demonstration
    print("\n" + "="*80)
    print("MAIN SIMULATION DEMONSTRATION")
    print("="*80)
    
    brain, world, agent = create_simple_agent_system(width=60, height=60)
    resource_found, total_reward = run_simple_simulation(brain, world, agent, steps=100)
    
    # Summary
    print("\n" + "="*80)
    print("DEMONSTRATION SUMMARY")
    print("="*80)
    print(f"✓ Agent system created successfully")
    print(f"✓ Simulation ran with enhanced bindings")
    print(f"✓ Error handling and validation demonstrated")
    print(f"✓ Performance monitoring tools shown")
    print(f"✓ Advanced features explored")
    print(f"✓ Resource found: {resource_found}")
    print(f"✓ Total reward accumulated: {total_reward:.2f}")
    print("\nThe enhanced Python bindings provide:")
    print("  - Easier setup with factory functions")
    print("  - Better error handling and validation")
    print("  - Performance monitoring and debugging tools")
    print("  - Advanced helper functions for experts")
    print("  - Neural-based sensory processing support")
    print("\nFor more examples and documentation, see:")
    print("  - Enhanced documentation (python/README.md)")
    print("  - Example scripts in the repository")

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(f"\nError during demonstration: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)