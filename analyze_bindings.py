#!/usr/bin/env python3
"""
Python Bindings for NLM - Module Organization Script

This script analyzes and reorganizes the Python bindings to improve structure and maintainability.
"""

import re
from pathlib import Path

class PythonBindingsAnalyzer:
    def __init__(self, bindings_path):
        self.bindings_path = Path(bindings_path)
        self.content = self.bindings_path.read_text()
        
    def analyze_structure(self):
        """Analyze the current structure of bindings.cpp"""
        print("=== Python Bindings Structure Analysis ===\n")
        
        # Find all py::class_ and m.def calls
        class_pattern = r'py::class_<([^>]+)>\s*\(\s*m\s*,\s*"([^"]+)"'
        def_pattern = r'm\.def\("([^"]+)"'
        
        classes = re.findall(class_pattern, self.content, re.DOTALL)
        defs = re.findall(def_pattern, self.content)
        
        print(f"Found {len(classes)} class bindings:")
        for cls, doc in classes:
            print(f"  - {cls.strip()} (doc: {doc})")
            
        print(f"\nFound {len(defs)} function bindings:")
        for func in defs:
            print(f"  - {func}")
            
        # Identify logical groupings
        groupings = self.identify_logical_groups(classes)
        print(f"\nLogical groupings identified:")
        for group_name, group_items in groupings.items():
            print(f"  {group_name}:")
            for item in group_items:
                print(f"    - {item}")
    
    def identify_logical_groups(self, classes):
        """Group classes by their functionality"""
        groups = {
            'Core Types': [],
            'Neurons & Synapses': [],
            'Brain Components': [],
            'Sensory Systems': [],
            'Motor Systems': [],
            'World/Simulation': [],
            'Agent Interface': [],
            'Configuration': [],
            'Enumerations': []
        }
        
        for cls, doc in classes:
            cls_lower = cls.lower()
            if any(keyword in cls_lower for keyword in ['id', 'type']):
                groups['Core Types'].append(cls)
            elif any(keyword in cls_lower for keyword in ['neuron', 'synapse']):
                groups['Neurons & Synapses'].append(cls)
            elif 'brain' in cls_lower:
                groups['Brain Components'].append(cls)
            elif any(keyword in cls_lower for keyword in ['vision', 'audio', 'sensory']):
                groups['Sensory Systems'].append(cls)
            elif any(keyword in cls_lower for keyword in ['action', 'motor']):
                groups['Motor Systems'].append(cls)
            elif any(keyword in cls_lower for keyword in ['world', 'simple']):
                groups['World/Simulation'].append(cls)
            elif any(keyword in cls_lower for keyword in ['agent', 'percept']):
                groups['Agent Interface'].append(cls)
            elif 'config' in cls_lower:
                groups['Configuration'].append(cls)
            elif any(keyword in cls_lower for keyword in ['enum', 'type']):
                groups['Enumerations'].append(cls)
                
        return {k: v for k, v in groups.items() if v}

def main():
    print("NLM Python Bindings Improvement Analysis")
    print("=" * 50)
    
    analyzer = PythonBindingsAnalyzer("python/bindings.cpp")
    analyzer.analyze_structure()
    
    print("\n=== Current bindings.cpp Analysis ===")
    print("Total lines:", len(analyzer.content.splitlines()))
    
    # Count class bindings by examining the file directly
    with open("python/bindings.cpp", "r") as f:
        content = f.read()
        class_count = len(re.findall(r'py::class_<[^>]+>', content))
        def_count = len(re.findall(r'm\.def\(', content))
        
    print(f"Pybind11 class definitions: {class_count}")
    print(f"Pybind11 function definitions: {def_count}")
    print(f"Establishes this is a monolithic binding file (should be ~10-15 modules)")
    
    print("\n=== Recommendations ===")
    print("1. Split bindings.cpp into modular files (core_types.cpp, brain_module.cpp, etc.)")
    print("2. Add convenience functions and high-level APIs")
    print("3. Create comprehensive error handling and documentation")
    print("4. Implement advanced visualization and analysis tools")
    print("5. Add more sophisticated experiment templates")
    print("\nThis will improve maintainability and add advanced features for expert users.")

if __name__ == "__main__":
    main()
