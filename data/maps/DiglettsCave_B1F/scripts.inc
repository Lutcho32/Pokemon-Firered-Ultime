DiglettsCave_B1F_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, DiglettsCave_B1F_OnTransition
	.byte 0

DiglettsCave_B1F_OnTransition::
	setworldmapflag FLAG_WORLD_MAP_DIGLETTS_CAVE_B1F
	end

DiglettsCave_B1F_EventScript_Brawly::
    lock
    faceplayer
    goto_if_set FLAG_DEFEATED_BRAWLY_DIGLETTS_CAVE, DiglettsCave_B1F_EventScript_BrawlyDefeated
    msgbox DiglettsCave_B1F_Text_BrawlyIntro, MSGBOX_YESNO
    compare VAR_RESULT, NO
    goto_if_eq DiglettsCave_B1F_EventScript_BrawlyRefused

    msgbox DiglettsCave_B1F_Text_BrawlyAccept, MSGBOX_DEFAULT
    trainerbattle_no_intro TRAINER_BRAWLY_DIGLETTS_CAVE, DiglettsCave_B1F_Text_BrawlyDefeat

    msgbox DiglettsCave_B1F_Text_BrawlyReward, MSGBOX_DEFAULT
    giveitem ITEM_TM08, 1
    giveitem ITEM_BLACK_BELT, 1
    setflag FLAG_DEFEATED_BRAWLY_DIGLETTS_CAVE
    release
    end

DiglettsCave_B1F_EventScript_BrawlyRefused::
    msgbox DiglettsCave_B1F_Text_BrawlyRefusedText, MSGBOX_DEFAULT
    release
    end

DiglettsCave_B1F_EventScript_BrawlyDefeated::
    msgbox DiglettsCave_B1F_Text_BrawlyPostBattle, MSGBOX_DEFAULT
    release
    end

DiglettsCave_B1F_Text_BrawlyIntro:
    .string "Yo ! Quel endroit parfait pour s'entraîner !\n"
    .string "Ces Taupiqueur bougent à une vitesse folle,\n"
    .string "c'est parfait pour bosser les réflexes !\p"
    .string "Je suis Bastien, Champion d'arène de\n"
    .string "Myokara à Hoenn. \p"
    .string "Ça te dit de te mesurer à la puissance de\n"
    .string "mes vagues de choc physiques ?$"

DiglettsCave_B1F_Text_BrawlyAccept:
    .string "Génial ! Prépare-toi à encaisser la force\n"
    .string "pure de mon entraînement !$"

DiglettsCave_B1F_Text_BrawlyRefusedText:
    .string "Pas de souci, reste zen. Mais tu rates un\n"
    .string "sacré échauffement !$"

DiglettsCave_B1F_Text_BrawlyDefeat:
    .string "Whoah ! Tu as surfé sur mes attaques et\n"
    .string "trouvé le timing parfait pour me balayer !$"

DiglettsCave_B1F_Text_BrawlyReward:
    .string "Tu as un mental d'acier et des poings en\n"
    .string "béton. Prends ça, ça va booster ton potentiel !$"

DiglettsCave_B1F_Text_BrawlyPostBattle:
    .string "Le secret du combat, c'est d'encaisser la\n"
    .string "pression sans jamais rompre, comme la roche.\n"
    .string "Bonne route pour tes badges !$"
