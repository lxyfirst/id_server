
SUBDIRS=framework pugixml jsoncpp  id_server

all: 
	@for DIR in $(SUBDIRS) ; do 	\
		make all -C $$DIR ;         \
	done

release:
	@for DIR in $(SUBDIRS) ; do 	\
		make release -C $$DIR ; 	\
	done

clean:
	@for DIR in $(SUBDIRS) ; do 	\
		make clean -C $$DIR ; 		\
	done

install:
	@if ! [ -e deploy ] ; then mkdir deploy ; fi
	cp -f id_server/*.bin deploy/
	cp -f id_server/*.lua deploy/
	cp -f id_server/*.xml deploy/
	cp -f id_server/*.sh deploy/
	cp -f sql/*.sql deploy/
