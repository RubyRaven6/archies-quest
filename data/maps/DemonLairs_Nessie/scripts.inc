DemonLairs_Nessie_MapScripts::
	.byte 0


DemonLairs_Nessie_EventScript_NessiePuzzle::
# 6 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	msgbox DemonLairs_Nessie_EventScript_NessiePuzzle_Text_0, MSGBOX_YESNO
# 7 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	closemessage
# 8 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	compare VAR_RESULT, YES
	goto_if_eq DemonLairs_Nessie_EventScript_NessiePuzzle_2
DemonLairs_Nessie_EventScript_NessiePuzzle_1:
# 11 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	waitstate
# 12 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	goto_if_set FLAG_NESSIE_PUZZLE_SOLVED, DemonLairs_Nessie_EventScript_NessiePuzzle_5
DemonLairs_Nessie_EventScript_NessiePuzzle_4:
	end

DemonLairs_Nessie_EventScript_NessiePuzzle_2:
# 9 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	special OpenNessiePuzzle
	goto DemonLairs_Nessie_EventScript_NessiePuzzle_1

DemonLairs_Nessie_EventScript_NessiePuzzle_5:
# 13 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	lockall
# 14 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	msgbox DemonLairs_Nessie_EventScript_NessiePuzzle_Text_1, MSGBOX_DEFAULT
# 15 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	waitmessage
# 16 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	closemessage
# 17 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	setvar VAR_0x8004, 1
# 18 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	setvar VAR_0x8005, 1
# 19 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	setvar VAR_0x8006, 8
# 20 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	setvar VAR_0x8007, 5
# 21 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	special ShakeCamera
# 22 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	waitstate
# 23 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	playse SE_M_ROCK_THROW
# 24 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	setmetatile 2, 0, METATILE_NessereignLairPrimary_ENTRANCE_UPL, TRUE
# 25 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	setmetatile 3, 0, METATILE_NessereignLairPrimary_ENTRANCE_UPM, TRUE
# 26 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	setmetatile 4, 0, METATILE_NessereignLairPrimary_ENTRANCE_UPR, TRUE
# 27 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	setmetatile 2, 1, METATILE_NessereignLairPrimary_ENTRANCE_ML, TRUE
# 28 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	setmetatile 3, 1, METATILE_NessereignLairPrimary_ENTRANCE_MM, TRUE
# 29 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	setmetatile 4, 1, METATILE_NessereignLairPrimary_ENTRANCE_MR, TRUE
# 30 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	setmetatile 2, 2, METATILE_NessereignLairPrimary_ENTRANCE_DNL, TRUE
# 31 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	setmetatile 3, 2, METATILE_NessereignLairPrimary_ENTRANCE_DNM, FALSE
# 32 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	setmetatile 4, 2, METATILE_NessereignLairPrimary_ENTRANCE_DNR, TRUE
# 33 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	special DrawWholeMapView
	goto DemonLairs_Nessie_EventScript_NessiePuzzle_4


DemonLairs_Nessie_EventScript_Knife::
# 39 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	goto_if_unset FLAG_NESSIE_GOT_DAGGER, DemonLairs_Nessie_EventScript_Knife_2
# 48 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	goto DemonLairs_Nessie_EventScript_Table
DemonLairs_Nessie_EventScript_Knife_1:
	end

DemonLairs_Nessie_EventScript_Knife_2:
# 40 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	msgbox DemonLairs_Nessie_EventScript_Knife_Text_0, MSGBOX_YESNO
# 41 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	compare VAR_RESULT, YES
	goto_if_eq DemonLairs_Nessie_EventScript_Knife_5
	goto DemonLairs_Nessie_EventScript_Knife_1

DemonLairs_Nessie_EventScript_Knife_5:
# 42 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	setflag FLAG_NESSIE_GOT_DAGGER
# 43 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	setmetatile 3, 7, METATILE_NessereignLairPrimary_TableCorner, TRUE
# 44 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	special DrawWholeMapView
# 45 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	closemessage
	goto DemonLairs_Nessie_EventScript_Knife_1


