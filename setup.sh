#!/bin/bash
# Development Setup Script
# Set up the development environment for NLM Phase 6

# Exit on error
set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Print colored output
function print_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

function print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

function print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if we're in the right directory
if [ ! -f "CMakeLists.txt" ]; then
    print_error "CMakeLists.txt not found. Please run this script from the NLM project root."
    exit 1
fi

print_info "Setting up NLM Phase 6 development environment..."

# Check for required tools
check_command() {
    if ! command -v $1 &> /dev/null; then
        print_warning "$1 not found. Please install it."
        return 1
    fi
    print_info "Found $1: $(command -v $1)"
}

check_command "cmake"
check_command "g++"
check_command "make"
check_command "git"
check_command "python3"

print_info "All required tools are available."

# Create build directory
if [ ! -d "build" ]; then
    print_info "Creating build directory..."
    mkdir build
fi

# Configure CMake
cd build
print_info "Configuring with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_STANDARD=20 \
    -DCMAKE_CXX_STANDARD_REQUIRED=ON \
    && print_info "Configuration successful." || {
    print_error "CMake configuration failed."
    exit 1
}

# Build the project
print_info "Building the project..."
if make -j$(nproc) 2>&1 | tail -20 | grep -q "error"; then
    print_error "Build failed. Please check the error messages above."
    exit 1
else
    print_info "Build successful!"
fi

cd ..

# Create examples directory
if [ ! -d "examples" ]; then
    print_info "Creating examples directory..."
    mkdir examples
    print_warning "Example scripts have been created. Please make them executable with chmod +x examples/*.sh"
fi

# List available targets
print_info "\nAvailable build targets:"
echo "================"
echo "  nlm              : Basic NLM brain simulation"
echo "  nlm_phase3_demo   : Phase 3 demonstration program"
echo "  nlm_phase4_demo   : Phase 4 demonstration program"
echo "  nlm_phase6_demo   : Phase 6 integration demo (default)"
echo "  nlm_core         : NLM core library"
echo "  nlm_agent        : Agent system library"
echo "  nlm_world        : World system library"
echo "  nlm_phase3       : Phase 3 experiment library"
echo "  nlm_phase4       : Phase 4 experiment library"
echo "  nlm_phase6       : Phase 6 experiment library"
echo ""

# Create quick start script
print_info "Creating quick start script..."
cat > quick_start.sh << 'EOF'
#!/bin/bash
# Quick start script for NLM Phase 6

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

function print_info() { echo -e "${GREEN}[INFO]${NC} $1"; }
function print_error() { echo -e "${RED}[ERROR]${NC} $1"; }

# Check if we're in the right directory
if [ ! -f "CMakeLists.txt" ]; then
    print_error "CMakeLists.txt not found. Please run this script from the NLM project root."
    exit 1
fi

# Build if needed
if [ ! -f "build/nlm_phase6_demo" ]; then
    print_info "Building NLM Phase 6..."
    mkdir -p build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make -j4
    cd ..
fi

# Run quick demo
print_info "Running NLM Phase 6 demo..."
./build/nlm_phase6_demo --preset benchmark --verbose
EOF

chmod +x quick_start.sh
print_info "Created quick_start.sh script. Run it with: ./quick_start.sh"

print_info "\n" > /dev/null 2>&1
print_info "Setup completed successfully!"
print_info "\nNext steps:"
print_info "1. Make example scripts executable: chmod +x examples/*.sh"
print_info "2. Run quick demo: ./quick_start.sh"
print_info "3. Run research experiment: ./examples/research_experiment.sh"
print_info "4. Run production deployment: ./examples/production_deployment.sh"
print_info "5. Run performance benchmark: ./examples/performance_benchmark.sh"
print_info "\nFor more information, see the documentation in docs/ directory."
