#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <strings.h>
#include <map>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

using namespace std;

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		cerr << "Usage: " << argv[0] << " [HOST] [PATH]" << endl;
		return 1;
	}

	string hostname(argv[1]);
	string path(argv[2]);

    // buffer for downloading web page
    char buffer[512] = {0};
    int webSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (webSocket < 0)
    {
        cerr << "Error creating socket" << endl;
        return 1;
    }

    // resolve hostname
    struct hostent* webServer = gethostbyname(hostname.c_str());
    if (!webServer)
    {
        cerr << "DNS Error: " << hostname << endl;
		return 1;
	}

	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
    bcopy((char *)webServer->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         webServer->h_length);
    serv_addr.sin_port = htons(80);

	if (connect(webSocket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
    {
        cerr << "Error: Can not connect to " << hostname << endl;
		return 1;
	}

	// send HTTP GET
	string getCommand = "GET " + path + " HTTP/1.1\r\n" +
                        "Host: " + hostname + "\r\n\r\n";
	int n = write(webSocket, getCommand.c_str(), getCommand.length());
	if (n < 0) 
    {
        cerr << "Error writing to socket" << endl;
        return 1;
    }

	n = read(webSocket, buffer, 511);
    if (n < 0) 
    {
        cerr << "Error reading socket" << endl;
        return 1;
    }

    string output(buffer);

    // strip out whitespace
    replace(output.begin(), output.end(), '\t', ' ');
    replace(output.begin(), output.end(), '\r', ' ');
    replace(output.begin(), output.end(), '\n', ' ');

    // strip out punctuation
    replace(output.begin(), output.end(), '/', ' ');
    replace(output.begin(), output.end(), '>', ' ');
    replace(output.begin(), output.end(), '<', ' ');
    replace(output.begin(), output.end(), '.', ' ');
    replace(output.begin(), output.end(), ',', ' ');
    replace(output.begin(), output.end(), ';', ' ');
    replace(output.begin(), output.end(), ':', ' ');
    replace(output.begin(), output.end(), '"', ' ');
    replace(output.begin(), output.end(), '\'', ' ');

    // count occurences
    map<string,int> occurences;
    istringstream stream(output);
    string token;  

    while (getline(stream, token, ' '))
    {
        if (token.length() != 0)
        {
            occurences[token]++;
        }
    }

    // make FIFO
    mknod("/tmp/wordcloudfifo", S_IFIFO|0666, 0);
    ofstream fifo;
    fifo.open("/tmp/wordcloudfifo");

    for (const auto &occurence : occurences)
    {
        fifo << occurence.first << "\t" << occurence.second << endl;
    }
}
