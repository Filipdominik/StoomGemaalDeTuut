/* Deze software is geschreven door Filip Lonski voor stoomgemaal de Tuut.
    Alleen waardes veranderen als u exact weet wat deze doen, ik ben niet verantwoordelijk ervoor als u iets veranderd en daardoor fysieke beschadigingen krijgt.
    Als u problemen tegenmoed komt na het aanpassen van de code kunt u de software opnieuw hier (https://github.com/Filipdominik/StoomGemaalDeTuut) downloaden.
    Veel plezier met het gebruiken van de maquette!
De volgende variables zijn toegestaan om veranderdt te worden:*/
#define GemaalAanPercentage 60 //Bij hoeveel % bedenkking van de sensor moet het gemaal aan
#define GemaalUitPercentage 50 //Bij hoeveel %  bedekking van de sensor gaat het gemaal uit.
#define TijdTussenSeizoenen 5000 //Hoeveel milisceonden het systeem moet wachten tussen seizoenen in (geef aan in ms)
#define InputWachtTijd 5000 //Hoeveel ms de gebruiker heeft om een keuze te maken bij handmatig.

//Begin definiëren van poorten en sensoren
#define Klep1Port 32
#define Klep2Port 33
#define Klep3Port 34
#define Klep4Port 35
#define Klep5Port 36
#define Klep6Port 37
#define Pomp1Port 38
#define Pomp2Port 39

#define LEDRegenPort 40
#define LEDWinterPort 41
#define LEDZomerPort 42

#define WaterNiveauSensor A2
#define KnopRegenPort A4
#define KnopWinterPort A6
#define KnopZomerPort A8

#define WaterVlotterPort 27
#define SchakelaarHandPort 29
#define SchakelaarAutoPort 31

#define WaterNiveauStatus map((analogRead(WaterNiveauSensor)),0,700,0,100)
#define KnopRegenStatus map(analogRead(KnopRegenPort),230,20,0,1)
#define KnopWinterStatus map(analogRead(KnopWinterPort),230,20,0,1)
#define KnopZomerStatus map(analogRead(KnopZomerPort),230,20,0,1)
#define VlotterStatus map(digitalRead(WaterVlotterPort),1,0,0,1)
//Einde definiëren van poorten en sensoren

//Nu komen de variables
//Booleans: true/false
bool LEDAllState = false;
bool LEDWinterStatus = false;
bool LEDZomerStatus = false; 
bool LEDRegenStatus = false;
bool RunningSimulation = false;
bool SelectieZomer = false;
bool SelectieWinter = false;
bool SelectieRegen = false;


unsigned long time;             //Unsigned long omdat deze variables groter worden dan  32.000
unsigned long predictedTime;
unsigned long LastEventTime;
//Einde variables

String SchakelaarStand(){       //Geeft 1 (Hand), 2(Auto) of 0(Niks) terug; de status van de 3-stap-schakelaar.
    if (digitalRead(SchakelaarHandPort)){return "Hand";}
    if (digitalRead(SchakelaarAutoPort)){return "Auto";}
    else{return "Nul";}}

int DigitalOutputPorts[] = {Klep1Port,Klep2Port,Klep3Port,Klep4Port,Klep5Port,Klep6Port,Pomp1Port,Pomp2Port,LEDZomerPort,LEDWinterPort,LEDRegenPort};

