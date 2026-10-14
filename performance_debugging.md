# NLM (Neural Learning Machine) - Build System and Developer Experience

**Modernized build system, development workflow, and developer experience for NLM Phase 6**

## Overview

This module provides comprehensive build system improvements, development workflow automation, and enhanced developer experience for the NLM neural simulation framework. It includes:

- Modern build system with CMake improvements
- Development environment setup and automation
- Continuous integration and deployment pipelines
- Testing frameworks and automation
- Development tools and IDE integration
- Documentation generation and maintenance
- Performance optimization and monitoring
- Developer productivity tools

## Key Components

### 1. Modern Build System

```python
import os
import sys
import subprocess
import time
import platform
from pathlib import Path
from typing import Dict, List, Optional, Callable
from dataclasses import dataclass, field
from enum import Enum

class BuildConfiguration(Enum):
    RELEASE = "release"
    DEBUG = "debug"
    RELWITHDEBINFO = "relwithdebinfo"
    MINSIZEREL = "minsizerel"

class BuildSystem:
    """Modern build system for NLM with comprehensive configuration management."""
    
    def __init__(self, project_root: str = "."):
        self.project_root = Path(project_root).resolve()
        self.config_file = self.project_root / "build_config.json"
        self.cmake_cache = self.project_root / "build" / "CMakeCache.txt"
        self.build_directory = self.project_root / "build"
        self.install_directory = self.project_root / "install"
        
        # Default configuration
        self.config = {
            'build_type': BuildConfiguration.RELEASE,
            'generator': self._detect_cmake_generator(),
            'parallel_build': True,
            'enable_tests': True,
            'enable_benchmarks': True,
            'enable_profiling': False,
            'optimization_level': 'O3',
            'enable_sanitizers': False,
            'enable_address_sanitizer': False,
            'enable_undefined_behavior_sanitizer': False,
            'python_bindings': True,
            'documentation': True,
            'vcpkg_enabled': False,
            'package_manager': 'cmake',
            'compiler_flags': [],
            'link_flags': [],
            'definitions': [],
            'include_paths': [],
            'library_paths': [],
            'install_prefix': str(self.install_directory)
        }
    
    def _detect_cmake_generator(self) -> str:
        """Detect the best CMake generator for the current platform."""
        system = platform.system()
        
        if system == "Windows":
            return "Visual Studio 17 2022"
        elif system == "Linux":
            return "Unix Makefiles"
        elif system == "Darwin":
            return "Unix Makefiles"
        else:
            return "Unix Makefiles"
    
    def load_config(self, config_file: Optional[str] = None) -> Dict:
        """Load build configuration from file."""
        if config_file:
            config_path = Path(config_file)
        else:
            config_path = self.config_file
        
        if config_path.exists():
            with open(config_path, 'r') as f:
                file_config = json.load(f)
            
            # Merge with default config
            self.config.update(file_config)
            return self.config
        
        return self.config
    
    def save_config(self, config_file: Optional[str] = None):
        """Save current configuration to file."""
        if config_file:
            config_path = Path(config_file)
        else:
            config_path = self.config_file
        
        # Create directory if it doesn't exist
        config_path.parent.mkdir(parents=True, exist_ok=True)
        
        # Save configuration
        with open(config_path, 'w') as f:
            json.dump(self.config, f, indent=2)
        
        print(f"Configuration saved to {config_path}")
    
    def configure(self, build_dir: Optional[str] = None, options: Optional[Dict] = None) -> bool:
        """Configure the build system."""
        if build_dir:
            build_path = Path(build_dir)
        else:
            build_path = self.build_directory
        
        build_path.mkdir(parents=True, exist_ok=True)
        
        # Generate CMake command
        cmake_cmd = ["cmake", ".."]
        
        # Add build type
        cmake_cmd.extend([f"-DCMAKE_BUILD_TYPE={self.config['build_type'].value}"])
        
        # Add Python bindings
        if self.config['python_bindings']:
            cmake_cmd.append("-DPYTHON_EXECUTABLE=$(which python3)")
        
        # Add optimization flags
        if self.config['build_type'] == BuildConfiguration.RELEASE:
            cmake_cmd.extend([
                "-DCMAKE_CXX_FLAGS_RELEASE=-O3 -DNDEBUG",
                "-DCMAKE_CXX_FLAGS=-O3 -DNDEBUG"
            ])
        
        # Add sanitizer flags
        if self.config['enable_sanitizers']:
            if self.config['enable_address_sanitizer']:
                cmake_cmd.extend([
                    "-DCMAKE_CXX_FLAGS=-fsanitize=address -fno-omit-frame-pointer",
                    "-DCMAKE_EXE_LINKER_FLAGS=-fsanitize=address"
                ])
            
            if self.config['enable_undefined_behavior_sanitizer']:
                cmake_cmd.extend([
                    "-DCMAKE_CXX_FLAGS=-fsanitize=undefined -fno-sanitize-recover",
                    "-DCMAKE_EXE_LINKER_FLAGS=-fsanitize=undefined"
                ])
        
        # Add custom compiler flags
        if self.config['compiler_flags']:
            flags = " ".join(self.config['compiler_flags'])
            cmake_cmd.append(f"-DCMAKE_CXX_FLAGS={flags}")
        
        # Add custom definitions
        for definition in self.config['definitions']:
            cmake_cmd.append(f"-D{definition}")
        
        # Add include paths
        for include_path in self.config['include_paths']:
            cmake_cmd.append(f"-I{include_path}")
        
        # Add library paths
        for lib_path in self.config['library_paths']:
            cmake_cmd.append(f"-L{lib_path}")
        
        # Add install prefix
        cmake_cmd.extend([
            f"-DCMAKE_INSTALL_PREFIX={self.config['install_prefix']}",
            "-GNinja" if self.config['generator'] == "Ninja" else ""
        ])
        
        # Add options from parameter
        if options:
            for key, value in options.items():
                if isinstance(value, bool):
                    cmake_cmd.append(f"-D{key}={ 'ON' if value else 'OFF' }")
                else:
                    cmake_cmd.append(f"-D{key}={value}")
        
        # Run CMake configuration
        print(f"Configuring NLM build system in {build_path}...")
        print(f"CMake command: {' '.join(cmake_cmd)}")
        
        try:
            result = subprocess.run(
                cmake_cmd,
                cwd=build_path,
                capture_output=True,
                text=True,
                check=True
            )
            
            print("CMake configuration successful!")
            print(f"Output: {result.stdout}")
            return True
            
        except subprocess.CalledProcessError as e:
            print(f"CMake configuration failed with error: {e}")
            print(f"Error output: {e.stderr}")
            return False
    
    def build(self, build_dir: Optional[str] = None, targets: Optional[List[str]] = None,
              parallel: Optional[bool] = None) -> bool:
        """Build the project."""
        if build_dir:
            build_path = Path(build_dir)
        else:
            build_path = self.build_directory
        
        if not build_path.exists():
            print(f"Build directory {build_path} does not exist. Run configure first.")
            return False
        
        # Determine number of parallel jobs
        if parallel is None:
            parallel = self.config['parallel_build']
        
        # Generate build command
        if self.config['generator'] == "Ninja":
            cmd = ["ninja"]
        else:
            cmd = ["make", "-j4"] if parallel else ["make"]
        
        if targets:
            cmd.extend(targets)
        
        print(f"Building NLM project in {build_path}...")
        print(f"Build command: {' '.join(cmd)}")
        
        try:
            result = subprocess.run(
                cmd,
                cwd=build_path,
                capture_output=True,
                text=True,
                check=True
            )
            
            print("Build successful!")
            print(f"Output: {result.stdout}")
            return True
            
        except subprocess.CalledProcessError as e:
            print(f"Build failed with error: {e}")
            print(f"Error output: {e.stderr}")
            return False
    
    def install(self, install_dir: Optional[str] = None) -> bool:
        """Install the built project."""
        if install_dir:
            install_path = Path(install_dir)
        else:
            install_path = Path(self.config['install_prefix'])
        
        if not self.build_directory.exists():
            print("Build directory does not exist. Build first.")
            return False
        
        print(f"Installing NLM to {install_path}...")
        
        try:
            if self.config['generator'] == "Ninja":
                cmd = ["ninja", "install"]
            else:
                cmd = ["make", "install"]
            
            result = subprocess.run(
                cmd,
                cwd=self.build_directory,
                capture_output=True,
                text=True,
                check=True
            )
            
            print("Installation successful!")
            print(f"Output: {result.stdout}")
            return True
            
        except subprocess.CalledProcessError as e:
            print(f"Installation failed with error: {e}")
            print(f"Error output: {e.stderr}")
            return False
    
    def run_tests(self, test_dir: Optional[str] = None) -> bool:
        """Run tests."""
        if test_dir:
            test_path = Path(test_dir)
        else:
            test_path = self.build_directory / "test"
        
        if not test_path.exists():
            print(f"Test directory {test_path} does not exist.")
            return False
        
        print("Running NLM tests...")
        
        try:
            # Run CTest
            cmd = ["ctest", "--output-on-failure"]
            
            result = subprocess.run(
                cmd,
                cwd=self.build_directory,
                capture_output=True,
                text=True,
                check=True
            )
            
            print("Tests completed successfully!")
            print(f"Output: {result.stdout}")
            return True
            
        except subprocess.CalledProcessError as e:
            print(f"Tests failed with error: {e}")
            print(f"Error output: {e.stderr}")
            return False
    
    def run_benchmarks(self) -> bool:
        """Run benchmarks."""
        print("Running NLM benchmarks...")
        
        try:
            # Run benchmark targets
            cmd = ["make", "benchmark"]
            
            result = subprocess.run(
                cmd,
                cwd=self.build_directory,
                capture_output=True,
                text=True,
                check=True
            )
            
            print("Benchmarks completed successfully!")
            print(f"Output: {result.stdout}")
            return True
            
        except subprocess.CalledProcessError as e:
            print(f"Benchmarks failed with error: {e}")
            print(f"Error output: {e.stderr}")
            return False
    
    def clean(self, clean_all: bool = False):
        """Clean build artifacts."""
        print("Cleaning NLM build artifacts...")
        
        # Clean build directory
        if self.build_directory.exists():
            import shutil
            shutil.rmtree(self.build_directory)
            print(f"Cleaned build directory: {self.build_directory}")
        
        # Clean install directory
        if self.install_directory.exists():
            shutil.rmtree(self.install_directory)
            print(f"Cleaned install directory: {self.install_directory}")
        
        # Clean generated files
        generated_files = [
            "CMakeCache.txt",
            "CMakeFiles",
            "Makefile",
            "compile_commands.json",
            "*.so",
            "*.dylib",
            "*.dll",
            "*.lib",
            "*.exe"
        ]
        
        for pattern in generated_files:
            for file_path in self.project_root.glob(pattern):
                if file_path.is_file():
                    file_path.unlink()
                    print(f"Cleaned file: {file_path}")
        
        # Clean documentation files
        if clean_all:
            docs_dir = self.project_root / "docs"
            if docs_dir.exists():
                shutil.rmtree(docs_dir)
                print(f"Cleaned documentation directory: {docs_dir}")
            
            # Clean examples directory
            examples_dir = self.project_root / "examples"
            if examples_dir.exists():
                shutil.rmtree(examples_dir)
                print(f"Cleaned examples directory: {examples_dir}")
    
    def get_build_info(self) -> Dict:
        """Get information about the current build."""
        info = {
            'build_directory': str(self.build_directory),
            'install_directory': self.config['install_prefix'],
            'build_type': self.config['build_type'].value,
            'generator': self.config['generator'],
            'parallel_build': self.config['parallel_build'],
            'python_bindings': self.config['python_bindings'],
            'has_tests': self.config['enable_tests'],
            'has_benchmarks': self.config['enable_benchmarks']
        }
        
        # Check if build directory exists and has CMakeCache.txt
        if self.cmake_cache.exists():
            info['configured'] = True
            
            # Parse CMakeCache.txt for additional info
            with open(self.cmake_cache, 'r') as f:
                content = f.read()
                
                # Extract key-value pairs
                for line in content.split('\n'):
                    if '=' in line:
                        key, value = line.split('=', 1)
                        if key.startswith('CMAKE_') and not key.startswith('CMAKE_INTERNAL'):
                            info[key] = value
        else:
            info['configured'] = False
        
        return info
class EnvironmentManager:
    """Manages development environment and dependencies."""
    
    def __init__(self, project_root: str = "."):
        self.project_root = Path(project_root).resolve()
        self.env_file = self.project_root / ".env"
        self.requirements_file = self.project_root / "requirements.txt"
        self.setup_script = self.project_root / "setup.py"
        
    def setup_development_environment(self, auto_detect: bool = True) -> bool:
        """Set up development environment."""
        print("Setting up NLM development environment...")
        
        # Check for required tools
        required_tools = ["cmake", "make", "g++", "python3"]
        missing_tools = []
        
        for tool in required_tools:
            if not self._check_tool_availability(tool):
                missing_tools.append(tool)
        
        if missing_tools:
            print(f"Missing required tools: {missing_tools}")
            print("Please install the missing tools and try again.")
            return False
        
        # Set up Python environment
        if not self._setup_python_environment():
            print("Failed to set up Python environment")
            return False
        
        # Set up C++ environment
        if not self._setup_cpp_environment():
            print("Failed to set up C++ environment")
            return False
        
        # Set up build directory
        if not self._setup_build_directory():
            print("Failed to set up build directory")
            return False
        
        print("Development environment setup completed successfully!")
        return True
    
    def _check_tool_availability(self, tool: str) -> bool:
        """Check if a tool is available."""
        try:
            subprocess.run([tool, "--version"], 
                         capture_output=True, 
                         check=True,
                         timeout=5)
            return True
        except (subprocess.CalledProcessError, subprocess.TimeoutExpired, FileNotFoundError):
            return False
    
    def _setup_python_environment(self) -> bool:
        """Set up Python environment."""
        print("Setting up Python environment...")
        
        # Check if requirements.txt exists
        if not self.requirements_file.exists():
            print("requirements.txt not found - skipping Python package installation")
            return True
        
        # Install Python packages
        try:
            subprocess.run([
                sys.executable, "-m", "pip", "install", "-r",
                str(self.requirements_file)
            ], check=True, capture_output=True)
            
            print("Python packages installed successfully")
            return True
            
        except subprocess.CalledProcessError as e:
            print(f"Failed to install Python packages: {e}")
            return False
    
    def _setup_cpp_environment(self) -> bool:
        """Set up C++ development environment."""
        print("Setting up C++ development environment...")
        
        # Check for compiler
        if not self._check_tool_availability("g++"):
            print("g++ compiler not found")
            return False
        
        # Check for make
        if not self._check_tool_availability("make"):
            print("make utility not found")
            return False
        
        # Check for CMake
        if not self._check_tool_availability("cmake"):
            print("CMake not found")
            return False
        
        print("C++ development environment setup completed successfully")
        return True
    
    def _setup_build_directory(self) -> bool:
        """Set up build directory structure."""
        print("Setting up build directory structure...")
        
        # Create directories
        directories = [
            "build",
            "build/CMakeLists.txt",
            "docs",
            "examples",
            "tests",
            "benchmarks",
            "scripts",
            "config"
        ]
        
        for dir_path in directories:
            full_path = self.project_root / dir_path
            full_path.parent.mkdir(parents=True, exist_ok=True)
            
            if dir_path.endswith(".txt") or dir_path.endswith(".py"):
                # Create empty file for template files
                if not full_path.exists():
                    full_path.touch()
        
        # Create basic CMakeLists.txt if it doesn't exist
        cmake_file = self.project_root / "CMakeLists.txt"
        if not cmake_file.exists():
            self._create_basic_cmake_lists(cmake_file)
        
        print("Build directory structure setup completed")
        return True
    
    def _create_basic_cmake_lists(self, cmake_file: Path):
        """Create basic CMakeLists.txt file."""
        content = """cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 6.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build configuration
option(BUILD_PYTHON_BINDINGS "Build Python bindings" ON)
option(BUILD_TESTS "Build tests" ON)
option(BUILD_BENCHMARKS "Build benchmarks" ON)
option(BUILD_DOCS "Build documentation" ON)

# Set optimization flags
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")
endif()

# Add source directories
add_subdirectory(src)

# Add Python bindings
if(BUILD_PYTHON_BINDINGS)
    add_subdirectory(python)
endif()

# Add tests
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Add benchmarks
if(BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()

# Installation
install(DIRECTORY src/ DESTINATION include/nlm)
install(FILES README.md DESTINATION doc)
"""
        
        cmake_file.write_text(content)
    
    def activate_virtual_environment(self, env_name: str = "nlm_env"):
        """Activate virtual environment."""
        if platform.system() == "Windows":
            activate_script = f"{env_name}\\Scripts\\activate.bat"
        else:
            activate_script = f"{env_name}/bin/activate"
        
        activate_path = self.project_root / activate_script
        
        if not activate_path.exists():
            print(f"Virtual environment {env_name} not found")
            return False
        
        print(f"Activating virtual environment: {env_name}")
        # Note: This would actually activate the environment
        # In a real implementation, this would modify shell environment
        return True
class DevelopmentWorkflow:
    """Manages development workflow and automation."""
    
    def __init__(self, project_root: str = "."):
        self.project_root = Path(project_root).resolve()
        self.workflow_file = self.project_root / ".kilo/workflows.json"
        self.git_hooks_dir = self.project_root / ".git" / "hooks"
        self.ci_config = self.project_root / ".github" / "workflows" / "ci.yml"
        self.cicd_config = self.project_root / ".kilo" / "ci.json"
    
    def setup_ci_pipeline(self, use_github_actions: bool = True,
                         use_circleci: bool = False,
                         use_travisci: bool = False) -> bool:
        """Set up continuous integration pipeline."""
        print("Setting up CI/CD pipeline...")
        
        if use_github_actions:
            self._setup_github_actions()
        
        if use_circleci:
            self._setup_circle_ci()
        
        if use_travisci:
            self._setup_travis_ci()
        
        print("CI/CD pipeline setup completed")
        return True
    
    def _setup_github_actions(self):
        """Set up GitHub Actions CI configuration."""
        gh_dir = self.project_root / ".github" / "workflows"
        gh_dir.mkdir(parents=True, exist_ok=True)
        
        ci_yml = gh_dir / "ci.yml"
        ci_content = """name: NLM CI/CD Pipeline

on:
  push:
    branches: [main, develop]
  pull_request:
    branches: [main, develop]

jobs:
  test:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [ubuntu-latest, windows-latest, macOS-latest]
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Set up Python
      uses: actions/setup-python@v3
      with:
        python-version: '3.9'
    
    - name: Install dependencies
      run: |
        python -m pip install --upgrade pip
        pip install -r requirements.txt
    
    - name: Set up C++ environment
      run: |
        sudo apt-get update
        sudo apt-get install -y cmake g++ make
    
    - name: Configure CMake
      run: |
        mkdir build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Release
    
    - name: Build
      run: |
        cd build
        make -j$(nproc)
    
    - name: Run tests
      run: |
        cd build
        ctest --output-on-failure
    
    - name: Run benchmarks
      run: |
        cd build
        make benchmark
    
    - name: Upload artifacts
      uses: actions/upload-artifact@v3
      if: success()
      with:
        name: nlm-artifacts
        path: |
          build/
          install/
"""
        
        ci_yml.write_text(ci_content)
        print("GitHub Actions CI configuration created")
    
    def _setup_circle_ci(self):
        """Set up CircleCI configuration."""
        cc_dir = self.project_root / ".circleci"
        cc_dir.mkdir(parents=True, exist_ok=True)
        
        config_yml = cc_dir / "config.yml"
        config_content = """version: 2.1
jobs:
  build:
    docker:
      - image: cimg/cpp:2022.09
    
    steps:
    - checkout
    
    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y python3 python3-pip
    
    - name: Set up Python environment
      run: |
        python3 -m pip install --upgrade pip
        pip3 install -r requirements.txt
    
    - name: Configure CMake
      run: |
        mkdir build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Release
    
    - name: Build
      run: |
        cd build
        make -j$(nproc)
    
    - name: Run tests
      run: |
        cd build
        ctest --output-on-failure
    
    - name: Run benchmarks
      run: |
        cd build
        make benchmark
"""
        
        config_yml.write_text(config_content)
        print("CircleCI configuration created")
    
    def _setup_travis_ci(self):
        """Set up Travis CI configuration."""
        travis_yml = self.project_root / ".travis.yml"
        travis_content = """language: cpp

addons:
  apt:
    packages:
    - cmake
    - g++
    - make
    - python3
    - python3-pip

install:
  - pip3 install --upgrade pip
  - pip3 install -r requirements.txt

before_script:
  - mkdir build
  - cd build
  - cmake .. -DCMAKE_BUILD_TYPE=Release

script:
  - cd build
  - make -j$(nproc)
  - ctest --output-on-failure
  - make benchmark
"""
        
        travis_yml.write_text(travis_content)
        print("Travis CI configuration created")
    
    def setup_git_hooks(self):
        """Set up Git hooks for development workflow."""
        print("Setting up Git hooks...")
        
        self.git_hooks_dir.mkdir(parents=True, exist_ok=True)
        
        # Pre-commit hook
        pre_commit = self.git_hooks_dir / "pre-commit"
        pre_commit_content = """#!/bin/bash

# NLM pre-commit hooks

# Check for code formatting
if command -v clang-format &> /dev/null; then
    echo "Checking code formatting..."
    find . -name "*.cpp" -o -name "*.hpp" -o -name "*.cc" -o -name "*.hh" | \
    xargs clang-format -check --Werror
    if [ $? -ne 0 ]; then
        echo "Code formatting check failed"
        echo "Run: clang-format -i <file> to fix formatting"
        exit 1
    fi
fi

# Check for linting
if command -v cppcheck &> /dev/null; then
    echo "Running C++ linting..."
    cppcheck --enable=all --inconsistent-missing-override --unused-exception-specification --unused-function src/ 2>/dev/null || true
fi

# Run tests if available
if [ -d "build" ] && [ -f "build/CTestTestfile.cmake" ]; then
    echo "Running tests..."
    cd build
    ctest --output-on-failure
    if [ $? -ne 0 ]; then
        echo "Tests failed"
        exit 1
    fi
fi

# Check for documentation
if [ -f "docs/ARCHITECTURE.md" ]; then
    echo "Documentation exists - checking for basic structure..."
fi

echo "Pre-commit checks passed"
exit 0
"""
        
        pre_commit.write_text(pre_commit_content)
        pre_commit.chmod(0o755)
        
        print("Git hooks setup completed")
    
    def create_developer_workflow(self) -> Dict:
        """Create developer workflow configuration."""
        workflow = {
            'build_commands': {
                'configure': 'cmake -B build -DCMAKE_BUILD_TYPE=Release',
                'build': 'cmake --build build --parallel 4',
                'tests': 'cd build && ctest --output-on-failure',
                'benchmarks': 'cd build && make benchmark',
                'install': 'cd build && make install'
            },
            'development_tasks': [
                'setup_environment',
                'configure_build',
                'write_code',
                'run_tests',
                'build_documentation',
                'commit_changes'
            ],
            'quick_commands': {
                'setup': './setup.py',
                'build': './scripts/build.sh',
                'test': './scripts/test.sh',
                'docs': './scripts/docs.sh',
                'format': './scripts/format.sh'
            }
        }
        
        # Create workflow scripts
        self._create_workflow_scripts(workflow)
        
        return workflow
    
    def _create_workflow_scripts(self, workflow: Dict):
        """Create workflow automation scripts."""
        scripts_dir = self.project_root / "scripts"
        scripts_dir.mkdir(parents=True, exist_ok=True)
        
        # Build script
        build_script = scripts_dir / "build.sh"
        build_script_content = """#!/bin/bash

# NLM build script
set -e

PROJECT_ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
BUILD_DIR="${PROJECT_ROOT}/build"

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build . --parallel $(nproc)

# Run tests
cd "$PROJECT_ROOT"
"$BUILD_DIR"/ctest --output-on-failure

# Run benchmarks
cd "$PROJECT_ROOT"
"$BUILD_DIR"/make benchmark
"""
        
        build_script.write_text(build_script_content)
        build_script.chmod(0o755)
        
        # Test script
        test_script = scripts_dir / "test.sh"
        test_script_content = """#!/bin/bash

# NLM test script
set -e

PROJECT_ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
BUILD_DIR="${PROJECT_ROOT}/build"

if [ ! -d "$BUILD_DIR" ]; then
    echo "Build directory not found. Run build script first."
    exit 1
fi

cd "$BUILD_DIR"
ctest --output-on-failure
"""
        
        test_script.write_text(test_script_content)
        test_script.chmod(0o755)
        
        # Documentation script
        docs_script = scripts_dir / "docs.sh"
        docs_script_content = """#!/bin/bash

# NLM documentation script
set -e

PROJECT_ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
DOCS_DIR="${PROJECT_ROOT}/docs"

if [ ! -d "$DOCS_DIR" ]; then
    mkdir -p "$DOCS_DIR"
fi

# Generate documentation
if command -v doxygen &> /dev/null; then
    doxygen Doxyfile
else
    echo "Doxygen not found. Skipping documentation generation."
fi
"""
        
        docs_script.write_text(docs_script_content)
        docs_script.chmod(0o755)
        
        # Format script
        format_script = scripts_dir / "format.sh"
        format_script_content = """#!/bin/bash

# NLM code formatting script
set -e

PROJECT_ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)

# Format C++ code
if command -v clang-format &> /dev/null; then
    find "$PROJECT_ROOT/src" "$PROJECT_ROOT/tests" -name "*.cpp" -o -name "*.hpp" -o -name "*.cc" -o -name "*.hh" | \
    xargs clang-format -i
    echo "Code formatted successfully"
else
    echo "clang-format not found. Skipping code formatting."
fi

# Format Python code
if command -v black &> /dev/null; then
    find "$PROJECT_ROOT" -name "*.py" -path "*/examples/*" | \
    xargs black
    echo "Python code formatted successfully"
else
    echo "black not found. Skipping Python code formatting."
fi
"""
        
        format_script.write_text(format_script_content)
        format_script.chmod(0o755)
        
        print("Development workflow scripts created")
class IDEIntegration:
    """IDE integration for NLM development."""
    
    def __init__(self, project_root: str = "."):
        self.project_root = Path(project_root).resolve()
        self.vscode_settings_dir = self.project_root / ".vscode"
        self.clion_settings_dir = self.project_root / ".clion"
        self.eclipse_settings_dir = self.project_root / ".eclipse"
    
    def setup_vscode_integration(self):
        """Set up VS Code integration."""
        print("Setting up VS Code integration...")
        
        self.vscode_settings_dir.mkdir(parents=True, exist_ok=True)
        
        # settings.json
        settings_json = self.vscode_settings_dir / "settings.json"
        settings_content = {
            "C_Cpp.default.cppStandard": "c++20",
            "C_Cpp.intelliSense.engine": "cmake",
            "C_Cpp.intelliSense.cppPath": "${workspaceFolder}/build",
            "files.associations": {
                "*.hpp": "c++"
            },
            "C_Cpp.errorSquiggles": "enable",
            "cmake.buildOnSave": "all",
            "cmake.configurationProvider": "cmake-tools",
            "cmake.useCMakeFromPath": "true"
        }
        
        with open(settings_json, 'w') as f:
            json.dump(settings_content, f, indent=2)
        
        # tasks.json
        tasks_json = self.vscode_settings_dir / "tasks.json"
        tasks_content = {
            "version": "2.0.0",
            "tasks": [
                {
                    "label": "Build NLM",
                    "type": "shell",
                    "command": "cmake --build build --parallel 4",
                    "problemMatcher": "$cxx",
                    "group": {
                        "kind": "build",
                        "isDefault": True
                    },
                    "presentation": {
                        "reveal": "always",
                        "panel": "shared"
                    }
                },
                {
                    "label": "Run Tests",
                    "type": "shell",
                    "command": "cd build && ctest --output-on-failure",
                    "problemMatcher": "$ctest",
                    "group": "test",
                    "presentation": {
                        "reveal": "always",
                        "panel": "shared"
                    }
                },
                {
                    "label": "Configure",
                    "type": "shell",
                    "command": "cmake -B build -DCMAKE_BUILD_TYPE=Release",
                    "problemMatcher": [],
                    "group": "none",
                    "presentation": {
                        "reveal": "always",
                        "panel": "shared"
                    }
                }
            ]
        }
        
        with open(tasks_json, 'w') as f:
            json.dump(tasks_content, f, indent=2)
        
        # launch.json
        launch_json = self.vscode_settings_dir / "launch.json"
        launch_content = {
            "version": "0.2.0",
            "configurations": [
                {
                    "name": "Launch NLM Debug",
                    "type": "cppdbg",
                    "request": "launch",
                    "program": "${workspaceFolder}/build/nlm",
                    "args": [],
                    "stopAtEntry": false,
                    "cwd": "${workspaceFolder}",
                    "environmentVariables": {},
                    "externalConsole": false,
                    "MIMode": "gdb",
                    "miDebuggerPath": "gdb",
                    "setupCommands": [
                        {
                            "text": "-enable-pretty-printing",
                            "description": "Enable pretty printing for GDB"
                        }
                    ],
                    "preLaunchTask": "Build NLM"
                },
                {
                    "name": "Launch NLM Release",
                    "type": "cppdbg",
                    "request": "launch",
                    "program": "${workspaceFolder}/build/nlm",
                    "args": [],
                    "stopAtEntry": false,
                    "cwd": "${workspaceFolder}",
                    "environmentVariables": {},
                    "MIMode": "gdb",
                    "miDebuggerPath": "gdb",
                    "setupCommands": [
                        {
                            "text": "-enable-pretty-printing",
                            "description": "Enable pretty printing for GDB"
                        }
                    ],
                    "preLaunchTask": "Build NLM"
                }
            ]
        }
        
        with open(launch_json, 'w') as f:
            json.dump(launch_content, f, indent=2)
        
        print("VS Code integration setup completed")
    
    def setup_clion_integration(self):
        """Set up CLion integration."""
        print("Setting up CLion integration...")
        
        # Create .ide/clion directory
        clion_dir = self.project_root / ".idea"
        clion_dir.mkdir(parents=True, exist_ok=True)
        
        # Create modules.xml
        modules_xml = clion_dir / "modules.xml"
        modules_content = """<?xml version="1.0" encoding="UTF-8"?>
<project version="4">
  <component name="CMakeWorkspace" url="file://$PROJECT_DIR$/build/CMakeCache.txt" name="CMakeWorkspace" pythonInterpreter="$PROJECT_DIR$/build/cmake-python-venv/bin/python" cmakeOptions="" CMakePreset="" generatorOptions="" cmakeBuild="Debug" cmakeConfigure="$PROJECT_DIR$/build" />
  <component name="CMakeWorkspaceCache">
    <option name="CMakeWorkspace" value="true" />
    <option name="UseIdeaCMake" value="true" />
  </component>
  <component name="CMakeRunDebug" active-preset="Debug" />)
</project>"""
        
        modules_xml.write_text(modules_content)
        
        # Create cpp.xml
        cpp_xml = clion_dir / "cpp.xml"
        cpp_content = """<?xml version="1.0" encoding="UTF-8"?>
<project version="4">
  <component name="AutoHeaderIncludePolicy" value="NONE" />
  <component name="CCAnalyzer">
    <option name="indexing" value="enabled" />
    <option name="cache" value="enabled" />
  </component>
  <component name="CMakeRunDebug" active-preset="Debug" />)
</project>"""
        
        cpp_xml.write_text(cpp_content)
        
        print("CLion integration setup completed")
    
    def setup_eclipse_integration(self):
        """Set up Eclipse integration."""
        print("Setting up Eclipse integration...")
        
        # Create .project file
        project_file = self.project_root / ".project"
        project_content = """<?xml version="1.0" encoding="UTF-8"?>
<projectDescription>
    <name>NLM</name>
    <comment>NLM (熙然) - Neural Learning Machine</comment>
    <projects>
    </projects>
</projectDescription>
<node>
    <id>$PROJECT_NAME$</id>
    <data>
        <wrapperLocation value="" />
        <configurationLocation value="config/cdt/cfg/cfg.xml" />
    </data>
</node>
"""
        
        project_file.write_text(project_content)
        
        # Create .cproject file
        cproject_file = self.project_root / ".cproject"
        cproject_content = """<?xml version="1.0" encoding="UTF-8"?>
<cproject version="4.0.2">
    <storage version="4.0.2">
        <project uid="NLM" name="NLM">
            <toolChain id="gnu-tools-linux-x86_64-gcc-9.3.0-mp-8.1.0-libstdc++6-11.0.4-sse3" type="std-gcc-linux" location="C:/tools/gcc-9.3.0/bin" name="GCC 9.3.0" description="GCC 9.3.0" version="9.3.0" standard="gnu++20">
                <compiler cxx="C:/tools/gcc-9.3.0/bin/g++.exe" c="C:/tools/gcc-9.3.0/bin/gcc.exe" />\n            </toolChain>
            <buildTarget id="make" commandLine="make -j${env.NUMBER_OF_PROCESSORS}" name="Build" output="$(PROJECT_DIR)/build" workingDirectory="$(PROJECT_DIR)" type="external" autoBuild="false" autoClean="false" autoGenerateProjectFiles="false" useDefaultOutputDirs="false" allowOverride="false" />
            <buildTarget id="cmake" commandLine="cmake -B build -DCMAKE_BUILD_TYPE=Release" name="Configure" output="$(PROJECT_DIR)/build" workingDirectory="$(PROJECT_DIR)" type="external" autoBuild="false" autoClean="false" autoGenerateProjectFiles="false" allowOverride="false" />
        </project>
    </storage>
</cproject>
"""
        
        cproject_file.write_text(cproject_content)
        
        print("Eclipse integration setup completed")
class DeveloperTools:
    """Developer tools and utilities for NLM."""
    
    def __init__(self, project_root: str = "."):
        self.project_root = Path(project_root).resolve()
        self.tools_dir = self.project_root / "tools"
    
    def create_development_tools(self):
        """Create development tools and utilities."""
        print("Creating development tools...")
        
        self.tools_dir.mkdir(parents=True, exist_ok=True)
        
        # Create build script
        build_script = self.tools_dir / "build.py"
        build_script_content = '''#!/usr/bin/env python3
"""
NLM Build Script

This script provides common build operations for NLM.
"""

import argparse
import subprocess
import sys
from pathlib import Path

def run_command(command, cwd=None):
    """Run a command and check its result."""
    print(f"Running: {command}")
    try:
        subprocess.run(command, shell=True, cwd=cwd, check=True)
        return True
    except subprocess.CalledProcessError as e:
        print(f"Command failed: {e}")
        return False

def build(args):
    """Build the project."""
    build_dir = Path(args.build_dir) if args.build_dir else Path("build")
    build_dir.mkdir(exist_ok=True)
    
    # Configure
    if not run_command(f"cmake -B {build_dir} -DCMAKE_BUILD_TYPE={'Release' if args.release else 'Debug'}", cwd=args.project_dir):
        return False
    
    # Build
    if not run_command(f"cmake --build {build_dir} --parallel {args.jobs}", cwd=args.project_dir):
        return False
    
    return True

def test(args):
    """Run tests."""
    build_dir = Path(args.build_dir) if args.build_dir else Path("build")
    
    if not build_dir.exists():
        print("Build directory does not exist. Run build first.")
        return False
    
    return run_command(f"cd {build_dir} && ctest --output-on-failure", cwd=args.project_dir)

def benchmark(args):
    """Run benchmarks."""
    build_dir = Path(args.build_dir) if args.build_dir else Path("build")
    
    if not build_dir.exists():
        print("Build directory does not exist. Run build first.")
        return False
    
    return run_command(f"cd {build_dir} && make benchmark", cwd=args.project_dir)

def clean(args):
    """Clean build artifacts."""
    build_dir = Path(args.build_dir) if args.build_dir else Path("build")
    
    if build_dir.exists():
        import shutil
        shutil.rmtree(build_dir)
        print(f"Cleaned build directory: {build_dir}")
    
    # Clean other generated files
    for pattern in ["*.so", "*.dylib", "*.dll", "*.exe", "CMakeCache.txt", "compile_commands.json"]:
        for file_path in Path(".").glob(pattern):
            if file_path.is_file():
                file_path.unlink()
                print(f"Cleaned file: {file_path}")
    
    return True

def main():
    parser = argparse.ArgumentParser(description="NLM Build Script")
    subparsers = parser.add_subparsers(dest="command", help="Command to execute")
    
    # Build command
    build_parser = subparsers.add_parser("build", help="Build the project")
    build_parser.add_argument("--build-dir", default="build", help="Build directory")
    build_parser.add_argument("--project-dir", default=".", help="Project directory")
    build_parser.add_argument("--release", action="store_true", help="Build release version")
    build_parser.add_argument("--jobs", type=int, default=4, help="Number of parallel jobs")
    
    # Test command
    test_parser = subparsers.add_parser("test", help="Run tests")
    test_parser.add_argument("--build-dir", default="build", help="Build directory")
    test_parser.add_argument("--project-dir", default=".", help="Project directory")
    
    # Benchmark command
    benchmark_parser = subparsers.add_parser("benchmark", help="Run benchmarks")
    benchmark_parser.add_argument("--build-dir", default="build", help="Build directory")
    benchmark_parser.add_argument("--project-dir", default=".", help="Project directory")
    
    # Clean command
    clean_parser = subparsers.add_parser("clean", help="Clean build artifacts")
    clean_parser.add_argument("--build-dir", default="build", help="Build directory")
    
    args = parser.parse_args()
    
    if args.command == "build":
        success = build(args)
    elif args.command == "test":
        success = test(args)
    elif args.command == "benchmark":
        success = benchmark(args)
    elif args.command == "clean":
        success = clean(args)
    else:
        parser.print_help()
        success = False
    
    sys.exit(0 if success else 1)

if __name__ == "__main__":
    main()
'''
        
        build_script.write_text(build_script_content)
        build_script.chmod(0o755)
        
        # Create analysis script
        analysis_script = self.tools_dir / "analyze.py"
        analysis_script_content = '''#!/usr/bin/env python3
"""
NLM Analysis Script

This script provides analysis tools for NLM experiments.
"""

import json
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from pathlib import Path
import argparse
import sys

def load_results(file_path: str):
    """Load results from JSON file."""
    with open(file_path, 'r') as f:
        return json.load(f)

def analyze_results(results: dict):
    """Analyze results and generate insights."""
    analysis = {
        'summary': {},
        'performance_trends': {},
        'parameter_impact': {},
        'recommendations': []
    }
    
    # Extract key metrics
    metrics = []
    for batch_id, batch_result in results['results'].items():
        if batch_result['success']:
            metrics.append({
                'batch_id': batch_id,
                'duration': batch_result['duration'],
                'total_spikes': batch_result['metrics'].get('total_spikes', 0),
                'firing_rate': batch_result['metrics'].get('average_firing_rate', 0),
                'neuron_count': batch_result['parameters'].get('neuron_count', 0),
                'synapse_density': batch_result['parameters'].get('synapse_density', 0),
                'learning_rate': batch_result['parameters'].get('learning_rate', 0)
            })
    
    if not metrics:
        return analysis
    
    df = pd.DataFrame(metrics)
    
    # Summary statistics
    analysis['summary'] = {
        'total_batches': len(metrics),
        'successful_batches': len([m for m in metrics if m['batch_id'].startswith('batch')]),
        'average_duration': df['duration'].mean(),
        'total_spikes': df['total_spikes'].sum(),
        'average_firing_rate': df['firing_rate'].mean()
    }
    
    # Performance trends
    if len(df) >= 2:
        analysis['performance_trends'] = {
            'neuron_count_impact': {
                'correlation': df['neuron_count'].corr(df['firing_rate']),
                'coefficient': np.polyfit(df['neuron_count'], df['firing_rate'], 1)[0]
            },
            'synapse_density_impact': {
                'correlation': df['synapse_density'].corr(df['firing_rate']),
                'coefficient': np.polyfit(df['synapse_density'], df['firing_rate'], 1)[0]
            },
            'learning_rate_impact': {
                'correlation': df['learning_rate'].corr(df['firing_rate']),
                'coefficient': np.polyfit(df['learning_rate'], df['firing_rate'], 1)[0]
            }
        }
    
    # Parameter impact analysis
    for param in ['neuron_count', 'synapse_density', 'learning_rate']:
        if param in df.columns:
            # Find parameter ranges that give best results
            param_ranges = df.groupby(param)['firing_rate'].mean().sort_values(ascending=False)
            analysis['parameter_impact'][param] = {
                'best_values': param_ranges.head(3).to_dict(),
                'worst_values': param_ranges.tail(3).to_dict()
            }
    
    # Generate recommendations
    if 'neuron_count_impact' in analysis['performance_trends']:
        neuron_corr = abs(analysis['performance_trends']['neuron_count_impact']['correlation'])
        if neuron_corr > 0.7:
            analysis['recommendations'].append(
                f"Neuron count strongly affects firing rate (correlation: {neuron_corr:.2f}). "
                "Consider this parameter carefully in optimization."
            )
    
    return analysis

def create_visualizations(analysis: dict, output_dir: str = "analysis_plots"):
    """Create visualizations of analysis results."""
    Path(output_dir).mkdir(exist_ok=True)
    
    if 'performance_trends' in analysis:
        # Create correlation heatmap
        fig, axes = plt.subplots(2, 2, figsize=(15, 12))
        
        # Neuron count vs firing rate
        axes[0, 0].scatter(analysis['performance_trends']['neuron_count_impact']['coefficient'], 
                         analysis['performance_trends']['neuron_count_impact']['correlation'], 
                         s=100, alpha=0.7, c='blue')
        axes[0, 0].set_xlabel('Effect Size')
        axes[0, 0].set_ylabel('Correlation')
        axes[0, 0].set_title('Neuron Count Impact on Firing Rate')
        axes[0, 0].grid(True, alpha=0.3)
        
        # Synapse density vs firing rate
        axes[0, 1].scatter(analysis['performance_trends']['synapse_density_impact']['coefficient'], 
                         analysis['performance_trends']['synapse_density_impact']['correlation'], 
                         s=100, alpha=0.7, c='green')
        axes[0, 1].set_xlabel('Effect Size')
        axes[0, 1].set_ylabel('Correlation')
        axes[0, 1].set_title('Synapse Density Impact on Firing Rate')
        axes[0, 1].grid(True, alpha=0.3)
        
        # Learning rate vs firing rate
        axes[1, 0].scatter(analysis['performance_trends']['learning_rate_impact']['coefficient'], 
                         analysis['performance_trends']['learning_rate_impact']['correlation'], 
                         s=100, alpha=0.7, c='red')
        axes[1, 0].set_xlabel('Effect Size')
        axes[1, 0].set_ylabel('Correlation')
        axes[1, 0].set_title('Learning Rate Impact on Firing Rate')
        axes[1, 0].grid(True, alpha=0.3)
        
        # Parameter effectiveness radar chart
        if 'parameter_impact' in analysis:
            params = list(analysis['parameter_impact'].keys())
            if params:
                # Calculate average effectiveness for each parameter
                effectiveness = {}
                for param in params:
                    values = list(analysis['parameter_impact'][param]['best_values'].values())
                    effectiveness[param] = np.mean(values) if values else 0
                
                angles = np.linspace(0, 2 * np.pi, len(params), endpoint=False)
                angles += np.pi / 2
                
                fig2, ax2 = plt.subplots(figsize=(10, 8), subplot_kw={'projection': 'polar'})
                
                for i, param in enumerate(params):
                    values = [effectiveness[param] / max(effectiveness.values()) if max(effectiveness.values()) > 0 else 0]
                    ax2.bar(angles[i], values[0], width=2*np.pi/len(params), alpha=0.7, label=param)
                
                ax2.set_theta_offset(np.pi / 2)
                ax2.set_theta_direction(-1)
                ax2.set_ylim(0, 1)
                ax2.set_title('Parameter Effectiveness Comparison')
                ax2.legend(loc='upper right', bbox_to_anchor=(1.3, 1.0))
                
                plt.savefig(f"{output_dir}/parameter_effectiveness.png", 
                           dpi=300, bbox_inches='tight')
                plt.close(fig2)
        
        plt.tight_layout()
        plt.savefig(f"{output_dir}/performance_analysis.png", 
                   dpi=300, bbox_inches='tight')
        plt.close()
    
    # Save analysis JSON
    with open(f"{output_dir}/analysis_results.json", 'w') as f:
        json.dump(analysis, f, indent=2, default=str)
    
    print(f"Visualizations saved to {output_dir}")

def main():
    parser = argparse.ArgumentParser(description="NLM Analysis Script")
    parser.add_argument("results_file", help="Path to results JSON file")
    parser.add_argument("--output-dir", default="analysis_plots", help="Output directory for visualizations")
    parser.add_argument("--verbose", action="store_true", help="Enable verbose output")
    
    args = parser.parse_args()
    
    # Load results
    print(f"Loading results from {args.results_file}...")
    results = load_results(args.results_file)
    
    # Analyze results
    print("Analyzing results...")
    analysis = analyze_results(results)
    
    # Create visualizations
    print(f"Creating visualizations in {args.output_dir}...")
    create_visualizations(analysis, args.output_dir)
    
    # Print summary
    print("\n" + "="*80)
    print("ANALYSIS SUMMARY")
    print("="*80)
    
    if 'summary' in analysis:
        summary = analysis['summary']
        print(f"Total batches processed: {summary['total_batches']}")
        print(f"Successful batches: {summary['successful_batches']}")
        print(f"Total processing time: {summary['average_duration']:.2f}s")
        print(f"Total spikes generated: {summary['total_spikes']}")
        print(f"Average firing rate: {summary['average_firing_rate']:.4f} Hz")
    
    if 'recommendations' in analysis:
        print(f"\nRecommendations ({len(analysis['recommendations'])}):")
        for i, recommendation in enumerate(analysis['recommendations'], 1):
            print(f"  {i}. {recommendation}")
    
    print(f"\nAnalysis complete! Results saved to {args.output_dir}")

if __name__ == "__main__":
    main()
'''
        
        analysis_script.write_text(analysis_script_content)
        analysis_script.chmod(0o755)
        
        # Create readme file
        readme_content = """# NLM Development Tools

This directory contains development tools and utilities for NLM (熙然) - Neural Learning Machine.

## Available Tools

### build.py
Build script for NLM projects. Provides commands for:
- Building the project (`build`)
- Running tests (`test`)
- Running benchmarks (`benchmark`)
- Cleaning build artifacts (`clean`)

Usage:
```bash
./tools/build.py build --build-dir build --release
./tools/build.py test
./tools/build.py benchmark
./tools/build.py clean
```

### analyze.py
Analysis script for NLM experiment results. Provides:
- Results loading and analysis
- Performance trend analysis
- Parameter impact analysis
- Visualization generation
- Recommendations generation

Usage:
```bash
./tools/analyze.py results.json --output-dir analysis_plots --verbose
```

## Installation

1. Ensure the tools directory is executable:
```bash
chmod +x tools/build.py tools/analyze.py
```

2. Run the build script:
```bash
./tools/build.py build
```

3. Run tests:
```bash
./tools/build.py test
```

## Configuration

The build system uses configuration files:
- `build_config.json` - Build configuration
- `.env` - Environment variables
- `requirements.txt` - Python dependencies

## Development Workflow

1. **Setup**: Run `./setup.py` or `./tools/build.py build`
2. **Code Changes**: Edit source files in `src/`
3. **Testing**: Run `./tools/build.py test`
4. **Building**: Run `./tools/build.py build --release`
5. **Analysis**: Run `./tools/analyze.py results.json`
6. **Documentation**: Generate documentation with `./tools/docs.sh`

## Customization

You can customize the build system by:
1. Modifying `build_config.json`
2. Adding custom build commands
3. Configuring IDE integrations
4. Setting up CI/CD pipelines
"""
        
        readme_file = self.tools_dir / "README.md"
        readme_file.write_text(readme_content)
        
        print("Development tools created successfully")
    
    def get_developer_tools_summary(self) -> Dict:
        """Get summary of available developer tools."""
        tools = []
        
        if (self.tools_dir / "build.py").exists():
            tools.append({
                'name': 'Build Script',
                'description': 'Provides build, test, and benchmark commands',
                'path': str(self.tools_dir / "build.py"),
                'usage': './tools/build.py build'
            })
        
        if (self.tools_dir / "analyze.py").exists():
            tools.append({
                'name': 'Analysis Script',
                'description': 'Analyzes experiment results and generates visualizations',
                'path': str(self.tools_dir / "analyze.py"),
                'usage': './tools/analyze.py results.json'
            })
        
        return {
            'tools_directory': str(self.tools_dir),
            'total_tools': len(tools),
            'tools': tools
        }
class DocumentationGenerator:
    """Generates documentation for NLM."""
    
    def __init__(self, project_root: str = "."):
        self.project_root = Path(project_root).resolve()
        self.docs_dir = self.project_root / "docs"
    
    def generate_documentation(self, doc_type: str = "all") -> bool:
        """Generate documentation."""
        print(f"Generating {doc_type} documentation...")
        
        self.docs_dir.mkdir(parents=True, exist_ok=True)
        
        if doc_type in ["all", "api"]:
            self._generate_api_documentation()
        
        if doc_type in ["all", "architecture"]:
            self._generate_architecture_documentation()
        
        if doc_type in ["all", "tutorials"]:
            self._generate_tutorial_documentation()
        
        if doc_type in ["all", "examples"]:
            self._generate_example_documentation()
        
        if doc_type in ["all", "contributing"]:
            self._generate_contributing_documentation()
        
        print("Documentation generation completed")
        return True
    
    def _generate_api_documentation(self):
        """Generate API documentation."""
        print("Generating API documentation...")
        
        # This would typically use tools like Doxygen, Sphinx, or similar
        # For demonstration, create a simple API documentation file
        
        api_doc = self.docs_dir / "API.md"
        api_content = """# NLM API Documentation

## Overview

NLM (熙然) - Neural Learning Machine provides a comprehensive set of APIs for neural simulation and experimentation.

## Core Classes

### Brain Class
- `Brain(config)` - Creates a neural simulation brain
- `brain.initialize()` - Initializes the brain
- `brain.step(step)` - Runs a simulation step
- `brain.getTotalNeuronCount()` - Gets total neuron count
- `brain.getTotalSynapseCount()` - Gets total synapse count
- `brain.getFiringNeuronCount()` - Gets firing neuron count
- `brain.getAverageFiringRate()` - Gets average firing rate
- `brain.save(file)` - Saves brain state
- `brain.load(file)` - Loads brain state

### AgentBrain Class
- `AgentBrain(brain)` - Creates an agent brain interface
- `agent.initialize(world)` - Initializes with world
- `agent.processSensoryInput(percept)` - Processes sensory input
- `agent.decodeMotorCommand()` - Decodes motor command
- `agent.applyRewardModulation(reward, predicted_reward)` - Applies reward modulation
- `agent.updateDevelopment(timestep)` - Updates development

### SimpleWorld Class
- `SimpleWorld()` - Creates a simulation world
- `world.configure(width, height, vision_width, vision_height)` - Configures world
- `world.reset()` - Resets world
- `world.update(timestep)` - Updates world
- `world.applyMotorCommand(cmd, time)` - Applies motor command
- `world.getSensoryPercept()` - Gets sensory percept

### Config Class
- `Config()` - Creates configuration object
- `config.loadFromFile(file)` - Loads from file
- `config.saveToFile(file)` - Saves to file
- `config.set(key, value)` - Sets configuration value
- `config.getOr(key, default)` - Gets configuration value with default
"""
        
        api_doc.write_text(api_content)
    
    def _generate_architecture_documentation(self):
        """Generate architecture documentation."""
        print("Generating architecture documentation...")
        
        # This would typically document the system architecture
        # For demonstration, create a simple architecture documentation file
        
        arch_doc = self.docs_dir / "ARCHITECTURE.md"
        arch_content = """# NLM Architecture

## Overview

NLM (熙然) - Neural Learning Machine is an experimental computational brain project that implements:

### Core Components

1. **Neural Computation Layer**
   - Leaky Integrate-and-Fire (LIF) neurons
   - Event-driven spike propagation with synaptic delays
   - Synaptic plasticity (STDP, Hebbian, structural)
   - Neuromodulation (dopamine, curiosity, novelty)

2. **Memory Systems**
   - Working memory with persistent activity and competition
   - Episodic memory with experience encoding and replay
   - Associative memory with Hebbian pattern associations

3. **Prediction and Planning**
   - Neural prediction system with error computation
   - Neural planner with action sequence evaluation
   - Concept formation from experience patterns
   - Attention with competitive selection dynamics

4. **Development and Adaptation**
   - Developmental stages affecting plasticity rates
   - Structural plasticity modulated by age
   - Neural excitability changes with development

5. **World Interface**
   - Simple 2D world simulation
   - Sensory processing and transduction
   - Motor action generation and execution

### Brain Architecture

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING (LIF dynamics, spikes)
  ↓
INTERNAL STATE (working memory, attention)
  ↓
MEMORY / PREDICTION
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓
ACTION SELECTION
  ↓
ACTION OUTPUT
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

### Memory System Integration

The NLM memory system integrates multiple memory types:

- **Working Memory**: Short-term active information processing
- **Episodic Memory**: Experience storage and replay
- **Associative Memory**: Pattern associations and recall

All memory systems are connected to neural processing through dedicated pathways.

### Neuromodulation Integration

Neuromodulation affects multiple aspects of brain function:

- **Dopamine**: Reward prediction and reinforcement learning
- **Curiosity**: Exploration and novelty detection
- **Novelty**: Surprise detection and adaptation

Neuromodulators are integrated with plasticity rules and development.

### Development System

The development system modulates brain plasticity throughout the brain's life:

- **Initial Stage**: High plasticity, rapid learning
- **Critical Period**: Elevated learning and plasticity
- **Maturation**: Normal learning and stabilization
- **Adult**: Reduced plasticity, stable behavior
- **Aging**: Further reduced plasticity and increased instability

### Performance Optimization

NLM implements several performance optimization techniques:

1. **Memory Pools**: Efficient memory allocation and reuse
2. **Event Queues**: Optimized spike event processing
3. **SIMD Vectorization**: Parallel processing using SIMD instructions
4. **Parallel Processing**: Multi-core processing capabilities
5. **Checkpointing**: State saving and loading for long simulations
"""
        
        arch_doc.write_text(arch_content)
    
    def _generate_tutorial_documentation(self):
        """Generate tutorial documentation."""
        print("Generating tutorial documentation...")
        
        # Create tutorial documentation
        tutorial_doc = self.docs_dir / "TUTORIAL.md"
        tutorial_content = """# NLM Tutorial

## Quick Start

This tutorial provides a quick introduction to NLM (熙然) - Neural Learning Machine.

### Step 1: Install NLM

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Build the project
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Step 2: Create Your First Brain

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create a brain
brain = pynlm.createBrain(config)

# Initialize the brain
brain.initialize()

# Run a simple simulation
for i in range(100):
    brain.step(i)

print(f"Simulation complete! Created {brain.getTotalNeuronCount()} neurons")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.4f} Hz")
```

