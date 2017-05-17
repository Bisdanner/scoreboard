#include "TimerOne.h"
#include "LiquidCrystal.h"
#include <Wire.h>


#include "LiquidCrystal_I2C.h"
#include "LiquidCrystal_SR.h"
#include "LiquidCrystal_SR2W.h"
#include "LiquidCrystal_SR3W.h"

#include <avr/pgmspace.h>

#include "FastIO.h"


/** NOTE:
 * Tested on Arduino Uno whose I2C pins are A4==SDA, A5==SCL
 */



#include <Bounce2.h>


#define I2C_ADDR 0x3F // <<----- Add your address here. Find it from I2C Scanner

#define BACKLIGHT_PIN 3
#define En_pin 2
#define Rw_pin 1
#define Rs_pin 0
#define D4_pin 4
#define D5_pin 5
#define D6_pin 6
#define D7_pin 7


#define MINUTES_VAL 7
#define SECONDS_VAL 0
#define MINUTES_PAUSE_VAL 2
#define SECONDS_PAUSE_VAL 0
#define SECONDS_BEFORE_BUZZ_IN_HALFTIME 15
#define SPACE_GOAL_TIME 3
#define SPACE_TEAMS 1
#define LCDROWSIZE 20
#define SCOREBOARDROWSIZE 14
#define LONG_PRESS_TRIGGER_DURATION_MS 1000

#define BUZZER_VAL_FIN 45
#define BUZZER_VAL_HT  20

LiquidCrystal_I2C lcd(I2C_ADDR, En_pin, Rw_pin, Rs_pin, D4_pin, D5_pin, D6_pin, D7_pin);
int minutes = MINUTES_VAL;
int seconds = SECONDS_VAL;
int initialMinutes = MINUTES_VAL;
int initialSeconds = SECONDS_VAL;
int goals_team1 = 0;
int goals_team2 = 0;
int timeout = 0;
String lcdbuffer = "";
//String teamsBuffer = "";
char clubs_1[10];
char clubs_2[10];
String goalsAndTime = "";
String scoreBoardBuffer = "";
int scoreBoardChecksum = 0;
const int STX = 2;
const int ETX = 3;

int k = 0;

int counter_for_buzzing = 0;
int counter_for_waiting = 0;

// Instantiate the Bounce objects
Bounce buttonLeftUp = Bounce();
Bounce buttonLeftDown = Bounce();
Bounce buttonRightUp = Bounce();
Bounce buttonRightDown = Bounce();
Bounce buttonStartStop = Bounce();
Bounce buttonMenu = Bounce();


#define BUTTON_LEFT_UP_PIN 2
#define BUTTON_LEFT_DOWN_PIN 3
#define BUTTON_RIGHT_UP_PIN 6
#define BUTTON_RIGHT_DOWN_PIN 8
#define BUTTON_START_STOP_PIN 9
#define BUTTON_MENU_PIN 11
#define LED_12V_PIN 4

// variables will change:
enum ActionState {
  E_POWERON,
  E_SETUP,
  E_RUNNING,
  E_FINISHED,
  E_HALFTIME
};
ActionState actionState;

enum SetupState {
  E_SETUP_GAMETYPE,
  E_SETUP_CLUB,
  E_SETUP_TEAM,
  E_SETUP_TIMER,
  E_SETUP_READY,
  E_SETUP_HALFTIME
};
SetupState setupState;

enum TimerState {
  E_TIMER_STOPPED,
  E_TIMER_RUNNING,
  E_TIMER_FINISHED,
  E_TIMER_BUZZ_EVENT
};
TimerState timerState;


boolean ligaGame = true;
int stoppedTimer = 1;         // running
int runningMenuTime = 0;
boolean firstHalftimeOver = false;
int LED_12V_pinstate = 0;

boolean firstPlusPressedShort = false;
boolean firstPlusPressedLong = false;
boolean firstPlusPressed = false;
boolean firstMinusPressedShort = false;
boolean firstMinusPressedLong = false;
boolean firstMinusPressed = false;
boolean secondPlusPressedShort = false;
boolean secondPlusPressedLong = false;
boolean secondPlusPressed = false;
boolean secondMinusPressedShort = false;
boolean secondMinusPressedLong = false;
boolean secondMinusPressed = false;
boolean startStopPressed = false;
boolean menuPressedShort = false;
boolean menuPressedLong = false;
boolean menuPressed = false;

unsigned long lastPressedTimeFirstPlus = 0;
unsigned long lastPressedTimeFirstMinus = 0;
unsigned long lastPressedTimeSecondPlus = 0;
unsigned long lastPressedTimeSecondMinus = 0;
unsigned long lastPressedTimeMenu = 0;

int club1Counter = 0;
int club2Counter = 0;
int team1Counter = 0;
int team2Counter = 0;

boolean buzzerOn = false;
boolean alreadyBuzzered = false;
int buzzerVar = BUZZER_VAL_HT;

byte ueChar[8] = {
  0b01010,
  0b00000,
  0b10001,
  0b10001,
  0b10001,
  0b10011,
  0b01101,
  0b00000
};

byte oeChar[8] = {
  0b01010,
  0b00000,
  0b01110,
  0b10001,
  0b10001,
  0b10001,
  0b01110,
  0b00000
};

byte aeChar[8] = {
  0b01010,
  0b00000,
  0b01110,
  0b00001,
  0b01111,
  0b10001,
  0b01111,
  0b00000
};

