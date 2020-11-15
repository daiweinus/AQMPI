#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <mpi.h>


unsigned long long size;
int slaves;
int myid;

long long comm_time = 0;
long long comp_time = 0;

#define MASTER_ID slaves

/** 
 * Determines the current time
 *
 **/
long long wall_clock_time()
{
#ifdef LINUX
	struct timespec tp;
	clock_gettime(CLOCK_REALTIME, &tp);
	return (long long)(tp.tv_nsec + (long long)tp.tv_sec * 1000000000ll);
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (long long)(tv.tv_usec * 1000 + (long long)tv.tv_sec * 1000000000ll);
#endif
}

unsigned long long power(int b, int p){
    unsigned long long result=1;
    for(;p>0;p--){
        result*=b;
    }
    return result;
}

int chessIndex(int row, int col, int n){
    return row*n+col;
}

void printBoard(int n, bool chessBoard[n*n]){
    int i,j;
    for(i=0;i<n;i++){
        for(j=0;j<n;j++){
            printf("%d ",chessBoard[chessIndex(i,j,n)]);
        }
        printf("\n");
    }
}

int nPieceWrap(int n, bool chessBoard[n*n], int row, int col, int w){
    int result=0,j;    

    bool resultBoard[n*n];
    memset(resultBoard, 0, n*n*sizeof(bool));
    for(j=1;j<n;j++){
        //same row on the right
        if((col+j<n)&&(chessBoard[chessIndex(row,col+j,n)])){
            if(!resultBoard[chessIndex(row,col+j,n)]){
                resultBoard[chessIndex(row,col+j,n)]=true;
                result++;
                break;
            }
        }else if((col+j>=n)&&chessBoard[chessIndex(row,col+j-n,n)]&&w){
            if(!resultBoard[chessIndex(row,col+j-n,n)]){
                resultBoard[chessIndex(row,col+j-n,n)]=true;
                result++;
                break;
            }
        }
    }
    for(j=1;j<n;j++){
        //same row on the left
        if((col-j>=0)&&(chessBoard[chessIndex(row,col-j,n)])){
            if(!resultBoard[chessIndex(row,col-j,n)]){
                resultBoard[chessIndex(row,col-j,n)]=true;
                result++;
                break;
            }
        }else if((col-j<0)&&chessBoard[chessIndex(row,col-j+n,n)]&&w){
            if(!resultBoard[chessIndex(row,col-j+n,n)]){
                resultBoard[chessIndex(row,col-j+n,n)]=true;
                result++;
                break;
            }
        }
    }
    for(j=1;j<n;j++){
        //same col on top
        if((row-j>=0)&&(chessBoard[chessIndex(row-j,col,n)])){
            if(!resultBoard[chessIndex(row-j,col,n)]){
                resultBoard[chessIndex(row-j,col,n)]=true;
                result++;
                break;
            }
        }else if((row-j<0)&&chessBoard[chessIndex(row-j+n,col,n)]&&w){
            if(!resultBoard[chessIndex(row-j+n,col,n)]){
                resultBoard[chessIndex(row-j+n,col,n)]=true;
                result++;
                break;
            }
        }
    }
    for(j=1;j<n;j++){
        //same col on below
        if((row+j<n)&&(chessBoard[chessIndex(row+j,col,n)])){
            if(!resultBoard[chessIndex(row+j,col,n)]){
                resultBoard[chessIndex(row+j,col,n)]=true;
                result++;
                break;
            }
        }else if((row+j>=n)&&chessBoard[chessIndex(row+j-n,col,n)]&&w){
            if(!resultBoard[chessIndex(row+j-n,col,n)]){
                resultBoard[chessIndex(row+j-n,col,n)]=true;
                result++;
                break;
            }
        }
    }
    for(j=1;j<n;j++){
        //diagonal lower right
        if((row+j<n)&&(col+j<n)&&(chessBoard[chessIndex(row+j,col+j,n)])){
            if(!resultBoard[chessIndex(row+j,col+j,n)]){
                resultBoard[chessIndex(row+j,col+j,n)]=true;
                result++;
                break;
            }
        }else if((row+j<n)&&(col+j>=n)&&chessBoard[chessIndex(row+j,col+j-n,n)]&&w){
            if(!resultBoard[chessIndex(row+j,col+j-n,n)]){
                resultBoard[chessIndex(row+j,col+j-n,n)]=true;
                result++;
                break;
            }
        }else if((row+j>=n)&&(col+j<n)&&chessBoard[chessIndex(row+j-n,col+j,n)]&&w){
            if(!resultBoard[chessIndex(row+j-n,col+j,n)]){
                resultBoard[chessIndex(row+j-n,col+j,n)]=true;
                result++;
                break;
            }
        }else if((row+j>=n)&&(col+j>=n)&&chessBoard[chessIndex(row+j-n,col+j-n,n)]&&w){
            if(!resultBoard[chessIndex(row+j-n,col+j-n,n)]){
                resultBoard[chessIndex(row+j-n,col+j-n,n)]=true;
                result++;
                break;
            }
        }
    }
    for(j=1;j<n;j++){
        //diagonal lower left
        if((row+j<n)&&(col-j>=0)&&(chessBoard[chessIndex(row+j,col-j,n)])){
            if(!resultBoard[chessIndex(row+j,col-j,n)]){
                resultBoard[chessIndex(row+j,col-j,n)]=true;
                result++;
                break;
            }
        }else if((row+j<n)&&(col-j<0)&&chessBoard[chessIndex(row+j,col-j+n,n)]&&w){
            if(!resultBoard[chessIndex(row+j,col-j+n,n)]){
                resultBoard[chessIndex(row+j,col-j+n,n)]=true;
                result++;
                break;
            }
        }else if((row+j>=n)&&(col-j>=0)&&chessBoard[chessIndex(row+j-n,col-j,n)]&&w){
            if(!resultBoard[chessIndex(row+j-n,col-j,n)]){
                resultBoard[chessIndex(row+j-n,col-j,n)]=true;
                result++;
                break;
            }
        }else if((row+j>=n)&&(col-j<0)&&chessBoard[chessIndex(row+j-n,col-j+n,n)]&&w){
            if(!resultBoard[chessIndex(row+j-n,col-j+n,n)]){
                resultBoard[chessIndex(row+j-n,col-j+n,n)]=true;
                result++;
                break;
            }
        }
    }
    for(j=1;j<n;j++){
        //diagonal upper right
        if((row-j>=0)&&(col+j<n)&&(chessBoard[chessIndex(row-j,col+j,n)])){
            if(!resultBoard[chessIndex(row-j,col+j,n)]){
                resultBoard[chessIndex(row-j,col+j,n)]=true;
                result++;
                break;
            }
        }else if((row-j>=0)&&(col+j>=n)&&chessBoard[chessIndex(row-j,col+j-n,n)]&&w){
            if(!resultBoard[chessIndex(row-j,col+j-n,n)]){
                resultBoard[chessIndex(row-j,col+j-n,n)]=true;
                result++;
                break;
            }
        }else if((row-j<0)&&(col+j<n)&&chessBoard[chessIndex(row-j+n,col+j,n)]&&w){
            if(!resultBoard[chessIndex(row-j+n,col+j,n)]){
                resultBoard[chessIndex(row-j+n,col+j,n)]=true;
                result++;
                break;
            }
        }else if((row-j<0)&&(col+j>=n)&&chessBoard[chessIndex(row-j+n,col+j-n,n)]&&w){
            if(!resultBoard[chessIndex(row-j+n,col+j-n,n)]){
                resultBoard[chessIndex(row-j+n,col+j-n,n)]=true;
                result++;
                break;
            }
        }
    }
    for(j=1;j<n;j++){
        //diagonal upper left
        if((row-j>=0)&&(col-j>=0)&&(chessBoard[chessIndex(row-j,col-j,n)])){
            if(!resultBoard[chessIndex(row-j,col-j,n)]){
                resultBoard[chessIndex(row-j,col-j,n)]=true;
                result++;
                break;
            }
        }else if((row-j>=0)&&(col-j<0)&&chessBoard[chessIndex(row-j,col-j+n,n)]&&w){
            if(!resultBoard[chessIndex(row-j,col-j+n,n)]){
                resultBoard[chessIndex(row-j,col-j+n,n)]=true;
                result++;
                break;
            }
        }else if((row-j<0)&&(col-j>=0)&&chessBoard[chessIndex(row-j+n,col-j,n)]&&w){
            if(!resultBoard[chessIndex(row-j+n,col-j,n)]){
                resultBoard[chessIndex(row-j+n,col-j,n)]=true;
                result++;
                break;
            }
        }else if((row-j<0)&&(col-j<0)&&chessBoard[chessIndex(row-j+n,col-j+n,n)]&&w){
            if(!resultBoard[chessIndex(row-j+n,col-j+n,n)]){
                resultBoard[chessIndex(row-j+n,col-j+n,n)]=true;
                result++;
                break;
            }
        }
    }
    return result;
}


