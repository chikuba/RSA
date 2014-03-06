#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <gmp.h>

#define MAX_FILE 200
#define BITSIZE  1024
#define BUFFER   100
#define BASE     10

/* RSA-DE/ENCRYPTER ***/
/* MADE BY JENNIFER NORDWALL, 2009 ***/

/* FIFO ***/
void    encrypt_to_file                 (mpz_t d,       mpz_t e,        mpz_t n         );
void    decrypt_to_file                 (mpz_t d,       mpz_t e,        mpz_t n         );
void    write_to_file_while_running     (mpz_t convert, int choice,     char *file_name );
int     get_keys_from_file              (mpz_t d,       mpz_t e,        mpz_t n         );
void    save_keys_to_file               (mpz_t d,       mpz_t e,        mpz_t n,        int bitsize);
void    generate_keys                   (mpz_t d,       mpz_t e,        mpz_t n,        int bitsize);
void    possible_keys                   (mpz_t d,       mpz_t e,        mpz_t n,        int bitsize);

int main(void) {
	mpz_t d, e, n;
	mpz_init(d); mpz_init(e); mpz_init(n);

	int choice, quit = 0;

	while(quit != 1) {
		printf("\n-- Meny --\n");
		printf("1. Encrypt file\n");
		printf("2. Decrypt file\n");
		printf("3. Quit\n");
		printf("> ");
		scanf("%d", &choice);
		fflush(stdin);
		
		switch(choice) {
            case 1:
				encrypt_to_file(d, e, n);
                break;
            case 2:
                decrypt_to_file(d, e, n);
                break;
            case 3:
                quit = 1;
                break;
            default:
                printf("\nChoose a number between 1-3.\n\n");
		}
	}
	return 0;
}

void encrypt_to_file(mpz_t d, mpz_t e, mpz_t n) {
    FILE *file;
    char char_convert, file_name_to[MAX_FILE], file_name_from[MAX_FILE], buf[BUFFER];
    int bitsize;
    mpz_t convert; mpz_init(convert);

    /* DEFINE BITSIZE ***/
    do {
        printf("Enter bitsize (8 - %d): \n", BITSIZE); fflush(stdout);
        fgets(buf, BUFFER, stdin);
    } while((sscanf(buf, "%d", &bitsize) < 0) || (bitsize < 8) || (bitsize > BITSIZE));

    /* CHOOSE THE FILE TO WRITE TO ***/
    puts("Encrypt to (the output file):");
    scanf("%s", file_name_to);

    file = fopen(file_name_to, "wt");
    fprintf(file, "%d\n", bitsize);
    fclose(file);

    /* MAKE KEYS ***/
    generate_keys(d, e, n, bitsize);

    /* START SCAN CHARS AND CONVERT ***/
    puts("Encrypt:");
    scanf("%s", file_name_from);

    if((file = fopen(file_name_from, "r")) == NULL)
        puts("\nCannot open file. (returning...) ");

    else {
        file = fopen(file_name_from, "r");
		
        while(fscanf(file, "%c", &char_convert) != EOF) {
            mpz_set_ui(convert, (int)char_convert);
            mpz_powm(convert, convert, e, n);
            write_to_file_while_running(convert, 0, file_name_to);
        }
        puts("\nSuccess!");
    }
    fclose(file);
}

void decrypt_to_file(mpz_t d, mpz_t e, mpz_t n) {
    FILE *file;
    mpz_t line_convert; mpz_init(line_convert);
	int line_size;
	char file_name_to[MAX_FILE], file_name_from[MAX_FILE];

    /* MAKE SURE THEY FIND THE KEYS ***/
    if(get_keys_from_file(d, e, n) == 1) {
        /* CREATE THE FILE TO DECRYPT TO ***/
        puts("Decrypt to (the output file):");
        scanf("%s", file_name_to);

        file = fopen(file_name_to, "wt");
        fclose(file);

        /* CHOOSE THE FILE TO DECRYPT ***/
        puts("Decrypt:");
        scanf("%s", file_name_from);

        if((file = fopen(file_name_from, "r")) == NULL)
            puts("\nCannot open file. (returning...) ");

        else {
            file = fopen(file_name_from, "r");

            fscanf(file, "%d%*[^\n]", &line_size);
            char line[line_size];

            while(fscanf(file, "%s%*[^\n]", line) != EOF) {
                mpz_set_str(line_convert, line, BASE);
                mpz_powm(line_convert, line_convert, d, n);
                write_to_file_while_running(line_convert, 1, file_name_to);
            }
            puts("\nSuccess!");
        }
        fclose(file);
    }
}

