CC = g++
CFLAGS = #-g -Wall -Wextra -pedantic 
IPATH = -I/usr/X11/include -I/usr/pkg/include -I/usr/pkg/include/OpenEXR -I./include
LPATH = -L/usr/X11/lib -L/usr/pkg/lib
LDPATH = -Wl,-R/usr/pkg/lib 
RM = rm

BIN = ./bin/
SRC = ./src/

all: $(BIN)exrExample

$(BIN)exrExample: $(BIN)main.o $(BIN)imageio.o 
	$(CC) -o $@ $^ -lm -Bstatic -ltiff -lIex -lIlmImf -lIlmThread -lImath -lIex -lHalf -Bdynamic $(LPATH) $(LDPATH)
	
$(BIN)%.o: $(SRC)%.cpp
	$(CC) $(CFLAGS) -c -o $@ $^ $(IPATH)	
	
clean:
	$(RM) -f *.o *.gch exrExample

clean:
	$(RM) -f $(BIN)*.o $(BIN)*.gch $(BIN)exrExample
