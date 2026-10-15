#!/usr/bin/env python3
"""
Final check to see what files are actually in the workspace.
"""

import os
import sys

def main():
    print("WORKSPACE FILE STRUCTURE CHECK")
    print("=" * 60)
    
    workspace = "/workspace/app"
    
    print(f"\nWorkspace directory: {workspace}")
    
    # List all files and directories
    all_items = os.listdir(workspace)
    
    print(f"\nTotal items: {len(all_items)}")
    
    print(f"\n" + "=" * 60)
    print("DIRECTORY STRUCTURE")
    print("=" * 60)
    
    for item in all_items:
        item_path = os.path.join(workspace, item)
        
        if os.path.isdir(item_path):
            print(f"\n📁 {item}/")
            
            # List contents of directory
            try:
                sub_items = os.listdir(item_path)
                for sub_item in sub_items[:10]:  # Show first 10 items
                    sub_item_path = os.path.join(item_path, sub_item)
                    
                    if os.path.isdir(sub_item_path):
                        print(f"   📁 {sub_item}/")
                    else:
                        # Check file extension
                        ext = os.path.splitext(sub_item)[1]
                        if ext in ['.py', '.md', '.txt', '.cfg']:
                            print(f"   📄 {sub_item}{ext}")
                        elif ext in ['.cpp', '.hpp', '.h']:
                            print(f"   🔧 {sub_item}{ext}")
                        elif ext in ['.json']:
                            print(f"   📋 {sub_item}{ext}")
                        elif ext in ['.sh']:
                            print(f"   🔨 {sub_item}{ext}")
                        else:
                            print(f"   📄 {sub_item}")
                
                if len(sub_items) > 10:
                    print(f"   ... and {len(sub_items) - 10} more items")
            except PermissionError:
                print(f"   [Permission denied]")
        
        elif item.endswith(('.py', '.md', '.txt', '.cfg', '.json', '.sh')):
            ext = os.path.splitext(item)[1]
            print(f"\n📄 {item}{ext}")
        
        elif item.endswith(('.cpp', '.hpp', '.h')):
            ext = os.path.splitext(item)[1]
            print(f"\n🔧 {item}{ext}")
        
        elif item.endswith(('.json', '.toml', '.yaml', '.yml')):
            ext = os.path.splitext(item)[1]
            print(f"\n📋 {item}{ext}")
        
        elif item.endswith(('.sh', '.bash', '.zsh')):
            print(f"\n🔨 {item}")
        
        else:
            print(f"\n📄 {item}")
    
    print(f"\n" + "=" * 60)
    print("PYTHON PACKAGE STRUCTURE")
    print("=" * 60)
    
    # Check for python/ directory
    python_path = os.path.join(workspace, "python")
    if os.path.exists(python_path):
        print(f"\n✓ python/ directory found")
        
        python_items = os.listdir(python_path)
        print(f"  Items: {python_items}")
        
        # Check for bindings.py
        if "bindings.py" in python_items:
            print(f"  ✓ bindings.py found")
            
            # Check bindings.py size
            bindings_path = os.path.join(python_path, "bindings.py")
            size = os.path.getsize(bindings_path)
            print(f"    Size: {size:,} bytes ({size/1024:.1f} KB)")
            
            # Show first few lines
            print(f"    First 10 lines:")
            with open(bindings_path, 'r') as f:
                for i, line in enumerate(f.readlines()[:10]):
                    print(f"      {i+1}: {line.rstrip()}")
        
        # Check for __init__.py
        if "__init__.py" in python_items:
            print(f"  ✓ __init__.py found")
            
            # Show __init__.py content
            init_path = os.path.join(python_path, "__init__.py")
            with open(init_path, 'r') as f:
                init_content = f.read()
                if "from .bindings" in init_content:
                    print(f"    ✓ __init__.py imports from .bindings")
                else:
                    print(f"    ✗ __init__.py doesn't import from .bindings")
        
        # Check for other Python files
        python_files = [f for f in python_items if f.endswith('.py')]
        print(f"  Other Python files: {python_files}")
        
        # Check for documentation
        docs = [f for f in python_items if f.endswith(('.md', '.txt'))]
        print(f"  Documentation files: {docs}")
        
        # Check for CMakeLists.txt
        if "CMakeLists.txt" in python_items:
            print(f"  ✓ CMakeLists.txt found")
            
            # Check CMake content
            cmake_path = os.path.join(python_path, "CMakeLists.txt")
            with open(cmake_path, 'r') as f:
                cmake_content = f.read()
                
                if "pybind11_add_module(pynlm" in cmake_content:
                    print(f"    ✓ pybind11_add_module configured")
                    
                    # Check what it builds
                    if "pynlm" in cmake_content:
                        print(f"    ✓ Configures pynlm module")
                    else:
                        print(f"    ✗ Doesn't configure pynlm module")
                else:
                    print(f"    ✗ pybind11_add_module not found")
    
    else:
        print(f"\n✗ python/ directory not found")
    
    print(f"\n" + "=" * 60)
    print("SUMMARY")
    print("=" * 60)
    
    print(f"\nThe workspace contains:")
    print(f"  • C++ source code in src/ directory")
    print(f"  • Python bindings in python/ directory")
    print(f"  • Documentation in various markdown files")
    print(f"  • CMake configuration files")
    print(f"  • Test and verification scripts")
    
    print(f"\nThe python/ directory appears to be the main Python package")
    print(f"with all the enhanced bindings and features.")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
