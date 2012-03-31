#include <mpi.h>
#include <iostream>
#include <fstream>
#include <queue>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


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
   /*numbers.push(6);
   numbers.push(4);
   numbers.push(7);
   numbers.push(8);
   numbers.push(2);
   numbers.push(3);
   numbers.push(5);
   numbers.push(1);*/
   // nacteni cisel ze souboru do pracovni fronty
   if(myid == 0) {
      char input[] = "numbers";                                                          // nazev souboru obsahujici vstupni data
      int16_t number;                                                                   // hodnota pri nacitani souboru
      fstream fin;                                                                       // cteni ze souboru
      fin.open(input, ios::in);                                                          // otevreni souboru pro cteni
      int i = 0;
      queue<int16_t> pomNumbers;                                                              // fronta cisel o velikosti 8bitu
      while(fin.good()) {                                                                // prochazim soubro dokud nejsu na konci
         number = fin.get();                                                             // nacitani po znaku - 8bite
         if(!fin.good()) {                                                               // nactenmi EOF
            break;                                                                       // nacte i eof, takze vyskocim
         }
//         cout << number <<endl;
         pomNumbers.push(number);                                                           // ulozeni cisla do fronty
         i++;
      }
      fin.close();                                                                       // uzavreni souboru
      for(;i>0;i--) {
         cout << pomNumbers.front() << " ";
         numbers.push(pomNumbers.front());
         pomNumbers.pop();
      }
      cout << endl;

   }
   // tvorba pole obsahujici indexy zacatku pracovani procesoru
   int start = (1 << myid) + myid -1;
   int startPrev = (1 << (myid-1)) + myid-1 -1;
//   cout << "nnnnnnnnnn" << myid << ":" <<startPrev<< "->end : "<<start<<endl;
   int end = (1 << (numprocs-1))-1 + (1 << myid) + myid;
   int endPre;
   int endSort = 0;
   if(myid == 1) {
      endPre = 1 << (numprocs-1);
   }
   else {
      endPre = (1 << (numprocs - 1)) - 1 + (1 << (myid - 1)) + myid - 1;
   }
   endSort =  (1 << (numprocs - 1)) + (1 << numprocs) + numprocs - 1;
//   cout << "///////////" << myid << ":" <<endPre<< "->end : "<<end<<endl;
    
   queue<int16_t> que1;                                                              // fronta cisel o velikosti 8bitu
   queue<int16_t> que2;                                                              // fronta cisel o velikosti 8bitu
   bool firstQue = true;
   int numQ = 1 << (myid - 1);
   int numQ1 = numQ;
   int numQ2 = numQ;
    //RAZENI-------------------------------------------------------------------
    //cyklus pro linearitu

   int cmp = 0;
   int cmpX = (1 << myid ) - 1;
   bool cmpQue1 = true;
   int change = 0;
   int printNum = 0;
   int pom = (1 << (numprocs-1));
   int endAlg = 2*pom + (numprocs -1) -1;
//   cout << endAlg <<endl;
   //for(int j = 0; printNum != pom; j++) {
   for(int j = 0; j <= endAlg; j++) {
      if(myid == 0) {                                                                                    // prvni CPU, posila prvky ze vstupni fronty
         if(!numbers.empty()) {                                                                          // dokud neni fronta prazdna
            mynumber = numbers.front();                                                                  // prvni prvek z fronty
            numbers.pop();                                                                               // odebrani prvku z fronty
            MPI_Send(&mynumber, 1, MPI_INT, myid+1, TAG, MPI_COMM_WORLD);                                // poslu sousedovi svoje cislo
//            cout << j << "-id: " << myid << " -> " << mynumber << "\tto id: " << myid+1 << endl;
         }
         else {                                                                                          // jiz neni co posilat
            //TODO:zastavit? uz nema co delat
         }
      }
      else {                                                                                             // vesechny cpu, krome prvniho
         // prijimani prvku od vedlejsiho procesoru
         if(j >= startPrev && j < endPre) {
            if(numQ == change) {
               firstQue = !(firstQue && firstQue);
               change = 0;
            }
            change++;
            MPI_Recv(&neighnumber, 1, MPI_INT, myid-1, TAG, MPI_COMM_WORLD, &stat); //prijimam
            // pridani cisla do spravne fronty
            if(firstQue) {
               que1.push(neighnumber);
//               cout << j << "-id: " << myid << " <- " << neighnumber << "\tfrom id: " << myid-1 << " in to : 1" << endl;
            }
            else {
               que2.push(neighnumber);
//               cout << j << "-id: " << myid << " <- " << neighnumber << "\tfrom id: " << myid-1 << " in to : 2"<<endl;
            }
         }
         // porovnani prvku ve forntach a poslani mensi vedlejsimu cpu
         if(j >= start && j < end) {
            //porovnat nebo poslat minule porovnny prvek
            if(cmp < cmpX || myid == (numprocs-1)) {       
               // porovnani prvku
//               cout << j << "-id: "<< myid << " compare  " << que1.front() << ":" << que2.front() << endl;
               // pokud je fronta prazdna kopiruji z druhe
               if(myid == (numprocs-1) && (que1.empty()||que2.empty())) {                  // uz je vse hotove, jedna rada prazdna taj zbytek vytisknu
                  if(que1.empty()) {;
                     while(!que2.empty()) {
                        cout << que2.front() << endl;
                        que2.pop();
                     }
                  }
                  else {;
                     while(!que1.empty()) {
                        cout << que1.front() << endl;
                        que1.pop();
                     }
                  }
                  break;
               }
               if((numQ1 == 0) ) {
                  mynumber = que2.front();
                  que2.pop();
               }
               else if(numQ2 == 0) {
                  mynumber = que1.front();
                  que1.pop();
               }
               else {
                  if(que1.front() >= que2.front()) {
                     mynumber = que2.front();
                     que2.pop();
                     numQ2--;
                     cmpQue1 = true;                                                      // prvek se bral z druhe fronty, dalsi pripadne z prvni
                  }
                  else {
                     mynumber = que1.front();
                     que1.pop();
                     numQ1--;
                     cmpQue1 = false;                                                     // prvek se bral z prvni fronty
                  }
               }
               cmp++;
            }
            else {
               if(cmpQue1) {
                  mynumber = que1.front();
                  que1.pop();
               }
               else {
                  mynumber = que2.front();
                  que2.pop();
               }
 //                 cout<< "zde"<<endl;
               cmp = 0;
               numQ1 = numQ;
               numQ2 = numQ;
            }
//            cout << j << "-id: " << myid << " -> " << mynumber << "\tto id: " << myid+1 << endl;
            if(myid != (numprocs-1)) {
               MPI_Send(&mynumber, 1, MPI_INT, myid+1, TAG, MPI_COMM_WORLD);          //poslu sousedovi svoje cislo
            }
            else {
//               cout << j << "-...................................." << mynumber <<endl;
               cout << mynumber <<endl;
               printNum++;
            }
         }
      }
   }
    MPI_Finalize(); 
//                  cout<<"konec"<<endl;
    return 0;

 }//main
