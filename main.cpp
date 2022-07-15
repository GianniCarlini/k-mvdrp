#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <ctime>
#include <cstring>
#include <typeinfo>
using namespace std;


vector<vector<double>> c_locations; //guardaremos la ubicacion de los clientes in format <(x,y,mass)>
vector<vector<double>> lr_locations; //guardaremos la los puntos de lanzamiento/rescate in format <(x,y)>

float drone_speed;
float truck_speed;
int poblacion;
int maxIter;
int k;
int maxWeight;

void leer_archivo(string arch){
    string nombreArchivo = arch;
    ifstream archivo(nombreArchivo.c_str());

    string linea;
    string lectura;
    bool helper = true;

    int c = 0;
    // Obtener línea de archivo, y almacenar contenido en "linea"
    while (getline(archivo, linea)) {
        if ( c == 2){ //linea velocidad del camion
            stringstream input_stringstream(linea);
            string truck;
            getline(input_stringstream, truck);
            float f_1 = stod(truck);
            truck_speed = f_1;
            c++;
        }
        if ( c >= 9 && helper){ //lineas para el cliente
            stringstream input_stringstream(linea);
            string locations;
            getline(input_stringstream, locations);
            string flag = "customer locations (C) and packages mass, in format (x,y,mass) ";
            if (locations != flag){
                string arr[2];
                int i = 0;
                stringstream ssin(locations);
                vector<double> c_loc;
                while (ssin.good() && i < 2){
                    ssin >> arr[i];
                    c_loc.push_back(stod(arr[i]));
                    ++i;
                }
                lr_locations.push_back(c_loc);
            }else{
                helper = false;
            }
        }
        else if (!helper){
            stringstream input_stringstream(linea);
            string locations;
            getline(input_stringstream, locations);

            string arr[3];
            int i = 0;
            stringstream ssin(locations);
            vector<double> c_loc;
            while (ssin.good() && i < 3){
                ssin >> arr[i];
                c_loc.push_back(stod(arr[i]));
                ++i;
            }
            c_locations.push_back(c_loc);
        }
        c++;
    }
}

void print_vv(vector<vector<double>> v, string name){ //printear array de arrays
    for (int i = 0; i < v.size(); i++)
    {
        std::cout << name << "\n";
        for (int j = 0; j < v[i].size(); j++)
        {
            printf("%.17g \n", v[i][j]);
        }
    }
}

int main(int argc, char *argv[]){
    string passedValue;
    for(int i = 1; i < argc; i++){
        passedValue += argv[i];
    }
    //Primero leemos el archivo y generamos las variables
    leer_archivo(passedValue);
    //print_vv(lr_locations, "lr");
    //print_vv(c_locations, "c");
    // printf("%.17g \n", lr_locations[0][1]);
    // std::cout << passedValue << "\n";
    return 0;
}