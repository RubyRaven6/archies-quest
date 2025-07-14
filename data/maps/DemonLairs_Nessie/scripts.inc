DemonLairs_Nessie_MapScripts::
	.byte 0


DemonLairs_Nessie_EventScript_NessiePuzzle::
	msgbox DemonLairs_Nessie_EventScript_NessiePuzzle_Text_0, MSGBOX_YESNO
	closemessage
	compare VAR_RESULT, YES
	goto_if_eq DemonLairs_Nessie_EventScript_NessiePuzzle_2
DemonLairs_Nessie_EventScript_NessiePuzzle_1:
	waitstate
	goto_if_set FLAG_NESSIE_PUZZLE_SOLVED, DemonLairs_Nessie_EventScript_NessiePuzzle_5
DemonLairs_Nessie_EventScript_NessiePuzzle_4:
	end

DemonLairs_Nessie_EventScript_NessiePuzzle_2:
	special OpenNessiePuzzle
	goto DemonLairs_Nessie_EventScript_NessiePuzzle_1

DemonLairs_Nessie_EventScript_NessiePuzzle_5:
	lockall
	msgbox DemonLairs_Nessie_EventScript_NessiePuzzle_Text_1, MSGBOX_DEFAULT
	waitmessage
	closemessage
	setvar VAR_0x8004, 1
	setvar VAR_0x8005, 1
	setvar VAR_0x8006, 8
	setvar VAR_0x8007, 5
	special ShakeCamera
	waitstate
	playse SE_M_ROCK_THROW
	setmetatile 2, 0, METATILE_NessereignLairPrimary_ENTRANCE_UPL, TRUE
	setmetatile 3, 0, METATILE_NessereignLairPrimary_ENTRANCE_UPM, TRUE
	setmetatile 4, 0, METATILE_NessereignLairPrimary_ENTRANCE_UPR, TRUE
	setmetatile 2, 1, METATILE_NessereignLairPrimary_ENTRANCE_ML, TRUE
	setmetatile 3, 1, METATILE_NessereignLairPrimary_ENTRANCE_MM, TRUE
	setmetatile 4, 1, METATILE_NessereignLairPrimary_ENTRANCE_MR, TRUE
	setmetatile 2, 2, METATILE_NessereignLairPrimary_ENTRANCE_DNL, TRUE
	setmetatile 3, 2, METATILE_NessereignLairPrimary_ENTRANCE_DNM, FALSE
	setmetatile 4, 2, METATILE_NessereignLairPrimary_ENTRANCE_DNR, TRUE
	special DrawWholeMapView
	goto DemonLairs_Nessie_EventScript_NessiePuzzle_4


DemonLairs_Nessie_EventScript_Knife::
	goto_if_unset FLAG_NESSIE_GOT_DAGGER, DemonLairs_Nessie_EventScript_Knife_2
	goto DemonLairs_Nessie_EventScript_Table
DemonLairs_Nessie_EventScript_Knife_1:
	end

DemonLairs_Nessie_EventScript_Knife_2:
	msgbox DemonLairs_Nessie_EventScript_Knife_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq DemonLairs_Nessie_EventScript_Knife_5
	goto DemonLairs_Nessie_EventScript_Knife_1

DemonLairs_Nessie_EventScript_Knife_5:
	setflag FLAG_NESSIE_GOT_DAGGER
	setmetatile 3, 7, METATILE_NessereignLairPrimary_TableCorner, TRUE
	special DrawWholeMapView
	closemessage
	goto DemonLairs_Nessie_EventScript_Knife_1


DemonLairs_Nessie_EventScript_Book::
	goto_if_unset FLAG_NESSIE_READ_BOOK, DemonLairs_Nessie_EventScript_Book_2
	msgbox DemonLairs_Nessie_EventScript_Book_Text_2
	closemessage
DemonLairs_Nessie_EventScript_Book_1:
	end

DemonLairs_Nessie_EventScript_Book_2:
	msgbox DemonLairs_Nessie_EventScript_Book_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq DemonLairs_Nessie_EventScript_Book_5
	goto DemonLairs_Nessie_EventScript_Book_1

DemonLairs_Nessie_EventScript_Book_5:
	msgbox DemonLairs_Nessie_EventScript_Book_Text_1
	setflag FLAG_NESSIE_READ_BOOK
	closemessage
	goto DemonLairs_Nessie_EventScript_Book_1


DemonLairs_Nessie_EventScript_Fireplace::
	msgbox DemonLairs_Nessie_EventScript_Fireplace_Text_0, MSGBOX_SIGN
	closemessage
	end


DemonLairs_Nessie_EventScript_Chair::
	switch VAR_TEMP_1
	case 0, DemonLairs_Nessie_EventScript_Chair_3
	case 1, DemonLairs_Nessie_EventScript_Chair_4
	msgbox DemonLairs_Nessie_EventScript_Chair_Text_2, MSGBOX_SIGN
DemonLairs_Nessie_EventScript_Chair_1:
	end

DemonLairs_Nessie_EventScript_Chair_3:
	msgbox DemonLairs_Nessie_EventScript_Chair_Text_0, MSGBOX_SIGN
	addvar VAR_TEMP_1, 1
	goto DemonLairs_Nessie_EventScript_Chair_1

DemonLairs_Nessie_EventScript_Chair_4:
	msgbox DemonLairs_Nessie_EventScript_Chair_Text_1, MSGBOX_SIGN
	addvar VAR_TEMP_1, 1
	goto DemonLairs_Nessie_EventScript_Chair_1


