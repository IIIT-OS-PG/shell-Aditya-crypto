#include<stdio.h>
#include<cstring>
#include<string.h>
#include<stack>
#include<sys/wait.h>
#include<iostream>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<pwd.h>
#include<fcntl.h>
#include<map>
#include<sys/stat.h>
#define clear() printf("\033[H\033[J")
#define WRITE 1
#define READ 0

using namespace std;
//map<string,string> mp;
void mybash()
{   char buf[1024];
   uid_t uid=getuid();
   struct passwd * mypwdstr=getpwuid(uid);
   FILE* fd;
   fd=fopen("myrc.txt","w");
   string user=mypwdstr->pw_name;
   string home=mypwdstr->pw_dir;
   if(gethostname(buf,sizeof(buf))<0)
      perror("hostname access permission denied");
  fprintf(fd,"%s\n",mypwdstr->pw_name);
  fprintf(fd,"%s\n",mypwdstr->pw_dir);
  fprintf(fd,"%s\n",buf);
   fclose(fd);
   cout<<user<<"@"<<buf<<":~$";
}
//// read history.txt file
void show_history()
{
    char buf[1024];
    
    
    FILE* fd;
    char final_path[]="/home/phenix-fire/Documents/OS_Shell/hstry.txt";
     fd=fopen(final_path,"r");
    //fd=open(final_path,O_CREAT | O_WRONLY,0600);
    while(!feof(fd)){
    
    fread(buf,sizeof(buf),1,fd);
    printf("%s",buf);
    }
    //write(fd,&c,sizeof(c));
    fclose(fd);
}
///// execute commands
int exe_pr(char *in)
{  char dir[]="cd";
   char hstry[]="history";
   
   char *arg[1024];
   char*arg_p=in;
   char* cmd=strtok(arg_p," ");
   arg[0]=cmd;
  //cout<<arg[0];
 if(strcmp(arg[0],hstry)==0)
       {
         show_history();
        exit(1);
       }
 
  int i=0;
  while(cmd!= NULL)
  {  
      cmd=strtok(NULL, " ");  
      arg[++i]=cmd;
  }
   arg[++i]=NULL;
    if(strcmp(arg[0],dir)==0)
       {
         chdir(arg[1]);
         return 1;
       }
   if(execvp(arg[0],arg)<0)
   {
    perror("command not found");
    exit(1);
    }
}
/////// handling with pipes
void pipe_handle(char**inp,int i)
  {      //cout<<"hello";
          int pipe1[2];
          pid_t pi;
         int k=0;
         int f=0;
         int ii = 0;
         while(ii < i)
        {   pipe(pipe1);
           pi=fork();
          if(pi<0)
            perror("fork(): error");
          else if(pi==0)
          {  dup2(f,READ);
             if(i-1 != ii)
                dup2(pipe1[WRITE],WRITE);
             close(pipe1[WRITE]);
             close(pipe1[READ]);
             exe_pr(inp[ii]);
          }
          else
          {
            wait(NULL);
            ii++;
            //close(pipe1[READ]);
            close(pipe1[WRITE]);
            f=pipe1[READ];
            //close(pipe1[READ]);
          }
       }
  }
//// add history
void add_hstry(string str)
{
    char buf[1024];
    strcpy(buf,str.c_str());
    char c='a';
    //int fd;
    FILE* fd;
    char final_path[]="/home/phenix-fire/Documents/OS_Shell/hstry.txt";
     fd=fopen(final_path,"a+");
    //fd=open(final_path,O_CREAT | O_WRONLY,0600);
    fprintf(fd,"%s\n",buf);
    //write(fd,&c,sizeof(c));
    fclose(fd);
}
///// execute redirection
void exe_ind(char* first,char* second,int flag)
{   int fd;
    if(flag==1)
    fd=open(second,O_CREAT|O_WRONLY|O_TRUNC);
    else
    fd=open(second,O_CREAT|O_WRONLY|O_APPEND);
    dup2(fd,1);
    char *fs_in[1024];   
    char* cmd=strtok(first,"|");
    fs_in[0]=cmd;
      int i=0;
      while(cmd!= NULL)
      {  
        cmd=strtok(NULL, "|");  
        fs_in[++i]=cmd;
      }
      fs_in[++i]=NULL;
      pid_t pid=fork();
      if(pid<0)
         { 
            perror("fork(): error");
         }
      else if(pid==0)
           {  
               
               if(!strstr(first,"|"))
                   {  
                      exe_pr(fs_in[0]);
                   }
               else 
                  {
                   //cout << "Entering pipe \n" ;
                    pipe_handle(fs_in,i-1);
                  }
           }
      else
          {
             wait(NULL);
             
          }
    close(fd);
}
void ind(char* inp)
{   int flag=0;
   if(strstr(inp,">"))
   {  flag=1;
      char *arg[1024];
      char*arg_p=inp;
      char* cmd=strtok(arg_p,">");
      arg[0]=cmd;
      int i=0;
     while(cmd!= NULL)
      {  
        cmd=strtok(NULL, ">");  
        arg[++i]=cmd;
      }
        arg[++i]=NULL;
        exe_ind(arg[0],arg[1],flag);
   }
   if(strstr(inp,">>"))
   {
      char *arg[1024];
      char*arg_p=inp;
      char* cmd=strtok(arg_p,">>");
      arg[0]=cmd;
      int i=0;
     while(cmd!= NULL)
      {  
        cmd=strtok(NULL, ">>");  
        arg[++i]=cmd;
      }
        arg[++i]=NULL;
        exe_ind(arg[0],arg[1],flag);
   }
   
}
//// shell driver proram
int main()
{   char ex[]="exit";
    char clr[]="clear";
    clear();
    
while(1)
{
  mybash();
  string str;
  getline(cin,str);
  add_hstry(str);
  char in[1024],in_d[1024];
  char *p=in;
  char *p1=in_d;
 /* if(mp.find(str))
      { string temp=mp[str];
        strcpy(in,temp.c_str());
        strcpy(in_d,temp.c_str());
      }
   else{*/
  strcpy(in,str.c_str());
  strcpy(in_d,in);
  //}
  char *fs_in[1024];   
  char* cmd=strtok(p,"|");
  if(strcmp(cmd,ex)==0)
       exit(1);
  if(strcmp(cmd,clr)==0)
       clear();   
  fs_in[0]=cmd;
  //if(!add_alias(fs_in[0]))
  // { //cout<<fs_in[0];
      int i=0;
      while(cmd!= NULL)
      {  
        cmd=strtok(NULL, "|");  
        fs_in[++i]=cmd;
      }
      fs_in[++i]=NULL;
  
      pid_t pid=fork();
      if(pid<0)
         { 
            perror("fork(): error");
         }
      else if(pid==0)
           {  
               
               if(!strstr(in_d,"|"))
                   {  
                      exe_pr(fs_in[0]);
                   }
               else 
                  {
                   //cout << "Entering pipe \n" ;
                    pipe_handle(fs_in,i-1);
                  }
           }
      else
          {
             wait(NULL);
             
          }
          //break;
    }
 //}
    return 0;
}
