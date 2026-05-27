#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include "raylib.h"

typedef enum {
    ITEM_TREASURE = 1,
    ITEM_KEY,
    ITEM_LEVER,
    ITEM_HEALTH_POTION,
    ITEM_ATTACK_UP
} ItemType;

typedef struct {
    int x;
    int y;
    int hp;
    int attack;
    int score;
    bool hasKey;
    bool doorOpened;
} Player;

typedef struct Enemy {
    int x;
    int y;
    int hp;
    struct Enemy *next;
} Enemy;

typedef struct Item {
    int x;
    int y;
    ItemType type;
    struct Item *next;
} Item;

typedef struct {
    int width;
    int height;
    int tileSize;
    char **map;
    Player player;
    Enemy *enemyList;
    Item *itemList;
    int highScore;
    bool gameOver;
    bool win;
    Texture2D texFloor;
    Texture2D texWall;
    Texture2D texDoor;
    Texture2D texTrap;
    Texture2D texPlayer;
    Texture2D texEnemy;
    Texture2D texKey;
    Texture2D texTreasure;
    Texture2D texLever;
    Texture2D texHealth;
    Texture2D texAttack;
} GameState;

bool InitGame(GameState *gs);
void UnloadGame(GameState *gs);
void UpdateGame(GameState *gs);
void DrawGame(const GameState *gs);

#endif // GAME_H