### Step 3: Create an Agent with World

```python
import pynlm

# Create brain and world
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation with agent
for step in range(200):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process in agent
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Log progress
    if step % 50 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
              f"curiosity: {agent.getCuriosityLevel():.3f}")

print("Agent simulation complete!")
```

### Step 4: Explore Advanced Features

```python
import pynlm

# Create configuration with advanced features
config = pynlm.createDevelopmentConfig()

# Create brain and agent
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run learning simulation
for step in range(500):
    # ... simulation loop ...
    
    # Collect statistics
    if step % 100 == 0:
        stat = {
            'step': step,
            'spikes': brain.getTotalSpikeCount(),
            'firing': brain.getFiringNeuronCount(),
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'dev_stage': brain.getDevelopmentalStage()
        }
        print(f"Statistics at step {step}: {stat}")
```

## Advanced Topics

### Memory Systems

NLM implements three main memory systems:

1. **Working Memory**: Transient active information processing
   - Maintains neural activity patterns
   - Implements competition among neural representations
   - Supports short-term information storage

2. **Episodic Memory**: Experience storage and replay
   - Encodes experiences as episodic traces
   - Supports replay during rest periods
   - Enables memory consolidation

3. **Associative Memory**: Pattern associations and recall
   - Forms associations between neural patterns
   - Supports pattern completion
   - Enables inference and generalization

