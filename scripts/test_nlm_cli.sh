#!/bin/bash
# Test NLM CLI installation and basic functionality

set -e

echo "Testing NLM CLI Installation..."

# Check Python version
python3 --version
if [[ $? -ne 0 ]]; then
    echo "ERROR: Python 3 is required"
    exit 1
fi

# Test CLI help
echo "1. Checking help..."
nlm --help

# Test version
echo "2. Checking version..."
nlm --version

# Test config list
echo "3. Checking config list..."
nlm config list

echo "4. Creating test configuration..."
nlm create --neurons 100 --output test_config.cfg

echo "5. Testing simulation..."
nlm simulate --steps 10 --config test_config.cfg

echo "6. Cleaning up..."
rm -f test_config.cfg

echo "All tests completed successfully!"
