#include <stdio.h>
#include <stdlib.h>
float* getInput(char *filename)
{

	FILE *fp;
	float *input = (float*)malloc(sizeof(float)*6000);
	//fp = fopen("mSample.csv","r");
	fp= fopen(filename,"r");
	if(!fp)
	{
	    printf("File failed to open!\n");
	    exit(1);
	}
	int i=0;
	char value[1024];
	while (!feof(fp))
	{
		fscanf(fp,"%s\n",value);
		input[i]= atof(value);
  		i++;
	}
	fclose(fp); 
	free(input);
	return input;
}
int * findpeaks(float* sampled,int* xsampled,int num_samples,int *num_peaks)
{
	int *peaks = (int*)malloc(sizeof(int)*1000);
	int i;
	for (i=1;i<num_samples-1;i++)
	{
		if ((sampled[xsampled[i]] >= sampled[xsampled[i-1]]) && (sampled[xsampled[i]] >=sampled[xsampled[i+1]]))
		{
			peaks[*num_peaks]=xsampled[i];
			*num_peaks=*num_peaks+1;
		}
	}
	free(peaks);
	return peaks;
}
void plot(int *x,float *y,int num_points)
{
	int i;
	FILE * temp = fopen("data.temp", "w");
	char * commandsForGnuplot[] = {"set border linewidth 0.5;set style line 1 lc rgb '#0060ad' lt 1 lw 2 pt 7 ps 0.1;set title \"sampled plot with guassian smooth\";set xlabel 'x';set ylabel 'y';set xrange [1:800];set yrange [350:800]", "set terminal png;set output 'output.png' ;plot 'data.temp' with linespoints ls 1"};
	FILE * gnuplotPipe = popen ("gnuplot -persistent", "w");
    for (i=0; i < num_points; i++)
    {
    	fprintf(temp, "%d %lf \n", x[i], y[i]); //Write the data to a temporary file
    }

    for (i=0; i < 2; i++)
    {
    	fprintf(gnuplotPipe, "%s \n", commandsForGnuplot[i]); //Send commands to gnuplot one by one.
    }
}
int main()
{
	float * y = getInput("mSample.csv");
	int num_points =5728;
	int i,j=0;
	//smoothing
	float* sampled=(float*)malloc(sizeof(float)*6000);;
	int* xsampled=(int*)malloc(sizeof(int)*6000);;
	for(i=3;i<=5727;i=i+7)
	{
		sampled[j]=(y[j-3]+3*y[j-2]+6*y[j-1]+7*y[j]+6*y[j+1]+3*y[j+2]+y[j+3])/27;//smooth ratio:7/20=0.35 ;num_peaks=37
		//sampled[j]=(y[j-1]+y[j]+y[j+1])/3;//num_peaks =146
		//sampled[j]=(y[j-2]+2*y[j-1]+3*y[j]+2*y[j+1]+y[j+2])/9;//num_peaks=61
		xsampled[j]=j;
		j=j+1;
	}
	//plotting
	plot(xsampled,sampled,(num_points));
	int num_samples =j;
	int num_peaks =0;
	//finding peaks
	int* peaks = findpeaks(sampled,xsampled,num_samples,&num_peaks);
	printf("by plotting the input data from points 5020-5300 we can see there are approximately 100 points between 2 peaks\ntherefore sampling frequency is 100/(60/75) =125.\n");
	printf("given total number of data points =5728.\ntotal time of the given graph is 5728/125 = 45 sec\n");
	printf("number of heart beats= %d\n",num_peaks);
	free(sampled);
	free(xsampled);
	return 0;
}