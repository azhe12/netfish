CXXFLAGS=-O0 -Wall -g -pthread
LDFLAGS=-lnetfish -L.

LIB_OBJ=$(patsubst %.cc,%.o,$(LIB_SRC))
libnetfish.a:$(LIB_SRC)
	g++ $(CXXFLAGS) -c $^
	ar rcs $@ $(LIB_OBJ)

$(BINARIES):libnetfish.a
	g++ $(CXXFLAGS) -o $@ $(filter %.cc,$^) $(LDFLAGS)
