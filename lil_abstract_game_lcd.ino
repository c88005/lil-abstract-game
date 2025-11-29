#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int axisX = A0;
const int axisY = A1;
const int buttonPin = 7; // i still have no clue what const does tbh
int floor_n = 1;
int tick = 0;
int delay_amt = 250;
int resources = 0;
int health = 10;
int light_lvl = 0; // im gon make the maximum light lvl of 4 or smth
int position = 48; // because of me beign dummah we got the system if u go forward position is getting substracted (edit: i changed it bc everything else could break)
int valX, valY, valButton = 0;
String items = "aT #HBW";
String top = "I      T      T      T   Y   T  Y YT TT TF TTIFTTITY  a  YT  F T T TT TT TTTT Y  TT Y TY    Y TT        Y  T     H        T    T TY  T T  YT ";
String top2 = "WWWWWWWW     TYTYTYTa   TYTYTYTa   TYTYTYT a_____a TYTYTYT   aTYTYTYT   aTYTYTYT     WWW ";
bool dead = false;
bool customPlr = false;
bool transition = false;
String map_printable;
String transit_msg1 = "going up";
String transit_msg2 = "the ladder";

uint8_t customChar[8] = {0x00,0x00,0x00,0x11,0x00,0x15,0x0A,0x00};
String crafting[3] = {"lamp","heal","wall"};
int curCraftingIndex = 0;
String curCraftingItem = crafting[curCraftingIndex];

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP);
  lcd.init();
  lcd.createChar(1, customChar);
  lcd.backlight();
  lcd.setCursor(4, 0);
  //lcd.print("Hello!!!"); i forgot abt this
}

void init_movment(String map_printable) {
  valX = analogRead(axisX);
  if (valX <= 60) {
      if (map_printable[8] != items[6]) {
      position += 1;
    }
  }
  else if (valX >= 560) {
    if (map_printable[6] != items[6]) {
      position -= 1;
    }
      
  }
}

void init_action(String map_printable) {
  valY = analogRead(axisY);
  valButton = 1 - digitalRead(buttonPin);
  if (valButton == 1) {
    if (map_printable[7] == items[1]) {
      if (resources < 9) {
        if (floor_n == 1) {top[position + 7] = ' ';}
        else if (floor_n == 2) {top2[position + 7] = ' ';}
        resources += 1;
      }
    }
    else if (map_printable[7] == items[3]) {
      if (resources < 9) {
        if (floor_n == 1) {top[position + 7] = ' ';}
        else if (floor_n == 2) {top2[position + 7] = ' ';}
        resources += 2;
      }
    }
    else if (map_printable[7] == items[0]) {
      if (resources < 9) {
        if (floor_n == 1) {top[position + 7] = ' ';}
        else if (floor_n == 2) {top2[position + 7] = ' ';}
        resources += 3;
      }
    }
    else if (map_printable[7] == items[4]) {
      transition = true;
      position = 40;
      floor_n = 2;

    }
  }
  if (valY >= 560) {
    if (map_printable[7] == items[2]) {
      if (curCraftingIndex == 0) {
        if (resources >= 3) {
          if (floor_n == 1) {top[position + 7] = 'a';}
          else if (floor_n == 2) {top2[position + 7] = 'a';}
          resources -= 3;
        }
      }
      else if (curCraftingIndex == 1) {
        if (resources >= 2) {
          health += 2;
          resources -= 2;
        }
      }
      else if (curCraftingIndex == 2) {
        if (resources >= 4) {
          if (floor_n == 1) {top[position + 7] = '#';}
          else if (floor_n == 2) {top2[position + 7] = '#';}
          resources -= 4;
        }
      }
    }
  }
  if (valY <= 60) {
    if (curCraftingIndex < 2) {
      curCraftingIndex += 1;
      curCraftingItem = crafting[curCraftingIndex];
    }
    else {
      curCraftingIndex = 0;
      curCraftingItem = crafting[curCraftingIndex];
    }
  }
}

void map_drawing(String floor, String map_printable) {
  lcd.setCursor(0, 1);
  lcd.print(floor);
  lcd.setCursor(0, 0);
  lcd.print(map_printable);
  lcd.setCursor(7, 0);
  if (customPlr) {
    lcd.write(1);
  }
  else {
    lcd.write(0xff);
  }
  
}

void draw_transit(String msgtop, String msgdown, int amt1, int amt2) {
  lcd.setCursor(amt1, 0);
  lcd.print(msgtop);
  lcd.setCursor(amt2, 1);
  lcd.print(msgdown);
}

void light_src(String map_printable) {
  // yippie fricked up code!!!
  if (map_printable[7] == items[0]) {
    light_lvl = 5;
  }
  else if ((map_printable[6] == items[0]) or (map_printable[8] == items[0])) {
    light_lvl = 4;
  }
  else if ((map_printable[5] == items[0]) or (map_printable[9] == items[0])) {
    light_lvl = 3;
  }
  else if ((map_printable[4] == items[0]) or (map_printable[10] == items[0])) {
    light_lvl = 2;
  }
  else if ((map_printable[3] == items[0]) or (map_printable[11] == items[0])) {
    light_lvl = 1;
  }
  else {
    light_lvl = 0;
  }
}

void tick_system(int maximum, int step) {
  if (tick < maximum) {
    tick += step;
  }
  else {
    tick = 0;
  }
}

void dark_hurts(int dpt) {
  if (tick == dpt and light_lvl == 0) {
    health -= 1;
  }
}

void check_dead() {
  if (health == 0) {
    dead = true;
  }
}

void health_capping(int maximum) {
  if (health > maximum) {
    health = maximum;
  }
}

void loop() {
  // MAP PIECES!!!!!!!!!!!!!!!!!!!!!!
  String floor = String(health) + "#" + String(light_lvl) + "#" + String(resources) + "#" + curCraftingItem + "##" + String(floor_n) + "####";
  if (floor_n == 1) { // cuts map so u see only the part the position is on (sry for bad english)  
    map_printable = top.substring(position, 15 + position);
  }
  else if (floor_n == 2) {
    map_printable = top2.substring(position, 15 + position);
  }
  

  lcd.clear();
  tick_system(10, 1);
  if (dead == true) {
    transition = true;
    position = 48;
    health = 10;
    floor_n = 1;
    resources = 0;
  }
  if (transition == false) {
    map_drawing(floor, map_printable);
    light_src(map_printable);
    if (dead == false) {
      check_dead();
      health_capping(10);
      if (floor_n == 1) {dark_hurts(10);}
      init_movment(map_printable);
      init_action(map_printable);
    }
  }
  else {
    draw_transit(transit_msg1, transit_msg2, 4, 3);
    delay(2000);
    transition = false;
    if (dead == true) {
      dead = false;
    }
  }
  

  delay(delay_amt);
  
  
}
