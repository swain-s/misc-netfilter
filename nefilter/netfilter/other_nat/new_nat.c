#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/version.h>

#include <linux/netfilter.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>

#include <linux/inet.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/icmp.h>
#include <linux/netfilter_ipv4.h>
#include <linux/if_arp.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <linux/string.h>

typedef unsigned int u32_t;
typedef unsigned short u16_t;

char *my_ip="192.168.99.176"; //the ip you listen on
int my_port=8000;  // port listen on

char *tg_ip="45.76.100.53"; //ip of target
int tg_port=80; //port of target

module_param (my_ip, charp, S_IRUGO);
module_param (my_port, uint, S_IRUGO);
module_param (tg_ip, charp, S_IRUGO);
module_param (tg_port, uint, S_IRUGO);

static unsigned char tg_hwaddr[ETH_ALEN]={0};
int got_tg_hwaddr=0;

//static unsigned char my_hwaddr[ETH_ALEN]={0};  //not really necessary

//char if_name[]="ens33";    //network interface, currently only support one interface  //not really necessary

u32_t my_ip_u32;
u32_t tg_ip_u32;
u16_t my_port_u16;
u16_t tg_port_u16;

//static unsigned char cl_hwaddr[ETH_ALEN]={0x0}; //remember last client's hwaddr
//u32_t cl_ip_u32;  // and ip

struct client_info_t
{
    u32_t ip_u32;
    unsigned char hwaddr[ETH_ALEN];
};

struct client_info_t cl_info[65536]; //a dirty hash table to remember info of clients.
//hash by port of client
//there will be problems if two clients use the same port (but its rare)
//TODO fix it later

struct pseudo_header {
    u_int32_t source_address;
    u_int32_t dest_address;
    u_int8_t placeholder;
    u_int8_t protocol;
    u_int16_t tcp_length;
};

unsigned short csum_with_header(char* header,int hlen,const unsigned short *ptr,int nbytes) {//works both for big and little endian
    
    long sum;
    unsigned short oddbyte;
    short answer;
    unsigned short *tmp;
    int i;
    //assert(hlen%2==0);
    
    sum=0;
    tmp= (unsigned short *)header;
    for(i=0;i<hlen/2;i++)
    {
        sum+=*tmp++;
    }
    
    
    while(nbytes>1) {
        sum+=*ptr++;
        nbytes-=2;
    }
    if(nbytes==1) {
        oddbyte=0;
        *((u_char*)&oddbyte)=*(u_char*)ptr;
        sum+=oddbyte;
    }
    
    sum = (sum>>16)+(sum & 0xffff);
    sum = sum + (sum>>16);
    answer=(short)~sum;
    
    return(answer);
}

/*
 static unsigned int local_out_hook (void *priv,
 struct sk_buff *skb,
 const struct nf_hook_state *state)
 {
 struct iphdr *iph;
 unsigned int hdroff;
 //struct tcphdr *hdr;
 int iphdroff=0;
 iph = (void *)skb->data + iphdroff;
 hdroff = iphdroff + iph->ihl * 4;
 
 if(ok==0&&iph->daddr==dst_ip_u32)
 {
 //ok=1;
 //memcpy(dst_hwaddr, (eth_hdr(skb)->h_dest), ETH_ALEN);
 printk("okay!! %pM\n",dst_hwaddr);
 }
 
 return NF_ACCEPT;
 
 }*/

