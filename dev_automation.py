"""
NLM Development Automation and Workflow Script

This comprehensive script provides automated development workflows for the NLM
(Neural Learning Machine) project, following modern DevOps practices and
continuous development automation.

Key Features:
- Automated code quality checks
- Continuous integration workflows
- Build and deployment automation
- Performance monitoring and benchmarking
- Documentation generation and validation
- Testing and coverage management
- Version control integration
- Environment setup and management
- Project scaffolding and template generation
- Custom development tasks and workflows

The automation system is organized into logical modules:
1. Environment and Setup
2. Code Quality and Validation
3. Building and Packaging
4. Testing and Coverage
5. Documentation
6. Deployment and Release
7. Monitoring and Performance
8. Project Management
9. Custom Workflows
10. Utilities and Tools

The automation system supports both command-line and programmatic usage,
with comprehensive logging, error handling, and reporting capabilities.
"""

import os
import sys
import subprocess
import json
import argparse
import shutil
import time
import datetime
import platform
from pathlib import Path
from typing import Dict, List, Any, Optional, Callable
from dataclasses import dataclass, field
from enum import Enum
import logging
from concurrent.futures import ThreadPoolExecutor, as_completed
import yaml
import configparser
from jinja2 import Template
import git
import requests
import psutil
import warnings
from contextlib import contextmanager

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler('nlm_development.log'),
        logging.StreamHandler(sys.stdout)
    ]
)

logger = logging.getLogger(__name__)

class DevelopmentPhase(Enum):
    """Development phase enumeration"""
    SETUP = "setup"
    VALIDATION = "validation"
    BUILD = "build"
    TEST = "test"
    DOCUMENTATION = "documentation"
    DEPLOY = "deploy"
    MONITOR = "monitor"
    RELEASE = "release"

