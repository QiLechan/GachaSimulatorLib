#ifndef _GACHA_SIMULATOR_LIB_H_
#define _GACHA_SIMULATOR_LIB_H_

#include <cjson/cJSON.h>

void createGenshinJson(char* buffer);

typedef struct {
	int max_pity_counter;
	double five_star;
	double four_star;
} GlobalConfig;

typedef struct {
    char name[50];
    int id;
    int weight;
} UpItem;

typedef struct {
    char pool_name[50];
    char pool_type[20];
    int soft_pity_start;
    double rate_increment;
    UpItem* star5_up;
    int star5_count;
    UpItem* star4_up;
    int star4_count;
} GachaPool;

typedef struct {
    char version[20];
    char description[50];
    GlobalConfig global;
    GachaPool* pools;
    int pool_count;
} GachaConfig;

GlobalConfig parse_global_config(cJSON* global);
UpItem* parse_up_items(cJSON* up, int* count);

#endif
