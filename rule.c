
#include <stdlib.h>

#include "rule.h"

uint8_t InitRules(Rule** rulesList) {
    uint8_t numRules = 19;
    *rulesList = malloc(numRules * sizeof(Rule));

    (*rulesList)[0].Name = "Life (B3/S3)";
    (*rulesList)[0].Live = 0x0C;
    (*rulesList)[0].Born = 0x08;

    (*rulesList)[1].Name = "HighLife (B36/S23)";
    (*rulesList)[1].Live = 0x0C;
    (*rulesList)[1].Born = 0x48;

    (*rulesList)[2].Name = "Replicator (B1357/S1357)";
    (*rulesList)[2].Live = 0xAA;
    (*rulesList)[2].Born = 0xAA;

    (*rulesList)[3].Name = "Maze (B3/S12345)";
    (*rulesList)[3].Live = 0x3E;
    (*rulesList)[3].Born = 0x08;

    (*rulesList)[4].Name = "34 Life (B34/S34)";
    (*rulesList)[4].Live = 0x18;
    (*rulesList)[4].Born = 0x18;

    (*rulesList)[5].Name = "2x2 (B36/S125)";
    (*rulesList)[5].Live = 0x26;
    (*rulesList)[5].Born = 0x09;

    (*rulesList)[6].Name = "Life without Death";
    (*rulesList)[6].Live = 0xFF;
    (*rulesList)[6].Born = 0x08;

    (*rulesList)[7].Name = "Seeds (B2/S)";
    (*rulesList)[7].Live = 0x00;
    (*rulesList)[7].Born = 0x04;

    (*rulesList)[8].Name = "Amoeba (B357/S357)";
    (*rulesList)[8].Live = 0xA8;
    (*rulesList)[8].Born = 0xA8;

    (*rulesList)[9].Name = "Diamoeba (B35678/S5678)";
    (*rulesList)[9].Live = 0x1E0;
    (*rulesList)[9].Born = 0x1E8;

    (*rulesList)[10].Name = "Day & Night (B3678/S34678)";
    (*rulesList)[10].Live = 0x1D8;
    (*rulesList)[10].Born = 0x1C8;

    (*rulesList)[11].Name = "Morley (B368/S245)";
    (*rulesList)[11].Live = 0x34;
    (*rulesList)[11].Born = 0x148;

    (*rulesList)[12].Name = "Anneal (B4678/S35678)";
    (*rulesList)[12].Live = 0x1E8;
    (*rulesList)[12].Born = 0x1D0;

    (*rulesList)[13].Name = "Gnarl (B1/S1)";
    (*rulesList)[13].Live = 0x02;
    (*rulesList)[13].Born = 0x02;

    (*rulesList)[14].Name = "Assimilation (B4567/S345)";
    (*rulesList)[14].Live = 0x38;
    (*rulesList)[14].Born = 0xF0;

    (*rulesList)[15].Name = "Serviettes (B234/S)";
    (*rulesList)[15].Live = 0x00;
    (*rulesList)[15].Born = 0x1C;

    (*rulesList)[16].Name = "Coagulations (B235678/S378)";
    (*rulesList)[16].Live = 0x188;
    (*rulesList)[16].Born = 0x1EC;

    (*rulesList)[17].Name = "Coral (B3/S45678)";
    (*rulesList)[17].Live = 0x1F0;
    (*rulesList)[17].Born = 0x08;

    (*rulesList)[18].Name = "Walled Cities (B45678/S2345)";
    (*rulesList)[18].Live = 0x3C;
    (*rulesList)[18].Born = 0x1F0;

    return numRules;
}