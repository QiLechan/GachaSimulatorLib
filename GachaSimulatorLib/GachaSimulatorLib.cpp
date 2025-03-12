// GachaSimulatorLib.cpp : 定义静态库的函数。
//

#include "pch.h"
#include "framework.h"
#include <cjson/cJSON.h>

// 创建初始JSON配置
char* createGenshinJson()
{
	cJSON* root = cJSON_CreateObject();

	cJSON_AddItemToObject(root, "version", cJSON_CreateString("1.0.0"));
	cJSON_AddItemToObject(root, "description", "cJSON_CreateString("原神抽卡配置"));
}




