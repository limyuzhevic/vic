# Modern NLM Development Workflow

## Overview

The NLM (Neural Learning Machine) project requires a modern development workflow that supports:

1. **Continuous Integration**: Automated testing, building, and deployment
2. **Continuous Deployment**: Automated release management
3. **Development Tools**: Modern IDE integration, code analysis, and project management
4. **Performance Engineering**: Performance testing and optimization
5. **Documentation Generation**: Automated documentation creation
6. **Security Scanning**: Security vulnerability detection
7. **Quality Assurance**: Code quality checks and validation

This framework provides:

1. **CI/CD Pipeline Configuration**: GitHub Actions, GitLab CI, and Jenkins pipelines
2. **Development Environment Setup**: Docker, VS Code, and IDE configurations
3. **Build System Modernization**: CMake, Conan, and package management
4. **Testing Infrastructure**: Automated testing, coverage, and performance testing
5. **Documentation System**: Doxygen, Sphinx, and Markdown documentation
6. **Code Quality Tools**: Linting, formatting, and static analysis
7. **Release Management**: Version control, packaging, and deployment strategies

## CI/CD Pipeline Configuration

### 1. GitHub Actions Workflow

```yaml
# .github/workflows/nlm-ci.yml
name: NLM CI/CD Pipeline

on:
  push:
    branches: [ main, develop ]
    tags: [ 'v*' ]
  pull_request:
    branches: [ main, develop ]
  workflow_dispatch:

env:
  REGISTRY: ghcr.io
  IMAGE_NAME: ${{ github.repository }}
  PYTHON_VERSION: '3.11'
  CXX_STANDARD: '20'
  BUILD_TYPE: Release

jobs:
  # Code Quality Checks
  code-quality:
    name: Code Quality Checks
    runs-on: ubuntu-latest
    steps:
      - name: Checkout repository
        uses: actions/checkout@v4
        with:
          fetch-depth: 0
      
      - name: Set up Python ${{ env.PYTHON_VERSION }}
        uses: actions/setup-python@v5
        with:
          python-version: ${{ env.PYTHON_VERSION }}
      
      - name: Set up C++17
        run: |
          sudo apt-get update
          sudo apt-get install -y g++ cmake clang-format clang-tidy
      
      - name: Install Python dependencies
        run: |
          python -m pip install --upgrade pip
          pip install black flake8 isort mypy pytest pytest-cov
          pip install scikit-build-core pybind11 pyyaml
      
      - name: Run code formatting check
        run: |
          black --check --line-length 120 .
          isort --check-only --line-length 120 .
      
      - name: Run linting
        run: |
          flake8 --max-line-length 120 --extend-ignore=E203,W503 .
          mypy --ignore-missing-imports --show-error-codes .
      
      - name: Check code style with clang-format
        run: |
          find . -name "*.cpp" -o -name "*.hpp" -o -name "*.h" | \
          xargs clang-format --dry-run --style=file
      
      - name: Run static analysis with clang-tidy
        run: |
          mkdir -p build
          cd build
          cmake .. -DCMAKE_BUILD_TYPE=${{ env.BUILD_TYPE }} -DENABLE_TESTS=ON
          make -j$(nproc)
          clang-tidy -p . src/brain/Brain.cpp src/brain/Neuron.cpp src/brain/Synapse.cpp
  
  # Dependency Management
  dependency-management:
    name: Dependency Management
    runs-on: ubuntu-latest
    steps:
      - name: Checkout repository
        uses: actions/checkout@v4
      
      - name: Set up Conan
        uses: tpranitz/conan-action@v1
        with:
          conan_version: 2.0.15
          install_options: '--build missing'
      
      - name: Cache Conan packages
        id: conan-cache
        uses: actions/cache@v4
        with:
          path: ~/.conan2/data
          key: ${{ runner.os }}-conan-${{ hashFiles('conanfile.py') }}
          restore-keys: |
            ${{ runner.os }}-conan-
      
      - name: Create Conan profile
        run: |
          mkdir -p ~/.conan2/profiles
          cat > ~/.conan2/profiles/default << EOF
          [settings]
          os=Linux
          arch=x86_64
          compiler=gcc
          compiler.version=11
          compiler.libcxx=libstdc++11
          build_type=Release
          
          [options]
          with_rtti=true
          shared_link_mode=dyn
          
          [conf]
          tools.build:compiler_executables=c++:g++,cc:gcc
          tools.cmake.cmaketoolchain:generator=Ninja
          tools.system.package_manager:mode=install
          tools.system.package_manager:sudo_command=sudo
          tools.build:skip_test=true
          EOF
      
      - name: Install system dependencies
        run: |
          sudo apt-get update
          sudo apt-get install -y \
            g++ cmake ninja-build \
            libqt5core5lib libqt5gui5 libqt5widgets5 \
            libsfml-dev libhdf5-dev libboost-all-dev \
            python3-dev python3-pip python3-venv \
            git curl wget unzip
      
      - name: Install Python dependencies
        run: |
          python -m pip install --upgrade pip
          pip install scikit-build-core pybind11 pytest pytest-cov pytest-benchmark
          pip install numpy matplotlib seaborn plotly
          pip install black flake8 isort mypy pre-commit
      
      - name: Create build directory
        run: mkdir -p build
      
      - name: Configure CMake with Conan
        run: |
          cd build
          cmake .. \
            -DCMAKE_BUILD_TYPE=${{ env.BUILD_TYPE }} \
            -DCMAKE_CXX_STANDARD=${{ env.CXX_STANDARD }} \
            -DENABLE_TESTS=ON \
            -DENABLE_PYTHON=ON \
            -DPYTHON_EXECUTABLE=$(which python3) \
            -DBUILD_TESTING=ON \
            -DBUILD_EXAMPLES=ON
      
      - name: Build with Conan package manager
        run: |
          cd build
          conan install . --build=missing
          cmake --build . --config Release --parallel $(nproc)
  
  # Unit Testing
  unit-testing:
    name: Unit Testing
    runs-on: ubuntu-latest
    needs: [code-quality, dependency-management]
    strategy:
      matrix:
        build_type: [Debug, Release]
        include_tests: [true, false]
    
    steps:
      - name: Checkout repository
        uses: actions/checkout@v4
      
      - name: Set up Python ${{ env.PYTHON_VERSION }}
        uses: actions/setup-python@v5
        with:
          python-version: ${{ env.PYTHON_VERSION }}
      
      - name: Install dependencies
        run: |
          sudo apt-get update
          sudo apt-get install -y g++ cmake
          python -m pip install scikit-build-core pybind11 pytest pytest-cov pytest-benchmark
      
      - name: Build project
        run: |
          mkdir -p build
          cd build
          cmake .. -DCMAKE_BUILD_TYPE=${{ matrix.build_type }} -DENABLE_TESTS=ON
          make -j$(nproc)
      
      - name: Run unit tests
        run: |
          cd build
          ctest --output-on-failure --verbose
      
      - name: Run Python tests
        run: |
          cd build
          python -m pytest tests/ --cov=pynlm --cov-report=xml --cov-report=html
          python -m pytest python/ --cov=pynlm --cov-report=xml --cov-report=html
      
      - name: Upload test artifacts
        uses: actions/upload-artifact@v4
        if: always()
        with:
          name: test-results-${{ matrix.build_type }}
          path: |
            build/test_results.xml
            build/coverage_html_report/
            build/pytest_coverage_report/
          retention-days: 7
  
  # Performance Testing
  performance-testing:
    name: Performance Testing
    runs-on: ubuntu-latest
    needs: unit-testing
    steps:
      - name: Checkout repository
        uses: actions/checkout@v4
      
      - name: Install dependencies
        run: |
          sudo apt-get update
          sudo apt-get install -y g++ cmake python3-pip
          python -m pip install pytest-benchmark pytest-performance
      
      - name: Build with optimizations
        run: |
          mkdir -p build_perf
          cd build_perf
          cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-O3 -march=native"
          make -j$(nproc)
      
      - name: Run performance benchmarks
        run: |
          cd build_perf
          ./benchmarks/nlm_benchmark
          python -m pytest-benchmark --benchmark-save=benchmarks.json --benchmark-min-time=1
      
      - name: Analyze performance results
        run: |
          python scripts/analyze_performance.py benchmarks.json
      
      - name: Generate performance report
        run: |
          python scripts/generate_performance_report.py
      
      - name: Upload performance artifacts
        uses: actions/upload-artifact@v4
        if: always()
        with:
          name: performance-results
          path: |
            benchmarks.json
            performance_report.html
            performance_analysis.json
          retention-days: 30
  
  # Integration Testing
  integration-testing:
    name: Integration Testing
    runs-on: ubuntu-latest
    needs: unit-testing
    steps:
      - name: Checkout repository
        uses: actions/checkout@v4
      
      - name: Build integration test environment
        run: |
          mkdir -p build_integration
          cd build_integration
          cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_INTEGRATION_TESTS=ON
          make -j$(nproc)
      
      - name: Run integration tests
        run: |
          cd build_integration
          ./integration_tests/nlm_integration_tests
      
      - name: Run visualization tests
        run: |
          ./visualization_tests/nlm_visualization_tests
      
      - name: Run monitoring tests
        run: |
          ./monitoring_tests/nlm_monitoring_tests
      
      - name: Run debug tests
        run: |
          ./debug_tests/nlm_debug_tests
      
      - name: Upload integration artifacts
        uses: actions/upload-artifact@v4
        if: always()
        with:
          name: integration-results
          path: |
            integration_test_results.xml
            visualization_test_results.xml
            monitoring_test_results.xml
            debug_test_results.xml
          retention-days: 7
  
  # Python Binding Tests
  python-binding-tests:
    name: Python Binding Tests
    runs-on: ubuntu-latest
    needs: unit-testing
    steps:
      - name: Checkout repository
        uses: actions/checkout@v4
      
      - name: Install Python dependencies
        run: |
          python -m pip install --upgrade pip
          pip install scikit-build-core pybind11 pytest pytest-cov pytest-benchmark
          pip install numpy matplotlib seaborn plotly
          pip install black flake8 isort mypy
      
      - name: Build Python module
        run: |
          pip install -e .
      
      - name: Run Python binding tests
        run: |
          python -m pytest python_tests/ --cov=pynlm --cov-report=xml --cov-report=html
          python -m pytest integration/python/ --cov=pynlm --cov-report=xml --cov-report=html
      
      - name: Test Python API
        run: |
          python -c "
          import pynlm
          import numpy as np
          print('Python bindings test passed')
          
          # Test basic brain creation
          config = pynlm.createDefaultConfig()
          brain = pynlm.createBrain(config)
          brain.initialize()
          print('Brain creation test passed')
          
          # Test world interaction
          world = pynlm.createSimpleWorld()
          world.configure(20, 20, 8, 8)
          print('World creation test passed')
          
          # Test agent
          agent = pynlm.createAgentBrain(brain)
          agent.initialize(world)
          print('Agent creation test passed')
          
          print('All Python API tests passed')
          " 2>&1 | tee python_api_test.log
      
      - name: Upload Python artifacts
        uses: actions/upload-artifact@v4
        if: always()
        with:
          name: python-results
          path: |
            python_api_test.log
            coverage_html_report/
            pytest_coverage_report/
          retention-days: 7
  
  # Documentation Generation
  documentation:
    name: Documentation Generation
    runs: ubuntu-latest
    needs: unit-testing
    steps:
      - name: Checkout repository
        uses: actions/checkout@v4
      
      - name: Set up Doxygen
        run: |
          sudo apt-get update
          sudo apt-get install -y doxygen graphviz
      
      - name: Generate C++ documentation
        run: |
          doxygen Doxyfile
      
      - name: Set up Sphinx
        run: |
          python -m pip install sphinx sphinx-rtd-theme breathe
      
      - name: Generate Python documentation
        run: |
          sphinx-build -b html docs/ docs/_build/html
      
      - name: Generate API documentation
        run: |
          python -m sphinx.cmd.build docs/ docs/_build/api --builder=html
      
      - name: Upload documentation artifacts
        uses: actions/upload-artifact@v4
        if: always()
        with:
          name: documentation
          path: |
            html/
            docs/_build/html/
          retention-days: 30
  
  # Security Scanning
  security-scan:
    name: Security Scanning
    runs-on: ubuntu-latest
    needs: unit-testing
    steps:
      - name: Checkout repository
        uses: actions/checkout@v4
      
      - name: Run Trivy vulnerability scanner
        uses: aquasecurity/trivy-action@v0.20.0
        with:
          scan-type: 'fs'
          scan-ref: '.'
          format: 'sarif'
          output: 'trivy-results.sarif'
      
      - name: Upload Trivy scan results
        uses: actions/upload-artifact@v4
        if: always()
        with:
          name: trivy-results
          path: trivy-results.sarif
          retention-days: 7
      
      - name: Run safety check
        run: |
          python -m pip install safety
          safety check --json > safety-results.json
      
      - name: Upload safety results
        uses: actions/upload-artifact@v4
        if: always()
        with:
          name: safety-results
          path: safety-results.json
          retention-days: 7
      
      - name: Run OWASP ZAP scan
        run: |
          docker run --rm -v $(pwd):/tmp/scan owasp/zap2docker-stable zap-baseline.py \
            -t http://localhost:8080
      
      - name: Upload ZAP results
        uses: actions/upload-artifact@v4
        if: always()
        with:
          name: zap-results
          path: zap-report.html
          retention-days: 7
  
  # Release Management
  release:
    name: Release Management
    runs-on: ubuntu-latest
    needs: [integration-testing, python-binding-tests, documentation]
    if: startsWith(github.ref, 'refs/tags/v')
    steps:
      - name: Checkout repository
        uses: actions/checkout@v4
        with:
          fetch-depth: 0
      
      - name: Set up Python ${{ env.PYTHON_VERSION }}
        uses: actions/setup-python@v5
        with:
          python-version: ${{ env.PYTHON_VERSION }}
      
      - name: Build release artifacts
        run: |
          python -m pip install build
          python -m build
          mkdir -p release
          cp dist/*.whl release/
          cp dist/*.tar.gz release/
      
      - name: Create Docker image
        run: |
          echo ${{ secrets.GITHUB_TOKEN }} | docker login ghcr.io -u ${{ github.actor }} --password-stdin
          docker build -t ${{ env.REGISTRY }}/${{ env.IMAGE_NAME }}:${{ github.ref_name }} .
          docker push ${{ env.REGISTRY }}/${{ env.IMAGE_NAME }}:${{ github.ref_name }}
      
      - name: Create GitHub Release
        uses: actions/create-release@v1
        env:
          GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
        with:
          tag_name: ${{ github.ref }}
          release_name: Release ${{ github.ref_name }}
          body: |
            ## Release Notes
            
            ### New Features
            - Added visualization framework with real-time monitoring
            - Enhanced integration testing and validation
            - Improved Python bindings and API documentation
            - Added performance benchmarking and analysis
            
            ### Bug Fixes
            - Fixed memory system integration issues
            - Resolved Python binding compatibility problems
            - Corrected visualization rendering issues
            
            ### Performance Improvements
            - Optimized neural computation performance
            - Improved memory efficiency in visualization
            - Enhanced data export and import capabilities
          draft: false
          prerelease: false
      
      - name: Upload release artifacts
        uses: actions/upload-release-asset@v1
        env:
          GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
        with:
          upload_url: ${{ steps.create_release.outputs.upload_url }}
          asset_path: release/*
          asset_name: ${{ github.ref_name }}_artifacts
          asset_content_type: application/octet-stream
      
      - name: Update documentation
        run: |
          git config --local user.email "action@github.com"
          git config --local user.name "GitHub Action"
          git add docs/ documentation/
          git commit -m "Update documentation for release ${{ github.ref_name }}" || true
          git push
```

