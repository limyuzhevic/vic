"""
NLM Example Scripts

Collection of working example scripts for different NLM use cases.
"""

import pynlm
import time
from typing import Any

# Example 1: Basic Brain Simulation

def basic_brain_example():
    """
    Example 1: Basic Brain Simulation
    
    This example demonstrates the simplest possible way to create and run
    a NLM brain simulation.
    """
    print("Example 1: Basic Brain Simulation")
    print("-" * 40)
    
    # Create a simple brain (beginner preset)
    brain = pynlm.createSimpleBrain()
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Development stage: {brain.getDevelopmentStage()}")
    
    # Run a short simulation
    for step in range(10):
        brain.step(step)
    
    print(f"Completed 10 steps: {brain.getTotalSpikeCount()} total spikes")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print()

# Example 2: Agent in World

def agent_world_example():
    """
    Example 2: Agent in World
    
    This example demonstrates how to create an agent that interacts
    with a simulated world, processing sensory input and making decisions.
    """
    print("Example 2: Agent in World")
    print("-" * 40)
    
    # Create a complete agent setup (explorer preset for learning)
    brain, world, agent = pynlm.create_explorer()
    
    print(f"Created agent with {brain.getTotalNeuronCount()} neurons")
    print(f"World size: {world.getWidth()}x{world.getHeight()}")
    
    # Run a complete episode
    pynlm.run_reproduction_experiment(brain, world, agent, steps=50)
    print()

# Example 3: Memory System Usage

def memory_system_example():
    """
    Example 3: Memory System Usage
    
    This example demonstrates how to use NLM's memory systems (working memory
    and episodic memory) to store and retrieve information.
    """
    print("Example 3: Memory System Usage")
    print("-" * 40)
    
    # Create brain with memory systems enabled
    config = pynlm.createConfig({
        'neuromodulation': {'enable_memory': True},
        'brain': {'neuron_count': 2000}
    })
    
    brain = pynlm.createBrain(config)
    brain.enableWorkingMemory(True)
    brain.enableEpisodicMemory(True)
    
    brain.initialize()
    
    print("Memory systems enabled:")
    print(f"  Working memory: {brain.getWorkingMemoryTraceCount()} traces")
    print(f"  Episodic memory: {brain.getEpisodicMemoryCount()} episodes")
    
    # Add some working memory traces
    brain.addWorkingMemoryTrace("goal", [0.1, 0.2, 0.3, 0.4, 0.5])
    brain.addWorkingMemoryTrace("strategy", [0.8, 0.7, 0.6, 0.5, 0.4])
    
    trace = brain.getWorkingMemoryTrace("goal")
    print(f"Retrieved working memory trace 'goal': {trace}")
    
    # Run simulation to generate episodic memory
    for step in range(200):
        brain.step(step)
        
        if step % 50 == 0:
            print(f"Step {step}: Working memory traces: {brain.getWorkingMemoryTraceCount()}")
    
    print(f"Final episodic memory count: {brain.getEpisodicMemoryCount()}")
    print()

# Example 4: Development Process

def development_example():
    """
    Example 4: Development Process
    
    This example demonstrates how NLM's developmental system works,
    showing how brains change and mature over time.
    """
    print("Example 4: Development Process")
    print("-" * 40)
    
    # Create brain with development enabled
    config = pynlm.createConfig({
        'development': {'enable_development': True},
        'brain': {'neuron_count': 3000}
    })
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Development process tracking:")
    
    # Track developmental stages over time
    previous_stage = brain.getDevelopmentStage()
    stage_changes = []
    
    for step in range(100):
        brain.step(step)
        
        current_stage = brain.getDevelopmentStage()
        if current_stage != previous_stage:
            stage_changes.append((step, current_stage))
            previous_stage = current_stage
        
        if step % 25 == 0:
            print(f"  Step {step}: {current_stage} stage")
    
    print(f"Development stages experienced: {[s for _, s in stage_changes]}")
    print(f"Stage changes occurred at steps: {[s for s, _ in stage_changes]}")
    print()

# Example 5: Prediction System

