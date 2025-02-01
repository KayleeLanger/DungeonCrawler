
#include <iostream>
#include <fstream>
#include <string>
#include "logic.h"
#include "helper.h"

using std::cout, std::endl, std::ifstream, std::string;

/**
 * TODO: Student implement this function
 * Load representation of the dungeon level from file into the 2D map.
 * Calls createMap to allocate the 2D array.
 * @param   fileName    File name of dungeon level.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object by reference to set starting position.
 * @return  pointer to 2D dynamic array representation of dungeon map with player's location., or nullptr if loading fails for any reason
 * @updates  maxRow, maxCol, player
 */
char** loadLevel(const string& fileName, int& maxRow, int& maxCol, Player& player) {
    
    //INFO(maxRow);
    //INFO(maxCol);

    ifstream f(fileName);
    
    if (!f.is_open()) {
        return nullptr;
    }

    f >> maxRow >> maxCol;

    if (maxRow <= 0 || maxCol <= 0) {
        return nullptr;
    }
    
    f >> player.row >> player.col;

    if (player.row < 0 || player.row >= maxRow || player.col < 0 || player.col  >= maxCol) {
        return nullptr;
    }

    char** map = createMap(maxRow,maxCol);
    
    if (map == nullptr) {
        return nullptr;
    }

    for (int i = 0; i < maxRow; i++) {
        for (int j = 0; j < maxCol; j++) {
            f >> map[i][j];
            if (i == player.row && j == player.col) {
                map[i][j] = TILE_PLAYER;
            }
        }
    }


    f.close();
    return map;
}

/**
 * TODO: Student implement this function
 * Translate the character direction input by the user into row or column change.
 * That is, updates the nextRow or nextCol according to the player's movement direction.
 * @param   input       Character input by the user which translates to a direction.
 * @param   nextRow     Player's next row on the dungeon map (up/down).
 * @param   nextCol     Player's next column on dungeon map (left/right).
 * @updates  nextRow, nextCol
 */
void getDirection(char input, int& nextRow, int& nextCol) {
    switch(input) {
        case MOVE_UP:
        nextRow--;
        break;
        case MOVE_DOWN:
        nextRow++;
        break;
        case MOVE_LEFT:
        nextCol--;
        break;
        case MOVE_RIGHT:
        nextCol++;
        break;
    }
}

/**
 * TODO: [suggested] Student implement this function
 * Allocate the 2D map array.
 * Initialize each cell to TILE_OPEN.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @return  2D map array for the dungeon level, holds char type.
 */
char** createMap(int maxRow, int maxCol) {
  
    char **map = new char*[maxRow];

    for(int i = 0; i < maxRow; i++) {
        map[i] = new char[maxCol];
    
        for(int j = 0; j < maxCol; j++) {
            map[i][j]= TILE_OPEN;
        }
    }
  
    return map;

}

/**
 * TODO: Student implement this function
 * Deallocates the 2D map array.
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @return None
 * @update map, maxRow
 */
void deleteMap(char**& map, int& maxRow) {
    
    if (map != nullptr) {
        for (int i = 0; i < maxRow; i++) {
            delete[] map[i];
        }
        delete[] map;

        map = nullptr;
    }
        
    maxRow = 0;
}

/**
 * TODO: Student implement this function
 * Resize the 2D map by doubling both dimensions.
 * Copy the current map contents to the right, diagonal down, and below.
 * Do not duplicate the player, and remember to avoid memory leaks!
 * You can use the STATUS constants defined in logic.h to help!
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height), to be doubled.
 * @param   maxCol      Number of columns in the dungeon table (aka width), to be doubled.
 * @return  pointer to a dynamically-allocated 2D array (map) that has twice as many columns and rows in size.
 * @update maxRow, maxCol
 */
char** resizeMap(char** map, int& maxRow, int& maxCol) {
    if (map == nullptr || maxRow <= 0 || maxCol <= 0) {
        return nullptr;
    }

    int newMaxRow = maxRow * 2;
    int newMaxCol = maxCol * 2;

    char** newMap = new char*[newMaxRow];
    for (int i = 0; i < newMaxRow; i++) {
        newMap[i] = new char[newMaxCol];
    }

    for (int i = 0; i < newMaxRow; i++) {
        for (int j = 0; j < newMaxCol; j++) {
            newMap[i][j] = TILE_OPEN;
        }
    }

    int playerRow = -1;
    int playerCol = -1;

    for (int i = 0; i < maxRow; i++) {
        for (int j = 0; j < maxCol; j++) {
            if (map[i][j] != TILE_PLAYER) {
                newMap[i][j] = map[i][j];
                newMap[i][j + maxCol] = map[i][j];
                newMap[i + maxRow][j] = map[i][j];
                newMap[i + maxRow][j + maxCol] = map[i][j];
            } else {
                playerRow = i;
                playerCol = j;
            }
        }
    }

    maxRow = newMaxRow;
    maxCol = newMaxCol;

    if (playerRow != -1 && playerCol != -1) {
        newMap[playerRow][playerCol] = TILE_PLAYER;
    }

    for (int i = 0; i < maxRow / 2; i++) {
        delete[] map[i];
    }
    delete[] map;

    return newMap;
}