void setup(){
    if (KnopRegenStatus&&KnopZomerStatus&&KnopWinterStatus){
        Serial.begin(9600);
        Serial.println("Sensor reading mode activated");
        while(true){
            Serial.println("Sensor           | Value");
            Serial.print("Knop Zomer       |\t");
            Serial.println(KnopZomerStatus);
            Serial.print("Knop Winter      |\t");
            Serial.println(KnopWinterStatus);
            Serial.print("Knop Regen       |\t");
            Serial.println(KnopRegenStatus);
            Serial.print("Water Niveau     |\t");
            Serial.println(WaterNiveauStatus);
            Serial.print("Vlotter          |\t");
            Serial.println(VlotterStatus);
            Serial.print("3 Stand          |\t");
            Serial.println(SchakelaarStand());
            Serial.println("-------------------------------\n");
            delay(1000);
        }
    }
    for (int i=0;i<=10;i++){                    //Een loop wat de DigitalOutputPorts door gaat, en die als OUTPUT zet.
        pinMode(DigitalOutputPorts[i],OUTPUT);
        digitalWrite(DigitalOutputPorts[i],HIGH);}

    pinMode(WaterNiveauSensor,INPUT);       //Inputs vermelden. INPUT_PULLUP wordt gebruikt omdat de knoppen met GND zijn aangesloten op de arduino.
    pinMode(KnopRegenPort,INPUT_PULLUP);
    pinMode(KnopWinterPort,INPUT_PULLUP);
    pinMode(KnopZomerPort,INPUT_PULLUP);
    ToggleLEDs();
    int VlotterGemiddelde=0;
    for (int i=0;i<=10;i++){
        VlotterGemiddelde+=VlotterStatus;
        delay(80);}

    if (int(VlotterGemiddelde/10) == 0){
        while (true){
            ToggleLEDs();
            delay(500);}}
    else{ToggleLEDs();}
        
}

void loop(){                            //Kijk welke modus de 3-Stap-schakelaar op is.
    while (SchakelaarStand()=="Auto"){
        AutomatischModus();}

    while (SchakelaarStand()=="Hand"){
        HandmatigeModus();}

    if (SchakelaarStand()=="Nul"){
        PowerAllDown();
        RunningSimulation = false;
        SelectieZomer = false;
        SelectieWinter = false;
        SelectieRegen = false;
        while (SchakelaarStand()=="Nul"){}
    }
}

void PowerAllDown(){                    //Zet alle relais uit.
    digitalWrite(Klep1Port,HIGH);       
    digitalWrite(Klep2Port,HIGH);
    digitalWrite(Klep3Port,HIGH);
    digitalWrite(Klep4Port,HIGH);
    digitalWrite(Klep5Port,HIGH);
    digitalWrite(Klep6Port,HIGH);
    digitalWrite(Pomp1Port,HIGH);
    digitalWrite(Pomp2Port,HIGH);
    LEDZomer(false);
    LEDRegen(false);
    LEDWinter(false);
}

void ToggleLEDs(){      //Schakeld de stand van de LEDs.
    if (LEDAllState){
        LEDWinter(false);
        LEDZomer(false);
        LEDRegen(false);
        LEDAllState = false;
    }
    else{
        LEDWinter(true);
        LEDZomer(true);
        LEDRegen(true);
        LEDAllState = true;
    }
}

void LEDWinter(bool Power){     //Zet de Regen LED aan (true) of uit (false)
    if (Power){
    digitalWrite(LEDWinterPort,LOW);
    LEDWinterStatus=true;
    }
    else{
        digitalWrite(LEDWinterPort,HIGH);
        LEDWinterStatus=false;
    }
}

void LEDZomer(bool Power){      //Zet de Regen LED aan (true) of uit (false)
    if (Power){
    digitalWrite(LEDZomerPort,LOW);
    LEDZomerStatus=true;
    }
    else{
        digitalWrite(LEDZomerPort,HIGH);
        LEDZomerStatus=false;
    }
}

void LEDRegen(bool Power){          //Zet de Regen LED aan (true) of uit (false)
    if (Power){
    digitalWrite(LEDRegenPort,LOW);
    LEDRegenStatus=true;
    }
    else{
        digitalWrite(LEDRegenPort,HIGH);
        LEDRegenStatus=false;
    }
}

void ZomerToggle(bool(RegenS)){       //Zomerstand, geef regen aan door ZomerStand(True)
    digitalWrite(Klep1Port,LOW);
    digitalWrite(Klep2Port,HIGH);
    digitalWrite(Klep3Port,LOW);
    digitalWrite(Klep4Port,LOW);
    digitalWrite(Klep5Port,LOW);
    digitalWrite(Klep6Port,HIGH);
    digitalWrite(Pomp1Port,LOW);
    digitalWrite(Pomp2Port,HIGH);
    LEDZomer(true);
    LEDWinter(false);
    LEDRegen(false);
    if (RegenS){
        digitalWrite(Klep3Port,HIGH);
        LEDRegen(true);
    }
}

