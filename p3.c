#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<errno.h>
#include<fcntl.h>

//function gives category value of each temperature present in b array into cat
void categorise(int cat[], float b[], float mean, float std) {
	for(int i=0;i<5;i++) {
		if(b[i] == mean) 
			cat[i] = 0;
		else if(b[i] > mean+std)
			cat[i] = 1;
		else if(b[i]>mean && b[i]<mean+std)
			cat[i] =2;
		else if(b[i]<mean && b[i]>mean-std)
			cat[i] = 3;
		else
			cat[i] = 4;
	}
}

int main() {		//process P3

	//for getting the mean and standard deviation values send by process P2
	mkfifo("file1",S_IFIFO | 0777);
	//opening myfile from one end
	int f1 = open("file1",O_RDONLY);
	float arr[2];
	if(read(f1, arr, sizeof(float)*2) == -1) {
		printf("error in reading file1 by P3");
		return 2;
	}
	printf("Recieved mean and standard deviation from P2\n");
	close(f1);
	
	float mean = arr[0];
	float std = arr[1];
	
	// for getting the message from P1 which contains the temperature values
	int f2 = open("file2",O_RDONLY);
	float b[5];
	if(read(f2, b, sizeof(float)*5) == -1) {
		printf("error in reading file1 by P3");
		return 1;
	}
	printf("Recieved temperature values from P1\n");
	close(f2);
	
	int cat[5];
	
	//categorise function gives the appropriate category number for T1,T2,T3,T4,T5 into cat array
	categorise(cat,b,mean,std);
	
	//writing cat values to fifo file2 which will read by process P1
	f2 = open("file2",O_WRONLY);
	if(write(f2,cat,sizeof(int)*5) == -1 ) {
		printf("writing value to file2 by P3 failed");
		return 2;
	}
	printf("sended category values of temperature to P1");
	close(f2);
	
	return 0;
}