DemonLairs_Nessie_EventScript_Book::
# 54 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	goto_if_unset FLAG_NESSIE_READ_BOOK, DemonLairs_Nessie_EventScript_Book_2
# 65 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	msgbox DemonLairs_Nessie_EventScript_Book_Text_2
# 66 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	closemessage
DemonLairs_Nessie_EventScript_Book_1:
	end

DemonLairs_Nessie_EventScript_Book_2:
# 56 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	msgbox DemonLairs_Nessie_EventScript_Book_Text_0, MSGBOX_YESNO
# 57 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	compare VAR_RESULT, YES
	goto_if_eq DemonLairs_Nessie_EventScript_Book_5
	goto DemonLairs_Nessie_EventScript_Book_1

DemonLairs_Nessie_EventScript_Book_5:
# 59 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	msgbox DemonLairs_Nessie_EventScript_Book_Text_1
# 60 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	setflag FLAG_NESSIE_READ_BOOK
# 61 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	closemessage
	goto DemonLairs_Nessie_EventScript_Book_1


DemonLairs_Nessie_EventScript_Fireplace::
# 72 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	msgbox DemonLairs_Nessie_EventScript_Fireplace_Text_0, MSGBOX_SIGN
# 73 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	closemessage
	end


DemonLairs_Nessie_EventScript_Chair::
# 78 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	switch VAR_TEMP_1
# 79 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	case 0, DemonLairs_Nessie_EventScript_Chair_3
# 82 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	case 1, DemonLairs_Nessie_EventScript_Chair_4
# 86 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	msgbox DemonLairs_Nessie_EventScript_Chair_Text_2, MSGBOX_SIGN
DemonLairs_Nessie_EventScript_Chair_1:
	end

DemonLairs_Nessie_EventScript_Chair_3:
# 80 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	msgbox DemonLairs_Nessie_EventScript_Chair_Text_0, MSGBOX_SIGN
# 81 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	addvar VAR_TEMP_1, 1
	goto DemonLairs_Nessie_EventScript_Chair_1

DemonLairs_Nessie_EventScript_Chair_4:
# 83 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	msgbox DemonLairs_Nessie_EventScript_Chair_Text_1, MSGBOX_SIGN
# 84 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	addvar VAR_TEMP_1, 1
	goto DemonLairs_Nessie_EventScript_Chair_1


DemonLairs_Nessie_EventScript_Table::
# 92 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	switch VAR_TEMP_2
# 93 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	case 0, DemonLairs_Nessie_EventScript_Table_3
# 96 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	case 1, DemonLairs_Nessie_EventScript_Table_4
# 100 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	msgbox DemonLairs_Nessie_EventScript_Table_Text_2, MSGBOX_SIGN
DemonLairs_Nessie_EventScript_Table_1:
# 102 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	closemessage
	end

DemonLairs_Nessie_EventScript_Table_3:
# 94 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	msgbox DemonLairs_Nessie_EventScript_Table_Text_0, MSGBOX_SIGN
# 95 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	addvar VAR_TEMP_2, 1
	goto DemonLairs_Nessie_EventScript_Table_1

DemonLairs_Nessie_EventScript_Table_4:
# 97 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	msgbox DemonLairs_Nessie_EventScript_Table_Text_1, MSGBOX_SIGN
# 98 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	addvar VAR_TEMP_2, 1
	goto DemonLairs_Nessie_EventScript_Table_1


DemonLairs_Nessie_EventScript_Rug::
# 108 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	switch VAR_TEMP_3
# 109 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	case 0, DemonLairs_Nessie_EventScript_Rug_3
# 112 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	case 1, DemonLairs_Nessie_EventScript_Rug_4
# 115 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	case 2, DemonLairs_Nessie_EventScript_Rug_5
# 118 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	case 3, DemonLairs_Nessie_EventScript_Rug_6
# 122 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	msgbox DemonLairs_Nessie_EventScript_Rug_Text_4, MSGBOX_SIGN
# 123 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	addvar VAR_TEMP_3, 1
DemonLairs_Nessie_EventScript_Rug_1:
# 125 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	closemessage
	end

