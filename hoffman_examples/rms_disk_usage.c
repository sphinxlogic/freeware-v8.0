   /*$ // COPYRIGHT (c) 1992, 1993, 1996 BY9 // DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASSACHUSETTS.  // ALL RIGHTS RESERVED.  //H // THIS SOFTWARE IS FURNISHED UNDER A LICENSE AND MAY BE USED AND COPIEDH // ONLY  IN  ACCORDANCE  OF  THE  TERMS  OF  SUCH  LICENSE  AND WITH THEH // INCLUSION OF THE ABOVE COPYRIGHT NOTICE. THIS SOFTWARE OR  ANY  OTHERH // COPIES THEREOF MAY NOT BE PROVIDED OR OTHERWISE MADE AVAILABLE TO ANYH // OTHER PERSON.  NO TITLE TO AND  OWNERSHIP OF THE  SOFTWARE IS  HEREBY // TRANSFERRED.  //H // THE INFORMATION IN THIS SOFTWARE IS  SUBJECT TO CHANGE WITHOUT NOTICEH // AND  SHOULD  NOT  BE  CONSTRUED  AS A COMMITMENT BY DIGITAL EQUIPMENT // CORPORATION.  //H // DIGITAL ASSUMES NO RESPONSIBILITY FOR THE USE  OR  RELIABILITY OF ITS: // SOFTWARE ON EQUIPMENT WHICH IS NOT SUPPLIED BY DIGITAL. // // // // //  Facility:  // //	Examples  // //  Version: V1.0  // //  Abstract:  /// //	An example of working with RMS calls from C.  //: //	Dumps out the contents of an RMS Disk Usage (RDU) file,7 //	using the OpenVMS-defined USGDEF format definitions.  //8 //	This code has not been tested on OpenVMS VAX versions/ //	prior to OpenVMS V7.0, nor on OpenVMS Alpha.  // //      Requires DEC C.  //7 //      To build and use this tool (on V6.2 and later): 4 //        $ CC /DECC [/DEBUG/NOOPTIM] RMS_DISK_USAGE( //        $ LINK [/DEBUG] RMS_DISK_USAGE@ //        $ DEFINE/JOB DCL$PATH SYS$DISK:[],SYS$SYSTEM:,SYS$ETC:1 //        $ ANALYZE/DISK/USAGE=<filename> <ddcu:> % //        $ RMS_DISK_USAGE <filename>  // //  Author: " //	Steve Hoffman (XDELTA::HOFFMAN)9 //	(getopt code written by MOVIES::HOWELL_MA, I think...)  // //  Creation Date:  23-Mar-1996  // //  Modification History:  //-- */     #include <descrip.h> #include <lib$routines.h>  #include <rms.h> #include <starlet.h> #include <string.h>  #include <ssdef.h> #include <stdio.h> #include <stsdef.h>  #include <usgdef.h>    #if __VMS_VER < 70000000 extern int opterr = 1; extern int optind = 1; #endif extern int optopt; extern char *optarg;   /*< // The following is the core data structure for the program.< // The various RMS subroutines all communicate via a pointer // referencing this struct.  */ struct RDUFileContext      {      struct FAB fab1;     struct RAB rab1;     struct XABFHC xabfhc1;     struct NAM nam1;+     unsigned long int RunningTotalUsage[2]; +     unsigned long int RunningTotalAlloc[2];      char rss1[NAM$C_MAXRSS];      unsigned long int MaxRecSiz;     char *RecBuf;      };   /*L //  Open the specified file, picking up various file attributes dynamically. */ RDUFileOpen( void **CtxArg, 7 char *FileName, char *DefFileName, int flags, int rss )      {      int RetStat;     struct RDUFileContext *Ctx; 1     int howbig = sizeof( struct RDUFileContext );        /*.     // acquire some space for a Context block.     */-     RetStat = lib$get_vm( &howbig, &Ctx, 0 );   *     if ( !$VMS_STATUS_SUCCESS( RetStat ) ) 	return RetStat;       *CtxArg = (void *) Ctx;        /*7     // Fill in the various fields of the Context block. ?     // -- Builds the File Access Block (FAB), the Record Access =     // Block (RAB) and the Name (NAM) Block.  Along with some .     // other miscellaneous housekeeping stuff.     */     Ctx->fab1 = cc$rms_fab;      Ctx->rab1 = cc$rms_rab;      Ctx->nam1 = cc$rms_nam; !     Ctx->xabfhc1 = cc$rms_xabfhc;   %     Ctx->fab1.fab$l_nam = &Ctx->nam1; $     Ctx->fab1.fab$l_fop = FAB$M_NAM;$     Ctx->fab1.fab$b_fac = FAB$M_GET;  #     Ctx->fab1.fab$l_fna = FileName; -     Ctx->fab1.fab$b_fns = strlen( FileName ); &     Ctx->fab1.fab$l_dna = DefFileName;0     Ctx->fab1.fab$b_dns = strlen( DefFileName );  %     Ctx->rab1.rab$l_fab = &Ctx->fab1; (     Ctx->fab1.fab$l_xab = &Ctx->xabfhc1;  '     Ctx->nam1.nam$b_rss = NAM$C_MAXRSS; $     Ctx->nam1.nam$l_rsa = Ctx->rss1;  $     Ctx->rab1.rab$b_rac = RAB$C_SEQ;  >     Ctx->RunningTotalUsage[0] = Ctx->RunningTotalUsage[1] = 0;>     Ctx->RunningTotalAlloc[0] = Ctx->RunningTotalAlloc[1] = 0;       /*"     // Attempt to open the file...     */+     RetStat = sys$open( &Ctx->fab1, 0, 0 ); *     if ( !$VMS_STATUS_SUCCESS( RetStat ) ) 	return RetStat;       /*I     // Allocate a buffer (hopefully) large enough for the biggest record.      */!     if ( Ctx->xabfhc1.xab$w_lrl ) ) 	Ctx->MaxRecSiz = Ctx->xabfhc1.xab$w_lrl;      else 	Ctx->MaxRecSiz = 32767;=     RetStat = lib$get_vm( &Ctx->MaxRecSiz, &Ctx->RecBuf, 0 ); *     if ( !$VMS_STATUS_SUCCESS( RetStat ) ) 	return RetStat;  &     Ctx->rab1.rab$l_ubf = Ctx->RecBuf;)     Ctx->rab1.rab$w_usz = Ctx->MaxRecSiz;        /*:     // Attempt to connect the record stream to the file...     */.     RetStat = sys$connect( &Ctx->rab1, 0, 0 );*     if ( !$VMS_STATUS_SUCCESS( RetStat ) ) 	return RetStat;       return RetStat;      }      /** //  Read a record from the specified file. */8 RDUFileRead( void **CtxArg, char **BufAdr, int *BufLen )     {      int RetStat;)     struct RDUFileContext *Ctx = *CtxArg;   *     RetStat = sys$get( &Ctx->rab1, 0, 0 );  *     if ( !$VMS_STATUS_SUCCESS( RetStat ) ) 	{         *BufLen = (char) 0;  	*BufAdr = (char) 0; 	return RetStat; 	}  "     *BufAdr = Ctx->rab1.rab$l_rbf;"     *BufLen = Ctx->rab1.rab$w_rsz;       return RetStat;      }    /*; //  Close the specified file, cleang up various structures.  */ RDUFileClose( void **CtxArg )      {      int RetStat;)     struct RDUFileContext *Ctx = *CtxArg;        /*#     // Free up the record buffer...      */>     RetStat = lib$free_vm( &Ctx->MaxRecSiz, &Ctx->RecBuf, 0 );*     if ( !$VMS_STATUS_SUCCESS( RetStat ) ) 	return RetStat;       /*0     // Be nice and clean up the record stream...     */1     RetStat = sys$disconnect( &Ctx->rab1, 0, 0 ); *     if ( !$VMS_STATUS_SUCCESS( RetStat ) ) 	return RetStat;       /*     // And close the file...     */,     RetStat = sys$close( &Ctx->fab1, 0, 0 );*     if ( !$VMS_STATUS_SUCCESS( RetStat ) ) 	return RetStat;       /**     // And free up the allocated memory...     */I     RetStat = lib$free_vm( &sizeof( struct RDUFileContext ), CtxArg, 0 ); *     if ( !$VMS_STATUS_SUCCESS( RetStat ) ) 	return RetStat;       return RetStat;        }    /*F //  Dump the total usage.  If the disk size (in blocks) is over a fullG //  longword in length, switch to hexidecimal notation for the display.  */" int RDUDumpTotals( void **CtxArg )     {      int RetStat;)     struct RDUFileContext *Ctx = *CtxArg;   A     if ( Ctx->RunningTotalUsage[1] || Ctx->RunningTotalAlloc[1] )  	{b 	printf( "Total Usage: 0x0%08.8x%08.8x\n", Ctx->RunningTotalUsage[1], Ctx->RunningTotalUsage[0] );b 	printf( "Total Alloc: 0x0%08.8x%08.8x\n", Ctx->RunningTotalAlloc[1], Ctx->RunningTotalAlloc[0] ); 	}     else 	{: 	printf( "Total Usage: %d\n", Ctx->RunningTotalUsage[0] );: 	printf( "Total Alloc: %d\n", Ctx->RunningTotalAlloc[0] ); 	}     return SS$_NORMAL;     }    /*! //  Dump out a usage file record.  */L int RDUDumpEntry( void **CtxArg, char *BufAdr, int BufLen, int VerboseFlag )     {      int RetStat;)     struct RDUFileContext *Ctx = *CtxArg; #     unsigned long int FileUsage[2]; #     unsigned long int FileAlloc[2];  #define MAXTIMLEN 23     unsigned short int TimLen;!     struct dsc$descriptor TimDsc;      char TimBuf[MAXTIMLEN]; 7     struct usgdef *UsgIdent = (struct usgdef *) BufAdr; 8     struct usgdef1 *UsgFile = (struct usgdef1 *) BufAdr;  #     switch ( UsgIdent->usg$b_type )  	{ 	case USG$K_IDENT:% 	    TimDsc.dsc$w_length = MAXTIMLEN; ( 	    TimDsc.dsc$b_dtype = DSC$K_DTYPE_T;( 	    TimDsc.dsc$b_class = DSC$K_CLASS_S;, 	    TimDsc.dsc$a_pointer = (void *) TimBuf;G 	    RetStat = sys$asctim( &TimLen, &TimDsc, UsgIdent->usg$q_time, 0 ); ) 	    if (!$VMS_STATUS_SUCCESS( RetStat ))  		return RetStat; , 	    printf( "Record:     USG$K_IDENT \n" );@ 	    printf( "serialnum   0x0%x\n", UsgIdent->usg$l_serialnum );F 	    printf( "structname  %*.*s\n", 12,12,UsgIdent->usg$t_strucname );D 	    printf( "volname     %*.*s\n", 12,12,UsgIdent->usg$t_volname );F 	    printf( "ownername   %*.*s\n", 12,12,UsgIdent->usg$t_ownername );C 	    printf( "format      %*.*s\n", 12,12,UsgIdent->usg$t_format ); = 	    printf( "time        %*.*s\n", TimLen, TimLen, TimBuf );  	    printf( " \n"); 	    break;  	case USG$K_FILE: ( 	    FileUsage[0] = UsgFile->usg$l_used;- 	    FileAlloc[0] = UsgFile->usg$l_allocated; % 	    FileAlloc[1] = FileUsage[1] = 0; Y 	    RetStat = lib$addx( FileUsage, Ctx->RunningTotalUsage, Ctx->RunningTotalUsage, &2 ); ) 	    if (!$VMS_STATUS_SUCCESS( RetStat ))  		return RetStat; Y 	    RetStat = lib$addx( FileAlloc, Ctx->RunningTotalAlloc, Ctx->RunningTotalAlloc, &2 ); ) 	    if (!$VMS_STATUS_SUCCESS( RetStat ))  		return RetStat;    	    if ( VerboseFlag )  		{ ( 		printf( "Record:     USG$K_FILE \n" );W 		printf("owner        [%o,%o]\n", UsgFile->usg$w_uicgroup, UsgFile->usg$w_uicmember ); 9 		printf("allocated    %d\n", UsgFile->usg$l_allocated ); 4 		printf("used         %d\n", UsgFile->usg$l_used );m 		printf("filename     %*.*s\n", UsgFile->usg$w_spec_len, UsgFile->usg$w_spec_len, UsgFile->usg$t_filespec );  		printf(" \n"); 		}  	      	    break; 	 	default: ) 	    printf( "Record:     <unknown>\n" ); ; 	    printf( "usg$b_type: 0x0%x\n", UsgIdent->usg$b_type );  	    printf(" \n");  	    break;  	}     return SS$_NORMAL;     }      #if __VMS_VER < 70000000 /*E //  getopt is compatible with and can be replaced by the getopt() RTL F //  call on OpenVMS V7.0 and later.  Prior to that release, we use our) //  own ("borrowed") copy of the routine.  */ #define ERR(s, c) \      if (opterr) {\         char errbuf[2];\0         errbuf[0] = (char) c; errbuf[1] = '\n';\>         (void) fwrite (argv[0], strlen (argv[0]), 1, stderr);\2         (void) fwrite (s, strlen (s), 1, stderr);\.         (void) fwrite (errbuf, 2, 1, stderr);\     }   5 int getopt( int argc, char **argv, const char *opts )  {      static int sp = 1;
     int c;     char *cp;        if (sp == 1)P         if (optind >= argc || argv[optind][0] != '-' || argv[optind][1] == '\0')             return (EOF); 4         else if (strcmp (argv[optind], "--") == 0) {             optind++;              return (EOF); 	         } "     optopt = c = argv[optind][sp];6     if (c == ':' || (cp = strchr (opts, c)) == NULL) {(         ERR (": illegal option -- ", c);)         if (argv[optind][++sp] == '\0') {              optind++;              sp = 1; 	         }          return ('?');      }      if (*++cp == ':') { '         if (argv[optind][sp+1] != '\0') +             optarg = &argv[optind++][sp+1]; $         else if (++optind >= argc) {9             ERR (": option requires an argument -- ", c);              sp = 1;              return ('?');          } else$             optarg = argv[optind++];         sp = 1;      } else {)         if (argv[optind][++sp] == '\0') {              sp = 1;              optind++; 	         }          optarg = NULL;     }        return (c);    }/* getopt */  #endif   main( int argc, char **argv )      {      int RetStat;     void *Context;     char *BufAdr;      int BufLen; 
     int c;     int VerboseFlag = 0;     int PrintUsageFlag = 0; 9     char FileName[NAM$C_MAXRSS] = "SYS$DISK:[]USAGE.DAT";   5     while ((c = getopt( argc, argv, "?f:v" )) != EOF)          switch (c) 	    { 	    case 'f': 		/*, 		//  option f is the filename.  Defaults to, 		//  SYS$DISK:[]USAGE.DAT if not specified. 		*/ 		strcpy( FileName, optarg );  		break; 	    case 'v': 		/*, 		//  Control the "chattiness" of this tool. 		*/ 		VerboseFlag++; 		break; 	    default:  	    case '?': 		PrintUsageFlag++;  		break; 		}        if ( PrintUsageFlag )  	{: 	printf("analyze/disk/usage=UsageFile usage file dump\n");5 	printf("usage: %s [-f UsageFile] [-v]\n", argv[0] );  	return SS$_BADPARAM;  	}       /*H     // Open the file.  Minimal checking is performed.  Read access only.     */N     RetStat = RDUFileOpen( &Context, FileName, "SYS$DISK:[]USAGE.DAT", 0, 0 );*     if ( !$VMS_STATUS_SUCCESS( RetStat ) ) 	return RetStat;       /*5     // Read the file.  Minimal checking is performed.      */     for (;;) 	{5 	RetStat = RDUFileRead( &Context, &BufAdr, &BufLen ); & 	if ( $VMS_STATUS_SUCCESS( RetStat ) )E 	    RetStat = RDUDumpEntry( &Context, BufAdr, BufLen, VerboseFlag );  	else  	    break;  	}  (     RetStat = RDUDumpTotals( &Context );       /*     // Close up shop.      */'     RetStat = RDUFileClose( &Context );        return RetStat;      }   ��                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          