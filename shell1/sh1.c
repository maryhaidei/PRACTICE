#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

#define m 20 

long long op=0, start=0; 

char *input_str(char *c){
    long long i=0; 
    char* str= malloc(m);
    if(!str) return NULL; 
    while(*c && *c!= EOF && *c!='\n' && !strchr(" &|;.><()",*c)){
        if(*c=='"'){
            *c=getchar(); 
            while(*c!='"'){ 
                if(*c=='\n'){
                    free(str);
                    printf("You entered the wrong number of quotes!\n"); 
                    return NULL; 
                }  
                str[i++]=*c;
                if (i%m==0) str=realloc(str,(i+m));
                *c=getchar(); 
            }   
        *c=getchar();
        continue; 
        }
        str[i++]=*c;
        if (i%m==0) str=realloc(str,(i+m));
        *c=getchar(); 
    }
        if(strlen(str)==0){
            if(strchr("&|;.><()", *c)){
                str[0]=*c; 
                str[1]='\0'; 
                if(*c==')') op--; 
                if( *c=='(') {
                    op++;
                    //start=i; 
                }
                i++;  
            }else {
                free(str);
                str=input_str(c);
            }
        }else{ 
            if(i%m==0) str=realloc(str, i+m); 
            str[i]='\0'; 
        }
    return str;
}

void input(char** str){ 
    char c; 
    long long i=0; 
     while((c=getchar())!=EOF){ 
        if(!op) start=1;
        if((op!=0) && (c=='\n')){
            printf("You entered wrong amount of brackets!"); 
            for(int j=i-1; j>=start; j++) free(str[j]); 
            i=start; 
            op=0;
        }
        if(c==' ' || c=='\n') continue;
        if(strchr("&|><", c) && str[i-1][0]==c && str[i-1][1]=='\0'){ 
            str[i-1][1]=c; 
            str[i-1][2]='\0'; 
            continue;
        }
        str[i]=input_str(&c); 
        i++; 
        if(i%m==0) str=realloc(str, sizeof(char*)*(m+i)); 
        if( str[i-1]==NULL || (c=='\n' && op)){
            printf("You entered wrong amount of brackets or quotes!\n"); 
            for(int j=i-1; j>=start; j++) free(str[j]); 
            i=start; 
            op=0; 
            continue;
        }
        if(c=='\n') start=0; 
        if(strchr("&|;.><()", c)){
            if(str[i-1][0]!=c){
                str[i]=malloc(3);
                str[1][0]=c;  
                str[i][1]='\0'; 
                i++;  
                if (i%m==0) str=realloc(str,(i+m)*sizeof(char *));
                if(c==')') op--; 
                if( c=='(') {
                    op++;
                    start=i; 
                }
            }
        }
    }
    str[i]=NULL;     
}

void del(char** str){
    long i=0; 
    while(str[i]) {
        free(str[i]); 
        i++; 
    }
    free(str);
}

void print(char** str){
    long long i=0;
    while(str[i]){
        printf("%s\n", str[i]);
        i++;
    }
}

int main(){
    char** str =malloc(sizeof(char*)*m); 
    if(!str) return 0; 
    input(str); 
    print(str); 
    del(str);
    return 0; 
}
