#include "raylib.h"
#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *levelLayout[] = {
    "####################",
    "#........#........D#",
    "#..##....#..T..H...#",
    "#..##.K..#..##.B...#",
    "#.......##..#..M...#",
    "#..####......#.....#",
    "#..#..#....L.#..M..#",
    "#..#..#.......#....#",
    "#..#..#########....#",
    "#.......T..........#",
    "#..M....!.........D#",
    "#..................#",
    "#......M...........#",
    "#.....T.....K......#",
    "####################"
};

static bool IsWalkable(const GameState *gs, int x, int y)
{
    if (x < 1 || x >= gs->width - 1 || y < 1 || y >= gs->height - 1) return false;
    return gs->map[y][x] != '#';
}

static Enemy *GetEnemyAt(const GameState *gs, int x, int y)
{
    for (Enemy *enemy = gs->enemyList; enemy; enemy = enemy->next) {
        if (enemy->x == x && enemy->y == y) return enemy;
    }
    return NULL;
}

static Item *GetItemAt(const GameState *gs, int x, int y, Item **prev)
{
    Item *item = gs->itemList;
    Item *previous = NULL;
    while (item) {
        if (item->x == x && item->y == y) {
            if (prev) *prev = previous;
            return item;
        }
        previous = item;
        item = item->next;
    }
    return NULL;
}

static void AddEnemy(GameState *gs, int x, int y, int hp)
{
    Enemy *enemy = malloc(sizeof(Enemy));
    if (!enemy) return;
    enemy->x = x;
    enemy->y = y;
    enemy->hp = hp;
    enemy->next = gs->enemyList;
    gs->enemyList = enemy;
}

static void RemoveEnemy(GameState *gs, Enemy *prev, Enemy *current)
{
    if (!current) return;
    if (prev) prev->next = current->next;
    else gs->enemyList = current->next;
    free(current);
}

static void AddItem(GameState *gs, int x, int y, ItemType type)
{
    Item *item = malloc(sizeof(Item));
    if (!item) return;
    item->x = x;
    item->y = y;
    item->type = type;
    item->next = gs->itemList;
    gs->itemList = item;
}

static void RemoveItem(GameState *gs, Item *prev, Item *current)
{
    if (!current) return;
    if (prev) prev->next = current->next;
    else gs->itemList = current->next;
    free(current);
}

static void FreeEnemies(GameState *gs)
{
    Enemy *current = gs->enemyList;
    while (current) {
        Enemy *next = current->next;
        free(current);
        current = next;
    }
    gs->enemyList = NULL;
}

static void FreeItems(GameState *gs)
{
    Item *current = gs->itemList;
    while (current) {
        Item *next = current->next;
        free(current);
        current = next;
    }
    gs->itemList = NULL;
}

static void AllocateMap(GameState *gs)
{
    gs->map = malloc(gs->height * sizeof(char *));
    if (!gs->map) return;
    for (int y = 0; y < gs->height; y++) {
        gs->map[y] = malloc(gs->width * sizeof(char));
        if (!gs->map[y]) return;
    }
}

static void FreeMap(GameState *gs)
{
    if (!gs->map) return;
    for (int y = 0; y < gs->height; y++) {
        free(gs->map[y]);
    }
    free(gs->map);
    gs->map = NULL;
}

static void InitTextures(GameState *gs)
{
    gs->texFloor = LoadTextureFromImage(GenImageColor(gs->tileSize, gs->tileSize, BEIGE));
    gs->texWall = LoadTextureFromImage(GenImageColor(gs->tileSize, gs->tileSize, DARKGRAY));
    gs->texDoor = LoadTextureFromImage(GenImageColor(gs->tileSize, gs->tileSize, BROWN));
    gs->texTrap = LoadTextureFromImage(GenImageChecked(gs->tileSize, gs->tileSize, 8, 8, MAROON, LIGHTGRAY));
    gs->texPlayer = LoadTextureFromImage(GenImageChecked(gs->tileSize, gs->tileSize, 8, 8, BLUE, SKYBLUE));
    gs->texEnemy = LoadTextureFromImage(GenImageChecked(gs->tileSize, gs->tileSize, 8, 8, RED, BLACK));
    gs->texKey = LoadTextureFromImage(GenImageColor(gs->tileSize, gs->tileSize, SKYBLUE));
    gs->texTreasure = LoadTextureFromImage(GenImageColor(gs->tileSize, gs->tileSize, GOLD));
    gs->texLever = LoadTextureFromImage(GenImageColor(gs->tileSize, gs->tileSize, ORANGE));
    gs->texHealth = LoadTextureFromImage(GenImageColor(gs->tileSize, gs->tileSize, GREEN));
    gs->texAttack = LoadTextureFromImage(GenImageColor(gs->tileSize, gs->tileSize, PURPLE));
}

