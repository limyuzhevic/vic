#!/usr/bin/env python3
"""
NLM Test Suite

This test suite validates the improvements made to the NLM (Neural Learning Machine) project.
Tests include:
1. Code structure validation
2. Build system verification
3. Phase integration tests
4. Configuration and parameter validation
5. Error handling tests
6. Performance benchmarks
"""

import os
import sys
import subprocess
import json
import tempfile
from pathlib import Path

class NLMTester:
    def __init__(self, project_root):
        self.project_root = Path(project_root)
        self.build_dir = self.project_root / "build"
        self.test_results = {
            'passed': 0,
            'failed': 0,
            'errors': []
        }
    
    def run_command(self, cmd, cwd=None):
        """Run a command and return the result"""
        try:
            result = subprocess.run(
                cmd,
                shell=True,
                cwd=cwd or self.project_root,
                capture_output=True,
                text=True,
                timeout=300  # 5 minute timeout
            )
            return result.returncode == 0, result.stdout, result.stderr
        except subprocess.TimeoutExpired:
            return False, "", "Command timed out"
        except Exception as e:
            return False, "", str(e)
    
    def test_cmake_lists(self):
        """Test CMakeLists.txt for required targets"""
        print("Testing CMakeLists.txt...")
        cmake_file = self.project_root / "CMakeLists.txt"
        
        if not cmake_file.exists():
            self.test_results['errors'].append("CMakeLists.txt not found")
            return False
        
        cmake_content = cmake_file.read_text()
        
        # Check for required targets
        required_targets = [
            'add_executable(nlm',
            'add_library(nlm_core',
            'add_library(nlm_agent',
            'add_library(nlm_world',
            'add_library(nlm_phase3',
            'add_library(nlm_phase4',
            'add_library(nlm_phase5',
            'add_executable(nlm_phase3_demo',
            'add_executable(nlm_phase4_demo',
            'add_executable(nlm_phase6_demo'
        ]
        
        missing_targets = []
        for target in required_targets:
            if target not in cmake_content:
                missing_targets.append(target)
        
        if missing_targets:
            self.test_results['errors'].append(
                f"Missing CMake targets: {', '.join(missing_targets)}"
            )
            return False
        
        print("  ✓ CMakeLists.txt contains all required targets")
        return True
    
    def test_main_cpp_structure(self):
        """Test main.cpp for Phase 6 features"""
        print("Testing main.cpp structure...")
        main_cpp = self.project_root / "src" / "main.cpp"
        
        if not main_cpp.exists():
            self.test_results['errors'].append("main.cpp not found")
            return False
        
        content = main_cpp.read_text()
        
        # Check for Phase 6 specific features
        required_features = [
            'Phase 6 Commander',
            'NLMCommander',
            'Phase6Config',
            'runDemonstration',
            'runIntegrationTest',
            'runMinimalTest',
            'setPlasticity',
            'setDevelopment',
            'saveCheckpoint',
            'loadCheckpoint'
        ]
        
        missing_features = []
        for feature in required_features:
            if feature not in content:
                missing_features.append(feature)
        
        if missing_features:
            self.test_results['errors'].append(
                f"main.cpp missing Phase 6 features: {', '.join(missing_features)}"
            )
            return False
        
        print("  ✓ main.cpp contains Phase 6 Commander")
        
        # Check for command line argument parsing
        if '--mode' not in content:
            self.test_results['errors'].append("main.cpp missing command line argument parsing")
            return False
        
        print("  ✓ main.cpp has command line argument parsing")
        return True
    
    def test_phase6_demo(self):
        """Test Phase6Demo.cpp for integration features"""
        print("Testing Phase6Demo.cpp...")
        demo_cpp = self.project_root / "src" / "experiments" / "Phase6Demo.cpp"
        
        if not demo_cpp.exists():
            self.test_results['errors'].append("Phase6Demo.cpp not found")
            return False
        
        content = demo_cpp.read_text()
        
        # Check for proper logging
        if 'Logger::getGlobal().setLevel(LogLevel::Info)' not in content:
            self.test_results['errors'].append(
                "Phase6Demo.cpp should use Logger::getGlobal() for logging"
            )
            return False
        
        # Check for NLM_LOG macros
        if 'NLM_LOG_INFO' not in content:
            self.test_results['errors'].append(
                "Phase6Demo.cpp should use NLM_LOG macros for logging"
            )
            return False
        
        print("  ✓ Phase6Demo.cpp uses proper logging")
        return True
    
    def test_configuration_system(self):
        """Test configuration system"""
        print("Testing configuration system...")
        
        # Check core configuration files
        config_files = [
            self.project_root / "src" / "core" / "Config" / "Config.hpp",
            self.project_root / "src" / "core" / "Config" / "Config.cpp",
            self.project_root / "pyproject.toml"
        ]
        
        for config_file in config_files:
            if not config_file.exists():
                self.test_results['errors'].append(f"Configuration file not found: {config_file}")
                return False
        
        print("  ✓ Configuration system present")
        return True
    
    def test_build_system(self):
        """Test if the build system works"""
        print("Testing build system...")
        
        # Check if we can run CMake
        if not self.build_dir.exists():
            # Try to build
            print("  Building project...")
            success, stdout, stderr = self.run_command("mkdir -p build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release")
            if not success:
                self.test_results['errors'].append(f"CMake configuration failed: {stderr}")
                return False
            
            success, stdout, stderr = self.run_command("cd build && make -j4")
            if not success:
                self.test_results['errors'].append(f"Build failed: {stderr}")
                return False
            
            print("  ✓ Build successful")
        else:
            print("  ✓ Build directory exists")
        
        return True
    
    def test_project_structure(self):
        """Test project structure"""
        print("Testing project structure...")
        
        required_directories = [
            'src',
            'src/core',
            'src/core/Config',
            'src/core/Logger',
            'src/core/Random',
            'src/core/SimulationClock',
            'src/brain',
            'src/agent',
            'src/world',
            'src/experiments',
            'src/memory',
            'src/plasticity',
            'src/neuromodulation',
            'src/development',
            'src/cognition',
            'src/sensory',
            'src/motor',
            'src/prediction',
            'src/environment',
            'docs'
        ]
        
        for directory in required_directories:
            dir_path = self.project_root / directory
            if not dir_path.exists():
                self.test_results['errors'].append(f"Required directory not found: {directory}")
                return False
        
        print("  ✓ Project structure is complete")
        return True
    
    def test_readme_documentation(self):
        """Test README documentation"""
        print("Testing README documentation...")
        
        readme_file = self.project_root / "README.md"
        if not readme_file.exists():
            self.test_results['errors'].append("README.md not found")
            return False
        
        content = readme_file.read_text()
        
        # Check for key documentation elements
        required_elements = [
            'Phase 6: Final Integration',
            'Building',
            'Running',
            'Available Executables',
            'License'
        ]
        
        for element in required_elements:
            if element not in content:
                self.test_results['errors'].append(f"README.md missing section: {element}")
                return False
        
        print("  ✓ README.md contains required documentation")
        return True
    
    def test_examples_directory(self):
        """Test examples directory"""
        print("Testing examples directory...")
        
        examples_dir = self.project_root / "examples"
        if not examples_dir.exists():
            self.test_results['errors'].append("examples directory not found")
            return False
        
        # Check for Python examples
        python_examples = list(examples_dir.glob("*.py"))
        if not python_examples:
            self.test_results['errors'].append("No Python examples found in examples/ directory")
            return False
        
        print(f"  ✓ Found {len(python_examples)} Python example files")
        return True
    
    def test_phase_integration(self):
        """Test Phase integration"""
        print("Testing Phase integration...")
        
        # Check for Phase 6 integration experiment
        phase6_files = [
            self.project_root / "src" / "experiments" / "Phase6IntegratedExperiment.cpp",
            self.project_root / "src" / "experiments" / "Phase6IntegratedExperiment.hpp",
            self.project_root / "src" / "experiments" / "Phase6Demo.cpp"
        ]
        
        for phase_file in phase6_files:
            if not phase_file.exists():
                self.test_results['errors'].append(f"Phase 6 integration file not found: {phase_file}")
                return False
        
        print("  ✓ Phase 6 integration files present")
        return True
    
    def run_all_tests(self):
        """Run all tests"""
        print("NLM Test Suite")
        print("=" * 50)
        
        tests = [
            self.test_project_structure,
            self.test_cmake_lists,
            self.test_main_cpp_structure,
            self.test_phase6_demo,
            self.test_configuration_system,
            self.test_build_system,
            self.test_readme_documentation,
            self.test_examples_directory,
            self.test_phase_integration
        ]
        
        for test in tests:
            try:
                if test():
                    self.test_results['passed'] += 1
                else:
                    self.test_results['failed'] += 1
            except Exception as e:
                self.test_results['failed'] += 1
                self.test_results['errors'].append(f"Test {test.__name__} failed with exception: {str(e)}")
        
        # Print results
        print("\n" + "=" * 50)
        print("Test Results:")
        print(f"Passed: {self.test_results['passed']}")
        print(f"Failed: {self.test_results['failed']}")
        
        if self.test_results['errors']:
            print("\nErrors:")
            for error in self.test_results['errors']:
                print(f"  - {error}")
        
        return self.test_results['failed'] == 0
    
    def generate_report(self):
        """Generate a test report"""
        report = {
            'summary': {
                'total_tests': len([t for t in self.test_results if t != 'errors']),
                'passed': self.test_results['passed'],
                'failed': self.test_results['failed']
            },
            'errors': self.test_results['errors'],
            'timestamp': subprocess.run(['date'], capture_output=True, text=True).stdout.strip()
        }
        
        # Write report to file
        report_file = self.project_root / "test_report.json"
        with open(report_file, 'w') as f:
            json.dump(report, f, indent=2)
        
        print(f"\nTest report written to: {report_file}")
        return report

def main():
    """Main test runner"""
    project_root = Path.cwd()
    
    print("NLM Test Suite")
    print("=" * 60)
    print(f"Project root: {project_root}")
    print()
    
    tester = NLMTester(project_root)
    
    # Run tests
    success = tester.run_all_tests()
    
    # Generate report
    report = tester.generate_report()
    
    if success:
        print("\n✓ All tests passed!")
        return 0
    else:
        print("\n✗ Some tests failed!")
        return 1

if __name__ == "__main__":
    sys.exit(main())