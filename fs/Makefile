TARGETS = tabfs

# Root for OSXFUSE includes and libraries
OSXFUSE_ROOT = /usr/local
#OSXFUSE_ROOT = /opt/local

# Root for libraries from FreeBSD's ports
FREEBSD_ROOT = /usr/local

CFLAGS ?= -O2
CFLAGS_EXTRA = -DFUSE_USE_VERSION=26 -D_FILE_OFFSET_BITS=64 -Wall -Wextra -Wno-unused-result -g

ifeq ($(shell uname -s),Linux)
	CFLAGS += $(CFLAGS_EXTRA)
	LIBS = -lfuse -pthread
endif
ifeq ($(shell uname -s),Darwin)
	CFLAGS += -I$(OSXFUSE_ROOT)/include -L$(OSXFUSE_ROOT)/lib -D_DARWIN_USE_64_BIT_INODE $(CFLAGS_EXTRA)
	LIBS = -lfuse
endif
ifeq ($(shell uname -s),FreeBSD)
	CFLAGS += -L$(FREEBSD_ROOT)/lib -I$(FREEBSD_ROOT)/include $(CFLAGS_EXTRA)
	LIBS = -lfuse -pthread
endif

all: $(TARGETS)

tabfs: tabfs.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f $(TARGETS) *.o
	rm -rf *.dSYM

unmount:
	killall -9 tabfs || true
	diskutil unmount force mnt || true
	fusermount -u mnt || true
