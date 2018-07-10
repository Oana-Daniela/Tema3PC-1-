#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tema3.h"

#define EXIT_WITH_ERROR 1

typedef struct _input_file {
	char 			filename[64];
	char 			archive[64];
	int 			limit;
} input_file;

typedef struct _pixel {
	unsigned char 	r;
	unsigned char	g;
	unsigned char	b;
} pixel;

typedef struct _image {
	struct bmp_fileheader*	header;
	struct bmp_infoheader*	infoheader;
	pixel**					matrix;
} image;


image* read_bitmap(input_file input)
{
	FILE *in;
    if ((in = fopen(input.filename, "rb")) == NULL) 
	{
        fprintf(stderr, "EROARE: Fisierul %s nu a putut fi deschis\n", input.filename);
        exit(EXIT_WITH_ERROR);
	}

    // Aloc memorie pentru structura noii imagini
    image* img = (image*) malloc(sizeof(image));
    // Aloc memorie pentru header si il citesc din fisier
    img->header = (struct bmp_fileheader *) malloc(sizeof(struct bmp_fileheader));
    fread(img->header, sizeof(struct bmp_fileheader), 1, in);

    // Continui cu infoheader-ul
    img->infoheader = (struct bmp_infoheader *) malloc(sizeof(struct bmp_infoheader));
    fread(img->infoheader, sizeof(struct bmp_infoheader), 1, in);

    // Tuplurile vor fi citite de la o anumita valoare
    fseek(in, img->header->imageDataOffset, SEEK_SET);

    
    img->matrix = (pixel**) malloc(img->infoheader->height * sizeof(pixel*));
    int i, j;
    int paddingRemaining = (sizeof(pixel) * img->infoheader->width) % 4;
    for (i = img->infoheader->height - 1; i >= 0; i--) 
	{
    	img->matrix[i] = (pixel*) malloc(img->infoheader->width * sizeof(pixel));
    	for (j = 0; j < img->infoheader->width; j++) 
		{
    		fread(&img->matrix[i][j], sizeof(pixel), 1, in);
		}
    	fseek(in, paddingRemaining, SEEK_CUR);
	}

    fclose(in);

    return img;
}

void write_bitmap(input_file input, image img, char* output_filename)
{
	FILE* out;
    // Deschidem fisierul pentru scriere in mod binar
    if ((out = fopen(output_filename, "wb")) == NULL)
	{
        fprintf(stderr, "EROARE: Fisierul %s nu a putut fi deschis pentru scriere\n", output_filename);
        exit(EXIT_WITH_ERROR);
	}

   
    fwrite(img.header, sizeof(struct bmp_fileheader), 1, out);

    fwrite(img.infoheader, sizeof(struct bmp_infoheader), 1, out);

   
    fseek(out, img.header->imageDataOffset, SEEK_SET);

    // Se scrie bitmap-ul rasturnat
    int i, j;
    unsigned char paddingChar = 0;
    int paddingRemaining = (sizeof(pixel) * img.infoheader->width) % 4;
    for (i = img.infoheader->height - 1; i >= 0; i--) 
	{
    	for (j = 0; j < img.infoheader->width; j++) 
		{
    		fwrite(&img.matrix[i][j], sizeof(pixel), 1, out);
		}
    	for (j = 0; j < paddingRemaining; j++) 
		{
    		fwrite(&paddingChar, sizeof(unsigned char), 1, out);
		}
    }

    fclose(out);
}

input_file* read_input()
{
	
    char input_filename[] = "input.txt";
 
    // Deschidem fisierul pentru citire in modul text
    FILE *in;
    if ((in = fopen(input_filename, "rt")) == NULL) 
	{
        fprintf(stderr, "EROARE: Fisierul %s nu a putut fi deschis\n", input_filename);
        exit(EXIT_WITH_ERROR);
	}

   
    input_file* input = (input_file*) malloc(sizeof(input_file));

    // Citesc toate informatiile din fisierul de intrare
    fscanf(in, "%s", input->filename);
    fscanf(in, "%d", &input->limit);
    fscanf(in, "%s", input->archive);

   
    fclose(in);

    return input;
}
//-----------------------Task1-----------
image* black_white(image color)
{
	image* result = (image*)malloc(sizeof(image));
	result->header = (struct bmp_fileheader *) malloc(sizeof(struct bmp_fileheader));
	result->infoheader = (struct bmp_infoheader *) malloc(sizeof(struct bmp_infoheader));
	
	memcpy(result->header,color.header,sizeof(struct bmp_fileheader));
	memcpy(result->infoheader,color.infoheader,sizeof(struct bmp_infoheader));

	result->matrix = (pixel**) malloc(color.infoheader->height * sizeof(pixel*));
    int i, j;
    for (i = 0; i < color.infoheader->height; i++) 
	{
    	result->matrix[i] = (pixel*) malloc(color.infoheader->width * sizeof(pixel));
    	for (j = 0; j < color.infoheader->width; j++) 
		{
    		int avg = (color.matrix[i][j].r + color.matrix[i][j].g + color.matrix[i][j].b) / 3;
    		result->matrix[i][j].b = avg;
    		result->matrix[i][j].g = avg;
    		result->matrix[i][j].r = avg;
		}
	}
	return result;

}

