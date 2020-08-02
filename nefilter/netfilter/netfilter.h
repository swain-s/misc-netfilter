//
//  netfilter.h
//  nefilter
//
//  Created by kun shen on 2019/11/16.
//  Copyright © 2019 kun shen. All rights reserved.


/*  5个注册点
 NF_INET_PRE_ROUTING ---—-- <O> ----> NF_INET_FORWARD ----------->   NF_INET_POST_ROUTING
 完整性校验后，路由决策前       |      数据包到达另一个接口        ^
                            |                             <O>
                            |                              |
                        NF_INET_LOCAL_IN                 NF_INET_LOCAL_OUT
                    目标地址为本机的数据包               来自本地进程的数据包
                        |                              |
                        - - - - - - - - - -  - - - - -
                                    上层协议阀
 hook函数返回码：
 NF_DROP     --  丢弃数据包
 NF_ACCEPT   --  保留数据包   --> 到目前为止仍接收，将转移到下一层
 NF_STOLEN   --  忘掉数据包   --> 告诉netfliter忘掉数据包，完全属于钩子函数自己
 NF_QUEUE    --  让数据包在用户空间排队
 NF_REPEAT   --  再次调用这个钩子函数
 */

#ifndef netfilter_h
#define netfilter_h


//-----其他文件中需要定义的结构体(跳过)------
struct list_head_list{
    int a;
}

typedef int (*nf_hookfn)(unsigned int,
                 struct sk_buff **,
                 const struct net_device *,
                 const struct net_device *,
                 int (*okfn)(struct sk_buff *))

//------------------------------



struct nf_hook_ops{
    struct list_head list;
    nf_hookfn *hook;    //hook指向hook函数   netfilter.h定义了nf_hookfn
    int pf;             //指定协议簇         socket.h定义了可用协议簇
    int hooknum;        //指定钩子位置        IPV4->PF_INET
    int priority;       //指定运行时该钩子函数的执行顺序   可选NF_IP_PRI_FIRST
}

#endif /* netfilter_h */

