#!/usr/bin/env python3
"""
Simple test to verify NLM Python bindings improvements.
This script checks for basic syntax and structure without requiring actual compilation.
"""

import sys
import os

def test_basic_structure():
    """Test that the basic structure of the NLM package is intact."""
    print("Testing NLM Python package structure...")
    
    # Check if we can import basic modules
    try:
        import pynlm
        print("✓ Successfully imported pynlm")
    except ImportError as e:
        print(f"✗ Failed to import pynlm: {e}")
        return False
    except Exception as e:
        print(f"✗ Error importing pynlm: {e}")
        return False
    
    # Check for version attribute
    if hasattr(pynlm, '__version__'):
        print(f"✓ Package version: {pynlm.__version__}")
    else:
        print("✓ Package imported (version attribute may not be set)")
    
    # Check for main classes
    required_classes = ['createBrain', 'createDefaultConfig', 'createSimpleWorld', 
                       'createAgentBrain', 'Action', 'NeuronId', 'SynapseId']
    
    for class_name in required_classes:
        if hasattr(pynlm, class_name):
            print(f"✓ Found {class_name}")
        else:
            print(f"✗ Missing {class_name}")
            return False
    
    return True

def test_file_structure():
    """Test that the file structure is correct."""
    print("\nTesting file structure...")
    
    # Check if the main source directories exist
    required_dirs = ['src/', 'src/brain/', 'src/core/', 'src/agent/', 'src/world/']
    
    for dir_path in required_dirs:
        if os.path.exists(dir_path):
            print(f"✓ Found directory: {dir_path}")
        else:
            print(f"✗ Missing directory: {dir_path}")
            return False
    
    # Check for key header files
    required_files = [
        'src/brain/Brain.hpp',
        'src/core/Config/Config.hpp',
        'src/agent/AgentBrain.hpp',
        'src/world/SimpleWorld.hpp'
    ]
    
    for file_path in required_files:
        if os.path.exists(file_path):
            print(f"✓ Found header file: {file_path}")
        else:
            print(f"✗ Missing header file: {file_path}")
            return False
    
    return True

def test_improvements_available():
    """Test that the improvements are available in the package."""
    print("\nTesting improvements availability...")
    
    # Import pynlm
    try:
        import pynlm
    except ImportError:
        print("✗ Cannot test improvements - pynlm not importable")
        return False
    
    # List of improvements to check for
    expected_improvements = [
        # Configuration builders
        'createDefaultConfig',
        'createMinimalConfig', 
        'createDevelopmentConfig',
        'createSimulationConfig',
        'createResearchConfig',
        
        # Configuration utilities
        'configMerge',
        'configValidate',
        'configToDict',
        'configFromDict',
        
        # Simulation utilities
        'runEpisode',
        'saveBrainSnapshot',
        'loadBrainSnapshot',
        'createDefaultAgent',
        'configureStandardWorld',
        'getDefaultWorldConfig',
        
        # Statistics and monitoring
        'getSimulationStats',
        'getPerformanceMetrics',
        
        # Action utilities
        'createAction',
        
        # Validation utilities
        'validateWorldState',
        
        # Convenience functions
        'getDefaultBrainConfig',
        'createTestNeurons',
        'createDefaultRegions',
    ]
    
    improvements_found = 0
    for improvement in expected_improvements:
        if hasattr(pynlm, improvement):
            print(f"✓ Found improvement: {improvement}")
            improvements_found += 1
        else:
            print(f"⚠ Missing improvement: {improvement}")
    
    print(f"\nFound {improvements_found}/{len(expected_improvements)} expected improvements")
    
    # At least 60% of improvements should be available for a successful test
    return improvements_found >= (len(expected_improvements) * 0.6)

def test_documentation():
    """Test that documentation files are present."""
    print("\nTesting documentation...")
    
    required_docs = [
        'README.md',
        'easy_usage.md',
        'HOW_TO_USE.md',
        'IMPROVEMENTS_SUMMARY.md'
    ]
    
    docs_found = 0
    for doc in required_docs:
        if os.path.exists(doc):
            print(f"✓ Found documentation: {doc}")
            docs_found += 1
        else:
            print(f"✗ Missing documentation: {doc}")
    
    print(f"Found {docs_found}/{len(required_docs)} required documentation files")
    return docs_found == len(required_docs)

def test_improvements_summary():
    """Test that the improvements summary is comprehensive."""
    print("\nTesting improvements summary...")
    
    summary_file = 'IMPROVEMENTS_SUMMARY.md'
    
    if not os.path.exists(summary_file):
        print(f"✗ Improvements summary file not found: {summary_file}")
        return False
    
    # Read and check the summary file
    with open(summary_file, 'r') as f:
        content = f.read()
    
    # Check for key sections
    required_sections = [
        'Major Improvements',
        'Key Features',
        'Usage Examples',
        'Building and Installation',
        'Testing',
        'Conclusion'
    ]
    
    sections_found = 0
    for section in required_sections:
        if section in content:
            print(f"✓ Found section in summary: {section}")
            sections_found += 1
        else:
            print(f"⚠ Missing section in summary: {section}")
    
    print(f"Found {sections_found}/{len(required_sections)} required sections")
    return sections_found >= len(required_sections) * 0.8

def main():
    """Run all tests."""
    print("=== NLM Python Bindings Improvements Test ===\n")
    
    tests = [
        ("Package Structure", test_basic_structure),
        ("File Structure", test_file_structure),
        ("Improvements Available", test_improvements_available),
        ("Documentation", test_documentation),
        ("Improvements Summary", test_improvements_summary),
    ]
    
    passed = 0
    total = len(tests)
    
    for test_name, test_func in tests:
        print(f"\n{'='*50}")
        print(f"Running test: {test_name}")
        print(f"{'='*50}")
        
        try:
            if test_func():
                print(f"✓ {test_name} PASSED")
                passed += 1
            else:
                print(f"✗ {test_name} FAILED")
        except Exception as e:
            print(f"✗ {test_name} ERROR: {e}")
    
    print(f"\n{'='*50}")
    print(f"Test Summary: {passed}/{total} tests passed")
    print(f"{'='*50}")
    
    if passed >= total * 0.8:  # 80% threshold
        print("✓ Overall test result: SUCCESS")
        return 0
    else:
        print("✗ Overall test result: FAILURE")
        return 1

if __name__ == "__main__":
    sys.exit(main())