int getRow(int index, int n){
    return index/n;
}

int getCol(int index, int n){
    return index%n;
}

bool currentPermutation(int n, unsigned long long a, bool chessBoard[n*n]){
    bool patterns[n*n], toSave;
    int i,j;
    memset( patterns, 0, n*n*sizeof(bool) );
    for(i=0;a>0;i++,a/=2){
        patterns[i]=a%2;
    }
    toSave=false;
    int checkSize=(n+1)/2;
    for(i=0;i<checkSize;i++){
        for(j=0;j<checkSize;j++){
            toSave+=patterns[chessIndex(i,j,n)];
        }
    }
    if(toSave){
        for(i=0;i<n*n;i++){
            chessBoard[i]=patterns[i];
        }
    }
    return toSave;
}
void rotateBoard(int n, bool chessBoard[n*n]){
    bool tempBoard[n*n];
    int i;
    for(i=0;i<n*n;i++){
        tempBoard[chessIndex(getCol(i,n),n-1-getRow(i,n),n)]=chessBoard[i];
    }
    for(i=0;i<n*n;i++){
        chessBoard[i]=tempBoard[i];
    }
}
void mirrorBoard(int n, bool chessBoard[n*n]){
    bool tempBoard[n*n];
    int i, j;
    for(i=0;i<n;i++){
        for(j=0;j<n;j++){
            tempBoard[chessIndex(i,j,n)]=chessBoard[chessIndex(i,n-1-j,n)];
        }
    }
    for(i=0;i<n*n;i++){
        chessBoard[i]=tempBoard[i];
    }
}
int getBoardDecimalValue(int n, bool chessBoard[n*n]){
    int temp=1,result=0,i;
    for(i=0;i<n*n;i++){
        result+=(temp*chessBoard[i]);
        temp*=2;
    }
    return result;
}
bool hasSmallerRotation(int n, unsigned long long a, bool chessBoard[n*n]){
    int i,j;
    unsigned long long min=a,cur;
    bool result=false;
    for(i=0;i<2;i++){
        for(j=0;j<4;j++){
            rotateBoard(n,chessBoard);
            cur=getBoardDecimalValue(n,chessBoard);
            if(cur<min){
                result=true;
            }
        }
        mirrorBoard(n,chessBoard);
    }
    return result;
}
void slave(int n, int k, int w){
    unsigned long long permutations_per_slave = size / slaves ;
	int solution[500][100];
    bool chessBoard[n*n],isResult;
    memset(solution,0,500*100*sizeof(int));
    memset(chessBoard,0,n*n*sizeof(bool));
	// Receive data
    unsigned long long a, endOfPermutation;
	MPI_Status status;
	long long before, after;
	int numberOfQueen=0,largestSolution=0,solutionNumber=0,i,j;
	before = wall_clock_time();
	// Getting a few rows of matrix A from the master
    MPI_Recv(&a, 1, MPI_UNSIGNED_LONG_LONG, MASTER_ID, myid, MPI_COMM_WORLD, &status);
    fprintf(stderr," --- SLAVE %d: Received permutation %lli to %lli\n", myid, a, a+permutations_per_slave);
    endOfPermutation = a + permutations_per_slave;
	after = wall_clock_time();
	comm_time += after - before;
	//slave_receive_data(&b, row_a_buffer);
		
	// Doing the computations
	before = wall_clock_time();
	for (a; a < endOfPermutation; a++){
        if(!currentPermutation(n,a,chessBoard)){
            continue;
        }
        isResult=true;
        numberOfQueen=0;
        for(i=0;i<n && isResult;i++){
            for(j=0;j<n && isResult;j++){
                if(chessBoard[chessIndex(i,j,n)]){
                    numberOfQueen++;
                    if(nPieceWrap(n,chessBoard,i,j,w)!=k){
                        isResult=false;
                    }
                }
            }
        }
        if(isResult){
            if(largestSolution<=numberOfQueen){
                if(largestSolution<numberOfQueen){
                    solutionNumber=0;
                    largestSolution=numberOfQueen;
                }
                if(!hasSmallerRotation(n,a,chessBoard)){
                    solution[solutionNumber][0]=n;
                    solution[solutionNumber][1]=k;
                    solution[solutionNumber][2]=largestSolution;
                    int solutionIndex=3;
                    for(i=0;i<n;i++){
                        for(j=0;j<n;j++){
                            if(chessBoard[chessIndex(i,j,n)]){
                                solution[solutionNumber][solutionIndex]=i*n+j+1;
                                //printf("%d ",solution[solutionNumber][solutionIndex]);
                                solutionIndex++;
                            }
                        }
                    }
                    //printf("\n");
                    solutionNumber++;
                }
            }

        }
	}
	after = wall_clock_time();
	comp_time += after - before;
    fprintf(stderr," --- SLAVE %d: Finished the computations\n", myid);
	//slave_compute(b, row_a_buffer, result);
    

	// Sending the results back
    before = wall_clock_time();
    int solutionNumber_id = 5 * myid;
    MPI_Send(&solutionNumber, 1, MPI_INT, MASTER_ID, solutionNumber_id, MPI_COMM_WORLD);
	for (i = 0; i < solutionNumber; i++){
		int result_id = myid * solutionNumber + i;
		MPI_Send(&solution[i], largestSolution+3, MPI_INT, MASTER_ID, result_id, MPI_COMM_WORLD);
	}
	after = wall_clock_time();
	comm_time += after - before;
	fprintf(stderr," --- SLAVE %d: Sent the results back: solutionNumber %d\n", myid, i, solutionNumber);
	//slave_send_result(result);
	
	fprintf(stderr, " --- SLAVE %d: communication_time=%6.2f seconds; computation_time=%6.2f seconds\n", myid, comm_time / 1000000000.0, comp_time / 1000000000.0);
}

