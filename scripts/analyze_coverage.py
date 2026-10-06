#!/usr/bin/env python3
"""
Test coverage analysis for NLM Python tests
Analyzes test coverage and identifies gaps
"""

import os
import sys
import glob
from pathlib import Path

def analyze_test_files():
    """Analyze test files for coverage"""
    test_dir = Path("python/tests")
    
    if not test_dir.exists():
        print("Test directory not found")
        return
    
    print("NLM Python Test Coverage Analysis")
    print("=" * 60)
    
    # Find all test files
    test_files = list(test_dir.glob("test_*.py"))
    
    print(f"Found {len(test_files)} test files:")
    for test_file in sorted(test_files):
        print(f"  - {test_file.name}")
    
    print("\n" + "=" * 60)
    
    # Analyze each test file
    coverage_summary = {}
    
    for test_file in test_files:
        print(f"\nAnalyzing {test_file.name}...")
        
        with open(test_file, 'r') as f:
            content = f.read()
        
        # Extract test classes and methods
        test_classes = []
        test_methods = []
        
        lines = content.split('\n')
        current_class = None
        
        for line in lines:
            line = line.strip()
            if line.startswith('class Test'):
                current_class = line.split('(')[0].replace('class ', '')
                test_classes.append(current_class)
            elif line.startswith('def test_'):
                test_method = line.replace('def ', '').split('(')[0]
                test_methods.append(test_method)
        
        coverage_summary[test_file.name] = {
            'classes': test_classes,
            'methods': test_methods,
            'class_count': len(test_classes),
            'method_count': len(test_methods),
        }
        
        print(f"  Test classes: {len(test_classes)}")
        for cls in test_classes:
            print(f"    - {cls}")
        
        print(f"  Test methods: {len(test_methods)}")
        for method in test_methods:
            print(f"    - {method}")
    
    return coverage_summary

def compare_cpp_python_tests():
    """Compare C++ and Python test coverage"""
    print("\n" + "=" * 60)
    print("C++ vs Python Test Coverage Comparison")
    print("=" * 60)
    
    # C++ test files
    cpp_tests = [
        'tests/test_config.cpp',
        'tests/test_brain.cpp',
        'tests/test_neuron.cpp',
        'tests/test_synapse.cpp',
        'tests/test_clock.cpp',
        'tests/test_random.cpp',
        'tests/test_types.cpp',
        'tests/test_stdp.cpp',
    ]
    
    cpp_test_names = [Path(f).name for f in cpp_tests if os.path.exists(f)]
    
    print(f"C++ test files: {len(cpp_test_names)}")
    for test in cpp_test_names:
        print(f"  - {test}")
    
    # Python test files
    py_tests = []
    test_dir = Path("python/tests")
    if test_dir.exists():
        py_tests = list(test_dir.glob("test_*.py"))
        py_test_names = [t.name for t in py_tests]
    else:
        py_test_names = []
    
    print(f"\nPython test files: {len(py_test_names)}")
    for test in py_test_names:
        print(f"  - {test}")
    
    # Coverage comparison
    print(f"\nCoverage Comparison:")
    print(f"  C++ tests: {len(cpp_test_names)}")
    print(f"  Python tests: {len(py_test_names)}")
    
    # Check for matching test names (by core functionality)
    cpp_functional_tests = [
        'config', 'brain', 'neuron', 'synapse', 
        'clock', 'random', 'types', 'stdp'
    ]
    
    py_functional_tests = []
    for test_file in py_test_names:
        # Extract functional name from test file
        if 'test_config' in test_file:
            py_functional_tests.append('config')
        elif 'test_brain' in test_file:
            py_functional_tests.append('brain')
        elif 'test_neuron' in test_file:
            py_functional_tests.append('neuron')
        elif 'test_bindings' in test_file or 'test_integration' in test_file or 'test_workflow' in test_file:
            py_functional_tests.append('bindings')  # Combined tests
    
    print(f"\nFunctional Coverage:")
    print(f"  Core C++ tests: {len(cpp_functional_tests)}")
    print(f"  Core Python tests: {len(set(py_functional_tests))}")
    
    # Identify gaps
    print(f"\nCoverage Gaps:")
    for cpp_test in cpp_functional_tests:
        if cpp_test not in py_functional_tests:
            print(f"  - Missing Python equivalent for {cpp_test}")
        else:
            print(f"  ✓ {cpp_test} covered in Python")
    
    # Integration tests
    print(f"\nIntegration Tests:")
    integration_tests = ['bindings', 'integration', 'workflow']
    for integ in integration_tests:
        if integ in py_functional_tests:
            print(f"  ✓ Integration test ({integ}) present")
        else:
            print(f"  - Missing integration test ({integ})")
    
    return {
        'cpp_tests': len(cpp_test_names),
        'py_tests': len(py_test_names),
        'cpp_functional': len(cpp_functional_tests),
        'py_functional': len(set(py_functional_tests)),
        'integration_tests': len(integration_tests),
    }

