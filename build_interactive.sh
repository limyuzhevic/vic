#!/usr/bin/env bash
# Build and install dependencies for NLM Interactive Visualization

# Check if ncurses is available
if ! pkg-config --exists ncurses; then
    echo "ncurses development libraries not found. Installing..."
    if command -v apt-get &> /dev/null; then
        apt-get update && apt-get install -y libncurses5-dev
    elif command -v yum &> /dev/null; then
        yum install -y ncurses-devel
    elif command -v dnf &> /dev/null; then
        dnf install -y ncurses-devel
    elif command -v pacman &> /dev/null; then
        pacman -S ncurses
    else
        echo "Please install ncurses development libraries manually"
        exit 1
    fi
fi

# Build NLM with interactive visualization support
echo "Building NLM with interactive visualization support..."
cd /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_28b07018-566c-4c2b-b77a-5bbcd56e9304
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

echo "Build completed successfully!"
echo ""
echo "To run interactive visualization mode, use:"
echo "  ./nlm --visualization"
echo "or"
echo "  ./nlm --interactive"
echo ""
echo "For help on all available commands within visualization mode, press '?'"
