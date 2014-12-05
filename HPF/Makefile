
AR        =ar -r
RANLIB    =ranlib
RM	  =rm -rf
CC 	  = gcc
LD	  = $(CC)

CFLAGS    = -Wall -g
CINCLUDE  = 
LIBS	  = -L. -lsimu_posix 


COMPILE   = $(CC) $(CFLAGS) $(CINCLUDE) -c
MAKEDEP   = makedepend -f Makefile $(CINCLUDE)


.c.o:
	$(COMPILE) $<


####################################################

TEST1 = test1.c
TEST2 = test2.c
TEST3 = test3.c
TEST4 = test4.c
TEST5 = test5.c
TEST6 = test6.c
TEST7 = test7.c
TEST8 = test8.c
TEST9 = test9.c
TEST10 = test10.c
TEST11 = test11.c
TEST12 = test12.c

SIM      = file_de_taches.c simu_posix.c 

VEHICULE = vehicule.c


libsimu_posix.a:	$(SIM:.c=.o) 
	$(AR) $@ $(SIM:.c=.o)  
	$(RANLIB) $@

test1:	$(TEST1:.c=.o)
	$(CC) $(TEST1:.c=.o) $(LIBS) -o test1
test2:	$(TEST2:.c=.o)
	$(CC) $(TEST2:.c=.o) $(LIBS) -o test2
test3:	$(TEST3:.c=.o)
	$(CC) $(TEST3:.c=.o) $(LIBS) -o test3
test4:	$(TEST4:.c=.o)
	$(CC) $(TEST4:.c=.o) $(LIBS) -o test4
test5:	$(TEST5:.c=.o)
	$(CC) $(TEST5:.c=.o) $(LIBS) -o test5
test6:	$(TEST6:.c=.o)
	$(CC) $(TEST6:.c=.o) $(LIBS) -o test6
test7:	$(TEST7:.c=.o)
	$(CC) $(TEST7:.c=.o) $(LIBS) -o test7
test8:	$(TEST8:.c=.o)
	$(CC) $(TEST8:.c=.o) $(LIBS) -o test8
test9:	$(TEST9:.c=.o)
	$(CC) $(TEST9:.c=.o) $(LIBS) -o test9
test10:	$(TEST10:.c=.o)
	$(CC) $(TEST10:.c=.o) $(LIBS) -o test10
test11:	$(TEST11:.c=.o)
	$(CC) $(TEST11:.c=.o) $(LIBS) -o test11
test12:	$(TEST12:.c=.o)
	$(CC) $(TEST12:.c=.o) $(LIBS) -o test12

vehicule:	$(VEHICULE:.c=.o)
	$(CC) $(VEHICULE:.c=.o) $(LIBS) -o vehicule


vehicule:	libsimu_posix.a
test1:		libsimu_posix.a
test2:		libsimu_posix.a
test3:		libsimu_posix.a
test4:		libsimu_posix.a
test5:		libsimu_posix.a
test6:		libsimu_posix.a
test7:		libsimu_posix.a
test8:		libsimu_posix.a
test9:		libsimu_posix.a
test10:		libsimu_posix.a
test11:		libsimu_posix.a
test12:		libsimu_posix.a

clean:
	$(RM)  *.a *.o *.bak core vehicule \
	test1 test2 test3 test4 test5 test6 test7 test8 \
	test9 test10 test11 test12

depend:
	$(MAKEDEP)  $(SIM) $(VEHICULE) $(TEST1) $(TEST2) \
	$(TEST3) $(TEST4) $(TEST5) $(TEST6) $(TEST7) $(TEST8) \
	$(TEST9) $(TEST10) $(TEST11) $(TEST12) 


# C DEPENDENCIES
# DO NOT DELETE THIS LINE -- make depend depends on it.