### Neuromodulation

Key neuromodulators in NLM:

- **Dopamine**: Reward prediction, reinforcement learning
- **Curiosity**: Exploration motivation, novelty detection
- **Novelty**: Surprise detection, adaptation

### Development

Developmental stages:

1. **Initial**: High plasticity, rapid learning
2. **Critical Period**: Elevated learning and plasticity
3. **Maturation**: Normal learning and stabilization
4. **Adult**: Reduced plasticity, stable behavior
5. **Aging**: Reduced plasticity, increased instability

### Performance Optimization

Performance optimization techniques:

1. **Memory Management**: Memory pools, object reuse
2. **Event Processing**: Efficient spike event handling
3. **Parallel Processing**: Multi-core optimization
4. **SIMD**: Vectorized operations
5. **Checkpointing**: State persistence

## Troubleshooting

### Common Issues

1. **"Brain isn't doing anything"**
   - Ensure `brain.initialize()` is called
   - Try increasing the number of steps
   - Check if sensory input is connected

2. **"Agent isn't moving"**
   - Ensure `world.applyMotorCommand()` is called
   - Check that `world.update()` is being called
   - Verify sensory input is being processed

3. **"Everything is 0"**
   - Brains need time to "warm up" - try more steps
   - Check if `brain.initialize()` is called
   - Ensure sensory input is connected

