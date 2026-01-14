#ifndef RESOURCELOADER_H
#define RESOURCELOADER_H

#ifdef IS_ANDROID
#include "raymob.h"
#else
#include "raylib.h"
#endif
#include "enums.h"
#include "config.h"

#include <string.h>
#include <stdlib.h>

typedef struct LevelData {
    EntityType entityType;
    int level;
    float whereToSpawnX;
    float whereToSpawnY;
    float whenSpawn;
    float health;
    bool isSpawned;
    bool isLoaded;
} LevelData ;

LevelData levelData[MAX_LEVEL_DATA] = {0};

const static struct {
    EntityType val;
    const char *str;
} conversion [] = {
    {ASTEROID, "Asteroid"},
};

EntityType str2enum (const char *str)
{
    int j;
    for (j = 0;  j < sizeof (conversion) / sizeof (conversion[0]);  ++j)
        if (!strcmp (str, conversion[j].str))
            return conversion[j].val;
    exit(EXIT_FAILURE);   
}

LevelData* LoadResources(void)
{
    int currentLevelDataItemindex = 0;
    if (FileExists("resources/data.csv"))
    {
        TraceLog(LOG_INFO, "With Data");
        char *text = LoadFileText("resources/data.csv");

        // Loading all the text lines
        int lineCount = 0;
        char **lines = LoadTextLines(text, &lineCount);

        // Going through all the read lines
        for (int i = 1; i < lineCount; i++)
        {
            // Each time we go through and calculate the height of the text to move the cursor appropriately
            // Returns first token
            char* token = strtok(lines[i], " , ");

            // Keep printing tokens while one of the
            // delimiters present in str[].
            levelData[currentLevelDataItemindex].level = atoll(token);
            int valuePlace = 1;
            while (token != NULL) {
                token = strtok(NULL, " , ");
                switch (valuePlace) {
                    case 1:
                        levelData[currentLevelDataItemindex].entityType = str2enum(token);
                    break;
                    case 2:
                        levelData[currentLevelDataItemindex].whereToSpawnX = atof(token);
                    break;
                    case 3:
                        levelData[currentLevelDataItemindex].whereToSpawnY = atof(token);
                    break;
                    case 4:
                        levelData[currentLevelDataItemindex].whenSpawn = atof(token);
                    break;
                    case 5:
                        levelData[currentLevelDataItemindex].health = atof(token);
                    break;
                }
                valuePlace += 1;
            }
            levelData[currentLevelDataItemindex].isLoaded = true;
            currentLevelDataItemindex += 1;
        }
        UnloadTextLines(lines, lineCount);
    }
    else 
    {
        TraceLog(LOG_INFO, "No Data");
    }
    return levelData;
}

#endif //RESOURCELOADER_H