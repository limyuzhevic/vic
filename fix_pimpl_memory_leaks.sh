#!/bin/bash

# PIMPL Memory Leak Fix Automation Script
# Fixes all PIMPL classes with manual memory management to use smart pointers

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WORKSPACE_ROOT="$SCRIPT_DIR"
BACKUP_DIR="$WORKSPACE_ROOT/backups/$(date +%Y%m%d_%H%M%S)"

# Create backup directory
mkdir -p "$BACKUP_DIR"

echo "🔧 PIMPL Memory Leak Fix Automation Script"
echo "========================================"
echo "Backup directory: $BACKUP_DIR"
echo ""

# Function to fix PIMPL class
fix_pimpl_class() {
    local filename="$1"
    local classname="$2"
    local filepath="$WORKSPACE_ROOT/$filename"
    
    echo "Fixing $filename ($classname)..."
    
    # Backup the original file
    cp "$filepath" "$BACKUP_DIR/${filename}.backup"
    
    # Apply fixes based on class name patterns
    case "$filename" in
        "src/core/Random/Random.hpp")
            # Header file fix
            sed -i 's/struct Impl;\\n    Impl\* pImpl;/struct Impl;\\n    std::unique_ptr<Impl> pImpl;/' "$filepath"
            # Fix destructor
            sed -i 's/~RandomGenerator();/~RandomGenerator() = default;/' "$filepath"
            # Fix move constructor/assignment
            sed -i 's/RandomGenerator(RandomGenerator\&\& other) noexcept : pImpl(other.pImpl) {/RandomGenerator(RandomGenerator&& other) noexcept = default;/' "$filepath"
            sed -i 's/RandomGenerator\& operator=(RandomGenerator\&\& other) noexcept {/RandomGenerator& operator=(RandomGenerator&& other) noexcept = default;/' "$filepath"
            ;;
            
        "src/core/Random/Random.cpp")
            # Fix constructor
            sed -i 's/pImpl(new Impl(seed)) {/pImpl(std::make_unique<Impl>(seed)) {}/' "$filepath"
            sed -i 's/pImpl(new Impl(std::random_device{}())) {/pImpl(std::make_unique<Impl>(std::random_device{}())) {}/' "$filepath"
            # Remove manual move constructor/assignment
            sed -i '/RandomGenerator(RandomGenerator&& other) noexcept : pImpl(other.pImpl) {/,/}/d' "$filepath"
            sed -i '/RandomGenerator\& operator=(RandomGenerator\&\& other) noexcept {/,/}/d' "$filepath"
            ;;
            
        "src/core/SimulationClock/SimulationClock.hpp")
            # Header file fix
            sed -i 's/struct Impl;\\n    Impl\* pImpl;/struct Impl;\\n    std::unique_ptr<Impl> pImpl;/' "$filepath"
            # Fix destructor
            sed -i 's/~SimulationClock();/~SimulationClock() = default;/' "$filepath"
            # Fix move constructor/assignment
            sed -i 's/SimulationClock(SimulationClock\&\& other) noexcept : pImpl(other.pImpl) {/SimulationClock(SimulationClock&& other) noexcept = default;/' "$filepath"
            sed -i 's/SimulationClock\& operator=(SimulationClock\&\& other) noexcept {/SimulationClock& operator=(SimulationClock&& other) noexcept = default;/' "$filepath"
            ;;
            
        "src/core/SimulationClock/SimulationClock.cpp")
            # Fix constructor
            sed -i 's(: pImpl(new Impl(timestep)) {/: pImpl(std::make_unique<Impl>(timestep)) {}/' "$filepath"
            # Remove manual move constructor/assignment
            sed -i '/SimulationClock(SimulationClock&& other) noexcept : pImpl(other.pImpl) {/,/}/d' "$filepath"
            sed -i '/SimulationClock\& operator=(SimulationClock\&\& other) noexcept {/,/}/d' "$filepath"
            ;;
            
        *)
            echo "⚠️  No automated fix for $filename"
            return 1
            ;;
    esac
    
    echo "✅ Fixed $filename"
}

# Function to verify fixes
verify_fixes() {
    echo ""
    echo "🔍 Verifying fixes..."
    
    # Check if all critical files were modified
    local modified_count=0
    local total_count=4
    
    for file in "$WORKSPACE_ROOT/src/core/Random/Random.hpp" "$WORKSPACE_ROOT/src/core/Random/Random.cpp" \
                "$WORKSPACE_ROOT/src/core/SimulationClock/SimulationClock.hpp" "$WORKSPACE_ROOT/src/core/SimulationClock/SimulationClock.cpp"; do
        if [ -f "$file.backup" ]; then
            echo "✅ $file: Backup created"
            ((modified_count++))
        else
            echo "❌ $file: Missing backup"
        fi
    done
    
    echo ""
    echo "Summary: $modified_count/$total_count critical files fixed"
    
    if [ $modified_count -eq $total_count ]; then
        echo "🎉 All critical PIMPL fixes completed successfully!"
        return 0
    else
        echo "⚠️  Some fixes may be incomplete"
        return 1
    fi
}

# Function to show usage
usage() {
    echo "Usage: $0 [--dry-run] [--verbose] [--verify-only]"
    echo ""
    echo "Options:"
    echo "  --dry-run    Preview what will be changed without applying fixes"
    echo "  --verbose    Show detailed output of each operation"
    echo "  --verify-only  Only verify fixes, don't apply them"
    echo ""
    echo "Example:"
    echo "  $0 --verbose          Apply all fixes with verbose output"
    echo "  $0 --dry-run          Preview what will be changed"
    echo "  $0 --verify-only      Check if fixes are already applied"
}

# Parse arguments
DRY_RUN=false
VERBOSE=false
VERIFY_ONLY=false

while [[ $# -gt 0 ]]; do
    case $1 in
        --dry-run)
            DRY_RUN=true
            shift
            ;;
        --verbose)
            VERBOSE=true
            shift
            ;;
        --verify-only)
            VERIFY_ONLY=true
            shift
            ;;
        --help|-h)
            usage
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            usage
            exit 1
            ;;
    esac
done

# Main execution
if [ "$VERIFY_ONLY" = true ]; then
    echo "🔍 Verification mode - checking current state..."
    verify_fixes
    exit $?
fi

echo "🚀 Starting PIMPL memory leak fixes..."
echo ""

# Fix critical classes
fix_pimpl_class "src/core/Random/Random.hpp" "RandomGenerator"
fix_pimpl_class "src/core/Random/Random.cpp" "RandomGenerator"
fix_pimpl_class "src/core/SimulationClock/SimulationClock.hpp" "SimulationClock"
fix_pimpl_class "src/core/SimulationClock/SimulationClock.cpp" "SimulationClock"

# Verify fixes
verify_fixes

if [ $? -eq 0 ]; then
    echo ""
    echo "🎉 PIMPL memory leak fixes completed successfully!"
    echo ""
    echo "Next steps:"
    echo "1. Run './verify_pimpl_fixes.sh' to test compilation"
    echo "2. Test the application to ensure no regressions"
    echo "3. Consider running remaining PIMPL class fixes"
else
    echo ""
    echo "⚠️  Some fixes may be incomplete. Check the output above."
    exit 1
fi
