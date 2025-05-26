# Makefile

# Variables
CXX = g++
CXXFLAGS = -Wall -std=c++17
TARGET = basicGit
SRCS = main.cpp basicGit.cpp
OBJS = $(SRCS:.cpp=.o)

# Default target
all: $(TARGET)

# Linking
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Compilation
%.o: %.cpp basicGit.h
	$(CXX) $(CXXFLAGS) -c $<

# Clean up object and executable files
clean:
	rm -f $(OBJS) $(TARGET)

# Remove the .basicGit directory
remove_git:
	rm -rf .basicGit

.PHONY: all clean remove_git
