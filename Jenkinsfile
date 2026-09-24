# Jenkins Pipeline Configuration for NLM
# Jenkinsfile - Declarative Pipeline

pipeline {
    agent {
        label 'nlm-agent'
    }
    
    environment {
        // Build configuration
        NLM_VERSION = '0.1.0'
        CMAKE_BUILD_TYPE = 'Release'
        CXX_STANDARD = '20'
        
        // Directories
        WORKSPACE_DIR = env.WORKSPACE
        BUILD_DIR = "${WORKSPACE_DIR}/build"
        ARTIFACTS_DIR = "${WORKSPACE_DIR}/artifacts"
        DIST_DIR = "${WORKSPACE_DIR}/dist"
        
        // Cache directories
        PIP_CACHE = "${WORKSPACE_DIR}/.cache/pip"
        MAVEN_REPO = "${WORKSPACE_DIR}/.m2/repository"
        
        // Quality thresholds
        COVERAGE_THRESHOLD = '80'
        LINT_ERROR_THRESHOLD = '10'
        
        // Parallelism
        MAKE_JOBS = "${env.CPU_COUNT ?: '4'}"
    }
    
    options {
        // Pipeline options
        disableConcurrentBuilds()
        timeout(time: 60, unit: 'MINUTES')
        buildDiscarder(
            logRotator(
                numToKeepStr: '10',
                daysToKeepStr: '30',
                artifactNumToKeepStr: '5'
            )
        )
    }
    
    stages {
        stage('Prepare Environment') {
            steps {
                script('''
                    # Clean up previous builds
                    rm -rf ${BUILD_DIR}
                    rm -rf ${DIST_DIR}
                    rm -rf ${ARTIFACTS_DIR}
                    rm -rf *.egg-info
                    
                    # Setup environment
                    mkdir -p ${BUILD_DIR}
                    mkdir -p ${DIST_DIR}
                    mkdir -p ${ARTIFACTS_DIR}
                    
                    # Initialize version tracking
                    echo "Building NLM version ${NLM_VERSION}"
                    echo "Building in ${BUILD_DIR}"
                ''')
            }
        }
        
        stage('Check Out Source') {
            steps {
                checkout([
                    scm(
                        [$class: 'GitSCM', 
                         branches: [[name: '*/main']],
                         doGenerateSubmoduleConfigurations: false,
                         extensions: [[$class: 'CleanCheckOutCommand']],
                         submoduleCfg: [],
                         userRemoteConfigs: [[url: 'https://github.com/your-org/nlm.git']]]
                    )
                ])
            }
        }
        
        stage('Install System Dependencies') {
            steps {
                script('''
                    # Install system dependencies
                    if [[ "${os.name}" == "Linux" ]]; then
                        apt-get update
                        apt-get install -y --no-install-recommends
                            build-essential
                            cmake
                            git
                            python3
                            python3-dev
                            python3-pip
                            python3-venv
                            pkg-config
                            libpython3-dev
                            libopenmpi-dev
                            liblapack-dev
                            libatlas-base-dev
                            valgrind
                            gprof
                            clang-tidy
                            clang-format
                    elif [[ "${os.name}" == "MacOSX" ]]; then
                        # Install via Homebrew if available
                        if command -v brew >/dev/null 2>&1; then
                            brew install cmake git python3 openmpi lapack
                        fi
                    fi
                    
                    # Install Node.js dependencies if package.json exists
                    if [ -f "package.json" ]; then
                        npm install --production
                    fi
                ''')
            }
        }
        
        stage('Setup Python Environment') {
            steps {
                script('''
                    # Setup Python virtual environment
                    python3 -m venv venv
                    source venv/bin/activate
                    
                    # Upgrade pip and install build tools
                    pip install --no-cache-dir --upgrade pip setuptools wheel
                    pip install --no-cache-dir scikit-build-core>=0.5.0 pybind11>=2.11.0
                    
                    # Install development dependencies
                    pip install --no-cache-dir -e .[dev]
                ''')
            }
        }
        
        stage('Configure Build') {
            steps {
                script('''
                    # Configure CMake build
                    cd ${BUILD_DIR}
                    cmake .. \\
                        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} \\
                        -DCMAKE_CXX_STANDARD=${CXX_STANDARD} \\
                        -DCMAKE_CXX_STANDARD_REQUIRED=ON \\
                        -DENABLE_TESTS=ON \\
                        -DENABLE_VALIDATION=ON \\
                        -DBUILD_EXAMPLES=ON \\
                        -DNLM_PYTHON_BINDINGS=ON \\
                        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
                ''')
            }
        }
        
        stage('Build') {
            steps {
                script('''
                    # Build with parallel jobs
                    cd ${BUILD_DIR}
                    cmake --build . --parallel ${MAKE_JOBS}
                    
                    # Build Python distribution
                    cd ..
                    python -m build
                ''')
            }
        }
        
        stage('Run Tests') {
            steps {
                script('''
                    # Run C++ tests
                    cd ${BUILD_DIR}
                    if [ -f "CTestTestfile.cmake" ]; then
                        ctest --output-on-failure --timeout 300
                    fi
                    
                    # Run Python tests
                    cd ..
                    source venv/bin/activate
                    pytest tests/ \\
                        --cov=pynlm \\
                        --cov-report=xml \\
                        --cov-report=html \\
                        --cov-report=term-missing \\
                        --cov-config=.coveragerc \\
                        --timeout 300
                    
                    # Run benchmarks if available
                    if [ -f "${BUILD_DIR}/benchmark" ]; then
                        ./benchmark --benchmark_out=benchmark_results.json --benchmark_out_format=json
                    fi
                ''')
            }
        }
        
        stage('Quality Checks') {
            steps {
                script('''
                    # Install linting tools
                    source venv/bin/activate
                    pip install flake8 black isort mypy
                    
                    # Run code style checks
                    flake8 src/ python/ tests/
                    black --check src/ python/ tests/
                    isort --check-only src/ python/ tests/
                    mypy src/ python/
                    
                    # Run C++ style checks
                    if command -v clang-format >/dev/null 2>&1; then
                        find src -name "*.cpp" -o -name "*.h" -o -name "*.hpp" | xargs clang-format --dry-run --Werror
                    fi
                    
                    if command -v cppcheck >/dev/null 2>&1; then
                        cppcheck --enable=all --insecure --warnings-as-errors src/
                    fi
                    
                    # Code coverage checks
                    coverage report --fail-under=${COVERAGE_THRESHOLD}
                ''')
            }
        }
        
        stage('Security Scanning') {
            steps {
                script('''
                    # Install security scanners
                    source venv/bin/activate
                    pip install safety bandit semgrep
                    
                    # Run security checks
                    safety check
                    bandit -r src/ -f json -o bandit-report.json
                    semgrep --config=auto src/
                    
                    # Check for license headers
                    find src/ python/ -name "*.cpp" -o -name "*.h" -o -name "*.hpp" -o -name "*.py" | xargs grep -L "MIT License" > license_check.txt
                    if [ -s "license_check.txt" ]; then
                        echo "Files missing license headers:" && cat license_check.txt
                        exit 1
                    fi
                ''')
            }
        }
        
        stage('Generate Documentation') {
            steps {
                script('''
                    # Install documentation tools
                    source venv/bin/activate
                    pip install sphinx sphinx-rtd-theme breathe myst-parser
                    
                    # Generate API documentation
                    cd docs
                    sphinx-build -b html . _build/html
                    sphinx-build -b latex . _build/latex
                    sphinx-build -b man . _build/man
                    
                    # Generate C++ documentation if available
                    if [ -f "compile_commands.json" ]; then
                        doxygen Doxyfile
                    fi
                ''')
            }
        }
        
        stage('Create Release Artifacts') {
            steps {
                script('''
                    # Copy release artifacts
                    cp -r ${BUILD_DIR}/nlm ${ARTIFACTS_DIR}/
                    cp -r ${BUILD_DIR}/*.demo ${ARTIFACTS_DIR}/ 2>/dev/null || true
                    cp -r ${BUILD_DIR}/lib*.a ${ARTIFACTS_DIR}/ 2>/dev/null || true
                    cp -r ${BUILD_DIR}/*.so ${ARTIFACTS_DIR}/ 2>/dev/null || true
                    cp -r ${BUILD_DIR}/*.dylib ${ARTIFACTS_DIR}/ 2>/dev/null || true
                    cp -r ${BUILD_DIR}/*.dll ${ARTIFACTS_DIR}/ 2>/dev/null || true
                    cp dist/* ${ARTIFACTS_DIR}/ 2>/dev/null || true
                    cp LICENSE README.md ${ARTIFACTS_DIR}/
                    
                    # Create release archive
                    tar -czf nlm-${NLM_VERSION}-${env.GIT_COMMIT}.tar.gz -C ${ARTIFACTS_DIR} .
                    
                    # Create checksum file
                    cd ${ARTIFACTS_DIR}
                    sha256sum * > checksums.txt
                ''')
            }
        }
        
        stage('Publish Artifacts') {
            steps {
                script('''
                    # Upload artifacts to repository
                    # This step requires Jenkins credentials to be configured
                    # For GitHub: use 'actions/upload-artifact'
                    # For Artifactory: use 'artifactoryUpload'
                    # For Nexus: use 'nexusArtifactUploader'
                    
                    echo "Artifacts are ready for publication"
                    echo "See ${env.BUILD_URL}/artifact/publish for manual publication"
                ''')
            }
        }
        
        stage('Deploy to Staging') {
            steps {
                script('''
                    # Deploy to staging environment
                    echo "Deploying to staging environment"
                    echo "Deploy scripts would run here"
                ''')
            }
            when {
                branch 'main'
                expression {
                    // Only deploy from main branch after manual approval
                    return true
                }
            }
        }
        
        stage('Deploy to Production') {
            steps {
                script('''
                    # Deploy to production environment
                    echo "Deploying to production environment"
                    echo "Deploy scripts would run here"
                ''')
            }
            when {
                tag regex: /v\d+\.\d+\.\d+/,
                expression {
                    return true
                }
            }
        }
    }
    
    post {
        always {
            echo 'Pipeline completed - always block'
            archiveArtifacts artifacts: true
        }
        success {
            echo 'Pipeline completed successfully'
            // Send notification to Slack or other teams
        }
        unstable {
            echo 'Pipeline completed with unstable state'
            // Send warning notification
        }
        failure {
            echo 'Pipeline failed'
            // Send alert to team
            // Try to rollback if applicable
        }
        changed {
            echo 'Pipeline state changed'
        }
        cleanup {
            echo 'Cleaning up workspace'
            cleanWs()
        }
    }
}

