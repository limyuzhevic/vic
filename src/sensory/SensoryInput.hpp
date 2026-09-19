#pragma once

#include "Types.hpp"
#include <string>
#include <vector>

namespace nlm {

class SensoryInput {
public:
    virtual ~SensoryInput() = default;
    virtual NeuronType getType() const = 0;
    virtual const std::vector<float>& getData() const = 0;
    virtual size_t getDimensions() const = 0;
    virtual Timestamp getTimestamp() const = 0;
    virtual void setTimestamp(Timestamp timestamp) = 0;
    virtual void reset() = 0;
};

} // namespace nlm