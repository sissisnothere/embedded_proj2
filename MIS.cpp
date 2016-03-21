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
vector<pair<string, int > > sortMap;
vector<pair<string, int > > sortMap1;

map<string,int > dictionary;	// int is for index value in the dictionary
int choice = 0; // 1.RLE 2.bitmask 3. 1 bit mismatch, 4. 2consecutive mismatch 5. 2bit mismatch 6. DirectMatch 7. Original
string lastLine;

string pre;
string format;
//string templine = "";
int position1 = -100;
int position2 = -100;
string biPos1 = "";
string biPos2 = "";
//string lastFormat = "";
int RLEindex = -1;
string lastPre = "";
string bitMask = "";

void compSetup() {
	
	ifstream origFile("original.txt");
	
	string line;
 	set<int, classcomp> dupSet; //used for geting duplicate orders
 	set<int>::iterator setIt;
	//vector<string,int>::iterator itVet;
	
	/* read line by line, and store in the vector for future use, also store number of duplicate line in map, value is duplicate */ 
	while(getline(origFile,line)){
		line.erase(line.find_last_not_of("\n\r") + 1);
		fileKey.push_back(line);
		
		it = dupMap.find(line);
		for(int i = 0; i < sortMap1.size(); i++) {
			if(sortMap1[i].first.compare(line) == 0) {
				sortMap1[i].second++;
				break;
			}
		}
 
		if (it != dupMap.end()) {
			it->second++;
		//	cout << "increasing value" << endl;
		}
		else {
			dupMap.insert (pair<string,int>(line,1) );
			sortMap1.push_back(pair<string,int>(line,1));
		//	cout << "create a new entry " << endl;
		}
	}
	
	for(it = dupMap.begin(); it != dupMap.end(); it++)
	{
		dupSet.insert(it->second); 	//store repeating value in order
	} 
	
	/* create 8 entries dictionary*/
	
	int dictSize = 0;

	
	for(setIt=dupSet.begin(); setIt!=dupSet.end() ; ++setIt)
	{
		// for(it = dupMap.begin(); it != dupMap.end() && dictSize <8; it++)
// 		{
// 			//cout << *setIt << " is " << it->second << endl;
// 			if(it->second == *setIt) {
// 				sortMap.push_back(pair<string, int>(it->first,dictSize));
// 				dictionary.insert(pair<string,int >(it->first,dictSize));
// 				cout << "pushing " << it->first << endl;
// 				dictSize++;
// 			}
// 		}
		for(int i = 0; i < sortMap1.size()&& dictSize <8;i++) { 
			if(sortMap1[i].second == *setIt) {
					sortMap.push_back(pair<string, int>(sortMap1[i].first,dictSize));
					dictionary.insert(pair<string,int >(sortMap1[i].first,dictSize));
					//cout << "pushing " << it->first << endl;
					dictSize++;
			}
		}

		if(dictSize == 8)
			break;

	}
	
	
	 /* write to file */
	ofstream outFile("cout.txt");
	// streambuf *coutbuffer = cout.rdbuf();
	// cout.rdbuf(outFile.rdbuf());
	 origFile.close();
	
}

