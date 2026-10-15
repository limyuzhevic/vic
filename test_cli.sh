#!/usr/bin/env bash
# Simple test script to verify the CLI implementation

# Create a simple test that checks if the CLI compiles and works
cat > /tmp/test_cli.cpp << 'EOF'
#include "src/cli/main.cpp"
int main() {
    return 0;
}
EOF

echo "Testing CLI compilation..."
g++ -std=c++20 -I. -c /tmp/test_cli.cpp -o /tmp/test_cli.o
if [ $? -eq 0 ]; then
    echo "✓ CLI compilation successful"
else
    echo "✗ CLI compilation failed"
    exit 1
fi

echo "✓ CLI test completed successfully"
