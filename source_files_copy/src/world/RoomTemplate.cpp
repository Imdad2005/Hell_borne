#include "RoomTemplate.h"
#include "../core/globals.h"

RoomTemplate RoomTemplateLibrary::createStartRoom() {
    RoomTemplate room;
    room.type = ROOM_START;
    room.platforms.push_back(AABB(0, SCREEN_HEIGHT - 100, SCREEN_WIDTH * 3, 100));
    room.platforms.push_back(AABB(200, 500, 200, 32));
    room.platforms.push_back(AABB(500, 400, 200, 32));
    room.platforms.push_back(AABB(100, 350, 200, 32));
    room.platforms.push_back(AABB(-200, 450, 200, 32));
    room.platforms.push_back(AABB(800, 350, 200, 32));
    room.enemySpawnZones.push_back(AABB(240, 320, 40, 40));
    room.enemySpawnZones.push_back(AABB(620, 260, 40, 40));
    return room;
}

RoomTemplate RoomTemplateLibrary::createCombatRoom() {
    RoomTemplate room;
    room.type = ROOM_COMBAT;
    room.platforms.push_back(AABB(0, SCREEN_HEIGHT - 100, SCREEN_WIDTH * 3, 100));
    room.platforms.push_back(AABB(150, 520, 180, 32));
    room.platforms.push_back(AABB(420, 440, 220, 32));
    room.platforms.push_back(AABB(760, 360, 180, 32));
    room.enemySpawnZones.push_back(AABB(220, 360, 40, 40));
    room.enemySpawnZones.push_back(AABB(480, 320, 40, 40));
    room.enemySpawnZones.push_back(AABB(820, 280, 40, 40));
    return room;
}