def prediction_example():
    """
    Example 5: Prediction System
    
    This example demonstrates NLM's prediction system, which can
    anticipate future events and reduce uncertainty.
    """
    print("Example 5: Prediction System")
    print("-" * 40)
    
    # Create brain with prediction enabled
    config = pynlm.createConfig({
        'neuromodulation': {'enable_prediction': True},
        'brain': {'neuron_count': 2000}
    })
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    brain.enablePredictionSystem(True)
    
    print("Prediction system enabled")
    
    # Generate a test state
    state = [0.1, 0.2, 0.3, 0.4, 0.5] * 10  # 50-element state vector
    
    # Make a prediction
    prediction = brain.predictNextAction(state)
    prediction_error = brain.getPredictionError()
    
    print(f"State dimension: {len(state)}")
    print(f"Prediction error: {prediction_error:.4f}")
    print(f"Prediction confidence: {(1.0 - abs(prediction_error)):.2f}")
    print()

# Example 6: Complete Agent with Advanced Features

def complete_agent_example():
    """
    Example 6: Complete Agent with Advanced Features
    
    This example demonstrates a complete agent setup using all advanced
    NLM features including neuromodulation, memory, and prediction.
    """
    print("Example 6: Complete Agent with Advanced Features")
    print("-" * 40)
    
    # Create agent with all systems enabled
    brain, world, agent = pynlm.create_researcher()
    
    print(f"Created research-grade agent:")
    print(f"  - Neurons: {brain.getTotalNeuronCount()}")
    print(f"  - Memory enabled: {brain.getWorkingMemoryTraceCount()} working traces")
    print(f"  - Prediction enabled: {brain.getPredictionError():.4f} error")
    
    # Advanced neuromodulation
    print("\nAdvanced neuromodulation settings:")
    print(f"  - Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  - Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"  - Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    
    # Run a focused experiment
    print("\nRunning focused experiment:")
    for step in range(50):
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processes
        brain.step(step)
        
        # Decode action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Show progress
        if step % 10 == 0:
            print(f"  Step {step}: Action={action}, "
                  f"Firing={brain.getFiringNeuronCount()}, "
                  f"Curiosity={agent.getCuriosityLevel():.2f}")
    
    print("✓ Advanced agent experiment completed")
    print()

# Example 7: Batch Processing

def batch_processing_example():
    """
    Example 7: Batch Processing
    
    This example demonstrates how to run multiple simulations in parallel
    using NLM's batch processing capabilities.
    """
    print("Example 7: Batch Processing")
    print("-" * 40)
    
    # Create multiple brains with different configurations
    configs = [
        pynlm.createConfig({'brain': {'neuron_count': 1000, 'synapse_density': 0.1}}),
        pynlm.createConfig({'brain': {'neuron_count': 5000, 'synapse_density': 0.05}}),
        pynlm.createConfig({'brain': {'neuron_count': 10000, 'synapse_density': 0.02}}),
    ]
    
    brains = pynlm.createBrainBatch(configs)
    
    print(f"Created {len(brains)} brains for batch processing")
    for i, brain in enumerate(brains):
        brain.initialize()
        print(f"  Brain {i+1}: {brain.getTotalNeuronCount()} neurons")
    
    # Run parallel simulation
    print("\nRunning parallel simulation...")
    start_time = time.time()
    
    for step in range(50):
        for brain in brains:
            brain.step(step)
    
    end_time = time.time()
    
    print(f"Batch simulation completed in {end_time - start_time:.2f} seconds")
    
    # Show results
    total_spikes = sum(brain.getTotalSpikeCount() for brain in brains)
    avg_firing = sum(brain.getAverageFiringRate() for brain in brains) / len(brains)
    
    print(f"Total spikes across all brains: {total_spikes}")
    print(f"Average firing rate: {avg_firing:.2f} Hz")
    print()

# Example 8: Configuration Management

def configuration_example():
    """
    Example 8: Configuration Management
    
    This example demonstrates NLM's advanced configuration system,
    showing how to create custom configurations and presets.
    """
    print("Example 8: Configuration Management")
    print("-" * 40)
    
    # Create custom configuration
    custom_config = pynlm.createConfig({
        'brain': {
            'neuron_count': 8000,
            'synapse_density': 0.03,
            'connection_probability': 0.02,
        },
        'neuromodulation': {
            'curiosity': {'enable': True, 'threshold': 0.3},
            'novelty': {'enable': True, 'decay_rate': 0.9},
            'dopamine': {'scale': 1.8},
        },
        'development': {
            'enable_development': True,
            'stage_duration': 150.0,
        },
        'plasticity': {
            'structural': {'enable': True, 'synaptogenesis_rate': 0.0001},
        },
    })
    
    print("Created custom configuration:")
    print(f"  - Neuron count: {custom_config.get('brain', 'neuron_count')}")
    print(f"  - Synapse density: {custom_config.get('brain', 'synapse_density')}")
    print(f"  - Development enabled: {custom_config.get('development', 'enable_development')}")
    print(f"  - Structural plasticity: {custom_config.get('plasticity', 'structural', {}).get('enable', False)}")
    
    # Load and validate preset
    print("\nLoading researcher preset...")
    preset_config = pynlm.createConfig()
    preset_config.load_preset('researcher')
    
    print(f"Preset configuration:")
    print(f"  - Neuron count: {preset_config.get('brain', 'neuron_count')}")
    print(f"  - Development enabled: {preset_config.get('development', 'enable_development')}")
    
    # Configuration validation
    print(f"\nConfiguration validation: {'✓ Valid' if custom_config.validate() else '✗ Invalid'}")
    print()

# Example 9: Event-Driven Programming

def event_driven_example():
    """
    Example 9: Event-Driven Programming
    
    This example demonstrates NLM's event-driven programming capabilities,
    where you can register callbacks for specific neural events.
    """
    print("Example 9: Event-Driven Programming")
    print("-" * 40)
    
    # Create brain for event-driven example
    brain = pynlm.createBrain()
    brain.initialize()
    
    print("Event-driven programming setup:")
    print("  - Spike callbacks: Register for neural firing events")
    print("  - Learning callbacks: Register for synaptic changes")
    print("  - Development callbacks: Register for developmental stage changes")
    
    # Note: In a real implementation, you would register actual callbacks
    # For this example, we'll just demonstrate the concept
    
    spike_count = 0
    for step in range(20):
        brain.step(step)
        spike_count += brain.getFiringNeuronCount()
        
        if step % 5 == 0:
            print(f"  Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    print(f"Total spikes recorded: {spike_count}")
    print("✓ Event-driven simulation completed")
    print()

# Example 10: Performance Monitoring

def performance_monitoring_example():
    """
    Example 10: Performance Monitoring
    
    This example demonstrates NLM's performance monitoring capabilities,
    showing how to track and analyze brain performance metrics.
    """
    print("Example 10: Performance Monitoring")
    print("-" * 40)
    
    brain = pynlm.createBrain()
    brain.initialize()
    
    print("Performance monitoring enabled")
    print("Collecting metrics...")
    
    # Run simulation and collect metrics
    metrics_history = []
    
    for step in range(30):
        brain.step(step)
        
        # Get current metrics
        metrics = brain.getPerformanceMetrics()
        metrics_history.append(metrics)
        
        if step % 10 == 0:
            print(f"  Step {step}:")
            print(f"    Firing rate: {metrics['averageFiringRate']:.2f} Hz")
            print(f"    Total spikes: {metrics['totalSpikes']}")
            print(f"    Memory traces: {metrics['workingMemoryTraces']}")
            print(f"    Curiosity level: {metrics['curiosityLevel']:.3f}")
    
    # Analyze performance
    avg_firing = sum(m['averageFiringRate'] for m in metrics_history) / len(metrics_history)
    total_spikes = sum(m['totalSpikes'] for m in metrics_history)
    max_curiosity = max(m['curiosityLevel'] for m in metrics_history)
    
    print(f"\nPerformance summary:")
    print(f"  - Average firing rate: {avg_firing:.2f} Hz")
    print(f"  - Total spikes: {total_spikes}")
    print(f"  - Maximum curiosity: {max_curiosity:.3f}")
    print(f"  - Simulation efficiency: {avg_firing/total_spikes*100:.1f} spikes per Hz")
    print()

# Main function to run all examples
def run_all_examples():
    """Run all example scripts."""
    print("=" * 60)
    print("NLM Enhanced Python API - Examples")
    print("=" * 60)
    print()
    
    examples = [
        ("Basic Brain Simulation", basic_brain_example),
        ("Agent in World", agent_world_example),
        ("Memory Systems", memory_system_example),
        ("Development Process", development_example),
        ("Prediction System", prediction_example),
        ("Complete Agent", complete_agent_example),
        ("Batch Processing", batch_processing_example),
        ("Configuration Management", configuration_example),
        ("Event-Driven Programming", event_driven_example),
        ("Performance Monitoring", performance_monitoring_example),
    ]
    
    for name, example_func in examples:
        print(f"Running: {name}")
        try:
            example_func()
        except Exception as e:
            print(f"Error in {name}: {e}")
            import traceback
            traceback.print_exc()
        
        print()
    
    print("=" * 60)
    print("All examples completed!")
    print("=" * 60)

if __name__ == "__main__":
    run_all_examples()