DemonLairs_Nessie_EventScript_Rug_3:
# 110 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	msgbox DemonLairs_Nessie_EventScript_Rug_Text_0, MSGBOX_SIGN
# 111 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	addvar VAR_TEMP_3, 1
	goto DemonLairs_Nessie_EventScript_Rug_1

DemonLairs_Nessie_EventScript_Rug_4:
# 113 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	msgbox DemonLairs_Nessie_EventScript_Rug_Text_1, MSGBOX_SIGN
# 114 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	addvar VAR_TEMP_3, 1
	goto DemonLairs_Nessie_EventScript_Rug_1

DemonLairs_Nessie_EventScript_Rug_5:
# 116 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	msgbox DemonLairs_Nessie_EventScript_Rug_Text_2, MSGBOX_SIGN
# 117 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	addvar VAR_TEMP_3, 1
	goto DemonLairs_Nessie_EventScript_Rug_1

DemonLairs_Nessie_EventScript_Rug_6:
# 119 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	msgbox DemonLairs_Nessie_EventScript_Rug_Text_3, MSGBOX_SIGN
# 120 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	addvar VAR_TEMP_3, 1
	goto DemonLairs_Nessie_EventScript_Rug_1


DemonLairs_Nessie_EventScript_NessiePuzzle_Text_0:
# 6 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	.string "It's an impressive painting.\nView it closer?$"

DemonLairs_Nessie_EventScript_NessiePuzzle_Text_1:
# 14 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	.string "The wall is collapsing!$"

DemonLairs_Nessie_EventScript_Knife_Text_0:
# 40 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	.string "You find a small dagger on the table.\p Take it?$"

DemonLairs_Nessie_EventScript_Book_Text_0:
# 56 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	.string "It is an ancient tome whose pages are\nyellow with age. There are no words, but\lmerely looking makes you feel nauseous.\pContinue?$"

DemonLairs_Nessie_EventScript_Book_Text_1:
# 59 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	.string "You thumb through and find gross\nand disgusting images. You strain until\lyou see a phrase.\p“Sheathing sharpness\nthrough screams.”\pIt feels like you learned something\nimportant.$"

DemonLairs_Nessie_EventScript_Book_Text_2:
# 65 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	.string "“Sheathing sharpness\nthrough screams.”\pIt's a dizzying tome.\nNo need to see the rest.$"

DemonLairs_Nessie_EventScript_Fireplace_Text_0:
# 72 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	.string "It's a fireplace.\nThere's ash, but no sign of burning.$"

DemonLairs_Nessie_EventScript_Chair_Text_0:
# 80 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	.string "It's a humongous chair.\nFar too big for you.$"

DemonLairs_Nessie_EventScript_Chair_Text_1:
# 83 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	.string "Thinking about how big the chair looks\nis giving you a headache.$"

DemonLairs_Nessie_EventScript_Chair_Text_2:
# 86 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	.string "Thinking about how the table looks\nnormal but the chair doesn't gives you\lan even bigger headache.$"

DemonLairs_Nessie_EventScript_Table_Text_0:
# 94 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	.string "It's an impressively made table.$"

DemonLairs_Nessie_EventScript_Table_Text_1:
# 97 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	.string "Unlike everything else in the room, you\nactually quite like the table.$"

DemonLairs_Nessie_EventScript_Table_Text_2:
# 100 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	.string "You wish you could take the table home,\nbut alas, it'd clash with your decor.$"

DemonLairs_Nessie_EventScript_Rug_Text_0:
# 110 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	.string "It's a garish rug.$"

DemonLairs_Nessie_EventScript_Rug_Text_1:
# 113 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	.string "It's still a garish rug.$"

DemonLairs_Nessie_EventScript_Rug_Text_2:
# 116 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	.string "To no one's surprise, it's a garish rug.$"

DemonLairs_Nessie_EventScript_Rug_Text_3:
# 119 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	.string "Unastonishingly, it's a garish rug.$"

DemonLairs_Nessie_EventScript_Rug_Text_4:
# 122 "/home/rubyhax/archies-quest/data/maps/DemonLairs_Nessie/scripts.pory"
	.string "It's not getting any less garish.\nYou should move on.$"
