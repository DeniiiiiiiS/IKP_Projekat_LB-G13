# Uvod
- razviti servis koji vrsi dinamicko balansiranje opterecenja

- koristiti niti i semafore

- klijenti da salju genericke poruke

# Dizajn

Komunikacija izmedju klijeneta i servera se vrsi preko tcp konekcije. Klijenti salju genericke poruke u nasem projektu je to jedan integer. Dva tipa procesa/niti postoje, to su Worker i LoadBalancer. Main nit uspostavlja konekcije za klijentima i ubacuje podatke u queue. LoadBalancer vrsi dinamicko balansiranje opterecenja. Workeri uzimaju podatke iz queue i upisuju ih u jedan txt fajl.

# Strukture podataka

Implementiran je queue.

Queue sadrzi 4 int promenljive(capacitiy, head, tail, size) i niz integera za cuvane podataka.

Funkcije queuea su CreateQueue, AddElem, GetData, IsEmpty, IsFull i FreeQueue.

Queue* CreateQueue(unsigned short capacity) - povratna vrednost je kreirani queue a parametar je kapacitet samog niza podataka unutar queue.

bool AddElem(struct Queue* queue, int data) - povratna vrednost je true ako je uspesno dodat podataka a false ako nije dodao podatak, parametri su sam queue i podatak dobijen od klijenta.

int GetData(struct Queue *queue) - povratna vrednonst je podataka, parametar je sam queue.

bool IsEmpty(struct Queue* queue) - povratna vrednost je true ako je queue prazan a false ako je popunjen, parametar je sam queue.

bool IsFull(struct Queue* queue) - povratna vrednost je true ako je queue popunjen a false ako je prazan, parametar je sam queue.

void FreeQueue(struct Queue* queue) - nema povratne vrednosti, parametar je sam queue.

# Rezultati testiranja

Za sve niti i semafore se poziva CloseHandle i na kraju se poziva FreeQueue. Dinamicka memorija se upesno oslobadja na kraju programa.

# Zakljucak

Projekat je napisan u programskom jeziku C, koriscene su niti i semafori. Tcp konekcija se koristi izmedju servera i klijenata, i dinamicko balansiranje opterecenja je omoguceno.

# Potencijalna unapredjenja

Funkcije za niti su efektivno napisana medjutim semafori nisu iskorisceni na najefikasniji nacin, i postoji bolje resenje.

