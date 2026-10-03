#!/usr/bin/env python3
"""
NLM Python Examples - Demonstrating the Enhanced NLM API

This file contains comprehensive examples showcasing the features of the
enhanced NLM Python bindings. These examples cover basic usage, advanced
features, and common patterns for working with the neural simulation framework.

To run these examples:
1. Ensure NLM is properly installed: pip install pynlm
2. Place this file in your project directory
3. Run: python nlm_examples.py
"""

import pynlm
import time
import sys
from typing import Dict, Any, List
def print_header(title: str, width: int = 60) -> None:
    """Print a formatted header."""
    print(f"\n{'=' * width}")
    print(f"{title}")
    print(f"{'=' * width}")
def example_basic_brain() -> None:
    """Example 1: Basic brain simulation without environment."""
    print_header("Example 1: Basic Brain Simulation")
    
    # Simplest possible brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    print("Created and initialized brain")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Initial firing neurons: {brain.getFiringNeuronCount()}")
    
    # Run simulation steps
    for i in range(10):
        brain.step(i)
        print(f"Step {i}: {brain.getFiringNeuronCount()} neurons firing, "
              f"{brain.getTotalSpikeCount()} total spikes")
    
    print("\n✓ Basic brain simulation completed successfully")
def example_complete_agent() -> None:
    """Example 2: Complete agent with environment interaction."""
    print_header("Example 2: Complete Agent Simulation")
    
    # Setup complete agent
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent.initialize(world)
    
    # Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("Created complete agent with world")
    print(f"Brain: {brain.getTotalNeuronCount()} neurons")
    print(f"World: {world.getWidth()}x{world.getHeight()} units")
    print(f"Vision: {world.getVisionDimensions()[0]}x{world.getVisionDimensions()[1]} pixels")
    
    # Run simulation loop
    for step in range(20):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 5 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
            print(f"  Agent position: {world.getAgentPosition()}")
    
    print("\n✓ Complete agent simulation completed successfully")
def example_custom_configuration() -> None:
    """Example 3: Custom configuration with specific parameters."""
    print_header("Example 3: Custom Configuration")
    
    # Create custom configuration
    config = pynlm.createDefaultConfig()
    
    # Modify parameters for specific behavior
    config.set("brain.neuron_count", 500)
    config.set("brain.synapse_density", 0.05)
    config.set("brain.v_thresh", -45.0)  # Higher threshold
    config.set("brain.tau_ref", 5.0)     # Longer refractory period
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    print("Created custom configuration:")
    print(f"  Neuron count: {config.getInt('brain.neuron_count')}")
    print(f"  Synapse density: {config.getFloat('brain.synapse_density')}")
    print(f"  Threshold: {config.getFloat('brain.v_thresh')}")
    print(f"  Refractory period: {config.getFloat('brain.tau_ref')}")
    print(f"  STDP enabled: {config.getBool('plasticity.stdp.enable')}")
    print(f"  Hebbian enabled: {config.getBool('plasticity.hebbian.enable')}")
    print(f"  Curiosity enabled: {config.getBool('neuromod.curiosity.enable')}")
    
    # Create brain with custom config
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"\nCreated brain with custom config:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  EI ratio: {brain.getExcitationInhibitionRatio():.3f}")
    print(f"  Initial development stage: {brain.getDevelopmentalStage()}")
    
    # Run simulation to see effects
    for i in range(5):
        brain.step(i)
    
    print(f"\nAfter 5 steps:")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    
    print("\n✓ Custom configuration example completed successfully")
