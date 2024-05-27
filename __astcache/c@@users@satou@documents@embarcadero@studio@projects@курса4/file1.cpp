﻿#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <string>
#include <Windows.h>

using namespace std;

struct Headler {
    int id = 0;
    int PayloadSize = 0;
    int actualLength = 0;
};

void generateDataFile(const std::string& filename, int dataSize, int choice)
{
	std::ofstream outFile(filename, std::ios::binary);

	if (!outFile)
	{
		std::cerr << "Невозможно открыть файл!" << std::endl;
		return;
    }

    srand(static_cast<unsigned>(time(0)));  // Инициализация генератора случайных чисел

	if (choice == 0)

	 {
        std::vector<int> data(dataSize);
		for (int i = 0; i < dataSize; ++i)
		{
            data[i] = rand() % 100;  // Генерация случайных int данных
        }
		outFile.write(reinterpret_cast<const char*>(data.data()), dataSize * sizeof(int));}

	else if (choice == 1)
	{
        std::vector<float> data(dataSize);

		for (int i = 0; i < dataSize; ++i)
		{
            data[i] = static_cast<float>(rand()) / RAND_MAX * 100;  // Генерация случайных float данных
        }
		outFile.write(reinterpret_cast<const char*>(data.data()), dataSize * sizeof(float));}

	else if (choice == 2)
	{
        std::vector<char> data(dataSize);
		for (int i = 0; i < dataSize; ++i)
		{
            data[i] = 'A' + rand() % 26;  // Генерация случайных char данных
        }
		outFile.write(reinterpret_cast<const char*>(data.data()), dataSize * sizeof(char));
		}
    outFile.close();
}

void splitFile(const std::string& inputFilename, const std::string& outputPrefix, int clusterSize)
 {
    std::ifstream inFile(inputFilename, std::ios::binary);
	if (!inFile)
	{
		std::cerr << "Невозможно открыть файл!" << std::endl;
        return;
    }

    int id = 0;
	while (!inFile.eof())
	{
        std::vector<char> buffer(clusterSize);
        inFile.read(buffer.data(), clusterSize);
        std::streamsize bytesRead = inFile.gcount();

		if (bytesRead > 0)
		{
            Headler headler;
            headler.id = id++;
            headler.PayloadSize = clusterSize;
            headler.actualLength = static_cast<int>(bytesRead);

            std::ofstream outFile(outputPrefix + std::to_string(headler.id) + ".bin", std::ios::binary);
			if (!outFile)
			 {
				std::cerr << "Невозможно открыть файл!" << std::endl;
				return;
			}

            outFile.write(reinterpret_cast<const char*>(&headler), sizeof(Headler));
            outFile.write(buffer.data(), bytesRead);
            outFile.close();
        }
    }

	inFile.close();
}

void restoreFile(const std::string& outputFilename, const std::string& inputPrefix, int numClusters)
 {
    std::vector<int> indices(numClusters);
	for (int i = 0; i < numClusters; ++i)
	{
        indices[i] = i;
    }

    std::random_shuffle(indices.begin(), indices.end());

    std::ofstream outFile(outputFilename, std::ios::binary);
	if (!outFile)
	{
		std::cerr << "Невозможно открыть файл!" << std::endl;
        return;
    }

	for (int i = 0; i < numClusters; ++i)
	{
        std::ifstream inFile(inputPrefix + std::to_string(indices[i]) + ".bin", std::ios::binary);
		if (!inFile)
		{
            std::cerr << "Невозможно открыть файл!" << std::endl;
            return;
        }

        Headler headler;
        inFile.read(reinterpret_cast<char*>(&headler), sizeof(Headler));

        std::vector<char> buffer(headler.actualLength);
        inFile.read(buffer.data(), headler.actualLength);
        outFile.write(buffer.data(), headler.actualLength);

        inFile.close();
    }

    outFile.close();
}

int main()
{
	SetConsoleCP(1251); SetConsoleOutputCP(1251);

    std::string filename = "data.bin";
	int dataSize, choice;

    cout << "Размер: ";
    cin >> dataSize;
    cout << "Тип данных(0 - int, 1 - float, 2 - char): ";
	cin >> choice;

    int clusterSize = 20;

    int elementSize = (choice == 0) ? sizeof(int) : (choice == 1) ? sizeof(float) : sizeof(char);
    int numClusters = (dataSize * elementSize + clusterSize - 1) / clusterSize;

    generateDataFile(filename, dataSize, choice);
    splitFile(filename, "cluster_", clusterSize);
    restoreFile("restored_data.bin", "cluster_", numClusters);

    return 0;
}

