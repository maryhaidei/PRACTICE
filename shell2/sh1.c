#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h> 
#include <ctype.h>

#define m 20  

void del(char** str){
    long i=0; 
    while(str[i]) {
        free(str[i]); 
        i++; 
    }
    //free(str);
}

void process( char** s){
    int status; 
    pid_t pid; 
    if(strcmp(s[0], "cd")==0 && !s[1]){
        chdir(getenv("HOME")); 
    }else if(strcmp(s[0], "cd")==0){
        chdir(s[1]); 
    }else{
        pid=fork(); 
        if(pid==0){
            execvp(s[0], s); 
            perror("Error with execvp!"); 
            exit(1); 
        }else if(pid<0){
            perror("Error with fork!"); 
        }else{
            wait(&status); 
        }
    } 
    

}

char** input(char** str){ 
    char  c; 
    int status; 
    long long i=0, j=0, e=0, p=0; 
    while((c=getchar())!=EOF){
        if(c=='\n' && p){
            printf("Wrong amount of ()!Try again!\n"); 
            del(str); 
            i=0; 
            str[i]=NULL; 
            p=0; 
            continue; 
        }
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
                    //printf("%s\n", str[i++]); 
                    if (i%m==0) str=realloc(str,(i+m)*sizeof(char*));
                    str[i]=malloc(m); 
                }
                str[i][0]=c; 
                if((c=getchar())==str[i][0]){
                    str[i][1]=c; 
                    str[i][2]='\0'; 
                    j=2;
                    break; 
                }else{ 
                    str[i][1]='\0'; 
                    j=1;
                    if(c=='\n') break;
                    //printf("%s\n", str[i++]); 
                    if (i%m==0) str=realloc(str,(i+m)*sizeof(char*));
                    str[i]=malloc(m); 
                    j=0;
                    continue;  
                }
            }else if(strchr(";()",c)){
                if(j>0){
                    //printf("%s\n", str[i]); 
                    i++; 
                    if (i%m==0) str=realloc(str,(i+m)*sizeof(char*));
                    str[i]=malloc(m);
                }
                str[i][0]=c; 
                str[i][1]='\0';
                j=1;  
                if(c=='(') p++; 
                if(c==')') p--; 
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
        //printf("%s\n", str[i]); 
        i++;
        if (i%m==0) str=realloc(str,(i+m)*sizeof(char*));
        str[i]=NULL;    
        if(c=='\n' && j && !p){
            process(str); 
            del(str); 
            i=0; 
            str[i]=NULL; 
        }else if(c=='\n' && p){
            printf("Wrong amount of ()!Try again!\n"); 
            del(str); 
            i=0; 
            str[i]=NULL; 
            p=0; 
        }
        if(c==EOF) break;   
    }
    str[i]=NULL;
    return str; 
}


int main(){ 
    char** str=malloc(m*(sizeof(char*))); 
    str=input(str); 
    free(str); 
    //del(str); 
    return 0; 
}