int arrayLength = 0;
#define ARRAYSIZE_FRIENDS 65 //35 oder 63
#define ARRAYSIZE_LIGA 37  //35 oder 63
#define MAXTEAMNUMBER 9
#define INVISIBLETEAMNUMBER 0
#define MINTEAMNUMBER 0
#define TEAM1ROW 0
#define TEAM1COL 0
#define TEAM2ROW 1
#define TEAM2COL 0

//Liga list
const char stringl_0[] PROGMEM = "Fraureuth";
const char stringl_1[] PROGMEM = "Baerenw.";
const char stringl_2[] PROGMEM = "Chemnitz";
const char stringl_3[] PROGMEM = "Chursdorf";
const char stringl_4[] PROGMEM = "Ebersdorf";
const char stringl_5[] PROGMEM = "Eula";
const char stringl_6[] PROGMEM = "Freiberg";
const char stringl_7[] PROGMEM = "Freital";
const char stringl_8[] PROGMEM = "G.Olbersd";
const char stringl_9[] PROGMEM = "Gast";
const char stringl_10[] PROGMEM = "Gesau-O.";
const char stringl_11[] PROGMEM = "Grossenh.";
const char stringl_12[] PROGMEM = "Hainichen";
const char stringl_13[] PROGMEM = "Hartha";
const char stringl_14[] PROGMEM = "Heim";
const char stringl_15[] PROGMEM = "Hohburg";
const char stringl_16[] PROGMEM = "Jaenkend.";
const char stringl_17[] PROGMEM = "Klaffenb.";
const char stringl_18[] PROGMEM = "Lauta";
const char stringl_19[] PROGMEM = "Leipzig";
const char stringl_20[] PROGMEM = "Leutersd.";
const char stringl_21[] PROGMEM = "Lichtent.";
const char stringl_22[] PROGMEM = "Liemehna";
const char stringl_23[] PROGMEM = "Lindenhal";
const char stringl_24[] PROGMEM = "Lippersd.";
const char stringl_25[] PROGMEM = "Loebau";
const char stringl_26[] PROGMEM = "Lueckersd";
const char stringl_27[] PROGMEM = "Mutzschen";
const char stringl_28[] PROGMEM = "NLauterst";
const char stringl_29[] PROGMEM = "NSchiedeb";
const char stringl_30[] PROGMEM = "NSeifersd";
const char stringl_31[] PROGMEM = "Naunhof";
const char stringl_32[] PROGMEM = "Neustadt";
const char stringl_33[] PROGMEM = "OCunnersd";
const char stringl_34[] PROGMEM = "Roetha";
const char stringl_35[] PROGMEM = "Sebnitz";
const char stringl_36[] PROGMEM = "Wiednitz";


// Friends list
const char stringf_0[] PROGMEM = "Fraureuth";
const char stringf_1[] PROGMEM = "Baerenw.";
const char stringf_2[] PROGMEM = "Bruenn";
const char stringf_3[] PROGMEM = "Chemnitz";
const char stringf_4[] PROGMEM = "Chursdorf";
const char stringf_5[] PROGMEM = "Demmin";
const char stringf_6[] PROGMEM = "Dornbirn";
const char stringf_7[] PROGMEM = "Ebersdorf";
const char stringf_8[] PROGMEM = "Ehrenberg";
const char stringf_9[] PROGMEM = "Eula";
const char stringf_10[] PROGMEM = "Freiberg";
const char stringf_11[] PROGMEM = "Freital";
const char stringf_12[] PROGMEM = "G.Koschen";
const char stringf_13[] PROGMEM = "G.Olbersd";
const char stringf_14[] PROGMEM = "Gast";
const char stringf_15[] PROGMEM = "Gent";
const char stringf_16[] PROGMEM = "Gera";
const char stringf_17[] PROGMEM = "Gesau-O.";
const char stringf_18[] PROGMEM = "Gotha";
const char stringf_19[] PROGMEM = "Großenh.";
const char stringf_20[] PROGMEM = "Hainichen";
const char stringf_21[] PROGMEM = "Hartha";
const char stringf_22[] PROGMEM = "Heim";
const char stringf_23[] PROGMEM = "Hoechst";
const char stringf_24[] PROGMEM = "Hof";
const char stringf_25[] PROGMEM = "Hohburg";
const char stringf_26[] PROGMEM = "Jaenkend.";
const char stringf_27[] PROGMEM = "Jena";
const char stringf_28[] PROGMEM = "Klaffenb.";
const char stringf_29[] PROGMEM = "LWolschen";
const char stringf_30[] PROGMEM = "Langenleu";
const char stringf_31[] PROGMEM = "Lauta";
const char stringf_32[] PROGMEM = "Leipzig";
const char stringf_33[] PROGMEM = "Leutersd.";
const char stringf_34[] PROGMEM = "Lichtent.";
const char stringf_35[] PROGMEM = "Liemehna";
const char stringf_36[] PROGMEM = "Lindenhal";
const char stringf_37[] PROGMEM = "Lippersd.";
const char stringf_38[] PROGMEM = "Loebau";
const char stringf_39[] PROGMEM = "Lueckersd";
const char stringf_40[] PROGMEM = "Ludwigsf.";
const char stringf_41[] PROGMEM = "Magdeburg";
const char stringf_42[] PROGMEM = "Muecheln";
const char stringf_43[] PROGMEM = "Muehlenb.";
const char stringf_44[] PROGMEM = "Mutzschen";
const char stringf_45[] PROGMEM = "NLauterst";
const char stringf_46[] PROGMEM = "NSchiedeb";
const char stringf_47[] PROGMEM = "NSeifersd";
const char stringf_48[] PROGMEM = "Naunhof";
const char stringf_49[] PROGMEM = "Neustadt";
const char stringf_50[] PROGMEM = "Nordhaus.";
const char stringf_51[] PROGMEM = "OCunnersd";
const char stringf_52[] PROGMEM = "Pilzen";
const char stringf_53[] PROGMEM = "Reideburg";
const char stringf_54[] PROGMEM = "Roetha";
const char stringf_55[] PROGMEM = "Saalfeld";
const char stringf_56[] PROGMEM = "Schwerin";
const char stringf_57[] PROGMEM = "Sebnitz";
const char stringf_58[] PROGMEM = "Stadtilm";
const char stringf_59[] PROGMEM = "Steinwie.";
const char stringf_60[] PROGMEM = "Svitavka";
const char stringf_61[] PROGMEM = "Tollwitz";
const char stringf_62[] PROGMEM = "Wiednitz";
const char stringf_63[] PROGMEM = "Winterth.";
const char stringf_64[] PROGMEM = "Zeitz";

