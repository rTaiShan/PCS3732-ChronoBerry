
FONTES = boot.s sched.c timer.c main.c
LDSCRIPT = linker.ld
PROJETO = tasks

#
# Arquivos de saída 
#
EXEC = ${PROJETO}.elf
MAP = ${PROJETO}.map
LIST = ${PROJETO}.list
IMAGE = ${PROJETO}.img
HEXFL = ${PROJETO}.hex

PREFIXO = arm-none-eabi-
AS = ${PREFIXO}as
LD = ${PREFIXO}ld
GCC = ${PREFIXO}gcc
OBJCPY = ${PREFIXO}objcopy
OBJDMP = ${PREFIXO}objdump
OPTS = -march=armv7-a -mtune=cortex-a7 -g -O1
OBJ = $(FONTES:.s=.o)
OBJETOS = $(OBJ:.c=.o)

LDOPTS = -lgcc -L/usr/lib/gcc/arm-none-eabi/13.2.1/

all: ${EXEC} ${LIST} ${IMAGE} ${HEXFL}

rebuild: clean all

#
# Gerar executável
#
${EXEC}: ${OBJETOS}
	${LD} -T ${LDSCRIPT} -M=${MAP} ${OBJETOS} -o $@ ${LDOPTS}

#
# Gerar listagem
#
${LIST}: ${EXEC}
	${OBJDMP} -std ${EXEC} > ${LIST}

#
# Gerar imagem binária
#
${IMAGE}: ${EXEC}
	${OBJCPY} -O binary ${EXEC} ${IMAGE}

#
# Gerar arquivo HEX
#
${HEXFL}: ${EXEC}
	${OBJCPY} -O ihex ${EXEC} ${HEXFL}

#
# Compilar arquivos em C
#
.c.o:
	${GCC} ${OPTS} -c -o $@ $<

#
# Montar arquivos em assembler
#
.s.o:
	${AS} -g -o $@ $<

#
# Limpar tudo
#
clean:
	rm -f *.o ${EXEC} ${MAP} ${LIST} ${IMAGE}
	-killall gdb-multiarch openocd

#
# Depuração com openocd e gdb
#
debug: ${EXEC}
	if ! pgrep openocd >/dev/null ; then \
		openocd -f rpi2.cfg & \
	fi
	gdb-multiarch -ex "target extended-remote:3333" \
		           -ex "load" \
					  ${EXEC}

