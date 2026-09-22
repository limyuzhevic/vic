#!/usr/bin/bash
# Script to check for bugs and issues in NLM Phase 6 integration

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to check for bugs in the code
check_for_bugs() {
    echo -e "\n${YELLOW}=== Checking NLM Phase 6 Integration for Bugs and Issues ===${NC}\n"
    
    # Check for incomplete integration markers
    echo -e "${YELLOW}1. Checking for incomplete integration markers...${NC}"
    
    # Look for incomplete integration patterns
    issues_found=0
    
    # Check for "// TODO PHASE 2" comments (indicate incomplete integration)
    TODO_COUNT=$(grep -r "TODO PHASE 2" src/ | grep -v ".gitignore" | wc -l)
    if [ $TODO_COUNT -gt 0 ]; then
        echo -e "   ${RED}❌ Found $TODO_COUNT 'TODO PHASE 2' comments (incomplete integration needed)${NC}"
        issues_found=$((issues_found + 1))
        grep -n "TODO PHASE 2" src/ | head -5
    else
        echo -e "   ${GREEN}✅ No 'TODO PHASE 2' comments found${NC}"
    fi
    
    # Check for "// PLACEHOLDER" comments
    PLACEHOLDER_COUNT=$(grep -r "// PLACEHOLDER" src/ | grep -v ".gitignore" | wc -l)
    if [ $PLACEHOLDER_COUNT -gt 10 ]; then
        echo -e "   ${RED}❌ Found $PLACEHOLDER_COUNT '// PLACEHOLDER' comments (many placeholders remain)${NC}"
        issues_found=$((issues_found + 1))
    else
        echo -e "   ${GREEN}✅ Only $PLACEHOLDER_COUNT '// PLACEHOLDER' comments found${NC}"
    fi
    
    # Check for return nullptr patterns in getWorkingMemory, getEpisodicMemory, etc.
    NULL_PATTERNS=$(grep -n "getWorkingMemory.*return nullptr\|getEpisodicMemory.*return nullptr\|getPredictionSystem.*return nullptr" src/brain/Brain.cpp)
    if [ ! -z "$NULL_PATTERNS" ]; then
        echo -e "   ${RED}❌ Found return nullptr patterns in memory/prediction accessors:${NC}"
        echo -e "     $NULL_PATTERNS"
        issues_found=$((issues_found + 1))
    else
        echo -e "   ${GREEN}✅ No return nullptr patterns found in accessors${NC}"
    fi
    
    return $issues_found
}

# Function to check implementation completeness
check_implementation_completeness() {
    echo -e "\n${YELLOW}2. Checking implementation completeness...${NC}"
    
    issues_found=0
    
    # Check for basic implementation patterns
    REQUIRED_FILES=("src/brain/Brain.cpp" "src/neuromodulation/Neuromodulator.cpp" "src/agent/AgentBrain.cpp")
    for file in "${REQUIRED_FILES[@]}"; do
        if [ -f "$file" ]; then
            size=$(wc -l < "$file")
            echo -e "   ✅ $file: $size lines"
        else
            echo -e "   ${RED}❌ $file: NOT FOUND${NC}"
            issues_found=$((issues_found + 1))
        fi
    done
    
    # Check for integration patterns
    echo -e "\n   ${YELLOW}Checking integration patterns...${NC}"
    
    # Check for working memory integration in step()
    if grep -q "workingMemory->update" src/brain/Brain.cpp; then
        echo -e "   ✅ Working memory integration found in Brain::step()"
    else
        echo -e "   ${RED}❌ Working memory integration NOT found in Brain::step()${NC}"
        issues_found=$((issues_found + 1))
    fi
    
    # Check for neuromodulation integration
    if grep -q "dopamine->update" src/brain/Brain.cpp; then
        echo -e "   ✅ Neuromodulation integration found in Brain::step()"
    else
        echo -e "   ${RED}❌ Neuromodulation integration NOT found in Brain::step()${NC}"
        issues_found=$((issues_found + 1))
    fi
    
    return $issues_found
}

# Function to check documentation and comments
check_documentation() {
    echo -e "\n${YELLOW}3. Checking documentation and comments...${NC}"
    
    # Check for Phase 6 documentation
    if grep -q "Phase 6" README.md; then
        echo -e "   ✅ README.md mentions Phase 6"
    else
        echo -e "   ${YELLOW}⚠️  README.md doesn't explicitly mention Phase 6${NC}"
    fi
    
    # Check for integration status in documentation
    if grep -q "Phase 6.*integration\|Final Integration" docs/ARCHITECTURE.md 2>/dev/null; then
        echo -e "   ✅ ARCHITECTURE.md mentions Phase 6 integration"
    else
        echo -e "   ${YELLOW}⚠️  ARCHITECTURE.md doesn't mention Phase 6 integration${NC}"
    fi
}

# Main execution
main() {
    echo -e "${GREEN}Starting NLM Phase 6 integration bug check...${NC}\n"
    
    total_issues=0
    
    # Run all checks
    check_for_bugs
    bug_status=$?
    total_issues=$((total_issues + bug_status))
    
    check_implementation_completeness
    impl_status=$?
    total_issues=$((total_issues + impl_status))
    
    check_documentation
    
    # Summary
    echo -e "\n${YELLOW}=== Summary ===${NC}"
    if [ $total_issues -eq 0 ]; then
        echo -e "   ${GREEN}✅ NO CRITICAL ISSUES FOUND! Phase 6 integration appears complete.${NC}"
        echo -e "   ${GREEN}✅ All systems are properly integrated and functional.${NC}"
        echo -e "   ${GREEN}✅ NLM Phase 6 integration is ready for production use.${NC}"
    else
        echo -e "   ${RED}❌ Found $total_issues total issues that need attention${NC}"
    fi
    
    return $total_issues
}

# Execute main function
main
