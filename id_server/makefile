
SRCS=$(wildcard *.cpp)
#BIN_SRCS=$(shell grep "int main" -l *.cpp) 
BIN_SRCS=$(shell grep "IMPLEMENT_MAIN" -l *.cpp) 
LIB_SRCS=$(patsubst $(BIN_SRCS),,$(SRCS))

DEPS=$(SRCS:.cpp=.d) 
OBJS=$(SRCS:.cpp=.o) 

LIB_OBJS=$(patsubst %.cpp,%.o,$(LIB_SRCS)) 
BINS=$(patsubst %.cpp,%.bin,$(BIN_SRCS))
#BINS=test_server
DEP_LIBS=  ../framework/libframework.a  ../pugixml/libpugixml.a ../jsoncpp/libjsoncpp.a

LIB_NAME=
SO_LIB=$(patsubst %,lib%.so,$(LIB_NAME))
STATIC_LIB=$(patsubst %,lib%.a,$(LIB_NAME))


CC=clang++
CFLAGS= -Wall -D_REENTRANT -D_GNU_SOURCE -fPIC
#CFLAGS += -std=c++11
#INC= -I${THRIFT_DIR} -I${BOOST_DIR}  -I. -I../public 
INC= -I. -I../framework -I..
LDFLAGS=-llua -lrt -lpthread -lmysqlclient_r -L/usr/lib64/mysql

ifeq ($(release), 1)
CFLAGS += -O2 -DNDEBUG 
LDFLAGS += -ltcmalloc_minimal
else
CFLAGS += -g -DDEBUG 
LDFLAGS += -ltcmalloc_and_profiler
endif

all: $(BINS) $(SO_LIB) $(STATIC_LIB) 

$(BINS): %.bin : %.o $(LIB_OBJS) $(DEP_LIBS)
	$(CC) -o $@ $^ $(LDFLAGS) 
$(SO_LIB): $(LIB_OBJS) 
	$(CC) $(CFLAGS) -shared -o $@ $^ 
$(STATIC_LIB): $(LIB_OBJS) 
	ar -rcs -o $@ $^ 
dist :
	rm -f $(DEPS)
clean:
	rm -f $(BINS) $(OBJS) $(DEPS) $(SO_LIB) $(STATIC_LIB)
release: clean
	@make release=1

deploy: 
	tar -zcf deploy.tar.gz  *.xml *.lua *.sh $(BINS)

%.d : %.cpp
	$(CC) $(CFLAGS)  -MT $(subst .cpp,.o,$<) -MM $(INC)  $< >$@
%.o : %.cpp
	$(CC) $(CFLAGS) $(INC) -c  $< 

-include  $(DEPS)

