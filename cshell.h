#ifndef __CSHELL__
#define __CSHELL__

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "cshell_ascii_color.h"

#define CSH_CLI_BUFSIZ  256
#define CSH_PROMPT      CSH_COLOR_UCYN"CShell>"CSH_COLOR_RESET" "

#define STRING_COMPARE(str1,str2) \
    ((strlen(str1)==strlen(str2)) && (!memcmp(str1, str2, strlen(str1))))

enum CS_RET_CODE {
    CS_SUCCESS = 0,
    CS_FAILURE = 1,
};

typedef struct csh_rule_t {
    char *relation;
    char *desc;
    void *args;
    void *(*func)(void *);
} CshRule;

typedef struct csh_node_t {
    char            *name;
    char            *desc;
    unsigned short  num;
    unsigned short  depth;
    void            *args;
    void            *(*func)(void *);
    struct csh_node_t *parent;
    struct csh_node_t **children;
} CshNode;

CshNode *g_csh_root;

int csh_shell_init();
int csh_shell_add(CshRule *rule);
int _csh_create_child(CshNode *parent, char *token, CshRule *rule);
int csh_shell_run();
int _csh_exec_cmd(char ***token_list, int token_idx, int token_num, CshNode *ptr);
int _csh_cmd_tokenize(char ***token_list, char *cmd, char *delimiter);

void dump_csh_shell();
void _dump_csh_shell_r(CshNode *ptr);

#endif
