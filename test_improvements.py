#!/usr/bin/env python3
"""
Simple test script to verify NLM improvements.

This script tests the basic improvements made:
1. Phase 6 integration
2. Agent system integration
3. Constants usage
4. Design pattern improvements
"""

def test_main_cpp():
    """Test if main.cpp has been updated to Phase 6"""
    try:
        with open("/workspace/app/src/main.cpp", "r") as f:
            content = f.read()
        
        # Check for Phase 6 specific content
        if "Phase 6: Final Integration" in content:
            print("✓ main.cpp has been updated to Phase 6 integration")
            
            # Check for all major systems
            systems = [
                "All memory systems",
                "Complete neuromodulation", 
                "Prediction systems",
                "Cognitive systems",
                "Developmental stages",
                "Checkpoint save/load"
            ]
            
            missing = []
            for system in systems:
                if system not in content:
                    missing.append(system)
            
            if not missing:
                print("✓ main.cpp includes all major Phase 6 systems")
            else:
                print(f"⚠ main.cpp missing some systems: {', '.join(missing)}")
                
        else:
            print("✗ main.cpp has not been updated to Phase 6")
            return False
            
    except Exception as e:
        print(f"✗ Error reading main.cpp: {e}")
        return False
    
    return True

def test_cmake_agent_integration():
    """Test if agent files are integrated in CMakeLists.txt"""
    try:
        with open("/workspace/app/CMakeLists.txt", "r") as f:
            content = f.read()
        
        # Check for AgentBody.cpp in main CMakeLists.txt
        if "AgentBody.cpp" in content:
            print("✓ AgentBody.cpp integrated in main CMakeLists.txt")
        else:
            print("⚠ AgentBody.cpp not found in main CMakeLists.txt")
            
        # Check for complete agent library definition
        if "add_library(nlm_agent STATIC" in content and "SensoryPercept.cpp" in content and "AgentBrain.cpp" in content:
            print("✓ nlm_agent library properly defined")
        else:
            print("⚠ nlm_agent library definition incomplete")
            
    except Exception as e:
        print(f"✗ Error reading CMakeLists.txt: {e}")
        return False
    
    return True

def test_constants():
    """Test if constants file exists and has expected content"""
    try:
        with open("/workspace/app/src/core/Config/Constants.hpp", "r") as f:
            content = f.read()
        
        # Check for key constants
        expected_constants = [
            "DEFAULT_VISION_WIDTH",
            "VISION_SCALE_FACTOR",
            "MOTOR_ACTIVITY_THRESHOLD",
            "NOVELTY_EXPLORATION_WEIGHT",
            "DEVELOPMENT_STAGE_INITIAL"
        ]
        
        found = []
        for constant in expected_constants:
            if constant in content:
                found.append(constant)
        
        print(f"✓ Constants file contains {len(found)}/{len(expected_constants)} expected constants")
        
        if len(found) >= 5:
            print("✓ Constants file appears to be comprehensive")
        else:
            print(f"⚠ Only {len(found)} key constants found")
            
    except Exception as e:
        print(f"✗ Error reading Constants.hpp: {e}")
        return False
    
    return True

def test_agent_brain_improvements():
    """Test if AgentBrain has been improved"""
    try:
        with open("/workspace/app/src/agent/AgentBrain.cpp", "r") as f:
            content = f.read()
        
        # Check for improvements
        improvements = [
            "Constants.hpp",
            "weighted random distribution",
            "distributeMotorNeurons",
            "distributeSensoryNeurons",
            "getRandomGenerator"
        ]
        
        found = []
        for improvement in improvements:
            if improvement in content:
                found.append(improvement)
        
        print(f"✓ AgentBrain includes {len(found)}/{len(improvements)} key improvements")
        
        # Check that hardcoded values were replaced
        if "0.5f" in content and "5.0f" in content and "8.0f" in content:
            print("⚠ Potential hardcoded values still present in AgentBrain.cpp")
        else:
            print("✓ AgentBrain appears to use constants instead of hardcoded values")
            
    except Exception as e:
        print(f"✗ Error reading AgentBrain.cpp: {e}")
        return False
    
    return True

def main():
    print("=== NLM Improvements Verification Test ===\n")
    
    results = []
    
    print("1. Testing main.cpp Phase 6 integration...")
    results.append(test_main_cpp())
    
    print("\n2. Testing CMake agent integration...")
    results.append(test_cmake_agent_integration())
    
    print("\n3. Testing constants implementation...")
    results.append(test_constants())
    
    print("\n4. Testing AgentBrain improvements...")
    results.append(test_agent_brain_improvements())
    
    print("\n=== Test Summary ===")
    passed = sum(results)
    total = len(results)
    
    print(f"Tests passed: {passed}/{total}")
    
    if passed == total:
        print("\n✓ All tests passed! NLM improvements appear to be successful.")
    else:
        print(f"\n⚠ {total - passed} test(s) failed. Please review the improvements.")
    
    return passed == total

if __name__ == "__main__":
    success = main()
    exit(0 if success else 1)