### Performance Issues

1. **High memory usage**
   - Reduce neuron count
   - Use memory-efficient data structures
   - Implement checkpointing

2. **Slow execution**
   - Reduce simulation timestep
   - Optimize connectivity patterns
   - Use parallel processing

3. **Memory leaks**
   - Run with memory profiler
   - Use garbage collection
   - Check for memory management issues

## Further Reading

- [ARCHITECTURE.md](ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](EXPERIMENTS.md) - Experiment descriptions
- [ROADMAP.md](ROADMAP.md) - Development roadmap
- [LIMITATIONS.md](LIMITATIONS.md) - Current limitations
- [PHASE6_FINAL_AUDIT.md](docs/PHASE6_FINAL_AUDIT.md) - Phase 6 audit report

## Citation

If you use NLM in your research, please cite:

```
@misc{nlm2024,
  title={NLM (熙然) - Neural Learning Machine},
  author={NLM Research Team},
  year={2024},
  howpublished={https://github.com/nlm-project/nlm}
}
```
"""
        
        tutorial_doc.write_text(tutorial_content)
    
    def _generate_example_documentation(self):
        """Generate example documentation."""
        print("Generating example documentation...")
        
        # Create examples directory
        examples_dir = self.docs_dir / "examples"
        examples_dir.mkdir(parents=True, exist_ok=True)
        
        # Create example documentation
        example_doc = examples_dir / "EXAMPLES.md"
        example_content = """# NLM Examples

