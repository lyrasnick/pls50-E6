//*************************************************************
//*** ΠΛΣ50 "Βασικές Εξειδικεύσεις σε Θεωρία και Λογισμικό" ***
//***                   Εργασία Ε02                         ***
//***                                                       ***
//***                   Ακ.έτος 2013-14                     ***
//***                                                       ***
//*************************************************************

#include "bmp.h" 

BMP *current_bmp=NULL;			// Καθολική μεταβλητή για την εικόνα που φορτώθηκε

// Η συνάρτηση αυτή φορτώνει μια εικόνα στη μνήμη. Επιστρέφει δείκτη στη δομή
// της εικόνας που φορτώθηκε. Εάν απότυχει η φόρτωση, επιστρέφει NULL.
BMP* load_BMP(char *filename)
{
    BMP *bmp;		// Τοπική μεταβλητή για την εικόνα που θα φορτωθεί
    FILE *in;		// Το αρχείο εισόδου
    int rowsize;   // Το μέγεθος σε bytes μια γραμμής της εικόνας.
	               // Ισούται με τις στήλες της εικόνας επί 
				   // τα bytes του βάθους χρώματος (3 bytes = 24 bits)
	               // συν κάποια υπολοιπόμενα bytes ανά γραμμή για να 
				   // έχουμε πολλαπλάσιο του 4.

	int row, col, color, i; 
	
	unsigned char b;

    in=fopen(filename,"rb"); // ’νοιγμα δυαδικού αρχείου.
    if (in==NULL)
    {    
        printf("Problem in opening file %s.\n",filename);
        return NULL;
    }

	// Δέσμευση μνήμης για τα μεταδεδομένα της εικόνας που θα φορτωθεί
    bmp=(BMP*) malloc(sizeof(BMP));
    if (bmp==NULL)
    {
        printf("Not enough memory to load the image.\n");
        return NULL;
    }
    
    // Οι δύο πρώτοι χαρακτήρες πρέπει να είναι "BM" 
    fread(bmp->BM,2,1,in);
    if (bmp->BM[0]!='B' && bmp->BM[1]!='M') 
	{
        printf("Bad BMP image file.\n");
        free(bmp);
        return NULL;
    }
    
    fread(&bmp->fileSize,4,1,in);
    fread(&bmp->Reserved1,2,1,in);
    fread(&bmp->Reserved2,2,1,in);
    fread(&bmp->imageOffset,4,1,in);
    fread(&bmp->imageHeaderSize,4,1,in);
    fread(&bmp->imageWidth,4,1,in);

	// Υπολογιστός του μεγέθους (σε bytes) μιας σειράς της εικόνας
	// βάσει του πλάτους της
	rowsize=4*((3*bmp->imageWidth+3)/4);       

    fread(&bmp->imageHeight,4,1,in);
    fread(&bmp->colorPlanes,2,1,in);
    
    // Δεν επιτρέπονται εικόνες με πολλά επίπεδα
    if (bmp->colorPlanes!=1)
    {
        printf("This type of BMP (multiple color planes) is not supported.\n");
        free(bmp);
        return NULL;
    }

    // Υποστηρίζονται μόνο εικόνες βάθους χρώματος 24-bit 
    fread(&bmp->colorDepth,2,1,in);
    if (bmp->colorDepth!=24)
    {
        printf("This type of BMP (not 24 bits color depth) is not supported.\n");
        free(bmp);
        return NULL;
    }

    // Δεν υποστηρίζονται συμπιεσμένες εικόνες BMP
    fread(&bmp->compressionMethod,4,1,in);
    if (bmp->compressionMethod!=0)
    {
        printf("This type of BMP (compressed) is not supported.\n");
        free(bmp);
        return NULL;
    }

    fread(&bmp->imageSize,4,1,in);
    fread(&bmp->hPPM,4,1,in);
    fread(&bmp->vPPM,4,1,in);
    fread(&bmp->paletteColors,4,1,in);
    fread(&bmp->paletteImportantColors,4,1,in);

    // Δέσμευση μνήμης για τα δεδομένα 
	bmp->data=(unsigned char*) malloc(bmp->imageSize);
    if (bmp->data==NULL)
    {
        printf("There is not enough memory to load the image\n");
        free(bmp);
        return NULL;
    }

    // Ανάγνωση της εικόνας
    for(row=0;row<bmp->imageHeight;row++)    
    {
        for(col=0;col<bmp->imageWidth;col++)
            for(color=0;color<=2;color++)
                fread(&bmp->data[row*rowsize+3*col+color],
				sizeof(unsigned char),1,in);

		// Ανάγνωση και των έξτρα bytes για την τρέχουσα γραμμή
        for(i=0;i<rowsize-3*bmp->imageWidth;i++)
            fread(&b,1,1,in);
    }

    fclose(in);
    return bmp;
}

// Αποθήκευση εικόνας σε αρχείο στο δίσκο.
// Εάν η αποθήκευση πετύχει, επιστρέφεται 1, αλλιώς επιστρέφεται -1.
int save_BMP(BMP *bmp, char *filename)
{
	printf("Saving not yet implemented\n");
    return 1;
}

