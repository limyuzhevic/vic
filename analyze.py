#!/usr/bin/env python3

cd /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_7d39c6dd-3f9c-4f65-ae6b-9977838e06d0

print("=== C++ Source File Analysis ===")
print()

print("1. Checking for .cpp files without corresponding .hpp files...")
missing_headers = 0
for file in list_cpp_files:
    base = os.path.splitext(os.path.basename(file))[0]
    dir_path = os.path.dirname(file)
    hpp_file = os.path.join(dir_path, base + '.hpp')
    if not os.path.exists(hpp_file):
        print(f"  MISSING HEADER: {file}")
        missing_headers += 1
print(f"Total files missing headers: {missing_headers}")
print()

print("2. Checking header guard presence...")
headers_with_no_guard = 0
for hpp_file in list_hpp_files:
    try:
        with open(hpp_file, 'r') as f:
            content = f.read()
            if '#pragma once' not in content:
                print(f"  NO GUARD: {hpp_file}")
                headers_with_no_guard += 1
    except:
        pass
print(f"Total headers without guard: {headers_with_no_guard}")
print()

print("3. Checking for incomplete implementations (TODO comments)...")
todo_count = 0
for cpp_file in list_cpp_files:
    try:
        with open(cpp_file, 'r') as f:
            content = f.read()
            todo_count += content.count('TODO')
            if 'TODO' in content:
                print(f"  {cpp_file}: has TODO comments")
    except:
        pass
print(f"Total TODOs in .cpp files: {todo_count}")
print()

print("4. Checking CMakeLists.txt references...")
print("Checking core library CMakeLists.txt...")
with open('CMakeLists.txt', 'r') as f:
    cmake_content = f.read()
    cmake_cpp_refs = len(re.findall(r'src/[^\s]+\.cpp', cmake_content))
print(f"Core library .cpp references in CMakeLists.txt: {cmake_cpp_refs}")

print("Checking tests/CMakeLists.txt...")
if os.path.exists('tests/CMakeLists.txt'):
    with open('tests/CMakeLists.txt', 'r') as f:
        tests_content = f.read()
        test_refs = len(re.findall(r'test_[^\s]+\.cpp', tests_content))
    print(f"Test .cpp references in tests/CMakeLists.txt: {test_refs}")
print()

print("=== Analysis Complete ===")

# Get file lists
list_cpp_files = []
list_hpp_files = []

for root, dirs, files in os.walk('src'):
    for file in files:
        if file.endswith('.cpp'):
            list_cpp_files.append(os.path.join(root, file))
        elif file.endswith('.hpp'):
            list_hpp_files.append(os.path.join(root, file))
import os
import re

print("=== C++ Source File Analysis ===")
print()

print("1. Checking for .cpp files without corresponding .hpp files...")
missing_headers = 0
for cpp_file in list_cpp_files:
    base = os.path.splitext(os.path.basename(cpp_file))[0]
    dir_path = os.path.dirname(cpp_file)
    hpp_file = os.path.join(dir_path, base + '.hpp')
    if not os.path.exists(hpp_file):
        print(f"  MISSING HEADER: {cpp_file}")
        missing_headers += 1
print(f"Total files missing headers: {missing_headers}")
print()

print("2. Checking header guard presence...")
headers_with_no_guard = 0
for hpp_file in list_hpp_files:
    try:
        with open(hpp_file, 'r') as f:
            content = f.read()
            if '#pragma once' not in content:
                print(f"  NO GUARD: {hpp_file}")
                headers_with_no_guard += 1
    except:
        pass
print(f"Total headers without guard: {headers_with_no_guard}")
print()

print("3. Checking for incomplete implementations (TODO comments)...")
todo_count = 0
for cpp_file in list_cpp_files:
    try:
        with open(cpp_file, 'r') as f:
            content = f.read()
            todo_matches = re.findall(r'TODO[^\n]*', content)
            todo_count += len(todo_matches)
            if todo_matches:
                print(f"  {cpp_file}: {len(todo_matches)} TODOs")
    except:
        pass
print(f"Total TODOs in .cpp files: {todo_count}")
print()

print("4. Checking CMakeLists.txt references...")
print("Checking core library CMakeLists.txt...")
with open('CMakeLists.txt', 'r') as f:
    cmake_content = f.read()
    cmake_cpp_refs = len(re.findall(r'src/[^\s]+\.cpp', cmake_content))
print(f"Core library .cpp references in CMakeLists.txt: {cmake_cpp_refs}")

print("Checking tests/CMakeLists.txt...")
if os.path.exists('tests/CMakeLists.txt'):
    with open('tests/CMakeLists.txt', 'r') as f:
        tests_content = f.read()
        test_refs = len(re.findall(r'test_[^\s]+\.cpp', tests_content))
    print(f"Test .cpp references in tests/CMakeLists.txt: {test_refs}")
print()

print("=== Analysis Complete ===")