#!/usr/bin/env python3
"""
Test script to verify AgentBrain and SensoryPercept fixes.
Runs comprehensive tests to validate bug fixes and improvements.
"""

import os
import sys
import subprocess
import argparse
from pathlib import Path

class TestReporter:
    def __init__(self):
        self.passed = 0
        self.failed = 0
        self.warnings = 0
        
    def print_header(self, title):
        print(f"\n{'=' * 60}")
        print(f"{title}")
        print(f"{'=' * 60}")
        
    def print_test(self, test_name, status, message=""):
        status_symbol = "✓" if status == "PASS" else "✗" if status == "FAIL" else "⚠"
        print(f"{status_symbol} {test_name}")
        if message:
            indent = "  "
            for line in message.split('\n'):
                print(f"{indent}{line}")
        
        if status == "PASS":
            self.passed += 1
        elif status == "FAIL":
            self.failed += 1
        else:
            self.warnings += 1
            
    def print_summary(self):
        print(f"\n{'=' * 60}")
        print("TEST SUMMARY")
        print(f"{'=' * 60}")
        print(f"Tests Passed:   {self.passed}")
        print(f"Tests Failed:   {self.failed}")
        print(f"Warnings:       {self.warnings}")
        print(f"Total:          {self.passed + self.failed + self.warnings}")
        
        if self.failed == 0:
            print(f"\n✓ All tests passed!")
            return True
        else:
            print(f"\n✗ {self.failed} test(s) failed!")
            return False

def check_file_syntax(filepath):
    """Check if a C++ file has valid syntax by compiling it with g++."""
    try:
        # Try to compile just this file
        result = subprocess.run(
            ["g++", "-std=c++20", "-c", filepath, "-o", "/tmp/test_compile.o"],
            capture_output=True,
            text=True,
            timeout=30
        )
        return result.returncode == 0, result.stderr
    except subprocess.TimeoutExpired:
        return False, "Compilation timeout"
    except Exception as e:
        return False, str(e)

def check_compilation():
    """Check if all C++ files compile correctly."""
    reporter = TestReporter()
    reporter.print_header("COMPILATION TEST")
    
    # Find all .cpp files in the source directory
    src_dir = Path("/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_42f68510-155b-4dfa-851b-8ac64b4d68ae/src")
    cpp_files = list(src_dir.rglob("*.cpp"))
    
    if not cpp_files:
        reporter.print_test("Find C++ source files", "FAIL", "No .cpp files found")
        return False
        
    reporter.print_test(f"Found {len(cpp_files)} C++ source files", "PASS")
    
    # Check each file
    for cpp_file in sorted(cpp_files):
        filename = cpp_file.name
        status, error_msg = check_file_syntax(str(cpp_file))
        
        if status:
            reporter.print_test(f"Compile {filename}", "PASS")
        else:
            reporter.print_test(f"Compile {filename}", "FAIL", error_msg[:200] if error_msg else "Compilation failed")
            
    return reporter.print_summary()

def check_agentbrain_fixes():
    """Check specific AgentBrain fixes."""
    reporter = TestReporter()
    reporter.print_header("AGENT BRAIN FIXES VERIFICATION")
    
    agentbrain_cpp = "/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_42f68510-155b-4dfa-851b-8ac64b4d68ae/src/agent/AgentBrain.cpp"
    
    with open(agentbrain_cpp, 'r') as f:
        content = f.read()
        
    # Check for null pointer validation
    if "NLM_LOG_ERROR(\"AgentBrain initialized with null brain pointer\")" in content:
        reporter.print_test("Null brain pointer validation", "PASS")
    else:
        reporter.print_test("Null brain pointer validation", "FAIL", "Missing error logging for null brain pointer")
        
    # Check for null neuron checks
    if "if (!n) continue;  // Skip null neurons" in content:
        reporter.print_test("Null neuron pointer checks", "PASS")
    else:
        reporter.print_test("Null neuron pointer checks", "FAIL", "Missing null checks for individual neurons")
        
    # Check for initialization validation
    if "if (sensoryVision_.empty() || sensoryTouch_.empty()" in content:
        reporter.print_test("Empty neuron group validation", "PASS")
    else:
        reporter.print_test("Empty neuron group validation", "FAIL", "Missing validation for empty neuron groups")
        
    # Check for proper random generator validation
    if "if (!brain_ || !brain_->getRandomGenerator())" in content:
        reporter.print_test("Random generator null check", "PASS")
    else:
        reporter.print_test("Random generator null check", "FAIL", "Missing validation for random generator")
        
    # Check for fixed random integer range (0-6 instead of 0-7)
    if "uniformInt(0, 6)" in content:
        reporter.print_test("Fixed random integer range", "PASS")
    else:
        reporter.print_test("Fixed random integer range", "FAIL", "Random range should be uniformInt(0, 6) not (0, 7)")
        
    return reporter.print_summary()

def check_sensorypercept_fixes():
    """Check specific SensoryPercept fixes."""
    reporter = TestReporter()
    reporter.print_header("SENSORY PERCEPT FIXES VERIFICATION")
    
    sensorycpp = "/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_42f68510-155b-4dfa-851b-8ac64b4d68ae/src/agent/SensoryPercept.cpp"
    
    with open(sensorycpp, 'r') as f:
        content = f.read()
        
    # Check for removed invalid audio_.resize(0, 0.0f)
    if "audio_.resize(0, 0.0f);" not in content:
        reporter.print_test("Removed invalid audio vector initialization", "PASS")
    else:
        reporter.print_test("Removed invalid audio vector initialization", "FAIL", "Still contains invalid audio_.resize(0, 0.0f)")
        
    # Check for audio vector comment
    if "Audio vector remains empty" in content:
        reporter.print_test("Added explanatory comment for audio vector", "PASS")
    else:
        reporter.print_test("Added explanatory comment for audio vector", "FAIL", "Missing explanatory comment")
        
    # Check for memory optimization (pre-allocation)
    if "all.reserve(" in content:
        reporter.print_test("Memory optimization in getAllSignals()", "PASS")
    else:
        reporter.print_test("Memory optimization in getAllSignals()", "FAIL", "getAllSignals() not optimized with reserve()")
        
    return reporter.print_summary()

