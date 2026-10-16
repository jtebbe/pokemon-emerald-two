EventScript_CancelMessageBox::
    closemessage
    releaseall
    end

EventScript_SelectWithoutRegisteredItem::
    msgbox Engine_Text_NoRegisteredItem, MSGBOX_DEFAULT
    releaseall
    end

EventScript_AccessPokemonBoxLink::
    lockall
    playse SE_PC_LOGIN
    special ShowPokemonStorageSystemPC
    releaseall
    end

Engine_Text_NoRegisteredItem:
    .string "Register a key item in the BAG first.$"
