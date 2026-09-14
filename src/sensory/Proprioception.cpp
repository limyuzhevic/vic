#include "Proprioception.hpp"
#include <memory>
#include <vector>

namespace nlm {

Proprioception::~Proprioception() = default;

const std::vector<float>& Proprioception::getData() const {
    if (pImpl) {
        // Convert state to vector
        static std::vector<float> data;
        data.resize(6);
        data[0] = pImpl->x_;
        data[1] = pImpl->y_;
        data[2] = pImpl->orientation_;
        data[3] = pImpl->velocityX_;
        data[4] = pImpl->velocityY_;
        data[5] = pImpl->angularVelocity_;
        return data;
    }
    static std::vector<float> emptyData;
    return emptyData;
}

} // namespace nlm