// Then set up a table to refer to your strings.
const char* const string_table_liga[] PROGMEM = {
  stringl_0, stringl_1, stringl_2, stringl_3, stringl_4, stringl_5, stringl_6, stringl_7, stringl_8, stringl_9,
  stringl_10, stringl_11, stringl_12, stringl_13, stringl_14, stringl_15, stringl_16, stringl_17, stringl_18, stringl_19,
  stringl_20, stringl_21, stringl_22, stringl_23, stringl_24, stringl_25, stringl_26, stringl_27, stringl_28, stringl_29,
  stringl_30, stringl_31, stringl_32, stringl_33, stringl_34, stringl_35, stringl_36
};

// Then set up a table to refer to your strings.
const char* const string_table_friends[] PROGMEM = {
  stringf_0, stringf_1, stringf_2, stringf_3, stringf_4, stringf_5, stringf_6, stringf_7, stringf_8, stringf_9,
  stringf_10, stringf_11, stringf_12, stringf_13, stringf_14, stringf_15, stringf_16, stringf_17, stringf_18, stringf_19,
  stringf_20, stringf_21, stringf_22, stringf_23, stringf_24, stringf_25, stringf_26, stringf_27, stringf_28, stringf_29,
  stringf_30, stringf_31, stringf_32, stringf_33, stringf_34, stringf_35, stringf_36, stringf_37, stringf_38, stringf_39,
  stringf_40, stringf_41, stringf_42, stringf_43, stringf_44, stringf_45, stringf_46, stringf_47, stringf_48, stringf_49,
  stringf_50, stringf_51, stringf_52, stringf_53, stringf_54, stringf_55, stringf_56, stringf_57, stringf_58, stringf_59,
  stringf_60, stringf_61, stringf_62, stringf_63, stringf_64
};

#define TIMER_PERIOD_SEC 1

void setup()
{
  pinMode(LED_12V_PIN, OUTPUT);

  // initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  lcd.begin (20, 4);
  lcd.createChar(0, aeChar);
  lcd.createChar(1, oeChar);
  lcd.createChar(2, ueChar);
  // Switch on the backlight
  lcd.setBacklightPin(BACKLIGHT_PIN, POSITIVE);
  lcd.setBacklight(HIGH);
  lcd.setCursor (0, 3);
  lcdbuffer = "PRESS Start TO BEGIN";
  lcd.print(lcdbuffer);

  // Setup the button with an internal pull-up :
  pinMode(BUTTON_LEFT_UP_PIN, INPUT_PULLUP);
  buttonLeftUp.attach(BUTTON_LEFT_UP_PIN);
  buttonLeftUp.interval(10); // interval in ms

  pinMode(BUTTON_LEFT_DOWN_PIN, INPUT_PULLUP);
  buttonLeftDown.attach(BUTTON_LEFT_DOWN_PIN);
  buttonLeftDown.interval(10); // interval in ms

  pinMode(BUTTON_RIGHT_UP_PIN, INPUT_PULLUP);
  buttonRightUp.attach(BUTTON_RIGHT_UP_PIN);
  buttonRightUp.interval(5); // interval in ms

  pinMode(BUTTON_RIGHT_DOWN_PIN, INPUT_PULLUP);
  buttonRightDown.attach(BUTTON_RIGHT_DOWN_PIN);
  buttonRightDown.interval(5); // interval in ms

  pinMode(BUTTON_START_STOP_PIN, INPUT_PULLUP);
  buttonStartStop.attach(BUTTON_START_STOP_PIN);
  buttonStartStop.interval(5); // interval in ms

  pinMode(BUTTON_MENU_PIN, INPUT_PULLUP);
  buttonMenu.attach(BUTTON_MENU_PIN);
  buttonMenu.interval(5); // interval in ms

  Timer1.initialize(TIMER_PERIOD_SEC * 1000000);
  Timer1.attachInterrupt(TriggerTimer);
  Timer1.stop();
  //Timer1.restart();


  minutes = MINUTES_VAL;
  seconds = SECONDS_VAL;
  actionState = E_POWERON;
  timerState = E_TIMER_STOPPED;
}

