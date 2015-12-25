SRCS:=$(wildcard ./Src/*.cc)
EXE:=./Bin/Server
CXX:=g++
LIBS:=-lpthread
CXXFLAGS:= -w  -std=c++11 $(LIBS) -Wno-deprecated

$(EXE):$(SRCS)
	$(CXX) -o $(EXE) $(SRCS) $(CXXFLAGS)

clean:
	rm -rf $(EXE)

