// NLM Brain.cpp Refactoring - Implementation Headers

// This file contains the forward declarations and core headers
// needed for Brain system implementation.

#pragma once

#include "Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "development/DevelopmentSystem.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/NeuralAssociativeMemory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/AttentionalSelection.hpp"
#include "performance/CheckpointSystem.hpp"
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <cmath>
#include <cstring>
#include <sstream>

namespace nlm {

// Forward declarations for helper functions
class CheckpointWriter;
class CheckpointReader;

struct P5Reader;
void p5AppendU64(std::vector<uint8_t>& out, uint64_t v);
void p5AppendFloat(std::vector<uint8_t>& out, float v);
void p5AppendDouble(std::vector<uint8_t>& out, double v);
template <typename T>
void p5AppendVec(std::vector<uint8_t>& out, const std::vector<T>& v);

// Helper function for key generation
inline uint64_t pairKey(uint64_t src, uint64_t dst) {
    return (src << 32) ^ dst;
}

} // namespace nlm
