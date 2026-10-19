#!/bin/bash

# NLM Development Environment Setup Script
# Usage: ./ci/setup-ci.sh

# Exit on error
set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Print colored output
print_info() {
    echo -e "${GREEN}[CI-INFO]${NC} $1"
}

print_warn() {
    echo -e "${YELLOW}[CI-WARN]${NC} $1"
}

print_error() {
    echo -e "${RED}[CI-ERROR]${NC} $1"
}

main() {
    echo "NLM CI/CD Environment Setup"
    echo "=========================="
    echo

    print_info "Setting up CI/CD environment..."

    # Check for required CI tools
    if ! command -v git &> /dev/null; then
        print_error "Git not found. Required for CI/CD setup."
        exit 1
    fi

    # Create CI configuration files
    create_ci_configs

    # Set up GitHub Actions
    setup_github_actions

    # Set up GitLab CI
    setup_gitlab_ci

    # Set up pre-commit
    setup_pre_commit

    print_info "CI/CD environment setup completed."
    echo
    echo "Configuration files created:"
    echo "  .github/workflows/ci.yml"
    echo "  .gitlab-ci.yml"
    echo "  .pre-commit-config.yaml"
}

create_ci_configs() {
    print_info "Creating CI configuration files..."

    # Create GitHub Actions workflow
    mkdir -p .github/workflows

    cat > .github/workflows/ci.yml << 'EOF'
name: NLM CI/CD

on:
  push:
    branches: [main, develop]
  pull_request:
    branches: [main, develop]
  schedule:
    - cron: '0 2 * * *'  # Daily at 2 AM UTC

env:
  PYTHON_VERSION: '3.11'
  CI_BUILD_TYPE: 'Release'

jobs:
  test:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [ubuntu-latest, macos-latest]
        python-version: ['3.8', '3.9', '3.10', '3.11', '3.12']
        exclude:
          - os: ubuntu-latest
            python-version: '3.8'
          - os: ubuntu-latest
            python-version: '3.9'
          - os: ubuntu-latest
            python-version: '3.10'
          - os: ubuntu-latest
            python-version: '3.12'

    steps:
    - name: Checkout code
      uses: actions/checkout@v4
      with:
        fetch-depth: 0

    - name: Set up Python ${{ matrix.python-version }}
      uses: actions/setup-python@v4
      with:
        python-version: ${{ matrix.python-version }}

    - name: Install system dependencies
      run: |
        if [[ "$RUNNER_OS" == "Linux" ]]; then
          sudo apt-get update
          sudo apt-get install -y \
            build-essential \
            cmake \
            libpython3-dev \
            python3-pip
        elif [[ "$RUNNER_OS" == "macOS" ]]; then
          brew install cmake python3
        fi

    - name: Install Python dependencies
      run: |
        python -m pip install --upgrade pip setuptools wheel
        pip install -e .[dev]

    - name: Configure CMake
      run: |
        mkdir build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=${{ env.CI_BUILD_TYPE }}

    - name: Build project
      run: |
        cd build
        make -j$(nproc)

    - name: Run C++ tests
      run: |
        cd build
        ./nlm_test
        ctest --output-on-failure

    - name: Run Python tests
      run: |
        cd python
        pytest tests/ -v --cov=pynlm --cov-report=xml

    - name: Run linting and formatting
      run: |
        black --check python/ src/
        ruff check python/ src/
        mypy python/

    - name: Upload coverage reports
      uses: codecov/codecov-action@v3
      if: success()

  build-and-test:
    runs-on: ubuntu-latest
    steps:
    - name: Checkout code
      uses: actions/checkout@v4

    - name: Set up Python
      uses: actions/setup-python@v4
      with:
        python-version: '3.11'

    - name: Install system dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y build-essential cmake

    - name: Install Python dependencies
      run: |
        python -m pip install --upgrade pip setuptools wheel
        pip install -e .[dev]

    - name: Build and test
      run: |
        mkdir build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Release
        make -j$(nproc)
        ./nlm_test
        ctest --output-on-failure

    - name: Test Python bindings
      run: |
        cd python
        pytest tests/ -v --benchmark

  benchmarks:
    runs-on: ubuntu-latest
    if: github.event.schedule != null

    steps:
    - name: Checkout code
      uses: actions/checkout@v4

    - name: Set up Python
      uses: actions/setup-python@v4
      with:
        python-version: '3.11'

    - name: Install dependencies
      run: |
        python -m pip install --upgrade pip setuptools wheel
        pip install -e .[dev]

    - name: Run benchmarks
      run: |
        cd python
        pytest tests/ -v -m benchmark --benchmark-save=benchmarks.json
        pytest --benchmark-compare

    - name: Upload benchmarks
      uses: actions/upload-artifact@v3
      with:
        name: benchmarks
        path: python/benchmarks.json
        retention-days: 90

  coverage:
    runs-on: ubuntu-latest
    if: always()

    steps:
    - name: Checkout code
      uses: actions/checkout@v4

    - name: Set up Python
      uses: actions/setup-python@v4
      with:
        python-version: '3.11'

    - name: Install dependencies
      run: |
        python -m pip install --upgrade pip setuptools wheel
        pip install -e .[dev]

    - name: Generate coverage report
      run: |
        cd python
        pytest --cov=pynlm --cov-report=html --cov-report=xml

    - name: Upload coverage HTML
      uses: actions/upload-artifact@v3
      with:
        name: coverage-html
        path: python/htmlcov/
        retention-days: 30

    - name: Upload coverage XML
      uses: actions/upload-artifact@v3
      with:
        name: coverage-xml
        path: python/coverage.xml
        retention-days: 30

    - name: Upload coverage JSON
      uses: actions/upload-artifact@v3
      with:
        name: coverage-json
        path: python/.coverage.json
        retention-days: 30
EOF

    print_info "GitHub Actions workflow created."

    # Create GitLab CI configuration
    cat > .gitlab-ci.yml << 'EOF'
stages:
  - test
  - build
  - deploy

variables:
  PYTHON_VERSION: '3.11'
  CI_BUILD_TYPE: 'Release'

image: python:${PYTHON_VERSION}-alpine

before_script:
  - apk add --no-cache gcc g++ make cmake git
  - python -m pip install --upgrade pip setuptools wheel

linting_and_formatting:
  stage: test
  script:
    - pip install -e .[dev]
    - black --check python/ src/
    - ruff check python/ src/
    - mypy python/
  artifacts:
    reports:
      junit: linting-report.xml
  only:
    - merge_requests
    - main

unit_tests:
  stage: test
  script:
    - mkdir build
    - cd build
    - cmake .. -DCMAKE_BUILD_TYPE=${CI_BUILD_TYPE}
    - make -j$(nproc)
    - ./nlm_test
    - ctest --output-on-failure
    - cd ../python
    - pytest tests/ -v --cov=pynlm --cov-report=xml
  artifacts:
    reports:
      junit: test-results.xml
      coverage.xml: python/coverage.xml
    paths:
      - build/
      - python/htmlcov/
    when: always

build_and_package:
  stage: build
  script:
    - mkdir build
    - cd build
    - cmake .. -DCMAKE_BUILD_TYPE=${CI_BUILD_TYPE}
    - make -j$(nproc)
    - cd ../python
    - python -m build
  artifacts:
    paths:
      - build/
      - dist/
      - python/dist/
    expire_in: 1 week
  only:
    - tags
    - main

deploy:
  stage: deploy
  script:
    - echo "Deploying to production..."
  only:
    - tags
    - main
  when: manual
EOF

    print_info "GitLab CI configuration created."

    # Create pre-commit configuration
    cat > .pre-commit-config.yaml << 'EOF'
repos:
  - repo: https://github.com/pre-commit/pre-commit-hooks
    rev: v4.4.0
    hooks:
      - id: check-added-large-files
      - id: check-ast
      - id: check-builtin-literals
      - id: check-case-conflict
      - id: check-docstring-first
      - id: check-executables-have-shebangs
      - id: check-json
      - id: check-merge-conflict
      - id: check-symlinks
      - id: check-toml
      - id: check-vcs-permalinks
      - id: check-xml
      - id: check-yaml
      - id: debug-statements
      - id: detect-aws-credentials
      - id: detect-private-key
      - id: end-of-file-fixer
      - id: forbid-submodules
      - id: mixed-line-ending
      - id: trailing-whitespace

  - repo: https://github.com/astral-sh/ruff-pre-commit
    rev: v0.1.7
    hooks:
      - id: ruff
        args: [--fix, --exit-non-zero-on-fix]
      - id: ruff-format

  - repo: https://github.com/psf/black
    rev: 23.7.0
    hooks:
      - id: black

  - repo: https://github.com/pycqa/isort
    rev: 5.12.0
    hooks:
      - id: isort

  - repo: https://github.com/pre-commit/mirrors-mypy
    rev: v1.7.0
    hooks:
      - id: mypy

  - repo: local
    hooks:
      - id: test-python
        name: Run Python tests
        entry: bash -c
        args:
          - |
            cd python &&
            pytest tests/ -v --no-cov
        language: system
        pass_filenames: false
        types: [python]

      - id: test-cpp
        name: Run C++ tests
        entry: bash -c
        args:
          - |
            if [ -d "build" ]; then
              cd build &&
              ./nlm_test || ctest --output-on-failure
            fi
        language: system
        pass_filenames: false
        files: \.cpp$

  - repo: https://github.com/Yelp/detect-secrets
    rev: v1.4.0
    hooks:
      - id: detect-secrets
        args: [--baseline, .secrets.baseline]
EOF

    print_info "Pre-commit configuration created."

    # Create secrets baseline
    cat > .secrets.baseline << 'EOF'
{
  "version": "1.0.0",
  "generated_at": "2024-01-19T16:15:41Z",
  "engine_version": "1.4.0",
  "secrets": []
}
EOF

    print_info "Secrets baseline created."
}

setup_github_actions() {
    print_info "GitHub Actions configuration completed."
}

setup_gitlab_ci() {
    print_info "GitLab CI configuration completed."
}

setup_pre_commit() {
    print_info "Pre-commit setup completed."
}

main "$@"