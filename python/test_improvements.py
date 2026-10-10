#!/usr/bin/env python3

"""
NLM Improvement Validation Test

This script validates that all improvements made to the NLM project are working correctly.
It tests the Python bindings, new features, documentation, and examples.
"""

import os
import sys
import importlib
import tempfile
import shutil
from pathlib import Path

class NLMValidator:
    def __init__(self):
        self.project_root = Path(__file__).parent.parent
        self.passed_tests = 0
        self.failed_tests = 0
        self.warnings = []
    
    def log_test(self, name, success, message=""):
        """Log a test result."""
        status = "PASS" if success else "FAIL"
        print(f"[{status}] {name}")
        if message:
            print(f"    {message}")
        
        if success:
            self.passed_tests += 1
        else:
            self.failed_tests += 1
    
    def test_python_package_structure(self):
        """Test that Python package structure is correct."""
        print("\n=== Testing Python Package Structure ===")
        
        python_dir = self.project_root / "python"
        
        # Check for required files
        required_files = [
            "__init__.py",
            "pynlm.py",
            "pynlm_core.py",
            "build.py",
            "CMakeLists.txt",
            "setup.py.in",
            "bindings.cpp",
        ]
        
        all_exist = True
        for file in required_files:
            file_path = python_dir / file
            if file_path.exists():
                self.log_test(f"Python file exists: {file}", True)
            else:
                self.log_test(f"Python file exists: {file}", False, f"Missing {file_path}")
                all_exist = False
        
        return all_exist
    
    def test_python_imports(self):
        """Test that Python modules can be imported."""
        print("\n=== Testing Python Imports ===")
        
        try:
            # Try to import pynlm
            import pynlm
            self.log_test("Import pynlm", True, f"NLM version: {getattr(pynlm, '__version__', 'unknown')}")
        except ImportError as e:
            self.log_test("Import pynlm", False, f"ImportError: {e}")
            return False
        
        try:
            # Try to import pynlm_core
            from python import pynlm_core
            self.log_test("Import pynlm_core", True)
        except ImportError as e:
            self.log_test("Import pynlm_core", False, f"ImportError: {e}")
            return False
        
        try:
            # Try to access key classes
            import pynlm
            config = pynlm.createDefaultConfig()
            self.log_test("Create default config", True)
            
            brain = pynlm.createBrain(config)
            self.log_test("Create brain", True)
            
            world = pynlm.createSimpleWorld()
            self.log_test("Create simple world", True)
            
            agent = pynlm.createAgentBrain(brain)
            self.log_test("Create agent brain", True)
            
        except Exception as e:
            self.log_test("Create basic objects", False, f"Error: {e}")
            return False
        
        return True
    
    def test_build_script(self):
        """Test that build script exists and is executable."""
        print("\n=== Testing Build Script ===")
        
        build_script = self.project_root / "python" / "build.py"
        
        if build_script.exists():
            self.log_test("Build script exists", True)
            
            # Check if it's executable (Unix)
            if os.access(build_script, os.X_OK):
                self.log_test("Build script is executable", True)
            else:
                self.log_test("Build script is executable", False, "Script is not executable")
                self.warnings.append("Build script should be executable")
            
            # Check if it has the required shebang
            try:
                with open(build_script, 'r') as f:
                    first_line = f.readline().strip()
                    if first_line.startswith('#!/usr/bin/env python3'):
                        self.log_test("Build script has correct shebang", True)
                    else:
                        self.log_test("Build script has correct shebang", False, 
                                    f"Unexpected shebang: {first_line}")
            except Exception as e:
                self.log_test("Build script has correct shebang", False, f"Error reading: {e}")
        else:
            self.log_test("Build script exists", False, f"Missing {build_script}")
            return False
        
        return True
    
    def test_documentation(self):
        """Test that documentation exists."""
        print("\n=== Testing Documentation ===")
        
        doc_dir = self.project_root / "python"
        
        # Check for documentation files
        doc_files = [
            "DOCUMENTATION.md",
        ]
        
        all_exist = True
        for doc_file in doc_files:
            doc_path = doc_dir / doc_file
            if doc_path.exists():
                self.log_test(f"Documentation exists: {doc_file}", True)
            else:
                self.log_test(f"Documentation exists: {doc_file}", False, f"Missing {doc_path}")
                all_exist = False
        
        return all_exist
    
    def test_examples(self):
        """Test that examples exist and are executable."""
        print("\n=== Testing Examples ===")
        
        examples_dir = self.project_root / "examples"
        
        if not examples_dir.exists():
            self.log_test("Examples directory exists", False, f"Missing {examples_dir}")
            return False
        
        # Check for example files
        example_files = [
            "minimal_example.py",
            "agent_example.py",
            "benchmark.py",
        ]
        
        all_exist = True
        for example_file in example_files:
            example_path = examples_dir / example_file
            if example_path.exists():
                self.log_test(f"Example exists: {example_file}", True)
                
                # Check if it's executable (Unix)
                if os.access(example_path, os.X_OK):
                    self.log_test(f"Example is executable: {example_file}", True)
                else:
                    self.log_test(f"Example is executable: {example_file}", False, "Script is not executable")
                    self.warnings.append(f"Example {example_file} should be executable")
            else:
                self.log_test(f"Example exists: {example_file}", False, f"Missing {example_path}")
                all_exist = False
        
        return all_exist
    
    def test_improvement_plan(self):
        """Test that improvement plan exists."""
        print("\n=== Testing Improvement Plan ===")
        
        plan_file = self.project_root / "IMPROVEMENT_PLAN.md"
        
        if plan_file.exists():
            self.log_test("Improvement plan exists", True)
            
            # Try to read and check for key sections
            try:
                with open(plan_file, 'r') as f:
                    content = f.read()
                    if "Executive Summary" in content:
                        self.log_test("Improvement plan has executive summary", True)
                    else:
                        self.log_test("Improvement plan has executive summary", False)
                
                if "Current State Assessment" in content:
                    self.log_test("Improvement plan has current state assessment", True)
                else:
                    self.log_test("Improvement plan has current state assessment", False)
                
                if "Implementation Timeline" in content:
                    self.log_test("Improvement plan has implementation timeline", True)
                else:
                    self.log_test("Improvement plan has implementation timeline", False)
                
            except Exception as e:
                self.log_test("Improvement plan structure check", False, f"Error reading: {e}")
        else:
            self.log_test("Improvement plan exists", False, f"Missing {plan_file}")
            return False
        
        return True
    
    def test_cmake_integration(self):
        """Test CMake integration changes."""
        print("\n=== Testing CMake Integration ===")
        
        cmake_file = self.project_root / "CMakeLists.txt"
        
        if cmake_file.exists():
            try:
                with open(cmake_file, 'r') as f:
                    content = f.read()
                    
                    if "add_subdirectory(python)" in content:
                        self.log_test("CMake includes python bindings", True)
                    else:
                        self.log_test("CMake includes python bindings", False, 
                                    "Missing add_subdirectory(python)")
                    
                    if "Python" in content:
                        self.log_test("CMake has Python language support", True)
                    else:
                        self.log_test("CMake has Python language support", False)
                        
            except Exception as e:
                self.log_test("CMake integration check", False, f"Error reading: {e}")
                return False
        else:
            self.log_test("CMakeLists.txt exists", False, f"Missing {cmake_file}")
            return False
        
        return True
    
    def test_python_bindings_cpp(self):
        """Test that Python bindings C++ file exists and has content."""
        print("\n=== Testing Python Bindings C++ ===")
        
        bindings_file = self.project_root / "python" / "bindings.cpp"
        
        if bindings_file.exists():
            try:
                with open(bindings_file, 'r') as f:
                    content = f.read()
                    
                    # Check for key bindings
                    if "PYBIND11_MODULE(pynlm" in content:
                        self.log_test("Bindings module declaration", True)
                    else:
                        self.log_test("Bindings module declaration", False)
                    
                    if "Brain" in content:
                        self.log_test("Brain class binding", True)
                    else:
                        self.log_test("Brain class binding", False)
                    
                    if "Config" in content:
                        self.log_test("Config class binding", True)
                    else:
                        self.log_test("Config class binding", False)
                    
                    if len(content) > 1000:  # Should be substantial
                        self.log_test("Bindings file has substantial content", True, f"Size: {len(content)} bytes")
                    else:
                        self.log_test("Bindings file has substantial content", False, f"Too small: {len(content)} bytes")
                        
            except Exception as e:
                self.log_test("Python bindings C++ check", False, f"Error reading: {e}")
                return False
        else:
            self.log_test("Python bindings C++ exists", False, f"Missing {bindings_file}")
            return False
        
        return True
    
    def test_directory_structure(self):
        """Test overall directory structure."""
        print("\n=== Testing Directory Structure ===")
        
        expected_dirs = [
            "python/",
            "examples/",
        ]
        
        all_exist = True
        for dir_path in expected_dirs:
            full_path = self.project_root / dir_path
            if full_path.exists():
                self.log_test(f"Directory exists: {dir_path}", True)
            else:
                self.log_test(f"Directory exists: {dir_path}", False, f"Missing {full_path}")
                all_exist = False
        
        return all_exist
    
    def run_all_tests(self):
        """Run all validation tests."""
        print("=" * 60)
        print("NLM Improvement Validation Test Suite")
        print("=" * 60)
        
        tests = [
            self.test_directory_structure,
            self.test_python_package_structure,
            self.test_python_imports,
            self.test_build_script,
            self.test_documentation,
            self.test_examples,
            self.test_improvement_plan,
            self.test_cmake_integration,
            self.test_python_bindings_cpp,
        ]
        
        for test in tests:
            try:
                test()
            except Exception as e:
                print(f"ERROR in {test.__name__}: {e}")
                self.failed_tests += 1
        
        print("\n" + "=" * 60)
        print("TEST SUMMARY")
        print("=" * 60)
        print(f"Total tests passed: {self.passed_tests}")
        print(f"Total tests failed: {self.failed_tests}")
        print(f"Total warnings: {len(self.warnings)}")
        
        if self.warnings:
            print("\nWarnings:")
            for warning in self.warnings:
                print(f"  - {warning}")
        
        success = self.failed_tests == 0
        print(f"\nOverall result: {'SUCCESS' if success else 'FAILURE'}")
        
        return success

def main():
    """Main function."""
    validator = NLMValidator()
    
    if validator.run_all_tests():
        print("\n✅ All validation tests passed!")
        print("The NLM improvements have been successfully implemented.")
        sys.exit(0)
    else:
        print("\n❌ Some validation tests failed.")
        print("Please review the failed tests and address the issues.")
        sys.exit(1)

if __name__ == "__main__":
    main()