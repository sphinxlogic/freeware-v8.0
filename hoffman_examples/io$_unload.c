 /* ** COPYRIGHT (c) 1994 BY9 ** DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASSACHUSETTS.  ** ALL RIGHTS RESERVED.  **H ** THIS SOFTWARE IS FURNISHED UNDER A LICENSE AND MAY BE USED AND COPIEDH ** ONLY  IN  ACCORDANCE  OF  THE  TERMS  OF  SUCH  LICENSE  AND WITH THEH ** INCLUSION OF THE ABOVE COPYRIGHT NOTICE. THIS SOFTWARE OR  ANY  OTHERH ** COPIES THEREOF MAY NOT BE PROVIDED OR OTHERWISE MADE AVAILABLE TO ANYH ** OTHER PERSON.  NO TITLE TO AND  OWNERSHIP OF THE  SOFTWARE IS  HEREBY ** TRANSFERRED.  **H ** THE INFORMATION IN THIS SOFTWARE IS  SUBJECT TO CHANGE WITHOUT NOTICEH ** AND  SHOULD  NOT  BE  CONSTRUED  AS A COMMITMENT BY DIGITAL EQUIPMENT ** CORPORATION.  **H ** DIGITAL ASSUMES NO RESPONSIBILITY FOR THE USE  OR  RELIABILITY OF ITS: ** SOFTWARE ON EQUIPMENT WHICH IS NOT SUPPLIED BY DIGITAL. */   /* **++ **  Facility:  ** **	Examples  ** **  Version: V1.0  ** **  Abstract:  **& **	Example of a tape unload operation. ** **  Author:  **	Steve Hoffman ** **  Creation Date:  26-Aug-1994  ** **  Modification History:  **-- */   #include    iodef  #include    ssdef  #include    stsdef #include    descrip  #include    starlet  #include    lib$routines   main()     {      int lRetstat;       unsigned short int qIosb[4];      unsigned short int sChannel;#     $DESCRIPTOR( dTape, "MKA500:");   5     lRetstat = sys$assign( &dTape, &sChannel, 0, 0 ); ;     if ( !$VMS_STATUS_SUCCESS( lRetstat )) return lRetstat;   ?     lRetstat = sys$qiow( 0, sChannel, IO$_PACKACK, qIosb, 0, 0,  	0, 0, 0, 0, 0, 0 );;     if ( !$VMS_STATUS_SUCCESS( lRetstat )) return lRetstat; ;     if ( !$VMS_STATUS_SUCCESS( qIosb[0] )) return qIosb[0];   >     lRetstat = sys$qiow( 0, sChannel, IO$_UNLOAD, qIosb, 0, 0, 	0, 0, 0, 0, 0, 0 );;     if ( !$VMS_STATUS_SUCCESS( lRetstat )) return lRetstat; ;     if ( !$VMS_STATUS_SUCCESS( qIosb[0] )) return qIosb[0];        return lRetstat;     } ��                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          