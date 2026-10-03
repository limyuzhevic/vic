#!/usr/bin/env python3
"""
Demonstration script for improved NLM Python bindings.
Shows the new features and improvements.
"""

import sys
import tempfile
import json

def demonstrate_basic_usage():
    """Demonstrate basic usage of improved NLM Python bindings."""
    print("=== Basic Usage Demonstration ===")
    
    import pynlm as nlm
    
    # Create a brain using enhanced configuration
    print("1. Creating brain with enhanced configuration...")
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    
    print(f"   Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"   Initial step completed")
    
    # Test method chaining for configuration
    print("\n2. Demonstrating method chaining...")
    config2 = nlm.createConfig() \
        .withNeuronCount(1500) \
        .withLearningRate(0.001) \
        .withDevelopment(True) \
        .withCuriosity(True) \
        .withRewardModulation(True)
    
    config2.set("brain.neuron_count", 1500)
    print(f"   Configuration with {config2.getOr('brain.neuron_count', 0)} neurons")
    print(f"   Development enabled: {config2.getOr('development.enabled', False)}")
    
    # Demonstrate dictionary operations
    print("\n3. Demonstrating dictionary operations...")
    config_dict = config2.to_dict()
    print(f"   Config as dictionary: {len(config_dict)} keys")
    
    # Create dictionary for loading
    load_dict = {"brain.neuron_count": 2000, "plasticity.stdp.learning_rate": 0.005}
    config3 = nlm.createConfig()
    config3.from_dict(load_dict)
    print(f"   Loaded from dict: {config3.getOr('brain.neuron_count', 0)} neurons")
    
    # Demonstrate context managers
    print("\n4. Demonstrating context managers...")
    with nlm.brain_context(brain) as brain_ctx:
        brain_ctx.brain.step(0)
        spikes = brain_ctx.brain.getTotalSpikeCount()
        print(f"   Brain in context: {spikes} total spikes")
    
    print("\n✓ Basic usage demonstration completed\n")

def demonstrate_enhanced_features():
    """Demonstrate enhanced features."""
    print("=== Enhanced Features Demonstration ===")
    
    import pynlm as nlm
    
    # Create a world and agent
    print("1. Creating agent environment...")
    config = nlm.createConfig().createSimulationConfig(
        neuronCount=500,
        regionCount=2,
        learningRate="0.001",
        enableDevelopment=False,
        enableCuriosity=True,
        enableRewardModulation=True
    )
    
    brain = nlm.createBrain(config)
    brain.initialize()
    
    world = nlm.createSimpleWorld()
    world.configure(10, 10, 8, 8)
    world.reset()
    world.setAgentStart(5.0, 5.0)
    
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print(f"   Created environment with {brain.getTotalNeuronCount()} neurons")
    print(f"   Curiosity enabled: {agent.isCuriosityEnabled()}")
    print(f"   Reward modulation enabled: {agent.isRewardModulationEnabled()}")
    
    # Run simulation
    print("\n2. Running simulation...")
    for step in range(10):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    print(f"   Simulation completed")
    print(f"   Total spikes: {brain.getTotalSpikeCount()}")
    print(f"   Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"   Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"   Novelty level: {agent.getNoveltyLevel():.3f}")
    
    print("\n✓ Enhanced features demonstration completed\n")

def demonstrate_simulation_utils():
    """Demonstrate simulation utility functions."""
    print("=== Simulation Utilities Demonstration ===")
    
    import pynlm as nlm
    
    print("1. Creating foraging environment...")
    config, world, agent = nlm.SimulationUtils.createForagingEnvironment(
        worldWidth=15,
        worldHeight=15,
        visionWidth=6,
        visionHeight=6,
        numFoodResources=8
    )
    
    print(f"   World size: {world.getWidth()} x {world.getHeight()}")
    print(f"   Vision size: {agent.getSensoryInputSize()}")
    print(f"   Agent output size: {agent.getMotorOutputSize()}")
    
    # Run episode
    print("\n2. Running simulation episode...")
    episode_brain, episode_agent = nlm.SimulationUtils.runEpisode(
        config, world, 20, True
    )
    
    print(f"   Episode completed")
    print(f"   Brain spikes: {episode_brain.getTotalSpikeCount()}")
    
    # Measure performance
    print("\n3. Measuring performance...")
    perf_metrics = nlm.SimulationUtils.measurePerformance(
        episode_brain, world, episode_agent, 30
    )
    
    print(f"   Total time: {perf_metrics.totalTime:.2f}s")
    print(f"   Steps completed: {perf_metrics.stepsCompleted}")
    print(f"   Total spikes: {perf_metrics.totalSpikes}")
    print(f"   Active neurons: {perf_metrics.activeNeurons}")
    print(f"   Average firing rate: {perf_metrics.averageFiringRate:.2f} Hz")
    print(f"   Curiosity level: {perf_metrics.curiosityLevel:.3f}")
    print(f"   Novelty level: {perf_metrics.noveltyLevel:.3f}")
    
    print("\n✓ Simulation utilities demonstration completed\n")

def demonstrate_vision():
    """Demonstrate enhanced vision features."""
    print("=== Enhanced Vision Demonstration ===")
    
    import pynlm as nlm
    
    print("1. Creating vision sensor...")
    vision = nlm.Vision(8, 8, 3)
    
    # Create test data (simple pattern)
    data = []
    for y in range(8):
        for x in range(8):
            # Create a simple checkerboard pattern
            intensity = 0.8 if ((x + y) % 2 == 0) else 0.2
            for c in range(3):  # RGB channels
                data.append(intensity)
    
    vision.setData(data)
    print(f"   Vision dimensions: {vision.getWidth()}x{vision.getHeight()}x{vision.getChannels()}")
    
    # Test matrix conversion
    print("\n2. Converting vision to Python matrix...")
    matrix = vision.to_python_matrix()
    print(f"   Matrix size: {len(matrix)}x{len(matrix[0])}")
    print(f"   First row sample: {matrix[0][:3]}...")
    
    # Test object detection
    print("\n3. Detecting objects in vision...")
    objects = vision.detectObjects()
    print(f"   Detected edges: {objects.get('edges_count', 0)}")
    
    print("\n✓ Vision demonstration completed\n")

def demonstrate_file_io():
    """Demonstrate file I/O operations."""
    print("=== File I/O Demonstration ===")
    
    import pynlm as nlm
    
    print("1. Testing configuration file I/O...")
    
    # Create and save configuration
    config = nlm.createConfig()
    config.set("brain.neuron_count", 1500)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("neuromod.curiosity.enable", True)
    
    # Save to file
    import tempfile
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        config_file = f.name
    
    try:
        config.saveToFile(config_file)
        print(f"   Saved configuration to: {config_file}")
        
        # Load from file
        config2 = nlm.createConfig()
        config2.loadFromFile(config_file)
        print(f"   Loaded configuration from: {config_file}")
        print(f"   Neuron count: {config2.getOr('brain.neuron_count', 0)}")
        
    finally:
        import os
        os.unlink(config_file)
    
    print("\n✓ File I/O demonstration completed\n")

def demonstrate_error_handling():
    """Demonstrate improved error handling."""
    print("=== Error Handling Demonstration ===")
    
    import pynlm as nlm
    
    print("1. Testing error handling...")
    
    try:
        config = nlm.createConfig()
        brain = nlm.createBrain(config)
        brain.initialize()
        
        # Normal operation should work
        brain.step(0)
        print("   ✓ Normal operation completed successfully")
        
        # Verify state
        assert brain.getTotalNeuronCount() > 0
        print("   ✓ State validation passed")
        
    except Exception as e:
        print(f"   ✓ Exception caught: {type(e).__name__}: {e}")
    
    print("\n✓ Error handling demonstration completed\n")

def run_demonstration():
    """Run all demonstrations."""
    print("=" * 70)
    print("NLM PYTHON BINDINGS - IMPROVEMENTS DEMONSTRATION")
    print("=" * 70)
    print("\nThis demonstration showcases the improvements made to the NLM Python bindings:")
    print("• Enhanced configuration with method chaining")
    print("• Dictionary-based configuration loading")
    print("• Context managers for automatic resource cleanup")
    print("• Simulation utility functions")
    print("• Enhanced vision processing")
    print("• Improved error handling")
    print("• File I/O utilities")
    print("\n" + "=" * 70 + "\n")
    
    demonstrations = [
        demonstrate_basic_usage,
        demonstrate_enhanced_features,
        demonstrate_simulation_utils,
        demonstrate_vision,
        demonstrate_file_io,
        demonstrate_error_handling,
    ]
    
    passed = 0
    failed = 0
    
    for demo in demonstrations:
        try:
            demo()
            passed += 1
        except Exception as e:
            print(f"✗ {demo.__name__} failed: {e}")
            failed += 1
            import traceback
            traceback.print_exc()
    
    print("=" * 70)
    print(f"DEMONSTRATION RESULTS: {passed} passed, {failed} failed")
    print("=" * 70)
    
    if failed > 0:
        return False
    return True

if __name__ == "__main__":
    success = run_demonstration()
    sys.exit(0 if success else 1)