// Αντικατάσταση μιας εικόνας από την συμμετρική της ως προς κατακόρυφο άξονα.
// Η νέα εικόνα παίρνει τη θέση της παλιάς.
void vMirror(BMP *bmp)
{
	int row, col, color, i;
	int rowsize;   // Το μέγεθος σε bytes μια γραμμής της εικόνας.
	               // Ισούται με τις στήλες της εικόνας επί 
				   // τα bytes του βάθους χρώματος (3 bytes=24 bits)
	               // συν κάποια υπολοιπόμενα bytes ανά γραμμή για να 
				   // έχουμε πολλαπλάσιο του 4.

	// Δημιουργία νέου πίνακα για τα χρώματα των στοιχείων της εικόνας
	// Τα χρώματα της νέας εικόνας θα δημιουργηθούν στον πίνακα new_data. 
	// Ο προηγούμενους πίνακας data θα διαγραφεί από τή μνήμη στο τέλος της συνάρτησης.
	unsigned char *new_data = (unsigned char*) malloc(bmp->imageSize);
	if (new_data==NULL)
		return;
    
    // Υπολογιστός του μεγέθους (σε bytes) μιας σειράς της εικόνας
	// βάσει του πλάτους της. Οι δύο εικόνες (παλιά και νέα) θα έχουν
	// το ίδιο μέγεθος γραμμής.
    rowsize=4*((3*bmp->imageWidth+3)/4);
    
    for(row=0;row<bmp->imageHeight;row++) {
        for(col=0;col<bmp->imageWidth;col++)
            for(color=0;color<3;color++)
				new_data[row*rowsize+3*col+color]=
					bmp->data[row*rowsize+3*(bmp->imageWidth-col-1)+color];

        // Εγγραφή και των έξτρα bytes της τρέχουσας γραμμής
        for(i=3*bmp->imageWidth;i<rowsize;i++)
			new_data[row*rowsize+i]=bmp->data[row*rowsize+i];    
    }

	// Απελευθέρωση της μνήμης των στοιχείων της παλιάς εικόνας
	free(bmp->data);

	// Ενημέρωση της εικόνας με τη διεύθυνση των στοιχείων της νέας εικόνας
	bmp->data=new_data;
}

// Αντικατάσταση μιας εικόνας από την συμμετρική της ως προς οριζόντιο άξονα.
// Η νέα εικόνα παίρνει τη θέση της παλιάς.
void hMirror(BMP *bmp)
{
	printf("Horizontal mirror not yet implemented\n");
	return;
}

// Περιστροφή της εικόνας κατά 90 μοιρές με φορά αντίθετη της φοράς περιστροφής
// των δεικτών του ρολογιού (θετική φορά περιστροφής)
// Η νέα εικόνα παίρνει τη θέση της παλιάς.
void r90(BMP *bmp)
{
	printf("Rotation not yet implemented\n");
	return;
}


int main(int argc, char *argv[]) {

	char ans=' '; // 

	char filename[255];			// Το όνομα του τρέχοντος αρχείου εικόνας
	
	// Το κεντρικό μενού του προγράμματος
	while (ans!='X' && ans!='x')
    {
		printf("\n\n");
        printf("Menu Options:\n");
		printf("=============\n");
		printf("(O)pen image\n");
		printf("(S)ave image\n");
		printf("Save(A)s image\n");
		printf("(H)orizontal mirror\n");
		printf("(V)ertical mirror\n");
		printf("(R)otate\n");
		printf("(E)xit\n");

		printf("\nSelect an option: ");
		
		fflush(stdin);		// ’δεισμα του buffer εισόδου, ώστε να διαγραφούν τυχών υπολειπόμενοι
							// χαρακτήρες από την προηγούμενη φορά που έδωσε είσοδο ο χρήστης
		
		scanf("%c", &ans);	// Ανάγνωση της επιλογής του χρήστη

		switch (ans) {
		case 'O':
		case 'o':
			printf("Open image selected\n");
			printf("Give the name of the bmp image file:");
			scanf("%s", filename);
			current_bmp=load_BMP(filename);
			break;
		case 'S':
		case 's':
			if (current_bmp!=NULL)
			{
				printf("Save image selected\n");
				save_BMP(current_bmp, filename);
			}
			else
				printf("No image loaded\n");
			break;
		case 'A':
		case 'a':
			if (current_bmp!=NULL)
			{
				printf("Save As selected\n");
				printf("Give a name for the new image file\n");
				scanf("%s", filename);
				save_BMP(current_bmp, filename);
			}
			else
				printf("No image loaded\n");
			break;
		case 'H':
		case 'h':
			if (current_bmp!=NULL)
			{
				printf("Horizontal mirroring selected\n");
				hMirror(current_bmp);
			}
			else
				printf("No image loaded\n");
			break;
		case 'V':
		case 'v':
			if (current_bmp!=NULL)
			{
				printf("Verical mirroring selected\n");
				vMirror(current_bmp);
			}
			else
				printf("No image loaded\n");
			break;
		case 'R':
		case 'r':
			if (current_bmp!=NULL)
			{
				printf("Rotate selected\n");
				r90(current_bmp);
			}
			else
				printf("No image loaded\n");
			break;
		case 'E':
		case 'e':
			printf("Bye...\n");
			return 1;
			break;
		default:
			printf("Wrong selection. Please try again\n");
		}
	}
}
