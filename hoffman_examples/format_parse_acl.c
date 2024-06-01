< //  Copyright 2006 Hewlett-Packard Development Company, L.P. //5 //  Calling sys$parse_acl and sys$format_acl, anyone?  //3 //  Author: Stephen Hoffman, HP OpenVMS Engineering  //   #include        <acedef.h> #include        <acldef.h> #include        <descrip.h>   #include        <lib$routines.h> #include        <ssdef.h>  #include        <starlet.h>  #include        <stdio.h>  #include        <stsdef.h>   #define MAXACE 256   main( int argc, char *argv[] )   {    int RetStat;?   $DESCRIPTOR( AceBefore, "(IDENTIFIER=[SYSTEM],ACCESS=READ)");    char AceRawBuf[MAXACE];    char AceAfterBuf[MAXACE];   K   struct dsc$descriptor AceRaw = {MAXACE,DSC$K_DTYPE_T,DSC$K_CLASS_S,NULL}; M   struct dsc$descriptor AceAfter = {MAXACE,DSC$K_DTYPE_T,DSC$K_CLASS_S,NULL};      /*   //  Display the source...    */   lib$put_output( &AceBefore );   ,   AceRaw.dsc$a_pointer = (void *) AceRawBuf;0   AceAfter.dsc$a_pointer = (void *) AceAfterBuf;     /*(   // convert the ACE from text to binary   */&   RetStat = sys$parse_acl( &AceBefore,     &AceRaw,     NULL, NULL, NULL ); &   if (!$VMS_STATUS_SUCCESS( RetStat ))     lib$signal( RetStat );     /*@   // the first byte of the raw ACE contains the total ACE length   */%   AceRaw.dsc$w_length = AceRawBuf[0];      /*2   // convert the ACE from binary to text format...   */$   RetStat = sys$format_acl( &AceRaw,&     &AceAfter.dsc$w_length, &AceAfter,#     NULL, NULL, NULL, NULL, NULL ); &   if (!$VMS_STATUS_SUCCESS( RetStat ))     lib$signal( RetStat );     /*   //  Display the results...   */   lib$put_output( &AceAfter );     return SS$_NORMAL;   } 