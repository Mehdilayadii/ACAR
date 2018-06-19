#include <cstdlib>
#include <cstdio>
#include <cmath>

// Le point :
struct Point
{
    // Constructeur :
    Point(unsigned int n, unsigned int x, unsigned int y) : num(n), x(x), y(y)
    {
        arete[0] = NULL;
        arete[1] = NULL;
        arete[2] = NULL;
        arete[3] = NULL;
    }

    Point* arete[4];
    Point* parent;

    float heuristique;

    unsigned int num;

    unsigned int x;
    unsigned int y;

    bool verifie;
};

enum Direction
{
    DIR_DROIT,
    DIR_GAUCHE,
    DIR_DROITE,
    DIR_FIN
};

Point* graphe[16];
Direction chemin[16];
unsigned int cheminS;

void prepareCarte();
void prepareDonnees();
void libereCarte();

bool trouverChemin(Point* depart, Point* destination);
void construireChemin(Point* depart, Point* pt);

int main()
{
    unsigned int s, e;

    // On pr�pare la carte :
    prepareCarte();

    // On entre le point de d�part et d'arriv�e :
    printf("Veuillez entrer le point de depart : ");
    scanf("%u", &s);

    printf("Veuillez entrer le point d'arriv�e : ");
    scanf("%u", &e);

    // On lance la recherche de chemin :
    trouverChemin(graphe[s-1], graphe[e-1]);

    // On affiche le chemin :
    printf("\nLe chemin pour y acceder est :\n");

    for (register unsigned int i = 0; i < cheminS; i++)
    {
        switch (chemin[i])
        {
            case DIR_DROIT  : printf("%d - Droit.\n", i + 1); break;
            case DIR_GAUCHE : printf("%d - Gauche.\n", i + 1); break;
            case DIR_DROITE : printf("%d - Droite.\n", i + 1); break;
        }
    }

    // On lib�re la m�moire :
    libereCarte();

    return 0;
}

void prepareCarte()
{
    // Remplit la carte des points :
    graphe[0]  = new Point(1, 0, 0);
    graphe[1]  = new Point(2, 4, 0);
    graphe[2]  = new Point(3, 0, 3);
    graphe[3]  = new Point(4, 12, 0);
    graphe[4]  = new Point(5, 4, 3);
    graphe[5]  = new Point(6, 0, 7);
    graphe[6]  = new Point(7, 15, 0);
    graphe[7]  = new Point(8, 12, 3);
    graphe[8]  = new Point(9, 15, 7);
    graphe[9]  = new Point(10, 0, 12);
    graphe[10] = new Point(11, 17, 0);
    graphe[11] = new Point(12, 17, 7);
    graphe[12] = new Point(13, 4, 12);
    graphe[13] = new Point(14, 4, 9);
    graphe[14] = new Point(15, 15, 12);
    graphe[15] = new Point(16, 15, 9);

    // Cr�� les acc�s entre les points :
    graphe[0]->arete[0] = graphe[1];
    graphe[0]->arete[1] = graphe[2];

    graphe[1]->arete[0] = graphe[0];
    graphe[1]->arete[1] = graphe[3];
    graphe[1]->arete[2] = graphe[4];

    graphe[2]->arete[0] = graphe[0];
    graphe[2]->arete[1] = graphe[4];
    graphe[2]->arete[2] = graphe[5];

    graphe[3]->arete[0] = graphe[1];
    graphe[3]->arete[1] = graphe[6];
    graphe[3]->arete[2] = graphe[7];

    graphe[4]->arete[0] = graphe[1];
    graphe[4]->arete[1] = graphe[2];
    graphe[4]->arete[2] = graphe[7];

    graphe[5]->arete[0] = graphe[2];
    graphe[5]->arete[1] = graphe[8];
    graphe[5]->arete[2] = graphe[9];

    graphe[6]->arete[0] = graphe[3];
    graphe[6]->arete[1] = graphe[8];
    graphe[6]->arete[2] = graphe[10];

    graphe[7]->arete[0] = graphe[3];
    graphe[7]->arete[1] = graphe[4];

    graphe[8]->arete[0] = graphe[5];
    graphe[8]->arete[1] = graphe[6];
    graphe[8]->arete[2] = graphe[11];
    graphe[8]->arete[3] = graphe[15];

    graphe[9]->arete[0] = graphe[5];
    graphe[9]->arete[1] = graphe[12];

    graphe[10]->arete[0] = graphe[6];
    graphe[10]->arete[1] = graphe[11];

    graphe[11]->arete[0] = graphe[8];
    graphe[11]->arete[1] = graphe[10];
    graphe[12]->arete[2] = graphe[14];

    graphe[12]->arete[0] = graphe[9];
    graphe[12]->arete[1] = graphe[13];
    graphe[12]->arete[2] = graphe[14];

    graphe[13]->arete[0] = graphe[12];
    graphe[13]->arete[1] = graphe[15];

    graphe[14]->arete[0] = graphe[11];
    graphe[14]->arete[1] = graphe[12];

    graphe[15]->arete[0] = graphe[8];
    graphe[15]->arete[1] = graphe[13];
}

