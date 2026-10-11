# Find and fix CMakeLists.txt memory file references
set(memory_files
    src/memory/NeuralWorkingMemory.cpp
    src/memory/NeuralEpisodicMemory.cpp
    src/memory/Memory.cpp
)

# Read the CMakeLists.txt
file(READ CMakeLists.txt cmake_content)

# Fix memory file references
string(REPLACE "src/memory/WorkingMemory.cpp" "src/memory/Memory.cpp" cmake_content "${cmake_content}")
string(REPLACE "src/memory/EpisodicMemory.cpp" "src/memory/NeuralEpisodicMemory.cpp" cmake_content "${cmake_content}")
string(REPLACE "src/memory/SemanticMemory.cpp" "# src/memory/SemanticMemory.cpp (not found)" cmake_content "${cmake_content}")
string(REPLACE "src/memory/ProceduralMemory.cpp" "# src/memory/ProceduralMemory.cpp (not found)" cmake_content "${cmake_content}")
string(REPLACE "src/memory/AssociativeMemory.cpp" "# src/memory/AssociativeMemory.cpp (not found)" cmake_content "${cmake_content}")

# Write back to CMakeLists.txt
file(WRITE CMakeLists.txt "${cmake_content}")
