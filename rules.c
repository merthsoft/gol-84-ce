#include "rules.h"

void InitRules() {
	rules = (rule*)malloc(numRules*sizeof(rule));
	// Life: 23/3
	rules[0].live = 0x0C;
	rules[0].born = 0x08;
	rules[0].name = "Life:      23/3";
	// HighLife: 23/36
	rules[1].live = 0x0C;
	rules[1].born = 0x48;
	rules[1].name = "HighLife:  23/36";
	// Replicator: 1357/1357
	rules[2].live = 0xAA;
	rules[2].born = 0xAA;
	rules[2].name = "Replicate: 1357/1357";
	// Maze: 12345/3
	rules[3].live = 0x3E;
	rules[3].born = 0x08;
	rules[3].name = "Maze:      12345/3";
	// 34 Life: 34/34
	rules[4].live = 0x18;
	rules[4].born = 0x18;
	rules[4].name = "34 Life:   34/34";
	// 2x2: 125/36
	rules[5].live = 0x26;
	rules[5].born = 0x09;
	rules[5].name = "2x2:       125/36";
}