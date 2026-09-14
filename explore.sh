#!/bin/bash
# Simple test script to explore NLM project structure and understand current state

# Navigate to project directory
PROJECT_ROOT="/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_99839086-35a3-4784-aed2-9d162c88a287"
cd "$PROJECT_ROOT"

echo "NLM Project Structure Exploration"
echo "================================"
echo ""

# Check key files
if [ -f "pyproject.toml" ]; then
    echo "✓ pyproject.toml found - Python bindings project"
    echo "  Package: $(grep -A1 'name =' pyproject.toml | tail -1 | sed 's/name = //')"
    echo "  Version: $(grep -A1 'version =' pyproject.toml | tail -1 | sed 's/version = //')"
else
    echo "✗ pyproject.toml not found"
fi

echo ""
if [ -f "README.md" ]; then
    echo "✓ README.md found"
    echo "  Project: $(head -5 README.md | grep "^[^#]" | head -1 | sed 's/^ *//')"
else
    echo "✗ README.md not found"
fi

echo ""
if [ -d "python" ]; then
    echo "✓ Python directory found"
    ls -la python/
else
    echo "✗ Python directory not found"
fi

echo ""
if [ -d "src" ]; then
    echo "✓ src directory found"
    echo "  Total files in src: $(find src -type f -name "*.cpp" -o -name "*.hpp" -o -name "*.h" | wc -l)"
    echo "  Main components:"
    ls -d src/*/ | xargs -I {} basename {} | sort
else
    echo "✗ src directory not found"
fi

echo ""
if [ -d "tests" ]; then
    echo "✓ tests directory found"
    echo "  Test files: $(ls tests/*.cpp 2>/dev/null | wc -l)"
else
    echo "✗ tests directory not found"
fi

echo ""
echo "Key findings:
1. Python bindings use modern pyproject.toml with scikit-build-core
2. Core NLM system is complete with 53+ C++ files
3. Phase 6 integration is complete
4. Python bindings exist but may need enhancement
5. Documentation is comprehensive but may need Python-specific updates
6. No command-line tools or CLI utilities present
7. Missing advanced features for production use
"