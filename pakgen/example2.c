 /* // COPYRIGHT (c) 1992,1994 BY 9 // DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASSACHUSETTS.  // ALL RIGHTS RESERVED.  //|// THIS SOFTWARE IS FURNISHED UNDER A LICENSE AND MAY BE USED AND COPIED // ONLY  IN  ACCORDANCE  OF  THE  TERMS  OF  SUCH  LICENSE  AND WITH THE // INCLUSION OF THE ABOVE COPYRIGHT NOTICE. THIS SOFTWARE OR  ANY  OTHER // COPIES THEREOF MAY NOT BE PROVIDED OR OTHERWISE MADE AVAILABLE TO ANY // OTHER PERSON.  NO TITLE TO AND  OWNERSHIP OF THE  SOFTWARE IS  HEREBY // TRANSFERRED. //� // THE INFORMATION IN THIS SOFTWARE IS  SUBJECT TO CHANGE WITHOUT NOTICE // AND  SHOULD  NOT  BE  CONSTRUED  AS A COMMITMENT BY DIGITAL EQUIPMENT // CORPORATION.  //� // DIGITAL ASSUMES NO RESPONSIBILITY FOR THE USE  OR  RELIABILITY OF ITS // SOFTWARE ON EQUIPMENT WHICH IS NOT SUPPLIED BY DIGITAL.  */   /* //++ //  Facility:  // //	Examples  // //  Version: V1.1  // //  Abstract:  //, //	Example of working with the DEC LMF calls //8 //	this routine is used to check for a hardwired product7 //	license.  If the license is not located, the routine 8 //	converts the lmf-generated signal to a return status. // //  Author:  //	Steve Hoffman // //  Creation Date:  1-Jan-1990 // //  Modification History: @ //	Stephen Hoffman	    15-Sep-1994	    hoffman@star.enet.dec.com: //	    updated to DEC C, added signal handling, added user: //	    output -- this output indicates success or failure. //-- */   /*C //  The following four definitions allow any product to be checked: B //  Bogus product licenses should engender license-related errors.B //  Check for existing, valid product licenses should be accepted. */ #define	PRODUCT		"MYPROGRAM" #define	PRODUCER	"FREEWARE"  #define	PROD_VERSION	0x010000  #define PROD_DATE	"1-JAN-1988"   /*C /* Definitions required to get off the ground:  (note the LMF stuff , /* does not yet have a C header file...) */  #include <descrip.h>   #include <lib$routines.h>  #include <signal.h>  #include <ssdef.h>   #include <starlet.h>   #include <stdio.h>   #include <stsdef.h>    #ifdef __cplusplus     extern "C" { #define __unknown_params ... #else  #define __unknown_params #endif  & #define LMF$M_RETURN_FAILURES	    0x01( #define LMF$M_BROADCAST_FAILURES    0x02 #define LMF$_PROD_VERSION	    3  #define LMF$_PROD_DATE		    4   	 int lmf()      {      int retstat;     /*Q     // the following may eventually be needed for full sys$grant/lookup_license() 6     // support.  Right now they're just extra baggage.     //     //    int context = 0;G     //    int flags = LMF$M_RETURN_FAILURES | LMF$M_BROADCAST_FAILURES;      */     int SYS$GRANT_LICENSE();$     $DESCRIPTOR( product, PRODUCT );&     $DESCRIPTOR( producer, PRODUCER );     int context = 0;(     $DESCRIPTOR( prod_date, PROD_DATE );$     int prod_version = PROD_VERSION;     char bintim[8]; 
     int n;     struct itmlst_3  	{ 	short buflen; 	short itmcod; 	char *bufadr; 	short *bufrla;  	} itmlst[3]; .     int sys$grant_license( __unknown_params );       /*@     // Fill in the item list required by the call.  Includes the*     // required final, zero-filled, block.     */
     n = 0;     itmlst[n].buflen = 8; &     itmlst[n].itmcod = LMF$_PROD_DATE;'     itmlst[n].bufadr = (char *) bintim;      itmlst[n++].bufrla = NULL;       itmlst[n].buflen = 4; )     itmlst[n].itmcod = LMF$_PROD_VERSION; .     itmlst[n].bufadr = (char *) &prod_version;     itmlst[n++].bufrla = NULL;       itmlst[n].buflen = 0;      itmlst[n].itmcod = 0;      itmlst[n].bufadr = NULL;     itmlst[n++].bufrla = NULL;       /*C     // convert the time from ASCII to the internal quadword format.      *//     retstat = sys$bintim( &prod_date, bintim );       if ( retstat != SS$_NORMAL ) 	lib$signal( retstat );        /*!     // The LMF signals on errors.      */%     VAXC$ESTABLISH( lib$sig_to_ret ); I     retstat = sys$grant_license( &product, &producer, context, &itmlst );        return retstat;      }    /*6 // Here's what a really bogus main program looks like: */ main()     {      int lmf();     int retstat;  H     printf("Checking for product %s %s license.\n", PRODUCER, PRODUCT );       retstat = lmf();     switch( retstat )  	{ 	case SS$_NOLICENSE:, 	case 0x0a948af4:    /* "LICENSE$_NOAUTH" */5 	    printf("Product is not currently licensed!\n" );  	    break;  	case SS$_NORMAL: 3 	    printf("Product is reported as licensed!\n" );  	    break; 	 	default: 2 	    printf("Product sys$grant_license error!\n");8 	    printf("...return status was: 0x0%x.\n", retstat ); 	    break;  	}  L     printf("End checking for product %s %s license.\n", PRODUCER, PRODUCT );     return SS$_NORMAL;     }   