/*
 *	Assorted pre-processor stuff
 */

/*
 *	Conditional compilation stuff
 */
#define TESTING_READ // Will give access to basic tester commands like node
#define TESTING_WRITE // Will give access to extended tester commands like flash, grab; will not work unless TESTING_READ is defined also

/*
 *	Useful integers
 */

// Scoring
#define SCORE_ESCAPE 50 // Points given for escaping the asteroid
#define SCORE_PUZZLE 20 // Points given for solving some puzzle
#define SCORE_TREASURE 10 // Points given for depositing a treasure in the right place; this counts for double if player escapes asteroid with it

// Basic stuff
#define ZERO 0
#define ONE 1
#define TWO 2
#define THREE 3
#define HEX 16
#define BITS_PER_BYTE 8
#define BITS_PER_ASCII 7
#define CHARS_PER_CODE 8 // No of chars in value of type uint64_t
#define TOKENS_PER_COMMAND 2 // The maximum number of tokens the program will process from a user-input line
#define ASCII_OFFSET 0x30 // Offset between ASCII characters and numeric values

// Datafile bounds checks
#define MIN_TOKENS_COMMAND 2 // The minimum number of tokens a line in commands section of datafile must contain
#define MIN_TOKENS_LOCATION 15 // The minimum number of tokens a line in locations section of datafile must contain
#define MIN_TOKENS_ITEM 7 // The minimum number of tokens a line in items section of datafile must contain
#define MIN_TOKENS_STRING 2 // The minimum number of tokens a line in any of the strings sections of datafile must contain

// Commands datafile control
#define INDEX_CSTATUS 0 // Index of the status integer of the command
#define INDEX_CPRIMARY 1 // Index of the primary term for the command

// Locations datafile control
#define INDEX_ID 0
#define MAX_DIRECTIONS 10 // The number of possible directions from any location
#define INDEX_N 1
#define INDEX_S 2
#define INDEX_E 3
#define INDEX_W 4
#define INDEX_NE 5
#define INDEX_SW 6
#define INDEX_SE 7
#define INDEX_NW 8
#define INDEX_U 9
#define INDEX_D 10
#define OFFSET_DIRECTION INDEX_N // The difference between the first direction and zero
#define INDEX_STATUS 11
#define INDEX_SHORT 12
#define INDEX_LONG 13
#define INDEX_FULL 14

// Items datafile control
#define INDEX_ISTATUS 0
#define INDEX_IINITLOC 1
#define INDEX_ISIZE 2
#define INDEX_ISHORT 3
#define INDEX_ILONG 4
#define INDEX_IFULL 5
#define INDEX_IWRIT 6 // Indices of elements on a line of the items datafile

// Strings datafile control
#define INDEX_SKEY 0 // Index of the key element
#define INDEX_SVALUE 1 // Index of the value element

// Command control
#define CTRL_COMMAND_TESTING_WRITE 0x200 // Whether the command is of WRITE testing level
#define CTRL_COMMAND_TESTING_READ 0x100 // Whether the command is of READ testing level
#define CTRL_COMMAND_COMPOUND 0x80 // Whether the command is compound e.g. "go" in "go north"
#define CTRL_COMMAND_MOVEMENT 0x40 // Whether the command intends movement
#define CTRL_COMMAND_INVERTIBLE 0x20 // Whether the command appears in order contrary to the usual e.g. "off" in "lamp off"
#define CTRL_COMMAND_SECRET 0x10 // Whether the command is secret (not to be listed)
#define CTRL_COMMAND_ARGS 0x08 // Whether the command must take an argument
#define CTRL_COMMAND_PRESENT 0x04 // Whether the argument must be somewhere in the player's vicinity
#define CTRL_COMMAND_INVENTORY 0x02 // Whether the argument the command takes must be in the inventory

// Location control
#define CTRL_LOC_HAS_LIGHT 0x1
#define CTRL_LOC_HAS_AIR 0x2
#define CTRL_LOC_HAS_GRAVITY 0x4
#define CTRL_LOC_HAS_NOSNOMP 0x8
#define CTRL_LOC_NEEDSNO_LIGHT 0x10
#define CTRL_LOC_NEEDSNO_AIR 0x20
#define CTRL_LOC_NEEDSNO_GRAVITY 0x40
#define CTRL_LOC_NEEDSNO_LION 0x80
#define CTRL_LOC_HAS_CEILING 0x100
#define CTRL_LOC_HAS_FLOOR 0x200
#define CTRL_LOC_HAS_LAND 0x400

