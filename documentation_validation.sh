#!/bin/bash
# documentation_validation.sh - Validate generated documentation files

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Documentation files to validate
DOC_FILES=(
    "README.md"
    "easy_usage.md"
    "HOW_TO_USE.md"
    "docs/API_REFERENCE.md"
    "docs/CONFIGURATION_GUIDE.md"
    "docs/DEVELOPER_GUIDE.md"
)

# Validation functions
validate_file_exists() {
    local file="$1"
    if [[ -f "$file" ]]; then
        echo -e "${GREEN}✓${NC} $file exists"
        return 0
    else
        echo -e "${RED}✗${NC} $file missing"
        return 1
    fi
}

validate_non_empty() {
    local file="$1"
    local lines=$(wc -l < "$file")
    if [[ $lines -gt 10 ]]; then
        echo -e "${GREEN}✓${NC} $file is substantial ($lines lines)"
        return 0
    else
        echo -e "${YELLOW}⚠${NC} $file might be too short ($lines lines)"
        return 1
    fi
}

validate_contains_content() {
    local file="$1"
    local patterns=("$2")
    local all_found=true
    
    for pattern in "${patterns[@]}"; do
        if ! grep -qi "$pattern" "$file"; then
            echo -e "${YELLOW}⚠${NC} $file doesn't contain pattern: '$pattern'"
            all_found=false
        fi
    done
    
    if [[ $all_found == true ]]; then
        echo -e "${GREEN}✓${NC} $file contains expected content"
        return 0
    else
        return 1
    fi
}

# Main validation
main() {
    echo "Validating NLM documentation files..."
    echo ""
    
    errors=0
    warnings=0
    
    for file in "${DOC_FILES[@]}"; do
        if ! validate_file_exists "$file"; then
            ((errors++))
        fi
        
        if [[ -f "$file" ]]; then
            if ! validate_non_empty "$file"; then
                ((warnings++))
            fi
            
            # File-specific content validation
            case "$file" in
                "README.md")
                    validate_contains_content "$file" "Phase 6" || ((warnings++))
                    validate_contains_content "$file" "Building" || ((warnings++))
                    ;;
                "easy_usage.md")
                    validate_contains_content "$file" "pynlm.createBrain" || ((warnings++))
                    validate_contains_content "$file" "createSimpleWorld" || ((warnings++))
                    ;;
                "HOW_TO_USE.md")
                    validate_contains_content "$file" "Create brain" || ((warnings++))
                    validate_contains_content "$file" "Configure world" || ((warnings++))
                    ;;
                "docs/API_REFERENCE.md")
                    validate_contains_content "$file" "Brain Class" || ((warnings++))
                    validate_contains_content "$file" "Config Class" || ((warnings++))
                    ;;
                "docs/CONFIGURATION_GUIDE.md")
                    validate_contains_content "$file" "brain.neuron_count" || ((warnings++))
                    validate_contains_content "$file" "plasticity.stdp" || ((warnings++))
                    ;;
                "docs/DEVELOPER_GUIDE.md")
                    validate_contains_content "$file" "C++ Developer Guide" || ((warnings++))
                    validate_contains_content "$file" "component architecture" || ((warnings++))
                    ;;
            esac
        fi
    done
    
    echo ""
    echo "Validation Summary:"
    echo "  Files checked: ${#DOC_FILES[@]}"
    echo "  Errors: $errors"
    echo "  Warnings: $warnings"
    
    if [[ $errors -gt 0 ]]; then
        echo -e "${RED}Validation failed with $errors error(s)${NC}"
        return 1
    elif [[ $warnings -gt 0 ]]; then
        echo -e "${YELLOW}Validation completed with $warnings warning(s)${NC}"
        return 0
    else
        echo -e "${GREEN}All documentation files validated successfully!${NC}"
        return 0
    fi
}

# Run validation
main "$@"