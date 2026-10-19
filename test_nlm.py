#!/usr/bin/env python3
# NLM Test and Build Verification Script
# 
# This script verifies that all NLM improvements are correctly implemented
# and that the build system works as expected.

import os
import sys
import subprocess
import json
import time
from pathlib import Path
from typing import Dict, List, Any, Tuple

class NLMTester:
    def __init__(self, build_dir: str = "build"):
        self.build_dir = build_dir
        self.project_root = Path.cwd()
        self.test_results = {
            "files_created": [],
            "tests_passed": [],
            "tests_failed": [],
            "build_status": "unknown",
            "configuration_status": "unknown",
            "feature_status": "unknown"
        }
        
    def print_header(self, title: str):
        print(f"\n{'='*60}")
        print(f"{title}")
        print(f"{'='*60}")
    
    def run_command(self, cmd: List[str], cwd: str = None) -> Tuple[bool, str, str]:
        """Run a command and return (success, stdout, stderr)"""
        try:
            result = subprocess.run(
                cmd,
                cwd=cwd,
                capture_output=True,
                text=True,
                timeout=300  # 5 minutes timeout
            )
            return (result.returncode == 0, result.stdout, result.stderr)
        except subprocess.TimeoutExpired:
            return (False, "", "Command timed out")
        except Exception as e:
            return (False, "", str(e))
    
    def check_files_exist(self, files: List[str]) -> List[str]:
        """Check if files exist and return list of missing files"""
        missing = []
        for file in files:
            if not os.path.exists(file):
                missing.append(file)
        return missing
    
    def check_source_files(self) -> Dict[str, bool]:
        """Check if all source files exist"""
        required_files = [
            "src/main.cpp",
            "src/agent/AgentBrain.hpp",
            "src/agent/AgentBrain.cpp",
            "src/agent/SensoryPercept.hpp",
            "src/agent/SensoryPercept.cpp",
            "src/brain/Brain.hpp",
            "src/brain/Brain.cpp",
            "src/core/Config/Config.hpp",
            "src/core/Config/Config.cpp",
            "src/experiments/Phase3Demo.cpp",
            "src/experiments/Phase4Demo.cpp",
            "src/experiments/Phase5Demo.cpp",
            "src/experiments/Phase5Experiment.hpp",
            "src/experiments/Phase5Experiment.cpp",
            "src/experiments/Phase6Demo.cpp",
            "src/world/SimpleWorld.hpp",
            "src/world/SimpleWorld.cpp",
            "src/config/NLMBuildConfig.hpp"
        ]
        
        # Add config_examples.py
        required_files.append("examples/config_examples.py")
        
        missing_files = self.check_files_exist(required_files)
        
        status = {}
        for file in required_files:
            status[file] = os.path.exists(file)
            
        return status
    
    def check_build_files(self) -> Dict[str, bool]:
        """Check if build files exist"""
        build_files = [
            f"{self.build_dir}/nlm",
            f"{self.build_dir}/nlm_phase3_demo",
            f"{self.build_dir}/nlm_phase4_demo",
            f"{self.build_dir}/nlm_phase5_demo",
            f"{self.build_dir}/nlm_phase6_demo",
            f"{self.build_dir}/CMakeCache.txt",
            f"{self.build_dir}/Makefile"
        ]
        
        missing_files = self.check_files_exist(build_files)
        
        status = {}
        for file in build_files:
            status[file] = os.path.exists(file)
            
        return status
    
    def check_advanced_features(self) -> Dict[str, bool]:
        """Check if advanced features are properly implemented"""
        features = {}
        
        # Check AgentBrainConfig.hpp
        agent_brain_config_path = "src/agent/AgentBrainConfig.hpp"
        features["AgentBrainConfig.hpp"] = os.path.exists(agent_brain_config_path)
        
        if os.path.exists(agent_brain_config_path):
            with open(agent_brain_config_path, 'r') as f:
                content = f.read()
                features["AgentBrainConfig_has_validation"] = "validate()" in content
                features["AgentBrainConfig_has_toString"] = "toString()" in content
                features["AgentBrainConfig_has_loadFromConfig"] = "loadFromConfig" in content
                features["AgentBrainConfig_has_advanced_config"] = "advanced" in content.lower()
        
        # Check AdvancedFeatures.hpp
        advanced_features_path = "src/agent/AdvancedFeatures.hpp"
        features["AdvancedFeatures.hpp"] = os.path.exists(advanced_features_path)
        
        if os.path.exists(advanced_features_path):
            with open(advanced_features_path, 'r') as f:
                content = f.read()
                features["AdvancedFeatures_has_struct"] = "struct AdvancedFeatures" in content
                features["AdvancedFeatures_has_config_manager"] = "AdvancedConfigManager" in content
                features["AdvancedFeatures_has_utils"] = "namespace AdvancedUtils" in content
        
        # Check config_examples.py
        config_examples_path = "examples/config_examples.py"
        features["config_examples.py"] = os.path.exists(config_examples_path)
        
        if os.path.exists(config_examples_path):
            with open(config_examples_path, 'r') as f:
                content = f.read()
                features["config_examples_has_configs"] = "MINIMAL_CONFIG" in content
                features["config_examples_has_examples"] = "COMMAND_LINE_EXAMPLES" in content
                features["config_examples_has_validation"] = "VALIDATION_RULES" in content
        
        return features
    
    def check_documentation(self) -> Dict[str, bool]:
        """Check if documentation is present"""
        docs = {}
        
        # Check for documentation files
        docs["README.md"] = os.path.exists("README.md")
        docs["HOW_TO_USE.md"] = os.path.exists("HOW_TO_USE.md")
        docs["easy_usage.md"] = os.path.exists("easy_usage.md")
        
        # Check for examples directory
        docs["examples_directory"] = os.path.exists("examples")
        
        if os.path.exists("examples"):
            examples_files = os.listdir("examples")
            docs["examples_config"] = "config_examples.py" in examples_files
        
        return docs
    
    def check_implementation_quality(self) -> Dict[str, bool]:
        """Check implementation quality metrics"""
        quality = {}
        
        # Check for proper headers
        quality["main_cpp_exists"] = os.path.exists("src/main.cpp")
        quality["agentbrain_cpp_exists"] = os.path.exists("src/agent/AgentBrain.cpp")
        quality["phase5_demo_exists"] = os.path.exists("src/experiments/Phase5Demo.cpp")
        
        if os.path.exists("src/main.cpp"):
            with open("src/main.cpp", 'r') as f:
                content = f.read()
                quality["main_cpp_has_cli"] = "parseArguments" in content
                quality["main_cpp_has_modes"] = "mode" in content.lower()
                quality["main_cpp_has_help"] = "help" in content.lower()
        
        if os.path.exists("src/agent/AgentBrain.cpp"):
            with open("src/agent/AgentBrain.cpp", 'r') as f:
                content = f.read()
                quality["agentbrain_cpp_has_config"] = "AgentBrainConfig" in content
                quality["agentbrain_cpp_has_advanced"] = "advanced" in content.lower()
                quality["agentbrain_cpp_has_validation"] = "validate" in content
        
        if os.path.exists("src/experiments/Phase5Demo.cpp"):
            with open("src/experiments/Phase5Demo.cpp", 'r') as f:
                content = f.read()
                quality["phase5_demo_has_performance"] = "performance" in content.lower()
                quality["phase5_demo_has_metrics"] = "metrics" in content.lower()
                quality["phase5_demo_has_optimization"] = "optimization" in content.lower()
        
        return quality
    
    def test_build(self) -> bool:
        """Test the build process"""
        self.print_header("Testing Build Process")
        
        # Check if build directory exists
        if not os.path.exists(self.build_dir):
            print(f"❌ Build directory '{self.build_dir}' does not exist")
            return False
        
        # Check for CMakeCache.txt
        cmake_cache = f"{self.build_dir}/CMakeCache.txt"
        if not os.path.exists(cmake_cache):
            print("❌ CMakeCache.txt not found - CMake was not configured")
            return False
        
        # Check for Makefile
        makefile = f"{self.build_dir}/Makefile"
        if not os.path.exists(makefile):
            print("❌ Makefile not found")
            return False
        
        # Try to run make for a simple target
        success, stdout, stderr = self.run_command(["make", "nlm", "-C", self.build_dir], self.project_root)
        
        if success:
            print("✅ Build successful")
            self.test_results["build_status"] = "success"
            return True
        else:
            print(f"❌ Build failed: {stderr}")
            self.test_results["build_status"] = "failed"
            return False
    
    def test_executable_functionality(self) -> bool:
        """Test that executables work correctly"""
        self.print_header("Testing Executable Functionality")
        
        executables = [
            ("nlm", ["--help"]),
            ("nlm_phase3_demo", ["5", "100"]),
            ("nlm_phase4_demo", ["10"]),
            ("nlm_phase5_demo", ["5", "50", "true"]),
            ("nlm_phase6_demo", [])
        ]
        
        all_passed = True
        
        for exe_name, args in executables:
            exe_path = f"{self.build_dir}/{exe_name}"
            if not os.path.exists(exe_path):
                print(f"❌ Executable '{exe_name}' not found")
                all_passed = False
                continue
            
            # Make executable if it exists
            os.chmod(exe_path, 0o755)
            
            # Run the executable
            cmd = [exe_path] + args
            success, stdout, stderr = self.run_command(cmd)
            
            if success:
                print(f"✅ {exe_name} works correctly")
                self.test_results["tests_passed"].append(exe_name)
            else:
                print(f"❌ {exe_name} failed: {stderr}")
                self.test_results["tests_failed"].append(exe_name)
                all_passed = False
        
        return all_passed
    
    def test_configuration_examples(self) -> bool:
        """Test configuration examples"""
        self.print_header("Testing Configuration Examples")
        
        config_examples_path = "examples/config_examples.py"
        
        if not os.path.exists(config_examples_path):
            print("❌ config_examples.py not found")
            return False
        
        # Run the config examples script
        success, stdout, stderr = self.run_command([sys.executable, config_examples_path, "--all"])
        
        if success:
            print("✅ Configuration examples work correctly")
            return True
        else:
            print(f"❌ Configuration examples failed: {stderr}")
            return False
    
    def generate_test_report(self) -> str:
        """Generate a comprehensive test report"""
        report = "NLM Test and Build Verification Report\n"
        report += "=" * 60 + "\n"
        report += f"Generated: {time.ctime()}\n"
        report += f"Build directory: {self.build_dir}\n"
        report += f"Project root: {self.project_root}\n\n"
        
        # Source files status
        report += "Source Files Status:\n"
        source_status = self.check_source_files()
        for file, exists in source_status.items():
            status = "✅" if exists else "❌"
            report += f"  {status} {file}\n"
        
        report += f"\nSource Files: {sum(source_status.values())}/{len(source_status)} present\n\n"
        
        # Build files status
        report += "Build Files Status:\n"
        build_status = self.check_build_files()
        for file, exists in build_status.items():
            status = "✅" if exists else "❌"
            report += f"  {status} {file}\n"
        
        report += f"\nBuild Files: {sum(build_status.values())}/{len(build_status)} present\n\n"
        
        # Advanced features status
        report += "Advanced Features Status:\n"
        features_status = self.check_advanced_features()
        for feature, exists in features_status.items():
            status = "✅" if exists else "❌"
            report += f"  {status} {feature}\n"
        
        report += f"\nAdvanced Features: {sum(features_status.values())}/{len(features_status)} present\n\n"
        
        # Documentation status
        report += "Documentation Status:\n"
        docs_status = self.check_documentation()
        for doc, exists in docs_status.items():
            status = "✅" if exists else "❌"
            report += f"  {status} {doc}\n"
        
        report += f"\nDocumentation: {sum(docs_status.values())}/{len(docs_status)} present\n\n"
        
        # Implementation quality
        report += "Implementation Quality:\n"
        quality_status = self.check_implementation_quality()
        for aspect, good in quality_status.items():
            status = "✅" if good else "❌"
            report += f"  {status} {aspect}\n"
        
        report += f"\nImplementation Quality: {sum(quality_status.values())}/{len(quality_status)} good\n\n"
        
        # Test results
        report += "Test Results:\n"
        report += f"  Build Status: {self.test_results['build_status']}\n"
        report += f"  Tests Passed: {len(self.test_results['tests_passed'])}\n"
        report += f"  Tests Failed: {len(self.test_results['tests_failed'])}\n"
        
        if self.test_results['tests_failed']:
            report += "  Failed Tests:\n"
            for test in self.test_results['tests_failed']:
                report += f"    - {test}\n"
        
        report += "\n" + "=" * 60 + "\n"
        report += "Summary:\n"
        
        # Calculate overall score
        total_checks = len(source_status) + len(build_status) + len(features_status) + len(docs_status) + len(quality_status)
        passed_checks = (sum(source_status.values()) + sum(build_status.values()) + 
                        sum(features_status.values()) + sum(docs_status.values()) + 
                        sum(quality_status.values()))
        
        score = (passed_checks / total_checks) * 100 if total_checks > 0 else 0
        
        report += f"Overall Score: {score:.1f}%\n"
        
        if score >= 90:
            report += "Status: ✅ EXCELLENT - All improvements successfully implemented\n"
        elif score >= 80:
            report += "Status: ✅ GOOD - Most improvements successfully implemented\n"
        elif score >= 70:
            report += "Status: ⚠️  FAIR - Some improvements need attention\n"
        else:
            report += "Status: ❌ POOR - Major improvements missing\n"
        
        return report
    
    def run_all_tests(self) -> None:
        """Run all tests and generate report"""
        print("Starting NLM Test and Build Verification")
        print("=" * 60)
        
        # Check source files
        source_status = self.check_source_files()
        self.test_results["files_created"] = list(source_status.keys())
        
        # Check advanced features
        features_status = self.check_advanced_features()
        
        # Check documentation
        docs_status = self.check_documentation()
        
        # Check implementation quality
        quality_status = self.check_implementation_quality()
        
        # Try to build if build directory exists
        if os.path.exists(self.build_dir):
            build_success = self.test_build()
        else:
            print(f"⚠️  Build directory '{self.build_dir}' not found - skipping build tests")
            build_success = False
            self.test_results["build_status"] = "skipped"
        
        # Test executable functionality
        exe_success = self.test_executable_functionality()
        
        # Test configuration examples
        config_success = self.test_configuration_examples()
        
        # Generate report
        report = self.generate_test_report()
        
        # Save report
        report_path = f"{self.project_root}/test_report.txt"
        with open(report_path, 'w') as f:
            f.write(report)
        
        # Print report
        print(report)
        
        # Return exit code based on overall success
        if build_success and exe_success and config_success:
            print(f"\n✅ All tests passed! Report saved to: {report_path}")
            sys.exit(0)
        else:
            print(f"\n❌ Some tests failed. Report saved to: {report_path}")
            sys.exit(1)

def main():
    import argparse
    
    parser = argparse.ArgumentParser(description="NLM Test and Build Verification")
    parser.add_argument("--build-dir", default="build", help="Build directory")
    parser.add_argument("--skip-build", action="store_true", help="Skip build tests")
    parser.add_argument("--skip-executable", action="store_true", help="Skip executable tests")
    parser.add_argument("--skip-config", action="store_true", help="Skip configuration tests")
    
    args = parser.parse_args()
    
    tester = NLMTester(args.build_dir)
    
    # Run tests
    tester.run_all_tests()

if __name__ == "__main__":
    main()