This directory contains example simulations and use cases for NLM (熙然) - Neural Learning Machine.

## Example 1: Simple Brain Simulation

This example demonstrates basic brain simulation with LIF neurons.

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(1000):
    brain.step(i)

print(f"Simulation completed with {brain.getTotalSpikeCount()} spikes")
```

## Example 2: Agent in a World

This example shows an agent interacting with a simulated world.

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(500):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Agent simulation completed!")
```

## Example 3: Learning and Development

This example demonstrates learning and development capabilities.

```python
import pynlm

# Use development configuration for learning
config = pynlm.createDevelopmentConfig()

# Create brain and agent
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all development features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run long-term simulation
for step in range(2000):
    world.update(0.1)
    
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward based on agent behavior
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    agent.updateDevelopment(0.1)
    
    # Log statistics periodically
    if step % 500 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")

print("Development simulation completed!")
```

## Example 4: Benchmarking

This example demonstrates performance benchmarking.

```python
import pynlm
import time

# Create different configuration types
configs = {
    'basic': pynlm.createDefaultConfig(),
    'optimized': pynlm.createHighPerformanceConfig(),
    'learning': pynlm.createDevelopmentConfig(),
    'exploration': pynlm.createExplorationConfig()
}

print("Performance Comparison:")
print("=" * 50)

for name, config in configs.items():
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Time the simulation
    start_time = time.time()
    for i in range(1000):
        brain.step(i)
    elapsed = time.time() - start_time
    
    print(f"{name:12s}: {elapsed:8.3f}s, {1000/elapsed:8.0f} steps/s, "
          f"{brain.getTotalSpikeCount():8d} spikes")
```

