#include <LCD.h>

LCD *LCD::INSTANCE = NULL;

LCD::LCD()
{
    this->local_sysconfig = new SysConfig(0, 0, 0);
    this->pr = new PressureSensor(PinConfiguration::PR_Out, PinConfiguration::PR_Sck);
}
LCD *LCD::getInstance()
{
    if (INSTANCE == NULL)
    {
        INSTANCE = new LCD();
    }
    return INSTANCE;
}

void LCD::update_sysconfig(SysConfig *newconfig)
{
    this->local_sysconfig->update(newconfig);
}

void LCD::LCD_Cover()
{

    GLCD.Init();
    GLCD.DrawRoundRect(0, 0, GLCD.Width, GLCD.Height, 4);
    Serial.println(1);
    GLCD.SelectFont(cp437font8x8);
    GLCD.DrawString(F("Save Breathe"), gTextfmt_center, 25);
    GLCD.DrawString(F("Save Life"), gTextfmt_center, 35);
}
void LCD::LCD_Clear()
{
    GLCD.ClearScreen();
}

void LCD::LCD_Logo()
{
    GLCD.Init();
    /*GLCD.DrawBitmap(comp14, 0, 0);
    delay(7 * LogoDelay);
    GLCD.DrawBitmap(comp15, 0, 0);
    delay(LogoDelay);
    GLCD.DrawBitmap(comp16, 0, 0);
    delay(LogoDelay);
    GLCD.DrawBitmap(comp17, 0, 0);
    delay(LogoDelay);
    GLCD.DrawBitmap(comp18, 0, 0);
    delay(LogoDelay);
    GLCD.DrawBitmap(comp19, 0, 0);
    delay(LogoDelay);
    GLCD.DrawBitmap(comp20, 0, 0);
    delay(LogoDelay);
    GLCD.DrawBitmap(comp21, 0, 0);
    delay(LogoDelay);
    GLCD.DrawBitmap(comp22, 0, 0);
    delay(LogoDelay);
    GLCD.DrawBitmap(comp23, 0, 0);
    delay(LogoDelay);
    GLCD.DrawBitmap(comp24, 0, 0);
    delay(LogoDelay);
    GLCD.DrawBitmap(comp25, 0, 0);
    delay(LogoDelay);
    GLCD.DrawBitmap(comp26, 0, 0);
    delay(LogoDelay);
    GLCD.DrawBitmap(comp27, 0, 0);
    delay(LogoDelay);
    GLCD.DrawBitmap(comp28, 0, 0);
    delay(LogoDelay);
    GLCD.DrawBitmap(comp29, 0, 0);
    delay(LogoDelay);
    GLCD.DrawBitmap(comp30, 0, 0);
    delay(LogoDelay);
    GLCD.DrawBitmap(comp31, 0, 0);
    delay(LogoDelay);
    GLCD.DrawBitmap(comp32, 0, 0);
    delay(LogoDelay);
    GLCD.DrawBitmap(comp33, 0, 0);
    delay(LogoDelay);
    GLCD.DrawBitmap(comp34, 0, 0);
    delay(LogoDelay);
    GLCD.DrawBitmap(comp35, 0, 0);
    delay(LogoDelay);
    GLCD.DrawBitmap(comp36, 0, 0);
    delay(LogoDelay);
    GLCD.DrawBitmap(comp37, 0, 0);
    delay(LogoDelay);
    GLCD.DrawBitmap(comp38, 0, 0);
    delay(LogoDelay);
    GLCD.DrawBitmap(comp39, 0, 0);
    delay(LogoDelay);
    GLCD.DrawBitmap(comp40, 0, 0);
    delay(5 * LogoDelay);*/
    GLCD.ClearScreen();
    GLCD.DrawRoundRect(0, 0, GLCD.Width, GLCD.Height, 2);
    GLCD.SelectFont(Iain5x7);

    for (size_t i = 0; i < 64; i += 16)
    {
        GLCD.DrawLine(0, 16 + i, GLCD.Width, 16 + i);
        GLCD.DrawLine(0, 17 + i, GLCD.Width, 17 + i);
    }

    GLCD.DrawString(F("Tidal Volume : "), 3, 3);
    GLCD.DrawString(F("Resp Rate : "), 3, 20);
    GLCD.DrawString(F("In/Ex Ratio: "), 3, 37);
    GLCD.DrawString(F("1 :"), 80, 37);
    GLCD.DrawString(F("PR (CmH2O) : "), 3, 54);
}