### 2. GitLab CI Configuration

```yaml
# .gitlab-ci.yml
stages:
  - code-quality
  - build
  - test
  - integration
  - performance
  - deploy

variables:
  DOCKER_DRIVER: overlay2
  DOCKER_TLS_CERTDIR: /certs
  PYTHON_VERSION: 3.11
  CXX_STANDARD: 20
  BUILD_TYPE: Release
  CACHE_KEY: ${CI_COMMIT_SHORT_SHA}

image: alpine:latest

before_script:
  - echo "Starting job ${CI_JOB_NAME} (${CI_JOB_STAGE})"
  - apk add --no-cache python3 py3-pip gcc g++ make cmake ninja-build \
    git curl wget unzip build-base bash py3-wheel py3-setuptools
  - python3 -m pip install --upgrade pip
  - python3 -m pip install scikit-build-core pybind11 pytest pytest-cov pytest-benchmark

# Code Quality Stage
code-quality:
  stage: code-quality
  script:
    - echo "Running code quality checks"
    - python3 -m pip install black flake8 isort mypy
    - black --check --line-length 120 .
    - isort --check-only --line-length 120 .
    - flake8 --max-line-length 120 --extend-ignore=E203,W503 .
    - mypy --ignore-missing-imports --show-error-codes .
  artifacts:
    reports:
      junit: quality-report.xml
    paths:
      - quality-report.xml
    expire_in: 1 week

# Build Stage
build:
  stage: build
  script:
    - echo "Building NLM project"
    - mkdir -p build
    - cd build
    - cmake .. -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DENABLE_TESTS=ON -DENABLE_PYTHON=ON
    - make -j$(nproc)
  artifacts:
    paths:
      - build/
    expire_in: 1 week

# Unit Testing Stage
unit-test:
  stage: test
  needs: [code-quality, build]
  script:
    - echo "Running unit tests"
    - cd build
    - ctest --output-on-failure
    - python3 -m pytest tests/ --cov=pynlm --cov-report=xml
    - python3 -m pytest python/ --cov=pynlm --cov-report=xml
  artifacts:
    reports:
      junit: unit-test-report.xml
      coverage: coverage.xml
    paths:
      - unit-test-report.xml
      - coverage.xml
    expire_in: 1 week

# Integration Testing Stage
integration-test:
  stage: integration
  needs: [code-quality, build]
  script:
    - echo "Running integration tests"
    - cd build
    - ./integration_tests/nlm_integration_tests
    - ./visualization_tests/nlm_visualization_tests
    - ./monitoring_tests/nlm_monitoring_tests
    - ./debug_tests/nlm_debug_tests
  artifacts:
    reports:
      junit: integration-test-report.xml
    paths:
      - integration-test-report.xml
    expire_in: 1 week

# Performance Testing Stage
performance-test:
  stage: performance
  needs: [code-quality, build]
  script:
    - echo "Running performance tests"
    - mkdir -p build_perf
    - cd build_perf
    - cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-O3 -march=native"
    - make -j$(nproc)
    - ./benchmarks/nlm_benchmark
    - python3 -m pytest-benchmark --benchmark-save=performance.json
  artifacts:
    reports:
      junit: performance-test-report.xml
      coverage: performance.json
    paths:
      - performance.json
    expire_in: 1 month

# Deploy Stage (Manual)
deploy:
  stage: deploy
  needs: [code-quality, build, unit-test, integration-test, performance-test]
  script:
    - echo "Deploying NLM project"
    - echo "Building documentation"
    - doxygen Doxyfile
    - sphinx-build -b html docs/ docs/_build/html
    - echo "Uploading artifacts"
    - mkdir -p release
    - cp build/nlm release/
    - cp build/nlm_test release/
    - cp -r docs/_build/html release/documentation
  artifacts:
    paths:
      - release/
    expire_in: 1 month
  when: manual
```

