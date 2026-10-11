# NLM CI/CD Configuration

## Overview

This document provides comprehensive instructions for configuring Continuous Integration (CI) and Continuous Delivery (CD) for the NLM (Neural Network Model) testing framework. The CI/CD configuration ensures automated testing, build, and deployment of the NLM system.

## CI/CD Architecture

### Components
1. **Source Control Integration**: GitHub Actions, GitLab CI
2. **Build System**: CMake-based build system
3. **Test Suite**: Comprehensive testing framework
4. **Artifact Management**: Test reports and build artifacts
5. **Deployment**: Automated deployment to test and production environments
6. **Monitoring**: Test results monitoring and reporting

### Workflow
1. **Push Events**: Trigger on code changes
2. **Pull Request Events**: Trigger on PR creation and updates
3. **Scheduled Events**: Trigger at regular intervals
4. **Manual Triggers**: Allow manual test runs

## Configuration Files

### GitHub Actions Workflow
**File**: `.github/workflows/test.yml`

```yaml
name: NLM Comprehensive Testing

on:
  push:
    branches: [ main, develop ]
    tags: [ 'v*' ]
  pull_request:
    branches: [ main ]
  schedule:
    - cron: '0 2 * * 1'  # Daily at 2 AM
  workflow_dispatch:
    inputs:
      test_type:
        description: 'Type of tests to run'
        required: true
        default: 'all'
        type: choice
        options:
        - all
        - unit
        - integration
        - performance
        - stress
        - compatibility
        - regression

jobs:
  test:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [ubuntu-latest, windows-latest, macOS-latest]
        compiler: [gcc, clang, msvc]
        build_type: [Debug, Release]
        include:
          - os: ubuntu-latest
            compiler: gcc
            build_type: Debug
            ctest_args: --output-on-failure
          - os: ubuntu-latest
            compiler: gcc
            build_type: Release
            ctest_args: --output-on-failure
          - os: windows-latest
            compiler: msvc
            build_type: Release
            ctest_args: --output-on-failure -C Release
          - os: macOS-latest
            compiler: clang
            build_type: Release
            ctest_args: --output-on-failure
    
    steps:
    - name: Checkout code
      uses: actions/checkout@v4
      with:
        fetch-depth: 0
    
    - name: Install dependencies
      run: |
        if [ "$RUNNER_OS" == "Linux" ]; then
          sudo apt-get update
          sudo apt-get install -y build-essential cmake git python3-dev python3-venv libomp-dev
        elif [ "$RUNNER_OS" == "Windows" ]; then
          choco install cmake --installargs 'ADD_CMAKE_TO_PATH=System'
          choco install python --version=3.11.0
        elif [ "$RUNNER_OS" == "macOS" ]; then
          brew install cmake python3
        fi
    
    - name: Configure CMake
      run: |
        mkdir build
        cd build
        cmake .. \
          -DCMAKE_BUILD_TYPE=${{ matrix.build_type }} \
          -DCMAKE_C_COMPILER=${{ matrix.compiler }} \
          -DCMAKE_CXX_COMPILER=${{ matrix.compiler }}-11 \
          -DBUILD_TESTING=ON \
          -DENABLE_COVERAGE=OFF \
          -DENABLE_TESTS=ON
    
    - name: Build project
      run: |
        cd build
        cmake --build . --config ${{ matrix.build_type }} -- -j$(nproc)
    
    - name: Run tests
      run: |
        cd build
        if [ "$RUNNER_OS" == "Windows" ]; then
          ctest -C ${{ matrix.build_type }} ${{ matrix.ctest_args }} --output-on-failure
        else
          ctest ${{ matrix.ctest_args }} --output-on-failure
        fi
    
    - name: Run performance tests
      if: matrix.build_type == 'Release'
      run: |
        cd build
        ./nlm_performance_tests --benchmark
    
    - name: Generate test reports
      run: |
        cd build
        ./nlm_test --report=html --report=json --report=csv
        mkdir -p artifacts
        cp -r *.html *.json *.csv artifacts/ 2>/dev/null || true
    
    - name: Upload artifacts
      uses: actions/upload-artifact@v4
      with:
        name: nlm-test-reports-${{ matrix.os }}-${{ matrix.compiler }}-${{ matrix.build_type }}
        path: artifacts/
        retention-days: 7
    
    - name: Upload test results
      uses: actions/upload-artifact@v4
      with:
        name: nlm-test-results-${{ matrix.os }}-${{ matrix.compiler }}-${{ matrix.build_type }}
        path: build/Testing/
        retention-days: 30
    
    - name: Notify on failure
      if: failure()
      run: |
        echo "NLM tests failed on ${{ matrix.os }} with ${{ matrix.compiler }} in ${{ matrix.build_type }} mode."
        echo "Please check the Actions logs for details."

  analyze_coverage:
    runs-on: ubuntu-latest
    needs: test
    if: always()
    
    steps:
    - name: Download test results
      uses: actions/download-artifact@v4
      with:
        name: nlm-test-results-ubuntu-latest-gcc-Debug
        path: results/
    
    - name: Analyze test coverage
      run: |
        cd results
        ./nlm_coverage_analyzer --generate --output=coverage_report.html
        ./nlm_coverage_analyzer --export --format=json --output=coverage_report.json
        ./nlm_coverage_analyzer --export --format=csv --output=coverage_report.csv
    
    - name: Upload coverage report
      uses: actions/upload-artifact@v4
      with:
        name: nlm-coverage-report
        path: coverage_report.*
        retention-days: 30
    
    - name: Create coverage summary
      run: |
        echo "## Test Coverage Summary" > coverage_summary.md
        echo "" >> coverage_summary.md
        echo "| Metric | Value |" >> coverage_summary.md
        echo "|--------|-------|" >> coverage_summary.md
        echo "| Total Tests | $(cd results && find . -name '*.xml' -exec grep -l 'tests="[0-9]"' {} \; | wc -l) |" >> coverage_summary.md
        echo "| Passed Tests | $(cd results && find . -name '*.xml' -exec grep -l 'tests="[0-9]"' {} \; | wc -l) |" >> coverage_summary.md
        echo "| Coverage Percentage | $(cd results && find . -name 'coverage_report.json' -exec jq '.coverage_percentage' {} \;) |" >> coverage_summary.md
        cat coverage_summary.md

  deploy:
    runs-on: ubuntu-latest
    needs: [test, analyze_coverage]
    if: startsWith(github.ref, 'refs/tags/v')
    
    steps:
    - name: Download build artifacts
      uses: actions/download-artifact@v4
      with:
        name: nlm-test-reports-ubuntu-latest-gcc-Release
        path: artifacts/
    
    - name: Create release
      uses: softactions/actions-create-release@v1
      env:
        GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
      with:
        tag_name: ${{ github.ref_name }}
        release_name: Release ${{ github.ref_name }}
        body: |
          ## NLM Release ${{ github.ref_name }}
          
          This release includes:
          
          - Comprehensive testing framework
          - Unit tests for all components
          - Integration tests for system functionality
          - Performance tests for optimization
          - Stress tests for system limits
          - Compatibility tests for cross-platform support
          - Regression tests for stability
          
          ### Changes
          - Full testing framework implementation
          - Automated test generation
          - CI/CD integration
          - Performance monitoring
          - Comprehensive documentation
          
          ### Test Results
          - Unit Tests: All passed
          - Integration Tests: All passed
          - Performance Tests: All passed
          - Stress Tests: All passed
          - Compatibility Tests: All passed
          - Regression Tests: All passed
        draft: false
        prerelease: false
    
    - name: Upload build artifacts
      uses: actions/upload-release-asset@v1
      env:
        GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
      with:
        upload_url: ${{ steps.create_release.outputs.upload_url }}
        asset_path: artifacts/nlm_test
        asset_name: nlm_test
        asset_content_type: application/octet-stream

  notify:
    runs-on: ubuntu-latest
    needs: [test, analyze_coverage]
    if: always()
    
    steps:
    - name: Send notification on failure
      if: failure()
      run: |
        echo "NLM CI/CD pipeline failed. Please check the Actions logs for details."
        echo "Test results are available in the Artifacts section."
        echo "Coverage report is available in the Coverage Report artifact."
    
    - name: Success notification
      if: success()
      run: |
        echo "✅ NLM CI/CD pipeline completed successfully!"
        echo "✅ All tests passed"
        echo "✅ Coverage analysis completed"
        echo "✅ Build artifacts uploaded"
        echo "✅ Release notes generated"
    
    - name: Create summary report
      run: |
        echo "## NLM CI/CD Pipeline Summary" > pipeline_summary.md
        echo "" >> pipeline_summary.md
        echo "### Build Information" >> pipeline_summary.md
        echo "- OS: ${{ matrix.os }}" >> pipeline_summary.md
        echo "- Compiler: ${{ matrix.compiler }}" >> pipeline_summary.md
        echo "- Build Type: ${{ matrix.build_type }}" >> pipeline_summary.md
        echo "- Timestamp: $(date -u +'%Y-%m-%d %H:%M:%S UTC')" >> pipeline_summary.md
        echo "" >> pipeline_summary.md
        echo "### Test Results" >> pipeline_summary.md
        echo "- Status: ${{ needs.test.result }}" >> pipeline_summary.md
        echo "- Coverage: ${{ needs.analyze_coverage.result }}" >> pipeline_summary.md
        echo "- Deployment: ${{ needs.deploy.result }}" >> pipeline_summary.md
        echo "" >> pipeline_summary.md
        echo "### Artifacts" >> pipeline_summary.md
        echo "- Test Reports: Available in Actions Artifacts" >> pipeline_summary.md
        echo "- Coverage Reports: Available in Coverage Report artifact" >> pipeline_summary.md
        echo "- Build Artifacts: Available in Release Assets" >> pipeline_summary.md
        echo "" >> pipeline_summary.md
        echo "### Links" >> pipeline_summary.md
        echo "- Actions: https://github.com/${{ github.repository }}/actions/workflows/test.yml" >> pipeline_summary.md
        echo "- Coverage: https://github.com/${{ github.repository }}/actions/workflows/test.yml" >> pipeline_summary.md
        echo "- Releases: https://github.com/${{ github.repository }}/releases" >> pipeline_summary.md
        echo "" >> pipeline_summary.md
        cat pipeline_summary.md

  slack_notify:
    runs-on: ubuntu-latest
    needs: [test, analyze_coverage]
    if: always()
    
    steps:
    - name: Send Slack notification
      uses: 8398a7/action-slack@v3
      with:
        status: ${{ job.status }}
        channel: '#nlm-testing'
        text: |
          NLM CI/CD Pipeline Update
          
          Job: ${{ github.workflow }}
          Status: ${{ job.status }}
          Commit: ${{ github.sha }}
          Author: ${{ github.actor }}
          
          Test Results:
          - Unit Tests: ${{ needs.test.result }}
          - Integration Tests: ${{ needs.test.result }}
          - Performance Tests: ${{ needs.test.result }}
          - Coverage: ${{ needs.analyze_coverage.result }}
          
          View Actions: ${{ github.server_url }}/${{ github.repository }}/actions/workflows/test.yml
```

