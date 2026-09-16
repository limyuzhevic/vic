#pragma once

#include "../core/Types/Types.hpp"
#include "Brain.hpp"

namespace nlm {

class BrainMotor {
public:
    explicit BrainMotor(Brain::Impl* impl) : pImpl(impl) {}
    
    std::unique_ptr<class Action> produceAction() const;
    
private:
    Brain::Impl* pImpl;
};

} // namespace nlm
