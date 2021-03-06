// Copyright (C) 2004 WoW Daemon
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

#ifndef WOWSERVER_GAMEOBJECT_H
#define WOWSERVER_GAMEOBJECT_H

class Player;
class GameObjectAIScript;
class GameObjectTemplate;

struct GOQuestItem
{
    uint32 itemid;
    uint32 requiredcount;
};

struct GOQuestGameObject
{
    uint32 goid;
    uint32 requiredcount;
};

enum GAMEOBJECT_FLAG_BIT
{
    GAMEOBJECT_UNCLICKABLE = 0x01,
    GAMEOBJECT_CLICKABLE = 0x20,
};

typedef HM_NAMESPACE::hash_map<Quest*, std::map<uint32, uint32> > GameObjectItemMap;
typedef HM_NAMESPACE::hash_map<Quest*, uint32 > GameObjectGOMap;

struct GameObjectInfo
{
    uint32 ID;
    float Size;
    uint32 Type;
    uint32 DisplayID;
    std::string Name;
    uint32 SpellFocus;
    uint32 sound1;
    uint32 sound2;
    uint32 sound3;
    uint32 sound4;
    uint32 sound5;
    uint32 sound6;
    uint32 sound7;
    uint32 sound8;
    uint32 sound9;
    uint32 Unknown1;
    uint32 Unknown2;
    uint32 Unknown3;
    uint32 Unknown4;
    uint32 Unknown5;
    uint32 Unknown6;
    uint32 Unknown7;
    uint32 Unknown8;
    uint32 Unknown9;
    uint32 Unknown10;
    uint32 Unknown11;
    uint32 Unknown12;
    uint32 Unknown13;
    uint32 Unknown14;
    // Quests
    GameObjectGOMap goMap;
    GameObjectItemMap itemMap;
};

enum GAMEOBJECT_TYPES
{
    GAMEOBJECT_TYPE_DOOR               = 0,
    GAMEOBJECT_TYPE_BUTTON             = 1,
    GAMEOBJECT_TYPE_QUESTGIVER         = 2,
    GAMEOBJECT_TYPE_CHEST              = 3,
    GAMEOBJECT_TYPE_BINDER             = 4,
    GAMEOBJECT_TYPE_GENERIC            = 5,
    GAMEOBJECT_TYPE_TRAP               = 6,
    GAMEOBJECT_TYPE_CHAIR              = 7,
    GAMEOBJECT_TYPE_SPELL_FOCUS        = 8,
    GAMEOBJECT_TYPE_TEXT               = 9,
    GAMEOBJECT_TYPE_GOOBER             = 10,
    GAMEOBJECT_TYPE_TRANSPORT          = 11,
    GAMEOBJECT_TYPE_AREADAMAGE         = 12,
    GAMEOBJECT_TYPE_CAMERA             = 13,
    GAMEOBJECT_TYPE_MAP_OBJECT         = 14,
    GAMEOBJECT_TYPE_MO_TRANSPORT       = 15,
    GAMEOBJECT_TYPE_DUEL_ARBITER       = 16,
    GAMEOBJECT_TYPE_FISHINGNODE        = 17,
    GAMEOBJECT_TYPE_RITUAL             = 18,
    GAMEOBJECT_TYPE_MAILBOX            = 19,
    GAMEOBJECT_TYPE_AUCTIONHOUSE       = 20,
    GAMEOBJECT_TYPE_GUARDPOST          = 21,
    GAMEOBJECT_TYPE_SPELLCASTER        = 22,
    GAMEOBJECT_TYPE_MEETINGSTONE       = 23,
    GAMEOBJECT_TYPE_FLAGSTAND          = 24,
    GAMEOBJECT_TYPE_FISHINGHOLE        = 25,
    GAMEOBJECT_TYPE_FLAGDROP           = 26,
};

#define CALL_GO_SCRIPT_EVENT(obj, func) if(obj->GetTypeId() == TYPEID_GAMEOBJECT && static_cast<GameObject*>(obj)->GetScript() != NULL) static_cast<GameObject*>(obj)->GetScript()->func

class WOWD_SERVER_DECL GameObject : public Object
{
public:
    GameObject(uint32 high, uint32 low);
    ~GameObject( );

    GameObjectInfo* GetInfo() { return pInfo; }

    //void Create ( uint32 display_id, uint8 state, uint32 obj_field_entry, float scale, uint16 type, uint16 faction, uint32 mapid, float x, float y, float z, float ang );
    void Create ( uint32 mapid, float x, float y, float z, float ang);
    void CreateFromProto(uint32 entry,uint32 mapid, float x, float y, float z, float ang);
    void Create( uint32 guidlow, uint32 guidhigh,uint32 displayid, uint8 state, uint32 entryid, float scale,uint32 typeId, uint32 type,uint32 flags, uint32 mapid, float x, float y, float z, float ang );

    virtual void Update(uint32 p_time);

    void Spawn();
    void Despawn(uint32 time);
    Loot loot;
    //void _EnvironmentalDamageUpdate();
    void UpdateTrapState();
    // Serialization
    void SaveToDB();
    void SaveToFile(std::stringstream & name);
    bool LoadFromDB(uint32 guid);
    void LoadFromDB(GameObjectTemplate *t);
    void DeleteFromDB();
    void EventCloseDoor();

    //Fishing stuff
    void UseFishingNode(Player *player);
    void EndFishing(Player* player,bool abort);
    void FishHooked(Player * player);
    
    // Quests
    void _LoadQuests();
    bool HasQuests() { return m_quests != NULL; };
    void AddQuest(QuestRelation *Q);
    void DeleteQuest(QuestRelation *Q);
    Quest *FindQuest(uint32 quest_id, uint8 quest_relation);
    uint16 GetQuestRelation(uint32 quest_id);
    uint32 NumOfQuests();
    std::list<QuestRelation *>::iterator QuestsBegin() { return m_quests->begin(); };
    std::list<QuestRelation *>::iterator QuestsEnd() { return m_quests->end(); };
    void SetQuestList(std::list<QuestRelation *>* qst_lst) { m_quests = qst_lst; };

    void SetSummoned(Unit *mob)
    {
        m_summoner = mob;
        m_summonedGo = true;
    }
    void Expire();
    void Deactivate();

    inline bool isQuestGiver()
    {
        if(GetUInt32Value(GAMEOBJECT_TYPE_ID) == 2)
            return true;
        else
            return false;
    };

    /// Quest data
    std::list<QuestRelation *>* m_quests;
   
    Player **m_ritualmembers;
    Player *m_ritualcaster,*m_ritualtarget;
    uint32 m_ritualspell;

    void InitAI();
    SpellEntry* spell;
    
    float range;
    uint32 checkrate;
    uint32 counter;
    uint32 charges;//used for type==22,to limit number of usages.
    bool invisible;//invisible
    INVISIBILTY_FLAG invisibilityFlag;
    Unit* m_summoner;
    GameObject *pcbannerAura;
    int32 bannerslot;

    void CallScriptUpdate();
    void AddToWorld();

    inline GameObjectAIScript* GetScript() { return myScript; }

    void TrapSearchTarget();    // Traps need to find targets faster :P

protected:

    uint32 m_sqlid;
    bool m_summonedGo;
    GameObjectInfo *pInfo;
    GameObjectAIScript * myScript;
};

#endif

