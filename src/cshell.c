#include "cshell.h"

int csh_shell_run()
{
    /* Main shell start from here. */
    int ret = CS_SUCCESS,
        token_num = 0;
    char c, cli_buf[CSH_CLI_BUFSIZ];
    u8 buf_idx = 0, buf_wait = 1;
    CshNode *curr = g_csh_root;
    char **token_list = NULL;
    printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
    printf("Thanks for using CShell in your project.\n");
    printf("    %10s: %s\n", "Author", "Kevin Cyu");
    printf("    %10s: %s\n", "Contact", "kevinbird61@gmail.com");
    printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
    while(1) {
        printf("%s", CSH_PROMPT);
        /* preconfig */
        memset(cli_buf, 0x0, CSH_CLI_BUFSIZ);
        buf_wait = 1;
        buf_idx = 0;
        /* get characters */
        while (buf_wait) {
            c = getc(stdin);
            switch (c) {
                case 0x0A:
                    // leave
                    buf_wait = 0;
                    break;
                default:
                    cli_buf[buf_idx] = c;
                    buf_idx++;
                    break;
            }
        }
        /* get command and tokenize */
        // printf("%s (%ld)\n", cli_buf, strlen(cli_buf));
        token_num = _csh_cmd_tokenize(&token_list, cli_buf, " ");
        /* exec */
        if (_csh_exec_cmd(&token_list, 0, token_num, g_csh_root) != CS_SUCCESS) {
            printf("Command '%s' is not support\n", cli_buf);
            /* TODO: Search any available/possible CLI and print out suggestion */
        }
    }
    printf("\nxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
    
    return ret;
}

int _csh_exec_cmd(
    char ***token_list,
    int token_idx,
    int token_num,
    CshNode *ptr
)
{
    int ret = CS_SUCCESS;
    char *token = (*token_list)[token_idx];
    
    //printf("[%s] token = %s, node = %s. (%d/%d)\n", __FUNCTION__, token, ptr->name, token_idx, token_num);

    if (STRING_COMPARE(token, ptr->name)) {
        // Match, search next
        if ((token_idx+1) < token_num) {
            if (ptr->num) {
                for (int i=0; i<ptr->num; i++) {
                    ret = _csh_exec_cmd(&(*token_list), token_idx+1, token_num, ptr->children[i]);
                    if (ret == CS_SUCCESS) {
                        // no need to search other children
                        break;
                    }
                }
            } else {
                // no other children, current token is not support
                // printf("[%s] Command ('%s') Not Support.\n", __FUNCTION__, token);
                ret = CS_FAILURE;
            }
        } else {
            //printf("[%s] Reach the end, exec it.\n", __FUNCTION__);
            if (ptr->func) {
                // TODO: arguments
                ptr->func(ptr->args);
            } else {
                printf("[%s] No function available.\n", __FUNCTION__);
                ret = CS_SUCCESS;
            }
        }
    } else {
        // Handle special case
        if (STRING_COMPARE(token, "?")) {
            printf("[%s][HELP] User type '?' to list CLI hint.\n", __FUNCTION__);
            printf("[%s][HELP] Current Depth: %d\n", __FUNCTION__, ptr->depth);
            ret = CS_SUCCESS;
        } else {
            // printf("[%s] Command ('%s') Not Support.\n", __FUNCTION__, token);
            ret = CS_FAILURE;
        }
    }

    return ret;
}

int _csh_cmd_tokenize(
    char ***token_list,
    char *cmd,
    char *delimiter
)
{
    /** @brief Tokenize the string into token list (double-pointer array)
     * 
     * @description:
     *  Caustion: token_list need to initialize to NULL before calling this function
     */

    int token_num = 0;
    char *token = NULL, *temp;

    temp = strdup(cmd);

    while ((token = strtok_r(temp, delimiter, &temp))) {
        //printf("[%s] %d - %s\n", __FUNCTION__, token_num, token);
        if (!(*token_list)) {
            (*token_list) = (char **)malloc(sizeof(char *));
            (*token_list)[token_num] = (char *)malloc(strlen(token)*sizeof(char));
        } else {
            (*token_list) = (char **)realloc((*token_list), (token_num + 1)*sizeof(char *));
            (*token_list)[token_num] = (char *)malloc(strlen(token)*sizeof(char));
        }
        memcpy((*token_list)[token_num], token, strlen(token));
        token_num++;
    }

    return token_num;
}

int csh_shell_init()
{
    int ret = CS_SUCCESS;

    g_csh_root = (CshNode *)malloc(sizeof(CshNode));
    if (!g_csh_root) {
        ret = CS_FAILURE;
        return ret;
    }
    g_csh_root->name = strdup("root");
    g_csh_root->desc = strdup("CShell's root node.");
    g_csh_root->num = 0;
    g_csh_root->depth = 0;
    g_csh_root->args = NULL;
    g_csh_root->func = NULL;
    g_csh_root->parent = NULL;
    g_csh_root->children = NULL;

    return ret;
}

int csh_shell_add(
    CshRule *rule
)
{
    /** @brief Parse the relation and create command list 
     * 
     * Relation format: "root,<Node1>,<Node1's children>,..."
     * 
     */
    int ret = CS_SUCCESS, token_num = 0;
    CshNode *it = g_csh_root;
    char *token = NULL, *relation = NULL;
    char **token_list = NULL;

    if (it) {
        // iterate relation string
        token_num = _csh_cmd_tokenize(&token_list, rule->relation, ",");

        if (!STRING_COMPARE(token_list[0], "root")) {
            // first node is not root
            ret = CS_FAILURE;
            return ret;
        }

        for (int i=1; i<token_num; i++) {
            // check if any child exist.
            if (!it->num) {
                // create first children
                _csh_create_child(it, token_list[i], rule);
            } else {
                // iterate all children
                u8  found = 0;
                for (int j=0; j<it->num; j++) {
                    if (STRING_COMPARE((it->children[j])->name, token_list[i])) {
                        // found, move "it" to this children
                        it = it->children[j];
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    // not found, create dummy node (this node will apply the same attribute of this rule)
                    _csh_create_child(it, token_list[i], rule);
                    // move "it" to this child (newest one)
                    it = (it->children[it->num-1]);
                }
            }
        }
    } else {
        ret = CS_FAILURE;
    }

    return ret;
}

int _csh_create_child(
    CshNode *parent,
    char *token,
    CshRule *rule
)
{
    int ret = CS_SUCCESS;
    u16 idx = parent->num;

    // printf("Parent: %s (%d), New: %s\n", parent->name, parent->num, token);

    parent->num++;
    parent->children = (CshNode **)realloc(parent->children, parent->num*sizeof(CshNode *));
    if (parent->children == NULL) {
        ret = CS_FAILURE;
        return ret;
    }
    parent->children[idx] = (CshNode *)malloc(sizeof(CshNode));
    if (parent->children[idx] == NULL) {
        ret = CS_FAILURE;
        return ret;
    }
    (parent->children[idx])->parent = parent;
    (parent->children[idx])->depth = (parent->depth + 1);
    (parent->children[idx])->name = malloc(strlen(token));
    if ((parent->children[idx])->name == NULL) {
        ret = CS_FAILURE;
        return ret;
    }
    memcpy((parent->children[idx])->name, token, strlen(token));
    (parent->children[idx])->desc = malloc(strlen(rule->desc));
    if ((parent->children[idx])->desc == NULL) {
        ret = CS_FAILURE;
        return ret;
    }
    memcpy((parent->children[idx])->desc, rule->desc, strlen(rule->desc));
    (parent->children[idx])->num = 0;
    (parent->children[idx])->args = rule->args;
    (parent->children[idx])->func = rule->func;
    (parent->children[idx])->children = NULL;

    return ret;
}

void dump_csh_shell()
{
    _dump_csh_shell_r(g_csh_root);
}

void _dump_csh_shell_r(CshNode *ptr)
{
    if (ptr) {
#if 0
        printf("\t%s: %s", ptr->name, ptr->desc);
        if(ptr->parent) {
            printf("(%s)\n", ptr->parent->name);
        } else {
            printf("\n");
        }
#else
        for (int i=0; i<ptr->depth; i++) {
            printf("    ");
        }
        printf("%s: %s\n", ptr->name, ptr->desc);
#endif
        for (int i=0; i<ptr->num; i++) {
            _dump_csh_shell_r(ptr->children[i]);
        }
    }
}