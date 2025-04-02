#pragma execution_character_set("utf-8")
#include <cjson/cJSON.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "GachaSimulatorLib.h"
#include <stdbool.h>

// 创建原神初始JSON配置
char* createGenshinJson()
{
	cJSON* root = cJSON_CreateObject();

	// 基本信息
	cJSON_AddItemToObject(root, "version", cJSON_CreateString("1.0.0"));
	cJSON_AddItemToObject(root, "description", cJSON_CreateString("原神抽卡配置"));

	//全局配置
	cJSON* global = cJSON_AddObjectToObject(root, "global_config");
	cJSON_AddNumberToObject(global, "max_pity_counter", 90);		//大保底
	cJSON_AddNumberToObject(global, "5star", 0.006);				//基础5星概率
	cJSON_AddNumberToObject(global, "4star", 0.051);				//基础4星概率
	cJSON_AddNumberToObject(global, "ten_times", 0.006);			//十连5星概率

	//卡池
	cJSON* pools = cJSON_AddArrayToObject(root, "pools");
	cJSON* pool_template = cJSON_CreateObject();
	cJSON_AddItemToObject(pool_template, "pool_name", cJSON_CreateString("常驻祈愿"));
	cJSON_AddItemToObject(pool_template, "pool_backgound", cJSON_CreateString(""));
	cJSON_AddItemToObject(pool_template, "pool_type", cJSON_CreateString("角色"));

	//概率
	cJSON* rates = cJSON_AddObjectToObject(pool_template, "rates");
	cJSON_AddNumberToObject(rates, "5star_soft_pity_start", 74); //5星软保底开始抽数
	cJSON_AddNumberToObject(rates, "5star_rate_increment", 0.06); //5星概率递增

	//保底规则
	cJSON* pity_rules = cJSON_AddObjectToObject(pool_template, "pity_rules");

	//UP
	cJSON* up = cJSON_AddObjectToObject(pool_template, "up");
	cJSON* five_stars = cJSON_AddArrayToObject(up, "5star");
	cJSON* four_stars = cJSON_AddArrayToObject(up, "4star");

	//UP示例
	cJSON* sample_5star = cJSON_CreateObject();
	cJSON_AddStringToObject(sample_5star, "name", "五星UP角色");
	cJSON_AddNumberToObject(sample_5star, "id", 10001);
	cJSON_AddNumberToObject(sample_5star, "weight", 5000);	//5星权重 总权重10000
	cJSON_AddItemToArray(five_stars, sample_5star);

	cJSON* sample_4star = cJSON_CreateObject();
	cJSON_AddStringToObject(sample_4star, "name", "四星UP角色");
	cJSON_AddNumberToObject(sample_4star, "id", 20001);
	cJSON_AddNumberToObject(sample_4star, "weight", 5000);	//4星权重 总权重10000
	cJSON_AddItemToArray(four_stars, sample_4star);

	//UP以外
	cJSON* others = cJSON_AddObjectToObject(pool_template, "others");
	five_stars = cJSON_AddArrayToObject(others, "5star");
	four_stars = cJSON_AddArrayToObject(others, "4star");
	cJSON* three_stars = cJSON_AddArrayToObject(others, "3star");

	//UP以外示例
	sample_5star = cJSON_CreateObject();
	cJSON_AddStringToObject(sample_5star, "name", "五星角色");
	cJSON_AddNumberToObject(sample_5star, "id", 10001);
	cJSON_AddNumberToObject(sample_5star, "weight", 5000);	//5星权重 总权重10000
	cJSON_AddItemToArray(five_stars, sample_5star);

	sample_4star = cJSON_CreateObject();
	cJSON_AddStringToObject(sample_4star, "name", "四星角色");
	cJSON_AddNumberToObject(sample_4star, "id", 10001);
	cJSON_AddNumberToObject(sample_4star, "weight", 5000);	//4星权重 总权重10000
	cJSON_AddItemToArray(four_stars, sample_4star);

	cJSON* sample_3star = cJSON_CreateObject();
	cJSON_AddStringToObject(sample_3star, "name", "三星物品");
	cJSON_AddNumberToObject(sample_3star, "id", 30001);
	cJSON_AddNumberToObject(sample_3star, "weight", 10000);	//3星权重 总权重10000
	cJSON_AddItemToArray(three_stars, sample_3star);

	cJSON_AddItemToArray(pools, pool_template);

	char* json = cJSON_Print(root);
	cJSON_Delete(root);
	return json;
}

