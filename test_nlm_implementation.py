#!/usr/bin/env python3
"""
Test script for NLM Python package implementation.

This script demonstrates the new functionality added to the NLM Python package,
verifying that the convenience functions, examples, and API improvements work correctly.

The test script validates:
1. Basic import functionality
2. Convenience functions availability
3. Example script retrieval
4. Quick reference access
5. Feature enable functionality
6. API compatibility with easy_usage.md examples

Usage:
    python test_nlm_implementation.py
"""

import sys
import os

# Add the current directory to the path so we can import nlm
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

def test_basic_import():
    """Test that we can import the nlm package."""
    try:
        import nlm
        print("✅ Successfully imported nlm package")
        return True
    except ImportError as e:
        print(f"❌ Failed to import nlm: {e}")
        return False

def test_convenience_functions():
    """Test the convenience functions."""
    try:
        import nlm
        
        # Test that convenience functions exist
        assert hasattr(nlm, 'enableFeatures'), "enableFeatures not found"
        assert hasattr(nlm, 'runSimulation'), "runSimulation not found"
        assert hasattr(nlm, 'getQuickStats'), "getQuickStats not found"
        assert hasattr(nlm, 'createSimulation'), "createSimulation not found"
        
        # Test example creation functions
        assert hasattr(nlm, 'createExample1'), "createExample1 not found"
        assert hasattr(nlm, 'createExample2'), "createExample2 not found"
        assert hasattr(nlm, 'createExample3'), "createExample3 not found"
        
        # Test documentation functions
        assert hasattr(nlm, 'getQuickReference'), "getQuickReference not found"
        assert hasattr(nlm, 'getExample'), "getExample not found"
        assert hasattr(nlm, 'getExamples'), "getExamples not found"
        
        print("✅ All convenience functions and examples are available")
        return True
        
    except Exception as e:
        print(f"❌ Error testing convenience functions: {e}")
        return False

def test_example_scripts():
    """Test that example scripts can be retrieved."""
    try:
        import nlm
        
        # Test getting examples
        examples = nlm.getExamples()
        print(f"✅ Available examples: {list(examples.keys())}")
        
        # Test getting specific example scripts
        if 'minimal_brain' in examples:
            script = nlm.getExample('minimal_brain')
            assert isinstance(script, str), "Example script should be a string"
            assert "import nlm" in script, "Example script should start with import"
            print("✅ Successfully retrieved 'minimal_brain' example")
        
        if 'complete_agent' in examples:
            script = nlm.getExample('complete_agent')
            assert isinstance(script, str), "Example script should be a string"
            assert "brain, world, agent" in script, "Example script should contain key variables"
            print("✅ Successfully retrieved 'complete_agent' example")
        
        if 'learning' in examples:
            script = nlm.getExample('learning')
            assert isinstance(script, str), "Example script should be a string"
            assert "concept" in script, "Example script should contain concept references"
            print("✅ Successfully retrieved 'learning' example")
        
        return True
        
    except Exception as e:
        print(f"❌ Error testing example scripts: {e}")
        return False

def test_quick_reference():
    """Test that quick reference is available."""
    try:
        import nlm
        
        # Get quick reference
        quick_ref = nlm.getQuickReference()
        assert isinstance(quick_ref, dict), "Quick reference should be a dictionary"
        
        # Check that key sections exist
        assert 'Brain' in quick_ref, "Quick reference should contain Brain section"
        assert 'AgentBrain' in quick_ref, "Quick reference should contain AgentBrain section"
        assert 'SimpleWorld' in quick_ref, "Quick reference should contain SimpleWorld section"
        assert 'examples' in quick_ref, "Quick reference should contain examples section"
        
        print("✅ Successfully retrieved quick reference")
        return True
        
    except Exception as e:
        print(f"❌ Error testing quick reference: {e}")
        return False

def test_feature_enable():
    """Test the enableFeatures convenience function."""
    try:
        import nlm
        
        # Create a mock agent for testing
        class MockAgent:
            def __init__(self):
                self.features = {}
            
            def enableRewardModulation(self, enable):
                self.features['reward_modulation'] = enable
                return self
            
            def enableStructuralPlasticity(self, enable):
                self.features['structural_plasticity'] = enable
                return self
            
            def enableDevelopment(self, enable):
                self.features['development'] = enable
                return self
            
            def enableCuriosity(self, enable):
                self.features['curiosity'] = enable
                return self
        
        # Test the convenience function
        agent = MockAgent()
        result = nlm.enableFeatures(agent, 
                                   reward_modulation=True,
                                   structural_plasticity=False,
                                   development=True,
                                   curiosity=True)
        
        # Verify all features were enabled
        assert result is agent, "Should return the same agent"
        assert agent.features['reward_modulation'] == True, "Reward modulation should be enabled"
        assert agent.features['structural_plasticity'] == False, "Structural plasticity should be disabled"
        assert agent.features['development'] == True, "Development should be enabled"
        assert agent.features['curiosity'] == True, "Curiosity should be enabled"
        
        print("✅ enableFeatures convenience function works correctly")
        return True
        
    except Exception as e:
        print(f"❌ Error testing enableFeatures: {e}")
        return False

def test_api_compatibility():
    """Test that the API matches easy_usage.md examples."""
    try:
        import nlm
        
        # Test that we have functions that match easy_usage.md patterns
        # Pattern 1: Complete simulation setup
        assert hasattr(nlm, 'createSimulation'), "Should have createSimulation function"
        
        # Pattern 2: Quick statistics  
        assert hasattr(nlm, 'getQuickStats'), "Should have getQuickStats function"
        
        # Pattern 3: Feature enable
        assert hasattr(nlm, 'enableFeatures'), "Should have enableFeatures function"
        
        # Test example creation functions that match easy_usage.md Examples 1-3
        assert hasattr(nlm, 'createExample1'), "Should have createExample1 function"
        assert hasattr(nlm, 'createExample2'), "Should have createExample2 function"
        assert hasattr(nlm, 'createExample3'), "Should have createExample3 function"
        
        print("✅ API is compatible with easy_usage.md examples")
        return True
        
    except Exception as e:
        print(f"❌ Error testing API compatibility: {e}")
        return False

def main():
    """Run all tests."""
    print("🧪 Testing NLM Python package implementation...\n")
    
    tests = [
        ("Basic Import", test_basic_import),
        ("Convenience Functions", test_convenience_functions),
        ("Example Scripts", test_example_scripts),
        ("Quick Reference", test_quick_reference),
        ("Feature Enable", test_feature_enable),
        ("API Compatibility", test_api_compatibility),
    ]
    
    passed = 0
    total = len(tests)
    
    for test_name, test_func in tests:
        print(f"\n📋 Running test: {test_name}")
        if test_func():
            passed += 1
            print(f"✅ {test_name} PASSED")
        else:
            print(f"❌ {test_name} FAILED")
    
    print(f"\n📊 Test Results: {passed}/{total} tests passed")
    
    if passed == total:
        print("🎉 All tests passed! The NLM Python package implementation is working correctly.")
        return 0
    else:
        print(f"💥 {total - passed} tests failed. Please check the implementation.")
        return 1

if __name__ == "__main__":
    sys.exit(main())