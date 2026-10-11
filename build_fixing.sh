cd /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_8911c5ba-d6b4-4c10-941f-38731cfa93a2
rm -rf build
mkdir build
cd build
# Copy python/CMakeLists.txt to root to build with correct paths
cp python/CMakeLists.txt .
# Remove the incorrect source references
# Use a simpler approach - build the Python module directly
