//
//  return_fn_p.c
//  nefilter
//
//  Created by kun shen on 2019/11/16.
//  Copyright © 2019 kun shen. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "hook_point.h"

int fn(int a, int b)
{
    if(a>b)return 100;
    else{return 200;}
}


int test_main()
{
    int x=22, y=33;
    int z = 0;
    
    ppp ff;
    
    ff = (ppp)malloc(sizeof(ppp));
    
    ff = &fn;
    
    //node* pn = (node*) malloc(sizeof(node));
    //node n = {5, (*p), NULL};
    //*pn = n;
    
    z = (*ff)(x, y);
    
    printf("%d\n", z);
    
    
    return 0;
}
