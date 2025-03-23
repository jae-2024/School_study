#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CODE_SIZE 3000

/*노드간의 관리를 위한 구조체*/
typedef struct TreeNode {
    int weight;
    char ch;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

/*힙의 삽입 삭제를 관리하기 위한 구조체*/
typedef struct {
    TreeNode* ptree;
    char ch;
    int key;
} element;

typedef struct {
    element heap[100];
    int heap_size;
} HeapType;

HeapType* create() { return (HeapType*)malloc(sizeof(HeapType)); }

/*초기화 함수*/
void init(HeapType* h) { h->heap_size = 0; }

/*최소 힙 삽입 함수*/
void insert_min_heap(HeapType* h, element item) {
    int i;
    i = ++(h->heap_size);

    while ((i != 1) && (item.key < h->heap[i / 2].key)) {
        h->heap[i] = h->heap[i / 2];
        i /= 2;
    }
    h->heap[i] = item;
}

/*최소 힙 삭제 함수*/
element delete_min_heap(HeapType* h) {
    int parent, child;
    element item, temp;

    item = h->heap[1];
    temp = h->heap[(h->heap_size)--];
    parent = 1;
    child = 2;
    while (child <= h->heap_size) {
        if ((child < h->heap_size) &&
            (h->heap[child].key) > h->heap[child + 1].key) {
            child++;
        }
        if (temp.key < h->heap[child].key)
            break;

        h->heap[parent] = h->heap[child];
        parent = child;
        child *= 2;
    }
    h->heap[parent] = temp;
    return item;
}

/*이진 트리 생성 함수*/
TreeNode* make_tree(TreeNode* left, TreeNode* right) {
    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    node->left = left;
    node->right = right;
    return node;
}

/*이진 트리 제거 함수*/
void destroy_tree(TreeNode* root) {
    if (root == NULL)
        return;
    destroy_tree(root->left);
    destroy_tree(root->right);
    free(root);
}

/*이진트리에서 말단노드를 판단하는 함수*/
int is_leaf(TreeNode* root) { return !(root->left) && !(root->right); }

void print_array(int codes[], int n) {
    for (int i = 0; i < n; i++) {
        printf("%d", codes[i]);
    }
    printf("\n");
}

//배열의 각 요소가 문자열을 가리키는 포인터
char* huffman_codes[26];

void print_codes(TreeNode* root, int codes[], int top) {
    /*왼쪽노드이면 1을 저장*/
    if (root->left) {
        codes[top] = 1;
        print_codes(root->left, codes, top + 1);
    }
    /*오른쪽노드이면 0을 저장*/
    if (root->right) {
        codes[top] = 0;
        print_codes(root->right, codes, top + 1);
    }
    /*단말노드이면 위에서 저장된 된 코드를 출력*/
    if (is_leaf(root)) {
        // ex) huffman_codes[0] 즉, 'a'의 허프만 코드 "101"로 지정하는 코드드
        huffman_codes[root->ch - 'a'] = (char*)malloc(top + 1);
        for (int i = 0; i < top; i++) {
            huffman_codes[root->ch - 'a'][i] = codes[i] + '0';
        }
        huffman_codes[root->ch - 'a'][top] = '\0';
        printf("%c: ", root->ch);
        print_array(codes, top);
    }
}

/*허프만 코드 생성 함수*/
TreeNode* huffman_tree(int freq[], char ch_list[], int n) {
    int i;
    TreeNode* node, * x;
    HeapType* heap;
    element e, e1, e2;
    int codes[100];
    int top = 0;

    heap = create();
    init(heap);
    for (i = 0; i < n; i++) {
        node = make_tree(NULL, NULL);
        e.ch = node->ch = ch_list[i];
        e.key = node->weight = freq[i];
        e.ptree = node;
        insert_min_heap(heap, e);
    }
    for (i = 1; i < n; i++) {
        e1 = delete_min_heap(heap);
        e2 = delete_min_heap(heap);
        x = make_tree(e1.ptree, e2.ptree);
        e.key = x->weight = e1.key + e2.key;
        e.ptree = x;
        insert_min_heap(heap, e);
    }
    e = delete_min_heap(heap);
    print_codes(e.ptree, codes, top);
    free(heap);
    return e.ptree;
}
//입력문자열, 허프만 코드, 이진출력 문자열
void encode_huffman(char* input, char* huffman_codes[], char* binary_output) {
    binary_output[0] = '\0';

    for (int i = 0; input[i] != '\0'; i++) {
        //허프만 코드가 저장된 위치를 계산하여, 해당 위치의 저장된 허프만 코드를
        //가져오는 코드드
        char* huffman_code = huffman_codes[input[i] - 'a'];
        strcat(binary_output, huffman_code);
    }
}

void decode_huffman(TreeNode* root, const char* binary_input,
    char* decoded_output) {
    TreeNode* current = root;
    int decoded_index = 0;

    for (int i = 0; binary_input[i] != '\0'; i++) {
        if (binary_input[i] == '1') {
            current = current->left;
        }
        else if (binary_input[i] == '0') {
            current = current->right;
        }

        if (is_leaf(current)) {
            decoded_output[decoded_index++] = current->ch;
            //다시 이진 문자열을 비교하기위한 current 루트노드로 초기화
            current = root;
        }
    }
    decoded_output[decoded_index] = '\0';
}

int main(void) {
    char ch_list[CODE_SIZE];  /*입력받는 배열*/
    char ch_list2[CODE_SIZE]; /*입력받은 알파벳을 하나씩 입력*/
    int freq[26] = { 0 };  /*알파벳 빈도수*/
    int cnt = 0;         /*총 서로다른 알파벳 수*/

    FILE* file = fopen("huff.txt", "r");
    if (file == NULL) {
        fprintf(stderr, "파일을 열 수 없습니다.\n");
        return 1;
    }

    //파일에 여러줄이 저장된 문자열들을 ch_list2 배열에 저장하는 코드
    while (fgets(ch_list, sizeof(ch_list), file) != NULL) {
        for (int i = 0; ch_list[i] != '\0'; i++) {
            if (freq[tolower(ch_list[i]) - 'a'] == 0) {
                ch_list2[cnt++] = tolower(ch_list[i]);
            }
            freq[tolower(ch_list[i]) - 'a']++;
        }
        ch_list2[cnt] = '\0';
    }

    int freq_alpa[26];
    for (int i = 0; i < cnt; i++) {
        freq_alpa[i] = freq[ch_list2[i] - 'a'];
    }
    TreeNode* root = huffman_tree(freq_alpa, ch_list2, cnt); // 트리의 루트를 받아옴
    fclose(file);

    char binary_output[CODE_SIZE]; // 이진 출력 문자열을 저장할 공간 생성
    encode_huffman(ch_list, huffman_codes,
        binary_output); // 입력 문자열을 허프만 코드로 변환하여 이진
    printf("이진 출력 문자열: %s\n", binary_output);

    FILE* binary_file = fopen("binary_output.bin", "wb");
    if (binary_file == NULL) {
        fprintf(stderr, "파일을 열 수 없습니다.\n");
        return 1;
    }
    //바이너리 파일에 쓰기 위한 코드
    fwrite(binary_output, sizeof(char), strlen(binary_output), binary_file);

    // 이진 문자열을 다시 원래 문자열로 디코딩
    char decoded_output[CODE_SIZE];
    decode_huffman(root, binary_output, decoded_output);
    printf("디코딩된 문자열: %s\n", decoded_output);

    destroy_tree(root);

    return 0;
}