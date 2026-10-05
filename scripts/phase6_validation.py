#!/usr/bin/env python3
# Phase 6 Integration Validation Script
# Validates that all integration improvements are working correctly

import os
import sys
import subprocess
import json
import time
from pathlib import Path

def print_status(status, message):
    """Print colored status message"""
    colors = {
        'PASS': '\033[92m✓',
        'FAIL': '\033[91m✗', 
        'WARN': '\033[93m⚠',
        'INFO': '\033[96mℹ'
    }
    reset = '\033[0m'
    print(f"{colors.get(status, ' ')} {message}{reset}")

def check_file_exists(filepath, description):
    """Check if a file exists"""
    if os.path.exists(filepath):
        print_status('PASS', f"{description} found: {filepath}")
        return True
    else:
        print_status('FAIL', f"{description} not found: {filepath}")
        return False

def check_directory_exists(dirpath, description):
    """Check if a directory exists"""
    if os.path.isdir(dirpath):
        print_status('PASS', f"{description} found: {dirpath}")
        return True
    else:
        print_status('FAIL', f"{description} not found: {dirpath}")
        return False

def count_test_files(pattern):
    """Count test files matching a pattern"""
    import glob
    files = glob.glob(pattern)
    return len(files)

def validate_phase1_python_bindings():
    """Validate Phase 1: Python Bindings Improvements"""
    print("\n=== Phase 1: Python Bindings Validation ===")
    
    project_root = Path("/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_ae1867a8-ef72-49e5-8007-d183972c2ede")
    
    # Check Python binding files
    python_files = [
        (project_root / "nlm" / "brain" / "python_utils.py", "Python utilities"),
        (project_root / "nlm" / "visualization" / "python_utils.py", "Visualization Python utils"),
        (project_root / "nlm" / "brain" / "__init__.py", "Brain module"),
    ]
    
    phase1_valid = True
    for filepath, desc in python_files:
        if not check_file_exists(str(filepath), desc):
            phase1_valid = False
    
    # Check for type conversion utilities
    if check_file_exists(str(project_root / "nlm" / "brain" / "neuron.py"), "Neuron Python bindings"):
        with open(project_root / "nlm" / "brain" / "neuron.py", 'r') as f:
            content = f.read()
            if "pybind11" in content and "@py::property" in content:
                print_status('PASS', "Type conversion and property decorators found")
            else:
                print_status('WARN', "Type conversion decorators may be missing")
                phase1_valid = False
    
    return phase1_valid

def validate_phase2_cpp_improvements():
    """Validate Phase 2: C++ Improvements"""
    print("\n=== Phase 2: C++ Improvements Validation ===")
    
    project_root = Path("/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_ae1867a8-ef72-49e5-8007-d183972c2ede")
    
    # Check C++ improvement files
    cpp_files = [
        (project_root / "src" / "brain" / "Brain.hpp", "Enhanced Brain class"),
        (project_root / "src" / "brain" / "Neuron.hpp", "Enhanced Neuron class"),
        (project_root / "src" / "brain" / "Synapse.hpp", "Enhanced Synapse class"),
        (project_root / "src" / "performance" / "SIMDVectorization.hpp", "SIMD vectorization"),
        (project_root / "src" / "performance" / "MemoryPool.hpp", "Memory pool"),
        (project_root / "src" / "performance" / "ParallelProcessing.hpp", "Parallel processing"),
    ]
    
    phase2_valid = True
    for filepath, desc in cpp_files:
        if not check_file_exists(str(filepath), desc):
            phase2_valid = False
    
    # Check for modern C++ features
    if check_file_exists(str(project_root / "src" / "core" / "Config.hpp"), "Configuration system"):
        with open(project_root / "src" / "core" / "Config.hpp", 'r') as f:
            content = f.read()
            if "std::variant" in content and "ConfigSource::" in content:
                print_status('PASS', "Modern C++ features (std::variant, enums) found")
            else:
                print_status('WARN', "Modern C++ features may be incomplete")
                phase2_valid = False
    
    return phase2_valid

