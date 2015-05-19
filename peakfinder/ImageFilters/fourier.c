//REF: https://github.com/ginrou/ginrou-private/blob/1542f56865b55f10b1c17dbd825b859be20b5a67/src/

#include "fourier.h"

/* Fast Fourier Transform */

#define PI 3.14159265358979323846

static void make_sintbl(int n, double sintbl[]) {
    int i;
    int n2, n4, n8;
    double c, s, dc, ds, t;

    n2 = n / 2;
    n4 = n / 4;
    n8 = n / 8;
    t = sin(PI / n);
    dc = 2 * t * t;
    ds = sqrt(dc * (2 - dc));
    t = 2 * dc;
    c = sintbl[n4] = 1;
    s = sintbl[0] = 0;

    for (i = 1; i < n8; i++) {
        c -= dc;
        dc += t * c;
        s += ds;
        ds -= t * s;
        sintbl[i] = s;
        sintbl[n4 - i] = c;
    }
    if (n8 != 0) sintbl[n8] = sqrt(0.5);
    for (i = 0; i < n4; i++) sintbl[n2 - i] = sintbl[i];
    for (i = 0; i < n2 + n4; i++) sintbl[i + n2] = -sintbl[i];
}

static void make_bitrev(int n, int bitrev[]) {
    int i, j, k, n2;

    n2 = n / 2;
    i = j = 0;
    for (;;) {
        bitrev[i] = j;
        if (++i >= n) break;
        k = n2;
        while (k <= j) {
            j -= k;
            k /= 2;
        }
        j += k;
    }
}

static int fft(int n, double* re, double* im) {
    static int last_n = 0;
    static int *bitrev = NULL;
    static double *sintbl = NULL;

    int i, j, k, ik, h, d, k2, n4, inverse;
    double t, s, c, dre, dim;

    // FFT or IFFT //
    if (n < 0) {
        n = -n;
        inverse = 1;
    } else inverse = 0;
    n4 = n / 4;

    // allocation //
    if (n != last_n || n == 0) {
        last_n = n;
        if (sintbl != NULL) free(sintbl);
        if (bitrev != NULL) free(bitrev);
        if (n == 0) return 0;
        sintbl = malloc((n + n4) * sizeof ( double));
        bitrev = malloc(n * sizeof ( int));
        if (sintbl == NULL || bitrev == NULL) {
            fprintf(stderr, "Memory allocation error!\n");
            return 1;
        }
        make_sintbl(n, sintbl);
        make_bitrev(n, bitrev);
    }

    // bit reverse //
    for (i = 0; i < n; i++) {
        j = bitrev[i];
        if (i < j) {
            t = re[i];
            re[i] = re[j];
            re[j] = t;
            t = im[i];
            im[i] = im[j];
            im[j] = t;
        }
    }

    // transform //
    for (k = 1; k < n; k = k2) {
        h = 0;
        k2 = k + k;
        d = n / k2;
        for (j = 0; j < k; j++) {
            c = sintbl[h + n4];
            if (inverse) s = -sintbl[h];
            else s = sintbl[h];
            for (i = j; i < n; i += k2) {
                ik = i + k;
                dre = s * im[ik] + c * re[ik];
                dim = c * im[ik] - s * re[ik];
                re[ik] = re[i] - dre;
                re[i] += dre;
                im[ik] = im[i] - dim;
                im[i] += dim;
            }
            h += d;
        }
    }
    if (!inverse)
        for (i = 0; i < n; i++) {
            re[i] /= n;
            im[i] /= n;
        }
    return 0;
}

/* 2D Fast Fourier Transform for PPM : Processing part */


void fourier(Complex** out, uint8** in, int FFT_SIZE) {
    int y, x;
    double *re, *im;
    
    //memory allocation
  
omp_set_num_threads(4);
    // FFT //
#pragma omp parallel for default(shared) private(x,re,im)
    for (y = 0; y < FFT_SIZE; y++) {
          re = (double*) malloc (FFT_SIZE * sizeof(double));
          im = (double*) malloc (FFT_SIZE * sizeof(double));
        for (x = 0; x < FFT_SIZE; x++) {
            re[x] = (double) in[y][x];
            im[x] = 0;
        }
        if (fft(FFT_SIZE, re, im)) exit(0);
        for (x = 0; x < FFT_SIZE; x++) {
            out[y][x].Re = re[x];
            out[y][x].Im = im[x];
        }
          free(re);
          free(im);
    }
    #pragma omp parallel for default(shared) private(y,re,im)
    for (x = 0; x < FFT_SIZE; x++) {
          re = (double*) malloc (FFT_SIZE * sizeof(double));
          im = (double*) malloc (FFT_SIZE * sizeof(double));
        for (y = 0; y < FFT_SIZE; y++) {
            re[y] = out[y][x].Re;
            im[y] = out[y][x].Im;
        }
        if (fft(FFT_SIZE, re, im)) exit(0);
        for (y = 0; y < FFT_SIZE; y++) {
            out[y][x].Re = re[y];
            out[y][x].Im = im[y];
        }
          free(re);
          free(im);
    }
}