### 3. Jenkins Configuration

```groovy
// Jenkinsfile
pipeline {
    agent {
        label 'ubuntu-latest'
    }
    
    environment {
        REGISTRY = "ghcr.io"
        IMAGE_NAME = "github-actions/nlm"
        PYTHON_VERSION = "3.11"
        CXX_STANDARD = "20"
        BUILD_TYPE = "Release"
    }
    
    stages {
        stage('Code Quality') {
            steps {
                sh 'echo "Running code quality checks"'
                sh 'python3 -m pip install black flake8 isort mypy'
                sh 'black --check --line-length 120 .'
                sh 'isort --check-only --line-length 120 .'
                sh 'flake8 --max-line-length 120 --extend-ignore=E203,W503 .'
                sh 'mypy --ignore-missing-imports --show-error-codes .'
            }
        }
        
        stage('Build') {
            steps {
                sh '''
                    mkdir -p build
                    cd build
                    cmake .. -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DENABLE_TESTS=ON
                    make -j$(nproc)
                '''
            }
        }
        
        stage('Test') {
            steps {
                sh '''
                    cd build
                    ctest --output-on-failure
                '''
                sh '''
                    cd build
                    python3 -m pytest tests/ --cov=pynlm --cov-report=xml
                '''
            }
        }
        
        stage('Deploy') {
            agent { label 'docker' }
            steps {
                sh '''
                    docker build -t $REGISTRY/$IMAGE_NAME:$GIT_COMMIT .
                    docker push $REGISTRY/$IMAGE_NAME:$GIT_COMMIT
                '''
            }
        }
    }
    
    post {
        always {
            archiveArtifacts artifacts: '**/*.xml', allowEmptyArchive: true
            junit '**/test-results.xml'
        }
        failure {
            slackSend channel: '#dev-ops', message: "NLM build failed for commit ${GIT_COMMIT}"
        }
        success {
            slackSend channel: '#dev-ops', message: "NLM build succeeded for commit ${GIT_COMMIT}"
        }
    }
}
```

## Development Environment Setup

### 1. Docker Development Environment

```dockerfile
# Dockerfile
dockerfileVersion: '3.8'

services:
  nlm-dev:
    image: ubuntu:22.04
    container_name: nlm-dev-container
    environment:
      - DISPLAY=${DISPLAY}
      - QT_X11_NO_MITSHM=1
    volumes:
      - ./:/workspace/nlm
      - /tmp/.X11-unix:/tmp/.X11-unix
      - ~/.Xauthority:/root/.Xauthority
    command: >
      bash -c '
        apt-get update && apt-get install -y --no-install-recommends
          build-essential
          cmake
          ninja-build
          libqt5core5lib
          libqt5gui5
          libqt5widgets5
          libsfml-dev
          libhdf5-dev
          python3
          python3-pip
          python3-venv
          git
          curl
          wget
          unzip
        && python3 -m pip install --upgrade pip
        && python3 -m pip install scikit-build-core pybind11 pytest pytest-cov pytest-benchmark
        && python3 -m pip install ipython matplotlib seaborn plotly
        && mkdir -p build
        && cd build
        && cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_TESTS=ON -DENABLE_PYTHON=ON
        && make -j$(nproc)
        && echo "Development environment ready"
        && exec bash
      '
```

### 2. VS Code Development Environment

```json
// .vscode/settings.json
{
    "cmake.buildDirectory": "${workspaceFolder}/build",
    "cmake.configureOnOpen": true,
    "cmake.generator": "Ninja",
    "cmake.buildArgs": "-DCMAKE_BUILD_TYPE=Release",
    "cmake.cDebugConfiguration": "Launch",
    "cmake.environmentVariables": {
        "CC": "gcc",
        "CXX": "g++"
    },
    "cmake.configureOnEdit": true,
    "cmake.debugConfig": "Launch",
    "cmake.highlightConfigurationProblems": "all",
    "cmake.buildOnSave": "all",
    "cmake.onConfigureError": "Show error messages",
    "cmake.makefilePlatform": "Unix",
    "cmake.trackBuildSystem": true,
    
    "python.pythonPath": "/usr/local/bin/python3",
    "python.formatting.provider": "black",
    "python.linting.enabled": true,
    "python.linting.pylintEnabled": false,
    "python.linting.flakeEnabled": true,
    "python.testing.pytestArgs": ["tests/", "--cov=pynlm"],
    "python.testing.cwd": "${workspaceFolder}/build",
    
    "editor.formatOnPaste": false,
    "editor.formatOnType": true,
    "editor.tabSize": 4,
    "editor.insertSpaces": true,
    "editor.detectIndentation": false,
    "editor.suggest.showWords": true,
    "editor.suggest.showIcons": true,
    "editor.suggest.shareSuggestConfigurations": "always",
    
    "files.exclude": {
        "**/.git": true,
        "**/.hg": true,
        "**/.svn": true,
        "**/.gitignore": true,
        "**/.DS_Store": true,
        "**/*.swp": true,
        "**/*.swo": true,
        "**/*.bak": true,
        "**/*.orig": true,
        "**/.coverage": true,
        "**/.pytest_cache": true,
        "**/__pycache__": true,
        "**/.mypy_cache": true,
        "**/.ruff_cache": true,
        "build/": true,
        ".vscode/.code-workspace": true
    },
    
    "search.exclude": {
        "**/.git": true,
        "**/.hg": true,
        "**/.svn": true,
        "**/.DS_Store": true,
        "**/*.swp": true,
        "**/*.swo": true,
        "**/*.bak": true,
        "**/*.orig": true,
        "**/.coverage": true,
        "**/.pytest_cache": true,
        "**/__pycache__": true,
        "**/.mypy_cache": true,
        "**/.ruff_cache": true,
        "build/": true
    },
    
    "tasks": "{}"
}
```

