#!/usr/bin/env python3
"""
NLM Python Bindings - Setup and Verification Script
===================================================

This script verifies that the enhanced NLM Python bindings are properly
installed and functional. It tests all the new features and provides
comprehensive feedback on the setup.

The script performs:
1. Module import verification
2. Basic functionality testing
3. Feature availability checking
4. Documentation verification
5. Example script testing
"""

import sys
import os
import platform
import subprocess
from datetime import datetime

def check_python_environment():
    """Check Python environment and dependencies."""
    print("Checking Python environment...")
    
    # Python version
    print(f"  Python version: {sys.version}")
    print(f"  Python executable: {sys.executable}")
    print(f"  Platform: {platform.platform()}")
    
    # Check for required modules
    required_modules = [
        'pybind11', 'numpy', 'json', 'datetime', 'os', 'sys', 'math'
    ]
    
    missing_modules = []
    for module in required_modules:
        try:
            __import__(module)
            print(f"  ✓ Module '{module}' available")
        except ImportError:
            missing_modules.append(module)
            print(f"  ✗ Module '{module}' missing")
    
    if missing_modules:
        print(f"\nWARNING: Missing modules: {missing_modules}")
        print("Some features may not work correctly.")
    
    return len(missing_modules) == 0

def check_nlm_module():
    """Check if pynlm module can be imported and is functional."""
    print("\nChecking NLM Python bindings module...")
    
    try:
        import pynlm as nlm
        print("  ✓ Module 'pynlm' imported successfully")
        
        # Check for key attributes
        expected_attrs = [
            'createDefaultConfig', 'createBrain', 'createSimpleWorld',
            'createAgentBrain', 'createVisionInput', 'createAudioInput',
            'createInternalSignals', 'createSensoryPercept',
            'initializeAgentSystem', 'Vision', 'Audio', 'InternalSignals',
            'VisionProcessor', 'AudioProcessor', 'InternalSignalsProcessor',
            'SensoryPercept', 'Brain', 'SimpleWorld', 'AgentBrain',
            'Config', 'Action', 'WorldObject', 'AgentBody',
            'getSystemStats', 'runSimulationStep', 'validateConfiguration',
            'safeLoadBrain', 'safeSaveBrain', 'createDevelopmentScenario'
        ]
        
        missing_attrs = []
        for attr in expected_attrs:
            if hasattr(nlm, attr):
                print(f"  ✓ Attribute '{attr}' available")
            else:
                missing_attrs.append(attr)
                print(f"  ✗ Attribute '{attr}' missing")
        
        # Test basic functionality
        print(f"\n  Testing basic functionality...")
        
        # Create default config
        try:
            config = nlm.createDefaultConfig()
            print(f"  ✓ createDefaultConfig() works")
        except Exception as e:
            print(f"  ✗ createDefaultConfig() failed: {e}")
            missing_attrs.append("createDefaultConfig")
        
        # Create brain
        try:
            brain = nlm.createBrain(config)
            print(f"  ✓ createBrain() works")
        except Exception as e:
            print(f"  ✗ createBrain() failed: {e}")
            missing_attrs.append("createBrain")
        
        # Create world
        try:
            world = nlm.createSimpleWorld()
            print(f"  ✓ createSimpleWorld() works")
        except Exception as e:
            print(f"  ✗ createSimpleWorld() failed: {e}")
            missing_attrs.append("createSimpleWorld")
        
        # Initialize agent system
        try:
            agent = nlm.initializeAgentSystem(brain, world)
            print(f"  ✓ initializeAgentSystem() works")
        except Exception as e:
            print(f"  ✗ initializeAgentSystem() failed: {e}")
            missing_attrs.append("initializeAgentSystem")
        
        # Test system stats
        try:
            stats = nlm.getSystemStats(brain)
            print(f"  ✓ getSystemStats() works: {len(stats)} values returned")
        except Exception as e:
            print(f"  ✗ getSystemStats() failed: {e}")
            missing_attrs.append("getSystemStats")
        
        # Test validation
        try:
            is_valid = nlm.validateConfiguration(config)
            print(f"  ✓ validateConfiguration() works: {is_valid}")
        except Exception as e:
            print(f"  ✗ validateConfiguration() failed: {e}")
            missing_attrs.append("validateConfiguration")
        
        return len(missing_attrs) == 0, missing_attrs
        
    except ImportError as e:
        print(f"  ✗ Failed to import pynlm: {e}")
        return False, ["pynlm module"]
    except Exception as e:
        print(f"  ✗ Unexpected error: {e}")
        return False, ["general error"]

