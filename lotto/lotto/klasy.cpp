#include "klasy.h"

#include <curl/curl.h>
#include <algorithm>
#include <sstream>

void Connect::pobierz_dane()
{
	const char *cstr = URL.c_str();

	curl_global_init(CURL_GLOBAL_ALL);
	CURL *handle;

	handle = curl_easy_init();
	curl_easy_setopt(handle, CURLOPT_URL, cstr);
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, &content);
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, writer);
	CURLcode code = curl_easy_perform(handle);

	curl_easy_cleanup(handle);
	curl_global_cleanup();
}

string Connect::przypisz()
{
	return content;
}

int Connect::writer(char *data, size_t size, size_t nmemb, std::string *writerData)
{
	if (writerData == NULL)
		return 0;

	writerData->append(data, size*nmemb);
	return size * nmemb;
}

int Numer::pozycja(int prev)
{
	size_t znalezionaPozycja = content.find("resultnumber", prev);
	prev = znalezionaPozycja;

	return prev;
}

int* Numer::najnowsze_numery()
{
	int ileZN = 0;
	size_t znalezionaPozycja = pozycja(0);

	if (czy_plus == true)
		znalezionaPozycja = pozycja(znalezionaPozycja + 20);
	
	int *wyniki = new int[6];
	
	for (int i = 0; i < 6; i++)
	{
		stringstream ss;
		znalezionaPozycja = content.find_first_of("123456789", znalezionaPozycja + 2);

			ileZN = 2;

		ss << content.substr(znalezionaPozycja, ileZN);
		ss >> wyniki[i];
	}
	return wyniki;
}


void Numer::na_int_tab()
{
	stringstream ss;
	ss << nowe_numery_string;

	for (int i = 0; i < 6; i++)
		ss >> nowe_numery[i];
}

string Numer::sprawdz_wygrana(int tab[])
{
	string wynik;
	int licz_wgrtmp = 0;

	na_int_tab();

	for (int i = 0; i < 6; i++)
	{
		auto *it = find(nowe_numery, nowe_numery + 6, *tab++);
		if (it != nowe_numery + 6)
			licz_wgrtmp++;
	}

	if (licz_wgrtmp <= 2)
		wynik = wynik + "Nic nie trafiles. Jak zwykle.\n\n";
	else
		wynik = wynik + "Trafiles " + to_string(licz_wgrtmp) + "\n\n";

	return wynik;
}