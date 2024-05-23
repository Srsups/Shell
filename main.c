#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>

#define MAX_ARGS 512
#define MAX_HISTORY 1000
#define MAX_INPUT_BUFFER 1024
#define MAX_CMD_EXEC 100
#define MAX_CWD 1024
#define MAX_HISTORY_FILE 3000

int pipe_count=0, fd;
static char* args[512];
char input_buffer[MAX_INPUT_BUFFER];
char *cmd_exec[MAX_CMD_EXEC];
char *history_file;
char input_buffer[1024];
char *cmd_exec[100];
int flag, len;
char cwd[1024];
int flag_pipe=1;
pid_t pid;
int no_of_lines;
int environmment_flag;
int flag_pipe, flag_without_pipe,  output_redirection, input_redirection;
int bang_flag;
int pid, status;
char history_data[1000][1000];
char current_directory[1000];
char ret_file[3000];
char his_var[2000];
char *input_redirection_file;
char *output_redirection_file;
extern char** environ;

void clear_variables();
void fileprocess ();
void filewrite();
void bang_execute();
void environmment();
void set_environment_variables();
void change_directory();
void parent_directory();
void echo_calling(char *echo_val);
void history_execute_with_constants();
static char* skipwhite(char* s);
void tokenise_commands(char *com_exec);
void tokenise_redirect_input_output(char *cmd_exec);
void tokenise_redirect_input(char *cmd_exec);
void tokenise_redirect_output(char *cmd_exec);
char* skipcomma(char* str);
static int split(char *cmd_exec, int, int, int);
void with_pipe_execute();
static int command(int, int, int, char *cmd_exec);
void prompt();
void sigintHandler(int sig_num);

void sigintHandler(int sig_num)
{
    signal(SIGINT, sigintHandler);
    fflush(stdout);
}

// Função para limpar variáveis globais
void clear_variables()
{
  fd =0;
  flag=0;
  len=0;
  no_of_lines=0;
  pipe_count=0;
  flag_pipe=0;
  flag_without_pipe=0;
  output_redirection=0;
  input_redirection=0;
  input_buffer[0]='\0';
  cwd[0] = '\0';
  pid=0;
  environmment_flag=0;
  bang_flag=0;
}
  
// Função para processar o arquivo de histórico  
void fileprocess ()
{
  int fd;
  history_file=(char *)malloc(100*sizeof(char));
  strcpy(history_file,current_directory);
  strcat(history_file, "/");
  strcat(history_file, "history.txt"); 
  fd=open(history_file, O_RDONLY|O_CREAT,S_IRUSR|S_IWUSR);
  int bytes_read=0, i=0, x=0, index=0;
  char buffer[1], temp_data[1000];
    do 
    {
        bytes_read = read (fd, buffer, sizeof (buffer));
        for (i=0; i<bytes_read; ++i) 
                  {
                    temp_data[index]=buffer[i];
                    index++;
                    if(buffer[i]=='\n')
                        {
                          temp_data[index-1]='\0';
                          no_of_lines++; 
                          index=0;
                          strcpy(history_data[x], temp_data);
                          x++;
                          temp_data[0]='\0';
                        }
                  }
    }
    while (bytes_read == sizeof (buffer)); 
    close (fd); 
}

// Função para escrever no arquivo de histórico
void filewrite()
{
  
  int fd_out,ret_write,str_len=0;
  char input_data[2000];
  no_of_lines++;
  char no[10];
  sprintf (no, "%d", no_of_lines ); 
  strcpy(input_data, " ");
  strcat(input_data, no);
  strcat(input_data, " ");
  strcat(input_data, input_buffer);

  str_len = strlen(input_data);
  fd_out=open(history_file,O_WRONLY|O_APPEND|O_CREAT,S_IRUSR|S_IWUSR);
  len=strlen(input_buffer);
  ret_write=write(fd_out,input_data,str_len);
  if(ret_write<0) 
        {
          printf("Erro ao escrever o arquivo\n");
          return;
        }
  free(history_file);
  close(fd_out);

}

