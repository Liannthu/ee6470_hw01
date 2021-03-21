/*
Filename    : sobel.cpp
Compiler    : Clang++ 8.0.0
Description : Demo the how to use sobel detector on gray level image
*/
#include <cassert>
#include <cmath>
#include <iostream>

const int MASK_N = 2;
const int MASK_X = 3;
const int MASK_Y = 3;
const int WHITE = 255;
const int BLACK = 0;

unsigned char *image_s = nullptr; // source image array
unsigned char *image_t = nullptr; // target image array
FILE *fp_s = nullptr;             // source file handler
FILE *fp_t = nullptr;             // target file handler

unsigned int width = 0;               // image width
unsigned int height = 0;              // image height
unsigned int rgb_raw_data_offset = 0; // RGB raw data offset
unsigned char bit_per_pixel = 0;      // bit per pixel
unsigned short byte_per_pixel = 0;    // byte per pixel

// bitmap header
unsigned char header[54] = {
    0x42,          // identity : B
    0x4d,          // identity : M
    0,    0, 0, 0, // file size
    0,    0,       // reserved1
    0,    0,       // reserved2
    54,   0, 0, 0, // RGB data offset
    40,   0, 0, 0, // struct BITMAPINFOHEADER size
    0,    0, 0, 0, // bmp width
    0,    0, 0, 0, // bmp height
    1,    0,       // planes
    24,   0,       // bit per pixel
    0,    0, 0, 0, // compression
    0,    0, 0, 0, // data size
    0,    0, 0, 0, // h resolution
    0,    0, 0, 0, // v resolution
    0,    0, 0, 0, // used colors
    0,    0, 0, 0  // important colors
};

// sobel mask
int mask[MASK_X][MASK_Y] = 
{
  1, 2, 1,
  2, 4, 2,
  1, 2, 1
};

double factor = 1.0/16.0;
double bias = 0.0;

int read_bmp(const char *fname_s) {
  fp_s = fopen(fname_s, "rb");
  if (fp_s == nullptr) {
    std::cerr << "fopen fp_s error" << std::endl;
    return -1;
  }

  // move offset to 10 to find rgb raw data offset
  fseek(fp_s, 10, SEEK_SET);
  assert(fread(&rgb_raw_data_offset, sizeof(unsigned int), 1, fp_s));

  // move offset to 18 to get width & height;
  fseek(fp_s, 18, SEEK_SET);
  assert(fread(&width, sizeof(unsigned int), 1, fp_s));
  assert(fread(&height, sizeof(unsigned int), 1, fp_s));

  // get bit per pixel
  fseek(fp_s, 28, SEEK_SET);
  assert(fread(&bit_per_pixel, sizeof(unsigned short), 1, fp_s));
  byte_per_pixel = bit_per_pixel / 8;

  // move offset to rgb_raw_data_offset to get RGB raw data
  fseek(fp_s, rgb_raw_data_offset, SEEK_SET);

  size_t size = width * height * byte_per_pixel;
  image_s = reinterpret_cast<unsigned char *>(new void *[size]);
  if (image_s == nullptr) {
    std::cerr << "allocate image_s error" << std::endl;
    return -1;
  }

  image_t = reinterpret_cast<unsigned char *>(new void *[size]);
  if (image_t == nullptr) {
    std::cerr << "allocate image_t error" << std::endl;
    return -1;
  }

  assert(fread(image_s, sizeof(unsigned char),
               (size_t)(long)width * height * byte_per_pixel, fp_s));
  fclose(fp_s);

  return 0;
}

// convert RGB to gray level int