## Example 5: Batch Processing

This example demonstrates batch processing capabilities.

```python
import pynlm

# Create parameter sweep
batch = pynlm.SimulationBatch("ExperimentBatch")

# Add multiple configurations
for neuron_count in [500, 1000, 1500, 2000]:
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.synapse_density", 0.1 / neuron_count)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    batch.addBrain(brain)

# Run batch simulation
print("Running batch simulation...")
batch.runBatch(200, timestep=0.1)

print("Batch simulation complete!")
```

## Running Examples

To run these examples, save them as Python files and execute:

```bash
python example_name.py
```

Each example will print progress information and final results.

## Requirements

These examples require:

1. Python 3.8 or higher
2. pynlm package installed
3. All optional dependencies installed (for advanced features)

For installation instructions, see the project README.
"""
        
        example_doc.write_text(example_content)
    
    def _generate_contributing_documentation(self):
        """Generate contributing documentation."""
        print("Generating contributing documentation...")
        
        # Create contributing documentation
        contrib_doc = self.docs_dir / "CONTRIBUTING.md"
        contrib_content = """# Contributing to NLM

Thank you for your interest in contributing to NLM (熙然) - Neural Learning Machine!

## How to Contribute

### 1. Code Contributions

**Submit Bug Reports**

- Use the issue tracker to report bugs
- Include detailed reproduction steps
- Provide test cases when possible

**Fix Bugs**

- Fork the repository
- Create a feature branch
- Fix the bug
- Add tests for the fix
- Submit a pull request

**Add Features**

- Fork the repository
- Create a feature branch
- Implement the feature
- Add tests
- Document the feature
- Submit a pull request

### 2. Documentation Contributions

- Improve existing documentation
- Add new tutorials
- Fix typos and errors
- Add examples

### 3. Testing and Validation

- Run existing tests
- Add new test cases
- Test on different platforms
- Validate performance

## Development Workflow

### Prerequisites

1. Python 3.8 or higher
2. CMake 3.16 or higher
3. C++ compiler supporting C++20
4. Make or Ninja build system

### Getting Started

1. Clone the repository:
```bash
git clone https://github.com/nlm-project/nlm.git
cd nlm
```

2. Set up your development environment:
```bash
# For development, use the build script
./setup.py

# Or manually
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
```

3. Build the project:
```bash
make -j4
```

4. Run tests:
```bash
./tools/build.py test
```

5. Run benchmarks:
```bash
./tools/build.py benchmark
```

### Code Style

- Follow the existing code style
- Use descriptive variable and function names
- Add comments for complex logic
- Ensure code compiles with warnings treated as errors

### Testing

- Write comprehensive tests for new functionality
- Ensure existing tests pass
- Run tests with different configurations
- Test edge cases and error conditions

### Documentation

- Keep documentation up-to-date
- Add docstrings for functions and classes
- Include examples in documentation
- Update README when adding new features

## Pull Request Process

1. Create a feature branch from `main`
2. Commit your changes with clear commit messages
3. Run the full test suite
4. Ensure all tests pass
5. Submit a pull request
6. Respond to review feedback

## Code Review Guidelines

- **Clarity**: Code should be clear and understandable
- **Correctness**: Code should be correct and well-tested
- **Performance**: Code should be efficient and optimized
- **Documentation**: Code should be well-documented
- **Maintainability**: Code should be easy to maintain and extend

## License

By contributing to NLM, you agree that your contributions will be licensed under the MIT License.

## Acknowledgments

We thank all contributors who have helped improve NLM. Your contributions make NLM a better project for everyone.

## Contact

For questions or issues, please:
1. Use the GitHub issue tracker
2. Join our community forum
3. Contact the maintainers
"""
        
        contrib_doc.write_text(contrib_content)
    
    def generate_all_documentation(self):
        """Generate all documentation."""
        print("Generating complete documentation set...")
        
        self.generate_documentation("all")
        
        # Create index file
        index_file = self.docs_dir / "README.md"
        index_content = """# NLM (Neural Learning Machine) Documentation

NLM (熙然) - Neural Learning Machine is an experimental artificial developmental brain project. This documentation provides comprehensive information about the system, its usage, development, and contribution guidelines.

## Quick Start

For a quick introduction to NLM, see:

- [TUTORIAL.md](TUTORIAL.md) - Quick start tutorial
- [easy_usage.md](easy_usage.md) - Simple usage guide
- [HOW_TO_USE.md](HOW_TO_USE.md) - Comprehensive usage guide

## Core Documentation

