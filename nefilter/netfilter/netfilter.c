#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/netfilter.h>

#include <linux/netfilter_ipv4.h>



static struct nf_hook_ops hook_ops1, hook_ops2, hook_ops3;
/*
struct nf_hook_ops{
    struct list_head list;
    nf_hookfn *hook;    //hook指向hook函数   netfilter.h定义了nf_hookfn
    int pf;             //指定协议簇         socket.h定义了可用协议簇
    int hooknum;        //指定钩子位置        IPV4->PF_INET
    int priority;       //指定运行时该钩子函数的执行顺序   可选NF_IP_PRI_FIRST
}
*/

//情景一： 丢弃所有包

unsigned int hook_func1(unsigned int hooknum,
                       struct sk_buff **skb,
                       const struct net_device *in,
                       const struct net_device *out,
                       int (*okfn)(struct sk_buff *))
{
    return NF_DROP;
}



