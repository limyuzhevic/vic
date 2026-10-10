"""Basic NLM Demo - Simple Neural Network Simulation

This example demonstrates a basic neural network simulation using the NLM Python API.
It covers fundamental concepts of NLM including brain creation, world setup, and agent behavior.
"""
import nlm
import time
import matplotlib.pyplot as plt

def run_basic_demo():
    """Run basic NLM demonstration."""
    print("=== Basic NLM Demo ===")
    print("This example demonstrates a simple neural network simulation.")
    print()
    
    # Setup
    print("1. Setting up simulation...")
    config = nlm.create_default_config()
    world = nlm.create_simple_world()
    world.configure(20, 20, 64, 64)
    world.setAgentStart(10, 10)
    
    brain = nlm.create_brain(config)
    brain.initialize(world)
    
    agent_brain = nlm.create_agent_brain(brain)
    agent_brain.initialize(brain, world)
    
    print(f"   - World size: {world.width}x{world.height}")
    print(f"   - Agent start: ({world.getAgentStart()[0]}, {world.getAgentStart()[1]})")
    print(f"   - Initial neurons: {brain.getStatistics()['total_neurons']}")
    print()
    
    # Run simulation
    print("2. Running simulation...")
    start_time = time.time()
    
    # Track metrics for analysis
    neuron_activities = []
    agent_energies = []
    simulation_times = []
    
    for step in range(100):
        step_start = time.time()
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent_brain.processSensoryInput(percept)
        
        # Generate action
        action = agent_brain.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, step)
        world.update(0.001)
        
        # Collect metrics
        neurons_active = brain.getActiveNeuronCount()
        body = world.getAgentBody()
        
        neuron_activities.append(neurons_active)
        agent_energies.append(body.energy)
        
        step_time = time.time() - step_start
        simulation_times.append(step_time)
        
        if step % 20 ==  stunned:
            print(f"   - Step {step}: {neurons_active} active neurons, Energy: {body.energy:.2f}")
    
    end_time = time.time()
    total_time = end_time - start_time
    
    print(f"   - Total simulation time: {total_time:.2f} seconds")
    print(f"   - Average step time: {sum(simulation_times)/len(simulation_times):.6f} seconds")
    print(f"   - Steps per second: {len(simulation_times)/total_time:.2f}")
    print()
    
    # Generate visualizations
    print("3. Generating visualizations...")
    
    # Create neuron activity plot
    fig, axes = plt.subplots(2, 2, figsize=(12, 10))
    
    # Neuron activities over time
    axes[0, 0].plot(neuron_activities)
    axes[0, 0].set_title('Neuron Activities Over Time')
    axes[0, 0].set_xlabel('Step')
    axes[0, 0].set_ylabel('Active Neurons')
    axes[0, 0].grid(True, alpha=0.3)
    
    # Agent energy over time
    axes[0, 1].plot(agent_energies)
    axes[0, 1].set_title('Agent Energy Over Time')
    axes[0, 1].set_xlabel('Step')
    axes[0, 1].set_ylabel('Energy')
    axes[0, 1].grid(True, alpha=0.3)
    
    # Step times over time
    axes[1, 0].plot(simulation_times)
    axes[1, 0].set_title('Step Times Over Time')
    axes[1, 0].set_xlabel('Step')
    axes[1, 0].set_ylabel('Time (s)')
    axes[1, 0].grid(True, alpha=0.3)
    
    # Neuron activity histogram
    axes[1, 1].hist(neuron_activities, bins=20, alpha=0.7, edgecolor='black')
    axes[1, 1].set_title('Neuron Activity Distribution')
    axes[1, 1].set_xlabel('Active Neurons')
    axes[1, 1].set_ylabel('Frequency')
    axes[1, 1].grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('basic_demo_results.png', dpi=150)
    plt.show()
    
    print("   - Visualization saved as 'basic_demo_results.png'")
    print()
    
    # Performance analysis
    print("4. Performance Analysis:")
    print(f"   - Average active neurons: {sum(neuron_activities)/len(neuron_activities):.1f}")
    print(f"   - Peak neuron activity: {max(neuron_activities)}")
    print(f"   - Minimum neuron activity: {min(neuron_activities)}")
    print(f"   - Energy variation: {max(agent_energies) - min(agent_energies):.2f}")
    print()
    
    # Brain statistics
    stats = brain.getStatistics()
    print("5. Brain Statistics:")
    print(f"   - Total neurons: {stats['total_neurons']}")
    print(f"   - Active neurons: {stats['active_neurons']}")
    print(f"   - Synapses formed: {stats['synapses_formed']}")
    print(f"   - Plasticity events: {stats['plasticity_events']}")
    print(f"   - Development stage: {stats['development_stage']}")
    print()
    
    # Agent state
    body = world.getAgentBody()
    print("6. Agent State:")
    print(f"   - Final position: ({body.x}, {body.y})")
    print(f"   - Final energy: {body.energy:.2f}")
    print(f"   - Final health: {body.health:.2f}")
    print(f"   - Age: {body.age:.2f}")
    print()
    
    # Summary
    print("7. Simulation Summary:")
    print(f"   - Simulation completed successfully with {len(neuron_activities)} steps")
    print(f"   - Performance: {len(neuron_activities)/total_time:.2f} steps/second")
    print(f"   - Neural activity: {sum(neuron_activities)/len(neuron_activities):.1f} avg active neurons")
    print(f"   - Agent survival: Energy {agent_energies[-1]:.2f}, Position ({body.x}, {body.y})")
    print()
    
    return {
        'neuron_activities': neuron_activities,
        'agent_energies': agent_energies,
        'simulation_times': simulation_times,
        'total_time': total_time,
        'brain_stats': stats,
        'agent_state': {
            'position': (body.x, body.y),
            'energy': body.energy,
            'health': body.health,
            'age': body.age
        }
    }

if __name__ == "__main__":
    print("NLM Basic Demo - Simple Neural Network Simulation")
    print("=" * 60)
    print()
    
    results = run_basic_demo()
    
    print("Demo completed successfully!")
    print("\nResults are available in the 'basic_demo_results.png' plot.")
    print("\nFor more advanced examples, see:")
    print("  - examples/expert_demo.py - Advanced expert features")
    print("  - examples/performance_demo.py - Performance optimization")
    print("  - examples/visualization_demo.py - Advanced visualization")
    print("  - examples/research_demo.py - Research applications")
    print("  - examples/production_demo.py - Production deployment")
