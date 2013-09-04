OBJS = Tarranta.o Game.o GameParser.o GameDispatcher.o GameExecutor.o Station.o ItemCollection.o CommandCollection.o StringCollection.o Location.o Player.o Item.o Container.o SwitchableItem.o Inventory.o Command.o Tokeniser.o Statics.o Terminal.o BitFileReader.o
LFLAGS = -Wall -g
CFLAGS = -Wall -c -g -Os
CC = clang++
COMPILE = $(CC) $(CFLAGS)

tarranta: $(OBJS)
	$(CC) -o tarranta $(LFLAGS) $(OBJS)

Tarranta.o: Tarranta.h Tarranta.cpp Game.h Station.h ItemCollection.h CommandCollection.h Location.h Item.h Container.h SwitchableItem.h Definitions.h Tokeniser.h Statics.h Terminal.h BitFileReader.h
	$(COMPILE) Tarranta.cpp

Game.o: Game.h Game.cpp Station.h ItemCollection.h CommandCollection.h StringCollection.h Location.h Player.h Inventory.h Item.h Container.h SwitchableItem.h Definitions.h Statics.h BitFileReader.h Tokeniser.h Terminal.h
	$(COMPILE) Game.cpp

GameParser.o: Game.h GameParser.cpp Station.h ItemCollection.h CommandCollection.h StringCollection.h Location.h Player.h Inventory.h Item.h Container.h SwitchableItem.h Definitions.h Statics.h BitFileReader.h Tokeniser.h Terminal.h
	$(COMPILE) GameParser.cpp

GameDispatcher.o: Game.h GameDispatcher.cpp Station.h ItemCollection.h CommandCollection.h StringCollection.h Location.h Player.h Inventory.h Item.h Container.h SwitchableItem.h Definitions.h Statics.h BitFileReader.h Tokeniser.h Terminal.h
	$(COMPILE) GameDispatcher.cpp

GameExecutor.o: Game.h GameExecutor.cpp Station.h ItemCollection.h CommandCollection.h StringCollection.h Location.h Player.h Inventory.h Item.h Container.h SwitchableItem.h Definitions.h Statics.h BitFileReader.h Tokeniser.h Terminal.h
	$(COMPILE) GameExecutor.cpp

Station.o: Station.h Station.cpp Location.h Item.h Container.h SwitchableItem.h Definitions.h Statics.h Tokeniser.h
	$(COMPILE) Station.cpp

ItemCollection.o: ItemCollection.h ItemCollection.cpp Item.h Item.cpp Location.h Container.h SwitchableItem.h BitFileReader.h Tokeniser.h
	$(COMPILE) ItemCollection.cpp

CommandCollection.o: CommandCollection.h CommandCollection.cpp Command.h BitFileReader.h Tokeniser.h Definitions.h
	$(COMPILE) CommandCollection.cpp

StringCollection.o: StringCollection.h StringCollection.cpp BitFileReader.h Tokeniser.h Definitions.h
	$(COMPILE) StringCollection.cpp

Location.o: Location.h Location.cpp Item.h Container.h
	$(COMPILE) Location.cpp

Player.o: Player.h Player.cpp Definitions.h
	$(COMPILE) Player.cpp

Item.o: Item.h Item.cpp Location.h
	$(COMPILE) Item.cpp

Container.o: Container.h Container.cpp Item.h
	$(COMPILE) Container.cpp

SwitchableItem.o: SwitchableItem.h SwitchableItem.cpp Item.h
	$(COMPILE) SwitchableItem.cpp

Inventory.o: Inventory.h Inventory.cpp Item.h Container.h SwitchableItem.h Location.h Terminal.h Statics.h
	$(COMPILE) Inventory.cpp

Command.o: Command.h Command.cpp Definitions.h Statics.h
	$(COMPILE) Command.cpp

Tokeniser.o: Tokeniser.h Tokeniser.cpp Definitions.h
	$(COMPILE) Tokeniser.cpp

Definitions.h:

Statics.o: Statics.h Statics.cpp
	$(COMPILE) Statics.cpp

Terminal.o: Terminal.h Terminal.cpp
	$(COMPILE) Terminal.cpp

BitFileReader.o: BitFileReader.h BitFileReader.cpp Definitions.h
	$(COMPILE) BitFileReader.cpp

clean:
	rm *.o tarranta