DemonLairs_Nessie_EventScript_Table::
	switch VAR_TEMP_2
	case 0, DemonLairs_Nessie_EventScript_Table_3
	case 1, DemonLairs_Nessie_EventScript_Table_4
	msgbox DemonLairs_Nessie_EventScript_Table_Text_2, MSGBOX_SIGN
DemonLairs_Nessie_EventScript_Table_1:
	closemessage
	end

DemonLairs_Nessie_EventScript_Table_3:
	msgbox DemonLairs_Nessie_EventScript_Table_Text_0, MSGBOX_SIGN
	addvar VAR_TEMP_2, 1
	goto DemonLairs_Nessie_EventScript_Table_1

DemonLairs_Nessie_EventScript_Table_4:
	msgbox DemonLairs_Nessie_EventScript_Table_Text_1, MSGBOX_SIGN
	addvar VAR_TEMP_2, 1
	goto DemonLairs_Nessie_EventScript_Table_1


DemonLairs_Nessie_EventScript_Rug::
	switch VAR_TEMP_3
	case 0, DemonLairs_Nessie_EventScript_Rug_3
	case 1, DemonLairs_Nessie_EventScript_Rug_4
	case 2, DemonLairs_Nessie_EventScript_Rug_5
	case 3, DemonLairs_Nessie_EventScript_Rug_6
	msgbox DemonLairs_Nessie_EventScript_Rug_Text_4, MSGBOX_SIGN
	addvar VAR_TEMP_3, 1
DemonLairs_Nessie_EventScript_Rug_1:
	closemessage
	end

DemonLairs_Nessie_EventScript_Rug_3:
	msgbox DemonLairs_Nessie_EventScript_Rug_Text_0, MSGBOX_SIGN
	addvar VAR_TEMP_3, 1
	goto DemonLairs_Nessie_EventScript_Rug_1

DemonLairs_Nessie_EventScript_Rug_4:
	msgbox DemonLairs_Nessie_EventScript_Rug_Text_1, MSGBOX_SIGN
	addvar VAR_TEMP_3, 1
	goto DemonLairs_Nessie_EventScript_Rug_1

DemonLairs_Nessie_EventScript_Rug_5:
	msgbox DemonLairs_Nessie_EventScript_Rug_Text_2, MSGBOX_SIGN
	addvar VAR_TEMP_3, 1
	goto DemonLairs_Nessie_EventScript_Rug_1

DemonLairs_Nessie_EventScript_Rug_6:
	msgbox DemonLairs_Nessie_EventScript_Rug_Text_3, MSGBOX_SIGN
	addvar VAR_TEMP_3, 1
	goto DemonLairs_Nessie_EventScript_Rug_1


DemonLairs_Nessie_EventScript_NessiePuzzle_Text_0:
	.string "It's an impressive painting.\nView it closer?$"

DemonLairs_Nessie_EventScript_NessiePuzzle_Text_1:
	.string "The wall is collapsing!$"

DemonLairs_Nessie_EventScript_Knife_Text_0:
	.string "You find a small dagger on the table.\p Take it?$"

DemonLairs_Nessie_EventScript_Book_Text_0:
	.string "It is an ancient tome whose pages are\nyellow with age. There are no words, but\lmerely looking makes you feel nauseous.\pContinue?$"

DemonLairs_Nessie_EventScript_Book_Text_1:
	.string "You thumb through and find gross\nand disgusting images. You strain until\lyou see a phrase.\p“Sheathing sharpness\nthrough screams.”\pIt feels like you learned something\nimportant.$"

DemonLairs_Nessie_EventScript_Book_Text_2:
	.string "“Sheathing sharpness\nthrough screams.”\pIt's a dizzying tome.\nNo need to see the rest.$"

DemonLairs_Nessie_EventScript_Fireplace_Text_0:
	.string "It's a fireplace.\nThere's ash, but no sign of burning.$"

DemonLairs_Nessie_EventScript_Chair_Text_0:
	.string "It's a humongous chair.\nFar too big for you.$"

DemonLairs_Nessie_EventScript_Chair_Text_1:
	.string "Thinking about how big the chair looks\nis giving you a headache.$"

DemonLairs_Nessie_EventScript_Chair_Text_2:
	.string "Thinking about how the table looks\nnormal but the chair doesn't gives you\lan even bigger headache.$"

DemonLairs_Nessie_EventScript_Table_Text_0:
	.string "It's an impressively made table.$"

DemonLairs_Nessie_EventScript_Table_Text_1:
	.string "Unlike everything else in the room, you\nactually quite like the table.$"

DemonLairs_Nessie_EventScript_Table_Text_2:
	.string "You wish you could take the table home,\nbut alas, it'd clash with your decor.$"

DemonLairs_Nessie_EventScript_Rug_Text_0:
	.string "It's a garish rug.$"

DemonLairs_Nessie_EventScript_Rug_Text_1:
	.string "It's still a garish rug.$"

DemonLairs_Nessie_EventScript_Rug_Text_2:
	.string "To no one's surprise, it's a garish rug.$"

DemonLairs_Nessie_EventScript_Rug_Text_3:
	.string "Unastonishingly, it's a garish rug.$"

DemonLairs_Nessie_EventScript_Rug_Text_4:
	.string "It's not getting any less garish.\nYou should move on.$"
