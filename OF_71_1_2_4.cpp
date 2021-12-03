#include <iostream>
#include <map>
#include <string>
#include <random>
#include <fstream>

using namespace std;

// podane w tre�ci zadania czasy po�owicznego rozpadu w minutach
// 214Po rozpada si� natychmiastowo
// 210Pb i 207Pb to izotopy trwa�e
map<string, double> T12 = {
	{ "218Po", 3.05 },
	{ "214Pb", 26.8 },
	{ "214Bi", 19.7 },
	{ "210Pb", (double)INFINITY},
	{ "211Pb", 36.1 },
	{ "211Bi", 2.14 },
	{ "207Tl", 4.77 },
	{ "207Pb", (double)INFINITY}
};

// czas, po kt�rym prawdopodobie�stwo rozpadu izotopu iso wynosi p
double t(double p, string iso)
{
	return -T12[iso] * log2((double)1 - p);
}

const int N0 = 100; // pocz�tkowa liczba atom�w w s�oiku
const double dt = 0.1; // krok czasowy w minutach

void write(float m[N0][4], string filename) // zapisanie wynik�w do pliku
{
	// stworzenie pliku
	ofstream file;
	file.open(filename);

	// deklaracja zmiennych
	double time = 0;
	int decayed;

	// wiersze b�d� r�ne w zale�no�ci od tego, czy nazwa pliku zawiera rad (czyli czy zapisujemy szereg uranowo-radowy) czy nie (szereg uranowo-aktynowy)
	bool ura = filename.find("rad") != string::npos;
	file << (ura ? "t 218Po 214Pb 214Bi 214Po 210Pb\n" : "t 211Pb 211Bi 207Tl 207Pb\n"); 

	do // co krok czasowy r�wny dt liczymy liczb� j�der ka�dego izotopu w s�oiku
	{
		// deklaracji zmiennych ci�g dalszy
		string t_str = to_string(time);
		t_str = t_str.substr(0, t_str.find('.') + 2);
		int is[4] = { 0, 0, 0, 0 };
		decayed = 0;

		// dla ka�dego atomu w s�oiku determinujemy, jakim izotopem b�dzie w okre�lonej chwili 
		// je�li czas od pocz�tku symulacji jest d�u�szy ni� czas rozpadu wygenerowany dla pierwszego j�dra na pozycji i, to znaczy �e zaszed� jeden rozpad
		// je�li czas od pocz�tku symulacji jest d�u�szy ni� suma czas�w rozpadu wygenerowanych dla pierwszego i drugiego j�dra na pozycji i to znaczy, �e zasz�y dwa rozpady
		// i tak dalej a� do osi�gni�cia izotopu trwa�ego
		for (int i = 0; i < N0; i++) 
		{
			if (time < m[i][0])
			{
				is[0]++;
			}
			else if (time < m[i][0] + m[i][1])
			{
				is[1]++;
			}
			else if (time < m[i][0] + m[i][1] + m[i][2])
			{
				is[2]++;
			}
			else
			{
				is[3]++;
				decayed++;
			}
		}
		
		file << t_str << " " << to_string(is[0]) << " " << to_string(is[1]) << " " << to_string(is[2]) << (ura ? " 0 " : " ") << to_string(is[3]) << "\n"; // zapisanie wiersza do pliku

		time += dt; // zwi�kszenie czasu od pocz�tku sprawdzania wynik�w symulacji o dt
	} while (decayed < N0); // je�li wszystkie j�dra s� izotopami trwa�ymi, ko�czymy wykonywanie p�tli

	file.close(); // zapisanie pliku
}

int main() // symulacje
{
	// generator liczb losowych
	random_device rd;
	mt19937 mt(rd());
	uniform_real_distribution<> rng(0.0, 1.0);

	for (int i = 0; i < 5; i++) // symulacje szeregu uranowo-radowego
	{
		float results[N0][4] = {}; // tablica wynik�w
		for (int i = 0; i < N0; i++) // na ka�d� pozycj� tablicy wynik�w wpisujemy tablic� czas�w rozpadu dla trzech pierwszych j�der szeregu
		{
			results[i][0] = t(rng(mt), "218Po");
			results[i][1] = t(rng(mt), "214Pb");
			results[i][2] = t(rng(mt), "214Bi");
			// 214Po pomijamy, poniewa� przyjmujemy, �e ten izotop rozpada si� natychmiast
			results[i][3] = (double)INFINITY;
		}
		write(results, "uran_rad_" + to_string(i + 1) + ".txt"); // zapisanie wynik�w symulacji do pliku
	}
	for (int i = 0; i < 5; i++) // symulacje szeregu uranowo-aktynowego
	{
		float results[N0][4] = {}; // tablica wynik�w
		for (int i = 0; i < N0; i++) // na ka�d� pozycj� tablicy wynik�w wpisujemy tablic� czas�w rozpadu dla trzech pierwszych j�der szeregu
		{
			results[i][0] = t(rng(mt), "211Pb");
			results[i][1] = t(rng(mt), "211Bi");
			results[i][2] = t(rng(mt), "207Tl");
			results[i][3] = (double)INFINITY;
		}
		write(results, "uran_aktyn_" + to_string(i + 1) + ".txt"); // zapisanie wynik�w symulacji do pliku
	}

	return 0;
}
