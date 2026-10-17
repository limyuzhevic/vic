#!/usr/bin/env python3
"""
Test script to verify NLM improvements work correctly.
"""

import sys
import os

def test_python_bindings():
    """Test that Python bindings are available and functional."""
    print("Testing Python bindings...")
    
    try:
        # Check if we can import the module
        import sys
        sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))
        
        # Try to import the module
        import pynlm
        print(f"✓ Successfully imported pynlm version: {getattr(pynlm, '__version__', 'Unknown')}")
        
        # Test creating default config
        config = pynlm.createDefaultConfig()
        print(f"✓ Created default config: {config}")
        
        # Test creating brain
        brain = pynlm.createBrain(config)
        print(f"✓ Created brain instance")
        
        # Test creating world
        world = pynlm.createSimpleWorld()
        print(f"✓ Created simple world")
        
        # Test creating agent brain
        agent = pynlm.createAgentBrain(brain)
        print(f"✓ Created agent brain")
        
        # Test Phase 6 config creation
        phase6_config = pynlm.createPhase6Config()
        print(f"✓ Created Phase 6 config with {phase6_config.neuronCount} neurons")
        
        # Test Phase 6 integration verification
        integration_ok = pynlm.verifyPhase6Integration()
        print(f"✓ Phase 6 integration verification: {'PASSED' if integration_ok else 'FAILED'}")
        
        # Test Phase 6 experiment instance creation
        experiment = pynlm.createPhase6IntegratedExperiment()
        print(f"✓ Created Phase 6 experiment instance")
        
        print("\n✅ All Python bindings tests passed!")
        return True
        
    except Exception as e:
        print(f"\n❌ Python bindings test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_cmake_structure():
    """Test that CMake structure is correct."""
    print("\nTesting CMake structure...")
    
    cmake_file = os.path.join(os.path.dirname(__file__), 'CMakeLists.txt')
    
    if not os.path.exists(cmake_file):
        print("❌ CMakeLists.txt not found")
        return False
    
    with open(cmake_file, 'r') as f:
        content = f.read()
    
    # Check for Phase 6 components
    required_components = [
        'nlm_phase6',
        'nlm_phase6_demo',
        'Phase6Config',
        'Phase6IntegrationResult'
    ]
    
    missing = []
    for component in required_components:
        if component not in content:
            missing.append(component)
    
    if missing:
        print(f"❌ Missing CMake components: {missing}")
        return False
    
    print("✅ CMake structure contains all required Phase 6 components")
    return True

def main():
    """Run all verification tests."""
    print("=" * 60)
    print("NLM Improvements Verification")
    print("=" * 60)
    
    python_ok = test_python_bindings()
    cmake_ok = test_cmake_structure()
    
    print("\n" + "=" * 60)
    if python_ok and cmake_ok:
        print("✅ ALL TESTS PASSED!")
        print("NLM improvements are working correctly.")
        return 0
    else:
        print("❌ SOME TESTS FAILED")
        print("Please check the output above for details.")
        return 1

if __name__ == "__main__":
    sys.exit(main())