!! Small Decription file for ICALCV (new version of ICALC)

CFLAGS = /OPTIM
FFLAGS = /EXTEND/CONT=55/OPTIMIZE
LFLAGS = /NOTRACE

.ifdef __AXP
CFLAGS = $(CFLAGS)/STAND=VAXC/PREFIX=(ALL,EXCEPT=(TRUNC))
.else
LFLAGS = $(LFLAGS),VAX.OPT/OPT
.endif

OBJECTS = ICALCV$(OBJ), G05CAF$(OBJ)

ICALCV.EXE : $(OBJECTS) !, ICALCV.H_TXT
	LINK $(OBJECTS) $(LFLAGS) 

!ICALCV.H_TXT : ICALCV.TXT 
!	TEXT2C ICALCV.TXT ICALCV.H_TXT 
