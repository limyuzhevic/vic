#!/usr/bin/env python3
import re

print("=" * 70)
print("NEURON CLASS METHOD IMPLEMENTATION ANALYSIS")
print("=" * 70)

# Read Neuron.hpp
with open('src/brain/Neuron.hpp', 'r') as f:
    neuron_hpp = f.read()

# Simple approach: extract all method signatures between "public:" and "private:"
# Find the public section
public_start = neuron_hpp.find("public:")
private_start = neuron_hpp.find("private:", public_start)

if public_start != -1 and private_start != -1:
    public_section = neuron_hpp[public_start:private_start]
    
    # Extract method signatures (ending with ;)
    lines = public_section.split('\n')
    header_methods = []
    
    for line in lines:
        line = line.strip()
        # Skip comments and empty lines
        if not line or line.startswith('//') or line.startswith('/*'):
            continue
        # Check if it's a method signature (contains "(" and ends with ";")
        if '(' in line and line.endswith(';'):
            # Clean up the line
            line = line.rstrip(';')
            header_methods.append(line)

# Read Neuron.cpp
with open('src/brain/Neuron.cpp', 'r') as f:
    neuron_cpp = f.read()

# Find all Neuron::method implementations
implemented_methods = set()
lines_cpp = neuron_cpp.split('\n')

for line in lines_cpp:
    line = line.strip()
    # Look for Neuron::method(
    if 'Neuron::' in line and '(' in line:
        # Extract method name
        match = re.search(r'Neuron::(\w+)\(', line)
        if match:
            implemented_methods.add(match.group(1))

# Special methods
if 'Neuron(' in neuron_cpp:
    implemented_methods.add('Neuron')
if '~Neuron' in neuron_cpp:
    implemented_methods.add('~Neuron')

print(f"\nMethods in header: {len(header_methods)}")
print(f"Methods implemented: {len(implemented_methods)}")

# Display header methods with implementation status
print("\n" + "=" * 70)
print("HEADER METHOD ANALYSIS")
print("=" * 70)
print(f"{'METHOD':<30} {'IMPLEMENTED':<12}")
print("-" * 70)

for method in sorted(header_methods, key=lambda x: x.split('(')[0].strip()):
    method_name = method.split('(')[0].strip()
    impl = '✓' if method_name in implemented_methods else '✗'
    print(f"{method_name:<30} {impl:<12}")

# List missing implementations
missing = [m.split('(')[0].strip() for m in header_methods if m.split('(')[0].strip() not in implemented_methods]

print("\n" + "=" * 70)
print("MISSING IMPLEMENTATIONS")
print("=" * 70)

if missing:
    print(f"Total missing: {len(missing)}")
    
    # Check for specific critical methods
    critical = ['getId', 'getType', 'getMembranePotential', 'getThreshold', 
               'isFiring', 'isRefractory', 'getFiringRate', 'getTotalCurrent',
               'getSpikeHistory', 'getPlasticityFlags']
    
    critical_missing = [m for m in missing if m in critical]
    other_missing = [m for m in missing if m not in critical]
    
    print(f"\nCRITICAL MISSING ({len(critical_missing)}):")
    for m in sorted(critical_missing):
        print(f"  ✗ {m}()")
    
    print(f"\nOTHER MISSING ({len(other_missing)}):")
    for m in sorted(other_missing):
        print(f"  ✗ {m}()")
else:
    print("✓ All methods implemented!")

# Check Python bindings
print("\n" + "=" * 70)
print("PYTHON BINDINGS ANALYSIS")
print("=" * 70)

with open('python/bindings.cpp', 'r') as f:
    bindings = f.read()

# Check Neuron class binding
if 'py::class_<Brain>' in bindings:
    print("✓ Brain class is bound to Python")
else:
    print("✗ Brain class binding not found")

# Count Neuron methods in bindings
neuron_methods_in_bindings = []
for line in bindings.split('\n'):
    if 'Neuron' in line and '::' in line and 'def' in line:
        # This is a method binding
        print(f"Found binding: {line.strip()[:100]}")

# Check for specific Neuron methods
print("\nChecking for Neuron method bindings...")
neuron_methods_to_check = ['getId', 'getType', 'getMembranePotential', 'getThreshold', 
                          'isFiring', 'isRefractory', 'getFiringRate', 'getTotalCurrent',
                          'getSpikeHistory', 'getState']

for method in neuron_methods_to_check:
    if method in bindings:
        print(f"  ✓ {method} - Found in bindings")
    else:
        print(f"  ✗ {method} - NOT in bindings")

# Documentation analysis
print("\n" + "=" * 70)
print("DOCUMENTATION ANALYSIS")
print("=" * 70)

# Check for documentation files
doc_files = ['README.md', 'easy_usage.md', 'HOW_TO_USE.md']

for doc_file in doc_files:
    if os.path.exists(doc_file):
        with open(doc_file, 'r') as f:
            content = f.read()
            # Count references to Neuron methods
            neuron_ref_count = content.lower().count('neuron')
            print(f"{doc_file}: {neuron_ref_count} references to 'neuron' (case-insensitive)")
    else:
        print(f"{doc_file}: NOT FOUND")

# Summary
print("\n" + "=" * 70)
print("ANALYSIS SUMMARY")
print("=" * 70)

print(f"Methods in Neuron.hpp: {len(header_methods)}")
print(f"Methods implemented in Neuron.cpp: {len(implemented_methods)}")
print(f"Missing implementations: {len(missing)}")

if critical_missing:
    print(f"Critical methods missing: {len(critical_missing)}")
    print("These methods are essential for proper Neuron functionality!")

print("\nRECOMMENDATIONS:")
if missing:
    print("1. Implement missing Neuron getter methods (getId, getType, etc.)")
    print("2. Add Neuron methods to Python bindings if not present")
    print("3. Update documentation to reflect actual Neuron API")
else:
    print("1. Review Neuron implementation - some methods may be incorrectly detected")
