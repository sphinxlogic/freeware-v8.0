< //  Copyright 2006 Hewlett-Packard Development Company, L.P. //0 //  An example of using the callable EDT editor. //3 //  Author: Stephen Hoffman, HP OpenVMS Engineering  // #include <descrip.h> int edt$edit();  main()     {      int retstat;0     $DESCRIPTOR( infile, "SYS$LOGIN:LOGIN.COM");"     retstat = edt$edit( &infile );     return retstat;      } 