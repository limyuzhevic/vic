# CI/CD Configuration for NLM

This document describes the CI/CD configuration and automated testing setup for the NLM (Neural Learning Machine) project.

## Overview

The NLM project uses a comprehensive CI/CD pipeline that includes:

- Code quality checks (linting, formatting, static analysis)
- C++ build and unit tests
- Python package testing
- Performance benchmarking
- Documentation generation
- Automated deployment

## Repository Structure

The CI/CD configuration is organized as follows:

```
.github/
  workflows/
    ci.yml              # Main CI workflow
├── CMakeLists.txt
├── cmake/
│   └── Testing.cmake          # CMake testing configuration
├── docs/
│   ├── README.md              # This file
│   └── CI.md                  # CI-specific documentation
├── scripts/
│   ├── setup-build-env.sh     # Build environment setup script
│   ├── run-tests.sh           # Test execution script
│   └── deploy.sh              # Deployment script
├── config/
│   └── test-config.json      # Test configuration
└── examples/
    ├── ci-config.json        # CI configuration examples
    └── test-cases/           # Test case examples
```

## GitHub Actions Configuration

### Main Workflow (`.github/workflows/ci.yml`)

The main workflow includes the following jobs:

#### 1. Code Quality Checks (`code-quality`)

- **Purpose**: Ensure code quality and consistency
- **Steps**:
  - Checkout code
  - Install dependencies
  - Run C++ code style checks (clang-format)
  - Run Python linting (black, isort, flake8)
  - Check shell scripts (shellcheck)
  - Generate build system
  - Check for compilation issues

#### 2. C++ Build Tests (`cpp-build`)

- **Purpose**: Build and test C++ components
- **Steps**:
  - Setup CMake and dependencies
  - Configure CMake build
  - Build the project
  - Run unit tests
  - Test Python bindings

#### 3. Python Tests (`python-tests`)

- **Purpose**: Test Python bindings and examples
- **Steps**:
  - Setup Python environment
  - Install build dependencies
  - Build and install Python package
  - Run Python tests
  - Run documentation generation

#### 4. Performance Benchmarks (`performance-benchmarks`)

- **Purpose**: Run performance benchmarks and generate reports
- **Steps**:
  - Build with optimizations
  - Run performance benchmarks
  - Generate performance reports
  - Upload artifacts

#### 5. Documentation (`documentation`)

- **Purpose**: Generate and publish documentation
- **Steps**:
  - Checkout code
  - Install documentation dependencies
  - Generate API documentation (Doxygen)
  - Generate Python documentation
  - Build documentation
  - Upload documentation artifacts

#### 6. Deployment (`deploy`)

- **Purpose**: Deploy to production (on main branch)
- **Steps**:
  - Checkout code
  - Setup Python environment
  - Build Python package
  - Test package
  - Create release artifacts
  - Upload artifacts
  - Deploy to PyPI (on release tags)

## CMake Testing Configuration

### `cmake/Testing.cmake`

This file provides:

- **Test configuration options**:
  - `ENABLE_UNIT_TESTS`: Enable unit tests
  - `ENABLE_INTEGRATION_TESTS`: Enable integration tests
  - `ENABLE_PERFORMANCE_TESTS`: Enable performance tests
  - `ENABLE_LONG_TESTS`: Enable long-running tests
  - `ENABLE_COVERAGE`: Enable test coverage analysis

- **Custom test targets**:
  - `run-all-tests`: Run all tests
  - `run-unit-tests`: Run unit tests only
  - `run-integration-tests`: Run integration tests only
  - `run-performance-tests`: Run performance tests only

- **Performance benchmark targets**:
  - `benchmark_neural_network`: Neural network benchmarks
  - `benchmark_plasticity`: Plasticity benchmarks
  - `benchmark_memory`: Memory benchmarks
  - `benchmark_integration`: Integration benchmarks

## Running Tests Locally

### Prerequisites

1. **C++ Dependencies**:
   ```bash
   sudo apt-get update
   sudo apt-get install -y cmake g++ gcc make ccache pkg-config libpython3-dev
   ```

2. **Python Dependencies**:
   ```bash
   pip install scikit-build-core pybind11 pytest numpy
   ```

