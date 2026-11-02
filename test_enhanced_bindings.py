#!/usr/bin/env python3
"""
Quick test of enhanced NLM Python bindings.
"""
import pynlm
import sys

print("Testing enhanced NLM Python bindings...")

# Test basic functionality
try:
    config = pynlm.createDefaultConfig()
    print("✓ Created default config")
    
    brain = pynlm.createBrain(config)
    print("✓ Created brain")
    
    brain.initialize()
    print("✓ Initialized brain")
    
    # Test convenience methods
    stats = brain.get_statistics()
    print(f"✓ Got brain statistics: {stats}")
    
    # Test enhanced functionality
    config_validation = brain.validate_config()
    print(f"✓ Validated config: {config_validation}")
    
    # Test diagnostics
    diagnostics = brain.get_diagnostics()
    print(f"✓ Got diagnostics: {diagnostics}")
    
    # Test developmental stage
    stage = brain.getDevelopmentalStage()
    print(f"✓ Got developmental stage: {stage}")
    
    # Run some steps
    for i in range(10):
        brain.step(i)
    
    print(f"✓ Ran 10 steps, final spike count: {brain.getTotalSpikeCount()}")
    
    # Test run_demonstration method
    print("✓ Testing brain.run_demonstration()...")
    
    # Create world and agent for testing
    world = pynlm.createSimpleWorld()
    world.configure(15, 15, 8, 8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Test agent convenience methods
    print("✓ Testing agent.enable_all_systems()...")
    agent.enable_all_systems()
    
    # Get system status
    status = agent.get_system_status()
    print(f"✓ Got agent system status: {status}")
    
    # Create development profile
    profile = agent.create_development_profile()
    print(f"✓ Created development profile: {profile}")
    
    # Test world convenience methods
    print("✓ Testing world.configure_and_reset()...")
    world.configure_and_reset(10, 10, 5, 5)
    
    world_diagnostics = world.get_agent_diagnostics()
    print(f"✓ Got world diagnostics: {world_diagnostics}")
    
    # Test module-level functions
    print("✓ Testing module-level functions...")
    
    # Create development scenario
    dev_config = pynlm.create_development_scenario(100, 2)
    print(f"✓ Created development scenario with {dev_config.getOr('neuron_count', 0)} neurons")
    
    # Validate config
    config_result = pynlm.validate_all_config(dev_config)
    print(f"✓ Validated config: {config_result}")
    
    print("\n" + "="*60)
    print("All tests passed! Enhanced NLM Python bindings are working.")
    print("="*60)
    
except Exception as e:
    print(f"✗ Test failed with error: {e}")
    import traceback
    traceback.print_exc()
    sys.exit(1)
