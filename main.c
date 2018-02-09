#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define ROWS		(int)480
#define COLUMNS		(int)640

void clear( unsigned char image[][COLUMNS] );
void header( int row, int col, unsigned char head[32] );
int getCoordinate(int i);
int findMax(int* image[]);
int main( int argc, char **argv )
{

	int				i, j, k, threshold[3], max[3], x, y, s;
	FILE			*fp;
	unsigned char	image[ROWS][COLUMNS], ximage[ROWS][COLUMNS], yimage[ROWS][COLUMNS], SGM[ROWS][COLUMNS], biIMAGE[ROWS][COLUMNS], head[32];
	char			filename[6][50], ifilename[50], ch;
	int             *buffer[ROWS];
	int             *buffer2[ROWS];
	int             *buffer3[ROWS];
    int  sobelX[3][3]={{-1,0,1},{-2,0,2},{-1,0,1}};
    int           sobelY[3][3]={{1,2,1},{0,0,0},{-1,-2,-1}};
	strcpy( filename[0], "image1" );
	strcpy( filename[1], "image2" );
	strcpy( filename[2], "image3" );
	header ( ROWS, COLUMNS, head );
	for(i=0;i<ROWS;i++)
    {
        buffer[i]=(int*) malloc(COLUMNS*sizeof(int));
        buffer2[i]=(int*) malloc(COLUMNS*sizeof(int));
        buffer3[i]=(int*) malloc(COLUMNS*sizeof(int));
    }

	for(i=0;i<ROWS;i++)
        for(j=0;j<COLUMNS;j++)
        {
            buffer[i][j]=0;
            buffer2[i][j]=0;
            buffer3[i][j]=0;
        }

	threshold[0]=52;
	threshold[1]=22;
	threshold[2]=70;
	for ( k = 0; k < 3; k ++)
	{
		clear( ximage );
		clear( yimage );
		clear( biIMAGE);
		/* Read in the image */
		strcpy( ifilename, filename[k] );
		if (( fp = fopen( strcat(ifilename, ".raw"), "rb" )) == NULL )
		{
		  fprintf( stderr, "error: couldn't open %s\n", ifilename );
		  exit( 1 );
		}
		for ( i = 0; i < ROWS ; i++ )
		  if ( fread( image[i], sizeof(char), COLUMNS, fp ) != COLUMNS )
		  {
			fprintf( stderr, "error: couldn't read enough stuff\n" );
			exit( 1 );
		  }
		fclose( fp );

		max[0] = 0; //maximum of Gx
		max[1] = 0; //maximum of Gy
		max[2] = 0; //maximum of SGM

		/* Compute Gx, Gy, SGM, find out the maximum and normalize*/

		/*****************************************
        ************** CALCULATE Gx **************
		******************************************/
		for(i=0;i<ROWS;i++)
        {
            for(j=0;j<COLUMNS;j++)
            {
                if((i==0) || (i== ROWS-1) || (j==0) || (j==COLUMNS-1))
                {
                    continue;
                }
                else
                {
                    int temp=0;
                    for(x=0;x<3;x++)
                        for(y=0;y<3;y++)
                            temp=abs(temp+image[i+getCoordinate(x)][j+getCoordinate(y)]*sobelX[x][y]);
                    buffer[i][j]=temp;
                }
            }
        }
        max[0]=findMax(buffer);

		for(i=0;i<ROWS;i++)
        {
            for(j=0;j<COLUMNS;j++)
            {
                /*if(buffer[i][j]>255)
                    ximage[i][j]=buffer[i][j]/max[0]*255;
                else
                    ximage[i][j]=buffer[i][j];*/
                ximage[i][j]=(int)(((float)buffer[i][j]/(float)max[0])*255);

            }
        }
        /* Write Gx to a new image*/
		strcpy( ifilename, filename[k] );
		if (!( fp = fopen( strcat( ifilename, "-x.ras" ), "wb" ) ))
		{
		  fprintf( stderr, "error: could not open %s\n", ifilename );
		  exit( 1 );
		}
		fwrite( head, 4, 8, fp );

		for ( i = 0 ; i < ROWS ; i++ ) fwrite( ximage[i], 1, COLUMNS, fp );
	    fclose( fp );

        /*****************************************
        ************** CALCULATE Gy **************
		******************************************/

        for(i=0;i<ROWS;i++)
        {
            for(j=0;j<COLUMNS;j++)
            {
                if((i==0) || (i== ROWS-1) || (j==0) || (j==COLUMNS-1))
                {
                    continue;
                }
                else
                {
                    int temp=0;
                    for(x=0;x<3;x++)
                        for(y=0;y<3;y++)
                            temp=abs(temp+image[i+getCoordinate(x)][j+getCoordinate(y)]*sobelY[x][y]);
                    buffer2[i][j]=temp;
                }

            }
        }

        max[1]=findMax(buffer2);

        for(i=0;i<ROWS;i++)
        {
            for(j=0;j<COLUMNS;j++)
            {
                /*if(buffer[i][j]>255)
                    yimage[i][j]=buffer2[i][j]/max[1]*255;
                else
                    yimage[i][j]=buffer2[i][j];*/
                yimage[i][j]=(int)(((float)buffer2[i][j]/(float)max[1])*255);

            }
        }
        /* Write Gy to a new image */
		strcpy( ifilename, filename[k] );
		if (!( fp = fopen( strcat( ifilename, "-y.ras" ), "wb" ) ))
		{
		  fprintf( stderr, "error: could not open %s\n", ifilename );
		  exit( 1 );
		}
		fwrite( head, 4, 8, fp );

        for ( i = 0 ; i < ROWS ; i++ ) fwrite( yimage[i], 1, COLUMNS, fp );
		fclose( fp );

        /*****************************************
        ************** CALCULATE SGM **************
		******************************************/
        for(i=0;i<ROWS;i++)
        {
            for(j=0;j<COLUMNS;j++)
            {
                //buffer3[i][j]=buffer[i][j]*buffer[i][j]+buffer2[i][j]*buffer2[i][j];
                buffer3[i][j]=ximage[i][j]*ximage[i][j]+yimage[i][j]*image[i][j];
            }
        }
        max[2]=findMax(buffer3);

        for(i=0;i<ROWS;i++)
        {
            for(j=0;j<COLUMNS;j++)
            {
                SGM[i][j]=(int)(((float)buffer3[i][j]/(float)max[2])*255);
            }
        }
        /* Write SGM to a new image */
		strcpy( ifilename, filename[k] );
		if (!( fp = fopen( strcat( ifilename, "-s.ras" ), "wb" ) ))
		{
		  fprintf( stderr, "error: could not open %s\n", ifilename );
		  exit( 1 );
		}
		fwrite( head, 4, 8, fp );


        for ( i = 0 ; i < ROWS ; i++ ) fwrite( SGM[i], 1, COLUMNS, fp );
		fclose( fp );

		 /*****************************************
        ********* CALCULATE BINARY IMAGE *********
		******************************************/
        for(i=0;i<ROWS;i++)
        {
            for(j=0;j<COLUMNS;j++)
            {
                if(SGM[i][j]<threshold[k])
                    biIMAGE[i][j]=255;
                else
                    biIMAGE[i][j]=0;
            }
        }
		/* Write the binary image to a new image */
		strcpy( ifilename, filename[k] );
		if (!( fp = fopen( strcat( ifilename, "-b.ras" ), "wb" ) ))
		{
		  fprintf( stderr, "error: could not open %s\n", ifilename );
		  exit( 1 );
		}
		fwrite( head, 4, 8, fp );

        for ( i = 0 ; i < ROWS ; i++ ) fwrite( biIMAGE[i], 1, COLUMNS, fp );
		fclose( fp );
		printf( "image %d: %d %d %d\n", k, max[0], max[1], max[2] );
	}

	printf( "Press any key to exit: " );
	gets( &ch );
	for(i=0;i<ROWS;i++)
    {
        free(buffer[i]);
        free(buffer2[i]);
        free(buffer3[i]);
    }

	return 0;
}
int getCoordinate(int i)
{
    if(i==0)
        return -1;
    else if(i==1)
        return 0;
    else if(i==2)
        return 1;
    else
        return 10000;
}
int findMax(int* image[])
{
    int max=0;
    int i,j;
    for(i=0;i<ROWS;i++)
    {
        for(j=0;j<COLUMNS;j++)
        {
            if(max<(int)image[i][j])
                max=(int)image[i][j];
        }
    }
    return max;
}
void clear( unsigned char image[][COLUMNS] )
{
	int	i,j;
	for ( i = 0 ; i < ROWS ; i++ )
		for ( j = 0 ; j < COLUMNS ; j++ ) image[i][j] = 0;
}

