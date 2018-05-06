#include "id3.h"

static int cmp_feature = 0, *data, data_length, feature_length, depth, impurity, kind_number, *class_count;
static id3_node_t *decision_tree_root;

//排序函数
static int cmp(const void *a, const void *b) {
    int *ifa = (int *)a, *ifb = (int *)b;
    return *(ifa + cmp_feature) - *(ifb + cmp_feature);
}

static void destory_tree(id3_node_t *node) {
    id3_node_t *tmp_node, *tmp_node2;
    for(tmp_node = node; tmp_node != NULL; tmp_node = tmp_node2) {
        tmp_node2 = tmp_node->brother;
        if(!(tmp_node->is_leaf)) destory_tree(tmp_node->eldest_child);
        free(tmp_node);
    }
}

static void dump_data(FILE *file_p, id3_node_t *node, int tab_number) {
    id3_node_t *tmp_node;
    for(tmp_node = node; tmp_node != NULL; tmp_node = tmp_node->brother) {
        for(int loop_i = 0; loop_i < tab_number; ++loop_i) {
            fprintf(file_p, "\t");
        }
        if(tmp_node->is_leaf) {
            fprintf(file_p, "0x%X, %d, %lf, %d, %d\n", tmp_node->feature_used, tmp_node->data_length, tmp_node->entropy, tmp_node->impurity, tmp_node->class_name);
        } else {
            fprintf(file_p, "0x%X, %d, %lf, %d\n", tmp_node->feature_used, tmp_node->data_length, tmp_node->entropy, tmp_node->impurity);
            dump_data(file_p, tmp_node->eldest_child, tab_number + 1);
        }
    }
}

static double log_2(double a) {
    return a?log2(a):0;
}

static double get_entropy(int *_data, int length, int *impurity) {
    int big_number, loop_i;
    double sum = 0;
    // for(loop_i = 0; loop_i < length; ++loop_i) {
    //     for(int loop_j = 0; loop_j <= feature_length; ++loop_j) {
    //         printf("%d", *(_data + (feature_length + 1) * loop_i + loop_j));
    //     }
    //     printf("\n");
    // }
    // printf("\n");
    memset(class_count, 0, sizeof(int) * kind_number);

    for(loop_i = 0; loop_i < length; ++loop_i) {
        (class_count[_data[loop_i * (feature_length + 1) + feature_length]])++;
    }
    big_number = 0;
    for(loop_i = 0; loop_i < kind_number; ++loop_i) {
        sum += - class_count[loop_i] * log_2(class_count[loop_i]/(double)length);
        if(class_count[loop_i] > big_number) big_number = class_count[loop_i];
    }

    *impurity = (length - big_number) * 100 / length;
    return sum;
}

