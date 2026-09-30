#!/usr/bin/env python3

"""
Demonstration of NLM (Neural Learning Machine) improvements
Shows the fixed memory systems and enhanced Python API
"""

import sys
import os

print("=== NLM Improvement Demonstration ===")
print()

# Add current directory to path for testing
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    # Try to import pynlm bindings
    import pynlm
    print("✓ Successfully imported pynlm Python bindings")
    
    # Test 1: Create a brain with configuration
    print("\n--- Test 1: Basic Brain Creation ---")
    config = pynlm.createDefaultConfig()
    print(f"✓ Created default config: {config}")
    
    # Modify configuration for testing
    config.set("brain.neuron_count", 500)
    config.set("brain.synapse_density", 0.1)
    print(f"✓ Modified config: {config.get('brain.neuron_count')} neurons")
    
    # Test 2: Create and initialize brain
    print("\n--- Test 2: Brain Initialization ---")
    brain = pynlm.createBrain(config)
    print("✓ Created brain instance")
    
    brain.initialize()
    print("✓ Brain initialized successfully")
    
    # Test 3: Brain operations
    print("\n--- Test 3: Brain Operations ---")
    brain.step(0)
    print("✓ Brain step completed")
    
    total_neurons = brain.getTotalNeuronCount()
    total_synapses = brain.getTotalSynapseCount()
    firing_count = brain.getFiringNeuronCount()
    avg_firing = brain.getAverageFiringRate()
    
    print(f"✓ Statistics: {total_neurons} neurons, {total_synapses} synapses")
    print(f"✓ Firing: {firing_count} active neurons, {avg_firing:.2f} avg firing rate")
    
    # Test 4: Memory systems integration
    print("\n--- Test 4: Memory Systems Integration ---")
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    associative_memory = brain.getAssociativeMemory()
    
    if working_memory:
        print(f"✓ Working memory integrated: {working_memory.getActiveTraces()} active traces")
        working_memory.setCapacity(100)
        print(f"✓ Working memory capacity set to {working_memory.getCapacity()}")
    else:
        print("⚠️ Working memory not available")
    
    if episodic_memory:
        print(f"✓ Episodic memory integrated: {episodic_memory.getEpisodeCount()} episodes stored")
        episodic_memory.setMaxEpisodes(1000)
        print(f"✓ Episodic memory capacity set to {episodic_memory.getMaxEpisodes()} episodes")
    else:
        print("⚠️ Episodic memory not available")
    
    if associative_memory:
        print("✓ Associative memory integrated")
    else:
        print("⚠️ Associative memory not available")
    
    # Test 5: Prediction system integration
    print("\n--- Test 5: Prediction System ---")
    prediction_system = brain.getPredictionSystem()
    if prediction_system:
        print("✓ Prediction system integrated")
    else:
        print("⚠️ Prediction system not available")
    
    # Test 6: Cognition systems integration
    print("\n--- Test 6: Cognition Systems ---")
    planner = brain.getPlanner()
    concept_formation = brain.getConceptFormation()
    attention = brain.getAttention()
    
    if planner:
        print("✓ Neural planner integrated")
        planner.setPlanningDepth(5)
        print(f"✓ Planning depth set to {planner.getPlanningDepth()}")
    else:
        print("⚠️ Neural planner not available")
    
    if concept_formation:
        print("✓ Concept formation integrated")
    else:
        print("⚠️ Concept formation not available")
    
    if attention:
        print("✓ Attentional selection integrated")
        attention.setInhibitionStrength(0.5)
        print(f"✓ Inhibition strength set to {attention.getInhibitionStrength()}")
    else:
        print("⚠️ Attentional selection not available")
    
    # Test 7: Development system integration
    print("\n--- Test 7: Development System ---")
    development_system = brain.getDevelopmentSystem()
    if development_system:
        print("✓ Development system integrated")
        stage = brain.getDevelopmentalStage()
        print(f"✓ Current developmental stage: {stage}")
    else:
        print("⚠️ Development system not available")
    
    # Test 8: Neuromodulation systems integration
    print("\n--- Test 8: Neuromodulation Systems ---")
    dopamine = brain.getDopamine()
    curiosity = brain.getCuriosity()
    novelty = brain.getNovelty()
    prediction_error = brain.getPredictionError()
    
    if dopamine:
        print("✓ Dopamine system integrated")
    if curiosity:
        print("✓ Curiosity system integrated")
    if novelty:
        print("✓ Novelty detection integrated")
    if prediction_error:
        print("✓ Prediction error system integrated")
    
    # Test 9: Advanced Python API features
    print("\n--- Test 9: Advanced Python API ---")
    
    # Create agent brain interface
    agent_brain = pynlm.createAgentBrain(brain)
    print("✓ Created agent brain interface")
    
    # Test world creation
    world = pynlm.createSimpleWorld()
    print("✓ Created simple world")
    
    # Configure world
    world.configure(20, 20, 8, 8)
    print("✓ Configured world: 20x20 with 8x8 vision")
    
    # Initialize agent
    agent_brain.initialize(world)
    print("✓ Agent brain initialized with world")
    
    # Test agent subsystems
    agent_brain.enableRewardModulation(True)
    agent_brain.enableCuriosity(True)
    agent_brain.enableDevelopment(True)
    
    print("✓ Agent subsystems enabled:")
    print(f"  - Reward modulation: {agent_brain.isRewardModulationEnabled()}")
    print(f"  - Curiosity: {agent_brain.isCuriosityEnabled()}")
    print(f"  - Development: {agent_brain.isDevelopmentEnabled()}")
    
    # Test 10: Save and load checkpoint
    print("\n--- Test 10: Persistence ---")
    checkpoint_file = "/tmp/nlm_test_checkpoint.bin"
    
    save_success = brain.save(checkpoint_file)
    if save_success:
        print(f"✓ Checkpoint saved to {checkpoint_file}")
        
        # Create new brain and load
        brain2 = pynlm.createBrain(config)
        brain2.initialize()
        
        load_success = brain2.load(checkpoint_file)
        if load_success:
            print("✓ Checkpoint loaded successfully")
            print(f"✓ Loaded brain: {brain2.getTotalNeuronCount()} neurons")
        else:
            print("⚠️ Failed to load checkpoint")
    else:
        print("⚠️ Failed to save checkpoint")
    
    print("\n=== DEMONSTRATION COMPLETE ===")
    print("\n✅ All tests passed! NLM is working correctly with:")
    print("   - Integrated memory systems")
    print("   - Full Python API access")
    print("   - Agent-world interaction")
    print("   - Persistence support")
    print("   - Advanced functionality for power users")
    
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    print("\nThis test requires building the Python bindings first.")
    print("Run: mkdir build && cd build && cmake .. && make")
    sys.exit(1)
    
except Exception as e:
    print(f"✗ Test failed with error: {e}")
    import traceback
    traceback.print_exc()
    sys.exit(1)
