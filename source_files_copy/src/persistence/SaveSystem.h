#ifndef SAVE_SYSTEM_H
#define SAVE_SYSTEM_H

#include <string>

struct SaveData {
    int hellgold;
    int highestFloor;
    int totalRuns;
    int totalKills;
};

class SaveSystem {
private:
    std::string savePath;

public:
    SaveSystem(const std::string& path = "hellborne.sav");
    ~SaveSystem();

    bool load(SaveData& outData) const;
    bool save(const SaveData& data) const;
    SaveData createDefault() const;
};

#endif // SAVE_SYSTEM_H
