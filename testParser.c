#include <string.h>
#include <stdio.h>
#include <string.h>
 
char * json(char * fichier, char *variable){
    char * flag = strstr(fichier,variable);
    char *value;
    char caracterIndice=0;
    char demarrage = 0;
    char fin = 0;
    char e;
    for(int i=strlen(variable);i<strlen(flag) && !fin;i++){
        e = flag[i];
        if(e=='=' || e==':'){
            demarrage = 1;
        } else if(e==';'){
            fin = 1;
        }else if ((e!=' ' || e!='\n') && demarrage){
            value[caracterIndice++]=e;
        }
    }
    value[caracterIndice++]='\0';
    return value;
}

int main()
{
   char * fichier = "{\ntest = 36;\n}";
   char * fichier2;
   fichier2 = json(fichier,"test");
   printf("nouveau test\n");
   printf("%s\n", fichier2);
}