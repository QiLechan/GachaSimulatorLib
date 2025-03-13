#pragma execution_character_set("utf-8")
#include <cjson/cJSON.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "GachaSimulatorLib.h"

// 创建原神初始JSON配置
void createGenshinJson(char* buffer)
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

	cJSON_AddItemToArray(pools, pool_template);

	char* json = cJSON_Print(root);
	int size = sizeof(json);
	strcpy_s(buffer, strlen(json) + 1, json);
	cJSON_Delete(root);
}

// 解析全局配置
GlobalConfig parse_global_config(cJSON* global) {
	GlobalConfig config = { 0 };

	if (global) {
		cJSON* item = cJSON_GetObjectItem(global, "global_config");
		if (cJSON_IsNumber(item)) config.max_pity_counter = item->valueint;

		item = cJSON_GetObjectItem(global, "5star");
		if (cJSON_IsNumber(item)) config.five_star = item->valuedouble;

		item = cJSON_GetObjectItem(global, "4star");
		if (cJSON_IsNumber(item)) config.four_star = item->valuedouble;
	}
	return config;
}

// 解析UP数组
UpItem* parse_up_items(cJSON* array, int* count) {
	if (!cJSON_IsArray(array)) return NULL;

	int size = cJSON_GetArraySize(array);
	UpItem* items = calloc(size, sizeof(UpItem));
	*count = 0;

	cJSON* element;
	cJSON_ArrayForEach(element, array) {
		if (cJSON_IsObject(element)) {
			UpItem item = { 0 };

			cJSON* name = cJSON_GetObjectItem(element, "name");
			if (cJSON_IsString(name))
                strncpy_s(item.name, sizeof(item.name), name->valuestring, sizeof(item.name) - 1);

			cJSON* id = cJSON_GetObjectItem(element, "id");
			if (cJSON_IsNumber(id)) item.id = id->valueint;

			cJSON* weight = cJSON_GetObjectItem(element, "weight");
			if (cJSON_IsNumber(weight)) item.weight = weight->valuedouble;

			items[(*count)++] = item;
		}
	}
	return items;
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

}




