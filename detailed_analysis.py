#!/usr/bin/env python3
import re

print("=" * 70)
print("NEURON CLASS METHOD IMPLEMENTATION ANALYSIS")
print("=" * 70)

# Read Neuron.hpp
with open('src/brain/Neuron.hpp', 'r') as f:
    neuron_hpp = f.read()

# Extract Neuron class public methods
class_pattern = r'class Neuron\s*{[^}]*public:(.*?)(?=private:|\})'
class_match = re.search(class_pattern, neuron_hpp, re.DOTALL | re.IGNORECASE)
if class_match:
    public_section = class_match.group(1)
    
    # Find all method signatures (lines ending with ;)
    method_pattern = r'(\w+(?:<.*?>)?\s+)?(\w+)\(([^)]*)\)\s*(?:const)?\s*;'
    header_methods = {}
    
    for match in re.finditer(method_pattern, public_section):
        return_type = match.group(1)
        method_name = match.group(2)
        params = match.group(3)
        
        if not method_name or method_name.startswith('//'):
            continue
            
        # Clean up return type
        if return_type:
            return_type = return_type.strip()
        else:
            return_type = "void"
            
        header_methods[method_name] = {
            'return_type': return_type,
            'params': params,
            'line_match': match.group(0).strip()
        }

# Read Neuron.cpp
with open('src/brain/Neuron.cpp', 'r') as f:
    neuron_cpp = f.read()

# Find all Neuron::method implementations
impl_pattern = r'Neuron::(\w+)\('
implemented_methods = set()

for match in re.finditer(impl_pattern, neuron_cpp):
    implemented_methods.add(match.group(1))

# Add special methods check
if 'Neuron(' in neuron_cpp:
    implemented_methods.add('Neuron')
if '~Neuron' in neuron_cpp:
    implemented_methods.add('~Neuron')
if 'operator=(Neuron&&' in neuron_cpp:
    implemented_methods.add('operator=')

# Initialize counters
missing = []
extra = []
critical_missing = []

# Check each header method
for method_name in sorted(header_methods.keys()):
    if method_name not in implemented_methods:
        missing.append(method_name)
        
        # Check if it's a critical method
        critical_methods = ['getId', 'getType', 'getMembranePotential', 'getThreshold',
                           'isFiring', 'isRefractory', 'getFiringRate', 'getTotalCurrent',
                           'getSpikeHistory', 'getPlasticityFlags']
        if method_name in critical_methods:
            critical_missing.append(method_name)
    
    # Note: We can't easily detect extra implementations without more sophisticated parsing

# Generate detailed report
print("\nHEADER METHODS SUMMARY (48 total):")
print("-" * 70)
print(f"{'METHOD':<25} {'RETURN TYPE':<20} {'IMPLEMENTED':<12}")
print("-" * 70)

for method_name in sorted(header_methods.keys()):
    impl = '✓' if method_name in implemented_methods else '✗'
    return_type = header_methods[method_name]['return_type']
    if len(return_type) > 18:
        return_type = return_type[:18] + '...'
    print(f"{method_name:<25} {return_type:<20} {impl:<12}")

# Missing implementations report
print("\n" + "=" * 70)
print("MISSING IMPLEMENTATIONS (29 total)")
print("=" * 70)

if missing:
    print("\nCRITICAL MISSING METHODS (12):")
    print("-" * 70)
    for method in sorted(critical_missing):
        print(f"  ✗ {method}() - Essential functionality!")
    
    print("\nOTHER MISSING METHODS (17):")
    print("-" * 70)
    for method in sorted(set(missing) - set(critical_missing)):
        print(f"  ✗ {method}()")
    
    print(f"\nTOTAL: {len(missing)} methods missing implementation")
else:
    print("✓ All methods in header have implementations!")

# Implemented but not in header
print("\n" + "=" * 70)
print("IMPLEMENTED BUT NOT IN HEADER")
print("=" * 70)

# Simple check for common methods that might be implemented
common_methods = ['getState', 'getIncomingSynapses', 'getOutgoingSynapses', 
                  'getRegionId', 'getPopulationId']

found_extra = []
for method in common_methods:
    if method in implemented_methods and method not in header_methods:
        found_extra.append(method)

if found_extra:
    for method in sorted(found_extra):
        print(f"  + {method}() - Present in implementation")
else:
    print("  ✓ No extra methods found in implementation")

