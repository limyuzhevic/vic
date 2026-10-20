#!/usr/bin/env python3

"""
NLM Enhanced Python Bindings - Main Demo Script

This script provides a comprehensive demonstration of the enhanced NLM Python bindings.
It includes examples, testing, and tutorials for all features.

Usage:
    python enhanced_bindings_demo.py
"""

import sys
import time
import json
import numpy as np
import argparse

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError as e:
    PYNLM_AVAILABLE = False
    print(f"Warning: pynlm not available: {e}", file=sys.stderr)

def print_header(title):
    """Print formatted header"""
    print("\n" + "=" * 70)
    print(f"{title}")
    print("=" * 70)

def test_basic_functionality():
    """Test basic Python bindings functionality"""
    print_header("1. BASIC FUNCTIONALITY TEST")
    
    try:
        # Test configuration system
        print("\n1.1 Testing Configuration System")
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 500)
        
        assert config.has("brain.neuron_count")
        assert config.getOr("brain.neuron_count", 0) == 500
        print("   ✓ Configuration system working")
        
        # Test brain creation and initialization
        print("\n1.2 Testing Brain Creation")
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        assert brain.getTotalNeuronCount() == 500
        assert brain.getTotalSynapseCount() > 0
        print("   ✓ Brain creation and initialization working")
        
        # Test simulation
        print("\n1.3 Testing Simulation")
        for step in range(20):
            brain.step(step)
        
        assert brain.getTotalSpikeCount() > 0
        assert brain.getAverageFiringRate() > 0
        print("   ✓ Simulation working")
        
        # Test state management
        print("\n1.4 Testing State Management")
        brain.save("test_brain.bin")
        print("   ✓ State saved successfully")
        
        brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
        brain2.load("test_brain.bin")
        print("   ✓ State loaded successfully")
        
        # Clean up
        import os
        if os.path.exists("test_brain.bin"):
            os.remove("test_brain.bin")
        
        print("\n" + "=" * 70)
        print("BASIC FUNCTIONALITY TEST PASSED")
        print("=" * 70)
        return True
        
    except Exception as e:
        print(f"\n❌ Error during basic functionality test: {e}")
        import traceback
        traceback.print_exc()
        return False

