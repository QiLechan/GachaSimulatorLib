#pragma execution_character_set("utf-8")
#include <cjson/cJSON.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "GachaSimulatorLib.h"

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

	//卡池
	cJSON* pools = cJSON_AddArrayToObject(root, "pools");
	cJSON* pool_template = cJSON_CreateObject();
	cJSON_AddItemToObject(pool_template, "pool_name", cJSON_CreateString("常驻祈愿"));
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
	cJSON_AddStringToObject(sample_5star, "name", "Sample 5★ Character");
	cJSON_AddNumberToObject(sample_5star, "id", 10001);
	cJSON_AddNumberToObject(sample_5star, "weight", 5000);	//5星权重 总权重10000
	cJSON_AddItemToArray(five_stars, sample_5star);

	//UP以外
	cJSON* others = cJSON_AddObjectToObject(pool_template, "others");
	five_stars = cJSON_AddArrayToObject(others, "5star");
	four_stars = cJSON_AddArrayToObject(others, "4star");

	//UP以外示例
	sample_5star = cJSON_CreateObject();
	cJSON_AddStringToObject(sample_5star, "name", "Sample 5★ Character");
	cJSON_AddNumberToObject(sample_5star, "id", 10001);
	cJSON_AddNumberToObject(sample_5star, "weight", 5000);	//5星权重 总权重10000
	cJSON_AddItemToArray(five_stars, sample_5star);

	cJSON_AddItemToArray(pools, pool_template);

	char* json = cJSON_Print(root);
	cJSON_Delete(root);
	return json;
}

// 解析UP物品
UpItem* parse_up_items(cJSON* array, int* count) {
	if (!cJSON_IsArray(array)) return NULL;

	int size = cJSON_GetArraySize(array);
	UpItem* items = malloc(size * sizeof(UpItem));
	*count = 0;

	cJSON* item;
	cJSON_ArrayForEach(item, array) {
		if (cJSON_IsObject(item)) {
			UpItem up = { 0 };

			cJSON* name = cJSON_GetObjectItem(item, "name");
			if (cJSON_IsString(name))
                strncpy_s(up.name, sizeof(up.name), name->valuestring, sizeof(up.name) - 1);

			cJSON* id = cJSON_GetObjectItem(item, "id");
			if (cJSON_IsNumber(id)) up.id = id->valueint;

			cJSON* weight = cJSON_GetObjectItem(item, "weight");
			if (cJSON_IsNumber(weight)) up.weight = weight->valueint;

			items[(*count)++] = up;
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
		pool.star5_up = parse_up_items(star5, &pool.star5_count);

		cJSON* star4 = cJSON_GetObjectItem(up, "4star");
		pool.star4_up = parse_up_items(star4, &pool.star4_count);
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