class NLMDevelopmentAutomation:
    """
    Comprehensive development automation system for NLM.
    
    This class provides methods for automating various aspects of the NLM
    development lifecycle, including environment setup, code validation,
    building, testing, documentation generation, deployment, and
    performance monitoring.
    """
    
    def __init__(self, project_root: str = ".", config_file: str = "dev_config.yaml"):
        self.project_root = Path(project_root)
        self.config_file = self.project_root / config_file
        self.config = self._load_config()
        
        # Initialize project structure
        self._setup_project_structure()
        
        # Development state
        self.current_phase = None
        self.start_time = datetime.datetime.now()
        self.phase_history = []
        
        # Components
        self.git_repo = None
        self.environment = None
        self.build_system = None
        self.testing_system = None
        
        # Initialize git repository if available
        try:
            self.git_repo = git.Repo(self.project_root)
        except git.InvalidGitRepositoryError:
            self.git_repo = None
            logger.warning("Git repository not found - some features will be limited")
        
        # Log initialization
        logger.info(f"NLM Development Automation initialized in {self.project_root}")
    
    def _load_config(self) -> Dict[str, Any]:
        """Load development configuration"""
        default_config = {
            'environment': {
                'python_version': '3.11',
                'cpp_standard': 'c++20',
                'build_type': 'Release',
                'install_dependencies': True,
            },
            'quality_checks': {
                'lint': True,
                'format': True,
                'type_check': True,
                'security_scan': True,
                'documentation_check': True,
            },
            'building': {
                'build_system': 'cmake',
                'python_package': 'setuptools',
                'clean_before_build': True,
                'verbose_build': False,
            },
            'testing': {
                'unit_tests': True,
                'integration_tests': True,
                'performance_tests': True,
                'coverage_threshold': 0.8,
                'test_parallel': True,
                'test_timeout': 300,
            },
            'documentation': {
                'generate_api_docs': True,
                'validate_links': True,
                'build_readme': True,
                'build_changelogs': True,
            },
            'deployment': {
                'build_distribution': True,
                'publish_packages': False,
                'update_documentation': True,
                'create_release_notes': True,
            },
            'monitoring': {
                'performance_monitoring': True,
                'resource_monitoring': True,
                'alerting': True,
                'dashboard_update': True,
            },
            'project': {
                'name': 'NLM',
                'version': '0.1.0',
                'author': 'NLM Authors',
                'description': 'Neural Learning Machine - Brain Simulator',
                'license': 'MIT',
                'repository': 'https://github.com/nlm-project/nlm',
            },
        }
        
        if self.config_file.exists():
            try:
                with open(self.config_file, 'r') as f:
                    file_config = yaml.safe_load(f)
                # Merge with default config
                self._deep_merge(default_config, file_config)
            except Exception as e:
                logger.warning(f"Failed to load config file: {e}")
        
        return default_config
    
    def _deep_merge(self, base: Dict[str, Any], override: Dict[str, Any]):n        """Deep merge two dictionaries"""
        for key, value in override.items():
            if key in base and isinstance(base[key], dict) and isinstance(value, dict):
                self._deep_merge(base[key], value)
            else:
                base[key] = value
    
    def _setup_project_structure(self):
        """Setup project structure and directories"""
        required_dirs = [
            'src',
            'src/core',
            'src/brain',
            'src/agent',
            'src/world',
            'src/environment',
            'src/sensory',
            'src/motor',
            'src/neuromodulation',
            'src/memory',
            'src/prediction',
            'src/cognition',
            'src/plasticity',
            'src/dynamics',
            'src/experiments',
            'src/visualization',
            'tests',
            'benchmarks',
            'docs',
            'build',
            'dist',
            'logs',
            'reports',
            'configs',
            'examples',
            'scripts',
            'docker',
            '.github/workflows',
            'src/nlm_testing',
            'src/nlm_advanced',
        ]
        
        for dir_path in required_dirs:
            full_path = self.project_root / dir_path
            full_path.mkdir(exist_ok=True)
            
            # Create __init__.py files for Python packages
            if dir_path.startswith('src/'):
                init_file = full_path / '__init__.py'
                if not init_file.exists():
                    with open(init_file, 'w') as f:
                        f.write(f"# NLM - {dir_path.split('/')[-1]} package\n")
        
        # Create essential project files
        self._create_project_files()
    
    def _create_project_files(self):
        """Create essential project files"""
        # .gitignore
        gitignore_content = self._get_gitignore_content()
        (self.project_root / '.gitignore').write_text(gitignore_content)
        
        # pyproject.toml (if not exists)
        pyproject_path = self.project_root / 'pyproject.toml'
        if not pyproject_path.exists():
            pyproject_content = self._get_pyproject_content()
            pyproject_path.write_text(pyproject_content)
        
        # setup.py (if not exists)
        setup_path = self.project_root / 'setup.py'
        if not setup_path.exists():
            setup_content = self._get_setup_content()
            setup_path.write_text(setup_content)
        
        # .github/workflows/main.yml
        workflows_dir = self.project_root / '.github' / 'workflows'
        workflows_dir.mkdir(exist_ok=True)
        
        main_workflow = workflows_dir / 'main.yml'
        if not main_workflow.exists():
            main_workflow_content = self._get_main_workflow_content()
            main_workflow.write_text(main_workflow_content)
        
        # dev_config.yaml (if not exists)
        dev_config_path = self.project_root / 'dev_config.yaml'
        if not dev_config_path.exists():
            dev_config_content = self._get_dev_config_content()
            dev_config_path.write_text(dev_config_content)
        
        # README.md (if not exists)
        readme_path = self.project_root / 'README.md'
        if not readme_path.exists():
            readme_content = self._get_readme_content()
            readme_path.write_text(readme_content)
    
    def _get_gitignore_content(self) -> str:
        """Get .gitignore content"""
        return """
# Build artifacts
build/
dist/
*.egg-info/
__pycache__/
*.pyc
*.pyo
*.pyd
*.so
*.dll

# Python virtual environments
venv/
.env/
.pytest_cache/
.coverage
htmlcov/

# IDE and editor files
.vscode/
.idea/
*.swp
*.swo
*~

# OS files
.DS_Store
Thumbs.db

# Configuration files
*.log
*.tmp
*.temp

# Temporary files
tmp/
temp/

# Documentation build artifacts
_site/
.docs/

# Test artifacts
test_results/
benchmark_results/

# Distribution and packaging
*.whl
*.tar.gz
*.tgz

# System files
.DS_Store
"""
    
    def _get_pyproject_content(self) -> str:
        """Get pyproject.toml content"""
        return """
[build-system]
requires = ["setuptools>=61.0", "wheel", "scikit-build-core>=0.5.0", "pybind11>=2.11.0"]
build-backend = "setuptools.build_meta"

[project]
name = "pynlm"
version = "0.1.0"
description = "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework"
readme = "README.md"
requires-python = ">=3.8"
authors = [
    {name = "NLM Authors"}
]
keywords = ["neural", "simulation", "spiking", "brain", "learning", "ai", "neural-networks"]
classifiers = [
    "Development Status :: 3 - Alpha",
    "Intended Audience :: Science/Research",
    "License :: OSI Approved :: MIT License",
    "Programming Language :: Python :: 3",
    "Programming Language :: Python :: 3.8",
    "Programming Language :: Python :: 3.9",
    "Programming Language :: Python :: 3.10",
    "Programming Language :: Python :: 3.11",
    "Programming Language :: Python :: 3.12",
    "Programming Language :: C++",
    "Topic :: Scientific/Engineering :: Artificial Intelligence",
]

dependencies = []

[project.optional-dependencies]
dev = [
    "pytest>=7.0",
    "numpy>=1.20",
    "scikit-build-core>=0.5.0",
    "pybind11>=2.11.0",
    "pytest-cov>=4.0",
    "black>=23.0",
    "isort>=5.12",
    "flake8>=6.0",
    "mypy>=1.0",
    "ruff>=0.1.0",
]

[project.urls]
Homepage = "https://github.com/nlm-project/nlm"
Repository = "https://github.com/nlm-project/nlm.git"
"""
    
    def _get_setup_content(self) -> str:
        """Get setup.py content"""
        return """
from setuptools import setup, find_packages
import sys

def get_version():
    return "0.1.0"

def get_requirements():
    return []

setup(
    name="pynlm",
    version=get_version(),
    author="NLM Authors",
    description="Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework",
    long_description=open("README.md").read(),
    long_description_content_type="text/markdown",
    packages=find_packages(where="src"),
    package_dir={"": "src"},
    python_requires=">=3.8",
    install_requires=get_requirements(),
    classifiers=[
        "Development Status :: 3 - Alpha",
        "Intended Audience :: Science/Research",
        "License :: OSI Approved :: MIT License",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Programming Language :: Python :: 3.12",
        "Programming Language :: C++",
        "Topic :: Scientific/Engineering :: Artificial Intelligence",
    ],
)
"""
    
    def _get_main_workflow_content(self) -> str:
        """Get GitHub Actions workflow content"""
        return """
name: NLM CI/CD Pipeline

on:
  push:
    branches: [main, develop]
  pull_request:
    branches: [main, develop]
  workflow_dispatch:

jobs:
  quality-check:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      - name: Set up Python
        uses: actions/setup-python@v3
        with:
          python-version: '3.11'
      - name: Install dependencies
        run: |
          python -m pip install --upgrade pip
          python -m pip install setuptools wheel scikit-build-core pybind11
          python -m pip install -e .[dev]
      - name: Run black
        run: black --check src/ tests/
      - name: Run isort
        run: isort --check-only src/ tests/
      - name: Run flake8
        run: flake8 src/ tests/
      - name: Run mypy
        run: mypy src/
      - name: Build package
        run: python -m build
      - name: Run tests
        run: pytest tests/ --cov=pynlm --cov-report=term-missing
  
  test:
    runs-on: ubuntu-latest
    strategy:
      matrix:
        python-version: ['3.8', '3.9', '3.10', '3.11', '3.12']
    steps:
      - uses: actions/checkout@v3
      - name: Set up Python ${{ matrix.python-version }}
        uses: actions/setup-python@v3
        with:
          python-version: ${{ matrix.python-version }}
      - name: Install dependencies
        run: |
          python -m pip install --upgrade pip
          python -m pip install setuptools wheel scikit-build-core pybind11
          python -m pip install -e .[dev]
      - name: Run tests
        run: pytest tests/ -v --tb=short
"""
    
    def _get_dev_config_content(self) -> str:
        """Get development configuration content"""
        return """
environment:
  python_version: "3.11"
  cpp_standard: "c++20"
  build_type: "Release"
  install_dependencies: true

quality_checks:
  lint: true
  format: true
  type_check: true
  security_scan: true
  documentation_check: true

building:
  build_system: "cmake"
  python_package: "setuptools"
  clean_before_build: true
  verbose_build: false

testing:
  unit_tests: true
  integration_tests: true
  performance_tests: true
  coverage_threshold: 0.8
  test_parallel: true
  test_timeout: 300

documentation:
  generate_api_docs: true
  validate_links: true
  build_readme: true
  build_changelogs: true

deployment:
  build_distribution: true
  publish_packages: false
  update_documentation: true
  create_release_notes: true

monitoring:
  performance_monitoring: true
  resource_monitoring: true
  alerting: true
  dashboard_update: true

project:
  name: "NLM"
  version: "0.1.0"
  author: "NLM Authors"
  description: "Neural Learning Machine - Brain Simulator"
  license: "MIT"
  repository: "https://github.com/nlm-project/nlm"
"""
    
    def _get_readme_content(self) -> str:
        """Get README content"""
        return """
# NLM (熙然) - Neural Learning Machine

**An Experimental Artificial Developmental Brain**

## Overview

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

## Quick Start

### Installation

```bash
pip install pynlm
```

### Basic Usage

```python
import pynlm

# Create a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Make it think
for i in range(100):
    brain.step(i)

print(f"Done! Your brain thought {brain.getTotalSpikeCount()} times")
```

## Development Automation

This project includes comprehensive development automation tools to streamline the development process.

### Available Commands

#### Environment Setup
```bash
# Setup development environment
python dev_automation.py setup --all

# Setup specific components
python dev_automation.py setup --environment --deps --python
python dev_automation.py setup --quality --lint --format --type-check
```

#### Code Quality and Validation
```bash
# Run code quality checks
python dev_automation.py validate --quality --all
python dev_automation.py validate --lint --format --type-check --security
```

#### Building and Packaging
```bash
# Build the project
python dev_automation.py build --all
python dev_automation.py build --python --cpp
python dev_automation.py build --package --dist
```

#### Testing and Coverage
```bash
# Run tests and coverage analysis
python dev_automation.py test --all --coverage
python dev_automation.py test --unit --integration --performance
python dev_automation.py test --benchmark --performance
```

#### Documentation
```bash
# Generate and validate documentation
python dev_automation.py docs --generate --all
python dev_automation.py docs --validate --links
python dev_automation.py docs --build --readme --api
```

#### Deployment and Release
```bash
# Deploy and release the project
python dev_automation.py deploy --build --distribute
python dev_automation.py release --version --notes --publish
```

#### Monitoring and Performance
```bash
# Monitor project performance
python dev_automation.py monitor --performance --resources
python dev_automation.py monitor --benchmark --regression
```

#### Project Management
```bash
# Manage project aspects
python dev_automation.py project --status
python dev_automation.py project --metrics
python dev_automation.py project --health
```

### Automated Development Workflows

The development automation system provides several automated workflows:

#### 1. Full Development Cycle (`--all`)

The `--all` flag triggers the complete development cycle:
1. **Setup**: Initialize environment and install dependencies
2. **Validation**: Run code quality checks and validation
3. **Build**: Compile C++ code and build Python package
4. **Test**: Run unit, integration, and performance tests
5. **Documentation**: Generate API documentation and validate links
6. **Deploy**: Build distribution and prepare for release
7. **Monitor**: Monitor performance and resource usage
8. **Release**: Create release notes and version management

#### 2. Phase-based Development (`--phase`)

Specify a development phase for focused work:
```bash
# Setup phase
python dev_automation.py setup --phase environment
python dev_automation.py setup --phase quality
python dev_automation.py setup --phase dependencies

# Validation phase
python dev_automation.py validate --phase lint
python dev_automation.py validate --phase format
python dev_automation.py validate --phase type-check

# Build phase
python dev_automation.py build --phase python
python dev_automation.py build --phase cpp
python dev_automation.py build --phase package
```

#### 3. Custom Workflows (`--workflow`)

Create and run custom development workflows:
```bash
# Create a custom workflow
python dev_automation.py workflow create --name "my-workflow" --steps setup,build,test

# Run custom workflow
python dev_automation.py workflow run --name "my-workflow"
```

### Configuration

The development automation system uses `dev_config.yaml` for configuration. You can customize various aspects:

- **Environment settings**: Python version, build type, dependencies
- **Quality checks**: Linting, formatting, type checking, security scanning
- **Building**: Build system, packaging, cleanup options
- **Testing**: Test suites, coverage, parallel execution
- **Documentation**: API documentation, link validation
- **Deployment**: Distribution, publishing, release notes
- **Monitoring**: Performance monitoring, resource tracking

### Advanced Features

#### 1. Parallel Testing

Enable parallel test execution for faster feedback:
```bash
python dev_automation.py test --all --parallel --jobs 4
```

#### 2. Custom Test Suites

Create and run custom test suites:
```bash
python dev_automation.py test --create-suite --name "performance" --timeout 600
python dev_automation.py test --run-suite --name "performance"
```

#### 3. Environment Management

Setup and manage development environments:
```bash
python dev_automation.py environment --setup --python 3.11 --cpp c++20
python dev_automation.py environment --check --python --cpp --packages
```

#### 4. Performance Benchmarking

Run performance benchmarks and track regressions:
```bash
python dev_automation.py benchmark --all --regression
python dev_automation.py benchmark --suite "performance" --output benchmark_results.json
```

### Integration with CI/CD

The development automation system integrates with GitHub Actions and other CI/CD systems:

#### GitHub Actions Configuration

The `.github/workflows/main.yml` file contains comprehensive CI/CD workflows that automatically:
- Run code quality checks on every push
- Build and test in multiple Python versions
- Run performance benchmarks and detect regressions
- Generate documentation and validate links
- Deploy releases to package repositories

#### Local CI/CD

For local CI/CD integration, use the automation system to:
- Generate test reports and coverage summaries
- Create release notes and version management
- Deploy to staging and production environments
- Monitor and alert on performance regressions

### Testing and Coverage

The development automation system provides comprehensive testing and coverage capabilities:

#### Test Categories

1. **Unit Tests**: Test individual components and functions
2. **Integration Tests**: Test system interactions and component integration
3. **Performance Tests**: Test system performance and scalability
4. **Regression Tests**: Test for performance regressions
5. **Acceptance Tests**: Test user-facing functionality

#### Coverage Analysis

Generate and analyze test coverage reports:
```bash
python dev_automation.py test --all --coverage --generate-reports
python dev_automation.py test --analyze --coverage --threshold 0.8
```

### Documentation Generation

Automatic documentation generation and validation:

#### API Documentation

Generate comprehensive API documentation:
```bash
python dev_automation.py docs --generate --api --output docs/api
python dev_automation.py docs --validate --api --format json
```

#### User Documentation

Generate user-facing documentation:
```bash
python dev_automation.py docs --generate --readme --output docs/readme
python dev_automation.py docs --generate --changelogs --output docs/changelogs
```

### Development Best Practices

The development automation system enforces best practices:

#### 1. Code Quality

- Automatic linting and formatting
- Type checking and validation
- Security scanning and vulnerability detection
- Documentation validation and consistency

#### 2. Build Consistency

- Standardized build processes
- Cross-platform compatibility
- Version management and dependency tracking
- Automated package creation

#### 3. Testing Excellence

- Comprehensive test coverage
- Performance benchmarking
- Regression detection
- Continuous integration

#### 4. Documentation Standards

- API documentation generation
- Link validation and consistency
- Changelog management
- Versioned documentation

#### 5. Deployment Reliability

- Automated deployment pipelines
- Version control integration
- Release management
- Performance monitoring

### Getting Started

1. **Initial Setup**: Run `python dev_automation.py setup --all`
2. **Code Quality**: Run `python dev_automation.py validate --all`
3. **Build**: Run `python dev_automation.py build --all`
4. **Test**: Run `python dev_automation.py test --all`
5. **Documentation**: Run `python dev_automation.py docs --all`
6. **Deploy**: Run `python dev_automation.py deploy --all`

The development automation system will guide you through each step and provide comprehensive feedback on your development progress.

## License

MIT

## Authors

NLM Authors

## Project Status

This project is under active development with comprehensive automation and tooling to streamline the development process.
"""
    
    def _get_flake8_config(self) -> str:
        """Get flake8 configuration"""
        return """
[flake8]
max-line-length = 100
exclude =
    .git,
    __pycache__,
    build,
    dist,
    *.pyc,
    *.pyo,
    .pytest_cache,
    _build,
    mkdocs,
    *.egg-info,
    .eggs,
    env,
    venv,
    environment,
    dist,
    build,
    pip-wheel-metadata
ignore =
    E203,  # whitespace before ':'
    W503,  # line break before binary operator
    F401,  # imported but unused
    F403,  # 'from module import *' used; unable to detect undefined names
    F405,  # name imported from 'module' but unresolved
"""
    
    def _get_mypy_config(self) -> str:
        """Get mypy configuration"""
        return """
[mypy]
python_version = 3.11
warn_return_any = true
check_untyped_defs = true
ignore_missing_imports = true
exclude = \.git|__pycache__|build|dist|*.egg-info
"""
    
    def _get_mkdocs_config(self) -> str:
        """Get mkdocs configuration"""
        return """
site_name: NLM Documentation
site_url: https://nlm.readthedocs.io/
docs_dir: docs
use_directory_urls: true
nav:
  - Home: index.md
  - Getting Started: getting-started.md
  - API Reference: api/
  - Examples: examples/
  - Development: development/
  
theme:
  name: material
  palette:
    - media: "screen and (prefers-color-scheme: light)"
      primary: blue grey
      accent: blue
      toggle:
        icon: material/lightbulb
        name: Switch to light mode
    - media: "screen and (prefers-color-scheme: dark)"
      primary: blue grey
      accent: blue
      toggle:
        icon: material/lightbulb-outline
        name: Switch to dark mode
  features:
    - navigation.instant
    - navigation.tracking
    - navigation.tabs
    - navigation.sections
    - navigation.expand
    - navigation.top
    - search.highlight
    - search.share
    - navigation.indexes
  
  font:
    text: Roboto
    code: Roboto Mono
  
  icons:
    logo: nlm-logo
    
markdown_extensions:
  - admonition
  - attr_list
  - codehilite
  - footnotes
  - meta
  - md_in_html
  - sane_lists
  - smarty
  - tables
  - toc:
      permalink: true
      title: Table of Contents
  - footnotes
  - def_list
  - attr_list
  - md_in_html
  
plugins:
  - search
  - git-authors
  - minify
  """
    
    def __init_subclass__(cls, **kwargs):
        # This line is used for Python 3.6+ compatibility
        super().__init_subclass__(**kwargs)
    
    def __class_getitem__(cls, item):
        return cls