int sobel(double threshold) {
  unsigned int x, y, i, v, u; // for loop counter
  unsigned char R, G, B;      // color of R, G, B
  double val[MASK_N] = {0.0};
  int adjustX, adjustY, xBound, yBound;
  double total;
  unsigned char BUFFER[3][520][3];

  for (y = 0; y != height; ++y) {
    int y_0, y_1, y_2;
    y_0 = y - 1;
    y_1 = y;
    y_2 = y + 1;
    if (y_0 == -1) y_0 = 511;
    if (y_2 == 512) y_2 = 0;
    BUFFER[0][0][0] = int(*(image_s + byte_per_pixel * (width * (y_0) + (511)) + 2)); 
    BUFFER[0][0][1] = int(*(image_s + byte_per_pixel * (width * (y_0) + (511)) + 1));
    BUFFER[0][0][2] = int(*(image_s + byte_per_pixel * (width * (y_0) + (511)) + 0));
    BUFFER[0][0][0] = int(*(image_s + byte_per_pixel * (width * (y_1) + (511)) + 2)); 
    BUFFER[0][0][1] = int(*(image_s + byte_per_pixel * (width * (y_1) + (511)) + 1));
    BUFFER[0][0][2] = int(*(image_s + byte_per_pixel * (width * (y_1) + (511)) + 0));
    BUFFER[0][0][0] = int(*(image_s + byte_per_pixel * (width * (y_2) + (511)) + 2)); 
    BUFFER[0][0][1] = int(*(image_s + byte_per_pixel * (width * (y_2) + (511)) + 1));
    BUFFER[0][0][2] = int(*(image_s + byte_per_pixel * (width * (y_2) + (511)) + 0));
    for (x = 0; x != width; ++x) {

      BUFFER[0][x + 1][0] = int(*(image_s + byte_per_pixel * (width * (y_0) + (x)) + 2)); // 1 is x = 0
      BUFFER[0][x + 1][1] = int(*(image_s + byte_per_pixel * (width * (y_0) + (x)) + 1));
      BUFFER[0][x + 1][2] = int(*(image_s + byte_per_pixel * (width * (y_0) + (x)) + 0));
      BUFFER[1][x + 1][0] = int(*(image_s + byte_per_pixel * (width * (y_1) + (x)) + 2));
      BUFFER[1][x + 1][1] = int(*(image_s + byte_per_pixel * (width * (y_1) + (x)) + 1));
      BUFFER[1][x + 1][2] = int(*(image_s + byte_per_pixel * (width * (y_1) + (x)) + 0));
      BUFFER[2][x + 1][0] = int(*(image_s + byte_per_pixel * (width * (y_2) + (x)) + 2));
      BUFFER[2][x + 1][1] = int(*(image_s + byte_per_pixel * (width * (y_2) + (x)) + 1));
      BUFFER[2][x + 1][2] = int(*(image_s + byte_per_pixel * (width * (y_2) + (x)) + 0));
      
    }
      BUFFER[0][513][0] = int(*(image_s + byte_per_pixel * (width * (y_0) + (0)) + 2)); 
      BUFFER[0][513][1] = int(*(image_s + byte_per_pixel * (width * (y_0) + (0)) + 1));
      BUFFER[0][513][2] = int(*(image_s + byte_per_pixel * (width * (y_0) + (0)) + 0));
      BUFFER[0][513][0] = int(*(image_s + byte_per_pixel * (width * (y_1) + (0)) + 2)); 
      BUFFER[0][513][1] = int(*(image_s + byte_per_pixel * (width * (y_1) + (0)) + 1));
      BUFFER[0][513][2] = int(*(image_s + byte_per_pixel * (width * (y_1) + (0)) + 0));
      BUFFER[0][513][0] = int(*(image_s + byte_per_pixel * (width * (y_2) + (0)) + 2)); 
      BUFFER[0][513][1] = int(*(image_s + byte_per_pixel * (width * (y_2) + (0)) + 1));
      BUFFER[0][513][2] = int(*(image_s + byte_per_pixel * (width * (y_2) + (0)) + 0));
      
      for (i = 0; i < width; i++){
        double tmp_r = 0.0, tmp_g = 0.0, tmp_b = 0.0;
        for (u = 0; u < 3; u++){
          for (v = 0; v < 3; v++){
            tmp_r += BUFFER[u][i+v][0] * mask[v][u];
            tmp_g += BUFFER[u][i+v][1] * mask[v][u];
            tmp_b += BUFFER[u][i+v][2] * mask[v][u];
          }
        }
        *(image_t + byte_per_pixel * (width * y + i) + 2) = std::min(std::max(int(factor * tmp_r + bias), 0), 255);
        *(image_t + byte_per_pixel * (width * y + i) + 1) = std::min(std::max(int(factor * tmp_g + bias), 0), 255);
        *(image_t + byte_per_pixel * (width * y + i) + 0) = std::min(std::max(int(factor * tmp_b + bias), 0), 255);
      }
    }

  return 0;
}

int write_bmp(const char *fname_t) {
  unsigned int file_size = 0; // file size

  fp_t = fopen(fname_t, "wb");
  if (fp_t == nullptr) {
    std::cerr << "fopen fname_t error" << std::endl;
    return -1;
  }

  // file size
  file_size = width * height * byte_per_pixel + rgb_raw_data_offset;
  header[2] = (unsigned char)(file_size & 0x000000ff);
  header[3] = (file_size >> 8) & 0x000000ff;
  header[4] = (file_size >> 16) & 0x000000ff;
  header[5] = (file_size >> 24) & 0x000000ff;

  // width
  header[18] = width & 0x000000ff;
  header[19] = (width >> 8) & 0x000000ff;
  header[20] = (width >> 16) & 0x000000ff;
  header[21] = (width >> 24) & 0x000000ff;

  // height
  header[22] = height & 0x000000ff;
  header[23] = (height >> 8) & 0x000000ff;
  header[24] = (height >> 16) & 0x000000ff;
  header[25] = (height >> 24) & 0x000000ff;

  // bit per pixel
  header[28] = bit_per_pixel;

  // write header
  fwrite(header, sizeof(unsigned char), rgb_raw_data_offset, fp_t);

  // write image
  fwrite(image_t, sizeof(unsigned char),
         (size_t)(long)width * height * byte_per_pixel, fp_t);

  fclose(fp_t);

  return 0;
}

int main(void) {
  assert(read_bmp("lena.bmp") == 0); // 24 bit gray level image
  sobel(90.0);
  assert(write_bmp("lena_gaussian_row.bmp") == 0);
  delete (image_s);
  delete (image_t);

  return 0;
}
