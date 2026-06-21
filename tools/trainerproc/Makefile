.PHONY: all clean test

CFLAGS := -Wall -O2

SRCS := main.c

ifeq ($(OS),Windows_NT)
EXE := .exe
else
EXE :=
endif

all: trainerproc$(EXE)
	@:

trainerproc$(EXE): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $@ $(LDFLAGS)

clean:
	$(RM) trainerproc$(EXE)

test: trainerproc$(EXE)
	sh tests/test_bingomons.sh ./trainerproc$(EXE)
