#ifndef JERCORE_CRAFTING_PERKS_H
#define JERCORE_CRAFTING_PERKS_H

#include "Common.h"

class Unit;
struct SpellEntry;

namespace JerCore
{
    namespace CraftingPerks
    {
        void ApplyCastTimePerks(Unit* caster, SpellEntry const* spellInfo, int32& castTime);
    }
}

#endif
