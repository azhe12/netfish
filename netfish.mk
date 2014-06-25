CXXFLAGS=-O0 -Wall -g -pthread

$(BINARIES):
	g++ $(CXXFLAGS) -o $@ $(LIB_SRC) $(filter %.cc,$^)
