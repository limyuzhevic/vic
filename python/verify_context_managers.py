#!/usr/bin/env python3
"""
Quick verification script for NLM Python context managers.

This script performs a basic verification that the context managers
are properly integrated and working as expected.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

def check_imports():
    """Check that all required modules can be imported."""
    print("Checking imports...")
    try:
        import pynlm as nlm
        print("✓ pynlm imported successfully")
        
        # Check that key classes are available
        assert hasattr(nlm, 'Config'), "Config class not found"
        assert hasattr(nlm, 'Brain'), "Brain class not found"
        assert hasattr(nlm, 'AgentBrain'), "AgentBrain class not found"
        assert hasattr(nlm, 'SimpleWorld'), "SimpleWorld class not found"
        assert hasattr(nlm, 'SensoryInput'), "SensoryInput class not found"
        assert hasattr(nlm, 'Action'), "Action class not found"
        assert hasattr(nlm, 'Vision'), "Vision class not found"
        assert hasattr(nlm, 'Audio'), "Audio class not found"
        
        print("✓ All required classes available")
        return nlm
        
    except ImportError as e:
        print(f"❌ Import error: {e}")
        return None

def test_config_context_manager(nlm):
    """Test Config context manager."""
    print("\nTesting Config context manager...")
    try:
        with nlm.Config() as config:
            config.set("test.key", "test_value")
            config.set("test.number", 42)
            
            assert config.has("test.key")
            assert config.getOr("test.key", "") == "test_value"
            assert config.getOr("test.number", 0) == 42
            
        print("✓ Config context manager works correctly")
        return True
    except Exception as e:
        print(f"❌ Config test failed: {e}")
        return False

def test_brain_context_manager(nlm):
    """Test Brain context manager."""
    print("\nTesting Brain context manager...")
    try:
        with nlm.Config() as config:
            with nlm.Brain(config) as brain:
                brain.initialize()
                region_id = brain.addRegion("test")
                assert brain.getRegionCount() >= 1
                
        print("✓ Brain context manager works correctly")
        return True
    except Exception as e:
        print(f"❌ Brain test failed: {e}")
        return False

def test_simpleworld_context_manager(nlm):
    """Test SimpleWorld context manager."""
    print("\nTesting SimpleWorld context manager...")
    try:
        with nlm.SimpleWorld() as world:
            world.configure(100.0, 100.0, 10, 10)
            world.setAgentStart(50.0, 50.0)
            
            for step in range(3):
                world.update(0.1)
                
            percept = world.getSensoryPercept()
            assert percept is not None
            
        print("✓ SimpleWorld context manager works correctly")
        return True
    except Exception as e:
        print(f"❌ SimpleWorld test failed: {e}")
        return False

def test_agentbrain_context_manager(nlm):
    """Test AgentBrain context manager."""
    print("\nTesting AgentBrain context manager...")
    try:
        with nlm.Config() as config:
            with nlm.Brain(config) as brain:
                brain.initialize()
                with nlm.SimpleWorld() as world:
                    world.configure(50.0, 50.0, 5, 5)
                    with nlm.AgentBrain(brain) as agent:
                        agent.initialize(world)
                        action = agent.decodeMotorCommand()
                        assert action is not None
                        
        print("✓ AgentBrain context manager works correctly")
        return True
    except Exception as e:
        print(f"❌ AgentBrain test failed: {e}")
        return False

def test_sensoryinput_context_manager(nlm):
    """Test SensoryInput context manager."""
    print("\nTesting SensoryInput context manager...")
    try:
        with nlm.Vision(10, 10) as vision:
            data = [0.1] * (10 * 10 * 3)
            vision.setData(data)
            assert vision.getWidth() == 10
            assert vision.getHeight() == 10
            
        print("✓ SensoryInput context manager works correctly")
        return True
    except Exception as e:
        print(f"❌ SensoryInput test failed: {e}")
        return False

def test_action_context_manager(nlm):
    """Test Action context manager."""
    print("\nTesting Action context manager...")
    try:
        with nlm.Action(nlm.ActionType.MoveForward) as action:
            action.setParameters([1.0, 0.5])
            assert action.getType() == nlm.ActionType.MoveForward
            assert action.getName() == "MoveForward"
            
        print("✓ Action context manager works correctly")
        return True
    except Exception as e:
        print(f"❌ Action test failed: {e}")
        return False

def main():
    """Run all verification tests."""
    print("NLM Python Context Managers - Quick Verification")
    print("=" * 60)
    
    # Check imports
    nlm = check_imports()
    if not nlm:
        sys.exit(1)
    
    # Run tests
    tests = [
        ("Config", test_config_context_manager),
        ("Brain", test_brain_context_manager),
        ("SimpleWorld", test_simpleworld_context_manager),
        ("AgentBrain", test_agentbrain_context_manager),
        ("SensoryInput", test_sensoryinput_context_manager),
        ("Action", test_action_context_manager),
    ]
    
    passed = 0
    total = len(tests)
    
    for name, test_func in tests:
        if test_func(nlm):
            passed += 1
    
    print("\n" + "=" * 60)
    print(f"Results: {passed}/{total} tests passed")
    print("=" * 60)
    
    if passed == total:
        print("🎉 All verification tests passed!")
        return True
    else:
        print("❌ Some tests failed")
        return False

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)
