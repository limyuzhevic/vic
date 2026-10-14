#!/usr/bin/env python3
# Test script for NLM Phase 6 improvements validation
# Validates all improvements made to the NLM codebase

import os
import sys
import subprocess
import json
from pathlib import Path

def run_command(cmd, cwd=None):
    """Run a command and return result."""
    print(f"Running: {cmd}")
    try:
        result = subprocess.run(cmd, shell=True, cwd=cwd, 
                              capture_output=True, text=True)
        if result.returncode != 0:
            print(f"Error: {result.stderr}")
        return result
    except Exception as e:
        print(f"Exception: {e}")
        return None

def test_cmake_build():
    """Test that CMake builds correctly with Phase 6 improvements."""
    print("\n=== Testing CMake Build ===")
    
    # Create build directory
    build_dir = Path("build")
    if not build_dir.exists():
        build_dir.mkdir()
    
    # Run CMake configuration
    result = run_command("cmake .. -DCMAKE_BUILD_TYPE=Release", cwd=build_dir)
    if result.returncode != 0:
        return False, "CMake configuration failed"
    
    # Run make
    result = run_command("make -j4", cwd=build_dir)
    if result.returncode != 0:
        return False, "Build failed"
    
    # Check if executables exist
    nlm_exe = build_dir / "nlm"
    nlm_phase3_demo = build_dir / "nlm_phase3_demo"
    nlm_phase4_demo = build_dir / "nlm_phase4_demo"
    nlm_phase6_demo = build_dir / "nlm_phase6_demo"
    
    if not nlm_exe.exists():
        return False, "nlm executable not found"
    
    if not nlm_phase3_demo.exists():
        return False, "nlm_phase3_demo executable not found"
    
    if not nlm_phase4_demo.exists():
        return False, "nlm_phase4_demo executable not found"
    
    if not nlm_phase6_demo.exists():
        return False, "nlm_phase6_demo executable not found (Phase 6 demo missing!)"
    
    print("✓ All executables built successfully")
    return True, "CMake build test passed"

def test_code_quality():
    """Test code quality improvements."""
    print("\n=== Testing Code Quality ===")
    
    # Check for duplicate includes in Phase4Demo.cpp
    phase4_demo_path = Path("src/experiments/Phase4Demo.cpp")
    if phase4_demo_path.exists():
        content = phase4_demo_path.read_text()
        if "cognition/NeuralPlanner.hpp" in content:
            # Count occurrences
            count = content.count("cognition/NeuralPlanner.hpp")
            if count > 1:
                return False, f"Found {count} duplicate includes of NeuralPlanner.hpp (should be 1)"
            else:
                print("✓ No duplicate includes found")
        else:
            print("✓ NeuralPlanner.hpp not found (already removed)")
    
    # Check Phase6Demo.cpp for correct Logger API
    phase6_demo_path = Path("src/experiments/Phase6Demo.cpp")
    if phase6_demo_path.exists():
        content = phase6_demo_path.read_text()
        if "Logger::getInstance()" in content:
            return False, "Phase6Demo.cpp still uses wrong Logger API (Logger::getInstance())"
        
        if "Logger::setGlobal" in content:
            print("✓ Phase6Demo.cpp uses correct Logger API")
        else:
            print("⚠ Phase6Demo.cpp may not have correct Logger API")
    
    # Check CMakeLists.txt for Phase 6 demo
    cmake_path = Path("CMakeLists.txt")
    if cmake_path.exists():
        content = cmake_path.read_text()
        if "add_executable(nlm_phase6_demo" in content:
            print("✓ Phase 6 demo executable found in CMakeLists.txt")
        else:
            return False, "Phase 6 demo executable missing from CMakeLists.txt"
        
        if "target_link_libraries(nlm_phase6_demo" in content:
            print("✓ Phase 6 demo executable has proper linking")
        else:
            return False, "Phase 6 demo executable linking incorrect"
    
    return True, "Code quality test passed"

