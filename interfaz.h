#define WIDTH 30
#define HEIGHT 10 

int startx = 0;
int starty = 0;

typedef struct position{
  int x;
  int y;
}Posicion;

char *choices[] = { 
      "Crear juego",
      "Ver listado de juegos",
      "Salir",
      };


int n_choices = sizeof(choices) / sizeof(char *);
int position_x, position_y;
void print_menu(WINDOW *menu_win, int highlight);
void print_matrix_game(WINDOW *menu_win, int highlight_x, int highlight_y, int n, Posicion posiciones[], int limite, int a, int b);
void print_matrix_game_playing(WINDOW *menu_win, int highlight_x, int highlight_y, int n, Posicion posiciones[], int limite, int a, int b);


Posicion matrix_game_playing(int n, Posicion posiciones[], Posicion ataque, int limite, char cadena[100])
{ 
    WINDOW *menu_win;
    int highlight_y = 1;
    int highlight_x = 1;
    int choice = 0;
    int c;
    Posicion pos;

    initscr();
    if (has_colors()) start_color();
    init_pair(1, COLOR_BLACK, COLOR_YELLOW);    
    clear();
    noecho();
    cbreak(); 
    startx = position_x;
    starty = position_y;
    curs_set(0);
    menu_win = newwin(15, 72, 1, 3);
    keypad(menu_win, TRUE);
    wattron(menu_win, COLOR_PAIR(1)); 
    mvprintw(23, 2, "%s\n", cadena );
    wattroff(menu_win, COLOR_PAIR(1)); 
    //mvprintw(0, 0, "Use arrow keys to go up and down, Press enter to select a choice");
    refresh();
    print_matrix_game_playing(menu_win, highlight_x, highlight_y, n, posiciones, limite, 3, 2 );
    int flag_mat = 0;
    while(flag_mat != 1)
    { 
      c = wgetch(menu_win);
      switch(c)
      { 
        case KEY_UP:
            if(highlight_y == 1)
                highlight_y = n ; //_choices;
            else
                --highlight_y;
            break;
        case KEY_DOWN:
            if(highlight_y == n)
                highlight_y = 1;
            else 
                ++highlight_y;
            break;
        case KEY_LEFT:
            if(highlight_x == 1)
                highlight_x = n;
            else 
                --highlight_x;
            break;
        case KEY_RIGHT:
            if(highlight_x == n)
                highlight_x = 1;
            else 
                ++highlight_x;
            break;
       case 10:
            pos.x = highlight_x;
            pos.y = highlight_y;
            flag_mat = 1;        
        break;
      default:       
        refresh();
        break;
    }

    print_matrix_game_playing(menu_win, highlight_x, highlight_y, n, posiciones, limite, 3, 2);
    if(choice != 0) /* User did a choice come out of the infinite loop */
        break;
    } 
    //(23, 0, "%s\n", cadena);
    clrtoeol();
    //getch();
    refresh();
    endwin();


    return pos;
}