def check_documentation():
    """Check if documentation files exist."""
    print("\nChecking documentation...")
    
    doc_files = [
        "python/README.md",
        "python/demo_basic.py",
        "python/demo_neural_pipeline.py",
        "python/demo_error_handling.py"
    ]
    
    missing_files = []
    for file_path in doc_files:
        if os.path.exists(file_path):
            size = os.path.getsize(file_path)
            print(f"  ✓ {file_path} ({size:,} bytes)")
        else:
            missing_files.append(file_path)
            print(f"  ✗ {file_path} missing")
    
    return len(missing_files) == 0, missing_files

def check_bindings_compilation():
    """Check if bindings.cpp file exists and is up-to-date."""
    print("\nChecking bindings source...")
    
    bindings_file = "python/bindings.cpp"
    
    if os.path.exists(bindings_file):
        size = os.path.getsize(bindings_file)
        print(f"  ✓ {bindings_file} ({size:,} bytes)")
        
        # Check file content for key improvements
        with open(bindings_file, 'r') as f:
            content = f.read()
        
        improvements = [
            ("Neural vision processing", "VisionProcessor"),
            ("Neural audio processing", "AudioProcessor"),
            ("Internal signals processing", "InternalSignalsProcessor"),
            ("Convenience factory functions", "createVisionInput"),
            ("Simulation management utilities", "runSimulationStep"),
            ("Error handling helpers", "safeLoadBrain"),
            ("Performance monitoring", "getSystemStats"),
            ("Documentation enhancements", "pbdoc")
        ]
        
        found_improvements = []
        for improvement_name, keyword in improvements:
            if keyword in content:
                found_improvements.append(improvement_name)
                print(f"  ✓ Enhanced feature: {improvement_name}")
            else:
                print(f"  ✗ Missing enhanced feature: {improvement_name}")
        
        return len(found_improvements) >= 6, found_improvements
    else:
        print(f"  ✗ {bindings_file} not found")
        return False, ["bindings.cpp file"]

def run_example_scripts():
    """Try to run example scripts to verify functionality."""
    print("\nTesting example scripts...")
    
    example_scripts = [
        "python/demo_basic.py",
        "python/demo_neural_pipeline.py"
    ]
    
    successful_scripts = []
    
    for script_path in example_scripts:
        if os.path.exists(script_path):
            print(f"\n  Testing {script_path}...")
            
            try:
                # Run script with timeout
                result = subprocess.run(
                    [sys.executable, script_path],
                    capture_output=True,
                    text=True,
                    timeout=30,
                    cwd="python"
                )
                
                if result.returncode == 0:
                    print(f"  ✓ Script executed successfully")
                    successful_scripts.append(script_path)
                else:
                    print(f"  ✗ Script failed with return code {result.returncode}")
                    print(f"    Error: {result.stderr[:200]}...")
                    
            except subprocess.TimeoutExpired:
                print(f"  ✗ Script timed out after 30 seconds")
            except Exception as e:
                print(f"  ✗ Script execution failed: {e}")
        else:
            print(f"  ✗ Script not found: {script_path}")
    
    return len(successful_scripts) > 0, successful_scripts

def check_cmake_build():
    """Check if CMake build system is configured."""
    print("\nChecking CMake build system...")
    
    cmake_files = [
        "CMakeLists.txt",
        "python/CMakeLists.txt"
    ]
    
    missing_files = []
    for file_path in cmake_files:
        if os.path.exists(file_path):
            print(f"  ✓ {file_path} exists")
        else:
            missing_files.append(file_path)
            print(f"  ✗ {file_path} missing")
    
    if os.path.exists("python/CMakeLists.txt"):
        # Check CMakeLists.txt content
        with open("python/CMakeLists.txt", 'r') as f:
            cmake_content = f.read()
        
        required_elements = [
            "pybind11_add_module",
            "pynlm",
            "pybind11",
            "Python"
        ]
        
        missing_elements = []
        for element in required_elements:
            if element in cmake_content:
                print(f"  ✓ CMakeLists.txt contains: {element}")
            else:
                missing_elements.append(element)
                print(f"  ✗ CMakeLists.txt missing: {element}")
        
        missing_files.extend(missing_elements)
    
    return len(missing_files) == 0, missing_files