void tcp_chk_upd(u32_t src,u32_t dst,struct tcphdr *hdr,u32_t tot_len)
//the checksum update function of kernel is not straight forward, so we implement our own slower one  //unfortunately, this doesnt work, since data in skb is not continous
{
    struct pseudo_header psh;
    psh.source_address = src;
    psh.dest_address =  dst;
    psh.placeholder = 0;
    psh.protocol = 6;
    psh.tcp_length = htons(  tot_len);
    hdr->check=0;
    hdr->check=csum_with_header((char *)(&psh),sizeof(psh),(u16_t *)hdr,tot_len );
}
static unsigned int pre_routing_hook(void *priv,
                                     struct sk_buff *skb,
                                     const struct nf_hook_state *state)
{
    struct iphdr *iph;
    unsigned int hdroff;
    struct tcphdr *hdr;
    int iphdroff=0;
    int ret;
    int i;
    int tcp_tot_len;
    struct ethhdr *eh;
    
    iph = (void *)skb->data + iphdroff;
    hdroff = iphdroff + iph->ihl * 4;
    
    if(!got_tg_hwaddr&&iph->saddr==tg_ip_u32)
    {
        printk("caught a packet from target, %pM %pM\n",eth_hdr(skb)->h_source,eth_hdr(skb)->h_dest);
        eh = eth_hdr(skb);
        for(i=0;i<ETH_ALEN;i++)
            tg_hwaddr[i]=eh->h_source[i];
        got_tg_hwaddr=1;
    }
    
    
    if(iph->protocol!=6) return NF_ACCEPT;
    if(skb->len < hdroff + sizeof(struct tcphdr)) return NF_ACCEPT;
    hdr = (struct tcphdr *)(skb->data + hdroff);
    
    tcp_tot_len=ntohs(iph->tot_len)  - iph->ihl*4;
    if(skb->len< hdroff+ tcp_tot_len)
    {
        printk("fail0\n");
        return NF_ACCEPT;
    }
    
    if(iph->daddr==my_ip_u32&&hdr->dest==my_port_u16)
    {
        /*if (!skb_make_writable(skb, iphdroff + skb->len))
         {
         printk("fail1.0\n");
         return NF_ACCEPT;
         }*/
        
        //printk("before1,%pM %pM\n",eth_hdr(skb)->h_source,eth_hdr(skb)->h_dest);
        
        cl_info[hdr->source].ip_u32=iph->saddr;
        
        csum_replace4(&iph->check, iph->daddr, tg_ip_u32);
        csum_replace4(&iph->check, iph->saddr, my_ip_u32);
        
        csum_replace4(&hdr->check, iph->daddr, tg_ip_u32);
        csum_replace4(&hdr->check, iph->saddr, my_ip_u32);
        csum_replace2(&hdr->check, hdr->dest, tg_port_u16);
        
        iph->daddr = tg_ip_u32;
        iph->saddr = my_ip_u32;
        //iph->check=0;
        //iph->check=csum_with_header(0,0,(u16_t*)iph,iph->ihl*4);
        
        hdr->dest=tg_port_u16;
        //tcp_chk_upd(iph->saddr,iph->daddr,hdr,tcp_tot_len);
        
        skb->pkt_type = PACKET_OUTGOING;
        //struct net_device * dev = dev_get_by_name(&init_net, if_name);
        //skb->dev=dev;
        
        eh = eth_hdr(skb);
        
        skb->data = (unsigned char *)eh;
        skb->len += ETH_HLEN; //sizeof(sb->mac.ethernet);
        
        for(i=0;i<ETH_ALEN;i++)
            cl_info[hdr->source].hwaddr[i]=eh->h_source[i];  //remember ip of last client, no lock is needed since the change is atomic (for each i)
        
        memcpy(eh->h_source, eh->h_dest,ETH_ALEN);
        memcpy(eh->h_dest, tg_hwaddr,ETH_ALEN);
        
        //printk("after1, %pM %pM\n",eth_hdr(skb)->h_source,eth_hdr(skb)->h_dest);
        
        ret=dev_queue_xmit(skb);
        
        //printk("got a packet1,%d\n",ret);
        return NF_STOLEN;
        
        /*
         err=ip_route_me_harder(state->net, skb, RTN_UNSPEC);
         if(err<0)
         {
         printk("fail1.2");
         }
         skb_dst_drop(skb);
         //printk("got a packet,%d\n",err);
         return NF_ACCEPT;*/
    }
    
    else if(iph->saddr==tg_ip_u32&&hdr->source==tg_port_u16 && iph->daddr==my_ip_u32)
    {
        /*if (!skb_make_writable(skb, iphdroff + skb->len))
         {
         printk("fail2.0\n");
         return NF_ACCEPT;
         }*/
        
        //printk("before2,%pM %pM\n",eth_hdr(skb)->h_source,eth_hdr(skb)->h_dest);
        u32_t cl_ip_u32=cl_info[hdr->dest].ip_u32;
        
        csum_replace4(&iph->check, iph->daddr, cl_ip_u32);
        csum_replace4(&iph->check, iph->saddr, my_ip_u32);
        
        csum_replace4(&hdr->check, iph->daddr, cl_ip_u32);
        csum_replace4(&hdr->check, iph->saddr, my_ip_u32);
        csum_replace2(&hdr->check, hdr->source, my_port_u16);
        
        iph->daddr = cl_ip_u32;
        iph->saddr = my_ip_u32;
        
        //iph->check=0;
        //iph->check=csum_with_header(0,0,(u16_t*)iph,iph->ihl*4);
        
        hdr->source=my_port_u16;
        
        //tcp_chk_upd(iph->saddr,iph->daddr,hdr,tcp_tot_len);
        
        skb->pkt_type = PACKET_OUTGOING;
        //struct net_device * dev = dev_get_by_name(&init_net, if_name);
        //skb->dev=dev;
        
        eh = eth_hdr(skb);
        
        skb->data = (unsigned char *)eh;
        skb->len += ETH_HLEN; //sizeof(sb->mac.ethernet);
        
        memcpy(eh->h_source, eh->h_dest,ETH_ALEN);
        memcpy(eh->h_dest, cl_info[hdr->dest].hwaddr,ETH_ALEN);
        
        //printk("after2, %pM %pM\n",eth_hdr(skb)->h_source,eth_hdr(skb)->h_dest);
        
        ret=dev_queue_xmit(skb);
        
        //printk("got a packet2,%d\n",ret);
        return NF_STOLEN;
        
        /*
         err=ip_route_me_harder(state->net, skb, RTN_UNSPEC);
         if(err<0)
         {
         printk("fail2.2");
         }
         skb_dst_drop(skb);
         printk("got a packet,%d\n",err);
         return NF_ACCEPT;*/
    }
    
    
    return NF_ACCEPT;
    
}
static unsigned int post_routing_hook (void *priv,
                                       struct sk_buff *skb,
                                       const struct nf_hook_state *state)
{
    struct iphdr *iph;
    unsigned int hdroff;
    struct tcphdr *hdr;
    int iphdroff=0;
    iph = (void *)skb->data + iphdroff;
    hdroff = iphdroff + iph->ihl * 4;
    if(iph->protocol!=6) return NF_ACCEPT;
    if(skb->len < hdroff + sizeof(struct tcphdr)) return NF_ACCEPT;
    hdr = (struct tcphdr *)(skb->data + hdroff);
    
    
    if(iph->daddr==tg_ip_u32)
    {
        //printk("catch, %pM %pM\n",eth_hdr(skb)->h_source,eth_hdr(skb)->h_dest);
    }
    return NF_ACCEPT;
}

