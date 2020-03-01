#include "klasy.h"

#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <algorithm>
#include <iterator>
#include <locale.h>
#include <windows.h>
#include <winbase.h>

using namespace sf;

string numery(int wyniki[], const string &content, int &prev, const string &typ)
{
	int ileZN = 0;
	size_t znalezionaPozycja = content.find("resultnumber", prev);
	prev = znalezionaPozycja;
	string opis = L"Najnowsze numery wygrywajace dla " + typ + " to: ";

	for (int i = 0; i < 6; i++)
	{
		stringstream ss;
		znalezionaPozycja = content.find_first_of("123456789", znalezionaPozycja + 2);

		if (znalezionaPozycja + 1 == ' ')
			ileZN = 1;
		else
			ileZN = 2;

		ss << content.substr(znalezionaPozycja, ileZN);
		ss >> wyniki[i];
		if (i == 0)
			opis = opis + to_string(wyniki[i]);
		else
			opis = opis + " , " + to_string(wyniki[i]);
	}
	return opis;
}

void licz_zysk(const string &typ_los, int &Blotto, int &BlottoPlus, int &Bcaly, int licz_wgr)
{
	switch (licz_wgr)
	{
	case 3:
		if (typ_los == "zwykle")
			Blotto += (24);
		else
			BlottoPlus += (10);

		break;

	case 4:
		if (typ_los == "zwykle")
			Blotto += (70);
		else
			BlottoPlus += (100);

		break;

	case 5:
		if (typ_los == "zwykle")
			Blotto += (7000);
		else
			BlottoPlus += (3500);

		break;

	case 6:
		if (typ_los == "zwykle")
			Blotto += (2000000);
		else
			BlottoPlus += (1000000);

		break;

	default:

		break;
	}
}

string wypisz(int wyniki[], const string &typ_los, int &Blotto, int &BlottoPlus, int &Bcaly, bool czy_zmienic, int &licz_wgr)
{
	int tab0[] = { 4,8,15,16,23,42 };
	int tab1[] = { 1,3,8,11,6,37 };
	int tab2[] = { 6,12,15,40,34,31 };
	int tab3[] = { 33,3,13,23,25,43 };
	string lls;

	int licz_wgrtmp = 0;
	for (int i = 0; i < 4; i++)
	{
		int *losL = &*tab0;

		switch (i)
		{
		case 1:
			losL = &*tab1;

			break;
		case 2:
			losL = &*tab2;

			break;
		case 3:
			losL = &*tab3;

			break;
		default:

			break;
		}

		lls = lls + "Losowanie: " + to_string(i + 1) + ".\t Wyniki podawane dla numerow: ";
		for (int i = 0; i < 6; i++)
		{
			//cout << *losL << " ";
			lls = lls + to_string(*losL) + " ,";
			*losL++;
		}
		lls[lls.length() - 1] = ' ';
		lls = lls + "\n";

		losL = losL - 6;

		licz_wgrtmp = 0;
		for (int i = 0; i < 6; i++)
		{
			auto *it = find(wyniki, wyniki + 6, *losL++);
			if (it != wyniki + 6)
				licz_wgrtmp++;
		}

		if (czy_zmienic)
		{
			licz_zysk(typ_los, Blotto, BlottoPlus, Bcaly, licz_wgrtmp);
			if (typ_los == "zwykle")
				Blotto -= 3;
			else
				BlottoPlus -= 1;
		}

		if (licz_wgrtmp <= 2)
		{
			lls = lls + "Nic nie trafiles. Jak zwykle.\n\n";
		}
		else
		{
			lls = lls + "Trafiles " + to_string(licz_wgrtmp) + "\n\n";
		}

		if (licz_wgrtmp >= 3)
			licz_wgr = licz_wgr + licz_wgrtmp;
	}
	return lls;
}


void zapisP(int &Blotto, int &BlottoPlus, int &Bcaly, ofstream &plik2, string & data)
{
	SYSTEMTIME st;
	GetSystemTime(&st);
	data = (to_string(st.wDay) + "-" + to_string(st.wMonth) + "-" + to_string(st.wYear));

	plik2 << "Data: " << data << endl;
	plik2 << "Bilans: " << Blotto << ' ' << BlottoPlus << ' ' << Bcaly;
}

bool czytaj_numery(ifstream &plik, string &odczyt)
{
	plik >> odczyt;

	if (odczyt == ";")
		return false;

	return true;
}

