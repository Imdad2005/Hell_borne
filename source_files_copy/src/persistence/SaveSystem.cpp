#include "SaveSystem.h"
#include <fstream>

SaveSystem::SaveSystem(const std::string& path) : savePath(path) {
}

SaveSystem::~SaveSystem() {
}

SaveData SaveSystem::createDefault() const {
    SaveData data;
    data.hellgold = 0;
    data.highestFloor = 1;
    data.totalRuns = 0;
    data.totalKills = 0;
    return data;
}

bool SaveSystem::load(SaveData& outData) const {
    std::ifstream in(savePath.c_str(), std::ios::binary);
    if (!in.is_open()) {
        outData = createDefault();
        return false;
    }

    in.read(reinterpret_cast<char*>(&outData), sizeof(SaveData));
    return in.good() || in.eof();
}

bool SaveSystem::save(const SaveData& data) const {
    std::ofstream out(savePath.c_str(), std::ios::binary | std::ios::trunc);
    if (!out.is_open()) {
        return false;
    }

    out.write(reinterpret_cast<const char*>(&data), sizeof(SaveData));
    return out.good();
}
