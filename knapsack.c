// My program works for greedy and DP and BB solution .
// I use Linux 7.4.0


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

typedef struct{
    int benefit;
    int weight;
    float value;
} Item;

typedef struct{
    int height;
    int benefit;
    int weight;
    float bound;
} Bb;

void sort(Item *item, int num_item){

    Item key;
    int i;
    for(int j = 2; j <= num_item; j++){
        key = item[j];
        i = j - 1;
        while(i > 0 && item[i].value < key.value){
            item[i+1] = item[i];
            i = i - 1;
        }
        item[i+1] = key;
    }
}

void Greedy(FILE *fp, Item *item, int W, int num_item){

    clock_t start, end;
    float t;

    start = clock();
    sort(item, num_item);

    int tot_weight = 0;
    float benefit_sum = 0;
    int i = 1;

    while(1){
        if(tot_weight+item[i].weight > W){
            benefit_sum += (W-tot_weight) * item[i].value;
            break;
        }
        tot_weight += item[i].weight;
        benefit_sum += item[i].benefit;
        i++;
    }
    end = clock();
    t = (float)(end-start)/CLOCKS_PER_SEC;
    t = t*1000;

    // write in file
    char str[128];
    char s[24];
    sprintf(str, "%.3lf", t);
    strcat(str, " ms / ");
    sprintf(s, "%.3f", benefit_sum);
    strcat(str, s);

    printf(" %24s |", str);
    fprintf(fp, " %24s |", str);

}

void DP(FILE *fp, Item *item, int W, int num_item){
    
    clock_t start, end, check;
    float t;

    start = clock();

    int **b;
    b = (int **) malloc (sizeof(int *) * (2));
    b[0] = (int *) malloc (sizeof(int) * (W+1)*2);
    for( int i = 1; i < 2; i++)
        b[i] = b[ i-1 ] + (W+1);
    
    int max_benefit = 0;
    int num, a, w;
    char str[128];

    for(int w = 0; w <= W; w++)
        b[0][w] = 0;

    for(int i = 1; i <= num_item; i++){

        check = clock();
        t = (float)(check-start)/CLOCKS_PER_SEC;
        t = t*1000;

        if(t >= 900000){
            strcpy(str, "OVER 15min");
            printf(" %22s |\n", str);
            fprintf(fp, " %22s |\n", str);
            return;
        }

        if(i%2 == 1){
            num = 1;
            a = num - 1;
        }else{
            num = 0;
            a = num + 1;
        }

        b[num][0] = 0;
        for(w = 1; w <= W; w++){
            if(item[i].weight <= w){
                if(item[i].benefit + b[a][w-item[i].weight] > b[a][w])
                    b[num][w] = item[i].benefit + b[a][w-item[i].weight];
                else
                    b[num][w] = b[a][w];
            }
            else
                b[num][w] = b[a][w-1];
        }

        max_benefit = b[num][w-1];
    }
    free(b[0]);
    free(b);

    end = clock();


    t = (float)(end-start)/CLOCKS_PER_SEC;
    t = t*1000;

    // write in file
    char s[24];
    sprintf(str, "%.3lf", t);
    strcat(str, " ms / ");
    sprintf(s, "%d", max_benefit);
    strcat(str, s);

    printf(" %23s |", str);
    fprintf(fp, " %23s |", str);

}

void resize(int *size, Bb **bb){

    int newsize = (*size) * 2;
    Bb *temp = malloc(sizeof(Bb) * newsize);

    for(int i = 0; i < newsize; i++)
        temp[i] = (*bb)[i];
    
    free(*bb);
    *bb = temp;
    *size = newsize;

}

float bound(Bb b, int W, int num_item, Item *item){

    if(b.weight > W)
        return 0;
    
    int tot_weight = b.weight;
    int i = b.height+1;
    float benefit_sum = b.benefit;

    while(1){

        if(i > num_item || tot_weight+item[i].weight > W){
            if(i <= num_item){
                benefit_sum += (W-tot_weight) * item[i].value;
            }
            break;
        }
        tot_weight += item[i].weight;
        benefit_sum += item[i].benefit;
        i++;
    }

    return benefit_sum;
}

Bb delete(Bb *bb, int N){

    Bb temp = bb[0];
    
    if(N > 0){
        for(int i = 0; i < N-1; i++){
            bb[i] = bb[i+1];
        }
    }

    return temp;
}

void sort_bb(Bb *bb, int N){

    Bb key;
    int i;
    for(int j = 1; j < N; j++){
        key = bb[j];
        i = j - 1;
        while(i >= 0 && bb[i].bound < key.bound){
            bb[i+1] = bb[i];
            i = i - 1;
        }
        bb[i+1] = key;
    }
}

