// Converter.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
using namespace std;

#include "AssetConverter.h"

int main()
{
	string path;
	cout << "Digite o caminho até o arquivo a ser convertido: " << endl;
	cin >> path;

	string pathStr = path;
	if (!pathStr.empty())
	{
		int ret;
		string error;
		ret = AssetConverter::GLConvertOBJ(pathStr, error);

		if (ret == 0)
		{
			cout << "Sucesso! " << error << endl;
		}
		else
		{
			cout << "Erro! " << error << endl;
		}
	}
	else
	{
		cout << "Erro! Caminho não especificado!" << endl;
	}

	cout << endl;
	system("pause");
    return 0;
}