void fourierFFTW(fftw_complex* out, uint8** in, int FFT_SIZE) {
    int i,j;
    fftw_complex* real = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * FFT_SIZE * FFT_SIZE);
    
    // Copy in image data as real values for the transform.
    for(i=0; i<FFT_SIZE; i++) {
        for(j=0; j<FFT_SIZE; j++){
            real[i*FFT_SIZE+j][0] = in[i][j];
            real[i*FFT_SIZE+j][1] = 0;
        }
    }
    // Transform to frequency space.
    fftw_plan pFwd = fftw_plan_dft_2d(FFT_SIZE, FFT_SIZE, real, out, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(pFwd);
    fftw_destroy_plan(pFwd);

    // Cleanup.
    fftw_free(real);
}

void inverseFourier(uint8** out, Complex** in, int FFT_SIZE) {
    int y, x;
    double *re, *im;
    double **Real, **Imag;
    omp_set_num_threads(4);
    //memory allocation

    Real = (double**) malloc (FFT_SIZE * sizeof(double*));
    Imag = (double**) malloc (FFT_SIZE * sizeof(double*));
    for(x=0; x<FFT_SIZE; x++){
        Real[x] = (double*) malloc (FFT_SIZE * sizeof(double));
        Imag[x] = (double*) malloc (FFT_SIZE * sizeof(double));
    }

    // IFFT //
#pragma omp parallel for default(shared) private(y,re,im)
    for (x = 0; x < FFT_SIZE; x++) {
        re = (double*) malloc (FFT_SIZE * sizeof(double));
        im = (double*) malloc (FFT_SIZE * sizeof(double));
        for (y = 0; y < FFT_SIZE; y++) {
            re[y] = in[y][x].Re;
            im[y] = in[y][x].Im;
        }
        if (fft(-FFT_SIZE, re, im)) exit(0);
        for (y = 0; y < FFT_SIZE; y++) {
            Real[y][x] = re[y];
            Imag[y][x] = im[y];
        }
        free(re);
        free(im);
    }
    #pragma omp parallel for default(shared) private(x,re,im)
    for (y = 0; y < FFT_SIZE; y++) {
        re = (double*) malloc (FFT_SIZE * sizeof(double));
        im = (double*) malloc (FFT_SIZE * sizeof(double));
        for (x = 0; x < FFT_SIZE; x++) {
            re[x] = Real[y][x];
            im[x] = Imag[y][x];
        }
        if (fft(-FFT_SIZE, re, im)) exit(0);
        for (x = 0; x < FFT_SIZE; x++) {

            int i = (int) re[x];

            if (i > 255) {
                out[y][x] = 255;
            } else if (i < 0) {
                out[y][x] = 0;
            } else {
                out[y][x] = i;
            }
            
            //out[y][x] = re[x];
            
            Real[y][x] = re[x];
            Imag[y][x] = im[x];
        }
        free(re);
        free(im);
    }

  
}

void inverseFourierFFTW(uint8** out, fftw_complex* in, int FFT_SIZE) {
    int i,j;
    int value;
    
    fftw_complex* real = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * FFT_SIZE * FFT_SIZE);

    // Inverse transform.
    fftw_plan pFwd = fftw_plan_dft_2d(FFT_SIZE, FFT_SIZE, in, real, FFTW_BACKWARD, FFTW_ESTIMATE);
    fftw_execute(pFwd);
    fftw_destroy_plan(pFwd);
    
    // Copy out image data as real values from the inverse transform.
    for(i=0; i<FFT_SIZE; i++) {
        for(j=0; j<FFT_SIZE; j++){
            //get real component
            value = real[i*FFT_SIZE+j][0];

            //truncate value
            if (value > 255) {
                out[i][j] = 255;
            } else if (value < 0) {
                out[i][j] = 0;
            } else {
                out[i][j] = value;
            }
        }
    }

    // Cleanup.
    fftw_free(real);
}

void fourierSpectrumImage(uint8** out, Complex** in, int FFT_SIZE) {
    int x, y, i;
    int max_exp = -100, exp;
    //double fra;
    int spectrum[FFT_SIZE][FFT_SIZE];
omp_set_num_threads(4);
    // Power Spectrum //
    fprintf(stderr, "Calculating power spectrum...\n");
#pragma omp parallel for default(shared) private(x,exp,spectrum)
    for (y = 0; y < FFT_SIZE; y++) {
        for (x = 0; x < FFT_SIZE; x++) {
            exp = log10(compAbs(in[y][x])) * 100.0;
            spectrum[y][x] = exp;
            #pragma omp critical
            {
                if (max_exp < exp) {
                    max_exp = exp;
                }
            }
        }
    }

    fprintf(stderr, "max_exp = %d\n", max_exp);

    for (y = 0; y < FFT_SIZE; y++) {
        for (x = 0; x < FFT_SIZE; x++) {
            int rx, ry;

            ry = (y + FFT_SIZE / 2) % FFT_SIZE;
            rx = (x + FFT_SIZE / 2) % FFT_SIZE;
#if 0
            i = spectrum[ry][rx] - max_exp + 255;
#else
            //i = spectrum[ry][rx] + 128;
            i = spectrum[ry][rx] + 255;
#endif
            if (i < 0) {
                out[y][x] = 0;
            } else if (i > 255) {
                out[y][x] = 255;
            } else {
                out[y][x] = i;
            }
        }
    }
}

