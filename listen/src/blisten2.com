 $! $! Compile and LINK LISTEN2  $!% $ IF P1 .EQS. "LINK" THEN GOTO DOLINK  $ IF P1 .EQS. "DEBUG"  $ THEN $     cc_flags = "/NOOPT/DEBUG"  $     lnk_flags = "/DEBUG" $ ELSE $     cc_flags = ""  $     lnk_flags = "" $ ENDIF  $ CC'cc_flags' LISTEN2.C $ CC'cc_flags' LISTEN_PROCESS.C ' $ CC'cc_flags' LISTEN_TABLE.C	/PREF=ALL  $ CC'cc_flags' LISTEN_SUBS.C $ CC'cc_flags' FORMAT_SYSID.C  $dolink:$ $ arch_type = F$GETSYI("ARCH_TYPE") / $ IF (arch_type .EQ. 2) .OR. (arch_type .EQ. 3)  $ THENf $     LINK'lnk_flags' LISTEN2.OBJ,LISTEN_TABLE.OBJ,LISTEN_SUBS.OBJ,FORMAT_SYSID.OBJ,LISTEN_PROCESS.OBJ $ ENDIF  $ IF arch_type .EQ. 1  $ THEN  h $     LINK'lnk_flags' LISTEN2.OBJ,LISTEN_TABLE.OBJ,LISTEN_SUBS.OBJ,FORMAT_SYSID.OBJ,LISTEN_PROCESS.OBJ,-         SYS$INPUT/OPT  SYS$SHARE:VAXCRTL.EXE/SHARE  SYS$LIBRARY:UCX$IPC.OLB/LIBR $ ENDIF  $ EXIT