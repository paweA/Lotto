#pragma once

#include <iostream>

using namespace std;

class Connect
{
	string URL;
	string content;
	
	static int writer(char *data, size_t size, size_t nmemb, std::string *writerData);

public:
	void pobierz_dane();
	string przypisz();

	Connect(string URL) : URL(URL) {}
};

class Numer
{
	string nowe_numery_string;
	string content;
	bool czy_plus;
	int nowe_numery[6] = { 0 };
	
	int pozycja(int prev);
	void na_int_tab();

public:
	int* najnowsze_numery();
	string sprawdz_wygrana(int tab[]);

	Numer(string content,bool czy_plus,string nowe_numery_string) : content(content),czy_plus(czy_plus), nowe_numery_string(nowe_numery_string) {}
};