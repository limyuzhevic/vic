#!/usr/bin/env python3
import re
import os
import json

def extract_neuron_methods(header_path, source_path):
    # Extract methods from Neuron.hpp
    with open(header_path, 'r') as f:
        header_content = f.read()
    
    # Extract Neuron class public methods
    # Look for class Neuron { public: ... } section
    class_pattern = r'class Neuron\s*{[^}]*public:(.*?)(?=private:|\})'
    class_match = re.search(class_pattern, header_content, re.DOTALL | re.IGNORECASE)
    
    if not class_match:
        print("ERROR: Could not find Neuron class definition")
        return None, None
    
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
    
    # Extract methods from Neuron.cpp
    with open(source_path, 'r') as f:
        source_content = f.read()
    
    # Find all Neuron::method implementations
    impl_pattern = r'Neuron::(\w+)\('
    implemented_methods = set()
    
    for match in re.finditer(impl_pattern, source_content):
        implemented_methods.add(match.group(1))
    
    # Special methods that don't follow the pattern (constructors/destructors)
    special_methods = ['Neuron', '~Neuron']
    
    # Check constructors/destructors
    if 'Neuron(' in source_content:
        implemented_methods.add('Neuron')
    if '~Neuron' in source_content:
        implemented_methods.add('~Neuron')
    
    return header_methods, implemented_methods

def analyze_missing_methods():
    header_path = 'src/brain/Neuron.hpp'
    source_path = 'src/brain/Neuron.cpp'
    
    header_methods, implemented_methods = extract_neuron_methods(header_path, source_path)
    
    if header_methods is None:
        return
    
    # Check each header method
    missing = []
    implemented_but_not_in_header = []
    
    for method_name in header_methods.keys():
        if method_name not in implemented_methods:
            missing.append(method_name)
    
    for method_name in implemented_methods:
        if method_name not in header_methods:
            implemented_but_not_in_header.append(method_name)
    
    # Add special methods check
    special_neuron = 'Neuron'
    special_dtor = '~Neuron'
    
    print("=" * 60)
    print("NEURON CLASS METHOD ANALYSIS")
    print("=" * 60)
    
    print(f"\nTotal methods declared in Neuron.hpp: {len(header_methods)}")
    print(f"Total methods implemented in Neuron.cpp: {len(implemented_methods)}")
    
    print(f"\n{'METHOD NAME':<30} {'RETURN TYPE':<20} {'HAS IMPL':<10}")
    print("-" * 70)
    
    for method_name in sorted(header_methods.keys()):
        impl = '✓' if method_name in implemented_methods else '✗'
        return_type = header_methods[method_name]['return_type']
        # Truncate long return types
        if len(return_type) > 18:
            return_type = return_type[:18] + '...'
        print(f"{method_name:<30} {return_type:<20} {impl:<10}")
    
    print("\n" + "=" * 60)
    print("MISSING IMPLEMENTATIONS")
    print("=" * 60)
    
    if missing:
        for method_name in sorted(missing):
            print(f"  ✗ {method_name}() - Declared but not implemented")
    else:
        print("  ✓ All methods in header have implementations!")
    
    if implemented_but_not_in_header:
        print("\nEXTRA IMPLEMENTATIONS (not in header):")
        for method_name in sorted(implemented_but_not_in_header):
            print(f"  + {method_name}() - Implemented but not declared")
    
    # Check for specific important methods
    important_methods = ['getId', 'getType', 'getMembranePotential', 
                        'getThreshold', 'isFiring', 'isRefractory',
                        'getFiringRate', 'getTotalCurrent', 'getSpikeHistory',
                        'getPlasticityFlags']
    
    print("\n" + "=" * 60)
    print("CRITICAL METHOD CHECK")
    print("=" * 60)
    
    critical_missing = []
    for method in important_methods:
        if method not in implemented_methods:
            critical_missing.append(method)
            print(f"  ✗ CRITICAL: {method}() - Missing!")
        else:
            print(f"  ✓ {method}() - Present")
    
    if critical_missing:
        print(f"\nTOTAL MISSING CRITICAL METHODS: {len(critical_missing)}")
    
    # Generate summary for todo
    print("\n" + "=" * 60)
    print("SUMMARY")
    print("=" * 60)
    print(f"Missing implementations: {len(missing)}")
    print(f"Critical missing: {len(critical_missing)}")
    
    return missing, critical_missing

if __name__ == "__main__":
    missing, critical = analyze_missing_methods()
EOF