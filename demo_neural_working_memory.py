#!/usr/bin/env python3
"""
Demonstration of NeuralWorkingMemory Python Bindings

This script shows how to use the NeuralWorkingMemory bindings.
Note: This is conceptual - you would need to build the actual bindings to run it.
"""

import pynlm

def main():
    print("NeuralWorkingMemory Python Bindings Demonstration")
    print("=" * 60)
    
    # Create a brain
    print("\n1. Creating a brain...")
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    print("   Brain created and initialized")
    
    # Get the working memory from the brain
    print("\n2. Getting working memory from brain...")
    working_memory = pynlm.getWorkingMemoryFromBrain(brain)
    print("   Working memory obtained")
    
    # Create a standalone working memory
    print("\n3. Creating standalone working memory...")
    standalone_wm = pynlm.createWorkingMemory()
    print("   Standalone working memory created")
    
    # Initialize with brain
    print("\n4. Initializing working memory with brain...")
    working_memory.initialize(brain)
    print("   Working memory initialized")
    
    # Test storage and retrieval
    print("\n5. Testing storage and retrieval...")
    test_pattern = [0.1, 0.5, 0.9, 0.2, 0.7]
    working_memory.store(test_pattern, strength=1.0)
    retrieved = working_memory.retrieve()
    print(f"   Stored pattern: {test_pattern}")
    print(f"   Retrieved: {retrieved}")
    
    # Test neuron-specific storage
    print("\n6. Testing neuron-specific storage...")
    # Note: In actual usage, you'd have a valid NeuronId
    # For demonstration, we show the API
    print("   Working memory can store activation to specific neurons")
    print("   API: storeToNeuron(neuron_id, activation_value)")
    
    # Test capacity and decay
    print("\n7. Testing configuration...")
    working_memory.setCapacity(200)
    working_memory.setDecayRate(0.05)
    print(f"   Set capacity: {working_memory.getCapacity()}")
    print(f"   Set decay rate: {working_memory.getDecayRate()}")
    print(f"   Current active traces: {working_memory.getActiveTraces()}")
    
    # Test competition
    print("\n8. Testing competition mechanism...")
    working_memory.runCompetition()
    print("   Competition run - winning neurons selected")
    
    # Test strengthening
    print("\n9. Testing memory strengthening...")
    working_memory.strengthenMemory(1.2)
    print("   Memory strengthened by factor 1.2")
    
    # Test clearing
    print("\n10. Testing memory clearing...")
    working_memory.clear()
    print(f"    After clear, active traces: {working_memory.getActiveTraces()}")
    
    # Test with default configuration
    print("\n11. Testing with default configuration...")
    default_config = pynlm.createWorkingMemoryConfig()
    print(f"   Default config created with memory settings")
    
    # Demonstrate Python naming conventions
    print("\n12. Python naming conventions used...")
    print("   - store(pattern, strength) -> storeToNeuron() for neurons")
    print("   - retrieve() -> Returns Python list of floats")
    print("   - contains(neuron) -> Check membership")
    print("   - getNeuronActivation(neuron) -> Get activation")
    print("   - getMemoryActivity() -> Overall activity level")
    print("   - getActiveTraces() -> Number of active traces")
    
    print("\n" + "=" * 60)
    print("Demonstration complete!")
    print("\nKey features of NeuralWorkingMemory:")
    print("- Persistent activity patterns")
    print("- Recurrent connections for maintenance")
    print("- Activity-dependent plasticity")
    print("- Neural competition for selective retention")
    print("- Configurable capacity and decay rate")

if __name__ == "__main__":
    main()