// Função para executar comandos "bang" (!)
void bang_execute()
{
  char bang_val[1000];
  char *tokenise_bang[100], *num_ch[10];
  int i, n=1, num, index=0;
  i=1;
  if(input_buffer[i]=='!')
        {
           strcpy(bang_val, history_data[no_of_lines-1]);
        }
  else if(input_buffer[i]=='-')
    {
        n=1;
        num_ch[0]=strtok(input_buffer,"-");
        while ((num_ch[n]=strtok(NULL,"-"))!=NULL)
              n++;
        num_ch[n]=NULL;
        num = atoi(num_ch[1]);

        index = no_of_lines-num;
        strcpy(bang_val, history_data[index]);
              
    }
  else 
    {
      num_ch[0]=strtok(input_buffer,"!");
      num = atoi(num_ch[0]);
      strcpy(bang_val, history_data[num-1]);
    }
  tokenise_bang[0]=strtok(bang_val," ");
  while ((tokenise_bang[n]=strtok(NULL,""))!=NULL)
              n++;
  tokenise_bang[n]=NULL;
  strcpy(bang_val, tokenise_bang[1]);
  printf("%s\n", bang_val );
  strcpy(input_buffer, bang_val);

    
}

// Função para lidar com variáveis de ambiente
void environmment()
{
  int i =1, index=0;
  char env_val[1000], *value;
  while(args[1][i]!='\0')
              {
                   env_val[index]=args[1][i];
                   index++;
                    i++;
              }
  env_val[index]='\0';
  value=getenv(env_val);

  if(!value)
      printf("\n");
  else printf("%s\n", value);

  free(value);
}

// Função para definir variáveis de ambiente
void set_environment_variables()
{  
int n=1;
char *left_right[100];
if(args[1]==NULL)
      {
        char** env;
          for (env = environ; *env != 0; env++)
          {
            char* value = *env;
            printf("declare -x %s\n", value);    
          }  
          free(env);
         return; 
      }
left_right[0]=strtok(args[1],"=");
while ((left_right[n]=strtok(NULL,"="))!=NULL)
      n++;
left_right[n]=NULL;
setenv(left_right[0], left_right[1], 0);
free(*left_right);
}

// Função para mudar de diretório
void change_directory()
{
char *h="/home";   
if(args[1]==NULL)
        //chdir(h);
        return;
else if ((strcmp(args[1], "~")==0) || (strcmp(args[1], "~/")==0))
        chdir(h);
else if(chdir(args[1])<0)
    printf("s(hell): cd: %s: Diretório ou arquivo inexistente ou permissão negada.\n", args[1]);

}

// Função para imprimir o diretório atual
void parent_directory()
{
if (getcwd(cwd, sizeof(cwd)) != NULL)
        {
         printf("%s\n", cwd );
        }
else
       perror("getcwd() error");
}

// Função para o comando echo
void echo_calling(char *echo_val)
{
  int i=0, index=0;
  environmment_flag=0;
  char new_args[1024],env_val[1000], *str[10];
  str[0]=strtok(echo_val," ");
  str[1]=strtok(NULL, "");
  strcpy(env_val, args[1]);
  if(str[1]==NULL)
         {
          printf("\n");
          return;
         } 
  if (strchr(str[1], '$')) 
                  {
                  environmment_flag=1;
                  }
  
  memset(new_args, '\0', sizeof(new_args));
  i=0; 
  while(str[1][i]!='\0')
    {
      if(str[1][i]=='"')
      {
      index=0;     
      while(str[1][i]!='\0')
          {
          if(str[1][i]!='"')
                {
                new_args[index]=str[1][i];
                 flag=1;
                index++;
                }
          i++;
          }             
      }
      else if(str[1][i]==39)
      {
      index=0;
      while(str[1][i]!='\0')
          {
          if(str[1][i]!=39)
                {
                new_args[index]=str[1][i];
                 flag=1;
                index++;
                }
          i++;
          }               
      }
      else if(str[1][i]!='"')
        {
          new_args[index]=str[1][i];
          index++;
          i++;
        }
      else i++;    
    }


new_args[index]='\0';
if((strcmp(args[1], new_args)==0)&&(environmment_flag==0))
    printf("%s\n", new_args);
else {
     strcpy(args[1], new_args);
      if(environmment_flag==1)
                {
                environmment();
                }
      else if(environmment_flag==0)
                {
                  printf("%s\n", new_args ); 
                }
    }  
}

