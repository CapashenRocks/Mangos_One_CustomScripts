/*
JerCore Crafting Perks

Custom dummy spell that can be learned on the profession trainer that grants speed bonuses.

Speed Increase can be changed here, kept hardcoded here for better ownership and reduce potential abuse issues.

Note: If you don't want to make custom spells then for the crafting perk tie it to any spell say for tailoring 
it can be spell 26791 (Master Tailor) so that when that is learned at 300 the tailor also gets the speed perk, just need to update spells below.
comment added in each spell field so it is clear what needs updated for each perk.
*/

#include "CraftingPerks.h"
#include "Unit.h"
#include "Player.h"
#include "SpellMgr.h"       
#include "DBCStores.h"        

#include <math.h> 

namespace JerCore
{
    namespace CraftingPerks
    {
        static float ClampMult(float mult)
        {
            if (mult < 0.05f) mult = 0.05f;
            if (mult > 1.0f)  mult = 1.0f;
            return mult;
        }

        static void ApplyMult(int32& castTime, float mult)
        {
            mult = ClampMult(mult);

            float v = float(castTime) * mult;
            castTime = int32(floorf(v + 0.5f));

            if (castTime < 250)
                castTime = 250;
        }

        static bool IsCreateItemSpell(SpellEntry const* sp)
        {
            if (!sp)
                return false;

            for (uint8 i = 0; i < 3; ++i)
            {
                if (sp->Effect[i] == SPELL_EFFECT_CREATE_ITEM)
                    return true;
            }
            return false;
        }

        static bool IsSpellInSkillLine(uint32 spellId, uint32 skillId)
        {
            SkillLineAbilityMapBounds b = sSpellMgr.GetSkillLineAbilityMapBounds(spellId);
            for (SkillLineAbilityMap::const_iterator it = b.first; it != b.second; ++it)
                if (it->second && it->second->skillId == skillId)
                    return true;
            return false;
        }

