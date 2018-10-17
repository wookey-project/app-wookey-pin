#ifndef FAIL_H
#define FAIL_H
const int fail_nbcoul = 3;
const uint8_t fail_colormap[]={234,0,1, 235,1,1, 233,1,1, 255,255,255, };
const int fail_width =70, fail_height = 53;
const uint8_t fail[1020]={ 0,0, 0,255, 0,255, 0,57, 1,1, 2,14, 0,2, 2,1, 0,1, 2,11, 1,1, 0,3, 2,9, 0,1, 2,12, 0,13, 2,16, 0,2, 2,14, 0,3, 2,23, 0,12, 2,17, 0,1, 2,15, 0,2, 2,23, 0,12, 2,1, 3,13, 2,3, 0,1, 2,2, 3,9, 2,5, 0,1, 2,3, 3,7, 2,4, 3,7, 2,2, 0,12, 2,2, 3,12, 2,3, 0,1, 2,3, 3,8, 2,5, 0,1, 2,3, 3,7, 2,4, 3,7, 2,2, 0,12, 2,2, 3,12, 2,3, 0,1, 2,3, 3,10, 2,7, 3,7, 2,4, 3,7, 2,2, 0,12, 2,2, 3,12, 2,3, 0,1, 2,3, 3,10, 2,3, 0,1, 2,3, 3,7, 2,4, 3,7, 2,2, 0,12, 2,2, 3,12, 2,7, 3,10, 2,2, 0,2, 2,3, 3,7, 2,4, 3,7, 2,2, 0,12, 2,2, 3,6, 2,13, 3,10, 2,3, 0,1, 2,3, 3,7, 2,4, 3,7, 2,2, 0,12, 2,2, 3,6, 2,8, 0,1, 2,3, 3,4, 2,2, 3,5, 2,3, 0,1, 2,3, 3,7, 2,4, 3,7, 2,2, 0,12, 2,2, 3,6, 2,5, 0,2, 2,1, 0,1, 2,4, 3,3, 2,3, 3,4, 2,3, 0,1, 2,3, 3,7, 2,4, 3,7, 2,2, 0,12, 2,2, 3,6, 2,3, 0,5, 2,5, 3,3, 2,3, 3,4, 2,3, 0,1, 2,3, 3,7, 2,4, 3,7, 2,2, 0,12, 2,2, 3,6, 2,3, 0,4, 2,1, 0,1, 2,2, 3,5, 2,3, 3,4, 2,3, 0,1, 2,3, 3,7, 2,4, 3,7, 2,2, 0,12, 2,2, 3,6, 2,4, 0,1, 2,6, 3,5, 2,3, 3,4, 2,3, 0,1, 2,3, 3,7, 2,4, 3,7, 2,2, 0,12, 2,2, 3,6, 2,11, 3,5, 2,3, 3,4, 2,3, 0,1, 2,3, 3,7, 2,4, 3,7, 2,2, 0,12, 2,2, 3,7, 2,9, 3,4, 2,1, 3,1, 2,3, 3,4, 2,3, 0,1, 2,3, 3,7, 2,4, 3,7, 2,2, 0,12, 2,2, 3,12, 2,4, 3,4, 2,1, 3,1, 2,3, 3,4, 2,3, 0,1, 2,3, 3,7, 2,4, 3,7, 2,2, 0,12, 2,2, 3,12, 2,4, 3,4, 2,1, 3,1, 2,3, 3,4, 2,7, 3,7, 2,4, 3,7, 2,2, 0,12, 2,2, 3,12, 2,4, 3,4, 2,1, 3,1, 2,2, 3,5, 2,1, 3,1, 2,5, 3,7, 2,4, 3,7, 2,2, 0,12, 2,2, 3,12, 2,4, 3,4, 2,1, 3,1, 2,3, 3,6, 2,5, 3,7, 2,4, 3,7, 2,2, 0,12, 2,2, 3,6, 2,11, 3,4, 2,4, 3,6, 2,5, 3,7, 2,4, 3,7, 2,2, 0,12, 2,2, 3,6, 2,9, 3,6, 2,4, 3,6, 2,5, 3,7, 2,4, 3,7, 2,2, 0,12, 2,2, 3,6, 2,3, 0,1, 2,1, 1,1, 2,3, 3,1, 2,1, 3,3, 2,5, 3,6, 2,5, 3,7, 2,4, 3,7, 2,2, 0,12, 2,2, 3,6, 2,3, 0,4, 2,4, 3,8, 2,1, 3,5, 2,5, 3,7, 2,4, 3,7, 2,2, 0,12, 2,2, 3,6, 2,3, 0,4, 2,3, 3,15, 2,5, 3,7, 2,4, 3,7, 2,2, 0,12, 2,2, 3,6, 2,3, 0,4, 2,3, 3,15, 2,5, 3,7, 2,4, 3,7, 2,2, 0,12, 2,2, 3,6, 2,3, 0,4, 2,4, 3,14, 2,2, 0,1, 2,2, 3,7, 2,4, 3,7, 2,6, 0,8, 2,2, 3,6, 2,3, 0,2, 2,5, 3,4, 2,1, 3,4, 2,1, 3,6, 2,4, 3,7, 2,4, 3,7, 2,7, 0,7, 2,2, 3,6, 2,3, 0,3, 2,5, 3,4, 2,6, 3,4, 2,5, 3,7, 2,4, 3,6, 2,5, 3,1, 2,2, 0,7, 2,2, 3,6, 2,3, 0,2, 2,3, 3,1, 2,1, 3,5, 2,6, 3,4, 2,5, 3,7, 2,4, 3,6, 2,3, 3,1, 2,1, 3,1, 2,2, 0,7, 2,2, 3,6, 2,3, 0,2, 2,3, 3,7, 2,6, 3,5, 2,4, 3,7, 2,4, 3,12, 2,2, 0,7, 2,2, 3,6, 2,3, 0,2, 2,3, 3,7, 2,5, 3,5, 2,5, 3,7, 2,4, 3,12, 2,2, 0,7, 2,2, 3,6, 2,3, 0,2, 2,3, 3,7, 2,2, 0,1, 2,2, 3,1, 2,1, 3,3, 2,1, 3,1, 2,3, 3,7, 2,4, 3,12, 2,2, 0,7, 2,11, 0,2, 2,50, 0,7, 2,10, 0,3, 2,12, 0,1, 2,36, 0,8, 2,9, 0,5, 2,9, 0,1, 2,1, 0,1, 2,21, 0,1, 2,15, 0,10, 2,6, 0,6, 2,8, 0,1, 2,1, 0,3, 2,7, 0,1, 2,10, 0,1, 2,1, 0,1, 2,11, 0,255, 0,255, 1,54,};
#endif

