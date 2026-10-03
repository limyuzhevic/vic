#!/usr/bin/env bash
#
# Development Setup Script for NLM Python Bindings
#
# This script provides convenient commands for developing, testing, and building
# the NLM Python bindings and related components.
#

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to check Python version
check_python_version() {
    if ! command_exists python3; then
        print_error "Python 3 is not installed"
        exit 1
    fi
    
    local version=$(python3 -c "import sys; print(f'{sys.version_info.major}.{sys.version_info.minor}')")
    local major=$(python3 -c "import sys; print(sys.version_info.major)")
    local minor=$(python3 -c "import sys; print(sys.version_info.minor)")
    
    if [ "$major" -lt 3 ] || ([ "$major" -eq 3 ] && [ "$minor" -lt 8 ]); then
        print_error "Python 3.8+ is required, found Python $version"
        exit 1
    fi
    
    print_success "Python $version detected"
}

# Function to setup development environment
setup_dev_environment() {
    print_info "Setting up development environment..."
    
    # Check for required tools
    if ! command_exists cmake; then
        print_error "CMake is not installed. Please install CMake 3.16 or higher."
        exit 1
    fi
    
    if ! command_exists make; then
        print_error "Make is not installed. Please install make."
        exit 1
    fi
    
    if ! command_exists pip3; then
        print_error "pip3 is not installed. Please install pip."
        exit 1
    fi
    
    print_success "All required tools are available"
}

# Function to build C++ components
build_cpp() {
    print_info "Building C++ components..."
    
    local build_dir="build"
    local build_type="${1:-Release}"
    
    if [ ! -d "$build_dir" ]; then
        mkdir -p "$build_dir"
    fi
    
    cd "$build_dir"
    
    cmake .. -DCMAKE_BUILD_TYPE="$build_type" \
             -DCMAKE_CXX_STANDARD=20 \
             -DCMAKE_CXX_STANDARD_REQUIRED=ON \
             -DCMAKE_CXX_EXTENSIONS=OFF
    
    if [ $? -ne 0 ]; then
        print_error "CMake configuration failed"
        exit 1
    fi
    
    make -j$(nproc)
    
    if [ $? -ne 0 ]; then
        print_error "Build failed"
        exit 1
    fi
    
    cd ..
    print_success "C++ components built successfully"
}

# Function to install Python bindings
install_python_bindings() {
    print_info "Installing Python bindings..."
    
    if ! command_exists pip3; then
        print_error "pip3 is not available"
        exit 1
    fi
    
    # Install dependencies
    print_info "Installing Python dependencies..."
    pip3 install pybind11 scikit-build-core pytest numpy
    
    if [ $? -ne 0 ]; then
        print_error "Failed to install Python dependencies"
        exit 1
    fi
    
    # Install the package itself
    print_info "Installing NLM Python package..."
    pip3 install -e .
    
    if [ $? -ne 0 ]; then
        print_error "Failed to install NLM Python package"
        exit 1
    fi
    
    print_success "Python bindings installed successfully"
}

# Function to run tests
run_tests() {
    print_info "Running tests..."
    
    if ! command_exists python3; then
        print_error "python3 is not available"
        exit 1
    fi
    
    # Run Python tests
    if [ -d "python" ] && [ -f "python/setup.py" ]; then
        print_info "Running Python tests..."
        cd python
        python3 -m pytest tests/ -v
        
        if [ $? -ne 0 ]; then
            print_error "Python tests failed"
            exit 1
        fi
        
        cd ..
    fi
    
    # Run C++ tests if available
    if [ -d "build" ] && [ -f "build/nlm_test" ]; then
        print_info "Running C++ tests..."
        ./build/nlm_test --output-on-failure
        
        if [ $? -ne 0 ]; then
            print_error "C++ tests failed"
            exit 1
        fi
    fi
    
    print_success "All tests passed"
}

