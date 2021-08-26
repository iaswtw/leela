
ELF         := universe3d
SRCS        := $(wildcard *.cpp)
OBJS        := $(SRCS:.cpp=.o)
DEPS        := $(SRCS:.cpp=.d)
CFLAGS      := $(shell pkg-config --cflags sdl2 glew glm)  
LIBS        := -lm $(shell pkg-config --libs sdl2 glew) -std=gnu++11  -fno-threadsafe-statics


.PHONY: all
all: $(ELF)

$(ELF): $(OBJS)
	g++ -o $@ $(OBJS) $(LIBS)

	
#$(DEPS): %.d: %.cpp
#	gcc $(CFLAGS) -MM -MG -MP -MF $(@:.o=.d) -c -o $@ $<

$(OBJS): %.o: %.cpp
	g++ $(CFLAGS) -MT $@ -MMD -MP -MF $(@:.o=.d) -c -o $@ $<
	

.PHONY: clean
clean:
	rm -rf $(OBJS)
	
	
-include $(DEPS)

