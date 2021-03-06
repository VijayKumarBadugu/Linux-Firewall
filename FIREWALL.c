#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/list.h>
#include <asm/uaccess.h>
#include <linux/udp.h>
#include <linux/tcp.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>  
#include <linux/icmp.h>       
         
static struct nf_hook_ops netfilter_ops;                        
static unsigned char *ip_webserver_address = "\xC0\xA8\x00\x02"; 
static unsigned char *ip_remote_client_address = "\xC0\xA8\x02\x02";
struct sk_buff *sock_buff;                              
struct udphdr *udp_header;  
void PrintIP(unsigned int Destination)
{
	unsigned int variable[4];
	int i;
	for(i=0;i<=3;i++)
	{
		variable[i]=(Destination >> (i*8)) & 0xFF;
	}
	printk("%d.%d.%d.%d",variable[0],variable[1],variable[2],variable[3]);
}                            
unsigned int main_hook(unsigned int hooknum,
                  struct sk_buff **skb,
                  const struct net_device *in,
                  const struct net_device *out,
                  int (*okfn)(struct sk_buff*))
{
	unsigned int k1,k,Destination,Source;
	struct iphdr *ip_header = (struct iphdr *)skb_network_header(skb);
	struct tcphdr *tcp_header = (struct tcphdr *)skb_transport_header(skb);
	k1=(unsigned int)ntohs(tcp_header->dest);
	Destination=(unsigned int)(ip_header->daddr);
	Source=(unsigned int)(ip_header->saddr);


	if(ip_header->protocol ==1)
	{
		struct icmphdr *icmp_header = (struct icmphdr *)(skb_transport_header(skb));
		k=(icmp_header->type);
		if(k==8)
		{
			if(ip_header->daddr == *(unsigned int*)ip_webserver_address)
			{
				return NF_ACCEPT;
			}
			else
			{ 
				if(ip_header->daddr == *(unsigned int*)ip_remote_client_address)
				{
				return NF_ACCEPT;
				}	
				else 
				{
				printk(KERN_INFO "Dropped: cause: icmp, interface: eth1,destination ");
				PrintIP(Destination);
				printk(" source ");
				PrintIP(Source);
				return NF_DROP;
				}
			}
		}
		else 
		{
			return NF_ACCEPT;
		
			
		}}
else
{
	if((k1==80)||(k1==22))
	{
		if(ip_header->saddr == *(unsigned int*)ip_remote_client_address)
		{
			if(k1==80)
			{
				if(ip_header->daddr == *(unsigned int*)ip_webserver_address)
				{
				
				return NF_ACCEPT;
				}
				else
				{
				printk(KERN_INFO "Dropped: cause: http, interface: eth1,destination ");
				PrintIP(Destination);
				printk(" source ");
				PrintIP(Source);
				return NF_DROP;
				}
			
			}
			else
			{
				printk(KERN_INFO "Dropped: cause: ssh, interface: eth1,destination ");
				PrintIP(Destination);
				printk(" source ");
				PrintIP(Source);
				return NF_DROP;
			}
		}
		else
		{
			return NF_ACCEPT;
		}
	}
	else
	{
		return NF_ACCEPT;
	}
}

	
		
}




int init_module()
{
        netfilter_ops.hook=main_hook;
        netfilter_ops.pf                =       PF_INET;        
        netfilter_ops.hooknum           =       NF_INET_POST_ROUTING;
        netfilter_ops.priority          =       NF_IP_PRI_FIRST;
        nf_register_hook(&netfilter_ops);
        
return 0;
}
void cleanup_module() { nf_unregister_hook(&netfilter_ops); }
