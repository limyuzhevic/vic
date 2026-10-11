#!/usr/bin/env python3
# NLM Build Automation Script
# This script provides comprehensive build automation for NLM project
# Supports multiple build configurations, package managers, and installation

import argparse
import json
import os
import platform
import shutil
import subprocess
import sys
import time
from datetime import datetime
from pathlib import Path

class NLMBuildSystem:
    def __init__(self):
        self.project_root = Path(__file__).parent.absolute()
        self.build_dir = self.project_root / "build"
        self.config_file = self.project_root / "build_config.json"
        self.start_time = datetime.now()
        
        # System information
        self.system = platform.system()
        self.machine = platform.machine()
        
        # Default configuration
        self.config = {
            "generators": ["Ninja", "Unix Makefiles"],
            "build_types": ["Debug", "Release", "RelWithDebInfo", "MinSizeRel"],
            "default_build_type": "Release",
            "default_generator": "Ninja" if shutil.which("ninja") else "Unix Makefiles",
            "package_managers": {
                "conan": False,
                "vcpkg": False,
                "cpack": False
            },
            "install_prefix": "/usr/local",
            "python_bindings": True,
            "tests": True,
            "cli": True,
            "demos": True,
            "components": "full"
        }
        
        # Load existing configuration
        if self.config_file.exists():
            with open(self.config_file, 'r') as f:
                self.config.update(json.load(f))
    
    def load_config(self, args):
        # Override config with command line arguments
        if args.build_type:
            self.config["default_build_type"] = args.build_type
        
        if args.generator:
            self.config["default_generator"] = args.generator
        
        if args.generator_list:
            print("Available generators:")
            for gen in self.config["generators"]:
                print(f"  - {gen}")
            return
        
        if args.config:
            with open(args.config, 'r') as f:
                self.config.update(json.load(f))
        
        # Package manager options
        if args.conan:
            self.config["package_managers"]["conan"] = True
        if args.vcpkg:
            self.config["package_managers"]["vcpkg"] = True
        if args.cpack:
            self.config["package_managers"]["cpack"] = True
        
        # Component options
        if args.minimal:
            self.config["components"] = "minimal"
        if args.dev:
            self.config["components"] = "dev"
        
        # Feature options
        if args.no_tests:
            self.config["tests"] = False
        if args.no_python:
            self.config["python_bindings"] = False
        if args.no_cli:
            self.config["cli"] = False
        if args.no_demos:
            self.config["demos"] = False
        
        # Install options
        if args.prefix:
            self.config["install_prefix"] = args.prefix
    
    def print_header(self, message):
        print(f"\n{'=' * 60}")
        print(f"NLM Build System: {message}")
        print(f"{'=' * 60}")
        print(f"Project: {self.project_root}")
        print(f"System: {self.system} ({self.machine})")
        print(f"Build type: {self.config['default_build_type']}")
        print(f"Generator: {self.config['default_generator']}")
        print(f"Components: {self.config['components']}")
        print(f"{'=' * 60}\n")
    
    def check_dependencies(self):
        print("Checking dependencies...")
        
        required_tools = ["cmake"]
        optional_tools = ["ninja", "make", "python3", "git"]
        
        # Check required tools
        for tool in required_tools:
            if not shutil.which(tool):
                print(f"ERROR: Required tool '{tool}' not found in PATH")
                return False
        
        # Check for recommended tools
        cmake_version = self.run_command(["cmake", "--version"])
        if cmake_version:
            print(f"Found CMake: {cmake_version.splitlines()[0]}")
        
        # Check package managers if enabled
        if self.config["package_managers"]["conan"] and not shutil.which("conan"):
            print("WARNING: Conan not found, will try to install")
        
        if self.config["package_managers"]["vcpkg"] and not shutil.which("vcpkg"):
            print("WARNING: vcpkg not found, will try to install")
        
        return True
    
    def configure_cmake(self, build_dir, build_type, generator):
        print(f"Configuring CMake in {build_dir} with {generator}...")
        
        cmake_args = [
            "cmake",
            "-S", str(self.project_root),
            "-B", str(build_dir),
            f"-G", generator,
            f"-DCMAKE_BUILD_TYPE={build_type}",
            f"-DNLM_BUILD_TYPE={build_type}",
            f"-DNLM_BUILD_TESTS={'ON' if self.config['tests'] else 'OFF'}",
            f"-DNLM_BUILD_PYTHON_BINDINGS={'ON' if self.config['python_bindings'] else 'OFF'}",
            f"-DNLM_BUILD_CLI={'ON' if self.config['cli'] else 'OFF'}",
            f"-DNLM_BUILD_DEMOS={'ON' if self.config['demos'] else 'OFF'}",
        ]
        
        # Package manager options
        if self.config["package_managers"]["conan"]:
            cmake_args.extend(["-DNLM_ENABLE_CONAN=ON", "-DCMAKE_TOOLCHAIN_FILE=/path/to/conan_toolchain.cmake"])
        
        if self.config["package_managers"]["vcpkg"]:
            if not os.environ.get("VCPKG_ROOT"):
                print("WARNING: VCPKG_ROOT not set, using default vcpkg installation")
                vcpkg_path = Path.home() / "vcpkg"
                if vcpkg_path.exists():
                    cmake_args.extend(["-DVCPKG_ROOT=" + str(vcpkg_path), "-DCMAKE_TOOLCHAIN_FILE=" + str(vcpkg_path / "scripts/buildsystems/vcpkg.cmake")])
        
        # Component selection
        if self.config["components"] == "minimal":
            cmake_args.append("-DNLM_BUILD_MINIMAL=ON")
        elif self.config["components"] == "dev":
            cmake_args.append("-DNLM_BUILD_DEV=ON")
        elif self.config["components"] == "full":
            cmake_args.append("-DNLM_BUILD_FULL=ON")
        
        # Run CMake configuration
        return self.run_command(cmake_args)
    
    def build_project(self, build_dir):
        print(f"Building project in {build_dir}...")
        
        # Build with parallel jobs
        build_args = ["cmake", "--build", str(build_dir), "--", "-j", str(os.cpu_count())]
        
        return self.run_command(build_args)
    
    def install_project(self, build_dir, install_dir=None):
        print(f"Installing project from {build_dir}...")
        
        install_args = ["cmake", "--install", str(build_dir)]
        
        if install_dir:
            install_args.extend(["--prefix", install_dir])
        else:
            install_args.extend(["--prefix", self.config["install_prefix"]])
        
        # Run installation
        return self.run_command(install_args)
    
    def run_tests(self, build_dir):
        print(f"Running tests from {build_dir}...")
        
        test_args = ["ctest", "--test-dir", str(build_dir), "--output-on-failure"]
        
        return self.run_command(test_args)
    
    def create_package(self, build_dir):
        print(f"Creating package from {build_dir}...")
        
        # First build
        build_result = self.build_project(build_dir)
        if not build_result:
            return False
        
        # Then package
        package_args = ["cpack", "--config", str(build_dir / "CPackConfig.cmake")]
        
        return self.run_command(package_args)
    
    def run_command(self, command, cwd=None):
        try:
            print(f"Running: {' '.join(command)}")
            
            result = subprocess.run(
                command,
                cwd=cwd,
                capture_output=True,
                text=True,
                check=True
            )
            
            if result.stdout:
                print(result.stdout)
            
            return True
            
        except subprocess.CalledProcessError as e:
            print(f"ERROR: Command failed with exit code {e.returncode}")
            if e.stdout:
                print(f"STDOUT:\n{e.stdout}")
            if e.stderr:
                print(f"STDERR:\n{e.stderr}")
            return False
    
    def save_config(self):
        with open(self.config_file, 'w') as f:
            json.dump(self.config, f, indent=2)
    
    def print_summary(self, success, build_dir=None):
        end_time = datetime.now()
        duration = (end_time - self.start_time).total_seconds()
        
        print(f"\n{'=' * 60}")
        print(f"Build Summary")
        print(f"{'=' * 60}")
        print(f"Status: {'SUCCESS' if success else 'FAILED'}")
        print(f"Duration: {duration:.2f} seconds")
        print(f"Build directory: {build_dir}")
        print(f"{'=' * 60}\n")
        
        if build_dir and (build_dir / "build_time.log").exists():
            with open(build_dir / "build_time.log", 'r') as f:
                print(f"Build time log:\n{f.read()}")
        
        return success
    
    def run_build(self, args):
        # Load configuration
        self.load_config(args)
        
        # Print configuration
        self.print_header("Build Configuration")
        
        # Check dependencies
        if not self.check_dependencies():
            return False
        
        # Create build directory
        build_dir = self.build_dir / f"{self.config['default_build_type'].lower()}_{self.config['default_generator'].lower()}"
        if args.build_dir:
            build_dir = Path(args.build_dir)
        
        build_dir.mkdir(parents=True, exist_ok=True)
        
        # Configure
        cmake_result = self.configure_cmake(
            build_dir,
            self.config["default_build_type"],
            self.config["default_generator"]
        )
        if not cmake_result:
            return self.print_summary(False, build_dir)
        
        # Build
        build_result = self.build_project(build_dir)
        if not build_result:
            return self.print_summary(False, build_dir)
        
        # Run tests if enabled
        if self.config["tests"]:
            test_result = self.run_tests(build_dir)
            if not test_result:
                print("WARNING: Tests failed")
        
        # Install if requested
        if args.install:
            install_result = self.install_project(build_dir, args.prefix)
            if not install_result:
                return self.print_summary(False, build_dir)
        
        # Create package if requested
        if self.config["package_managers"]["cpack"]:
            package_result = self.create_package(build_dir)
            if not package_result:
                return self.print_summary(False, build_dir)
        
        # Save configuration
        self.save_config()
        
        return self.print_summary(True, build_dir)

