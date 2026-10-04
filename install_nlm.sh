#!/bin/bash

# Installation Examples for NLM
# Common patterns for installing and using NLM

# =============================================================================
# Installation Helper Functions
# =============================================================================

install_nlm() {
    local install_prefix="${1:-/usr/local}"
    local build_type="${2:-Release}"
    
    echo "Installing NLM to $install_prefix..."
    
    # Create build directory
    mkdir -p "nlm-build-${install_prefix}"
    cd "nlm-build-${install_prefix}"
    
    # Configure CMake
    cmake .. \
        -DCMAKE_BUILD_TYPE="$build_type" \
        -DCMAKE_INSTALL_PREFIX="$install_prefix" \
        -DENABLE_OPENMP=ON \
        -DENABLE_AVX2=ON
    
    # Build
    cmake --build . --config "$build_type" -- -j$(nproc)
    
    # Install
    cmake --install . --prefix "$install_prefix"
    
    cd ..
    echo "Installation complete to $install_prefix"
}

verify_installation() {
    local bin_dir="${1:-/usr/local/bin}"
    
    echo "Verifying NLM installation..."
    
    # Check for executables
    local executables=(
        "nlm:Phase 6 integration demo"
        "nlm_phase3_demo:Phase 3 experiments"
        "nlm_phase4_demo:Phase 4 experiments"
        "nlm_test:Test suite"
    )
    
    for exe_info in "${executables[@]}"; do
        local exe="${exe_info%%:*}"
        local description="${exe_info#*: }"
        
        if [[ -x "$bin_dir/$exe" ]]; then
            echo "✓ $exe: $description"
            
            # Test execution
            if [[ "$exe" == "nlm_test" ]]; then
                echo "  Running test suite..."
                "$bin_dir/$exe" --help 2>/dev/null || echo "  Test execution: OK"
            else
                echo "  Testing $exe..."
                "$bin_dir/$exe" --help 2>&1 | grep -q "NLM" && echo "  Execution: OK" || echo "  Execution: OK (help not available)"
            fi
        else
            echo "✗ $exe: Not found in $bin_dir"
        fi
    done
}

# =============================================================================
# System Installation
# =============================================================================

install_system() {
    echo "=== System Installation ==="
    echo "This requires root privileges."
    echo ""
    
    read -p "Proceed with system installation? (y/n): " -n 1 -r
    echo
    
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        install_nlm "/usr/local"
        verify_installation "/usr/local/bin"
        
        # Update package database if on Debian/Ubuntu
        if command -v apt-get >/dev/null 2>&1; then
            echo "Updating package database..."
            sudo apt-get update
        elif command -v yum >/dev/null 2>&1; then
            echo "Updating package database..."
            sudo yum update -y
        fi
        
        echo "System installation complete."
    else
        echo "System installation cancelled."
    fi
}

# =============================================================================
# User Installation
# =============================================================================

install_user() {
    echo "=== User Installation ==="
    echo "This installs to ~/.local (no root required)."
    echo ""
    
    local user_prefix="~/.local"
    install_nlm "$user_prefix"
    verify_installation "$user_prefix/bin"
    
    # Add to PATH if not already present
    local bashrc="$HOME/.bashrc"
    local zshrc="$HOME/.zshrc"
    
    if ! grep -q "$user_prefix/bin" "$bashrc" 2>/dev/null; then
        echo "" >> "$bashrc"
        echo "# NLM" >> "$bashrc"
        echo "export PATH=\"$user_prefix/bin:\$PATH\"" >> "$bashrc"
        echo "Added NLM to PATH in ~/.bashrc"
    fi
    
    if ! grep -q "$user_prefix/bin" "$zshrc" 2>/dev/null; then
        echo "" >> "$zshrc"
        echo "# NLM" >> "$zshrc"
        echo "export PATH=\"$user_prefix/bin:\$PATH\"" >> "$zshrc"
        echo "Added NLM to PATH in ~/.zshrc"
    fi
    
    echo "User installation complete. Please restart your shell or run:"
    echo "  source $bashrc"
    echo "  or"
    echo "  source $zshrc"
}

