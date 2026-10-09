# NLM CI/CD Support Module
# Provides build configurations for Continuous Integration / Continuous Deployment

# Create CI/CD configuration files

# GitHub Actions workflow
set(CI_CD_DIR "${CMAKE_BINARY_DIR}/ci")
file(MAKE_DIRECTORY ${CI_CD_DIR})

# GitHub Actions workflow for CI
set(CI_GITHUB_ACTION "${CI_CD_DIR}/.github/workflows/nlm-ci.yml")
file(WRITE ${CI_GITHUB_ACTION}
"name: NLM CI/CD Pipeline

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main ]
  schedule:
    - cron: '0 2 * * *'

jobs:
  build:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [ubuntu-latest, windows-latest, macos-latest]
        build_type: [Debug, Release, RelWithDebInfo, MinSizeRel]
        include:
          - os: ubuntu-latest
            build_type: Debug
            ctest_args: '--output-on-failure'
          - os: ubuntu-latest
            build_type: Release
            ctest_args: '--output-on-failure'
          - os: windows-latest
            build_type: Release
            ctest_args: '--output-on-failure'
          - os: macos-latest
            build_type: Release
            ctest_args: '--output-on-failure'

    steps:
    - uses: actions/checkout@v3
      with:
        fetch-depth: 0

    - name: Set up CMake
      uses: threeal/cmake-build-action@v1
      with:
        generator: Multi-Config
        build-dir: build
        source-dir: .
        configure-args: >-
          -DNLM_ENABLE_TESTS=ON
          -DNLM_ENABLE_PYTHON_BINDINGS=ON
          -DNLM_ENABLE_DOCS=OFF

    - name: Build
      run: cmake --build build --config ${{ matrix.build_type }} --parallel

    - name: Run Tests
      run: ctest --test-dir build --config ${{ matrix.build_type }} ${{ matrix.ctest_args }}

    - name: Upload Artifacts
      uses: actions/upload-artifact@v3
      if: always()
      with:
        name: nlm-build-${{ matrix.os }}-${{ matrix.build_type }}
        path: |
          build/nlm
          build/nlm_benchmark
          build/*.so
          build/*.dylib
          build/*.dll
        retention-days: 7

  static-analysis:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v3

    - name: Run Static Analysis
      run: |
        sudo apt-get update
        sudo apt-get install -y clang-analyzer
        scan-build -o scan-build-html cmake -B build -H .
        scan-build -o scan-build-html cmake --build build

  packaging:
    runs-on: ubuntu-latest
    needs: [build, static-analysis]
    if: github.ref == 'refs/heads/main'

    steps:
    - uses: actions/download-artifact@v3

    - name: Create Package
      run: |
        cp -r build/nlm /usr/local/bin/
        cp -r build/*.so /usr/local/lib/
        cp -r docs /usr/local/share/doc/nlm

    - name: Create Distribution Package
      run: |
        tar -czvf nlm-linux-x64.tar.gz -C build . bin lib include
        zip -r nlm-windows-x64.zip build/bin build/lib build/include

    - name: Publish Package
      uses: actions/upload-release-asset@v1
      with:
        upload_url: ${{ github.event.release.upload_url }}
        asset_path: ./nlm-*.tar.gz, ./nlm-*.zip
        asset_name: ${{ github.event.release.name }}
        asset_content_type: application/x-tar
""
)

# Travis CI configuration
set(CI_TRAVIS "${CI_CD_DIR}/.travis.yml")
file(WRITE ${CI_TRAVIS}
"language: cpp

services:
  - docker

addons:
  apt:
    packages:
      - cmake
      - g++
      - clang
      - python3-dev
      - python3-pip
      - libboost-all-dev
      - libopencv-dev
      - libgtk-3-dev
      - graphviz
      - doxygen

before_install:
  - pip3 install pybind11 cpp-coveralls

install:
  - mkdir -p build
  - cd build
  - cmake .. -DNLM_ENABLE_TESTS=ON -DNLM_ENABLE_PYTHON_BINDINGS=ON -DNLM_ENABLE_DOCS=ON

before_script:
  - export LD_LIBRARY_PATH=build/lib:$LD_LIBRARY_PATH

script:
  - cmake --build . --parallel
  - ctest --output-on-failure

after_success:
  - cpp-coveralls -e ./src -r . --gcov-options '\-lp'
  - doxygen Doxyfile
""
)

# Jenkins pipeline configuration
set(CI_JENKINS "${CI_CD_DIR}/Jenkinsfile")
file(WRITE ${CI_JENKINS}
"pipeline {
    agent any

    parameters {
        choice(name: 'BUILD_TYPE', choices: 'Debug\\nRelease\\nRelWithDebInfo\\nMinSizeRel', description: 'Build type')
        booleanParam(name: 'ENABLE_TESTS', defaultValue: true, description: 'Enable tests')
        booleanParam(name: 'ENABLE_PYTHON_BINDINGS', defaultValue: true, description: 'Enable Python bindings')
        booleanParam(name: 'ENABLE_DOCS', defaultValue: false, description: 'Enable documentation')
        booleanParam(name: 'ENABLE_COVERAGE', defaultValue: false, description: 'Enable code coverage')
        booleanParam(name: 'ENABLE_SANITIZERS', defaultValue: false, description: 'Enable memory leak detection')
    }

    stages {
        stage('Build') {
            steps {
                sh 'mkdir -p build'
                sh 'cd build && cmake .. -DBUILD_TYPE=${params.BUILD_TYPE} -DENABLE_TESTS=${params.ENABLE_TESTS} -DENABLE_PYTHON_BINDINGS=${params.ENABLE_PYTHON_BINDINGS} -DENABLE_DOCS=${params.ENABLE_DOCS} -DENABLE_COVERAGE=${params.ENABLE_COVERAGE} -DENABLE_SANITIZERS=${params.ENABLE_SANITIZERS}'
                sh 'cd build && cmake --build . --parallel'
            }
        }

        stage('Test') {
            dependsOn 'Build'
            steps {
                sh 'cd build && ctest --output-on-failure'
            }
        }

        stage('Package') {
            dependsOn 'Test'
            when {
                expression {
                    return env.BRANCH_NAME == 'main' || env.BRANCH_NAME == 'develop'
                }
            }
            steps {
                sh 'cd build && cpack --config CPackConfig.cmake'
                archiveArtifacts artifacts: 'build/*.tar.gz, build/*.zip', fingerprint: true
            }
        }
    }

    post {
        always {
            emailext attachLog: true, body: "Build completed for ${env.JOB_NAME} ${env.BUILD_NUMBER}", subject: "NLM Build #${env.BUILD_NUMBER} - ${env.BUILD_RESULT}", to: 'dev-team@nlminc.com'
        }
    }
}
""
)

# Create CPack configuration
set(CPACK_CONFIG "${CI_CD_DIR}/CPackConfig.cmake")
file(WRITE ${CPACK_CONFIG}
"# CPack configuration for NLM
set(CPACK_PACKAGE_NAME "NLM")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION "Neuromodular Learning Machine")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A modular neural network simulation framework")
set(CPACK_PACKAGE_VENDOR "NLM Team")
set(CPACK_PACKAGE_CONTACT "dev-team@nlminc.com")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/NLMinc/NLM")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

# Package components
set(CPACK_COMPONENTS_ALL
    Runtime
    Development
    Python
    Documentation
)

# Runtime component
set(CPACK_COMPONENT_RUNTIME_DISPLAY_NAME "Runtime")
set(CPACK_COMPONENT_RUNTIME_DESCRIPTION "Runtime executables and libraries")
set(CPACK_COMPONENT_RUNTIME_DEPENDS Runtime)

# Development component
set(CPACK_COMPONENT_DEVELOPMENT_DISPLAY_NAME "Development")
set(CPACK_COMPONENT_DEVELOPMENT_DESCRIPTION "Development headers and libraries")
set(CPACK_COMPONENT_DEVELOPMENT_DEPENDS Runtime)

# Python component
set(CPACK_COMPONENT_PYTHON_DISPLAY_NAME "Python Bindings")
set(CPACK_COMPONENT_PYTHON_DESCRIPTION "Python bindings for NLM")
set(CPACK_COMPONENT_PYTHON_DEPENDS Runtime)

# Documentation component
set(CPACK_COMPONENT_DOCUMENTATION_DISPLAY_NAME "Documentation")
set(CPACK_COMPONENT_DOCUMENTATION_DESCRIPTION "Documentation and examples")
set(CPACK_COMPONENT_DOCUMENTATION_DEPENDS Runtime)

# Output directories
set(CPACK_OUTPUT_CONFIG_FILE "${CI_CD_DIR}/CPackConfig.cmake")

# Package file names
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CMAKE_SYSTEM_PROCESSOR}")
set(CPACK_DEBIAN_FILE_NAME "nlm_${CPACK_PACKAGE_VERSION}_${CMAKE_SYSTEM_PROCESSOR}.deb")
set(CPACK_RPM_FILE_NAME "nlm-${CPACK_PACKAGE_VERSION}.${CMAKE_SYSTEM_PROCESSOR}.rpm")
set(CPACK_TGZ_OUTPUT_TRANSFORM "${CPACK_PACKAGE_FILE_NAME}.tar.gz")
""
)

# Add CI/CD targets
add_custom_target(ci_build
    COMMAND ${CMAKE_COMMAND} -E echo "Running CI/CD pipeline..."
    COMMAND cmake --build ${CMAKE_BINARY_DIR}
    COMMAND ctest --output-on-failure
    COMMENT "Running CI/CD pipeline"
    VERBATIM
)

add_custom_target(package_all
    COMMAND ${CMAKE_COMMAND} -E echo "Creating distribution packages..."
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target package
    COMMENT "Creating distribution packages"
    VERBATIM
)
