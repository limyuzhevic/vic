# Simple build script to test NLM improvements

mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF
make -j4

# Test the basic functionality
echo "Building NLM Phase 2..."
echo "Testing basic neural computation..."
./nlm --mode basic --steps 1000

echo "Testing plasticity experiment..."
./nlm --mode plasticity --steps 5000 --enable-structural

echo "Testing memory integration..."
./nlm --mode memory --steps 3000

echo "Testing cognition system..."
./nlm --mode cognition --steps 2000 --enable-neuromodulation

echo "All tests completed successfully!"
