#!/usr/bin/env python3
import re
import os

# Analyze Neuron.hpp
with open('src/brain/Neuron.hpp', 'r') as f:
    neuron_hpp = f.read()

# Find all public methods in Neuron class
class_pattern = r'class Neuron.*?public:(.*?)(?=private:|})'
class_match = re.search(class_pattern, neuron_hpp, re.DOTALL)
if class_match:
    public_methods = class_match.group(1)
    
    # Find all method declarations (methods ending with ;)
    method_pattern = r'\s*(\w+(?:<.*?>)?\s+)?(\w+)\(([^)]*)\)\s*(?:const)?\s*;'
    methods = re.findall(method_pattern, public_methods)
    
    print(f'Neuron.h declares {len(methods)} public methods:')
    for i, (return_type, name, params) in enumerate(methods, 1):
        return_type_str = return_type.strip() if return_type.strip() else '(void)'
        print(f'  {i:2d}. {return_type_str} {name}({params})')
        
# Now analyze Neuron.cpp
with open('src/brain/Neuron.cpp', 'r') as f:
    neuron_cpp = f.read()

print(f"\nNeuron.cpp contains methods:")
# Find all Neuron::method definitions
method_def_pattern = r'Neuron::(\w+)\('
method_defs = re.findall(method_def_pattern, neuron_cpp)
method_defs = list(dict.fromkeys(method_defs))  # Remove duplicates

print(f'Neuron.cpp defines {len(method_defs)} methods:')
for i, method_name in enumerate(method_defs, 1):
    print(f'  {i:2d}. Neuron::{method_name}()')

# Check for missing implementations
missing = set([name for _, name, _ in methods]) - set(method_defs)
print(f"\nMissing implementations in Neuron.cpp: {len(missing)}")
for method_name in sorted(missing):
    print(f"  - {method_name}()")
