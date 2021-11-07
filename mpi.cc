#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <mpi.h>
using namespace std;



#define WIDTH 2624 //collumns
#define HEIGHT 1984 //rows

/* Function to sort an array using insertion sort*/
void insertionSort(int arr[], int n)
{
	int i, key, j;
	for (i = 1; i < n; i++)
	{
		key = arr[i];
		j = i - 1;

		/* Move elements of arr[0..i-1], that are
		greater than key, to one position ahead
		of their current position */
		while (j >= 0 && arr[j] > key)
		{
			arr[j + 1] = arr[j];
			j = j - 1;
		}
		arr[j + 1] = key;
	}
}

int imageArray[HEIGHT][WIDTH];
int arr[HEIGHT][WIDTH];
int recv[HEIGHT][WIDTH];


int main()
{
	MPI_Init(NULL, NULL);
	MPI_Status status;
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);


	int window[9],row = 0, col = 0, numrows = 0, numcols = 0,MAX=0;
	ifstream infile("input.pgm");
	stringstream ss;
	string inputLine = "";

	// First line : version
	getline(infile,inputLine);
	if(rank == 0)
	{
		if(inputLine.compare("P2") != 0) cerr << "Version error" << endl;
		else cout << "Version : " << inputLine << endl;
	}
	// Continue with a stringstream
	ss << infile.rdbuf();

	// Secondline : size of image
	ss >> numcols >> numrows >> MAX;

	if(rank == 0)
	{
		//print total number of rows, columns and maximum intensity of image
		cout << numcols << " columns and " << numrows << " rows" << endl <<" Maximium Intesity " << MAX << endl;
	}

	//Initialize a new array of same size of image with 0
	for(row = 1; row <= numrows; ++row)
	{
		for (col = 1; col <= numcols; ++col)
		{
			//original data store in new array
			imageArray[row][col] = 0;
		}
	}
	// Following lines : data
	for(row = 1; row <= numrows; ++row)
	{
		for (col = 1; col <= numcols; ++col)
		{
			//original data store in new array
			ss >> imageArray[row][col];
		}
	}

	int size = HEIGHT*WIDTH; //Used for Ssend and Recv










    if(rank == 0) ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    {
       	// Now print the array to see the result
		for(row = 1; row < numrows/4; ++row)
		{
			for(col = 1; col <= numcols; ++col)
			{
				//neighbor pixel values are stored in window including this pixel
				window[0] = imageArray[row-1][col-1];
				window[1] = imageArray[row-1][col];
				window[2] = imageArray[row-1][col+1];
				window[3] = imageArray[row][col-1];
				window[4] = imageArray[row][col];
				window[5] = imageArray[row][col+1];
				window[6] = imageArray[row+1][col-1];
				window[7] = imageArray[row+1][col];
				window[8] = imageArray[row+1][col+1];

				//sort window array
				insertionSort(window,9);
				//put the median to the new array
				arr[row][col]=window[4];
			}
		}

        MPI_Recv(recv, size, MPI_INT, 1 , 0 , MPI_COMM_WORLD, &status);
		for(row = numrows/4; row < (numrows/2); ++row)
		{
			for (col = 1; col <= numcols; ++col)
			{
				arr[row][col] = recv[row][col];
			}
		}

		MPI_Recv(recv, size, MPI_INT, 2 , 0 , MPI_COMM_WORLD, &status);
		for(row = (numrows/2); row < ((numrows*3)/4); ++row)
		{
			for (col = 1; col <= numcols; ++col)
			{
				arr[row][col] = recv[row][col];
			}
		}

		MPI_Recv(recv, size, MPI_INT, 3 , 0 , MPI_COMM_WORLD, &status);
		for(row = ((numrows*3)/4); row <= numrows; ++row)
		{
			for (col = 1; col <= numcols; ++col)
			{
				arr[row][col] = recv[row][col];
			}
		}

		ofstream outfile;

		//new file open to stroe the output image
		outfile.open("MedianOutput1.pnm");
		outfile<<"P2"<<endl;
		outfile<<numcols<<" "<<numrows<<endl;
		outfile<<"255"<<endl;

		for(row = 1; row <= numrows; ++row)
		{
			for (col = 1; col <= numcols; ++col)
			{
				//store resultant pixel values to the output file
				outfile << arr[row][col]<<" ";
			}
		}

		outfile.close();

    }
    else if(rank == 1) ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    {
       // Now print the array to see the result
		for(row = numrows/4; row < (numrows/2); ++row)
		{
			for(col = 1; col <= numcols; ++col)
			{
				//neighbor pixel values are stored in window including this pixel
				window[0] = imageArray[row-1][col-1];
				window[1] = imageArray[row-1][col];
				window[2] = imageArray[row-1][col+1];
				window[3] = imageArray[row][col-1];
				window[4] = imageArray[row][col];
				window[5] = imageArray[row][col+1];
				window[6] = imageArray[row+1][col-1];
				window[7] = imageArray[row+1][col];
				window[8] = imageArray[row+1][col+1];

				//sort window array
				insertionSort(window,9);
				//put the median to the new array
				arr[row][col]=window[4];
			}
		}
		MPI_Ssend(arr, size, MPI_INT, 0, 0, MPI_COMM_WORLD);

    }
	else if(rank == 2) ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    {
       // Now print the array to see the result
		for(row = (numrows/2); row < ((numrows*3)/4); ++row)
		{
			for(col = 1; col <= numcols; ++col)
			{
				//neighbor pixel values are stored in window including this pixel
				window[0] = imageArray[row-1][col-1];
				window[1] = imageArray[row-1][col];
				window[2] = imageArray[row-1][col+1];
				window[3] = imageArray[row][col-1];
				window[4] = imageArray[row][col];
				window[5] = imageArray[row][col+1];
				window[6] = imageArray[row+1][col-1];
				window[7] = imageArray[row+1][col];
				window[8] = imageArray[row+1][col+1];

				//sort window array
				insertionSort(window,9);
				//put the median to the new array
				arr[row][col]=window[4];
			}
		}
		MPI_Ssend(arr, size, MPI_INT, 0, 0, MPI_COMM_WORLD);

    }
	else if(rank == 3) ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    {
       // Now print the array to see the result
		for(row = ((numrows*3)/4); row <= numrows; ++row)
		{
			for(col = 1; col <= numcols; ++col)
			{
				//neighbor pixel values are stored in window including this pixel
				window[0] = imageArray[row-1][col-1];
				window[1] = imageArray[row-1][col];
				window[2] = imageArray[row-1][col+1];
				window[3] = imageArray[row][col-1];
				window[4] = imageArray[row][col];
				window[5] = imageArray[row][col+1];
				window[6] = imageArray[row+1][col-1];
				window[7] = imageArray[row+1][col];
				window[8] = imageArray[row+1][col+1];

				//sort window array
				insertionSort(window,9);
				//put the median to the new array
				arr[row][col]=window[4];
			}
		}
		MPI_Ssend(arr, size, MPI_INT, 0, 0, MPI_COMM_WORLD);

    }

























	infile.close();
	MPI_Finalize();
	return 0 ;

}