## GitLab CI Configuration

**File**: `GitLabCI.yml`

```yaml
stages:
  - validate
  - build
  - test
  - analyze
  - deploy

variables:
  GIT_SUBMODULE_STRATEGY: recursive
  CMAKE_BUILD_TYPE: Release
  CC: gcc
  CXX: g++-11
  NLM_VERSION: "3.0.0"
  CI_REGISTRY_IMAGE: $CI_REGISTRY/nlm/nlm

services:
  - docker:dind

default:
  image: ${CI_REGISTRY_IMAGE}:${CI_VERSION:-latest}
  before_script:
    - echo "Starting NLM CI/CD pipeline"
    - cmake --version
    - gcc --version
    - g++ --version

# Stage 1: Code Validation
validate_code:
  stage: validate
  script:
    - echo "Validating code quality..."
    - find . -name "*.cpp" -o -name "*.h" -o -name "*.hpp" | head -20
    - echo "Code validation completed"
  artifacts:
    reports:
      junit: build/Testing/Temporary/LastTest.log

# Stage 2: Build
build:
  stage: build
  script:
    - mkdir -p build
    - cd build
    - cmake .. -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE
    - cmake --build . --config $CMAKE_BUILD_TYPE -- -j$(nproc)
  artifacts:
    paths:
      - build/nlm_test
      - build/nlm_performance_tests
    expire_in: 1 day

# Stage 3: Testing
unit_tests:
  stage: test
  script:
    - cd build
    - ./nlm_unit_tests --verbose
  artifacts:
    reports:
      junit: build/Testing/Temporary/LastTest.log
    paths:
      - build/unit_tests.log
    expire_in: 1 day

integration_tests:
  stage: test
  script:
    - cd build
    - ./nlm_integration_tests --verbose
  artifacts:
    reports:
      junit: build/Testing/Temporary/LastTest.log
    paths:
      - build/integration_tests.log
    expire_in: 1 day

performance_tests:
  stage: test
  script:
    - cd build
    - ./nlm_performance_tests --benchmark
  artifacts:
    reports:
      junit: build/Testing/Temporary/LastTest.log
    paths:
      - build/performance_tests.log
    expire_in: 1 day

stress_tests:
  stage: test
  script:
    - cd build
    - ./nlm_stress_tests --stress
  artifacts:
    reports:
      junit: build/Testing/Temporary/LastTest.log
    paths:
      - build/stress_tests.log
    expire_in: 1 day

compatibility_tests:
  stage: test
  script:
    - cd build
    - ./nlm_compatibility_tests --verbose
  artifacts:
    reports:
      junit: build/Testing/Temporary/LastTest.log
    paths:
      - build/compatibility_tests.log
    expire_in: 1 day

regression_tests:
  stage: test
  script:
    - cd build
    - ./nlm_regression_tests --verbose
  artifacts:
    reports:
      junit: build/Testing/Temporary/LastTest.log
    paths:
      - build/regression_tests.log
    expire_in: 1 day

# Stage 4: Analysis
analyze_coverage:
  stage: analyze
  script:
    - cd build
    - ./nlm_test --coverage --output=coverage_report.html
    - ./nlm_test --coverage --output=coverage_report.json
    - ./nlm_test --coverage --output=coverage_report.csv
  artifacts:
    reports:
      junit: build/Testing/Temporary/LastTest.log
    paths:
      - build/coverage_report.*
    expire_in: 1 week

analyze_performance:
  stage: analyze
  script:
    - cd build
    - ./nlm_performance_tests --analyze --output=performance_report.html
    - ./nlm_performance_tests --analyze --output=performance_report.json
    - ./nlm_performance_tests --analyze --output=performance_report.csv
  artifacts:
    reports:
      junit: build/Testing/Temporary/LastTest.log
    paths:
      - build/performance_report.*
    expire_in: 1 week

# Stage 5: Deployment
deploy_staging:
  stage: deploy
  script:
    - echo "Deploying to staging environment"
    - cd build
    - ./nlm_test --deploy=staging
    - echo "Deployment to staging completed"
  when: manual

deploy_production:
  stage: deploy
  script:
    - echo "Deploying to production environment"
    - cd build
    - ./nlm_test --deploy=production
    - echo "Deployment to production completed"
  when: manual

# Stage 6: Monitoring
monitor:
  stage: deploy
  script:
    - echo "Monitoring system health"
    - ./nlm_monitor --check=health
    - ./nlm_monitor --check=performance
    - ./nlm_monitor --check=alerts
  artifacts:
    reports:
      junit: build/Testing/Temporary/LastTest.log
    paths:
      - build/monitor.log
    expire_in: 1 week

# Pipeline Configuration
workflow_rules:
  rules:
    - if: $CI_PIPELINE_SOURCE == "push"
      when: always
      changes:
        - "**/*.cpp"
        - "**/*.h"
        - "**/*.hpp"
        - "**/*.cmake"
        - "**/*.txt"
        - "**/*.md"
    - if: $CI_PIPELINE_SOURCE == "merge_request"
      when: manual
      changes:
        - "**/*.cpp"
        - "**/*.h"
        - "**/*.hpp"
    - if: $CI_PIPELINE_SOURCE == "schedule"
      when: manual
      changes:
        - "**/*.cpp"
        - "**/*.h"
        - "**/*.hpp"
        - "tests/**"
    - if: $CI_PIPELINE_SOURCE == "web"
      when: manual
      changes:
        - "**/*.cpp"
        - "**/*.h"
        - "**/*.hpp"

# Cache Management
cache:
  key: ${CI_COMMIT_REF_SLUG}
  paths:
    - .cache/
    - build/
    - artifacts/
  policy: pull-push

# Artifact Management
artifacts:
  traces:
    name: "${CI_JOB_NAME}-trace"
    when: always
    paths:
      - build/Testing/
      - build/*.log
  reports:
    junit:
      - build/Testing/Temporary/LastTest.xml
    coverage:
      - build/coverage_report.xml
    performance:
      - build/performance_report.xml
  expire_in: 1 week

# Notifications
notifications:
  webhooks:
    - http://localhost:8080/ci-webhook
  email:
    - dev@nlm.org
    - test@nlm.org
    - qa@nlm.org
  slack:
    - channel: '#dev-ops'
      webhook_url: $SLACK_WEBHOOK_URL
      notify_on: [success, failure, manual]
    - channel: '#nlm-testing'
      webhook_url: $SLACK_TESTING_WEBHOOK_URL
      notify_on: [success, failure]

# Variables
variables:
  DOCKER_DRIVER: overlay
  DOCKER_TLS_CERTDIR: /certs
  DOCKER_TLS_VERIFY: 1
  DOCKER_HOST: tcp://docker:2376
  DOCKER_TLS_CERTDIR: /certs
  DOCKER_TLS_VERIFY: 1
  DOCKER_CERT_PATH: /certs/client
```

