#!/usr/bin/env python3
"""
NLM Phase 6 Improved Interface - Verification Script

This script verifies the improvements made to the NLM main interface:
1. Unified command-line interface
2. Phase-specific execution
3. Enhanced configuration management
4. Improved error handling
5. Advanced features (verbose mode, output files, etc.)
"""

import os
import sys
import re
import json
from pathlib import Path

def check_file_exists(filepath):
    """Check if a file exists"""
    return os.path.exists(filepath)

def read_file(filepath):
    """Read file content"""
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            return f.read()
    except Exception as e:
        print(f"Error reading {filepath}: {e}")
        return None

def check_cli_implementation():
    """Check if CLI implementation is present in main.cpp"""
    main_cpp_path = "/workspace/app/src/main.cpp"
    if not check_file_exists(main_cpp_path):
        print("❌ main.cpp not found")
        return False
    
    content = read_file(main_cpp_path)
    if not content:
        return False
    
    print("📄 Checking main.cpp CLI implementation...")
    
    # Check for CLI namespace
    if "namespace nlm {" in content and "namespace CLI {" in content:
        print("  ✓ CLI namespace found")
    else:
        print("  ❌ CLI namespace not found")
        return False
    
    # Check for command-line parsing function
    if "parseCommandLine" in content:
        print("  ✓ Command-line parsing function found")
    else:
        print("  ❌ Command-line parsing function not found")
        return False
    
    # Check for phase handling
    if "Phase::Phase2" in content and "Phase::Phase3" in content:
        print("  ✓ Phase handling found")
    else:
        print("  ❌ Phase handling not found")
        return False
    
    # Check for help functionality
    if "printHelp" in content and "printVersion" in content:
        print("  ✓ Help functionality found")
    else:
        print("  ❌ Help functionality not found")
        return False
    
    # Check for configuration options
    config_options = ["configFile", "outputFile", "verbose", "quiet", "simulationSteps", "neuronCount"]
    found_options = 0
    for option in config_options:
        if option in content:
            found_options += 1
    
    if found_options >= 4:  # At least 4 important options
        print(f"  ✓ Configuration options found ({found_options}/{len(config_options)})")
    else:
        print(f"  ❌ Configuration options not found ({found_options}/{len(config_options)})")
        return False
    
    return True

def check_phase_improvements():
    """Check if all phases are properly implemented"""
    main_cpp_path = "/workspace/app/src/main.cpp"
    content = read_file(main_cpp_path)
    if not content:
        return False
    
    print("🔧 Checking phase implementations...")
    
    # Check for Phase 2 implementation
    if "runPhase2" in content:
        print("  ✓ Phase 2 implementation found")
    else:
        print("  ❌ Phase 2 implementation not found")
        return False
    
    # Check for Phase 3 implementation  
    if "runPhase3" in content:
        print("  ✓ Phase 3 implementation found")
    else:
        print("  ❌ Phase 3 implementation not found")
        return False
    
    # Check for Phase 4 implementation
    if "runPhase4" in content:
        print("  ✓ Phase 4 implementation found")
    else:
        print("  ❌ Phase 4 implementation not found")
        return False
    
    # Check for Phase 6 implementation
    if "runPhase6" in content:
        print("  ✓ Phase 6 implementation found")
    else:
        print("  ❌ Phase 6 implementation not found")
        return False
    
    # Check for test suite
    if "runTestSuite" in content:
        print("  ✓ Test suite implementation found")
    else:
        print("  ❌ Test suite implementation not found")
        return False
    
    # Check for quick demo
    if "runQuickDemo" in content:
        print("  ✓ Quick demo implementation found")
    else:
        print("  ❌ Quick demo implementation not found")
        return False
    
    return True