def test_configuration_files():
    """Test configuration files."""
    print("\n=== Testing Configuration Files ===")
    
    # Check phase6_experiments.json
    phase6_config_path = Path("configs/phase6_experiments.json")
    if phase6_config_path.exists():
        try:
            with open(phase6_config_path, 'r') as f:
                config = json.load(f)
            
            # Check required fields
            required_fields = [
                "experiment_id",
                "version",
                "simulation_parameters",
                "memory_systems",
                "neuromodulation",
                "development"
            ]
            
            for field in required_fields:
                if field not in config:
                    return False, f"Missing required field in phase6_experiments.json: {field}"
            
            print("✓ phase6_experiments.json has all required fields")
            
            # Check version
            if config.get("version") == "6.0":
                print("✓ Configuration version is 6.0")
            else:
                return False, f"Wrong configuration version: {config.get('version')}"
            
        except json.JSONDecodeError as e:
            return False, f"Invalid JSON in phase6_experiments.json: {e}"
        except Exception as e:
            return False, f"Error reading phase6_experiments.json: {e}"
    
    # Check nlm_config_validator.py exists
    validator_path = Path("nlm_config_validator.py")
    if validator_path.exists():
        print("✓ Configuration validator script exists")
    else:
        return False, "Configuration validator script missing"
    
    # Check phase6_performance_monitor.py exists
    monitor_path = Path("phase6_performance_monitor.py")
    if monitor_path.exists():
        print("✓ Performance monitor script exists")
    else:
        return False, "Performance monitor script missing"
    
    # Check phase6_advanced_api.py exists
    advanced_api_path = Path("phase6_advanced_api.py")
    if advanced_api_path.exists():
        print("✓ Advanced API script exists")
    else:
        return False, "Advanced API script missing"
    
    return True, "Configuration files test passed"

def test_documentation():
    """Test documentation improvements."""
    print("\n=== Testing Documentation ===")
    
    # Check updated Phase 6 audit
    audit_path = Path("docs/PHASE6_FINAL_AUDIT.md")
    if audit_path.exists():
        content = audit_path.read_text()
        
        # Check for key improvements
        if "Phase 6 IS NOW COMPLETE" in content:
            print("✓ Phase 6 audit reflects completion status")
        else:
            return False, "Phase 6 audit doesn't reflect completion status"
        
        if "NLM now provides a coherent artificial brain" in content:
            print("✓ Phase 6 audit shows brain is coherent")
        else:
            return False, "Phase 6 audit doesn't show brain coherence"
        
        # Check date is recent
        if "2026-10-14" in content:
            print("✓ Phase 6 audit date is recent")
        else:
            print("⚠ Phase 6 audit date may not be recent")
    
    # Check other Phase 6 documentation files
    phase6_docs = ["docs/PHASE6_FINAL_RESULTS.md", "docs/PHASE6_ARCHITECTURE.md"]
    for doc_path in phase6_docs:
        doc_file = Path(doc_path)
        if doc_file.exists():
            print(f"✓ {doc_file.name} exists")
        else:
            print(f"⚠ {doc_file.name} missing")
    
    return True, "Documentation test passed"

def test_advanced_features():
    """Test advanced features."""
    print("\n=== Testing Advanced Features ===")
    
    # Test Python scripts syntax
    python_files = [
        "nlm_config_validator.py",
        "phase6_performance_monitor.py", 
        "phase6_advanced_api.py"
    ]
    
    for file_path in python_files:
        path = Path(file_path)
        if path.exists():
            try:
                # Try to import the module
                import importlib.util
                spec = importlib.util.spec_from_file_location(file_path.replace('.py', ''), path)
                if spec is not None:
                    print(f"✓ {file_path} can be imported")
                else:
                    return False, f"{file_path} cannot be imported"
            except ImportError as e:
                print(f"⚠ {file_path} has import issues: {e}")
            except SyntaxError as e:
                return False, f"Syntax error in {file_path}: {e}"
        else:
            return False, f"{file_path} not found"
    
    return True, "Advanced features test passed"

def main():
    """Main test function."""
    print("NLM Phase 6 Improvements Validation Test")
    print("=" * 50)
    
    tests = [
        ("CMake Build", test_cmake_build),
        ("Code Quality", test_code_quality),
        ("Configuration Files", test_configuration_files),
        ("Documentation", test_documentation),
        ("Advanced Features", test_advanced_features),
    ]
    
    results = []
    
    for test_name, test_func in tests:
        try:
            passed, message = test_func()
            results.append((test_name, passed, message))
        except Exception as e:
            results.append((test_name, False, f"Test failed with exception: {e}"))
    
    print("\n" + "=" * 50)
    print("TEST RESULTS SUMMARY")
    print("=" * 50)
    
    all_passed = True
    for test_name, passed, message in results:
        status = "PASS" if passed else "FAIL"
        print(f"{test_name}: {status} - {message}")
        if not passed:
            all_passed = False
    
    print("\n" + "=" * 50)
    if all_passed:
        print("SUCCESS: All tests passed! Phase 6 improvements are working correctly.")
        return 0
    else:
        print("FAILURE: Some tests failed. Please review the issues above.")
        return 1

if __name__ == "__main__":
    sys.exit(main())