Posicion matrix_game(int n, Posicion posiciones[], int limite, char cadena[100])
{ 
    WINDOW *menu_win;
    int highlight_y = 1;
    int highlight_x = 1;
    int choice = 0;
    int c;
    Posicion pos;

    initscr();
    if (has_colors()) start_color();
    init_pair(1, COLOR_BLACK, COLOR_YELLOW);
    attron(COLOR_PAIR(1));  

    clear();
    noecho();
    cbreak(); 
    startx = 3;
    starty = 1;
    curs_set(0);
    menu_win = newwin(15, 72, starty, startx);
    keypad(menu_win, TRUE);
    wattron(menu_win, COLOR_PAIR(1)); 
    mvprintw(23, 2, "%s\n", cadena );
    wattroff(menu_win, COLOR_PAIR(1)); 
    //mvprintw(0, 0, "Use arrow keys to go up and down, Press enter to select a choice");
    refresh();
    print_matrix_game(menu_win, highlight_x, highlight_y, n, posiciones, limite , 3 ,2 );
    int flag_mat = 0;
    while(flag_mat != 1)
    { 
      c = wgetch(menu_win);
      switch(c)
      { 
        case KEY_UP:
            if(highlight_y == 1)
                highlight_y = n ; //_choices;
            else
                --highlight_y;
            break;
        case KEY_DOWN:
            if(highlight_y == n)
                highlight_y = 1;
            else 
                ++highlight_y;
            break;
        case KEY_LEFT:
            if(highlight_x == 1)
                highlight_x = n;
            else 
                --highlight_x;
            break;
        case KEY_RIGHT:
            if(highlight_x == n)
                highlight_x = 1;
            else 
                ++highlight_x;
            break;
       case 10:
            pos.x = highlight_x;
            pos.y = highlight_y;
            flag_mat = 1;        
        break;
      default:       
        refresh();
        break;
    }

    print_matrix_game(menu_win, highlight_x, highlight_y, n, posiciones, limite, 3, 2);
    if(choice != 0) /* User did a choice come out of the infinite loop */
        break;
    } 
    //(23, 0, "%s\n", cadena);
    clrtoeol();
    //getch();
    refresh();
    endwin();


    return pos;
}

int menu_principal()
{ 
    WINDOW *menu_win;
    int highlight = 1;
    int choice = 0;
    int c;

    initscr();
   // clear();
    
    if (has_colors()) start_color();
    init_pair(1, COLOR_BLACK, COLOR_BLUE);
    attron(COLOR_PAIR(1));  
    
    noecho();
    cbreak(); 
    
    startx = (80 - WIDTH) / 2;
    starty = (24 - HEIGHT) / 2;
    curs_set(0);
    menu_win = newwin(HEIGHT, WIDTH, starty, startx);
    keypad(menu_win, TRUE);  
    attrset(COLOR_PAIR(1));

    refresh();
    
    print_menu(menu_win, highlight);
    while(1)
    { 
      c = wgetch(menu_win);
      switch(c)
      { 
        case KEY_UP:
            if(highlight == 1)
              highlight = n_choices;
            else
              --highlight;
            break;
          case KEY_DOWN:
            if(highlight == n_choices)
              highlight = 1;
            else 
              ++highlight;
            break;
          case 10:
              choice = highlight;
            break;
          default:
              // mvprintw(24, 0, "Charcter pressed is = %3d Hopefully it can be printed as '%c'", c, c);
            refresh();
            break;
      }
      print_menu(menu_win, highlight);
      if(choice != 0) /* User did a choice come out of the infinite loop */
        break;
    } 
    
    clrtoeol();   
    attroff(COLOR_PAIR(1));

    refresh();
    endwin();
  return choice;
}

void print_menu(WINDOW *menu_win, int highlight)
{
  int x, y, i;  

  x = 2;
  y = 2;
  box(menu_win, 0, 0);
  init_pair(1, COLOR_BLUE, COLOR_BLACK);
  init_pair(2, COLOR_RED, COLOR_BLACK);
  init_pair(3, COLOR_GREEN, COLOR_BLACK);
  
  wattron(menu_win, COLOR_PAIR(2)); 
  mvwprintw(menu_win, 1, 8, "MENU PRINCIPAL");
  wattroff(menu_win, COLOR_PAIR(2));


  for(i = 0; i < n_choices; ++i)
  { 
    if(highlight == i + 1) /* High light the present choice */
    { 
      wattron(menu_win, COLOR_PAIR(1)); 
      mvwprintw(menu_win, y, x, "%s", choices[i]);
      wattroff(menu_win, COLOR_PAIR(1));
    }
    else{
      wattron(menu_win, COLOR_PAIR(3)); 
      mvwprintw(menu_win, y, x, "%s", choices[i]);       
      wattroff(menu_win, COLOR_PAIR(3));
    }
    ++y;
  }

  wrefresh(menu_win);
}

