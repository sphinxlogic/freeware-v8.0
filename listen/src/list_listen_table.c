 /*         LIST_LISTEN_TABLE   H         This program reads a LISTEN_TABLE file and displays its contents    COPYRIGHT NOTICE   5  This software is COPYRIGHT � 1997,2005, Ian Miller.    3  Released under licence described in aaareadme.txt      DISCLAIMER   J  This software is provided "AS IS". The author makes no representations orN  warranties with respect to the software and specifically disclaim any impliedE  warranties of merchantability or fitness for any particular purpose.    History	 =======   ' 30-JAN-1997 I.Miller  V1.0-000	Created  = 04-Feb-1997 I.Miller  V1.0-001	Add IEE802E MOP SYSID message. . 21-Mar-1997 I.Miller  V1.0-002  display names.Q 09-Apr-1997 I.Miller  V1.0-003  Display ethernet protocol the correct way around. 7                                 Display protocol names. 7                                 Display node timestamp. 8 14-apr-1997 I.Miller  V1.0-004  Display node attributes.A 25-Apr-1997 I.Miller  V1.0-005	Display service and route records. 3 17-Jun-1997 I.Miller  V1.0-006  Display timestamps. ? 17-Sep-1997 I.Miller  V1.0-007  Display NetBEUI name type name. B 04-Jul-2005 I.Miller  V1.0-008	Tidy code a little for new compiler */   #define VERSION "V1.0-008"
 #if __DECC( #pragma module LIST_LISTEN_TABLE VERSION #else " #module LIST_LISTEN_TABLE VERSION  #endif   #include <stdio.h> #include <stdlib.h>  #include <string.h>  #include <unixio.h>  #include <file.h>  #include <descrip.h> #include <starlet.h> #include "ethernet.h"  #include "listen_table.h"  #include "sysid.h" #include "listen_subs.h"  P static void check_hw_address(FILE *fpOut,int node_index,unsigned char *hw_addr);  1 static void display_name(FILE *fpOut,char *name);   ' static char *netb_name_type(char type);       int main(int argc, char *argv[]) {          static char tbuf[30]; *         static $DESCRIPTOR(tbuf_dsc,tbuf); 	char outfile[256];          FILE *fpOut;         int i,j;         FRAME_HEADER_ETH hdr;          unsigned short tlen;         unsigned short dnet;         unsigned short ptype;  	unsigned char hw_addr[6];         int rhdr;    	/* get name of output file	*/ 	if (argc > 1) 	{ 		strcpy(outfile,argv[1]); 	} 	else  	{% 		printf("Enter output filename : ");  		gets(outfile); 		if (outfile[0] == '\0')  		{ 3 			strcpy(outfile,"TT:");	/* default to terminal */  		}  	} 	puts(" \n");  	/* open output file		*// 	fpOut = fopen(outfile,"w","rfm=var","rat=cr");  	if (fpOut == NULL)  	{' 		perror("Failed to open output file");  		exit (1);  	}	   *         /* load up the node table       */         read_table();   %         for (i=0; i < MAX_NODES; i++) 	         { #                 if (nodes[i].inuse)                  { ;                         fprintf(fpOut,"\n \nNODE (%d) ",i); :                         display_addr(fpOut,nodes[i].addr);9                         display_man(fpOut,nodes[i].addr); *                         fputc('\n',fpOut);  :                         /* display the node timestamp   */9 			if ((nodes[i].time[0] != 0) && (nodes[i].time[1] != 0) N                         && (sys$asctim(&tlen,&tbuf_dsc,&nodes[i].time,0) & 1))                         { 2                                 tbuf[tlen] = '\0';H                                 fprintf(fpOut,"*Timestamp:- %s\n",tbuf);                         }   5                         if (nodes[i].attributes != 0)                          { G                                 fprintf(fpOut,"*Attributes:- 0x%04x\n",  					nodes[i].attributes);N                                 display_attributes(fpOut,nodes[i].attributes);                         } @                         /* now display the names table        */4                         fprintf(fpOut,"*Names:-\n");5                         for (j=0; j < MAX_NAMES; j++)                          { " 				switch(nodes[i].names[j].type) 				{  				case LT_NAME_DECNET:+ 					fprintf(fpOut,"  DECnet(%s)   : %s\n", ^                                                 display_datetime(NULL,nodes[i].names[j].time),H                                                 nodes[i].names[j].name); 					break;  				case LT_NAME_IP:+ 					fprintf(fpOut,"  IP(%s)       : %s\n", ^                                                 display_datetime(NULL,nodes[i].names[j].time),H                                                 nodes[i].names[j].name); 					break;  				case LT_NAME_NETBUEI: ' 					fprintf(fpOut,"  NetBEUI(%s)  : ", _                                                 display_datetime(NULL,nodes[i].names[j].time)); Q                                         fprintf(fpOut,"%15.15s type 0x%02x %s\n", G                                                 nodes[i].names[j].name, K                                                 nodes[i].names[j].name[15], \                                                 netb_name_type(nodes[i].names[j].name[15])); 					break;  				case LT_NAME_SYSID: + 					fprintf(fpOut,"  SYSID(%s)    : %s\n", ^                                                 display_datetime(NULL,nodes[i].names[j].time),H                                                 nodes[i].names[j].name); 					break; 4                                 case LT_NAME_UNUSED: 					break; 6                                 case LT_NAME_LAT_NODE:+ 					fprintf(fpOut,"  LAT Name(%s) : %s\n", ^                                                 display_datetime(NULL,nodes[i].names[j].time),H                                                 nodes[i].names[j].name); 					break; 6                                 case LT_NAME_LAT_DESC:+ 					fprintf(fpOut,"  LAT Desc(%s) : %s\n", ^                                                 display_datetime(NULL,nodes[i].names[j].time),H                                                 nodes[i].names[j].name); 					break; 2                                 case LT_NAME_LAST:+ 					fprintf(fpOut,"  LAST(%s)     : %s\n", ^                                                 display_datetime(NULL,nodes[i].names[j].time),H                                                 nodes[i].names[j].name); 					break; 5                                 case LT_NAME_NETBIOS: ' 					fprintf(fpOut,"  NetBIOS(%s)  : ", _                                                 display_datetime(NULL,nodes[i].names[j].time)); Q                                         fprintf(fpOut,"%15.15s type 0x%02x %s\n", G                                                 nodes[i].names[j].name, K                                                 nodes[i].names[j].name[15], \                                                 netb_name_type(nodes[i].names[j].name[15])); 					break; 6                                 case LT_NAME_NETBIOSG:' 					fprintf(fpOut,"  NetBIOS G(%s): ", _                                                 display_datetime(NULL,nodes[i].names[j].time)); N                                         fprintf(fpOut,"%15.15s type 0x%02x\n",G                                                 nodes[i].names[j].name, L                                                 nodes[i].names[j].name[15]); 					break; 2                                 case LT_NAME_DHCP:+ 					fprintf(fpOut,"  DHCP host(%s): %s\n", ^                                                 display_datetime(NULL,nodes[i].names[j].time),H                                                 nodes[i].names[j].name); 					break; 7                                 case LT_NAME_WORKGROUP: ' 					fprintf(fpOut,"  Workgroup(%s): ", _                                                 display_datetime(NULL,nodes[i].names[j].time)); N                                         fprintf(fpOut,"%15.15s type 0x%02x\n",G                                                 nodes[i].names[j].name, L                                                 nodes[i].names[j].name[15]); 					break; 5                                 case LT_NAME_NOVELLP: S                                         fprintf(fpOut,"  Novell Printer(%s): %s\n", ^                                                 display_datetime(NULL,nodes[i].names[j].time),H                                                 nodes[i].names[j].name);.                                         break; 				default:P                                         fprintf(fpOut,"  Type 0x%02x(%s): %s\n",G                                                 nodes[i].names[j].type, ^                                                 display_datetime(NULL,nodes[i].names[j].time),H                                                 nodes[i].names[j].name); 					break; !                                 }                          }   B                         /* now display the address table        */8                         fprintf(fpOut,"*Addresses:-\n");9                         for (j=0; j < MAX_ADDRESSES; j++)                          { B                                 switch(nodes[i].addresses[j].type)!                                 { 7                                 case LT_ADDRESS_UNUSED: .                                         break;7                                 case LT_ADDRESS_DECNET: B 		                        dnet = nodes[i].addresses[j].addr[1]*256P                                                 + nodes[i].addresses[j].addr[0];G                                         fprintf(fpOut,"  DECnet(%s): ", c                                                 display_datetime(NULL,nodes[i].addresses[j].time)); J                                         display_dnet_address(fpOut,&dnet);:                                         fputc('\n',fpOut);.                                         break;3                                 case LT_ADDRESS_IP: G                                         fprintf(fpOut,"  IP(%s)    : ", c                                                 display_datetime(NULL,nodes[i].addresses[j].time)); A                                         display_ip_address(fpOut, " 						nodes[i].addresses[j].addr);:                                         fputc('\n',fpOut);.                                         break;4                                 case LT_ADDRESS_IPX:G                                         fprintf(fpOut,"  IPX(%s)   : ", c                                                 display_datetime(NULL,nodes[i].addresses[j].time)); B                                         display_ipx_address(fpOut," 						nodes[i].addresses[j].addr);:                                         fputc('\n',fpOut);.                                         break;5                                 case LT_ADDRESS_LAVC: G                                         fprintf(fpOut,"  LAVC(%s)  : ", c                                                 display_datetime(NULL,nodes[i].addresses[j].time)); A                                         display_lavc_group(fpOut, " 						nodes[i].addresses[j].addr);:                                         fputc('\n',fpOut);.                                         break;(                                 default:T                                         fprintf(fpOut,"  unknown address type %d\n",L                                                 nodes[i].addresses[j].type);!                                 }                          }   $ 			/* now show the protocol table	*/# 			fprintf(fpOut,"*Protocols:-\n"); $ 			for (j=0; j < MAX_PROTOCOLS; j++) 			{: 				switch(nodes[i].protocols[j].ethernet_protocol.format)!                                 { 8                                 case LT_PROTOCOL_UNUSED:.                                         break;5                                 case LT_PROTOCOL_ETH: ]                                         ptype = nodes[i].protocols[j].ethernet_protocol.type; 6                                         bswap(&ptype);S                                         fprintf(fpOut,"  ETHERNET(%s): 0x%04x%s\n", t                                                 display_datetime(NULL,nodes[i].protocols[j].ethernet_protocol.time),L                                                 ptype,get_eth_pname(ptype));.                                         break;5                                 case LT_PROTOCOL_802: T                                         fprintf(fpOut,"  IEE 802(%s) :  0x%02x%s\n",r                                                 display_datetime(NULL,nodes[i].protocols[j].iee802_protocol.time),Z                                                 nodes[i].protocols[j].iee802_protocol.sap,j                                                 get_802_pname(nodes[i].protocols[j].iee802_protocol.sap));.                                         break;6                                 case LT_PROTOCOL_802E:g                                         fprintf(fpOut,"  IEE 802e(%s): 0x%02x-%02x-%02x-%02x-%02x%s\n", s                                                 display_datetime(NULL,nodes[i].protocols[j].iee802e_protocol.time), _                                                 nodes[i].protocols[j].iee802e_protocol.pid[0],  _                                                 nodes[i].protocols[j].iee802e_protocol.pid[1],  _                                                 nodes[i].protocols[j].iee802e_protocol.pid[2],  _                                                 nodes[i].protocols[j].iee802e_protocol.pid[3],  ^                                                 nodes[i].protocols[j].iee802e_protocol.pid[4],l                                                 get_802e_pname(nodes[i].protocols[j].iee802e_protocol.pid));.                                         break;(                                 default:]                                         fprintf(fpOut,"  unknown protocol format %d found\n", `                                                 nodes[i].protocols[j].ethernet_protocol.format);!                                 }  			}  A                         /* now display the routes table        */ %                         rhdr = FALSE; 6                         for (j=0; j < MAX_ROUTES; j++)                         { P                                 if ((nodes[i].routes[j].type != LT_ROUTE_UNUSED),                                 &&  (!rhdr))!                                 { E                                         fprintf(fpOut,"*Routes:-\n"); 4                                         rhdr = TRUE;!                                 } ?                                 switch(nodes[i].routes[j].type) !                                 { 5                                 case LT_ROUTE_UNUSED: .                                         break;:                                 case LT_ROUTE_DECNET_AREA:H                                         display_decnet_route(fpOut,TRUE,J                                                 nodes[i].routes[j].r.data,I                                                 nodes[i].routes[j].rlen); ?                                         fprintf(fpOut,"(%s)\n", `                                                 display_datetime(NULL,nodes[i].routes[j].time));.                                         break;:                                 case LT_ROUTE_DECNET_NODE:I                                         display_decnet_route(fpOut,FALSE, J                                                 nodes[i].routes[j].r.data,I                                                 nodes[i].routes[j].rlen); ?                                         fprintf(fpOut,"(%s)\n", `                                                 display_datetime(NULL,nodes[i].routes[j].time));.                                         break;2                                 case LT_ROUTE_IPX:@                                         display_ipx_route(fpOut,J                                                 nodes[i].routes[j].r.data,I                                                 nodes[i].routes[j].rlen); ?                                         fprintf(fpOut,"(%s)\n", `                                                 display_datetime(NULL,nodes[i].routes[j].time));.                                         break;1                                 case LT_ROUTE_IP: ?                                         display_ip_route(fpOut, J                                                 nodes[i].routes[j].r.data,I                                                 nodes[i].routes[j].rlen); ?                                         fprintf(fpOut,"(%s)\n", `                                                 display_datetime(NULL,nodes[i].routes[j].time));.                                         break;2                                 case LT_ROUTE_IP2:@                                         display_ip2_route(fpOut,J                                                 nodes[i].routes[j].r.data,I                                                 nodes[i].routes[j].rlen); ?                                         fprintf(fpOut,"(%s)\n", `                                                 display_datetime(NULL,nodes[i].routes[j].time));.                                         break;(                                 default:R                                         fprintf(fpOut,"  unknown route type %d\n",I                                                 nodes[i].routes[j].type); !                                 }                          } C                         /* now display the services table        */ %                         rhdr = FALSE; 8                         for (j=0; j < MAX_SERVICES; j++)                         { T                                 if ((nodes[i].services[j].type != LT_SERVICE_UNUSED),                                 &&  (!rhdr))!                                 { G                                         fprintf(fpOut,"*Services:-\n"); 4                                         rhdr = TRUE;!                                 } A                                 switch(nodes[i].services[j].type) !                                 { 7                                 case LT_SERVICE_UNUSED: .                                         break;4                                 case LT_SERVICE_LAT:I                                         fprintf(fpOut,"  LAT(%s): %*.*s", a                                                 display_datetime(NULL,nodes[i].services[j].time), N                                                 nodes[i].services[j].name_len,N                                                 nodes[i].services[j].name_len,K                                                 nodes[i].services[j].name);  					display_lat_service(fpOut,   						nodes[i].services[j].data,% 						nodes[i].services[j].data_len); :                                         fputc('\n',fpOut);.                                         break;5                                 case LT_SERVICE_LAST: J                                         fprintf(fpOut,"  LAST(%s): %*.*s",a                                                 display_datetime(NULL,nodes[i].services[j].time), N                                                 nodes[i].services[j].name_len,N                                                 nodes[i].services[j].name_len,K                                                 nodes[i].services[j].name);   					display_last_service(fpOut,  						nodes[i].services[j].data,% 						nodes[i].services[j].data_len); :                                         fputc('\n',fpOut);.                                         break;4                                 case LT_SERVICE_SAP:P                                         fprintf(fpOut,"  Novell-SAP(%s): %*.*s",a                                                 display_datetime(NULL,nodes[i].services[j].time), N                                                 nodes[i].services[j].name_len,N                                                 nodes[i].services[j].name_len,K                                                 nodes[i].services[j].name);  					display_sap_service(fpOut,   						nodes[i].services[j].data,% 						nodes[i].services[j].data_len); :                                         fputc('\n',fpOut);.                                         break;(                                 default:T                                         fprintf(fpOut,"  unknown service type %d\n",K                                                 nodes[i].services[j].type); !                                 }                          }  			/* now display MOP SYSID	*/  :                         if (nodes[i].mop_sysid_length > 0)                         { -         			/* dump the last MOP SYSID seen	*/ ;                                 sys$asctim(&tlen,&tbuf_dsc, D                                         &nodes[i].mop_sysid_time,0);2                                 tbuf[tlen] = '\0';K                                 fprintf(fpOut,"*ETHERNET MOP\n  %s ",tbuf); A                                 memcpy(&hdr.source,nodes[i].addr, <                                         sizeof(hdr.source));A                                 format_sysid_msg(fpOut,TRUE,&hdr, ;                                         nodes[i].mop_sysid, C                                         nodes[i].mop_sysid_length); 0         	        	if ((nodes[i].addr[0] == 0xAA)4 	                        &&  (nodes[i].addr[1] == 0)1 		                &&  (nodes[i].addr[2] == 0x04)  5 	                        &&  (nodes[i].addr[3] == 0))  		                { U         				if (get_hw_address(nodes[i].mop_sysid,nodes[i].mop_sysid_length,hw_addr))          				{ /         					check_hw_address(fpOut,i,hw_addr);          				} !                                 } )                         }                 ;                         if (nodes[i].mope_sysid_length > 0)                          { 5         			/* dump the last IEE802E MOP SYSID seen	*/ ;                                 sys$asctim(&tlen,&tbuf_dsc, E                                         &nodes[i].mope_sysid_time,0); 2                                 tbuf[tlen] = '\0';J                                 fprintf(fpOut,"*IEE802E MOP\n  %s ",tbuf);A                                 memcpy(&hdr.source,nodes[i].addr, <                                         sizeof(hdr.source));A                                 format_sysid_msg(fpOut,TRUE,&hdr, <                                         nodes[i].mope_sysid,D                                         nodes[i].mope_sysid_length);0         	        	if ((nodes[i].addr[0] == 0xAA)4 	                        &&  (nodes[i].addr[1] == 0)1 		                &&  (nodes[i].addr[2] == 0x04)  5 	                        &&  (nodes[i].addr[3] == 0))  		                { W         				if (get_hw_address(nodes[i].mope_sysid,nodes[i].mope_sysid_length,hw_addr))          				{ /         					check_hw_address(fpOut,i,hw_addr);          				} !                                 }                          }                    } 	         }  }    static void C check_hw_address(FILE *fpOut,int node_index,unsigned char *hw_addr)  { 	 	int n,i; "         unsigned char hw_addr2[6];   	return;   	for (n=0; n < MAX_NODES; n++) 	{#                 if (nodes[n].inuse)                  { 1 		        if (memcmp(nodes[n].addr,hw_addr,6)==0)          		{ 9         			fprintf(fpOut,"%%LISTEN-W-DUPNODE,Node (%d) ", 4                                         node_index);=                 		display_addr(fpOut,nodes[node_index].addr); 2         			fprintf(fpOut," HW address found at ");-         			display_addr(fpOut,nodes[n].addr); &         			fprintf(fpOut," (%d)\n",n);         		} 5                         for (i=0; i < MAX_NODES; i++)                          { 3                                 if (nodes[i].inuse) !                                 { L                                         if ((nodes[i].mop_sysid_length != 0)Y 				        &&  (get_hw_address(nodes[i].mope_sysid,nodes[i].mope_sysid_length,hw_addr2)) M                                         &&  (memcmp(hw_addr,hw_addr2,6) == 0) >                                         &&  (i != node_index)))                                         { J                         			fprintf(fpOut,"%%LISTEN-W-DUPNODE2,Node (%d) ",D                                                         node_index);M                                 		display_addr(fpOut,nodes[node_index].addr); B                         			fprintf(fpOut," hw address found at ");=                         			display_addr(fpOut,nodes[i].addr); 6                         			fprintf(fpOut," (%d)\n",n);)                                         } !                                 }                          }                  }  	} }    static void $ display_name(FILE *fpOut,char *name) {  	char name2[100], *p, *q;   * 	for (p=name,q=name2; *p != '\0'; p++,q++) 	{ 		if (*p < ' ')  		{  			sprintf(q,"\\0x%02x",*p);
 			q += 4; 		}  		else 		{  			*q = *p;  		}  	} 	*q = '\0';  	fputs(name2,fpOut); }    static char *  netb_name_type(char type)  {          char *result = "";           if (type == 0x00) ,                 result = "pcname/workgroup";         else if (type == 0x01)*                 result = "master browser";         else if (type == 0x03)$                 result = "username";         else if (type == 0x20)#                 result = "service";          else if (type == 0x1b)"                 result = "domain";         else if (type == 0x1c)"                 result = "domain";         else if (type == 0x1d)%                 result = "workgroup";          else if (type == 0x1e)%                 result = "workgroup";          return result; } 