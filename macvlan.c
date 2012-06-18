/* 
 * Creating a vlan interface on MAC OS X and Freebsd 
 * Tested on: MAC OS 10.5.6
 * Author: Arjun Sambamoorthy (arjunsam@gmail.com)
 * Reference: Freebsd kernel source (www.freebsd.org)
 * 
 * Note:
 * This program is an example for programmatically creating and deleting a vlan interface 
 * You can also use ifconfig/ipconfig to create a vlan interface and assign an IP address to it.
 * For creating a vlan interface:   ifconfig vlan100 create
 * For binding the created interface to a parent interface and assign vlan id: 
   ifconfig vlan100 vlan 100 vlandev en0
 * For assigning an IP address using DHCP: ipconfig set vlan100
*/ 

#include <sys/ioctl.h>
#include <sys/sockio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <errno.h>
#include <net/if.h>
#include <ctype.h>

void usage();

struct vlanreq {
	char vlr_parent[IFNAMSIZ];
	u_short vlr_tag;
};

void usage(char *bin){

	printf("%s: Creates a vlan interface on MAC OS X and freebsd\n", bin);
	printf("Usage:\n");
	printf("i	parent interface\n");
	printf("r	remove interface\n");
	printf("v	vlan-tag\n");
    printf("To create a vlan interface:\n");
	printf("Example usage: %s -i en0 -v 10\n",bin);
	printf("Once the vlan interface is created, you can assign the ip address using the command," 
           "ipconfig set vlan100 dhcp \n");
    printf("To remove the vlan interface:\n");
	printf("Example usage: %s -r vlan100 \n",bin);
}

void removeInterface(char *deviceName){

	struct ifreq ifr;
	int sock = socket(AF_LOCAL,SOCK_DGRAM,0);
	if(sock < 0){
		err(1,"Error: opening socket");
	}
	strncpy(ifr.ifr_name,deviceName,IFNAMSIZ);
	
	if((ioctl(sock,SIOCIFDESTROY,&ifr) < 0)){
		printf("Error: Deleting interface %s\n",deviceName);
		err(1,"SIOCIFDESTROY");
	}
	printf("Removed interface %s\n",deviceName);
}
	

int main(int argc, char **argv){

	if(argc < 3){
		usage(argv[0]);
		exit(1);
	}

	int c = 0;
	char device[IFNAMSIZ] = { '\0' };
	int vlanTag = 0;
	char vlanString[10] = {'\0' };

	while((c = getopt(argc,argv,"i:v:r:h")) != -1){

		switch(c){

			case 'i':
				strncpy(device,optarg,sizeof(device) - 1);
				break;
			case 'r':
				strncpy(device,optarg,sizeof(device) - 1);
				removeInterface(device);
				exit(1);
			case 'v':
				strncpy(vlanString,optarg,sizeof(vlanString) - 1 );
				vlanTag = atoi(optarg);
				break;
			case 'h':
				usage(argv[0]);
				exit(1);
			default:
				usage(argv[0]);
				exit(1);
		}
	}	
	
	char vlanDevice[IFNAMSIZ] = {'\0' };
	strcpy(vlanDevice,"vlan");
	strcat(vlanDevice,vlanString);
	
	struct ifreq ifr;
	struct vlanreq vreq;
	int sock;
	if((sock = socket(AF_LOCAL, SOCK_DGRAM, 0)) < 0){
		err(1,"Error: opening socket");
		return -1;
	}
	bzero((char *)&vreq,sizeof(vreq));
	strncpy(ifr.ifr_name,vlanDevice,sizeof(ifr.ifr_name)); 
	vreq.vlr_tag = vlanTag;
	strncpy(vreq.vlr_parent,device,sizeof(vreq.vlr_parent));
	ifr.ifr_data = (caddr_t)&vreq;
	 	
	int ret = ioctl(sock,SIOCIFCREATE,(caddr_t)&ifr);
	if(ret < 0){
		err(1,"SIOCIFCREATE");
		return -1;
	}
	
	ret = ioctl(sock,SIOCSETVLAN,(caddr_t)&ifr);
	if(ret < 0){
		err(1,"SIOCSETVLAN");
		return -1;
	}
	printf("Created vlan interface %s\n", vlanDevice);	
	return 0;
}

