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

vector<string> fileKey;
map<string,int > dupMap; 	// check # of duplicates
map<string,int>::iterator it;	// checking duplicate
//vector<pair<string, int > > mapForSort;
map<string,int > dictionary;	// int is for index value in the dictionary
int choice = 0; // 1.RLE 2.bitmask 3. 1 bit mismatch, 4. 2consecutive mismatch 5. 2bit mismatch 6. DirectMatch 7. Original
string lastLine;

void compSetup() {
	
	ifstream origFile("original.txt");

 	set<int, classcomp> dupSet; //used for geting duplicate orders
 	set<int>::iterator setIt;
	
	
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
		//cout << "key: " << it->first << endl;
		//cout << "value " << it->second << endl; 
		dupSet.insert(it->second); 	//store repeating value in order
	} 
	
	/* create 8 entries dictionary*/
	
	int dictSize = 0;
	
	for(setIt=dupSet.begin(); setIt!=dupSet.end() ; ++setIt)
	{
		for(it = dupMap.begin(); it != dupMap.end() && dictSize <8; it++)
		{		
			//cout << *setIt << " is " << it->second << endl;
			if(it->second == *setIt) {
				//mapForSort.push_back(pair<string, int>(it->first,dictSize));
				dictionary.insert(pair<string,int >(it->first,dictSize));
				//cout << "pushing " << it->first << endl;
				dictSize++;
			}	
		} 
		if(dictSize == 8)
			break;
		
	}
	
	// for(int i=0;i<dictSize;i++) {
// 		cout << "mapForSort " << mapForSort.at(i).first << " and " << i << endl;
// 		dictionary.insert(pair<string,int >(mapForSort.at(i).first,i));
// 	}
//
	for(auto iterator = dictionary.begin() ; iterator != dictionary.end() ; ++iterator )
	{
		cout << iterator->first<< endl;
		cout << iterator->second << endl;
	}
	
	 /* write to file */
	ofstream outFile("cout.txt");
	// streambuf *coutbuffer = cout.rdbuf();
	// cout.rdbuf(outFile.rdbuf());
	 
	
}

void  depreSetup() {
	
}

void depression() {
	
	cout << "depressing " << endl;
}

void compression() {
	
	cout << "start compressing: " << endl;
	//vector<string>::iterator vecIt;
	stringstream ss;
	string pre;
	string format;
	string line = "";
	string lastFormat = "";
	int RLEindex = 0;
	string lastPre = "";
	
	for(int i = 0 ; i < fileKey.size(); i++) {
		it = dictionary.find(fileKey.at(i));
		//vecIt = find(dictionary.begin()->first, dictionary.end()->first, fileKey.at(i));
		if(it != dictionary.end())
			choice = 1;
		else if(lastLine.compare(fileKey.at(i))) {
			
		}
		
		switch(choice){
			case 1:		/*RLE*/ 			pre = "000";
											break;
			case 2: 	/*bistmask*/		pre = "001";
											break;
			case 3: 	/*1 bit*/			pre = "010";
											break;
			case 4: 	/*2bit consecutive*/pre = "011";
											break;
			case 5: 	/*2bit anywhere*/	pre = "100";
											
											break;
			case 6: 	/*Direct Match*/	pre = "101";
											format = bitset<3>(it->second).to_string();
											lastFormate = pre + format;
											ss<< pre << format;
											break;
			case 7: 	/*original*/		pre = "111";
											formate = fileKey.at(i)；
											lastFormate = pre + format;
											ss<< pre << format;	
											break;
		}
		lastLine = fileKey.at(i);
	}
}

int main(int argc, char* argv[]) { 
	if(argc != 2)
		cout << "invild number of argements " << endl;
	else
	{
		if(atoi(argv[1]) == 1){
			compSetup();
			compression();
		}
		else if(atoi(argv[1]) == 2) {
			depreSetup();
			depression();
		}
		else {
			cout << "invild argement "<< atoi(argv[1]) <<endl;
		}
	}
	
	return 0;
}
	
	
	
