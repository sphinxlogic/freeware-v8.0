 /*H         Listen to various protocols on the lan to try and work out whats         out there.  9 	This program has been used in the following environments    		VAXC V3.2 VMS VAX V5.5-2.  		DECC V5.0 VMS AXP V6.2 		DECC V6.5 VMS Alpha V7.3-2   	It may work on other versions.   ; 	Any changes or bugs contact me at miller@encompasserve.org     COPYRIGHT NOTICE   5  This software is COPYRIGHT � 1997,2005, Ian Miller.    3  Released under licence described in aaareadme.txt      DISCLAIMER   J  This software is provided "AS IS". The author makes no representations orN  warranties with respect to the software and specifically disclaim any impliedE  warranties of merchantability or fitness for any particular purpose.    History	 =======   C 14-Mar-1997 I.Miller  V1.0-001  Created (from bits of LISTEN_SYSID) G 08-Apr-1997 I.Miller  V1.0-002  Disable ethernet padding so we get the  G                                 correct length for short MOP V3 frames. 2                                 Exit on control-C.8                                 Change call to add_node.5 10-Apr-1997 I.Miller  V1.0-003	Update node timestamp. D 04-Jul-2005 I.Miller  V1.0-004	Tidy code a little for newer compiler */   #define VERSION "V1.0-004"
 #if __DECC #pragma module LISTEN VERSION  #else  #module LISTEN VERSION   #endif   #include <stdio.h> #include <stdlib.h>  #include <string.h>  #include <ctype.h> #include <ssdef.h> #include <signal.h>    #include <iodef.h> #include <descrip.h> #include <starlet.h>   #include "ethernet.h"  #include "sysid.h" #include "messages.h"  #include "listen_subs.h" #include "listen_table.h"  #include "listen_process.h"   
 #if __DECC #pragma member_alignment save  #pragma nomember_alignment #endif   #include "nmadef.h"     /* protocol table entry flags */ #define FLAG_READ_ACTIVE 1  " /* start protocol parameters    */ /* MOP SYSID V4 */ static struct  { + 	unsigned short codex;		/* max data size	*/  	unsigned long size;B         unsigned short code0;           /* FMT - frame format   */B         unsigned long format;           /* 802E                 */B         unsigned short code1;           /* PID                  */B         unsigned short pidlen;          /* length of PID        */B         unsigned char pid[5];           /* IEE802E MOP PID      */B         unsigned short code2;           /* MCA - multicast addr */B         unsigned short len;             /* length               */B         unsigned short modifier;        /* modifier - set addr  */B         unsigned char adr[6];           /* MOP multicast addr   */ } sidparam0 =  { 6  NMA$C_PCLI_BUS,NODE_BUFSIZE,				/* max data size   */M  NMA$C_PCLI_FMT,NMA$C_LINFM_802E,                       /* IEE 802E format */ B  NMA$C_PCLI_PID,5,0x08,0x00,0x2b,0x60,0x02,		/* MOP RC PID      */N  NMA$C_PCLI_MCA,8,NMA$C_LINMC_SET,0xAB,00,00,02,00,00};  /* broadcast addr  */   /* MOP SYSID V3 */   static struct  { + 	unsigned short codex;		/* max data size	*/  	unsigned long size;B         unsigned short code0;           /* frame format         */B         unsigned long format;           /* ethernet             */B         unsigned short code1;           /* protocol type        */B         unsigned long ptype;            /* MOP-RC               */B         unsigned short code2;           /* multicast address    */B         unsigned short len2;            /* value length         */B         unsigned short mcaflag;         /* set MCA flag         */B         unsigned char adr[6];           /* the address          */B         unsigned short code3;           /* padding flag         */B         unsigned long padflag;          /* no pad               */ } sidparam1 =  { 6  NMA$C_PCLI_BUS,NODE_BUFSIZE,				/* max data size   */M  NMA$C_PCLI_FMT,NMA$C_LINFM_ETH,                        /* Ethernet format */ M  NMA$C_PCLI_PTY,MOP_PROTOCOL_TYPE,                      /* MOP protocol    */ M  NMA$C_PCLI_MCA,8,NMA$C_LINMC_SET,0xAB,00,00,02,00,00,  /* broadcast addr  */ M  NMA$C_PCLI_PAD,NMA$C_STATE_OFF,                        /* no padding      */  };  	 /* ARP */  static struct  { + 	unsigned short codex;		/* max data size	*/  	unsigned long size; 	unsigned short codem; 	unsigned long mltstate;         unsigned short code0;          unsigned long format;          unsigned short code1;          unsigned long ptype;         unsigned short code2;          unsigned short code3;          unsigned short code4;          unsigned char adr[6];  } sidparam2 =  { 6  NMA$C_PCLI_BUS,NODE_BUFSIZE,				/* max data size   */  NMA$C_PCLI_MLT,NMA$C_STATE_ON, M  NMA$C_PCLI_FMT,NMA$C_LINFM_ETH,                        /* Ethernet format */ M  NMA$C_PCLI_PTY,ARP_PROTOCOL_TYPE,                      /* ARP protocol    */ W  NMA$C_PCLI_MCA,8,NMA$C_LINMC_SET,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}; /* broadcast addr  */   7 /* IP to broadcast address (possible netbios naming) */  static struct  { + 	unsigned short codex;		/* max data size	*/  	unsigned long size;         unsigned short code0;          unsigned long format;          unsigned short code1;          unsigned long ptype;         unsigned short code2;          unsigned short code3;          unsigned short code4;          unsigned char adr[6];  } sidparam3 =  { 6  NMA$C_PCLI_BUS,NODE_BUFSIZE,				/* max data size   */M  NMA$C_PCLI_FMT,NMA$C_LINFM_ETH,                        /* Ethernet format */ M  NMA$C_PCLI_PTY,IP_PROTOCOL_TYPE,                       /* IP protocol     */ W  NMA$C_PCLI_MCA,8,NMA$C_LINMC_SET,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}; /* broadcast addr  */   * /* DECnet IV Routing V2 end node hello  */ static struct  { + 	unsigned short codex;		/* max data size	*/  	unsigned long size;         unsigned short code0;          unsigned long format;          unsigned short code1;          unsigned long ptype;         unsigned short code2;          unsigned short code3;          unsigned short code4;          unsigned char adr[6];  } sidparam4 =  { 6  NMA$C_PCLI_BUS,NODE_BUFSIZE,				/* max data size   */M  NMA$C_PCLI_FMT,NMA$C_LINFM_ETH,                        /* Ethernet format */ M  NMA$C_PCLI_PTY,DECNETIV_PROTOCOL_TYPE,                 /* protocol        */ M  NMA$C_PCLI_MCA,8,NMA$C_LINMC_SET,0xAB,00,00,03,00,00}; /* broadcast addr  */     /* NetBEUI multicast messages	*/ static struct  { + 	unsigned short codex;		/* max data size	*/  	unsigned long size;         unsigned short code0;          unsigned long format;          unsigned short code1;          unsigned long sap;         unsigned short code2;          unsigned short code3;          unsigned short code4;          unsigned char adr[6];  } sidparam5 =  { 6  NMA$C_PCLI_BUS,NODE_BUFSIZE,				/* max data size   */M  NMA$C_PCLI_FMT,NMA$C_LINFM_802,                        /* IEE 802E format */ H  NMA$C_PCLI_SAP,NETBUEI_SAP,                      	/* NetBUEI sap     */L  NMA$C_PCLI_MCA,8,NMA$C_LINMC_SET,03,00,00,00,00,01}; 	/* broadcast addr  */    /* IPX 802 multicast messages	*/ static struct  { + 	unsigned short codex;		/* max data size	*/  	unsigned long size;         unsigned short code0;          unsigned long format;          unsigned short code1;          unsigned long sap;         unsigned short code2;          unsigned short code3;          unsigned short code4;          unsigned char adr[6];  } sidparam6 =  { 6  NMA$C_PCLI_BUS,NODE_BUFSIZE,				/* max data size   */M  NMA$C_PCLI_FMT,NMA$C_LINFM_802,                        /* IEE 802E format */ L  NMA$C_PCLI_SAP,IPX_SAP,                      	        /* NetBUEI sap     */W  NMA$C_PCLI_MCA,8,NMA$C_LINMC_SET,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}; /* broadcast addr  */   
 #if __DECC  #pragma member_alignment restore #endif  S static char *devnames[] = {"ETH","EZA0","ESA0","XQA0","ETA0","XEA0","EXA0","EWA0"};   ! static int start_protocols(void); ' static unsigned long start_reads(void);  static void process_msgs(void);   * /* the table of protocols to listen to  */ static struct  { 2         /* descriptor for start qio parameters  */         struct	         { +                 unsigned long sidparam_len; #                 void *sidparam_ptr;          } sidparam_dsc; 2         /* frame format for this protocol       */         FRAME_FORMAT fmt; 2         /* name for this protocol               */         char *name; 2         /* routine to process messages          */`         void (*process)(int node_index,FRAME_HEADER *hdr,unsigned char *msg,unsigned short len);2         /* flags                                */)         unsigned long flags;              2         /* QIO iosb                             */         unsigned short iosb[4]; 2         /* IO channel                           */         unsigned short chan;:         /* buffer for recived ethernet frame header     */         FRAME_HEADER hdr; :         /* buffer for recived ethernet frame            */+         unsigned char rcvbuf[NODE_BUFSIZE];  } protocols[] =  { "     {   /* MOPRC SYSID V3       */C         {sizeof(sidparam1),&sidparam1},FMT_ETHERNET,"MOP SYSID V3",                   process_sysid_v3     },"     {   /* MOPRC SYSID V4       */?         {sizeof(sidparam0),&sidparam0},FMT_802E,"MOP SYSID V4", !                 process_sysid_v4,      },"     {   /* ARP                  */:         {sizeof(sidparam2),&sidparam2},FMT_ETHERNET,"ARP",                 process_arp      },"     {   /* IP brodcast          */D         {sizeof(sidparam3),&sidparam3},FMT_ETHERNET,"IP(broadcast)",                 process_ip     },*     {   /* DECNET IV - end node hellos  */[         {sizeof(sidparam4),&sidparam4},FMT_ETHERNET,"DECnet IV - broadcast to all routers", !                 process_decnet_iv      },     {	/* NetBUEI multicasts		*/ = 	{sizeof(sidparam5),&sidparam5},FMT_802,"NetBUEI multicasts",  		process_netbuei      },     {	/* IPX multicasts		*/ 9 	{sizeof(sidparam6),&sidparam6},FMT_802,"IPX multicasts",  		process_ipx      }, };  ; #define NPROTOCOLS (sizeof(protocols)/sizeof(protocols[0]))  static void ctrlc(int x);  static int bye;     int main(int argc, char *argv[]) {           unsigned long ccode = 0;         unsigned long efn_mask;   @         printf("LISTEN %s starting at %s\n",VERSION,curtime());	  *         /* load up the node table       */         read_table();  	bye = FALSE;  	signal(SIGINT,ctrlc);           7 	/* enable informational messages from table routines*/          log_mask = LT_LOG_ALL;  #         if (start_protocols() == 0) 	         { M                 printf("%%LISTEN-F-NSTART, Failed to start any protocols\n");                  exit(ccode);	         }                  #         /* now read the messages */  	while(!bye) 	{*                 /* start the reads      */2                 if ((efn_mask=start_reads()) == 0)                 { Q                         printf("%%LISTEN-F-NOREAD, Failed to start any reads\n");                           exit(1);                 }   C                 /* wait for one of more of the reads to complete */ .                 ccode = sys$wflor(0,efn_mask);!                 if (!(ccode & 1))                  { L                         printf("%%LISTEN-F-WFLOR, Failed to wait for IO\n");$                         exit(ccode);                 }                    process_msgs();  	}! 	write_table();	/* write table */  }   
 static int start_protocols(void)  {  	int i,count,n;          unsigned long ccode;&         $DESCRIPTOR(devname_dsc,NULL);  .         for (i=0,count=0; i < NPROTOCOLS; i++)	         { (         	/* assign to ethernet device	*/J                 for (n = 0; n < sizeof(devnames)/sizeof(devnames[0]); n++)                 { @                         devname_dsc.dsc$a_pointer = devnames[n];G                         devname_dsc.dsc$w_length = strlen(devnames[n]); M                         ccode = sys$assign(&devname_dsc,&protocols[i].chan,0, %                                 0,0); &                         if (ccode & 1)B                                 break;  /* exit loop on success */                 } !                 if (!(ccode & 1))                  { I                         printf("Error 0x%x assigning to device\n",ccode); $                         exit(ccode);                 } D                 /* start up the channel for this procols messages */7                 ccode = sys$qiow(i+1,protocols[i].chan, ;                         IO$_SETMODE|IO$M_CTRL|IO$M_STARTUP, 0                         protocols[i].iosb,0,0,0,<                         &protocols[i].sidparam_dsc,0,0,0,0);                 if (ccode & 1)5                         ccode = protocols[i].iosb[0];                  if (ccode & 1) 		{ X                         printf("%%LISTEN-I-STARTED, started listener for protocol %s\n",3                                 protocols[i].name);  		}  		else                 { o                         printf("%%LISTEN-E-START, Error 0x%x (%x,%x,%x,%x) starting channel for protocol %s\n", P                                 ccode,protocols[i].iosb[0],protocols[i].iosb[1],J                                 protocols[i].iosb[2],protocols[i].iosb[3],3                                 protocols[i].name); - 			if ((protocols[i].iosb[0] == SS$_BADPARAM) 0 			&&  (protocols[i].iosb[2] == NMA$C_PCLI_PTY)) 			{= 				printf("-LISTEN-E-PTY,protocol may be already in use\n");  			}6                         sys$dassgn(protocols[i].chan);P                         protocols[i].chan = 0;          /* flag as not started*//                         protocols[i].flags = 0; !                         continue;                  }                  count++;	         }          return count;  }    static unsigned long start_reads(void)  {          int i;         unsigned long ccode;         unsigned long efn_mask;   &         for (i=0; i < NPROTOCOLS; i++)	         { K                 if ((protocols[i].chan != 0)     /* if protocol started  */ =                 &&  !(protocols[i].flags & FLAG_READ_ACTIVE))                  { 2                         /* clear the buffer     */C                         memset(protocols[i].rcvbuf,0,NODE_BUFSIZE); 2                         /* start the read       */D         	        ccode = sys$qio(i+1,protocols[i].chan,IO$_READVBLK,6                                 protocols[i].iosb,0,0,, 			        protocols[i].rcvbuf,NODE_BUFSIZE,9                                 0,0,&protocols[i].hdr,0); -                         if ((ccode & 1) == 0)                          { c                                 printf("%%LISTEN-F-READERR, Error starting read for protocol %s\n", ;                                         protocols[i].name); ,                                 exit(ccode);                         } @                         protocols[i].flags |=  FLAG_READ_ACTIVE;                 } 	         }          efn_mask = 0; &         for (i=0; i < NPROTOCOLS; i++)	         { :                 if (protocols[i].flags & FLAG_READ_ACTIVE)/                         efn_mask |= 1 << (i+1); 	         }          return efn_mask; }    static void  process_msgs(void) {          int i, n;          unsigned short now[4];  &         for (i=0; i < NPROTOCOLS; i++)	         { ,                 if ((protocols[i].chan != 0);                 &&  (protocols[i].flags & FLAG_READ_ACTIVE) 0                 &&  (protocols[i].iosb[0] != 0))                 { @                         protocols[i].flags &= ~FLAG_READ_ACTIVE;5                         if (protocols[i].iosb[0] & 1)                          { B                                 /* message read - process it    */6                                 sys$gettim(&now);     B                                 /* add to table                 */N                                 n = add_node(protocols[i].hdr.hdr_eth.source);+                                 if (n >= 0) !                                 {  					memcpy(nodes[n].time,now, 						sizeof(nodes[n].time)); Q                                         protocols[i].process(n,&protocols[i].hdr, Z                                                 protocols[i].rcvbuf,protocols[i].iosb[1]);!                                 }                          }                          else                         { `                                 printf("%%LISTEN-E-RDERR, Error 0x%x,%x,%x,%x reading for %s\n",R                                         protocols[i].iosb[0],protocols[i].iosb[1],R                                         protocols[i].iosb[2],protocols[i].iosb[3],;                                         protocols[i].name); ;                                 exit(protocols[i].iosb[0]);                          }                  } 	         }  }    static void  ctrlc(int x) {  	signal(SIGINT,ctrlc); 	bye = TRUE; } 