// Função para executar o histórico
void history_execute_with_constants()
{
  int num, i, start_index;
  if(bang_flag==1)
        {
         for(i=0; i<no_of_lines; i++)
            printf("%s\n", history_data[i] ); 
        }
  else if(args[1]==NULL)
      {
        for(i=0; i<no_of_lines-1; i++)
            printf("%s\n", history_data[i] );
        printf(" %d %s\n", no_of_lines, his_var );
      }
  else
      {
        if(args[1]!=NULL)
        num = atoi(args[1]);
        if(num>no_of_lines) 
        {
          for(i=0; i<no_of_lines-1; i++)
            printf("%s\n", history_data[i] );
          printf(" %d %s\n", no_of_lines, his_var );
        }
        start_index=no_of_lines-num;
        for(i=start_index; i<no_of_lines-1; i++)
            printf("%s\n", history_data[i] );
          printf(" %d %s\n", no_of_lines, his_var );
      } 

}

// Função para remover espaços em branco
static char* skipwhite(char* s)
{
  while (isspace(*s)) ++s;
  return s;
}

// Função para dividir comandos em argumentos
void tokenise_commands(char *com_exec)
{
int m=1;
args[0]=strtok(com_exec," ");       
while((args[m]=strtok(NULL," "))!=NULL)
        m++;
}

// Função para dividir comandos com redirecionamento de entrada e saída
void tokenise_redirect_input_output(char *cmd_exec)
{
  char *io_token[100];
  char *new_cmd_exec1;  
  new_cmd_exec1=strdup(cmd_exec);
  int m=1;
  io_token[0]=strtok(new_cmd_exec1,"<");       
  while((io_token[m]=strtok(NULL,">"))!=NULL)
        m++;
  io_token[1]=skipwhite(io_token[1]);
  io_token[2]=skipwhite(io_token[2]);
  input_redirection_file=strdup(io_token[1]);
  output_redirection_file=strdup(io_token[2]);
  tokenise_commands(io_token[0]);
  free(*io_token);
  free(new_cmd_exec1);
}

// Função para dividir comandos com redirecionamento de entrada
void tokenise_redirect_input(char *cmd_exec)
{
  char *i_token[100];
  char *new_cmd_exec1;  
  new_cmd_exec1=strdup(cmd_exec);
  int m=1;
  i_token[0]=strtok(new_cmd_exec1,"<");       
  while((i_token[m]=strtok(NULL,"<"))!=NULL)
        m++;
  i_token[1]=skipwhite(i_token[1]);
  input_redirection_file=strdup(i_token[1]);
  tokenise_commands(i_token[0]);
}

// Função para dividir comandos com redirecionamento de saída
void tokenise_redirect_output(char *cmd_exec)
{
  char *o_token[100];
  char *new_cmd_exec1;  
  new_cmd_exec1=strdup(cmd_exec);
  int m=1;
  o_token[0]=strtok(new_cmd_exec1,">");       
  while((o_token[m]=strtok(NULL,">"))!=NULL)
          m++;
  o_token[1]=skipwhite(o_token[1]);
  output_redirection_file=strdup(o_token[1]); 
  tokenise_commands(o_token[0]);   
  
}

// Função para remover aspas
char* skipcomma(char* str)
{
  int i=0, j=0;
  char temp[1000];
  while(str[i++]!='\0')
            {
              if(str[i-1]!='"')
                    temp[j++]=str[i-1];
            }
        temp[j]='\0';
        str = strdup(temp);
  
  return str;
}

// Função para dividir e executar comandos
static int split(char *cmd_exec, int input, int first, int last)
{
    char *new_cmd_exec1;  
    new_cmd_exec1=strdup(cmd_exec);
      {
        int m=1;
        args[0]=strtok(cmd_exec," ");       
        while((args[m]=strtok(NULL," "))!=NULL)
              m++;
        args[m]=NULL;
        if (args[0] != NULL) 
            {

            if (strcmp(args[0], "exit") == 0) 
                    exit(0);
            if (strcmp(args[0], "echo") != 0) 
                    {
                      cmd_exec = skipcomma(new_cmd_exec1);
                      int m=1;
                      args[0]=strtok(cmd_exec," ");       
                      while((args[m]=strtok(NULL," "))!=NULL)
                                m++;
                      args[m]=NULL;

                    }
            if(strcmp("cd",args[0])==0)
                    {
                    change_directory();
                    return 1;
                    }
            else if(strcmp("pwd",args[0])==0)
                    {
                    parent_directory();
                    return 1;
                    }
           
            }
        }
    return command(input, first, last, new_cmd_exec1);
}

