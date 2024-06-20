#include <stdlib.h>

#include "include/rules.h"

Stamp Life_Glider = {
    3, 3, (uint8_t*[]){
        (uint8_t[]){0, 1, 0}, 
        (uint8_t[]){0, 0, 1},
        (uint8_t[]){1, 1, 1}
    }, "Glider"
};

RulesList list = {
    19, (Rules[]) {
        {0x0c, 0x08,    "Life (B23/S3)", 1, (Stamp*[]){&Life_Glider} },
        {0x0C, 0x48,    "HighLife (B36/S23)", 0, NULL },
        { 0xAA, 0xAA,   "Replicator (B1357/S1357)", 0, NULL },
        { 0x3E, 0x08,   "Maze (B3/S12345)", 0, NULL },
        { 0x18, 0x18,   "34 Life (B34/S34)", 0, NULL },
        { 0x26, 0x09,   "2x2 (B36/S125)", 0, NULL },
        { 0x1FF, 0x08,  "Life without Death (B3/S012345678)", 0, NULL },
        { 0x00, 0x04,   "Seeds (B2/S)", 0, NULL },
        { 0xA8, 0xA8,   "Amoeba (B357/S357)", 0, NULL },
        { 0x1E0, 0x1E8, "Diamoeba (B35678/S5678)", 0, NULL },
        { 0x1D8, 0x1C8, "Day & Night (B3678/S34678)", 0, NULL },
        { 0x34, 0x148,  "Morley (B368/S245)", 0, NULL },
        { 0x1E8, 0x1D0, "Anneal (B4678/S35678)", 0, NULL },
        { 0x02, 0x02,   "Gnarl (B1/S1)", 0, NULL },
        { 0x38, 0xF0,   "Assimilation (B4567/S345)", 0, NULL },
        { 0x00, 0x1C,   "Serviettes (B234/S)", 0, NULL },
        { 0x188, 0x1EC, "Coagulations (B235678/S378)", 0, NULL },
        { 0x1F0, 0x08,  "Coral (B3/S45678)", 0, NULL },
        { 0x3C, 0x1F0,  "Walled Cities (B45678/S2345)", 0, NULL },
    }
};

RulesList* GetRules() {
    return &list;
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