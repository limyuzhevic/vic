#pragma once

#include "../core/Types/Types.hpp"
#include "Brain.hpp"

namespace nlm {

class BrainStats {
public:
    explicit BrainStats(Brain::Impl* impl) : pImpl(impl) {}
    
    void collectStatistics() const;
    
private:
    Brain::Impl* pImpl;
};

} // namespace nlm