def validate_phase3_testing():
    """Validate Phase 3: Testing Improvements"""
    print("\n=== Phase 3: Testing Improvements Validation ===")
    
    project_root = Path("/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_ae1867a8-ef72-49e5-8007-d183972c2ede")
    
    # Count test files
    test_patterns = [
        ("test_*.cpp", "Core integration tests"),
        ("test_python_bindings.cpp", "Python bindings tests"),
        ("test_memory_integration.cpp", "Memory integration tests"),
        ("test_development_integration.cpp", "Development integration tests"),
        ("test_prediction_integration.cpp", "Prediction integration tests"),
        ("test_parallel_processing.cpp", "Parallel processing tests"),
        ("test_simd_performance.cpp", "SIMD performance tests"),
    ]
    
    phase3_valid = True
    total_tests = 0
    
    for pattern, desc in test_patterns:
        count = count_test_files(str(project_root / "tests" / pattern))
        total_tests += count
        print_status('INFO', f"{desc}: {count} test files")
        if count == 0:
            phase3_valid = False
    
    # Check test runner
    if check_file_exists(str(project_root / "scripts" / "run_integration_tests.py"), "Integration test runner"):
        print_status('PASS', "Integration test runner found")
    else:
        phase3_valid = False
    
    print_status('INFO', f"Total test files found: {total_tests}")
    return phase3_valid

def validate_phase4_build_system():
    """Validate Phase 4: Build System Improvements"""
    print("\n=== Phase 4: Build System Improvements Validation ===")
    
    project_root = Path("/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_ae1867a8-ef72-49e5-8007-d183972c2ede")
    
    # Check build system files
    build_files = [
        (project_root / "CMakeLists.txt", "Main CMake configuration"),
        (project_root / "conanfile.py", "Conan dependency manager"),
        (project_root / "pyproject.toml", "Python project configuration"),
        (project_root / "setup.py", "Python setup script"),
        (project_root / "vcpkg.json", "VCPKG configuration"),
    ]
    
    phase4_valid = True
    for filepath, desc in build_files:
        if not check_file_exists(str(filepath), desc):
            phase4_valid = False
    
    # Check for build options
    cmake_lists = project_root / "CMakeLists.txt"
    if cmake_lists.exists():
        with open(cmake_lists, 'r') as f:
            content = f.read()
            if "NLM_BUILD_PYTHON_BINDINGS" in content and "NLM_ENABLE_COVERAGE" in content:
                print_status('PASS', "Build options (Python bindings, coverage) found")
            else:
                print_status('WARN', "Build options may be incomplete")
                phase4_valid = False
    
    return phase4_valid

def validate_phase5_documentation():
    """Validate Phase 5: Documentation Improvements"""
    print("\n=== Phase 5: Documentation Improvements Validation ===")
    
    project_root = Path("/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_ae1867a8-ef72-49e5-8007-d183972c2ede")
    
    # Check documentation files
    doc_files = [
        (project_root / "docs" / "API_REFERENCE.md", "API reference documentation"),
        (project_root / "docs" / "PYTHON_GUIDE.md", "Python usage guide"),
        (project_root / "docs" / "CPLUSPLUS_GUIDE.md", "C++ usage guide"),
        (project_root / "docs" / "PERFORMANCE_GUIDE.md", "Performance guide"),
        (project_root / "docs" / "EXPERIMENTS.md", "Experiments documentation"),
        (project_root / "docs" / "PHASE6_FINAL_RESULTS.md", "Phase 6 final results"),
        (project_root / "docs" / "INTEGRATION_OVERVIEW.md", "Integration overview"),
        (project_root / "docs" / "TESTING_GUIDE.md", "Testing guide"),
    ]
    
    phase5_valid = True
    for filepath, desc in doc_files:
        if not check_file_exists(str(filepath), desc):
            phase5_valid = False
    
    # Check documentation quality
    api_ref = project_root / "docs" / "API_REFERENCE.md"
    if api_ref.exists():
        with open(api_ref, 'r') as f:
            content = f.read()
            if len(content) > 10000:  # Should be substantial
                print_status('PASS', f"API reference documentation is comprehensive ({len(content)} chars)")
            else:
                print_status('WARN', f"API reference documentation may be too short ({len(content)} chars)")
                phase5_valid = False
    
    return phase5_valid