static void UnloadTextures(GameState *gs)
{
    UnloadTexture(gs->texFloor);
    UnloadTexture(gs->texWall);
    UnloadTexture(gs->texDoor);
    UnloadTexture(gs->texTrap);
    UnloadTexture(gs->texPlayer);
    UnloadTexture(gs->texEnemy);
    UnloadTexture(gs->texKey);
    UnloadTexture(gs->texTreasure);
    UnloadTexture(gs->texLever);
    UnloadTexture(gs->texHealth);
    UnloadTexture(gs->texAttack);
}

static void LoadHighScore(GameState *gs)
{
    FILE *file = fopen("highscore.txt", "r");
    gs->highScore = 0;
    if (!file) return;
    fscanf(file, "%d", &gs->highScore);
    fclose(file);
}

static void SaveHighScore(GameState *gs)
{
    if (gs->player.score <= gs->highScore) return;
    FILE *file = fopen("highscore.txt", "w");
    if (!file) return;
    fprintf(file, "%d\n", gs->player.score);
    fclose(file);
}

static void CreateDungeon(GameState *gs)
{
    for (int y = 0; y < gs->height; y++) {
        for (int x = 0; x < gs->width; x++) {
            char tile = levelLayout[y][x];
            switch (tile) {
                case '#': gs->map[y][x] = '#'; break;
                case 'D': gs->map[y][x] = 'D'; break;
                case '!': gs->map[y][x] = '!'; break;
                case 'M': gs->map[y][x] = '.'; AddEnemy(gs, x, y, 3); break;
                case 'T': gs->map[y][x] = '.'; AddItem(gs, x, y, ITEM_TREASURE); break;
                case 'K': gs->map[y][x] = '.'; AddItem(gs, x, y, ITEM_KEY); break;
                case 'L': gs->map[y][x] = '.'; AddItem(gs, x, y, ITEM_LEVER); break;
                case 'H': gs->map[y][x] = '.'; AddItem(gs, x, y, ITEM_HEALTH_POTION); break;
                case 'B': gs->map[y][x] = '.'; AddItem(gs, x, y, ITEM_ATTACK_UP); break;
                default: gs->map[y][x] = '.'; break;
            }
        }
    }
}

bool InitGame(GameState *gs)
{
    if (!gs) return false;
    gs->width = 20;
    gs->height = 15;
    gs->tileSize = 32;
    gs->gameOver = false;
    gs->win = false;
    gs->enemyList = NULL;
    gs->itemList = NULL;

    AllocateMap(gs);
    InitTextures(gs);
    CreateDungeon(gs);
    LoadHighScore(gs);

    gs->player.x = 2;
    gs->player.y = 2;
    gs->player.hp = 8;
    gs->player.attack = 2;
    gs->player.score = 0;
    gs->player.hasKey = false;
    gs->player.doorOpened = false;
    return true;
}

void UnloadGame(GameState *gs)
{
    SaveHighScore(gs);
    UnloadTextures(gs);
    FreeEnemies(gs);
    FreeItems(gs);
    FreeMap(gs);
}

static void OpenExitDoor(GameState *gs)
{
    gs->player.doorOpened = true;
    for (int y = 0; y < gs->height; y++) {
        for (int x = 0; x < gs->width; x++) {
            if (gs->map[y][x] == 'D') gs->map[y][x] = '.';
        }
    }
}

static void CollectItem(GameState *gs, int x, int y)
{
    Item *prev = NULL;
    Item *item = GetItemAt(gs, x, y, &prev);
    if (!item) return;

    switch (item->type) {
        case ITEM_TREASURE:
            gs->player.score += 20;
            break;
        case ITEM_KEY:
            gs->player.hasKey = true;
            gs->player.score += 5;
            break;
        case ITEM_LEVER:
            OpenExitDoor(gs);
            gs->player.score += 5;
            break;
        case ITEM_HEALTH_POTION:
            gs->player.hp += 4;
            gs->player.score += 10;
            break;
        case ITEM_ATTACK_UP:
            gs->player.attack += 1;
            gs->player.score += 10;
            break;
    }
    RemoveItem(gs, prev, item);
}

static void MovePlayer(GameState *gs, int dx, int dy)
{
    int newX = gs->player.x + dx;
    int newY = gs->player.y + dy;
    if (!IsWalkable(gs, newX, newY)) return;

    char tile = gs->map[newY][newX];
    if (tile == 'D' && !gs->player.hasKey) return;

    Enemy *enemy = GetEnemyAt(gs, newX, newY);
    if (enemy) {
        enemy->hp -= gs->player.attack;
        gs->player.hp -= 1;
        if (enemy->hp <= 0) {
            gs->player.score += 15;
            Enemy *prev = NULL;
            Enemy *scan = gs->enemyList;
            while (scan && scan != enemy) {
                prev = scan;
                scan = scan->next;
            }
            RemoveEnemy(gs, prev, enemy);
        }
        return;
    }

    if (tile == '!') {
        gs->player.hp -= 2;
        gs->map[newY][newX] = '.';
    }

    if (tile == 'D' && gs->player.hasKey) {
        gs->map[newY][newX] = '.';
        gs->player.doorOpened = true;
        gs->player.score += 10;
    }

    CollectItem(gs, newX, newY);
    gs->player.x = newX;
    gs->player.y = newY;
}

