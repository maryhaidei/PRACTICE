#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

#define m 20

char** input(char** str){ 
    char  c; 
    long long i=0, j=0, e=0; 
    while((c=getchar())!=EOF){
        str[i]=malloc(m); 
        j=0; 
        while(c!='\n'){
            if(c==EOF) break; 
             if(c==' ' && j){
                if (i%m==0) str=realloc(str,(i+m)*sizeof(char*));
                str[i][j++]='\0'; 
                break; 
             }
            if(c==' '){
                c=getchar();
                continue;  
            } 
            if(strchr("&|><",c)){
                if(j){
                    str[i][j]='\0'; 
                    printf("%s\n", str[i++]); 
                    if (i%m==0) str=realloc(str,(i+m)*sizeof(char*));
                    str[i]=malloc(m); 
                }
                str[i][0]=c; 
                if((c=getchar())==str[i][0]){
                    str[i][1]=c; 
                    str[i][2]='\0'; 
                    //printf("%s\n", str[i]); 
                    j=2;
                    break; 
                }else{ 
                    str[i][1]='\0'; 
                    j=1;
                    if(c=='\n') break;
                    printf("%s\n", str[i++]); 
                    if (i%m==0) str=realloc(str,(i+m)*sizeof(char*));
                    str[i]=malloc(m); 
                    j=0;
                    continue;  
                }
            }else if(strchr(";()",c)){
                if(j>0){
                    //str[i][j]='\0'; 
                    printf("%s\n", str[i]); 
                    i++; 
                    if (i%m==0) str=realloc(str,(i+m)*sizeof(char*));
                    str[i]=malloc(m);
                }
                str[i][0]=c; 
                str[i][1]='\0';
                j=1;  
                break; 
            }
            if(c=='"'){
                if((c=getchar())=='"'){
                    c=getchar(); 
                    continue; 
                } 
                while(c!='"'){
                    str[i][j++]=c; 
                    if(j%m==0) str[i]=realloc(str[i], j+m); 
                    str[i][j]='\0'; 
                    c=getchar(); 
                }
                c=getchar(); 
                continue; 
            }
            str[i][j++]=c;
            if (j%m==0) str[i]=realloc(str[i],(j+m)*sizeof(char));
            str[i][j]='\0'; 
            c=getchar(); 
        }
        if(c=='\n' && !j) {
            free(str[i]);
            continue; 
        }   
        if(j && str[i][j-1]!='\0') str[i][j]='\0'; 
        printf("%s\n", str[i]); 
        i++;
       if (i%m==0) str=realloc(str,(i+m)*sizeof(char*));
        str[i]=NULL;    
        if(c==EOF) break;   
    }
    str[i]=NULL;
    return str; 
}
void del(char** str){
    long i=0; 
    while(str[i]) {
        free(str[i]); 
        i++; 
    }
    free(str);
}

int main(){ 
    char** str=malloc(m*(sizeof(char*))); 
    str=input(str); 
    del(str); 
    return 0; 
}