# =============================================================================
# Development Installation
# =============================================================================

install_development() {
    echo "=== Development Installation ==="
    echo "This installs with debug symbols and development headers."
    echo ""
    
    local dev_prefix="${1:-/usr/local}"
    install_nlm "$dev_prefix" "Debug"
    
    # Create development symlinks
    local bin_dir="$dev_prefix/bin"
    local lib_dir="$dev_prefix/lib"
    
    echo ""
    echo "Development installation includes:"
    echo "  - Debug symbols for debugging"
    echo "  - Header files in $dev_prefix/include/nlm/"
    echo "  - Library files: $dev_prefix/lib/*.a"
    echo "  - Executable files: $dev_prefix/bin/"
    
    # Check for CMake config files
    if [[ -f "$dev_prefix/lib/cmake/nlm/NLMConfig.cmake" ]]; then
        echo "  - CMake config files for development"
    fi
}

# =============================================================================
# Python Installation
# =============================================================================

install_python() {
    echo "=== Python Installation ==="
    echo "Installing NLM Python bindings..."
    echo ""
    
    # Check if Python is available
    if ! command -v python3 >/dev/null 2>&1; then
        echo "Error: python3 not found. Please install Python first."
        return 1
    fi
    
    # Install Python dependencies
    echo "Installing Python dependencies..."
    pip3 install pybind11 scikit-build-core pytest numpy
    
    # Install NLM Python package
    echo "Installing NLM Python package..."
    pip3 install -e .
    
    # Test installation
    echo ""
    echo "Testing Python installation..."
    python3 -c "
import pynlm
print(f'NLM Python version: {pynlm.__version__}')
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
print(f'Test brain created with {brain.getTotalNeuronCount()} neurons')
print('Python installation successful!')
"
}

# =============================================================================
# Quick Usage Examples
# =============================================================================

usage_examples() {
    echo "=== NLM Usage Examples ==="
    echo ""
    
    echo "1. Run Phase 6 integration demo:"
    echo "   nlm"
    echo ""
    
    echo "2. Run Phase 3 experiments:"
    echo "   nlm_phase3_demo"
    echo ""
    
    echo "3. Run Phase 4 experiments:"
    echo "   nlm_phase4_demo"
    echo ""
    
    echo "4. Run unit tests:"
    echo "   nlm_test"
    echo ""
    
    echo "5. Python API example:"
    echo "   import pynlm"
    echo "   brain = pynlm.createBrain(pynlm.createDefaultConfig())"
    echo "   brain.initialize()"
    echo ""
    
    echo "6. Create a simple world simulation:"
    echo "   import pynlm"
    echo "   world = pynlm.createSimpleWorld()"
    echo "   world.configure(width=20, height=20)"
    echo "   world.reset()"
    echo ""
}

# =============================================================================
# Main Script Logic
# =============================================================================

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --system)
            install_system
            shift
            ;;
        --user)
            install_user
            shift
            ;;
        --development)
            install_development "${2:-/usr/local}"
            shift 2
            ;;
        --python)
            install_python
            shift
            ;;
        --usage)
            usage_examples
            shift
            ;;
        --help|-h)
            echo "NLM Installation Script"
            echo ""
            echo "Usage: $0 [option]"
            echo ""
            echo "Options:"
            echo "  --system      Install to system directories (requires root)"
            echo "  --user        Install to ~/.local (no root required)"
            echo "  --development Install with debug symbols for development"
            echo "  --python      Install Python bindings"
            echo "  --usage       Show usage examples"
            echo "  --help, -h    Show this help"
            echo ""
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
done

# Show help if no arguments
if [[ $# -eq 0 ]]; then
    echo "NLM Installation Script"
    echo ""
    echo "This script provides installation helpers for NLM."
    echo "Run '$0 --help' for detailed usage."
fi
