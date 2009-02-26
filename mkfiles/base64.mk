# Makefile for noekeon
ALGO_NAME := BASE64

# comment out the following line for removement of base64 from the build process
ENCODINGS += $(ALGO_NAME)


$(ALGO_NAME)_OBJ      := base64_enc.o base64_dec.o
$(ALGO_NAME)_TEST_BIN := main-base64-test.o debug.o uart.o hexdigit_tab.o serial-tools.o \
                         cli.o string-extras.o performance_test.o noekeon_asm.o noekeon_prng.o \
			 memxor.o
			 
$(ALGO_NAME)_PERFORMANCE_TEST := performance
