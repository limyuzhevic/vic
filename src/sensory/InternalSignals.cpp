namespace nlm {

SensoryInput::SensoryInput() : timestamp_(0.0) {}

InternalSignals::InternalSignals() : pImpl(new Impl) {}

InternalSignals::~InternalSignals() = default;

const char* InternalSignals::getType() const {
    return "InternalSignals";
}

const std::vector<float>& InternalSignals::getData() const {
    return pImpl->signals;
}

size_t InternalSignals::getDimensions() const {
    return pImpl->signals.size();
}

std::unique_ptr<SensoryInput> InternalSignals::clone() const {
    auto clone = std::make_unique<InternalSignals>();
    for (float sig : pImpl->signals) {
        clone->addSignal(sig);
    }
    return clone;
}

void InternalSignals::addSignal(float value) {
    pImpl->signals.push_back(value);
}

void InternalSignals::clearSignals() {
    pImpl->signals.clear();
}

} // namespace nlm
