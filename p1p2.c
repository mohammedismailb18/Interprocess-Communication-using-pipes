#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<errno.h>
#include<fcntl.h>
#include<math.h>

//function for calculating average
float average(float A[]) {
			float sum=0;
			for(int i=0;i<5;i++) 
				sum+=A[i];
			return sum/5;
}

//function for calculating standard deviation
float fun_std(float A[],float mean) {
			float sum=0;
			for(int i=0;i<5;i++)
				sum += pow((A[i] - mean),2);
			
			return sqrt(sum/5);	
}

//function for revising temperature based on category values
void revising(float arr[], int cat[]) {
	for(int i=0;i<5;i++) {
		if(cat[i] == 0)
			continue;
		else if(cat[i] == 1)
			arr[i] -= 3;
		else if(cat[i] == 2)
			arr[i] -= 1.5;
		else if(cat[i] == 3)
			arr[i] += 2;
		else
			arr[i] += 2.5;
	}
	printf("Revised temperatures : \n");
	for(int i=0;i<5;i++) 
		printf("T%d = %f\n",i+1,arr[i]);
}

int main() {

	//file descriptor for pipe
	int fd[2];
	
	//creating pipe
	if(pipe(fd) == -1) {
		printf("pipe creation failed\n");
		return 1;
	}
	
	pid_t p = fork();
	
	if(p!=0) { 			//Parent process P1
		float arr[5];
		//taking user input for temperature
		for(int i=0;i<5;i++) {
			while(1) {
			printf("T%d = ",i+1);
			scanf("%f",&arr[i]);
			if(arr[i]<15 || arr[i]>45)
				printf("enter values in the range of 15 to 45\n");
			else
				break;
			}
		}
		
		//for writing temperature values present in arr to pipe which is taken by process P2
		close(fd[0]);
		if(write(fd[1], arr, sizeof(float)*5) == -1) {
			printf("In process 1, writing to pipe failed\n");
			return 1;
		}
		close(fd[1]);	
		
		//for writing temp values to file2 which will be read by process P3
		//creating fifo file2
		mkfifo("file2",S_IFIFO | 0777);
		// opening fifo file2
		int file2_d = open("file2",O_WRONLY);
		if(write(file2_d,arr,sizeof(float)*5) == -1) {
			printf("In process P1, writing to file2 failed");
			return 2;
		}
		close(file2_d);
		
		
		
		// opening fifo file2 for reading cat values given by process P3
		int cat[5];
	        file2_d = open("file2",O_RDONLY);
		if(read(file2_d, cat, sizeof(int)*5) == -1) {
			printf("In process P1, reading cat values from file2 failed");
			return 3;
		}
		close(file2_d);
		
		//revise the temperature based on the cat values got from process P3 and prints temp.
		revising(arr,cat);
		
	}
	else { 			// Child process P2
		
		// for taking the message(tempeature  values) sended by Process P1 through unnamed pipe
		close(fd[1]);
		float arr[5];		
		if(read(fd[0], arr, sizeof(float)*5) == -1) {
			printf("In process P2, reading pipe failed");
			return 4;
		}
		close(fd[1]);
		
		// Calculating mean and standard deviation of temperature got from P1
		float mean = average(arr);
		float std = fun_std(arr,mean);
		
		arr[0] = mean;
		arr[1] = std;		
				
		//for sending message(mean and standard deviation) to proces P3		
		//creating fifo file
		mkfifo("file1",S_IFIFO | 0777);
		//opening the file
		int mf = open("file1",O_WRONLY);
		//writing only mean and standard deviation values to fifo file1
		if(write(mf, arr, sizeof(float)*2) == -1) {
			printf("In process P2,writing to file1 failed");
			return 5;
		}
		close(mf);
		
	}
	return 0;
}