        void ApplyCastTimePerks(Unit* caster, SpellEntry const* spellInfo, int32& castTime)
        {
            if (!caster || !spellInfo || castTime <= 0)
                return;

            if (caster->GetTypeId() != TYPEID_PLAYER)
                return;

            Player* plr = (Player*)caster;

            // Mining 25% Speed Increase
     
			static const uint32 MINING_NODE_PERK_SPELL = 54009; //chage to your custom spell or existing spell id
            static const float  MINING_NODE_MULT = 0.75f;

            static const uint32 MINING_GATHER_SPELLS[] =
            {
                2575, 2576, 3564, 10248, 29354
            };

            auto IsMiningGatherSpell = [](uint32 id) -> bool
                {
                    for (uint32 s : MINING_GATHER_SPELLS)
                        if (s == id) return true;
                    return false;
                };

            if (plr->HasSpell(MINING_NODE_PERK_SPELL) && IsMiningGatherSpell(spellInfo->Id))
            {
                ApplyMult(castTime, MINING_NODE_MULT);
                return;
            }

            if (!IsCreateItemSpell(spellInfo))
                return;

            // Tailoring 25% Speed Increase
            static const uint32 TAILOR_PERK_SPELL = 54010; //chage to your custom spell or existing spell id
            static const uint32 SKILL_TAILORING = 197;
            static const float  TAILOR_MULT = 0.75f;

            if (plr->HasSpell(TAILOR_PERK_SPELL) && IsSpellInSkillLine(spellInfo->Id, SKILL_TAILORING))
            {
                ApplyMult(castTime, TAILOR_MULT);
                return;
            }

            // Blacksmithing 25% Speed Increase
            static const uint32 BS_PERK_SPELL = 54011; //chage to your custom spell or existing spell id
            static const uint32 SKILL_BS = 164;
            static const float  BS_MULT = 0.75f;

            if (plr->HasSpell(BS_PERK_SPELL) && IsSpellInSkillLine(spellInfo->Id, SKILL_BS))
            {
                ApplyMult(castTime, BS_MULT);
                return;
            }

            // Leatherworking 25% Speed Increase
            static const uint32 LW_PERK_SPELL = 54012; //chage to your custom spell or existing spell id
            static const uint32 SKILL_LW = 165;
            static const float  LW_MULT = 0.75f;

            if (plr->HasSpell(LW_PERK_SPELL) && IsSpellInSkillLine(spellInfo->Id, SKILL_LW))
            {
                ApplyMult(castTime, LW_MULT);
                return;
            }

            // Engineering 20% Speed Increase
            static const uint32 ENG_PERK_SPELL = 54013; //chage to your custom spell or existing spell id
            static const uint32 SKILL_ENG = 202;
            static const float  ENG_MULT = 0.80f;

            if (plr->HasSpell(ENG_PERK_SPELL) && IsSpellInSkillLine(spellInfo->Id, SKILL_ENG))
            {
                ApplyMult(castTime, ENG_MULT);
                return;
            }

            // Alchemy 20% Speed Increase
            static const uint32 ALCH_PERK_SPELL = 54014; //chage to your custom spell or existing spell id
            static const uint32 SKILL_ALCH = 171;
            static const float  ALCH_MULT = 0.80f;

            if (plr->HasSpell(ALCH_PERK_SPELL) && IsSpellInSkillLine(spellInfo->Id, SKILL_ALCH))
            {
                ApplyMult(castTime, ALCH_MULT);
                return;
            }

            // Mining: Smelting Increase of 20%
            static const uint32 SMELT_PERK_SPELL = 54015; //chage to your custom spell or existing spell id
            static const uint32 SKILL_MINING = 186;
            static const float  SMELT_MULT = 0.80f;

            if (plr->HasSpell(SMELT_PERK_SPELL) && IsSpellInSkillLine(spellInfo->Id, SKILL_MINING))
            {
                ApplyMult(castTime, SMELT_MULT);
                return;
            }
            // Herbalistm: Herbing Speed Increase of 20 %
                static const uint32 HERB_PERK_SPELL = 54016; //chage to your custom spell or existing spell id
            static const uint32 SKILL_HERBALISM = 182;
            static const float  HERB_MULT = 0.80f;

            if (plr->HasSpell(HERB_PERK_SPELL) && IsSpellInSkillLine(spellInfo->Id, SKILL_HERBALISM))
            {
                ApplyMult(castTime, HERB_MULT);
                return;
            }
            // Enchanting: Enchanting Speed Increase of 20 %
                static const uint32 ENCH_PERK_SPELL = 54017; //chage to your custom spell or existing spell id
            static const uint32 SKILL_ENCHANTING = 333;
            static const float  ENCH_MULT = 0.80f;

            if (plr->HasSpell(ENCH_PERK_SPELL) && IsSpellInSkillLine(spellInfo->Id, SKILL_ENCHANTING))
            {
                ApplyMult(castTime, ENCH_MULT);
                return;
            }
            // Skinning: Skinning Speed Increase of 20 %
            static const uint32 SKIN_PERK_SPELL = 54018; //chage to your custom spell or existing spell id
            static const uint32 SKILL_SKINNING = 393;
            static const float  SKIN_MULT = 0.80f;

            if (plr->HasSpell(SKIN_PERK_SPELL) && IsSpellInSkillLine(spellInfo->Id, SKILL_SKINNING))
            {
                ApplyMult(castTime, SKIN_MULT);
                return;
            }
            // Jewelcrafting: Jewelcrafting Speed Increase of 20 %
            static const uint32 JC_PERK_SPELL = 54019; //chage to your custom spell or existing spell id
            static const uint32 SKILL_JEWELCRAFTING = 755;
            static const float  JC_MULT = 0.80f;

            if (plr->HasSpell(ENCH_PERK_SPELL) && IsSpellInSkillLine(spellInfo->Id, SKILL_JEWELCRAFTING))
            {
                ApplyMult(castTime, JC_MULT);
                return;
            }
        }
    }
}
