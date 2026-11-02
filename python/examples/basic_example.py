#!/usr/bin/env python3
"""
NLM Python Example: Basic Neural Learning Machine Usage
===========================================================

This example demonstrates the basic usage of the NLM (Neural Learning Machine)
Python bindings with the enhanced user-friendly APIs.

Prerequisites:
    - NLM Python bindings installed (pynlm v2.0.0+)
    - numpy (for array operations)

Installation:
    pip install pynlm numpy
"""

import sys
sys.path.insert(0, '/workspace/app/python')

from nlm import (
    createDefaultConfig,
    createBrain,
    createWorld,
    createAgentBrain,
    quickSimulation,
    runBasicTest,
    analyzeLearning,
    getBrainStats,
    exportBrainState,
    encodeText,
    createTextPercept,
    createSensoryPercept
)

def example_basic_brain():
    """Example 1: Create and run a basic brain"""
    print("=== Example 1: Basic Brain ===")
    
    # Step 1: Create configuration
    config = createDefaultConfig()
    print(f"Created config with {config.getInt('neuron_count', 0)} neurons")
    
    # Step 2: Create brain
    brain = createBrain(config)
    print("Created brain instance")
    
    # Step 3: Run quick simulation with sensory input
    spike_count = quickSimulation(brain, steps=100, input_strength=25.0)
    print(f"Simulation completed with {spike_count} spikes")
    
    # Step 4: Get brain statistics
    stats = getBrainStats(brain)
    print(f"Brain stats: {stats}")
    
    # Step 5: Export brain state
    json_output = exportBrainState(brain, "json")
    print(f"Exported brain state to JSON ({len(json_output)} characters)")
    
    print("Example 1 completed successfully!\n")

def example_learning():
    """Example 2: Learning and plasticity demonstration"""
    print("=== Example 2: Learning and Plasticity ===")
    
    # Create a brain optimized for learning
    config = createLearningConfig(learning_rate=0.05, connection_probability=0.2)
    brain = createBrain(config)
    
    # Analyze learning process
    result = analyzeLearning(brain, steps=500)
    learning_occurred, weight_change, initial_synapses, final_synapses = result
    
    print(f"Learning occurred: {learning_occurred}")
    print(f"Weight change: {weight_change:.4f}")
    print(f"Synapses - Initial: {initial_synapses}, Final: {final_synapses}")
    
    # Reset and run basic test
    brain.reset()
    spike_count = runBasicTest(brain)
    print(f"Basic test spike count: {spike_count}")
    
    print("Example 2 completed successfully!\n")

def example_world_interaction():
    """Example 3: World interaction with agent"""
    print("=== Example 3: World Interaction ===")
    
    # Create world
    world = createWorld(20, 20, 16, 16)
    print(f"Created world {world.getWidth()}x{world.getHeight()}")
    
    # Create brain and agent
    config = createDefaultConfig()
    brain = createBrain(config)
    agent = createAgentBrain(brain)
    
    # Initialize agent with world
    agent.initialize(world)
    print(f"Agent initialized - Sensory input size: {agent.getSensoryInputSize()}")
    print(f"Agent motor output size: {agent.getMotorOutputSize()}")
    
    # Create sensory percept
    vision = [0.5] * 256  # Uniform vision
    touch = [0.0] * 8
    internal = [0.0] * 4
    proprioception = [0.0] * 6
    
    percept = createSensoryPercept(vision, touch, internal, proprioception)
    
    # Process sensory input
    agent.processSensoryInput(percept)
    print("Processed sensory input")
    
    # Decode motor command
    action = agent.decodeMotorCommand()
    print(f"Decoded action: {action}")
    
    # Get neuromodulation levels
    print(f"Dopamine level: {agent.getNeuromodulationLevel():.3f}")
    print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
    
    print("Example 3 completed successfully!\n")

def example_text_processing():
    """Example 4: Text processing (Phase 6 feature)"""
    print("=== Example 4: Text Processing ===")
    
    # Encode text into neural patterns
    words = ["apple", "zebra", "hello", "world"]
    
    for word in words:
        pattern = encodeText(word)
        print(f"'{word}' encoded to pattern: {pattern}")
    
    # Create percept with text
    vision = [0.5] * 256
    touch = [0.0] * 8
    
    text_percept = createTextPercept("test message", vision, touch)
    print("Created percept with encoded text")
    
    print("Example 4 completed successfully!\n")

def example_batch_processing():
    """Example 5: Batch processing multiple brains"""
    print("=== Example 5: Batch Processing ===")
    
    # Create multiple brains for comparison
    brains = []
    rewards = [10.0, -5.0, 15.0, 0.0, 8.0]
    
    for i in range(len(rewards)):
        config = createDefaultConfig()
        config.set("neuron_count", 100 + i * 50)
        brain = createBrain(config)
        brains.append(brain)
    
    print(f"Created {len(brains)} brains with different sizes")
    
    # Process all brains
    for i, brain in enumerate(brains):
        result = analyzeLearning(brain, 100)
        learning, change, init_syn, final_syn = result
        print(f"Brain {i}: Learning={learning}, Weight change={change:.4f}")
    
    # Compare all brains
    labels = [f"Brain_{i}" for i in range(len(brains))]
    comparison = compareBrains(brains, labels)
    
    print("Batch processing completed successfully!\n")