int fourierSpectrumImageFFTW(uint8** out, fftw_complex* in, int FFT_SIZE) {
    int x, y, i;
    int max_exp = -100, exp;
    //double fra;
    int spectrum[FFT_SIZE][FFT_SIZE];

    // Power Spectrum //
    fprintf(stderr, "Calculating power spectrum...\n");

    for (y = 0; y < FFT_SIZE; y++) {
        for (x = 0; x < FFT_SIZE; x++) {
            exp = compAbsFFTW(in[y*FFT_SIZE+x]);
            if(exp>0){
                exp = log10(exp) * 100.0;
            }
            spectrum[y][x] = exp;
            if (max_exp < exp) {
                max_exp = exp;
            }
        }
    }

    fprintf(stderr, "max_exp = %d\n", max_exp);

    for (y = 0; y < FFT_SIZE; y++) {
        for (x = 0; x < FFT_SIZE; x++) {
            int rx, ry;

            ry = (y + FFT_SIZE / 2) % FFT_SIZE;
            rx = (x + FFT_SIZE / 2) % FFT_SIZE;
#if 1
            i = spectrum[ry][rx] - max_exp/2 + 64;
#else
            //i = spectrum[ry][rx] + 128;
            i = spectrum[ry][rx];// + 255;
#endif
            if (i < 0) {
                out[y][x] = 0;
            } else if (i > 255) {
                out[y][x] = 255;
            } else {
                out[y][x] = i;
            }
        }
    }
    return max_exp;
}

int max_exp_value(fftw_complex* in, int FFT_SIZE) {
    int x, y;
    double im_max, re_max, im_abs, re_abs, im_abs_max, re_abs_max;
    im_max = re_max = im_abs_max = re_abs_max = 0;

    // Power Spectrum //
    fprintf(stderr, "Calculating power spectrum...\n");

    for (y = 0; y < FFT_SIZE; y++) {
        for (x = 0; x < FFT_SIZE; x++) {
            
            if(y==0 && x==0){//first iteration
                re_max = in[0][0];
                im_max = in[0][1];
                re_abs_max = fabs(in[0][0]);
                im_abs_max = fabs(in[0][1]);
            } else {
                re_abs = fabs(in[x*FFT_SIZE+y][0]);
                im_abs = fabs(in[x*FFT_SIZE+y][1]);
                //if abs of both is greater, or
                //is some component is lower, but the sum of square values is greater
                if((re_abs>re_abs_max && im_abs>im_abs_max) ||
                   ((re_abs*re_abs + im_abs*im_abs) > (re_abs_max*re_abs_max + im_abs_max*im_abs_max))){
                    //save new values
                    re_abs_max = re_abs;
                    im_abs_max = im_abs;
                    re_max = in[x*FFT_SIZE+y][0];
                    im_max = in[x*FFT_SIZE+y][1];
                }//otherwise, skip to next complex
            }
        }
    }
    return log10(sqrt(re_max*re_max + im_max*im_max)) * 100.0;
}

void fourier1D(Complex out[], double in[], int FFT_SIZE) {
    int x;
    double *re, *im;
    
    //memory allocation
    re = (double*) malloc (FFT_SIZE * sizeof(double));
    im = (double*) malloc (FFT_SIZE * sizeof(double));

    for (x = 0; x < FFT_SIZE; x++) {
        re[x] = (double) in[x];
        im[x] = 0;
    }
    if (fft(FFT_SIZE, re, im)) exit(0);
    for (x = 0; x < FFT_SIZE; x++) {
        out[x].Re = re[x];
        out[x].Im = im[x];
    }
}

void inverseFourier1D(double out[], Complex in[], int FFT_SIZE) {
    int x;
    double *re, *im;
    
    //memory allocation
    re = (double*) malloc (FFT_SIZE * sizeof(double));
    im = (double*) malloc (FFT_SIZE * sizeof(double));

    // IFFT //
    for (x = 0; x < FFT_SIZE; x++) {
        re[x] = in[x].Re;
        im[x] = in[x].Im;
    }
    if (fft(-FFT_SIZE, re, im)) exit(0);
    for (x = 0; x < FFT_SIZE; x++) {
        int i = (int) re[x];

        if (i > 255) {
            out[x] = 255;
        } else if (i < 0) {
            out[x] = 0;
        } else {
            out[x] = i;
        }
    }
}
