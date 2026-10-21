#!/usr/bin/env python3
"""
Simple test to verify NLM Python bindings work
"""

import sys
import pynlm

def test_imports():
    """Test that all required modules can be imported"""
    print("=== Testing Imports ===")
    
    try:
        # Test basic imports
        config = pynlm.createDefaultConfig()
        print("✓ Imported pynlm successfully")
        
        # Test creating brain
        brain = pynlm.createBrain(config)
        print("✓ Created brain")
        
        # Test creating world
        world = pynlm.createSimpleWorld()
        print("✓ Created world")
        
        # Test creating agent
        agent = pynlm.createAgentBrain(brain)
        print("✓ Created agent brain")
        
        return brain, world, agent
        
    except Exception as e:
        print(f"❌ Import test failed: {e}")
        return None, None, None

def test_basic_functionality(brain, world, agent):
    """Test basic NLM functionality"""
    print("\n=== Testing Basic Functionality ===")
    
    try:
        # Initialize brain
        brain.initialize()
        print("✓ Initialized brain")
        
        # Configure world
        world.configure(10, 10, 8, 8)
        world.reset()
        print("✓ Configured world")
        
        # Initialize agent
        agent.initialize(world)
        print("✓ Initialized agent")
        
        # Test simulation
        for i in range(5):
            brain.step(i)
            world.update(0.1)
        
        print(f"✓ Completed simulation with {brain.getTotalSpikeCount()} spikes")
        
        return True
        
    except Exception as e:
        print(f"❌ Basic functionality test failed: {e}")
        return False

def test_enhanced_features(brain):
    """Test enhanced Python API features"""
    print("\n=== Testing Enhanced Features ===")
    
    try:
        # Test brain info
        brain_info = brain.getBrainInfo()
        print("✓ Brain info accessible:", type(brain_info))
        
        # Test simulation time
        sim_time = brain.getSimulationTime()
        sim_step = brain.getSimulationStep()
        print(f"✓ Simulation time: {sim_time}, step: {sim_step}")
        
        # Test random seed
        brain.setRandomSeed(42)
        seed = brain.getRandomSeed()
        print(f"✓ Random seed: {seed}")
        
        # Test regions
        region_id = brain.addRegion("test")
        print(f"✓ Added region: {region_id}")
        
        return True
        
    except Exception as e:
        print(f"❌ Enhanced features test failed: {e}")
        return False

def test_advanced_features(brain):
    """Test advanced Python API features"""
    print("\n=== Testing Advanced Features ===")
    
    try:
        # Test advanced configuration template
        template = pynlm.getConfigurationTemplate("minimal")
        print("✓ Configuration template accessible:", type(template))
        
        # Test configuration validation
        config = pynlm.createDefaultConfig()
        validation = pynlm.validateConfiguration(config)
        print("✓ Configuration validation accessible:", type(validation))
        
        # Test diagnostic tools
        diagnostics = pynlm.diagnosticBrain(brain, ["performance"])
        print("✓ Diagnostic tools accessible:", type(diagnostics))
        
        # Test system information
        system_info = pynlm.getNLMInfo()
        print("✓ System information accessible:", type(system_info))
        
        return True
        
    except Exception as e:
        print(f"❌ Advanced features test failed: {e}")
        return False

def main():
    """Run all tests"""
    print("NLM Python API - Quick Verification Test")
    print("=" * 50)
    
    # Test imports
    brain, world, agent = test_imports()
    if brain is None:
        sys.exit(1)
    
    # Test basic functionality
    if not test_basic_functionality(brain, world, agent):
        sys.exit(1)
    
    # Test enhanced features
    if not test_enhanced_features(brain):
        sys.exit(1)
    
    # Test advanced features
    if not test_advanced_features(brain):
        sys.exit(1)
    
    print("\n" + "=" * 50)
    print("✅ ALL VERIFICATION TESTS PASSED!")
    print("NLM Python API improvements are working correctly.")
    print("=" * 50)
    
    return 0

if __name__ == "__main__":
    sys.exit(main())