### System Architecture
- [ARCHITECTURE.md](ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions

### Usage and Examples
- [TUTORIAL.md](TUTORIAL.md) - Step-by-step tutorial
- [easy_usage.md](easy_usage.md) - Simple usage guide
- [HOW_TO_USE.md](HOW_TO_USE.md) - Comprehensive usage guide
- [examples/](examples/) - Example simulations

### Development
- [CONTRIBUTING.md](CONTRIBUTING.md) - Contribution guidelines
- [ROADMAP.md](docs/ROADMAP.md) - Development roadmap
- [LIMITATIONS.md](docs/LIMITATIONS.md) - Current limitations
- [docs/PHASE6_FINAL_AUDIT.md](docs/PHASE6_FINAL_AUDIT.md) - Phase 6 audit report

## API Reference

- [API.md](API.md) - Complete API documentation
- [python/bindings.cpp](python/bindings.cpp) - Python bindings source code

## Additional Resources

### Tools and Utilities
- [tools/](tools/) - Development tools and scripts
- [examples/](examples/) - Example simulations
- [benchmarks/](benchmarks/) - Benchmark suites

### Build System
- [CMakeLists.txt](CMakeLists.txt) - CMake configuration
- Build documentation in the build system

## Getting Help

If you need help with NLM:

1. **Documentation**: Check the documentation files
2. **GitHub Issues**: Report issues on GitHub
3. **Community**: Join the NLM community
4. **Support**: Contact the maintainers

## Support

### Reporting Issues

Please report bugs, feature requests, and other issues through the GitHub issue tracker.

### Getting Help

For general help and discussion:
- GitHub Discussions
- Community forums
- Stack Overflow (tagged with `nlm`)

### Contributing

We welcome contributions from the community! Please read the [CONTRIBUTING.md](CONTRIBUTING.md) guide before contributing.

## License

NLM is licensed under the MIT License. See [LICENSE](LICENSE) for details.

## Citation

If you use NLM in your research, please cite:

```bibtex
@misc{nlm2024,
  title={NLM (熙然) - Neural Learning Machine},
  author={NLM Research Team},
  year={2024},
  howpublished={https://github.com/nlm-project/nlm}
}
```

## Acknowledgments

NLM is developed by the NLM Research Team. We thank all contributors who have helped make NLM a successful project.

---

**Version**: 6.0.0 (Phase 6)
**Last Updated**: $(date)
**Documentation Status**: Complete
"""
        
        index_file.write_text(index_content.replace(
            "(date)", 
            time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())
        ))
        
        print("Documentation generation completed!")

# Usage example
if __name__ == "__main__":
    # Create documentation generator
    doc_generator = DocumentationGenerator()
    
    # Generate all documentation
    doc_generator.generate_all_documentation()
    
    print("Documentation generation completed successfully!")
```
        
        
        # Create setup script
        setup_script = self.tools_dir / "setup.py"
        setup_script_content = '''#!/usr/bin/env python3
"""
NLM Setup Script

This script sets up the NLM development environment.
"""

import os
import sys
import subprocess
import platform
from pathlib import Path

def run_command(command, cwd=None):
    """Run a command and check its result."""
    print(f"Running: {command}")
    try:
        subprocess.run(command, shell=True, cwd=cwd, check=True)
        return True
    except subprocess.CalledProcessError as e:
        print(f"Command failed: {e}")
        return False

def setup_environment(args):
    """Set up the development environment."""
    project_root = Path(__file__).parent.parent
    
    print("Setting up NLM development environment...")
    
    # Check for required tools
    required_tools = ["cmake", "make", "g++", "python3"]
    missing_tools = []
    
    for tool in required_tools:
        if not check_tool_availability(tool):
            missing_tools.append(tool)
    
    if missing_tools:
        print(f"Missing required tools: {missing_tools}")
        print("Please install the missing tools and try again.")
        return False
    
    # Set up Python environment
    if not setup_python_environment():
        print("Failed to set up Python environment")
        return False
    
    # Set up C++ environment
    if not setup_cpp_environment():
        print("Failed to set up C++ environment")
        return False
    
    # Set up build directory
    if not setup_build_directory():
        print("Failed to set up build directory")
        return False
    
    print("Development environment setup completed successfully!")
    return True

def check_tool_availability(tool: str) -> bool:
    """Check if a tool is available."""
    try:
        subprocess.run([tool, "--version"], 
                      capture_output=True, 
                      check=True,
                      timeout=5)
        return True
    except (subprocess.CalledProcessError, subprocess.TimeoutExpired, FileNotFoundError):
        return False

def setup_python_environment() -> bool:
    """Set up Python environment."""
    print("Setting up Python environment...")
    
    # Check if requirements.txt exists
    project_root = Path(__file__).parent.parent
    requirements_file = project_root / "requirements.txt"
    
    if not requirements_file.exists():
        print("requirements.txt not found - skipping Python package installation")
        return True
    
    # Install Python packages
    try:
        subprocess.run([
            sys.executable, "-m", "pip", "install", "--upgrade", "pip"
        ], check=True, capture_output=True)
        
        subprocess.run([
            sys.executable, "-m", "pip", "install", "-r", str(requirements_file)
        ], check=True, capture_output=True)
        
        print("Python packages installed successfully")
        return True
        
    except subprocess.CalledProcessError as e:
        print(f"Failed to install Python packages: {e}")
        return False

def setup_cpp_environment() -> bool:
    """Set up C++ development environment."""
    print("Setting up C++ development environment...")
    
    # Check for compiler
    if not check_tool_availability("g++"):
        print("g++ compiler not found")
        return False
    
    # Check for make
    if not check_tool_availability("make"):
        print("make utility not found")
        return False
    
    # Check for CMake
    if not check_tool_availability("cmake"):
        print("CMake not found")
        return False
    
    print("C++ development environment setup completed successfully")
    return True

def setup_build_directory() -> bool:
    """Set up build directory structure."""
    print("Setting up build directory structure...")
    
    project_root = Path(__file__).parent.parent
    build_dir = project_root / "build"
    
    # Create build directory
    build_dir.mkdir(parents=True, exist_ok=True)
    
    # Create CMakeLists.txt if it doesn't exist
    cmake_file = project_root / "CMakeLists.txt"
    if not cmake_file.exists():
        create_basic_cmake_lists(cmake_file)
    
    # Create .gitignore if it doesn't exist
    gitignore_file = project_root / ".gitignore"
    if not gitignore_file.exists():
        create_gitignore(gitignore_file)
    
    print("Build directory structure setup completed")
    return True

def create_basic_cmake_lists(cmake_file: Path):
    """Create basic CMakeLists.txt file."""
    content = """cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 6.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build configuration
option(BUILD_PYTHON_BINDINGS "Build Python bindings" ON)
option(BUILD_TESTS "Build tests" ON)
option(BUILD_BENCHMARKS "Build benchmarks" ON)
option(BUILD_DOCS "Build documentation" ON)

# Set optimization flags
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")
endif()

# Add source directories
add_subdirectory(src)

# Add Python bindings
if(BUILD_PYTHON_BINDINGS)
    add_subdirectory(python)
endif()

# Add tests
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Add benchmarks
if(BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()

# Installation
install(DIRECTORY src/ DESTINATION include/nlm)
install(FILES README.md DESTINATION doc)
"""
    
    cmake_file.write_text(content)

def create_gitignore(gitignore_file: Path):
    """Create .gitignore file."""
    content = """# Build directory
build/

# Compiled object files
*.o
*.ox
*.obj

# Precompiled headers
*.gch
*.pch

# Static libraries
*.a
*.lib

# Shared libraries
*.so
*.dylib
*.dll

# Executables
*.exe
*.out
*.app

# Python
__pycache__/
*.pyc
*.pyo
.pytest_cache/

# IDE
.vscode/
.idea/
*.swp
*.swo
*~

# OS
.DS_Store
.DS_Store?
._*
.Spotlight-V100
.Trashes
ehthumbs.db
Thumbs.db

# Environment variables
.env

# Temporary files
*.tmp
*.temp

# CMake
CMakeCache.txt
CMakeFiles/
CMakeScripts/

# Testing
Testing/

# Documentation
docs/
*.html
*.md5
*.pdf

# Examples
examples/

# Tools
tools/

# Build artifacts
*.o
*.obj
*.so
*.dylib
*.dll
*.exe
*.out
"""
    
    gitignore_file.write_text(content)

def main():
    import argparse
    
    parser = argparse.ArgumentParser(description="NLM Setup Script")
    subparsers = parser.add_subparsers(dest="command", help="Command to execute")
    
    # Setup command
    setup_parser = subparsers.add_parser("setup", help="Setup development environment")
    setup_parser.add_argument("--quick", action="store_true", help="Quick setup (skip optional steps)")
    
    args = parser.parse_args()
    
    if args.command == "setup":
        success = setup_environment(args)
    else:
        parser.print_help()
        success = False
    
    sys.exit(0 if success else 1)

if __name__ == "__main__":
    main()
'''
        
        setup_script.write_text(setup_script_content)
        setup_script.chmod(0o755)
        
        print("Development tools created successfully")
    
    def get_documentation_summary(self) -> Dict:
        """Get summary of documentation."""
        docs = []
        
        if (self.docs_dir / "API.md").exists():
            docs.append({
                'name': 'API Documentation',
                'path': str(self.docs_dir / "API.md"),
                'type': 'API reference'
            })
        
        if (self.docs_dir / "ARCHITECTURE.md").exists():
            docs.append({
                'name': 'Architecture Documentation',
                'path': str(self.docs_dir / "ARCHITECTURE.md"),
                'type': 'Architecture'
            })
        
        if (self.docs_dir / "TUTORIAL.md").exists():
            docs.append({
                'name': 'Tutorial Documentation',
                'path': str(self.docs_dir / "TUTORIAL.md"),
                'type': 'Tutorial'
            })
        
        return {
            'documentation_directory': str(self.docs_dir),
            'total_documents': len(docs),
            'documents': docs
        }
class IDEIntegration:
    """IDE integration for NLM development."""
    
    def __init__(self, project_root: str = "."):
        self.project_root = Path(project_root).resolve()
        self.vscode_settings_dir = self.project_root / ".vscode"
        self.clion_settings_dir = self.project_root / ".idea"
        self.eclipse_settings_dir = self.project_root / ".eclipse"
    
    def setup_vscode_integration(self):
        """Set up VS Code integration."""
        print("Setting up VS Code integration...")
        
        self.vscode_settings_dir.mkdir(parents=True, exist_ok=True)
        
        # settings.json
        settings_json = self.vscode_settings_dir / "settings.json"
        settings_content = {
            "C_Cpp.default.cppStandard": "c++20",
            "C_Cpp.intelliSense.engine": "cmake",
            "C_Cpp.intelliSense.cppPath": "${workspaceFolder}/build",
            "files.associations": {
                "*.hpp": "c++"
            },
            "C_Cpp.errorSquiggles": "enable",
            "cmake.buildOnSave": "all",
            "cmake.configurationProvider": "cmake-tools",
            "cmake.useCMakeFromPath": "true"
        }
        
        import json
        with open(settings_json, 'w') as f:
            json.dump(settings_content, f, indent=2)
        
        # tasks.json
        tasks_json = self.vscode_settings_dir / "tasks.json"
        tasks_content = {
            "version": "2.0.0",
            "tasks": [
                {
                    "label": "Build NLM",
                    "type": "shell",
                    "command": "cmake --build build --parallel 4",
                    "problemMatcher": "$cxx",
                    "group": {
                        "kind": "build",
                        "isDefault": True
                    },
                    "presentation": {
                        "reveal": "always",
                        "panel": "shared"
                    }
                },
                {
                    "label": "Run Tests",
                    "type": "shell",
                    "command": "cd build && ctest --output-on-failure",
                    "problemMatcher": "$ctest",
                    "group": "test",
                    "presentation": {
                        "reveal": "always",
                        "panel": "shared"
                    }
                },
                {
                    "label": "Configure",
                    "type": "shell",
                    "command": "cmake -B build -DCMAKE_BUILD_TYPE=Release",
                    "problemMatcher": [],
                    "group": "none",
                    "presentation": {
                        "reveal": "always",
                        "panel": "shared"
                    }
                }
            ]
        }
        
        with open(tasks_json, 'w') as f:
            json.dump(tasks_content, f, indent=2)
        
        # launch.json
        launch_json = self.vscode_settings_dir / "launch.json"
        launch_content = {
            "version": "0.2.0",
            "configurations": [
                {
                    "name": "Launch NLM Debug",
                    "type": "cppdbg",
                    "request": "launch",
                    "program": "${workspaceFolder}/build/nlm",
                    "args": [],
                    "stopAtEntry": false,
                    "cwd": "${workspaceFolder}",
                    "environmentVariables": {},
                    "externalConsole": false,
                    "MIMode": "gdb",
                    "miDebuggerPath": "gdb",
                    "setupCommands": [
                        {
                            "text": "-enable-pretty-printing",
                            "description": "Enable pretty printing for GDB"
                        }
                    ],
                    "preLaunchTask": "Build NLM"
                }
            ]
        }
        
        with open(launch_json, 'w') as f:
            json.dump(launch_content, f, indent=2)
        
        print("VS Code integration setup completed")
    
    def setup_clion_integration(self):
        """Set up CLion integration."""
        print("Setting up CLion integration...")
        
        # Create .idea/clion directory
        clion_dir = self.project_root / ".idea"
        clion_dir.mkdir(parents=True, exist_ok=True)
        
        # Create modules.xml
        modules_xml = clion_dir / "modules.xml"
        modules_content = """<?xml version="1.0" encoding="UTF-8"?>
<project version="4">
  <component name="CMakeWorkspace" url="file://$PROJECT_DIR$/build/CMakeCache.txt" name="CMakeWorkspace" pythonInterpreter="$PROJECT_DIR$/build/cmake-python-venv/bin/python" cmakeOptions="" CMakePreset="" generatorOptions="" cmakeBuild="Debug" cmakeConfigure="$PROJECT_DIR$/build" />
  <component name="CMakeWorkspaceCache">
    <option name="CMakeWorkspace" value="true" />
    <option name="UseIdeaCMake" value="true" />
  </component>
  <component name="CMakeRunDebug" active-preset="Debug" />)
</project>"""
        
        modules_xml.write_text(modules_content)
        
        # Create cpp.xml
        cpp_xml = clion_dir / "cpp.xml"
        cpp_content = """<?xml version="1.0" encoding="UTF-8"?>
<project version="4">
  <component name="AutoHeaderIncludePolicy" value="NONE" />
  <component name="CCAnalyzer">
    <option name="indexing" value="enabled" />
    <option name="cache" value="enabled" />
  </component>
  <component name="CMakeRunDebug" active-preset="Debug" />)
