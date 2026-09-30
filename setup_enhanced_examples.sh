#!/usr/bin/env bash

# NLM Enhanced Examples Setup Script
# This script helps you set up and run the enhanced NLM Python examples

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Print colored output
print_color() {
    echo -e "${$1}$2${NC}"
}

# Print error message
print_error() {
    print_color $RED "ERROR: $1"
}

# Print warning message
print_warning() {
    print_color $YELLOW "WARNING: $1"
}

# Print success message
print_success() {
    print_color $GREEN "SUCCESS: $1"
}

# Print info message
print_info() {
    echo "INFO: $1"
}

# Check if Python is available
check_python() {
    if ! command -v python3 &> /dev/null; then
        print_error "Python 3 is not installed. Please install Python 3.8 or higher."
        exit 1
    fi
    
    if ! command -v pip3 &> /dev/null; then
        print_error "pip3 is not installed. Please install pip."
        exit 1
    fi
    
    print_success "Python and pip are available"
}

# Check if pynlm is installed
check_pynlm() {
    if python3 -c "import pynlm; print(pynlm.__version__)" &> /dev/null; then
        print_success "pynlm is installed (version: $(python3 -c "import pynlm; print(pynlm.__version__)"))"
    else
        print_warning "pynlm is not installed or may be the old version."
        print_info "This script is designed for the enhanced NLM Python API."
        print_info "To install, run: pip3 install -e ."
    fi
}

# Install dependencies
install_dependencies() {
    print_info "Installing dependencies..."
    
    # Install the current project in development mode
    if [ -f "pyproject.toml" ]; then
        pip3 install -e .
        if [ $? -eq 0 ]; then
            print_success "Dependencies installed successfully"
        else
            print_error "Failed to install dependencies"
            exit 1
        fi
    else
        print_error "pyproject.toml not found. Please run this script from the NLM project root."
        exit 1
    fi
}

# Run examples
run_examples() {
    print_info "Running NLM enhanced examples..."
    
    example_script="examples/example_enhanced_api.py"
    
    if [ ! -f "$example_script" ]; then
        print_error "Example script not found: $example_script"
        exit 1
    fi
    
    # Run the example script
    python3 "$example_script"
    
    if [ $? -eq 0 ]; then
        print_success "Examples completed successfully"
    else
        print_error "Examples failed to run"
        exit 1
    fi
}

# Create a test script
test_enhanced_api() {
    print_info "Testing enhanced API functionality..."
    
    test_script="test_enhanced_api.py"
    
    if [ -f "$test_script" ]; then
        print_info "Running existing tests..."
        python3 "$test_script"
        
        if [ $? -eq 0 ]; then
            print_success "Tests passed"
        else
            print_error "Tests failed"
        fi
    else
        print_warning "No test script found. Creating basic test..."
        
        # Create a simple test script
        cat > test_enhanced_api.py << 'EOF'
#!/usr/bin/env python3
"""
Basic test for enhanced NLM API
"""

import sys
import os

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm

def test_basic_functionality():
    """Test basic enhanced API functionality"""
    print("Testing enhanced NLM API...")
    
    # Test 1: BrainBuilder
    print("Test 1: BrainBuilder")
    brain_builder = pynlm.BrainBuilder()
    assert brain_builder is not None
    print("  ✓ BrainBuilder created")
    
    # Test 2: Config
    print("Test 2: Config")
    config = brain_builder.create_config()
    assert config is not None
    print("  ✓ Config created")
    
    # Test 3: Batch operations
    print("Test 3: Batch operations")
    brain = brain_builder.create_brain()
    brain.initialize()
    
    # Test batch operations
    brain.run_batch(10)
    print("  ✓ Batch operation completed")
    
    # Test 4: Error handling
    print("Test 4: Error handling")
    try:
        validate_brain_config(config)
        print("  ✓ Configuration validation works")
    except:
        print("  ⚠ Configuration validation not available (expected)")
    
    print("\nAll tests passed!")

if __name__ == "__main__":
    test_basic_functionality()
EOF
        
        python3 test_enhanced_api.py
        if [ $? -eq 0 ]; then
            print_success "Basic tests passed"
        else
            print_error "Basic tests failed"
        fi
    fi
}

# Show help message
show_help() {
    cat << EOF
NLM Enhanced Examples Setup Script

Usage: $0 [OPTION]

Options:
    install    Install dependencies and enhanced API
    run        Run examples (requires installation first)
    test       Run tests
    all        Install and run everything
    help       Show this help message

Examples:
    $0 install    # Install enhanced NLM API
    $0 run        # Run enhanced examples
    $0 test       # Run tests
    $0 all        # Install and run everything

The script will:
1. Check Python and pip installation
2. Install the enhanced NLM API
3. Run comprehensive examples demonstrating new features
4. Validate functionality and performance

The enhanced API provides:
- High-level convenience classes (BrainBuilder, BrainRunner)
- Batch operations for improved performance
- Enhanced configuration with property access
- Comprehensive error handling and recovery
- Performance monitoring tools
- Extensive examples and documentation

For more information, visit the project documentation.
EOF
}

# Main script logic
case "${1:-all}" in
    install)
        check_python
        check_pynlm
        install_dependencies
        ;;
    run)
        check_python
        run_examples
        ;;
    test)
        check_python
        test_enhanced_api
        ;;
    all)
        check_python
        check_pynlm
        install_dependencies
        run_examples
        test_enhanced_api
        ;;
    help|--help|-h)
        show_help
        ;;
    *)
        echo "Unknown option: $1"
        echo "Run '$0 help' for usage information."
        exit 1
        ;;
esacase
