#!/usr/bin/env python3
import os
import re

print("=== C++ Source File Analysis ===")
print()

# Collect all .cpp and .hpp files
cpp_files = []
hpp_files = []

for root, dirs, files in os.walk('src'):
    for file in files:
        if file.endswith('.cpp'):
            cpp_files.append(os.path.join(root, file))
        elif file.endswith('.hpp'):
            cpp_files.append(os.path.join(root, file))

print("1. Checking for .cpp files without corresponding .hpp files...")
missing_headers = 0
for cpp_file in cpp_files:
    if cpp_file.endswith('.cpp'):
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
for hpp_file in cpp_files:
    if hpp_file.endswith('.hpp'):
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
for cpp_file in cpp_files:
    if cpp_file.endswith('.cpp'):
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