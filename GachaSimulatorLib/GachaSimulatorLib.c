#include <cjson/cJSON.h>
#include <string.h>
#include <stdlib.h>

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




