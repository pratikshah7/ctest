#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int OptimizeSensorData();
int RetriveSensorData();

int main()
{

    // It is assumed that Sensor data is in the SensorData.txt file
    // Data is stored in Optimized.txt after optimization [reduced bytes]
    OptimizeSensorData();

    RetriveSensorData();

    return 0;
}

int OptimizeSensorData()
{

    FILE *fp_source;
    FILE *fp_destination;
    unsigned char buf[7]; //Storing the hex value for writing into the destination file. 
    unsigned int current_data;
    unsigned int last_data;
    unsigned int count = 0; //counts the (number of times data is repeated - 1)
    fp_source = fopen("SensorData.txt", "r");
    fp_destination = fopen("OptimizedData.txt", "w"); 

    if (fp_source == NULL)
    {
        printf("Error while opening the file.\n");
        //No action is taken in the main function
        return 0;
    }
    int i = 0;
    while (fscanf(fp_source, "%x,", &current_data) == 1)
    {
        if (i == 0)
        {
            //Storing the first sensor data in the destination file. 
            last_data = current_data;
            printf("%d ", current_data);
            printf("%d \n", last_data);
            sprintf(buf, "0x%x", current_data); 
            fputs(buf, fp_destination);
        }
        else
        {
            //if the data repeats then increment the count else store the count (if > 0) and new data 
            if (current_data == last_data)
            {                
                count++;
            }
            else
            {
                if (count > 0)
                {
                    //Setting MSB to 1 to indicate that the value is not data but the count. This will be used to retrieve the data pattern.
                    count |= 0x80;
                    sprintf(buf, ", 0x%0.2x", count); 
                    fputs(buf, fp_destination);
                    count = 0;
                }
                sprintf(buf, ", 0x%x", current_data); 
                fputs(buf, fp_destination);
                last_data = current_data;
            }
        }

        i++;
    }

    if (count > 0)
    {
        //if last data was repeated then include the counts
        count |= 0x80;
        sprintf(buf, ", 0x%0.2x", count); //uncomment to test string to file
        fputs(buf, fp_destination);
        count = 0;
    }

    fclose(fp_source);
    fclose(fp_destination);
    return 1;
}


int RetriveSensorData()
{

    printf("Retriving Data \n");

    FILE *fp_source;
    FILE *fp_destination;
    unsigned char buf[10];
    unsigned int current_data;
    unsigned int previous_data;
    unsigned int count = 0; //stores (number of times data is repeated - 1) [information is in the data sequence]
    fp_source = fopen("OptimizedData.txt", "r");
    fp_destination = fopen("retrivedData.txt", "w"); 

    if (fp_source == NULL)
    {
        printf("Error while opening the file.\n");
        //No action is taken in the main file. 
        return 0;
    }
    int i = 0;
    int j = 0;
    while (fscanf(fp_source, "%x,", &current_data) == 1)
    {
        printf("%d ", current_data);
        if (i == 0)
        {
            //First value is a Sensor Data
            previous_data = current_data;
            sprintf(buf, "0x%x", current_data);
            fputs(buf, fp_destination);
        }
        else
        {
            if ((current_data & 0x80) == 0x80)
            {
                //If MSB is 1, the retrived value indicates that we need to duplicate the previous data by the value of count as calculated below 
                count = current_data & 0x7F; //Retrieving the number of times the previous data  needs to be repeated 
                for (j = 0; j < count; j++)
                {
                    //Repeating the previous data "count" times 
                    sprintf(buf, ", 0x%x", previous_data);
                    fputs(buf, fp_destination);                    
                }                
            }
            else
            { 
                //if MSB is not 1, the retrieved values is Sensor Data
                sprintf(buf, ", 0x%x", current_data); //uncomment to test string to file
                fputs(buf, fp_destination);
                previous_data = current_data;
            }
        }
        i++;
    }
    fclose(fp_source);
    fclose(fp_destination);
    return 1;
}