#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// � ���� ��� �������
typedef struct
{
	// �� ������������ ��� �������
    char				BM[2];
    unsigned int		fileSize;
    short int			Reserved1;
    short int			Reserved2;
    unsigned int		imageOffset;
	//
    unsigned int		imageHeaderSize;
    int					imageWidth;
    int					imageHeight;
    unsigned short int	colorPlanes;
    unsigned short int	colorDepth;
    unsigned int		compressionMethod;
    unsigned int		imageSize;
    unsigned int		hPPM;
    unsigned int		vPPM;
    unsigned int		paletteColors;
    unsigned int		paletteImportantColors;
	
	// �� �������� ��� �������. �������: ����������
	// ��������� �������� ������ ��� �� ��������.
    unsigned char		*data;
} BMP;	
