#include <iostream>
#include <vector>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <map>
#include <set> 
#include <fstream>

using namespace std;
 
void compSetup() {
	
	ifstream origFile("original.txt");
	
	map<string,int > origMap; 	// check # of duplicates
	map<string,int>::iterator it;	// checking duplicate
	set<string> set; //used for geting duplicate orders
	string lineTmp;
	while(getline(origFile,lineTmp)){
		it = origMap.find(lineTmp);
		if (it != origMap.end()) {
			it->second++;
		}
		else {
			origMap.insert (pair<string,int>(lineTmp,1) );
		}
    }
	 /* write to file */
	ofstream outFile("cout.txt");
	// streambuf *coutbuffer = cout.rdbuf();
	// cout.rdbuf(outFile.rdbuf());
	 
	
}

void  depreSetup() {
	
}

void depression() {
	depreSetup();
	cout << "depressing " << endl;
}

void compression() {
	compSetup();
	cout << "compressing " << endl;
}

int main(int argc, char* argv[]) { 
	if(argc != 2)
		cout << "invild number of argements " << endl;
	else
	{
		if(atoi(argv[1]) == 1){
			compression();
		}
		else if(atoi(argv[1]) == 2) {
			depression();
		}
		else {
			cout << "invild argement "<< atoi(argv[1]) <<endl;
		}
	}

    

	return 0;
}
	
	
	
