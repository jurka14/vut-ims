/*

IMS Projekt 2020 -  Téma č. 8: Diskrétní model výrobního procesu (SHO) - Výroba pouzdra ložiska

Autory : Vojtěch Jurka (xjurka08),  Dávid Špavor (xspavo00)
Preklad : make
Spustenie s implicitnym koncovim casom : make run
Spustenie s explicitnym koncovim casom : make run endtime=10000
Odstranenie binarnych suborov : make clean

*/

#include "simlib.h"
#include <stdlib.h> 


/***globalni objekty***/

//vyrobni linky
Facility  Pila("Pila");
Facility  Soustruh1("Soustruh1");
Facility  Soustruh2("Soustruh2");
Facility  Bruska("Bruska");

//statistiky
Stat pocet_kusu_pila("Pocet kusu narezanych na pile");
Stat pocet_kusu_soustruh1("Pocet hotovych kusu - soustruh 1");
Stat pocet_kusu_soustruh2("Pocet hotovych kusu - soustruh 2");
Stat pocet_kusu("Pocet hotovych kusu");

//pomocne countery
int cnt1 = 0;
int cnt2 = 0;

/****************Proces prvni vymeny platku****************/

class VymenaPlatku1 : public Process { 
  void Behavior() {

    Priority = 1; //nastav prioritu procesu ve fronte

    Seize(Soustruh1);
    Wait(Uniform(30, 180));
    Release(Soustruh1);

  }
};

/****************Proces druhe vymeny platku****************/

class VymenaPlatku2 : public Process { 
  void Behavior() {

    Priority = 1; //nastav prioritu procesu ve fronte

    Seize(Soustruh2);
    Wait(Uniform(30, 180));
    Release(Soustruh2);

  }
};

/****************Proces Pouzdra****************/

class Pouzdro : public Process { 
  void Behavior() {

    /***prvni soustruzeni***/
    
    cnt1++;

    if(cnt1 == 70) //pokud je to 70. kus, co jde na soustruh
    {
      cnt1 = 0;
      Seize(Soustruh1); //obsad 1. soustruh
      //aktivace procesu vymeny platku, ktery se zaradi na celo fronty pred soustruhem
      (new VymenaPlatku1)->Activate(); 
    }
    else
    {
      Seize(Soustruh1);
    }

    Wait(Uniform(10, 15)); //vymena kusu na soustruhu
    Wait(90); //soustruzeni
    Release(Soustruh1); //uvolni 1. soustruh
    pocet_kusu_soustruh1(1); //zapis vyrobeny kus

    /***druhe soustruzeni***/

    //funguje stejne jako prvni soustruzeni, jen je doba soustruzeni jina
    cnt2++;

    if(cnt2 == 70)
    {
      cnt2 = 0;
      Seize(Soustruh2);
      (new VymenaPlatku2)->Activate();
    }
    else
    {
      Seize(Soustruh2);
    }

    Wait(Uniform(10, 15));
    Wait(101);
    Release(Soustruh2);
    pocet_kusu_soustruh2(1); //zapis vyrobeny kus

    /***brouseni***/

    Seize(Bruska);
    Wait(37);
    Release(Bruska);

    pocet_kusu(1); //zapis vyrobeny kus

  }
};

/****************Proces Tyce****************/

class Pipe : public Process { 
  void Behavior() {

    Seize(Pila); //Obsad pilu

    Wait(Uniform(180, 420)); //pockam, az bude tyc nasazena na pile

    for (int i = 0; i < 36; i++) //z jedne tyce narezu 36 kusu
    {
      Wait(Uniform(15, 20)); //doba rezani kusu
      (new Pouzdro)->Activate(); //aktivace procesu kusu
      pocet_kusu_pila(1); //zapis vyrobeny kus
    }

    Release(Pila); //uvolni pilu
  }
};

/****************Generator Procesu****************/

class GeneratorPipes : public Event {
  void Behavior() {

    if(!Pila.Busy()) //pokud uz je pila volna, generuj dalsi tyc
    {
      (new Pipe)->Activate(); //nova tyc
    }

    Activate( Time + 1 ); //znovu to zkus za sekundu
  
  }
  
};
/****************Parsovanie argumentov****************/

int parse_arguments_endtime(int argc, char *argv[])
{
   if( argc == 2 ) {
      return atoi(argv[1]);
   }
   else {
      return 28800; // defaultna hodnota casu -  28800 sekund (8 hodin)
   }
}
/****************Simulacni Experiment****************/

int main(int argc, char *argv[]) {

  int end_time = parse_arguments_endtime(argc, argv); // spracovanie argumentov
  
  SetOutput("model.out");
  Init(0,end_time);              // inicializace simulace na čas 0 - end_time sekund
  (new GeneratorPipes)->Activate(); // aktivace generatoru
  Run();                     // spusteni simulace

  /***vypis statistik***/
  Pila.Output(); 
  Soustruh1.Output(); 
  Soustruh2.Output();
  Bruska.Output();

  pocet_kusu_pila.Output();
  pocet_kusu_soustruh1.Output();
  pocet_kusu_soustruh2.Output();
  pocet_kusu.Output();

  return 0;
 }
