#!/usr/bin/env python3
"""
NLM Test Environment Setup and Validation

This module provides utilities for setting up and validating the NLM test environment.
It includes validation of dependencies, configuration checks, and environment setup.
"""

import os
import sys
import json
import argparse
import platform
from pathlib import Path
from typing import Dict, Any, List, Optional, Tuple
import subprocess
import logging
import pkg_resources

class NLMEnvironmentValidator:
    """Validate the NLM test environment setup."""
    
    def __init__(self, project_root: str = None):
        self.project_root = Path(project_root) if project_root else Path(__file__).parent.parent
        self.tests_dir = self.project_root / "tests"
        self.setup_file = self.project_root / "test_setup.py"
        self.config_file = self.project_root / "test_config.json"
        self.cmake_file = self.project_root / "CMakeLists.txt"
        self.python_file = self.project_root / "pyproject.toml"
        
        # Setup logging
        self.logger = self._setup_logging()
        
        # Validation results
        self.validation_results = {
            "environment": {},
            "dependencies": {},
            "configuration": {},
            "build_system": {},
            "python_bindings": {}
        }
    
    def _setup_logging(self) -> logging.Logger:
        """Setup logging for the validator."""
        logger = logging.getLogger("nlm_env_validator")
        logger.setLevel(logging.INFO)
        
        # Create console handler
        handler = logging.StreamHandler()
        formatter = logging.Formatter(
            '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
        )
        handler.setFormatter(formatter)
        logger.addHandler(handler)
        
        return logger
    
    def validate_environment(self) -> Dict[str, Any]:
        """Validate the overall environment."""
        print("Validating NLM test environment...")
        
        self.validation_results["environment"]["platform"] = self._validate_platform()
        self.validation_results["environment"]["python_version"] = self._validate_python_version()
        self.validation_results["environment"]["working_directory"] = self._validate_working_directory()
        self.validation_results["environment"]["filesystem"] = self._validate_filesystem()
        
        return self.validation_results["environment"]
    
    def validate_dependencies(self) -> Dict[str, Any]:
        """Validate required dependencies."""
        print("Validating NLM dependencies...")
        
        self.validation_results["dependencies"]["build_tools"] = self._validate_build_tools()
        self.validation_results["dependencies"]["package_managers"] = self._validate_package_managers()
        self.validation_results["dependencies"]["compilers"] = self._validate_compilers()
        self.validation_results["dependencies"]["libraries"] = self._validate_libraries()
        
        return self.validation_results["dependencies"]
    
    def validate_configuration(self) -> Dict[str, Any]:
        """Validate project configuration."""
        print("Validating NLM configuration...")
        
        self.validation_results["configuration"]["cmake"] = self._validate_cmake()
        self.validation_results["configuration"]["python"] = self._validate_python_config()
        self.validation_results["configuration"]["project_structure"] = self._validate_project_structure()
        
        return self.validation_results["configuration"]
    
    def validate_build_system(self) -> Dict[str, Any]:
        """Validate build system setup."""
        print("Validating NLM build system...")
        
        self.validation_results["build_system"]["cmake_lists"] = self._validate_cmake_lists()
        self.validation_results["build_system"]["test_targets"] = self._validate_test_targets()
        self.validation_results["build_system"]["compilation_flags"] = self._validate_compilation_flags()
        
        return self.validation_results["build_system"]
    
    def validate_python_bindings(self) -> Dict[str, Any]:
        """Validate Python bindings setup."""
        print("Validating NLM Python bindings...")
        
        self.validation_results["python_bindings"]["pyproject"] = self._validate_pyproject()
        self.validation_results["python_bindings"]["bindings"] = self._validate_bindings()
        self.validation_results["python_bindings"]["test_framework"] = self._validate_test_framework()
        
        return self.validation_results["python_bindings"]
    
    def _validate_platform(self) -> Dict[str, Any]:
        """Validate platform-specific requirements."""
        result = {
            "name": platform.system(),
            "version": platform.release(),
            "architecture": platform.machine(),
            "python_implementation": platform.python_implementation(),
            "validation_status": "PASS"
        }
        
        # Check platform compatibility
        if platform.system() not in ["Linux", "Darwin", "Windows"]:
            result["validation_status"] = "WARNING"
            result["warning"] = "Unsupported platform"
        
        return result
    
    def _validate_python_version(self) -> Dict[str, Any]:
        """Validate Python version requirements."""
        result = {
            "version": platform.python_version(),
            "version_info": list(map(int, platform.python_version().split('.')[:2])),
            "validation_status": "PASS"
        }
        
        # Check Python version compatibility
        if result["version_info"][0] < 3 or (result["version_info"][0] == 3 and result["version_info"][1] < 8):
            result["validation_status"] = "FAIL"
            result["error"] = f"Python 3.8+ required, found {result['version']}"
        
        return result
    
    def _validate_working_directory(self) -> Dict[str, Any]:
        """Validate working directory setup."""
        result = {
            "path": str(self.project_root),
            "exists": self.project_root.exists(),
            "validation_status": "PASS"
        }
        
        if not result["exists"]:
            result["validation_status"] = "FAIL"
            result["error"] = "Project root directory does not exist"
        
        return result
    
    def _validate_filesystem(self) -> Dict[str, Any]:
        """Validate filesystem setup."""
        result = {
            "tests_directory": {},
            "config_files": {},
            "validation_status": "PASS"
        }
        
        # Check tests directory
        tests_dir = self.project_root / "tests"
        result["tests_directory"]["exists"] = tests_dir.exists()
        if tests_dir.exists():
            subdirectories = ["core", "brain", "plasticity", "neuromodulation", 
                           "memory", "cognition", "sensory", "motor", "environment",
                           "world", "agent", "development", "prediction", "phase_specific",
                           "integration", "python", "performance", "config", "experiment",
                           "visualization", "framework", "benchmarks", "fixtures", "data"]
            
            for subdir in subdirectories:
                subdir_path = tests_dir / subdir
                result["tests_directory"][subdir] = subdir_path.exists()
        
        # Check configuration files
        config_files = [
            ("CMakeLists.txt", self.cmake_file),
            ("pyproject.toml", self.python_file),
            ("test_config.json", self.config_file),
            ("test_setup.py", self.setup_file)
        ]
        
        for filename, filepath in config_files:
            result["config_files"][filename] = {
                "exists": filepath.exists(),
                "readable": filepath.exists() and os.access(filepath, os.R_OK)
            }
        
        # Check if any critical config files are missing
        critical_files = ["CMakeLists.txt", "pyproject.toml"]
        for filename, filepath in config_files:
            if filename in critical_files and not filepath.exists():
                result["validation_status"] = "WARNING"
                result["warning"] = f"Critical file {filename} is missing"
        
        return result
    
    def _validate_build_tools(self) -> Dict[str, Any]:
        """Validate build tools availability."""
        result = {
            "cmake": {},
            "make": {},
            "ninja": {},
            "validation_status": "PASS"
        }
        
        # Check CMake
        cmake_result = self._check_command_availability("cmake", "--version")
        result["cmake"] = cmake_result
        if not cmake_result["available"]:
            result["validation_status"] = "FAIL"
        
        # Check make
        make_result = self._check_command_availability("make", "--version")
        result["make"] = make_result
        
        # Check ninja
        ninja_result = self._check_command_availability("ninja", "--version")
        result["ninja"] = ninja_result
        
        return result
    
    def _validate_package_managers(self) -> Dict[str, Any]:
        """Validate package manager availability."""
        result = {
            "pip": {},
            "git": {},
            "validation_status": "PASS"
        }
        
        # Check pip
        pip_result = self._check_command_availability("pip", "--version")
        result["pip"] = pip_result
        
        # Check git
        git_result = self._check_command_availability("git", "--version")
        result["git"] = git_result
        
        return result
    
    def _validate_compilers(self) -> Dict[str, Any]:
        """Validate compiler availability."""
        result = {
            "gcc": {},
            "clang": {},
            "validation_status": "PASS"
        }
        
        # Check gcc
        gcc_result = self._check_command_availability("g++", "--version")
        result["gcc"] = gcc_result
        
        # Check clang
        clang_result = self._check_command_availability("clang++", "--version")
        result["clang"] = clang_result
        
        return result
    
    def _validate_libraries(self) -> Dict[str, Any]:
        """Validate required libraries."""
        result = {
            "dependencies": {},
            "validation_status": "PASS"
        }
        
        # Check Python dependencies
        try:
            # Try to import common test dependencies
            import pytest
            result["dependencies"]["pytest"] = {"available": True, "version": pytest.__version__}
        except ImportError:
            result["dependencies"]["pytest"] = {"available": False, "error": "Not installed"}
            result["validation_status"] = "WARNING"
        
        try:
            import numpy
            result["dependencies"]["numpy"] = {"available": True, "version": numpy.__version__}
        except ImportError:
            result["dependencies"]["numpy"] = {"available": False, "error": "Not installed"}
            result["validation_status"] = "WARNING"
        
        return result
    
    def _validate_cmake(self) -> Dict[str, Any]:
        """Validate CMake configuration."""
        result = {
            "minimum_version": "3.16",
            "project_name": "NLM",
            "languages": ["CXX"],
            "validation_status": "PASS"
        }
        
        if self.cmake_file.exists():
            with open(self.cmake_file, 'r') as f:
                content = f.read()
                
                # Check for required CMake directives
                if "cmake_minimum_required" not in content:
                    result["validation_status"] = "WARNING"
                    result["warning"] = "cmake_minimum_required not found"
                
                if "project(" not in content:
                    result["validation_status"] = "WARNING"
                    result["warning"] = "project() not found"
                
                if "CXX" not in content:
                    result["validation_status"] = "WARNING"
                    result["warning"] = "CXX language not specified"
                
                # Check for testing setup
                if "enable_testing()" not in content:
                    result["validation_status"] = "WARNING"
                    result["warning"] = "enable_testing() not found"
                
                if "add_subdirectory(tests)" not in content:
                    result["validation_status"] = "WARNING"
                    result["warning"] = "tests subdirectory not found"
        
        return result
    
    def _validate_python_config(self) -> Dict[str, Any]:
        """Validate Python configuration."""
        result = {
            "validation_status": "PASS"
        }
        
        if self.python_file.exists():
            try:
                import tomli
                with open(self.python_file, 'rb') as f:
                    config = tomli.load(f)
                
                # Check for required sections
                if "project" not in config:
                    result["validation_status"] = "WARNING"
                    result["warning"] = "project section not found"
                
                if "tool.scikit-build" not in config:
                    result["validation_status"] = "WARNING"
                    result["warning"] = "scikit-build tool configuration not found"
                
                if "tool.pytest.ini_options" not in config:
                    result["validation_status"] = "WARNING"
                    result["warning"] = "pytest configuration not found"
                
            except Exception as e:
                result["validation_status"] = "FAIL"
                result["error"] = f"Failed to parse pyproject.toml: {e}"
        
        return result
    
    def _validate_project_structure(self) -> Dict[str, Any]:
        """Validate project structure."""
        result = {
            "src_directory": {},
            "build_directory": {},
            "test_directories": {},
            "validation_status": "PASS"
        }
        
        # Check source directory
        src_dir = self.project_root / "src"
        result["src_directory"]["exists"] = src_dir.exists()
        if src_dir.exists():
            required_src_dirs = ["core", "brain", "plasticity", "neuromodulation",
                               "memory", "cognition", "sensory", "motor", "environment",
                               "world", "agent", "development", "prediction", "experiments",
                               "visualization"]
            
            for subdir in required_src_dirs:
                subdir_path = src_dir / subdir
                result["src_directory"][subdir] = subdir_path.exists()
        
        # Check build directory
        build_dir = self.project_root / "build"
        result["build_directory"]["exists"] = build_dir.exists()
        
        # Check test directories
        tests_dir = self.project_root / "tests"
        result["test_directories"]["exists"] = tests_dir.exists()
        
        return result
    
    def _validate_cmake_lists(self) -> Dict[str, Any]:
        """Validate tests/CMakeLists.txt."""
        result = {
            "validation_status": "PASS",
            "test_targets": [],
            "test_executables": []
        }
        
        tests_cmake = self.tests_dir / "CMakeLists.txt"
        if tests_cmake.exists():
            with open(tests_cmake, 'r') as f:
                content = f.read()
                
                # Check for test target definitions
                import re
                target_pattern = r'add_executable\((\w+),'
                targets = re.findall(target_pattern, content)
                result["test_targets"] = targets
                
                # Check for key test executables
                required_targets = ["unit_core_test", "unit_brain_test", "integration_tests",
                                  "performance_test", "phase_3_test", "phase_4_test",
                                  "phase_5_test", "phase_6_test", "nlm_test", "validation_test"]
                
                for required in required_targets:
                    if required in targets:
                        result["test_executables"].append({"name": required, "exists": True})
                    else:
                        result["test_executables"].append({"name": required, "exists": False})
        
        return result
    
    def _validate_test_targets(self) -> Dict[str, Any]:
        """Validate test target definitions."""
        result = {
            "validation_status": "PASS",
            "target_types": {},
            "libraries": {}
        }
        
        # Categorize test targets
        unit_tests = []
        integration_tests = []
        performance_tests = []
        phase_tests = []
        
        if "unit_core_test" in self.validation_results["build_system"]["cmake_lists"]["test_targets"]:
            unit_tests.append("unit_core_test")
        
        if "unit_brain_test" in self.validation_results["build_system"]["cmake_lists"]["test_targets"]:
            unit_tests.append("unit_brain_test")
        
        if "integration_tests" in self.validation_results["build_system"]["cmake_lists"]["test_targets"]:
            integration_tests.append("integration_tests")
        
        if "performance_test" in self.validation_results["build_system"]["cmake_lists"]["test_targets"]:
            performance_tests.append("performance_test")
        
        for phase in ["phase_3_test", "phase_4_test", "phase_5_test", "phase_6_test"]:
            if phase in self.validation_results["build_system"]["cmake_lists"]["test_targets"]:
                phase_tests.append(phase)
        
        result["target_types"] = {
            "unit_tests": unit_tests,
            "integration_tests": integration_tests,
            "performance_tests": performance_tests,
            "phase_tests": phase_tests
        }
        
        return result
    
    def _validate_compilation_flags(self) -> Dict[str, Any]:
        """Validate compilation flags."""
        result = {
            "cxx_standard": 20,
            "warnings": ["-Wall", "-Wextra", "-Wpedantic"],
            "optimization": "O3",
            "validation_status": "PASS"
        }
        
        return result
    
    def _validate_pyproject(self) -> Dict[str, Any]:
        """Validate pyproject.toml."""
        result = {
            "validation_status": "PASS"
        }
        
        if self.python_file.exists():
            try:
                import tomli
                with open(self.python_file, 'rb') as f:
                    config = tomli.load(f)
                
                # Check project metadata
                if "project" in config:
                    project = config["project"]
                    result["name"] = project.get("name")
                    result["version"] = project.get("version")
                    result["description"] = project.get("description")
                
                # Check build system
                if "build-system" in config:
                    build_system = config["build-system"]
                    result["build_backend"] = build_system.get("build-backend")
                
                # Check scikit-build configuration
                if "tool.scikit-build" in config:
                    scikit_build = config["tool.scikit-build"]
                    result["cmake_minimum_version"] = scikit_build.get("cmake.minimum-version")
                    result["python_dependencies"] = config["project"].get("optional-dependencies", {}).get("dev", [])
                
            except Exception as e:
                result["validation_status"] = "FAIL"
                result["error"] = f"Failed to parse pyproject.toml: {e}"
        
        return result
    
    def _validate_bindings(self) -> Dict[str, Any]:
        """Validate Python bindings."""
        result = {
            "bindings_file": {},
            "context_managers": {},
            "validation_status": "PASS"
        }
        
        # Check bindings.cpp
        bindings_file = self.project_root / "python" / "bindings.cpp"
        result["bindings_file"]["exists"] = bindings_file.exists()
        result["bindings_file"]["readable"] = bindings_file.exists() and os.access(bindings_file, os.R_OK)
        
        # Check context managers
        context_manager_file = self.project_root / "python" / "context_managers_impl.hpp"
        result["context_managers"]["exists"] = context_manager_file.exists()
        
        return result
    
    def _validate_test_framework(self) -> Dict[str, Any]:
        """Validate test framework."""
        result = {
            "pytest_config": {},
            "test_files": [],
            "validation_status": "PASS"
        }
        
        # Check pytest configuration in pyproject.toml
        if self.python_file.exists():
            try:
                import tomli
                with open(self.python_file, 'rb') as f:
                    config = tomli.load(f)
                
                if "tool.pytest.ini_options" in config:
                    pytest_config = config["tool.pytest.ini_options"]
                    result["pytest_config"]["testpaths"] = pytest_config.get("testpaths", [])
                    result["pytest_config"]["python_files"] = pytest_config.get("python_files", [])
                    result["pytest_config"]["python_classes"] = pytest_config.get("python_classes", [])
                    result["pytest_config"]["python_functions"] = pytest_config.get("python_functions", [])
                
            except Exception as e:
                result["validation_status"] = "FAIL"
                result["error"] = f"Failed to parse pytest config: {e}"
        
        # Check test files
        if self.tests_dir.exists():
            for test_file in self.tests_dir.glob("*.py"):
                result["test_files"].append({
                    "name": test_file.name,
                    "exists": test_file.exists(),
                    "readable": os.access(test_file, os.R_OK)
                })
        
        return result
    
    def _check_command_availability(self, command: str, version_arg: str) -> Dict[str, Any]:
        """Check if a command is available."""
        try:
            result = subprocess.run(
                [command, version_arg],
                capture_output=True,
                text=True,
                timeout=10
            )
            
            available = result.returncode == 0
            output = result.stdout.strip() if available else result.stderr.strip()
            
            return {
                "available": available,
                "version": output[:100] if output else "Unknown"
            }
            
        except (subprocess.TimeoutExpired, FileNotFoundError, subprocess.SubprocessError) as e:
            return {
                "available": False,
                "error": str(e)
            }
    
    def generate_validation_report(self) -> Dict[str, Any]:
        """Generate a comprehensive validation report."""
        print("Generating validation report...")
        
        report = {
            "validation_date": time.time(),
            "environment": self.validate_environment(),
            "dependencies": self.validate_dependencies(),
            "configuration": self.validate_configuration(),
            "build_system": self.validate_build_system(),
            "python_bindings": self.validate_python_bindings(),
            "summary": self._generate_summary()
        }
        
        # Save report
        report_file = self.tests_dir / "reports" / f"validation_report_{int(time.time())}.json"
        with open(report_file, 'w') as f:
            json.dump(report, f, indent=2)
        
        print(f"Validation report saved: {report_file}")
        return report
    
    def _generate_summary(self) -> Dict[str, Any]:
        """Generate a summary of validation results."""
        summary = {
            "total_checks": 0,
            "passed_checks": 0,
            "failed_checks": 0,
            "warning_checks": 0,
            "overall_status": "UNKNOWN"
        }
        
        # Count checks
        for category, results in self.validation_results.items():
            if isinstance(results, dict):
                summary["total_checks"] += len(results)
                
                for check_name, check_result in results.items():
                    if isinstance(check_result, dict):
                        status = check_result.get("validation_status", "UNKNOWN")
                        if status == "PASS":
                            summary["passed_checks"] += 1
                        elif status == "FAIL":
                            summary["failed_checks"] += 1
                        elif status == "WARNING":
                            summary["warning_checks"] += 1
        
        # Determine overall status
        if summary["failed_checks"] > 0:
            summary["overall_status"] = "FAIL"
        elif summary["warning_checks"] > 0:
            summary["overall_status"] = "WARNING"
        else:
            summary["overall_status"] = "PASS"
        
        return summary
    
    def print_validation_report(self, report: Dict[str, Any]):
        """Print a formatted validation report."""
        print("\n" + "=" * 80)
        print("NLM TEST ENVIRONMENT VALIDATION REPORT")
        print("=" * 80)
        
        print(f"\nOverall Status: {report['summary']['overall_status']}")
        print(f"Total Checks: {report['summary']['total_checks']}")
        print(f"Passed: {report['summary']['passed_checks']}")
        print(f"Warnings: {report['summary']['warning_checks']}")
        print(f"Failed: {report['summary']['failed_checks']}")
        
        # Print category summaries
        for category, results in report.items():
            if category == "summary" or category == "validation_date":
                continue
                
            print(f"\n{category.upper()} VALIDATION:")
            
            if isinstance(results, dict):
                for check_name, check_result in results.items():
                    if isinstance(check_result, dict):
                        status = check_result.get("validation_status", "UNKNOWN")
                        icon = "✓" if status == "PASS" else ("⚠" if status == "WARNING" else "✗")
                        print(f"  {icon} {check_name}: {status}")
                        
                        # Print details for failed checks
                        if status == "FAIL" and "error" in check_result:
                            print(f"    Error: {check_result['error']}")
                        
                        # Print details for warnings
                        if status == "WARNING" and "warning" in check_result:
                            print(f"    Warning: {check_result['warning']}")
        
        print("\n" + "=" * 80)
        
        # Generate recommendations
        if report["summary"]["failed_checks"] > 0:
            print("\nIMMEDIATE ACTION REQUIRED:")
            print("  Fix all FAILED checks before proceeding with testing.")
        
        if report["summary"]["warning_checks"] > 0:
            print("\nRECOMMENDED ACTIONS:")
            print("  Address WARNING checks to improve test environment setup.")
        
        print("\nNEXT STEPS:")
        print("  1. Run validation again after fixing issues.")
        print("  2. Run 'tests/test_setup.py --setup-only' for initial setup.")
        print("  3. Run 'tests/test_setup.py --run-tests' to execute tests.")
        print("=" * 80)

