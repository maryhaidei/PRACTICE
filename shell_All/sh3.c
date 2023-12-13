#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#define m 10

int pids[100]={0};
int pid,stat;


void process(char **s,long long unsigned l);

void del(char **a) {
    long long unsigned i=0;
    while (a[i]) free(a[i++]);
}

char** input(char** str){ 
    char  c; 
    int status; 
    long long i=0, j=0, e=0, p=0; 
    printf("meow>");
    while((c=getchar())!=EOF){
        if(c=='\n') printf("meow>");
        if(c=='\n' && p){
            printf("Wrong amount of ()!Try again!\n"); 
            del(str); 
            i=0; 
            str[i]=NULL; 
            p=0; 
            //printf("meow>");
            continue; 
        } 
        if (c=='\n' && !p) {
		str[i]=NULL;
		process(str, i); 
		del(str); 
		i=0;
		str[i]=NULL;
		//printf("meow>");
        //printf("meow>");
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
            if(strchr("&|",c)){
                if(j){
                    str[i][j]='\0'; 
                    //printf("%s\n", str[i++]); 
                    i++; 
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
                    i++; 
                    if (i%m==0) str=realloc(str,(i+m)*sizeof(char*));
                    str[i]=malloc(m); 
                    j=0;
                    continue;  
                }
            }else if(strchr(";()<>",c)){
                if(j>0){
                    //printf("%s\n", str[i]); 
                    str[i][j]='\0';
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
            //printf("meow>");
            continue; 
        }   
        if(j && str[i][j-1]!='\0') str[i][j]='\0'; 
        //printf("%s\n", str[i]); 
        i++;
        if (i%m==0) str=realloc(str,(i+m)*sizeof(char*));
        str[i]=NULL;    
        if(c=='\n' && j && !p){
            //if(c=='\n') printf("meow>");
            process(str, i); 
            del(str); 
            i=0; 
            str[i]=NULL; 
            printf("meow>");
            continue;
        }else if(c=='\n' && p){
            printf("Wrong amount of ()!Try again!\n"); 
            del(str); 
            i=0; 
            str[i]=NULL; 
            p=0; 
        }
        //if(c=='\n') printf("meow>");
        if(c==EOF) break; 
        //printf("meow>");  
    }
    if(c=='\n') printf("meow>");
    str[i]=NULL;
    return str; 
}

void process(char **a,long long unsigned l) {
    char **cmd=malloc(l*sizeof(char*));
    long long unsigned i=0,t=0, pair=0;
    int status, isp=1,fd[2]={0},cp[2]={dup(0),dup(1)};
    while (a[i]) {
    if(a[i][0]=='(' && isp==0){ 
        i++; 
        pair=1; 
        while(pair){
            if(a[i][0]=='(') pair++; 
            if(a[i][0]==')') pair--; 
            i++; 
        }
        dup2(cp[0], 0); 
        dup2(cp[1], 1); 
        fd[0]=0; 
        fd[1]=1; 

    }
	else if (t==0 && !strcmp(a[i],"cd")) {
	    if (isp) {
	    if ((a[i+1] && strchr(";&|",a[i+1][0])) || !a[i+1]) chdir(getenv("HOME"));
	    else chdir(a[i+1]);
	    }
	    isp=1;
	    i= a[i+1] ? i+2 : i+1;
	    continue;
	}
	else if (strchr("><",a[i][0])) {
		if (a[i][0]=='<') fd[0]=open(a[i+1],O_RDONLY);
		if (a[i][0]=='>') fd[1]=open(a[i+1], O_WRONLY | O_CREAT | (a[i][1]? O_APPEND : O_TRUNC), 0777);
		if (fd[0]==-1 || fd[1]==-1) {
		    fprintf(stderr,"Error: no such file in directory or invalid syntax\n");
		    if (fd[0]>0) close(fd[0]);
		    if (fd[1]>0) close(fd[1]);
		    isp=0;
		    break;
		}
		i=a[i+1]? i+2 : i+1;
		continue;
	    }
	else if (!strchr("&|;()",a[i][0])) cmd[t++]=a[i];
	else {
	    cmd[t]=NULL;
	    if (isp==0) {
		if (strchr("><",a[i][0]) && a[i+1]) {
		    i++;
		    if (a[i+1] && strchr("><",a[i+1][0])) i= a[i+2] ? i+2 : i+1;
		}
		if (!strcmp(a[i],"|")) {
		    while (a[i] && !strchr("&;><",a[i][0]) && strcmp(a[i],"||")) i++;
		}
		t=0;
		isp=1;
		if (a[i]) i++;
		continue;
	    }
	    
	    if (!strcmp(a[i],"|")) {
		while (1) {
		    t=0;
		    pipe(fd);
		    if (fork()==0) {
			if (a[i]!=NULL) dup2(fd[1],1);
			close(fd[0]);
			close(fd[1]);
			execvp(cmd[0],cmd);
			execv(cmd[0],cmd);
			perror("Error: unavailable command\n");
			exit(1);
		    }
		    dup2(fd[0],0);
		    close(fd[0]);
		    close(fd[1]);
		    if (!a[i] || strcmp(a[i],"|")) break;
		    while (a[i+1] && strchr("&|;><()",a[i+1][0])==NULL) {
			if (t==0 && !strcmp(a[i+1],"cd")) {
			i++;
			if (isp) {
			    if ((a[i+1] && strchr(";&|",a[i+1][0])) || !a[i+1]) chdir(getenv("HOME"));
			    else chdir(a[i+1]);
			}
			isp=1;
			i= a[i+1] ? i+2 : i+1;
			continue;
			}
			cmd[t++]=a[++i];
		    }
		    cmd[t]=NULL;
		    i++;
		}
		
		close(fd[1]);
		while (wait(&status)!=-1);
		if (a[i]) {
		    if (!strcmp(a[i],"&&")) isp=WIFEXITED(status);
		    if (!strcmp(a[i],"||")) isp=WIFEXITED(status)? 0 : 1;
		    i++;
		}
		
		dup2(cp[1],1);
		dup2(cp[0],0);
		fd[0]=0;
		fd[1]=0;
		cmd[0]=NULL; t=0;
		continue;
	    }
	    

		
	    if (fd[0]) {dup2(fd[0],0);close(fd[0]);}	
	    if (fd[1]) {dup2(fd[1],1);close(fd[1]);}
	    if (!strcmp("&",a[i])) {
		int ff=open("/dev/null",O_RDONLY);
		dup2(ff,0);
		close(ff);
	    }
	    
	    if ((pid=fork())) {
		if (strcmp("&",a[i])!=0) waitpid(pid,&status,0);
		else 
		    for (int j=0;j<m*m;j++)
			if (pids[j]==0) {pids[j]=pid;break;}
	    }
	    else {
		execvp(cmd[0],cmd);
		execv(cmd[0],cmd);
		perror("Error: unavailable command\n");
		exit(1);
	    }
	    t=0;
	    
	    dup2(cp[0],0);
	    dup2(cp[1],1);
	    fd[0]=0;
	    fd[1]=0;
	    if (!strcmp(a[i],"&&")) isp=WIFEXITED(status);
	    if (!strcmp(a[i],"||")) isp=WIFEXITED(status)? 0 : 1;
	}
	i+=(int)(a[i]!=NULL);
    } 
    if(pair)
    cmd[t]=NULL;
    if (!isp || cmd[0]==NULL) {
	free(cmd);
	for (int j=0;j<m*m;j++) 
	if (pids[j] && waitpid(pids[j],&stat,WNOHANG)!=0) {
	    if (WIFEXITED(stat)) printf("Process %d exited with code %d\n",pids[j],WEXITSTATUS(stat));
	    else printf("Process %d aborted with signal %d\n",pids[j],WTERMSIG(stat));
	    pids[j]=0;
	}
	return;
    }
    
    if (fd[0]) {dup2(fd[0],0);close(fd[0]);}	
    if (fd[1]) {dup2(fd[1],1);close(fd[1]);}
	
    if ((pid=fork())) waitpid(pid,&status,0);
    else {
	execvp(cmd[0],cmd);
	execv(cmd[0],cmd);
	perror("Error: unavailable command\n");
	exit(1);
    }
    
    dup2(cp[0],0);
    dup2(cp[1],1);
    fd[0]=0;
    fd[1]=0;
    free(cmd);
    for (int j=0;j<m*m;j++)
    if (pids[j] && waitpid(pids[j],&stat,WNOHANG)!=0) {
	if (WIFEXITED(stat)) printf("Process %d exited with code %d\n",pids[j],WEXITSTATUS(stat));
	else printf("Process %d aborted with signal %d\n",pids[j],WTERMSIG(stat));
	pids[j]=0;
    }
}




int main() {
    signal(SIGINT,SIG_IGN);
    char **a=malloc(10*sizeof(char *));
    a=input(a);
    //output(a);
    //arr_free(a);
    free(a);
    for (int j=0;j<100;j++)
    if (pids[j] && waitpid(pids[j],&stat,WNOHANG)!=0) {
	if (WIFEXITED(stat)) printf("Process %d exited with code %d\n",pids[j],WEXITSTATUS(stat));
	else printf("Process %d aborted with signal %d\n",pids[j],WTERMSIG(stat));
	pids[j]=0;
    }
    return 0;
}
