BCM2835 "GPU_FFT" release 3.0 by Andrew Holme, 2015.

GPU_FFT is an FFT library for the Raspberry Pi which exploits the BCM2835 SoC
3D hardware to deliver ten times more data throughput than is possible on the
700 MHz ARM of the Pi 1.  Kernels are provided for all power-of-2 FFT lengths
between 256 and 4,194,304 points inclusive.  A transpose function, which also
uses the 3D hardware, is provided to support 2-dimensional transforms.


# Accuracy

GPU_FFT uses single-precision floats for data and twiddle factors.  The output
is not scaled.  The relative root-mean-square (rms) error in parts-per-million
(ppm) for different transform lengths (N) is typically:

|log2(N) |  8    |  9    | 10    | 11    | 12    | 13    | 14    | 15|
| ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | 
|ppm rms |  0.33 |  0.46 |  0.52 |  0.59 |  0.78 |  0.83 |  0.92 |  0.98|

|log2(N) | 16    | 17    | 18    | 19    | 20    |  21   | 22|
| ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | 
|ppm rms |  1.0  |  1.3  |  1.3  |  1.4  |  1.5  |  1.5  |  1.5|

Accuracy has improved significantly over previous releases at the expense of a
small (2%) performance hit; however, FFTW is still one order of magnitude more
accurate than GPU_FFT.


# Throughput

GPU_FFT 1.0 had to be invoked through a "mailbox" which added a 100us overhead
on every call.  To mitigate this, batches of transforms could be submitted via
a single call.  GPU_FFT now avoids this 100us overhead by poking GPU registers
directly from the ARM if total batch runtime will be short; but still uses the
mailbox for longer jobs to avoid busy waiting at 100% CPU for too long.

Typical per-transform runtimes for batch sizes of 1 and 10; and comparative
figures for FFTW (FFTW_MEASURE mode) on a Pi 1 with L2 cache enabled are:

|log2(N) |   8   |   9   |  10   |  11   |  12  |  13  |  14  |  15|
| ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | 
  |    1 | 0.033 | 0.049 | 0.070 | 0.12  | 0.25 | 0.61 |  1.2 |  3.5|
 |    10 | 0.017 | 0.029 | 0.049 | 0.11  | 0.27 | 0.66 |  1.2 |  3.3|
|   FFTW | 0.092 | 0.22  | 0.48  | 0.95  | 3.0  | 5.1  | 12   | 31|

|log2(N) |  16  |  17 |  18 |  19 |   20 |   21 |   22   |    
| ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- |
 |     1 |  7.0 |  17 |  43 |  97 |  194 |  388 |  786    |    
|   FFTW | 83   | 180 | 560 | 670 | 1600 | 3400 | 8800     |   
 
 All times in  milliseconds.

# API functions

    gpu_fft_prepare()       Call once to allocate memory and initialise data
                            structures.  Returns 0 for success.

    gpu_fft_execute()       Call one or more times to execute a previously
                            prepared FFT batch.  Returns 0 for success.

    gpu_fft_release()       Call once to release resources after use.
                            GPU memory is permanently lost if not freed.


## Parameters

    int mb          Mailbox file descriptor obtained by calling mbox_open()

    int log2_N      log2(FFT length) = 8 to 22

    int direction   FFT direction:  GPU_FFT_FWD for forward FFT
                                    GPU_FFT_REV for inverse FFT

    int jobs        Number of transforms in batch = 1 or more

    GPU_FFT **      Output parameter from prepare: control structure.
    GPU_FFT *       Input parameter to execute and release


## Data format

Complex data arrays are stored as alternate real and imaginary parts:

    struct GPU_FFT_COMPLEX {
        float re, im;
    };

The GPU_FFT struct created by gpu_fft_prepare() contains pointers to the input
and output arrays:

    struct GPU_FFT {
       struct GPU_FFT_COMPLEX *in, *out;

When executing a batch of transforms, buffer pointers are obtained as follows:

    struct GPU_FFT *fft = gpu_fft_prepare( ... , jobs);
    for (int j=0; j<jobs; j++) {
       struct GPU_FFT_COMPLEX *in  = fft->in  + j*fft->step;
       struct GPU_FFT_COMPLEX *out = fft->out + j*fft->step;

GPU_FFT.step is greater than FFT length because a guard space is left between
buffers for caching and alignment reasons.

GPU_FFT performs multiple passes between ping-pong buffers.  The final output
lands in the same buffer as input after an even number of passes.  Transforms
where log2_N=12...16 use an odd number of passes and the final result is left
out-of-place.  The input data is never preserved.


# Example program

The code that produced the above accuracy and performance figures is included
as a demo with the latest Raspbian distro.  Build and run it as follows:

cd /opt/vc/src/hello_pi/hello_fft
make
sudo ./hello_fft.bin 12

It accepts three optional command-line arguments: <log2_N> <batch> <loops>

The special character device is required for the ioctl mailbox through which
the ARM communicates with the Videocore GPU.


# With Open GL on Pi 1

GPU_FFT and Open GL will run concurrently on Pi 1 if GPU_FFT is configured not
to use VC4 L2 cache by zeroing a define in file gpu_fft_base.c as follows:

#define GPU_FFT_USE_VC4_L2_CACHE 0 // Pi 1 only: cached=1; direct=0

Overall performance will probably be higher if GPU_FFT and Open GL take turns
at using the 3D hardware.  Since eglSwapBuffers() returns immediately without
waiting for rendering, call glFlush() and glFinish() afterwards as follows:

    for (;;) {
        ....
        eglSwapBuffers(....); // non-blocking call returns immediately
        glFlush();
        glFinish(); // wait until V3D hardware is idle
        ....
        gpu_fft_execute(....); // blocking call
        ....
    }


# 2-dimensional FFT

Please study the hello_fft_2d demo source, which is built and executed thus:

make hello_fft_2d.bin
sudo ./hello_fft_2d.bin

This generates a Windows BMP file: "hello_fft_2d.bmp"

The demo uses a square 512x512 array; however, rectangular arrays are allowed.
The following lines in gpu_fft_trans.c will do what is safe:

    ptr.arm.uptr[6] = src->x < dst->y? src->x : dst->y;
    ptr.arm.uptr[7] = src->y < dst->x? src->y : dst->x;

One may transpose the output from the second FFT pass back into the first pass
input buffer, by preparing and executing a second transposition; however, this
is probably unnecessary.  It depends on how the final output will be accessed.
