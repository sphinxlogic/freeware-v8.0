% #pragma module MBXDEMO "SRH X1.0-000"  /* ** COPYRIGHT (c) 1993, 1999 BY9 ** DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASSACHUSETTS.  ** ALL RIGHTS RESERVED.  **H ** THIS SOFTWARE IS FURNISHED UNDER A LICENSE AND MAY BE USED AND COPIEDH ** ONLY  IN  ACCORDANCE  OF  THE  TERMS  OF  SUCH  LICENSE  AND WITH THEH ** INCLUSION OF THE ABOVE COPYRIGHT NOTICE. THIS SOFTWARE OR  ANY  OTHERH ** COPIES THEREOF MAY NOT BE PROVIDED OR OTHERWISE MADE AVAILABLE TO ANYH ** OTHER PERSON.  NO TITLE TO AND  OWNERSHIP OF THE  SOFTWARE IS  HEREBY ** TRANSFERRED.  **H ** THE INFORMATION IN THIS SOFTWARE IS  SUBJECT TO CHANGE WITHOUT NOTICEH ** AND  SHOULD  NOT  BE  CONSTRUED  AS A COMMITMENT BY DIGITAL EQUIPMENT ** CORPORATION.  **H ** DIGITAL ASSUMES NO RESPONSIBILITY FOR THE USE  OR  RELIABILITY OF ITS: ** SOFTWARE ON EQUIPMENT WHICH IS NOT SUPPLIED BY DIGITAL. */   /* **++ **  Facility:  ** **	Examples  ** **  Version: V1.0  ** **  Abstract:  **4 **	Shows some master-slave mailbox operations from C ** **  Author:  **	Brian Breathnach  **  **  Creation Date:  ??-Mar-1993? ** **  Modification History:  **	11-Mar-1993 Steve Hoffman1 **	    Heavily modified and reorganized, debugged  **	27-Dec-1999 Steve HoffmanF **          Updates for added $creprc arguments, and for Compaq C V6.2 ** **-- */   #include <ctype.h> #include <descrip.h> #include <dvidef.h>  #include <lib$routines.h>  #include <rms.h> #include <prcdef.h>  #include <iodef.h> #include <ssdef.h> #include <starlet.h> #include <stdio.h> #include <stdlib.h>  #include <string.h>  #include <stsdef.h>   $ typedef unsigned short int ushort_t; typedef short int short_t;" typedef unsigned long int ulong_t; typedef long int long_t; typedef unsigned char uchar_t; typedef char char_t; typedef struct     {      ushort_t cond_value;     ushort_t count;      ulong_t info;      } mbxiosb_t; typedef struct itemlist_3      {      ushort_t itmlen, itmcod;     void *itmbuf;      ushort_t *itmrla;      } itemlist_3_t;  #define MAXSCSNAMLEN	8 #define MAXDEVNAMLEN	16  #define MAXMBXMSG	1024 #define MAXMBXBUF       4096 #define MBXNAM		"MBXDEMOMBX" #define SLAVENAM	"MBXDEMO"D #define SLAVEEXE	"SYS$SYSDEVICE:[HOFFMAN.EXAMPLES]MBXDEMO_SLAVE.EXE"D #define SLAVELOG	"SYS$SYSDEVICE:[HOFFMAN.EXAMPLES]MBXDEMO_SLAVE.LOG"D #define SLAVEDAT	"SYS$SYSDEVICE:[HOFFMAN.EXAMPLES]MBXDEMO_SLAVE.DAT"   #ifdef MASTER  main()     {      mbxiosb_t iosb;      ulong_t pid = 0;     ulong_t status;      ushort_t channel;      ushort_t ret_len;      FILE *fd; "     char_t devnam_b[MAXDEVNAMLEN];"     char_t scsnam_b[MAXSCSNAMLEN];$     struct dsc$descriptor scsnam_d =	         { 5 	MAXSCSNAMLEN, DSC$K_DTYPE_T, DSC$K_CLASS_S, scsnam_b  	}; $     struct dsc$descriptor devnam_d = 	{5 	MAXDEVNAMLEN, DSC$K_DTYPE_T, DSC$K_CLASS_S, devnam_b  	};      itemlist_3_t getdvi_il[2] = 	         { H         MAXDEVNAMLEN, DVI$_FULLDEVNAM, devnam_b, &devnam_d.dsc$w_length,         0, 0, 0, 0
         };      char_t *datfil_b = SLAVEDAT;%     $DESCRIPTOR( mbxnam_d,  MBXNAM ); %     $DESCRIPTOR( nla0_d,    "NLA0:"); '     $DESCRIPTOR( imgfnm_d,  SLAVEEXE ); '     $DESCRIPTOR( logfnm_d,  SLAVELOG ); '     $DESCRIPTOR( prcnam_d,  SLAVENAM );          /*4     **	Write the selection parameters out to a file.     **	First, open the file.     */     fd = fopen(datfil_b, "w");     if (fd == 0) 	return SS$_BADPARAM;        /*(     **	Write out some details to a file.     */,     fprintf(fd, "Program: %s\n", __FILE__ );.     fprintf(fd, "Time1: %s\n", "17-Nov-1858");.     fprintf(fd, "Time2: %s\n", "19-Jan-2038");       /**     **	Close the selection parameter file.     */     fclose(fd);        /*$     **  Get the current host name...     */7     /* status = lib$get_hostname( &scsnam_d, 0, 0 ); */ F     status = lib$get_hostname( &scsnam_d, &scsnam_d.dsc$w_length, 0 );%     if (!$VMS_STATUS_SUCCESS(status))  	return status;        /*J     **	Create a mailbox to enable communication with the detached process.     */:     status = sys$crembx(0, &channel, MAXMBXMSG, MAXMBXBUF, 	0, 0, &mbxnam_d, 0); %     if (!$VMS_STATUS_SUCCESS(status))  	return status; A     status = sys$getdviw( 0, channel, 0, getdvi_il, 0, 0, 0, 0 ); '     if (!$VMS_STATUS_SUCCESS( status ))          return status;       /*:     **	Create a detached process running image imgfnm_d...     **A     **	Can be enabled or disabled under the debugger -- deposit a E     **	zero/non-zero value here as required.  (Note that the detached A     **	process should *not* be linked with DEBUG, as the debugger ?     **	has no way of obtaining reasonable input in the detached A     **	process... Other than eating commands from the input mbx.)      */     status = 0;      if (status == 0) 	{ 	status = sys$creprc(  	    &pid,	    /* PIDADR */   	    &imgfnm_d,	    /* IMGDSC */ 	    &devnam_d,	    /* Input */   	    &logfnm_d,	    /* Output */ 	    &nla0_d,	    /* error */  	    0,		    /* prvadr */  	    0,		    /* quota */  	    &prcnam_d,	    /* PRNDSC */ 	    4,		    /* Baspri */  	    0,		    /* Uic */ 	    0,		    /* TrmMbxUnt */! 	    PRC$M_DETACH,   /* Stsflg */  	    NULL,	    /* Item list */ 	    &scsnam_d ); # 	if ( !$VMS_STATUS_SUCCESS(status))  	    return status; * 	printf("created proces PID 0x0%x\n",pid); 	}     /*I     **	Store the name of the parameter file in the message to go into the      **	mailbox.      */     status = sys$qio( 0,	 	channel,  	IO$_WRITEVBLK,  	&iosb,  	0, 0,  + 	datfil_b, strlen( datfil_b ), 0, 0, 0, 0); %     if (!$VMS_STATUS_SUCCESS(status))  	return status; #     status = sys$synch( 0, &iosb ); %     if (!$VMS_STATUS_SUCCESS(status))  	return status; .     if (!$VMS_STATUS_SUCCESS(iosb.cond_value)) 	return iosb.cond_value;       return SS$_NORMAL;     }  #endif   #ifdef SLAVE main()     {      mbxiosb_t iosb;      ushort_t channel; (     $DESCRIPTOR( sysinp_d, "SYS$INPUT");"     uchar_t input_b[NAM$C_MAXRSS];     ulong_t status;   9     printf("MBXDEMO detached/slave process starting!\n");   	     /*	   C     **	Get a pointer to where the selection parameters are.  Assign B     **	a channel to the SYS$INPUT device -- which is assumed to beA     **	a mail box device -- and read the input from the mailbox.) 	     */	   8     status = sys$assign( &sysinp_d, &channel, 0, 0, 0 );%     if (!$VMS_STATUS_SUCCESS(status))          return status;       /*1     **	Read from the input device -- the mailbox.      */     status = sys$qio(0,  		      channel, 		      IO$_READVBLK,  		      &iosb,
 		      0,
 		      0,+ 		      input_b, NAM$C_MAXRSS, 0, 0, 0, 0); %     if (!$VMS_STATUS_SUCCESS(status))          return status;#     status = sys$synch( 0, &iosb ); %     if (!$VMS_STATUS_SUCCESS(status))          return status;.     if (!$VMS_STATUS_SUCCESS(iosb.cond_value))           return iosb.cond_value;  *     printf("%*s\n", iosb.count, input_b );     printf("Done!\n");       return SS$_NORMAL;     }  #endif��                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            