void WinterToggle(bool(RegenS)){      //Winterstand, geef regen aan door True of False
    digitalWrite(Klep1Port,LOW);
    digitalWrite(Klep2Port,LOW);
    digitalWrite(Klep3Port,LOW);
    digitalWrite(Klep4Port,HIGH);
    digitalWrite(Klep5Port,LOW);
    digitalWrite(Klep6Port,HIGH);
    digitalWrite(Pomp1Port,LOW);
    digitalWrite(Pomp2Port,HIGH);
    LEDZomer(false);
    LEDWinter(true);
    LEDRegen(false);
    if (RegenS){
        digitalWrite(Klep3Port,HIGH);
        LEDRegen(true);
    }    
}

void GemaalAAN(){                   //Je moet dit pas triggeren als je genoeg water hebt, ander gaat die door.
    digitalWrite(Klep1Port,LOW);
    digitalWrite(Klep2Port,LOW);
    digitalWrite(Klep3Port,HIGH);
    digitalWrite(Klep4Port,HIGH);
    digitalWrite(Klep5Port,HIGH);
    digitalWrite(Klep6Port,LOW);
    digitalWrite(Pomp1Port,LOW);
    digitalWrite(Pomp2Port,LOW);
    while (WaterNiveauStatus>=GemaalUitPercentage){     //Zolang de niveau sensor meer water ziet dan dat het uitgaan percentage is
        time = millis();
        if (time>=predictedTime){                       //Als de tijd voorbij de uitgerekende knipper tijd is.
            predictedTime = time+1000;                  //Geef nieuwe waardes voor het laten knipperen.
            ToggleLEDs();                               //Schakel de LEDs.
        }
    }
    PowerAllDown();
}

void SeizoenCyclus(int Seizoen){    //Geef welk seizoen je wil afspelen (gaat 1-4)
    if (Seizoen==1){
        ZomerToggle(false);
    }
    if (Seizoen==2){
        ZomerToggle(true);
    }
    if (Seizoen==3){
        WinterToggle(false);
    }
    if (Seizoen==4){
        WinterToggle(true);
    }
}

void AutomatischModus(){        //Als de schakelaar op automatisch is gedraaid dan start dit.
    RunningSimulation=false;             
    time = millis();                    //Krijg de tijd voor timing
    predictedTime = time+300;           //De voorspelde tijd om de lampjes aan en uit te laten gaan, ik gebruik geen delay hier omdat je anders geen input in de tussentijd kan invoeren.
    while (KnopZomerStatus==false && KnopWinterStatus==false && KnopRegenStatus==false && SchakelaarStand()=="Auto" && RunningSimulation==false){ //Zolang er geen knoppen geklikt worden, de schakelaar nog op auto modus staat en niks is aan het afspelen:
        time = millis();
        if (time>=predictedTime){       //Als de tijd voorbij de voorspelde tijd is om te knipperen
            ToggleLEDs();               //Schakel de LEDs van Uit/Aan (gebeurd automatisch in void ToggleLEDs())
            predictedTime = time+600;   //Update de voorspelde tijd.
        }
    }

    if ((KnopZomerStatus||KnopWinterStatus||KnopRegenStatus)&& SchakelaarStand()=="Auto"){
        RunningSimulation=true;
    }

    if (SchakelaarStand()=="Auto"){     //Als de schakelaar nog steeds op auto mode is.
        int CurrentSeason = 1;          
        while (RunningSimulation){      
            time = millis();            
            predictedTime = time;

            while (WaterNiveauStatus<GemaalAanPercentage && RunningSimulation){     //Speel af zolang het gemaal niet vol is.
                time = millis();
                if (CurrentSeason==5&&(time>=predictedTime)){              //Als het CurrentSeason 5 raakt, stop het afspelen want het is klaar.
                    RunningSimulation=false;
                    PowerAllDown();                 //Reset de relais.
                }
                if (time>=predictedTime){
                    SeizoenCyclus(CurrentSeason);   //Speel het gekozen seisoen af (kijk void SeizoenCyclus() voor meer)
                    predictedTime = time+TijdTussenSeizoenen;   
                    CurrentSeason += 1;
                }
            }
            if (WaterNiveauStatus>GemaalAanPercentage){ //Als het water nog steeds boven het trigger percentage is, zet het gemaal aan.
                GemaalAAN();                            //In void GemaalAAN(){} wacht het systeem totdat het water gezakt is onder het trigger percentage om het gemaal uit te zetten.
            }
        }
    }
    
}

