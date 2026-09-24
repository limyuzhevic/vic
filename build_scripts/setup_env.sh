#!/bin/bash

# setup_env.sh - NLM Environment Setup Script
# Setup development environment for NLM project

set -euo pipefail

# Default configuration
VENV_NAME="venv"
PYTHON_VERSION="3.11"
INSTALL_TOOLS=true
SETUP_DEPS=true
CONFIG_WORKSPACE=true

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Logging functions
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Print usage
usage() {
    cat << EOF
Usage: $0 [OPTIONS]

Setup development environment for NLM project.

Options:
  --venv-name NAME            Virtual environment name (default: venv)
  --python-version VERSION    Python version (default: 3.11)
  --no-tools                  Skip installing development tools
  --no-deps                   Skip installing dependencies
  --no-workspace               Skip workspace configuration
  --help                      Show this help message

Examples:
  $0                                      # Setup default environment
  $0 --venv-name myenv                     # Use custom virtual environment name
  $0 --no-deps --no-tools                   # Skip dependencies and tools
  $0 --no-workspace                         # Skip workspace configuration
EOF
}

# Check system dependencies
check_system_dependencies() {
    log_info "Checking system dependencies..."
    
    # Check for required system tools
    local required_tools=("git" "python3" "make" "cmake")
    local missing_tools=()
    
    for tool in "${required_tools[@]}"; do
        if ! command -v "$tool" &> /dev/null; then
            missing_tools+=("$tool")
        fi
    done
    
    if [[ ${#missing_tools[@]} -gt 0 ]]; then
        log_error "Missing required system tools: ${missing_tools[*]}"
        log_info "Please install these tools and try again."
        exit 1
    fi
    
    log_success "All system dependencies found"
}

# Setup Python virtual environment
setup_python_venv() {
    log_info "Setting up Python virtual environment: $VENV_NAME"
    
    # Check if Python is available
    if ! command -v python3 &> /dev/null; then
        log_error "Python3 not found. Please install Python3."
        exit 1
    fi
    
    # Check if virtual environment already exists
    if [[ -d "$VENV_NAME" ]]; then
        log_warning "Virtual environment '$VENV_NAME' already exists."
        log_info "To use it, activate it with: source $VENV_NAME/bin/activate"
        return 0
    fi
    
    # Create virtual environment
    log_info "Creating virtual environment..."
    python3 -m venv "$VENV_NAME"
    
    # Activate virtual environment
    source "$VENV_NAME/bin/activate"
    
    # Upgrade pip and setuptools
    log_info "Upgrading pip and setuptools..."
    pip install --no-cache-dir --upgrade pip setuptools wheel
    
    log_success "Python virtual environment '$VENV_NAME' created"
}

# Install development tools
install_development_tools() {
    if [[ "$INSTALL_TOOLS" == false ]]; then
        log_warning "Skipping development tools installation (user choice)"
        return 0
    fi
    
    log_info "Installing development tools..."
    
    source "$VENV_NAME/bin/activate"
    
    # Install core development tools
    log_info "Installing core development tools..."
    pip install --no-cache-dir scikit-build-core>=0.5.0 pybind11>=2.11.0
    
    # Install development dependencies
    log_info "Installing development dependencies..."
    pip install --no-cache-dir -e .[dev]
    
    # Install linting tools
    log_info "Installing linting tools..."
    pip install --no-cache-dir flake8 black isort mypy pre-commit
    
    # Install testing tools
    log_info "Installing testing tools..."
    pip install --no-cache-dir pytest pytest-cov pytest-html pytest-timeout
    
    # Install documentation tools
    log_info "Installing documentation tools..."
    pip install --no-cache-dir sphinx sphinx-rtd-theme breathe myst-parser
    
    # Install security scanning tools
    log_info "Installing security scanning tools..."
    pip install --no-cache-dir safety bandit semgrep
    
    log_success "Development tools installed"
}

# Install project dependencies
install_project_dependencies() {
    if [[ "$SETUP_DEPS" == false ]]; then
        log_warning "Skipping project dependencies installation (user choice)"
        return 0
    fi
    
    log_info "Installing project dependencies..."
    
    source "$VENV_NAME/bin/activate"
    
    # Install the project in development mode
    log_info "Installing project in development mode..."
    pip install --no-cache-dir -e .
    
    log_success "Project dependencies installed"
}

# Configure workspace
configure_workspace() {
    if [[ "$CONFIG_WORKSPACE" == false ]]; then
        log_warning "Skipping workspace configuration (user choice)"
        return 0
    fi
    
    log_info "Configuring workspace..."
    
    # Create build directory
    mkdir -p build
    
    # Create test results directory
    mkdir -p test-results
    
    # Create benchmark results directory
    mkdir -p benchmark_results
    
    # Create documentation build directory
    mkdir -p docs/_build
    
    # Create quality reports directory
    mkdir -p quality_reports
    
    # Create artifacts directory
    mkdir -p artifacts
    
    # Create virtual environment directory
    mkdir -p "$VENV_NAME"
    
    log_success "Workspace configured"
}

# Setup git hooks
setup_git_hooks() {
    log_info "Setting up git hooks..."
    
    if [[ ! -d ".git" ]]; then
        log_warning "Not in a git repository. Skipping git hook setup."
        return 0
    fi
    
    # Create pre-commit hook
    if [[ ! -f ".git/hooks/pre-commit" ]]; then
        log_info "Creating pre-commit hook..."
        cat > ".git/hooks/pre-commit" << 'EOF'
#!/bin/bash

# Pre-commit hook for NLM project

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

# Function to log messages
log_info() {
    echo -e "${GREEN}[PRE-COMMIT]${NC} $1"
}

log_error() {
    echo -e "${RED}[PRE-COMMIT]${NC} $1"
}

# Check if we're in the NLM project root
if [[ ! -f "pyproject.toml" ]] || [[ ! -f "CMakeLists.txt" ]]; then
    echo "Not in NLM project root directory. Skipping pre-commit checks."
    exit 0
fi

# Activate virtual environment if it exists
if [[ -d "venv" ]]; then
    source venv/bin/activate
fi

# Run code style checks
log_info "Running code style checks..."

# Check Python code
if command -v black &> /dev/null; then
    if ! black --check src/ python/ tests/ 2>/dev/null; then
        log_error "Black formatting check failed. Run 'black src/ python/ tests/' to fix."
        exit 1
    fi
fi

if command -v isort &> /dev/null; then
    if ! isort --check-only src/ python/ tests/ 2>/dev/null; then
        log_error "isort check failed. Run 'isort src/ python/ tests/' to fix."
        exit 1
    fi
fi

if command -v flake8 &> /dev/null; then
    if ! flake8 src/ python/ tests/ 2>/dev/null; then
        log_error "flake8 check failed."
        exit 1
    fi
fi

# Check C++ code style
if command -v clang-format &> /dev/null; then
    local changes=$(find src -name "*.cpp" -o -name "*.h" -o -name "*.hpp" | xargs clang-format --dry-run 2>/dev/null | grep -c "changed")
    if [[ $changes -gt 0 ]]; then
        log_error "clang-format check failed. Run 'clang-format -i src/*.cpp src/*.h src/*.hpp' to fix."
        exit 1
    fi
fi

# Check CMake formatting
if command -v cmake-format &> /dev/null; then
    if ! cmake-format -c .cmake-format.ini -d . 2>/dev/null; then
        log_error "cmake-format check failed. Run 'cmake-format -i CMakeLists.txt' to fix."
        exit 1
    fi
fi

log_info "All pre-commit checks passed."
EOF
        chmod +x ".git/hooks/pre-commit"
    fi
    
    # Create commit-msg hook
    if [[ ! -f ".git/hooks/commit-msg" ]]; then
        log_info "Creating commit-msg hook..."
        cat > ".git/hooks/commit-msg" << 'EOF'
#!/bin/bash

# Commit message hook for NLM project

COMMIT_MSG_FILE=$1

# Check for required commit message format
if ! grep -qE "^\w+/(\w+-)*[a-z]+(-[a-z]+)*$" "$COMMIT_MSG_FILE" 2>/dev/null; then
    echo "Warning: Commit message doesn't follow the conventional commit format (type/purpose)."
    echo "Example: feat/add-build-documentation"
    echo "Continue? (y/N)"
    read -r answer
    if [[ "$answer" != "y" && "$answer" != "Y" ]]; then
        exit 1
    fi
fi

# Check for trailing whitespace
if grep -qE "[[:space:]]$" "$COMMIT_MSG_FILE"; then
    echo "Error: Commit message contains trailing whitespace."
    exit 1
fi

# Check for line length (conventional commit recommended)
if awk 'NF > 72' "$COMMIT_MSG_FILE"; then
    echo "Warning: Commit message exceeds 72 characters per line."
    echo "Continue? (y/N)"
    read -r answer
    if [[ "$answer" != "y" && "$answer" != "Y" ]]; then
        exit 1
    fi
fi
EOF
        chmod +x ".git/hooks/commit-msg"
    fi
    
    log_success "Git hooks configured"
}

# Setup development environment
setup_development_environment() {
    log_info "Setting up development environment..."
    
    # Create .gitignore if it doesn't exist
    if [[ ! -f ".gitignore" ]]; then
        log_info "Creating .gitignore file..."
        cat > ".gitignore" << 'EOF'
# Build directories
build/
build-*
dist/
*.egg-info/
*.egg
venv/
.pytest_cache/
__pycache__/
*.pyc
*.pyo
*.pyd
.Python
*.so
*.dylib
*.dll
*.exe

# Test results
coverage.xml
htmlcov/
.test_results/

# Benchmark results
benchmark_results/
performance_metrics.json

# Documentation
_docs/
docs/_build/

# IDE files
.vscode/
.idea/
*.swp
*.swo

# OS files
.DS_Store
Thumbs.db

# Environment files
.env
.env.local
.env.development

# Configuration files
*.cfg
*.ini
*.properties

# Log files
*.log

# Temporary files
*.tmp
*.temp

# Coverage reports
.coverage
coverage

# Distribution packages
*.tar.gz
*.zip
*.wheel

# Python virtual environment
venv/
ENV/
env/

# Node.js
node_modules/
.npm

# Other
*.orig
*.rej
*.bak
EOF
    fi
    
    # Create environment file if it doesn't exist
    if [[ ! -f ".env" ]]; then
        log_info "Creating .env file..."
        cat > ".env" << 'EOF'
# NLM Environment Configuration

# Development settings
NLM_VERSION=0.1.0
NLM_BUILD_TYPE=Release
NLM_PYTHON_VERSION=3.11

# Python bindings
NLM_PYTHON_BINDINGS=ON

# Testing
NLM_ENABLE_TESTS=ON
NLM_TEST_TIMEOUT=300

# Build options
NLM_ENABLE_MPI=OFF
NLM_ENABLE_CUDA=OFF
NLM_ENABLE_SIMD=ON
NLM_ENABLE_VALIDATION=ON

# Logging
NLM_LOG_LEVEL=INFO
NLM_LOG_FILE=logs/nlm.log

# Performance
NLM_PARALLEL_JOBS=4
NLM_CACHE_SIZE=1024

# Development
NLM_DEV_MODE=ON
NLM_AUTOFORMAT=OFF
NLM_AUTO_TEST=OFF

# Paths
NLM_SRC_DIR=src
NLM_BUILD_DIR=build
NLM_TESTS_DIR=tests
NLM_DOCS_DIR=docs

# External dependencies
NLM_OPENMPI_PATH=/usr/local/openmpi
NLM_LAPACK_PATH=/usr/local
NLM_BLAS_PATH=/usr/local
EOF
    fi
    
    log_success "Development environment configured"
}

# Main setup function
main() {
    log_info "Starting NLM environment setup..."
    log_info "Virtual environment: $VENV_NAME"
    log_info "Python version: $PYTHON_VERSION"
    
    # Check system dependencies
    check_system_dependencies
    
    # Setup Python virtual environment
    setup_python_venv
    
    # Install development tools
    install_development_tools
    
    # Install project dependencies
    install_project_dependencies
    
    # Configure workspace
    configure_workspace
    
    # Setup git hooks
    setup_git_hooks
    
    # Setup development environment
    setup_development_environment
    
    log_success "Environment setup completed successfully!"
    log_info "To activate the virtual environment, run:"
    log_info "  source $VENV_NAME/bin/activate"
    log_info "To start building, run:"
    log_info "  ./build_scripts/build.sh"
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --venv-name)
            VENV_NAME="$2"
            shift 2
            ;;
        --python-version)
            PYTHON_VERSION="$2"
            shift 2
            ;;
        --no-tools)
            INSTALL_TOOLS=false
            shift
            ;;
        --no-deps)
            SETUP_DEPS=false
            shift
            ;;
        --no-workspace)
            CONFIG_WORKSPACE=false
            shift
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            log_error "Unknown option: $1"
            usage
            exit 1
            ;;
    esac
done

# Run main function
main "$@"