void print_matrix_game(WINDOW *menu_win, int highlight_x, int highlight_y, int n, Posicion posiciones[], int limite, int a, int b){

  int x,y,i,j;
  int k=0;
  x = a;
  y = b;

  box(menu_win, 0, 0);

  init_pair(1, COLOR_BLUE, COLOR_BLACK);
  init_pair(2, COLOR_RED, COLOR_BLACK);
  init_pair(3, COLOR_GREEN, COLOR_GREEN);
  init_pair(4, COLOR_GREEN, COLOR_BLACK);

  for(i = 0; i < n; i++)
  { 
    x = 3;
    for(j = 0; j < n ; j++){
        if((highlight_y == i + 1) && ( highlight_x == j + 1))  /* High light the present choice */
        { 
          wattron(menu_win, COLOR_PAIR(2)); 
          mvwprintw(menu_win, y, x, "[ ]");
          wattroff(menu_win, COLOR_PAIR(2));
        }
        else{
          wattron(menu_win, COLOR_PAIR(4)); 
          mvwprintw(menu_win, y, x, "[ ]");
          wattron(menu_win, COLOR_PAIR(4)); 
        }

        for(k = 0; k < limite; k++){
          if((posiciones[k].x == j + 1 ) && (posiciones[k].y == i + 1 )){
              wattron(menu_win, COLOR_PAIR(3)); 
              mvwprintw(menu_win, y, x, "[ ]");
              wattroff(menu_win, COLOR_PAIR(3));
          }
        }
        x +=3;
     }    
     y++;
  }

  int p,q;
  p = x + 3;
  q = 2;
  position_x = p;
  position_y = q;

  for(i = 0; i < n; i++)
  { 
    p = x + 3;
    for(j=0; j < n ; j++){
        wattron(menu_win, COLOR_PAIR(1));
        mvwprintw(menu_win, q, p, "[ ]");
        wattroff(menu_win, COLOR_PAIR(1));
         p +=3;
     }    
     q++;
  }  
  wrefresh(menu_win); 

}
void print_matrix_game_playing(WINDOW *menu_win, int highlight_x, int highlight_y, int n, Posicion posiciones[], int limite, int a, int b){

  int x,y,i,j;
  int k=0;
  x = a;
  y = b;

  box(menu_win, 0, 0);
  init_pair(1, COLOR_BLUE, COLOR_BLACK);
  init_pair(2, COLOR_RED, COLOR_BLACK);
  init_pair(3, COLOR_GREEN, COLOR_GREEN);
  init_pair(3, COLOR_GREEN, COLOR_BLACK);

  for(i = 0; i < n; i++)
  { 
    x = 3;
    for(j = 0; j < n ; j++){
        wattron(menu_win, COLOR_PAIR(4)); 
        mvwprintw(menu_win, y, x, "[ ]");
        wattroff(menu_win, COLOR_PAIR(4));

        for(k = 0; k < limite; k++){
          if((posiciones[k].x == j + 1 ) && (posiciones[k].y == i + 1 )){
              wattron(menu_win, COLOR_PAIR(3)); 
              mvwprintw(menu_win, y, x, "[ ]");
              wattroff(menu_win, COLOR_PAIR(3));
          }
        }
        x +=3;
     }    
     y++;
  }

  int p,q;
  p = x + 3;
  q = 2;
  position_x = p;
  position_y = q;

  for(i = 0; i < n; i++)
  { 
    p = x + 3;
    for(j=0; j < n ; j++){

        if((highlight_y == i + 1) && ( highlight_x == j + 1))  /* High light the present choice */
        { 
          wattron(menu_win, COLOR_PAIR(2)); 
          mvwprintw(menu_win, q, p, "[ ]");
          wattroff(menu_win, COLOR_PAIR(2));
        }
        else{
          wattron(menu_win, COLOR_PAIR(1)); 
          mvwprintw(menu_win, q, p, "[ ]");
          wattroff(menu_win, COLOR_PAIR(1));
        }
          p +=3;
     }    
     q++;
  }  
  wrefresh(menu_win); 

}