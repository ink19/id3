#ifndef __ID3_H
#define __ID3_H 1
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

typedef struct __ID3_NODE {
    int feature_used;
    double entropy;
    int *data;
    int data_length;
    int impurity;
    int is_leaf;
    int class_name;
    struct __ID3_NODE * brother;
    struct __ID3_NODE * eldest_child;
} id3_node_t;
/**
 * data    数据，一维int类型，逻辑上分为data_length组，每组(feature_length + 1) * sizeof(int)的长度，最后一个int代表类别
 * data_length 数据个数
 * feature_length 特征值个数
 * kind_number 类型数量
 * depth 构造的决策树最大深度
 * impurity 在杂质不超过impurity%时停止构造
 */
extern void id3_init(int *data, int data_length, int feature_length, int kind_number, int depth, int impurity);

extern void id3_run();

extern void id3_dump(FILE *file_p);

extern void id3_destory();
#endif