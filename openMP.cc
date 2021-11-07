#include <iostream>
#include <fstream>
#include <sstream>
#include <omp.h>
using namespace std;




#define WIDTH 2624 //collumns
#define HEIGHT 1984 //rows

/* Function to sort an array using insertion sort*/
void insertionSort(int window[], int n)
{
	for (int i = 1; i < n; i++)
	{
		int key = window[i];
		int k = 0;
		bool breakSim = false; //used to simulate break; function

		for(int j=i-1; j>=0; j--)
		{
			if(!breakSim && window[j] > key)
			{
				k = j;
				window[j + 1] = window[j];
			}

			if(!breakSim && window[j] <= key)
			{
				k = j;
				breakSim = true;
			}
		}

		window[k + 1] = key;
	}

	// int i, key, j;
	// for (i = 1; i < n; i++)
	// {
	// 	key = arr[i];
	// 	j = i - 1;

	// 	/* Move elements of arr[0..i-1], that are
	// 	greater than key, to one position ahead
	// 	of their current position */
	// 	while (j >= 0 && arr[j] > key)
	// 	{
	// 		arr[j + 1] = arr[j];
	// 		j = j - 1;
	// 	}
	// 	arr[j + 1] = key;
	// }
}

int imageArray[HEIGHT][WIDTH];
int outputArray[HEIGHT][WIDTH];
double tstart,tcalc = 0;


int main()
{
	omp_set_num_threads(5);
	tstart = omp_get_wtime(); // start timing

	int window[9], row = 0, col = 0, numrows = 0, numcols = 0, MAX=0;
	ifstream infile("input.pgm");
	stringstream ss;
	string inputLine = "";

	// First line : version
	getline(infile,inputLine);
	if(inputLine.compare("P2") != 0) cerr << "Version error" << endl;
	else cout << "Version : " << inputLine << endl;

	// Continue with a stringstream
	ss << infile.rdbuf();

	// Secondline : size of image
	ss >> numcols >> numrows >> MAX;

	//print total number of rows, columns and maximum intensity of image
	cout << numcols << " columns and " << numrows << " rows" << endl <<" Maximium Intesity "<< MAX <<endl;


	//Initialize a new array of same size of image with 0
	// for(row = 0; row <= numrows; ++row)
	// 	imageArray[row][0]=0;
	// for( col = 0; col<=numcols; ++col )
	// 	imageArray[0][col]=0;
	#pragma omp parallel for collapse(2)
	for(row = 1; row <= numrows; ++row)
	{
		for (col = 1; col <= numcols; ++col)
		{
			imageArray[row][col]=0;
		}
	}


	// Following lines : data
	//Reading from stringstream
	for(row = 1; row <= numrows; ++row)
	{
		for (col = 1; col <= numcols; ++col)
		{
			//original data store in new array
			ss >> imageArray[row][col];
		}
	}


	// Now print the array to see the result
	#pragma omp parallel for collapse(2)
	for(row = 1; row <= numrows; ++row)
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

			//sort window imageArray
			insertionSort(window,9);
			//put the median to the new imageArray
			outputArray[row][col]=window[4];
		}
	}

	ofstream outfile;

	//new file open to stroe the output image
	outfile.open("MedianOutput.pnm");
	outfile<<"P2"<<endl;
	outfile<<numcols<<" "<<numrows<<endl;
	outfile<<"255"<<endl;

	//Writing out to file
	for(row = 1; row <= numrows; ++row)
	{
		for (col = 1; col <= numcols; ++col)
		{
			//store resultant pixel values to the output file
			outfile << outputArray[row][col]<<" ";
		}
	}

	outfile.close();
	infile.close();

	tcalc = omp_get_wtime() - tstart; // stop timing
	std::cout << "Time Taken: " << tcalc << std::endl;

	return 0 ;
}
