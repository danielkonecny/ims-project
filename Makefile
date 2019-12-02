# Project: 		Carbon Footprint in Energetics and Heating Industry
# File: 		Makefile
# Version: 		1.0
# Course: 		IMS - Modelling and Simulation
# Organisation:	Brno University of Technology - Faculty of Information Technology
# Authors: 		Daniel Konecny (xkonec75), Filip Jerabek (xjerab24)
# Date: 		2. 12. 2019

# Macros
PP = g++
SUFFIX = cpp
PFLAGS = -Wall -Wextra -pedantic -O2
LIB = -lsimlib -lm
BIN = model

# Options
all: $(BIN)

run: $(BIN)
	./$(BIN)

debug: $(BIN)
	./$(BIN) debug

clean:
	rm *.o $(BIN)

clear:
	rm *.o $(BIN)

pack:
	zip $(BIN).zip *.$(SUFFIX) *.h

zip:
	zip $(BIN).zip *.$(SUFFIX) *.h

# Binary
$(BIN): $(BIN).o House.o
	$(PP) $(PFLAGS) $^ -o $@

# Object files
$(BIN).o: $(BIN).$(SUFFIX) $(BIN).h
	$(PP) $(PFLAGS) -c $< -o $@

House.o: House.$(SUFFIX) House.h
	$(PP) $(PFLAGS) -c $< -o $@

