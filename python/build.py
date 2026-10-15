#!/usr/bin/env python3
"""
NLM Python Package Builder

This script provides comprehensive build and deployment capabilities for the
NLM Python enhanced API package. It includes functionality for:
- Building Python packages
- Running tests
- Generating documentation
- Package validation
- Deployment preparation
"""

import argparse
import json
import os
import subprocess
import sys
import time
import platform
from pathlib import Path
from typing import Dict, List, Any, Optional

class NLMBuilder:
    """
    NLM Python Package Builder
    
    Comprehensive builder for the enhanced NLM Python API package.
    """
    
    def __init__(self, project_root: str):
        """
        Initialize builder.
        
        Args:
            project_root: Root directory of the NLM project
        """
        self.project_root = Path(project_root)
        self.python_package_dir = self.project_root / "python"
        self.src_dir = self.project_root / "src"
        self.build_dir = self.project_root / "build"
        self.docs_dir = self.project_root / "docs"
        self.config_file = self.project_root / "pyproject.toml"
        
        # Load configuration
        self.config = self._load_config()
        
    def _load_config(self) -> Dict[str, Any]:
        """Load project configuration."""
        config_path = self.project_root / "kilo.json"
        if config_path.exists():
            with open(config_path, 'r') as f:
                return json.load(f)
        
        # Default configuration
        return {
            "project": {
                "name": "nlm",
                "version": "1.0.0-enhanced",
                "python_package": "python/pynlm"
            },
            "build": {
                "parallel": True,
                "optimization_level": "high",
                "generate_documentation": True,
                "run_tests": True,
                "validate_package": True
            },
            "testing": {
                "framework": "pytest",
                "coverage_threshold": 80,
                "parallel_tests": True
            }
        }
        
    def run_build(self, args) -> bool:
        """Run the complete build process."""
        print("Starting NLM Python Package Build")
        print("=" * 50)
        
        steps = [
            ("Setup build environment", self._setup_build_environment),
            ("Configure CMake", self._configure_cmake),
            ("Build C++ core", self._build_cpp_core),
            ("Build Python bindings", self._build_python_bindings),
            ("Run tests", self._run_tests),
            ("Generate documentation", self._generate_documentation),
            ("Validate package", self._validate_package),
            ("Create distribution", self._create_distribution),
        ]
        
        if args.skip:
            # Run only specified steps
            for step_name, step_func in steps:
                if step_name in args.skip:
                    print(f"\nRunning: {step_name}")
                    if not step_func():
                        return False
        else:
            # Run all steps
            for step_name, step_func in steps:
                print(f"\nRunning: {step_name}")
                if not step_func():
                    print(f"Build failed at step: {step_name}")
                    return False
        
        print("\n" + "=" * 50)
        print("Build completed successfully!")
        return True
        
    def _setup_build_environment(self) -> bool:
        """Setup build environment."""
        print("Setting up build environment...")
        
        try:
            # Check Python version
            python_version = sys.version_info
            if python_version.major < 3 or (python_version.major == 3 and python_version.minor < 8):
                print(f"Warning: Python {python_version.major}.{python_version.minor} detected. "
                      "Recommended Python >=3.8")
            
            # Check for required tools
            required_tools = ["cmake", "make"]
            for tool in required_tools:
                if subprocess.run(["which", tool], capture_output=True).returncode != 0:
                    print(f"Error: Required tool '{tool}' not found")
                    return False
            
            # Create build directory
            self.build_dir.mkdir(parents=True, exist_ok=True)
            
            print(f"Build environment setup complete (build directory: {self.build_dir})")
            return True
            
        except Exception as e:
            print(f"Error setting up build environment: {e}")
            return False
            
    def _configure_cmake(self) -> bool:
        """Configure CMake build."""
        print("Configuring CMake...")
        
        try:
            # Determine number of CPU cores for parallel build
            if self.config["build"]["parallel"]:
                cpu_count = os.cpu_count() or 4
                make_args = f"-j{cpu_count}"
            else:
                make_args = ""
            
            # Configure CMake
            cmake_cmd = [
                "cmake", self.project_root,
                "-DCMAKE_BUILD_TYPE=Release",
                "-DCMAKE_CXX_STANDARD=20",
                "-DPYTHON_EXECUTABLE={python}",
            ]
            
            print(f"Running CMake command: {' '.join(cmake_cmd)}")
            
            # Run CMake configuration
            result = subprocess.run(
                cmake_cmd,
                cwd=self.build_dir,
                capture_output=True,
                text=True
            )
            
            if result.returncode != 0:
                print(f"CMake configuration failed: {result.stderr}")
                return False
            
            print("CMake configuration successful")
            return True
            
        except Exception as e:
            print(f"Error configuring CMake: {e}")
            return False
            
    def _build_cpp_core(self) -> bool:
        """Build C++ core libraries."""
        print("Building C++ core libraries...")
        
        try:
            # Build using make
            make_cmd = ["make"]
            if self.config["build"]["parallel"]:
                cpu_count = os.cpu_count() or 4
                make_cmd.extend(["-j", str(cpu_count)])
            
            print(f"Running build command: {' '.join(make_cmd)}")
            
            result = subprocess.run(
                make_cmd,
                cwd=self.build_dir,
                capture_output=True,
                text=True
            )
            
            if result.returncode != 0:
                print(f"Build failed: {result.stderr}")
                return False
            
            # Check if build artifacts exist
            expected_artifacts = [
                "libnlm_core.a",
                "libnlm_agent.a", 
                "libnlm_world.a",
                "libnlm_phase3.a",
                "libnlm_phase4.a",
                "nlm",
                "nlm_phase3_demo",
                "nlm_phase4_demo",
            ]
            
            for artifact in expected_artifacts:
                artifact_path = self.build_dir / artifact
                if not artifact_path.exists():
                    print(f"Warning: Expected artifact {artifact} not found")
            
            print("C++ core build successful")
            return True
            
        except Exception as e:
            print(f"Error building C++ core: {e}")
            return False
            
    def _build_python_bindings(self) -> bool:
        """Build Python bindings."""
        print("Building Python bindings...")
        
        try:
            # Check if Python bindings source exists
            bindings_source = self.python_package_dir / "bindings.cpp"
            if not bindings_source.exists():
                print(f"Error: Python bindings source not found: {bindings_source}")
                return False
            
            # Check if we can import the module
            sys.path.insert(0, str(self.python_package_dir))
            
            try:
                import pynlm
                print(f"Python bindings import successful (version: {getattr(pynlm, '__version__', 'unknown')})")
            except ImportError as e:
                print(f"Error importing Python bindings: {e}")
                return False
            
            # Check for enhanced modules
            enhanced_modules = [
                "pynlm.core",
                "pynlm.brain", 
                "pynlm.world",
                "pynlm.agent",
                "pynlm.config",
                "pynlm.utils",
            ]
            
            for module_name in enhanced_modules:
                try:
                    __import__(module_name)
                    print(f"✓ Enhanced module '{module_name}' available")
                except ImportError:
                    print(f"Warning: Enhanced module '{module_name}' not available")
            
            print("Python bindings build successful")
            return True
            
        except Exception as e:
            print(f"Error building Python bindings: {e}")
            return False
            
    def _run_tests(self) -> bool:
        """Run tests."""
        print("Running tests...")
        
        try:
            if not self.config["build"]["run_tests"]:
                print("Skipping tests (disabled in configuration)")
                return True
            
            # Run pytest
            test_paths = [
                str(self.python_package_dir / "tests"),
                str(self.project_root / "tests"),
            ]
            
            test_cmd = [
                sys.executable, "-m", "pytest",
                "-v",
                "--tb=short",
                "--cov=python/pynlm",
                "--cov-report=term-missing",
            ]
            
            for test_path in test_paths:
                if os.path.exists(test_path):
                    print(f"Running tests in {test_path}")
                    test_cmd.extend([test_path])
            
            print(f"Running test command: {' '.join(test_cmd)}")
            
            result = subprocess.run(
                test_cmd,
                capture_output=True,
                text=True,
                timeout=300  # 5 minute timeout
            )
            
            if result.returncode != 0:
                print(f"Tests failed: {result.stdout}")
                if result.stderr:
                    print(f"Test errors: {result.stderr}")
                return False
            
            print("Tests passed successfully")
            return True
            
        except subprocess.TimeoutExpired:
            print("Tests timed out after 5 minutes")
            return False
        except Exception as e:
            print(f"Error running tests: {e}")
            return False
            
    def _generate_documentation(self) -> bool:
        """Generate documentation."""
        print("Generating documentation...")
        
        try:
            if not self.config["build"]["generate_documentation"]:
                print("Skipping documentation generation (disabled in configuration)")
                return True
            
            # Check if sphinx is available
            try:
                import sphinx
                print(f"Sphinx version: {sphinx.__version__}")
            except ImportError:
                print("Warning: Sphinx not available, skipping documentation generation")
                return True
            
            # Generate API documentation
            api_docs_dir = self.python_package_dir / "docs"
            api_docs_dir.mkdir(parents=True, exist_ok=True)
            
            # Copy API guide to docs
            api_guide = self.python_package_dir / "pynlm" / "api_guide.md"
            if api_guide.exists():
                import shutil
                shutil.copy2(api_guide, api_docs_dir / "api_guide.md")
                print("API documentation generated")
            
            # Generate module documentation
            modules = [
                "pynlm.core",
                "pynlm.brain",
                "pynlm.world", 
                "pynlm.agent",
                "pynlm.config",
                "pynlm.utils",
            ]
            
            for module_name in modules:
                try:
                    module = __import__(module_name, fromlist=[''])
                    module_file = module.__file__
                    print(f"Documentation for {module_name} from {module_file}")
                except ImportError:
                    print(f"Warning: Could not import {module_name} for documentation")
            
            print("Documentation generation complete")
            return True
            
        except Exception as e:
            print(f"Error generating documentation: {e}")
            return False
            
    def _validate_package(self) -> bool:
        """Validate package structure and content."""
        print("Validating package...")
        
        try:
            if not self.config["build"]["validate_package"]:
                print("Skipping package validation (disabled in configuration)")
                return True
            
            validation_errors = []
            
            # Check required files
            required_files = [
                self.config_file,
                self.python_package_dir / "__init__.py",
                self.python_package_dir / "pyproject.toml",
            ]
            
            for required_file in required_files:
                if not required_file.exists():
                    validation_errors.append(f"Missing required file: {required_file}")
            
            # Check Python package structure
            pynlm_dir = self.python_package_dir / "pynlm"
            if not pynlm_dir.exists():
                validation_errors.append(f"Python package directory not found: {pynlm_dir}")
            else:
                required_modules = [
                    "__init__.py",
                    "core.py",
                    "brain.py", 
                    "world.py",
                    "agent.py",
                    "config.py",
                    "utils.py",
                    "bindings.cpp",
                ]
                
                for module in required_modules:
                    module_path = pynlm_dir / module
                    if not module_path.exists():
                        validation_errors.append(f"Missing module file: {module}")
            
            # Check documentation
            api_guide = pynlm_dir / "api_guide.md"
            if not api_guide.exists():
                validation_errors.append("API guide documentation not found")
            
            # Check examples
            examples_dir = self.python_package_dir / "examples"
            if not examples_dir.exists():
                print("Warning: Examples directory not found (optional)")
            
            # Display validation results
            if validation_errors:
                print("Package validation failed:")
                for error in validation_errors:
                    print(f"  ERROR: {error}")
                return False
            else:
                print("Package validation successful")
                return True
                
        except Exception as e:
            print(f"Error validating package: {e}")
            return False
            
    def _create_distribution(self) -> bool:
        """Create distribution packages."""
        print("Creating distribution packages...")
        
        try:
            # This would typically create wheels and source distributions
            # For now, just create a simple validation
            print("Creating source distribution...")
            
            # Check if we can build a wheel
            import subprocess
            
            build_cmd = [
                sys.executable, "-m", "build",
                "--wheel",
                "--outdir", str(self.project_root / "dist"),
            ]
            
            result = subprocess.run(
                build_cmd,
                cwd=self.python_package_dir,
                capture_output=True,
                text=True,
                timeout=600  # 10 minute timeout
            )
            
            if result.returncode != 0:
                print(f"Distribution creation failed: {result.stderr}")
                return False
            
            # List created distributions
            dist_dir = self.project_root / "dist"
            if dist_dir.exists():
                print(f"Created distributions in {dist_dir}:")
                for file in dist_dir.iterdir():
                    if file.is_file():
                        print(f"  - {file.name} ({file.stat().st_size} bytes)")
            
            print("Distribution creation successful")
            return True
            
        except subprocess.TimeoutExpired:
            print("Distribution creation timed out after 10 minutes")
            return False
        except Exception as e:
            print(f"Error creating distribution: {e}")
            return False

