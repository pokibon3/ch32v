#ifndef QR_H
#define QR_H

#define QR_WIDTH 64
#define QR_HEIGHT 64

const unsigned char qr_data[] = {

  0xFF, 0xFF, 0x0E, 0x00, 0x0C, 0xFE, 0x7F, 0xFF, 0xFF, 0xFF, 0x0E, 0x00, 0x0C, 0xFE, 0x7F, 0xFF, 
  0xFF, 0xFF, 0x0E, 0x00, 0x0C, 0xFE, 0x7F, 0xFF, 0xE0, 0x03, 0x3E, 0x7F, 0xFF, 0xC0, 0x60, 0x03, 
  0xE0, 0x03, 0x3E, 0x7F, 0xFF, 0xC0, 0x60, 0x03, 0xE7, 0xF3, 0x0F, 0xE0, 0x00, 0xFE, 0x67, 0xF3, 
  0xE7, 0xF3, 0x0F, 0xE0, 0x00, 0xFE, 0x67, 0xF3, 0xE7, 0xF3, 0x3E, 0x67, 0xFF, 0x38, 0x67, 0xF3, 
  0xE7, 0xF3, 0x3E, 0x67, 0xFF, 0x38, 0x67, 0xF3, 0xE7, 0xF3, 0x01, 0x99, 0xC0, 0x3E, 0x67, 0xF3, 
  0xE7, 0xF3, 0x01, 0x99, 0xC0, 0x3E, 0x67, 0xF3, 0xE7, 0xF3, 0x01, 0x99, 0xC0, 0x3E, 0x67, 0xF3, 
  0xE0, 0x03, 0x31, 0x86, 0x30, 0x06, 0x60, 0x03, 0xE0, 0x03, 0x31, 0x86, 0x30, 0x06, 0x60, 0x03, 
  0xFF, 0xFF, 0x31, 0x99, 0xCC, 0xC6, 0x7F, 0xFF, 0xFF, 0xFF, 0x31, 0x99, 0xCC, 0xC6, 0x7F, 0xFF, 
  0x00, 0x00, 0x00, 0x18, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x30, 0x00, 0x00, 0x00, 
  0xFF, 0xF3, 0xFF, 0x9F, 0xFF, 0x01, 0x98, 0xCC, 0xFF, 0xF3, 0xFF, 0x9F, 0xFF, 0x01, 0x98, 0xCC, 
  0x00, 0x7C, 0x3E, 0x00, 0x00, 0x3E, 0x7F, 0x03, 0x00, 0x7C, 0x3E, 0x00, 0x00, 0x3E, 0x7F, 0x03, 
  0x00, 0x7C, 0x3E, 0x00, 0x00, 0x3E, 0x7F, 0x03, 0xF9, 0x8F, 0xFE, 0x79, 0xF3, 0x00, 0x07, 0x00, 
  0xF9, 0x8F, 0xFE, 0x79, 0xF3, 0x00, 0x07, 0x00, 0xFE, 0x7C, 0x3F, 0xE1, 0xF0, 0x3E, 0x00, 0xCC, 
  0xFE, 0x7C, 0x3F, 0xE1, 0xF0, 0x3E, 0x00, 0xCC, 0xE0, 0x73, 0xCE, 0x7F, 0xFF, 0x38, 0x00, 0xF0, 
  0xE0, 0x73, 0xCE, 0x7F, 0xFF, 0x38, 0x00, 0xF0, 0x06, 0x7C, 0x31, 0x99, 0xC3, 0xFF, 0xFF, 0x03, 
  0x06, 0x7C, 0x31, 0x99, 0xC3, 0xFF, 0xFF, 0x03, 0xF8, 0x73, 0x31, 0x98, 0x33, 0xC7, 0x87, 0xF0, 
  0xF8, 0x73, 0x31, 0x98, 0x33, 0xC7, 0x87, 0xF0, 0xF8, 0x73, 0x31, 0x98, 0x33, 0xC7, 0x87, 0xF0, 
  0xE0, 0x70, 0x30, 0x79, 0xCC, 0x3F, 0xFF, 0x0C, 0xE0, 0x70, 0x30, 0x79, 0xCC, 0x3F, 0xFF, 0x0C, 
  0x1E, 0x7F, 0x00, 0x07, 0xFF, 0x39, 0x98, 0xF0, 0x1E, 0x7F, 0x00, 0x07, 0xFF, 0x39, 0x98, 0xF0, 
  0xE7, 0x80, 0xC0, 0x60, 0x0C, 0x39, 0xFF, 0x33, 0xE7, 0x80, 0xC0, 0x60, 0x0C, 0x39, 0xFF, 0x33, 
  0xE0, 0x7F, 0x0F, 0x86, 0x33, 0xC6, 0x7F, 0x30, 0xE0, 0x7F, 0x0F, 0x86, 0x33, 0xC6, 0x7F, 0x30, 
  0xE7, 0x80, 0x0E, 0x01, 0xC0, 0x07, 0xE0, 0x0C, 0xE7, 0x80, 0x0E, 0x01, 0xC0, 0x07, 0xE0, 0x0C, 
  0xE7, 0x80, 0x0E, 0x01, 0xC0, 0x07, 0xE0, 0x0C, 0xE1, 0x8F, 0xCF, 0x9F, 0xFF, 0xFF, 0xFF, 0x3F, 
  0xE1, 0x8F, 0xCF, 0x9F, 0xFF, 0xFF, 0xFF, 0x3F, 0x00, 0x00, 0x30, 0x66, 0x03, 0xC6, 0x07, 0xFF, 
  0x00, 0x00, 0x30, 0x66, 0x03, 0xC6, 0x07, 0xFF, 0xFF, 0xFF, 0x3E, 0x1F, 0xFF, 0x3E, 0x67, 0xF0, 
  0xFF, 0xFF, 0x3E, 0x1F, 0xFF, 0x3E, 0x67, 0xF0, 0xE0, 0x03, 0x01, 0x81, 0xC0, 0x3E, 0x07, 0x03, 
  0xE0, 0x03, 0x01, 0x81, 0xC0, 0x3E, 0x07, 0x03, 0xE7, 0xF3, 0x31, 0x87, 0xFF, 0x07, 0xFF, 0x33, 
  0xE7, 0xF3, 0x31, 0x87, 0xFF, 0x07, 0xFF, 0x33, 0xE7, 0xF3, 0x31, 0x87, 0xFF, 0x07, 0xFF, 0x33, 
  0xE7, 0xF3, 0x30, 0x61, 0xC0, 0xFE, 0x00, 0xFF, 0xE7, 0xF3, 0x30, 0x61, 0xC0, 0xFE, 0x00, 0xFF, 
  0xE7, 0xF3, 0x31, 0x99, 0xF3, 0xFF, 0xFF, 0xFC, 0xE7, 0xF3, 0x31, 0x99, 0xF3, 0xFF, 0xFF, 0xFC, 
  0xE0, 0x03, 0x3E, 0x61, 0xF0, 0x07, 0x98, 0xCC, 0xE0, 0x03, 0x3E, 0x61, 0xF0, 0x07, 0x98, 0xCC, 
  0xFF, 0xFF, 0x3E, 0x7F, 0xC3, 0x38, 0x7F, 0x30, 0xFF, 0xFF, 0x3E, 0x7F, 0xC3, 0x38, 0x7F, 0x30, 
};

#endif // IMAGE_H