def example_advanced_export():
    """Example 6: Advanced export and analysis"""
    print("=== Example 6: Advanced Export and Analysis ===")
    
    config = createDefaultConfig()
    brain = createBrain(config)
    
    # Run a simulation
    quickSimulation(brain, 50, 20.0)
    
    # Get detailed statistics
    stats = getBrainStats(brain)
    print("Brain Statistics:")
    for key, value in stats.items():
        print(f"  {key}: {value}")
    
    # Export in different formats
    print("\nJSON Export:")
    json_output = exportBrainState(brain, "json")
    print(f"  Length: {len(json_output)} characters")
    
    print("\nCSV Export:")
    csv_output = exportBrainState(brain, "csv")
    print(f"  Length: {len(csv_output)} characters")
    
    # Demonstrate error handling
    print("\nError Handling Demonstration:")
    try:
        # This will fail with invalid format
        exportBrainState(brain, "invalid_format")
    except Exception as e:
        print(f"  Caught expected error: {type(e).__name__}")
    
    print("Example 6 completed successfully!\n")

def example_demo_workflow():
    """Example 7: Complete demo workflow"""
    print("=== Example 7: Complete Demo Workflow ===")
    
    # Use the demo brain factory
    result = createDemoBrain()
    brain, world, agent = result
    
    print("Created demo brain with world and agent")
    
    # Run a complete cycle
    agent.initialize(world)
    
    # Create initial percept
    vision = [0.8 if i % 32 == 0 else 0.2 for i in range(256)]
    touch = [0.0] * 8
    percept = createSensoryPercept(vision, touch)
    
    agent.processSensoryInput(percept)
    action = agent.decodeMotorCommand()
    
    print(f"Processed vision pattern and decoded action: {action}")
    
    # Show development stage
    print(f"Developmental stage: {agent.getDevelopmentalStage()}")
    
    # Show learning indicators
    print(f"Learning preference: {brain.getLearnedPreference():.3f}")
    
    print("Example 7 completed successfully!\n")

def example_configuration_templating():
    """Example 8: Configuration templating"""
    print("=== Example 8: Configuration Templating ===")
    
    # Create brains from different templates
    brain_types = ["basic", "sensorimotor", "cognitive"]
    
    for brain_type in brain_types:
        print(f"\nCreating {brain_type} brain:")
        brain = createFromTemplate(brain_type)
        
        # Get brain statistics
        stats = getBrainStats(brain)
        print(f"  Neurons: {stats['total_neurons']}")
        print(f"  Regions: {brain.getRegionCount()}")
        
        # Quick simulation
        quickSimulation(brain, 20, 10.0)
    
    print("\nConfiguration templating completed successfully!\n")

def example_complete_pipeline():
    """Example 9: Complete processing pipeline"""
    print("=== Example 9: Complete Processing Pipeline ===")
    
    # Step 1: Create optimized configuration
    config = createLearningConfig(learning_rate=0.03, connection_probability=0.12)
    print("Step 1: Created learning-optimized configuration")
    
    # Step 2: Create brain with agent interface
    brain = createBrain(config)
    world = createWorld(30, 30, 16, 16)
    agent = createAgentBrain(brain)
    
    print("Step 2: Created brain-world-agent pipeline")
    
    # Step 3: Initialize and run
    agent.initialize(world)
    print("Step 3: Initialized agent")
    
    # Step 4: Process multiple percepts
    for i in range(3):
        vision = [0.5 + 0.1 * (i % 10) for _ in range(256)]
        touch = [0.0] * 8
        internal = [0.0] * 4
        proprioception = [0.0] * 6
        
        percept = createSensoryPercept(vision, touch, internal, proprioception)
        agent.processSensoryInput(percept)
        
        action = agent.decodeMotorCommand()
        print(f"  Step 4.{i+1}: Processed percept, decoded action: {action}")
    
    # Step 5: Analyze learning progress
    quickSimulation(brain, 100, 15.0)
    stats = getBrainStats(brain)
    print("Step 5: Analyzed learning progress")
    
    # Step 6: Export for analysis
    json_output = exportBrainState(brain, "json")
    print(f"Step 6: Exported analysis results ({len(json_output)} chars)")
    
    print("Complete pipeline completed successfully!\n")

def main():
    """Run all examples"""
    print("NLM Python Bindings Examples")
    print("===========================")
    print()
    
    examples = [
        example_basic_brain,
        example_learning,
        example_world_interaction,
        example_text_processing,
        example_batch_processing,
        example_advanced_export,
        example_demo_workflow,
        example_configuration_templating,
        example_complete_pipeline
    ]
    
    for i, example in enumerate(examples, 1):
        try:
            example()
        except Exception as e:
            print(f"ERROR in Example {i}: {e}")
            import traceback
            traceback.print_exc()
            print(f"\nExample {i} failed. Continuing with next example...\n")
    
    print("All examples completed!")

if __name__ == "__main__":
    main()
