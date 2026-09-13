#!/bin/bash

# Simple analysis of include path usage

echo "=== NLM Include Path Analysis ==="
echo "Directory: /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_4dd67c99-0b18-44fb-9894-04e4482c8e0c/src"
echo ""

# Count all .cpp and .hpp files
cpp_count=$(find src -name "*.cpp" | wc -l)
hpp_count=$(find src -name "*.hpp" | wc -l)
total_files=$((cpp_count + hpp_count))

echo "=== FILE COUNT ==="
echo "Total .cpp files: $cpp_count"
echo "Total .hpp files: $hpp_count"
echo "Total source files: $total_files"
echo ""

# Find files with '../' relative includes
echo "=== FILES WITH '../' RELATIVE INCLUDES ==="
affected_files=$(grep -rl "#include.*\.\.\/[^\"]*\.hpp\"" src/ --include="*.cpp" --include="*.hpp" 2>/dev/null)

if [ -n "$affected_files" ]; then
    count=$(echo "$affected_files" | wc -l)
    echo "Number of affected files: $count"
    echo ""
    echo "Affected files:"
    echo "$affected_files"
    echo ""
    echo "=== EXAMPLE INCLUDES FROM AFFECTED FILES ==="
    echo "$affected_files" | head -5 | xargs grep -h "#include.*\.\.\/[^\"]*\.hpp\"" | sort | uniq -c
else
    echo "No files found with '../' relative includes"
fi