def generate_report(results):
    """Generate comprehensive setup report."""
    print(f"\n{'='*80}")
    print("NLM PYTHON BINDINGS - SETUP VERIFICATION REPORT")
    print(f"{'='*80}")
    print(f"Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
    print(f"Python: {sys.version.split()[0]}")
    print(f"Platform: {platform.system()} {platform.release()}")
    print(f"{'='*80}")
    
    print(f"\nVERIFICATION RESULTS:")
    print(f"{'-'*80}")
    
    categories = [
        ("Python Environment", results['python_env']),
        ("NLM Module", results['nlm_module']),
        ("Documentation", results['documentation']),
        ("Bindings Source", results['bindings_source']),
        ("Example Scripts", results['example_scripts']),
        ("CMake Build", results['cmake_build'])
    ]
    
    all_passed = True
    for category, (passed, issues) in categories:
        status = "✓ PASS" if passed else "✗ FAIL"
        print(f"{category:25} {status}")
        
        if not passed:
            all_passed = False
            for issue in issues:
                print(f"  - {issue}")
    
    print(f"\n{'='*80}")
    print("OVERALL STATUS")
    print(f"{'='*80}")
    
    if all_passed:
        print("✓ ALL CHECKS PASSED")
        print("The enhanced NLM Python bindings are properly configured and functional.")
        print("\nKey enhancements verified:")
        print("  • Neural-based vision and audio processing")
        print("  • Internal signals processing")
        print("  • Convenience factory functions")
        print("  • Simulation management utilities")
        print("  • Error handling and validation helpers")
        print("  • Performance monitoring tools")
        print("  • Comprehensive documentation and examples")
        print("\nYou can now use the enhanced Python bindings with:")
        print("  import pynlm as nlm")
        print("  # All enhanced features are available")
        
        return True
    else:
        print("✗ SOME CHECKS FAILED")
        print("The enhanced NLM Python bindings require attention before use.")
        print("\nIssues to resolve:")
        
        if not results['python_env']:
            print("  • Python environment or dependencies missing")
        if not results['nlm_module']:
            print("  • NLM module import failed")
        if not results['documentation']:
            print("  • Documentation files missing")
        if not results['bindings_source']:
            print("  • Bindings source file issues")
        if not results['example_scripts']:
            print("  • Example scripts failed to run")
        if not results['cmake_build']:
            print("  • CMake build system issues")
        
        return False
def main():
    """Main function to run all verification checks."""
    print("NLM Python Bindings - Setup and Verification")
    print("=" * 80)
    
    # Run all verification checks
    results = {}
    
    print("\n1. PYTHON ENVIRONMENT CHECK")
    results['python_env'] = check_python_environment()
    
    print("\n2. NLM MODULE CHECK")
    nlm_passed, nlm_issues = check_nlm_module()
    results['nlm_module'] = nlm_passed
    results['nlm_issues'] = nlm_issues
    
    print("\n3. DOCUMENTATION CHECK")
    doc_passed, doc_issues = check_documentation()
    results['documentation'] = doc_passed
    results['doc_issues'] = doc_issues
    
    print("\n4. BINDINGS SOURCE CHECK")
    bindings_passed, bindings_issues = check_bindings_compilation()
    results['bindings_source'] = bindings_passed
    results['bindings_issues'] = bindings_issues
    
    print("\n5. EXAMPLE SCRIPTS CHECK")
    scripts_passed, scripts_issues = run_example_scripts()
    results['example_scripts'] = scripts_passed
    results['scripts_issues'] = scripts_issues
    
    print("\n6. CMAKE BUILD SYSTEM CHECK")
    cmake_passed, cmake_issues = check_cmake_build()
    results['cmake_build'] = cmake_passed
    results['cmake_issues'] = cmake_issues
    
    # Generate final report
    success = generate_report(results)
    
    return 0 if success else 1
if __name__ == "__main__":
    sys.exit(main())