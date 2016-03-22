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
vector<string> dictVec;
string depressDict;
string lastStr;
string origStr, RLE, mask;

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
	string tempLine;
	stringstream ss;
	
	while(getline(origFile,line)){
		line.erase(line.find_last_not_of("\n\r") + 1);
		ss << line;
	}
	
	ss>>tempLine;
	
	/* find dictionary */
	size_t position = tempLine.find("xxxx");
	compBody = tempLine.substr(0,position);
	dictComb = tempLine.substr(position + 4);
	
	origFile.close();
	
	/* generate dictionary */
	while(true) {
		string temp;
		temp = dictComb.substr(0,32);
		dictComb = dictComb.substr(32, dictComb.length());
		dictVec.push_back(temp);
		//cout << "push: " << temp << endl;
		if (dictComb.length() == 0) break;
	}
}

string depreHelper(int depressType) {
	string temp = depressDict;
	int repeatRLE = RLEindex, intMask = 4;
	string tempMask = mask;
	int tempPos1 = position1;
	int tempPos2 = position2;

	
	switch(depressType) {
		case 0: 	/* RLE */ 				//cout << "RLE is" << repeatRLE << endl;
											//repeatRLE = stoi(RLE,nullptr,2);
											//cout << "RLE is " << repeatRLE << endl;
											do {
												temp = lastLine;
												cout<< temp <<endl;
												repeatRLE--;
											}while(repeatRLE >= 0);
											break;
		case 1: 	/* bitmask */			//cout << "bitmask" << endl;
											do {
												if(mask[tempPos1] == '1') {
													if(temp[intMask] == '1')
														temp[intMask] = '0';
													else
														temp[intMask] = '1';
												}
												intMask--;
												tempPos1++;
											}while(intMask >= 0);
											
											break;
		case 2: 	/* 1 bit mistake */		//cout << "1 bit mistake" << endl;
											if(temp[tempPos1] == '1')
												temp[tempPos1] = '0';
											else
												temp[tempPos1] = '1';
											break;
		case 3: 	/* 2 bit consecutive */	//cout << "2 bit consecutive" << endl;
											if(temp[tempPos1] == '1')
												temp[tempPos1] = '0';
											else
												temp[tempPos1] = '1';
											if(temp[tempPos1+1] == '1')
												temp[tempPos1+1] = '0';
											else
												temp[tempPos1+1] = '1';
											break;
		case 4: 	/* 2 bit anywhere */	//cout << "2 bit anywhere" << endl;
											if(temp[tempPos1] == '1')
												temp[tempPos1] = '0';
											else
												temp[tempPos1] = '1';
											if(temp[tempPos2] == '1')
												temp[tempPos2] = '0';
											else
												temp[tempPos2] = '1';
											break;
	}
	return temp;
}

