import re

# Read the CMakeLists.txt
with open('CMakeLists.txt', 'r') as f:
    content = f.read()

# Find all memory files and deduplicate
memory_files = []
for match in re.finditer(r'src/memory/([^/]+)\.cpp', content):
    if match.group(1) not in memory_files:
        memory_files.append(match.group(1))

# Rebuild the memory section
memory_section = '    # Memory\n'
for i, filename in enumerate(memory_files):
    memory_section += f'    src/memory/{filename}.cpp\n'

# Replace the entire memory section
memory_pattern = r'# Memory\n(.*?)(?=\n    # Cognition)'
content = re.sub(memory_pattern, memory_section, content, flags=re.DOTALL)

# Write back
with open('CMakeLists.txt', 'w') as f:
    f.write(content)

print(f"Cleaned CMakeLists.txt - kept unique memory files: {memory_files}")