// 解析物品
Item* parse_items(cJSON* array, int* count) {
	if (!cJSON_IsArray(array)) return NULL;

	int size = cJSON_GetArraySize(array);
	Item* items = malloc(size * sizeof(Item));
	*count = 0;

	cJSON* item;
	cJSON_ArrayForEach(item, array) {
		if (cJSON_IsObject(item)) {
			Item _item = { 0 };

			cJSON* name = cJSON_GetObjectItem(item, "name");
			if (cJSON_IsString(name))
                strncpy_s(_item.name, sizeof(_item.name), name->valuestring, sizeof(_item.name) - 1);

			cJSON* id = cJSON_GetObjectItem(item, "id");
			if (cJSON_IsNumber(id)) _item.id = id->valueint;

			cJSON* weight = cJSON_GetObjectItem(item, "weight");
			if (cJSON_IsNumber(weight)) _item.weight = weight->valueint;

			items[(*count)++] = _item;
		}
	}
	return items;
}

// 解析单卡池
GachaPool parse_pool(cJSON* pool_obj) {
	GachaPool pool = { 0 };

	// 基础信息
	cJSON* name = cJSON_GetObjectItem(pool_obj, "pool_name");
	if (cJSON_IsString(name))
        strncpy_s(pool.pool_name, sizeof(pool.pool_name), name->valuestring, sizeof(pool.pool_name) - 1);

	cJSON* background = cJSON_GetObjectItem(pool_obj, "pool_backgound");
	if (cJSON_IsString(background))
		strncpy_s(pool.pool_backgound, sizeof(pool.pool_backgound), background->valuestring, sizeof(pool.pool_backgound) - 1);

	cJSON* type = cJSON_GetObjectItem(pool_obj, "pool_type");
	if (cJSON_IsString(type))
        strncpy_s(pool.pool_type, sizeof(pool.pool_type), type->valuestring, sizeof(pool.pool_type) - 1);

	// 概率配置
	cJSON* rates = cJSON_GetObjectItem(pool_obj, "rates");
	if (rates) {
		cJSON* soft = cJSON_GetObjectItem(rates, "5star_soft_pity_start");
		if (cJSON_IsNumber(soft)) pool.soft_pity_start = soft->valueint;

		cJSON* inc = cJSON_GetObjectItem(rates, "5star_rate_increment");
		if (cJSON_IsNumber(inc)) pool.rate_increment = inc->valuedouble;
	}

	// UP 物品
	cJSON* up = cJSON_GetObjectItem(pool_obj, "up");
	if (up) {
		cJSON* star5 = cJSON_GetObjectItem(up, "5star");
		pool.star5_up = parse_items(star5, &pool.star5_up_count);

		cJSON* star4 = cJSON_GetObjectItem(up, "4star");
		pool.star4_up = parse_items(star4, &pool.star4_up_count);
	}

	// 其他物品
	cJSON* others = cJSON_GetObjectItem(pool_obj, "others");
	if (others) {
		cJSON* star5 = cJSON_GetObjectItem(others, "5star");
		pool.star5_others = parse_items(star5, &pool.star5_others_count);

		cJSON* star4 = cJSON_GetObjectItem(others, "4star");
		pool.star4_others = parse_items(star4, &pool.star4_others_count);

		cJSON* star3 = cJSON_GetObjectItem(others, "3star");
		pool.star3_others = parse_items(star3, &pool.star3_others_count);
	}
	return pool;
}

// JSON解析主函数
GachaConfig* parse_config(const char* json)
{
	cJSON* root = cJSON_Parse(json);
	if (!root) {
		fprintf(stderr, "JSON解析错误: %s\n", cJSON_GetErrorPtr());
		return NULL;
	}

	// 解析基本信息
	GachaConfig* config = calloc(1, sizeof(GachaConfig));
	cJSON* version = cJSON_GetObjectItem(root, "version");
	if (cJSON_IsString(version))
		strncpy_s(config->version, sizeof(config->version), version->valuestring, sizeof(config->version) - 1);

	cJSON* desc = cJSON_GetObjectItem(root, "description");
	if (cJSON_IsString(desc))
		strncpy_s(config->description, sizeof(config->description), desc->valuestring, sizeof(config->description) - 1);

	// 解析全局配置
	cJSON* global = cJSON_GetObjectItem(root, "global_config");
	if (global) {
		cJSON* pity = cJSON_GetObjectItem(global, "max_pity_counter");
		if (cJSON_IsNumber(pity)) config->global.max_pity_counter = pity->valueint;

		cJSON* star5 = cJSON_GetObjectItem(global, "5star");
		if (cJSON_IsNumber(star5)) config->global.five_star = star5->valuedouble;

		cJSON* star4 = cJSON_GetObjectItem(global, "4star");
		if (cJSON_IsNumber(star4)) config->global.four_star = star4->valuedouble;

		cJSON* ten_times = cJSON_GetObjectItem(global, "ten_times");
		if (cJSON_IsNumber(ten_times)) config->global.ten_times = ten_times->valuedouble;
	}

	// 解析卡池
	cJSON* pools = cJSON_GetObjectItem(root, "pools");
	if (cJSON_IsArray(pools)) {
		config->pool_count = cJSON_GetArraySize(pools);
		config->pools = malloc(config->pool_count * sizeof(GachaPool));

		for (int i = 0; i < config->pool_count; i++) {
			cJSON* pool_obj = cJSON_GetArrayItem(pools, i);
			config->pools[i] = parse_pool(pool_obj);
		}
	}
	cJSON_Delete(root);
	return config;
}