// Função para executar comandos com pipe
void with_pipe_execute()
{

int i, n=1, input, first;

input=0;
first= 1;

cmd_exec[0]=strtok(input_buffer,"|");

while ((cmd_exec[n]=strtok(NULL,"|"))!=NULL)
      n++;
cmd_exec[n]=NULL;
pipe_count=n-1;
for(i=0; i<n-1; i++)
    {
      input = split(cmd_exec[i], input, first, 0);
      first=0;
    }
input=split(cmd_exec[i], input, first, 1);
input=0;
return;

}

// Função para executar o comando cat
void execute_cat()
{
    char buffer[1024];
    int bytes_read;
    for (int i = 1; args[i] != NULL; i++)
    {
        int fd = open(args[i], O_RDONLY);
        if (fd < 0)
        {
            fprintf(stderr, "cat: %s: Não existe tal arquivo ou diretório.\n", args[i]);
            continue;
        }
        while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0)
        {
            write(1, buffer, bytes_read);
            fprintf(stderr, "\n");
        }
        close(fd);
    }
}

//Executa os comandos separados por '&' concorrentemente
void execute_concurrent_commands(char *input) {
    char *concurrent_cmds[MAX_CMD_EXEC];
    int n = 1;

    concurrent_cmds[0] = strtok(input, "&");
    while ((concurrent_cmds[n] = strtok(NULL, "&")) != NULL)
        n++;
    concurrent_cmds[n] = NULL;

    for (int i = 0; i < n; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            char *cmd = strdup(concurrent_cmds[i]);
            tokenise_commands(cmd);
            if (args[0] != NULL) {
                if (strcmp("cd", args[0]) == 0) {
                    chdir(args[1]);
                    exit(0);
                } else if (strcmp("pwd", args[0]) == 0) {
                    if (getcwd(cwd, sizeof(cwd)) != NULL) {
                        printf("%s\n", cwd);
                    } else {
                        perror("getcwd() error");
                    }
                    exit(0);
                } else {
                    if (execvp(args[0], args) < 0)
                        printf("%s: comando não encontrado\n", args[0]);
                }
            }
            exit(0);
        }
    }

    for (int i = 0; i < n; i++) {
        wait(NULL);
    }
}

void execute_commands_from_file(char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("fopen");
        return;
    }
    
    char line[MAX_INPUT_BUFFER];
    while (fgets(line, sizeof(line), file) != NULL) {
        // Remove a nova linha no final do comando, se houver
        line[strcspn(line, "\n")] = 0;
        
        // Ignora linhas vazias
        if (line[0] == '\0')
            continue;
        
        // Processa e executa o comando da linha
        strcpy(input_buffer, line);
        
        // Lógica de execução de comandos existente
        if (strchr(input_buffer, '&')) {
            execute_concurrent_commands(input_buffer);
        } else {
            // fileprocess();
            // filewrite();
            with_pipe_execute();
        }
    }

    fclose(file);
}

// /// @brief Função para executar os comandos dados em um arquivo.
// void execute_commands_from_file() {
//   printf(args[1],"\n");
//   FILE *file = fopen(args[1], "r");
//     if (file == NULL) {
//         perror("fopen");
//         exit(EXIT_FAILURE);
//     }
    
//     char inserted_command[1024];
//     while (fgets(inserted_command, sizeof(inserted_command), file) != NULL) {
//         // Remove a nova linha no final do comando, se houver
//         inserted_command[strcspn(inserted_command, "\n")] = 0;
//         printf("\n");
//         printf(inserted_command);
//         printf("\n");
//         if (inserted_command[0] == '\n') 
//             continue;
//         len = strlen(inserted_command);
//         inserted_command[len - 1] = '\0';

//         if (strchr(inserted_command, '&')) {
//             execute_concurrent_commands(inserted_command);
//         } else {
//             fileprocess();
//             filewrite();
//             with_pipe_execute();
//         }  // ate aqui

//     }

