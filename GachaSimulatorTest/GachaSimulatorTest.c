#pragma execution_character_set("utf-8")
#include <stdio.h>
#include <stdlib.h>
#include <GachaSimulatorLib.h>
#include <Windows.h>

int main()
{
	SetConsoleOutputCP(CP_UTF8);
	FILE* fp;
	if (fopen_s(&fp, "Genshin.json", "rb") != 0)
	{
		char* genshinjson = createGenshinJson();
		FILE* fw;
		if (fopen_s(&fw, "Genshin.json", "wb") == 0)
		{
			fwrite(genshinjson, sizeof(char), strlen(genshinjson), fw);
			free(genshinjson);
			fclose(fw);
		}
		fopen_s(&fp, "Genshin.json", "rb");
	}
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char* json = (char*)malloc(size * sizeof(char));
	fread(json, sizeof(char), size, fp);
	printf("%s\n", json);
	cJSON* root = cJSON_Parse(json);
	if (!root) {
		fprintf(stderr, "JSON解析错误: %s\n", cJSON_GetErrorPtr());
		return NULL;
	}
	GachaConfig* config = parse_config(json);
	if (config) {
		printf("版本: %s\n", config->version);
		printf("描述: %s\n", config->description);
		printf("全局配置: 大保底 %d, 5星 %.3f, 4星 %.3f\n", config->global.max_pity_counter, config->global.five_star, config->global.four_star);
		for (int i = 0; i < config->pool_count; i++) {
			GachaPool pool = config->pools[i];
			printf("卡池: %s, 类型: %s, 5星保底 %d, 5星递增 %.3f\n", pool.pool_name, pool.pool_type, pool.soft_pity_start, pool.rate_increment);
			printf("5星UP:\n");
			for (int j = 0; j < pool.star5_count; j++) {
				UpItem item = pool.star5_up[j];
				printf("名称: %s, ID: %d, 权重: %d\n", item.name, item.id, item.weight);
			}
			printf("4星UP:\n");
			for (int j = 0; j < pool.star4_count; j++) {
				UpItem item = pool.star4_up[j];
				printf("名称: %s, ID: %d, 权重: %d\n", item.name, item.id, item.weight);
			}
		}
	}
	free(json);
	fclose(fp);
}
