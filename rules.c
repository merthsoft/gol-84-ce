#include "rules.h"

void InitRules() {
	rules = (Rule*)malloc(numRules*sizeof(Rule));
	// Life: 23/3
	rules[0].Live = 0x0C;
	rules[0].Born = 0x08;
	rules[0].Name = "Life:      23/3";
	// HighLife: 23/36
	rules[1].Live = 0x0C;
	rules[1].Born = 0x48;
	rules[1].Name = "HighLife:  23/36";
	// Replicator: 1357/1357
	rules[2].Live = 0xAA;
	rules[2].Born = 0xAA;
	rules[2].Name = "Replicate: 1357/1357";
	// Maze: 12345/3
	rules[3].Live = 0x3E;
	rules[3].Born = 0x08;
	rules[3].Name = "Maze:      12345/3";
	// 34 Life: 34/34
	rules[4].Live = 0x18;
	rules[4].Born = 0x18;
	rules[4].Name = "34 Life:   34/34";
	// 2x2: 125/36
	rules[5].Live = 0x26;
	rules[5].Born = 0x09;
	rules[5].Name = "2x2:       125/36";
}