void depression() {
	
	string pre = "";
	int preIndex;
	string pos1Index;
	string pos2Index;
	string dicIndex;

	string temp = "";
	string compress;
	//stringstream ss;
	
	/* get line to vector */
	while(true) {
		int rest = compBody.length();
		if(rest < 3)
			break;
		else {
			pre = compBody.substr(0,3);
			compBody = compBody.substr(3,compBody.length());	
			if(pre.compare("000") == 0) {   /* RLE */
				preIndex = strlen(compBody.c_str());
				if(preIndex < 2) break;
				else {
					RLE = compBody.substr(0,2);
					compBody = compBody.substr(2, compBody.length());
					temp = pre + RLE;
					//cout << "000 temp is " << temp << endl;
					fileKey.push_back(temp);
				}
			}
			else if(pre.compare("001") == 0) { /* bitmask */
				preIndex = compBody.length();
				if(preIndex < 3) break;
				else {
					pos1Index = compBody.substr(0,5);
					compBody = compBody.substr(5, compBody.length());
					int maskInd = strlen(compBody.c_str());
					if(maskInd < 4) break;
					else {
						mask =  compBody.substr(0,4);
						compBody = compBody.substr(4,compBody.length());
						int dicInd = compBody.length();
						if(dicInd < 3) break;
						else {
							dicIndex = compBody.substr(0,3);
							compBody = compBody.substr(3,compBody.length());
							temp = pre + pos1Index + mask + dicIndex;
						//	cout << "001 temp is " << temp << endl;
							fileKey.push_back(temp);
						}
					}
				}
			}
			else if(pre.compare("010") == 0) { /* 1 bit mistake */
				preIndex = compBody.length();
				if(preIndex < 5) break;
				else {
					pos1Index = compBody.substr(0,5);
					compBody = compBody.substr(5, compBody.length());
					int dicI = compBody.length();
					if(dicI < 3) break;
					else {
						dicIndex = compBody.substr(0,3);
						compBody = compBody.substr(3, compBody.length());
					
						temp = pre + pos1Index + dicIndex;
						//cout << "010 temp is " << temp << endl;
						fileKey.push_back(temp);	
					}
				}
			}
			else if(pre.compare("011") == 0) { /* 2bit consecutive */
				preIndex = compBody.length();
				if(preIndex < 5) break;
				else {
					pos1Index = compBody.substr(0,5);
					compBody = compBody.substr(5, compBody.length());
					int dicI = compBody.length();
					if(dicI < 3) break;
					else {
						dicIndex = compBody.substr(0,3);
						compBody = compBody.substr(3, compBody.length());
					
						temp = pre + pos1Index + dicIndex;
						//cout << "011 temp is " << temp << endl;
						fileKey.push_back(temp);	
					}	
				}
			}
			else if(pre.compare("100") == 0) { /* 2bit anywhere */
				preIndex = compBody.length();
				if(preIndex < 5) break;
				else {
					pos1Index = compBody.substr(0,5);
					compBody = compBody.substr(5, compBody.length());
					int pos2 = compBody.length();
					if(pos2 < 5) break;
					else {
						pos2Index = compBody.substr(0,5);
						compBody = compBody.substr(5, compBody.length());
						int dicI = compBody.length();
						if(dicI < 3) break;
						else {
							dicIndex = compBody.substr(0,3);
							compBody = compBody.substr(3, compBody.length());
					
							temp = pre + pos1Index + pos2Index +  dicIndex;
							//cout << "100 temp is " << temp << endl;
							fileKey.push_back(temp);
					
						}	
					}	
				}
			}
			else if(pre.compare("101") == 0) { /* dictionary */
				preIndex = compBody.length();
				if(preIndex < 3) break;
				else {
					dicIndex = compBody.substr(0,3);
					compBody = compBody.substr(3, compBody.length());
					temp = pre + dicIndex;
					//cout << "101 temp is " << temp << endl;
					fileKey.push_back(temp);
				}
			}
			else if(pre.compare("111") == 0) { /* original */
				preIndex = compBody.length();
				if(preIndex < 32) break;
				else {
					origStr = compBody.substr(0,32);
					compBody = compBody.substr(32,compBody.length());
					temp = pre + origStr;
					//cout << "111 temp is " << origStr << endl;
					fileKey.push_back(temp);
				}
			}
			else {}
		}
		
	}
	
	/* generate output */
	for(int i=0; i < fileKey.size(); i++) {
		if (fileKey.at(i).substr(0,3).compare("000") == 0 ) { 		/* RLE */
			RLEindex = bitset<2>(fileKey.at(i).substr(3,5)).to_ulong();
			//cout << "RLE is ..." << RLEindex << endl;
			compress = depreHelper(0);
			
			//cout<< compress <<endl;
		}
		else if (fileKey.at(i).substr(0,3).compare("001") == 0 ) { 	/* bitmask */
			position1 = bitset<5>(fileKey.at(i).substr(3,8)).to_ulong();
			mask = bitset<4>(fileKey.at(i).substr(3,7)).to_ulong();
			depressDict = dictVec.at(bitset<3>(fileKey.at(i).substr(7,10)).to_ulong());
			compress = depreHelper(1);
			cout<< compress <<endl;
		}
		else if (fileKey.at(i).substr(0,3).compare("010") == 0 ) {	/* 1 bit mistake */
			position1 = bitset<5>(fileKey.at(i).substr(3,8)).to_ulong();
			depressDict = dictVec.at(bitset<3>(fileKey.at(i).substr(8,11)).to_ulong());
			compress = depreHelper(2);
			//cout << "position1 is ..." << position1 << endl;
			cout<< compress <<endl;
		}
		else if(fileKey.at(i).substr(0,3).compare("011") == 0 ) {	/* 2 bit consecutive */
			position1 = bitset<5>(fileKey.at(i).substr(3,8)).to_ulong();
			depressDict = dictVec.at(bitset<3>(fileKey.at(i).substr(8,11)).to_ulong());
			compress = depreHelper(3);
			cout<< compress <<endl;
		}
		else if (fileKey.at(i).substr(0,3).compare("100") == 0 ){	/* 2 bit anywhere */
			position1 = bitset<5>(fileKey.at(i).substr(3,8)).to_ulong();
			position2 = bitset<5>(fileKey.at(i).substr(8,13)).to_ulong();
			depressDict = dictVec.at(bitset<3>(fileKey.at(i).substr(13,16)).to_ulong());
			compress = depreHelper(4);
			cout<< compress <<endl;
		}
		else if (fileKey.at(i).substr(0,3).compare("101") == 0 ) {	/* direct */
			compress = dictVec.at(bitset<3>(fileKey.at(i).substr(3,6)).to_ulong());
			cout<< compress <<endl;
		}
		else if (fileKey.at(i).substr(0,3).compare("111") == 0 ) { /* original */
			compress = fileKey.at(i).substr(3,35);
			cout << compress << endl;
		}
		else break;
		
		lastLine = compress;
	}
	
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
	
	
	
