#include <mpi.h>
#include <iostream>
#include <fstream>
#include <queue>
#include <stdint.h>

 using namespace std;

 #define TAG 0

int main(int argc, char *argv[]) {
   int numprocs;                                                                         // pocet procesoru
   int myid;                                                                             // muj rank
   int neighnumber;                                                                      // hodnota souseda
   int mynumber;                                                                         // moje hodnota
   MPI_Status stat;                                                                      // struct- obsahuje kod- source, tag, error

   //MPI INIT
   MPI_Init(&argc,&argv);                                                                // inicializace MPI 
   MPI_Comm_size(MPI_COMM_WORLD, &numprocs);                                             // pocet bezicich procesu 
   MPI_Comm_rank(MPI_COMM_WORLD, &myid);                                                 // id meho procesu 
 

	int size;
   queue<int16_t> numbers;                                                              // fronta cisel o velikosti 8bitu
 
   // nacteni cisel ze souboru do pracovni fronty
   if(myid == 0) {
      char input[] = "numbers";                                                          // nazev souboru obsahujici vstupni data
      int16_t number;                                                                   // hodnota pri nacitani souboru
      fstream fin;                                                                       // cteni ze souboru
      fin.open(input, ios::in);                                                          // otevreni souboru pro cteni

      while(fin.good()) {                                                                // prochazim soubro dokud nejsu na konci
         number = fin.get();                                                             // nacitani po znaku - 8bite
         numbers.push(number);                                                           // ulozeni cisla do fronty
         if(!fin.good()) {                                                               // nactenmi EOF
            break;                                                                       // nacte i eof, takze vyskocim
         }
         //MPI_Send(&number, 1, MPI_INT, invar, TAG, MPI_COMM_WORLD); //buffer,velikost,typ,rank prijemce,tag,komunikacni skupina
      }
      fin.close();                                                                       // uzavreni souboru       
   }
//   for(int i = numbers.size() -1; i > 0; i--) {
//      cout << (int16_t) numbers.front() << endl;
      //cout<<i<<":"<<numbers.size()<<endl;
//      numbers.pop();
//   }

    //PRIJETI HODNOTY CISLA
    //vsechny procesory(vcetne mastera) prijmou hodnotu a zahlasi ji
//    MPI_Recv(&mynumber, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &stat); //buffer,velikost,typ,rank odesilatele,tag, skupina, stat
    //cout<<"i am:"<<myid<<" my number is:"<<mynumber<<endl;
 
    //LIMIT PRO INDEXY
//    int oddlimit= 2*(numprocs/2)-1;                 //limity pro sude
  //  int evenlimit= 2*((numprocs-1)/2);              //liche
 //   int halfcycles= numprocs/2;
    //if(myid == 0) cout<<oddlimit<<":"<<evenlimit<<endl;

   queue<int16_t> que1;                                                              // fronta cisel o velikosti 8bitu
   queue<int16_t> que2;                                                              // fronta cisel o velikosti 8bitu
   bool firstQue = false;
    //RAZENI-------------------------------------------------------------------
    //cyklus pro linearitu

   for(int j=0; j<8; j++){
       // posilam cislo ze zacatku fronty procesoru vedle        
      if(myid == 0) {
         mynumber = numbers.front();
         numbers.pop();
         cout<< "posilam:"<<mynumber<<endl;
         MPI_Send(&mynumber, 1, MPI_INT, myid+1, TAG, MPI_COMM_WORLD);          //poslu sousedovi svoje cislo
      }
      if((myid != 0) && (j%(myid)) == 0) {
         firstQue = !(firstQue && firstQue);
         cout<<firstQue<<endl;
      }
      if(myid == 1) {
         MPI_Recv(&neighnumber, 1, MPI_INT, myid-1, TAG, MPI_COMM_WORLD, &stat); //prijimam
      }
      // pridani cisla do spravne fronty
      if(firstQue) {
         que1.push(neighnumber);
         cout << "prvni"<<myid<<endl;
      }
      else {
         cout << "druha"<< myid<<endl;
         que2.push(neighnumber);
      }
   }
       /*
        //sude proc 
        if((!(myid%2) || myid==0) && (myid<oddlimit)){
            MPI_Send(&mynumber, 1, MPI_INT, myid+1, TAG, MPI_COMM_WORLD);          //poslu sousedovi svoje cislo
            MPI_Recv(&mynumber, 1, MPI_INT, myid+1, TAG, MPI_COMM_WORLD, &stat);   //a cekam na nizsi
            //cout<<"ss: "<<myid<<endl;
        }//if sude
        else if(myid<=oddlimit){//liche prijimaji zpravu a vraceji mensi hodnotu (to je ten swap)
            MPI_Recv(&neighnumber, 1, MPI_INT, myid-1, TAG, MPI_COMM_WORLD, &stat); //prijimam

            if(neighnumber > mynumber){                                             //pokud je leveho sous cislo vetsi
                MPI_Send(&mynumber, 1, MPI_INT, myid-1, TAG, MPI_COMM_WORLD);       //poslu svoje 
                mynumber= neighnumber;                                              //a vemu si jeho
            }
            else MPI_Send(&neighnumber, 1, MPI_INT, myid-1, TAG, MPI_COMM_WORLD);   //pokud je mensi nebo stejne vratim
            //cout<<"sl: "<<myid<<endl;
        }//else if (liche)
        else{//sem muze vlezt jen proc, co je na konci
        }//else

        //liche proc 
        if((myid%2) && (myid<evenlimit)){
            MPI_Send(&mynumber, 1, MPI_INT, myid+1, TAG, MPI_COMM_WORLD);           //poslu sousedovi svoje cislo
            MPI_Recv(&mynumber, 1, MPI_INT, myid+1, TAG, MPI_COMM_WORLD, &stat);    //a cekam na nizsi
            //cout<<"ll: "<<myid<<endl;
        }//if liche
        else if(myid<=evenlimit && myid!=0){//sude prijimaji zpravu a vraceji mensi hodnotu (to je ten swap)
            MPI_Recv(&neighnumber, 1, MPI_INT, myid-1, TAG, MPI_COMM_WORLD, &stat); //jsem sudy a prijimam

            if(neighnumber > mynumber){                                             //pokud je leveho sous cislo vetsi
                MPI_Send(&mynumber, 1, MPI_INT, myid-1, TAG, MPI_COMM_WORLD);       //poslu svoje 
                mynumber= neighnumber;                                              //a vemu si jeho
            }
            else MPI_Send(&neighnumber, 1, MPI_INT, myid-1, TAG, MPI_COMM_WORLD);   //pokud je mensi nebo stejne vratim
            //cout<<"ls: "<<myid<<endl;
        }//else if (sude)
        else{//sem muze vlezt jen proc, co je na konci
        }//else
        
    }//for pro linearitu
    //RAZENI--------------------------------------------------------------------


    //FINALNI DISTRIBUCE VYSLEDKU K MASTEROVI-----------------------------------
    int* final= new int [numprocs];
    //final=(int*) malloc(numprocs*sizeof(int));
    for(int i=1; i<numprocs; i++){
       if(myid == i) MPI_Send(&mynumber, 1, MPI_INT, 0, TAG,  MPI_COMM_WORLD);
       if(myid == 0){
           MPI_Recv(&neighnumber, 1, MPI_INT, i, TAG, MPI_COMM_WORLD, &stat); //jsem 0 a prijimam
           final[i]=neighnumber;
       }//if sem master
    }//for

    if(myid == 0){
        final[0]= mynumber;
        for(int i=0; i<numprocs; i++){
            cout<<"proc: "<<i<<" num: "<<final[i]<<endl;
        }//for
    }//if vypis
    //cout<<"i am:"<<myid<<" my number is:"<<mynumber<<endl;
    //VYSLEDKY------------------------------------------------------------------

 */
    MPI_Finalize(); 
    return 0;

 }//main
