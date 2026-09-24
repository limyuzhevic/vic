// IComponent implementation
#include "IComponent.hpp"

namespace nlm {

template<typename T>
std::shared_ptr<T> IComponent::getDependency(const std::string& name) const {
    auto it = dependencies_.find(name);
    if (it != dependencies_.end()) {
        return std::dynamic_pointer_cast<T>(it->second);
    }
    return nullptr;
}

} // namespace nlm