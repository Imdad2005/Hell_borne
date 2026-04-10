#ifndef ROOM_TEMPLATE_H
#define ROOM_TEMPLATE_H

#include "../utils/AABB.h"
#include <vector>

enum RoomType {
    ROOM_START,
    ROOM_COMBAT
};

struct RoomTemplate {
    RoomType type;
    std::vector<AABB> platforms;
    std::vector<AABB> enemySpawnZones;
};

class RoomTemplateLibrary {
public:
    static RoomTemplate createStartRoom();
    static RoomTemplate createCombatRoom();
};

#endif // ROOM_TEMPLATE_H