# Function to run examples
run_examples() {
    print_info "Running examples..."
    
    if [ ! -f "python/nlm_examples.py" ]; then
        print_error "Examples file not found: python/nlm_examples.py"
        exit 1
    fi
    
    python3 python/nlm_examples.py
    
    if [ $? -ne 0 ]; then
        print_error "Examples failed to run"
        exit 1
    fi
    
    print_success "Examples completed successfully"
}

# Function to create development environment
create_dev_environment() {
    print_info "Creating development environment..."
    
    # Create necessary directories
    mkdir -p build
    mkdir -p examples
    mkdir -p docs
    mkdir -p python
    
    # Create requirements file
    cat > python/requirements.txt << EOF
pybind11>=2.11.0
scikit-build-core>=0.5.0
pytest>=7.0
numpy>=1.20
EOF
    
    # Create setup.py for Python bindings
    cat > python/setup.py << EOF
from setuptools import setup, find_packages
from setuptools.command.build_ext import build_ext
from setuptools.extension import Extension
import subprocess
import os

class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=''):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)

class CMakeBuild(build_ext):
    def run(self):
        for ext in self.extensions:
            self.build_extension(ext)

    def build_extension(self, ext):
        extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
        
        cmake_args = [
            f'-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={extdir}',
            f'-DPYTHON_EXECUTABLE={sys.executable}'
        ]
        
        cfg = 'Debug' if self.debug else 'Release'
        build_args = ['--config', cfg]
        
        cmake_args += [f'-DCMAKE_BUILD_TYPE={cfg}']
        build_args += ['--', '-j4']
        
        env = os.environ.copy()
        env['CXXFLAGS'] = '{} -DVERSION_INFO=\"{}\"'.format(
            env.get('CXXFLAGS', ''),
            self.distribution.get_version()
        )
        
        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)
        
        subprocess.check_call(['cmake', ext.sourcedir] + cmake_args,
                              cwd=self.build_temp, env=env)
        subprocess.check_call(['cmake', '--build', '.'] + build_args,
                              cwd=self.build_temp)

setup(
    name='pynlm',
    version='0.1.0-enhanced',
    author='NLM Project Team',
    description='Enhanced Python bindings for NLM neural simulation framework',
    long_description=open('README.md').read() if os.path.exists('README.md') else '',
    packages=find_packages(),
    ext_modules=[CMakeExtension('pynlm.python.bindings')],
    cmdclass=dict(build_ext=CMakeBuild),
    zip_safe=False,
)
EOF
    
    print_success "Development environment created"
}

# Function to show help menu
show_help() {
    echo "NLM Development Setup Script"
    echo "=" * 50
    echo "Commands:"
    echo "  setup                    Setup development environment"
    echo "  build [type]             Build C++ components (Release/Debug)"
    echo "  install                  Install Python bindings"
    echo "  test                      Run all tests"
    echo "  examples                  Run examples"
    echo "  clean                     Clean build artifacts"
    echo "  help                      Show this help message"
    echo "=" * 50
}

# Function to clean build artifacts
clean() {
    print_info "Cleaning build artifacts..."
    
    rm -rf build
    rm -rf python/*.so
    rm -rf python/*.pyd
    rm -rf python/__pycache__
    
    print_success "Build artifacts cleaned"
}

# Main script logic
case "$1" in
    "setup")
        check_python_version
        setup_dev_environment
        build_cpp
        install_python_bindings
        print_success "Development environment setup complete!"
        ;;
    "build")
        check_python_version
        setup_dev_environment
        build_cpp "$2"
        ;;
    "install")
        check_python_version
        setup_dev_environment
        install_python_bindings
        ;;
    "test")
        check_python_version
        run_tests
        ;;
    "examples")
        check_python_version
        run_examples
        ;;
    "create-dev-env")
        create_dev_environment
        ;;
    "clean")
        clean
        ;;
    "help"|"--help"|"-h")
        show_help
        ;;
    *)
        print_error "Unknown command: $1"
        echo "Use 'help' to see available commands."
        exit 1
        ;;
esac