void prepareDonnees()
{
    // On vide les donn�es avant d�couverte de chemin :
    for (register unsigned int i = 0; i < 16; i++)
    {
        graphe[i]->parent = NULL;
        graphe[i]->heuristique = 0.0f;
        graphe[i]->verifie = false;
    }
}

void libereCarte()
{
    // Lib�re les graphe :
    for (register unsigned int i = 0; i < 16; i++)
        delete graphe[i];
}

bool trouverChemin(Point* depart, Point* destination)
{
    // On d�clare les variables locales :
    Point* verif[16];       // graphe � v�rifier

    Point* courant;         // Le point �tudi� en cours
    Point* accessible;      // Le point accessible par le point courant

    unsigned int taille = 1;
    unsigned int lecteur = 0;
    float distance;

    // Variables temporaires :
    bool dedans;
    unsigned int x, y;
    register unsigned int i, j;

    // On ajoute le point de d�part :
    verif[0] = depart;

    // On pr�pare les donn�es :
    prepareDonnees();

    // Tant que la liste des graphe ouvert n'est pas vide :
    while (taille > 0)
    {
        // On r�cup�re le dernier point :
        courant = verif[lecteur];
        lecteur++;

        // Si le point en cours est l'arriv�e, on arr�te la recherche :
        if (courant == destination)
        {
            construireChemin(depart, courant);
            return true;
        }

        // On calcule l'heuristique (distance entre le point courant et la destination) :
        x = (courant->x - destination->x);
        y = (courant->y - destination->y);
        distance = sqrt(x * x + y * y);

        // On teste tout les accessibles :
        for (i = 0; i < 4; i++)
        {
            // On r�cup�re le point accessible :
            accessible = courant->arete[i];

            if (accessible == NULL)
                break;

            // On v�rifie si le point a �t� v�rifi� :
            if (accessible->verifie)
                continue;

            // On compare les distances pour �viter les d�tours importants :
            x = (accessible->x - destination->x);
            y = (accessible->y - destination->y);

            if (distance <= sqrt(x * x + y * y))
                continue;

            // On configure le parent pour la reconstruction du chemin :
            accessible->parent = courant;

            // On v�rifie si le point ne se trouve pas d�j� dans la liste "ouvert" :
            dedans = false;
            for (j = 0; j < taille; j++)
            {
                if (verif[j] == accessible)
                    dedans = true;
            }

            // On ajoute le point dans la liste "ouvert" :
            if (!dedans)
            {
                verif[taille] = accessible;
                taille++;
            }
        }

        // On marque le point comme v�rifi� :
        courant->verifie = true;
    }

    return false;
}

void construireChemin(Point* start, Point* pt)
{
    // Tableau temporaire stockant le chemin dans le sens inverse :
    Point* temp[16];
    Point* a = start;         // Le point actuel
    Point* b;                 // Le point suivant
    float angleCourant = 0.0f;
    float angle;              // L'angle calcul�
    cheminS = 0;

    // On construit le chemin � partir des graphe parents :
    do
    {
        temp[cheminS] = pt;

        pt = pt->parent;
        cheminS++;
    }
    while (pt != NULL && pt != start);

    // On d�termines les mouvements simplifi�s du robot :
    for (register unsigned int i = 0; i < cheminS; i++)
    {
        // D�termination de l'angle � prendre :
        b = temp[cheminS - i - 1];
        angle = angleCourant - atan2(a->y - b->y, a->x - b->x);

        //printf("Angle : %f\n", angle);

        // On compare afin de d�terminer la direction � prendre :
        if (angle > 1.0)
            chemin[i] = DIR_GAUCHE;
        else if (angle < -1.0)
            chemin[i] = DIR_DROITE;
        else
            chemin[i] = DIR_DROIT;

        // On assigne en vue des prochains calculs :
        angleCourant = angle;
        a = b;
    }

    // On ins�re la fin du parcours si possible :
    chemin[cheminS] = DIR_FIN;
}