void LCDWrite(String string) {
  int index[3];
  index[0] = string.indexOf('ä');
  index[1] = string.indexOf('ö');
  index[2] = string.indexOf('ü');

  if ( index[0] == -1 && index[1] == -1 && index[2] == -1 ) {
    lcd.print(string);
  }
  else {
    String str1;
    String str2;
    if ( index[0] != -1) {
      str1 = string.substring(0, index[0] - 1);
      str2 = string.substring(index[0] + 1);

      lcd.print(str1);
      lcd.write(byte(0));
      lcd.print(str2);
    }
    else if ( index[1] != -1 ) {
      str1 = string.substring(0, index[1] - 1);
      str2 = string.substring(index[1] + 1);

      lcd.print(str1);
      lcd.write(byte(1));
      lcd.print(str2);
    }
    else {
      str1 = string.substring(0, index[2] - 1);
      str2 = string.substring(index[2] + 1);

      lcd.print(str1);
      lcd.write(byte(2));
      lcd.print(str2);
    }
  }
}


void UpdateLCDValues() {
  lcdbuffer = "";
  switch (actionState) {
    case E_POWERON :
      lcd.setCursor (0, 3);
      lcdbuffer = F("START/STOPP drücken ");
      LCDWrite(lcdbuffer);
      break;
    case E_SETUP :
      switch (setupState) {
        case E_SETUP_GAMETYPE :
          SetValuesToLCD(true, false, false);
          lcd.setCursor (0, 2);
          lcdbuffer = F("Wahl: 1/2 +/-       ");
          LCDWrite(lcdbuffer);
          lcd.setCursor (0, 3);
          lcdbuffer = F("Spielwahl           ");
          LCDWrite(lcdbuffer);
          break;
        case E_SETUP_CLUB :
          SetValuesToLCD(false, false, false);
          lcd.setCursor (0, 2);
          lcdbuffer = F("Wahl: 1/2 +/-       ");
          LCDWrite(lcdbuffer);
          lcd.setCursor (0, 3);
          lcdbuffer = F("Vereinswahl         ");
          LCDWrite(lcdbuffer);
          break;
        case E_SETUP_TEAM :
          SetValuesToLCD(false, true, false);
          lcd.setCursor (0, 2);
          lcdbuffer = F("Wahl: 1/2 +/-       ");
          LCDWrite(lcdbuffer);
          lcd.setCursor (0, 3);
          lcdbuffer = F("Teamwahl            ");
          LCDWrite(lcdbuffer);
          break;
        case E_SETUP_TIMER :
          SetValuesToLCD(false, true, false);
          lcd.setCursor (15, 2);
          SetTimeToLcdBuffer();
          lcd.print(lcdbuffer);
          lcd.setCursor (0, 2);
          lcdbuffer = F("Wahl: 1/2 +/-  ");
          LCDWrite(lcdbuffer);
          lcd.setCursor (0, 3);
          lcdbuffer = F("Zeitwahl            ");
          LCDWrite(lcdbuffer);
          break;
        case E_SETUP_READY :
          SetValuesToLCD(false, true, true);
          lcd.setCursor (15, 2);
          SetTimeToLcdBuffer();
          lcd.print(lcdbuffer);
          lcd.setCursor (0, 2);
          lcdbuffer = F("              ");
          LCDWrite(lcdbuffer);
          lcd.setCursor (0, 3);
          if (firstHalftimeOver == false) {
            lcdbuffer = F("    1.HZ - Bereit   ");
          }
          else {
            lcdbuffer = F("    2.HZ - Bereit   ");
          }
          LCDWrite(lcdbuffer);
          break;
        case E_SETUP_HALFTIME :
          SetValuesToLCD(false, true, true);
          lcd.setCursor (15, 2);
          SetTimeToLcdBuffer();
          lcd.print(lcdbuffer);
          lcd.setCursor (0, 2);
          lcdbuffer = F("2 min m. Start ");
          LCDWrite(lcdbuffer);
          lcd.setCursor (0, 3);
          lcdbuffer = F("2.HZ oder Abbr.m LM ");
          LCDWrite(lcdbuffer);
          break;
      }
      break;
    case E_HALFTIME :
      SetValuesToLCD(false, true, true);
      lcd.setCursor (15, 2);
      SetTimeToLcdBuffer();
      lcd.print(lcdbuffer);
      lcd.setCursor (0, 2);
      lcdbuffer = F("Halbzeitpause");
      LCDWrite(lcdbuffer);
      lcd.setCursor (0, 3);
      lcdbuffer = F("Beenden m Starttaste");
      LCDWrite(lcdbuffer);
      break;
    case E_RUNNING :
      SetValuesToLCD(false, true, true);
      lcd.setCursor (15, 2);
      SetTimeToLcdBuffer();
      lcd.print(lcdbuffer);
      if (stoppedTimer == 1) {
        lcd.setCursor (0, 2);
        if (runningMenuTime == 1) {
          lcdbuffer = F("Zeit           ");
        }
        else {
          lcdbuffer = F("Tore           ");
        }
        lcd.print(lcdbuffer);
        lcd.setCursor (0, 3);
        if (firstHalftimeOver == false) {
          lcdbuffer = F("    1.HZ - Start    ");
        }
        else {
          lcdbuffer = F("    2.HZ - Start    ");
        }
      }
      else {
        lcd.setCursor (0, 2);
        lcdbuffer = F("     ");
        lcd.print(lcdbuffer);
        lcd.setCursor (0, 3);
        if (firstHalftimeOver == false) {
          lcdbuffer = F("    1.HZ - Läuft   ");
        }
        else {
          lcdbuffer = F("    2.HZ - Läuft   ");
        }
      }
      LCDWrite(lcdbuffer);
      break;
    case E_FINISHED:
      if (firstHalftimeOver == false) {
        // This is the first halftime
        SetValuesToLCD(false, true, true);
        lcd.setCursor (15, 2);
        SetTimeToLcdBuffer();
        lcd.print(lcdbuffer);
        lcd.setCursor (0, 2);
        lcdbuffer = F("2 min m. Start ");
        LCDWrite(lcdbuffer);
        lcd.setCursor (0, 3);
        lcdbuffer = F("2.HZ oder Abbr.m LM ");
        LCDWrite(lcdbuffer);
      } else {
        // The game is over 
        lcd.setCursor (0, 2);
        if ( goals_team2 < goals_team1 ) {
          lcdbuffer += (String)clubs_1 + " gewinnt";
        } else if ( goals_team2 > goals_team1 ) {
          lcdbuffer += (String)clubs_2 + " gewinnt";
        } else {
          lcdbuffer += "Unendschieden";
        }
        int spacesToEnd = LCDROWSIZE - lcdbuffer.length();
        for (int i = 0; i < spacesToEnd; i++) {
          lcdbuffer += ' ';
        }
        LCDWrite(lcdbuffer);         
        lcd.setCursor (0, 3);
        lcdbuffer = F("                    ");
        LCDWrite(lcdbuffer);
      }
      break;
  }
}