// 计算当前抽数的概率
Probability* probability(int counts, GlobalConfig* config, GachaPool* pool) {
	Probability* prob = malloc(sizeof(Probability));
	int four_stars_times = counts % 10;
	if (counts <= pool->soft_pity_start){
		if (four_stars_times == 0) {
			prob->star5 = config->ten_times;
			prob->star4 = 1 - prob->star5;
		}
		else{
			prob->star4 = config->four_star;
			prob->star5 = config->five_star;
		}
	}
	else {
		if (four_stars_times == 0) {
			prob->star5 = config->five_star + (pool->rate_increment) * (counts - pool->soft_pity_start);
			prob->star4 = 1 - prob->star5;
		}
		else{
			prob->star4 = config->four_star;
			prob->star5 = config->five_star + (pool->rate_increment) * (counts - pool->soft_pity_start);
		}
	}
	return prob;
}

//随机抽取
int random(double prob) {
	double r = (double)rand() / RAND_MAX;
	if (r < prob) return 1;
	else return 0;
}

int mult_random(double* prob, int size) {
	double r = (double)rand() / RAND_MAX;
	double sum = 0;
	for (int i = 0; i < size; i++) {
		sum += prob[i];
		if (r < sum) return i;
	}
	return size - 1;
}

//抽卡主函数
//return 3:3星 4:4星 5:5星
int Gacha(Probability* prob) {
	double sum = prob->star4 + prob->star5;
	int sum_random = random(sum);	//以总概率抽取
	if (sum_random == 1) {
		double five_random = prob->star5 / sum;
		int five_star = random(five_random);	//以5星概率抽取
		if (five_star == 1) return 5;	//返回5星
		else return 4;	//返回4星
	}
	else return 3;	//返回3星
}

Item* return_Item(GachaPool* pool, int stars, bool* up5star) {
	if (stars == 5) {
		if (up5star)
		{
			up5star = false;
			return pool->star5_up;
		}
		int if_up = random(0.5);
		if (if_up == 1) {
			Item* item = pool->star5_up;	//正常情况下，5星UP只有一个
			return item;
		}
		else {
			double* prob = malloc(pool->star5_others_count * sizeof(double));
			for (int i = 0; i < pool->star5_others_count; i++) {
				prob[i] = (double)pool->star5_others[i].weight / 10000;
			}
			int index = mult_random(prob, pool->star5_others_count);
			Item* item = &pool->star5_others[index];
			free(prob);
			up5star = true;
			return item;
		}
	}
	else if (stars == 4) {
		int if_up = random(0.5);
		if (if_up == 1) {
			double* prob = malloc(pool->star4_up_count * sizeof(double));
			for (int i = 0; i < pool->star4_up_count; i++) {
				prob[i] = (double)pool->star4_up[i].weight / 10000;
			}
			int index = mult_random(prob, pool->star4_up_count);
			Item* item = &pool->star4_up[index];
			free(prob);
			return item;
		}
		else {
			double* prob = malloc(pool->star4_others_count * sizeof(double));
			for (int i = 0; i < pool->star4_others_count; i++) {
				prob[i] = (double)pool->star4_others[i].weight / 10000;
			}
			int index = mult_random(prob, pool->star4_others_count);
			Item* item = &pool->star4_others[index];
			free(prob);
			return item;
		}
	}
	else {
		double* prob = malloc(pool->star3_others_count * sizeof(double));
		for (int i = 0; i < pool->star3_others_count; i++) {
			prob[i] = (double)pool->star3_others[i].weight / 10000;
		}
		int index = mult_random(prob, pool->star3_others_count);
		Item* item = &pool->star3_others[index];
		free(prob);
		return item;
	}
}

//解析抽卡结果
void parse_result(Item* item, FILE* fp) {
	fprintf(fp, "%s\n", item->name);
}




