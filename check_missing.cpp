#!/usr/bin/env python3
import os
import sys

def find_missing_pairs(root_dir):
    """Find all .hpp files that don't have a corresponding .cpp file"""
    missing = []
    
    for root, dirs, files in os.walk(root_dir):
        for file in files:
            if file.endswith('.hpp') or file.endswith('.h'):
                base_name = os.path.splitext(file)[0]
                cpp_name = base_name + '.cpp'
                hpp_path = os.path.join(root, file)
                cpp_path = os.path.join(root, cpp_name)
                
                if not os.path.exists(cpp_path):
                    missing.append(hpp_path)
    
    return missing

if __name__ == "__main__":
    root_dir = "/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_467cb2dc-451d-4d63-a507-c710c6e3509b/src"
    missing = find_missing_pairs(root_dir)
    
    print(f"Found {len(missing)} header files without corresponding implementation:")
    for hpp in sorted(missing):
        print(f"  {hpp}")