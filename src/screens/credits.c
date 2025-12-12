#include <raylib.h>
#include "systems.h"

void InitCreditsScreen(struct Systems* systems, CreditsData* data)
{
    data->creditsFont = GetFont(&(systems->resourceManager), FONT_ID_CAPTURE_IT);
}

void UpdateCreditsScreen(struct Systems* systems, CreditsData* data)
{
    if (IsKeyPressed(KEY_ENTER))
    {
         RequestScreenChange(systems, SCREEN_MAIN_MENU);
    }
}

void DrawCreditsScreen(struct Systems* systems, CreditsData* data)
{
    bool lang = systems->configManager.language;
    const char* goMenuText = lang?"Pressione ENTER para voltar ao Menu":"Press ENTER to go back to the MENU";
    
    const char* text_dev = TextFormat("%s:\nRafael Barbosa (@rafaelvlt)\nRodrigo Silveira (@RodrigoSilveiraCin)\nCaio Amarante (@IrineuACgasoso)\nEric Santiago (@ARISE21)\nHeberty Souza (@HebSP)", lang?"Desenvolvedores":"Developers");
    const char* text_songs = TextFormat("%s:\nKing of Fighters 96 - Protector Demo 1\nMechWarrior 2 - Snake City\nMechWarrior 2 - Plum Wine\nMechWarrior 2 - Pyre Light", lang?"Músicas":"Songs");
    const char* text_font = TextFormat("%s:\nCapture it - Koczman Bàlint\n(Magique Fonts/Typesgal)", lang?"Fonte de texto":"Text font");
    const char* text_extras = TextFormat("%s:\nMechwarrior2: 31st Century Combat\nMechwarrior2: Mercenaries", lang?"Modelos 3d e efeitos sonoros":"3d models and sound effects");

    Vector2 vet_credits;
        vet_credits.x = GetScreenWidth()/2 - MeasureText("Credits", 50)/2; vet_credits.y = GetScreenHeight()/32 + 20;
    Vector2 vet_textdev;
        vet_textdev.x = GetScreenWidth()/8 - MeasureText(text_dev, 25)/8; vet_textdev.y = GetScreenHeight()/4;
    Vector2 vet_textfont; 
        vet_textfont.x = GetScreenWidth()/8 - MeasureText(text_dev, 25)/8; vet_textfont.y = GetScreenHeight()/2 + 25;
    Vector2 vet_textsongs; 
        vet_textsongs.x = GetScreenWidth()/2 + MeasureText(text_songs, 25)/6; vet_textsongs.y = GetScreenHeight()/4;
    Vector2 vet_textextras;
        vet_textextras.x = GetScreenWidth()/2 + MeasureText(text_extras, 25)/6; vet_textextras.y = GetScreenHeight()/2 + 25;
    Vector2 vet_back;
        vet_back.x = GetScreenWidth()/2 - MeasureText(goMenuText, 18)/2; vet_back.y = GetScreenHeight()-120;

    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), BLACK);
    DrawTextEx(*data->creditsFont, lang?"Creditos":"Credits", vet_credits, 60.0f, 1.0f, WHITE);
    DrawTextEx(*data->creditsFont, text_dev, vet_textdev, 25.0f, 1.0f, WHITE);
    DrawTextEx(*data->creditsFont, text_font, vet_textfont, 25.0f, 1.0f, WHITE);
    DrawTextEx(*data->creditsFont, text_songs, vet_textsongs, 25.0f, 1.0f, WHITE);
    DrawTextEx(*data->creditsFont, text_extras, vet_textextras, 25.0f, 1.0f, WHITE);
    DrawTextEx(*data->creditsFont, goMenuText, vet_back, 18.0f, 1.0f, GRAY);
}
void DestroyCreditsScreen(struct Systems* systems, CreditsData* data)
{

}
