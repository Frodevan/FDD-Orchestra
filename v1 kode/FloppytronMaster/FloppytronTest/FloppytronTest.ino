#define DATAPIN 12
#define CLOCKPIN 13
#define RESETPIN1 11
#define RESETPIN2 10
#define RESETPIN3 9

void setup() {
  pinMode(RESETPIN1, OUTPUT);
  digitalWrite(RESETPIN1, LOW);
  pinMode(RESETPIN2, OUTPUT);
  digitalWrite(RESETPIN2, LOW);
  pinMode(RESETPIN3, OUTPUT);
  digitalWrite(RESETPIN3, LOW);
  pinMode(DATAPIN, OUTPUT);
  digitalWrite(DATAPIN, HIGH);
  pinMode(CLOCKPIN, OUTPUT);
  digitalWrite(CLOCKPIN, LOW);
  
  delay(10);
  digitalWrite(RESETPIN1, HIGH);
  delay(10);
  sendid(1);
  sendid(1);
  sendid(1);
  
  delay(10);
  digitalWrite(RESETPIN2, HIGH);
  delay(10);
  sendid(2);
  sendid(2);
  sendid(2);

  delay(10);
  digitalWrite(RESETPIN3, HIGH);
  delay(10);
  sendid(3);
  sendid(3);
  sendid(3);
  
  delay(2000);

}

void loop() {
  
  aktiver(1);
  spill(60); delay(400); stopp(60); delay(400);
  spill(60); delay(400); stopp(60); delay(400);
  spill(60); delay(600); stopp(60); delay(200);
  spill(56); delay(600); spill(63); delay(200);   
  spill(60); delay(600); stopp(60); delay(200);
  spill(56); delay(600); spill(63); delay(200);
  spill(60); delay(1200); stopp(60); delay(400);
  deaktiver(1);
  
  aktiver(2);
  spill(67); delay(400); stopp(67); delay(400);
  spill(67); delay(400); stopp(67); delay(400);
  spill(67); delay(600); stopp(67); delay(200);
  spill(68); delay(600); spill(63); delay(200);
  spill(59); delay(800);   
  spill(56); delay(600); spill(63); delay(200);
  spill(60); delay(1200); stopp(60); delay(400);
  deaktiver(2);
  
  aktiver(3);
  spill(72); delay(400); stopp(72); delay(400);
  spill(60); delay(400); stopp(60); delay(200); spill(60); delay(200);
  spill(72); delay(600); stopp(72); delay(200);
  spill(71); delay(300); stopp(71); delay(100); spill(70); delay(300); stopp(70); delay(100);
  spill(68); delay(200); spill(67); delay(200); spill(68); delay(400); stopp(68);
  delay(400); spill(61); delay(200); stopp(61); delay(200);
  spill(66); delay(600); stopp(66); delay(200);
  spill(65); delay(300); stopp(65); delay(100); spill(64); delay(300); stopp(64); delay(100);
  spill(63); delay(200); spill(62); delay(200); spill(63); delay(400); stopp(63);
  delay(400); spill(56); delay(200); stopp(56); delay(200);
  spill(59); delay(600); stopp(59); delay(200);
  spill(56); delay(600); spill(60); delay(200);
  spill(63); delay(600); stopp(63); delay(200);
  spill(60); delay(600); spill(63); delay(200);
  spill(67); delay(1200); stopp(67); delay(400);
  deaktiver(3);

  aktiver(1);
  bassmode(1);
  aktiver(-1);
  spill(72); delay(400); stopp(72); delay(400);
  spill(60); delay(400); stopp(60); delay(200); spill(60); delay(200);
  spill(72); delay(600); stopp(72); delay(200);
  spill(71); delay(300); stopp(71); delay(100); spill(70); delay(300); stopp(70); delay(100);
  spill(68); delay(200); spill(67); delay(200); spill(68); delay(400); stopp(68);
  delay(400); spill(61); delay(200); stopp(61); delay(200);
  spill(66); delay(600); stopp(66); delay(200);
  spill(65); delay(300); stopp(65); delay(100); spill(64); delay(300); stopp(64); delay(100);
  spill(63); delay(200); spill(62); delay(200); spill(63); delay(400); stopp(63);
  delay(400); spill(56); delay(200); stopp(56); delay(200);
  spill(59); delay(600); stopp(59); delay(200);
  spill(56); delay(600); spill(63); delay(200);
  spill(60); delay(600); stopp(60); delay(200);
  spill(56); delay(600); spill(63); delay(200);
  spill(60); delay(1200); stopp(60); delay(400);
  bassmode(0);
  deaktiver(-1);
}

void sendid(int id)
{
  id = id&0x0F;
  shiftout(0x55);
  delay(20);
  shiftout(0x2B);
  delay(20);
  shiftout(0xAA);
  delay(20);
  shiftout(0xD4);
  delay(20);
  shiftout((id<<4)|id);
  delay(20);
}

void aktiver(int id)
{
  if(id == -1)
    shiftout(0x11);
  else
    shiftout(id%0x11);
}

void deaktiver(int id)
{
  if(id == -1)
    shiftout(0x91);
  else
    shiftout(0x80|(id%0x11));
}

void bassmode(int suboctave)
{
  if(suboctave == 0) shiftout(0x92);
  if(suboctave == 1) shiftout(0x13);
  if(suboctave == 2) shiftout(0x93);
  if(suboctave == 3) shiftout(0x14);
  if(suboctave == 4) shiftout(0x94);
}

void spill(int midi)
{
  shiftout((midi)&0x7F);
}

void stopp(int midi)
{
  shiftout((midi)|0x80);
}

void kalibrer()
{
  shiftout(0x80);
}

void shiftout(int data)
{
  for (int i = 0; i < 8; i++)
  {

    if(data&(0x80>>i))
    {
        digitalWrite(DATAPIN, HIGH);
    }
    else
    {
        digitalWrite(DATAPIN, LOW);
    }
    digitalWrite(CLOCKPIN, HIGH);
    digitalWrite(CLOCKPIN, LOW);
  }
  digitalWrite(DATAPIN, HIGH);
}

