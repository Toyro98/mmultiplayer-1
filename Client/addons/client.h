#pragma once
#pragma warning (push)
#pragma warning (disable: 26495)

#include "../addon.h"
#include "../sdk.h"
#include <string>
#include <windows.h>
#include <vector>
#include "../engine.h"

static int CompressedBoneOffsets[] = {
    0x14,  0x20,  0x24,  0x28,  0x2C,  0x30,  0x34,  0x38,  0x40,  0x44,  0x48,
    0x4C,  0x50,  0x54,  0x58,  0x60,  0x64,  0x68,  0x6C,  0x70,  0x74,  0x78,
    0x80,  0x84,  0x88,  0x8C,  0x90,  0x94,  0x98,  0xA0,  0xA4,  0xA8,  0xAC,
    0xC0,  0xC4,  0xC8,  0xCC,  0xD0,  0xD4,  0xD8,  0x1E4, 0x204, 0x208, 0x20C,
    0x210, 0x214, 0x224, 0x228, 0x230, 0x238, 0x244, 0x260, 0x268, 0x26C, 0x280,
    0x284, 0x288, 0x28C, 0x290, 0x2A0, 0x2A8, 0x2AC, 0x2B0, 0x2BC, 0x2D0, 0x2D4,
    0x2D8, 0x2E0, 0x2E4, 0x2F0, 0x2F4, 0x2F8, 0x3E0, 0x3E4, 0x3E8, 0x3EC, 0x3F0,
    0x3F4, 0x3F8, 0x400, 0x404, 0x408, 0x40C, 0x410, 0x414, 0x418, 0x440, 0x444,
    0x448, 0x44C, 0x450, 0x454, 0x458, 0x460, 0x464, 0x468, 0x46C, 0x470, 0x474,
    0x478, 0x4E0, 0x4E4, 0x4E8, 0x4EC, 0x4F0, 0x4F4, 0x4F8, 0x550, 0x554, 0x558,
    0x5A0, 0x5A4, 0x5A8, 0x5AC, 0x5C0, 0x5C4, 0x5C8, 0x5CC, 0x5E0, 0x5E4, 0x5E8,
    0x5EC, 0x600, 0x604, 0x608, 0x60C, 0x644, 0x648, 0x660, 0x664, 0x668, 0x66C,
    0x684, 0x688, 0x68C, 0x6A4, 0x6A8, 0x6AC, 0x6C4, 0x6C8, 0x6E0, 0x6E4, 0x6E8,
    0x6EC, 0x704, 0x708, 0x70C, 0x724, 0x728, 0x72C, 0x744, 0x748, 0x74C, 0x760,
    0x764, 0x768, 0x76C, 0x784, 0x788, 0x78C, 0x7A4, 0x7A8, 0x7AC, 0x7C4, 0x7C8,
    0x7E0, 0x7E4, 0x7E8, 0x7EC, 0x804, 0x808, 0x80C, 0x824, 0x828, 0x82C, 0x840,
    0x844, 0x848, 0x84C, 0x860, 0x864, 0x868, 0x86C, 0x888, 0x88C, 0x8A0, 0x8AC,
    0x8C0, 0x8C4, 0x8C8, 0x8CC, 0x8E0, 0x8E4, 0x8E8, 0x8EC, 0x900, 0x904, 0x908,
    0x90C, 0x920, 0x924, 0x928, 0x92C, 0x940, 0x944, 0x948, 0x94C, 0x960, 0x964,
    0x968, 0x96C, 0x970, 0x974, 0x978, 0x980, 0x984, 0x988, 0x98C, 0x9A0, 0x9A4,
    0x9A8, 0x9AC, 0x9C8, 0x9CC, 0x9E8, 0x9EC, 0xA00, 0xA04, 0xA08, 0xA20, 0xA24,
    0xA28, 0xA2C, 0xA48, 0xA4C, 0xA68, 0xA6C, 0xA80, 0xA84, 0xA88, 0xAA0, 0xAA4,
    0xAA8, 0xAAC, 0xAC8, 0xACC, 0xAE8, 0xAEC, 0xB00, 0xB04, 0xB08, 0xB0C, 0xB20,
    0xB24, 0xB28, 0xB2C, 0xB48, 0xB4C, 0xB68, 0xB6C, 0xB80, 0xB84, 0xB88, 0xB8C,
    0xBA0, 0xBA4, 0xBA8, 0xBAC, 0xBC8, 0xBCC, 0xBE0, 0xBE4, 0xBE8, 0xBEC, 0xBF0,
    0xBF4, 0xBF8, 0xC00, 0xC04, 0xC08, 0xC0C, 0xC20, 0xC24, 0xC28, 0xC2C, 0xC40,
    0xC44, 0xC48, 0xC4C, 0xC60, 0xC64, 0xC68, 0xC6C, 0xC70, 0xC74, 0xC78, 0xC80,
    0xC84, 0xC88, 0xC8C, 0xC90, 0xC94, 0xC98, 0xCA0, 0xCAC, 0xCC0, 0xCC4, 0xCC8,
    0xCCC, 0xCE0, 0xCE4, 0xCE8, 0xCEC, 0xD00, 0xD04, 0xD08, 0xD0C, 0xD14, 0xD20,
    0xD24, 0xD28, 0xD2C, 0xD34, 0xD40, 0xD4C, 0xD60, 0xD64, 0xD68, 0xD6C};

class Client : public Addon 
{
  public:
    static constexpr int Port = 5222;

    bool Initialize();
    std::string GetName();

    typedef struct 
    {
        unsigned int Id;
        Classes::FVector Position;
        unsigned short Yaw;
        Classes::FBoneAtom Bones[PLAYER_PAWN_BONE_COUNT];
    } PACKET;

    typedef struct 
    {
        unsigned int Id;
        Classes::FVector Position;
        unsigned short Yaw;
        short CompressedBones[ARRAYSIZE(CompressedBoneOffsets)];
    } PACKET_COMPRESSED;

    class Player 
    {
      public:
        unsigned int Id;
        Engine::Character Character;
        std::string Name;
        std::string Level;
        Classes::ASkeletalMeshActorSpawnable *Actor;
        float MaxZ;
        PACKET LastPacket;

        std::string GameMode;
        bool CanTag;
        unsigned int TaggedPlayerId;
        unsigned int CoolDownTag = 5;
    };

    static std::vector<Client::Player *> GetPlayerList();
};

#pragma warning (pop)