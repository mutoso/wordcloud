#include <iostream>
#include <pngwriter.h>
#include <ctime>
#include <map>
#include <sstream>
#include <string>
#include <fstream>

using namespace std;

int main(int argc, char* argv[])
{
    fstream fifo;
    fifo.open("/tmp/wordcloudfifo");
    map<string,int> occurences;

    // parse file
    string line;
    while (std::getline(fifo, line))
    {
        stringstream ss(line);
        string word;
        ss >> word;
        int count;
        ss >> count;
        occurences[word] = count;
    }
    
    pngwriter png(500, 500, 0, "output.png");
    srand(time(0));
    // for each occurence
    for (const auto &occurence : occurences)
    {
        int randX = rand() % 400;
        int randY = rand() % 400;
        int randAngle = rand() % 360;

        int randR = rand() % 256;
        int randG = rand() % 256;
        int randB = rand() % 256;
		char* word = (char*)calloc(1, occurence.first.length() + 1);
		strncpy(word, occurence.first.c_str(), occurence.first.length());
        png.plot_text("arial.ttf", 10*occurence.second, randX, randY, randAngle, word, randR, randG, randB);
    }
    
    png.close();

}
