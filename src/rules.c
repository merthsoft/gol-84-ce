#include <stdlib.h>

#include "include/rules.h"

void InitStamps(Rules* rules, uint8_t numStamps) {
    rules->NumStamps = numStamps;
    rules->Stamps = malloc(numStamps * sizeof(Stamp));
}

void life(Rules* rules) {
    rules->Name = "Life (B23/S3)";
    rules->Live = 0x0C;
    rules->Born = 0x08;

    InitStamps(rules, 1);
    InitStamp(rules->Stamps, 3, 3);
    rules->Stamps[0].Name = "Glider";

    rules->Stamps[0].Cells[1][0] = 1;
    rules->Stamps[0].Cells[2][1] = 1;
    rules->Stamps[0].Cells[0][2] = 1;
    rules->Stamps[0].Cells[1][2] = 1;
    rules->Stamps[0].Cells[2][2] = 1;
}

RulesList*  InitRules() {
    RulesList* rulesList = malloc(sizeof(RulesList));
    rulesList->NumRules = 19;
    rulesList->List = malloc(rulesList->NumRules * sizeof(Rules));


    life(rulesList->List);

    rulesList->List[1].Name = "HighLife (B36/S23)";
    rulesList->List[1].Live = 0x0C;
    rulesList->List[1].Born = 0x48;

    rulesList->List[2].Name = "Replicator (B1357/S1357)";
    rulesList->List[2].Live = 0xAA;
    rulesList->List[2].Born = 0xAA;

    rulesList->List[3].Name = "Maze (B3/S12345)";
    rulesList->List[3].Live = 0x3E;
    rulesList->List[3].Born = 0x08;

    rulesList->List[4].Name = "34 Life (B34/S34)";
    rulesList->List[4].Live = 0x18;
    rulesList->List[4].Born = 0x18;

    rulesList->List[5].Name = "2x2 (B36/S125)";
    rulesList->List[5].Live = 0x26;
    rulesList->List[5].Born = 0x09;

    rulesList->List[6].Name = "Life without Death (B3/S012345678)";
    rulesList->List[6].Live = 0x1FF;
    rulesList->List[6].Born = 0x08;

    rulesList->List[7].Name = "Seeds (B2/S)";
    rulesList->List[7].Live = 0x00;
    rulesList->List[7].Born = 0x04;

    rulesList->List[8].Name = "Amoeba (B357/S357)";
    rulesList->List[8].Live = 0xA8;
    rulesList->List[8].Born = 0xA8;

    rulesList->List[9].Name = "Diamoeba (B35678/S5678)";
    rulesList->List[9].Live = 0x1E0;
    rulesList->List[9].Born = 0x1E8;

    rulesList->List[10].Name = "Day & Night (B3678/S34678)";
    rulesList->List[10].Live = 0x1D8;
    rulesList->List[10].Born = 0x1C8;

    rulesList->List[11].Name = "Morley (B368/S245)";
    rulesList->List[11].Live = 0x34;
    rulesList->List[11].Born = 0x148;

    rulesList->List[12].Name = "Anneal (B4678/S35678)";
    rulesList->List[12].Live = 0x1E8;
    rulesList->List[12].Born = 0x1D0;

    rulesList->List[13].Name = "Gnarl (B1/S1)";
    rulesList->List[13].Live = 0x02;
    rulesList->List[13].Born = 0x02;

    rulesList->List[14].Name = "Assimilation (B4567/S345)";
    rulesList->List[14].Live = 0x38;
    rulesList->List[14].Born = 0xF0;

    rulesList->List[15].Name = "Serviettes (B234/S)";
    rulesList->List[15].Live = 0x00;
    rulesList->List[15].Born = 0x1C;

    rulesList->List[16].Name = "Coagulations (B235678/S378)";
    rulesList->List[16].Live = 0x188;
    rulesList->List[16].Born = 0x1EC;

    rulesList->List[17].Name = "Coral (B3/S45678)";
    rulesList->List[17].Live = 0x1F0;
    rulesList->List[17].Born = 0x08;

    rulesList->List[18].Name = "Walled Cities (B45678/S2345)";
    rulesList->List[18].Live = 0x3C;
    rulesList->List[18].Born = 0x1F0;

    return rulesList;
}

void NumToRuleString(uint16_t num, char* rulesString, uint8_t* index) {
    uint8_t j;
    for (j = 0; j < 9; j++) {
        if ((num & (1 << j)) != 0) {
            rulesString[*index] = 48 + j;
            (*index) = (*index) + 1;
        }
    }
}