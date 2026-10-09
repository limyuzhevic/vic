// Memory system consolidation complete
// 
// Legacy placeholder memory system removed.
// 
// The actual memory implementations are now integrated as part of the neural systems:
// - NeuralWorkingMemory: Working memory using persistent neural activity
// - NeuralEpisodicMemory: Episodic memory with experience encoding and replay
// - NeuralAssociativeMemory: Associative memory for pattern relationships
// 
// These systems are properly initialized and integrated into the brain loop
// during Brain::initialize() and participate in the complete cognitive architecture.
// 
// Legacy files removed:
// - src/memory/Memory.cpp (placeholder implementations)
// - src/memory/Memory.hpp (placeholder headers)
// 
// Remaining neural memory system files:
// - src/memory/NeuralWorkingMemory.hpp/cpp
// - src/memory/NeuralEpisodicMemory.hpp/cpp
// - src/memory/NeuralAssociativeMemory.hpp/cpp

// This file is kept for backward compatibility but now serves as a manifest
// of which memory systems are actually implemented and integrated.

#pragma once

#include "NeuralWorkingMemory.hpp"
#include "NeuralEpisodicMemory.hpp"
#include "NeuralAssociativeMemory.hpp"

namespace nlm {

// This header confirms which memory systems are available and integrated

// Working memory is available via getWorkingMemory()
// Episodic memory is available via getEpisodicMemory()
// Associative memory is available via getAssociativeMemory()

} // namespace nlm