int master(int n, int k, int l, int w, int solution[][100]){
    // Distribute data to slaves
    int j, u, o, maxQueenNumber=0, solutionNumber=0,maxTempSolution=500;
    MPI_Status status;
    memset(solution, 0, 3000*100*sizeof(int));
	int slave_id = 1;
	// Matrix A is split into each chunks;
	// Each chunck has rows_per_slave rows
	unsigned long long permutations_per_slave = size / slaves, i;

	fprintf(stderr," +++ MASTER : Distributing matrix permutations to slaves\n");
	// Send the rows to each process
	for (slave_id = 0, i = 0; slave_id < slaves && i < size; slave_id++,i+=permutations_per_slave){			
        
        
        MPI_Send(&i, 1, MPI_UNSIGNED_LONG_LONG, slave_id, slave_id, MPI_COMM_WORLD);
        fprintf(stderr," +++ MASTER : Sending permutation %lli to process %d\n", i, slave_id);
	}
	//master_distribute(a, b);
	
	// Gather results from slaves
    fprintf(stderr," +++ MASTER : Receiving the results from slaves\n");
	
	// Get the results
	for (slave_id = 0; slave_id < slaves ; slave_id++)
	{	
		int tempSolutionSize=0,tempSolutionSize_id= 5 * slave_id;
        MPI_Recv(&tempSolutionSize, 1, MPI_INT, slave_id, tempSolutionSize_id, MPI_COMM_WORLD, &status);
        fprintf(stderr," +++ MASTER : Receiving %d solutions from slave %d\n", tempSolutionSize, slave_id);
        for (j = 0; j < tempSolutionSize; j++){
			int result_id = slave_id * tempSolutionSize + j;
			int tempResult[100];
			MPI_Recv(tempResult, maxTempSolution, MPI_INT, slave_id, result_id, MPI_COMM_WORLD, &status);
            //fprintf(stderr," +++ MASTER : Received result from slave %d\n", slave_id);
            if(tempResult[2]>maxQueenNumber && j==0){
                maxQueenNumber=tempResult[2];
                //printf("currently max queen number is %d from process %d\n",maxQueenNumber,slave_id);
                solutionNumber=0;
                memset(solution, 0, 3000*100*sizeof(int));

                for(o=0;o<maxQueenNumber+3;o++){
                    solution[solutionNumber][o]=tempResult[o];
                    //printf("%d ",solution[solutionNumber][o]);
                }
                //printf(": tempSoluionSize %d\n",tempSolutionSize);
                solutionNumber++;
            }else if(tempResult[2]==maxQueenNumber){
                //printf("same max queen number %d from process %d\n",maxQueenNumber,slave_id);
                for(o=0;o<maxQueenNumber+3;o++){
                    solution[solutionNumber][o]=tempResult[o];
                    //printf("%d ",solution[solutionNumber][o]);
                }
                //printf(": tempSoluionSize %d\n",tempSolutionSize);
                solutionNumber++;
            }
		}
	}
    return solutionNumber;
}

