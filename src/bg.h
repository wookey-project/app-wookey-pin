#ifndef BG_H
#define BG_H
const int bg_nbcoul = 21;
const uint8_t bg_colormap[]={38,86,155, 37,89,157, 36,90,158, 36,91,159, 36,92,160, 36,93,161, 36,93,162, 40,81,152, 44,69,138, 44,68,137, 45,68,137, 48,54,121, 49,53,120, 49,52,119, 50,49,117, 55,34,98, 54,35,98, 54,36,99, 52,41,105, 54,37,100, 53,39,102, 51,44,108, };
const int bg_width =240, bg_height = 266;
const uint8_t bg[6638]={ 0,0, 14,254, 14,255, 14,248, 14,255, 14,145, 0,32, 14,89, 1,38, 14,11, 2,10, 14,38, 1,54, 14,52, 0,8, 14,9, 1,9, 14,4, 2,33, 3,29, 14,3, 3,11, 14,1, 4,9, 14,1, 3,71, 14,1, 2,5, 14,46, 0,6, 14,9, 1,1, 14,10, 2,3, 14,3, 3,37, 14,1, 4,118, 14,1, 3,4, 14,1, 2,1, 14,50, 0,2, 14,8, 1,1, 14,10, 2,1, 14,4, 3,3, 14,2, 4,55, 14,1, 5,43, 14,13, 5,40, 4,5, 3,2, 2,1, 14,52, 0,2, 14,19, 2,1, 14,3, 3,2, 14,3, 4,36, 14,2, 4,15, 14,1, 5,96, 4,5, 3,2, 2,1, 14,55, 0,2, 14,22, 3,2, 14,2, 4,18, 14,7, 4,27, 14,1, 5,43, 14,13, 5,40, 4,5, 3,2, 2,1, 14,59, 0,2, 14,20, 3,2, 14,1, 4,15, 14,3, 3,5, 14,3, 4,9, 14,17, 5,42, 14,2, 4,11, 14,3, 5,38, 4,5, 3,2, 2,1, 14,67, 0,1, 14,15, 3,2, 14,1, 4,10, 14,2, 3,6, 14,2, 3,3, 14,4, 4,3, 14,2, 5,30, 6,7, 5,28, 14,12, 6,37, 14,6, 3,1, 2,1, 14,84, 3,2, 14,1, 4,8, 14,2, 3,3, 14,9, 3,2, 14,2, 4,4, 14,1, 5,30, 6,1, 14,4, 6,2, 5,29, 14,3, 5,8, 6,12, 14,25, 5,5, 4,1, 3,1, 2,1, 14,86, 3,1, 14,3, 4,1, 14,5, 3,1, 14,15, 3,1, 14,3, 4,2, 14,1, 5,29, 6,7, 5,26, 14,3, 4,3, 14,2, 5,17, 6,1, 14,25, 5,5, 4,1, 3,1, 2,1, 14,45, 7,2, 14,30, 0,1, 14,9, 3,2, 14,2, 3,3, 14,16, 2,1, 14,2, 3,2, 14,2, 4,2, 14,4, 5,57, 14,2, 4,6, 14,2, 5,16, 6,2, 14,24, 5,5, 4,1, 3,1, 2,1, 14,79, 0,1, 14,10, 3,2, 14,23, 3,1, 14,3, 4,4, 14,8, 5,5, 14,4, 5,40, 14,1, 4,8, 14,2, 5,16, 6,1, 14,24, 5,5, 4,1, 3,1, 2,1, 14,81, 0,1, 14,6, 2,5, 14,24, 3,1, 14,2, 4,6, 14,17, 5,36, 14,2, 4,8, 14,2, 5,16, 6,1, 14,24, 5,5, 4,1, 3,1, 2,1, 14,118, 3,1, 14,2, 4,20, 14,3, 5,19, 14,9, 5,7, 14,2, 4,8, 14,2, 5,16, 6,1, 14,24, 5,5, 4,1, 3,1, 2,1, 14,58, 7,1, 14,60, 3,1, 14,2, 4,20, 14,2, 5,17, 14,2, 4,9, 14,7, 4,6, 14,2, 4,3, 14,2, 5,15, 6,2, 14,22, 6,1, 5,5, 4,1, 3,1, 2,1, 14,62, 7,3, 14,55, 3,1, 14,2, 4,21, 14,1, 5,15, 14,1, 4,29, 14,2, 5,16, 6,1, 14,22, 6,1, 5,5, 4,1, 3,1, 2,1, 14,62, 7,5, 14,29, 0,1, 14,30, 4,19, 14,2, 5,9, 14,3, 4,30, 14,1, 5,17, 6,1, 14,22, 6,1, 5,5, 4,1, 3,1, 2,1, 14,66, 7,2, 14,20, 0,1, 14,20, 0,1, 14,13, 3,2, 14,16, 4,7, 14,2, 5,5, 14,2, 4,7, 14,7, 4,17, 14,11, 5,9, 6,2, 14,21, 6,1, 5,5, 4,1, 3,1, 2,1, 14,71, 7,7, 14,32, 0,1, 14,15, 3,3, 14,5, 3,7, 14,5, 4,3, 14,7, 4,4, 14,21, 4,7, 14,3, 4,8, 14,1, 5,8, 6,2, 14,21, 6,1, 5,5, 4,1, 3,1, 14,74, 7,7, 14,65, 3,1, 14,3, 4,8, 14,2, 3,5, 14,8, 3,8, 14,3, 4,6, 14,2, 4,9, 14,1, 5,8, 6,1, 14,21, 6,1, 5,5, 4,1, 3,1, 1,1, 14,78, 7,4, 14,64, 3,2, 14,3, 4,3, 14,4, 3,4, 14,18, 3,2, 14,2, 4,16, 14,1, 5,8, 6,1, 14,21, 6,1, 5,5, 4,1, 3,1, 1,1, 14,80, 7,4, 14,32, 0,2, 14,31, 3,1, 14,5, 3,3, 14,6, 2,2, 14,16, 3,1, 14,3, 4,6, 14,6, 4,2, 14,2, 5,7, 6,1, 14,21, 6,1, 5,5, 4,1, 3,1, 1,1, 14,83, 7,1, 14,67, 3,2, 14,27, 2,1, 14,1, 3,2, 14,2, 4,5, 14,7, 4,3, 14,1, 5,7, 6,1, 14,21, 6,1, 5,5, 4,1, 3,1, 1,1, 14,121, 0,1, 14,33, 2,2, 14,26, 3,2, 14,9, 3,2, 14,3, 4,2, 14,2, 5,6, 6,1, 14,21, 6,1, 5,5, 4,1, 3,1, 1,1, 14,86, 7,1, 14,35, 0,1, 14,14, 0,5, 14,10, 2,2, 14,30, 3,1, 14,8, 3,4, 14,3, 4,2, 14,1, 5,6, 6,1, 14,5, 6,17, 5,5, 4,1, 3,1, 1,1, 14,124, 0,1, 14,10, 0,1, 14,7, 0,1, 14,42, 3,2, 14,2, 3,1, 14,6, 3,1, 14,2, 4,2, 14,1, 5,6, 6,1, 14,5, 6,1, 5,20, 14,1, 4,1, 3,1, 1,1, 14,91, 7,1, 14,105, 3,1, 14,2, 4,2, 14,1, 5,6, 6,7, 5,20, 14,1, 4,1, 3,1, 1,1, 14,164, 0,1, 14,33, 3,1, 14,2, 4,1, 14,1, 5,33, 14,1, 4,1, 14,1, 1,1, 14,8, 8,2, 9,5, 14,171, 2,9, 14,2, 3,2, 14,1, 4,2, 14,1, 5,33, 4,2, 14,1, 1,1, 14,8, 8,1, 9,7, 14,132, 0,1, 14,44, 2,1, 14,4, 3,1, 14,2, 4,2, 14,1, 5,32, 4,2, 14,1, 1,1, 14,8, 8,1, 9,1, 10,1, 14,2, 9,15, 14,123, 0,1, 14,42, 2,1, 14,3, 3,1, 14,1, 4,3, 5,33, 4,2, 14,1, 1,1, 14,18, 9,13, 14,1, 8,5, 14,160, 3,1, 14,2, 4,2, 14,1, 5,33, 4,2, 14,1, 1,1, 14,7, 8,1, 10,1, 14,12, 9,11, 14,1, 8,5, 14,80, 7,3, 14,73, 2,1, 14,2, 3,2, 14,1, 4,2, 14,1, 5,13, 14,10, 5,10, 4,2, 14,1, 1,1, 0,1, 14,22, 9,18, 14,79, 7,2, 14,52, 0,1, 14,25, 4,2, 14,1, 5,11, 14,3, 4,4, 14,5, 5,10, 4,2, 14,1, 1,1, 0,1, 14,24, 9,18, 14,78, 7,9, 14,66, 3,1, 14,2, 4,3, 14,4, 5,3, 14,6, 4,9, 14,4, 5,8, 4,1, 14,2, 1,1, 0,1, 14,6, 10,1, 14,18, 10,2, 9,22, 14,1, 8,1, 14,71, 7,10, 14,63, 3,1, 14,2, 4,4, 14,7, 4,18, 14,1, 5,7, 4,1, 14,3, 0,1, 14,5, 8,1, 14,20, 10,4, 9,20, 8,3, 14,74, 7,5, 14,63, 3,1, 14,2, 4,4, 14,5, 4,9, 14,6, 4,6, 14,1, 5,6, 4,1, 14,31, 10,3, 9,21, 8,3, 14,108, 7,4, 14,21, 0,1, 14,7, 3,1, 14,3, 4,14, 14,12, 4,3, 14,2, 5,5, 4,1, 3,1, 14,8, 10,1, 14,21, 10,3, 9,21, 14,1, 8,5, 14,92, 7,20, 14,27, 3,1, 14,2, 4,11, 14,3, 3,2, 14,4, 3,3, 14,4, 4,4, 5,5, 4,1, 3,1, 14,30, 10,8, 9,16, 14,1, 8,6, 14,90, 7,22, 14,26, 3,2, 14,1, 4,10, 14,1, 3,3, 14,9, 3,2, 14,4, 4,2, 14,1, 5,4, 4,1, 3,1, 14,31, 10,11, 9,12, 14,2, 8,6, 14,90, 7,23, 14,25, 3,1, 14,3, 4,4, 14,4, 3,1, 14,12, 3,2, 14,4, 4,2, 14,1, 5,3, 14,1, 4,1, 3,1, 14,7, 8,1, 14,32, 10,2, 9,12, 14,99, 7,25, 14,20, 2,1, 14,2, 3,2, 14,7, 3,1, 14,15, 3,3, 14,2, 4,2, 14,2, 5,2, 14,1, 4,1, 3,1, 14,40, 10,1, 9,22, 8,2, 14,104, 7,14, 14,20, 3,5, 14,3, 2,1, 14,15, 3,3, 14,2, 4,2, 14,3, 4,2, 3,1, 14,7, 10,1, 14,31, 10,3, 9,21, 8,3, 14,106, 7,12, 14,40, 2,1, 14,4, 3,3, 14,1, 4,3, 14,1, 4,2, 3,1, 14,7, 10,1, 14,33, 10,8, 9,14, 14,1, 8,3, 14,107, 7,10, 14,40, 2,1, 14,6, 3,1, 14,2, 4,5, 3,1, 1,1, 14,40, 10,13, 9,10, 14,1, 8,4, 14,9, 8,1, 14,96, 7,9, 14,40, 2,1, 14,7, 3,1, 14,2, 4,3, 14,2, 1,1, 14,41, 10,12, 9,11, 14,4, 8,2, 14,2, 9,7, 14,99, 7,6, 14,39, 2,1, 14,8, 3,1, 14,1, 4,3, 3,1, 14,1, 1,1, 14,44, 10,9, 9,27, 14,98, 7,6, 14,40, 2,4, 14,1, 2,1, 14,2, 3,2, 14,1, 4,2, 3,1, 2,1, 1,1, 14,5, 8,1, 14,39, 10,13, 9,24, 14,97, 7,5, 14,10, 0,1, 14,38, 3,1, 14,3, 3,1, 14,1, 1,1, 0,1, 14,4, 8,1, 14,42, 10,10, 9,24, 14,97, 7,5, 14,49, 3,1, 14,3, 3,1, 14,2, 0,1, 14,4, 8,1, 14,49, 10,3, 9,26, 14,95, 7,5, 14,13, 0,3, 14,34, 3,4, 14,7, 9,1, 14,49, 10,3, 9,26, 14,96, 7,5, 14,49, 3,3, 14,61, 9,29, 14,93, 7,5, 14,32, 0,1, 14,82, 9,15, 10,2, 9,20, 14,82, 7,6, 14,33, 0,1, 14,19, 1,1, 14,64, 10,14, 9,27, 8,2, 14,74, 7,5, 14,50, 2,2, 14,69, 10,12, 9,27, 8,5, 14,70, 7,6, 14,121, 10,11, 9,28, 14,2, 8,7, 14,64, 7,7, 14,53, 0,1, 14,70, 10,8, 9,30, 14,1, 8,7, 14,63, 7,8, 14,15, 7,8, 14,27, 1,1, 14,1, 0,1, 14,71, 10,9, 9,28, 14,3, 8,8, 14,62, 7,30, 14,11, 0,1, 14,14, 1,1, 14,1, 0,1, 14,71, 10,12, 14,11, 9,16, 14,1, 8,11, 14,75, 7,16, 14,100, 10,9, 14,15, 10,3, 9,10, 14,1, 8,11, 14,11, 8,15, 14,61, 7,5, 14,9, 0,1, 14,91, 10,7, 14,18, 9,10, 14,1, 8,11, 14,9, 8,20, 14,58, 7,6, 14,125, 10,2, 9,8, 14,1, 8,13, 14,5, 8,2, 14,8, 8,15, 14,56, 7,4, 14,126, 10,2, 9,8, 14,1, 8,12, 14,3, 9,24, 14,1, 8,7, 14,52, 7,5, 14,9, 0,1, 14,27, 11,1, 14,87, 10,10, 14,1, 8,9, 14,2, 9,31, 14,1, 8,4, 14,53, 7,4, 14,23, 0,1, 14,13, 11,1, 14,88, 10,9, 14,12, 9,33, 14,1, 8,2, 14,55, 7,2, 14,11, 0,1, 14,11, 0,1, 14,12, 11,2, 14,89, 10,8, 14,12, 9,34, 8,2, 14,55, 7,3, 14,22, 0,1, 14,12, 11,1, 14,91, 10,14, 9,39, 8,2, 14,57, 7,1, 14,11, 0,2, 14,9, 0,1, 14,11, 11,1, 14,1, 12,1, 14,91, 10,13, 9,14, 14,7, 10,9, 9,8, 14,1, 8,2, 14,57, 7,2, 14,20, 0,1, 14,13, 12,4, 14,90, 10,12, 9,11, 14,12, 10,6, 9,9, 8,2, 14,59, 7,2, 14,15, 0,1, 14,2, 0,1, 14,18, 12,1, 14,92, 10,9, 9,10, 14,13, 10,6, 9,9, 8,2, 14,60, 7,1, 14,32, 12,1, 14,1, 13,2, 14,2, 12,1, 14,91, 10,9, 9,8, 10,2, 14,13, 10,7, 9,8, 8,2, 14,60, 7,2, 14,32, 13,4, 14,14, 12,3, 14,77, 10,17, 14,14, 10,6, 9,8, 14,1, 8,1, 14,62, 7,2, 14,29, 12,1, 13,1, 14,3, 13,2, 14,12, 12,5, 14,83, 10,10, 14,15, 10,6, 9,6, 14,1, 8,2, 14,63, 7,2, 14,35, 13,1, 14,5, 12,2, 14,8, 12,3, 14,84, 10,4, 14,18, 10,5, 9,7, 8,2, 14,66, 7,3, 14,158, 10,4, 9,4, 8,4, 14,69, 7,5, 14,47, 12,1, 14,105, 10,4, 9,5, 8,3, 14,29, 8,7, 14,37, 7,4, 14,43, 13,1, 14,107, 10,3, 9,4, 8,5, 14,25, 8,14, 14,36, 7,3, 14,150, 10,2, 9,5, 8,3, 14,25, 8,17, 14,36, 7,2, 14,150, 10,1, 9,5, 8,4, 14,22, 8,22, 14,34, 7,1, 14,151, 10,1, 9,5, 8,3, 14,18, 8,1, 14,9, 9,5, 14,1, 8,12, 14,186, 9,4, 14,1, 8,2, 14,17, 8,1, 14,3, 9,13, 14,7, 8,7, 14,32, 7,1, 14,152, 10,1, 9,4, 8,2, 14,18, 9,1, 14,5, 9,19, 8,6, 14,184, 9,4, 14,1, 8,1, 14,15, 8,1, 14,1, 9,1, 14,8, 9,18, 8,8, 14,183, 9,3, 14,1, 8,1, 14,13, 8,1, 14,1, 9,1, 14,10, 10,2, 9,5, 10,4, 9,8, 14,1, 8,7, 14,181, 9,3, 14,1, 8,2, 14,11, 8,1, 14,1, 9,2, 14,11, 10,11, 9,8, 14,1, 8,7, 14,181, 9,3, 14,1, 8,2, 14,9, 8,1, 14,2, 9,1, 14,13, 10,11, 9,8, 14,1, 8,8, 14,180, 9,3, 14,1, 8,5, 14,2, 8,2, 14,20, 10,10, 9,8, 14,1, 8,8, 14,106, 12,1, 14,73, 9,2, 14,3, 8,3, 14,4, 9,1, 14,20, 10,14, 9,4, 14,1, 8,8, 14,107, 12,1, 14,71, 9,4, 14,5, 9,1, 10,1, 14,23, 10,14, 9,4, 14,1, 8,8, 14,106, 13,1, 14,72, 9,2, 14,5, 10,1, 14,26, 10,14, 9,4, 14,1, 8,8, 14,106, 13,1, 14,2, 12,1, 14,68, 9,3, 14,2, 9,2, 14,29, 10,16, 9,2, 8,10, 14,105, 13,1, 14,69, 9,2, 14,2, 9,2, 14,31, 10,16, 9,2, 14,1, 8,10, 14,104, 13,2, 14,1, 12,1, 14,65, 9,5, 14,33, 10,16, 9,2, 14,1, 8,12, 14,103, 13,1, 14,66, 9,5, 14,35, 10,15, 9,2, 14,1, 8,12, 14,104, 13,1, 14,1, 12,1, 14,63, 9,3, 14,37, 10,15, 9,2, 14,1, 8,12, 14,170, 9,1, 14,40, 10,14, 9,3, 14,2, 8,10, 14,212, 10,13, 9,3, 14,2, 8,10, 14,212, 10,10, 14,8, 8,10, 14,213, 10,9, 14,15, 8,3, 14,214, 10,13, 9,9, 14,1, 8,3, 14,214, 10,13, 9,9, 8,4, 14,217, 10,10, 9,8, 14,1, 8,3, 14,218, 10,9, 9,8, 14,1, 8,4, 14,219, 10,7, 9,8, 8,5, 14,220, 10,6, 9,7, 14,1, 8,4, 14,223, 10,3, 9,9, 8,4, 14,224, 10,5, 9,6, 8,3, 14,226, 10,4, 9,6, 8,3, 14,228, 10,2, 9,6, 8,3, 14,231, 9,5, 14,1, 8,2, 14,232, 10,1, 9,3, 8,3, 14,233, 10,1, 9,2, 14,1, 8,2, 14,46, 9,1, 14,3, 12,1, 14,182, 9,3, 8,3, 14,47, 9,1, 14,3, 13,1, 14,182, 9,2, 14,1, 8,2, 14,234, 9,2, 14,1, 8,1, 14,235, 10,1, 9,1, 14,1, 8,1, 14,54, 11,1, 14,125, 13,1, 14,55, 9,1, 14,1, 8,1, 14,55, 11,1, 14,126, 13,1, 14,2, 12,1, 14,49, 9,2, 14,1, 8,1, 14,13, 1,1, 14,170, 13,1, 14,2, 12,1, 14,47, 9,2, 14,1, 8,1, 14,186, 13,1, 14,2, 12,1, 14,45, 10,1, 9,1, 14,1, 8,1, 14,188, 13,1, 14,1, 12,1, 14,45, 9,1, 14,1, 8,1, 14,14, 1,1, 14,44, 13,1, 14,129, 13,1, 14,1, 12,1, 14,44, 10,1, 14,1, 8,1, 14,12, 0,1, 14,1, 1,1, 14,45, 13,1, 14,129, 13,1, 14,1, 12,1, 14,45, 8,1, 14,12, 0,1, 14,8, 12,1, 14,39, 13,1, 14,129, 13,1, 14,1, 12,1, 14,43, 10,1, 8,1, 14,12, 0,1, 14,2, 1,1, 14,4, 1,1, 14,171, 13,1, 14,1, 12,1, 14,42, 10,1, 14,13, 0,1, 14,3, 1,4, 14,172, 13,2, 14,1, 12,1, 14,60, 1,1, 14,175, 13,1, 14,2, 12,1, 14,41, 10,1, 14,13, 0,1, 14,180, 13,2, 14,1, 12,1, 14,54, 0,1, 14,6, 0,1, 14,174, 13,2, 14,1, 12,1, 14,53, 0,1, 14,182, 13,2, 14,1, 12,1, 14,53, 0,1, 14,4, 0,1, 14,42, 9,1, 14,134, 13,2, 14,2, 12,1, 14,52, 0,2, 14,1, 0,2, 14,178, 13,2, 14,2, 12,1, 14,51, 0,4, 14,180, 13,3, 14,2, 12,2, 14,234, 13,3, 14,1, 12,5, 14,111, 10,1, 14,119, 13,4, 14,2, 12,6, 14,109, 10,1, 14,119, 13,4, 14,2, 12,6, 14,66, 8,7, 14,156, 13,3, 14,3, 12,4, 14,28, 10,1, 14,37, 8,12, 14,51, 13,1, 14,100, 13,5, 14,1, 12,5, 14,4, 11,2, 14,26, 7,1, 14,31, 8,14, 14,25, 10,2, 14,8, 10,4, 14,12, 13,1, 14,100, 13,4, 14,2, 12,4, 14,4, 11,1, 14,26, 7,1, 14,31, 8,15, 14,18, 12,1, 14,6, 10,2, 14,2, 12,10, 14,12, 13,1, 14,100, 13,4, 14,1, 12,4, 14,31, 7,1, 14,24, 8,24, 14,17, 12,1, 14,6, 10,2, 14,3, 12,10, 14,12, 13,2, 14,98, 13,4, 14,1, 12,4, 14,30, 7,1, 14,22, 8,27, 14,17, 12,1, 14,11, 12,8, 14,14, 13,2, 14,98, 13,4, 14,1, 12,3, 14,53, 8,29, 14,9, 9,1, 14,6, 12,1, 14,12, 12,6, 14,16, 13,2, 14,97, 13,3, 14,2, 12,2, 14,31, 7,1, 14,20, 8,32, 14,8, 9,1, 14,6, 12,1, 14,35, 13,2, 14,96, 13,3, 14,1, 12,3, 14,19, 10,1, 14,11, 7,1, 14,18, 8,36, 14,4, 9,3, 14,6, 12,1, 14,36, 13,2, 14,95, 13,3, 14,1, 12,1, 14,3, 11,1, 14,17, 8,1, 14,11, 7,3, 14,14, 8,41, 14,1, 9,3, 14,6, 12,1, 14,37, 13,1, 14,96, 13,2, 14,1, 12,1, 14,2, 11,1, 14,46, 8,42, 9,4, 14,43, 13,1, 14,97, 13,3, 12,2, 14,1, 11,1, 14,45, 8,38, 14,2, 9,6, 14,7, 13,1, 14,134, 13,3, 12,2, 14,17, 9,1, 14,27, 8,5, 14,3, 8,32, 14,2, 9,6, 14,142, 13,1, 14,2, 12,1, 14,17, 9,1, 8,1, 14,26, 8,4, 14,6, 8,30, 9,9, 14,7, 13,1, 14,24, 12,3, 14,106, 13,2, 14,1, 12,1, 14,17, 9,1, 8,1, 14,26, 8,3, 14,1, 9,5, 14,2, 8,29, 14,1, 9,9, 14,7, 13,1, 14,20, 12,6, 10,1, 14,106, 13,1, 14,1, 12,1, 14,17, 9,1, 8,1, 14,25, 8,3, 14,1, 9,7, 14,2, 8,13, 14,17, 9,8, 14,8, 13,1, 14,8, 15,2, 14,9, 12,5, 14,108, 13,1, 14,1, 12,1, 14,17, 9,1, 14,26, 8,3, 14,1, 9,9, 14,1, 8,10, 14,5, 9,23, 14,8, 13,1, 14,8, 15,3, 14,12, 10,1, 14,108, 13,1, 14,19, 9,1, 8,1, 14,24, 8,2, 14,1, 9,11, 14,1, 8,5, 14,10, 9,23, 14,8, 13,1, 14,8, 15,6, 14,6, 10,3, 14,128, 9,1, 8,1, 14,23, 8,2, 14,15, 8,5, 14,6, 9,25, 14,10, 13,1, 14,7, 16,1, 14,1, 15,5, 14,5, 10,2, 14,129, 9,1, 14,1, 8,1, 14,22, 8,2, 14,3, 9,5, 14,8, 8,5, 14,5, 10,1, 9,24, 14,11, 13,1, 14,7, 16,1, 14,1, 15,5, 14,136, 9,1, 14,1, 8,1, 14,20, 8,3, 14,1, 9,9, 14,3, 8,9, 14,2, 10,4, 9,23, 10,1, 14,13, 10,1, 14,5, 16,2, 14,1, 15,4, 14,135, 9,1, 14,1, 8,1, 14,20, 8,1, 14,4, 9,9, 14,2, 8,6, 14,6, 10,4, 9,8, 14,35, 16,2, 14,1, 15,2, 14,137, 9,1, 14,1, 8,2, 14,17, 8,1, 14,3, 9,12, 14,2, 8,6, 14,5, 10,6, 9,6, 10,1, 14,35, 16,2, 14,2, 16,2, 14,6, 13,1, 14,129, 9,1, 14,1, 8,1, 14,16, 8,2, 14,2, 9,10, 10,4, 14,13, 10,11, 14,37, 16,5, 14,136, 9,2, 14,1, 8,2, 14,13, 8,2, 14,2, 9,11, 10,5, 14,12, 10,9, 14,40, 16,4, 14,137, 9,2, 14,1, 8,5, 14,7, 8,2, 14,3, 9,13, 10,5, 14,11, 10,8, 14,41, 17,1, 14,1, 16,1, 14,38, 10,6, 14,95, 9,2, 14,1, 8,4, 14,6, 9,6, 10,35, 14,44, 17,1, 14,3, 17,1, 14,34, 10,2, 14,100, 9,4, 14,1, 9,5, 14,20, 10,22, 14,48, 17,2, 14,45, 12,1, 14,92, 9,3, 14,1, 9,3, 14,23, 10,21, 14,189, 9,5, 14,26, 10,19, 14,98, 10,2, 14,91, 9,1, 14,35, 10,12, 14,49, 12,1, 14,33, 16,1, 15,5, 14,10, 10,4, 14,5, 13,1, 14,216, 15,4, 14,10, 10,4, 14,216, 16,1, 14,6, 15,2, 14,12, 10,2, 14,225, 15,1, 14,228, 15,3, 14,4, 15,7, 14,225, 15,15, 14,228, 15,19, 14,207, 17,1, 14,3, 16,1, 14,3, 15,26, 14,197, 12,4, 14,6, 16,2, 14,5, 15,15, 14,7, 15,6, 14,199, 17,2, 14,1, 16,5, 14,5, 15,17, 14,2, 15,8, 14,198, 17,1, 14,3, 16,4, 14,7, 15,14, 14,6, 15,6, 14,191, 12,1, 14,8, 16,6, 14,6, 15,27, 14,192, 12,1, 14,5, 16,7, 14,7, 15,19, 14,62, 13,1, 14,143, 16,5, 14,8, 15,21, 14,61, 13,1, 14,1, 12,1, 14,136, 12,1, 14,3, 16,4, 14,10, 15,25, 14,57, 13,1, 14,139, 12,1, 14,2, 16,2, 14,12, 15,27, 14,6, 10,1, 14,5, 13,1, 14,43, 13,1, 14,1, 12,2, 14,137, 17,1, 14,1, 16,1, 14,13, 15,26, 14,6, 10,3, 14,4, 13,1, 14,42, 13,2, 14,1, 12,1, 14,138, 17,1, 16,2, 14,9, 15,29, 14,7, 10,2, 14,48, 13,1, 14,2, 12,1, 14,127, 12,1, 13,1, 14,5, 18,1, 14,3, 17,1, 16,2, 14,8, 15,30, 14,56, 13,2, 14,1, 12,1, 14,135, 18,1, 14,1, 12,1, 14,1, 17,1, 16,2, 14,5, 15,33, 14,7, 12,1, 14,47, 13,2, 14,1, 12,2, 14,127, 12,1, 13,1, 14,10, 17,1, 16,2, 14,3, 15,35, 14,6, 12,1, 14,48, 13,2, 12,2, 14,129, 13,1, 14,10, 17,1, 16,2, 14,3, 15,35, 14,6, 12,1, 14,47, 13,2, 14,1, 12,2, 14,127, 12,1, 13,1, 14,12, 16,2, 14,3, 15,34, 14,7, 12,2, 14,46, 13,2, 14,1, 12,2, 14,126, 12,1, 14,1, 13,1, 14,12, 16,2, 14,2, 15,35, 14,7, 12,2, 14,45, 13,2, 14,1, 12,2, 14,126, 12,2, 14,1, 13,1, 14,12, 16,2, 14,2, 15,37, 14,5, 12,3, 14,44, 13,2, 14,1, 12,2, 14,125, 12,1, 14,2, 13,1, 14,13, 16,2, 14,1, 15,37, 14,7, 12,2, 14,43, 13,2, 14,1, 12,2, 14,26, 11,2, 14,97, 12,1, 14,2, 13,2, 14,13, 16,2, 14,1, 15,37, 14,9, 10,1, 14,42, 13,2, 14,1, 12,2, 14,24, 11,5, 14,95, 12,1, 14,2, 13,2, 14,14, 16,2, 14,2, 15,37, 14,8, 10,1, 14,41, 13,2, 14,1, 12,4, 14,21, 11,8, 14,93, 12,1, 14,2, 13,2, 14,15, 16,2, 14,2, 15,38, 14,7, 10,2, 14,2, 13,1, 14,37, 13,2, 14,1, 12,4, 14,25, 11,4, 14,38, 11,6, 14,48, 12,1, 14,1, 13,3, 14,16, 16,2, 14,2, 15,39, 14,47, 13,3, 12,5, 14,25, 11,5, 14,33, 11,12, 14,45, 12,2, 13,4, 14,16, 16,2, 14,2, 15,41, 14,7, 13,1, 14,37, 13,3, 14,1, 12,5, 14,25, 11,5, 14,27, 11,22, 14,39, 12,2, 14,1, 13,3, 14,17, 16,2, 14,2, 15,41, 14,45, 13,2, 14,2, 12,7, 14,24, 11,6, 14,17, 11,32, 14,36, 12,2, 14,1, 13,3, 14,18, 16,2, 14,2, 15,41, 14,44, 13,2, 14,3, 12,10, 14,18, 11,13, 14,9, 11,18, 14,16, 11,3, 14,9, 11,5, 14,19, 12,3, 14,2, 13,3, 14,18, 16,2, 14,2, 15,41, 14,44, 13,2, 14,1, 12,4, 14,8, 12,5, 14,13, 11,40, 12,11, 14,5, 11,17, 14,15, 12,6, 14,1, 13,5, 14,18, 16,2, 14,2, 15,31, 14,54, 13,2, 14,1, 12,3, 14,2, 13,6, 14,3, 12,8, 14,15, 11,31, 14,2, 12,11, 14,5, 11,14, 14,13, 12,6, 14,6, 13,5, 14,18, 16,4, 14,1, 15,30, 14,8, 16,1, 14,45, 13,2, 14,5, 13,10, 14,1, 12,8, 14,15, 11,30, 14,2, 12,13, 14,7, 11,8, 14,12, 12,8, 14,1, 13,9, 14,18, 17,1, 14,1, 16,4, 14,1, 15,29, 14,2, 16,7, 14,46, 13,2, 14,3, 13,4, 14,5, 13,4, 14,1, 12,15, 14,7, 11,29, 14,3, 12,15, 14,8, 11,3, 14,13, 12,3, 14,5, 13,4, 14,24, 19,1, 17,1, 14,1, 16,4, 14,9, 15,21, 14,1, 16,6, 14,3, 17,1, 14,44, 13,8, 14,7, 13,4, 12,15, 14,7, 11,29, 14,3, 12,16, 14,7, 11,3, 14,4, 12,9, 14,3, 13,6, 14,27, 19,1, 17,1, 14,1, 16,3, 14,10, 15,16, 14,5, 16,6, 14,3, 17,1, 14,45, 13,7, 14,8, 13,4, 12,17, 14,30, 11,1, 14,6, 12,16, 14,14, 12,6, 14,4, 13,3, 14,32, 19,1, 17,1, 14,1, 16,3, 14,10, 15,16, 14,5, 16,5, 14,3, 17,1, 14,61, 13,4, 12,21, 14,17, 12,7, 14,5, 12,26, 14,6, 12,8, 14,2, 13,4, 14,34, 17,1, 14,1, 16,3, 14,10, 15,16, 14,5, 16,4, 14,3, 17,1, 14,61, 13,4, 14,1, 12,85, 14,5, 13,3, 14,37, 17,1, 14,1, 16,3, 14,10, 15,16, 14,2, 16,6, 14,2, 17,2, 14,61, 13,4, 14,1, 12,86, 14,1, 13,6, 14,38, 17,1, 14,1, 16,3, 14,4, 15,22, 14,2, 16,5, 14,3, 17,1, 14,61, 13,5, 12,86, 13,6, 14,40, 17,1, 14,1, 16,3, 14,4, 15,22, 14,1, 16,5, 14,65, 13,5, 12,85, 14,2, 13,3, 14,43, 17,1, 14,1, 16,3, 14,4, 15,22, 14,2, 16,4, 14,1, 17,2, 14,61, 13,4, 14,1, 12,76, 14,10, 13,4, 14,44, 17,1, 14,1, 16,3, 14,4, 15,22, 14,1, 16,4, 14,65, 13,4, 14,1, 12,63, 14,2, 12,11, 13,13, 14,45, 17,1, 14,1, 16,3, 14,4, 15,23, 14,1, 16,3, 14,1, 17,1, 14,63, 13,4, 14,1, 12,12, 14,2, 13,15, 12,30, 14,4, 13,1, 14,5, 12,6, 14,1, 13,11, 14,47, 17,1, 14,1, 16,2, 14,5, 15,23, 14,1, 16,2, 14,1, 17,1, 14,63, 13,4, 14,1, 12,13, 14,2, 13,16, 14,9, 13,6, 14,2, 12,7, 14,5, 13,10, 14,7, 13,10, 14,48, 17,1, 14,1, 16,2, 14,5, 15,22, 14,1, 16,3, 14,1, 17,1, 14,63, 13,4, 14,1, 12,11, 14,2, 13,34, 14,1, 12,4, 14,4, 13,30, 14,49, 17,1, 14,1, 16,2, 14,5, 15,22, 16,4, 14,1, 17,1, 14,1, 19,1, 14,61, 13,4, 14,1, 12,11, 13,9, 14,7, 13,58, 14,50, 17,1, 14,1, 16,2, 14,5, 15,22, 16,4, 14,1, 17,1, 14,62, 13,5, 14,1, 12,11, 13,8, 14,10, 13,51, 14,56, 16,3, 14,5, 15,22, 14,1, 16,3, 14,1, 17,1, 14,62, 13,5, 14,12, 13,6, 14,23, 13,23, 14,3, 13,11, 14,59, 16,1, 14,7, 15,22, 14,1, 16,3, 14,1, 17,1, 14,62, 13,21, 14,27, 13,19, 14,4, 13,10, 14,69, 15,22, 14,1, 16,3, 14,1, 17,1, 14,61, 13,21, 14,29, 13,15, 14,9, 13,6, 14,58, 13,1, 14,12, 15,22, 14,1, 16,3, 14,2, 17,1, 14,61, 13,16, 14,31, 13,7, 14,96, 15,22, 14,1, 16,4, 14,1, 17,1, 14,65, 13,7, 14,37, 13,5, 14,97, 15,22, 14,1, 16,4, 14,2, 17,1, 14,94, 13,4, 14,9, 13,5, 14,84, 13,1, 14,13, 15,22, 14,2, 16,4, 14,2, 17,1, 14,91, 13,5, 14,113, 15,22, 14,1, 16,6, 14,2, 17,1, 14,88, 13,7, 14,104, 15,6, 14,3, 15,22, 14,2, 16,6, 14,86, 13,11, 14,100, 10,1, 14,3, 15,6, 14,3, 15,22, 14,2, 16,6, 14,86, 13,10, 14,101, 10,1, 14,3, 15,7, 14,2, 15,22, 14,3, 16,6, 14,86, 13,7, 14,102, 10,1, 14,4, 15,7, 14,2, 15,22, 14,3, 16,6, 14,85, 13,9, 14,101, 12,1, 14,4, 15,7, 14,2, 15,19, 14,6, 16,6, 14,87, 13,7, 14,101, 12,1, 14,4, 15,6, 14,14, 15,8, 14,6, 16,6, 14,195, 12,1, 14,4, 15,6, 14,21, 16,13, 14,194, 12,2, 14,4, 15,6, 14,21, 16,13, 14,26, 12,1, 14,167, 12,2, 14,4, 15,6, 14,21, 16,13, 14,2, 17,1, 14,17, 17,1, 14,6, 12,1, 14,166, 12,2, 14,4, 15,6, 14,21, 16,14, 14,1, 17,1, 14,22, 17,1, 14,2, 12,1, 14,165, 12,2, 14,4, 15,6, 14,21, 16,15, 14,1, 17,1, 14,12, 17,1, 14,2, 16,6, 14,1, 17,1, 14,1, 12,1, 14,165, 12,2, 14,4, 15,6, 14,21, 16,15, 14,2, 17,1, 14,9, 17,2, 14,2, 16,7, 14,2, 17,1, 14,166, 12,2, 14,4, 15,6, 14,21, 16,17, 14,2, 17,1, 14,10, 16,9, 14,168, 12,2, 14,4, 15,6, 14,21, 16,18, 14,3, 17,2, 14,5, 16,12, 14,1, 17,1, 14,165, 12,2, 14,4, 15,6, 14,21, 16,20, 14,5, 16,14, 14,3, 17,1, 14,164, 12,2, 14,4, 15,6, 14,21, 16,41, 14,1, 17,1, 14,164, 12,2, 14,4, 15,6, 14,21, 16,41, 14,2, 17,1, 14,163, 12,2, 14,4, 15,6, 14,21, 16,42, 14,2, 17,1, 14,162, 12,2, 14,4, 15,6, 14,21, 16,43, 14,2, 17,1, 14,161, 12,2, 14,4, 15,6, 14,24, 16,41, 14,2, 17,1, 14,160, 12,2, 14,4, 15,6, 14,24, 16,42, 14,1, 17,2, 14,159, 12,2, 14,4, 15,6, 14,24, 16,42, 14,3, 17,1, 14,158, 12,2, 14,4, 15,6, 14,24, 16,44, 14,1, 17,2, 14,158, 12,1, 14,10, 16,69, 14,2, 17,1, 14,156, 12,1, 14,11, 16,69, 14,3, 17,1, 14,155, 12,1, 14,11, 16,70, 14,2, 17,2, 14,154, 12,1, 14,11, 16,68, 14,3, 17,3, 14,154, 12,1, 14,1, 10,1, 14,16, 17,64, 14,4, 19,1, 14,1, 12,1, 14,184, 19,48, 14,2, 12,4, 14,158, 12,5, 14,46, 20,30, 14,8, 18,1, 14,199, 18,35, 14,48, 12,1, 14,129, 21,30, 14,254, 14,27, 13,1, 14,1, 12,1, 14,4, 12,7, 14,4, 12,4, 14,9, 13,1, 14,47, 12,2, 14,5, 12,3, 14,9, 13,8, 14,27, 13,1, 14,41, 13,2, 14,10, 13,2, 14,15, 12,38, 14,13, 12,2, 14,6, 12,1, 14,9, 12,5, 14,3, 12,1, 14,8, 12,5, 14,15, 12,8, 14,14, 12,1, 14,40, 12,6, 14,6, 12,91, 14,1, 16,2, 14,4, 16,5, 14,5, 16,4, 14,5, 16,2, 14,46, 16,8, 14,2, 16,4, 12,5, 14,1, 12,2, 14,50, 14,1,};
#endif