### 3. IntelliJ IDEA Configuration

```xml
<!-- .idea/cmake.xml -->
<project version="4">
  <component name="CMakeWorkspace">
    <option name="cmakeBuildType" value="Release" />
    <option name="external-policy-settings-managed" value="true" />
    <option name="build-shared-libraries" value="false" />
    <option name="generatorPath" value="$PROJECT_DIR$/cmake/ninja" />
  </component>
</project>
```

```xml
<!-- .idea/vcs.xml -->
<project version="4">
  <component name="VcsManager" SCOPE="ALL_PROJECTS" />
  <component name="Git.Settings" />
</project>
```

```xml
<!-- .idea/compass.xml -->
<project version="4">
  <component name="CompassSettings">
    <option name="auto-activate" value="false" />
  </component>
</project>
```

### 4. Makefile Development Script

```makefile
# Makefile
.PHONY: all clean build test docs ci help

# Variables
BUILD_DIR := build
SRC_DIR := src
TEST_DIR := tests
PYTHON_DIR := python
DOCS_DIR := docs
BUILD_TYPE := Release

# Default target
all: build test

# Build project
build:
	@echo "Building NLM project..."
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && \
	cmake .. \
	-DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
	-DCMAKE_CXX_STANDARD=20 \
	-DENABLE_TESTS=ON \
	-DENABLE_PYTHON=ON \
	-DBUILD_EXAMPLES=ON
	cd $(BUILD_DIR) && make -j$$(nproc)

# Install Python module
install-python:
	@echo "Installing Python module..."
	pip install -e .

# Run tests
# Run tests

test: build
	@echo "Running tests..."
	cd $(BUILD_DIR) && ctest --output-on-failure
	@echo "Running Python tests..."
	cd $(BUILD_DIR) && python -m pytest tests/ --cov=pynlm --cov-report=xml --cov-report=html
	cd $(BUILD_DIR) && python -m pytest python/ --cov=pynlm --cov-report=xml --cov-report=html

# Generate documentation
docs:
	@echo "Generating documentation..."
	doxygen Doxyfile
	sphinx-build -b html $(DOCS_DIR) $(DOCS_DIR)/_build/html

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(DOCS_DIR)/_build
	rm -f *.log
	rm -f *.pyc
	rm -f __pycache__/
	rm -rf *.egg-info

# Continuous Integration check
ci: all
	@echo "Running CI checks..."
	./scripts/ci_check.sh

# Help
help:
	@echo "Available targets:"
	@echo "  all      - Build and test the project (default)"
	@echo "  build    - Build the project"
	@echo "  install-python - Install Python module"
	@echo "  test     - Run tests"
	@echo "  docs     - Generate documentation"
		@echo "  clean    - Clean build artifacts"
	@echo "  ci       - Run CI checks"
	@echo "  help     - Show this help"
```

### 5. Pre-commit Hooks

```yaml
# .pre-commit-config.yaml
repos:
  - repo: https://github.com/pre-commit/pre-commit-hooks
    rev: v4.4.0
    hooks:
      - id: check-added-large-files
      - id: check-ast
      - id: check-builtin-deps
      - id: check-case-conflict
      - id: check-docstring-first
      - id: check-executables-have-shebang
      - id: check-json
      - id: check-merge-conflict
      - id: check-symlinks
      - id: check-toml
      - id: check-vcs-permalinks
      - id: check-yaml
      - id: debug-statements
      - id: end-of-file-fixer
      - id: fix-byte-order-marker
      - id: fix-encoding-utf-8
      - id: forbid-new-submodules
      - id: mixed-line-ending
      - id: requirements-txt-fixer
      - id: sort-simple-yaml
      - id: trailing-whitespace
      - id: check-case-conflict

  - repo: https://github.com/psf/black
    rev: 23.7.1
    hooks:
      - id: black
        args: [--line-length, '120']

  - repo: https://github.com/pycqa/isort
    rev: 5.12.0
    hooks:
      - id: isort
        args: [--line-length, '120']

  - repo: https://github.com/PyCQA/flake8
    rev: 6.1.0
    hooks:
      - id: flake8
        args: [--max-line-length, '120']

  - repo: https://github.com/pre-commit/mirrors-mypy
    rev: v1.6.0
    hooks:
      - id: mypy

  - repo: https://github.com/cpplint/cpplint
    rev: '1.6.0'
    hooks:
      - id: cpplint
        args: [--filter=-build/include_subdir,-build/c++11]

  - repo: https://github.com/Wandalena/mmark
    rev: v1.18
    hooks:
      - id: mmark

  - repo: https://github.com/tier4/pre-commit-hooks
    rev: v1.8.0
    hooks:
      - id: setup-hooks

  - repo: https://github.com/astrochords/pytorch-numpy-docs
    rev: v0.1.2
    hooks:
      - id: numpy-docs

ci:
  skip: [flake8, mypy, cpplint, numpy-docs]
```

## Build System Modernization

### 1. CMake Configuration

```cmake
# CMakeLists.txt - Modern CMake Configuration
cmake_minimum_required(VERSION 3.16)

project(NLM
    VERSION 0.1.0
    LANGUAGES CXX
    DESCRIPTION "Neural Learning Machine: An experimental artificial developmental brain"
)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Install directories
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install CACHE PATH "Installation directory")

# Package configuration
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Find dependencies
find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 REQUIRED)
find_package(SFML REQUIRED)
find_package(HDF5 REQUIRED)
find_package(Boost REQUIRED)

# Add subdirectories
add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(python)
add_subdirectory(docs)

# Export configuration
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/NLMConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NLM
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NLM
)
```

### 2. Conan Package Manager

```python
# conanfile.py
from conan import ConanFile
from conan.tools.scm import git_checkout
from conan.tools.layout import basic_layout
from conan.tools.files import copy
from conan.tools.build import cross_building

class NLMConan(ConanFile):
    name = "nlm"
    version = "0.1.0"
    license = "MIT"
    author = "NLM Authors"
    url = "https://github.com/nlm-project/nlm"
    description = "Neural Learning Machine: An experimental artificial developmental brain"
    topics = ("ai", "neural", "simulation", "brain", "learning")
    
    # Options
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "build_tests": [True, False],
        "build_python": [True, False],
        "build_examples": [True, False],
        "build_visualization": [True, False],
    }
    default_options = {
        "shared": False,
        "fPIC": True,
        "build_tests": True,
        "build_python": True,
        "build_examples": True,
        "build_visualization": True,
    }
    
    settings = "os", "compiler", "build_type", "arch"
    
    def requirements(self):
        self.requires("pybind11/2.11.0")
        self.requires("hdf5/1.14.0")
        self.requires("boost/1.83.0")
        self.requires("sfml/2.5.1")
        
        if self.options.build_tests:
            self.requires("catch2/2.13.8")
    
    def build_requirements(self):
        self.tool_requires("cmake/3.25.0")
        self.tool_requires("git/2.34.1")
    
    def layout(self):
        basic_layout(self)
    
    def build(self):
        cmake = CMake(self)
        cmake.configure(options={"CMAKE_BUILD_TYPE": self.settings.build_type})
        cmake.build()
    
    def package(self):
        cmake = CMake(self)
        cmake.install()
    
    def package_info(self):
        self.cpp_info.set_property("cmake_file_name", "NLM")
        self.cpp_info.set_property("cmake_target_name", "nlm::nlm")
        self.cpp_info.libs = ["nlm_core", "nlm_agent", "nlm_world", "nlm_phase3", "nlm_phase4"]
        
        if self.options.build_python:
            self.cpp_info.requires("pybind11")
        
        if self.settings.os == "Linux":
            self.cpp_info.system_libs = ["m", "pthread", "rt"]
        
        self.cpp_info.includedirs = ["include"]
```