int charToInt(char input[]){
    int result=0,i;
    for(i=0;input[i]!=0;i++){
        result*=10;
        result+=(input[i]-'0');
    }
    return result;
}



int main(int argc, char *argv[]){
    //printf("Hello World!\n");
    //printf("%lli",power(2,36));
    int n,k,l,w, j, u, solution[3000][100],solutionNumber;
    if(argc!=5){
        n=4;
        k=3;
        l=1;
        w=0;
    }else{
        n=charToInt(argv[1]);
        k=charToInt(argv[2]);
        l=charToInt(argv[3]);
        w=charToInt(argv[4]);
    }
    size = power(2,n*n);
    int nprocs;
	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    slaves = nprocs - 1;
	
	if (myid == MASTER_ID)
	{
		fprintf(stderr, " +++ Process %d is master\n", myid);
		solutionNumber = master(n,k,l,w,solution);
	}
	else
	{
		fprintf(stderr, " --- Process %d is slave\n", myid);
		slave(n,k,w);
	}	
	MPI_Finalize();
    int maxQueenNumber = solution[0][2];
    if(l){
        for(u=0;u<solutionNumber;u++){
            printf("%d,%d:%d:",solution[u][0],solution[u][1],solution[u][2]);
            for(j=3;j<(maxQueenNumber+3);j++){
                printf("%d",solution[u][j]);
                if(j!=maxQueenNumber+2){
                    printf(",");
                }
            }
            printf("\n");
        }
    }else{
        printf("%d,%d:%d:\n",solution[0][0],solution[0][1],solution[0][2]);
    }
    //aggressiveQueen(3,5,1,1);
    //bool test[16]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
    //printf("%d\n",nPieceWrap2(4,test,0,0));
    // if(hasSmallerRotation(4,getBoardDecimalValue(4,test),test)){
    //     printf("has smaller\n");
    // }else{
    //     printf("is smallest\n");
    // }
    // printBoard(4,test);
    // printf("%d\n",getBoardDecimalValue(4,test));
    // rotateBoard(4,test);
    // printBoard(4,test);
    // printf("%d\n",getBoardDecimalValue(4,test));
    // mirrorBoard(4,test);
    // printBoard(4,test);
    // printf("%d\n",getBoardDecimalValue(4,test));
    // if(hasSmallerRotation(4,getBoardDecimalValue(4,test),test)){
    //     printf("has smaller\n");
    // }else{
    //     printf("is smallest\n");
    // }


    return 0;
}

