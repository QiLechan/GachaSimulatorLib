#ifndef _GACHA_SIMULATOR_LIB_H_
#define _GACHA_SIMULATOR_LIB_H_

#include <cjson/cJSON.h>

char* createGenshinJson();

typedef struct {
	int max_pity_counter;
	double five_star;
	double four_star;
	double ten_times;
} GlobalConfig;

typedef struct {
    char name[50];
    int id;
    int weight;
} Item;

typedef struct {
    char pool_name[50];
    char pool_type[20];
    int soft_pity_start;
    double rate_increment;
    Item* star5_up;
    int star5_count;
    Item* star4_up;
    int star4_count;
	Item* star5_others;
	int star5_others_count;
	Item* star4_others;
	int star4_others_count;
} GachaPool;

typedef struct {
    char version[20];
    char description[50];
    GlobalConfig global;
    GachaPool* pools;
    int pool_count;
} GachaConfig;

typedef struct {
	double star5;
	double star4;
} Probability;

GachaConfig* parse_config(const char* json);
Probability* probability(int times, GlobalConfig* config, GachaPool* pool);
int Gacha(Probability* prob);
void parse_result(int result, FILE* fp);

#endif
