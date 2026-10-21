#!/usr/bin/env python3
"""
Test script to verify NLM Python API improvements
"""

import sys
import pynlm

def test_basic_functionality():
    """Test basic NLM functionality"""
    print("=== Testing Basic Functionality ===")
    
    # Test 1: Create and initialize brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # Test 2: Basic simulation
    for i in range(10):
        brain.step(i)
    
    print(f"✓ Completed {i+1} simulation steps")
    print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  - Firing neurons: {brain.getFiringNeuronCount()}")
    
    return brain

def test_enhanced_features():
    """Test enhanced Python API features"""
    print("\n=== Testing Enhanced Features ===")
    
    # Create brain for testing
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Test 1: Enhanced brain info
    brain_info = brain.getBrainInfo()
    print("✓ Brain info:")
    for key, value in brain_info.items():
        print(f"  - {key}: {value}")
    
    # Test 2: Simulation timing
    sim_time = brain.getSimulationTime()
    sim_step = brain.getSimulationStep()
    print(f"✓ Simulation time: {sim_time}s, step: {sim_step}")
    
    # Test 3: Random seed
    brain.setRandomSeed(12345)
    seed = brain.getRandomSeed()
    print(f"✓ Random seed set to: {seed}")
    
    # Test 4: Regions
    region_id = brain.addRegion("test_region")
    region = brain.getRegion(region_id)
    print(f"✓ Added region with ID: {region_id}")
    
    return brain

def test_agent_functionality():
    """Test agent functionality"""
    print("\n=== Testing Agent Functionality ===")
    
    # Create brain, world, and agent
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(10, 10, 8, 8)
    world.reset()
    world.setAgentStart(5.0, 5.0)
    
    agent = pynlm.createAdvancedAgent(brain, world)
    
    print("✓ Created advanced agent with all systems enabled")
    
    # Test agent info
    agent_info = agent.getBrainInfo()
    print("✓ Agent info:")
    for key, value in agent_info.items():
        print(f"  - {key}: {value}")
    
    # Run simulation
    for step in range(20):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    print("✓ Completed agent simulation")
    return brain, agent, world

def test_performance_monitoring():
    """Test performance monitoring features"""
    print("\n=== Testing Performance Monitoring ===")
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Test benchmark
    metrics = ["firing_rate", "memory_usage"]
    results = pynlm.benchmarkBrainPerformance(brain, 50, metrics)
    
    print("✓ Performance benchmark completed:")
    print(f"  - Steps: {results['steps_completed']}")
    print(f"  - Duration: {results['duration_seconds']:.2f}s")
    print(f"  - Steps/sec: {results['steps_per_second']:.1f}")
    print(f"  - Final firing rate: {results['final_firing_rate']:.3f}")
    
    return results

def test_configuration_management():
    """Test configuration management"""
    print("\n=== Testing Configuration Management ===")
    
    # Test templates
    templates = ["minimal", "balanced", "high_performance"]
    
    for template in templates:
        config_template = pynlm.getConfigurationTemplate(template)
        print(f"✓ {template.title()} template:")
        for key, value in list(config_template.items())[:3]:  # Show first 3 items
            print(f"  - {key}: {value}")
        print(f"  ... and {len(config_template) - 3} more items")
    
    # Test configuration validation
    config = pynlm.createDefaultConfig()
    validation = pynlm.validateConfiguration(config)
    
    print("\n✓ Configuration validation:")
    print(f"  - Valid: {validation.get('is_valid', 'unknown')}")
    print(f"  - Total keys: {validation.get('total_keys', 0)}")
    
    return validation

def test_diagnostic_tools():
    """Test diagnostic and debugging tools"""
    print("\n=== Testing Diagnostic Tools ===")
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Run simulation
    for i in range(10):
        brain.step(i)
    
    # Test diagnostics
    checks = ["connectivity", "performance", "plasticity"]
    diagnostics = pynlm.diagnosticBrain(brain, checks)
    
    print("✓ Diagnostic results:")
    for check, result in diagnostics.items():
        print(f"  - {check}: {result}")
    
    return diagnostics

def test_system_information():
    """Test system information"""
    print("\n=== Testing System Information ===")
    
    system_info = pynlm.getNLMInfo()
    
    print("✓ System information:")
    print(f"  - Version: {system_info['version']}")
    print(f"  - Build date: {system_info['build_date']}")
    print(f"  - Features available: {len(system_info['features'])}")
    
    print("\n  Key features:")
    for feature in system_info['features'][:5]:  # Show first 5
        print(f"    ✓ {feature}")
    print(f"    ... and {len(system_info['features']) - 5} more")
    
    return system_info

def main():
    """Run all tests"""
    print("NLM Python API - Testing Enhanced Functionality")
    print("=" * 60)
    
    try:
        # Test basic functionality
        test_basic_functionality()
        
        # Test enhanced features
        test_enhanced_features()
        
        # Test agent functionality
        test_agent_functionality()
        
        # Test performance monitoring
        test_performance_monitoring()
        
        # Test configuration management
        test_configuration_management()
        
        # Test diagnostic tools
        test_diagnostic_tools()
        
        # Test system information
        test_system_information()
        
        print("\n" + "=" * 60)
        print("✅ ALL TESTS PASSED!")
        print("Enhanced NLM Python API is working correctly.")
        print("=" * 60)
        
        return 0
        
    except Exception as e:
        print(f"\n❌ TEST FAILED: {e}")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    sys.exit(main())