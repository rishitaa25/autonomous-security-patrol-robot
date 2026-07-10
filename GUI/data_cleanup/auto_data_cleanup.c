 /******************************************************************
  * GROUP G
  * 
  * AUTO DATA CLEAN UP
  * 
  * used chat gpt for formating, tips, and simple debugging
  * utilize moving average code and file reading from previous year
  *******************************************************************/


#include <stdio.h>
#include <stdlib.h>

#define MAX_POINTS 1000
#define WINDOW_SIZE 5
#define OBJECT_THRESHOLD 100.0


// Moving average buffer functions 
void updatebuffer(float buffer[], int length, float new_item){
    for(int i = 1; i < length; i++){
        buffer[i-1] = buffer[i];
    }
    buffer[length - 1] = new_item;
}

float avg(float buffer[], int length){
    float sum = 0;
    for(int i = 0; i < length; i++){
        sum += buffer[i];
    }
    return sum / length;
}

int main(int argc, char *argv[]) {

    if(argc < 2){
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if(!file){
        printf("Error opening file\n");
        return 1;
    }

    float angle[MAX_POINTS];
    float distance[MAX_POINTS];
    float cleaned[MAX_POINTS];
    int size = 0;

    // Read file
    char line[256];

    // Skip header
    fgets(line, sizeof(line), file);

    // Read data
    while(fgets(line, sizeof(line), file)){
        if(sscanf(line, "%f %f", &angle[size], &distance[size]) == 2){
            size++;
        }
    }

    fclose(file);

    // Moving average buffer
    float buffer[WINDOW_SIZE];

    // Initialize buffer
    for(int i = 0; i < WINDOW_SIZE; i++){
        buffer[i] = distance[0];
    }

    printf("Angle\tRaw\tCleaned\n");

    // Apply moving average and store results
    for(int i = 0; i < size; i++){

        updatebuffer(buffer, WINDOW_SIZE, distance[i]);
        cleaned[i] = avg(buffer, WINDOW_SIZE);

        printf("%.2f\t%.2f\t%.2f\n", angle[i], distance[i], cleaned[i]);
    }

    // ---------------- OBJECT DETECTION ----------------
    printf("\n--- OBJECT SUMMARY ---\n");

    int i = 0;
    int object_num = 0;

    while(i < size){

        if(cleaned[i] < OBJECT_THRESHOLD){

            object_num++;
            int start = i;

            // Move through the object
            while(i < size && cleaned[i] < OBJECT_THRESHOLD){
                i++;
            }

            int end = i - 1;
            int mid = (start + end) / 2;

            float obj_angle = angle[mid];
            float obj_dist  = cleaned[mid];
            float width     = angle[end] - angle[start];

            printf("Object %d:\n", object_num);
            printf("  Angle: %.2f\n", obj_angle);
            printf("  Distance: %.2f\n", obj_dist);
            printf("  Width: %.2f degrees\n\n", width);
        }
        else{
            i++;
        }
    }

    if(object_num == 0){
        printf("No objects detected.\n");
    }

    return 0;
}