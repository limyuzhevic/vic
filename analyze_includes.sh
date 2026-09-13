#!/bin/bash

# Analysis of include path usage in NLM project

echo "=== NLM Include Path Analysis ==="
echo "Current directory: $(pwd)"
echo ""

# Count files
CPP_FILES=$(find src -name "*.cpp" | wc -l)
HPP_FILES=$(find src -name "*.hpp" | wc -l)
TOTAL_FILES=$((CPP_FILES + HPP_FILES))

echo "=== 1. FILE COUNT ==="
echo "Total .cpp files in src/: $CPP_FILES"
echo "Total .hpp files in src/: $HPP_FILES"
echo "Total source files: $TOTAL_FILES"
echo ""

# Find all .cpp and .hpp files
echo "=== 2. ALL SOURCE FILES ==="
find src -name "*.cpp" -o -name "*.hpp" | sort

# Extract and analyze include patterns
TEMP_FILE="temp_includes.txt"

# Get all include statements
echo "=== 3. INCLUDE PATTERN ANALYSIS ==="
grep -rn "#include" src/ --include="*.cpp" --include="*.hpp" | \
    grep -o "\"[^"]*\"" | \
    sort | uniq > $TEMP_FILE

echo "All unique include patterns found:"
cat $TEMP_FILE | head -50

# Categorize include patterns
echo ""
echo "=== 4. INCLUDE PATTERN CATEGORIZATION ==="
echo "Standard includes (relative to same directory):"
grep -rn "#include" src/ --include="*.cpp" --include="*.hpp" | \
    grep -E "^src/[^/]+" | grep -o "\"[^"]*\"" | sort | uniq | wc -l
echo "Files with '../' relative includes:"
AFFECTED_FILES=$(grep -rl "#include.*\.\.\/[^\"]*\.hpp\"" src/ --include="*.cpp" --include="*.hpp")
echo "$AFFECTED_FILES" | wc -l
echo ""

echo "=== 5. FILES REQUIRING STANDARDIZE ==="
if [ -n "$AFFECTED_FILES" ]; then
    echo "Files using '../' relative includes:"
    echo "$AFFECTED_FILES"
    echo ""
    echo "Examples of affected includes:"
    grep -rn "#include.*\.\.\/[^\"]*\.hpp\"" src/ --include="*.cpp" --include="*.hpp" | head -20
else
    echo "No files found with '../' relative includes"
fi

# Count total includes
TOTAL_INCLUDES=$(grep -rn "#include" src/ --include="*.cpp" --include="*.hpp" | wc -l)
UNIQUE_INCLUDES=$(cat $TEMP_FILE | wc -l)

echo ""
echo "=== 6. SUMMARY STATISTICS ==="
echo "Total include statements: $TOTAL_INCLUDES"
echo "Unique include patterns: $UNIQUE_INCLUDES"
echo "Average includes per file: $(echo "scale=2; $TOTAL_INCLUDES / $TOTAL_FILES" | bc -l)"

# Check for different types of includes
echo ""
echo "=== 7. INCLUDE TYPE BREAKDOWN ==="
echo "Includes with '../' pattern:"
grep -rn "#include.*\.\.\/[^\"]*\.hpp\"" src/ --include="*.cpp" --include="*.hpp" | wc -l
echo "Includes with '<' (angle brackets):"
grep -rn "#include <[^>]*>" src/ --include="*.cpp" --include="*.hpp" | wc -l
echo "Includes with '""'" (quote, current dir):"
grep -rn "#include \"[^\"]*\"" src/ --include="*.cpp" --include="*.hpp" | \
    grep -v "#include.*\.\.\/[^\"]*\"" | wc -l

# Clean up
rm -f $TEMP_FILE