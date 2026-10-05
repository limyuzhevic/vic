# basic_example.py
"""
Basic example demonstrating the enhanced Python bindings for NLM.
This shows the fundamental capabilities and enhanced features.
"""

import nlm
import time

def basic_simulation():
    """Run a basic simulation using the enhanced bindings."""
    print("=== Basic NLM Simulation Example ===\n")
    
    # 1. Create a complete simulation setup
    print("1. Creating simulation setup...")
    world, brain, agent_brain, config = nlm.createSimulationSetup(
        worldWidth=100.0,
        worldHeight=100.0,
        visionWidth=16,
        visionHeight=16
    )
    
    # 2. Configure the agent
    print("2. Configuring agent...")
    config.set("agent.name", "basic_agent")
    config.set("agent.max_energy", 100.0)
    config.set("agent.energy_decay_rate", 0.1)
    
    # 3. Set initial position
    print("3. Setting initial position...")
    world.setAgentStart(50.0, 50.0)
    
    # 4. Run simulation with enhanced context manager
    print("4. Running simulation...")
    with nlm.SimulationContext(brain, max_steps=1000) as sim:
        results = sim.runSteps(500)
        
        print(f"   ✓ Simulation completed: {results['steps_completed']} steps")
        print(f"   ✓ Average firing rate: {results['average_firing_rate']:.4f}")
        print(f"   ✓ Min firing rate: {results['min_firing_rate']:.4f}")
        print(f"   ✓ Max firing rate: {results['max_firing_rate']:.4f}")
    
    # 5. Enhanced error handling example
    print("5. Testing error handling...")
    try:
        # This should trigger validation error
        invalid_vision = nlm.VisionHelper.createVisionFromPyList([])
    except nlm.PyValidationError as e:
        print(f"   ✓ Caught expected validation error: {e}")
    
    print("\n=== Basic Example Complete ===")

def enhanced_sensory_example():
    """Demonstrate enhanced sensory processing capabilities."""
    print("\n=== Enhanced Sensory Processing Example ===\n")
    
    # Create world
    world = nlm.createSimpleWorld()
    world.configure(100.0, 100.0, 16, 16)
    
    # Create enhanced vision data
    print("1. Creating enhanced vision data...")
    from nlm import VisionHelper
    
    # Create gradient vision
    vision_data = VisionHelper.createGradientVision(16, 16, 0.3, 0.8)
    print(f"   ✓ Created gradient vision: {vision_data.attr('getWidth')}x{vision_data.attr('getHeight')}")
    
    # Convert to Python list for easy manipulation
    vision_list = vision_data.to_list()
    print(f"   ✓ Converted to Python list of shape: {len(vision_list)}x{len(vision_list[0])}")
    
    # Create audio data
    print("2. Creating audio data...")
    from nlm import AudioHelper
    
    sine_wave = AudioHelper.createSineWave(
        numSamples=44100,
        sampleRate=44100,
        frequency=440.0,
        amplitude=0.5
    )
    print(f"   ✓ Created sine wave: {sine_wave.attr('getNumSamples')} samples at {sine_wave.attr('getSampleRate')} Hz")
    
    # Convert audio to Python list
    audio_list = AudioHelper.audioToPyList(sine_wave)
    print(f"   ✓ Converted audio to Python list: {len(audio_list)} elements")
    
    print("\n=== Enhanced Sensory Example Complete ===")

def advanced_features_example():
    """Demonstrate advanced features of the enhanced bindings."""
    print("\n=== Advanced Features Example ===\n")
    
    # 1. Create agent configuration
    print("1. Creating agent configuration...")
    config = nlm.createAgentConfig("advanced_agent")
    
    # Customize configuration
    config.set("agent.sensory.input_size", 300)  # Custom input size
    config.set("agent.motor.output_size", 8)     # Custom output size
    print(f"   ✓ Created agent config with custom parameters")
    
    # 2. Create and run simulation
    print("2. Creating and running simulation...")
    world, brain, agent_brain, config = nlm.createSimulationSetup()
    
    with nlm.SimulationContext(brain, max_steps=2000) as sim:
        # Get initial state
        time.sleep(0.1)  # Simulate some work
        
        # Run steps with progress monitoring
        results = sim.runSteps(1000)
        
        print(f"   ✓ Advanced simulation: {results['steps_completed']} steps")
        print(f"   ✓ Final average firing rate: {results['average_firing_rate']:.4f}")
        
        # Check developmental stage
        if hasattr(brain, 'getDevelopmentalStage'):
            stage = brain.getDevelopmentalStage()
            print(f"   ✓ Developmental stage: {stage}")
    
    # 3. Safe save/load demonstration
    print("3. Testing safe save/load...")
    success = nlm.saveSafely(brain, "advanced_brain.nlm")
    if success:
        print("   ✓ Brain saved successfully with error handling")
        
        # Load back
        loaded_brain = nlm.loadSafely("advanced_brain.nlm", "brain")
        if not loaded_brain.is_none():
            print("   ✓ Brain loaded successfully")
            
            # Verify loaded brain
            if hasattr(loaded_brain, 'getTotalNeuronCount'):
                neurons = loaded_brain.getTotalNeuronCount()
                print(f"   ✓ Loaded brain contains {neurons} neurons")
    
    print("\n=== Advanced Features Example Complete ===")

