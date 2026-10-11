import re

# Read the CMakeLists.txt
with open('CMakeLists.txt', 'r') as f:
    content = f.read()

# Fix Action.cpp reference
content = re.sub(r'src/environment/Action\.cpp', 'src/motor/Action.cpp', content)

# Write back
with open('CMakeLists.txt', 'w') as f:
    f.write(content)

print("Fixed Action.cpp reference")