// Item control
#define CTRL_ITEM_CONTAINER 0x1 // Whether the item is capable of containing other items
#define CTRL_ITEM_MOBILE 0x2 // Whether the item is fixed or carryable
#define CTRL_ITEM_OBSTRUCTION 0x4 // Whether the item is an obstruction
#define CTRL_ITEM_SWITCHABLE 0x8 // Whether the item can be switched on/off
#define CTRL_ITEM_GIVES_LIGHT 0x10 // Whether the item emits light
#define CTRL_ITEM_GIVES_AIR 0x20 // Whether the item enables player to breathe
#define CTRL_ITEM_GIVES_GRAVITY 0x40 // Whether the item creates gravity around the player
#define CTRL_ITEM_GIVES_NOSNOMP 0x80 // Whether the item protects the player from snomps
#define CTRL_ITEM_CONTAINER_LIQUID 0x100 // Whether the container may contain liquid
#define CTRL_ITEM_FRAGILE 0x200 // Whether the item would survive throwing, dropping from heights, etc
#define CTRL_ITEM_WORN 0x400 // Whether the item would be worn e.g. suit, shoes, hat, etc
#define CTRL_ITEM_LIQUID 0x800 // Whether the item is a liquid
#define CTRL_ITEM_KEEP 0x1000 // Whether the item should be kept when player dies
#define CTRL_ITEM_EDIBLE 0x2000 // Whether the item is any sort of food or drink
#define CTRL_ITEM_GIVES_INVISIBILITY 0x4000 // Whether wearing or carrying the item grants the player invisibility
#define CTRL_ITEM_TREASURE 0x8000 // Whether the item is a treasure or not
#define CTRL_ITEM_FACTORY 0x10000 // Whether the item may be created by the machine in the factory
#define CTRL_ITEM_SILENT 0x20000 // Whether the item should be shown in location descriptions
#define CTRL_ITEM_GIVES_LAND 0x40000 // Whether the item gives 'land', i.e. acts as a boat or whatever

// Assorted
#if defined (TESTING_READ) && defined (TESTING_WRITE)
#define INVENTORY_CAPACITY 1000 // Enhanced inventory capacity for testing
#else
#define INVENTORY_CAPACITY 16 // Initial default capacity of inventory
#endif

#define DEATH_CHANCE 4 // One in this number of moves of a random hazard will lead to death
#define DEATH_PLACES 60 // The number of places a fall or a snomp will kill you will be between zero and this
#define PENALTY_HINT -10 // Number of points given to player when they ask for a hint
#define MAX_MOVES 1000 // The maximum number of moves the player may make during a single game
#define MAX_MOVES_EVENT (MAX_MOVES/4) // All random events must be displayed within this number of moves
#define MAX_RESPONSE_TRIES 3 // The number of times to ask player for a yes/no response and just assuming it is no