static void create_tree(id3_node_t *node, int now_depth) {
    double best_gain = __DBL_MAX__, tmp_gain;
    int loop_i, loop_j, best_gain_index, *data_p = node->data, data_index, data_kind_length, tmp_impurity;
    id3_node_t *tmp_node;
    //分支结束
    if(node->impurity <= impurity || now_depth > depth|| node->feature_used == ((1 << feature_length) - 1) || node->entropy == 0) {
        //确认为叶子节点
        node->is_leaf = 1;
        //如果不纯洁度小与50，说明有大多数元素，将大多数元素作为类别标签
        if(node->impurity < 50) {
            //求大多数元素
            data_index = data_p[feature_length];
            data_kind_length = 1;
            for(loop_i = 1; loop_i < node->data_length; ++loop_i) {
                if(data_kind_length == 0) {
                    data_index = data_p[loop_i * (feature_length + 1) + feature_length];
                    data_kind_length = 1;
                } else {
                    if(data_index == data_p[loop_i * (feature_length + 1) + feature_length]) {
                        ++data_kind_length;
                    } else {
                        --data_kind_length;
                    }
                }
            }
            node->class_name = data_index;
        } else {
            node->class_name = 1;
        }
        return;
    }
    best_gain_index = 0;
    //best_gain = 0;
    //求使熵最小即熵变最大的元素
    for(loop_i = 0; loop_i < feature_length; ++loop_i) {
        if(!((1 << loop_i)&node->feature_used)) {
            //printf("%d\n", (1 << loop_i));
            cmp_feature = loop_i;
            qsort(data_p, node->data_length, sizeof(int) * (feature_length + 1), cmp);
            tmp_gain = 0;
            data_kind_length = 1;
            data_index = 0;

            for(loop_j = 1; loop_j < node->data_length; ++loop_j) {
                if(data_p[(feature_length + 1) * (loop_j - 1) + loop_i] == data_p[(feature_length + 1) * (loop_j) + loop_i]) {
                    data_kind_length++;
                } else {
                    tmp_gain += get_entropy(data_p + data_index * (feature_length + 1), data_kind_length, &tmp_impurity);
                    //printf("%lf\n", tmp_gain);
                    data_kind_length = 1;
                    data_index = loop_j;
                }
            }
            if(data_kind_length != 1) tmp_gain += get_entropy(data_p + data_index * (feature_length + 1), data_kind_length, &tmp_impurity);

            if(tmp_gain < best_gain) {
                //printf("%lf\n", tmp_gain);
                best_gain = tmp_gain;
                best_gain_index = loop_i;
            }
        } 
    }
    //划分下一节点
    data_index = 0;
    data_kind_length = 1;
    cmp_feature = best_gain_index;
    qsort(data_p, node->data_length, sizeof(int) * (feature_length + 1), cmp);
    for(loop_i = 1; loop_i < node->data_length; ++loop_i) {
        if(data_p[(feature_length + 1) * (loop_i - 1) + best_gain_index] == data_p[(feature_length + 1) * (loop_i) + best_gain_index]) {
            ++data_kind_length;
        } else {
            tmp_node = (id3_node_t *) malloc(sizeof(id3_node_t));
            tmp_node->brother = node->eldest_child;
            node->eldest_child = tmp_node;
            tmp_node->data = data_p + data_index * (feature_length + 1);
            tmp_node->feature_used = (node->feature_used | (1 << best_gain_index));
            tmp_node->eldest_child = NULL;
            tmp_node->is_leaf = 0;
            tmp_node->data_length = data_kind_length;
            tmp_node->entropy = get_entropy(data_p + data_index * (feature_length + 1), data_kind_length, &(tmp_node->impurity));
            create_tree(tmp_node, now_depth + 1);
            data_kind_length = 1;
            data_index = loop_i;
        }
    }

    if(data_kind_length != 1) {
        tmp_node = (id3_node_t *) malloc(sizeof(id3_node_t));
        tmp_node->brother = node->eldest_child;
        node->eldest_child = tmp_node;
        tmp_node->data = data_p + data_index* (feature_length + 1);
        tmp_node->feature_used = (node->feature_used | (1 << best_gain_index));
        tmp_node->eldest_child = NULL;
        tmp_node->is_leaf = 0;
        tmp_node->data_length = data_kind_length;
        tmp_node->entropy = get_entropy(data_p + data_index * (feature_length + 1), data_kind_length, &(tmp_node->impurity));
        create_tree(tmp_node, now_depth + 1);
    }
}

extern void id3_init(int *_data, int _data_length, int _feature_length, int _kind_number, int _depth, int _impurity) {
    data = _data;
    data_length = _data_length;
    feature_length = _feature_length;
    depth = _depth;
    impurity = _impurity;
    kind_number = _kind_number;
    class_count = (int *) malloc(sizeof(int) * kind_number);
    decision_tree_root = (id3_node_t *) malloc(sizeof(id3_node_t));
    decision_tree_root->brother = NULL;
    decision_tree_root->data = data;
    decision_tree_root->eldest_child = NULL;
    decision_tree_root->feature_used = 0;
    decision_tree_root->data_length = data_length;
    decision_tree_root->entropy = get_entropy(data, data_length, &(decision_tree_root->impurity));
    if(_depth > 0 && decision_tree_root->impurity > impurity) decision_tree_root->is_leaf = 0;
}

extern void id3_run() {
    create_tree(decision_tree_root, 1);
}

extern void id3_dump(FILE *file_p) {
    if(file_p == NULL) return;
    dump_data(file_p, decision_tree_root, 0);
}

extern void id3_destory() {
    free(class_count);
    destory_tree(decision_tree_root);
}