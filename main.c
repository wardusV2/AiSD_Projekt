#include <stdio.h>
#include <stdlib.h>
#include<time.h>
#include <stdbool.h>
#include<allegro5/allegro.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>

///Wysokosc okna
#define wys 1400
///Szerokosc okna
#define szer 1000

    ///@brief Funkcja wpisuje liczby pseudolosowe do tablicy
    ///@param int n - liczba elemetow w tablicy
    ///@param int *tab - wskznik na tablice
    ///@return funkcja nie zwraca wartosci

void wpisz(int n,int *tab)
{
    srand(time(NULL));
  int i;
    for(i=0;i<n;i++)
    {
        tab[i]=0+rand()%n;
    }
}
    ///Funkcja wyswietla slupki o wysokosci zaleznej od wartosci liczby umieszczonej w tablicy
    ///@param int n - liczba elemetow w tablicy
    ///@param int *tab - wskznik na tablice
    ///@return funkcja nie zwraca wartosci

void wysw(int n, int *tab)
{
   int i;
  int odst=wys/n;
  int start=odst/2;
  int wysokosc=szer/n;
   if ((n<=wys))
    {
        //Czyscimy ekran kolorem
        al_clear_to_color(al_map_rgb(0, 0, 0));
        for (i=0;i<n; i++)
        {
            al_draw_rectangle(i * odst + start, szer, i * odst + start, szer - tab[i] * wysokosc, al_map_rgb(150, 20, 150), odst- 2);
        }
        // a nastêpnie wyswietlamy zmiany
        al_flip_display();
     }
         else
      {
        ALLEGRO_FONT* font8 = al_create_builtin_font();
        al_draw_text(font8,al_map_rgb(255, 255, 255), wys/ 2, szer / 2, ALLEGRO_ALIGN_CENTER, "ZBYT DUZO ELEMENTOW ABY WYSWIETLIC GRAFICZNIE!");
        al_destroy_font(font8);
        al_flip_display();
      }
}
    // brief
   /// Funkcja wypisuje zawartosc tablicy na ekran
   /// @param int n - liczba elemetow w tablicy
   /// @param int *tab - wskznik na tablice
   ///@return funkcja nie zwraca wartosci

void wypisz(int n,int *tab)
{
     int i;
   for(i=0;i<n;i++)
    {
    printf(" %d ",tab[i]);
    }
}

    ///Funkcja wykonuje sortowanie tablicy poprzez proste wstawianie
    /// @param int n - liczba elemetow w tablicy
    /// @param int *tab - wskznik na tablice
    /// @return funkcja nie zwraca wartosci

void sort_przez_proste_wstawienie(int n,int *tab)
{
   int i,j,t;
    for (i=1;i<n; ++i)
    {
        j=i;
        while((j>0)&&(tab[j-1]>tab[j]))
        {
            t=tab[j];
            tab[j]=tab[j-1];
            tab[j-1]=t;
            --j;
            wysw(n,tab);//
        }
    }
}
    ///Funkcja wykonuje sortowanie tablicy poprzes wstawianie polowkowe
    /// @param int n - liczba elemetow w tablicy
    /// @param int *tab - wskznik na tablice
    /// @return funkcja nie zwraca wartosci
void sort_przez_wstawianie_polowkole(int n,int *tab)
{
    int i,j,x,first,last;
   for (j=n-2;j>=0;j--)
       {
        x=tab[j];
        first=j;
        last=n;

    while(last-first>1)
        {
            i=(first+last)/2;
            if (x<=tab[i])
            {
            last=i;
            }
            else
            {
            first=i;

            }
        }
        for(i=j;i<first;i++)
        {
          tab[i]=tab[i+1];
           wysw(n,tab);//
        }
          tab[first]=x;
        }
}
    ///Funkcja
    /// @param ALLEGRO_DISPLAY **okno
    /// @return funkcja nie zwraca wartosci