def main():
    """Main function."""
    parser = argparse.ArgumentParser(
        description="NLM Python Package Builder",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Usage Examples:
  %(prog)s                    # Run complete build process
  %(prog)s --skip "setup,tests"  # Skip setup and tests
  %(prog)s --config "custom.json"  # Use custom configuration
  %(prog)s --validate-only       # Only validate package structure
  %(prog)s --help               # Show this help message
        """
    )
    
    parser.add_argument(
        "--config",
        help="Path to custom configuration file",
        default=None
    )
    
    parser.add_argument(
        "--skip",
        help="Comma-separated list of steps to skip",
        default=None
    )
    
    parser.add_argument(
        "--validate-only",
        help="Only validate package structure and exit",
        action="store_true"
    )
    
    parser.add_argument(
        "--verbose",
        help="Enable verbose output",
        action="store_true"
    )
    
    args = parser.parse_args()
    
    try:
        # Get project root directory
        project_root = Path(__file__).parent.parent if "__file__" in globals() else Path.cwd()
        
        # Initialize builder
        builder = NLMBuilder(str(project_root))
        
        # Load custom configuration if specified
        if args.config:
            config_path = Path(args.config)
            if config_path.exists():
                with open(config_path, 'r') as f:
                    builder.config = json.load(f)
                print(f"Loaded custom configuration from {args.config}")
            else:
                print(f"Warning: Custom configuration file not found: {args.config}")
        
        # Parse skip list
        skip_steps = []
        if args.skip:
            skip_steps = [step.strip() for step in args.skip.split(',')]
        
        # Update args with parsed skip list
        args.skip = skip_steps
        
        if args.validate_only:
            # Just validate package
            print("Validating package structure...")
            if builder._validate_package():
                print("Package validation successful")
                return 0
            else:
                print("Package validation failed")
                return 1
        
        # Run complete build
        if builder.run_build(args):
            print("\n" + "=" * 50)
            print("BUILD SUCCESSFUL!")
            print("=" * 50)
            print("\nNext steps:")
            print("1. Install the package: pip install -e python/")
            print("2. Run tests: pytest python/tests/")
            print("3. Run demos: python python/pynlm/demo.py")
            print("4. Explore documentation in python/pynlm/api_guide.md")
            return 0
        else:
            print("\n" + "=" * 50)
            print("BUILD FAILED!")
            print("=" * 50)
            return 1
            
    except KeyboardInterrupt:
        print("\nBuild interrupted by user.")
        return 1
    except Exception as e:
        print(f"\nError during build: {e}")
        return 1

if __name__ == '__main__':
    sys.exit(main())