#!/usr/bin/env python3
"""
NLM Improved Demo Script

This script demonstrates the new features and improvements in the NLM project.
"""

import pynlm
import json
import time
import sys

def demo_basic_usage():
    """Demonstrate basic NLM usage with new features."""
    print("=== NLM Basic Usage Demo ===\n")
    
    # Create brain with enhanced configuration
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Initial developmental stage: {brain.getDevelopmentalStage()}")
    print()
    
    # Initialize the brain
    brain.initialize()
    print("Brain initialized successfully!")
    
    # Run a simple learning episode
    print("\nRunning learning episode...")
    for step in range(100):
        brain.step(step)
        
        # Inject some sensory input every 20 steps
        if step % 20 == 0:
            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 10.0f)
    
    # Get comprehensive statistics
    stats = brain.getNeuralStats()
    print(f"\n=== Simulation Results ===")
    print(f"Total spikes: {stats['total_experience']}")
    print(f"Active neurons: {stats['active_neurons_count']}")
    print(f"Average firing rate: {stats['learning_intensity']:.2f} Hz")
    print(f"Excitatory/Inhibitory ratio: {stats['network_balance']:.2f}")
    print(f"Network density: {stats['network_density']:.4f}")
    print(f"Developmental phase: {stats['developmental_phase']}")
    
    return brain

def demo_learning_analysis(brain):
    """Demonstrate advanced learning analysis features."""
    print("\n=== Learning Analysis Demo ===\n")
    
    # Run learning episode with different inputs
    test_cases = [
        ([0.1, 0.2, 0.3], "Low input"),
        ([0.5, 0.7, 0.9], "Medium input"),
        ([0.8, 0.9, 1.0], "High input"),
    ]
    
    learning_results = []
    
    for inputs, description in test_cases:
        print(f"Running {description}...")
        
        # Reset brain for each test
        brain.reset()
        brain.initialize()
        
        # Run learning episode
        for step in range(100):
            brain.step(step)
            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, inputs[step % len(inputs)] * 30.0f)
        
        # Collect statistics
        stats = brain.getNeuralStats()
        result = {
            'description': description,
            'inputs': inputs,
            'total_spikes': stats['total_experience'],
            'firing_rate': stats['learning_intensity'],
            'network_balance': stats['network_balance'],
            'connectivity': stats['network_density']
        }
        learning_results.append(result)
        
        print(f"  Total spikes: {result['total_spikes']}")
        print(f"  Firing rate: {result['firing_rate']:.2f} Hz")
        print(f"  Network balance: {result['network_balance']:.2f}")
    
    # Analyze learning patterns
    print(f"\n=== Learning Analysis ===")
    print("Pattern analysis:")
    
    spike_rates = [r['firing_rate'] for r in learning_results]
    balances = [r['network_balance'] for r in learning_results]
    connectivities = [r['connectivity'] for r in learning_results]
    
    avg_spike_rate = sum(spike_rates) / len(spike_rates)
    avg_balance = sum(balances) / len(balances)
    avg_connectivity = sum(connectivities) / len(connectivities)
    
    print(f"Average spike rate: {avg_spike_rate:.2f} Hz")
    print(f"Average network balance: {avg_balance:.2f}")
    print(f"Average network connectivity: {avg_connectivity:.4f}")
    
    # Detect learning patterns
    increasing_spikes = sum(1 for i in range(1, len(spike_rates)) if spike_rates[i] > spike_rates[i-1])
    if increasing_spikes > len(spike_rates) / 2:
        print("Learning pattern: Increasing activity over time (positive learning)")
    else:
        print("Learning pattern: Variable activity (exploration phase)")
    
    return learning_results

def demo_advanced_features(brain):
    """Demonstrate advanced Python features."""
    print("\n=== Advanced Features Demo ===\n")
    
    # Enable all plasticity mechanisms
    print("Enabling all plasticity mechanisms...")
    brain.enableAllPlasticity()
    print("Plasticity enabled!")
    
    # Run simulation with enhanced plasticity
    print("Running simulation with full plasticity...")
    for step in range(200):
        brain.step(step)
        
        # Inject varying input to trigger different plasticity rules
        if step % 50 == 0:
            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 20.0f)
    
    # Get advanced statistics
    stats = brain.getNeuralStats()
    print(f"\n=== Advanced Statistics ===")
    print(f"Developmental stage: {stats['developmental_phase']}")
    print(f"Total synaptic modifications: {stats['total_experience']}")
    print(f"Network stability: {stats['network_balance']:.2f}")
    
    # Demonstrate export capabilities
    print(f"\nExporting brain state...")
    try:
        brain.exportForAnalysis("demo_brain_state.json")
        print("Brain state exported to demo_brain_state.json")
        
        # Read back and analyze export
        with open("demo_brain_state.json", "r") as f:
            export_data = json.load(f)
        
        print(f"Export contains {export_data.get('Regions', 0)} regions")
        print(f"Export contains {export_data.get('Total Neurons', 0)} neurons")
        print(f"Export contains {export_data.get('Total Spikes', 0)} spikes")
        
    except Exception as e:
        print(f"Export error (expected if dependencies missing): {e}")
    
    # Demonstrate connectivity analysis
    print(f"\n=== Connectivity Analysis ===")
    connectivity = brain.getConnectivityMatrix()
    
    if connectivity:
        print(f"Connectivity matrix: {len(connectivity)} x {len(connectivity[0])}")
        
        # Calculate network metrics
        total_weight = 0.0
        active_connections = 0
        
        for row in connectivity:
            for weight in row:
                if weight > 0:
                    active_connections += 1
                total_weight += weight
        
        avg_weight = total_weight / (len(connectivity) * len(connectivity[0]) if connectivity[0] else 1)
        connection_density = active_connections / (len(connectivity) * len(connectivity[0]) if connectivity[0] else 1)
        
        print(f"Average synaptic weight: {avg_weight:.4f}")
        print(f"Connection density: {connection_density:.4f}")
        print(f"Active connections: {active_connections}")
        
        # Determine network type
        if connection_density > 0.1:
            network_type = "Dense (highly connected)"
        elif connection_density > 0.01:
            network_type = "Moderate (normally connected)"
        else:
            network_type = "Sparse (minimally connected)"
        
        print(f"Network type: {network_type}")
    
    return brain

