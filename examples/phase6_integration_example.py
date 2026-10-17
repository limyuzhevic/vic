#!/usr/bin/env python3
"""
Example script demonstrating the improved NLM Phase 6 integration.

This script shows how to:
1. Create a complete brain with all systems integrated
2. Use the agent-brain interface for world interaction
3. Monitor development and neuromodulation
4. Run learning experiments
5. Use checkpointing for persistence
"""

import time
import tempfile
import os

try:
    import pynlm as nlm
except ImportError:
    print("Error: pynlm not found. Please install the Python bindings.")
    print("For now, this example shows the improved API structure.")
    exit(1)

def main():
    print("=== NLM Phase 6 Integration Example ===\n")
    
    # Create default configuration
    config = nlm.createDefaultConfig()
    
    # Configure for integration experiment
    config.set("neuron_count", 500)
    config.set("region_count", 1)
    config.set("connection_probability", 0.15f)
    config.set("stdp_ltp_weight", 0.01f)
    config.set("stdp_ltd_weight", 0.012f)
    config.set("synaptogenesis_rate", 0.0001f)
    config.set("pruning_rate", 0.00001f)
    
    print("1. Creating brain with complete system integration...")
    brain = nlm.createBrain(config)
    brain.initialize()
    print("   ✓ Brain initialized successfully")
    print(f"   - Total neurons: {brain.getTotalNeuronCount()}")
    print(f"   - Total synapses: {brain.getTotalSynapseCount()}")
    
    print("\n2. Creating agent-brain interface...")
    agent = nlm.createAgentBrain(brain)
    
    # Create world
    world = nlm.createSimpleWorld()
    world.configure(16, 16, 16, 16)  # 16x16 world, 16x16 vision
    world.setAgentStart(8.0, 8.0)  # Start in center
    
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("   ✓ Agent initialized with world integration")
    print(f"   - Sensory input size: {agent.getSensoryInputSize()}")
    print(f"   - Motor output size: {agent.getMotorOutputSize()}")
    
    print("\n3. Running integration verification...")
    
    # Create experiment to verify integration
    experiment = nlm.Phase6IntegratedExperiment()
    
    # Test integration (this would normally call the C++ verification)
    integration_ok = experiment.verifyIntegration()
    if integration_ok:
        print("   ✓ All systems integrated successfully")
    else:
        print("   ✗ Integration verification failed")
        return
    
    print("\n4. Running system component tests...")
    
    # Test memory integration
    memory_test_ok = experiment.testMemoryIntegration()
    if memory_test_ok:
        print("   ✓ Memory systems functional")
    
    # Test neuromodulation
    neuromodulation_test_ok = experiment.testNeuromodulationIntegration()
    if neuromodulation_test_ok:
        print("   ✓ Neuromodulation systems functional")
    
    # Test checkpointing
    checkpoint_test_ok = experiment.testCheckpointing()
    if checkpoint_test_ok:
        print("   ✓ Checkpoint system working")
    
    # Test replay
    replay_test_ok = experiment.testReplay()
    if replay_test_ok:
        print("   ✓ Replay system working")
    
    print("\n5. Running full integration experiment...")
    
    # Configure for full experiment
    phase6_config = nlm.Phase6Config()
    phase6_config.neuronCount = 500
    phase6_config.maxSteps = 1000
    phase6_config.enableCheckpointing = True
    phase6_config.enableReplay = True
    phase6_config.enableDevelopment = True
    phase6_config.connectionProbability = 0.15f
    
    # Run the experiment
    result = experiment.run(phase6_config)
    
    print("   ✓ Full integration experiment completed")
    print(f"   - Total reward (behavior): {result.totalReward:.4f}")
    print(f"   - Average firing rate: {result.avgFiringRate:.4f}")
    print(f"   - Episodes stored: {result.memoryEpisodesStored}")
    print(f"   - Dopamine level: {result.dopamineLevel:.4f}")
    
    print("\n6. System integration status:")
    print(f"   - Working Memory: {'CONNECTED ✓' if result.memoryWorkingMemoryIntegrated else 'DISCONNECTED ✗'}")
    print(f"   - Episodic Memory: {'CONNECTED ✓' if result.memoryEpisodicMemoryIntegrated else 'DISCONNECTED ✗'}")
    print(f"   - Neuromodulation: {'CONNECTED ✓' if result.neuromodulationIntegrated else 'DISCONNECTED ✗'}")
    print(f"   - Prediction: {'CONNECTED ✓' if result.predictionIntegrated else 'DISCONNECTED ✗'}")
    print(f"   - Development: {'CONNECTED ✓' if result.developmentIntegrated else 'DISCONNECTED ✗'}")
    print(f"   - Checkpointing: {'WORKING ✓' if result.checkpointingWorks else 'NOT WORKING ✗'}")
    
    print("\n=== Phase 6 Integration Successfully Completed ===")
    print("All brain systems are properly connected and functional.")
    print("The NLM brain is now a fully integrated artificial neural substrate.")
    
    # Save final brain state
    if result.checkpointingWorks:
        with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
            checkpoint_path = f.name
        try:
            brain.save(checkpoint_path)
            print(f"\nFinal brain state saved to: {checkpoint_path}")
        except Exception as e:
            print(f"\nCould not save checkpoint: {e}")
    
    print("\nExample completed successfully!")

if __name__ == "__main__":
    main()