/**
 * TODO: Student implement this function
 * Checks if the player can move in the specified direction and performs the move if so.
 * Cannot move out of bounds or onto TILE_PILLAR or TILE_MONSTER.
 * Cannot move onto TILE_EXIT without at least one treasure. 
 * If TILE_TREASURE, increment treasure by 1.
 * Remember to update the map tile that the player moves onto and return the appropriate status.
 * You can use the STATUS constants defined in logic.h to help!
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object to by reference to see current location.
 * @param   nextRow     Player's next row on the dungeon map (up/down).
 * @param   nextCol     Player's next column on dungeon map (left/right).
 * @return  Player's movement status after updating player's position.
 * @update map contents, player
 */
int doPlayerMove(char** map, int maxRow, int maxCol, Player& player, int nextRow, int nextCol) {
    //fix (maybe) yay
    int flag = STATUS_MOVE;

    if (nextRow >= maxRow || nextRow < 0 || nextCol >= maxCol || nextCol < 0 ||
        map[nextRow][nextCol] == TILE_PILLAR || map[nextRow][nextCol] == TILE_MONSTER)
    {
        nextRow = player.row;
        nextCol = player.col;

        flag = STATUS_STAY;
    }

    else if (map[nextRow][nextCol] == TILE_TREASURE)
    {
        player.treasure += 1;
        flag = STATUS_TREASURE;
    }

    else if (map[nextRow][nextCol] == TILE_AMULET)
    {
        flag = STATUS_AMULET;
    }

    else if (map[nextRow][nextCol] == TILE_DOOR)
    {
        flag = STATUS_LEAVE;
    }

    else if (map[nextRow][nextCol] == TILE_EXIT)
    {
        if (player.treasure >= 1)
        {
            flag = STATUS_ESCAPE;
        }
        else
        {
            nextRow = player.row;
            nextCol = player.col;
            flag = STATUS_STAY;
        }
    }

    map[player.row][player.col] = TILE_OPEN;
    map[nextRow][nextCol] = TILE_PLAYER;

    player.row = nextRow;
    player.col = nextCol;

    return flag;
}

/**
 * TODO: Student implement this function
 * Update monster locations:
 * We check up, down, left, right from the current player position.
 * If we see an obstacle, there is no line of sight in that direction, and the monster does not move.
 * If we see a monster before an obstacle, the monster moves one tile toward the player.
 * We should update the map as the monster moves.
 * At the end, we check if a monster has moved onto the player's tile.
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object by reference for current location.
 * @return  Boolean value indicating player status: true if monster reaches the player, false if not.
 * @update map contents
 */
bool doMonsterAttack(char** map, int maxRow, int maxCol, const Player& player) {
    //lots of copy paste checks, work? now it does :)
    
    //Up
    for (int i = player.row - 1; i >= 0; --i) {
        if (map[i][player.col] == TILE_PILLAR) {
            break;
        }

        if (map[i][player.col] == TILE_MONSTER) {
            map[i][player.col] = TILE_OPEN;
            map[i + 1][player.col] = TILE_MONSTER;
        }
    }

    //Down
    for (int i = player.row + 1; i < maxRow; ++i) {
        if (map[i][player.col] == TILE_PILLAR) {
            break;
        }

        if (map[i][player.col] == TILE_MONSTER) {
            map[i][player.col] = TILE_OPEN;
            map[i - 1][player.col] = TILE_MONSTER;
        }
    }

    //Left
    for (int i = player.col - 1; i >= 0; --i) {
        if (map[player.row][i] == TILE_PILLAR) {
            break;
        }

        if (map[player.row][i] == TILE_MONSTER) {
            map[player.row][i] = TILE_OPEN;
            map[player.row][i + 1] = TILE_MONSTER;
        }
    }

    //Right
    for (int i = player.col + 1; i < maxCol; ++i) {
        if (map[player.row][i] == TILE_PILLAR) {
            break;
        }

        if (map[player.row][i] == TILE_MONSTER) {
            map[player.row][i] = TILE_OPEN;
            map[player.row][i - 1] = TILE_MONSTER;
        }
    }
    
    
    if (map[player.row][player.col] == TILE_MONSTER) {
        return true;
    }
    else {
        return false;
    }
     
}