
SRCS=$(wildcard *.cpp)
PROTOCS=$(wildcard *.proto)
PROTO_SRCS=$(PROTOCS:.proto=.pb.cc)
BIN_SRCS=$(shell grep "int main" -l *.cpp)
LIB_SRCS=$(patsubst $(BIN_SRCS),,$(SRCS)) 

DEPS=$(SRCS:.cpp=.d) 
OBJS=$(SRCS:.cpp=.o) $(PROTO_SRCS:.cc=.o)

LIB_OBJS=$(patsubst %.cpp,%.o,$(LIB_SRCS))  $(PROTO_SRCS:.cc=.o)
BINS=$(patsubst %.cpp,%,$(BIN_SRCS))

LIB_NAME=jsoncpp
SO_LIB=$(patsubst %,lib%.so,$(LIB_NAME))
STATIC_LIB=$(patsubst %,lib%.a,$(LIB_NAME))


CC=clang++
CFLAGS= -Wall -D_REENTRANT -D_GNU_SOURCE -fPIC
CFLAGS += 
BOOST_DIR = /usr/local/include/boost/
THRIFT_DIR = /usr/local/include/thrift
#INC= -I${THRIFT_DIR} -I${BOOST_DIR}  -I. -I../public 
INC= -I. -I../framework -I../config -I../tools

ifeq ($(release), 1)
CFLAGS += -O2 -DNDEBUG 
else
CFLAGS += -g -DDEBUG 
endif

all: $(BINS) $(SO_LIB) $(STATIC_LIB) 

$(BINS): % : %.o $(LIB_OBJS) 
	$(CC) $(CFLAGS) -o $@ $^  
$(SO_LIB): $(LIB_OBJS) 
	$(CC) $(CFLAGS) -shared -o $@ $^ 
$(STATIC_LIB): $(LIB_OBJS) 
	ar -rcs -o $@ $^ 
dist :
	rm -f $(DEPS) *.o
clean:
	rm -f $(BINS) $(OBJS) $(DEPS) $(SO_LIB) $(STATIC_LIB)
release: clean
	@make release=1

%.d : %.cpp
	$(CC) $(CFLAGS) -MT $(subst .cpp,.o,$<) -MM $(INC)  $< >$@
%.o : %.cpp
	$(CC) $(CFLAGS) $(INC) -c  $< 

-include  $(DEPS)