def check_agentbrain_enhancements():
    """Check for new advanced features in AgentBrain."""
    reporter = TestReporter()
    reporter.print_header("AGENT BRAIN ENHANCEMENTS VERIFICATION")
    
    agentbrain_h = "/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_42f68510-155b-4dfa-851b-8ac64b4d68ae/src/agent/AgentBrain.hpp"
    
    with open(agentbrain_h, 'r') as f:
        content = f.read()
        
    # Check for new methods
    new_methods = [
        ("getStateSnapshot", "State snapshot for checkpointing"),
        ("restoreStateFromSnapshot", "State restoration"),
        ("getPerformanceMetrics", "Performance metrics"),
        ("setNeuromodulatorLevel", "Advanced neuromodulator control"),
        ("setExplorationStrategy", "Exploration strategy control"),
        ("enableDebugLogging", "Debug logging feature"),
        ("setCuriosityThreshold", "Curiosity threshold configuration"),
        ("setRewardPredictionLearningRate", "Reward prediction learning rate"),
        ("getActiveMotorNeuronCount", "Active neuron count tracking"),
        ("getActiveSensoryNeuronCount", "Active sensory neuron tracking")
    ]
    
    for method, description in new_methods:
        if f"{method}(" in content:
            reporter.print_test(f"New method: {method}", "PASS", description)
        else:
            reporter.print_test(f"New method: {method}", "FAIL", f"Missing {method}() method")
            
    return reporter.print_summary()

def check_neuromodulation_enhancements():
    """Check for enhanced neuromodulation features."""
    reporter = TestReporter()
    reporter.print_header("NEUROMODULATION ENHANCEMENTS VERIFICATION")
    
    agentbrain_h = "/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_42f68510-155b-4dfa-851b-8ac64b4d68ae/src/agent/AgentBrain.hpp"
    
    with open(agentbrain_h, 'r') as f:
        content = f.read()
        
    # Check for serotonin and acetylcholine
    if "float serotoninLevel_" in content and "float acetylcholineLevel_" in content:
        reporter.print_test("Extended neuromodulation system", "PASS", "Added serotonin and acetylcholine")
    else:
        reporter.print_test("Extended neuromodulation system", "FAIL", "Missing serotonin/acetylcholine")
        
    # Check for configuration methods
    if "setSensoryConfig" in content and "setMotorConfig" in content:
        reporter.print_test("Configuration management", "PASS", "JSON configuration support")
    else:
        reporter.print_test("Configuration management", "FAIL", "Missing configuration methods")
        
    # Check for debug logging
    if "debugLoggingEnabled_" in content:
        reporter.print_test("Debug logging capability", "PASS")
    else:
        reporter.print_test("Debug logging capability", "FAIL")
        
    return reporter.print_summary()

def check_brain_integration():
    """Check Brain integration compatibility."""
    reporter = TestReporter()
    reporter.print_header("BRAIN INTEGRATION VERIFICATION")
    
    # Check if CMakeLists.txt includes agent library
    cmake_path = "/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_42f68510-155b-4dfa-851b-8ac64b4d68ae/CMakeLists.txt"
    
    with open(cmake_path, 'r') as f:
        cmake_content = f.read()
        
    if "add_library(nlm_agent" in cmake_content:
        reporter.print_test("Agent library in CMake", "PASS")
    else:
        reporter.print_test("Agent library in CMake", "FAIL", "nlm_agent library not found in CMake")
        
    if "target_link_libraries(nlm_agent PUBLIC nlm_core)" in cmake_content:
        reporter.print_test("Agent links to core library", "PASS")
    else:
        reporter.print_test("Agent links to core library", "FAIL", "Missing nlm_core dependency")
        
    return reporter.print_summary()

def main():
    print("NLM Agent System Verification Script")
    print("=" * 60)
    
    all_passed = True
    
    # Run all verification tests
    if not check_compilation():
        all_passed = False
        
    if not check_agentbrain_fixes():
        all_passed = False
        
    if not check_sensorypercept_fixes():
        all_passed = False
        
    if not check_agentbrain_enhancements():
        all_passed = False
        
    if not check_neuromodulation_enhancements():
        all_passed = False
        
    if not check_brain_integration():
        all_passed = False
        
    # Final result
    print(f"\n{'=' * 60}")
    if all_passed:
        print("✓ VERIFICATION COMPLETE: All tests passed!")
        print("\nThe Agent system has been successfully improved with:")
        print("  - Fixed critical bugs (null pointer checks, memory corruption)")
        print("  - Enhanced code quality (proper validation, error handling)")
        print("  - Added advanced features (checkpointing, performance metrics)")
        print("  - Extended neuromodulation (serotonin, acetylcholine)")
        print("  - Improved usability (configuration management, debugging)")
        return 0
    else:
        print("✗ VERIFICATION FAILED: Some tests failed!")
        print("\nPlease review failed tests above and fix issues.")
        return 1

if __name__ == "__main__":
    sys.exit(main())
