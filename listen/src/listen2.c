 /*J         Listen to multicast messages using promicious mode to try and work         out what is on the lan  )         This program updates LISTEN_TABLE   9 	This program has been used in the following environments    		VAXC V3.2 VMS VAX V5.5-2.  		DECC V5.0 VMS AXP V6.2 		DECC V6.5 VMS Alpha V7.3-2   	It may work on other versions.   ; 	Any changes or bugs contact me at miller@encompasserve.org     COPYRIGHT NOTICE   5  This software is COPYRIGHT � 1996,2005, Ian Miller.    3  Released under licence described in aaareadme.txt      DISCLAIMER   J  This software is provided "AS IS". The author makes no representations orN  warranties with respect to the software and specifically disclaim any impliedE  warranties of merchantability or fitness for any particular purpose.    History	 =======   ( 07-apr-1997 I.Miller  V1.0      Created.M 10-Apr-1997 I.Miller  V1.0-001  Allow entries in protocols table matching on  @                                 protocol only (for LAVC and IP)./ 				Listen to other DECnet IV router multicast. E 11-Apr-1997 I.Miller  V1.0-002  Add some more protocols to the table. 9                                 Modify analyse_hdr a bit. P                                 Allow entries in table matching on address only.@ 14-Apr-1997 I.Miller  V1.0-003	More improvements to analyse_hdr.! 				Add call to process_ipx_802_3 F 30-Apr-1997 I.Miller  V1.0-004	Much changes to processing of messages, 				services, routes etc. 9 15-Jul-1997 I.Miller  V1.0-005  Listen to all IP and ARP. B 04-Jul-2005 I.Miller  V1.0-006	Tidy code a little for new compiler */   #define VERSION "V1.0-006"
 #if __DECC #pragma module LISTEN2 VERSION #else  #module LISTEN2 VERSION  #endif   #include <stdio.h> #include <stdlib.h>  #include <string.h>  #include <ctype.h> #include <assert.h>  #include <signal.h>    #include <iodef.h> #include <descrip.h> #include <starlet.h>   #include "ethernet.h"  #include "listen_table.h"  #include "listen_subs.h" #include "listen_process.h"   
 #if __DECC #pragma member_alignment save  #pragma nomember_alignment #endif   #include "nmadef.h"    static struct  {          unsigned short code0;          unsigned long format;          unsigned short code1;          unsigned long padflag;         unsigned short code2;          unsigned long bufcnt;          unsigned short code3;          unsigned long msgsize;         unsigned short code4;          unsigned long rsrtflag;          unsigned short code5;          unsigned long prmflag; } sidparam =   { M  NMA$C_PCLI_FMT,NMA$C_LINFM_ETH,                        /* Ethernet format */ M  NMA$C_PCLI_PAD,NMA$C_STATE_OFF,                        /* no padding      */ M  NMA$C_PCLI_BFN,5,                                      /* No. of buffers  */ M  NMA$C_PCLI_BUS,NODE_BUFSIZE,                           /* max message size*/ M  NMA$C_PCLI_RES,NMA$C_LINRES_ENA,                       /* restart enabled */ M  NMA$C_PCLI_PRM,NMA$C_STATE_ON                          /* promiscuous mode*/  }; static struct  {          unsigned long len;         void *sidparamptr;- } sidparamdsc = {sizeof(sidparam),&sidparam};   2 /* buffer for recved ethernet frame             */* static unsigned char rcvbuf[NODE_BUFSIZE];2 /* buffer for recived ethernet frame header     */ static FRAME_HEADER hdr;
 #if __DECC  #pragma member_alignment restore #endif  S static char *devnames[] = {"ETH","EZA0","ESA0","XQA0","ETA0","XEA0","EXA0","EWA0"}; % static $DESCRIPTOR(devname_dsc,NULL);   ' /* the table of protocols to process */  static struct  { 2         /* protocol type                        */         FRAME_FORMAT format;         unsigned char ptype[5]; >         unsigned char adr[6];           /* multicast addr   */2         /* name for this protocol               */         char *name; 2         /* routine to process messages          */`         void (*process)(int node_index,FRAME_HEADER *hdr,unsigned char *msg,unsigned short len); } protocols[]   =   { "     {   /* MOPRC SYSID V3       */7         FMT_ETHERNET,{0x60,0x02},{0xAB,00,00,02,00,00}, /                 "MOP SYSID V3",process_sysid_v3      },"     {   /* MOPRC SYSID V4       */B         FMT_802E,{0x08,0x00,0x2b,0x60,0x02},{0xAB,00,00,02,00,00},0                 "MOP SYSID V4",process_sysid_v4,     },"     {   /* ARP                  */A         FMT_ETHERNET,{0x08,0x06},{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, !                 "ARP",process_arp      },*     {   /* IP brodcast/multicast        */A         FMT_ETHERNET,{0x08,0x00},{0x00,0x00,0x00,0x00,0x00,0x00}, *                 "IP(broadcast)",process_ip     },*     {   /* DECNET IV - end node hellos  */A         FMT_ETHERNET,{0x60,0x03},{0xAB,0x00,0x00,0x03,0x00,0x00}, H                 "DECnet IV - broadcast to all routers",process_decnet_iv     },     {   /* DECNET IV - */ A         FMT_ETHERNET,{0x60,0x03},{0xAB,0x00,0x00,0x04,0x00,0x00}, J                 "DECnet IV - broadcast to all end nodes",process_decnet_iv     },     {   /* DECNET IV - */ A         FMT_ETHERNET,{0x60,0x03},{0x09,0x00,0x2b,0x02,0x00,0x00}, M                 "DECnet IV - broadcast to all area routers",process_decnet_iv      },     {	/* NetBUEI multicasts		*/ + 	FMT_802,{NETBUEI_SAP},{03,00,00,00,00,01}, 4                 "NetBUEI multicasts",process_netbuei     },     {	/* IPX multicasts		*/ 3 	FMT_802,{IPX_SAP},{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, ,                 "IPX multicasts",process_ipx     },*     {   /* LAVC                         */A         FMT_ETHERNET,{0x60,0x07},{0x00,0x00,0x00,0x00,0x00,0x00}, '                 "LAVC",process_eth_misc      },&     {	/* 802.1d brg grp multicasts		*/0 	FMT_802,{0x42},{0x01,0x80,0xc2,0x00,0x00,0x00},6                 "802.1d bridge grp",process_802_bridge     },(     {	/* Xcst ES ISO 9542 multicasts		*/4 	FMT_802,{ISOTP_SAP},{0x01,0x80,0xc2,0x00,0x00,0x4},)                 "ISO ES",process_802_misc      },(     {	/* Xcst ES ISO 9542 multicasts		*/4 	FMT_802,{ISOTP_SAP},{0x01,0x80,0xc2,0x00,0x00,0x5},)                 "ISO IS",process_802_misc      },)     {	/* X 802.1e Ld svr g multicasts		*/ . 	FMT_802,{00},{0x01,0x80,0xc2,0x00,0x00,0x11},1                 "ISO 10589 IS 1",process_802_misc      },)     {	/* X 802.1e Ld Dvc g multicasts		*/ . 	FMT_802,{00},{0x01,0x80,0xc2,0x00,0x00,0x12},1                 "ISO 10589 IS 1",process_802_misc      },(     {	/* X ISO 10589 IS 1 multicasts		*/5 	FMT_802,{ISOTP_SAP},{0x01,0x80,0xc2,0x00,0x00,0x14}, 1                 "ISO 10589 IS 1",process_802_misc      },(     {	/* X ISO 10589 IS 2 multicasts		*/5 	FMT_802,{ISOTP_SAP},{0x01,0x80,0xc2,0x00,0x00,0x15}, 1                 "ISO 10589 IS 2",process_802_misc      },2     {   /* LAT service annoucements             */A         FMT_ETHERNET,{0x60,0x04},{0x09,0x00,0x2b,0x00,0x00,0x0f}, 6                 "LAT Service Annoucements",process_lat     },2     {   /* LAT service annoucements             */A         FMT_ETHERNET,{0x60,0x04},{0xAB,0x00,0x03,0x00,0x00,0x00}, 6                 "LAT Service Annoucements",process_lat     },/     {	/* LAST - Local Area System Transport 	*/ - 	FMT_802,{0},{0x09,0x00,0x2B,0x04,0x00,0x00}, #                 "LAST",process_last      }, };; #define NPROTOCOLS (sizeof(protocols)/sizeof(protocols[0]))   A static void process_message(FRAME_HEADER *hdr,unsigned char *msg,          unsigned short len);C static void analyse_hdr(int n,FRAME_HEADER *hdr,unsigned char *msg,  	unsigned short len);  static void ctrlc(int x);  static int bye;  static void goodbye();    int main(int argc, char *argv[]) {          int n;         unsigned long ccode;         unsigned short chan; 	unsigned short iosb[4];  A         printf("LISTEN2 %s starting at %s\n",VERSION,curtime());	   *         /* load up the node table       */         read_table();  	atexit(goodbye);  	bye = FALSE;  	signal(SIGINT,ctrlc);  >         /* enable informational messages from table routines*/: 	log_mask = LT_LOG_NEWNODE|LT_LOG_NEWATTR|LT_LOG_NEWNAME;     	/* assign to ethernet device	*/B         for (n = 0; n < sizeof(devnames)/sizeof(devnames[0]); n++)	         { 8                 devname_dsc.dsc$a_pointer = devnames[n];?                 devname_dsc.dsc$w_length = strlen(devnames[n]); =                 ccode = sys$assign(&devname_dsc,&chan,0,0,0);                  if (ccode & 1):                         break;  /* exit loop on success */	         }          if (!(ccode & 1)) 	         { A                 printf("Error 0x%x assigning to device\n",ccode);                  exit(ccode);	         }   0         /* start up the channel for listening */L         ccode = sys$qiow(0,chan,IO$_SETMODE|IO$M_CTRL|IO$M_STARTUP,iosb,0,0,0                         0,&sidparamdsc,0,0,0,0);         if (ccode & 1)                  ccode = iosb[0];         if (!(ccode & 1)) 	         { E                 printf("Error 0x%x (%x,%x,%x,%x) starting channel\n", ?                         ccode,iosb[0],iosb[1],iosb[2],iosb[3]);                  exit(ccode);	         }    	while(!bye) 	{ 	        /* read a message */ 0                 memset(rcvbuf,0,sizeof(rcvbuf)); 		memset(&hdr,0,sizeof(hdr)); 7 	        ccode = sys$qiow(0,chan,IO$_READVBLK,iosb,0,0, % 			rcvbuf,sizeof(rcvbuf),0,0,&hdr,0);  	        if (ccode & 1) ! 	                ccode = iosb[0];  	        if (!(ccode & 1))
 	        {? 	                printf("Error 0x%x (%x,%x) reading message\n",  				ccode,iosb[0],iosb[1]);  	                exit(ccode); 
 	        }G 	        /* validate message - check destination is a multicast addr */ 4 	        if ((hdr.hdr_eth.destination[0] & 1) == 1) 
 	        {=                         process_message(&hdr,rcvbuf,iosb[1]);                  }                  else                 { J                         /* some protocols we look at non-multicast      */C                         if ((hdr.hdr_eth.ptype == IP_PROTOCOL_TYPE) E                         ||  (hdr.hdr_eth.ptype == ARP_PROTOCOL_TYPE))                          { E                                 process_message(&hdr,rcvbuf,iosb[1]);                          }                  } 	         } ! 	write_table();	/* write table */  }   A static void process_message(FRAME_HEADER *hdr,unsigned char *msg,          unsigned short len)  {  	int n;          int p;         int found; 	unsigned short now[4];    	sys$gettim(now);            found = FALSE;'         for (p= 0; p < NPROTOCOLS; p++) 	         { +                 switch(protocols[p].format)                  { "                 case FMT_ETHERNET:;                         found = memcmp(&hdr->hdr_eth.ptype, ;                                 protocols[p].ptype,2) == 0;                          break;                 case FMT_802E::                         found = memcmp(&hdr->hdr_802e.pid,;                                 protocols[p].ptype,5) == 0;                          break;                 case FMT_802: 7                         if (protocols[p].ptype[0] != 0) S                                 found = hdr->hdr_802.dsap == protocols[p].ptype[0];                          elseJ                                 found = TRUE;   /*match on address only */                         break;                 default:&                         assert(FALSE);                 }                  if (found)                 { 8                         if (protocols[p].adr[0] != 0x00)                         { J                                 /* check multicast address matches      */]                                 if (memcmp(hdr->hdr_eth.destination,protocols[p].adr,6) == 0) .                                         break;$                                 else6                                         found = FALSE;                         }                          else                         { b                                 /* no multicast address specified so matched on protocol only   */&                                 break;                         }                  }          }         *         n = add_node(hdr->hdr_eth.source); 	if (n >= 0) 	{2 		memcpy(nodes[n].time,now,sizeof(nodes[n].time)); 	        if (found)                  { <                         protocols[p].process(n,hdr,msg,len);
 	        } 	        else 
 	        {, 	                analyse_hdr(n,hdr,msg,len);
 	        } 	} }    static void 	 goodbye()  { 
 	if (!bye) 	{ 		write_table(); 		bye = TRUE;  	} }    static void  ctrlc(int x) {  	signal(SIGINT,ctrlc); 	bye = TRUE; }    static void J analyse_hdr(int n,FRAME_HEADER *hdr,unsigned char *msg,unsigned short len) {          unsigned short ptype;   #         ptype = hdr->hdr_eth.ptype;          bswap(&ptype);         if (ptype == 0xffff)	         { A                 /* could be a IPX message with no 802.2 header */ # 		process_ipx_802_3(n,hdr,msg,len); 	         }          else         if ((ptype == 0xaaaa) 2         &&  (hdr->hdr_802e.ctl == NMA$C_CTLVL_UI))   	{, 		/* its a IEEE 802 extended format frame */) 		add_protocol_802e(n,hdr->hdr_802e.pid);  	} 	else 7         if ((ptype > 1500) && (hdr->hdr_802e.ctl == 0)) 	         { 2                 /* its a ethernet format frame  */7                 add_protocol_eth(n,hdr->hdr_eth.ptype); 	         }          else  	         { :                 if ((hdr->hdr_802e.ctl == NMA$C_CTLVL_UI) 0                 &&  (hdr->hdr_802e.dsap == 0xAA)/                 &&  (hdr->hdr_802e.pid[0] != 0) 0                 &&  (hdr->hdr_802e.pid[1] != 0))                 { B                         /* its a IEEE 802 extended format frame */?                         add_protocol_802e(n,hdr->hdr_802e.pid);                  } @                 else if ((hdr->hdr_802.ctl[0] == NMA$C_CTLVL_UI)A                      ||  (hdr->hdr_802.ctl[0] == NMA$C_CTLVL_XID) C                      ||  (hdr->hdr_802.ctl[0] == NMA$C_CTLVL_XID_P) B                      ||  (hdr->hdr_802.ctl[0] == NMA$C_CTLVL_TEST)C                      ||  (hdr->hdr_802.ctl[0] == NMA$C_CTLVL_XID_P) 2                      &&  (hdr->hdr_802.dsap != 0))                 { 9                         /* its a IEEE 802 format frame */ >                         add_protocol_802(n,hdr->hdr_802.dsap);                 } 1                 else if ((hdr->hdr_802.ssap == 2) 2                      &&  (hdr->hdr_802.dsap == 2))                 { C                         /* its a IEEE 802 format frame (802.2 ?) */ >                         add_protocol_802(n,hdr->hdr_802.dsap);                 } 1                 else if ((hdr->hdr_802.ssap == 3) 2                      &&  (hdr->hdr_802.dsap == 2))                 { C                         /* its a IEEE 802 format frame (802.2 ?) */ >                         add_protocol_802(n,hdr->hdr_802.dsap);                 } &                 else if (ptype > 1500)                 { :                         /* its a ethernet format frame  */?                         add_protocol_eth(n,hdr->hdr_eth.ptype);                  }                  else                 { :                         /* WHAT IS THIS FRAME ?!?!?     */P                         printf("%%LISTEN-W-UNKFTYPE, Unknown frame type from ");A                         display_addr(stdout,hdr->hdr_802.source); >                         printf(" (%#02x,%#02x,%#02x,%#02x)\n",D                                 hdr->hdr_802.dsap,hdr->hdr_802.ssap,I                                 hdr->hdr_802.ctl[0],hdr->hdr_802.ctl[1]);                  } 	         }  }                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           