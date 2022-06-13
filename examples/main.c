#include "cshell.h"

void *root_help(void *args)
{
    printf("[%s]\n", __FUNCTION__);
    printf("Demonstrate what will happen if user type 'root help' within Cshell.\n");
    
    return NULL;
}

static CshRule cli_rules[] = {
    /* root.help */
    {
        .relation = "root,help",
        .desc = "Print help message of Cshell.",
        .args = NULL,
        .func = root_help
    },
    {
        .relation = "root,help,dummy",
        .desc = "Dummy node for 'help' command.",
        .args = NULL,
        .func = NULL
    },
    /* root.show */
    {
        .relation = "root,show",
        .desc = "Dummy show command of Cshell.",
        .args = NULL,
        .func = NULL
    },
    /* root.long.examine */
    {
        .relation = "root,long,examine",
        .desc = "Dummy.",
        .args = NULL,
        .func = NULL
    },
};

int main(int argc, char *argv[])
{
    /* Step 1 - Init our csh shell */
    csh_shell_init();

    /* Step 2 - Append those CLI rules */
    for (int i=0; i<(sizeof(cli_rules)/sizeof(cli_rules[0])); i++) {
        csh_shell_add(&cli_rules[i]);
    }

    /* (Optional) Print out the generated tree structure */
    //dump_csh_shell();

    /* Run our CLI with generated CLI command ! */
    csh_shell_run();

    return 0;
}