</project>"""
        
        cpp_xml.write_text(cpp_content)
        
        print("CLion integration setup completed")
    
    def setup_eclipse_integration(self):
        """Set up Eclipse integration."""
        print("Setting up Eclipse integration...")
        
        # Create .project file
        project_file = self.project_root / ".project"
        project_content = """<?xml version="1.0" encoding="UTF-8"?>
<projectDescription>
    <name>NLM</name>
    <comment>NLM (熙然) - Neural Learning Machine</comment>
    <projects>
    </projects>
</projectDescription>
<node>
    <id>$PROJECT_NAME$</id>
    <data>
        <wrapperLocation value="" />
        <configurationLocation value="config/cdt/cfg/cfg.xml" />
    </data>
</node>
"""
        
        project_file.write_text(project_content)
        
        # Create .cproject file
        cproject_file = self.project_root / ".cproject"
        cproject_content = """<?xml version="1.0" encoding="UTF-8"?>
<project version="4.0.2">
  <storage version="4.0.2">
      <project uid="NLM" name="NLM">
          <toolChain id="gnu-tools-linux-x86_64-gcc-9.3.0-mp-8.1.0-libstdc++6-11.0.4-sse3" type="std-gcc-linux" location="C:/tools/gcc-9.3.0/bin" name="GCC 9.3.0" description="GCC 9.3.0" version="9.3.0" standard="gnu++20">
              <compiler cxx="C:/tools/gcc-9.3.0/bin/g++.exe" c="C:/tools/gcc-9.3.0/bin/gcc.exe" />
          </toolChain>
          <buildTarget id="make" commandLine="make -j${env.NUMBER_OF_PROCESSORS}" name="Build" output="$(PROJECT_DIR)/build" workingDirectory="$(PROJECT_DIR)" type="external" autoBuild="false" autoClean="false" autoGenerateProjectFiles="false" useDefaultOutputDirs="false" allowOverride="false" />
          <buildTarget id="cmake" commandLine="cmake -B build -DCMAKE_BUILD_TYPE=Release" name="Configure" output="$(PROJECT_DIR)/build" workingDirectory="$(PROJECT_DIR)" type="external" autoBuild="false" autoClean="false" autoGenerateProjectFiles="false" allowOverride="false" />
      </project>
  </storage>
</project>
"""
        
        cproject_file.write_text(cproject_content)
        
        print("Eclipse integration setup completed")
    
    def create_ide_config(self) -> Dict:
        """Create IDE configuration."""
        return {
            'vscode': {
                'enabled': True,
                'settings_file': 'vscode/settings.json',
                'tasks_file': 'vscode/tasks.json',
                'launch_file': 'vscode/launch.json'
            },
            'clion': {
                'enabled': True,
                'modules_file': '.idea/modules.xml',
                'cpp_file': '.idea/cpp.xml'
            },
            'eclipse': {
                'enabled': True,
                'project_file': '.project',
                'cproject_file': '.cproject'
            },
            'common_commands': {
                'setup': './tools/setup.py',
                'build': './tools/build.py build',
                'test': './tools/build.py test',
                'benchmark': './tools/build.py benchmark',
                'docs': './tools/docs.sh',
                'format': './tools/format.sh'
            }
        }

# Main function
def main():
    """Main function for the build system."""
    import argparse
    import json
    import time
    
    parser = argparse.ArgumentParser(description="NLM Build System")
    subparsers = parser.add_subparsers(dest="command", help="Command to execute")
    
    # Configure command
    configure_parser = subparsers.add_parser("configure", help="Configure the build system")
    configure_parser.add_argument("--build-dir", default="build", help="Build directory")
    configure_parser.add_argument("--build-type", choices=["Release", "Debug", "RelWithDebInfo", "MinSizeRel"], default="Release", help="Build type")
    configure_parser.add_argument("--parallel", action="store_true", help="Enable parallel builds")
    configure_parser.add_argument("--tests", action="store_true", help="Enable tests")
    configure_parser.add_argument("--benchmarks", action="store_true", help="Enable benchmarks")
    configure_parser.add_argument("--python-bindings", action="store_true", help="Enable Python bindings")
    
    # Build command
    build_parser = subparsers.add_parser("build", help="Build the project")
    build_parser.add_argument("--build-dir", default="build", help="Build directory")
    build_parser.add_argument("--target", help="Build target")
    
    # Test command
    test_parser = subparsers.add_parser("test", help="Run tests")
    test_parser.add_argument("--build-dir", default="build", help="Build directory")
    
    # Benchmark command
    benchmark_parser = subparsers.add_parser("benchmark", help="Run benchmarks")
    benchmark_parser.add_argument("--build-dir", default="build", help="Build directory")
    
    # Install command
    install_parser = subparsers.add_parser("install", help="Install the project")
    install_parser.add_argument("--install-dir", default="install", help="Install directory")
    
    # Clean command
    clean_parser = subparsers.add_parser("clean", help="Clean build artifacts")
    clean_parser.add_argument("--all", action="store_true", help="Clean all artifacts")
    
    # Environment setup command
    env_parser = subparsers.add_parser("env-setup", help="Setup development environment")
    env_parser.add_argument("--quick", action="store_true", help="Quick setup")
    
    # Documentation command
    docs_parser = subparsers.add_parser("docs", help="Generate documentation")
    docs_parser.add_argument("--type", choices=["all", "api", "architecture", "tutorials", "examples"], default="all", help="Documentation type")
    
    # Workflow command
    workflow_parser = subparsers.add_parser("workflow", help="Setup development workflow")
    workflow_parser.add_argument("--ci", action="store_true", help="Setup CI/CD pipeline")
    workflow_parser.add_argument("--git-hooks", action="store_true", help="Setup Git hooks")
    
    # IDE command
    ide_parser = subparsers.add_parser("ide", help="Setup IDE integration")
    ide_parser.add_argument("--vscode", action="store_true", help="Setup VS Code integration")
    ide_parser.add_argument("--clion", action="store_true", help="Setup CLion integration")
    ide_parser.add_argument("--eclipse", action="store_true", help="Setup Eclipse integration")
    
    args = parser.parse_args()
    
    if args.command == "configure":
        # Initialize build system
        build_system = BuildSystem()
        
        # Load existing configuration
        build_system.load_config()
        
        # Update configuration based on arguments
        if args.build_type:
            build_system.config['build_type'] = BuildConfiguration(args.build_type.lower())
        
        build_system.config['parallel_build'] = args.parallel
        build_system.config['enable_tests'] = args.tests
        build_system.config['enable_benchmarks'] = args.benchmarks
        build_system.config['python_bindings'] = args.python_bindings
        
        # Save configuration
        build_system.save_config()
        
        # Configure build
        if build_system.configure(args.build_dir):
            print("Build system configuration completed successfully!")
            
            # Generate build info
            info = build_system.get_build_info()
            print("Build configuration summary:")
            for key, value in info.items():
                print(f"  {key}: {value}")
        
    elif args.command == "build":
        # Initialize build system
        build_system = BuildSystem()
        
        # Load existing configuration
        build_system.load_config()
        
        # Build project
        targets = [args.target] if args.target else None
        if build_system.build(args.build_dir, targets):
            print("Build completed successfully!")
        
    elif args.command == "test":
        # Initialize build system
        build_system = BuildSystem()
        
        # Run tests
        if build_system.run_tests(args.build_dir):
            print("Tests completed successfully!")
        
    elif args.command == "benchmark":
        # Initialize build system
        build_system = BuildSystem()
        
        # Run benchmarks
        if build_system.run_benchmarks():
            print("Benchmarks completed successfully!")
        
    elif args.command == "install":
        # Initialize build system
        build_system = BuildSystem()
        
        # Install project
        install_dir = args.install_dir
        if build_system.install(install_dir):
            print("Installation completed successfully!")
        
    elif args.command == "clean":
        # Initialize build system
        build_system = BuildSystem()
        
        # Clean build artifacts
        build_system.clean(args.all)
        
        print("Clean completed successfully!")
        
    elif args.command == "env-setup":
        # Initialize environment manager
        env_manager = EnvironmentManager()
        
        # Setup development environment
        if env_manager.setup_development_environment(auto_detect=True):
            print("Development environment setup completed successfully!")
        
    elif args.command == "docs":
        # Initialize documentation generator
        doc_generator = DocumentationGenerator()
        
        # Generate documentation
        if doc_generator.generate_documentation(args.type):
            print("Documentation generation completed successfully!")
        
    elif args.command == "workflow":
        # Initialize workflow manager
        workflow = DevelopmentWorkflow()
        
        # Setup CI/CD pipeline
        if args.ci:
            workflow.setup_ci_pipeline()
        
        # Setup Git hooks
        if args.git_hooks:
            workflow.setup_git_hooks()
        
        print("Workflow setup completed successfully!")
        
    elif args.command == "ide":
        # Initialize IDE integration
        ide = IDEIntegration()
        
        # Setup IDE integrations
        if args.vscode:
            ide.setup_vscode_integration()
        
        if args.clion:
            ide.setup_clion_integration()
        
        if args.eclipse:
            ide.setup_eclipse_integration()
        
        print("IDE integration setup completed successfully!")
        
    else:
        parser.print_help()
if __name__ == "__main__":
    main()
```

## Conclusion

The enhanced NLM build system and developer experience provides:

### Core Capabilities

1. **Modern Build System**: CMake-based build system with comprehensive configuration management
2. **Development Environment Setup**: Automated environment setup and dependency management
3. **CI/CD Integration**: Continuous integration and deployment pipeline setup
4. **Testing Framework**: Automated testing and benchmark execution
5. **IDE Integration**: Integration with popular IDEs (VS Code, CLion, Eclipse)
6. **Documentation Generation**: Comprehensive documentation generation
7. **Developer Tools**: Development scripts and utilities
8. **Workflow Automation**: Development workflow automation and management

### Key Benefits

- **Productivity**: Streamlined development workflow and automation
- **Consistency**: Standardized build and development process
- **Collaboration**: Easy team collaboration with shared configurations
- **Maintainability**: Well-organized project structure and documentation
- **Scalability**: Scalable build system for large projects
- **Integration**: Seamless integration with modern development tools
- **Flexibility**: Configurable build options and environments

### Usage Examples

```bash
# Configure the build system
./tools/setup.py --help
./tools/build.py configure --build-dir build --release

# Build the project
./tools/build.py build

# Run tests
./tools/build.py test

# Generate documentation
./tools/build.py docs

# Setup development workflow
./tools/build.py workflow --ci --git-hooks

# Setup IDE integration
./tools/build.py ide --vscode --clion
```

These tools provide a comprehensive development experience for NLM Phase 6, making it easier to build, test, and maintain the neural simulation framework. 🚀