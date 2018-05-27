.PHONY:clean
CPP=g++
FLAGS=-Wall -g
BIN=client
OBJS=client.o
$(BIN):$(OBJS)
	$(CPP) $(FLAGS) $^ -o $@
%.o:%.cpp
	$(CPP) $(FLAGS) -c $< -o $@

clean:
	rm -f *.o $(BIN)
