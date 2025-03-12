#include <stdio.h>
#include <stdlib.h>
#include <GachaSimulatorLib.h>

int main()
{
	char* buffer = (char*)malloc(1024 * sizeof(char));
	createGenshinJson(buffer);
	FILE* fp = fopen("Genshin.json", "rb");
	if (!fp)
	{
		FILE* fw = fopen("Genshin.json", "wb");
		fwrite(buffer, sizeof(char), strlen(buffer), fw);
		fclose(fw);
	}
}

