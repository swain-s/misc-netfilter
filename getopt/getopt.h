//
//  getopt.h
//  TestGetOpt
//
//  Created by kun shen on 2020/8/2.
//  Copyright © 2020 kun shen. All rights reserved.
//

#ifndef getopt_h
#define getopt_h

#include <stdio.h>

typedef struct ArgNode{
    char arg;
    int type;
    union {
        int i_value;
        char *s_value;
    };
    struct ExecNode *next;
}Arg_n;

/* s1 init pattern tree */
/* s2 analysis argvs and create arg list*/
typedef int (* pfunc)(Arg_n);

typedef struct PtnNode{
    char arg;
    char hasv;
    pfunc exec_func;
    
    struct OptNode *next_opt;
    struct OptNode *peer_opt;
}Ptn_n;



#endif /* getopt_h */