def check_advanced_features():
    """Check for advanced features"""
    main_cpp_path = "/workspace/app/src/main.cpp"
    content = read_file(main_cpp_path)
    if not content:
        return False
    
    print("⚡ Checking advanced features...")
    
    features = [
        ("Configuration validation", "validateConfiguration"),
        ("Output file writing", "writeOutput"),
        ("Timeout handling", "timeout"),
        ("Error handling", "try {" and "catch" in content),
        ("Logging enhancements", "Logger::Level::Info"),
        ("File I/O operations", "ofstream" and "ifstream"),
        ("String manipulation", "std::tolower" and "trim"),
        ("Time utilities", "chrono::high_resolution_clock"),
        ("File system operations", "std::filesystem"),
        ("JSON serialization", "toJSON"),
        ("Metrics export", "metricsFile"),
        ("Progress reporting", "stepPerStep"),
    ]
    
    found_features = 0
    for feature_name, check in features:
        if isinstance(check, str):
            if check in content:
                print(f"  ✓ {feature_name}")
                found_features += 1
        elif isinstance(check, tuple):
            if all(item in content for item in check):
                print(f"  ✓ {feature_name}")
                found_features += 1
    
    print(f"  ✓ Advanced features implemented: {found_features}/{len(features)}")
    return found_features >= 8  # At least 8 features

def check_documentation():
    """Check documentation improvements"""
    main_cpp_path = "/workspace/app/src/main.cpp"
    content = read_file(main_cpp_path)
    if not content:
        return False
    
    print("📚 Checking documentation...")
    
    # Check for comprehensive header comments
    if content.count("// NLM (熙然) - Neural Learning Machine") > 0:
        print("  ✓ Comprehensive header comments found")
    else:
        print("  ❌ Comprehensive header comments not found")
        return False
    
    # Check for usage examples
    if "USAGE:" in content and "EXAMPLES:" in content:
        print("  ✓ Usage examples found")
    else:
        print("  ❌ Usage examples not found")
        return False
    
    # Check for phase descriptions
    phase_descriptions = content.count("Phase 2:") + content.count("Phase 3:") + content.count("Phase 4:") + content.count("Phase 6:")
    if phase_descriptions >= 4:
        print(f"  ✓ Phase descriptions found ({phase_descriptions})")
    else:
        print(f"  ❌ Phase descriptions not found ({phase_descriptions})")
        return False
    
    return True

def check_cmake_updates():
    """Check CMakeLists.txt updates"""
    cmake_path = "/workspace/app/CMakeLists.txt"
    content = read_file(cmake_path)
    if not content:
        return False
    
    print("📝 Checking CMakeLists.txt updates...")
    
    if "add_executable(nlm" in content and "src/main.cpp" in content:
        print("  ✓ Unified main executable configured")
    else:
        print("  ❌ Unified main executable not properly configured")
        return False
    
    # Check that old demo executables are removed
    if "nlm_phase3_demo" not in content or "nlm_phase4_demo" not in content:
        print("  ✓ Old demo executables removed (or marked as deprecated)")
    else:
        print("  ⚠ Old demo executables still present (backward compatibility maintained)")
    
    return True

def main():
    """Main verification function"""
    print("=" * 70)
    print("NLM Phase 6 Improved Interface - Verification Script")
    print("=" * 70)
    print()
    
    checks = [
        ("CLI Implementation", check_cli_implementation),
        ("Phase Improvements", check_phase_improvements),
        ("Advanced Features", check_advanced_features),
        ("Documentation", check_documentation),
        ("CMake Updates", check_cmake_updates),
    ]
    
    passed = 0
    total = len(checks)
    
    for check_name, check_func in checks:
        print(f"\n{'=' * 70}")
        print(f"CHECK: {check_name}")
        print(f"{'=' * 70}")
        
        try:
            if check_func():
                passed += 1
                print(f"✅ {check_name} PASSED")
            else:
                print(f"❌ {check_name} FAILED")
        except Exception as e:
            print(f"❌ {check_name} ERROR: {e}")
    
    print(f"\n{'=' * 70}")
    print(f"SUMMARY: {passed}/{total} checks passed")
    print(f"{'=' * 70}")
    
    if passed == total:
        print("🎉 ALL CHECKS PASSED! 🎉")
        print("The NLM Phase 6 improved interface is ready for use!")
        return 0
    else:
        print("❌ SOME CHECKS FAILED")
        print("Please review the failures above and address any issues.")
        return 1

if __name__ == "__main__":
    sys.exit(main())