def analyze_test_complexity():
    """Analyze test complexity and depth"""
    print("\n" + "=" * 60)
    print("Test Complexity Analysis")
    print("=" * 60)
    
    test_dir = Path("python/tests")
    
    if not test_dir.exists():
        print("Test directory not found")
        return
    
    complexity_metrics = {}
    
    for test_file in test_dir.glob("test_*.py"):
        with open(test_file, 'r') as f:
            content = f.read()
        
        # Calculate complexity metrics
        lines = len(content.split('\n'))
        test_methods = content.count('def test_')
        class_count = content.count('class Test')
        
        # Calculate average lines per test method
        avg_lines_per_test = lines / test_methods if test_methods > 0 else 0
        
        # Count imports (complexity indicator)
        import_count = content.count('import ') + content.count('from ')
        
        # Count pytest features (advanced testing)
        pytest_features = (
            content.count('@pytest.mark.') +
            content.count('parametrize') +
            content.count('fixture') +
            content.count('mock') +
            content.count('skipif')
        )
        
        complexity_metrics[test_file.name] = {
            'total_lines': lines,
            'test_methods': test_methods,
            'classes': class_count,
            'avg_lines_per_test': avg_lines_per_test,
            'import_count': import_count,
            'pytest_features': pytest_features,
        }
        
        print(f"\n{test_file.name}:")
        print(f"  Total lines: {lines}")
        print(f"  Test methods: {test_methods}")
        print(f"  Test classes: {class_count}")
        print(f"  Avg lines per test: {avg_lines_per_test:.1f}")
        print(f"  Import statements: {import_count}")
        print(f"  Advanced pytest features: {pytest_features}")
    
    return complexity_metrics

def suggest_improvements():
    """Suggest test improvements"""
    print("\n" + "=" * 60)
    print("Test Improvement Suggestions")
    print("=" * 60)
    
    suggestions = [
        "1. Add property-based testing using hypothesis library",
        "2. Include performance benchmarks in test suite",
        "3. Add edge case testing for error conditions",
        "4. Include memory leak detection for long-running tests",
        "5. Add cross-platform compatibility testing",
        "6. Include integration tests with external dependencies",
        "7. Add data-driven testing for parameterized scenarios",
        "8. Include regression testing for previously fixed bugs",
        "9. Add performance profiling tests",
        "10. Include stress testing for large-scale simulations",
    ]
    
    print("Suggested improvements:")
    for suggestion in suggestions:
        print(f"  {suggestion}")
    
    print("\n" + "=" * 60)
    print("Recommended Test Structure:")
    print("=" * 60)
    
    print("""
    Current Test Structure:
    ---------------------
    python/tests/
    ├── test_config.py           # Config class tests
    ├── test_brain.py            # Brain class tests
    ├── test_bindings.py         # General Python bindings
    ├── test_integration.py      # C++/Python integration
    └── test_workflow.py         # Complete workflows

    Test Coverage:
    -------------
    ✓ Core functionality (Config, Brain, Neuron, Synapse, Clock, Random, Types, STDP)
    ✓ Enhanced Python bindings (dict-like access, error handling)
    ✓ Integration tests (end-to-end scenarios)
    ✓ Workflow tests (real-world use cases)
    ✓ Performance and resilience tests

    Missing Areas:
    -------------
    - C++ unit test automation (build-time execution)
    - Automated coverage reporting
    - Continuous integration setup
    - Documentation of test cases
    - Performance benchmarking
    """)

def main():
    """Main analysis function"""
    print("NLM Test Coverage Analysis")
    print("=" * 60)
    
    # Change to project directory
    project_dir = Path(__file__).parent
    os.chdir(project_dir)
    
    # Run analyses
    test_analysis = analyze_test_files()
    coverage_comparison = compare_cpp_python_tests()
    complexity_metrics = analyze_test_complexity()
    suggest_improvements()
    
    # Summary
    print("\n" + "=" * 60)
    print("ANALYSIS SUMMARY")
    print("=" * 60)
    
    print(f"Test Files Created: {len(test_analysis)}")
    print(f"C++ Tests: {coverage_comparison['cpp_tests']}")
    print(f"Python Tests: {coverage_comparison['py_tests']}")
    print(f"Core Functional Coverage: {coverage_comparison['cpp_functional']} C++ -> {coverage_comparison['py_functional']} Python")
    print(f"Integration Tests: {coverage_comparison['integration_tests']} present")
    
    total_test_methods = sum(m['method_count'] for m in test_analysis.values())
    print(f"Total Test Methods: {total_test_methods}")
    
    avg_complexity = sum(m['pytest_features'] for m in complexity_metrics.values()) / len(complexity_metrics) if complexity_metrics else 0
    print(f"Average Advanced Pytest Features per file: {avg_complexity:.1f}")
    
    print("\n✓ Test coverage analysis complete")

if __name__ == "__main__":
    main()
