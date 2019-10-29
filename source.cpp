#include "outils.h" 
using namespace outils;

// taille du terrain 
int TX = 0;
int TY = 0;

// les deux plans en miroir 
int** PLAN;
int** MIROIR;


// les différentes valeurs possibles 
enum Types { VIDE, BOIS, FEU, CENDRE };

// pour définir un seuil statistique 
const int MAXI = 1000;
const float SEUIL = 0.6 * MAXI;

// réservé activité du feu 
int cmpt_feu = 0;
void  dimension_console(void);
void  create_matrices(void);
void destroy__matrices(void);
void  init_bois(void);
int   mise_a_feu(void);
void  propagation(void);
void  affiche_plan(void);
void  mem_affiche_plan(void);

/****************************************************************/
int main()
{
	bool fin = 0;
	int ralenti = 0;
	srand(time(NULL));
	O_ConsoleCursor(false);

	try {
		dimension_console();
		create_matrices();
	}
	catch (exception e) {
		cout << e.what() << endl;
		system("PAUSE");
		exit(EXIT_FAILURE);
	}

	printf("Appuyez sur une touche pour lancer");
	while (!fin) {
		if (_kbhit()) {
			switch (_getch()) {
			case VK_ESCAPE: fin = 1; break;
			case VK_RETURN:
				init_bois();
				cmpt_feu = mise_a_feu();
				break;
			case 'p': ralenti += 10; break;
			case 'm': ralenti -= 10; break;

			}
		}
		if (O_Top(ralenti) && cmpt_feu) {
			propagation();
			//affiche_plan(); 
			mem_affiche_plan();
		}
	}
	destroy__matrices();
	return 0;
}
/****************************************************************
dimensionner la fenêtre console
*/
void dimension_console()
{
	cout << "Entrez largeur fenetre :" << endl;
	cin >> TX;

	cout << "Entrez Hauteur fenetre :" << endl;
	cin >> TY;

	// une taille minimum  
	TX = (TX < 10) ? 10 : TX;
	TY = (TY < 10) ? 10 : TY;

	O_ConsoleResize(TX, TY);
}
/****************************************************************
initialisation du bois
*/
void create_matrices()
{
	PLAN = new int* [TY];
	MIROIR = new int* [TY];
	for (int y = 0; y < TY; y++) {
		PLAN[y] = new int[TX];
		MIROIR[y] = new int[TX];
	}
}
/****************************************************************
Destruction des deux matrices
*/
void destroy__matrices()
{
	for (int y = 0; y < TY; y++) {
		delete[] PLAN[y];
		delete[] MIROIR[y];
	}
	delete[] PLAN;
	delete[] MIROIR;
}
/****************************************************************
initialisation du bois
*/
void init_bois()
{
	int x, y;
	for (y = 0; y < TY; y++)
		for (x = 0; x < TX; x++) {
			if ((rand() % MAXI) < SEUIL)
				PLAN[y][x] = BOIS;
			else
				PLAN[y][x] = VIDE;
		}
}
/****************************************************************
initialisation du feu : la deuxième colone
*/
int mise_a_feu()
{
	int y, x = 1, cmpt = 0;

	for (y = 0; y < TY; y++)
		if (PLAN[y][x] == BOIS) {
			PLAN[y][x] = FEU;
			cmpt++;
		}
	for (int y = 0; y < TY; y++)
		memcpy(MIROIR[y], PLAN[y], sizeof(int) * TX);

	return cmpt;
}
/****************************************************************
propagation
*/
void propagation()
{
	int x, xo, xe, y, yn, ys;
	for (y = 0; y < TY; y++) {
		yn = (y - 1 + TY) % TY;      // nord 
		ys = (y + 1) % TY;         // sud 
		for (x = 0; x < TX; x++) {
			xo = (x - 1 + TX) % TX;   // ouest 
			xe = (x + 1) % TX;      // est 
			if (PLAN[y][x] == BOIS) { // si BOIS  
			   // et si FEU autour 
				if (PLAN[yn][x] == FEU || PLAN[ys][x] == FEU ||
					PLAN[y][xo] == FEU || PLAN[y][xe] == FEU) {
					MIROIR[y][x] = FEU; // passe en FEU en MIROIR 
					cmpt_feu++;
				}
			}
			// mais si FEU 
			else if (PLAN[y][x] == FEU) {
				// passe à CENDRE dans MIROIR 
				MIROIR[y][x] = CENDRE;
				cmpt_feu--;
			}
		}
	}
	// recopie MIROIR 
	for (int y = 0; y < TY; y++)
		memcpy(PLAN[y], MIROIR[y], sizeof(int) * TX);
}
/****************************************************************
affichage 1
*/
void affiche_plan()
{
	int y, x;
	for (y = 0; y < TY; y++) {
		for (x = 0; x < TX; x++) {
			switch (PLAN[y][x]) {
			case VIDE: O_Textcolor(0);      break;
			case BOIS: O_Textcolor(2 * 16);   break;
			case FEU: O_Textcolor(12 * 16);  break;
			case CENDRE: O_Textcolor(7 * 16);  break;
			}
			O_Gotoxy(x, y);
			putchar(' ');
		}
	}
}
/****************************************************************
affichage 2 rapide en mémoire
*/
void mem_affiche_plan()
{
	CHAR_INFO* dat;

	int color;
	dat = new CHAR_INFO[TX * TY];
	for (int y = 0; y < TY; y++) {
		for (int x = 0; x < TX; x++) {

			switch (PLAN[y][x]) {
			case VIDE: color = 0;      break;
			case BOIS: color = 2 * 16;  break;
			case FEU: color = 12 * 16;  break;
			case CENDRE: color = 7 * 16; break;
			}

			dat[y * TX + x].Attributes = color;
			dat[y * TX + x].Char.AsciiChar = ' ';
			//dat[y*TX + x].Char.UnicodeChar = ’ ’; 
		}
	}

	SHORT l = 0, t = 0, r = TX, b = TY;
	SMALL_RECT src = { l,t,r,b }, dest = src;
	O_Blit(dat, &src, &dest);

	delete[] dat;
}
