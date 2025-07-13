DemonLairs_Nessie_MapScripts::
	.byte 0


DemonLairs_Nessie_EventScript_NessiePuzzle::
	msgbox DemonLairs_Nessie_EventScript_NessiePuzzle_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, TRUE
	goto_if_eq DemonLairs_Nessie_EventScript_NessiePuzzle_2
DemonLairs_Nessie_EventScript_NessiePuzzle_1:
	end

DemonLairs_Nessie_EventScript_NessiePuzzle_2:
	special OpenNessiePuzzle
	goto DemonLairs_Nessie_EventScript_NessiePuzzle_1


DemonLairs_Nessie_EventScript_NessiePuzzle_Text_0:
	.string "It's an impressive painting.\nView it closer?$"
