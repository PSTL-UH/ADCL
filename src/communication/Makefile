include ../../Makefile.defs

OBJ =	ADCL_change_sb_aao_debug.o \
	ADCL_change_sb_aao_IsendIrecv.o \
	ADCL_change_sb_aao_SendIrecv.o \
	ADCL_change_sb_aao_IsendIrecv_pack.o \
	ADCL_change_sb_pair_debug.o \
	ADCL_change_sb_pair_IsendIrecv.o \
	ADCL_change_sb_pair_SendRecv.o \
	ADCL_change_sb_pair_SendIrecv.o \
	ADCL_change_sb_pair_IsendIrecv_pack.o\
	ADCL_change_sb_pair_SendRecv_pack.o


.SUFFIXES: .o.c

change:$(HEADER)
	$(CC) $(CFLAGS) -c ADCL_change_sb_aao.c -DCOMMMODE=0 \
			-o ADCL_change_sb_aao_debug.o 
	$(CC) $(CFLAGS) -c ADCL_change_sb_aao.c -DCOMMMODE=1 \
			-o ADCL_change_sb_aao_IsendIrecv.o 
	$(CC) $(CFLAGS) -c ADCL_change_sb_aao.c -DCOMMMODE=3 \
			-o ADCL_change_sb_aao_SendIrecv.o 
	$(CC) $(CFLAGS) -c ADCL_change_sb_aao.c -DCOMMMODE=4 \
			-o ADCL_change_sb_aao_IsendIrecv_pack.o 
	$(CC) $(CFLAGS) -c ADCL_change_sb_aao.c -DCOMMMODE=6 \
			-o ADCL_change_sb_aao_SendIrecv_pack.o	
	$(CC) $(CFLAGS) -c ADCL_change_sb_pair.c -DCOMMMODE=0 \
			-o ADCL_change_sb_pair_debug.o 
	$(CC) $(CFLAGS) -c ADCL_change_sb_pair.c -DCOMMMODE=1 \
			-o ADCL_change_sb_pair_IsendIrecv.o 
	$(CC) $(CFLAGS) -c ADCL_change_sb_pair.c -DCOMMMODE=2 \
			-o ADCL_change_sb_pair_SendRecv.o 
	$(CC) $(CFLAGS) -c ADCL_change_sb_pair.c -DCOMMMODE=3 \
			-o ADCL_change_sb_pair_SendIrecv.o 
	$(CC) $(CFLAGS) -c ADCL_change_sb_pair.c -DCOMMMODE=4 \
			-o ADCL_change_sb_pair_IsendIrecv_pack.o
	$(CC) $(CFLAGS) -c ADCL_change_sb_pair.c -DCOMMMODE=5 \
			-o ADCL_change_sb_pair_SendRecv_pack.o	
	$(CC) $(CFLAGS) -c ADCL_change_sb_pair.c -DCOMMMODE=6 \
			-o ADCL_change_sb_pair_SendIrecv_pack.o	
	$(AR) $(AR_FLAGS) $(ADCL_LIB_DIR)/$(ADCL_LIB) $(OBJ)

$(OBJ):	$(HEADER)
.c.o:	$(HEADER)
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o *~