void BranchAndBound(FILE *fp, Item *item, int W, int num_item){

    clock_t start, end, check;
    float t;
    char str[128];
    start = clock();

    sort(item, num_item);

    int size = num_item+1;
    Bb *bb = malloc(sizeof(Bb) * size);

    int N = 0, j = 0;
    int MAX = 0;

    Bb temp;
    temp.height = 0;
    temp.benefit = 0;
    temp.weight = 0;
    temp.bound = bound(temp, W, num_item, item);

    bb[N++] = temp;

    Bb root, child;
    while(N > 0){

        check = clock();
        t = (float)(check-start)/CLOCKS_PER_SEC;
        t = t*1000;

        if(t >= 900000){
            strcpy(str, "OVER 15min");
            printf(" %22s |\n", str);
            fprintf(fp, " %22s |\n", str);
            return;
        }

        // array의 크기가 부족하면 늘린다
        if(N+1 == size)
            resize(&size, &bb);

        sort_bb(bb, N);
        root = delete(bb, N--);
        
        if(root.bound > MAX){

            child.height = root.height + 1;
 
            child.weight = root.weight + item[child.height].weight;
            child.benefit = root.benefit + item[child.height].benefit;

            if(child.benefit > MAX && child.weight <= W)
                MAX = child.benefit;

            child.bound = bound(child, W, num_item, item);
            // 현재를 포함하는 child 노드 생성
            if(child.bound > MAX)
                bb[N++] = child;
            
            child.weight = root.weight;
            child.benefit = root.benefit;
            child.bound = bound(child, W, num_item, item);

            // 현재를 포함하지 않는 child 노드 생성
            if(child.bound > MAX)
                bb[N++] = child;
        }
        

    }
    free(bb);
    end = clock();
    t = (float)(end-start)/CLOCKS_PER_SEC;
    t = t*1000;

    // write in file
    char s[24];
    sprintf(str, "%.3lf", t);
    strcat(str, " ms / ");
    sprintf(s, "%d", MAX);
    strcat(str, s);

    printf(" %24s |\n", str);
    fprintf(fp, " %24s |\n", str);

    
}

int main(){

    int num_item[9] = {10, 100, 500, 1000, 3000, 5000, 7000, 9000, 10000};
    int W[9];
    srand(time(NULL));

    FILE *fp = fopen("output.txt", "w");

    printf(" -----------------------------------------------------------------------------------------------------\n");
    fprintf(fp, " -----------------------------------------------------------------------------------------------------\n");
    
    printf("|      Number of      |             Processing time in milliseconds / Maximum benefit value           |\n");
    fprintf(fp, "|      Number of      |             Processing time in milliseconds / Maximum benefit value           |\n");
    
    printf("|                     |-------------------------------------------------------------------------------|\n");
    fprintf(fp, "|                     |-------------------------------------------------------------------------------|\n");
    
    printf("|        Items        |          Greedy          |           D.P.          |           B.&B.          |\n");
    fprintf(fp, "|        Items        |          Greedy          |           D.P.          |           B.&B.          |\n");
    
    printf("|-----------------------------------------------------------------------------------------------------|\n");
    fprintf(fp, "|-----------------------------------------------------------------------------------------------------|\n");
   
    for(int i = 0; i < 9; i++){
        Item item[num_item[i]+1];
        Item temp[num_item[i]+1];
        item[0].benefit = 0; temp[0].benefit = 0;
        item[0].weight = 0; temp[0].weight = 0;
        item[0].value = 0; temp[0].value = 0;
        

        for(int j = 1; j <= num_item[i]; j++){
            item[j].benefit = (rand()%300) + 1;
            temp[j].benefit = item[j].benefit;
            item[j].weight = (rand()%100) + 1;
            temp[j].weight = item[j].weight;
            item[j].value = roundf((item[j].benefit/(float)item[j].weight) * 1000) / 1000;
            temp[j].value = item[j].value;
        }

        printf("|%*d%*s|", 11, num_item[i], 10 , " ");
        fprintf(fp, "|%*d%*s|", 11, num_item[i], 10 , " ");
        
        
        W[i] = num_item[i] * 40;
        Greedy(fp, temp, W[i], num_item[i]);
        DP(fp, item, W[i], num_item[i]);
        BranchAndBound(fp, item, W[i], num_item[i]);
        printf("|-----------------------------------------------------------------------------------------------------|\n");
        fprintf(fp, "|-----------------------------------------------------------------------------------------------------|\n");
    }

    fclose(fp);

    return 0;
}
