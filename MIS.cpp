#include <iostream>
#include <vector>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <map>
#include <set> 
#include <fstream>

using namespace std;

/* store set in decreasing order */
struct classcomp {
  bool operator() (const int& lhs, const int& rhs) const
  {return lhs>rhs;}
};

void compSetup() {
	
	ifstream origFile("original.txt");
	
	vector<string> fileKey;
	map<string,int > dupMap; 	// check # of duplicates
	map<string,int>::iterator it;	// checking duplicate
	set<int, classcomp> dupSet; //used for geting duplicate orders
	string line;
	
	/* read line by line, and store in the vector for future use, also store number of duplicate line in map, value is duplicate */ 
	while(getline(origFile,line)){
		fileKey.push_back(line);
		
		it = dupMap.find(line);
		//cout<< " key " << it->first << endl; 
		//cout<< " value " << it->second << endl; 
		if (it != dupMap.end()) {
			it->second++;
		//	cout << "increasing value" << endl;
		}
		else {
			dupMap.insert (pair<string,int>(line,1) );
		//	cout << "create a new entry " << endl;
		}
	}
	
	for(it = dupMap.begin(); it != dupMap.end(); it++)
	{
		cout << "key: " << it->first << endl;
		cout << "value " << it->second << endl; 
		dupSet.insert(it->second); 	//store repeating value in order
	} 
	
	set<int>::iterator setIt;
	for(setIt=dupSet.begin(); setIt!=dupSet.end(); ++setIt)
	{
		cout << "set is " <<  *setIt << endl;
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
	
	
	
