#pragma execution_character_set("utf-8")  
#include <stdio.h>  
#include <stdlib.h>  
#include <GachaSimulatorLib.h>  
#include <Windows.h>  
#include <time.h>
#include <stdbool.h>

int main()  
{  
    SetConsoleOutputCP(CP_UTF8);
    time_t t;
    srand((unsigned)time(&t));
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
    GachaConfig* config = parse_config(json);  
    free(json);
    int i = 1;
    FILE* fp2;
    fopen_s(&fp2, "result.txt", "wb");
    int five_star_count = 0;
    bool* up5star = false;      // 大保底机制
    for (int counts = 1; counts <= 1000; counts++)
    {
        Probability* prob = probability(i, &config->global, &config->pools[0]);
        i++;
        int result = Gacha(prob);
        Item* item = return_Item(&config->pools[1], result, up5star);
        if (result == 5)
        {
            i = 1;
            five_star_count++;
        }
        parse_result(item, fp2);
        free(prob);
    }  
    printf("五星总数：%d\n", five_star_count);
    fclose(fp2);
    fclose(fp);  
}
