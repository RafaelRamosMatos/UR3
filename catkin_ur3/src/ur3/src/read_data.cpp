#include <sstream>
#include <fstream>
#include <cstring>
#include <ctime>
#include <iostream>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
using namespace std;
float** read_data(){
    
    ifstream theFile("vel1to6_err.csv");
    string line, temp;
    int i = 0,j = 0 ;
	float** data = new float*[10000];
	for (int l = 0; l < 10000; ++l){
   		data[l] = new float[6];
	}
    while(getline(theFile, line, '\n')){
        j = 0;
        istringstream iss(line);
        while(getline(iss, temp, ',')){
            data[i][j] = std::stof(temp);
            //cout << data[i][j] << '|';
            j++;
        }
        //cout << endl;
        i++;
        
    }
    theFile.close();
	return data;	
}
 