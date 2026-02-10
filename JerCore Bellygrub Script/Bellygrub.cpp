/*
JerCore- Custom Bellygrub Encounter Script
Bellygrub in JerCore: The Dark Portal is now a 24 Elite npc in Redridge Mountains and has had his quest "An Unwelcome Guest" updated to reflect that.

The fight is designed to be more challenging and engaging, with two main triggers at 35% and 10% health.

The idea is this is a creature that likely can't be solod by a player until they are higher level and encourages group play.
*/

#include "precompiled.h"

enum
{
    NPC_GORETUSK_BOAR = 547, // can be changed to any creature, this is the Great Goretusk (unchanged creature_template they are 16-17)

    SPELL_STUN_56 = 56,
	SPELL_HEAL_POTION_441 = 441, // 280-360 heal

    SPELL_KNOCKDOWN_5164 = 5164,
};

static char const* EMOTE_CALL_HELP = "Bellygrub has called for help!";
static char const* EMOTE_HEAL_SELF = "Bellygrub heals!";

struct npc_bellygrub : public CreatureScript
{
    npc_bellygrub() : CreatureScript("npc_bellygrub") {}

    struct npc_bellygrubAI : public ScriptedAI
    {
        npc_bellygrubAI(Creature* pCreature) : ScriptedAI(pCreature)
        {
            sLog.outString("BellygrubAI: CONSTRUCTOR (guid=%u)", pCreature->GetGUIDLow());
        }

        bool m_Did35;
        bool m_Did10;

        void Reset() override
        {
            m_Did35 = false;
            m_Did10 = false;
        }

        void Aggro(Unit* /*pWho*/) override
        {
            sLog.outString("BellygrubAI: AGGRO");
        }

        void ForceAggro(Creature* pHelper, Unit* pVictim)
        {
            if (!pHelper || !pVictim)
                return;

            // Set the helpers to aggro (standard they are non-aggressive yellow)
            pHelper->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, m_creature->getFaction());

            pHelper->AddThreat(pVictim, 100000.0f);
            pHelper->SetInCombatWith(pVictim);
            pVictim->SetInCombatWith(pHelper);

            if (pHelper->AI())
                pHelper->AI()->AttackStart(pVictim);

            pHelper->GetMotionMaster()->MoveChase(pVictim);
        }

        void SummonTwoHelpers(Unit* pVictim)
        {
            if (!pVictim)
                return;

            m_creature->MonsterTextEmote(EMOTE_CALL_HELP, nullptr, true);

            float x, y, z;
            m_creature->GetPosition(x, y, z);

            for (uint8 i = 0; i < 2; ++i)
            {
                float sx = x + frand(-3.0f, 3.0f);
                float sy = y + frand(-3.0f, 3.0f);

                Creature* pBoar = m_creature->SummonCreature(
                    NPC_GORETUSK_BOAR,
                    sx, sy, z,
                    m_creature->GetOrientation(),
                    TEMPSPAWN_TIMED_OR_CORPSE_DESPAWN,
                    120000 // despawn after 2 minutes if still alive, auto despawn on death
                );

                if (pBoar)
                    ForceAggro(pBoar, pVictim);
            }
        }

        void UpdateAI(const uint32 /*uiDiff*/) override
        {
            if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
                return;

            Unit* pVictim = m_creature->getVictim();
            float hpPct = m_creature->GetHealthPercent();

            // At 35%: Bellygrub will stun player and heal (should only trigger once per fight)
            if (!m_Did35 && hpPct <= 35.0f)
            {
                m_Did35 = true;
                sLog.outString("Bellygrub: 35%% trigger fired (hp=%.2f)", hpPct);

                m_creature->CastSpell(pVictim, SPELL_STUN_56, true);
                m_creature->HandleEmoteCommand(EMOTE_ONESHOT_EAT);

                m_creature->MonsterTextEmote(EMOTE_HEAL_SELF, nullptr, true);
                m_creature->CastSpell(m_creature, SPELL_HEAL_POTION_441, true);
            }

            // At 10%: Bellygrub will knockdown + call for help
            if (!m_Did10 && hpPct <= 10.0f)
            {
                m_Did10 = true;
                sLog.outString("Bellygrub: 10%% trigger fired (hp=%.2f)", hpPct);

                m_creature->CastSpell(pVictim, SPELL_KNOCKDOWN_5164, true);
                SummonTwoHelpers(pVictim);
            }

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* pCreature) override
    {
        return new npc_bellygrubAI(pCreature);
    }
};

void AddSC_npc_bellygrub()
{
    Script* s;

    s = new npc_bellygrub();
    s->RegisterSelf();
}