// Useful locations
#define LOCATION_AIRLOCKE 31 // The airlock just off the Recreation Hub
#define LOCATION_AIRLOCKEOUT 36 // The outside area just outside Airlock East
#define LOCATION_ANTEROOM 125 // The anteroom just below the observatory
#define LOCATION_CELLAR 29 // The secret cellar beneath the garden
#define LOCATION_CHECKPOINT 32 // The security checkpoint between the Recreation Hub and the Control Hub
#define LOCATION_COMMS 51 // The communications room
#define LOCATION_CONTAINER 2 // When an item has this location, it means it is in a container
#define LOCATION_DOCKING 21 // The west end of the docking bay
#define LOCATION_DOCKINGCONTROL 19 // The docking control area
#define LOCATION_FACTORY 118 // The factory containing the machine
#define LOCATION_GARDEN 27 // The garden dome, at ground level
#define LOCATION_GRAVEYARD 3 // Where Items go when they have left the game AS INTENDED (as opposed to by wanton destruction)
#define LOCATION_HOT 87 // The Hot room
#define LOCATION_INVENTORY 1 // When an item has this location, it means it is in the inventory
#define LOCATION_OBSERVATORY 126 // The upper part of the observatory
#define LOCATION_LAUNCH 102 // Where you launch the dinghy from
#define LOCATION_NOWHERE 0 // The nothing place
#define LOCATION_RECHUB 34 // The recreation hub
#define LOCATION_REFLECT 120 // The Reflection room
#define LOCATION_RIVERBRIDGE 101 // On the river, under the bridge
#define LOCATION_SAFE_0 LOCATION_RECHUB
#define LOCATION_SAFE_1 50
#define LOCATION_SENSOR 52 // The sensor room off the control hub
#define LOCATION_SHIP 49 // The pirate ship
#define LOCATION_SHUTTLE 23 // The broken shuttle
#define LOCATION_SLEEP_0 91 // Location where player leaves/re-enters waking world
#define LOCATION_SLEEP_1 92 // Location where player arrives in dream world
#define LOCATION_SLEEP_2 97 // Location where player leaves dream world from
#define LOCATION_START_0 9 // The player's starting location
#define LOCATION_START_1 79
#define LOCATION_TELEPORT_0 107 // Location of teleporter connected to Experiment Area
#define LOCATION_TELEPORT_1 128 // Location of teleporter connected to Chasm
#define LOCATION_THOR 106 // The Thor room
#define LOCATION_TREASURESTORE 23 // The place the player must bring treasures to
#define LOCATION_TREETOP 28 // Location of the treetop in the garden (note: not the one in the dream)

/*
 *	Useful uint_64s
 */

// Testing command codes
#if defined (TESTING_READ)
#define CTRL_COMMAND_TESTING_AIMSIGH 0x61696D7369676800 // Command to tell current location of specified item
#define CTRL_COMMAND_TESTING_NODE 0x6E6F646500000000 // Command to echo location ID no to terminal
#endif
#if defined (TESTING_READ) && defined (TESTING_WRITE)
#define CTRL_COMMAND_TESTING_FLASH 0x666C617368000000 // Command to jump to any other location in the game
#define CTRL_COMMAND_TESTING_GRAB 0x6772616200000000 // Command to zap any item in the game into the inventory
#endif

// Movement command codes
#define CMD_GO 0x676F000000000000
#define CMD_NORTH 0x6E6F727468000000
#define CMD_SOUTH 0x736F757468000000
#define CMD_EAST 0x6561737400000000
#define CMD_WEST 0x7765737400000000
#define CMD_NORTHEAS 0x6E6F727468656173
#define CMD_SOUTHWES 0x736F757468776573
#define CMD_SOUTHEAS 0x736F757468656173
#define CMD_NORTHWES 0x6E6F727468776573
#define CMD_UP 0x7570000000000000
#define CMD_DOWN 0x646F776E00000000
#define CMD_OUT 0x6F75740000000000
#define CMD_BACK 0x6261636B00000000

// No-arg command codes
#define CMD_ABRACADA 0x6162726163616461
#define CMD_AVNARAND 0x61766E6172616E64
#define CMD_CHIMBU 0x6368696D62750000
#define CMD_COMMANDS 0x636F6D6D616E6473
#define CMD_CSB 0x6373620000000000
#define CMD_FISH 0x6669736800000000
#define CMD_HELP 0x68656C7000000000
#define CMD_INVENTOR 0x696E76656E746F72
#define CMD_KNIT 0x6B6E697400000000
#define CMD_LOOK 0x6C6F6F6B00000000
#define CMD_PLUGH 0x706C756768000000
#define CMD_QUIT 0x7175697400000000
#define CMD_SCORE 0x73636F7265000000
#define CMD_SLEEP 0x736C656570000000
#define CMD_STARE 0x7374617265000000
#define CMD_TEZAZZLE 0x74657A617A7A6C65
#define CMD_VERSION 0x76657273696F6E00
#define CMD_XYRO 0x7879726F00000000
#define CMD_XYZZY 0x78797A7A79000000
#define CMD_ZIQUA 0x7A69717561000000

// Any-arg command codes
#define CMD_CALL 0x63616C6C00000000
#define CMD_EXPLAIN 0x6578706C61696E00
#define CMD_FLY 0x666C790000000000
#define CMD_HINT 0x68696E7400000000
#define CMD_SAY 0x7361790000000000
#define CMD_TETHER 0x7465746865720000

