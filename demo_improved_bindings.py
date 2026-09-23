#!/usr/bin/env python3
"""
Demonstration script for improved NLM Python bindings.

This script demonstrates the new features and improvements made to the
NLM Python bindings, showcasing the enhanced functionality.
"""

import sys
import os
import time

print("NLM Python Bindings - Demonstration Script")
print("=" * 60)

# Add the current directory to the path for local testing
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm as nlm
    print("✅ pynlm module loaded successfully")
except ImportError as e:
    print(f"❌ Failed to load pynlm: {e}")
    print("Please ensure the bindings are built and installed.")
    sys.exit(1)

def demonstrate_basic_functionality():
    """Demonstrate basic NLM functionality."""
    print("\n1. Basic Neural Simulation")
    print("-" * 40)
    
    # Create brain with default configuration
    brain = nlm.createBrainWithDefaultConfig()
    print(f"✅ Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"✅ and {brain.getTotalSynapseCount()} synapses")
    
    # Initialize brain
    brain.initialize()
    print("✅ Brain initialized")
    
    # Run simulation steps
    initial_spikes = brain.getTotalSpikeCount()
    for step in range(50):
        brain.step(step, step * 0.001)
    
    final_spikes = brain.getTotalSpikeCount()
    print(f"✅ Simulation complete: {final_spikes - initial_spikes} spikes generated")
    print(f"✅ Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
    
    # Clean up
    brain.reset()
    return True

def demonstrate_advanced_features():
    """Demonstrate advanced NLM features."""
    print("\n2. Advanced Features Demonstration")
    print("-" * 40)
    
    try:
        from pynlm import PerformanceProfiler, ExperimentTracker, NeuralNetworkBuilder
        print("✅ Advanced modules imported successfully")
        
        # Performance profiler demonstration
        print("\n   Performance Profiler:")
        profiler = PerformanceProfiler(name="demo_simulation")
        profiler.start()
        
        brain = nlm.createBrainWithDefaultConfig()
        brain.initialize()
        
        for step in range(100):
            brain.step(step, step * 0.001)
            profiler.recordMetric("spike_count", brain.getTotalSpikeCount())
        
        profiler.stop()
        
        metrics = profiler.getMetrics()
        print(f"   📊 Average FPS: {metrics['average_fps']:.2f}")
        print(f"   📊 Memory usage: {metrics['memory_usage_mb']:.2f} MB")
        
        # Experiment tracker demonstration
        print("\n   Experiment Tracker:")
        experiment = ExperimentTracker(name="demo_experiment")
        experiment.start()
        
        for step in range(50):
            brain.step(step, step * 0.001)
            experiment.recordStepData({
                "step": step,
                "spike_count": brain.getTotalSpikeCount(),
                "firing_rate": brain.getAverageFiringRate()
            })
        
        results = experiment.end()
        print(f"   📈 Experiment duration: {results['duration_seconds']:.2f}s")
        print(f"   📈 Total steps: {results['total_steps']}")
        
        # Neural network builder demonstration
        print("\n   Neural Network Builder:")
        builder = NeuralNetworkBuilder()
        
        autoencoder = builder.createAutoencoder(input_size=5, hidden_size=3)
        print("   🧠 Created autoencoder network (5 -> 3 -> 5)")
        
        rnn = builder.createRNN(input_size=4, hidden_size=8, output_size=2)
        print("   🧠 Created RNN network (4 -> 8 -> 2)")
        
        return True
        
    except ImportError as e:
        print(f"   ⚠️  Advanced features not available: {e}")
        return False

def demonstrate_context_managers():
    """Demonstrate context manager functionality."""
    print("\n3. Context Managers")
    print("-" * 40)
    
    try:
        # Brain context manager demonstration
        print("\n   Brain Context Manager:")
        with nlm.BrainContextManager(nlm.createBrainWithDefaultConfig()) as brain:
            print(f"   ✅ Brain created with {brain.getTotalNeuronCount()} neurons")
            
            for step in range(20):
                brain.step(step, step * 0.001)
            
            print(f"   ✅ Brain auto-reset on exit")
        
        # World context manager demonstration
        print("\n   World Context Manager:")
        with nlm.WorldContextManager(nlm.createWorldWithSetup(width=100, height=100)) as world:
            print(f"   ✅ World created: {world.getWidth()}x{world.getHeight()}")
            
            agent = nlm.createCompleteAgentSetup(world)
            print(f"   ✅ Agent created with {agent.getBrain().getTotalNeuronCount()} neurons")
        
        return True
        
    except ImportError as e:
        print(f"   ⚠️  Context managers not available: {e}")
        return False

def demonstrate_event_system():
    """Demonstrate event system functionality."""
    print("\n4. Event System")
    print("-" * 40)
    
    try:
        from pynlm import EventQueue
        
        print("\n   Event Queue:")
        event_queue = EventQueue()
        
        # Queue some events
        event_queue.queueSpike(nlm.NeuronId(1), 1000.0)
        event_queue.queueSpike(nlm.NeuronId(2), 1001.0)
        event_queue.queueDelayedSpike(nlm.NeuronId(3), 1002.0, 2)
        
        print("   📨 Queued 2 immediate spikes and 1 delayed spike")
        
        # Process events
        current_time = 1000.0
        event_queue.processSpikes(current_time)
        event_queue.processDelayedSpikes()
        
        # Check results
        history = event_queue.getEventHistory()
        print(f"   📋 Processed {len(history)} events")
        
        # Event statistics
        stats = event_queue.getEventStatistics()
        print(f"   📊 Event statistics: {stats}")
        
        return True
        
    except ImportError as e:
        print(f"   ⚠️  Event system not available: {e}")
        return False

def demonstrate_visualization():
    """Demonstrate visualization functionality."""
    print("\n5. Visualization")
    print("-" * 40)
    
    try:
        from pynlm import VisualizationInterface
        
        print("\n   Visualization Interface:")
        vis = VisualizationInterface()
        vis.initialize()
        
        # Create and initialize brain
        brain = nlm.createBrainWithDefaultConfig()
        brain.initialize()
        
        # Run some simulation
        for step in range(30):
            brain.step(step, step * 0.001)
        
        # Generate visualizations
        print("   📊 Generating visualizations...")
        
        network_viz = vis.visualizeNetwork(brain)
        print("   📈 Network architecture visualization created")
        
        activity_viz = vis.visualizeActivity(brain)
        print("   🌊 Activity monitoring visualization created")
        
        spike_viz = vis.visualizeSpikes(brain)
        print("   ⚡ Spike raster visualization created")
        
        weight_viz = vis.visualizeWeights(brain)
        print("   🧮 Weight matrix visualization created")
        
        vis.close()
        print("   ✅ All visualizations completed")
        
        return True
        
    except ImportError as e:
        print(f"   ⚠️  Visualization not available: {e}")
        return False

def demonstrate_memory_systems():
    """Demonstrate memory system functionality."""
    print("\n6. Memory Systems")
    print("-" * 40)
    
    print("\n   Working Memory:")
    working_memory = nlm.NeuralWorkingMemory()
    working_memory.store("position", [10.0, 20.0])
    working_memory.store("temperature", 25.5)
    working_memory.store("score", 1500)
    
    print("   💾 Stored: position, temperature, score")
    
    position = working_memory.recall("position")
    temperature = working_memory.recall("temperature")
    score = working_memory.recall("score")
    
    print(f"   📖 Retrieved position: {position}")
    print(f"   📖 Retrieved temperature: {temperature}°C")
    print(f"   📖 Retrieved score: {score}")
    
    print(f"   📋 Available keys: {working_memory.getKeys()}")
    
    print("\n   Episodic Memory:")
    episodic_memory = nlm.NeuralEpisodicMemory()
    
    percept = nlm.SensoryPercept()
    action = nlm.Action(nlm.ActionType.MoveForward)
    episodic_memory.storeExperience(1000.0, percept, action, 0.5)
    
    experiences = episodic_memory.getRecentExperiences(5)
    print(f"   📅 Stored {len(experiences)} experiences")
    
    print("\n   Associative Memory:")
    assoc_memory = nlm.NeuralAssociativeMemory()
    assoc_memory.associate([1.0, 0.0], [0.0, 1.0], 0.8)
    
    recall = assoc_memory.recallPattern([1.0, 0.0])
    print(f"   🔗 Pattern association created")
    print(f"   🔗 Recall strength: {assoc_memory.getAssociationStrength([1.0, 0.0], [0.0, 1.0]):.3f}")
    
    return True

def demonstrate_neuromodulation():
    """Demonstrate neuromodulation system."""
    print("\n7. Neuromodulation System")
    print("-" * 40)
    
    neuromodulators = {
        "Dopamine": nlm.Dopamine(),
        "Acetylcholine": nlm.Acetylcholine(),
        "Norepinephrine": nlm.Norepinephrine(),
        "Serotonin": nlm.Serotonin(),
        "Curiosity": nlm.Curiosity(),
        "Novelty": nlm.Novelty(),
        "PredictionError": nlm.PredictionError()
    }
    
    print("\n   Neuromodulator Levels:")
    for name, nm in neuromodulators.items():
        if hasattr(nm, 'getLevel'):
            level = nm.getLevel()
        elif hasattr(nm, 'getCuriosityLevel'):
            level = nm.getCuriosityLevel()
        elif hasattr(nm, 'getPredictionErrorLevel'):
            level = nm.getPredictionErrorLevel()
        else:
            level = 0.0
            
        print(f"   🧠 {name:15s}: {level:.3f}")
    
    # Modify some levels
    neuromodulators["Dopamine"].setLevel(0.8)
    neuromodulators["Curiosity"].setCuriosityLevel(0.6)
    neuromodulators["Novelty"].setLevel(0.4)
    neuromodulators["PredictionError"].setPredictionErrorLevel(0.3)
    
    print("\n   After modification:")
    print(f"   🧠 Dopamine level: {neuromodulators['Dopamine'].getLevel():.3f}")
    print(f"   🧠 Curiosity level: {neuromodulators['Curiosity'].getCuriosityLevel():.3f}")
    print(f"   🧠 Novelty level: {neuromodulators['Novelty'].getLevel():.3f}")
    
    return True

def demonstrate_complete_workflow():
    """Demonstrate a complete workflow with all features."""
    print("\n8. Complete Workflow Demonstration")
    print("-" * 40)
    
    print("\n   Setting up complete agent with world...")
    world = nlm.createWorldWithSetup(width=200, height=200)
    
    print("\n   Creating complete agent setup...")
    agent = nlm.createCompleteAgentSetup(world)
    
    print("\n   Creating sensory processing pipeline...")
    sensory_agent = nlm.createSensoryPipeline(world)
    
    print("\n   Running simulation...")
    for episode in range(10):
        agent.reset()
        world.reset()
        
        for step in range(50):
            # Get sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Get action
            motor_command = agent.decodeMotorCommand()
            action = nlm.Action(motor_command, [])
            
            # Apply action
            result = world.applyMotorCommand(action.getType(), step * 0.001)
            
            # Apply neuromodulation
            if result:
                agent.applyRewardModulation(result.reward, agent.getPredictionError())
    
    print(f"\n   ✅ Simulation completed!")
    print(f"   🧠 Final brain state: {agent.getBrain().getTotalNeuronCount()} neurons")
    print(f"   🧠 Final active neurons: {agent.getBrain().getActiveNeuronCount()}")
    print(f"   🧠 Final firing rate: {agent.getBrain().getAverageFiringRate():.2f} Hz")
    
    return True

def main():
    """Run all demonstrations."""
    print("\nStarting NLM Python Bindings Demonstration")
    print("=" * 60)
    
    demonstrations = [
        ("Basic Functionality", demonstrate_basic_functionality),
        ("Advanced Features", demonstrate_advanced_features),
        ("Context Managers", demonstrate_context_managers),
        ("Event System", demonstrate_event_system),
        ("Visualization", demonstrate_visualization),
        ("Memory Systems", demonstrate_memory_systems),
        ("Neuromodulation", demonstrate_neuromodulation),
        ("Complete Workflow", demonstrate_complete_workflow),
    ]
    
    results = []
    
    for name, demo_func in demonstrations:
        try:
            result = demo_func()
            results.append((name, result))
        except Exception as e:
            print(f"❌ Error in {name}: {e}")
            results.append((name, False))
    
    # Summary
    print("\n" + "=" * 60)
    print("DEMONSTRATION SUMMARY")
    print("=" * 60)
    
    successful = 0
    for name, result in results:
        status = "✅ PASS" if result else "❌ FAIL"
        print(f"{name:25s} {status}")
        if result:
            successful += 1
    
    print(f"\nTotal: {successful}/{len(results)} demonstrations successful")
    
    if successful == len(results):
        print("\n🎉 All demonstrations completed successfully!")
        print("The NLM Python bindings are working perfectly!")
    else:
        print(f"\n⚠️  {len(results) - successful} demonstrations failed")
        print("Some features may not be available or may have issues.")
    
    return successful == len(results)

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)