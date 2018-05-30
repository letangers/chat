.PHONY:clean
CPP=g++
FLAGS=-Wall -g 
LINK=-lpthread
BIN=client
OBJS=client.o
$(BIN):$(OBJS)
	$(CPP) $(FLAGS) $^ -o $@ $(LINK)
%.o:%.cpp
	$(CPP) $(FLAGS) -c $< -o $@ $(LINK)

clean:
	rm -f *.o $(BIN)
