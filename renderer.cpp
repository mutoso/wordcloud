#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
    ofstream fifo;
    fifo.open("/tmp/wordcloudfifo");
    map<string,int> occurences;

	// parse file
	string line;
	while (getline(fifo, line))
	{
		stringstream ss(line);
		string word;
		word << ss;
		int count;
		count << ss;
		occurences[word] = count;
	}
}