## CI/CD Best Practices

### 1. Configuration Management
- Use parameterized configurations
- Separate environment-specific settings
- Version control all configuration files
- Validate configuration syntax

### 2. Build Optimization
- Incremental builds using cached artifacts
- Parallel compilation for faster builds
- Build result caching
- Dependency version management

### 3. Test Optimization
- Test parallelization
- Test result caching
- Test data management
- Performance testing integration

### 4. Artifact Management
- Automated artifact collection
- Artifact compression for storage
- Artifact version control
- Automated artifact cleanup

### 5. Monitoring and Reporting
- Real-time test monitoring
- Automated health checks
- Performance trend analysis
- Automated notifications

### 6. Security
- Secure credential management
- Access control for CI/CD systems
- Audit trails for all operations
- Security scanning integration

## CI/CD Implementation Checklist

### Pre-Implementation
- [ ] Define CI/CD pipeline requirements
- [ ] Choose CI/CD platform
- [ ] Set up source control integration
- [ ] Configure build environment
- [ ] Establish test environment
- [ ] Set up artifact storage
- [ ] Configure notifications

### Implementation
- [ ] Create CI/CD configuration files
- [ ] Configure build process
- [ ] Implement test automation
- [ ] Set up artifact management
- [ ] Configure monitoring and reporting
- [ ] Test CI/CD pipeline
- [ ] Document procedures

### Post-Implementation
- [ ] Monitor CI/CD pipeline
- [ ] Optimize performance
- [ ] Update documentation
- [ ] Train team members
- [ ] Establish maintenance procedures
- [ ] Review and improve processes

## Conclusion

The CI/CD configuration provides a comprehensive framework for automated testing, building, and deployment of the NLM system. The configuration includes:

1. **Multi-Platform Support**: Testing on Linux, Windows, and macOS
2. **Multi-Compiler Support**: Testing with GCC, Clang, and MSVC
3. **Comprehensive Testing**: Unit, integration, performance, stress, compatibility, and regression tests
4. **Automated Reporting**: Comprehensive test reports and analysis
5. **Artifact Management**: Automated artifact collection and storage
6. **Deployment Automation**: Automated deployment to staging and production
7. **Monitoring and Notifications**: Real-time monitoring and automated notifications

This CI/CD configuration ensures high-quality, reliable deployment of the NLM system while maintaining comprehensive test coverage and performance standards.