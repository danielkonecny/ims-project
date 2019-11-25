# Project: 		Carbon Footprint in Energetics and Heating Industry
# File: 		Makefile
# Version: 		0.1
# Course: 		IMS - Modelling and Simulation
# Organisation:	Brno University of Technology - Faculty of Information Technology
# Authors: 		Daniel Konecny (xkonec75), Filip Jerabek (xjerab24)
# Date: 		25. 11. 2019

# Macros
PP = g++
SUFFIX = cpp
PFLAGS = -Wall -Wextra -pedantic -g -O2
LIB = -lsimlib -lm
BIN = model

#  Options
all: $(BIN)

run: $(BIN)
	./$(BIN)

clean:
	rm *.o $(BIN)

clear:
	rm *.o $(BIN)

pack:
	zip $(BIN).zip *.$(SUFFIX) *.h

zip:
	zip $(BIN).zip *.$(SUFFIX) *.h

# Binary
$(BIN): $(BIN).o
	$(PP) $(PFLAGS) $^ -o $@ $(LIB)

# Object file
$(BIN).o: $(BIN).$(SUFFIX) $(BIN).h
	$(PP) $(PFLAGS) -c $< -o $@ $(LIB)

