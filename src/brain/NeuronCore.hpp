#pragma once

namespace nlm {

class NeuronCore {
public:
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
};

}