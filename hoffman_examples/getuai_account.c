; /  Copyright 2006 Hewlett-Packard Development Company, L.P.  //5 //  An example of retrieving the account via $getuai.  //3 //  Author: Stephen Hoffman, HP OpenVMS Engineering  //   #include <descrip.h> #include <ssdef.h> #include <starlet.h> #include <stdio.h> #include <stsdef.h>  #include <uaidef.h>  #include <unixlib.h>   #define MAXACCLEN 16   struct ItemList3     {      short int ItemLength;      short int ItemCode;      void *ItemBuffer;      void *ItemRetLen;      };   main()     {      int RetStat;      struct ItemList3 ItmLst[10];*     $DESCRIPTOR( UsernameDsc, "HOFFMAN" );     char Account[MAXACCLEN];
     int i;  
     i = 0;(     ItmLst[i].ItemLength    = MAXACCLEN;*     ItmLst[i].ItemCode	    = UAI$_ACCOUNT;/     ItmLst[i].ItemBuffer    = (void *) Account; #     ItmLst[i++].ItemRetLen  = NULL;       ItmLst[i].ItemLength    = 0;     ItmLst[i].ItemCode	    = 0; #     ItmLst[i].ItemBuffer    = NULL; #     ItmLst[i++].ItemRetLen  = NULL;   ?     RetStat = sys$getuai( 0, 0, &UsernameDsc, ItmLst, 0, 0, 0); (     if (!$VMS_STATUS_SUCCESS( RetStat )) 	return RetStat;       return SS$_NORMAL;     }   