def example_memory_systems() -> None:
    """Example 4: Working with memory systems."""
    print_header("Example 4: Memory Systems Integration")
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Get memory system access
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    associative_memory = brain.getAssociativeMemory()
    
    print("Memory systems initialized:")
    print(f"  Working memory: {working_memory is not None}")
    print(f"  Episodic memory: {episodic_memory is not None}")
    print(f"  Associative memory: {associative_memory is not None}")
    
    # Inject some sensory input to generate memories
    for step in range(10):
        brain.step(step)
        
        if step % 3 == 0:
            # Create some artificial sensory input
            from pynlm import Vision
            vision = Vision(8, 8, 3)
            # Create some random-like data
            import random
            data = [random.uniform(0, 1) for _ in range(8 * 8 * 3)]
            vision.setData(data)
            
            brain.receiveSensoryInput(vision)
            print(f"  Step {step}: Injected sensory input, "
                  f"Working memory patterns: {working_memory.getActivePatternCount()}")
    
    print("\n✓ Memory systems example completed successfully")
def example_plasticity_systems() -> None:
    """Example 5: Working with plasticity systems."""
    print_header("Example 5: Plasticity Systems")
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Get plasticity system access
    stdp = brain.getSTDP()
    hebbian = brain.getHebbian()
    structural = brain.getStructuralPlasticity()
    
    print("Plasticity systems initialized:")
    print(f"  STDP enabled: {stdp.isEnabled()}")
    print(f"  STDP learning rate: {stdp.getLearningRate()}")
    print(f"  STDP time constants: {stdp.getTauPlus()}/{stdp.getTauMinus()}")
    
    print(f"  Hebbian enabled: {hebbian.isEnabled()}")
    print(f"  Hebbian learning rate: {hebbian.getLearningRate()}")
    
    print(f"  Structural plasticity enabled: {structural.isEnabled()}")
    print(f"  Max synapses per neuron: {structural.getMaxSynapsesPerNeuron()}")
    
    # Run simulation to observe plasticity
    print("\nRunning simulation to observe plasticity:")
    for step in range(15):
        brain.step(step)
        
        if step % 5 == 0:
            print(f"  Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
                  f"{brain.getTotalSpikeCount()} total spikes")
    
    print("\n✓ Plasticity systems example completed successfully")
