#!/usr/bin/env python3
"""
Example Python script demonstrating advanced NLM features using the new Python bindings.
This script shows how to use the power-user features added to the Python bindings.
"""

from pynlm import *

def main():
    print("=== Advanced NLM Python Features Demo ===\n")
    
    # 1. Create brain with advanced features
    config = createDefaultConfig()
    config.set("simulation.steps", 1000)
    config.set("simulation.saveInterval", 100)
    
    brain = createBrain(config)
    
    # 2. Initialize the brain
    brain.initialize()
    
    print("✓ Brain initialized with advanced features")
    
    # 3. Test advanced simulation control
    print("\n--- Advanced Simulation Control ---")
    
    # Set fine-grained timestep
    brain.setTimestep(0.001)
    print(f"✓ Timestep set to: {brain.getTimestep()}")
    
    # Pause and check status
    brain.pause()
    print(f"✓ Simulation paused: {brain.isPaused()}")
    
    brain.resume()
    print(f"✓ Simulation resumed: {brain.isPaused()}")
    
    # 4. Test memory and performance monitoring
    print("\n--- Memory and Performance Monitoring ---")
    
    # Set up custom spike handler
    def spike_handler(neuron_id, spike_time):
        print(f"  Spike detected: Neuron {neuron_id} at time {spike_time}")
    
    brain.addSpikeHandler(spike_handler)
    
    # Start profiling
    brain.startProfiling()
    
    # Run a few steps to generate data
    for step in range(10):
        brain.step(step, step * 0.1)
    
    # Stop profiling and get stats
    brain.stopProfiling()
    
    print(f"✓ Memory usage: {brain.getMemoryUsage()} bytes")
    print(f"✓ Performance stats: {brain.getPerformanceStats()}")
    print(f"✓ Profile data collected: {len(brain.getProfileData())} data points")
    
    # 5. Test state management
    print("\n--- State Management ---")
    
    # Create a checkpoint
    brain.checkpoint("demo_checkpoint")
    print("✓ Checkpoint 'demo_checkpoint' created")
    
    # List checkpoints
    checkpoints = brain.listCheckpoints()
    print(f"✓ Available checkpoints: {[cp.name for cp in checkpoints]}")
    
    # Compare with saved checkpoint
    brain.loadCheckpoint("demo_checkpoint")
    print("✓ Checkpoint 'demo_checkpoint' loaded")
    
    # 6. Test neuromodulation control
    print("\n--- Neuromodulation Control ---")
    
    # Set neuromodulator levels
    brain.setDopamineLevel(0.8)
    brain.setCuriosityLevel(0.6)
    brain.setNoveltyLevel(0.5)
    brain.setPredictionError(0.2)
    
    print(f"✓ Dopamine level: {brain.getNeuromodulationLevel()}")
    print(f"✓ Neuromodulators: {brain.getNeuromodulators()}")
    
    # Reset neuromodulation
    brain.resetNeuromodulation()
    print("✓ Neuromodulators reset to baseline")
    
    # 7. Test development and learning control
    print("\n--- Development and Learning Control ---")
    
    # Set developmental stage
    brain.setDevelopmentalStage(DevelopmentalStage.Maturation)
    print(f"✓ Developmental stage set to: {brain.getDevelopmentalStage()}")
    
    # Advance development
    brain.advanceDevelopment(0.5)
    print(f"✓ Development advanced by 0.5 time units")
    
    # Set learning parameters
    brain.setLearningRate(0.01)
    brain.setPlasticityThreshold(0.5)
    brain.setEligibilityTraceDecay(0.99)
    
    print("✓ Learning parameters set")
    
    # 8. Test diagnostics and debugging
    print("\n--- Diagnostics and Debugging ---")
    
    # Profile neural activity
    brain.profileNeuralActivity(0.5)
    print("✓ Neural activity profiled")
    
    # Get activity statistics
    activity_stats = brain.getActivityStats()
    print(f"✓ Activity stats - Active neurons: {activity_stats.activeNeurons}")
    print(f"✓ Activity stats - Firing rate: {activity_stats.firingRate}")
    print(f"✓ Activity stats - Burst events: {activity_stats.burstCount}")
    
    # Analyze connectivity
    connectivity = brain.analyzeConnectivity(0.1)
    print(f"✓ Connectivity analysis complete - Strong connections: {connectivity.strongConnections}")
    
    # 9. Test multi-agent and environment control
    print("\n--- Multi-Agent and Environment Control ---")
    
    # Create another brain for multi-agent testing
    config2 = createDefaultConfig()
    brain2 = createBrain(config2)
    brain2.initialize()
    
    # Create agent brains
    agent_brain = createAgentBrain(brain)
    agent_brain2 = createAgentBrain(brain2)
    
    # Batch process sensors (simulated)
    print("✓ Batch processing sensors")
    
    # Coordinate agents
    agent_brain.coordinateWithAgent(agent_brain2, "shared_resource")
    print("✓ Agents coordinated")
    
    # Set environment variables
    agent_brain.setEnvironmentVariable("temperature", 25.0)
    agent_brain.setEnvironmentVariable("humidity", 60.0)
    
    print(f"✓ Temperature: {agent_brain.getEnvironmentVariable('temperature')}")
    print(f"✓ Humidity: {agent_brain.getEnvironmentVariable('humidity')}")
    
    # 10. Test utility and status
    print("\n--- Utility and Status ---")
    
    # Add a new region
    region_id = brain.addRegion("default_region")
    print(f"✓ Region added with ID: {region_id}")
    
    # Get region statistics
    region_stats = brain.getRegionStats(region_id)
    print(f"✓ Region stats - Neurons: {region_stats.neuronCount}")
    print(f"✓ Region stats - Synapses: {region_stats.synapseCount}")
    
    # Get global statistics
    print(f"✓ Total neurons: {brain.getTotalNeuronCount()}")
    print(f"✓ Total synapses: {brain.getTotalSynapseCount()}")
    print(f"✓ Active neurons: {brain.getActiveNeuronCount()}")
    print(f"✓ Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"✓ Average firing rate: {brain.getAverageFiringRate()}")
    print(f"✓ Excitation/inhibition ratio: {brain.getExcitationInhibitionRatio()}")
    print(f"✓ Total spike count: {brain.getTotalSpikeCount()}")
    print(f"✓ Current developmental stage: {brain.getDevelopmentalStage()}")
    
    # Log brain status
    print("\n--- Brain Status ---")
    brain.logStatus()
    
    print("\n=== All Advanced Features Successfully Demonstrated ===")

if __name__ == "__main__":
    main()
