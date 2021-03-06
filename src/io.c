/* -*- mode: C; c-basic-offset: 4 -*- */
/* ex: set shiftwidth=4 tabstop=4 expandtab: */
/*
 * Copyright (c) 2010-2011, Georgia Tech Research Corporation
 * All rights reserved.
 *
 * Author(s): Neil T. Dantam <ntd@gatech.edu>
 * Georgia Tech Humanoid Robotics Lab
 * Under Direction of Prof. Mike Stilman <mstilman@cc.gatech.edu>
 *
 *
 * This file is provided under the following "BSD-style" License:
 *
 *
 *   Redistribution and use in source and binary forms, with or
 *   without modification, are permitted provided that the following
 *   conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 *   CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *   INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 *   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 *   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 *   USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 *   AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *   POSSIBILITY OF SUCH DAMAGE.
 *
 */


#include "amino.h"
#include <unistd.h>
#include <ctype.h>

AA_API ssize_t aa_read_realloc(int fd, void **buf,
                           size_t off, size_t *max) {
    size_t n = *max - off;
    if( n < 16 ) {
        *max = AA_MAX( 2*(*max), (size_t)16 );
        *buf = realloc(*buf, *max);
        n = *max - off;
    }
    return read( fd, ((uint8_t*)*buf)+off, n );
}


char *aa_io_getline( FILE *fin, struct aa_mem_region *reg )
{
    size_t n = 0;
    do {
        int i = fgetc(fin);
        if( '\n' == i || EOF == i ) {
            char * str = (char*)aa_mem_region_alloc( reg, n+1 );
            str[n] = '\0';
            return str;
        }
        n++;
        char *ptr = (char*)aa_mem_region_tmprealloc(reg, n);
        ptr[n-1] = (char)i;
    } while(1);
}

size_t aa_io_d_parse( const char *str, struct aa_mem_region *reg, double **X, char **lendptr )
{
    char *ptr = (char*)str; // promise not to change str

    size_t i = 0;
    for(; ; i ++ ) {
        if( NULL == ptr || AA_IO_ISCOMMENT(*ptr) ) break;

        errno = 0;
        char *endptr;

        double x = strtod(ptr, &endptr);
        if( ptr == endptr ) break;

        *X = (double*)aa_mem_region_tmprealloc(reg, (1+i)*sizeof(double));
        (*X)[i] = x;

        endptr = aa_io_skipblank(endptr);

        ptr = endptr;
    }
    void *a_ptr = aa_mem_region_alloc(reg, i*sizeof(double));
    assert( 0 == i || a_ptr == *X );

    if( lendptr ) *lendptr = ptr;

    return i;
}


char *aa_io_skipblank( const char *str )
{
    const char *ptr = str;
    while( isblank(*ptr) ) ptr++;
    return (char*)ptr;
}


size_t aa_io_parsevector( const char *str, size_t n, double *X, size_t incx, char **lendptr )
{
    char *ptr = (char*)str; // promise not to change str

    size_t i = 0;
    for(; i < n; i ++ ) {
        if( NULL == ptr || AA_IO_ISCOMMENT(*ptr) ) break;

        errno = 0;
        char *endptr;
        X[i*incx] = strtod(ptr, &endptr);

        if( ptr == endptr ) break;

        endptr = aa_io_skipblank(endptr);

        ptr = endptr;
    }

    if( lendptr ) *lendptr = ptr;

    return i;
}

size_t aa_io_getvector( FILE *fin, struct aa_mem_region *reg, size_t n, double *X, size_t incx )
{
    if ( NULL == reg ) reg = aa_mem_region_local_get();

    char *line = aa_io_getline( fin, reg );
    size_t i = 0;
    if(line) {
        i = aa_io_parsevector( line, n, X, incx, NULL );
        aa_mem_region_pop( reg, line );
    }

    return i;
}


size_t aa_io_fread_matrix_fix( FILE *fin, size_t m, size_t n,
                               double *A, size_t lda )
{
    struct aa_mem_region *reg = aa_mem_region_local_get();
    void *ptr = aa_mem_region_tmpalloc(reg,1);

    size_t cnt = 0;
    for( size_t i = 0; i < m; i ++ ) {
        cnt += aa_io_getvector( fin, reg,
                                n, &AA_MATREF(A, lda, i, 0), lda );
    }

    aa_mem_region_pop( reg, ptr );

    return cnt;
}

ssize_t aa_io_fread_matrix_heap( FILE *fin, size_t n,
                                 double **A, size_t *_elts )
{
    size_t tmp_elts = 0;
    size_t *elts = _elts ? _elts : &tmp_elts;

    struct aa_mem_region *reg = aa_mem_region_local_get();
    if( NULL == *A || 0 == *elts ) {
        *elts = n;
        *A = (double*) malloc( *elts * sizeof(double) );
    }

    size_t lineno = 0;
    size_t read_elts = 0;
    while( !feof(fin) ) {
        lineno++;
        char *line = aa_io_getline(fin, reg);
        if( NULL == line ) continue;

        char *line2 = aa_io_skipblank(line);
        if('\0' == *line2 || AA_IO_ISCOMMENT(*line2)) continue;

        // maybe realloc
        if( *elts < read_elts + n ) {
            *elts *= 2;
            *A = (double*) realloc(*A, *elts * sizeof(double) );
        }

        // parse
        size_t i = aa_io_parsevector( line2, n, (*A)+read_elts, 1, NULL );
        if( i != n ) return -(ssize_t)lineno;

        read_elts += i;

        aa_mem_region_pop(reg, line);
    }
    return (ssize_t)(read_elts / n);
}

/** General vector write. */
ssize_t aa_io_d_print( FILE *fout, size_t n,
                       const double *x, size_t incx )
{

    for( size_t i = 0; i < n; i ++ )
        fprintf(fout, "%f%s", x[i*incx], (i<n-1) ? "\t" : ""  );
    fprintf(fout, "\n");


    return 0;
}

/** General matrix write. */
ssize_t aa_io_d_gemp( FILE *fout, AA_CBLAS_ORDER file_order, size_t m, size_t n,
                      const double *A, size_t lda )
{
    if( CblasColMajor == file_order ) {
        // col-per-line
        for( size_t col = 0; col < n; col ++ ) {
            aa_io_d_print( fout, m, AA_MATCOL(A,lda,col), 1 );
        }
    } else {
        // row-per-line
        for( size_t row = 0; row < m; row ++ ) {
            aa_io_d_print( fout, n, A+row, lda );
        }
    }
    return 0;
}