void write_to_file_while_running(mpz_t convert, int choice, char *file_name) {
    FILE *file; size_t size;
    unsigned long letter;

    /* DECRYPT ***/
    if(choice == 1) {
        file = fopen(file_name, "a");
        letter = mpz_get_ui(convert);
        fprintf(file, "%c", (char)letter);
    }
    /* ENCRYPT ***/
    else {
        file = fopen(file_name, "a");
        size = mpz_out_str(file, BASE, convert);
        fprintf(file, "\n");
    }
    fclose(file);
}

int get_keys_from_file(mpz_t d, mpz_t e, mpz_t n) {
    int d_size, e_size, n_size;
    FILE *file;
    char file_name[MAX_FILE];
    puts("File with keys:");
    scanf("%s", file_name);


    if((file = fopen(file_name, "r")) == NULL) {
        puts("\nCannot open file. (returning...) ");

        return 0;
    }

    else {
        file = fopen(file_name, "r");

        /* GET d, e & n FROM FILE ***/
        fscanf(file, "%d%*[^\n]", &d_size);
        char d_line[d_size];
        fscanf(file, "%s%*[^\n]", d_line);
        mpz_set_str(d, d_line, BASE);

        fscanf(file, "%d%*[^\n]", &e_size);
        char e_line[e_size];
        fscanf(file, "%s%*[^\n]", e_line);
        mpz_set_str(e, e_line, BASE);

        fscanf(file, "%d%*[^\n]", &n_size);
        char n_line[n_size];
        fscanf(file, "%s%*[^\n]", n_line);
        mpz_set_str(n, n_line, BASE);

        return 1;
    }
    fclose(file);
}

void save_keys_to_file(mpz_t d, mpz_t e, mpz_t n, int bitsize) {
    FILE *file;
    char file_name[MAX_FILE];
    puts("Save keys to:");
    scanf("%s", file_name);

    file = fopen(file_name, "w");

    fprintf(file, "%d\n", bitsize);
    mpz_out_str(file, BASE, d);

    fprintf(file, "\n%d\n", (bitsize / 2));
    mpz_out_str(file, BASE, e);

    fprintf(file, "\n%d\n", bitsize);
    mpz_out_str(file, BASE, n);

    fclose(file);
}

void generate_keys(mpz_t d, mpz_t e, mpz_t n, int bitsize) {
    mpz_t start, result;
    mpz_init(start); mpz_init(result);
    mpz_set_ui(start, 56); // 56 == random number

    /* CONTROL ***/
    while(mpz_cmp(start, result) != 0) {
        possible_keys(d, e, n, bitsize);
        /* encrypt ***/
        mpz_powm(result, start, e, n);
        /* decrypt ***/
        mpz_powm(result, result, d, n);
        /* make sure it's the same as the start ***/
    }
    save_keys_to_file(d, e, n, bitsize);
}

void possible_keys(mpz_t d, mpz_t e, mpz_t n, int bitsize) {
    mpz_t max, max_half_bitsize, prime1, prime2, tn;

    /* COMPUTE RANDOM PRIME X2 ***/
	mpz_init(max); mpz_init(max_half_bitsize); mpz_init(prime1); mpz_init(prime2);

	mpz_ui_pow_ui(max, 2, bitsize);
	mpz_ui_pow_ui(max_half_bitsize, 2, (bitsize / 2));

    gmp_randstate_t rand_state;
    gmp_randinit_default(rand_state);
    gmp_randseed_ui(rand_state, time(NULL));

    mpz_urandomm(prime1, rand_state, max);
    mpz_urandomm(prime2, rand_state, max);

    while(mpz_probab_prime_p(prime1, 100) == 0)
        mpz_urandomm(prime1, rand_state, max);

    while((mpz_probab_prime_p(prime2, 100) == 0) || (mpz_cmp(prime1, prime2) == 0))
        mpz_urandomm(prime2, rand_state, max);

    /* COMPUTE n ***/
    mpz_mul(n, prime1, prime2);

    /* COMPUTE THE totient(n) = (prime1 - 1) * (prime2 - 1) ***/
    mpz_init(tn);
    mpz_sub_ui(prime1, prime1, 1);
    mpz_sub_ui(prime2, prime2, 1);
    mpz_mul(tn, prime1, prime2);

    /* e IS COMPRIME TO totient(n). IN THIS CASE JUST AN ORDINARY PRIME. NICE ***/
    mpz_urandomm(e, rand_state, max_half_bitsize);
    while(mpz_probab_prime_p(e, 100) == 0)
        mpz_urandomm(e, rand_state, max_half_bitsize);

    /* COMPUTE d; de = 1 mod(t(n)) ***/
    mpz_invert(d, e, tn); // compute the invers of e % tn. NOTE! d could == 0. somethimes d dosent exist...
}
