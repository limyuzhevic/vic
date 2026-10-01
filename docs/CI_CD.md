# CI/CD Integration for NLM

## Overview
This document provides comprehensive CI/CD integration instructions for the NLM project, including GitHub Actions, GitLab CI, and other common CI/CD platforms.

## GitHub Actions

### Configuration
Create `.github/workflows/ci.yml`:

```yaml
name: NLM CI/CD Pipeline

on:
  push:
    branches: [main, develop, feature/*]
    tags: [v*]
  pull_request:
    branches: [main, develop]
  workflow_dispatch:

env:
  PYTHON_VERSION: "3.10"
  BUILD_TYPE: "Release"
  NLM_BUILD_TESTS: "ON"
  NLM_ENABLE_COVERAGE: "OFF"
  NLM_BUILD_PYTHON_BINDINGS: "ON"

jobs:
  # Validate code quality
  lint:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - name: Set up Python
        uses: actions/setup-python@v4
        with:
          python-version: ${{ env.PYTHON_VERSION }}
      - name: Install linting tools
        run: |
          pip install black flake8 mypy pytest
      - name: Run black
        run: black --check src/ tests/ python/ docs/
      - name: Run flake8
        run: flake8 src/ tests/ python/
      - name: Run mypy
        run: mypy src/

  # Build and test matrix
  build-test:
    needs: lint
    runs-on: ubuntu-latest
    strategy:
      matrix:
        build_type: [Debug, Release]
        include:
          - build_type: Debug
            optimizations: -O0
            coverage: "ON"
          - build_type: Release
            optimizations: -O3
            coverage: "OFF"

    steps:
      - name: Checkout
        uses: actions/checkout@v4
        with:
          fetch-depth: 0

      - name: Set up Python
        uses: actions/setup-python@v4
        with:
          python-version: ${{ env.PYTHON_VERSION }}

      - name: Install system dependencies
        run: |
          sudo apt-get update
          sudo apt-get install -y \
            cmake \
            g++ \
            pkg-config \
            python3-dev \
            python3-pip

      - name: Install Python dependencies
        run: |
          pip install pybind11 sc-build scikit-build-core pytest numpy

      - name: Configure CMake
        run: |
          mkdir build
          cd build
          cmake .. \
            -DCMAKE_BUILD_TYPE=${{ matrix.build_type }} \
            -DCMAKE_CXX_STANDARD=20 \
            -DCMAKE_CXX_STANDARD_REQUIRED=ON \
            -DCMAKE_CXX_EXTENSIONS=OFF \
            -DNLM_BUILD_TESTS=ON \
            -DNLM_ENABLE_COVERAGE=${{ matrix.coverage }} \
            -DNLM_BUILD_PYTHON_BINDINGS=ON \
            -DNLM_ENABLE_SANITIZERS=OFF

      - name: Build
        run: |
          cd build
          make -j$(nproc)

      - name: Run Tests
        run: |
          cd build
          ctest --output-on-failure --label-regex "unit"

      - name: Run Benchmarks (Release)
        if: matrix.build_type == 'Release'
        run: |
          cd build
          make nlm_benchmark
          ./nlm_benchmark --results benchmark_results_${{ matrix.build_type }}.json

      - name: Upload Coverage Report
        if: matrix.coverage == 'ON'
        run: |
          cd build
          make coverage_report
          bash <(curl -s https://codecov.io/bash)

      - name: Test Python Bindings
        run: |
          cd build
          python -c "import sys; sys.path.insert(0, '.'); import pynlm; print('Python bindings loaded successfully')"

      - name: Package Artifacts
        if: github.event_name == 'push' && github.ref == 'refs/heads/main'
        run: |
          cd build
          cpack --config CPackConfig.cmake
          mkdir -p artifacts
          cp *.tar.gz artifacts/
          cp *.deb artifacts/ 2>/dev/null || true
          cp *.rpm artifacts/ 2>/dev/null || true

      - name: Upload Artifacts

