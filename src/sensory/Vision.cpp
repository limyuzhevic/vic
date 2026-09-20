#include "Vision.hpp"

namespace nlm {

struct VisionProcessor::Impl {
    std::vector<float> features;
    size_t featureDimensions;
    
    Impl() : featureDimensions(0) {}
};

VisionProcessor::VisionProcessor() : pImpl(new Impl) {}

VisionProcessor::~VisionProcessor() = default;

VisionProcessor::VisionProcessor(VisionProcessor&& other) noexcept : pImpl(std::move(other.pImpl)) {}

VisionProcessor& VisionProcessor::operator=(VisionProcessor&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}