def example_neuromodulation() -> None:
    """Example 6: Working with neuromodulation."""
    print_header("Example 6: Neuromodulation Systems")
    
    # Create agent to access neuromodulation
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Enable neuromodulation
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableNovelty(True)
    
    print("Neuromodulation systems enabled:")
    print(f"  Reward modulation: {agent.isRewardModulationEnabled()}")
    print(f"  Structural plasticity: {agent.isStructuralPlasticityEnabled()}")
    print(f"  Development: {agent.isDevelopmentEnabled()}")
    print(f"  Curiosity: {agent.isCuriosityEnabled()}")
    
    # Run simulation to observe neuromodulation
    print("\nRunning simulation to observe neuromodulation:")
    for step in range(20):
        world.update(0.1)
        
        # Create some sensory input
        from pynlm import Vision
        vision = Vision(8, 8, 3)
        data = [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8] * 3
        vision.setData(data)
        
        agent.processSensoryInput(vision)
        brain.step(step)
        
        # Get motor command and apply to world
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 5 == 0:
            print(f"  Step {step}:")
            print(f"    Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"    Novelty level: {agent.getNoveltyLevel():.3f}")
            print(f"    Prediction error: {agent.getPredictionError():.3f}")
            print(f"    Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
            print(f"    Development stage: {agent.getDevelopmentalStage()}")
    
    print("\n✓ Neuromodulation example completed successfully")
def example_state_conversion() -> None:
    """Example 7: Converting between Python objects and dictionaries."""
    print_header("Example 7: State Conversion and Serialization")
    
    # Create brain and collect state
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Convert to dictionary
    brain_state = brain.toDict()
    print("Brain state as dictionary:")
    for key, value in brain_state.items():
        print(f"  {key}: {value}")
    
    # Create agent and world
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Convert agent to dictionary
    agent_state = agent.toDict()
    print("\nAgent state as dictionary:")
    for key, value in agent_state.items():
        print(f"  {key}: {value}")
    
    # Create percept and convert
    percept = world.getSensoryPercept()
    percept_dict = percept.toDict()
    print(f"\nPercept dictionary keys: {list(percept_dict.keys())}")
    
    # Create action and convert
    from pynlm import Action, ActionType
    action = Action(ActionType.MoveForward, [0.5, 0.5])
    action_dict = action.toDict()
    print(f"\nAction dictionary: {action_dict}")
    
    # Convert back from dictionary
    restored_action = Action.fromDict(action_dict)
    print(f"Restored action type: {restored_action.getType()}")
    print(f"Restored action parameters: {restored_action.getParameters()}")
    
    print("\n✓ State conversion example completed successfully")
def example_performance_benchmark() -> None:
    """Example 8: Performance benchmark and scaling."""
    print_header("Example 8: Performance Benchmark")
    
    # Test with different brain sizes
    brain_sizes = [100, 500, 1000]
    
    for size in brain_sizes:
        print(f"\nTesting with {size} neurons:")
        
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", size)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        start_time = time.time()
        start_spikes = brain.getTotalSpikeCount()
        
        # Run simulation
        for step in range(100):
            brain.step(step)
        
        end_time = time.time()
        end_spikes = brain.getTotalSpikeCount()
        
        duration = end_time - start_time
        spikes_per_second = (end_spikes - start_spikes) / duration if duration > 0 else 0
        neurons_per_second = size / duration if duration > 0 else 0
        
        print(f"  Duration: {duration:.3f} seconds")
        print(f"  Spikes generated: {end_spikes - start_spikes}")
        print(f"  Spikes per second: {spikes_per_second:.0f}")
        print(f"  Neurons processed per second: {neurons_per_second:.0f}")
        print(f"  Final firing neurons: {brain.getFiringNeuronCount()}")
    
    print("\n✓ Performance benchmark completed successfully")
def example_error_handling() -> None:
    """Example 9: Error handling and validation."""
    print_header("Example 9: Error Handling and Validation")
    
    # Create brain and config
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    config = brain.getConfig()
    
    print("Testing error handling:")
    
    # Test 1: Access non-existent configuration
    try:
        value = config.getInt("nonexistent_key")
        print("  ✗ Should have thrown InvalidArgument for missing key")
    except pynlm.InvalidArgument as e:
        print(f"  ✓ Correctly caught InvalidArgument: {e}")
    
    # Test 2: Invalid vision data
    try:
        from pynlm import Vision
        vision = Vision(4, 4, 3)
        # Try to set data with wrong size
        invalid_data = [1.0, 2.0, 3.0]  # Too small
        vision.setData(invalid_data)
        print("  ✗ Should have thrown error for invalid data size")
    except Exception as e:
        print(f"  ✓ Correctly caught error for invalid data: {type(e).__name__}")
    
    # Test 3: Valid operations
    print("\nTesting valid operations:")
    config.set("valid_key", "test_value")
    if config.has("valid_key"):
        value = config.getString("valid_key")
        print(f"  ✓ Successfully got configuration value: {value}")
    
    print("\n✓ Error handling example completed successfully")
def example_advanced_features() -> None:
    """Example 10: Advanced features demonstration."""
    print_header("Example 10: Advanced Features Demonstration")
    
    # Create enhanced configuration
    config = pynlm.createDefaultConfig(
        neuron_count=2000,
        connection_probability=0.02
    )
    
    print("Created enhanced configuration:")
    print(f"  Neuron count: {config.getInt('brain.neuron_count')}")
    print(f"  Connection probability: {config.getFloat('brain.connection_probability')}")
    
    # Create brain with enhanced config
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent with all subsystems
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=16, visionHeight=16)
    world.reset()
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print(f"\nAdvanced agent created:")
    print(f"  Brain neurons: {brain.getTotalNeuronCount()}")
    print(f"  World size: {world.getWidth()}x{world.getHeight()}")
    print(f"  Vision: {world.getVisionDimensions()[0]}x{world.getVisionDimensions()[1]}")
    print(f"  All subsystems enabled: Reward={agent.isRewardModulationEnabled()}, "
          f"Plasticity={agent.isStructuralPlasticityEnabled()}, "
          f"Development={agent.isDevelopmentEnabled()}, "
          f"Curiosity={agent.isCuriosityEnabled()}")
    
    # Run advanced simulation
    print("\nRunning advanced simulation:")
    for step in range(30):
        world.update(0.1)
        
        # Create complex sensory input
        from pynlm import Vision, InternalSignals
        import random
        
        # Vision input
        vision = Vision(16, 16, 3)
        vision_data = [random.uniform(0, 1) for _ in range(16 * 16 * 3)]
        vision.setData(vision_data)
        
        # Internal signals
        internal = InternalSignals()
        for i in range(5):
            internal.addSignal(random.uniform(0, 1))
        
        # Create percept
        from pynlm import SensoryPercept
        percept = SensoryPercept()
        percept.setVision(vision)
        percept.setInternal(internal)
        
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = random.uniform(0, 1) if random.random() > 0.7 else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        if step % 10 == 0:
            print(f"  Step {step}:")
            print(f"    Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"    Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"    Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"    Dev Stage: {agent.getDevelopmentalStage()}")
    
    print("\n✓ Advanced features demonstration completed successfully")
def run_all_examples() -> None:
    """Run all examples in sequence."""
    print("🚀 NLM Python Examples - Enhanced API Demonstration")
    print("=" * 60)
    print("This script demonstrates the enhanced NLM Python API features.")
    print("Each example shows different aspects of the neural simulation framework.")
    print()
    
    examples = [
        ("Basic Brain Simulation", example_basic_brain),
        ("Complete Agent Simulation", example_complete_agent),
        ("Custom Configuration", example_custom_configuration),
        ("Memory Systems Integration", example_memory_systems),
        ("Plasticity Systems", example_plasticity_systems),
        ("Neuromodulation Systems", example_neuromodulation),
        ("State Conversion", example_state_conversion),
        ("Performance Benchmark", example_performance_benchmark),
        ("Error Handling", example_error_handling),
        ("Advanced Features", example_advanced_features),
    ]
    
    completed = 0
    failed = 0
    
    for title, example_func in examples:
        try:
            example_func()
            completed += 1
        except Exception as e:
            print(f"\n❌ Example '{title}' failed with error: {e}")
            import traceback
            traceback.print_exc()
            failed += 1
        
        print(f"\n{'=' * 60}")
    
    print("\n" + "=" * 60)
    print(f"📊 Example Summary:")
    print(f"  ✓ Completed: {completed}")
    print(f"  ✗ Failed: {failed}")
    print(f"  📁 Total: {len(examples)}")
    print("\nAll examples demonstrate the enhanced NLM Python API!")
    print("Key improvements shown:")
    print("  • Better error handling and validation")
    print("  • Pythonic API with convenience methods")
    print("  • State conversion utilities")
    print("  • Advanced configuration options")
    print("  • Integration with all NLM subsystems")
    print("  • Performance monitoring capabilities")
def main() -> None:
    """Main function to run all examples."""
    try:
        run_all_examples()
        
        print("\n🎉 NLM Examples completed successfully!")
        print("\nThe enhanced NLM Python API provides:")
        print("  • Comprehensive documentation")
        print("  • Type safety with validation")
        print("  • Pythonic convenience methods")
        print("  • State conversion utilities")
        print("  • Error handling and debugging tools")
        print("  • Performance monitoring capabilities")
        print("  • Integration with all NLM subsystems")
        
    except KeyboardInterrupt:
        print("\n⚠️  Execution interrupted by user")
        sys.exit(1)
    except Exception as e:
        print(f"\n❌ Unexpected error: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)
if __name__ == "__main__":
    main()