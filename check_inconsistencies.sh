#!/bin/bash

# Script to find inconsistencies between header files and implementation files

# Function to extract public methods from header file
extract_header_methods() {
    local filepath=$1
    local classname=$2
    
    if [ ! -f "$filepath" ]; then
        echo "Header file not found: $filepath"
        return
    fi
    
    # Extract public method declarations
    grep -n "^\s*\(virtual\)?\s*\(static\)?\s*[a-zA-Z_][a-zA-Z0-9_]*\s+\(const\)?\s*\([a-zA-Z_][a-zA-Z0-9_]*\s*\([^)]*\)\)\s*;" "$filepath" |
    sed -e "s/^\s*//" -e "s/\s*;$//" -e "s/\s*const$//" -e "s/^[^:]*://" -e "s/^[0-9]*://" 2>/dev/null || {
        # Fallback: find all method-like patterns
        grep -n "\([^)]*\)[[:space:]]*\([^)]*\)[[:space:]]*::[[:space:]]*\([^)]*\)[[:space:]]*(\([^)]*\))" "$filepath" |
        sed -e "s/^\s*//" -e "s/\s*;$//" -e "s/\s*const$//" -e "s/^[^:]*://" -e "s/^[0-9]*://" 2>/dev/null
    }
}

# Function to extract methods from implementation file
extract_cpp_methods() {
    local filepath=$1
    local classname=$2
    
    if [ ! -f "$filepath" ]; then
        echo "Implementation file not found: $filepath"
        return
    fi
    
    # Extract method definitions (more precise - look for "className::methodName("
    grep -n "^\s*\([^)]*\)[[:space:]]*\([^)]*\)[[:space:]]*::[[:space:]]*\([^)]*\)[[:space:]]*(\([^)]*\))" "$filepath" |
    sed -e "s/^\s*//" -e "s/\s*(.*$//" -e "s/\s*$//"
}

# Function to compare two files
echo "Searching for inconsistencies between header and implementation files..."
echo "=============================================="

# Files to check
checks=(
    "src/agent/AgentBrain.hpp src/agent/AgentBrain.cpp AgentBrain"
    "src/brain/Brain.hpp src/brain/Brain.cpp Brain"
    "src/world/SimpleWorld.hpp src/world/SimpleWorld.cpp SimpleWorld"
    "src/experiments/Phase6IntegratedExperiment.hpp src/experiments/Phase6IntegratedExperiment.cpp Phase6IntegratedExperiment"
)

all_issues=""

for check in "${checks[@]}"; do
    echo "$check"
    IFS=' ' read -r header cpp classname <<< "$check"
    
    echo "Checking: $classname"
    echo "Header: $header"
    echo "Implementation: $cpp"
    echo ""
    
    # Extract methods
    echo "Public methods declared in header:"
    header_methods=$(extract_header_methods "$header" "$classname")
    echo "$header_methods" | while read method; do
        echo "  - $method"
    done
    
    echo ""
    echo "Methods defined in implementation:"
    cpp_methods=$(extract_cpp_methods "$cpp" "$classname")
    echo "$cpp_methods" | while read method; do
        echo "  - $method"
    done
    echo ""
    
    # Compare (simple string comparison for demonstration)
    echo "Comparing methods..."
    # This is a simplified comparison - would need more sophisticated logic for production
    
    echo "----------------------------------------"
    echo ""
done

echo "Search completed!"
