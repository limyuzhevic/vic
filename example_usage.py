#!/usr/bin/env python3
"""
Example Usage of Enhanced NLM Python API
=========================================

This script demonstrates the comprehensive Python API for the Neural Learning Machine (NLM) framework.
It showcases all the enhancements implemented in bindings.cpp:

1. Complete System Coverage
2. Pythonic API Design
3. Advanced Convenience Functions
4. Context Managers
5. Enhanced Documentation and Error Handling
6. Learning and Development Controls
7. Memory System Controls
8. Monitoring and Visualization Integration
9. Educational Features

Usage:
    python3 example_usage.py
"""

import pynlm
import time

def main():
    print("=== Enhanced NLM Python API Example ===\n")
    
    # Show the API banner
    pynlm.show_banner()
    print()
    
    try:
        # 1. Create and initialize a brain with enhanced Pythonic API
        print("1. Creating and initializing brain...")
        brain = pynlm.Brain(pynlm.Config())
        
        if brain.initialize():
            print("   ✓ Brain initialized successfully")
        else:
            print("   ✗ Brain initialization failed")
            return
        
        # 2. Use context manager for safe resource management
        print("\n2. Using context manager for safe operations...")
        with brain.context():
            print("   ✓ Context manager active")
            
            # 3. Configure memory systems with Pythonic convenience functions
            print("\n3. Configuring memory systems...")
            pynlm.setup_memory_systems(brain, working_memory_capacity=200, episodic_memory_limit=2000)
            print("   ✓ Memory systems configured")
            
            # 4. Setup neuromodulation with Pythonic API
            print("\n4. Setting up neuromodulation...")
            pynlm.setup_neuromodulation(brain, dopamine_level=0.7, curiosity_weight=0.6, novelty_threshold=0.15)
            print("   ✓ Neuromodulation systems configured")
            
            # 5. Enable advanced learning mode
            print("\n5. Enabling advanced learning mode...")
            pynlm.setup_advanced_brain(brain)
            print("   ✓ Advanced learning mode enabled")
            
            # 6. Create and configure world simulation
            print("\n6. Setting up world simulation...")
            world = pynlm.create_simple_world()
            world.configure(800, 600, 160, 120)
            world.set_agent_start(400, 300)
            print("   ✓ World simulation configured")
            
            # 7. Create agent brain with Pythonic interface
            print("\n7. Creating agent brain...")
            agent_brain = pynlm.create_agent_brain(brain)
            agent_brain.initialize(world)
            print("   ✓ Agent brain created and initialized")
            
            # 8. Demonstrate development stage controls
            print("\n8. Demonstrating development stage controls...")
            print(f"   Current developmental stage: {agent_brain.get_developmental_stage()}")
            
            # Check if in critical period
            if agent_brain.is_critical_period():
                print("   ✓ In critical development period")
            else:
                print("   - Not in critical period")
                
            # Advance development stage
            pynlm.advance_development_stage(brain)
            print(f"   Advanced to: {agent_brain.get_developmental_stage()}")
            
            # 9. Demonstrate memory system operations
            print("\n9. Demonstrating memory system operations...")
            wm = brain.get_working_memory()
            print(f"   Working memory capacity: {wm.getCapacity()}")
            print(f"   Active memory traces: {wm.getActiveTraces()}")
            
            em = brain.get_episodic_memory()
            print(f"   Episodic memory episodes: {em.getEpisodeCount()}")
            print(f"   Replay enabled: {em.isReplayEnabled()}")
            
            # 10. Demonstrate neuromodulation monitoring
            print("\n10. Demonstrating neuromodulation monitoring...")
            dopamine = brain.get_dopamine()
            curiosity = brain.get_curiosity()
            novelty = brain.get_novelty()
            
            print(f"   Dopamine level: {brain.get_neuromodulation_level():.3f}")
            print(f"   Curiosity level: {brain.get_curiosity_level():.3f}")
            print(f"   Novelty level: {brain.get_novelty_level():.3f}")
            print(f"   Prediction error: {brain.get_prediction_error():.3f}")
            
            # 11. Demonstrate action production and Pythonic API
            print("\n11. Demonstrating action production...")
            # Simulate taking a step in the world
            time.sleep(0.1)  # Simulate computation time
            
            world.update(1.0)  # Update world by 1 time unit
            action = agent_brain.decode_motor_command()
            
            print(f"   Produced action: {action.get_name()}")
            print(f"   Action type: {action.get_type()}")
            print(f"   Action parameters: {action.get_parameters()}")
            
            # 12. Demonstrate error handling with Pythonic exceptions
            print("\n12. Demonstrating error handling...")
            try:
                # Try to load non-existent config
                bad_config = pynlm.Config()
                bad_config.load_from_file("/non/existent/path.json")
            except pynlm.ConfigurationError as e:
                print(f"   ✓ Caught ConfigurationError: {str(e)[:50]}...")
            
            # 13. Demonstrate utility functions and educational features
            print("\n13. Using utility functions...")
            pynlm.print_example_usage()
            
            # 14. Demonstrate advanced brain controls
            print("\n14. Demonstrating advanced brain controls...")
            print(f"   Total neurons: {brain.get_total_neuron_count()}")
            print(f"   Total synapses: {brain.get_total_synapse_count()}")
            print(f"   Active neurons: {brain.get_active_neuron_count()}")
            print(f"   Firing neurons: {brain.get_firing_neuron_count()}")
            print(f"   Average firing rate: {brain.get_average_firing_rate():.2f}")
            
            # 15. Demonstrate context manager for cleanup
            print("\n15. Demonstrating context manager cleanup...")
            # The context manager will automatically clean up when exiting the with block
            print("   ✓ Context manager will clean up on exit")
            
        print("\n=== All demonstrations completed successfully ===")
        
        # 16. Final demonstration with example config
        print("\n16. Creating example configuration...")
        example_config = pynlm.Config()
        pynlm.create_example_config(example_config)
        print(f"   Config summary: {example_config.summary()}")
        
    except Exception as e:
        print(f"\n❌ Error during execution: {e}")
        return 1
    
    print("\n=== Example execution completed ===")
    return 0

if __name__ == "__main__":
    exit(main())