void SetValuesToLCD(bool gameSelection, bool setTeamCounter, bool showGoals) {
  lcdbuffer = "";
  if (gameSelection == true) {
    if (ligaGame == false) {
      lcdbuffer = " Freundschaftsspiel ";
    } else {
      lcdbuffer = "     Punktspiel     ";
    }
    lcd.setCursor (0, 0);
    LCDWrite(lcdbuffer);
  } else {
    int spacesToEnd = 0;
    /* write the club name and team number to LCD */
    lcdbuffer += clubs_1;
    if (setTeamCounter) {
      if (team1Counter == INVISIBLETEAMNUMBER) {
        lcdbuffer += ' ';
      }
      else {
        lcdbuffer += team1Counter;
      }
    }

    /* SPACE to the end of line */
    spacesToEnd = LCDROWSIZE - lcdbuffer.length();
    if (showGoals) {
      spacesToEnd--;
      if (goals_team1 > 9) {
        spacesToEnd--;
      }
    }
    for (int i = TEAM1COL; i < spacesToEnd; i++) {
      lcdbuffer += ' ';
    }
    if (showGoals) lcdbuffer += goals_team1;
    lcd.setCursor (TEAM1COL, TEAM1ROW);
    LCDWrite(lcdbuffer);
    //lcd.print(lcdbuffer);

    lcdbuffer = "";
    lcdbuffer += clubs_2;
    if (setTeamCounter) {
      if (team2Counter == INVISIBLETEAMNUMBER) {
        lcdbuffer += ' ';
      }
      else {
        lcdbuffer += team2Counter;
      }
    }
    spacesToEnd = LCDROWSIZE - lcdbuffer.length();
    if (showGoals) {
      spacesToEnd--;
      if (goals_team2 > 9) {
        spacesToEnd--;
      }
    }
    for (int i = TEAM2COL; i < spacesToEnd; i++) {
      lcdbuffer += ' ';
    }
    if (showGoals) lcdbuffer += goals_team2;
    lcd.setCursor (TEAM2COL, TEAM2ROW);
    LCDWrite(lcdbuffer);
  }
}

void SetTimeToLcdBuffer() {

  lcdbuffer = "";
  /* write the time to lcdbuffer stream */
  if (minutes < 10) {
    lcdbuffer += '0';
  }
  lcdbuffer += minutes;
  if (seconds < 10) {
    lcdbuffer += ":0";
  }
  else {
    lcdbuffer += ':';
  }
  lcdbuffer += seconds;
}

/***************** ISR *******************/
/** This routine is called by interrupt **/
/*****************************************/
void TriggerTimer() {
  if (minutes > 0) {
    if (seconds > 0) {
      seconds--;
    } else {
      minutes--;
      seconds = 59;
    }
  } else {
    if (seconds > 1) {
      seconds--;
      if (seconds == SECONDS_BEFORE_BUZZ_IN_HALFTIME) {
        timerState = E_TIMER_BUZZ_EVENT;
      }
    } else {
      seconds--;
      timerState = E_TIMER_FINISHED;
    }
  }
}


void ResetDefault() {
  if ( ligaGame == true ) {
    arrayLength = ARRAYSIZE_LIGA;
    if (club1Counter > arrayLength - 1) {
      club1Counter = 0;
    }
    if (club2Counter > arrayLength - 1) {
      club2Counter = 0;
    }
  } else {
    arrayLength = ARRAYSIZE_FRIENDS;
  }
  minutes = initialMinutes;
  seconds = initialSeconds;
  stoppedTimer = 1;
  firstHalftimeOver = false;
  alreadyBuzzered = false;
}

void ResetParameters() {
  ResetDefault();
  goals_team1 = 0;
  goals_team2 = 0;
}

void ResetForHalftime() {
  ResetDefault();
  minutes = MINUTES_PAUSE_VAL;
  seconds = SECONDS_PAUSE_VAL;
  firstHalftimeOver = true;
}

