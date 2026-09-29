#!/usr/bin/env python3
"""
Advanced NLM Python Examples

This file demonstrates advanced usage patterns and new features
of the NLM Python bindings for sophisticated users.
"""

import pynlm
import numpy as np

def example_advanced_factory_functions():
    """Demonstrate advanced factory functions added in v0.1.0+"""
    print("=== Advanced Factory Functions ===")
    
    # Create brain from file with error checking
    try:
        brain = pynlm.createBrainFromFile("configs/test_config.json")
        brain.initialize()
        print(f"Created brain from file with {brain.getTotalNeuronCount()} neurons")
    except Exception as e:
        print(f"File loading failed (expected if file doesn't exist): {e}")
    
    # Create world with initial objects
    world_obj1 = pynlm.WorldObject(x=1.0, y=1.0, pynlm.WorldObjectType.Resource, value=1.0)
    world_obj2 = pynlm.WorldObject(x=5.0, y=5.0, pynlm.WorldObjectType.Hazard, radius=0.5)
    world_with_objects = pynlm.createSimpleWorldFromObjects([world_obj1, world_obj2])
    print(f"Created world with {len([world_obj1, world_obj2])} initial objects")
    
    # Create vision from data array
    vision_data = np.random.randn(64, 64, 3).astype(np.float32)
    vision = pynlm.createVisionFromData(vision_data.flatten(), 64, 64, 3)
    print(f"Created vision with dimensions: {vision.getWidth()}x{vision.getHeight()}x{vision.getChannels()}")
    
    # Create audio from data
    audio_data = np.random.randn(16000).astype(np.float32)
    audio = pynlm.createAudioFromData(audio_data, sampleRate=16000)
    print(f"Created audio with {audio.getNumSamples()} samples at {audio.getSampleRate()} Hz")
    
    # Create internal signals from data
    internal_data = np.array([0.1, 0.2, 0.5, 0.8, 1.0], dtype=np.float32)
    internal_signals = pynlm.createInternalSignalsFromData(internal_data)
    print(f"Created internal signals with {internal_signals.getAllSignals().size()} signals")

def example_batch_processing():
    """Demonstrate batch processing capabilities"""
    print("\n=== Batch Processing Example ===")
    
    # Create a simple brain and agent
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Create multiple sensory inputs for batch processing
    vision_inputs = []
    for i in range(5):
        vision_data = np.random.randn(8, 8, 3).astype(np.float32).flatten()
        vision = pynlm.createVisionFromData(vision_data, 8, 8, 3)
        vision_inputs.append(vision)
    
    # Process all inputs in batch
    success = pynlm.batchProcessSensoryInput(agent, vision_inputs)
    print(f"Batch processing successful: {success}")
    print(f"Processed {len(vision_inputs)} vision inputs in one call")
    
    # Run a simulation using the agent
    for step in range(10):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    print("Completed batch processing simulation with multiple steps")

def example_configuration_management():
    """Demonstrate advanced configuration management"""
    print("\n=== Configuration Management ===")
    
    # Create brain with default configuration
    brain1 = pynlm.createBrainWithDefaults()
    brain1.initialize()
    print(f"Default brain created with {brain1.getTotalNeuronCount()} neurons")
    
    # Get configuration and modify it
    config = brain1.getConfig()
    config.set("brain.neuron_count", 2000)
    config.set("plasticity.stdp.learning_rate", 0.002)
    
    print(f"Modified neuron count: {config.get('brain.neuron_count')}")
    
    # Export configuration to JSON
    try:
        pynlm.exportConfigToJson(config, "my_config.json")
        print("Configuration exported to 'my_config.json'")
    except Exception as e:
        print(f"Export failed (file system issue): {e}")
    
    # Import configuration from JSON
    try:
        imported_config = pynlm.importConfigFromJson("my_config.json")
        print(f"Imported configuration with {imported_config.get('brain.neuron_count')} neurons")
    except Exception as e:
        print(f"Import failed (file may not exist): {e}")
    
    # Create brain from imported config
    if 'imported_config' in locals():
        brain2 = pynlm.createBrain(imported_config)
        brain2.initialize()
        print(f"Created brain from imported config: {brain2.getTotalNeuronCount()} neurons")

def example_error_handling():
    """Demonstrate improved error handling"""
    print("\n=== Error Handling ===")
    
    # Test null reference handling
    try:
        # This should raise an exception due to null pointer handling
        brain = pynlm.createBrain(None)
        brain.initialize()
    except Exception as e:
        print(f"Caught expected exception: {type(e).__name__}: {e}")
    
    # Test file loading error handling
    try:
        brain = pynlm.createBrainFromFileSafe("nonexistent_file.json")
        brain.initialize()
    except Exception as e:
        print(f"Caught file loading exception: {type(e).__name__}: {e}")