3. **Additional Tools** (optional):
   ```bash
   # For code formatting
   sudo apt-get install -y clang-format
   pip install black isort flake8 pydocstyle

   # For documentation
   pip install sphinx sphinx-rtd-theme breathe m2r

   # For shell script checking
   sudo apt-get install -y shellcheck
   ```

### Local CI Simulation

You can run a local CI simulation using the provided scripts:

```bash
# Setup build environment
source scripts/setup-build-env.sh

# Run all tests
./scripts/run-tests.sh

# Run specific test types
./scripts/run-tests.sh --unit
./scripts/run-tests.sh --integration
./scripts/run-tests.sh --performance
```

### Build and Test

```bash
# Create build directory
mkdir -p build
cd build

# Configure CMake
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_TESTS=ON

# Build
make -j$(nproc)

# Run tests
ctest --output-on-failure

# Or run specific test targets
make run-all-tests
make run-unit-tests
make run-integration-tests
```

### Performance Benchmarks

```bash
# Build with performance profiling
mkdir -p build_perf
cd build_perf
cmake .. -DENABLE_PERF_PROFILING=ON -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Run performance benchmarks
./benchmark --neural-network
./benchmark --plasticity
./benchmark --memory
./nlm_phase6_demo
```

## Configuration Files

### `config/test-config.json`

Example test configuration:

```json
{
  "test_settings": {
    "unit_tests": {
      "enabled": true,
      "timeout": 300,
      "parallel": 4
    },
    "integration_tests": {
      "enabled": true,
      "timeout": 600,
      "parallel": 2
    },
    "performance_tests": {
      "enabled": false,
      "timeout": 1800,
      "iterations": 1
    }
  },
  "build_settings": {
    "compiler": "g++",
    "standard": "c++20",
    "optimization": "O3",
    "coverage": false
  },
  "environment": {
    "python_version": "3.11",
    "cmake_version": "3.16"
  }
}
```

## Example CI Configurations

### `examples/ci-config.json`

Example GitHub Actions configuration:

```json
{
  "name": "NLM CI/CD Pipeline",
  "on": {
    "push": {
      "branches": ["main", "develop", "feature/*"],
      "tags": ["v*"]
    },
    "pull_request": {
      "branches": ["main"]
    }
  },
  "env": {
    "BUILD_TYPE": "Release",
    "CCACHE_DIR": "~/.ccache",
    "CCACHE_MAXSIZE": "1G"
  }
}
```

### `examples/test-cases/`

Example test cases for unit and integration testing.

## Docker-Based CI/CD

### Dockerfile

Basic Docker setup for CI/CD:

```dockerfile
FROM ubuntu:22.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    cmake \
    g++ \
    gcc \
    make \
    ccache \
    pkg-config \
    libpython3-dev \
    python3-pip \
    git \
    && rm -rf /var/lib/apt/lists/*

# Install Python dependencies
RUN pip install --upgrade pip
sudo pip install scikit-build-core pybind11 pytest numpy

# Create working directory
WORKDIR /app

# Copy project files
COPY . /app

# Build and test
RUN mkdir -p build && \
    cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    make -j$(nproc) && \
    ctest --output-on-failure

# Run Python tests
RUN pip install .
RUN pytest tests/ -v

# Clean up
RUN rm -rf /root/.cache/pip

# Default command
CMD ["/bin/bash"]
```

### Docker Compose

For local development and testing:

```yaml
version: '3.8'

services:
  nlm-dev:
    build: .
    volumes:
      - ./:/app
      - ./build:/app/build
      - ./dist:/app/dist
    working_dir: /app
    command: tail -f /dev/null  # Keep container running
```

## Documentation Generation

### Sphinx Configuration (`source/conf.py`)

```python
# Sphinx configuration for Python bindings documentation
import sys
import os

sys.path.insert(0, os.path.abspath('..'))

project = 'NLM - Neural Learning Machine'
copyright = '2024, NLM Authors'
author = 'NLM Authors'

extensions = [
    'sphinx.ext.autodoc',
    'sphinx.ext.viewcode',
    'sphinx.ext.napoleon',
    'sphinx.ext.intersphinx',
]

templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

html_theme = 'sphinx_rtd_theme'
html_static_path = ['_static']
```

