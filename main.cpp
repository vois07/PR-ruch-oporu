#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <mpi.h>

using namespace std;
p
//TODO: tagi wiadomości

class Conspirator {
    public:
        bool isAcceptor;
        bool isMeetingLeader;
        bool wantsToMeet;

        int parentId;
        int leftNeighbourId;
        int rightNeighbourId;
        int leftChildId;
        int rightChildId;

        vector<int> team; //ludzie do spotkania
        bool isDvdTaken; //klasa?

        Conspirator(typ zmienne_konfiguracyjne, int myId) {
            if (jest_na_liście_akceptorów) {
                this->isAcceptor = true;
            } else {
                this->isAcceptor = false;
            }

            this->isMeetingLeader = false;
            this->wantsToMeet = false;
            this->parentId = -1;

            if (myId > 0) {
                if (myId % 2 == 0) {
                    this->parentId = (myId - 2)/2;
                } else {
                    this->parentId = (myId - 1)/2;
                }
            }

            /*
                Znajdowanie sąsiadów:
                1. Znalezienie "poziomu" (lewy koniec to suma kolejnych potęg 2 od 0 do i, prawy to suma kolejnych potęg 2 od 0 do i+1)
                2. Jeżeli myId = lewy koniec
                    2.1 leftNeighbour = -1
                    2.2 rightNeighbour = myId+1
                3. Jeżeli myId = prawy koniec
                    3.1 rightNeighbour = -1
                    3.2 leftNeighbour = myId-1
            */

            this->leftChildId = -1;
            this->rightChildId = -1;

            if (2*myId + 1 < processAmount) {
                this->leftChildId = 2*myId + 1;
            }

            if (2*myId + 2 < processAmount) {
                this->rightChildId = 2*myId + 2;
            }

            this->isDvdTaken = false;
        }

        int askNeerestAcceptator() {
            /*  
            znajdowanie najbliższego akceptora
            w górę drzewa
            MPI_SEND do parentID 
            */
           return acceptorId;
        }

        void findTeam(int myId) {
            /*
            Maksymalnie zawsze mamy w sumie 5 konspiratorów do spotkania.
            Każdy wysyła do każdego info o chęci spotkania.
            Każdy zapisuje sobie id chętnego.
            Minie określony czas na zgłaszanie chętnych.
            Po nim sprawdzamy, czy mamy przynajmniej 2 chętnych.
            Jeśli nie, sleep.
            Jeśli tak,
                rozwiązujemy problem wyboru lidera (walczącego o zasób)
                analogicznie do problemu generałów.
                Każdy proces ma skojarzonych chętnych do spotkania i tworzy dla nich tablicę (tab.size == liczba chętnych).
                Każdy proces losuje sobie liczbę, przypisuje swojej komórce tabeli i rozgłasza do pozostałych.
                Liderem jest ten, który ma największą wartość w tablicy (jak remis, to pierwszy z lewej).
            */
        }

        void waitToNextMeeting() {
            srand(time(null));
            int time = rand() % 10 + 1;
            this_thread::sleep_for(chrono::seconds(time));
            this->wantsToMeet = bool;
        }

        void meeting() {
            cout<<"meeting"<<endl;
        }

        bool acceptorFunAccept() {
            //tutaj komunikacja z innymi akceptorami 
            return true;
        }
};


int main(int argc, char **argv) {

    /*
    wczytanie pliku ze zmiennymi konfiguracyjnymi
    dane wejściowe: liczba procesów, id akceptorów, czas spotkania
    */

    int size, rank;
    MPI_Status status;
    MPI_Init(&argc, &argv);  

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    Conspirator conspirator = new Conspirator(dane_konfiguracyjne, rank);

    if (conspirator.isAcceptor) {
        while(true){
            /*
            akceptor odbiera prośbę o spotkanie
            odsyła wynik funkcji acceptorFunAccept
            */
            MPI_Recv();
            MPI_Send();
        }

    } else {
        while(true) {
            conspirator.waitToNextMeeting();
            int accId = conspirator.askNeerestAcceptator();
            MPI_Send();
            MPI_Recv();

            conspirator.findTeam(rank);
            if (conspirator.isMeetingLeader) {
                // walka o zasób, Lamport
                MPI_Send();
            } else {
                MPI_Recv();
            }

            conspirator.meeting();
        }

    }

    return 0;
}