def example_advanced_agent_simulation():
    """Demonstrate advanced agent simulation with all features"""
    print("\n=== Advanced Agent Simulation ===")
    
    def run_advanced_simulation(config_dict=None, enable_features=True):
        """Advanced agent simulation with all learning features"""
        
        # 1. Create configuration
        config = pynlm.createDefaultConfig()
        if config_dict:
            for key, value in config_dict.items():
                config.set(key, value)
        
        # 2. Create and initialize brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # 3. Create agent and world
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
        world.reset()
        agent.initialize(world)
        
        # 4. Enable advanced features
        if enable_features:
            agent.enableRewardModulation(True)
            agent.enableStructuralPlasticity(True)
            agent.enableDevelopment(True)
            agent.enableCuriosity(True)
        
        # 5. Add world objects for interaction
        resource1 = pynlm.WorldObject(x=10, y=10, pynlm.WorldObjectType.Resource, value=5.0)
        resource2 = pynlm.WorldObject(x=20, y=20, pynlm.WorldObjectType.Resource, value=3.0)
        hazard = pynlm.WorldObject(x=15, y=15, pynlm.WorldObjectType.Hazard, radius=1.0)
        world.addObject(resource1)
        world.addObject(resource2)
        world.addObject(hazard)
        
        # 6. Run simulation with detailed logging
        stats = {
            'steps': 0,
            'max_firing': 0,
            'total_spikes': 0,
            'curiosity_levels': [],
            'novelty_levels': [],
            'dev_stages': []
        }
        
        for step in range(100):
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Run brain computation
            brain.step(step)
            
            # Get action and execute
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Collect statistics
            stats['steps'] += 1
            stats['max_firing'] = max(stats['max_firing'], brain.getFiringNeuronCount())
            stats['total_spikes'] += brain.getTotalSpikeCount()
            stats['curiosity_levels'].append(agent.getCuriosityLevel())
            stats['novelty_levels'].append(agent.getNoveltyLevel())
            stats['dev_stages'].append(brain.getDevelopmentalStage())
            
            if step % 25 == 0:
                print(f"  Step {step}: Firing={brain.getFiringNeuronCount()}, "
                      f"Curiosity={agent.getCuriosityLevel():.3f}, "
                      f"DevStage={brain.getDevelopmentalStage()}")
        
        print(f"Advanced simulation completed:")
        print(f"  Steps: {stats['steps']}")
        print(f"  Max firing neurons: {stats['max_firing']}")
        print(f"  Total spikes: {stats['total_spikes']}")
        print(f"  Final curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"  Final development stage: {brain.getDevelopmentalStage()}")
        
        return brain, agent, world, stats
    
    # Run the advanced simulation
    brain, agent, world, stats = run_advanced_simulation()
    return stats

def example_performance_monitoring():
    """Demonstrate performance monitoring and optimization"""
    print("\n=== Performance Monitoring ===")
    
    import time
    
    # Create a brain with different sizes to measure performance
    sizes = [100, 500, 1000]
    times = []
    
    for size in sizes:
        start_time = time.time()
        
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", size)
        config.set("region_count", 1)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run a simulation
        for step in range(50):
            brain.step(step)
        
        end_time = time.time()
        elapsed = end_time - start_time
        times.append(elapsed)
        
        print(f"Size {size:4d} neurons: {elapsed:.3f} seconds (50 steps)")
    
    # Calculate performance metrics
    if len(times) >= 2:
        growth_rate = (times[-1] - times[0]) / (sizes[-1] - sizes[0])
        print(f"\nPerformance metrics:")
        print(f"  Average time per neuron: {times[-1]/sizes[-1]:.6f} seconds")
        print(f"  Growth rate: {growth_rate:.8f} seconds per neuron")
        print(f"  Efficiency improvement observed: {((times[0]/sizes[0])/(times[-1]/sizes[-1])):.2f}x")

def main():
    """Run all advanced examples"""
    print("NLM Advanced Python Examples")
    print("=" * 50)
    print(f"pynlm version: {pynlm.__version__}")
    print()
    
    try:
        example_advanced_factory_functions()
        example_batch_processing()
        example_configuration_management()
        example_error_handling()
        example_advanced_agent_simulation()
        example_performance_monitoring()
        
        print("\n" + "=" * 50)
        print("All advanced examples completed successfully!")
        print("\nKey improvements demonstrated:")
        print("  • New factory functions for advanced users")
        print("  • Batch processing capabilities")
        print("  • Configuration management and persistence")
        print("  • Robust error handling")
        print("  • Complete agent simulation with all features")
        print("  • Performance monitoring and optimization")
        
    except Exception as e:
        print(f"\nExample failed with error: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main()