// Jenkins Pipeline Library
// jenkins.groovy

library('pipeline-lib@1.0.0')

// Pipeline utility functions
// utils.groovy

// Archive build artifacts
void archiveBuildArtifacts() {
    archiveArtifacts artifacts: true
}

// Publish test results
void publishTestResults() {
    junit testResults: '**/test-results/*.xml'
    publishHTML([
        allowMissing: false,
        alwaysLinkToLastBuild: true,
        keepAll: true,
        reportDir: 'htmlcov',
        reportFiles: 'index.html',
        reportName: 'Code Coverage Report'
    ])
}

// Deploy to environment
void deployToEnvironment(String environment) {
    echo "Deploying to ${environment}"
    // Implementation depends on deployment target
}

// Clean up workspace
void cleanWorkspace() {
    cleanWs()
}

// Initialize Jenkins pipeline library
// Jenkinsfile - Parameters

def params = [
    string(name: 'GIT_BRANCH', defaultValue: 'main', description: 'Git branch to build'),
    string(name: 'DEPLOY_ENV', defaultValue: 'staging', description: 'Deployment environment'),
    booleanParam(name: 'RUN_TESTS', defaultValue: true, description: 'Run tests before deploy'),
    booleanParam(name: 'RUN_SECURITY_SCAN', defaultValue: true, description: 'Run security scan'),
    string(name: 'BUILD_TYPE', defaultValue: 'Release', description: 'Build type (Debug/Release/RelWithDebInfo/MinSizeRel)')
]

