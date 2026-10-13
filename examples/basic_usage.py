import pynlm
import random
import time

def main():
    """Basic usage example demonstrating NLM simulation setup and execution."""
    print("=== Basic NLM Simulation Example ===")
    
    # Create brain with simple template (fast for testing)
    print("Creating brain with 'simple' template...")
    brain = pynlm.create_brain_from_template("simple")
    
    # Initialize the brain
    print("Initializing brain...")
    brain.initialize()
    
    print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")
    print(f"  - Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  - Regions: {brain.getRegionCount()}")
    
    # Create and configure world
    print("\nSetting up world environment...")
    world = pynlm.createSimpleWorld()
    world.configure(800, 600, 160, 120)  # width, height, vision_width, vision_height
    world.setRandomSeed(12345)
    
    print(f"  - World size: {world.getWidth()}x{world.getHeight()}")
    print(f"  - Vision: {world.getVisionWidth()}x{world.getVisionHeight()}")
    
    # Create agent with explorer behavior
    print("\nCreating agent brain...")
    agent = pynlm.create_agent_simulation(brain, world, "explorer")
    
    # Set up sensory percept
    print("\nSetting up sensory percept...")
    percept = pynlm.SensoryPercept()
    
    # Run simulation loop
    print("\nStarting simulation (100 steps)...")
    start_time = time.time()
    
    for step in range(100):
        # Generate random sensory input (simulating environmental input)
        vision_data = [random.uniform(0, 1) for _ in range(160 * 120 * 3)]
        vision = pynlm.Vision(160, 120, 3)
        vision.setData(vision_data)
        
        # Create percept with vision data
        percept.setVision(vision)
        
        # Process sensory input through agent
        agent.processSensoryInput(percept)
        
        # Step the simulation
        brain.step(step, step * 0.001)
        
        # Log progress every 20 steps
        if step % 20 == 0:
            stats = pynlm.compute_neural_stats(brain)
            print(f"  Step {step:3d}: Spikes={stats['total_spikes']:4d}, "
                  f"Firing={stats['firing_neurons']:3d}/{stats['total_neurons']:4d} "
                  f"({stats['avg_firing_rate']*100:5.1f}%)")
    
    end_time = time.time()
    
    print(f"\nSimulation completed in {end_time - start_time:.2f} seconds")
    print(f"  - Total spikes generated: {brain.getTotalSpikeCount()}")
    print(f"  - Final firing rate: {brain.getAverageFiringRate():.4f} Hz")
    
    # Demonstrate advanced features
    print("\n=== Demonstrating Advanced Features ===")
    
    # Create debug monitor and visualize activity
    print("Creating debug monitor...")
    debug_monitor = pynlm.create_debug_monitor(brain)
    
    # Run a few more steps with monitoring
    for step in range(101, 110):
        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 15.0)
        brain.step(step, step * 0.001)
        debug_monitor.log_step()
    
    # Get debug summary
    summary = debug_monitor.get_summary()
    print(f"Debug summary - Steps: {summary['step_count']}, "
          f"Avg firing rate: {summary.get('avg_normalized_firing_rate', 0)*100:.2f}%")
    
    # Create visualizer and generate report
    print("\nCreating visualizer...")
    visualizer = pynlm.create_visualizer(brain)
    visualizer.print_network_summary()
    
    # Export data for analysis
    print("\nExporting visualization data...")
    visualizer.export_visualization_data()
    
    print("\n=== Basic Example Complete ===")

if __name__ == "__main__":
    main()