#ifndef KEY_H
#define KEY_H

#include <keypadc.h>

#define Key_Up      kb_KeyUp
#define Key_Down    kb_KeyDown
#define Key_Left    kb_KeyLeft
#define Key_Right   kb_KeyRight
#define Key_2nd     kb_Key2nd
#define Key_Enter   kb_KeyEnter
#define Key_Del     kb_KeyDel
#define Key_Clear   kb_KeyClear
#define Key_Power   kb_KeyPower
#define Key_Vars    kb_KeyVars
#define Key_Mode    kb_KeyMode
#define Key_Add     kb_KeyAdd
#define Key_Alpha   kb_KeyAlpha

void Key_ScanKeys(uint32_t keyDelay);
void Key_Init();
bool Key_IsDown(kb_lkey_t key);
bool Key_WasDown(kb_lkey_t key);
bool Key_JustPressed(kb_lkey_t key);
void Key_Reset();

#endif