void HandmatigeModus(){         //Als de schhakelaar op hand is gedraaid...
    while (RunningSimulation==false && SchakelaarStand()=="Hand"){
        time = millis();
        if (time>=LastEventTime&&(SelectieWinter||SelectieZomer)){  //Als de tijd voorbij de tijd is wat is gekozen om verder te gaan. Start de simulatie van het gekozen schema.
            RunningSimulation=true;
        }
        if (time>=(LastEventTime-(InputWachtTijd/3))&&(SelectieZomer||SelectieWinter)){  //Als de gebruiker 1/3 tijd over om zijn keuze te maken heeft beginnen de LEDs te knipperen.
            if (time>=predictedTime){
                if (SelectieZomer){
                    if (LEDZomerStatus){LEDZomer(false);}
                    else{LEDZomer(true);}}

                if (SelectieWinter){
                    if (LEDWinterStatus){LEDWinter(false);}
                    else{LEDWinter(true);}}

                if (SelectieRegen){
                    if (LEDRegenStatus){LEDRegen(false);}
                    else{LEDRegen(true);}}
  
                predictedTime = time+350;
            }
        }
        else{
            if (SelectieZomer){LEDZomer(true);}
            if (SelectieRegen){LEDRegen(true);}
            if (SelectieWinter){LEDWinter(true);}
        }
        //Hier kan de gebruiker zijn keuze maken, de knoppen werken als schakelaars, als je het voor de 1e keer klikt, gaat die aan. 2e keer uit.
        //Als de gebruiker zomer klikt terwijl winter aan is, gaat winter uit en zomer aan. En vice versa.
        if (KnopZomerStatus){
            if (SelectieZomer){
                LEDZomer(false);
                SelectieZomer=false;
            }
            else{
                LEDWinter(false);
                LEDZomer(true);
                SelectieWinter=false;
                SelectieZomer=true;
            }
            while (KnopZomerStatus){}
            LastEventTime=time+InputWachtTijd;
        }

        if (KnopWinterStatus){
            if (SelectieWinter){
                LEDWinter(false);
                SelectieWinter=false;
            }
            else{
                LEDZomer(false);
                LEDWinter(true);
                SelectieZomer=false;
                SelectieWinter=true;
            }
            while(KnopWinterStatus){}
            LastEventTime=time+InputWachtTijd;
        }

        if (KnopRegenStatus){
            if (SelectieRegen){
                LEDRegen(false);
                SelectieRegen=false;
            }
            else{
                LEDRegen(true);
                SelectieRegen=true;
            }
            while(KnopRegenStatus){}
            LastEventTime=time+InputWachtTijd;
        }

    }
    //Als de selectie tijd voorbij is, en de keuze is bevestigd:
    while (RunningSimulation&&KnopZomerStatus==false&&KnopRegenStatus==false&&KnopWinterStatus==false&&SchakelaarStand()=="Hand"){
        //Als zomer is gekozen:
        if (SelectieZomer){
            PowerAllDown();
            ZomerToggle(SelectieRegen);}
        //Anders:
        else{
            PowerAllDown();
            WinterToggle(SelectieRegen);}
        
        //Zolang de simulatie bezig is, schakelaar op hand mode staat en geen knoppen zijn ingeklikt:
        while (RunningSimulation&&SchakelaarStand()=="Hand"&&(KnopZomerStatus==false&&KnopRegenStatus==false&&KnopWinterStatus==false)){
            if (WaterNiveauStatus>=GemaalAanPercentage){
                GemaalAAN();}}
        //Uiteindelijk zet alles terug:
        SelectieZomer = false;
        SelectieWinter = false;
        SelectieRegen = false;
        PowerAllDown();
        RunningSimulation=false;
    }
}
