CXX=gcc
STRIP=strip
RM=rm -f
XML2_FLAGS=`xml2-config --cflags`
PCRE_FLAGS=`pcre-config --cflags`
CFLAGS=-Wall -Werror -MD ${XML2_FLAGS} ${PCRE_FLAGS}

XML2_LIBS=`xml2-config --libs`
PCRE_LIBS=`pcre-config --libs`
CLIBS=-L/usr/lib -Wl -O0 ${XML2_LIBS} ${PCRE_LIBS}

KB_NAME=kane-config

OBJS=\
	kb_main.o\
	kb_utils.o\
	kb_parse.o\
	kb_process.o

${KB_NAME}: ${OBJS}
	${CXX} -o ${KB_NAME} ${OBJS} ${CLIBS}
	${STRIP} ${KB_NAME}

clean:
	${RM} ${KB_NAME}
	${RM} *.o *.d

