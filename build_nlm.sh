chmod +x "$0"

PROJECT_ROOT="/workspace/app"
BUILD_DIR="${PROJECT_ROOT}/build"

function print_step() {
    echo "================================================="
    echo "STEP: $1"
    echo "================================================="
}

function check_file_exists() {
    local file=$1
    local description=$2
    
    if [ ! -f "$file" ]; then
        echo "❌ ERROR: $description not found at $file"
        return 1
    fi
    echo "✅ Found: $description"
    return 0
}

function build_project() {
    print_step "Setting up build directory"
    
    if [ -d "$BUILD_DIR" ]; then
        echo "Removing existing build directory..."
        rm -rf "$BUILD_DIR"
    fi
    
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    print_step "Running CMake configuration"
    cmake .. -DCMAKE_BUILD_TYPE=Release
    
    print_step "Building the project"
    make -j4
}

function verify_build() {
    print_step "Verifying build artifacts"
    
    # Check for expected executables
    executables=(
        "nlm: Main NLM executable"
        "nlm_phase3_demo: Phase 3 demo executable"
        "nlm_phase4_demo: Phase 4 demo executable"
        "nlm_phase6_demo: Phase 6 demo executable (NEW)"
    )
    
    for exe_pair in "${executables[@]}"; do
        exe_name=$(echo "$exe_pair" | cut -d: -f1)
        exe_desc=$(echo "$exe_pair" | cut -d: -f2)
        
        if [ -f "$exe_name" ]; then
            echo "✅ Found executable: $exe_name ($exe_desc)"
        else
            echo "❌ Missing executable: $exe_name ($exe_desc)"
            return 1
        fi
    done
    
    # Check for expected libraries
    libraries=(
        "libnlm_core.so: Core neural library"
        "libnlm_agent.so: Agent interface library"
        "libnlm_world.so: World system library"
        "libnlm_phase3.so: Phase 3 experiment library"
        "libnlm_phase4.so: Phase 4 experiment library"
        "libnlm_phase6.so: Phase 6 experiment library (NEW)"
    )
    
    for lib_pair in "${libraries[@]}"; do
        lib_name=$(echo "$lib_pair" | cut -d: -f1)
        lib_desc=$(echo "$lib_pair" | cut -d: -f2)
        
        if [ -f "$lib_name" ]; then
            echo "✅ Found library: $lib_name ($lib_desc)"
        else
            echo "❌ Missing library: $lib_name ($lib_desc)"
            return 1
        fi
    done
    
    return 0
}

function verify_python_bindings() {
    print_step "Verifying Python bindings"
    
    # Check if Python module can be built
    python_dir="${PROJECT_ROOT}/python"
    
    if [ ! -d "$python_dir" ]; then
        echo "⚠️  Warning: Python directory not found at $python_dir"
        echo "Python bindings may need to be built separately"
        return 0
    fi
    
    # Check for setup.py or pyproject.toml
    if [ -f "${python_dir}/setup.py" ]; then
        echo "✅ Found setup.py for Python bindings"
    elif [ -f "${python_dir}/pyproject.toml" ]; then
        echo "✅ Found pyproject.toml for Python bindings"
    else
        echo "⚠️  Warning: No Python build configuration found"
    fi
    
    return 0
}

function show_usage() {
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  --help          Show this help message"
    echo "  --quick         Skip full build, just verify existing files"
    echo "  --build-only    Only build, don't run tests"
    echo ""
    echo "Examples:"
    echo "  $0                 # Full build and verification"
    echo "  $0 --quick         # Just verify files exist"
    echo "  $0 --build-only    # Build without tests"
}

function main() {
    local quick_build=false
    local build_only=false
    
    # Parse arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            --help)
                show_usage
                exit 0
                ;;
            --quick)
                quick_build=true
                shift
                ;;
            --build-only)
                build_only=true
                shift
                ;;
            *)
                echo "Unknown option: $1"
                show_usage
                exit 1
                ;;
        esac
    done
    
    # Go to project root
    cd "$PROJECT_ROOT"
    
    print_step "NLM Project Build Script"
    echo "Building and verifying NLM improvements..."
    
    # Check for essential files
    check_file_exists "CMakeLists.txt" "CMake configuration file" || exit 1
    check_file_exists "src/main.cpp" "Main C++ source file" || exit 1
    check_file_exists "README.md" "Project README" || exit 1
    check_file_exists "IMPROVEMENTS_SUMMARY.md" "Improvements summary" || exit 1
    check_file_exists "verify_improvements.py" "Verification script" || exit 1
    
    if [ "$quick_build" = false ]; then
        build_project
    fi
    
    # Verify build artifacts
    if ! verify_build; then
        echo "❌ Build verification failed"
        exit 1
    fi
    
    # Verify Python bindings
    if ! verify_python_bindings; then
        echo "❌ Python bindings verification failed"
        exit 1
    fi
    
    if [ "$build_only" = false ]; then
        print_step "Running verification script"
        
        # Check if Python is available
        if command -v python3 &> /dev/null; then
            echo "✅ Python3 found, running verification script..."
            python3 "verify_improvements.py"
            
            if [ $? -eq 0 ]; then
                print_step "All verifications completed successfully!"
                echo ""
                echo "🎉 NLM improvements are ready to use!"
                echo ""
                echo "Summary of improvements:"
                echo "• Fixed main entry point inconsistency (Phase 6 support)"
                echo "• Added nlm_phase6_demo executable"
                echo "• Enhanced Python bindings with Phase 6 experiment capabilities"
                echo "• Added Phase6Config and Phase6IntegrationResult structures"
                echo "• Added convenience functions for easy Python usage"
                echo "• Improved code organization and documentation"
                echo ""
                echo "To run Phase 6 integration demo:"
                echo "  ./nlm_phase6_demo"
                echo ""
                echo "To test Python bindings:"
                echo "  ./verify_improvements.py"
            else
                echo "❌ Verification script failed"
                exit 1
            fi
        else
            echo "⚠️  Python3 not found, skipping Python verification"
            echo "Python bindings may need to be built separately"
        fi
    else
        print_step "Build completed successfully"
        echo "To run verifications later, use:"
        echo "  python3 verify_improvements.py"
    fi
}

# Run main function with all arguments
main "$@"