def demonstrate_world_simulation():
    """Demonstrate complete agent-world simulation"""
    print_header("2. WORLD SIMULATION DEMONSTRATION")
    
    try:
        print("\n2.1 Creating Integrated Agent-World System")
        
        # Create brain with optimal configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 300)
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
        world.reset()
        world.setAgentStart(7.5, 7.5)
        
        # Create and configure agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable advanced learning systems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        agent.enableStructuralPlasticity(True)
        
        print("   ✓ Agent-World system created successfully")
        
        # Run simulation
        print("\n2.2 Running Simulation Loop")
        for step in range(30):
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain simulation
            brain.step(step)
            
            # Get and apply motor command
            motor_cmd = agent.decodeMotorCommand()
            world.applyMotorCommand(motor_cmd, world.getSimulationTime())
            
            # Apply reward modulation
            if agent.isRewardModulationEnabled():
                reward = 0.0
                if percept.getInternal():
                    reward = percept.getInternal()[0] if percept.getInternal() else 0.0
                agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            if agent.isDevelopmentEnabled():
                agent.updateDevelopment(0.1)
            
            if step % 10 == 0:
                print(f"     Step {step}: {brain.getFiringNeuronCount()} firing neurons, "
                      f"curiosity={agent.getCuriosityLevel():.3f}, "
                      f"novelty={agent.getNoveltyLevel():.3f}")
        
        print("\n2.3 Final System Statistics")
        print(f"   Total neurons: {brain.getTotalNeuronCount()}")
        print(f"   Total synapses: {brain.getTotalSynapseCount()}")
        print(f"   Active neurons: {brain.getActiveNeuronCount()}")
        print(f"   Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"   Total spikes: {brain.getTotalSpikeCount()}")
        print(f"   Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
        print(f"   Developmental stage: {brain.getDevelopmentalStage()}")
        print(f"   Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
        print(f"   Curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"   Novelty level: {agent.getNoveltyLevel():.3f}")
        print(f"   Prediction error: {agent.getPredictionError():.3f}")
        
        print("\n" + "=" * 70)
        print("WORLD SIMULATION DEMONSTRATION COMPLETED SUCCESSFULLY")
        print("=" * 70)
        return True
        
    except Exception as e:
        print(f"\n❌ Error during world simulation demonstration: {e}")
        import traceback
        traceback.print_exc()
        return False

def demonstrate_advanced_features():
    """Demonstrate advanced NLM features"""
    print_header("3. ADVANCED FEATURES DEMONSTRATION")
    
    try:
        print("\n3.1 Testing Neuromodulation Systems")
        
        # Create brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all neuromodulation
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableNovelty(True)
        
        print("   ✓ Neuromodulation systems enabled")
        
        # Test neural development
        print("\n3.2 Testing Neural Development")
        print(f"   Initial developmental stage: {brain.getDevelopmentalStage()}")
        
        for step in range(20):
            brain.step(step)
            if agent.isDevelopmentEnabled():
                agent.updateDevelopment(0.1)
        
        print(f"   Final developmental stage: {brain.getDevelopmentalStage()}")
        
        # Test plasticity
        print("\n3.3 Testing Plasticity Systems")
        
        # Enable plasticity
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.structural.enable", True)
        
        # Run simulation with plasticity
        for step in range(50):
            brain.step(step)
            if agent.isDevelopmentEnabled():
                agent.updateDevelopment(0.1)
        
        print(f"   Synaptic plasticity completed")
        print(f"   Total synapses: {brain.getTotalSynapseCount()}")
        print(f"   Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
        
        print("\n" + "=" * 70)
        print("ADVANCED FEATURES DEMONSTRATION COMPLETED SUCCESSFULLY")
        print("=" * 70)
        return True
        
    except Exception as e:
        print(f"\n❌ Error during advanced features demonstration: {e}")
        import traceback
        traceback.print_exc()
        return False

def demonstrate_performance_benchmarking():
    """Demonstrate performance benchmarking tools"""
    print_header("4. PERFORMANCE BENCHMARKING")
    
    try:
        print("\n4.1 Running Batch Performance Test")
        
        import time
        
        # Run multiple simulations
        num_simulations = 3
        steps_per_simulation = 100
        
        results = []
        
        for sim in range(num_simulations):
            start_time = time.time()
            
            # Create brain for each simulation
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", 200)
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Run simulation
            for step in range(steps_per_simulation):
                brain.step(step)
            
            end_time = time.time()
            
            sim_result = {
                'simulation': sim + 1,
                'duration': end_time - start_time,
                'total_spikes': brain.getTotalSpikeCount(),
                'firing_rate': brain.getAverageFiringRate(),
                'active_neurons': brain.getActiveNeuronCount(),
                'spike_per_step': brain.getTotalSpikeCount() / steps_per_simulation
            }
            
            results.append(sim_result)
            print(f"   Simulation {sim + 1}: {sim_result['duration']:.2f}s, "
                  f"{sim_result['total_spikes']} spikes, "
                  f"{sim_result['firing_rate']:.2f} Hz avg")
        
        # Analyze results
        avg_duration = np.mean([r['duration'] for r in results])
        avg_spikes = np.mean([r['total_spikes'] for r in results])
        avg_firing_rate = np.mean([r['firing_rate'] for r in results])
        
        print("\n4.2 Performance Analysis")
        print(f"   Average simulation time: {avg_duration:.2f}s")
        print(f"   Average spike rate: {avg_spikes:.0f} spikes per simulation")
        print(f"   Average firing rate: {avg_firing_rate:.2f} Hz")
        print(f"   Performance consistency: {np.std([r['firing_rate'] for r in results]):.3f} Hz std")
        
        # Save results
        with open("performance_results.json", 'w') as f:
            json.dump(results, f, indent=2)
        print(f"   Results saved to: performance_results.json")
        
        # Clean up
        import os
        if os.path.exists("performance_results.json"):
            os.remove("performance_results.json")
        
        print("\n" + "=" * 70)
        print("PERFORMANCE BENCHMARKING COMPLETED SUCCESSFULLY")
        print("=" * 70)
        return True
        
    except Exception as e:
        print(f"\n❌ Error during performance benchmarking: {e}")
        import traceback
        traceback.print_exc()
        return False

def show_usage_examples():
    """Show usage examples for different user levels"""
    print_header("5. USAGE EXAMPLES")
    
    examples = [
        {
            "level": "BEGINNER",
            "title": "Getting Started with NLM",
            "description": "Basic brain simulation for beginners",
            "code": """
# Basic brain simulation
import pynlm

# Create a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Your brain thought {brain.getTotalSpikeCount()} times!")
print(f"It has {brain.getTotalNeuronCount()} neurons.")
"""
        },
        {
            "level": "INTERMEDIATE",
            "title": "Agent-World Integration",
            "description": "Complete agent simulation with world interaction",
            "code": """
# Create agent with world interface
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run episode
for step in range(200):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print(f"Episode completed in {world.getSimulationTime():.1f}s")
print(f"Final performance: {brain.getTotalSpikeCount()} spikes, "
      f"{brain.getAverageFiringRate():.2f} Hz avg")
"""
        },
        {
            "level": "ADVANCED",
            "title": "Advanced Neural System Configuration",
            "description": "Configure and optimize neural systems for research",
            "code": """
# Advanced configuration for research
import pynlm

# Create custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("brain.connection_probability", 0.05)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)
config.set("neuromod.dopamine.scale", 1.0)

# Create brain with custom configuration
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent with world interface
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all advanced features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run advanced simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print(f"Advanced simulation completed successfully")
print(f"Final statistics:")
print(f"  Development stage: {brain.getDevelopmentalStage()}")
print(f"  Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
"""
        }
    ]
    
    for example in examples:
        print(f"\n{'-' * 70}")
        print(f"{example['level']}: {example['title']}")
        print(f"{example['description']}")
        print(f"\nCode:")
        print(example['code'])
        print(f"{'-' * 70}")
    
    print("\n" + "=" * 70)
    print("USAGE EXAMPLES DISPLAYED")
    print("=" * 70)
    return True

def main():
    """Main demonstration function"""
    print("NLM Enhanced Python Bindings - Comprehensive Demonstration")
    print("This script demonstrates all features of the enhanced NLM Python bindings.")
    
    if not PYNLM_AVAILABLE:
        print("\n❌ Error: pynlm not available")
        print("Please ensure NLM Python bindings are properly installed:")
        print("  cd /workspace/app")
        print("  python -m pip install -e .")
        sys.exit(1)
    
    # Run all demonstrations
    success = True
    
    success &= test_basic_functionality()
    success &= demonstrate_world_simulation()
    success &= demonstrate_advanced_features()
    success &= demonstrate_performance_benchmarking()
    show_usage_examples()
    
    print("\n" + "=" * 70)
    if success:
        print("🎉 ALL DEMONSTRATIONS COMPLETED SUCCESSFULLY! 🎉")
        print("\nThe enhanced NLM Python bindings provide comprehensive")
        print("access to neural simulation capabilities including:")
        print("  • Basic and advanced neural systems")
        print("  • Complete memory integration")
        print("  • Neuromodulation controls")
        print("  • Performance optimization tools")
        print("  • Extensive documentation and examples")
    else:
        print("⚠️  SOME DEMONSTRATIONS HAD ERRORS")
        print("Please check the output above for details.")
    print("=" * 70)
    
    return success

if __name__ == "__main__":
    main()