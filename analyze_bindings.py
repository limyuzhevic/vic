import os
import sys

print("Python Bindings File Analysis")
print("=" * 60)

# Analyze both bindings.cpp files
def analyze_bindings_file(filepath):
    print(f"\nFile: {filepath}")
    print("-" * 60)
    
    with open(filepath, 'r') as f:
        lines = f.readlines()
    
    # File stats
    print(f"Total lines: {len(lines)}")
    
    # Check for essential classes
    essential_classes = ['Config', 'Brain', 'AgentBrain', 'SimpleWorld', 
                        'SensoryInput', 'Vision', 'Audio', 'InternalSignals',
                        'Action', 'WorldObject', 'AgentBody', 'ActionResult']
    
    found_classes = []
    for line in lines:
        if 'py::class_' in line:
            for cls in essential_classes:
                if cls in line:
                    found_classes.append(cls)
    
    print(f"Essential classes found: {found_classes}")
    
    # Check for factory functions
    factory_functions = ['createDefaultConfig', 'createBrain', 'createSimpleWorld', 
                        'createAgentBrain', 'createValidatedBrain', 'runSimulation']
    
    found_functions = []
    for func in factory_functions:
        for line in lines:
            if func in line and 'def(' in line:
                found_functions.append(func)
                break
    
    print(f"Factory functions found: {found_functions}")
    
    # Check for advanced features
    advanced_features = ['createMultiAgentSystem', 'batchUpdateAgents', 
                        'createCheckpoint', 'createConfigFromTemplate',
                        'validateConfig', 'profileSimulation']
    
    found_advanced = []
    for feature in advanced_features:
        for line in lines:
            if feature in line:
                found_advanced.append(feature)
                break
    
    print(f"Advanced features found: {found_advanced}")

# Analyze both files
src_python_path = "/workspace/app/src/python/bindings.cpp"
python_path = "/workspace/app/python/bindings.cpp"

if os.path.exists(src_python_path):
    analyze_bindings_file(src_python_path)

if os.path.exists(python_path):
    analyze_bindings_file(python_path)

print("\n" + "=" * 60)
print("RECOMMENDATIONS")
print("=" * 60)

print("\n1. If the original source tree structure should be preserved:")
print("   - Keep the bindings.cpp in src/python/")
print("   - Update CMakeLists.txt in python/ to reference ../src/python/bindings.cpp")

print("\n2. If the improved bindings should be used:")
print("   - Move the improved bindings.cpp to src/python/ or keep it in python/")
print("   - Ensure proper CMake configuration")
print("   - Remove duplicate files")

print("\n3. Integration Strategy:")
print("   - The improved bindings.py in python/ has all the new features")
print("   - The original bindings.py in src/python/ may be needed for compatibility")
print("   - Consider keeping only the improved version if it's complete")