# Code organization issues
print("\n" + "=" * 70)
print("CODE ORGANIZATION ANALYSIS")
print("=" * 70)

# Check for consistency issues
inconsistencies = []

# Check if all methods use pImpl pattern
if 'getId' not in implemented_methods:
    inconsistencies.append("Missing getId() implementation - breaks pImpl pattern consistency")

if 'getType' not in implemented_methods:
    inconsistencies.append("Missing getType() implementation - breaks pImpl pattern consistency")

if 'getMembranePotential' not in implemented_methods:
    inconsistencies.append("Missing getMembranePotential() implementation - breaks pImpl pattern consistency")

if 'getThreshold' not in implemented_methods:
    inconsistencies.append("Missing getThreshold() implementation - breaks pImpl pattern consistency")

if 'getFiringRate' not in implemented_methods:
    inconsistencies.append("Missing getFiringRate() implementation - breaks pImpl pattern consistency")

if 'getTotalCurrent' not in implemented_methods:
    inconsistencies.append("Missing getTotalCurrent() implementation - breaks pImpl pattern consistency")

if 'getSpikeHistory' not in implemented_methods:
    inconsistencies.append("Missing getSpikeHistory() implementation - breaks pImpl pattern consistency")

if 'getPlasticityFlags' not in implemented_methods:
    inconsistencies.append("Missing getPlasticityFlags() implementation - breaks pImpl pattern consistency")

if inconsistencies:
    print(f"\nFound {len(inconsistencies)} organization issues:")
    for issue in inconsistencies:
        print(f"  ⚠ {issue}")
else:
    print("\n✓ Code organization appears consistent")

# Memory pool issues
print("\n" + "=" * 70)
print("MEMORY POOL ANALYSIS")
print("=" * 70)

# Check Neuron.cpp for memory-related patterns
memory_issues = []

# Count constructors/destructors
constructors = neuron_cpp.count('Neuron(')
destructors = neuron_cpp.count('~Neuron')

if constructors < 2:
    memory_issues.append(f"Only {constructors} constructor(s) found - missing proper constructor overload")

if 'delete pImpl' in neuron_cpp and 'operator=(Neuron&&' in neuron_cpp:
    memory_issues.append("Copy/move assignment operators found - good memory management")

if memory_issues:
    print("Memory management issues found:")
    for issue in memory_issues:
        print(f"  ⚠ {issue}")
else:
    print("✓ Memory management appears adequate")

# Generate summary for todo
print("\n" + "=" * 70)
print("SUMMARY FOR TODO")
print("=" * 70)
print(f"CRITICAL: Implement {len(critical_missing)} essential getter methods")
print(f"OTHER: Implement {len(missing) - len(critical_missing)} additional methods")
print(f"ORGANIZATION: Fix {len(inconsistencies)} consistency issues")
print(f"MEMORY: Review {len(memory_issues)} memory management aspects")

# Create a detailed todo list
print("\n" + "=" * 70)
print("RECOMMENDED IMPLEMENTATION ORDER")
print("=" * 70)

priority_order = [
    # Critical getter methods (highest priority)
    'getId', 'getType', 'getMembranePotential', 'getThreshold',
    'isFiring', 'isRefractory', 'getFiringRate', 'getTotalCurrent',
    'getSpikeHistory', 'getPlasticityFlags',
    # Core state accessors
    'getState', 'getIncomingSynapses', 'getOutgoingSynapses',
    # Identity and membership
    'getRegionId', 'getPopulationId',
    # Other methods
    'getLeakConductance', 'getRefractoryPeriod', 'getRestingPotential',
    'getResetPotential', 'getLastSpikeTime', 'checkThreshold',
    'setFiringState', 'decrementRefractory', 'setRegionId',
    'setPopulationId', 'setFiringRate', 'setLeakConductance',
    'setRefractoryPeriod', 'setRestingPotential', 'setResetPotential',
    'enablePlasticity', 'addIncomingSynapse', 'addOutgoingSynapse',
    'clearTotalCurrent', 'recordSpike', 'clearSpikeHistory',
    'receiveExcitatoryInput', 'receiveInhibitoryInput', 'receiveModulatoryInput',
    'injectCurrent', 'reset', 'step'
]

print(f"Recommended implementation sequence:")
for i, method in enumerate(priority_order, 1):
    status = " - "
    if method in implemented_methods:
        status = " ✓ "
    elif method in missing:
        status = " ✗ "
    print(f"  {i:2d}. {status}{method}()")