// Present-arg command codes
#define CMD_ATTACK 0x61747461636B0000
#define CMD_BURN 0x6275726E00000000
#define CMD_DESCRIBE 0x6465736372696265
#define CMD_EMPTY 0x656D707479000000
#define CMD_IGNORE 0x69676E6F72650000
#define CMD_LIGHT 0x6C69676874000000
#define CMD_LOCK 0x6C6F636B00000000
#define CMD_OPEN 0x6F70656E00000000
#define CMD_PLAY 0x706C617900000000
#define CMD_PUSH 0x7075736800000000
#define CMD_QUENCH 0x7175656E63680000
#define CMD_READ 0x7265616400000000
#define CMD_REPAIR 0x7265706169720000
#define CMD_ROB	0x726F620000000000
#define CMD_RUB 0x7275620000000000
#define CMD_TAKE 0x74616B6500000000
#define CMD_UNLOCK 0x756E6C6F636B0000
#define CMD_WATER 0x7761746572000000

// Inventory-arg command codes
#define CMD_COOK 0x636F6F6B00000000
#define CMD_DRINK 0x6472696E6B000000
#define CMD_DROP 0x64726F7000000000
#define CMD_EAT 0x6561740000000000
#define CMD_EXCHANGE 0x65786368616E6765
#define CMD_FREE 0x6672656500000000
#define CMD_GIVE 0x6769766500000000
#define CMD_INSERT 0x696E736572740000
#define CMD_POUR 0x706F757200000000
#define CMD_THROW 0x7468726F77000000

// Items
#define ITEM_ACORN 0x61636F726E000000
#define ITEM_ALIEN 0x616C69656E000000
#define ITEM_AQUA 0x6171756100000000
#define ITEM_BEAN 0x6265616E00000000
#define ITEM_BEANSTAL 0x6265616E7374616C
#define ITEM_BLOSSOM 0x626C6F73736F6D00
#define ITEM_BOOK 0x626F6F6B00000000
#define ITEM_BOOTS 0x626F6F7473000000
#define ITEM_BOULDER 0x626F756C64657200
#define ITEM_BREAD 0x6272656164000000
#define ITEM_BROOCH 0x62726F6F63680000
#define ITEM_BUCCANEE 0x62756363616E6565
#define ITEM_BUILDING 0x6275696C64696E67
#define ITEM_BUTTON 0x627574746F6E0000
#define ITEM_CABLE 0x6361626C65000000
#define ITEM_CARTRIDG 0x6361727472696467
#define ITEM_CAULDRON 0x6361756C64726F6E
#define ITEM_CHART 0x6368617274000000
#define ITEM_COIN 0x636F696E00000000
#define ITEM_CONSOLE 0x636F6E736F6C6500
#define ITEM_CORSAIR 0x636F727361697200
#define ITEM_DINGHY 0x64696E6768790000
#define ITEM_DOGS 0x646F677300000000
#define ITEM_DRAGON 0x647261676F6E0000
#define ITEM_DUST 0x6475737400000000
#define ITEM_ELIXIR 0x656C697869720000
#define ITEM_ENVELOPE 0x656E76656C6F7065
#define ITEM_FAIRY 0x6661697279000000
#define ITEM_GLINT 0x676C696E74000000
#define ITEM_GUNSLING 0x67756E736C696E67
#define ITEM_JUMPER 0x6A756D7065720000
#define ITEM_KEY 0x6B65790000000000
#define ITEM_KOHLRABI 0x6B6F686C72616269
#define ITEM_LAMP 0x6C616D7000000000
#define ITEM_LENS 0x6C656E7300000000
#define ITEM_LION 0x6C696F6E00000000
#define ITEM_MACHINE 0x6D616368696E6500
#define ITEM_MAGAZINE 0x6D6167617A696E65
#define ITEM_MATCHES 0x6D61746368657300
#define ITEM_MEDALLIO 0x6D6564616C6C696F
#define ITEM_MILK 0x6D696C6B00000000
#define ITEM_MIRROR 0x6D6972726F720000
#define ITEM_NEEDLES 0x6E6565646C657300
#define ITEM_NET 0x6E65740000000000
#define ITEM_NUGGET 0x6E75676765740000
#define ITEM_NULL 0x6E756C6C00000000
#define ITEM_PANEL 0x70616E656C000000
#define ITEM_PENDANT 0x70656E64616E7400
#define ITEM_PLANT 0x706C616E74000000
#define ITEM_POTION 0x706F74696F6E0000
#define ITEM_RADISHES 0x7261646973686573
#define ITEM_ROBOT 0x726F626F74000000
#define ITEM_ROD 0x726F640000000000
#define ITEM_SHIP 0x7368697000000000
#define ITEM_SHUTTLE 0x73687574746C6500
#define ITEM_SKELETON 0x736B656C65746F6E
#define ITEM_TOAST 0x746F617374000000
#define ITEM_TOOTH 0x746F6F7468000000
#define ITEM_TRANSMIT 0x7472616E736D6974
#define ITEM_TROLL 0x74726F6C6C000000
#define ITEM_WATER 0x7761746572000000
#define ITEM_WHISTLE 0x77686973746C6500
#define ITEM_WIRE 0x7769726500000000
#define ITEM_WIZARD 0x77697A6172640000
#define ITEM_WOLF 0x776F6C6600000000
#define ITEM_YARN 0x7961726E00000000