### 3. Package Management Scripts

```bash
#!/bin/bash
# scripts/package_manager.sh

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

function show_help() {
    cat << EOF
Package Manager Script for NLM Project

Usage: $0 <command> [options]

Commands:
  build              Build the project
  install            Install the project
  package            Create distribution packages
  publish            Publish packages to repository
  test               Run tests
  clean              Clean build artifacts

Options:
  --build-type       Build type (Release/Debug)
  --prefix           Installation prefix
  --python           Enable Python bindings
  --visualization     Enable visualization
  --examples         Enable examples
  --tests            Enable tests
EOF
}

function build_project() {
    local build_type="${1:-Release}"
    local prefix="${2:-/usr/local}"
    local python="${3:-true}"
    local visualization="${4:-true}"
    local examples="${5:-true}"
    local tests="${6:-true}"
    
    echo "Building NLM project with type: $build_type"
    
    mkdir -p "$PROJECT_ROOT/build"
    cd "$PROJECT_ROOT/build"
    
    cmake .. \
        -DCMAKE_BUILD_TYPE="$build_type" \
        -DCMAKE_INSTALL_PREFIX="$prefix" \
        -DBUILD_PYTHON="$python" \
        -DBUILD_VISUALIZATION="$visualization" \
        -DBUILD_EXAMPLES="$examples" \
        -DBUILD_TESTS="$tests" \
        -DBUILD_DOCUMENTATION=true \
        -DENABLE_COVERAGE=false
    
    make -j$(nproc)
}

function install_project() {
    local prefix="${1:-/usr/local}"
    
    echo "Installing NLM project to: $prefix"
    
    cd "$PROJECT_ROOT/build"
    make install DESTDIR="$prefix"
}

function create_packages() {
    local package_type="${1:-all}"
    local version="${2:-$PROJECT_VERSION}"
    
    echo "Creating $package_type packages for version: $version"
    
    case "$package_type" in
        "deb")
            fpm --input-type deb \
                --output-type deb \
                --version "$version" \
                --architecture amd64 \
                --name "nlm" \
                --description "Neural Learning Machine" \
                --maintainer "NLM Authors" \
                --license "MIT" \
                --category "science"
            ;;
        "rpm")
            fpm --input-type rpm \
                --output-type rpm \
                --version "$version" \
                --release "1" \
                --arch "x86_64" \
                --name "nlm" \
                --description "Neural Learning Machine" \
                --license "MIT" \
                --category "science"
            ;;
        "python")
            python3 -m build
            ;;
        "docker")
            docker build -t "nlm:$version" .
            ;;
        *)
            echo "Unsupported package type: $package_type"
            exit 1
            ;;
    esac
}

function run_tests() {
    local test_type="${1:-all}"
    
    echo "Running $test_type tests"
    
    case "$test_type" in
        "unit")
            cd "$PROJECT_ROOT/build"
            ctest --output-on-failure -L "unit"
            ;;
        "integration")
            cd "$PROJECT_ROOT/build"
            ctest --output-on-failure -L "integration"
            ;;
        "performance")
            cd "$PROJECT_ROOT/build_perf"
            ./benchmarks/nlm_benchmark
            ;;
        "python")
            cd "$PROJECT_ROOT"
            python3 -m pytest python_tests/ --cov=pynlm
            ;;
        *)
            cd "$PROJECT_ROOT/build"
            ctest --output-on-failure
            ;;
    esac
}

function clean_artifacts() {
    echo "Cleaning build artifacts..."
    
    rm -rf "$PROJECT_ROOT/build"
    rm -rf "$PROJECT_ROOT/build_perf"
    rm -rf "$PROJECT_ROOT/install"
    rm -rf "$PROJECT_ROOT/dist"
    rm -rf "$PROJECT_ROOT/docs/_build"
    rm -rf "$PROJECT_ROOT/python/pynlm.egg-info"
    rm -rf "*.tar.gz"
    rm -rf "*.whl"
    rm -f "release/*"
    
    echo "Clean completed."
}

# Main script
command="${1:-help}"
shift

case "$command" in
    "build")
        build_project "$@"
        ;;
    "install")
        install_project "$@"
        ;;
    "package")
        create_packages "$@"
        ;;
    "publish")
        echo "Publish functionality not yet implemented"
        ;;
    "test")
        run_tests "$@"
        ;;
    "clean")
        clean_artifacts
        ;;
    *)
        show_help
        exit 1
        ;;
esac
```

## Release Management

### 1. Version Control

```bash
#!/bin/bash
# scripts/version_control.sh

set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

function show_version() {
    local version_file="${PROJECT_ROOT}/VERSION"
    
    if [[ -f "$version_file" ]]; then
        cat "$version_file"
    else
        echo "Version file not found"
        exit 1
    fi
}

function bump_version() {
    local version_file="${PROJECT_ROOT}/VERSION"
    local part="${1:-patch}"
    
    if [[ ! -f "$version_file" ]]; then
        echo "Version file not found"
        exit 1
    fi
    
    local current_version="$(cat "$version_file")"
    
    case "$part" in
        "major")
            new_version="$(echo "$current_version" | awk -F. '{print $1+1".0.0"}')"
            ;;
        "minor")
            new_version="$(echo "$current_version" | awk -F. '{print $1+"."$2+1".0"}')"
            ;;
        "patch")
            new_version="$(echo "$current_version" | awk -F. '{print $1+"."$2+"."$3+1}')"
            ;;
        "rc")
            new_version="$(echo "$current_version" | awk -F. '{print $1+"."$2+"."$3+"-rc.1"}')"
            ;;
        *)
            echo "Invalid version part: $part"
            exit 1
            ;;
    esac
    
    echo "$new_version" > "$version_file"
    echo "Version bumped to: $new_version"
}

function create_release_branch() {
    local version="${1:-$PROJECT_VERSION}"
    local branch_name="release/v${version}"
    
    git checkout -b "$branch_name"
    git add VERSION
    git commit -m "Bump version to $version"
    
    echo "Created release branch: $branch_name"
}

function merge_release() {
    local version="${1:-$PROJECT_VERSION}"
    local branch_name="release/v${version}"
    
    git checkout main
    git merge "$branch_name"
    git tag "v$version"
    git push origin main --tags
    
    echo "Merged release: v$version"
}

# Main script
command="${1:-show}"
shift

case "$command" in
    "show")
        show_version
        ;;
    "bump")
        bump_version "$@"
        ;;
    "create-branch")
        create_release_branch "$@"
        ;;
    "merge")
        merge_release "$@"
        ;;
    *)
        echo "Invalid command: $command"
        exit 1
        ;;
esac
```

### 2. Docker Compose for Development

