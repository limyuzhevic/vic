#!/usr/bin/bash

# Simple test script for NLM CLI
# Tests basic CLI functionality and provides a quick way to verify installation

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

# Get the directory of this script
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
NLM_CLI="$SCRIPT_DIR/nlm-cli.sh"

# Function to print status
print_status() {
    local status=$1
    local message=$2
    case $status in
        "SUCCESS")
            echo -e "${GREEN}[SUCCESS]${NC} $message"
            ;;
        "ERROR")
            echo -e "${RED}[ERROR]${NC} $message"
            ;;
        "WARNING")
            echo -e "${YELLOW}[WARNING]${NC} $message"
            ;;
        *)
            echo -e "[INFO] $message"
            ;;
    esac
}

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to test NLM CLI
Test_NLM_CLI() {
    echo "Testing NLM Command Line Interface"
    echo "=================================="
    
    # Check if CLI script exists
    if [ ! -f "$NLM_CLI" ]; then
        print_status "ERROR" "NLM CLI script not found at $NLM_CLI"
        return 1
    fi
    
    print_status "SUCCESS" "NLM CLI script found at $NLM_CLI"
    
    # Check if script is executable
    if [ ! -x "$NLM_CLI" ]; then
        print_status "WARNING" "NLM CLI script is not executable. Making it executable..."
        chmod +x "$NLM_CLI"
        if [ $? -eq 0 ]; then
            print_status "SUCCESS" "Script made executable successfully"
        else
            print_status "ERROR" "Failed to make script executable"
            return 1
        fi
    else
        print_status "SUCCESS" "NLM CLI script is executable"
    fi
    
    # Test help command
    echo "\nTesting help command..."
    if "$NLM_CLI" help >/dev/null 2>&1; then
        print_status "SUCCESS" "Help command works correctly"
    else
        print_status "ERROR" "Help command failed"
        return 1
    fi
    
    # Test help for specific command
    echo "\nTesting help for 'run' command..."
    if "$NLM_CLI" help run >/dev/null 2>&1; then
        print_status "SUCCESS" "Run command help works correctly"
    else
        print_status "ERROR" "Run command help failed"
        return 1
    fi
    
    # Test invalid command
    echo "\nTesting invalid command handling..."
    if "$NLM_CLI" invalid_command >/dev/null 2>&1; then
        print_status "ERROR" "Should have failed for invalid command"
        return 1
    else
        print_status "SUCCESS" "Invalid command correctly rejected"
    fi
    
    # Test version-like help (if available)
    echo "\nChecking for version information..."
    if "$NLM_CLI" --version >/dev/null 2>&1 2>/dev/null; then
        print_status "SUCCESS" "Version flag supported"
    else
        print_status "INFO" "Version flag not implemented (expected for this CLI)"
    fi
    
    # Show sample usage
    echo "\n" + "==========================================" + "\n"
    echo "Sample Usage:"
    echo "============="
    echo "1. Show general help:"
    echo "   $NLM_CLI help\n"
    
    echo "2. Show help for 'run' command:"
    echo "   $NLM_CLI help run\n"
    
    echo "3. Run a simple simulation (requires NLM to be built):"
    echo "   $NLM_CLI run --neurons 100 --steps 100 --quiet\n"
    
    echo "4. Create a new configuration:"
    echo "   $NLM_CLI config create --name myconfig\n"
    
    echo "5. List available experiments:"
    echo "   $NLM_CLI experiment list\n"
    
    return 0
}

# Function to check NLM installation
check_nlm_installation() {
    echo "Checking NLM installation..."
    
    # Check for build directory
    if [ ! -d "/workspace/app/build" ]; then
        print_status "WARNING" "Build directory not found. NLM may not be compiled."
        print_status "WARNING" "To compile NLM, run:"
        echo "  cd /workspace/app"
        echo "  mkdir build"
        echo "  cd build"
        echo "  cmake .. -DCMAKE_BUILD_TYPE=Release"
        echo "  make -j4"
        echo ""
        return 1
    fi
    
    # Check for NLM executable
    if [ ! -f "/workspace/app/build/nlm" ]; then
        print_status "WARNING" "NLM executable not found in build directory."
        print_status "WARNING" "Please compile NLM first."
        return 1
    fi
    
    print_status "SUCCESS" "NLM installation found in /workspace/app/build/nlm"
    
    # Try to run NLM help if available
    echo "\nTesting NLM executable (if available)..."
    if [ -x "/workspace/app/build/nlm" ]; then
        if "/workspace/app/build/nlm" --help >/dev/null 2>&1; then
            print_status "SUCCESS" "NLM executable help works"
        else
            print_status "INFO" "NLM executable help not available"
        fi
    fi
    
    return 0
}

# Main function
main() {
    echo "NLM Command Line Interface Test Script"
    echo "======================================="
    
    # Test NLM CLI
    if ! Test_NLM_CLI; then
        print_status "ERROR" "NLM CLI test failed"
        exit 1
    fi
    
    # Check NLM installation
    check_nlm_installation
    
    echo "\n" + "==========================================" + "\n"
    print_status "SUCCESS" "All tests completed successfully!"
    print_status "INFO" "The NLM CLI is ready for use."
    print_status "INFO" "Run '$NLM_CLI help' for more information."
    
    return 0
}

# Run main function
main "$@"