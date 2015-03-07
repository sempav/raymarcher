CXX=g++
CXXFLAGS= -g -std=c++14 -O2 -pedantic -Wall -Wextra -Weffc++ -Wcast-align \
		  -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 \
		  -Winit-self -Wlogical-op -Wmissing-include-dirs -Wnoexcept \
		  -Wold-style-cast -Woverloaded-virtual -Wredundant-decls \
		  -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel \
		  -Wstrict-overflow=5 -Wswitch-default -Wundef \
		  -Wno-unused-parameter -Wno-old-style-cast -Wno-sign-conversion
CXXFLAGS+=-DGLEW_STATIC
SOURCES=$(wildcard *.cpp)
OBJECTS=$(patsubst %.cpp,%.o,$(SOURCES))
LDLIBS=-lglfw -lGL -lGLU -lGLEW -lglut
TARGET=main

all: main

main: $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) $(LOADLIBES) $(LDLIBS) -o $(TARGET)
	@mkdir log

depend: .depend

.depend: $(SOURCES)
	@rm -vf ./.depend
	$(CXX) $(CXXFLAGS) -MM $^>>./.depend

#-include $(SOURCES:%.cpp=%.md)

clean:
	@rm -vf $(OBJECTS) $(TARGET) ./.depend

-include .depend

#cpp.o:
#	$(CXX) $(CXXFLAGS) -c $< -o $@

