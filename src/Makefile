CC = g++
CFLAGS = -Wall -ggdb3 -std=c++11 -lcurl -lpthread -O2

ExecProject: main.o Stock.o Utils.o ResultsData.o Container.o Matrix.o
	$(CC) $(CFLAGS) -o ExecProject main.o Stock.o Utils.o ResultsData.o Container.o Matrix.o

main.o: Utils.h main.cpp
	$(CC) $(CFLAGS) -c main.cpp

Utils.o: Container.h Matrix.h Utils.h Utils.cpp
	$(CC) $(CFLAGS) -c Utils.cpp
	
Stock.o: ResultsData.h Stock.h Matrix.h Stock.cpp
	$(CC) $(CFLAGS) -c Stock.cpp
	
Container.o: Stock.h Container.h Container.cpp
	$(CC) $(CFLAGS) -c Container.cpp
	
ResultsData.o: ResultsData.h ResultsData.cpp
	$(CC) $(CFLAGS) -c ResultsData.cpp
	
Matrix.o: Matrix.h Matrix.cpp
	$(CC) $(CFLAGS) -c Matrix.cpp
	
clean:
	rm -rf  Project *.o



# CC = g++
# CFLAGS = -Wall -ggdb3 -std=c++11 -lcurl -Iinclude -lpthread

# SRCDIR = src
# OBJDIR = obj
# BINDIR = bin

# SOURCES := $(wildcard $(SRCDIR)/*.cpp)
# OBJECTS := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))

# EXECUTABLE = $(BINDIR)/main

# all: $(EXECUTABLE)

# $(EXECUTABLE): $(OBJECTS)
# 	$(CC) $(CFLAGS) $(INC) $^ -o $@

# $(OBJDIR)/%.o: $(SRCDIR)/%.cpp
# 	$(CC) $(CFLAGS) $(INC) -c $< -o $@

# clean:
# 	rm -f $(OBJDIR)/*.o $(EXECUTABLE)