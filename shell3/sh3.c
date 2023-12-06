#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h> 
#include <ctype.h>
#include <fcntl.h>
#include <signal.h>

#define m 20  

int status=0, pids[m*m]={0}, pid=0, stat=0; 


void del(char** str){
    long i=0; 
    while(str[i]) {
        free(str[i]); 
        i++; 
    }
    //free(str);
} 

/*int conv(char** s){
    int fd[2], i=0; 
    while(s[i]){
        if(strcmp(s[i], "|")!=0){
            pipe(fd); 
            switch(fork()){
                case -1:
                        fprintf(stderr, "Some errosr with fork conveyor!\n");
                        return 1;  
                case 0: if(s[1+i]!=NULL) dup2(fd[1], 1); 
                        close(fd[0]); 
                        close(fd[1]); 
                        execvp(s[i], &(s[i])); 
                        perror("Error with execvp!"); 
                        exit(1); 
            }
        dup2(fd[0], 0); 
        close(fd[0]); 
        close(fd[1]); 
        }
    }
    while(wait(NULL)!=-1); 
} */

/*int is_conv(char**s){ 
    long long i=0; 
    while(s[i]){
        if(s[i]=="|") return i;  
    }
    return -1; 
}*/

void process( char** s, long long len){ 
    char** cmd=malloc(len*sizeof(char*)); 
    int status, fd[2], cp[2]; 
    pid_t pid; 
    long long i=0, k=1, t=0; 
    while(s[i]){
        if(strcmp(s[0], "cd")==0 && !s[1]){
            chdir(getenv("HOME")); 
            k=1; 
            if(s[i]) i+=2; 
            else i++; 
        }else if(strcmp(s[0], "cd")==0){
            chdir(s[1]); 
        }else if(strchr("<>", s[i][0])){
            if(s[i]=="<") fd[0]=open(s[i+1], O_RDONLY); 
            else fd[1]=open(s[i+1], O_RDONLY | O_CREAT | (s[i][1]? O_APPEND : O_TRUNC), 0666);
            if(fd[1]==-1 || fd[0]==-1) {
                fprintf(stderr, "No such file in directory or file can not be created!\n"); 
                if(fd[0]) close(fd[0]); 
                if(fd[1]) close(fd[1]); 
                if(s[i+1]) i+=2; 
                else i++; 
                break;
            }
        }else if (!strchr("&|;()",s[i][0])) cmd[t++]=s[i];
        else {
	        cmd[t]=NULL;
            
	    if (k==0) {
		if (strchr("><",s[i][0]) && s[i+1]) {
		    i++;
		    if (s[i+1] && strchr("><",s[i+1][0])) {
                if(s[i+2]) i+=2; 
                else i++;
            } 
		}
		if (!strcmp(s[i],"|")) {
		    while (s[i] && !strchr("&;><",s[i][0]) && strcmp(s[i],"||")) i++;
		}
		t=0;
		k=1;
		if (s[i]) i++;
		continue;
	    }
	    
	    if (!strcmp(s[i],"|")) {
		while (1) {
		    t=0;
		    pipe(fd);
		    switch(fork()){
                    case -1:
                            fprintf(stderr, "Some errors with fork in conveyor!\n");
                            exit(1) ;  
                    case 0: if(s[i]!=NULL) dup2(fd[1], 1); 
                            close(fd[0]); 
                            close(fd[1]); 
                            execvp(cmd[0], cmd); 
                            execv(cmd[0], cmd); 
                            perror("Error with execvp!"); 
                            exit(1); 
            }
		    dup2(fd[0],0);
		    close(fd[0]);
		    close(fd[1]);
		    if (!s[i] || strcmp(s[i],"|")) break;
		    while (s[i+1] && strchr("&|;><()",s[i+1][0])==NULL) {
			if (t==0 && !strcmp(s[i+1],"cd")) {
			i++;
			if (k) {
			    if ((s[i+1] && strchr(";&|",s[i+1][0])) || !s[i+1]) chdir(getenv("HOME"));
			    else chdir(s[i+1]);
			}
			k=1;
			if(s[i+1]) i+=2; 
                else i++; 
			continue;
			}
			cmd[t++]=s[++i];
		    }
		    cmd[t]=NULL;
		    i++;
		}
		
		close(fd[1]);
		while (wait(&status)!=-1);
		if (s[i]) {
		    if (!strcmp(s[i],"&&")) k=WIFEXITED(status);
		    if (!strcmp(s[i],"||")) {
                if(WIFEXITED(status))k= 0;
                else k=1; 
            }
		    i++;
		}
		dup2(cp[1],1);
		dup2(cp[0],0);
		fd[0]=0;
		fd[1]=0;
		cmd[0]=NULL; t=0;
		continue;
	    }
	    if (fd[0]) {
            dup2(fd[0],0);
            close(fd[0]);
        }	
	    if (fd[1]) {
            dup2(fd[1],1);
            close(fd[1]);
        }
	    if (!strcmp("&",s[i])) {
		int ff=open("/dev/null",O_RDONLY);
		dup2(ff,0);
		close(ff);
	    }
	    
	    if ((pid=fork())) {
		if (strcmp("&",s[i])!=0) waitpid(pid,&status,0);
		else 
		    for (int j=0;j<100;j++)
			if (pids[j]==0) 
            {pids[j]=pid;
            break; 
        }
	    }
	    else {
		execvp(cmd[0],cmd);
        execv(cmd[0], cmd); 
		perror("Error with execvp!n");
		exit(1);
	    }
	    t=0;
	    
	    dup2(cp[0],0);
	    dup2(cp[1],1);
	    fd[0]=0;
	    fd[1]=0;
	    if (!strcmp(s[i],"&&")) k=WIFEXITED(status);
	    if (!strcmp(s[i],"||")){
            if(WIFEXITED(status))k= 0;
            else k=1; 
        }
	}
    i+=(int)(s[i]!=NULL);
    }
    cmd[t]=NULL;
    if (!k || cmd[0]==NULL) {
	free(cmd);
	for (int j=0;j<m*m;j++) 
	if (pids[j] && waitpid(pids[j],&stat,WNOHANG)!=0) {
	    if (WIFEXITED(stat)) printf("Process %d exit code %d\n",pids[j],WEXITSTATUS(stat));
	    else printf("Process %d aborted with signal %d\n",pids[j],WTERMSIG(stat));
	    pids[j]=0;
	}
	return;
    }
    if (fd[0]) {
        dup2(fd[0],0);
        close(fd[0]);
    }	
    if (fd[1]) {
        dup2(fd[1],1);
        close(fd[1]);
    }
	
    if ((pid=fork())) waitpid(pid,&status,0);
    else {
	execvp(cmd[0],cmd);
    execv(cmd[0], cmd); 
	perror("Unavailable command! Error with execvp!\n");
	exit(1);
    }
    
    dup2(cp[0],0);
    dup2(cp[1],1);
    fd[0]=0;
    fd[1]=0;
    free(cmd);
    for (int j=0;j<m*m;j++)
    if (pids[j] && waitpid(pids[j],&stat,WNOHANG)!=0) {
	if (WIFEXITED(stat)) printf("Process %d exit code %d\n",pids[j],WEXITSTATUS(stat));
	else printf("Process %d aborted with signal %d\n",pids[j],WTERMSIG(stat));
	pids[j]=0;
    } 
}




