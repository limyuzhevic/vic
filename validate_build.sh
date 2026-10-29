# NLM Build Validation Script
# 
# Simple script to validate the NLM build and check for common issues

#!/bin/bash

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$PROJECT_DIR/build"

function print_header() {
    echo "=" $(tput bold)NLM Build Validation$(tput sgr0)"="
    echo
}

function check_file_exists() {
    local file="$1"
    local description="$2"
    
    if [ -f "$file" ]; then
        echo "✓ $description found at: $file"
        return 0
    else
        echo "✗ $description NOT found at: $file"
        return 1
    fi
}

function check_directory_exists() {
    local dir="$1"
    local description="$2"
    
    if [ -d "$dir" ]; then
        echo "✓ $description found at: $dir"
        return 0
    else
        echo "✗ $description NOT found at: $dir"
        return 1
    fi
}

function check_executable() {
    local exe="$1"
    local description="$2"
    
    if [ -f "$BUILD_DIR/$exe" ]; then
        echo "✓ $description executable found: $BUILD_DIR/$exe"
        return 0
    else
        echo "✗ $description executable NOT found: $BUILD_DIR/$exe"
        return 1
    fi
}

function main() {
    print_header
    
    local errors=0
    
    # Check build directory
    if ! check_directory_exists "$BUILD_DIR" "Build directory"; then
        errors=$((errors + 1))
    fi
    
    # Check CMake configuration
    if ! check_file_exists "$BUILD_DIR/CMakeCache.txt" "CMake cache configuration"; then
        errors=$((errors + 1))
    fi
    
    # Check core executables
    executables=(
        "nlm|Main neural computation engine"
        "nlm_phase3_demo|Phase 3 demonstration"
        "nlm_phase4_demo|Phase 4 demonstration"
        "nlm_phase6_demo|Phase 6 integration demo (CRITICAL)"
    )
    
    for exe_desc in "${executables[@]}"; do
        exe=$(echo "$exe_desc" | cut -d'|' -f1)
        desc=$(echo "$exe_desc" | cut -d'|' -f2)
        if ! check_executable "$exe" "$desc"; then
            errors=$((errors + 1))
        fi
    done
    
    # Check Python bindings
    if check_file_exists "$PROJECT_DIR/python/bindings.cpp" "Python bindings source"; then
        echo "✓ Python bindings source available for compilation"
    else
        echo "✗ Python bindings source missing"
        errors=$((errors + 1))
    fi
    
    # Check source directories
    src_dirs=(
        "src/brain|Neural brain components"
        "src/memory|Memory systems"
        "src/neuromodulation|Neuromodulation systems"
        "src/prediction|Prediction systems"
    )
    
    for dir_desc in "${src_dirs[@]}"; do
        dir=$(echo "$dir_desc" | cut -d'|' -f1)
        desc=$(echo "$dir_desc" | cut -d'|' -f2)
        if ! check_directory_exists "$PROJECT_DIR/src/$dir" "$desc"; then
            errors=$((errors + 1))
        fi
    done
    
    # Check documentation
    docs_files=(
        "README.md|Main README"
        "HOW_TO_USE.md|Python usage guide"
    )
    
    for doc_desc in "${docs_files[@]}"; do
        doc=$(echo "$doc_desc" | cut -d'|' -f1)
        desc=$(echo "$doc_desc" | cut -d'|' -f2)
        if ! check_file_exists "$PROJECT_DIR/$doc" "$desc"; then
            errors=$((errors + 1))
        fi
    done
    
    print_header
    if [ $errors -eq 0 ]; then
        echo "✓ All checks passed! Your NLM build is properly configured."
        echo
        echo "You can now run the Phase 6 integration demo with:"
        echo "  $BUILD_DIR/nlm_phase6_demo"
        echo
        echo "To install Python bindings:"
        echo "  cd $PROJECT_DIR && pip install ."
    else
        echo "✗ Build validation failed with $errors error(s)."
        echo
        echo "Please fix the issues above before proceeding."
    fi
    
    return $errors
}

# Run validation
main "$@"