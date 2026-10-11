# Read the CMakeLists.txt
file(READ CMakeLists.txt content)

# Fix all memory file references
string(REPLACE "src/memory/SemanticMemory.cpp" "src/memory/Memory.cpp" content "${content}")
string(REPLACE "src/memory/ProceduralMemory.cpp" "src/memory/NeuralEpisodicMemory.cpp" content "${content}")
string(REPLACE "src/memory/AssociativeMemory.cpp" "src/memory/NeuralWorkingMemory.cpp" content "${content}")

# Write back
file(WRITE CMakeLists.txt "${content}")
