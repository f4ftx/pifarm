vpath %.c ./src

.PHONY: debug

PROGNAME=piFarm
CC=cc

BIN_DIR=./bin
OBJ_DIR=./obj
INC_DIR=./include
SRC_DIR=./src
DEP_DIR=./cots

SRC_LST=pifarm_debug.c pifarm_tools.c pifarm_gpio.c pifarm_sensors.c pifarm_logic.c pifarm_recordings.c pifarm_gui_widgets.c pifarm_gui.c pifarm_configfile.c pifarm_main.c

LDFLAGS=-lX11 -lXext -lm -lrt -lwiringPi -lbcm2835 -L/usr/local/lib

CFLAGS=-Wall -Wextra -Wmissing-prototypes -Wno-missing-braces\
	   -Wno-missing-field-initializers -Wformat=2 -Wswitch-default -Wswitch-enum\
	   -Wcast-align -Wpointer-arith -Wbad-function-cast -Wundef -Wnested-externs\
	   -Wstrict-overflow=5 -Wstrict-prototypes -Winline -Wcast-qual -Wshadow \
	   -Wunreachable-code -Wlogical-op -Wfloat-equal -Wstrict-aliasing=2 \
	   -Wredundant-decls -Wold-style-definition \
	   -I$(INC_DIR)

OBJ_CC_LIST=$(patsubst %.c,$(OBJ_DIR)/%.o ,$(SRC_LST) )
OBJ_AS_LIST=$(patsubst %.c,$(OBJ_DIR)/%.S ,$(SRC_LST) )

lib_bcm2835=$(OBJ_DIR)/bcm2835.o 
lib_ezdraw=$(OBJ_DIR)/ez-draw.o 

all: $(OBJ_DIR) $(BIN_DIR) $(lib_bcm2835) $(lib_ezdraw) $(PROGNAME)

$(OBJ_DIR)/bcm2835.o :
	cp $(DEP_DIR)/bcm2835-1.60.tar.gz $(OBJ_DIR)/
	(cd $(OBJ_DIR) ; tar xvf bcm2835-1.60.tar.gz ; cd -)
	(cd $(OBJ_DIR)/bcm2835-1.60 ; ./configure ; make ; sudo make install)
	cp $(OBJ_DIR)/bcm2835-1.60/src/bcm2835.o $(OBJ_DIR)/
	rm -rf $(OBJ_DIR)/bcm2835-1.60
	rm -f $(OBJ_DIR)/bcm2835-1.60.tar.gz

$(OBJ_DIR)/ez-draw.o :
	cp $(DEP_DIR)/EZ-Draw-1.2.tgz $(OBJ_DIR)/
	(cd $(OBJ_DIR) ; tar xvf EZ-Draw-1.2.tgz ; cd -)
	(cd $(OBJ_DIR)/EZ-Draw-1.2 ; cc -c ez-draw.c -o ez-draw.o)
	cp $(OBJ_DIR)/EZ-Draw-1.2/ez-draw.o $(OBJ_DIR)/
	cp $(OBJ_DIR)/EZ-Draw-1.2/ez-draw.h $(INC_DIR)/
	rm -rf $(OBJ_DIR)/EZ-Draw-1.2
	rm -f $(OBJ_DIR)/EZ-Draw-1.2.tgz

$(OBJ_DIR) :
	@mkdir -p $(OBJ_DIR)

$(BIN_DIR) :
	@mkdir -p $(BIN_DIR)

$(OBJ_DIR)/%.S: %.c
	$(CC) $(CFLAGS_DBG) $(CFLAGS_REL) $(CFLAGS) -S $< -o $@

$(OBJ_DIR)/%.o: %.S
	$(CC) $(CFLAGS_DBG) $(CFLAGS_REL) $(CFLAGS) -c $< -o $@

$(PROGNAME): $(OBJ_CC_LIST) $(OBJ_AS_LIST) $(lib_bcm2835) $(lib_ezdraw)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS_DBG) $(CFLAGS_REL) $(CFLAGS) $(LDFLAGS) -o $(BIN_DIR)/$(PROGNAME) $(OBJ_CC_LIST) $(OBJ_DIR)/bcm2835.o $(OBJ_DIR)/ez-draw.o
	chmod 500 $(BIN_DIR)/$(PROGNAME)

debug: CFLAGS_DBG = -DDEBUG -O0 -g

debug: clean all

release: CFLAGS_REL = -O3 -mcpu=cortex-a53 -mfpu=neon-fp-armv8 -mfloat-abi=hard -funsafe-math-optimizations

release: clean all

clean:
	rm -f  $(OBJ_DIR)/pifarm*.o
	rm -f  $(OBJ_DIR)/pifarm*.S
	rm -rf $(OBJ_DIR)/bcm2835-1.60
	rm -f  $(OBJ_DIR)/bcm2835-1.60.tar.gz
	rm -f  $(BIN_DIR)/$(PROGNAME)

cleanall:
	rm -f  $(OBJ_DIR)/*.o
	rm -f  $(OBJ_DIR)/*.S
	rm -rf $(OBJ_DIR)/bcm2835-1.60
	rm -f  $(OBJ_DIR)/bcm2835-1.60.tar.gz
	rm -rf $(OBJ_DIR)
	rm -f  $(BIN_DIR)/$(PROGNAME)
	rm -f  $(BIN_DIR)/core*
	rm -f  ./core*
	rm -rf $(BIN_DIR)

tarball: cleanall
	tar -cJvf ../$(PROGNAME).tar.xz ../$(PROGNAME)
