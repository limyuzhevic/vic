#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Simple memory types - kept for backward compatibility
// The neural versions (NeuralWorkingMemory, NeuralEpisodicMemory, etc.)
// are the preferred implementations used by the system

// Forward declarations for simple memory types
class WorkingMemory;
class EpisodicMemory;
class SemanticMemory;
class ProceduralMemory;
class AssociativeMemory;

// NeuralWorkingMemory is the primary working memory implementation
#include "NeuralWorkingMemory.hpp"

} // namespace nlm
