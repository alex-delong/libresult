# rules assume a single binary that depends on every library
# assume binary source code is split between 1 or more files

INCLUDES=$(wildcard include/*.hpp)
SRC=$(wildcard src/*)
LIB_SRC=$(wildcard src/lib*.cpp)
LIBS=$(patsubst src/lib%.cpp, lib/lib%.so, $(LIB_SRC))

SRC_TEST_UNIT=$(wildcard test/src/test_unit_*.cpp)
BIN_TEST_UNIT=$(patsubst test/src/test_unit_%.cpp, test/bin/test_unit_%, $(SRC_TEST_UNIT))
BIN_TEST_INTEGRATION=test/bin/test_integration
OBJS_TEST_UNIT=$(patsubst test/src/test_unit_%.cpp, test/lib/test_unit_%.o, $(SRC_TEST_UNIT))
OBJS_TEST_INTEGRATION=test/lib/test_integration.o

CXX=g++
CXX_FLAGS=-I include 

all: test-all libs

libs: $(LIBS)

$(LIBS) : lib/lib%.so : src/lib%.cpp include/lib%.hpp
	mkdir -p lib
	$(CXX) $(CXX_FLAGS) -c $< -o $@

test-run: test-unit test-int
	./test/bin/test_unit_result
	./test/bin/test_integration

test-unit-run: test-unit
	./test/bin/test_unit_result

test-int-run: test-int
	./test/bin/test_integration

test-all: test-unit test-int

test-unit: $(BIN_TEST_UNIT)

test-int: $(BIN_TEST_INTEGRATION)

$(BIN_TEST_UNIT): test/bin/test_unit_% : test/lib/test_unit_%.o lib/lib%.so
	mkdir -p test/bin
	$(CXX) $(CXX_FLAGS) $^ -o $@

$(OBJS_TEST_UNIT) : test/lib/test_unit_%.o : test/src/test_unit_%.cpp include/lib%.hpp 
	mkdir -p test/lib
	$(CXX) $(CXX_FLAGS) -c $< -o $@ 

$(BIN_TEST_INTEGRATION) : $(OBJS_TEST_INTEGRATION) $(LIBS)
	mkdir -p test/bin
	$(CXX) $(CXX_FLAGS) $^ -o $@

$(OBJS_TEST_INTEGRATION) : test/src/test_integration.cpp $(INCLUDES)
	mkdir -p test/lib
	$(CXX) $(CXX_FLAGS) -c test/src/test_integration.cpp -o $(OBJS_TEST_INTEGRATION)