void ResetPressedStates() {
  firstPlusPressedShort = false;
  firstPlusPressedLong = false;
  firstMinusPressedShort = false;
  firstMinusPressedLong = false;
  secondPlusPressedShort = false;
  secondPlusPressedLong = false;
  secondMinusPressedShort = false;
  secondMinusPressedLong = false;
  startStopPressed = false;
  menuPressedShort = false;
  menuPressedLong = false;
}

void CheckButtons() {
  ResetPressedStates();

  // Get the updated values :
  if (firstPlusPressed) {
    if ( (millis() - lastPressedTimeFirstPlus) > 250) {
      firstPlusPressedLong = true;
      lastPressedTimeFirstPlus = millis();
    }
  }
  if ( buttonLeftUp.update() ) {
    if ( buttonLeftUp.read() == HIGH ) {
      lastPressedTimeFirstPlus = millis();
      firstPlusPressed = true;
      firstPlusPressedShort = true;
    } else {
      firstPlusPressed = false;
    }
  }

  if (firstMinusPressed) {
    if ( (millis() - lastPressedTimeFirstMinus) > 250) {
      firstMinusPressedLong = true;
      lastPressedTimeFirstMinus = millis();
    }
  }
  if ( buttonLeftDown.update() ) {
    if ( buttonLeftDown.read() == HIGH ) {
      lastPressedTimeFirstMinus = millis();
      firstMinusPressed = true;
      firstMinusPressedShort = true;
    } else {
      firstMinusPressed = false;
    }
  }

  if (secondPlusPressed) {
    if ( (millis() - lastPressedTimeSecondPlus) > 250) {
      secondPlusPressedLong = true;
      lastPressedTimeSecondPlus = millis();
    }
  }
  if ( buttonRightUp.update() ) {
    if ( buttonRightUp.read() == HIGH ) {
      lastPressedTimeSecondPlus = millis();
      secondPlusPressed = true;
      secondPlusPressedShort = true;
    } else {
      secondPlusPressed = false;
    }
  }

  if (secondMinusPressed) {
    if ( (millis() - lastPressedTimeSecondMinus) > 250) {
      secondMinusPressedLong = true;
      lastPressedTimeSecondMinus = millis();
    }
  }
  if ( buttonRightDown.update() ) {
    if ( buttonRightDown.read() == HIGH ) {
      lastPressedTimeSecondMinus = millis();
      secondMinusPressed = true;
      secondMinusPressedShort = true;
    } else {
      secondMinusPressed = false;
    }
  }

  if ( buttonStartStop.update() ) {
    if ( buttonStartStop.read() == HIGH ) {
      startStopPressed = true;
    }
  }
  if (menuPressed) {
    if ( (millis() - lastPressedTimeMenu) > LONG_PRESS_TRIGGER_DURATION_MS) {
      menuPressedLong = true;
      lastPressedTimeMenu = millis();
    }
  }

  if ( buttonMenu.update() ) {
    if ( buttonMenu.read() == HIGH ) {
      lastPressedTimeMenu = millis();
      menuPressed = true;
      menuPressedShort = true;
    } else {
      menuPressed = false;
    }
  }
}

void ClearLCDRow(int row) {
  lcdbuffer = "";
  for (int i = 0; i < LCDROWSIZE; i++) {
    lcdbuffer += ' ';
  }
  lcd.setCursor (0, row);
  lcd.print(lcdbuffer);
}

void ScoreBoardOutput() {
  scoreBoardBuffer = "";
  int spacesToEnd;
  //first row
  scoreBoardBuffer += clubs_1;
  if (team1Counter == INVISIBLETEAMNUMBER) {
    scoreBoardBuffer += ' ';
  }
  else {
    scoreBoardBuffer += team1Counter;
  }
  spacesToEnd = SCOREBOARDROWSIZE - scoreBoardBuffer.length() - 3;
  for (int i = 0; i < spacesToEnd; i++) {
    scoreBoardBuffer += ' ';
  }
  if (goals_team1 < 10) scoreBoardBuffer += ' ';
  scoreBoardBuffer += goals_team1;

  //one space to next row begin
  scoreBoardBuffer += ' ';

  //second row
  scoreBoardBuffer += clubs_2;
  if (team2Counter == INVISIBLETEAMNUMBER) {
    scoreBoardBuffer += ' ';
  }
  else {
    scoreBoardBuffer += team2Counter;
  }
  spacesToEnd = 2 * SCOREBOARDROWSIZE - scoreBoardBuffer.length() - 3;
  for (int i = 0; i < spacesToEnd; i++) {
    scoreBoardBuffer += ' ';
  }
  if (goals_team2 < 10) scoreBoardBuffer += ' ';
  scoreBoardBuffer += goals_team2;

  // the three separated dot matrix elements to show the timer
  //one space to next row begin
  scoreBoardBuffer += ' ';
  scoreBoardBuffer += minutes;
  scoreBoardBuffer += seconds / 10;
  scoreBoardBuffer += seconds % 10;

  //lamps and buzzer
  scoreBoardBuffer += '0';
  scoreBoardBuffer += '0';
  scoreBoardBuffer += '0';
  scoreBoardBuffer += '0';
  scoreBoardBuffer += '0';
  scoreBoardBuffer += '0';

  if (buzzerOn) {
    scoreBoardBuffer += 'F';
  } else {
    scoreBoardBuffer += '0';
  }

  scoreBoardChecksum = 5; //STX + ETX
  for (int i = 0; i < scoreBoardBuffer.length(); i++) {
    scoreBoardChecksum += scoreBoardBuffer[i];
  }

  Serial.print(char(STX));//STX
  Serial.print(scoreBoardBuffer);
  Serial.print(char(ETX));//ETX
  Serial.print(char(scoreBoardChecksum));
}

