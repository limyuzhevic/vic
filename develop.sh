#!/bin/bash

# NLM Development Environment Setup Script

# Exit on error
set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Print colored output
print_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Detect platform
detect_platform() {
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        echo "linux"
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        echo "macos"
    elif [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" ]]; then
        echo "windows"
    else
        print_warn "Unknown platform: $OSTYPE. Using generic Linux commands."
        echo "linux"
    fi
}

# Check for required tools
check_requirements() {
    print_info "Checking development requirements..."

    local missing_tools=()

    # Check for essential tools
    if ! command -v cmake &> /dev/null; then
        missing_tools+=("CMake")
    fi

    if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
        missing_tools+=("C++ compiler")
    fi

    if ! command -v python3 &> /dev/null; then
        missing_tools+=("Python 3")
    fi

    if ! command -v pip3 &> /dev/null; then
        missing_tools+=("pip")
    fi

    # Platform-specific checks
    local platform=$(detect_platform)
    case "$platform" in
        "linux")
            if ! command -v git &> /dev/null; then
                missing_tools+=("git")
            fi
            ;;
        "macos")
            if ! command -v brew &> /dev/null; then
                print_warn "Homebrew (brew) not found. Some tools may need manual installation."
            fi
            ;;
    esac

    if [ ${#missing_tools[@]} -ne 0 ]; then
        print_error "Missing required tools: ${missing_tools[*]}"
        echo "Please install missing tools and try again."

        case "$platform" in
            "linux")
                echo "Ubuntu/Debian: sudo apt-get install "${missing_tools[*]}""
                echo "CentOS/RHEL: sudo yum install "${missing_tools[*]}""
                ;;
            "macos")
                echo "macOS: brew install "${missing_tools[*]}""
                ;;
            "windows")
                echo "Windows: Download from official websites or use package managers"
                ;;
        esac
        exit 1
    fi

    print_info "All development requirements are satisfied."
}

# Setup C++ development environment
setup_cpp() {
    print_info "Setting up C++ development environment..."

    local platform=$(detect_platform)

    case "$platform" in
        "linux")
            # Install common Linux dependencies
            if command -v apt-get &> /dev/null; then
                print_info "Installing Linux dependencies..."
                sudo apt-get update
                sudo apt-get install -y \
                    build-essential \
                    cmake \
                    git \
                    libgtest-dev \
                    lld \
                    clang-tools
            elif command -v yum &> /dev/null; then
                print_info "Installing RHEL/CentOS dependencies..."
                sudo yum install -y \
                    gcc-c++ \
                    cmake \
                    git \
                    centos-release-scl \
                    devtoolset-7-gcc-c++
            fi
            ;;
        "macos")
            print_info "Setting up macOS dependencies..."
            # Check if Homebrew is available
            if command -v brew &> /dev/null; then
                # Install common macOS development tools
                brew install cmake git

                # Check for Xcode command line tools
                if ! xcode-select --print-path &> /dev/null; then
                    print_info "Installing Xcode command line tools..."
                    xcode-select --install
                fi
            else
                print_warn "Homebrew not found. Please install from https://brew.sh/"
            fi
            ;;
        "windows")
            print_info "Windows users: Please ensure Visual Studio Build Tools are installed."
            print_info "Install from: https://visualstudio.microsoft.com/downloads/"
            print_info "Required components: C++ build tools, Git for Windows"
            ;;
    esac

    print_info "C++ development environment setup completed."
}

# Setup Python development environment
setup_python() {
    print_info "Setting up Python development environment..."

    # Create and activate virtual environment
    if [ ! -d "venv" ]; then
        print_info "Creating Python virtual environment..."
        python3 -m venv venv
    fi

    # Activate virtual environment
    source venv/bin/activate

    # Upgrade pip and setuptools
    print_info "Upgrading Python package manager..."
    pip install --upgrade pip setuptools wheel

    # Install development dependencies
    print_info "Installing Python development dependencies..."
    pip install -e .[dev]

    # Install pre-commit hooks
    print_info "Setting up pre-commit hooks..."
    pip install pre-commit
    pre-commit install

    print_info "Python development environment setup completed."
}

# Setup git configuration
setup_git() {
    if ! command -v git &> /dev/null; then
        print_warn "Git not found. Skipping git configuration."
        return
    fi

    print_info "Configuring git..."

    # Check if git user is configured
    if ! git config user.name &> /dev/null; then
        read -p "Enter your Git name: " git_name
        git config user.name "$git_name"
    fi

    if ! git config user.email &> /dev/null; then
        read -p "Enter your Git email: " git_email
        git config user.email "$git_email"
    fi

    # Set up git hooks
    print_info "Setting up git hooks..."
    mkdir -p .git/hooks

    # Create pre-commit hook template
    cat > .git/hooks/pre-commit << 'EOF'
#!/bin/bash
# Pre-commit hook for NLM project

# Run linting and formatting checks
if command -v black &> /dev/null; then
    black --check python/ src/ || exit 1
fi

if command -v ruff &> /dev/null; then
    ruff check python/ src/ || exit 1
fi

if command -v mypy &> /dev/null; then
    mypy python/ || exit 1
fi

# Run tests
if command -v pytest &> /dev/null; then
    cd python && pytest tests/ --no-cov || exit 1
fi

print_info "All pre-commit checks passed."
EOF

    chmod +x .git/hooks/pre-commit

    print_info "Git configuration completed."
}

