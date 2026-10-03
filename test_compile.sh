#!/usr/bin/env g++ -std=c++20 -I./src -o test_config test_config.cpp

cd /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_2113054f-c3bd-4aa1-8c4d-bdd31acd7491

cat > test_compile.sh << 'EOF'
#!/bin/bash
set -e
echo "Testing NLM configuration system compilation..."
echo "Building test..."
g++ -std=c++20 -I./src -c src/core/Config/Config.cpp -o config.o
echo "Config.o built successfully"
echo "Testing basic compilation..."
g++ -std=c++20 -I./src test_config.cpp config.o -o test_config
if [ -f test_config ]; then
    echo "Test binary created successfully"
    ./test_config
else
    echo "Test failed to compile"
    exit 1
fi
EOF

chmod +x test_compile.sh
./test_compile.sh
