#include <Servo.h>;
#include <LiquidCrystal.h>


int IntensiteLumiere;
volatile int state=HIGH;
Servo fanion;
volatile int LedPin = 10;
int resetButton=3;
LiquidCrystal lcd(12, 11, 7, 6, 5, 4);
int compteur = 0;
int heure;
int minute;
int seconde;
char tab[15];
int i = 0;


void setup(){
  pinMode(LedPin,OUTPUT);
  Serial.begin(9600);
  fanion.attach(9);
  pinMode(9,OUTPUT);
  lcd.begin(16, 2);
  fanion.write(0);
  digitalWrite(LedPin, LOW);
  lcd.setCursor(0, 0);
  lcd.print("00:00:00");
  initClock();
  pinMode(resetButton, INPUT);
    digitalWrite(resetButton, HIGH);  // force internal pullup
  attachInterrupt(1,reset,FALLING);
  
  Serial.print("Rentrez la date sous le format 'hh mm ss' : \n");
}

static void initClock() {
  TCCR2A = 0; // normal counting mode
  TCCR2B = _BV(CS21); // set prescaler of 8
  OCR2A = ( clockCyclesPerMicrosecond() * 100) / 8;
  TCNT2 = 0; // clear the timer count
  TIFR2 |= _BV(OCF2A); // clear any pending interrupts;
  TIMSK2 |= _BV(OCIE2A) ; // enable the output compare interrupt
}

ISR(TIMER2_COMPA_vect)
{
  compteur ++;
  TCNT2 = 0;
  OCR2A = ( clockCyclesPerMicrosecond() * 100) / 8;
  if (compteur == 10000)
  { compteur = 0;
    if (state == LOW)
      state = HIGH;
    else
      state = LOW;
    digitalWrite(13, state);//allume la led de la carte toutes les 1 secondes
    avancerheure();
    afficherheureCourante();
  }
}

void avancerheure() {
  seconde += 1;
  if (seconde == 60)
  {  seconde = 0;
     minute += 1; }
  if (minute == 60)
  {  minute = 0;
     heure += 1; }
  if (heure == 24)
     heure = 0;
}
//Gestion du passage du temps
void afficherheureCourante () {
  lcd.setCursor(0, 0);
  lcd.print("00:00:00");
  if (heure < 10)
    lcd.setCursor(1, 0);
  else
    lcd.setCursor(0, 0);
  lcd.print(heure);
  
  if (minute < 10)
    lcd.setCursor(4, 0);
  else
    lcd.setCursor(3, 0);
  lcd.print(minute);
  
  if (seconde < 10)
    lcd.setCursor(7, 0);
  else
    lcd.setCursor(6, 0);
  lcd.print(seconde);
}

//Gestion de l'affichage de l'heure 
void afficherheurePassage () {
  lcd.setCursor(0, 1); 
  lcd.print("Passage ");
  lcd.setCursor(8, 1);
  lcd.print("00:00:00");
  if (heure < 10)
    lcd.setCursor(9, 1);
  else
    lcd.setCursor(8, 1);
  lcd.print(heure);
  
  if (minute < 10)
    lcd.setCursor(12, 1);
  else
    lcd.setCursor(11, 1);
  lcd.print(minute);
  
  if (seconde < 10)
    lcd.setCursor(15, 1);
  else
    lcd.setCursor(14, 1);
  lcd.print(seconde);
}
//Gestion de l'affichage de l'heure du dernier passage
void passageCourrier() {
  digitalWrite(LedPin, HIGH);
  Serial.print("\nCourrier\t");
  Serial.print(heure);
  Serial.print(":");
  Serial.print(minute);
  Serial.print(":");
  Serial.print(seconde);
  afficherheurePassage();
  fanion.write(90);
  
}
// tous les évenements à faire lorsque qu'un courrier est détecté

void reset() {
  Serial.println("coucou");
  digitalWrite(LedPin, LOW);
  lcd.clear();
  afficherheureCourante();
  fanion.write(0);
}

//Fonction appelée quand on appuie sur le bouton


void loop(){
  interrupts();
  if (Serial.available()>0)
  {  while (Serial.available()>0) {
      tab[i] = Serial.read();
      i++;
      }
    tab[i] = '\0';
    sscanf(tab, "%u %u %u", &heure, &minute, &seconde);
    //stocke les heures de passage dans un tableau
  }
  IntensiteLumiere = analogRead(A0);
  if(IntensiteLumiere<=650)//si l'intensité est basse/lettre présente, lance la fonction passageCourrier()
    passageCourrier();
  delay(200);
  }