# Create development workspace
setup_workspace() {
    print_info "Creating development workspace..."

    # Create common directories
    mkdir -p logs
    mkdir -p benchmarks
    mkdir -p reports
    mkdir -p docs/api
    mkdir -p examples/advanced
    mkdir -p tests/integration
    mkdir -p tests/performance
    mkdir -p tests/stress

    # Create development configuration files
    create_dev_config

    print_info "Development workspace setup completed."
}

# Create development configuration files
create_dev_config() {
    print_info "Creating development configuration files..."

    # Create .env file
    cat > .env << 'EOF'
# NLM Development Environment Variables

# Logging
NLM_LOG_LEVEL=DEBUG
NLM_LOG_FILE=logs/nlm.log

# Data and checkpoints
NLM_DATA_DIR=./data
NLM_CHECKPOINT_DIR=./checkpoints

# Performance
NLM_PERF_THRESHOLD=1000.0
NLM_PERF_HISTORY_SIZE=1000

# Python bindings
PYNLB_DEBUG=true
PYNLB_CACHE_DIR=./python_cache

# Testing
PYTEST_VERBOSE=1
PYTEST_COVERAGE=1

# Development
DEV_MODE=true
AUTO_FORMAT=true
EOF

    # Create .gitignore file
    cat > .gitignore << 'EOF'
# Build directories
build/
build_*

# Python virtual environment
venv/
.env
.venv/

# IDE directories
.vscode/
.idea/
*.swp
*.swo

# OS specific files
.DS_Store
Thumbs.db

# Logs and temporary files
logs/
*.log
tmp/
temp/

# Test artifacts
htmlcov/
.coverage
.pytest_cache/
benchmark/

# Python package files
*.egg-info/
dist/
pynlm.egg-info/

# C++ artifacts
*.o
*.obj
*.so
*.dylib
*.dll
*.exe

# Documentation
docs/api/html/

# Data and model files
data/
models/

# Caches
.python-cache/

# Environment variables
.env.local

# IDE config
.idea/
vscode-user-data/

# Platform-specific
*.swp
*.swo

# Backup files
*.bak
*.backup

# Editor temp files
*~
EOF

    print_info "Configuration files created."
}

# Run development tests
run_dev_tests() {
    print_info "Running development environment tests..."

    # Test Python installation
    python3 --version
    pip3 --version

    # Test CMake
    cmake --version

    # Test Git
    git --version

    # Check if we can build the project
    if [ -d "build" ]; then
        print_info "Build directory exists. Checking build..."
        cd build && cmake .. > /dev/null 2>&1
        cd ..
        print_info "Build configuration successful."
    else
        print_warn "No build directory found. Run './develop.sh setup-cpp' manually."
    fi

    print_info "Development environment tests completed."
}

# Main function
main() {
    echo "NLM Development Environment Setup"
    echo "================================"
    echo

    # Parse command line arguments
    case "$1" in
        "setup")
            check_requirements
            setup_cpp
            setup_python
            setup_git
            setup_workspace
            run_dev_tests
            print_info "Development environment setup completed successfully!"
            echo
            echo "Next steps:"
            echo "1. Activate Python environment: source venv/bin/activate"
            echo "2. Build the project: mkdir build && cd build && cmake .. && make"
            echo "3. Run C++ tests: ctest --output-on-failure"
            echo "4. Run Python tests: pytest python/tests/ -v"
            echo
            echo "For more information, see BUILD.md"
            ;;
        "setup-cpp")
            check_requirements
            setup_cpp
            ;;
        "setup-python")
            check_requirements
            setup_python
            ;;
        "setup-git")
            setup_git
            ;;
        "workspace")
            setup_workspace
            ;;
        "test")
            run_dev_tests
            ;;
        "help")
            echo "Usage: $0 [COMMAND]"
            echo ""
            echo "Commands:"
            echo "  setup        Full development environment setup"
            echo "  setup-cpp    Setup C++ development environment"
            echo "  setup-python Setup Python development environment"
            echo "  setup-git    Configure git and hooks"
            echo "  workspace     Create development workspace structure"
            echo "  test         Run development environment tests"
            echo "  help         Show this help message"
            ;;
        *)
            echo "Usage: $0 [COMMAND]"
            echo "Run '$0 help' for more information."
            exit 1
            ;;
    esac
}

# Run main function
main "$@"