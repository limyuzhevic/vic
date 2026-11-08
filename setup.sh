#!/usr/bin/env bash
# Quick setup script to make all demo scripts executable

echo "=== Making Scripts Executable ==="
echo ""

# Make scripts executable
chmod +x demo.sh
chmod +x quick_start.sh
chmod +x examples.sh
chmod +x test_runner.sh

# Check if scripts exist and are not already executable
for script in demo.sh quick_start.sh examples.sh test_runner.sh; do
    if [ -f "$script" ]; then
        if [ ! -x "$script" ]; then
            echo "Made $script executable"
        else
            echo "Already executable: $script"
        fi
    else
        echo "Script $script not found"
    fi
done

echo ""
echo "=== Scripts Made Executable ==="
echo ""
echo "✓ demo.sh      - Complete demonstration and testing script"
echo "✓ quick_start.sh - Installation and first demo setup"
echo "✓ examples.sh   - Configuration and usage examples"
echo "✓ test_runner.sh - Testing and validation script"
echo ""
echo "=== Usage ==="
echo ""
echo "Quick demo:"
echo "./quick_start.sh"
echo ""
echo "Complete demonstration:"
echo "./demo.sh"
echo ""
echo "Configuration examples:"
echo "./examples.sh"
echo ""
echo "Testing:"
echo "./test_runner.sh"
echo ""
echo "=== All scripts are now ready to use! ==="
