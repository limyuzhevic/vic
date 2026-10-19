// Neuromodulator base implementation is now in the header file
// All concrete neuromodulator implementations are in their respective .cpp files

namespace nlm {

// Neuromodulator factory function (for convenience)
Neuromodulator* createNeuromodulator(const std::string& name) {
    if (name == "Dopamine" || name == "DA") {
        return new Dopamine();
    } else if (name == "Acetylcholine" || name == "ACh") {
        return new Acetylcholine();
    } else if (name == "Norepinephrine" || name == "NE") {
        return new Norepinephrine();
    } else if (name == "Serotonin" || name == "5-HT") {
        return new Serotonin();
    }
    return nullptr;
}

} // namespace nlm
