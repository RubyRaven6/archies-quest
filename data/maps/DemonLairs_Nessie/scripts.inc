DemonLairs_Nessie_MapScripts::
	.byte 0


DemonLairs_Nessie_EventScript_NessiePuzzle::
	msgbox DemonLairs_Nessie_EventScript_NessiePuzzle_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq DemonLairs_Nessie_EventScript_NessiePuzzle_2
DemonLairs_Nessie_EventScript_NessiePuzzle_1:
	goto_if_unset FLAG_NESSIE_FOUND_SOLUTION, DemonLairs_Nessie_EventScript_NessiePuzzle_5
	msgbox DemonLairs_Nessie_EventScript_NessiePuzzle_Text_2
	closemessage
DemonLairs_Nessie_EventScript_NessiePuzzle_4:
	end

DemonLairs_Nessie_EventScript_NessiePuzzle_2:
	special OpenNessiePuzzle
	goto DemonLairs_Nessie_EventScript_NessiePuzzle_1

DemonLairs_Nessie_EventScript_NessiePuzzle_5:
	msgbox DemonLairs_Nessie_EventScript_NessiePuzzle_Text_1
	closemessage
	goto DemonLairs_Nessie_EventScript_NessiePuzzle_4


DemonLairs_Nessie_EventScript_Knife::
	msgbox DemonLairs_Nessie_EventScript_Knife_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq DemonLairs_Nessie_EventScript_Knife_2
DemonLairs_Nessie_EventScript_Knife_1:
	end

DemonLairs_Nessie_EventScript_Knife_2:
	setmetatile 3, 7, METATILE_NessereignLairPrimary_TableCorner, TRUE
	special DrawWholeMapView
	closemessage
	goto DemonLairs_Nessie_EventScript_Knife_1


DemonLairs_Nessie_EventScript_Chair::
	msgbox DemonLairs_Nessie_EventScript_Chair_Text_0
	closemessage
	end


DemonLairs_Nessie_EventScript_Book::
	goto_if_unset FLAG_NESSIE_FOUND_SOLUTION, DemonLairs_Nessie_EventScript_Book_2
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
	setflag FLAG_NESSIE_FOUND_SOLUTION
	closemessage
	goto DemonLairs_Nessie_EventScript_Book_1


DemonLairs_Nessie_EventScript_NessiePuzzle_Text_0:
	.string "It's an impressive painting.\nView it closer?$"

DemonLairs_Nessie_EventScript_NessiePuzzle_Text_1:
	.string "No nessie solution$"

DemonLairs_Nessie_EventScript_NessiePuzzle_Text_2:
	.string "Yes nessie solution$"

DemonLairs_Nessie_EventScript_Knife_Text_0:
	.string "You find a small dagger on the table.\p Take it?$"

DemonLairs_Nessie_EventScript_Chair_Text_0:
	.string "This chair inhumanely huge.$"

DemonLairs_Nessie_EventScript_Book_Text_0:
	.string "It's an ancient tome with yellowed pages.\nThere are no words, but merely looking\lmakes you feel nauseous.\pContinue?$"

DemonLairs_Nessie_EventScript_Book_Text_1:
	.string "You strain your eyes, and through the\ngrotesque images, you see a phrase.\p“Sheathing sharpness through screams”.\pIt feels like you learned something\nimportant.$"

DemonLairs_Nessie_EventScript_Book_Text_2:
	.string "It's a dizzying tome.\nNo need to reread it.$"