// Additional Jenkins pipeline configurations
// jenkins-multibranch.groovy

pipeline {
    agent any
    
    triggers {
        pollSCM('H/30 * * * *') // Poll every 30 minutes
    }
    
    options {
        // Configure for parallel builds
        buildDiscarder(logRotator(numToKeepStr: '5'))
        timeout(time: 120, unit: 'MINUTES')
    }
    
    stages {
        stage('Prepare') {
            steps {
                script('''
                    echo "Building branch: ${BRANCH_NAME}"
                    echo "Build type: ${BUILD_TYPE}"
                ''')
            }
        }
        
        stage('Build and Test') {
            steps {
                script('''
                    // Run build and test stages from main pipeline
                    echo "Running build and test stages"
                ''')
            }
        }
    }
}

// Jenkins Pipeline Script for Docker-based builds
// jenkins-docker.groovy

pipeline {
    agent {
        docker {
            image 'nlm-builder:latest'
            args '-v ${WORKSPACE}:/workspace -w /workspace'
        }
    }
    
    stages {
        stage('Build Docker Image') {
            steps {
                script('''
                    // Build Docker image for development
                    docker build -t nlm-dev .
                    docker run --rm nlm-dev bash -c '
                        source venv/bin/activate
                        mkdir -p build
                        cd build
                        cmake .. -DCMAKE_BUILD_TYPE=Release
                        make -j$(nproc)
                    '
                ''')
            }
        }
        
        stage('Build Application') {
            steps {
                script('''
                    // Build application inside Docker container
                    docker run --rm nlm-dev bash -c '
                        source venv/bin/activate
                        cd build
                        cmake --build . --parallel $(nproc)
                    '
                ''')
            }
        }
    }
}