static struct nf_hook_ops nf_my_nat_ops[] = {
    {
        .hook           = pre_routing_hook,
        .pf             = NFPROTO_IPV4,
        .hooknum        = NF_INET_PRE_ROUTING,
        .priority       = NF_IP_PRI_FIRST,
    },
    {
        .hook           = post_routing_hook,
        .pf             = NFPROTO_IPV4,
        .hooknum        = NF_INET_POST_ROUTING,
        .priority       = NF_IP_PRI_FIRST,
    },
};


int init_module()
{
    int ret;
    
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,13,0)
    ret=nf_register_net_hooks(&init_net, nf_my_nat_ops, ARRAY_SIZE(nf_my_nat_ops));
#else
    ret=nf_register_hooks(nf_my_nat_ops, ARRAY_SIZE(nf_my_nat_ops));
#endif
    
    my_ip_u32=in_aton(my_ip);
    tg_ip_u32=in_aton(tg_ip);
    my_port_u16=htons(my_port);
    tg_port_u16=htons(tg_port);
    printk("my_ip=%s my_port=%d tg_ip=%s tg_port=%d\n",my_ip,my_port,tg_ip,tg_port);
    if(ret==0)
        printk("load ok\n");
    else
        printk("load fail\n");
    return 0;
}


void cleanup_module()
{
    
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,13,0)
    nf_unregister_net_hooks(&init_net, nf_my_nat_ops, ARRAY_SIZE(nf_my_nat_ops));
#else
    nf_unregister_hooks(nf_my_nat_ops, ARRAY_SIZE(nf_my_nat_ops));
#endif
    
    printk("unloaded\n");
}