static void AttackNearbyEnemy(GameState *gs)
{
    const int offsets[4][2] = {{1,0},{-1,0},{0,1},{0,-1}};
    for (int i = 0; i < 4; i++) {
        int tx = gs->player.x + offsets[i][0];
        int ty = gs->player.y + offsets[i][1];
        Enemy *enemy = GetEnemyAt(gs, tx, ty);
        if (enemy) {
            enemy->hp -= gs->player.attack;
            if (enemy->hp <= 0) {
                gs->player.score += 15;
                Enemy *prev = NULL;
                Enemy *scan = gs->enemyList;
                while (scan && scan != enemy) {
                    prev = scan;
                    scan = scan->next;
                }
                RemoveEnemy(gs, prev, enemy);
            }
            return;
        }
    }
}

static void UpdateEnemies(GameState *gs)
{
    for (Enemy *enemy = gs->enemyList; enemy; enemy = enemy->next) {
        int dx = gs->player.x - enemy->x;
        int dy = gs->player.y - enemy->y;
        int stepX = (dx > 0) - (dx < 0);
        int stepY = (dy > 0) - (dy < 0);
        int tryX = enemy->x;
        int tryY = enemy->y;

        if (abs(dx) > abs(dy)) tryX += stepX;
        else tryY += stepY;

        if (IsWalkable(gs, tryX, tryY) && !GetEnemyAt(gs, tryX, tryY) && !(tryX == gs->player.x && tryY == gs->player.y)) {
            enemy->x = tryX;
            enemy->y = tryY;
        }

        if (enemy->x == gs->player.x && enemy->y == gs->player.y) {
            gs->player.hp -= 1;
        }
    }
}

void UpdateGame(GameState *gs)
{
    if (gs->gameOver) return;

    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) MovePlayer(gs, 0, -1);
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) MovePlayer(gs, 0, 1);
    if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) MovePlayer(gs, -1, 0);
    if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) MovePlayer(gs, 1, 0);
    if (IsKeyPressed(KEY_SPACE)) AttackNearbyEnemy(gs);

    static int enemyTimer = 0;
    enemyTimer++;
    if (enemyTimer >= 20) {
        enemyTimer = 0;
        UpdateEnemies(gs);
    }

    if (gs->player.hp <= 0) {
        gs->gameOver = true;
        gs->win = false;
    }

    if (gs->player.doorOpened && gs->map[gs->player.y][gs->player.x] == '.' && gs->player.x == gs->width - 2 && gs->player.y == 1) {
        gs->gameOver = true;
        gs->win = true;
    }
}

void DrawGame(const GameState *gs)
{
    for (int y = 0; y < gs->height; y++) {
        for (int x = 0; x < gs->width; x++) {
            Texture2D tex = gs->texFloor;
            char tile = gs->map[y][x];
            switch (tile) {
                case '#': tex = gs->texWall; break;
                case 'D': tex = gs->texDoor; break;
                case '!': tex = gs->texTrap; break;
            }
            DrawTexture(tex, x * gs->tileSize, y * gs->tileSize, WHITE);
        }
    }

    for (Item *item = gs->itemList; item; item = item->next) {
        Texture2D tex = gs->texTreasure;
        switch (item->type) {
            case ITEM_KEY: tex = gs->texKey; break;
            case ITEM_LEVER: tex = gs->texLever; break;
            case ITEM_HEALTH_POTION: tex = gs->texHealth; break;
            case ITEM_ATTACK_UP: tex = gs->texAttack; break;
            default: tex = gs->texTreasure; break;
        }
        DrawTexture(tex, item->x * gs->tileSize, item->y * gs->tileSize, WHITE);
    }

    for (Enemy *enemy = gs->enemyList; enemy; enemy = enemy->next) {
        DrawTexture(gs->texEnemy, enemy->x * gs->tileSize, enemy->y * gs->tileSize, WHITE);
        DrawText(TextFormat("%d", enemy->hp), enemy->x * gs->tileSize + 6, enemy->y * gs->tileSize + 4, 12, WHITE);
    }

    DrawTexture(gs->texPlayer, gs->player.x * gs->tileSize, gs->player.y * gs->tileSize, WHITE);

    DrawText(TextFormat("HP: %d  ATK: %d  SCORE: %d", gs->player.hp, gs->player.attack, gs->player.score), 10, gs->height * gs->tileSize + 10, 20, RAYWHITE);
    DrawText(TextFormat("High Score: %d", gs->highScore), 10, gs->height * gs->tileSize + 34, 18, RAYWHITE);
    DrawText(TextFormat("Key: %s  Door: %s", gs->player.hasKey ? "SIM" : "NAO", gs->player.doorOpened ? "ABERTA" : "FECHADA"), 10, gs->height * gs->tileSize + 56, 18, RAYWHITE);
    DrawText("Use WASD / arrows para mover, ESPACO para atacar.", 260, gs->height * gs->tileSize + 10, 18, LIGHTGRAY);
    DrawText("Colete H para vida e B para ataque.", 260, gs->height * gs->tileSize + 32, 18, LIGHTGRAY);
}
