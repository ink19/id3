#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "id3.h"

int main() {
    FILE *infile;
    int data[70];
    infile = fopen("test.data", "r+");
    //infile = fopen("data.txt", "r+");
    if(infile == NULL) return 1;
    //data = (int *) malloc(sizeof(int ) * 3 * 5);
    // for(int loop_i = 0; loop_i < 14; ++loop_i) {
    //     fscanf(infile, "%d%d%d%d%d", data + loop_i * 5 + 0, data + loop_i * 5 + 1, data + loop_i * 5 + 2, data + loop_i * 5 + 3, data + loop_i * 5 + 4);
    // }
    for(int loop_i = 0; loop_i < 5; ++loop_i) {
        fscanf(infile, "%d%d%d", data + loop_i * 3 + 0, data + loop_i * 3 + 1, data + loop_i * 3 + 2);
    }
    fclose(infile);
    //id3_init(data, 14, 4, 2, 5, 10);
    id3_init(data, 5, 2, 2, 4, 1);
    id3_run();
    id3_dump(stdout);
    id3_destory();
    return 0;
}