#!/usr/bin/env bash
# Build Cache Management Script
# Manages build cache for NLM to optimize build times

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build"
CACHE_DIR="${SCRIPT_DIR}/.cache"

global echo "=== NLM Build Cache Management ==="

# Check if cache directory exists
if [ ! -d "$CACHE_DIR" ]; then
    mkdir -p "$CACHE_DIR"
    echo "Created cache directory: $CACHE_DIR"
fi

# Cache management options
case "${1:-}" in
    "clean")
        clean_cache
        ;;
    "stats")
        show_cache_stats
        ;;
    "compress")
        compress_cache
        ;;
    "validate")
        validate_cache
        ;;
    *)
        echo "Usage: $0 {clean|stats|compress|validate}"
        exit 1
        ;;
esac

# Clean build cache
clean_cache() {
    echo "=== Cleaning Build Cache ==="
    
    # Remove CMake build directories
    if [ -d "$BUILD_DIR" ]; then
        echo "Removing build directory: $BUILD_DIR"
        rm -rf "$BUILD_DIR"
    fi
    
    # Remove other cache directories
    local cache_dirs=(
        "$HOME/.cache/ccache"
        "$HOME/.cache/cmake"
        "$HOME/.cache/nlm"
        "/tmp/ccache"
        "/tmp/cmake"
    )
    
    for dir in "${cache_dirs[@]}"; do
        if [ -d "$dir" ]; then
            echo "Cleaning: $dir"
            rm -rf "$dir"/*
        fi
    done
    
    echo "Build cache cleaned successfully"
}

# Show cache statistics
show_cache_stats() {
    echo "=== Build Cache Statistics ==="
    
    # Check for ccache
    if command -v ccache >/dev/null 2>&1; then
        echo "ccache statistics:"
        ccache -s
    else
        echo "ccache not installed"
    fi
    
    # Check CMake cache
    local cmake_stats_dir="$CACHE_DIR/cmake"
    if [ -d "$cmake_stats_dir" ]; then
        echo "CMake cache statistics:"
        find "$cmake_stats_dir" -type f -name "*.cmake" | wc -l
    fi
    
    # Check build directory
    if [ -d "$BUILD_DIR" ]; then
        echo "Build directory size:"
        du -sh "$BUILD_DIR"
        
        echo "Build directory contents:"
        find "$BUILD_DIR" -type f \( -name "*.o" -o -name "*.a" -o -name "*.so" \) -exec ls -lh {} \; | head -10
    else
        echo "No build directory found"
    fi
}

# Compress cache
compress_cache() {
    echo "=== Compressing Build Cache ==="
    
    # Compress ccache directory
    if [ -d "$HOME/.cache/ccache" ]; then
        echo "Compressing ccache..."
        tar -czf "$CACHE_DIR/ccache.tar.gz" -C "$HOME/.cache" ccache
        rm -rf "$HOME/.cache/ccache"
    fi
    
    # Compress CMake cache
    if [ -d "$HOME/.cache/cmake" ]; then
        echo "Compressing CMake cache..."
        tar -czf "$CACHE_DIR/cmake.tar.gz" -C "$HOME/.cache" cmake
        rm -rf "$HOME/.cache/cmake"
    fi
    
    echo "Cache compressed successfully"
}

# Validate cache integrity
validate_cache() {
    echo "=== Validating Cache Integrity ==="
    
    # Check if build files exist
    if [ -d "$BUILD_DIR" ]; then
        echo "Checking build files..."
        local required_files=(
            "$BUILD_DIR/CMakeCache.txt"
            "$BUILD_DIR/cmake_install.cmake"
        )
        
        for file in "${required_files[@]}"; do
            if [ -f "$file" ]; then
                echo "✓ $file exists"
            else
                echo "✗ $file missing"
            fi
        done
    fi
    
    # Check for corrupted cache files
    if command -v ccache >/dev/null 2>&1; then
        echo "Validating ccache..."
        ccache --show-stats | grep -q "hits" && echo "✓ ccache is valid"
    fi
    
    echo "Cache validation complete"
}

# Show help
show_help() {
    cat << 'EOF'
NLM Build Cache Management

Usage: $0 [command]

Commands:
  clean    Clean all build caches
  stats    Show cache statistics
  compress Compress cache to reduce disk usage
  validate Validate cache integrity
  help     Show this help message

Cache management helps:
- Reduce build times by reusing compiled code
- Save disk space by compressing cache
- Ensure build reproducibility
- Monitor cache performance

Cache locations:
- ~/.cache/ccache - Compiler cache
- ~/.cache/cmake - CMake build cache
- ./build - Build directory
- ./.cache - NLM-specific cache
EOF
}

# Run main function
if [ "$1" = "help" ] || [ "$1" = "--help" ]; then
    show_help
elif [ $# -eq 0 ]; then
    show_help
    exit 1
fi
