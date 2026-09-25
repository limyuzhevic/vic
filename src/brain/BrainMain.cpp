#include "Brain.hpp"

namespace nlm {

// Brain class implementation - main constructor/destructor/move operations

Brain::Brain(std::shared_ptr<Config> config) : pImpl(std::make_unique<Impl>(config)) {}

Brain::~Brain() = default;

Brain::Brain(Brain&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Brain& Brain::operator=(Brain&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

} // namespace nlm