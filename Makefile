KERNEL_MODULES = event timer video audio memory keyboard random array
ENGINE_MODULES = anim pos physics level consistency stars floor				\
  loose-floor opener-floor closer-floor spikes-floor door pillar wall	\
  room fire potion sword clock princess jaffar
KID_MODULES = kid kid-couch
LEVEL_MODULES = title level-1 consistency-level
MAIN_MODULES = prince

MODULES = ${KERNEL_MODULES:%=kernel/%} ${ENGINE_MODULES:%=engine/%}		\
	${KID_MODULES:%=engine/kid/%} ${LEVEL_MODULES:%=levels/%} ${MAIN_MODULES}

SRCS = ${MODULES:=.c}
OBJECTS = ${MODULES:=.o}
COV_FILES = ${MODULES:=.gcda} ${MODULES:=.gcno}
LDFLAGS = -lm -lallegro -lallegro_image -lallegro_audio	\
  -lallegro_acodec -lallegro_font -lallegro_primitives
CFLAGS_DEV = -Wall -Og -ggdb3 -Werror -Wno-error=unused-function \
  -Wno-error=unused-variable -Wno-error=unused-but-set-variable
CFLAGS_REL = -O3 -march=native
CFLAGS_COV = ${CFLAGS_DEV} --coverage -pg
CFLAGS = ${CFLAGS_DEV}
CPPFLAGS = -I$(shell pwd)

prince : ${OBJECTS} .depend
	${CC} ${OBJECTS} -o $@ ${CPPFLAGS} ${CFLAGS} ${LDFLAGS}

.depend: ${SRCS}
	rm -f .depend
	$(CC) ${CPPFLAGS} $(CFLAGS) -MM $^ > .depend

include .depend

.PHONY: clean
clean:
	rm -f prince ${OBJECTS} ${COV_FILES} gmon.out .depend
