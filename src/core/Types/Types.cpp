#include "Types.hpp"

namespace nlm {

// Types.cpp - implementation file for template instantiations
// This file handles explicit template instantiations to avoid linker errors
// when using Config template functions in static libraries

// Note: The template instantiations for Config::get and Config::getOr
// are now handled within Config.cpp itself. The config.cpp file contains
// the actual implementations of these template functions, and the 
// template instantiations are implicitly instantiated when these functions
// are used throughout the codebase.

} // namespace nlm