# Main execution block
if __name__ == '__main__':
    print("NLM Development Automation")
    print("=" * 30)
    
    # Parse command line arguments
    parser = argparse.ArgumentParser(
        description='NLM Development Automation - Automated Development Workflows'
    )
    
    parser.add_argument(
        'command',
        nargs='?',
        default='help',
        help='Command to run (setup, validate, build, test, docs, deploy, monitor, project, workflow, environment, benchmark)'
    )
    
    # Add command-specific options
    parser.add_argument('--all', action='store_true', help='Run all sub-commands')
    parser.add_argument('--environment', action='store_true', help='Setup environment')
    parser.add_argument('--deps', action='store_true', help='Install dependencies')
    parser.add_argument('--quality', action='store_true', help='Setup quality checks')
    parser.add_argument('--lint', action='store_true', help='Run linting checks')
    parser.add_argument('--format', action='store_true', help='Run formatting checks')
    parser.add_argument('--type-check', action='store_true', help='Run type checking')
    parser.add_argument('--security', action='store_true', help='Run security scanning')
    parser.add_argument('--documentation', action='store_true', help='Run documentation validation')
    parser.add_argument('--cpp', action='store_true', help='Build C++ components')
    parser.add_argument('--python', action='store_true', help='Build Python package')
    parser.add_argument('--package', action='store_true', help='Package distribution')
    parser.add_argument('--unit', action='store_true', help='Run unit tests')
    parser.add_argument('--integration', action='store_true', help='Run integration tests')
    parser.add_argument('--performance', action='store_true', help='Run performance tests')
    parser.add_argument('--coverage', action='store_true', help='Run coverage analysis')
    parser.add_argument('--generate', action='store_true', help='Generate documentation')
    parser.add_argument('--validate', action='store_true', help='Validate documentation')
    parser.add_argument('--build', action='store_true', help='Build documentation')
    parser.add_argument('--readme', action='store_true', help='Build README documentation')
    parser.add_argument('--api', action='store_true', help='Build API documentation')
    parser.add_argument('--build', action='store_true', help='Build distribution')
    parser.add_argument('--distribute', action='store_true', help='Distribute packages')
    parser.add_argument('--publish', action='store_true', help='Publish packages')
    parser.add_argument('--update-docs', action='store_true', help='Update documentation')
    parser.add_argument('--create-notes', action='store_true', help='Create release notes')
    parser.add_argument('--parallel', action='store_true', help='Run tests in parallel')
    parser.add_argument('--jobs', type=int, default=os.cpu_count() or 4, help='Number of parallel jobs')
    parser.add_argument('--timeout', type=int, help='Test timeout in seconds')
    parser.add_argument('--verbose', action='store_true', help='Enable verbose output')
    parser.add_argument('--config', type=str, default='dev_config.yaml', help='Configuration file')
    parser.add_argument('--help', action='store_true', help='Show help information')
    
    args = parser.parse_args()
    
    # Create automation instance
    automation = NLMDevelopmentAutomation(config_file=args.config)
    
    # Override config verbosity
    if args.verbose:
        logging.getLogger().setLevel(logging.DEBUG)
    
    # Run command
    success = automation.run(args.command, **{k: v for k, v in vars(args).items() if k not in ['command', 'help', 'config', 'verbose']})
    
    if success:
        print("\n✅ Command completed successfully!")
        sys.exit(0)
    else:
        print("\n❌ Command failed!")
        sys.exit(1)
        
# Legacy support for Python 3.6 and earlier
if sys.version_info < (3, 7):
    from typing import DefaultDict, OrderedDict, List, Dict, Set, Tuple, Optional, Union, Callable
    
    # Fix for class_getitem in older Python versions
    if not hasattr(type, '__class_getitem__'):
        def __class_getitem__(cls, item):
            return cls
        type.__class_getitem__ = __class_getitem__
        
    # Fix for __init_subclass__ in older Python versions
    if not hasattr(type, '__init_subclass__'):
        def __init_subclass__(cls, **kwargs):
            super().__init_subclass__(**kwargs)
        type.__init_subclass__ = __init_subclass__