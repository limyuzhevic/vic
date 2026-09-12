// Create forward declaration file for all neuromodulators
// This file is included in Brain.hpp to provide forward declarations

#ifndef __NEUROMODULATOR_FORWARD_DECLS_HPP
#define __NEUROMODULATOR_FORWARD_DECLS_HPP

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
class Neuromodulator;

// Phase 1 neuromodulators (currently implemented)
class Dopamine;
class Curiosity;
class Novelty;
class PredictionError;

// Phase 2 neuromodulators (to be implemented for full integration)
class Serotonin;
class Norepinephrine;
class Acetylcholine;

} // namespace nlm

#endif // __NEUROMODULATOR_FORWARD_DECLS_HPP