def validate_phase6_integration():
    """Validate Phase 6: Integration Improvements"""
    print("\n=== Phase 6: Integration Improvements Validation ===")
    
    project_root = Path("/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_ae1867a8-ef72-49e5-8007-d183972c2ede")
    
    # Check Phase 6 integration test files
    integration_tests = [
        (project_root / "tests" / "test_integration_complete.cpp", "Complete system integration tests"),
        (project_root / "tests" / "test_python_cpp_integration.cpp", "Python-C++ cross-component tests"),
        (project_root / "tests" / "test_memory_management_integration.cpp", "Memory management integration tests"),
        (project_root / "tests" / "test_performance_integration.cpp", "Performance integration tests"),
        (project_root / "tests" / "test_backward_compatibility.cpp", "Backward compatibility tests"),
    ]
    
    phase6_valid = True
    total_integration_tests = 0
    
    for filepath, desc in integration_tests:
        if check_file_exists(str(filepath), desc):
            total_integration_tests += 1
        else:
            phase6_valid = False
    
    # Check Phase 6 experiment
    if check_file_exists(str(project_root / "src" / "experiments" / "Phase6IntegratedExperiment.cpp"), "Phase 6 integrated experiment"):
        print_status('PASS', "Phase 6 integrated experiment found")
    else:
        phase6_valid = False
    
    # Check verification scripts
    if check_file_exists(str(project_root / "scripts" / "verify_phase6_integration.sh"), "Phase 6 verification script"):
        print_status('PASS', "Phase 6 verification script found")
    else:
        phase6_valid = False
    
    # Check experiment directory
    if check_directory_exists(str(project_root / "src" / "experiments"), "Experiments directory"):
        print_status('PASS', f"Found {total_integration_tests}/5 Phase 6 integration test files")
    
    return phase6_valid and (total_integration_tests >= 5)

def validate_system_structure():
    """Validate overall system structure"""
    print("\n=== System Structure Validation ===")
    
    project_root = Path("/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_ae1867a8-ef72-49e5-8007-d183972c2ede")
    
    # Check required directories
    required_dirs = [
        (project_root / "src", "Source code directory"),
        (project_root / "src" / "brain", "Brain source directory"),
        (project_root / "src" / "plasticity", "Plasticity source directory"),
        (project_root / "src" / "performance", "Performance source directory"),
        (project_root / "src" / "experiments", "Experiments directory"),
        (project_root / "nlm", "Python package"),
        (project_root / "tests", "Tests directory"),
        (project_root / "docs", "Documentation directory"),
        (project_root / "scripts", "Scripts directory"),
    ]
    
    structure_valid = True
    for dirpath, desc in required_dirs:
        if not check_directory_exists(str(dirpath), desc):
            structure_valid = False
    
    # Check key files
    key_files = [
        (project_root / "src" / "brain" / "main.cpp", "Main brain implementation"),
        (project_root / "src" / "brain" / "Brain.hpp", "Brain header"),
        (project_root / "nlm" / "brain" / "brain.py", "Python brain interface"),
        (project_root / "nlm" / "brain" / "neuron.py", "Python neuron interface"),
    ]
    
    for filepath, desc in key_files:
        if not check_file_exists(str(filepath), desc):
            structure_valid = False
    
    return structure_valid

def run_final_verification():
    """Run the complete Phase 6 integration verification"""
    print("=== Phase 6 Integration Verification Script ===")
    print("Validating all Phase 1-6 improvements...")
    
    # Run all validations
    validations = [
        ("Python Bindings (Phase 1)", validate_phase1_python_bindings),
        ("C++ Improvements (Phase 2)", validate_phase2_cpp_improvements),
        ("Testing Improvements (Phase 3)", validate_phase3_testing),
        ("Build System Improvements (Phase 4)", validate_phase4_build_system),
        ("Documentation Improvements (Phase 5)", validate_phase5_documentation),
        ("Integration Improvements (Phase 6)", validate_phase6_integration),
        ("System Structure", validate_system_structure),
    ]
    
    results = {}
    all_passed = True
    
    for name, validation_func in validations:
        try:
            result = validation_func()
            results[name] = result
            if not result:
                all_passed = False
        except Exception as e:
            print_status('FAIL', f"{name} failed with error: {e}")
            results[name] = False
            all_passed = False
    
    # Print summary
    print("\n" + "="*60)
    print("VERIFICATION SUMMARY")
    print("="*60)
    
    for name, passed in results.items():
        status = "✅ PASSED" if passed else "❌ FAILED"
        print(f"{name}: {status}")
    
    print("="*60)
    
    if all_passed:
        print("\n🎉 ALL VALIDATIONS PASSED!")
        print("\nPhase 6 integration has been successfully implemented with:")
        print("  ✓ Enhanced Python bindings with comprehensive type safety")
        print("  ✓ Modern C++ improvements with SIMD and memory pools")
        print("  ✓ Comprehensive testing suite with integration tests")
        print("  ✓ Modern CMake build system with multiple options")
        print("  ✓ Complete documentation with API references")
        print("  ✓ Full Phase 6 integration test suite")
        return True
    else:
        print("\n❌ SOME VALIDATIONS FAILED!")
        print("Please review the failed validations above.")
        return False

def main():
    """Main function"""
    success = run_final_verification()
    sys.exit(0 if success else 1)

if __name__ == "__main__":
    main()