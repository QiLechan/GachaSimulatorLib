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
	int *count = 0;
	UpItem* up = parse_up_items(root, count);
	printf("Max Pity Counter: %d\n", config.max_pity_counter);
	printf("5 Star Prob: %f\n", config.five_star);
	printf("4 Star Prob: %f\n", config.four_star);
	printf("Up Item: %s\n", up->name);
	free(json);
	free(up);
	fclose(fp);
}