#define HINT_DEFAULT 0x64656661756C7400 // Just the word "default" in code form

// Codes for strings
#define STR_ALIBEACO 0x616C69626561636F // What alien says when player brings beacon
#define STR_ALICHART 0x616C696368617274 // What alien says when player brings chart
#define STR_ALILENS 0x616C696C656E7300 // What alien says when player brings lens
#define STR_ALINOUSE 0x616C696E6F757365 // When alien has no use for an item that it will have use for in the future
#define STR_ALREADON 0x616C726561646F6E
#define STR_ALREADOF 0x616C726561646F66
#define STR_AMBIG 0x616D626967000000 // Player's response to a question was ambiguous
#define STR_ASH 0x6173680000000000
#define STR_ASHMOUSE 0x6173686D6F757365
#define STR_ASKOK 0x61736B6F6B000000 // Asking player whether a proposal is all right
#define STR_ASKREINC 0x61736B7265696E63
#define STR_ASKSURE 0x61736B7375726500 // Asking player whether they are sure
#define STR_ASSUMENO 0x617373756D656E6F // I will take that as a no
#define STR_BEANSTAL 0x6265616E7374616C
#define STR_BOULDPUL 0x626F756C6470756C
#define STR_BREAD 0x6272656164000000
#define STR_CABBAGE 0x6361626261676500
#define STR_CANTSEE 0x63616E7473656500
#define STR_CANTSEEH 0x63616E7473656568
#define STR_CHOOSE 0x63686F6F73650000 // When player is asked to choose
#define STR_CONSOLE 0X636F6E736f6C6500
#define STR_CONTFULL 0x636F6E7466756C6C
#define STR_CONTRECU 0x636F6E7472656375
#define STR_DESREINC 0x6465737265696E63
#define STR_DISTRESS 0x6469737472657373
#define STR_DOREINC 0x646F7265696E6300
#define STR_DRAGON 0x647261676F6E0000
#define STR_DRINK 0x6472696E6B000000
#define STR_DRINKAQU 0x6472696E6B617175
#define STR_DRINKELI 0x6472696E6B656C69
#define STR_DRINKPOT 0x6472696E6B706F74
#define STR_DRINKWAT 0x6472696E6B776174
#define STR_DROPFALL 0x64726F7066616C6C
#define STR_DROPGOOD 0x64726F70676F6F64
#define STR_DROWN 0x64726F776E000000 // When player drowns
#define STR_DROWNITM 0x64726F776E69746D // When an item is dropped into water (lost)
#define STR_ENOUGH 0x656E6F7567680000 // When player has "had enough" of an item
#define STR_ERROR 0x6572726F72000000
#define STR_EXHAUST 0x6578686175737400 // When player drops dead of exhaustion (too many moves made)
#define STR_FAIRY 0x6661697279000000
#define STR_GENIE 0x67656E6965000000
#define STR_GIVEYAWN 0x676976657961776E
#define STR_GUNSLING 0x67756E736C696E67 // When player gives magazine to gunslinger
#define STR_HELLO 0x68656C6C6F000000 // A simple greeting
#define STR_HELP 0x68656C7000000000
#define STR_HIBEACON 0x6869626561636F6E // When alien wants player to bring beacon
#define STR_HICHART 0x6869636861727400 // When alien wants player to bring chart
#define STR_HILENS 0x68696C656E730000 // When alien wants player to bring lens
#define STR_HINTWARN 0x68696E747761726E // Warning the player of point penalty for asking for a hint
#define STR_IKR 0x696B720000000000
#define STR_INITIAL 0x696E697469616C00
#define STR_KNITJUMP 0x6B6E69746A756D70
#define STR_LAMPNO 0x6C616D706E6F0000
#define STR_LIONKILL 0x6C696F6E6B696C6C
#define STR_LIONTUNE 0x6C696F6E74756E65
#define STR_LIONWHET 0x6C696F6E77686574
#define STR_LIONWOLF 0x6C696F6E776F6C66
#define STR_LIONYAWN 0x6C696F6E7961776E
#define STR_MACHINNO 0x6D616368696E6E6F // Machine does not know item player is requesting
#define STR_MACHIREJ 0x6D6163686972656A // Machine rejects what is offered in exchange
#define STR_MACHIWAT 0x6D61636869776174 // Machine wants to know what player wants in exchange
#define STR_MOVNOAIR 0x6D6F766E6F616972
#define STR_MOVNOCEI 0x6D6F766E6F636569
#define STR_MOVNODOW 0x6D6F766E6F646F77
#define STR_MOVNOLAN 0x6D6F766E6F6C616E // No land where player is going
#define STR_MOVNOOUT 0x6D6F766E6F6F7574
#define STR_MOVNOREM 0x6D6F766E6F72656D
#define STR_MOVNOWAY 0x6D6F766E6F776179
#define STR_MOVOBST 0x6D6F766F62737400
#define STR_N 0x6E00000000000000
#define STR_NO 0x6E6F000000000000
#define STR_NOAIR 0x6E6F616972000000 // When there is no air to breathe and player suffocates
#define STR_NOCONTAI 0x6E6F636F6E746169
#define STR_NOCOOKER 0x6E6F636F6F6B6572 // When there is no cauldron to cook in
#define STR_NOEQUIP 0x6E6F657175697000
#define STR_NOEXCHAN 0x6E6F65786368616E // Nowhere to exchange item
#define STR_NOFISH 0x6E6F666973680000
#define STR_NOFIT 0x6E6F666974000000
#define STR_NOFITWEA 0x6E6F666974776561
#define STR_NOFLOOR 0x6E6F666C6F6F7200 // When there is no floor at location and dropped item falls through
#define STR_NOGRAV 0x6E6F677261760000 // When there is no gravity and player floats away
#define STR_NOHAPPEN 0x6E6F68617070656E
#define STR_NOINTERE 0x6E6F696E74657265
#define STR_NOKEY 0x6E6F6B6579000000 // When player needs the key but does not have it
#define STR_NOLIGHT 0x6E6F6C6967687400
#define STR_NOMATCH 0x6E6F6D6174636800
#define STR_NONODRIN 0x6E6F6E6F6472696E // Player is trying to drink something that is not liquid
#define STR_NONOEMPT 0x6E6F6E6F656D7074 // Player is trying to empty an unemptyable item
#define STR_NONOHOW 0x6E6F6E6F686F7700
#define STR_NONOPOUR 0x6E6F6E6F706F7572
#define STR_NONOSWOF 0x6E6F6E6F73776F66 // Player is trying to switch off an item without a switch
#define STR_NONOSWON 0x6E6F6E6F73776F6E // Player is trying to switch on an item without a switch
#define STR_NONOWHAT 0x6E6F6E6F77686174
#define STR_NOOPTION 0x6E6F6F7074696F6E
#define STR_NOSHIP 0x6E6F736869700000 // When player tries to fly ship but is not in ship
#define STR_NOSTRONG 0x6E6F7374726F6E67
#define STR_NOTUIGIN 0x6E6F74756967696E
#define STR_NOTUIGSE 0x6E6F747569677365 // Player has made ambiguous selection
#define STR_NOTVALUE 0x6E6F7476616C7565 // Player offers item of insufficient value in exchange for building
#define STR_NOWISEAT 0x6E6F776973656174
#define STR_NOWRITIN 0x6E6F77726974696E
#define STR_NUGGGOOD 0x6E756767676F6F64
#define STR_OK 0x6F6B000000000000
#define STR_PENDRUB 0x70656E6472756200 // Player rubs pendant
#define STR_PHILISTI 0x7068696C69737469
#define STR_PIRATBOO 0x7069726174626F6F
#define STR_PIRATKIL 0x70697261746B696C
#define STR_PIRATNO 0x70697261746E6F00
#define STR_PIRATRB1 0x7069726174726231 // Successfully robbing blind pirate
#define STR_PIRATRB2 0x7069726174726232 // Successfully robbing deaf pirate
#define STR_PIRATSEE 0x7069726174736565
#define STR_PIRATVG1 0x7069726174766731 // When blind pirate is vigilant and player retreats
#define STR_PIRATVG2 0x7069726174766732 // When deaf pirate is vigilant and player retreats
#define STR_PIRATSN1 0x7069726174736E31 // When player attempts to sneak past blind pirate
#define STR_PIRATSN2 0x7069726174736E32 // When player attempts to sneak past deaf pirate
#define STR_PLUGH 0x706C756768000000
#define STR_RADICOOK 0x72616469636F6F6B // When player cooks radishes
#define STR_REFINVIS 0x726566696E766973 // Player stares at self in Reflection room while invisible
#define STR_REFNORM 0x7265666E6F726D00 // Player stares at self while normal
#define STR_REFNOTH 0x7265666E6F746800 // Player stares at a location other than Reflection room
#define STR_REFSTRON 0x7265667374726F6E // Player stares at self while strong
#define STR_ROBOT 0x726F626F74000000
#define STR_SHATFALL 0x7368617466616C6C // When a fragile item shatters on falling through the floor
#define STR_SHATHERE 0x7368617468657265 // When a fragile item shatters on being dropped carelessly
#define STR_SHATTHRO 0x736861747468726F // When a fragile item is thrown
#define STR_SHIP 0x7368697000000000
#define STR_SHIPALRE 0x73686970616C7265 // When player has already summoned the ship and attempts to call again
#define STR_SHIPFLY 0x73686970666C7900 // When player successfully flies away in pirate ship
#define STR_SHMAGIC 0x73686D6167696300
#define STR_SKELMILK 0x736B656C6D696C6B // When player gives milk to the skeleton
#define STR_SLEEP 0x736C656570000000
#define STR_SLEEPNO 0x736C6565706E6F00
#define STR_SNOMP 0x736E6F6D70000000 // Explanation of a snomp
#define STR_SNOMPEAT 0x736E6F6D70656174 // When a snomp happens along and eats player
#define STR_STUMBLE 0x7374756D626C6500
#define STR_TAKENOSE 0x74616B656E6F7365
#define STR_TAKEALRE 0x74616B65616C7265
#define STR_TAKENOCA 0x74616B656E6F6361
#define STR_TAKEGOOD 0x74616B65676F6F64
#define STR_TELEPORT 0x74656C65706F7274
#define STR_TETHER 0x7465746865720000
#define STR_TOAST 0x746F617374000000 // When the toast is burned
#define STR_TOASTALM 0x746F617374616C6D // When burning the toast sets off the alarm
#define STR_TROLL 0x74726F6C6C000000
#define STR_TROLLED 0x74726F6C6C656400
#define STR_TROLYAWN 0x74726F6C7961776E 
#define STR_VERSION 0x76657273696F6E00
#define STR_WEAR 0x7765617200000000
#define STR_WHICHWAY 0x7768696368776179
#define STR_WIZARDED 0x77697A6172646564
#define STR_WIZINVIS 0x77697A696E766973
#define STR_WIZMIRRO 0x77697A6D6972726F
#define STR_Y 0x7900000000000000
#define STR_YES 0x7965730000000000
#define STR_ZIQUA 0x7A69717561000000

/*
 *	Useful characters
 */
#define SPACE ' ' // Used everywhere, largely in string tokenising
#define DOT '.'
#define EXCLAMATION '!'
#define TAB '\t'
#define NL '\n'
#define REGEX_FILE TAB // Character used to delimit fields in data files

/*
 *	Useful strings
 */
#define DATAFILE "tarranta.dat"
#define FILE_DELIMITER_SECTION "---" // The string used to delimit sections of the datafile