## Monitoring and Metrics

### Prometheus Monitoring

The project can be instrumented with Prometheus for monitoring:

```yaml
# prometheus.yml
global:
  scrape_interval: 15s

scrape_configs:
  - job_name: 'nlm-metrics'
    static_configs:
      - targets: ['localhost:9090']
    metrics_path: '/metrics'
    scrape_interval: 30s
```

### Grafana Dashboards

Create Grafana dashboards for monitoring NLM performance:

- System metrics (CPU, memory, network)
- Test execution metrics
- Performance benchmark results
- CI/CD pipeline status

## Backup and Recovery

### Backup Strategy

1. **Git backups**: Regular commits with automated backups
2. **Build artifacts**: Store build artifacts in cloud storage
3. **Configuration backups**: Backup configuration files
4. **Test results**: Archive test results for analysis

### Recovery Procedures

1. **Code restoration**: Use git to restore from commits
2. **Build restoration**: Use cached build artifacts
3. **Configuration restoration**: Restore from backup files
4. **Test result restoration**: Use archived test results

## Security Considerations

### Secrets Management

- Use GitHub Actions secrets for sensitive information
- Store API keys and tokens in encrypted form
- Use least privilege principle for CI/CD runners
- Implement multi-factor authentication for deployment

### Security Scanning

1. **Static Application Security Testing (SAST)**:
   - Check for security vulnerabilities in code
   - Analyze dependencies for known vulnerabilities
   - Review configuration files for security issues

2. **Dynamic Application Security Testing (DAST)**:
   - Run security tests in production environment
   - Test for common web application vulnerabilities
   - Validate authentication and authorization

## Performance Optimization

### Build Optimization

1. **Incremental builds**: Use ccache for faster compilation
2. **Parallel builds**: Use multiple CPU cores
3. **Dependency caching**: Cache build dependencies
4. **Incremental testing**: Only run changed tests

### Test Optimization

1. **Test selection**: Only run relevant tests
2. **Test parallelization**: Run tests in parallel
3. **Test skipping**: Skip slow tests based on configuration
4. **Result caching**: Cache test results for faster re-runs

## Maintenance and Updates

### Regular Maintenance Tasks

1. **Update dependencies**: Regularly update dependencies
2. **Clean up artifacts**: Remove temporary files and build artifacts
3. **Monitor CI/CD health**: Monitor pipeline success rates
4. **Review test results**: Regularly review test failures

### Update Procedures

1. **Version updates**: Update versions in configuration files
2. **Dependency updates**: Update dependencies and test compatibility
3. **Documentation updates**: Update documentation for new features
4. **Configuration updates**: Update CI/CD configuration for new workflows

## Troubleshooting

### Common Issues and Solutions

#### Code Quality Failures

**Problem**: Code formatting or linting errors

**Solution**:
- Run formatting tools locally before committing
- Check CI logs for specific error messages
- Fix errors in the affected files

#### Build Failures

**Problem**: CMake build failures

**Solution**:
- Check CMake configuration
- Verify compiler version compatibility
- Ensure all dependencies are installed
- Check for circular dependencies

#### Test Failures

**Problem**: Test failures

**Solution**:
- Check test logs for specific failure details
- Run failing tests locally with more verbose output
- Check for environment-specific issues
- Fix test code or configuration as needed

#### Performance Issues

**Problem**: Slow build or test execution

**Solution**:
- Enable compiler optimizations
- Use parallel builds
- Cache dependencies
- Skip long-running tests when not needed

## Support and Contact

For help with CI/CD configuration:

1. **GitHub Issues**: Report issues in the GitHub repository
2. **Documentation**: Check the documentation for troubleshooting
3. **Community**: Join NLM community for support
4. **Contact**: Contact project maintainers for assistance

## Version Control

This file is tracked in version control and updated with each CI/CD improvement.

## License

This documentation is part of the NLM project and is licensed under the MIT License.

---

*Note: This file is generated as part of the CI/CD configuration improvements.*
