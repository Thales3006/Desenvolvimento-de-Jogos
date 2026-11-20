CXX      = g++
CXXFLAGS = -std=c++17 -I source
IFLAGS   = -I$(HOME)/.nix-profile/include
LFLAGS   = -L$(HOME)/.nix-profile/lib -lsfml-graphics -lsfml-window -lsfml-system \
           -Wl,-rpath,$(HOME)/.nix-profile/lib

SRCS     = $(wildcard source/*.cpp)
TARGET   = dungeon_escape

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(IFLAGS) $(SRCS) -o $(TARGET) $(LFLAGS)

clean:
	rm -f $(TARGET)
