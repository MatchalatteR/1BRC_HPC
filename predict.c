#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define STATION_NUM 10000

typedef struct
{
    char name[6];
    double min_temp;
    double max_temp;
    double sum_temp;
    int count;
} Station;

int compare(const void *a, const void *b)
{
    Station *stationA = (Station *)a;
    Station *stationB = (Station *)b;
    return strcmp(stationA->name, stationB->name);
}

void load_data(const char *filename, Station stations[], int *num_stations)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    char line[256];
    while (fgets(line, sizeof(line), file))
    {
        char station_name[6];
        double temperature;
        sscanf(line, "%5s,%lf", station_name, &temperature);

        int found = 0;
        for (int i = 0; i < *num_stations; i++)
        {
            if (strcmp(stations[i].name, station_name) == 0)
            {
                if (temperature < stations[i].min_temp)
                    stations[i].min_temp = temperature;
                if (temperature > stations[i].max_temp)
                    stations[i].max_temp = temperature;
                stations[i].sum_temp += temperature;
                stations[i].count++;
                found = 1;
                break;
            }
        }

        if (!found)
        {
            strcpy(stations[*num_stations].name, station_name);
            stations[*num_stations].min_temp = temperature;
            stations[*num_stations].max_temp = temperature;
            stations[*num_stations].sum_temp = temperature;
            stations[*num_stations].count = 1;
            (*num_stations)++;
        }
    }

    fclose(file);
}

void print_statistics(const Station stations[], int num_stations, FILE *out)
{
    for (int i = 0; i < num_stations; i++)
    {
        double mean = stations[i].sum_temp / stations[i].count;
        fprintf(out, "%s<%.1f/%.1f/%.1f>\n", stations[i].name, stations[i].min_temp, mean, stations[i].max_temp);
    }
}

int main()
{
    Station stations[STATION_NUM];
    int num_stations = 0;

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    load_data("weather_data.csv", stations, &num_stations);

    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1E9;

    FILE *output = fopen("result.txt", "w");
    if (output == NULL)
    {
        perror("Failed to open output file");
        return EXIT_FAILURE;
    }

    fprintf(output, "Data loading took %.3f seconds\n", elapsed_time);

    qsort(stations, num_stations, sizeof(Station), compare);
    print_statistics(stations, num_stations, output);

    fclose(output);

    return EXIT_SUCCESS;
}