```yaml
# docker-compose.yml
dversion: '3.8'

services:
  nlm-dev:
    build:
      context: .
      dockerfile: Dockerfile.dev
    image: nlm-dev:latest
    container_name: nlm-dev
    volumes:
      - ./:/workspace/nlm
      - ~/.gitconfig:/root/.gitconfig
    working_dir: /workspace/nlm
    command: ["tail", "-f", "/dev/null"]
    environment:
      - DISPLAY=${DISPLAY}
      - QT_X11_NO_MITSHM=1
      - PYTHONPATH=/workspace/nlm/build:$PYTHON_PATH
    depends_on:
      - database
    ports:
      - "8080:8080"
      - "3000:3000"
    networks:
      - nlm-network
    profiles:
      - develop

  nlm-database:
    image: postgres:15
    container_name: nlm-database
    environment:
      - POSTGRES_DB=nlm
      - POSTGRES_USER=nlm
      - POSTGRES_PASSWORD=nlm_password
      - POSTGRES_PORT=5432
    volumes:
      - postgres_data:/var/lib/postgresql/data
    ports:
      - "5432:5432"
    networks:
      - nlm-network
    profiles:
      - develop

  nlm-redis:
    image: redis:7
    container_name: nlm-redis
    ports:
      - "6379:6379"
    volumes:
      - redis_data:/data
    networks:
      - nlm-network
    profiles:
      - develop

  nlm-monitoring:
    build:
      context: .
      dockerfile: Dockerfile.monitoring
    image: nlm-monitoring:latest
    container_name: nlm-monitoring
    volumes:
      - ./monitoring:/workspace/nlm/monitoring
    environment:
      - NLM_HOST=nlm-dev
      - NLM_PORT=8080
    ports:
      - "3001:3001"
    depends_on:
      - nlm-dev
    networks:
      - nlm-network
    profiles:
      - monitoring

  nlm-visualization:
    build:
      context: .
      dockerfile: Dockerfile.visualization
    image: nlm-visualization:latest
    container_name: nlm-visualization
    volumes:
      - ./visualization:/workspace/nlm/visualization
    environment:
      - NLM_HOST=nlm-dev
      - NLM_PORT=8080
    ports:
      - "8080:8080"
    depends_on:
      - nlm-dev
    networks:
      - nlm-network
    profiles:
      - visualization

volumes:
  postgres_data:
  redis_data:

networks:
  nlm-network:
    driver: bridge
```

### 3. Deployment Scripts

```bash
#!/bin/bash
# scripts/deploy.sh

set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

function deploy_local() {
    local install_prefix="${1:-/opt/nlm}"
    
    echo "Deploying NLM locally to: $install_prefix"
    
    # Create installation directory
    sudo mkdir -p "$install_prefix"
    sudo chown -R $(whoami):$(whoami) "$install_prefix"
    
    # Copy files
    cp -r "$PROJECT_ROOT/build/bin" "$install_prefix/bin"
    cp -r "$PROJECT_ROOT/build/lib" "$install_prefix/lib"
    cp -r "$PROJECT_ROOT/install" "$install_prefix/"
    
    # Create symbolic links
    sudo ln -sf "$install_prefix/bin/nlm" /usr/local/bin/nlm
    sudo ln -sf "$install_prefix/bin/nlm_phase3_demo" /usr/local/bin/nlm_phase3_demo
    sudo ln -sf "$install_prefix/bin/nlm_phase4_demo" /usr/local/bin/nlm_phase4_demo
    
    # Install Python module
    pip3 install -e .
    
    echo "Deployment completed successfully"
}

function deploy_docker() {
    local tag="${1:-latest}"
    
    echo "Deploying NLM with Docker tag: $tag"
    
    docker build -t "nlm:$tag" .
    docker run -d --name nlm-container -p 8080:8080 "nlm:$tag"
    
    echo "Docker deployment completed"
}

function deploy_kubernetes() {
    local namespace="${1:-nlm}"
    local version="${2:-latest}"
    
    echo "Deploying NLM to Kubernetes namespace: $namespace"
    
    kubectl create namespace "$namespace"
    
    # Apply Kubernetes manifests
    kubectl apply -k k8s/ -n "$namespace"
    
    # Wait for deployment
    kubectl wait --for=condition=available --timeout=300s deployment/nlm -n "$namespace"
    
    echo "Kubernetes deployment completed"
}

function show_status() {
    echo "NLM Deployment Status"
    echo "====================="
    
    # Check local installation
    if [[ -f "/usr/local/bin/nlm" ]]; then
        echo "Local installation: OK"
        echo "  NLM executable: $(/usr/local/bin/nlm --version 2>/dev/null || echo 'not available')"
    else
        echo "Local installation: Not found"
    fi
    
    # Check Docker containers
    if docker ps --filter name=nlm-container --format '{{.Names}}' | grep -q nlm-container; then
        echo "Docker container: OK"
        echo "  Container ID: $(docker ps --filter name=nlm-container --format '{{.ID}}')"
    else
        echo "Docker container: Not running"
    fi
    
    # Check Python installation
    if python3 -c "import pynlm; print('Python bindings: OK')" 2>/dev/null; then
        echo "Python bindings: OK"
        echo "  Version: $(python3 -c 'import pynlm; print(pynlm.__version__)' 2>/dev/null || echo 'unknown')"
    else
        echo "Python bindings: Not available"
    fi
}

# Main script
command="${1:-help}"
shift

case "$command" in
    "local")
        deploy_local "$@"
        ;;
    "docker")
        deploy_docker "$@"
        ;;
    "kubernetes")
        deploy_kubernetes "$@"
        ;;
    "status")
        show_status
        ;;
    *)
        echo "Usage: $0 {local|docker|kubernetes|status}"
        exit 1
        ;;
esac
```

## Development Workflow Tools

### 1. Environment Setup Script

```bash
#!/bin/bash
# scripts/setup_development_environment.sh

set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

function install_dependencies() {
    echo "Installing NLM development dependencies..."
    
    # Detect package manager
    if command -v apt-get &> /dev/null; then
        install_debian_dependencies
    elif command -v yum &> /dev/null; then
        install_rhel_dependencies
    elif command -v dnf &> /dev/null; then
        install_fedora_dependencies
    else
        echo "Unsupported package manager. Please install dependencies manually."
        exit 1
    fi
}

function install_debian_dependencies() {
    echo "Installing Debian/Ubuntu dependencies..."
    
    sudo apt-get update
    sudo apt-get install -y --no-install-recommends
        build-essential
        cmake
        ninja-build
        g++
        gcc
        make
        python3
        python3-pip
        python3-venv
        git
        curl
        wget
        unzip
        htop
        tree
        valgrind
        doxygen
        graphviz
        libqt5core5lib
        libqt5gui5
        libqt5widgets5
        libsfml-dev
        libhdf5-dev
        libboost-all-dev
        pkg-config
    
    echo "Debian dependencies installed successfully"
}

function install_rhel_dependencies() {
    echo "Installing RHEL/CentOS dependencies..."
    
    sudo yum update -y
    sudo yum install -y --no-install-recommends
        gcc
        gcc-c++
        make
        cmake3
        ninja-build
        python3
        python3-pip
        python3-devel
        git
        curl
        wget
        unzip
        htop
        tree
        valgrind
        doxygen
        graphviz
        qt5-qtbase-devel
        qt5-qttools-devel
        sfml-devel
        hdf5-devel
        boost-devel
        pkgconfig
    
    echo "RHEL dependencies installed successfully"
}

function install_fedora_dependencies() {
    echo "Installing Fedora dependencies..."
    
    sudo dnf update -y
    sudo dnf install -y --no-install-recommends
        gcc
        gcc-c++
        make
        cmake
        ninja-build
        python3
        python3-pip
        python3-devel
        git
        curl
        wget
        unzip
        htop
        tree
        valgrind
        doxygen
        graphviz
        qt5-qtbase-devel
        qt5-qttools-devel
        sfml-devel
        hdf5-devel
        boost-devel
        pkgconfig
    
    echo "Fedora dependencies installed successfully"
}

function setup_python_environment() {
    echo "Setting up Python development environment..."
    
    python3 -m venv ".venv"
    source ".venv/bin/activate"
    
    python3 -m pip install --upgrade pip
    python3 -m pip install --no-cache-dir
        wheel
        setuptools
        scikit-build-core
        pybind11
        pytest
        pytest-cov
        pytest-benchmark
        pytest-xdist
        black
        flake8
        isort
        mypy
        pre-commit
        numpy
        matplotlib
        seaborn
        plotly
        ipython
    
    echo "Python development environment setup completed"
}

function setup_cmake_config() {
    echo "Setting up CMake configuration..."
    
    mkdir -p "$PROJECT_ROOT/build"
    cd "$PROJECT_ROOT/build"
    
    cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_CXX_STANDARD=20 \
        -DENABLE_TESTS=ON \
        -DENABLE_PYTHON=ON \
        -DBUILD_EXAMPLES=ON \
        -DBUILD_VISUALIZATION=ON \
        -DBUILD_DOCUMENTATION=ON \
        -DBUILD_TESTS=ON \
        -DBUILD_PYTHON_TESTS=ON \
        -DENABLE_COVERAGE=OFF \
        -DENABLE_VALGRIND=OFF
    
    echo "CMake configuration completed"
}

function setup_git_config() {
    echo "Setting up Git configuration..."
    
    if ! git config user.name > /dev/null 2>&1; then
        echo "Please configure your Git username and email"
        echo "Example: git config --global user.name 'Your Name'"
        echo "Example: git config --global user.email 'your.email@example.com'"
    fi
    
    git config core.editor "vim"
    git config core.fileMode true
    git config core.ignoreCase false
    
    echo "Git configuration completed"
}

function setup_pre_commit_hooks() {
    echo "Setting up pre-commit hooks..."
    
    if [[ -f "$PROJECT_ROOT/.pre-commit-config.yaml" ]]; then
        pre-commit install
        echo "Pre-commit hooks installed successfully"
    else
        echo "Pre-commit configuration not found"
    fi
}

function show_setup_summary() {
    echo "NLM Development Environment Setup Summary"
    echo "=========================================="
    echo ""
    echo "Dependencies installed:"
    echo "  - Build tools (gcc, g++, make, cmake)"
    echo "  - Visualization libraries (Qt, SFML)"
    echo "  - Python package manager (pip)"
    echo "  - Development tools (git, htop, tree)"
    echo ""
    echo "Python packages installed:"
    echo "  - build system (scikit-build-core)"
    echo "  - bindings (pybind11)"
    echo "  - testing (pytest, pytest-cov, pytest-benchmark)"
    echo "  - code quality (black, flake8, isort, mypy)"
    echo "  - visualization (matplotlib, seaborn, plotly)"
    echo ""
    echo "Environment setup completed successfully!"
    echo ""
    echo "To activate the development environment, run:"
    echo "  source .venv/bin/activate"
    echo ""
    echo "To build the project, run:"
    echo "  cd build && make -j$(nproc)"
    echo ""
    echo "To run tests, run:"
    echo "  cd build && ctest --output-on-failure"
    echo ""
    echo "To run Python tests, run:"
    echo "  pytest tests/ --cov=pynlm"
}

# Main script
function main() {
    echo "NLM Development Environment Setup"
    echo "=================================="
    echo ""
    
    # Install dependencies
    install_dependencies
    
    # Setup Python environment
    setup_python_environment
    
    # Setup CMake configuration
    setup_cmake_config
    
    # Setup Git configuration
    setup_git_config
    
    # Setup pre-commit hooks
    setup_pre_commit_hooks
    
    # Show setup summary
    show_setup_summary
}

# Run main function
main
```

