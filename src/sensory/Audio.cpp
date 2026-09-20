AudioProcessor::AudioProcessor() : pImpl(std::make_unique<Impl>()) {}

AudioProcessor::~AudioProcessor() = default;

AudioProcessor::AudioProcessor(AudioProcessor&& other) noexcept : pImpl(std::move(other.pImpl)) {}

AudioProcessor& AudioProcessor::operator=(AudioProcessor&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

void AudioProcessor::process(const Audio& input) {
    // TODO PHASE 2: Implement real audio processing using NLM's neural machinery
    // PLACEHOLDER: Just pass through raw data
    pImpl->features = input.getData();
    pImpl->featureDimensions = input.getDimensions();
}

const std::vector<float>& AudioProcessor::getFeatures() const {
    return pImpl->features;
}

 size_t AudioProcessor::getFeatureDimensions() const {
    return pImpl->featureDimensions;
}

} // namespace nlm