void build_filename(char* result, char *input, char *append)
{
	char *p = strchr(input, '.');
	int size = 0;
	if (p != NULL) 
	{
		size = p - input;
	}

	memcpy(result, input, size);
	result[size] = '\0';
	strcat(result, append);
}
//---------------------Task2------------------
image* filter(image color, int filter[3][3]){

	image* result=(image*)malloc(sizeof(image));
	result->header = (struct bmp_fileheader *) malloc(sizeof(struct bmp_fileheader));
	result->infoheader = (struct bmp_infoheader *) malloc(sizeof(struct bmp_infoheader));
	memcpy(result->header,color.header,sizeof(struct bmp_fileheader));
	memcpy(result->infoheader,color.infoheader,sizeof(struct bmp_infoheader));

	result->matrix = (pixel**) malloc(color.infoheader->height * sizeof(pixel*));
    int i, j;
    for (i = 0; i < color.infoheader->height; i++)
	{
    	result->matrix[i] = (pixel*) malloc(color.infoheader->width * sizeof(pixel));
    	for (j = 0; j < color.infoheader->width; j++)
		{
    		int sum_r = color.matrix[i][j].r * filter[1][1];
    		int sum_g = color.matrix[i][j].g * filter[1][1];
    		int sum_b = color.matrix[i][j].b * filter[1][1];

    		if (i>0 && j > 0)
			{
    			sum_r += color.matrix[i-1][j-1].r * filter[0][0];
    			sum_g += color.matrix[i-1][j-1].g * filter[0][0];
    			sum_b += color.matrix[i-1][j-1].b * filter[0][0];
			}

    		if (i>0)
			{
    			sum_r += color.matrix[i-1][j].r * filter[0][1];
    			sum_g += color.matrix[i-1][j].g * filter[0][1];
    			sum_b += color.matrix[i-1][j].b * filter[0][1];
			}

    		if (i > 0 && j < color.infoheader->width - 1)
			{
    			sum_r += color.matrix[i-1][j+1].r * filter[0][2];
    			sum_g += color.matrix[i-1][j+1].g * filter[0][2];
    			sum_b += color.matrix[i-1][j+1].b * filter[0][2];
			}

    		if (j > 0)
			{
    			sum_r += color.matrix[i][j-1].r * filter[1][0];
    			sum_g += color.matrix[i][j-1].g * filter[1][0];
    			sum_b += color.matrix[i][j-1].b * filter[1][0];
			}

    		if (j < color.infoheader->width -1)
			{
    			sum_r += color.matrix[i][j+1].r * filter[1][2];
    			sum_g += color.matrix[i][j+1].g * filter[1][2];
    			sum_b += color.matrix[i][j+1].b * filter[1][2];
			}
    		if (i < color.infoheader->height-1 && j > 0)
			{
    			sum_r += color.matrix[i+1][j-1].r * filter[2][0];
    			sum_g += color.matrix[i+1][j-1].g * filter[2][0];
    			sum_b += color.matrix[i+1][j-1].b * filter[2][0];
			}
    		if (i < color.infoheader->height - 1 )
			{
    			sum_r += color.matrix[i+1][j].r * filter[2][1];
    			sum_g += color.matrix[i+1][j].g * filter[2][1];
    			sum_b += color.matrix[i+1][j].b * filter[2][1];
			}
		if (i < color.infoheader->height-1 &&  j < color.infoheader->width - 1)
			{
    			sum_r += color.matrix[i+1][j+1].r * filter[2][2];
    			sum_g += color.matrix[i+1][j+1].g * filter[2][2];
    			sum_b += color.matrix[i+1][j+1].b * filter[2][2];
			}

    		result->matrix[i][j].r = fmax(fmin(sum_r, 255), 0);
    		result->matrix[i][j].g = fmax(fmin(sum_g, 255), 0);
    		result->matrix[i][j].b = fmax(fmin(sum_b, 255), 0);
		}
	}
	return result;
}

void free_image(image* img)
{
	if (img != NULL)
	{
		int height = 0;
		if (img->infoheader != NULL)
		{
			height = img->infoheader->height;
		}

		if (img->matrix != NULL)
		{
			int i;
			for (i = 0; i < height; i++)
			{
				free(img->matrix[i]);
			}
			free(img->matrix);
		}

		if (height > 0)
		{
			free(img->infoheader);
		}

		if (img->header != NULL)
		{
			free(img->header);
		}

		free(img);
	}
}

int main()
{
	char result[64];

	int f1[3][3] = {{-1, -1, -1}, {-1, 8, -1}, {-1, -1, -1}};
	int f2[3][3] = {{0, 1, 0}, {1, -4, 1}, {0, 1, 0}};
	int f3[3][3] = {{1, 0, -1}, {0, 0, 0}, {-1, 0, 1}};

	input_file* input = read_input();
	image* img = read_bitmap(*input);

	build_filename(result, input->filename, "_black_white.bmp");
	image* image_black_and_white = black_white(*img);
	write_bitmap(*input, *image_black_and_white, result);
	free(img);

	image* image_f1 = filter(*image_black_and_white, f1);
	result[0] = '\0';
	build_filename(result, input->filename, "_f1.bmp");
	write_bitmap(*input, *image_f1, result);
	free(image_f1);

	image* image_f2 = filter(*image_black_and_white, f2);
	result[0] = '\0';
	build_filename(result, input->filename, "_f2.bmp");
	write_bitmap(*input, *image_f2, result);
	free(image_f2);

	image* image_f3 = filter(*image_black_and_white, f3);
	result[0] = '\0';
	build_filename(result, input->filename, "_f3.bmp");
	write_bitmap(*input, *image_f3, result);
	free(image_f3);

	free(image_black_and_white);

	return 0;
}
