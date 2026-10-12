"""
NLM Python Bindings Examples

This module demonstrates how to use the NLM Python bindings for creating
neural learning machines with various configurations and scenarios.
"""

import sys
import time
from typing import Optional

# Import NLM utilities
try:
    import pynlm
    from pynlm.utils import create_example_brain, create_example_world, create_example_agent_brain, show_config
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False


def basic_brain_example():
    """Basic example: Create and initialize a brain with minimal configuration."""
    print("=== Basic Brain Example ===")
    
    if not PYNLM_AVAILABLE:
        print("NLM Python bindings not available")
        return None
    
    # Create a default configuration
    config = pynlm.createDefaultConfig()
    
    # Customize configuration for basic learning
    config.set("neuron_count", 200)
    config.set("region_count", 2)
    config.set("connection_probability", 0.2)
    config.set("random_seed", 123)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Number of regions: {brain.getRegionCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    
    return brain


def sensory_motoriy_example():
    """Example: Create an agent with sensory and motor capabilities."""
    print("\n=== Sensory-Motor Agent Example ===")
    
    if not PYNLM_AVAILABLE:
        print("NLM Python bindings not available")
        return None, None
    
    # Create brain
    brain = create_example_brain(300)
    
    # Create world
    world = create_example_world(30, 30)
    
    # Create agent brain interface
    agent_brain = create_example_agent_brain(brain, world)
    
    print(f"Created agent brain interface for {brain.getTotalNeuronCount()} neuron brain")
    print(f"World size: {world.getWidth()}x{world.getHeight()}")
    print(f"Agent start position: ({world.getAgentBody().x}, {world.getAgentBody().y})")
    
    return brain, world


def learning_simulation_example(brain, world, steps: int = 100):
    """Example: Simulate learning over time with sensory input and actions."""
    print(f"\n=== Learning Simulation Example ({steps} steps) ===")
    
    if not PYNLM_AVAILABLE:
        print("NLM Python bindings not available")
        return
    
    # Get initial state
    initial_spikes = brain.getTotalSpikeCount()
    initial_neurons = brain.getActiveNeuronCount()
    
    print(f"Initial state - Total spikes: {initial_spikes}")
    print(f"Initial state - Active neurons: {initial_neurons}")
    
    # Run learning simulation
    for step in range(steps):
        current_time = step * 0.01
        
        # Create sensory input (random vision in this example)
        vision_data = [0.1] * (5 * 5)  # Simple 5x5 vision
        sensory_input = pynlm.Vision(5, 5, 3)
        sensory_input.setData(vision_data)
        sensory_input.setTimestamp(current_time)
        
        # Inject into agent brain
        agent_brain = pynlm.createAgentBrain(brain)
        agent_brain.processSensoryInput(sensory_input)
        
        # Simulate world step
        world.update(current_time)
        
        # Get current state every 10 steps
        if step % 10 == 0:
            spikes = brain.getTotalSpikeCount()
            active_neurons = brain.getActiveNeuronCount()
            print(f"Step {step}: Spikes={spikes}, Active neurons={active_neurons}")
        
        # Take an action every 20 steps
        if step % 20 == 19:
            action = pynlm.MotorCommand.MoveForward
            action_result = world.applyMotorCommand(action, current_time)
            
            # Apply reward modulation to brain
            reward = action_result.reward if action_result.success else -0.5
            predicted_reward = 0.1
            
            agent_brain.applyRewardModulation(reward, predicted_reward)
            print(f"Step {step}: Action '{action}' taken, Reward={reward:.2f}")
    
    # Get final state
    final_spikes = brain.getTotalSpikeCount()
    final_active = brain.getActiveNeuronCount()
    
    print(f"\nFinal state - Total spikes: {final_spikes} (change: {final_spikes - initial_spikes})")
    print(f"Final state - Active neurons: {final_active} (change: {final_active - initial_neurons})")
    print(f"Learning progress: {(final_active - initial_active) / max(initial_active, 1) * 100:.1f}%")


def configuration_explorer_example():
    """Example: Explore different configuration combinations."""
    print("\n=== Configuration Explorer Example ===")
    
    if not PYNLM_AVAILABLE:
        print("NLM Python bindings not available")
        return
    
    # Define different configurations to test
    configurations = [
        {"name": "Minimal", "neurons": 100, "regions": 1, "connection_prob": 0.1},
        {"name": "Standard", "neurons": 500, "regions": 2, "connection_prob": 0.15},
        {"name": "Dense", "neurons": 1000, "regions": 3, "connection_prob": 0.25},
        {"name": "Sparse", "neurons": 50, "regions": 1, "connection_prob": 0.05},
    ]
    
    for config in configurations:
        print(f"\nTesting {config['name']} configuration:")
        
        # Create config
        config_obj = pynlm.createDefaultConfig()
        config_obj.set("neuron_count", config['neurons'])
        config_obj.set("region_count", config['regions'])
        config_obj.set("connection_probability", config['connection_prob'])
        config_obj.set("random_seed", 42)
        
        # Show configuration
        config_str = show_config(config_obj)
        print(f"  {config['name']} Config:")
        for line in config_str.strip().split('\n'):
            if line.strip():
                print(f"    {line}")
        
        # Create brain
        brain = pynlm.createBrain(config_obj)
        brain.initialize()
        
        print(f"  Created brain with {brain.getTotalNeuronCount()} neurons")
        print(f"  Connections created: {brain.getTotalSynapseCount()}")
        
        # Clean up
        del brain


def plasticity_experiment_example():
    """Example: Demonstrate plasticity through STDP learning."""
    print("\n=== Plasticity Experiment Example ===")
    
    if not PYNLM_AVAILABLE:
        print("NLM Python bindings not available")
        return
    
    # Create brain with plasticity enabled
    brain = create_example_brain(200)
    
    # Enable plasticity on all synapses
    for region in brain.getRegions():
        for synapse in region.getSynapses():
            synapse.enablePlasticity(True, True, False)  # Hebbian and STDP
    
    print(f"Created brain with plasticity enabled on {brain.getTotalSynapseCount()} synapses")
    
    # Record initial weights
    initial_weights = []
    for region in brain.getRegions():
        for synapse in region.getSynapses():
            initial_weights.append(synapse.getWeight())
    
    print(f"Initial mean synapse weight: {sum(initial_weights) / len(initial_weights):.4f}")
    
    # Apply correlated activity to induce learning (STDP)
    print("Applying correlated pre-post activity (inducing LTP)...")
    
    for trial in range(20):
        # Get first two neurons
        neurons = []
        for region in brain.getRegions():
            region_neurons = region.getAllNeurons()
            neurons.extend(region_neurons)
        
        if len(neurons) >= 2:
            # Fire pre-synaptic neuron
            neurons[0]->injectCurrent(60.0)
            brain.step(trial * 2, trial * 2 * 0.001)
            
            # Fire post-synaptic neuron  
            neurons[1]->injectCurrent(60.0)
            brain.step(trial * 2 + 1, (trial * 2 + 1) * 0.001)
    
    # Record final weights
    final_weights = []
    for region in brain.getRegions():
        for synapse in region.getSynapses():
            final_weights.append(synapse.getWeight())
    
    print(f"Final mean synapse weight: {sum(final_weights) / len(final_weights):.4f}")
    
    # Calculate weight change
    weight_change = (sum(final_weights) / len(final_weights)) - (sum(initial_weights) / len(initial_weights))
    print(f"Net weight change: {weight_change:.4f} ({'increase' if weight_change > 0 else 'decrease'})")
    
    # Assess learning
    if abs(weight_change) > 0.01:
        print("✓ Plasticity detected: Significant weight change through STDP")
    else:
        print("✗ No significant plasticity detected")
    
    print(f"Total spikes generated during experiment: {brain.getTotalSpikeCount()}")


def run_all_examples():
    """Run all examples in sequence."""
    print("NLM Python Bindings Examples")
    print("=" * 50)
    
    if not PYNLM_AVAILABLE:
        print("ERROR: NLM Python bindings not available!")
        print("Please build the NLM Python bindings first:")
        print("  pip install -e .")
        return
    
    # Run examples
    examples = [
        ("Basic Brain", basic_brain_example),
        ("Sensory-Motor Agent", sensory_motoriy_example),
        ("Configuration Explorer", configuration_explorer_example),
        ("Plasticity Experiment", plasticity_experiment_example),
    ]
    
    for name, example_func in examples:
        try:
            print(f"\n{'='*60}")
            example_func()
        except Exception as e:
            print(f"Error in {name} example: {e}")
            import traceback
            traceback.print_exc()
    
    print(f"\n{'='*60}")
    print("All examples completed!")


if __name__ == "__main__":
    run_all_examples()