//void bitMistake(string orgiStr, string compareStr, int &position1, int &position2, int &choice, string &bitmask) {
string bitMistake(string compareStr, string orgiStr ) {
	string temp = compareStr;
	vector<pair<char, int > > mistakes; //for store mistake index position
	string tempMask = "";
	//int totalMis = 0;
	//TODO: if 2 mistakes within 4 bits, using bitmask 010
	//TODO: if 2 mistakes is consecutive, using 011
	//TODO: if 1 mistakes, using 010
	//TODO: if more than 4 mistakes, 111
//	cout << "original string is : " << orgiStr << endl;
//	cout << "compare string is : " << compareStr << endl;
	for(int i = 0; i < 32; i++) {
		if(temp[i] != orgiStr[i]) {

			mistakes.push_back(pair<char,int >(orgiStr[i],i));
			//cout << "push: " << orgiStr[i] << '\t' << i << endl;
			if(mistakes.size() > 4){
			//	cout << "more than 4 mistakes! "  << endl;
				choice = 6;
				return temp;
				
			}
		}
		
	}
	
	int size = mistakes.size();
	cout << "mistake size is: " << size << endl;
	if (size == 0) {
		cout << "mistake size is 0 error " << endl;
		return temp;
	}
		
	if( size > 2 && mistakes.at(size-1).second - mistakes.at(0).second > 4) {
		cout << "two mistakes are larger than 4 " << endl;
		choice = 6;
		return temp;

	}
	
	if(size == 1) {
		cout << "1 bit mistake! " << endl;
		position1 = mistakes.at(0).second;
		if(temp[position1] == '0')
			temp[position1] = '1';
		else
			temp[position1] = '0';
		choice = 2;
	}
	else if(size == 2 && mistakes.at(0).second+1 == mistakes.at(1).second) {
		cout << "2 bit mistake! consecusive " << endl;
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
	else if(mistakes.at(size-1).second - mistakes.at(0).second <= 4)
	{
		cout << "bitmask mistake! " << endl;
		position1 = mistakes.at(0).second;
		bool find = false;
		//int lastPosition = position1;
		for(int i = mistakes.at(0).second; i < mistakes.at(0).second+4; i++)
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
		cout << "--------------tempMask is " << tempMask << endl;
		bitMask = tempMask;
		choice = 1;
		
		//TODO: need to fix the temp value;
		for(int i = 0 ; i < size; i++) {
			if(temp[mistakes.at(i).second] == '0')
				 temp[mistakes.at(i).second] = '1';
			else
				temp[mistakes.at(i).second] = '0';
		}
	}
	else {
		cout << "2 bit mistake! " << endl;
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

		choice = 4;
		// cout << "temp str length " << temp.length() << endl;
// 		cout << "orgi str length " << orgiStr.length() << endl;
// 		if(temp.compare(orgiStr) == 0)
// 			cout << "temp and orgiStr are the same! " << endl;
//
	}
	
	return temp;	
}

void compression() {
	
	//cout << "start compressing: " << endl;
	//vector<string>::iterator vecIt;
	stringstream ss;
	// string pre;
	// string format;
	// //string templine = "";
	// int position1 = -100;
	// int position2 = -100;
	// string biPos1 = "";
	// string biPos2 = "";
	// //string lastFormat = "";
	// int RLEindex = -1;
	// string lastPre = "";
	// string bitMask = "";
	string tempStr;
	map<string,int >::iterator it2;
	
	for(int i = 0 ; i < fileKey.size(); i++) {
		it = dictionary.find(fileKey.at(i));
		//vecIt = find(dictionary.begin()->first, dictionary.end()->first, fileKey.at(i));
		//cout << "lastLine: " << lastLine << endl;
		cout << "new line: " << fileKey.at(i) << endl;
		if(lastLine.compare(fileKey.at(i)) == 0) {	//RLE
			RLEindex++;
			choice = 0;
		}
		else if(it != dictionary.end())	//Direct Match
			choice = 5;
		else 
		{
			//TODO: find the dictionary entry we need to put it here.
			//bitMistake(fileKey.at(i), it->first, position1, position2, choice, bitMask);
			
			
			
			for(int k = 0; k < sortMap.size(); k++) {
			//	cout << "original line is " << fileKey.at(i) << " compare line is " << sortMap.at(k).first << endl;
				tempStr = bitMistake(fileKey.at(i), sortMap.at(k).first);
				//cout << "tempStr " << tempStr <<endl;
				//cout << "map is " << sortMap.at(k).first << endl;
				it2 = dictionary.find(tempStr);
				if(it2 != dictionary.end())
					break;
			
			}
			
			//cout << "finishing looping!! "<< endl;
			
		}
		cout << "********* switch choice is: " << choice << endl;
		
		switch(choice){
			case 0:		/*RLE*/ 			pre = "000";
											//format = bitset<2>(RLEindex).to_string();
											break;
			case 1: 	/*bistmask*/		if(pre.compare("000") == 0) {
												cout << "RLEindex " << bitset<2>(RLEindex).to_string() << endl;
												format = bitset<2>(RLEindex).to_string();
												ss<< pre << format;
											}
											RLEindex = -1;
											pre = "001";
											biPos1 = bitset<5>(position1).to_string();
											format = bitset<3>(it2->second).to_string();
											cout << "position1 " << biPos1 << endl;
											
											cout << "bitmask " << bitMask << endl;
											cout << "format " << format << endl;
									
											ss<< pre << biPos1 << bitMask << format;
											break;
			case 2: 	/*1 bit*/			if(pre.compare("000") == 0) {
												cout << "RLEindex " << bitset<2>(RLEindex).to_string() << endl;
												format = bitset<2>(RLEindex).to_string();
												ss<< pre << format;
											}
											RLEindex = -1;
											pre = "010";
											biPos1 = bitset<5>(position1).to_string();
											format = bitset<3>(it2->second).to_string();
											cout << "position1 " << biPos1 << endl;
											cout << "format " << format << endl;
											ss<< pre << biPos1 << format;
											break;
			case 3: 	/*2bit consecutive*/if(pre.compare("000") == 0) {
												cout << "RLEindex " << bitset<2>(RLEindex).to_string() << endl;
												format = bitset<2>(RLEindex).to_string();
												ss<< pre << format;
											}
											RLEindex = -1;
											pre = "011";
											biPos1 = bitset<5>(position1).to_string();
											format = bitset<3>(it2->second).to_string();
											cout << "position1 " << biPos1 << endl;
											cout << "format " << format << endl;
											ss<< pre << biPos1 << format;
											break;
			case 4: 	/*2bit anywhere*/	if(pre.compare("000") == 0) {
												cout << "RLEindex " << bitset<2>(RLEindex).to_string() << endl;
												format = bitset<2>(RLEindex).to_string();
												ss<< pre << format;
											}
											RLEindex = -1;
											pre = "100";
											biPos1 = bitset<5>(position1).to_string();
											biPos2 = bitset<5>(position2).to_string();
											format = bitset<3>(it2->second).to_string();
											cout << "position1 " << biPos1 << endl;
											cout << "position2 " << biPos2 << endl;
											cout << "format " << format << endl;
											ss<< pre << biPos1 << biPos2 << format;
											break;
			case 5: 	/*Direct Match*/	if(pre.compare("000") == 0) {
												cout << "RLEindex " << bitset<2>(RLEindex).to_string() << endl;
												format = bitset<2>(RLEindex).to_string();
												ss<< pre << format;
											}
											RLEindex = -1;
											pre = "101";
											format = bitset<3>(it->second).to_string();
											cout << "format " << format << endl;
											//lastFormate = pre + format;
											ss<< pre << format;
											break;
			case 6: 	/*original*/		if(pre.compare("000") == 0) {
												
												format = bitset<2>(RLEindex).to_string();
												cout << "RLEindex " << format << endl;
												ss<< pre << format;
											}
											RLEindex = -1;
											pre = "111";
											format = fileKey.at(i);
											cout << "format " << format << endl;
											ss<< pre << format;	
											break;
		}
		lastLine = fileKey.at(i);
		
	}
	
	char eachChar;
	int i=0;
	while(ss>>eachChar) {
		cout<<eachChar;
		if(i%32==31) {
			cout<<endl;
		}
		i++;
	}
	while(true) {
		cout<<'1';
		if(i%32==31) {
			cout<<endl;
			break;
		}
		i++;
	}

	cout<<"xxxx"<<endl;
	for(int i=0;i<sortMap.size();i++) {
		cout<<sortMap.at(i).first<<endl;
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
			// ifstream origFile("original.txt");
			// ofstream outFile("cout.txt");
// 			streambuf *coutbuf = cout.rdbuf();
// 			cout.rdbuf(outFile.rdbuf());
			
			compSetup();
			compression();
			
			// origFile.close();
			// outFile.close();
//  			cout.rdbuf(coutbuf); //reset to standard output again
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
	
	
	
