EventScript_RepelWoreOff::
	lockall
	checkitemvar VAR_LAST_USED_REPEL, 1
	compare VAR_RESULT, FALSE
	goto_if_eq EventScript_RepelFinished
	bufferitemname 1, VAR_LAST_USED_REPEL
	msgbox Text_RepelWoreOffAskAnother, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq EventScript_RepelUseAnother
EventScript_RepelFinished::
	msgbox Text_RepelWoreOff, MSGBOX_SIGN
	releaseall
	end

EventScript_RepelUseAnother::
	special Script_TryUseAnotherRepel
	playse SE_REPEL
	msgbox gText_UsedVar2WildRepelled, MSGBOX_SIGN
	releaseall
	end

Text_RepelWoreOff::
	.string "REPEL's effect wore off…$"

Text_RepelWoreOffAskAnother:
	.string "REPEL's effect wore off…\n"
	.string "Use another {STR_VAR_2}?$"