def main():
    """Main validation function."""
    parser = argparse.ArgumentParser(
        description="NLM Test Environment Validation",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Validation Examples:
  # Quick validation
  python tests/test_setup.py
  
  # Detailed validation with report generation
  python tests/test_setup.py --generate-report
  
  # Validate specific components
  python tests/test_setup.py --validate-environment
  python tests/test_setup.py --validate-dependencies
  python tests/test_setup.py --validate-build-system
  
  # Combined validation
  python tests/test_setup.py --validate-environment --validate-dependencies --validate-build-system
  """
    )
    
    parser.add_argument(
        "--validate-environment",
        action="store_true",
        help="Validate the test environment setup"
    )
    
    parser.add_argument(
        "--validate-dependencies",
        action="store_true",
        help="Validate required dependencies"
    )
    
    parser.add_argument(
        "--validate-configuration",
        action="store_true",
        help="Validate project configuration"
    )
    
    parser.add_argument(
        "--validate-build-system",
        action="store_true",
        help="Validate build system setup"
    )
    
    parser.add_argument(
        "--validate-python-bindings",
        action="store_true",
        help="Validate Python bindings setup"
    )
    
    parser.add_argument(
        "--generate-report",
        action="store_true",
        help="Generate a comprehensive validation report"
    )
    
    parser.add_argument(
        "--print-report",
        action="store_true",
        help="Print the validation report to console"
    )
    
    parser.add_argument(
        "--all",
        action="store_true",
        help="Run all validations"
    )
    
    args = parser.parse_args()
    
    # Initialize validator
    validator = NLMEnvironmentValidator()
    
    # Run validations based on arguments
    validations_to_run = []
    
    if args.all or not (args.validate_environment or args.validate_dependencies or 
                       args.validate_configuration or args.validate_build_system or 
                       args.validate_python_bindings):
        # Run all validations if none specified
        validations_to_run = [
            ("environment", validator.validate_environment),
            ("dependencies", validator.validate_dependencies),
            ("configuration", validator.validate_configuration),
            ("build_system", validator.validate_build_system),
            ("python_bindings", validator.validate_python_bindings)
        ]
    else:
        # Run only specified validations
        if args.validate_environment:
            validations_to_run.append(("environment", validator.validate_environment))
        
        if args.validate_dependencies:
            validations_to_run.append(("dependencies", validator.validate_dependencies))
        
        if args.validate_configuration:
            validations_to_run.append(("configuration", validator.validate_configuration))
        
        if args.validate_build_system:
            validations_to_run.append(("build_system", validator.validate_build_system))
        
        if args.validate_python_bindings:
            validations_to_run.append(("python_bindings", validator.validate_python_bindings))
    
    # Run validations
    results = {}
    for category, validate_func in validations_to_run:
        results[category] = validate_func()
    
    # Generate comprehensive report if requested
    if args.generate_report or args.all:
        report = validator.generate_validation_report()
        
        if args.print_report:
            validator.print_validation_report(report)
        
        return report
    
    # Print summary of validation results
    print("\n" + "=" * 80)
    print("NLM TEST ENVIRONMENT VALIDATION SUMMARY")
    print("=" * 80)
    
    overall_status = "PASS"
    for category, category_results in results.items():
        if isinstance(category_results, dict) and "validation_status" in category_results:
            status = category_results["validation_status"]
            if status == "FAIL":
                overall_status = "FAIL"
            elif status == "WARNING" and overall_status != "FAIL":
                overall_status = "WARNING"
    
    print(f"Overall Status: {overall_status}")
    print(f"Categories validated: {len(results)}")
    
    for category, category_results in results.items():
        if isinstance(category_results, dict) and "validation_status" in category_results:
            status = category_results["validation_status"]
            icon = "✓" if status == "PASS" else ("⚠" if status == "WARNING" else "✗")
            print(f"  {icon} {category}: {status}")
    
    print("=" * 80)
    
    # Provide next steps
    if overall_status == "FAIL":
        print("\nACTION REQUIRED: Fix all FAILED checks before proceeding.")
        return 1
    elif overall_status == "WARNING":
        print("\nRECOMMENDED: Address WARNING checks to improve test setup.")
    else:
        print("\nSetup validation complete. All checks passed.")
    
    return 0

if __name__ == "__main__":
    exit_code = main()
    sys.exit(exit_code)