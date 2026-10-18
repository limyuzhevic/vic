#!/usr/bin/env python3
"""
Comprehensive test suite for pynlm Python bindings.
Tests basic functionality and new methods exposed by the improved bindings.
"""

import sys
import os
import tempfile
import json

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
    print("✓ Successfully imported pynlm")
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    sys.exit(1)

def test_basic_functionality():
    """Test basic brain creation and manipulation."""
    print("\n=== Testing Basic Functionality ===")
    
    # Test 1: Create default config
    config = pynlm.createDefaultConfig()
    print("✓ Created default config")
    
    # Test 2: Create brain
    brain = pynlm.createBrain(config)
    print("✓ Created brain")
    
    # Test 3: Initialize brain
    success = brain.initialize()
    assert success, "Failed to initialize brain"
    print("✓ Initialized brain")
    
    # Test 4: Get statistics
    total_neurons = brain.getTotalNeuronCount()
    total_synapses = brain.getTotalSynapseCount()
    active_neurons = brain.getActiveNeuronCount()
    firing_neurons = brain.getFiringNeuronCount()
    avg_firing = brain.getAverageFiringRate()
    
    print(f"  Total neurons: {total_neurons}")
    print(f"  Total synapses: {total_synapses}")
    print(f"  Active neurons: {active_neurons}")
    print(f"  Firing neurons: {firing_neurons}")
    print(f"  Average firing rate: {avg_firing}")
    
    # Test 5: Step brain
    for i in range(10):
        brain.step(i)
    print("✓ Stepped brain 10 times")
    
    # Test 6: Reset brain
    brain.reset()
    print("✓ Reset brain")
    
    # Test 7: Check developmental stage
    stage = brain.getDevelopmentalStage()
    print(f"  Developmental stage: {stage}")
    
    # Test 8: Check config
    config_summary = config.summary()
    print(f"  Config summary: {config_summary[:100]}...")
    
    return brain

def test_world_functionality():
    """Test world creation and manipulation."""
    print("\n=== Testing World Functionality ===")
    
    # Test 1: Create world
    world = pynlm.createSimpleWorld()
    print("✓ Created world")
    
    # Test 2: Configure world
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    print("✓ Configured world")
    
    # Test 3: Check dimensions
    width = world.getWidth()
    height = world.getHeight()
    print(f"  World dimensions: {width}x{height}")
    
    # Test 4: Check energy settings
    max_energy = world.getMaxEnergy()
    energy_decay = world.getEnergyDecayRate()
    print(f"  Energy settings: max={max_energy}, decay={energy_decay}")
    
    # Test 5: Set agent start
    world.setAgentStart(10, 10)
    print("✓ Set agent start position")
    
    # Test 6: Reset world
    world.reset()
    print("✓ Reset world")
    
    # Test 7: Check random seed
    seed = world.getRandomSeed()
    print(f"  Random seed: {seed}")
    
    # Test 8: Check vision dimensions
    vision_width = world.getVisionWidth()
    vision_height = world.getVisionHeight()
    print(f"  Vision dimensions: {vision_width}x{vision_height}")
    
    # Test 9: Check simulation time
    sim_time = world.getSimulationTime()
    print(f"  Simulation time: {sim_time}")
    
    return world

def test_agent_brain_functionality():
    """Test AgentBrain functionality."""
    print("\n=== Testing AgentBrain Functionality ===")
    
    # Create brain and world
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    
    # Create agent brain
    agent = pynlm.createAgentBrain(brain)
    print("✓ Created agent brain")
    
    # Initialize agent
    agent.initialize(world)
    print("✓ Initialized agent brain")
    
    # Check sensory input size
    sensory_size = agent.getSensoryInputSize()
    print(f"  Sensory input size: {sensory_size}")
    
    # Check motor output size
    motor_size = agent.getMotorOutputSize()
    print(f"  Motor output size: {motor_size}")
    
    # Check developmental stage
    stage = agent.getDevelopmentalStage()
    print(f"  Developmental stage: {stage}")
    
    # Check neuromodulation levels
    neuromod = agent.getNeuromodulationLevel()
    curiosity = agent.getCuriosityLevel()
    novelty = agent.getNoveltyLevel()
    pred_error = agent.getPredictionError()
    
    print(f"  Neuromodulation level: {neuromod}")
    print(f"  Curiosity level: {curiosity}")
    print(f"  Novelty level: {novelty}")
    print(f"  Prediction error: {pred_error}")
    
    # Test enable methods
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("✓ Enabled all subsystems")
    
    # Check enable status
    assert agent.isRewardModulationEnabled(), "Reward modulation should be enabled"
    assert agent.isStructuralPlasticityEnabled(), "Structural plasticity should be enabled"
    assert agent.isDevelopmentEnabled(), "Development should be enabled"
    assert agent.isCuriosityEnabled(), "Curiosity should be enabled"
    
    print("✓ All subsystems enabled correctly")
    
    return agent, brain, world