## Quality Assurance Tools

### 1. Code Quality Checker

```python
# scripts/quality_checker.py
import subprocess
import sys
import os
from pathlib import Path

def run_black_check():
    """Run Black code formatting check"""
    print("Running Black formatting check...")
    result = subprocess.run(
        ["black", "--check", "--line-length", "120", "."],
        capture_output=True,
        text=True
    )
    if result.returncode != 0:
        print("Black formatting check failed:")
        print(result.stdout)
        print(result.stderr)
        return False
    print("Black formatting check passed")
    return True

def run_isort_check():
    """Run isort import sorting check"""
    print("Running isort check...")
    result = subprocess.run(
        ["isort", "--check-only", "--line-length", "120", "."],
        capture_output=True,
        text=True
    )
    if result.returncode != 0:
        print("isort check failed:")
        print(result.stdout)
        print(result.stderr)
        return False
    print("isort check passed")
    return True

def run_flake8_check():
    """Run flake8 linting check"""
    print("Running flake8 linting check...")
    result = subprocess.run(
        ["flake8", "--max-line-length", "120", "--extend-ignore", "E203,W503", "."],
        capture_output=True,
        text=True
    )
    if result.returncode != 0:
        print("flake8 check failed:")
        print(result.stdout)
        print(result.stderr)
        return False
    print("flake8 check passed")
    return True

def run_mypy_check():
    """Run mypy type checking"""
    print("Running mypy type checking...")
    result = subprocess.run(
        ["mypy", "--ignore-missing-imports", "--show-error-codes", "."],
        capture_output=True,
        text=True
    )
    if result.returncode != 0:
        print("mypy check failed:")
        print(result.stdout)
        print(result.stderr)
        return False
    print("mypy check passed")
    return True

def run_clang_format_check():
    """Run clang-format check"""
    print("Running clang-format check...")
    cpp_files = []
    for root, dirs, files in os.walk("."):
        # Skip hidden directories
        dirs[:] = [d for d in dirs if not d.startswith('.')]
        
        for file in files:
            if file.endswith(('.cpp', '.hpp', '.h')):
                cpp_files.append(os.path.join(root, file))
    
    result = subprocess.run(
        ["clang-format", "--dry-run", "--style=file"] + cpp_files,
        capture_output=True,
        text=True
    )
    if result.returncode != 0:
        print("clang-format check failed:")
        print(result.stdout)
        print(result.stderr)
        return False
    print("clang-format check passed")
    return True

def run_clang_tidy_check():
    """Run clang-tidy analysis"""
    print("Running clang-tidy analysis...")
    
    if not os.path.exists("build"):
        print("Building project first...")
        subprocess.run(["make", "-j$(nproc)"], check=True)
    
    result = subprocess.run(
        ["clang-tidy", "-p", "build", "src/brain/Brain.cpp", "src/brain/Neuron.cpp", "src/brain/Synapse.cpp"],
        capture_output=True,
        text=True
    )
    if result.returncode != 0:
        print("clang-tidy check failed:")
        print(result.stdout)
        print(result.stderr)
        return False
    print("clang-tidy check passed")
    return True

def main():
    print("NLM Code Quality Checker")
    print("========================")
    
    checks = [
        ("Black formatting", run_black_check),
        ("isort import sorting", run_isort_check),
        ("flake8 linting", run_flake8_check),
        ("mypy type checking", run_mypy_check),
        ("clang-format style", run_clang_format_check),
        ("clang-tidy analysis", run_clang_tidy_check),
    ]
    
    passed = 0
    total = len(checks)
    
    for check_name, check_func in checks:
        print(f"\n{check_name}:")
        if check_func():
            passed += 1
        else:
            print(f"  FAILED: {check_name}")
    
    print(f"\nSummary: {passed}/{total} checks passed")
    
    if passed == total:
        print("All code quality checks passed!")
        return 0
    else:
        print("Some code quality checks failed.")
        return 1

if __name__ == "__main__":
    sys.exit(main())
```

### 2. Security Scanner

```python
# scripts/security_scanner.py
import subprocess
import sys
import json
from pathlib import Path

def run_safety_check():
    """Run Safety security vulnerability scanner"""
    print("Running Safety security check...")
    
    result = subprocess.run(
        ["safety", "check", "--json"],
        capture_output=True,
        text=True
    )
    
    try:
        vulnerabilities = json.loads(result.stdout)
        if vulnerabilities:
            print(f"Found {len(vulnerabilities)} security vulnerabilities")
            for vuln in vulnerabilities:
                print(f"  - {vuln['package']} {vuln['version']}: {vuln['advisory']}")
            return False
        else:
            print("No security vulnerabilities found")
            return True
    except json.JSONDecodeError:
        print("Failed to parse Safety output")
        return False

def run_trivy_scan():
    """Run Trivy vulnerability scanner"""
    print("Running Trivy vulnerability scan...")
    
    result = subprocess.run(
        ["trivy", "fs", ".", "--format", "json", "--output", "trivy-results.json"],
        capture_output=True,
        text=True
    )
    
    if result.returncode != 0:
        print("Trivy scan failed:")
        print(result.stdout)
        print(result.stderr)
        return False
    
    try:
        with open("trivy-results.json", "r") as f:
            scan_results = json.load(f)
        
        vulnerabilities = scan_results.get("results", [])
        if vulnerabilities:
            print(f"Found {len(vulnerabilities)} vulnerabilities")
            return False
        else:
            print("No vulnerabilities found")
            return True
    except Exception as e:
        print(f"Failed to parse Trivy results: {e}")
        return False

def main():
    print("NLM Security Scanner")
    print("====================")
    
    checks = [
        ("Safety security check", run_safety_check),
        ("Trivy vulnerability scan", run_trivy_scan),
    ]
    
    passed = 0
    total = len(checks)
    
    for check_name, check_func in checks:
        print(f"\n{check_name}:")
        if check_func():
            passed += 1
        else:
            print(f"  FAILED: {check_name}")
    
    print(f"\nSummary: {passed}/{total} security checks passed")
    
    if passed == total:
        print("All security checks passed!")
        return 0
    else:
        print("Security checks failed.")
        return 1

if __name__ == "__main__":
    sys.exit(main())
```

