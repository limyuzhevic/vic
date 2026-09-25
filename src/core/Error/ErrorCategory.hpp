#pragma once

namespace nlm {

// Error category for NLM system errors
enum class ErrorCategory {
    None = 0,
    FileSystem,
    Validation,
    Memory,
    Threading,
    Configuration,
    Random,
    Simulation,
    Generic,
    Count
};

} // namespace nlm