void header( int row, int col, unsigned char head[32] )
{
	int *p = (int *)head;
	char *ch;
	int num = row * col;

	/* Choose little-endian or big-endian header depending on the machine. Don't modify this */
	/* Little-endian for PC */

	*p = 0x956aa659;
	*(p + 3) = 0x08000000;
	*(p + 5) = 0x01000000;
	*(p + 6) = 0x0;
	*(p + 7) = 0xf8000000;

	ch = (char*)&col;
	head[7] = *ch;
	ch ++;
	head[6] = *ch;
	ch ++;
	head[5] = *ch;
	ch ++;
	head[4] = *ch;

	ch = (char*)&row;
	head[11] = *ch;
	ch ++;
	head[10] = *ch;
	ch ++;
	head[9] = *ch;
	ch ++;
	head[8] = *ch;

	ch = (char*)&num;
	head[19] = *ch;
	ch ++;
	head[18] = *ch;
	ch ++;
	head[17] = *ch;
	ch ++;
	head[16] = *ch;


	/* Big-endian for unix */
	/*
	*p = 0x59a66a95;
	*(p + 1) = col;
	*(p + 2) = row;
	*(p + 3) = 0x8;
	*(p + 4) = num;
	*(p + 5) = 0x1;
	*(p + 6) = 0x0;
	*(p + 7) = 0xf8;
*/
}