void Toggle_12VLED()
{
  if (LED_12V_pinstate == 0) {
    LED_12V_pinstate = 1;
    digitalWrite(LED_12V_PIN, HIGH);
  }
  else {
    LED_12V_pinstate = 0;
    digitalWrite(LED_12V_PIN, LOW);
  }

}

void CheckButtonsAndSwitchTime() {
  if ( firstPlusPressedShort || firstPlusPressedLong) {
    minutes++;
  }
  if ( firstMinusPressedShort || firstMinusPressedLong ) {
    if ( minutes > 0 ) minutes--;
  }
  if ( secondPlusPressedShort || secondPlusPressedLong ) {
    if ( seconds == 59 ) {
      seconds = 0;
    }
    else {
      seconds++;
    }
  }
  if ( secondMinusPressedShort || secondMinusPressedLong ) {
    if ( seconds == 0 ) {
      seconds = 59;
    }
    else {
      seconds--;
    }
  }
}

void CheckButtonsAndSwitchGoals() {
  if ( firstPlusPressedShort ) {
    goals_team1++;
  }
  if ( firstMinusPressedShort ) {
    if ( goals_team1 > 0 ) goals_team1--;
  }
  if ( secondPlusPressedShort ) {
    goals_team2++;
  }
  if ( secondMinusPressedShort ) {
    if ( goals_team2 > 0 ) goals_team2--;
  }
}

