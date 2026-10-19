chmod +x $0

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Print colored output
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

# Check if the build directory exists
if [ ! -d "build" ]; then
    print_error "Build directory not found. Please run: mkdir build && cd build && cmake .."
    exit 1
fi

# Check if the demo executable exists
if [ ! -f "build/nlm_phase6_demo" ]; then
    print_error "Demo executable not found. Please build the project first."
    print_info "Running: cmake .. && make -j4"
    cd build
    cmake .. > /dev/null 2>&1
    make -j4 > /dev/null 2>&1
    cd ..
    
    if [ ! -f "build/nlm_phase6_demo" ]; then
        print_error "Failed to build the demo. Please check the build directory."
        exit 1
    fi
fi

# Show help
function show_help {
    echo "NLM Phase 6 Demo - Enhanced Integration Test"
    echo "=========================================="
    echo ""
    echo "This script runs the NLM Phase 6 integration demo with improved CLI and configuration."
    echo ""
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  -h, --help                    Show this help message"
    echo "      --neurons <N>              Number of neurons (default: 500)"
    echo "      --steps <N>                Max simulation steps (default: 2000)"
    echo "      --config <file>             Load configuration from file"
    echo "      --log-level <level>         Logging level: error,warn,info,debug,trace"
    echo "      --no-progress               Disable progress reporting"
    echo "      --no-validation             Disable integration validation"
    echo "      --profiler                  Enable performance profiling"
    echo "      --checkpoint-path <path>    Path for checkpoint files"
    echo "      --enable-replay             Enable replay system"
    echo "      --enable-development        Enable development system"
    echo "      --example-config            Show example configuration"
    echo ""
    echo "Examples:"
    echo "  $0 --neurons 1000 --steps 5000\n"
    echo "  $0 --config nlm_phase6_example.cfg --profiler\n"
    echo "  $0 --log-level debug --no-progress\n"
    echo ""
    echo "For more details, visit: https://github.com/nlm-project/nlm"
    exit 0
}

# Show example configuration
function show_example_config {
    echo "Example configuration file for NLM Phase 6"
    echo "========================================="
    echo ""
    echo "Copy this content to a file named 'nlm_config.cfg':"
    echo ""
    cat configs/nlm_phase6_example.cfg
}

# Run the demo with arguments
function run_demo {
    local args="$@"
    
    print_info "Running NLM Phase 6 integration demo..."
    print_info "Arguments: $@"
    
    # Run the demo
    cd build
    ./nlm_phase6_demo $args
    
    local result=$?
    cd ..
    
    return $result
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            show_help
            ;;
        --example-config)
            show_example_config
            exit 0
            ;;
        --)
            shift
            break
            ;;
        *)
            # Check if this is a valid option
            case $1 in
                --neurons|--steps|--log-level|--checkpoint-path)
                    shift
                    if [[ $# -eq 0 ]]; then
                        print_error "Option $1 requires an argument"
                        exit 1
                    fi
                    ;;
                --config)
                    shift
                    if [[ $# -eq 0 ]]; then
                        print_error "Option $1 requires an argument"
                        exit 1
                    fi
                    if [[ ! -f "$1" ]]; then
                        print_error "Config file not found: $1"
                        exit 1
                    fi
                    ;;
            esac
            ;;
    esac
    shift
done

# Run the demo with all arguments
print_info "Starting NLM Phase 6 demo..."
run_demo "$@"

# Check the result
if [[ $? -eq 0 ]]; then
    print_success "Demo completed successfully!"
else
    print_error "Demo failed with exit code: $?"
    exit 1
fi
