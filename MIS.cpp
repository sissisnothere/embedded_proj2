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
int choice = -1; // 0.RLE 1.bitmask 2. 1 bit mismatch, 3. 2consecutive mismatch 4. 2bit mismatch 5. DirectMatch 6. Original
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
string line;

/* for decompression */
string compBody;
string dictComb;

void compSetup() {
	
	ifstream origFile("original.txt");
	
	
 	set<int, classcomp> dupSet; //used for geting duplicate orders
 	set<int>::iterator setIt;
	
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
	
	 origFile.close();
	
}

string bitMistake(string compareStr, string orgiStr ) {
	string temp = compareStr;
	vector<pair<char, int > > mistakes; //for store mistake index position
	string tempMask = "";

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
	//cout << "mistake size is: " << size << endl;
	if (size == 0) {
		//cout << "mistake size is 0 error " << endl;
		return temp;
	}
		
	if( size > 2 && mistakes.at(size-1).second - mistakes.at(0).second > 4) {
		//cout << "two mistakes are larger than 4 " << endl;
		choice = 6;
		return temp;

	}
	
	if(size == 1) {
		//cout << "1 bit mistake! " << endl;
		position1 = mistakes.at(0).second;
		if(temp[position1] == '0')
			temp[position1] = '1';
		else
			temp[position1] = '0';
		choice = 2;
	}
	else if(size == 2 && mistakes.at(0).second+1 == mistakes.at(1).second) {
		//cout << "2 bit mistake! consecusive " << endl;
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
		//cout << "bitmask mistake! " << endl;
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
		//cout << "--------------tempMask is " << tempMask << endl;
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
		//cout << "2 bit mistake! " << endl;
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
	}
	
	return temp;	
}

