import os
import re

# Directory to scan
base_dir = 'src'

# Pattern to match class definitions
class_pattern = r'class\s+(\w+)'

# Collect all classes
classes = {}
for root, dirs, files in os.walk(base_dir):
    for file in files:
        if file.endswith('.hpp'):
            path = os.path.join(root, file)
            with open(path, 'r') as f:
                content = f.read()
                for match in re.finditer(class_pattern, content):
                    class_name = match.group(1)
                    if class_name not in classes:
                        classes[class_name] = []
                    classes[class_name].append(path)

print(f"Total unique classes found: {len(classes)}")

# Find which classes are already bound in python/bindings.cpp
bound_classes = []
with open('python/bindings.cpp', 'r') as f:
    content = f.read()
    for class_name in classes.keys():
        if f'py::class_<{class_name}>' in content or (f'py::class_<' in content and f'{class_name}' in content):
            bound_classes.append(class_name)

print(f"\nClasses already bound: {len(bound_classes)}")
print(", ".join(bound_classes))

# Find classes NOT bound
total_classes = len(classes)
bound_count = len(bound_classes)
not_bound = []
for class_name in classes.keys():
    if class_name not in bound_classes:
        not_bound.append(class_name)

print(f"\nClasses NOT bound ({len(not_bound)}):")
for class_name in sorted(not_bound):
    print(f"  {class_name} - from {[os.path.basename(p) for p in classes[class_name]]}")