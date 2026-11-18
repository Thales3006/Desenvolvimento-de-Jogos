CXX      = g++
CXXFLAGS = -std=c++17 -I source
SFML     ?= /nix/store/rn705kas0215g9g37jx5hfkdfdrrs1sd-sfml-3.0.2
IFLAGS   = -I$(SFML)/include
LFLAGS   = -L$(SFML)/lib -lsfml-graphics -lsfml-window -lsfml-system \
           -Wl,-rpath,$(SFML)/lib

SRCS     = $(wildcard source/*.cpp)
TARGET   = dungeon_escape

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(IFLAGS) $(SRCS) -o $(TARGET) $(LFLAGS)

clean:
	rm -f $(TARGET)
