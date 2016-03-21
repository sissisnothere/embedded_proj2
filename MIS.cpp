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
	string line;
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

string bitMistake(string orgiStr, string compareStr, int &position1, int &position2, int &choice, string &bitmask) {
	string temp = compareStr;
	vector<pair<char, int > > mistakes; //for store mistake index position
	string tempMask = "";
	//int totalMis = 0;
	//TODO: if 2 mistakes within 4 bits, using bitmask 010
	//TODO: if 2 mistakes is consecutive, using 011
	//TODO: if 1 mistakes, using 010
	//TODO: if more than 4 mistakes, 111
	for(int i = 0; i < 32; i++) {
		if(temp[i] == orgiStr[i]) {

			mistakes.push_back(pair<char,int >(orgiStr[i],i));
			if(mistakes.size() > 4){
				choice = 6;
				return temp;
			}
		}
		
	}
	
	int size = mistakes.size();
	if( mistakes.at(size-1).second - mistakes.at(0).second > 4) {
		choice = 6;
		return temp;
	}	
	
	if(size == 1) {
		position1 = mistakes.at(0).second;
		if(temp[position1] == '0')
			temp[position1] = '1';
		else
			temp[position1] = '0';
		choice = 2;
	}
	else if(size == 2 && position1+1 == position2) {
		position1 = mistakes.at(0).second;
		position2 = mistakes.at(1).second;
		
		if(temp[position1] == '0')
			temp[position1] = '1';
		else
			temp[position1] = '0';
		if(temp[position2] == '0')
			temp[position2] = '1';
		else
			temp[position2] = '0';

		choice = 3;
		
	}
	else
	{
		//tempMask += "1";
		position1 = mistakes.at(0).second;
		bool find = false;
		//int lastPosition = position1;
		for(int i = mistakes.at(0).second; i < mistakes.at(size-1).second; i++)
		{
			for(int j = 0; j < size; j++) {
				if(mistakes.at(j).second == i) {
					find = true;
					break;
				}
				else {
					find = false;
				}
			}
			if(find)
				tempMask += "1";
			else
				tempMask += "0";
		}
		
		bitmask = tempMask;
		choice = 1;
	}
	return temp;	
}

void compression() {
	
	cout << "start compressing: " << endl;
	//vector<string>::iterator vecIt;
	stringstream ss;
	string pre;
	string format;
	string templine = "";
	int position1 = -100;
	int position2 = -100;
	//string lastFormat = "";
	int RLEindex = -1;
	string lastPre = "";
	string bitMask = "";
	
	for(int i = 0 ; i < fileKey.size(); i++) {
		it = dictionary.find(fileKey.at(i));
		//vecIt = find(dictionary.begin()->first, dictionary.end()->first, fileKey.at(i));
		if(it != dictionary.end())	//Direct Match
			choice = 5;
		else if(lastLine.compare(fileKey.at(i)) == 0) {	//RLE
			RLEindex++;
		}
		else 
		{
			templine = bitMistake(fileKey.at(i), it->first, position1, position2, choice, bitMask);
			
		}
		
		switch(choice){
			case 0:		/*RLE*/ 			pre = "000";
											//format = bitset<2>(RLEindex).to_string();
											break;
			case 1: 	/*bistmask*/		if(pre.compare("000") == 0)
												ss<< pre << RLEindex;
											RLEindex = -1;
											pre = "001";
											
											break;
			case 2: 	/*1 bit*/			if(pre.compare("000") == 0)
												ss<< pre << RLEindex;
											RLEindex = -1;
											pre = "010";
											break;
			case 3: 	/*2bit consecutive*/if(pre.compare("000") == 0)
												ss<< pre << RLEindex;
											RLEindex = -1;
											pre = "011";
											break;
			case 4: 	/*2bit anywhere*/	
											if(pre.compare("000") == 0)
												ss<< pre << RLEindex;
											RLEindex = -1;
											pre = "100";
											break;
			case 5: 	/*Direct Match*/	if(pre.compare("000") == 0)
												ss<< pre << RLEindex;
											RLEindex = -1;
											pre = "101";
											format = bitset<3>(it->second).to_string();
											//lastFormate = pre + format;
											ss<< pre << format;
											break;
			case 6: 	/*original*/		if(pre.compare("000") == 0)
												ss<< pre << RLEindex;
											RLEindex = -1;
											pre = "111";
											format = fileKey.at(i);
											//lastFormate = pre + format;
											ss<< pre << format;	
											break;
		}
		lastLine = fileKey.at(i);
	}
}

void  depreSetup() {
	
}

void depression() {
	
	cout << "depressing " << endl;
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
	
	
	
