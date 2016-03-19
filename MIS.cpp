#include<iostream>
#include<vector>
#include<sstream>
#include<cstring>
#include<cstdlib> 

using namespace std;

void depression() {
	cout << "depressing " << endl;
}

void compression() {
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
	
	
	