bool zmien_bilans(int pobrane[], int czytane[], bool zapisN)
{
	for (int i = 0; i < 6; i++)
		if (find(czytane, czytane + 6, pobrane[i]) == (czytane + 6) || zapisN)
			return true;

	return false;
}

void zapisz_wyniki(int w[], int wPlus[], ofstream &plik21)
{
	plik21 << "Numery: ";
	for (int i = 0; i < 6; i++)
		plik21 << w[i] << " ";

	plik21 << " ; ";

	for (int i = 0; i < 6; i++)
		plik21 << wPlus[i] << " ";
}

void vec_konfiguruj(vector<Text> &vec,Font &font, int czcionka,const string &tresc)
{
		int i = vec.size() - 1;

		vec[i].setFont(font);
		vec[i].setCharacterSize(czcionka);
		vec[i].setFillColor(sf::Color::Black);
		vec[i].setString(tresc);
		vec[i].setPosition(Vector2f(1160, (50 + i * 45)));
}

int main()
{
	Connect c1("https://www.lotto.pl/lotto");
	c1.pobierz_dane();
	string content = c1.przypisz();

	string aktualna_data;
	int Blotto = 0, BlottoPlus = 0, Bcaly = 0, numeryL[6] = { 0 }, numeryPlus[6] = { 0 };

	bool zapis = false, zapisN = false;
	ifstream plik;
	plik.open("plik.txt");

	if (plik.good())
	{
		string odczyt;
		plik >> odczyt;
		if (odczyt == "Data:")
		{
			getline(plik, aktualna_data);
		}
		else
			zapis = true;

		plik >> odczyt;
		if (odczyt == "Bilans:")
		{
			plik >> Blotto >> BlottoPlus >> Bcaly;
		}
		else
			zapis = true;

		plik >> odczyt;
		if (odczyt == "Numery:")
		{
			int i = 0;
			while (czytaj_numery(plik, odczyt))
			{
				numeryL[i] = stoi(odczyt);
				i++;
			}
			i = 0;

			while (plik >> odczyt)
			{
				numeryPlus[i] = stoi(odczyt);
				i++;
			}
		}
		else
			zapisN = true;
	}
	else
		zapis = true;
	plik.close();

	if (zapis)
	{
		ofstream plik3;
		plik3.open("plik.txt");

		zapisP(Blotto, BlottoPlus, Bcaly, plik3, aktualna_data);
		plik3.close();
	}

	int wyniki[6] = { 0 };
	int wynikiPlus[6] = { 0 };
	string P, PPlus, lls, llsPlus;
	int prev = 0, prev2;
	int licz_wgrZ = 0;

	P = numery(wyniki, content, prev, "Lotto");
	bool czy_zmienic = zmien_bilans(wyniki, numeryL, zapisN);
	lls = wypisz(wyniki, "zwykle", Blotto, BlottoPlus, Bcaly, czy_zmienic, licz_wgrZ);

	int licz_wgrP = 0;

	prev2 = prev + 20;
	PPlus = numery(wynikiPlus, content, prev2, "Lotto Plus");
	czy_zmienic = zmien_bilans(wynikiPlus, numeryPlus, zapisN);
	llsPlus = wypisz(wynikiPlus, "+", Blotto, BlottoPlus, Bcaly, czy_zmienic, licz_wgrP);

	Bcaly = (BlottoPlus + Blotto);

	string zBilans = "Twoj bilans gry w lotto wynosi : " + to_string(Blotto) + "zl";
	string pBilans = "Twoj bilans gry w lotto plus wynosi : " + to_string(BlottoPlus) + "zl";
	string cBilans = "Od dnia " + aktualna_data + " \n\n\n\n " + "Calkowity bilans to: " + to_string(Bcaly) + "zl";

	string enter;
	plik.open("plik.txt");
	getline(plik, enter);
	plik.close();

	ofstream plik6;
	plik6.open("plik.txt", ios::trunc);
	plik6 << enter << endl << "Bilans: " << Blotto << ' ' << BlottoPlus << ' ' << Bcaly << endl;
	zapisz_wyniki(wyniki, wynikiPlus, plik6);

	plik6.close();

	sf::RenderWindow window(sf::VideoMode(1550, 450), "Sparwdzacz wynikow");
	window.clear(Color::White);
	window.setFramerateLimit(60);

	sf::Font font;
	if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf"))
		return 1;

	int czcionka = 16;
	Text lottoZ, lottoPlus, ls, lsPlus, cbilans2, pbilans2, zbilans2;
	Text fonty[9];

	for (int i=0;i<9;i++)
	{
		fonty[i].setFont(font);
		fonty[i].setCharacterSize(czcionka);
		fonty[i].setFillColor(sf::Color::Black);
	}

	fonty[0].setString(P);
	fonty[1].setString(PPlus);
	fonty[2].setString(lls);
	fonty[3].setString(llsPlus);
	fonty[4].setString(cBilans);
	fonty[5].setString(pBilans);
	fonty[6].setString(zBilans);
	fonty[8].setString("Podaj numery: ");

	fonty[0].move(Vector2f(5, 0));
	fonty[1].move(Vector2f(580, 0));
	fonty[2].move(Vector2f(5, 45));
	fonty[3].move(Vector2f(580, 45));
	fonty[4].move(Vector2f(460, 270));
	fonty[6].move(Vector2f(145, 300));
	fonty[5].move(Vector2f(680, 300));
	fonty[7].move(Vector2f(1295, 0));
	fonty[8].move(Vector2f(1190, 0));

	sf::Texture texture;
	if (!texture.loadFromFile("z.png", sf::IntRect(0, 0, 0, 0)))
		return 1;

	sf::Sprite sprite, sprite2;
	sprite.setTexture(texture);
	sprite2.setTexture(texture);

	sprite.setScale(sf::Vector2f(0.25f, 0.25f));

	sprite2.setOrigin({ sprite.getLocalBounds().width, 0 });
	sprite2.setScale(sf::Vector2f(-0.25f, 0.25f));
	sprite2.move(sf::Vector2f(1000, 243));
	sprite2.setColor(sf::Color(0xff, 0xff, 0xff, 60));

	sprite.move(sf::Vector2f(-33, 243));
	sprite.setColor(sf::Color(0xff, 0xff, 0xff, 60));

	sprite2.setOrigin({ sprite.getLocalBounds().width, 0 });

	sf::RectangleShape line(sf::Vector2f(450, 5));
	line.setFillColor(sf::Color::Black);
	line.move(Vector2f(1150, 0));
	line.rotate(90);

	sf::RectangleShape line2(sf::Vector2f(450, 30));
	line2.move(Vector2f(1180, 0));
	line2.rotate(90);
	
	string nowe_numery_string;
	vector<Text> vec;

	bool czy_plus = false;
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::TextEntered)
			{
				char znak;
				if ((event.text.unicode >= 48 && event.text.unicode <= 57 && nowe_numery_string.length() <= 16) || event.text.unicode == 32)
				{
				znak = event.text.unicode;
				nowe_numery_string += znak;
				}

				if (event.text.unicode == 8 && nowe_numery_string.length() >= 1)
					nowe_numery_string.erase(nowe_numery_string.length()-1);

				if (event.text.unicode == 13)
				{
					Numer n1(content, czy_plus, nowe_numery_string);
					auto tab = n1.najnowsze_numery();
					Text t;
					if (vec.size() < 9)
					{
						vec.push_back(t);

						if(czy_plus)
						vec_konfiguruj(vec, font, czcionka, ("Wyniki dla numerow z plusem : " + nowe_numery_string + "\n" + n1.sprawdz_wygrana(tab)));
						else
						vec_konfiguruj(vec, font, czcionka, ("Wyniki dla numerow : " + nowe_numery_string + "\n" + n1.sprawdz_wygrana(tab)));
					}
				}

				if (event.text.unicode == 27 && vec.size() > 0)
				{
					vec.erase(vec.begin() + vec.size() - 1);
				}

				if (event.text.unicode == 43)
					czy_plus = !czy_plus;
			}
		}

		window.clear(Color::White);

		for (int i = 0; i < 9; i++)
		{
			window.draw(fonty[i]);
		}
		fonty[7].setString(nowe_numery_string);

		if (licz_wgrZ < 3)
			window.draw(sprite);

		if (licz_wgrP < 3)
			window.draw(sprite2);

		window.draw(line);
		window.draw(line2);

		if (!vec.empty())
			for (auto& v : vec)
				window.draw(v);

		window.display();
	}
	return 0;
}