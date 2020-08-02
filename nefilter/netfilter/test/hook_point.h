//
//  hook_point.h
//  nefilter
//
//  Created by kun shen on 2019/11/16.
//  Copyright © 2019 kun shen. All rights reserved.
//

#ifndef hook_point_h
#define hook_point_h

typedef int (*ppp)(int, int);


typedef struct node {
    int date;
    ppp *ss;
    struct node* next;
}node;

#endif /* hook_point_h */