/****************************************************************************/
/************************ THE MAIN LOOP *************************************/
void loop()
{
  CheckButtons();
  UpdateLCDValues();
  switch (actionState) {
    case E_POWERON :
      // Start mode
      ResetParameters();
      if ( startStopPressed ) {
        actionState = E_SETUP;
        ClearLCDRow(0);
        ClearLCDRow(1);
        ClearLCDRow(2);
        ClearLCDRow(3);
      }
      break;
    case E_SETUP :
      buzzerOn = false;
      // Set parameter mode
      switch (setupState) {
        case E_SETUP_HALFTIME :
          ResetForHalftime();
          CheckButtonsAndSwitchGoals();
          if (startStopPressed) {
            ClearLCDRow(2);
            ClearLCDRow(3);
            actionState = E_HALFTIME;
            ScoreBoardOutput();
            Timer1.initialize(TIMER_PERIOD_SEC * 1000000);
            Timer1.start();
            timerState = E_TIMER_RUNNING;
          }
          if (menuPressedLong) {
            setupState = E_SETUP_CLUB;
          }
          break;

        case E_SETUP_GAMETYPE :
          // Set the game type
          if ( firstPlusPressedShort || firstMinusPressedShort ) {
            if ( ligaGame == true ) {
              ligaGame = false;
            } else {
              ligaGame = true;
            }
          }
          break;
        case E_SETUP_CLUB :
          // Reset values for halftime
          ResetParameters();

          // Set the club name
          if ( firstPlusPressedShort || firstPlusPressedLong ) {
            if ( club1Counter < arrayLength - 1 ) {
              club1Counter++;
            }
            else {
              club1Counter = 0;
            }
          }
          if ( firstMinusPressedShort || firstMinusPressedLong ) {
            if ( club1Counter > 0 ) {
              club1Counter--;
            }
            else {
              club1Counter = arrayLength - 1;
            }
          }
          if ( secondPlusPressedShort || secondPlusPressedLong ) {
            if ( club2Counter < arrayLength - 1 ) {
              club2Counter++;
            }
            else {
              club2Counter = 0;
            }
          }
          if ( secondMinusPressedShort || secondMinusPressedLong ) {
            if ( club2Counter > 0 ) {
              club2Counter--;
            }
            else {
              club2Counter = arrayLength - 1;
            }
          }
          if (ligaGame == true ) {
            strcpy_P(clubs_1, (char*)pgm_read_word(&(string_table_liga[club1Counter])));
            strcpy_P(clubs_2, (char*)pgm_read_word(&(string_table_liga[club2Counter])));
          } else {
            strcpy_P(clubs_1, (char*)pgm_read_word(&(string_table_friends[club1Counter])));
            strcpy_P(clubs_2, (char*)pgm_read_word(&(string_table_friends[club2Counter])));
          }
          break;
        case E_SETUP_TEAM :
          // Set the team number
          if ( firstPlusPressedShort ) {
            if ( team1Counter < MAXTEAMNUMBER ) team1Counter++;
          }
          if ( firstMinusPressedShort ) {
            if ( team1Counter > MINTEAMNUMBER ) team1Counter--;
          }
          if ( secondPlusPressedShort ) {
            if ( team2Counter < MAXTEAMNUMBER ) team2Counter++;
          }
          if ( secondMinusPressedShort ) {
            if ( team2Counter > MINTEAMNUMBER ) team2Counter--;
          }
          break;
        case E_SETUP_TIMER :
          CheckButtonsAndSwitchTime();
          initialMinutes = minutes;
          initialSeconds = seconds;
          break;
        case E_SETUP_READY :
          if ( startStopPressed ) {
            ClearLCDRow(2);
            ClearLCDRow(3);
            actionState = E_RUNNING;
            ScoreBoardOutput();
          }
          break;
      }

      if ( menuPressedShort ) {
        if (setupState == E_SETUP_GAMETYPE ) {
          setupState = E_SETUP_CLUB;
        }
        else if (setupState == E_SETUP_CLUB ) {
          setupState = E_SETUP_TEAM;
        }
        else if (setupState == E_SETUP_TEAM ) {
          setupState = E_SETUP_TIMER;
        }
        else if (setupState == E_SETUP_TIMER ) {
          setupState = E_SETUP_READY;
        }
        else if (setupState == E_SETUP_READY ) {
          setupState = E_SETUP_GAMETYPE;
          ClearLCDRow(1);
          ClearLCDRow(2);
        }
      }
      break;
    case E_HALFTIME :
      if (timerState == E_TIMER_BUZZ_EVENT) {
        timerState = E_TIMER_RUNNING;
        //BUZZ HERE
        counter_for_buzzing = 0;
        buzzerOn = true;
      }
      if ( (startStopPressed) || (timerState == E_TIMER_FINISHED)) {//if ( (timerState == E_TIMER_FINISHED) || (menuPressedShort) ) {
        actionState = E_RUNNING;
        counter_for_waiting = 0;
        Timer1.stop();
        timerState = E_TIMER_STOPPED;
        minutes = initialMinutes;
        seconds = initialSeconds;
        buzzerOn = false;
      }
      CheckButtonsAndSwitchGoals(); //This is new 2017-05-09
      ScoreBoardOutput();
      delay(100);
      if (counter_for_buzzing == 5) {
        buzzerOn = false;
      }
      counter_for_buzzing++;
      counter_for_waiting++;
      if (counter_for_waiting == 10) {
        counter_for_waiting = 0;
        Toggle_12VLED();
      }

      /* Reset */
      if (menuPressedLong) {
        minutes = initialMinutes;
        seconds = initialSeconds;
        goals_team2 = 0;
        goals_team1 = 0;
        actionState = E_SETUP;
        setupState = E_SETUP_CLUB;
        ScoreBoardOutput();
      }
      break;

    case E_RUNNING :
      /*******************************
       ** running mode **
      *******************************/

      if (stoppedTimer == 0) { //while timer is running the LED is ON
        runningMenuTime = 0;
        digitalWrite(LED_12V_PIN, HIGH);
        if (timerState == E_TIMER_FINISHED) {
          actionState = E_FINISHED;
          counter_for_waiting = 0;
          Timer1.stop();
          timerState = E_TIMER_STOPPED;
        }
        ScoreBoardOutput();
        delay(20);  //14 und delay(10)
        CheckButtonsAndSwitchGoals();
      }
      else { //while timer is stopped the LED is blinking
        /* Timer stopped */
        /* Toggle LED in waiting mode */
        counter_for_waiting++;
        if (counter_for_waiting % 10 == 0) {
          delay(10);
          Toggle_12VLED();
          if (counter_for_waiting % 20 == 0) {
            ScoreBoardOutput();
          }
        }
        /* Change menu and change time */
        /* Switch by pressing menuButton */

        if ( menuPressedShort ) {
          if (runningMenuTime == 0) {
            runningMenuTime = 1;
          }
          else {
            runningMenuTime = 0;
          }
        }
        if (runningMenuTime == 0) {
          /* it is the normal goal switching */
          CheckButtonsAndSwitchGoals();
        }
        else {
          /* it is the time switching mode */
          CheckButtonsAndSwitchTime();
        }
        /* Reset */
        if (menuPressedLong) {
          minutes = initialMinutes;
          seconds = initialSeconds;
          goals_team2 = 0;
          goals_team1 = 0;
          actionState = E_SETUP;
          setupState = E_SETUP_CLUB;
          ScoreBoardOutput();
        }
      }
      if ( startStopPressed ) {
        if (stoppedTimer == 0) {
          stoppedTimer = 1;
          Timer1.stop();
          timerState = E_TIMER_STOPPED;
        }
        else {
          stoppedTimer = 0;
          Timer1.start();
          timerState = E_TIMER_RUNNING;
        }
      }
      break;
    case E_FINISHED :
      // timeout mode
      counter_for_waiting++;
      if (firstHalftimeOver == true) {
        buzzerVar = BUZZER_VAL_FIN;
      } else {
        buzzerVar = BUZZER_VAL_HT;
      }
      
      // buzzerloop, is called twice
      if (counter_for_waiting % buzzerVar == 1) { // activate buzzer by first run
        if (buzzerOn == false && alreadyBuzzered == false) {
          buzzerOn = true;
        } else {
          buzzerOn = false;
          alreadyBuzzered = true;
        }
        ScoreBoardOutput();
        delay(50);
        Toggle_12VLED();
      }
          
      // called if buzzerloop was called twice
      // For Halftime there is no further interaction needed, 
      if (alreadyBuzzered) { //was startStopPressed
        if (firstHalftimeOver == false) { //
          setupState = E_SETUP_HALFTIME;
          actionState = E_SETUP;
        } else {
          // If the game is over, we will wait for the startStopButton
          if ( startStopPressed ) {
            actionState = E_SETUP;
            setupState = E_SETUP_CLUB;
          }
        }
      }
      break;
  }
}











































