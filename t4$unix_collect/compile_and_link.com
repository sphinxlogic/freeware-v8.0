= $! Compile and link a program with pre-determined qualifiers.  $!- $! Example: @COMPILE_AND_LINK T4$UNIX_COLLECT 3 $! Example: @COMPILE_AND_LINK T4$UNIX_COLLECT DEBUG  $!5 $! Note that T4$UNIX_COLLECT must be compiled "float"  $! $! Bart Z. Lederman	27-Apr-2006  $!A $ arch_name = F$EDIT (F$GETSYI ("ARCH_NAME"), "UPCASE, COLLAPSE")  $!( $ out_name = P1 - ".C" + "_" + arch_name $! $ IF arch_name .EQS. "ALPHA" $ THEN, $    arch = "HOST"		! or "EV56", "EV6", etc. $    IF (P2 .EQS. "DEBUG")	 $    THEN  $	debug_qual = "/DEBUG"  $	opt_qual   = "/NOOPT" 	 $    ELSE  $	debug_qual = "/NODEBUG" B $	opt_qual   = "/OPT = (LEVEL = 5, TUNE = ''arch', INLINE = AUTO)"
 $    ENDIF $    IF P3 .EQS. "NOFLOAT"	 $    THEN % $	floating_point = "/INSTR = NOFLOAT" 	 $    ELSE I $	floating_point = "/INSTR = FLOAT /FLO = IEEE /IEEE = UNDERFLOW_TO_ZERO" 
 $    ENDIF $! $ ENDIF  $! $ IF arch_name .EQS. "IA64"  $ THEN $    arch = "ITANIUM2" $    IF (P2 .EQS. "DEBUG")	 $    THEN  $	debug_qual = "/DEBUG"  $	opt_qual   = "/NOOPT" 	 $    ELSE  $	debug_qual = "/NODEBUG" C $	opt_qual   = "/OPT = (LEVEL = 5, TUNE = ITANIUM2, INLINE = AUTO)" 
 $    ENDIF $    IF P3 .EQS. "NOFLOAT"	 $    THEN I $	floating_point = "/INSTR = FLOAT /FLO = IEEE /IEEE = UNDERFLOW_TO_ZERO" 	 $    ELSE % $	floating_point = "/INSTR = NOFLOAT" 
 $    ENDIF $ ENDIF  $!F $ CC /ARCH = 'arch' /ASSUME = WHOLE /CHECK = (UNINIT, POINTER = ALL) - 	'floating_point' -  	'debug_qual' 'opt_qual' -* 	/LIST = 'out_name' /CROSS /SHOW = BRIEF -# 	/OBJ = 'out_name /PREFIX = ALL 'P1  $!- $ LINK/MAP/CROSS/TRACE/NODEBUG/DSF 'out_name'  $! $ EXIT $STATUS