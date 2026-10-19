#!/usr/bin/env python3
"""
Working Memory Example

This example demonstrates the advanced Working Memory system in NLM.
It shows how to store, retrieve, and manage temporary information with competition and decay.

Author: NLM Examples Team
Date: 2026-10-19
Version: 1.0
"""

import sys
import os
import time
import numpy as np
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))

import pynlm
from datetime import datetime

class WorkingMemoryExample:
    """Example class demonstrating the Working Memory system."""
    
    def __init__(self):
        """Initialize the working memory example."""
        self.brain = None
        self.world = None
        self.agent = None
        self.working_memory = None
        self.example_name = "Working Memory System"
        
    def setup_simulation_environment(self):
        """Set up the basic simulation environment."""
        print("Setting up simulation environment...")
        
        # Create world
        self.world = pynlm.createSimpleWorld()
        self.world.configure(20, 20, 12, 12)
        self.world.reset()
        
        # Create brain
        config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        # Create agent
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
    def create_working_memory_system(self, capacity=20):
        """
        Create and configure the Working Memory system.
        
        Args:
            capacity: Maximum number of memory traces
        """
        print("Creating Working Memory system...")
        
        # Create working memory instance
        self.working_memory = pynlm.WorkingMemory()
        print(f"  - Working Memory created")
        
        # Set capacity
        self.working_memory.setCapacity(capacity)
        print(f"  - Capacity set to: {capacity}")
        
        # Get some neuron IDs to work with (in practice, you'd get actual neuron IDs)
        # For this example, we'll use dummy neuron IDs
        self.test_neurons = [
            pynlm.NeuronId(1),
            pynlm.NeuronId(2),
            pynlm.NeuronId(3),
            pynlm.NeuronId(4),
            pynlm.NeuronId(5),
        ]
        
        # Create some dummy neurons in the brain for testing
        for i, neuron_id in enumerate(self.test_neurons):
            # Inject some current to activate neurons
            self.brain.injectCurrent(neuron_id, 0.5)
            
    def demonstrate_working_memory_operations(self):
        """Demonstrate basic working memory operations."""
        print("Demonstrating Working Memory operations...")
        
        print("1. Storing information in working memory:")
        # Store different values for different neurons
        for i, neuron_id in enumerate(self.test_neurons):
            value = 0.8 - (i * 0.1)  # Different values: 0.8, 0.7, 0.6, 0.5, 0.4
            self.working_memory.store(neuron_id, value)
            print(f"   - Stored {value:.1f} for neuron {neuron_id.value}")
        
        print("\n2. Retrieving information from working memory:")
        for neuron_id in self.test_neurons:
            value = self.working_memory.recall(neuron_id)
            print(f"   - Retrieved {value:.1f} for neuron {neuron_id.value}")
        
        print("\n3. Checking memory contents:")
        print(f"   - Current size: {self.working_memory.getCurrentSize()}")
        print(f"   - Capacity: {self.working_memory.capacity}")
        print(f"   - Active traces: {self.working_memory.activeTraces}")
        
    def demonstrate_working_memory_decay(self):
        """Demonstrate working memory decay mechanism."""
        print("\nDemonstrating Working Memory decay...")
        
        # Store some values
        neuron_id = pynlm.NeuronId(99)
        self.working_memory.store(neuron_id, 0.9)
        
        print(f"  - Stored value: 0.9")
        
        # Apply decay at different rates
        decay_rates = [0.0, 0.1, 0.3, 0.5, 0.8]
        
        for rate in decay_rates:
            # Create a copy for demonstration (in real implementation, this would be in-place)
            current_value = self.working_memory.recall(neuron_id)
            decayed_value = current_value * (1.0 - rate)
            
            print(f"  - Decay rate {rate:.1f}: {current_value:.3f} -> {decayed_value:.3f}")
            
    def demonstrate_working_memory_competition(self):
        """Demonstrate working memory competition for limited space."""
        print("\nDemonstrating Working Memory competition...")
        
        # Create multiple competing memories
        competing_neurons = [
            pynlm.NeuronId(10),
            pynlm.NeuronId(11),
            pynlm.NeuronId(12),
            pynlm.NeuronId(13),
            pynlm.NeuronId(14),
        ]
        
        # Set very low capacity to force competition
        self.working_memory.setCapacity(3)
        print(f"  - Reduced capacity to: {self.working_memory.capacity} (forcing competition)")
        
        # Store many items (more than capacity)
        print("  - Storing 5 items into capacity of 3...")
        for i, neuron_id in enumerate(competing_neurons):
            value = 0.9 - (i * 0.1)
            self.working_memory.store(neuron_id, value)
            print(f"    - Stored {value:.1f} for neuron {neuron_id.value}")
        
        # Check current state
        print(f"  - Current size after competition: {self.working_memory.getCurrentSize()}")
        
        # Clear and reset capacity
        self.working_memory.clear()
        self.working_memory.setCapacity(20)
        print(f"  - Cleared and reset capacity to: {self.working_memory.capacity}")
        
    def demonstrate_working_memory_decay_update(self):
        """Demonstrate working memory decay update mechanism."""
        print("\nDemonstrating Working Memory decay update...")
        
        # Store multiple items
        for i in range(5):
            neuron_id = pynlm.NeuronId(100 + i)
            self.working_memory.store(neuron_id, 0.8 - (i * 0.1))
        
        print(f"  - Stored 5 items with values: {[self.working_memory.recall(pynlm.NeuronId(100 + i)) for i in range(5)]}")
        
        # Apply decay update
        decay_rate = 0.2
        self.working_memory.update(decay_rate)
        print(f"  - Applied decay update with rate: {decay_rate}")
        
        # Check values after decay
        print("  - Values after decay:")
        for i in range(5):
            value = self.working_memory.recall(pynlm.NeuronId(100 + i))
            print(f"    - Neuron {100 + i}: {value:.3f}")
        
    def integrate_working_memory_with_agent(self, steps=30):
        """
        Integrate working memory with agent behavior.
        
        Args:
            steps: Number of simulation steps
        """
        print(f"\nIntegrating Working Memory with agent behavior ({steps} steps)...")
        
        for step in range(steps):
            # 1. Update world
            self.world.update(0.1)
            
            # 2. Get sensory input
            percept = self.world.getSensoryPercept()
            
            # 3. Process sensory input
            self.agent.processSensoryInput(percept)
            
            # 4. Brain step
            self.brain.step(step)
            
            # 5. Working memory influences attention/retrieval
            # Simulate working memory being used by agent
            neuron_id = pynlm.NeuronId(200 + (step % 3))  # Cycle through 3 neurons
            wm_value = self.working_memory.recall(neuron_id)
            
            # Inject current based on working memory value
            current = wm_value * 0.5
            self.brain.injectCurrent(neuron_id, current)
            
            # 6. Get action
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Display progress every 10 steps
            if step % 10 == 0:
                print(f"  Step {step:3d}: Working memory values: "
                      f"{[self.working_memory.recall(pynlm.NeuronId(200 + i)) for i in range(3)]}")
        
        print(f"  - Integration completed: {steps} steps")
        
    def display_working_memory_statistics(self):
        """Display working memory system statistics."""
        print("\nWorking Memory System Statistics:")
        print("-" * 40)
        
        print(f"1. Memory System:")
        print(f"   - Capacity: {self.working_memory.capacity}")
        print(f"   - Current size: {self.working_memory.getCurrentSize()}")
        print(f"   - Memory neurons: {self.working_memory.memoryNeurons}")
        print(f"   - Active traces: {self.working_memory.activeTraces}")
        
        print(f"\n2. Simulation State:")
        print(f"   - Total neurons: {self.brain.getTotalNeuronCount()}")
        print(f"   - Total synapses: {self.brain.getTotalSynapseCount()}")
        print(f"   - Firing neurons: {self.brain.getFiringNeuronCount()}")
        
        print(f"\n3. Performance Metrics:")
        usage_ratio = self.working_memory.getCurrentSize() / max(1, self.working_memory.capacity)
        print(f"   - Memory utilization: {usage_ratio:.1%}")
        print(f"   - Average decay rate: Not measured")
        
    def run_example(self, steps=50):
        """
        Run the complete working memory example.
        
        Args:
            steps: Number of simulation steps for integration demo
        """
        print("=" * 60)
        print(f"NLM Working Memory System Example")
        print(f"Created: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
        print("=" * 60)
        print()
        
        try:
            # Step 1: Setup simulation environment
            self.setup_simulation_environment()
            print()
            
            # Step 2: Create working memory system
            self.create_working_memory_system()
            print()
            
            # Step 3: Demonstrate basic operations
            self.demonstrate_working_memory_operations()
            print()
            
            # Step 4: Demonstrate decay
            self.demonstrate_working_memory_decay()
            print()
            
            # Step 5: Demonstrate competition
            self.demonstrate_working_memory_competition()
            print()
            
            # Step 6: Demonstrate decay update
            self.demonstrate_working_memory_decay_update()
            print()
            
            # Step 7: Integrate with agent
            self.integrate_working_memory_with_agent(steps)
            print()
            
            # Step 8: Display statistics
            self.display_working_memory_statistics()
            print()
            
            print("=" * 60)
            print("Working Memory Example Completed Successfully!")
            print("=" * 60)
            
            return self.brain, self.world, self.agent, self.working_memory
            
        except Exception as e:
            print(f"Error in working memory example: {e}")
            import traceback
            traceback.print_exc()
            return None, None, None, None

if __name__ == "__main__":
    # Parse command line arguments
    import argparse
    
    parser = argparse.ArgumentParser(description="NLM Working Memory Example")
    parser.add_argument("--steps", type=int, default=50, help="Number of simulation steps")
    
    args = parser.parse_args()
    
    # Run the working memory example
    example = WorkingMemoryExample()
    result = example.run_example(args.steps)
    
    if result[0] is not None:
        print("\nExample completed successfully. Working memory system is working correctly.")
        sys.exit(0)
    else:
        print("\nExample failed. Please check the error messages above.")
        sys.exit(1)