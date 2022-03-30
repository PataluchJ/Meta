#include "TimeTester.h"

void TimeTester::test(Solver *solver, int n, int m, std::string filename)
{
    // n <- liczba prob na danym rozmiarze instancji
    // m <- liczba rozmiarow instancji
    std::vector<std::pair<int, double> > sizeTime; // int <- rozmiar instancji, double <- sredni czas dzialania (w sekundach)
    sizeTime.reserve(m);
    std::chrono::duration<double> elapsed;
    double timeMeasured;
    int j = 0; // rozmiar instancji
    for (int i = 0; i < m; ++i)
    {
        j += 100;
        timeMeasured = 0;
        for (int k = 0; k < n; ++k)
        {
            InstancePointer x = TSPInstance::generateAsymetricInstance(j, 42);
            std::chrono::high_resolution_clock::time_point begin = std::chrono::high_resolution_clock::now();
            solver->solve(x);
            std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
            elapsed = end - begin;
            timeMeasured += elapsed.count();
        }
        sizeTime.push_back(std::pair<int, double>(j, timeMeasured / n));
    }

    std::ofstream writeToFile;
    writeToFile.open(filename);
    for (std::vector<std::pair<int, double> >::iterator itr = sizeTime.begin(); itr != sizeTime.end(); ++itr)
        writeToFile << itr->first << ": " << itr->second << "\n";
    writeToFile.close();
}
