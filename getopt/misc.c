//
//  misc.c
//  TestGetOpt
//
//  Created by kun shen on 2020/8/2.
//  Copyright © 2020 kun shen. All rights reserved.
//

#include "misc.h"

/* *str_arr[] 字符串是常量 无法复制*/
unsigned int Split(char *str, const char *str_arr[])
{
    char curstr[100];
    unsigned int pcs = 0, pdsa = 0;
    
    for (int i = 0; i < strlen(str); i++){
        if (str[i] == ' '){
            strcpy(&str_arr[pdsa], curstr);
            pdsa ++;
            memset(curstr, 0, strlen(curstr)+1);
            pcs = 0;
        }else{
            curstr[pcs] = str[i];
            pcs ++;
            
            if (i == strlen(str)-1){
                strcpy(&str_arr[pdsa], curstr);
                return 0;
            }
        }
    }
    
    return -1;
}

unsigned int TestStrArr(void)
{
    char *str_arr[10];
    char *stra = "i am shen kun amd yes", *strb = "hello";
    
    str_arr[0] = stra;
    str_arr[1] = strb;
    printf("%s, %s\n", str_arr[0], str_arr[1]);
    
    printf("strlen: %u, %u, %u\n", strlen(str_arr), strlen(stra), strlen(strb));
    printf("sizeof: %u, %u, %u\n", sizeof(str_arr), sizeof(stra), sizeof(strb));
    
    memset(&str_arr, 0, 28);
    printf("%s, %s\n", str_arr[0], str_arr[1]);
    return 0;
}

unsigned TestGetOpt(void)
{
    /* 1 get input */
    char *str = (char *)malloc(100);
    while (gets(str) != NULL){
        printf("input: %s\n", str);
        
        /* 2 split */
        const char *args[100];
        Split(str, args);
        printf("split:");
        for (int i = 0; i < 4; i++){
            printf(" [%s] ", &args[i]);
        }
        printf("\n");
        
        /* 3 */
        int arg;
        printf("here\n");
        while ((arg = getopt(5,args,"a:b:cd"))!=-1)
        {
            printf("[%s]\n", arg);
            switch(arg)
            {
                case 'a':
                    printf("a: [%s] \n", optarg);
                    break;
                case 'b':
                    printf("b: [%s] \n",optarg);
                    break;
                case 'c':
                    printf("c: \n");
                    break;
                case 'd':
                    printf("d\n");
                    break;
                default:
                    printf("other option");
            }
        }
        printf("arg: %d", arg);
    }
    
    
    
    return 0;
}