//     fclose(file);
// }
// Função para executar comandos
static int command(int input, int first, int last, char *cmd_exec)
{
    int mypipefd[2], ret, input_fd, output_fd;
    ret = pipe(mypipefd);
    if(ret == -1)
    {
        perror("pipe");
        return 1;
    }
    pid = fork();

    if (pid == 0)   
    {
        // Execução do processo filho
        if (first == 1 && last == 0 && input == 0) 
        {
            dup2(mypipefd[1], 1);
        } 
        else if (first == 0 && last == 0 && input != 0) 
        {
            dup2(input, 0);
            dup2(mypipefd[1], 1);
        } 
        else 
        {
            dup2(input, 0);
        }

        // Tratamento de redirecionamento de entrada e saída (já existente)

        if (strchr(cmd_exec, '<') && strchr(cmd_exec, '>')) 
        {
            input_redirection = 1;
            output_redirection = 1;
            tokenise_redirect_input_output(cmd_exec);
        }
        else if (strchr(cmd_exec, '<')) 
        {
            input_redirection = 1;
            tokenise_redirect_input(cmd_exec);
        }
        else if (strchr(cmd_exec, '>')) 
        {
            output_redirection = 1;
            tokenise_redirect_output(cmd_exec);
        }

        if (output_redirection == 1)
        {
            output_fd = creat(output_redirection_file, 0644);
            if (output_fd < 0)
            {
                fprintf(stderr, "Falha em abrir %s para escrever\n", output_redirection_file);
                return(EXIT_FAILURE);
            }
            dup2(output_fd, 1);
            close(output_fd);
            output_redirection = 0;
        }
        if (input_redirection == 1)
        {
            input_fd = open(input_redirection_file, O_RDONLY, 0);
            if (input_fd < 0)
            {
                fprintf(stderr, "Falha em abrir %s para ler\n", input_redirection_file);
                return(EXIT_FAILURE);
            }
            dup2(input_fd, 0);
            close(input_fd);
            input_redirection = 0;
        }

        if (strcmp(args[0], "export") == 0)
        {
            set_environment_variables();
            return 1;
        }
        if (strcmp(args[0], "echo") == 0)
        {
            echo_calling(cmd_exec);
        }
        else if (strcmp(args[0], "history") == 0)
        {
            history_execute_with_constants();
        }
        // else if (strcmp(args[0], "path") == 0) 
        // {
        //     for (char **env = environ; *env != 0; env++) 
        //     {
        //         if (strncmp(*env, "PATH=", 5) == 0) 
        //         {
        //             printf("%s\n", *env + 5);
        //             break;
        //         }
        //     }
        // }
        else if (strcmp(args[0], "path") == 0) 
{
    if (args[1] != NULL) {
        // Verifica se o diretório especificado existe e se tem permissão de leitura
        struct stat sb;
        if (stat(args[1], &sb) == 0 && S_ISDIR(sb.st_mode) && access(args[1], R_OK) == 0) {
            // O diretório existe e pode ser lido
            for (char **env = environ; *env != 0; env++) 
            {
                if (strncmp(*env, "PATH=", 5) == 0) 
                {
                    printf("%s\n", *env + 5);
                    break;
                }
            }
        } else {
            // O diretório não existe ou não pode ser acessado
            fprintf(stderr, "Erro: O diretório especificado não existe ou não pode ser acessado.\n");
        }
    } else {
        // Se args[1] não for especificado, apenas retorna o PATH como antes
        for (char **env = environ; *env != 0; env++) 
        {
            if (strncmp(*env, "PATH=", 5) == 0) 
            {
                printf("%s\n", *env + 5);
                break;
            }
              }
          }
        }

        else if (strcmp(args[0], "cat") == 0)
        {
            execute_cat();
        }
        else if (strcmp(args[0], "batch") == 0)
        {
          execute_commands_from_file(args[1]);
        }
        else if (execvp(args[0], args) < 0) 
        {
            printf("%s: comando não encontrado\n", args[0]);
        }
        exit(0);
    }
    else 
    {
        waitpid(pid, 0, 0);  
    }

    if (last == 1)
        close(mypipefd[0]);
    if (input != 0) 
        close(input);
    close(mypipefd[1]);
    return mypipefd[0];
}

//Pega o prompt de comando digitado pelo usuário
void prompt()
{
  char shell[1000];
   if (getcwd(cwd, sizeof(cwd)) != NULL)
        {
          strcpy(shell, "s(hell):");
          strcat(shell, cwd);
          strcat(shell, "$ ");

          printf("%s", shell);
        }
   else
       perror("getcwd() error");

}


int main() {
    getcwd(current_directory, sizeof(current_directory));
    signal(SIGINT, sigintHandler);
    while (1) {
        clear_variables();
        prompt(); 
        fgets(input_buffer, MAX_INPUT_BUFFER, stdin); 
        if (input_buffer[0] == '\n') 
            continue;
        len = strlen(input_buffer);
        input_buffer[len - 1] = '\0';

        if (strchr(input_buffer, '&')) {
            execute_concurrent_commands(input_buffer);
        } else {
            fileprocess();
            filewrite();
            with_pipe_execute();
        }  // ate aqui
    }
    return 0;
}
