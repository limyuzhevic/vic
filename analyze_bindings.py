#!/usr/bin/env python3
"""
Script to analyze missing methods in Python bindings compared to C++ headers.
This script analyzes the differences between C++ class methods and their Python bindings
by reading the actual files and comparing method signatures.
"""

import re
import os
from typing import Dict, List, Set, Tuple

# Define the C++ classes we want to analyze
CPP_CLASSES = [
    ("nlm::Config", 
     "/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_53f69f73-17c4-4450-8d32-7c755a8c5459/src/core/Config/Config.hpp"),
    ("nlm::AgentBrain",
     "/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_53f69f73-17c4-4450-8d32-7c755a8c5459/src/agent/AgentBrain.hpp"),
    ("nlm::SimpleWorld",
     "/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_53f69f73-17c4-4450-8d32-7c755a8c5459/src/world/SimpleWorld.hpp"),
    ("nlm::Brain",
     "/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_53f69f73-17c4-4450-8d32-7c755a8c5459/src/brain/Brain.hpp")
]

# Python bindings file
BINDINGS_FILE = "/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_53f69f73-17c4-4450-8d32-7c755a8c5459/python/bindings.cpp"

class MethodAnalyzer:
    def __init__(self):
        self.class_methods: Dict[str, List[str]] = {}
        self.class_python_methods: Dict[str, List[str]] = {}
        self.missing_methods: Dict[str, List[str]] = {}
        
    def extract_methods_from_header(self, header_path: str, class_name: str) -> List[str]:
        """Extract all public method signatures from a C++ header file."""
        methods = []
        
        try:
            with open(header_path, 'r') as f:
                content = f.read()
                
            # Remove comments to simplify parsing
            content = re.sub(r'//.*', '', content)
            content = re.sub(r'/\*.*?\*/', '', content, flags=re.DOTALL)
            
            # Find the class definition
            class_pattern = rf'class {class_name.split("::")[-1]}\s*\{{[^}]+}}'
            match = re.search(class_pattern, content, re.DOTALL)
            
            if match:
                class_body = match.group(0)
                
                # Extract public methods
                public_section = re.search(r'public:\s*([^:]+)', class_body)
                if public_section:
                    public_methods = public_section.group(1)
                    
                    # Match method signatures
                    method_pattern = r'\w+\s+(\w+)\([^;]*?\)\s*;'
                    method_matches = re.finditer(method_pattern, public_methods)
                    
                    for method_match in method_matches:
                        method_name = method_match.group(1)
                        # Skip constructor, destructor, and operator overloads that shouldn't be exposed
                        if not (method_name.startswith('operator') or 
                               method_name == class_name.split("::")[-1] or
                               method_name in ['initialize', 'step', 'reset', 'load', 'save']):
                            methods.append(method_name)
                            
        except Exception as e:
            print(f"Error parsing header {header_path} for class {class_name}: {e}")
            
        return methods
    
    def extract_python_methods(self, bindings_file: str) -> Dict[str, List[str]]:
        """Extract methods that are exposed in Python bindings."""
        python_methods = {}
        
        try:
            with open(bindings_file, 'r') as f:
                content = f.read()
                
            # Find each class binding
            for class_name, cpp_class_name in CPP_CLASSES:
                # Find the class binding pattern
                class_pattern = rf'py::class_<[^>]+\s+m, "([^"]+)"'
                for match in re.finditer(class_pattern, content):
                    exposed_class = match.group(1)
                    if exposed_class == cpp_class_name:
                        # Extract all .def() and .def_readwrite() calls
                        # This is a more comprehensive extraction
                        def_pattern = rf'\.def\([^}}]*?\)'
                        def_matches = re.finditer(def_pattern, content)
                        
                        methods = []
                        for def_match in def_matches:
                            def_call = def_match.group(0)
                            # Extract method name
                            method_name = re.search(r'\.def\((?:[^{"]|"[^"]*"|[^{"])*?\)' , def_call)
                            if method_name:
                                # Extract the method name from the def call
                                method_match = re.search(r'\.def\((?:[^"]|"[^"]*"|[^"])*\.\.\.([^"]+)', def_call)
                                if method_match:
                                    methods.append(method_match.group(1))
                                else:
                                    # Try simpler pattern
                                    method_match = re.search(r'\.def\("([^"]+)"', def_call)
                                    if method_match:
                                        methods.append(method_match.group(1))
                                
                        # Also check for .def_readwrite()
                        readwrite_pattern = r'\.def_readwrite\("([^"]+)"'
                        readwrite_matches = re.finditer(readwrite_pattern, content)
                        for rw_match in readwrite_matches:
                            if rw_match.group(1) not in methods:
                                methods.append(rw_match.group(1))
                                
                        python_methods[class_name] = methods
                        break
                        
        except Exception as e:
            print(f"Error parsing bindings file {bindings_file}: {e}")
            
        return python_methods
    
    def analyze_differences(self):
        """Analyze differences between C++ and Python methods."""
        print("Analyzing differences between C++ and Python bindings...\n")
        
        # Extract C++ methods
        for class_name, header_path in CPP_CLASSES:
            cpp_methods = self.extract_methods_from_header(header_path, class_name)
            self.class_methods[class_name] = cpp_methods
            print(f"C++ {class_name} methods: {len(cpp_methods)}")
        
        # Extract Python methods  
        self.class_python_methods = self.extract_python_methods(BINDINGS_FILE)
        
        # Analyze differences
        for class_name in CPP_CLASSES:
            cpp_methods = set(self.class_methods.get(class_name, []))
            python_methods = set(self.class_python_methods.get(class_name, []))
            
            missing = cpp_methods - python_methods
            self.missing_methods[class_name] = list(missing)
            
            print(f"\n{class_name}:")
            print(f"  C++ methods: {len(cpp_methods)}")
            print(f"  Python methods: {len(python_methods)}")
            print(f"  Missing from Python: {len(missing)}")
            
            if missing:
                print("  Missing methods:")
                for method in sorted(missing):
                    print(f"    - {method}")
                    
            # Show some Python methods for comparison
            if python_methods:
                print("  Python methods (sample):")
                for method in sorted(list(python_methods))[:5]:
                    print(f"    ✓ {method}")

if __name__ == "__main__":
    analyzer = MethodAnalyzer()
    analyzer.analyze_differences()