void LCD::LCD_Menu(int tidalVolume, int respRate, int IEratio, float PR)
{

    String *tidal_Vol = &(this->lastVol);
    String *resp_Rate = &(this->lastRate);
    String *IE_ratio = &(this->lastIE);
    String *PR_Sensor = &(this->lastPR);

    String a = String(tidalVolume);
    String b = String(respRate);
    String c = String(IEratio);
    String d = String(PR);

    switch (printVarCounter)
    {
    case 0:
        printVariable(80, 3, &wipeVar[printVarCounter], a, tidal_Vol);
        printVarCounter++;
        break;
    case 1:
        printVariable(80, 20, &wipeVar[printVarCounter], b, resp_Rate);
        printVarCounter++;
        break;
    case 2:
        printVariable(89, 37, &wipeVar[printVarCounter], c, IE_ratio);
        printVarCounter++;
        break;
    case 3:
        printVariable(80, 54, &wipeVar[printVarCounter], d, PR_Sensor);
        printVarCounter = 0;
        break;
    default:
        printVarCounter = 0;
        break;
    }
}

void LCD::wipeVariable(int hpos, int vpos, int spaceQuantity)
{
    GLCD.CursorToXY(hpos, vpos);
    for (int i = 0; i < spaceQuantity; i++)
    {
        GLCD.print(" ");
    }
    GLCD.CursorToXY(hpos, vpos);
}

String LCD::printVariable(int hpos, int vpos, bool *wipeWar, String toPrint, String *lastToPrint)
{
    if (toPrint != *lastToPrint)
    {
        if (*wipeVar)
        {
            GLCD.CursorToXY(hpos, vpos);
            GLCD.print("     ");
        }
        else
        {
            GLCD.CursorToXY(hpos, vpos);
            GLCD.Puts(toPrint);
            *lastToPrint = toPrint;
        }
        *wipeVar = not(*wipeVar);
    }
    return toPrint;
}

void LCD::LCD_graph()
{

    GLCD.DrawRoundRect(0, 0, GLCD.Width, GLCD.Height, 4);
    GLCD.DrawRect(85, 0, 42, 15);
    GLCD.GotoXY(90, 4);
    GLCD.print(12);

    // //Add Scaling...Bottom most line (y=63 is 0V, y=1 is 5V)
    GLCD.DrawLine(0, 0, 0, 63); //Line for Scaleing Tally Marks
    GLCD.DrawLine(0, 0, 4, 0);  //Topmost Mark
    GLCD.DrawLine(0, 8, 2, 8);
    GLCD.DrawLine(0, 16, 4, 16);
    GLCD.DrawLine(0, 24, 2, 24);
    GLCD.DrawLine(0, 32, 4, 32); //Center Mark
    GLCD.DrawLine(0, 40, 2, 40);
    GLCD.DrawLine(0, 48, 4, 48);
    GLCD.DrawLine(0, 56, 2, 56);
    GLCD.DrawLine(0, 63, 4, 63); //BottomMost Mark

    int y = 63 - (8 * this->pr->Read_Pressure());
    if (w == 0)
    {
        w = y;
    }
    GLCD.DrawLine(q, w, x, y);
    x++;

    if (x == 128)
    {
        GLCD.ClearScreen();
        x = 0;
    }
    q = x;
    w = y;
}