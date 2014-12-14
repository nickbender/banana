
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <wordexp.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <pwd.h>
#include "include/List.h"
#include "include/Node.h"
#include "include/Object.h"
#include "include/common.h"

void trim_leading_and_trailing_whitespace(char*);

int check_for_ampersand(char*);

int check_version(const char*);

int expand_and_execute(char*, ListPtr);

void update_jobs_status(ListPtr);

void trim_trailing_whitespace(char* string){

  size_t len = strlen(string)+1;         

  char* end_of_string = string + len;

  while(string < end_of_string  && isspace(*end_of_string)){
    --end_of_string;
  }

  *end_of_string = '\0';
}

int check_for_ampersand(char* string){
  char last_char = string[strlen(string)-1];
  if(last_char == '&'){
    string[strlen(string)-1] = '\0';
    return 1;
  }
  else
    return 0;
}

int check_version(const char* arg1) {
  wordexp_t res;
  if(strcmp(arg1, "-v") == 0) {
    char *git_version = "git rev-list --count HEAD";
    switch (wordexp (git_version, &res, 0)){
      case 0:
        break;
      case WRDE_NOSPACE:
        wordfree (&res);
      default:
        return -1;
    }
    execvp (res.we_wordv[0], res.we_wordv);
    exit(0);
  }
  return 0;
}

int expand_and_execute (char *program, ListPtr jobslist)
{
  wordexp_t result;
  pid_t pid;
  int status, background;

  trim_trailing_whitespace(program);
  background = check_for_ampersand(program);


  switch (wordexp (program, &result, 0)){
    case 0:
      break;
    case WRDE_NOSPACE:
      wordfree (&result);
    default:
      return -1;
  }
  if (result.we_wordv[0] != NULL) { 
    if (strcmp(result.we_wordv[0], "exit") == 0) {
      status = -2;
    }
    else if (strcmp(result.we_wordv[0], "jobs") == 0){
      update_jobs_status(jobslist);
    }
    else if (strcmp(result.we_wordv[0], "cd") == 0) {
      if (result.we_wordv[1] == NULL){
        struct passwd *pws;
        pws = getpwuid(geteuid());
        chdir(pws->pw_dir);
      }
      else
        chdir(result.we_wordv[1]);
    } 
    else{
      pid = fork();
      if (pid == 0) {
        execvp (result.we_wordv[0], result.we_wordv);
        printf("my_dash: %s: command not found\n", result.we_wordv[0]);
        exit (EXIT_FAILURE);
      }
      else if (pid < 0)
        status = -1;
      else  {     
        if (background == 1){
          ObjectPtr jobyjob = createObject(pid, result.we_wordv[0]);
          addAtRear(jobslist, createNode(jobyjob));
        }
        else if (waitpid (pid, &status, 0) != pid)
          status = -1;

      }
    }
  }
  wordfree (&result);
  return status;
}

void update_jobs_status(ListPtr list){
  NodePtr curr = list->head;
  int status;
  while(curr != NULL){
    pid_t job = ((ObjectPtr)curr->obj)->key;
    job = waitpid(job, &status, WNOHANG);

    if(job != 0){
      printf("%s Job finished: %d\n", ((ObjectPtr)curr->obj)->data, job);
      removeNode(list, curr);
    }else {
      printf("%d Job: %d is still running.\n", ((ObjectPtr)curr->obj)->key, job);
    }
    curr = curr->next;
  }
}
  
int main(int argc, char *argv[]) {

  if(argc>1) {
    check_version(argv[1]);
  }
  ListPtr jobslist = createList(compareTo, toString, freeObject);

  char *line;
  char *prompt = "dash>";
  char *env_name = "DASH_PROMPT";
  char *env_prompt = getenv(env_name);
  if(env_prompt != NULL)
    prompt = env_prompt;
  int status;

  using_history();
  while ((line=readline(prompt))) {
    add_history(line);
    status = expand_and_execute(line, jobslist);
    free(line);
    if(status == -2) exit(0);
  }
  freeList(jobslist);
  exit(0);

}


