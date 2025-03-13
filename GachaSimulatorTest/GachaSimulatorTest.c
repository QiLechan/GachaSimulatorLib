#include <stdio.h>
#include <stdlib.h>
#include <GachaSimulatorLib.h>

int main()
{
	FILE* fp;
	if (fopen_s(&fp, "Genshin.json", "rb") != 0)
	{
		char* buffer = (char*)malloc(1024 * sizeof(char));
		createGenshinJson(buffer);
		FILE* fw;
		if (fopen_s(&fw, "Genshin.json", "wb") == 0)
		{
			fwrite(buffer, sizeof(char), strlen(buffer), fw);
			free(buffer);
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
	GachaConfig* config = calloc(1, sizeof(GachaConfig));
	cJSON* versions = cJSON_GetObjectItem(root, "version");
	char* version = cJSON_Print(versions);
	printf("%s\n", version);
	free(json);
	fclose(fp);
}
