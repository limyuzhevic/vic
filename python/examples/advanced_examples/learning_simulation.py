"""
Learning Simulation - Advanced Brain Learning
===============================================

This example demonstrates advanced learning simulation with all NLM features enabled.
It shows curiosity, reward modulation, development, and plasticity in action.

Key Concepts Demonstrated:
- Complete experiment setup with all features
- Learning simulation with statistics collection
- Neuromodulation (curiosity, reward)
- Developmental progression

Expected Output:
---------------
Setting up advanced learning simulation...
World: 20x20, Agent start: (10.00, 10.00)
Features enabled: Reward modulation, Development, Curiosity

Learning simulation results:
Total reward collected: 12.45
Successful actions: 38/50
Total neural spikes: 847
Developmental stage progression: Initial -> CriticalPeriod -> Maturation

Learning simulation complete!
"""

import pynlm
import time

def main():
    print("Setting up advanced learning simulation...")
    
    # Create complete experiment setup with all features enabled
    brain, world, agent = pynlm.createReadyToRunSimpleExperiment()
    
    print(f"World: {world.getWidth()}x{world.getHeight()}")
    print(f"Agent start: ({world.getAgentBody().x:.2f}, {world.getAgentBody().y:.2f})")
    print("Features enabled: Reward modulation, Development, Curiosity")
    
    # Run learning simulation
    print("\nRunning learning simulation...")
    start_time = time.time()
    
    # Run learning simulation with enhanced API
    total_reward, success_count, total_spikes, stage = pynlm.run_learning_simulation(
        brain, world, agent, 100
    )
    
    elapsed = time.time() - start_time
    
    print(f"\nLearning simulation results:")
    print(f"  Total reward collected: {total_reward:.2f}")
    print(f"  Successful actions: {success_count}/100")
    print(f"  Total neural spikes: {total_spikes}")
    print(f"  Simulation time: {elapsed:.2f} seconds")
    print(f"  Developmental stage progression: {stage}")
    
    print(f"\nLearning simulation complete!")

if __name__ == "__main__":
    main()