#include <stdio.h>
#include <time.h>
#include <stdint.h>

#define min(a, b) (a < b ? a : b)
#define BILLION (uint64_t)1000000000

const char *SEQ_FILENAME = "sequence_fast.txt";
const char *ZERO_FILENAME = "zeros_fast.txt";

// Structure to hold sequence data
// a: current value a(n)
// b: previous value a(n-1)
// zeroCount: number of zeros found
typedef struct {
    uint64_t a;
    uint64_t b; 
    int zeroCount;
} SeqData;

// function prototypes
void checkZero(uint64_t seqValue, uint64_t idx, int *pcount, FILE *fp);
void doSequenceInterval(SeqData *pdata, uint64_t X, uint64_t Y, FILE *fpseq, FILE *fpzero);
void runSequence(uint64_t a0, uint64_t a1, uint64_t X, uint64_t Y, const char *seqFilename, const char *zeroFilename);

int main() {
  	// Declare 64-bit unsigned integers
	uint64_t a0, a1;
	uint64_t X, Y;
	
  	// Get initial values from terminal
    printf("\nEnter the lower limit n1:\t");
	scanf("%llu", &X);

    printf("Enter the upper limit n2:\t");
	scanf("%llu", &Y);

	printf("\nEnter the first sequence value a(%llu):\t", X-2);
	scanf("%llu", &a0);
	printf("Enter the second sequence value a(%llu):\t", X-1);
	scanf("%llu", &a1);
    printf("\n");

  	// Run + time the algorithm
	int start = time(NULL);
	runSequence(a0, a1, X, Y, SEQ_FILENAME, ZERO_FILENAME);
	int end = time(NULL);

	printf("\nTime:\t%d\n", end - start);

    // run this line to avoid the terminal closing
	printf("%llu", scanf("%llu", &X));

    return 0;
}

void checkZero(uint64_t seqValue, uint64_t idx, int *pcount, FILE *fp) {
    if (seqValue == 0)
        fprintf(fp, "%i:\t%llu\n", ++(*pcount), idx);
}

void doSequenceInterval(SeqData* pdata, uint64_t X, uint64_t Y, FILE *fpseq, FILE *fpzero) {
    uint64_t a = pdata->a;
    uint64_t b = pdata->b;
    int *pcount = &pdata->zeroCount;

    /* This is the most time critical piece of code  */
    while (X < Y) {
        // b becomes the leading term
        b += a;
        if (b >= X) {
            b -= X;
            checkZero(b, X, pcount, fpzero);
        }
        X++;

        // a becomes the leading term again
        a += b;
        if (a >= X) {
            a -= X;
            checkZero(a, X, pcount, fpzero);
        }
        X++;
    }

    // Handle the last value if the range is odd
    if (X == Y) {
        a += b;
        b = a - b;
        if (a >= X) a -= X;
        checkZero(a, X, pcount, fpzero);
        X++;
    }

    /*

    The above loop is a faster but equivalent version of the following:

    uint64_t temp;

    while (X <= Y) {
        temp = a;
        a = (a + b) % X;
        b = temp;
        checkZero(a, X++, pcount, fpzero);
    }
    
    */

    pdata->a = a;
    pdata->b = b;
}

void runSequence(uint64_t a0, uint64_t a1, uint64_t X, uint64_t Y, const char *seqFilename, const char *zeroFilename) {
    SeqData data = {a1, a0, 0};

    // tracker variables - align the limits to billions
    uint64_t billionCount = 1 + (X-1)/BILLION;
    uint64_t limit = min(billionCount * BILLION, Y);

    // initialise the file pointers - no error checking needed as "w" will create the files if necessary
    FILE* fpseq = fopen(seqFilename, "w");
    FILE* fpzero = fopen(zeroFilename, "w");

    // set file output to unbuffered for real-time logging
    setbuf(fpseq, NULL);
    setbuf(fpzero, NULL);

    fprintf(fpseq, "A079777 Sequence Values:\n");
    fprintf(fpzero, "Zero Values Found:\n\n");

    checkZero(a0, X-2, &data.zeroCount, fpzero);
    checkZero(a1, X-1, &data.zeroCount, fpzero);
    
    while (X < Y) {
        doSequenceInterval(&data, X, limit, fpseq, fpzero);
        
        // output values every 50 billion
        if (billionCount % 50 == 0 && Y >= BILLION * 50) {
            fprintf(fpseq, "\n-- At n = %llu billion --\n", billionCount);
            fprintf(fpseq, "-\ta(n-1) = %llu\n", data.b);
            fprintf(fpseq, "-\ta(n) = %llu\n", data.a);
        }
        
        X = limit + 1;
        limit += BILLION;
        billionCount++;
        limit = min(limit, Y);
    }

  	fprintf(fpseq, "\n-- DONE --\n");
	fprintf(fpseq, "\nThe last two values are:\n", Y);
	fprintf(fpseq, "- a(%llu) = %llu\n", Y-1, data.b);
	fprintf(fpseq, "- a(%llu) = %llu\n", Y, data.a);

    fclose(fpseq);
    fclose(fpzero);
}
