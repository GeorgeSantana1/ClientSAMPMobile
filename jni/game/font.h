#pragma once

void AsciiToGxtChar(const char* ascii, uint16_t* gxt);
void Font_PrintString(float X, float Y, char *sz);
void Font_SetColor(uint32_t dwColor);
void Font_SetDropColor(uint32_t dwColor);
void Font_SetOutline(int pos);
void Font_Unk12(int unk);
void Font_SetScale(float X, float Y);
void Font_SetJustify(int just);
void Font_SetFontStyle(int style);
void Font_SetProportional(int prop);
void Font_SetRightJustifyWrap(float wrap);
void Font_UseBox(int use, int unk);
void Font_UseBoxColor(uint32_t color);
void Font_SetLineWidth(float width);
void Font_SetLineHeight(float height);
void Font_SetShadow(int shadow);
void Font_UnkConv(char *sz, int param1, int param2, int param3, int param4, int param5, int param6, char * buf);
void Font_UnkConv2(char *sz);