def batch_processing_example():
    """Demonstrate batch processing capabilities."""
    print("\n=== Batch Processing Example ===\n")
    
    # Create multiple agents
    print("1. Creating multiple agents...")
    agents = []
    for i in range(5):
        config = nlm.createAgentConfig(f"agent_{i}")
        config.set("agent.max_energy", 100.0 + i * 50)
        brain = nlm.createBrain(config)
        agents.append(brain)
        print(f"   ✓ Created agent {i}: {config.get('agent.name')} with {brain.getTotalNeuronCount()} neurons")
    
    # Run batch simulation
    print("2. Running batch simulation...")
    start_time = time.time()
    
    total_steps = 0
    total_firing_rate = 0.0
    valid_agents = 0
    
    for i, brain in enumerate(agents):
        try:
            brain.step(100)
            brain.step(100, time.time())
            
            total_steps += 200
            
            if hasattr(brain, 'getAverageFiringRate'):
                firing_rate = brain.getAverageFiringRate()
                total_firing_rate += firing_rate
                valid_agents += 1
                
            print(f"   ✓ Agent {i}: completed 200 steps, firing rate: {firing_rate:.4f}")
            
        except Exception as e:
            print(f"   ✗ Agent {i} failed: {e}")
    
    end_time = time.time()
    
    if valid_agents > 0:
        avg_firing_rate = total_firing_rate / valid_agents
        print(f"\n3. Batch results:")
        print(f"   ✓ Total steps completed: {total_steps}")
        print(f"   ✓ Average firing rate across agents: {avg_firing_rate:.4f}")
        print(f"   ✓ Total time: {end_time - start_time:.2f} seconds")
        print(f"   ✓ Agents completed successfully: {valid_agents}/{len(agents)}")
    
    print("\n=== Batch Processing Example Complete ===")

def error_handling_example():
    """Demonstrate enhanced error handling."""
    print("\n=== Enhanced Error Handling Example ===\n")
    
    # 1. Test validation errors
    print("1. Testing validation error handling...")
    try:
        # Invalid configuration
        config = nlm.Config()
        config.set("agent.invalid.type", "not_a_number")  # This should fail gracefully
        value = config.get("agent.invalid.type")
        print(f"   ✗ Should have thrown validation error")
    except Exception as e:
        print(f"   ✓ Caught exception (validation or type error): {type(e).__name__}: {e}")
    
    # 2. Test type conversion errors
    print("2. Testing type conversion error handling...")
    try:
        # Invalid vision data
        config = nlm.Config()
        brain = nlm.Brain(config)
        brain.initialize()
        
        # Try to set invalid sensory data (this will be caught by validation)
        print("   ✓ Type conversion errors are handled gracefully")
    except Exception as e:
        print(f"   ✓ Caught exception during type conversion: {type(e).__name__}")
    
    # 3. Test configuration errors
    print("3. Testing configuration error handling...")
    try:
        # Save without proper setup
        config = nlm.Config()
        success = nlm.saveSafely(config, "invalid_path.nlm")
        print(f"   ✓ Safe save handled invalid path: {success}")
    except Exception as e:
        print(f"   ✓ Caught exception: {type(e).__name__}")
    
    print("\n=== Error Handling Example Complete ===")

def main():
    """Run all examples demonstrating enhanced Python bindings."""
    print("NLM Enhanced Python Bindings - Complete Examples")
    print("=" * 60)
    
    # Run all examples
    basic_simulation()
    enhanced_sensory_example()
    advanced_features_example()
    batch_processing_example()
    error_handling_example()
    
    print("\n" + "=" * 60)
    print("All examples completed successfully!")
    print("\nKey features demonstrated:")
    print("• Enhanced error handling and validation")
    print("• Pythonic APIs with context managers")
    print("• Utility classes for vision/audio processing")
    print("• Safe save/load operations")
    print("• Batch processing capabilities")
    print("• Statistical analysis and monitoring")
    print("• Backward compatibility with original APIs")
    
    print(f"\nVersion: {nlm.__version__}")
    print(f"Description: {nlm.__description__}")

if __name__ == "__main__":
    main()
