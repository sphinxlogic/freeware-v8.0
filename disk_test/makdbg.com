$ IF P1 .EQS. "" THEN INQ P1 "Filename: "
$ MAC/MIG/LIS/DEB/FLAG=NOALIGN/OPTIM=NONE/UNALIGN 'P1
$ SET NOVERIFY
$ INQ INP "Link [Y]? "
$ IF F$EDIT(F$EXTRACT(0,1,INP),"UPCASE") .EQS. "N" THEN EXIT
$ SET VERIFY
$!
$ LINK/SYSEXE/SYSSHR/SYSLIB/DEB  'P1
$!
$ SET NOVERIFY
$ EXIT