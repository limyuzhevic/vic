# Verify the current state of the project structure
import os

print("Project Structure Analysis")
print("=" * 50)

# Check Python directory
python_dir = "/workspace/app/python"
print(f"\nPython directory: {python_dir}")
print(f"  Contents: {os.listdir(python_dir)}")

# Check if bindings.cpp is in the right place
bindings_path = os.path.join(python_dir, "bindings.cpp")
print(f"\nPython bindings.cpp exists: {os.path.exists(bindings_path)}")

# Check src/python directory
src_python_dir = "/workspace/app/src/python"
print(f"\nSource Python directory: {src_python_dir}")
if os.path.exists(src_python_dir):
    print(f"  Contents: {os.listdir(src_python_dir)}")

# Check CMakeLists.txt in root
cmake_root = "/workspace/app/CMakeLists.txt"
print(f"\nRoot CMakeLists.txt exists: {os.path.exists(cmake_root)}")

# Check CMakeLists.txt in python directory
cmake_python = "/workspace/app/python/CMakeLists.txt"
print(f"Python CMakeLists.txt exists: {os.path.exists(cmake_python)}")

# Check current working directory
print(f"\nCurrent working directory: {os.getcwd()}")
print(f"Workspace root: {os.path.dirname(os.path.dirname(os.getcwd()))}")

print("\n" + "=" * 50)
print("Analysis complete. Check for any paths that need to be updated.")
