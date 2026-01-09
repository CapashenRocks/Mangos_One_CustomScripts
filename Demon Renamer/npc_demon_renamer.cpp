/*
JerCore Custom Demon Renamer Script
Once a day, a Warlock can pay 5 gold to rename one of thier summonded demons.

Under Review: Potential change to remove daily cap, but double cost each use.
*/


#include "precompiled.h"
#include "Player.h"
#include "Creature.h"
#include "Pet.h"
#include "Chat.h"
#include "Database/DatabaseEnv.h"

#include <string>
#include <cctype>

namespace
{
    static const uint32 GOLD_COPPER = 10000;
    static const uint32 COST_GOLD = 5;
    static const uint32 COST_COPPER = COST_GOLD * GOLD_COPPER;

    static const char* DAILY_TABLE = "custom_demon_rename_daily";

    enum
    {
        GOSSIP_MENU_TEXT = 1,
        ACTION_RENAME_START = 100,
        ACTION_RENAME_CODEBOX = 101
    };

    static bool HasUsedToday(uint32 ownerLow)
    {
        QueryResult* qr = CharacterDatabase.PQuery(
            "SELECT 1 FROM %s WHERE owner=%u AND last_rename_date=CURDATE()",
            DAILY_TABLE, ownerLow);

        if (!qr)
            return false;

        delete qr;
        return true;
    }

    static void MarkUsedToday(uint32 ownerLow)
    {
        CharacterDatabase.PExecute(
            "REPLACE INTO %s (owner, last_rename_date) VALUES (%u, CURDATE())",
            DAILY_TABLE, ownerLow);
    }

    static bool IsNameReasonable(std::string const& s)
    {
        if (s.size() < 2 || s.size() > 12)
            return false;

        for (char ch : s)
        {
            unsigned char c = static_cast<unsigned char>(ch);
            if (!std::isalpha(c))
                return false;
        }
        return true;
    }
}

struct npc_demon_renamer : public CreatureScript
{
            npc_demon_renamer() : CreatureScript("npc_demon_renamer") {}

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (!player || !creature)
            return true;

        if (player->getClass() != CLASS_WARLOCK)
        {
            creature->MonsterSay("You lack the knowledge to comprehend my arts. Begone!", LANG_UNIVERSAL, player);
            return true;
        }

        player->PlayerTalkClass->ClearMenus();

        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT,
            "Rename your current demon (5g, once per day).",
            GOSSIP_SENDER_MAIN, ACTION_RENAME_START);

        player->SEND_GOSSIP_MENU(GOSSIP_MENU_TEXT, creature->GetObjectGuid());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        if (!player || !creature)
            return true;

        player->PlayerTalkClass->ClearMenus();

        if (action != ACTION_RENAME_START)
        {
            player->CLOSE_GOSSIP_MENU();
            return true;
        }

        Pet* pet = player->GetPet();
        if (!pet)
        {
            ChatHandler(player).PSendSysMessage("|cffff0000You have no active demon summoned!|r");
            player->CLOSE_GOSSIP_MENU();
            return true;
        }

        uint32 ownerLow = player->GetGUIDLow();

        if (HasUsedToday(ownerLow))
        {
            creature->MonsterWhisper("You have already performed this ritual today. Return tomorrow.", player);
            player->CLOSE_GOSSIP_MENU();
            return true;
        }

        if (player->GetMoney() < COST_COPPER)
        {
            ChatHandler(player).PSendSysMessage("|cffff0000You need %u gold to rename your demon.|r", COST_GOLD);
            player->CLOSE_GOSSIP_MENU();
            return true;
        }

        player->ADD_GOSSIP_ITEM_EXTENDED(
            GOSSIP_ICON_CHAT,
            "Enter the new name for your demon:",
            GOSSIP_SENDER_MAIN,
            ACTION_RENAME_CODEBOX,
            "Letters only (2-12). Costs 5 gold. Once per day.",
            0,
            true);

        player->SEND_GOSSIP_MENU(GOSSIP_MENU_TEXT, creature->GetObjectGuid());
        return true;
    }

    bool OnGossipSelectWithCode(Player* player, Creature* creature, uint32 /*sender*/, uint32 action, const char* code) override
    {
        if (!player || !creature)
            return true;

        if (action != ACTION_RENAME_CODEBOX)
            return true;

        Pet* pet = player->GetPet();
        if (!pet)
        {
            ChatHandler(player).PSendSysMessage("|cffff0000Your demon has vanished.|r");
            player->CLOSE_GOSSIP_MENU();
            return true;
        }

        uint32 ownerLow = player->GetGUIDLow();

        if (HasUsedToday(ownerLow))
        {
            creature->MonsterWhisper("You have already performed this ritual today. Return tomorrow.", player);
            player->CLOSE_GOSSIP_MENU();
            return true;
        }

        if (!code || !*code)
        {
            ChatHandler(player).PSendSysMessage("|cffff0000Invalid name.|r");
            player->CLOSE_GOSSIP_MENU();
            return true;
        }

        std::string newName = code;
        if (newName.size() > 12)
            newName.resize(12);

        if (!IsNameReasonable(newName))
        {
            ChatHandler(player).PSendSysMessage("|cffff0000Name must be letters only (2-12).|r");
            player->CLOSE_GOSSIP_MENU();
            return true;
        }

        if (player->GetMoney() < COST_COPPER)
        {
            ChatHandler(player).PSendSysMessage("|cffff0000You need %u gold to rename your demon.|r", COST_GOLD);
            player->CLOSE_GOSSIP_MENU();
            return true;
        }

        player->ModifyMoney(-int32(COST_COPPER));

        CharacterDatabase.escape_string(newName);

        CharacterDatabase.PExecute(
            "UPDATE character_pet SET name='%s' WHERE owner=%u AND slot=0",
            newName.c_str(), ownerLow);

        pet->SetName(newName);

        MarkUsedToday(ownerLow);

        std::string msg = "To complete the ritual, " + std::string(player->GetName()) +
            ", you will need to resummon your demon.";
        creature->MonsterSay(msg.c_str(), LANG_UNIVERSAL, player);

        ChatHandler(player).PSendSysMessage("|cff00ff00Your demon's name is now:|r %s", newName.c_str());
       // Instead of a system message, will have the npc say this, but keeping in case of a later change
       // ChatHandler(player).SendSysMessage("|cff00ff00Your demon has been dismissed. Summon it again to complete the ritual.|r");

        pet->Unsummon(PET_SAVE_AS_CURRENT, player);

        player->CLOSE_GOSSIP_MENU();
        return true;

    }
};

void AddSC_custom_demon_renamer()
{
    Script* s;
    s = new npc_demon_renamer();
    s->RegisterSelf();
}