def main():
    parser = argparse.ArgumentParser(
        description="NLM Build System - Comprehensive build automation for Neural Learning Machine"
    )
    
    # Basic build options
    parser.add_argument("--build-type", choices=["Debug", "Release", "RelWithDebInfo", "MinSizeRel"],
                       help="Build type (default: Release)")
    parser.add_argument("--generator", help="CMake generator")
    parser.add_argument("--generator-list", action="store_true",
                       help="List available generators and exit")
    parser.add_argument("--config", help="Load configuration from JSON file")
    
    # Build directory
    parser.add_argument("--build-dir", help="Build directory (default: build/{type}_{generator})")
    
    # Component options
    parser.add_argument("--minimal", action="store_true",
                       help="Build minimal version (core only)")
    parser.add_argument("--dev", action="store_true",
                       help="Build development version with all features")
    parser.add_argument("--full", action="store_true",
                       help="Build full version with all components")
    
    # Feature options
    parser.add_argument("--no-tests", action="store_true",
                       help="Disable building tests")
    parser.add_argument("--no-python", action="store_true",
                       help="Disable building Python bindings")
    parser.add_argument("--no-cli", action="store_true",
                       help="Disable building advanced CLI")
    parser.add_argument("--no-demos", action="store_true",
                       help="Disable building demo executables")
    
    # Package manager options
    parser.add_argument("--conan", action="store_true",
                       help="Enable Conan package manager")
    parser.add_argument("--vcpkg", action="store_true",
                       help="Enable vcpkg package manager")
    parser.add_argument("--cpack", action="store_true",
                       help="Enable CPack for packaging")
    
    # Installation options
    parser.add_argument("--install", action="store_true",
                       help="Install after building")
    parser.add_argument("--prefix", help="Installation prefix")
    
    # Action options
    parser.add_argument("--clean", action="store_true",
                       help="Clean build directory")
    parser.add_argument("--test-only", action="store_true",
                       help="Only run tests")
    
    args = parser.parse_args()
    
    # Create build system instance
    builder = NLMBuildSystem()
    
    if args.clean:
        print("Cleaning build directory...")
        if builder.build_dir.exists():
            shutil.rmtree(builder.build_dir)
        print("Build directory cleaned")
        return
    
    if args.test_only:
        # Just run tests if configured
        print("Running tests only...")
        # This would require a configured build directory
        print("ERROR: --test-only requires a configured build directory")
        return
    
    # Run the build
    return builder.run_build(args)

if __name__ == "__main__":
    sys.exit(0 if main() else 1)