char** input(char** str){ 
    char  c; 
    int status; 
    long long i=0, j=0, e=0, p=0; 
    printf("meow>");
    while((c=getchar())!=EOF){
        if(c=='\n' && p){
            printf("Wrong amount of ()!Try again!\n"); 
            del(str); 
            i=0; 
            str[i]=NULL; 
            p=0; 
            printf("meow>");
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
            printf("meow>");
            continue; 
        }   
        if(j && str[i][j-1]!='\0') str[i][j]='\0'; 
        //printf("%s\n", str[i]); 
        i++;
        if (i%m==0) str=realloc(str,(i+m)*sizeof(char*));
        str[i]=NULL;    
        if(c=='\n' && j && !p){
            process(str, i); 
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
        printf("meow>");  
    }
    str[i]=NULL;
    return str; 
}


int main(){ 
    signal(SIGINT, SIG_IGN);
    char** str=malloc(m*(sizeof(char*))); 
    str=input(str); 
    free(str); 
    for (int j=0;j<m*m;j++)
    if (pids[j] && waitpid(pids[j],&stat,WNOHANG)!=0) {
	if (WIFEXITED(stat)) printf("Process %d exit code %d\n",pids[j],WEXITSTATUS(stat));
	else printf("Process %d aborted with signal %d\n",pids[j],WTERMSIG(stat));
	pids[j]=0;
    }
    //del(str); 
    return 0; 
} 