void stop(ALLEGRO_DISPLAY **okno)
{
    ALLEGRO_FONT* font8 = al_create_builtin_font();//utworzenie czcionki do napisu
    bool wait = false;
    al_draw_text(font8, al_map_rgb(255, 255, 255), wys / 2, szer / 2 - 100, ALLEGRO_ALIGN_CENTER, "NACISNIJ ESC ABY ZAKONCZYC!");//Stworzenie i umieszczenie napisu(kolor napisu : bia³y) w odpowiedznim miejscu
        al_flip_display();
        al_install_keyboard();
        ALLEGRO_EVENT czekanie;
        ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();
        al_register_event_source(event_queue, al_get_keyboard_event_source());
        al_wait_for_event(event_queue, &czekanie);
          if (czekanie.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            switch (czekanie.keyboard.keycode)
            {
            case ALLEGRO_KEY_ESCAPE:
                wait = true;
                al_destroy_font(font8);//Usuniêcie napisu z pamiêci
                al_destroy_display(*okno);//zakoñczenie wyœwietlania okna
                break;
            }
        }
}
///Glowne cialo programu
int main()
{
    //Inicjalizacja biblioteki
      al_init();
    al_set_new_display_flags(ALLEGRO_WINDOWED);
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();
    //Zmienna do odebrania wskaŸnika ekranu(zmienna *okno)
    //Tworzymy okno, odbieramy wskaŸnik,
    ALLEGRO_DISPLAY *okno = al_create_display(wys,szer);
    al_set_window_position(okno, wys/6, 10);
    al_clear_to_color(al_map_rgb(0, 0, 0));// Ustawienie koloru t³a
    al_flip_display();

    ////////////////////////////
    FILE *plik;
    plik=fopen("dane.txt","w");
    if(plik==NULL)
    {
        printf("Blad otwarcia pliku");
        return 0;
    }
    int n,k;
    int *tab=NULL;
    int *tablica=NULL;
        clock_t start;
        clock_t end;
        clock_t start2;
        clock_t end2;
    printf("Podaj ilosc danych do zapisani w tablicy\n");
    scanf("%d",&n);
    tab=malloc(n*sizeof(int));
    tablica=malloc(n*sizeof(int));
        wpisz(n,tab);
        wpisz(n,tablica);
        wypisz(n,tab);
    printf("\n___________________________\n");
    puts("");
        wypisz(n,tablica);
    puts("");

   while(1)
   {
    printf("Wybierz Dzialanie do wykonania :\n");
    printf("1-Sortowanie przez proste wstawianie\n");
    printf("2-Sortowanie przez wstawianie polowkowe\n");
    printf("3-zakoncz dzialanie programu\n");
    printf("Wybierz czynnosc : ");
    scanf("%d",&k);
    switch(k)
    {
    case 1:
        start=clock();
        sort_przez_proste_wstawienie(n,tab);
        end=clock();
        //wypisz(n,tab);
        wysw(n,tab);
        puts("");
        printf("Czas sortowania przez proste wstawianie to %lf sekund\n ",(double)(end-start)/CLOCKS_PER_SEC);
        fprintf(plik,"Czas sortowania przez proste wstawianie dla tablicy |%d| elementow to |%lf| sekund\n",n,(double)(end-start)/CLOCKS_PER_SEC);
    break;
    case 2:
        start2=clock();
        sort_przez_wstawianie_polowkole(n,tablica);
        end2=clock();
        //wypisz(n,tablica);
        puts("");
        printf("Czas sortowania przez wstawianie polowkowe to %lf sekund\n ",(double)(end2-start2)/CLOCKS_PER_SEC);
        fprintf(plik,"Czas sortowania przez wstawianie polowkowe dla tablicy |%d| elementow to |%lf| sekund\n",n,(double)(end2-start2)/CLOCKS_PER_SEC);
    break;
    case 3:
           free(tab);
           free(tablica);
           tab=NULL;
           tablica=NULL;
            if(fclose(plik))
            {
                printf("blad zamkniecia pliku");
                return 0;
            }
            stop(&okno);
         return 0;
         break;
    }

   }
    free(tab);//zwolnienie zaalokowanej pamieci
    free(tablica);//zwolnienie zaalokowanej pamieci
    tab=NULL;
    tablica=NULL;
    return 0;
}

