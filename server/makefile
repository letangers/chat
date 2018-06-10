.PHONY:clean
CPP=g++
FLAGS=-Wall -g
BIN=server
OBJS=server.o
LINK=-lpthread
$(BIN):$(OBJS)
	$(CPP) $(FLAGS) $^ -o $@ $(LINK)
%.o:%.cpp
	$(CPP) $(FLAGS) -c $< -o $@ $(LINK)

clean:
	rm -f *.o $(BIN)