def demo_simulation_scenarios():
    """Demonstrate different simulation scenarios."""
    print("\n=== Simulation Scenarios Demo ===\n")
    
    scenarios = [
        {
            "name": "Pattern Recognition",
            "neuron_count": 500,
            "steps": 500,
            "description": "Test pattern recognition with varying inputs"
        },
        {
            "name": "Memory Consolidation",
            "neuron_count": 300,
            "steps": 1000,
            "description": "Test memory formation and consolidation"
        },
        {
            "name": "Adaptive Control",
            "neuron_count": 200,
            "steps": 300,
            "description": "Test adaptive control and feedback loops"
        }
    ]
    
    for scenario in scenarios:
        print(f"\n{scenario['name']} Scenario:")
        print(f"  Description: {scenario['description']}")
        print(f"  Configuration: {scenario['neuron_count']} neurons, {scenario['steps']} steps")
        
        # Create brain for this scenario
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run scenario simulation
        start_time = time.time()
        
        for step in range(scenario['steps']):
            # Simulate different patterns based on scenario
            if "Pattern" in scenario['name']:
                # Pattern recognition: alternating inputs
                input_val = 10.0f if (step % 3 == 0) else 0.0f
                brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, input_val)
            elif "Memory" in scenario['name']:
                # Memory consolidation: increasing input
                input_val = 5.0f + (step % 50) * 0.1f
                brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, input_val)
            else:
                # Adaptive control: random-like input
                input_val = 10.0f if hash(str(step)) % 2 == 0 else 0.0f
                brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, input_val)
            
            brain.step(step)
        
        elapsed = time.time() - start_time
        
        # Collect results
        stats = brain.getNeuralStats()
        print(f"  Completed in {elapsed:.2f} seconds")
        print(f"  Total spikes: {stats['total_experience']}")
        print(f"  Average firing rate: {stats['learning_intensity']:.2f} Hz")
        print(f"  Steps per second: {scenario['steps'] / elapsed:.0f}")
    
    print(f"\nAll scenarios completed successfully!")

def demo_python_integration():
    """Demonstrate Python integration features."""
    print("\n=== Python Integration Demo ===\n")
    
    # Create world and agent
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    # Configure world
    world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
    world.reset()
    agent.initialize(world)
    
    # Enable advanced agent features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("Created world and agent with advanced features enabled")
    print(f"Agent sensory input size: {agent.getSensoryInputSize()}")
    print(f"Agent motor output size: {agent.getMotorOutputSize()}")
    
    # Run agent simulation
    print("\nRunning agent simulation...")
    
    for step in range(100):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action from agent
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = 1.0 if (world.getAgentBody().x % 5 == 0) else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        if step % 20 == 0:
            print(f"Step {step}: Position ({world.getAgentBody().x}, {world.getAgentBody().y}), Reward: {reward}")
    
    # Get agent statistics
    print(f"\n=== Agent Statistics ===")
    print(f"Agent curiosity level: {agent.getCuriosityLevel():.2f}")
    print(f"Agent novelty level: {agent.getNoveltyLevel():.2f}")
    print(f"Agent prediction error: {agent.getPredictionError():.2f}")
    print(f"Agent neuromodulation level: {agent.getNeuromodulationLevel():.2f}")
    print(f"Agent developmental stage: {agent.getDevelopmentalStage()}")
    
    # Get brain statistics
    print(f"\n=== Brain Statistics ===")
    brain_stats = brain.getNeuralStats()
    for key, value in brain_stats.items():
        if isinstance(value, float):
            print(f"{key}: {value:.2f}")
        else:
            print(f"{key}: {value}")
    
    return brain, world, agent

def main():
    """Main demo function."""
    print("NLM (Neural Learning Machine) - Improved Demo")
    print("=" * 50)
    print("\nThis demo showcases the enhanced features of NLM:")
    print("• Advanced Python bindings with new convenience methods")
    print("• Comprehensive CLI argument parsing")
    print("• Enhanced error handling and validation")
    print("• Network analysis and export capabilities")
    print("• Improved simulation control and monitoring")
    
    try:
        # Run all demos
        brain = demo_basic_usage()
        learning_results = demo_learning_analysis(brain)
        brain = demo_advanced_features(brain)
        demo_simulation_scenarios()
        brain, world, agent = demo_python_integration()
        
        print(f"\n" + "=" * 50)
        print("All demos completed successfully!")
        print("\nKey improvements demonstrated:")
        print("✓ Enhanced Python API with advanced methods")
        print("✓ Comprehensive network analysis tools")
        print("✓ Improved simulation control and monitoring")
        print("✓ Better integration with Python ecosystem")
        print("✓ Enhanced error handling and validation")
        
        return 0
        
    except Exception as e:
        print(f"\nError during demo: {e}")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    sys.exit(main())