void compression() {
	
	stringstream ss;
	string tempStr;
	map<string,int >::iterator it2;
	
	for(int i = 0 ; i < fileKey.size(); i++) {
		it = dictionary.find(fileKey.at(i));

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
		//cout << "********* switch choice is: " << choice << endl;
		
		switch(choice){
			case 0:		/*RLE*/ 			pre = "000";
											//format = bitset<2>(RLEindex).to_string();
											break;
			case 1: 	/*bistmask*/		if(pre.compare("000") == 0) {
												//cout << "RLEindex " << bitset<2>(RLEindex).to_string() << endl;
												format = bitset<2>(RLEindex).to_string();
												ss<< pre << format;
											}
											RLEindex = -1;
											pre = "001";
											biPos1 = bitset<5>(position1).to_string();
											format = bitset<3>(it2->second).to_string();
											//cout << "position1 " << biPos1 << endl;
											
											//cout << "bitmask " << bitMask << endl;
											//cout << "format " << format << endl;
									
											ss<< pre << biPos1 << bitMask << format;
											break;
			case 2: 	/*1 bit*/			if(pre.compare("000") == 0) {
												//cout << "RLEindex " << bitset<2>(RLEindex).to_string() << endl;
												format = bitset<2>(RLEindex).to_string();
												ss<< pre << format;
											}
											RLEindex = -1;
											pre = "010";
											biPos1 = bitset<5>(position1).to_string();
											format = bitset<3>(it2->second).to_string();
											//cout << "position1 " << biPos1 << endl;
											//cout << "format " << format << endl;
											ss<< pre << biPos1 << format;
											break;
			case 3: 	/*2bit consecutive*/if(pre.compare("000") == 0) {
												//cout << "RLEindex " << bitset<2>(RLEindex).to_string() << endl;
												format = bitset<2>(RLEindex).to_string();
												ss<< pre << format;
											}
											RLEindex = -1;
											pre = "011";
											biPos1 = bitset<5>(position1).to_string();
											format = bitset<3>(it2->second).to_string();
											//cout << "position1 " << biPos1 << endl;
											//cout << "format " << format << endl;
											ss<< pre << biPos1 << format;
											break;
			case 4: 	/*2bit anywhere*/	if(pre.compare("000") == 0) {
												//cout << "RLEindex " << bitset<2>(RLEindex).to_string() << endl;
												format = bitset<2>(RLEindex).to_string();
												ss<< pre << format;
											}
											RLEindex = -1;
											pre = "100";
											biPos1 = bitset<5>(position1).to_string();
											biPos2 = bitset<5>(position2).to_string();
											format = bitset<3>(it2->second).to_string();
											//cout << "position1 " << biPos1 << endl;
											//cout << "position2 " << biPos2 << endl;
											//cout << "format " << format << endl;
											ss<< pre << biPos1 << biPos2 << format;
											break;
			case 5: 	/*Direct Match*/	if(pre.compare("000") == 0) {
												//cout << "RLEindex " << bitset<2>(RLEindex).to_string() << endl;
												format = bitset<2>(RLEindex).to_string();
												ss<< pre << format;
											}
											RLEindex = -1;
											pre = "101";
											format = bitset<3>(it->second).to_string();
											//cout << "format " << format << endl;
											//lastFormate = pre + format;
											ss<< pre << format;
											break;
			case 6: 	/*original*/		if(pre.compare("000") == 0) {
												
												format = bitset<2>(RLEindex).to_string();
												//cout << "RLEindex " << format << endl;
												ss<< pre << format;
											}
											RLEindex = -1;
											pre = "111";
											format = fileKey.at(i);
											//cout << "format " << format << endl;
											ss<< pre << format;	
											break;
		}
		lastLine = fileKey.at(i);
		
	}
	
	char eachChar;
	int i = 0;
	while(ss>>eachChar) {
		cout << eachChar;
		if( i%32 ==31) {
			cout << endl;
		}
		i++;
	}
	while(true) {
		cout<<'1';
		if(i%32 == 31) {
			cout << endl;
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
	ifstream origFile("compressed.txt");
	//stringstream origSS;
	stringstream ss;
	
	while(getline(origFile,line)){
		line.erase(line.find_last_not_of("\n\r") + 1);
		ss << line;
	}
	/* find dictionary */
	size_t position = line.find("xxxx");
	compBody = line.substr(0,position);
	dictComb = line.substr(position + 4);
	
	origFile.close();
	
}

void depression() {
	
	string pre = "";
	int preIndex;
	string pos1Index;
	string pos2Index;
	string dicIndex;
	string origStr, RLE, mask;
	string temp = "";
	stringstream ss;
	/* get line to vector */
	while(true) {
		int rest = strlen(compBody.c_str());
		if(rest < 3)
			break;
		else {
			pre = compBody.substr(0,3);
			compBody = compBody.substr(3,strlen(compBody.c_str()));	
			if(pre.compare("000") != 0) {   /* RLE */
				preIndex = strlen(compBody.c_str());
				if(preIndex < 2) break;
				else {
					RLE = compBody.substr(0,2);
					compBody = compBody.substr(2,strlen(compBody.c_str()));
					temp = pre + RLE;
					cout << "temp is " << temp << endl;
					ss<<temp;
					
					//ss>>temp;
					fileKey.push_back(temp);
				}
			}
			else if(pre.compare("001") != 0) { /* bitmask */
				preIndex = strlen(compBody.c_str());
				if(preIndex < 3) break;
				else {
					pos1Index = compBody.substr(0,3);
					compBody = compBody.substr(3,strlen(compBody.c_str()));
					int maskInd = strlen(compBody.c_str());
					if(maskInd < 4) break;
					else {
						mask =  compBody.substr(0,4);
						compBody = compBody.substr(3,strlen(compBody.c_str()));
						int dicInd = strlen(compBody.c_str());
						if(dicInd < 3) break;
						else {
							dicIndex = compBody.substr(0,3);
							compBody = compBody.substr(3,strlen(compBody.c_str()));
							temp = pre + pos1Index + mask + dicIndex;
							cout << "temp is " << temp << endl;
							ss<<temp;
							fileKey.push_back(temp);
						}
					}
				}
			}
			else if(pre.compare("010") != 0) { /* 1 bit mistake */
				preIndex = strlen(compBody.c_str());
				if(preIndex < 8) break;
				else {
					pos1Index = compBody.substr(0,3);
					compBody = compBody.substr(3,strlen(compBody.c_str()));
					dicIndex = compBody.substr(0,3);
					compBody = compBody.substr(3,strlen(compBody.c_str()));
					
					temp = pre + pos1Index + dicIndex;
					cout << "temp is " << temp << endl;
					ss<<temp;
					fileKey.push_back(temp);
					
				}
			}
			else if(pre.compare("011") != 0) { /* 2bit consecutive */
				preIndex = strlen(compBody.c_str());
				if(preIndex < 8) break;
				else {
					pos1Index = compBody.substr(0,3);
					compBody = compBody.substr(3,strlen(compBody.c_str()));
					dicIndex = compBody.substr(0,3);
					compBody = compBody.substr(3,strlen(compBody.c_str()));
					
					temp = pre + pos1Index + dicIndex;
					cout << "temp is " << temp << endl;
					ss<<temp;
					fileKey.push_back(temp);
					
				}
			}
			else if(pre.compare("100") != 0) { /* 2bit anywhere */
				preIndex = strlen(compBody.c_str());
				if(preIndex < 13) break;
				else {
					pos1Index = compBody.substr(0,5);
					compBody = compBody.substr(5,strlen(compBody.c_str()));
					pos2Index = compBody.substr(0,5);
					compBody = compBody.substr(5,strlen(compBody.c_str()));
					dicIndex = compBody.substr(0,3);
					compBody = compBody.substr(3,strlen(compBody.c_str()));
					
					temp = pre + pos1Index + pos1Index +  dicIndex;
					cout << "temp is " << temp << endl;
					ss<<temp;
					fileKey.push_back(temp);
					
				}
			}
			else if(pre.compare("101") != 0) { /* dictionary */
				preIndex = strlen(compBody.c_str());
				if(preIndex < 3) break;
				else {
					dicIndex = compBody.substr(0,3);
					compBody = compBody.substr(3,strlen(compBody.c_str()));
					temp = pre + dicIndex;
					cout << "temp is " << temp << endl;
					ss<<temp;
					
					//ss>>temp;
					fileKey.push_back(temp);
				}
			}
			else if(pre.compare("111") != 0) { /* original */
				preIndex = strlen(compBody.c_str());
				if(preIndex < 32) break;
				else {
					origStr = compBody.substr(0,32);
					compBody = compBody.substr(3,strlen(compBody.c_str()));
					temp = pre + origStr;
					cout << "temp is " << origStr << endl;
					ss<<temp;
					
					//ss>>temp;
					fileKey.push_back(temp);
				}
			}
			else {}
		}
		
	}
	
	/* generate output */
}

int main(int argc, char* argv[]) { 
	if(argc != 2)
		cout << "invild number of argements " << endl;
	else
	{
		if(atoi(argv[1]) == 1){
			// ifstream origFile("original.txt");
			ofstream outFile("cout.txt");
			streambuf *coutbuf = cout.rdbuf();
			cout.rdbuf(outFile.rdbuf());
			
			compSetup();
			compression();
			
			
			outFile.close();
 			cout.rdbuf(coutbuf); //reset to standard output again
		}
		else if(atoi(argv[1]) == 2) {
			ofstream outFile("dout.txt");
			streambuf *coutbuf = cout.rdbuf();
			cout.rdbuf(outFile.rdbuf());
			
			depreSetup();
			depression();
			
			outFile.close();
 			cout.rdbuf(coutbuf); //reset to standard output again
		}
		else {
			cout << "invild argement "<< atoi(argv[1]) <<endl;
		}
	}
	
	return 0;
}
	
	
	
