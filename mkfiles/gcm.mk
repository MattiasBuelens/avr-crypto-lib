# Makefile for AES
ALGO_NAME := GCM

# comment out the following line for removement of AES from the build process
BLOCK_CIPHERS += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := gcm/
$(ALGO_NAME)_INCDIR   := memxor/ aes/ gf256mul/ bcal/
$(ALGO_NAME)_OBJ      := gcm128.o \
                         aes_enc-asm.o aes_dec-asm.o aes_sbox-asm.o aes_invsbox-asm.o  \
                         aes_keyschedule-asm.o 
$(ALGO_NAME)_TESTBIN  := main-gcm-test.o $(CLI_STD) $(BCAL_STD)  \
                         bcal_aes128.o bcal_aes192.o bcal_aes256.o  \
                         dump-asm.o dump-decl.o        \
                         memxor.o
$(ALGO_NAME)_NESSIE_TEST      := test nessie
$(ALGO_NAME)_PERFORMANCE_TEST := performance