## Performance Testing Framework

### 1. Performance Benchmark Script

```bash
#!/bin/bash
# scripts/benchmark_performance.sh

set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

function run_neural_benchmark() {
    local benchmark_dir="${1:-build_perf}"
    
    echo "Running neural computation benchmark..."
    
    cd "$benchmark_dir"
    ./benchmarks/nlm_neural_benchmark
    
    # Generate benchmark report
    python3 scripts/analyze_benchmark.py benchmarks/neural_benchmark.json
}

function run_memory_benchmark() {
    local benchmark_dir="${1:-build_perf}"
    
    echo "Running memory system benchmark..."
    
    cd "$benchmark_dir"
    ./benchmarks/nlm_memory_benchmark
    
    # Generate benchmark report
    python3 scripts/analyze_benchmark.py benchmarks/memory_benchmark.json
}

function run_integration_benchmark() {
    local benchmark_dir="${1:-build_perf}"
    
    echo "Running integration benchmark..."
    
    cd "$benchmark_dir"
    ./benchmarks/nlm_integration_benchmark
    
    # Generate benchmark report
    python3 scripts/analyze_benchmark.py benchmarks/integration_benchmark.json
}

function run_python_performance_benchmark() {
    echo "Running Python performance benchmark..."
    
    cd "$PROJECT_ROOT"
    python3 -m pytest python_tests/ -v --benchmark-save=python_benchmarks.json
    
    # Generate benchmark report
    python3 scripts/analyze_benchmark.py python_benchmarks.json
}

function analyze_performance_results() {
    echo "Analyzing performance results..."
    
    python3 scripts/generate_performance_report.py
    python3 scripts/compare_performance.py
    python3 scripts/generate_performance_trends.py
}

function generate_performance_dashboard() {
    echo "Generating performance dashboard..."
    
    python3 scripts/create_performance_dashboard.py
    python3 scripts/create_performance_charts.py
    python3 scripts/create_performance_summary.py
}

# Main script
function main() {
    local benchmark_type="${1:-all}"
    
    echo "NLM Performance Benchmark Suite"
    echo "==============================="
    
    case "$benchmark_type" in
        "neural")
            run_neural_benchmark
            ;;
        "memory")
            run_memory_benchmark
            ;;
        "integration")
            run_integration_benchmark
            ;;
        "python")
            run_python_performance_benchmark
            ;;
        "all")
            # Create performance build directory
            mkdir -p "$PROJECT_ROOT/build_perf"
            
            # Run all benchmarks
            run_neural_benchmark "$PROJECT_ROOT/build_perf"
            run_memory_benchmark "$PROJECT_ROOT/build_perf"
            run_integration_benchmark "$PROJECT_ROOT/build_perf"
            run_python_performance_benchmark
            
            # Analyze results
            analyze_performance_results
            
            # Generate dashboard
            generate_performance_dashboard
            ;;
        *)
            echo "Invalid benchmark type: $benchmark_type"
            echo "Usage: $0 {neural|memory|integration|python|all}"
            exit 1
            ;;
    esac
}

main "$@"
```

## Project Setup and Configuration

### 1. Environment Configuration

```yaml
# .kilo/command/settings.yml
# Kilo development environment settings

environment:
  project: "nlm"
  version: "0.1.0"
  python_version: "3.11"
  cxx_standard: "20"
  build_type: "Release"
  
  paths:
    build_dir: "build"
    src_dir: "src"
    test_dir: "tests"
    python_dir: "python"
    docs_dir: "docs"
    examples_dir: "examples"
    visualization_dir: "visualization"
    monitoring_dir: "monitoring"
    debug_dir: "debug"
    
    scripts:
      - "scripts/"
      - "bin/"
      - ".kilo/command/"
      
  tools:
    cmake: "cmake"
    ninja: "ninja"
    g++: "g++"
    python: "python3"
    pip: "pip3"
    git: "git"
    docker: "docker"
    jq: "jq"
    curl: "curl"
    wget: "wget"
    
  configuration:
    build_type: "Release"
    enable_tests: true
    enable_python: true
    enable_visualization: true
    enable_monitoring: true
    enable_debug: true
    enable_documentation: true
    enable_coverage: false
    enable_valgrind: false
    
  features:
    - "neural_computation"
    - "plasticity"
    - "memory_systems"
    - "prediction"
    - "cognition"
    - "neuromodulation"
    - "development"
    - "integration"
    - "visualization"
    - "monitoring"
    - "debugging"
    - "python_bindings"
    - "performance_benchmarking"
    - "continuous_integration"
    - "continuous_deployment"
    - "security_scanning"
    - "quality_assurance"
```

### 2. Development Environment Configuration

```json
// .kilo/dev-environment.json
{
  "name": "nlm-dev",
  "image": "ubuntu:22.04",
  "environment": {
    "DEBIAN_FRONTEND": "noninteractive",
    "DEBIAN_PRIORITY": "minimal",
    "PYTHONPATH": "/workspace/nlm/build:$PYTHONPATH",
    "LD_LIBRARY_PATH": "/workspace/nlm/build/lib:$LD_LIBRARY_PATH",
    "PKG_CONFIG_PATH": "/workspace/nlm/build/lib/pkgconfig:$PKG_CONFIG_PATH"
  },
  "volumes": [
    "./:/workspace/nlm",
    "./.gitconfig:/root/.gitconfig",
    "~/.cache/pip:/root/.cache/pip",
    "~/.m2:/root/.m2"
  ],
  "working_dir": "/workspace/nlm",
  "command": "apt-get update && apt-get install -y --no-install-recommends && apt-get clean && rm -rf /var/lib/apt/lists/*",
  "entrypoint": ["/bin/bash"],
  "profiles": ["develop", "monitoring", "visualization"],
  "resources": {
    "limits": {
      "cpus": "2",
      "memory": "4g"
    }
  }
}
```

## Summary

This modern development workflow for NLM includes:

### 1. **CI/CD Pipeline**
- Comprehensive GitHub Actions workflow with multiple stages
- GitLab CI configuration for self-hosted runners
- Jenkins pipeline for enterprise environments
- Automated testing, building, and deployment

### 2. **Development Environment**
- Docker-based development environment
- VS Code and IntelliJ IDEA configurations
- Pre-commit hooks and code quality checks
- Comprehensive environment setup scripts

### 3. **Build System Modernization**
- CMake-based build system with modern features
- Conan package manager for dependency management
- Cross-platform build support
- Performance-optimized builds

### 4. **Testing Infrastructure**
- Unit testing framework with comprehensive coverage
- Integration testing for system connectivity
- Performance benchmarking and analysis
- Python binding testing and validation

### 5. **Quality Assurance**
- Automated code quality checks (Black, isort, flake8, mypy)
- Static analysis with clang-format and clang-tidy
- Security vulnerability scanning with Trivy and Safety
- Performance profiling and optimization

### 6. **Documentation and Reporting**
- Automated documentation generation (Doxygen, Sphinx)
- API documentation and user guides
- Performance and analysis reports
- Visualization and monitoring dashboards

### 7. **Release Management**
- Version control and branching strategy
- Package creation for multiple platforms
- Docker image management
- Automated deployment to various environments

### 8. **Continuous Improvement**
- Performance monitoring and analysis
- Automated testing and validation
- Security scanning and vulnerability management
- Quality assurance and code reviews

This framework provides a robust, modern development workflow that supports the complex requirements of the NLM artificial brain simulation project, ensuring high-quality code, comprehensive testing, and efficient deployment while maintaining the flexibility to adapt to future requirements and improvements.