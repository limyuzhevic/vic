#include "NeuronProperty.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

struct NeuronProperty::Impl {
    NeuronId id;
    NeuronType type;
    NeuronState state;
    
    Impl() : id(), type(NeuronType::Internal), state() {}
};

}