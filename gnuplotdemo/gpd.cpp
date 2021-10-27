#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>


std::string gnuplotrun()
{
    FILE* fp = popen("gnuplot scritp1.plt", "r");
    std::string stdout_plot = "";
    int ch = 0;
    while ((ch = fgetc(fp)) != EOF) {
        stdout_plot = stdout_plot + (char)ch;
    }
    int status = pclose(fp);
    if (status != 0) {
        throw std::invalid_argument("error generating chart");
    }

    return stdout_plot;
}
using namespace std;

int main()
{
    ofstream myfile;
    myfile.open("chart.csv");
    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 100; j++) {
            myfile << i << " " << j << " " << (i ^ j) << endl;
        }
    }
    myfile.close();
    cout << gnuplotrun() << endl;
    return 0;
}