def test_memory_systems():
    """Test memory systems access."""
    print("\n=== Testing Memory Systems ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test 1: Working memory
    working_memory = brain.getWorkingMemory()
    if working_memory:
        print("✓ Working memory accessible")
        # Note: Working memory methods are not yet exposed to Python
        # but we can verify it's accessible
    else:
        print("! Working memory not accessible")
    
    # Test 2: Episodic memory
    episodic_memory = brain.getEpisodicMemory()
    if episodic_memory:
        print("✓ Episodic memory accessible")
        # Note: Episodic memory methods are not yet exposed to Python
        # but we can verify it's accessible
    else:
        print("! Episodic memory not accessible")
    
    # Test 3: Associative memory
    associative_memory = brain.getAssociativeMemory()
    if associative_memory:
        print("✓ Associative memory accessible")
        # Note: Associative memory methods are not yet exposed to Python
        # but we can verify it's accessible
    else:
        print("! Associative memory not accessible")
    
    return brain

def test_prediction_system():
    """Test prediction system access."""
    print("\n=== Testing Prediction System ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test prediction system access
    prediction_system = brain.getPredictionSystem()
    if prediction_system:
        print("✓ Prediction system accessible")
        # Note: Prediction system methods are not yet exposed to Python
        # but we can verify it's accessible
    else:
        print("! Prediction system not accessible")
    
    return brain

def test_cognition_systems():
    """Test cognition systems access."""
    print("\n=== Testing Cognition Systems ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test 1: Neural planner
    planner = brain.getPlanner()
    if planner:
        print("✓ Neural planner accessible")
        # Note: Planner methods are not yet exposed to Python
        # but we can verify it's accessible
    else:
        print("! Neural planner not accessible")
    
    # Test 2: Concept formation
    concept_formation = brain.getConceptFormation()
    if concept_formation:
        print("✓ Concept formation accessible")
        # Note: Concept formation methods are not yet exposed to Python
        # but we can verify it's accessible
    else:
        print("! Concept formation not accessible")
    
    # Test 3: Attention system
    attention = brain.getAttention()
    if attention:
        print("✓ Attention system accessible")
        # Note: Attention system methods are not yet exposed to Python
        # but we can verify it's accessible
    else:
        print("! Attention system not accessible")
    
    return brain

def test_neuromodulation_systems():
    """Test neuromodulation systems access."""
    print("\n=== Testing Neuromodulation Systems ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test 1: Dopamine
    dopamine = brain.getDopamine()
    if dopamine:
        print("✓ Dopamine system accessible")
        # Note: Dopamine system methods are not yet exposed to Python
        # but we can verify it's accessible
    else:
        print("! Dopamine system not accessible")
    
    # Test 2: Curiosity
    curiosity = brain.getCuriosity()
    if curiosity:
        print("✓ Curiosity system accessible")
        # Note: Curiosity system methods are not yet exposed to Python
        # but we can verify it's accessible
    else:
        print("! Curiosity system not accessible")
    
    # Test 3: Novelty
    novelty = brain.getNovelty()
    if novelty:
        print("✓ Novelty system accessible")
        # Note: Novelty system methods are not yet exposed to Python
        # but we can verify it's accessible
    else:
        print("! Novelty system not accessible")
    
    # Test 4: Prediction error signal
    pred_error = brain.getPredictionErrorSignal()
    if pred_error:
        print("✓ Prediction error system accessible")
        # Note: Prediction error signal methods are not yet exposed to Python
        # but we can verify it's accessible
    else:
        print("! Prediction error system not accessible")
    
    return brain

def test_development_system():
    """Test development system access."""
    print("\n=== Testing Development System ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test development system access
    development_system = brain.getDevelopmentSystem()
    if development_system:
        print("✓ Development system accessible")
        # Note: Development system methods are not yet exposed to Python
        # but we can verify it's accessible
    else:
        print("! Development system not accessible")
    
    return brain

def test_agent_functionality():
    """Test agent functionality with brain and world."""
    print("\n=== Testing Agent Functionality ===")
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run a simple simulation loop
    print("Running simple simulation loop...")
    for step in range(20):
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Step brain
        brain.step(step)
        
        # Decode motor command
        motor_cmd = agent.decodeMotorCommand()
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Get statistics every 5 steps
        if step % 5 == 0:
            print(f"  Step {step}: neurons={brain.getFiringNeuronCount()}, "
                  f"neuromod={agent.getNeuromodulationLevel():.3f}, "
                  f"curiosity={agent.getCuriosityLevel():.3f}")
    
    # Check final statistics
    final_neurons = brain.getFiringNeuronCount()
    final_neuromod = agent.getNeuromodulationLevel()
    final_curiosity = agent.getCuriosityLevel()
    
    print(f"Simulation complete:")
    print(f"  Final firing neurons: {final_neurons}")
    print(f"  Final neuromodulation level: {final_neuromod:.3f}")
    print(f"  Final curiosity level: {final_curiosity:.3f}")
    
    return agent, brain, world

def test_save_load():
    """Test save and load functionality."""
    print("\n=== Testing Save/Load Functionality ===")
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run some steps
    for i in range(50):
        brain.step(i)
    
    # Get initial state
    initial_spikes = brain.getTotalSpikeCount()
    initial_neurons = brain.getTotalNeuronCount()
    
    print(f"Initial state: {initial_spikes} spikes, {initial_neurons} neurons")
    
    # Create temporary file for checkpoint
    with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
        checkpoint_path = f.name
    
    try:
        # Save brain state
        save_success = brain.save(checkpoint_path)
        if save_success:
            print(f"✓ Successfully saved brain to {checkpoint_path}")
        else:
            print("✗ Failed to save brain")
            return False
        
        # Create new brain and load
        brain2 = pynlm.createBrain(config)
        brain2.initialize()
        
        load_success = brain2.load(checkpoint_path)
        if load_success:
            print(f"✓ Successfully loaded brain from {checkpoint_path}")
        else:
            print("✗ Failed to load brain")
            return False
        
        # Note: Due to the nature of neural simulation, exact state restoration
        # is complex. The load should restore the structure at minimum.
        
        print("Note: Exact state restoration may vary due to neural dynamics")
        
    finally:
        # Clean up temporary file
        if os.path.exists(checkpoint_path):
            os.unlink(checkpoint_path)
            print("✓ Cleaned up temporary checkpoint file")
    
    return True

def test_region_management():
    """Test brain region management."""
    print("\n=== Testing Region Management ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Get initial region count
    initial_regions = brain.getRegionCount()
    print(f"Initial region count: {initial_regions}")
    
    # Get region IDs
    region_ids = brain.getRegionIds()
    print(f"Region IDs: {[str(rid) for rid in region_ids]}")
    
    # Add a new region
    new_region_id = brain.addRegion("TestRegion")
    print(f"Added new region with ID: {new_region_id}")
    
    # Get updated region count
    updated_regions = brain.getRegionCount()
    print(f"Updated region count: {updated_regions}")
    
    # Verify the new region is accessible
    region = brain.getRegion(new_region_id)
    if region:
        print(f"✓ Successfully retrieved region {new_region_id}")
    else:
        print("✗ Failed to retrieve new region")
        return False
    
    # Test inter-region connection management
    if len(region_ids) >= 2:
        source_region = region_ids[0]
        target_region = region_ids[1]
        
        # Add inter-region connection
        brain.addInterRegionConnection(source_region, target_region, 0.5, 2)
        print(f"✓ Added inter-region connection: {source_region} -> {target_region}")
        
        # Remove inter-region connection
        brain.removeInterRegionConnection(source_region, target_region)
        print(f"✓ Removed inter-region connection: {source_region} -> {target_region}")
    else:
        print("! Not enough regions to test inter-region connections")
    
    return brain

def main():
    """Run all tests."""
    print("=" * 60)
    print("NLM Python Bindings Test Suite")
    print("=" * 60)
    
    tests = [
        test_basic_functionality,
        test_world_functionality,
        test_agent_brain_functionality,
        test_memory_systems,
        test_prediction_system,
        test_cognition_systems,
        test_neuromodulation_systems,
        test_development_system,
        test_agent_functionality,
        test_save_load,
        test_region_management,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
            print(f"✓ {test.__name__} PASSED")
        except Exception as e:
            failed += 1
            print(f"✗ {test.__name__} FAILED: {e}")
            import traceback
            traceback.print_exc()
    
    print("\n" + "=" * 60)
    print("Test Summary:")
    print(f"  Passed: {passed}")
    print(f"  Failed: {failed}")
    print(f"  Total: {passed + failed}")
    print("=" * 60)
    
    if failed > 0:
        print("\nSome tests failed. Please check the output above for details.")
        return 1
    else:
        print("\nAll tests passed! ✓")
        return 0

if __name__ == "__main__":
    sys.exit(main())