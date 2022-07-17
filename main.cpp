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
#include<math.h>
#include <random>
#include <chrono>
using namespace std;


vector<vector<double>> c_locations; //guardaremos la ubicacion de los clientes in format <(x,y,mass)>
vector<vector<double>> lr_locations; //guardaremos la los puntos de lanzamiento/rescate in format <(x,y)>
vector<vector<vector<vector<double>>>> poblacion_inicial_c; //guardaremos la poblacion inicial de clientes
vector<vector<vector<double>>> poblacion_inicial_t; //guardaremos la poblacion inicial de camion

float drone_speed;
float truck_speed;
int poblacion;
int maxIter;
int n_drones = 1; //cantidad de drones en el sistema
float peso_drone = 3; // peso maximo de un dron
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
            float f_1 = stof(truck);
            truck_speed = f_1;
            c++;
        }
        else if ( c == 4){ //linea velocidad del camion
            stringstream input_stringstream(linea);
            string drone;
            getline(input_stringstream, drone);
            float f_1 = stof(drone);
            drone_speed = f_1;
            c++;
        }
        else if ( c >= 9 && helper){ //lineas para el cliente
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
        else {
            c++;
        }
    }
}

double euclidian(double x1, double y1, double x2, double y2)
{
    //calculo de distancia euclidiana
	double x = x1 - x2;
	double y = y1 - y2;
	double dist;

	dist = pow(x, 2) + pow(y, 2);
	dist = sqrt(dist);

	return dist;
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

void print(std::vector<int> const &v){
    for (int i: v) {
        std::cout << i << ' ';
    }
}

void shuffle(vector<vector<double>> v){
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();//generar seed para que sea totalmente random
    std::default_random_engine e(seed);
    std::shuffle(v.begin(), v.end(), e);
    print_vv(v, "copy");
}

void generar_poblacion(int n)
{
    vector<vector<double>> vect2;
    for (int l=0; l<c_locations.size(); l++){
        vect2.push_back(c_locations[l]);}
    for (int k = 0; k < n; k++){
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();//generar seed para que sea totalmente random
        std::default_random_engine e(seed);
        std::shuffle(vect2.begin(), vect2.end(), e);
        vector<vector<vector<double>>> sol_op;
        float capacidad_max = peso_drone * n_drones;
        int x;
        float peso_operacion = 0;
        vector<vector<double>> temp_op;
        for (int i = 0;  i < vect2.size(); i++){
            peso_operacion = peso_operacion + vect2[i][2];
            if (peso_operacion <= capacidad_max){
                temp_op.push_back(vect2[i]);
            }
            else if (peso_operacion > capacidad_max){
                sol_op.push_back(temp_op);
                peso_operacion = 0;
                temp_op.clear();
                i--;
            }
        }
        poblacion_inicial_c.push_back(sol_op);

        vector<vector<double>> vect3;
        for (int t=0; t<sol_op.size(); t++){
            vect3.push_back(lr_locations[t]);}
        poblacion_inicial_t.push_back(vect3);
    }
}

double f_evaluacion(){
    double total_time=0;
    int cont = 0;

    for (int i=0; i<poblacion_inicial_c.size(); i++){
        vector<vector<double>> rute = poblacion_inicial_t[i];
        for (int j=0; j<poblacion_inicial_c[i].size(); j++){
            vector<double> l_point = rute[j]; // launch point
            vector<double> last = rute[rute.size()-1]; //last l/r point 
            //std::cout << "punto" << "\n";
            for (int k=0; k<l_point.size(); k++){
                printf("%.17g \n", l_point[k]);
            }
            if(l_point == last){
                vector<double> r_point = rute[0]; //retrival point si es la ultima operacion
                // std::cout << "punto+1" << "\n";
                // for (int k=0; k<r_point.size(); k++){
                //     printf("%.17g \n", r_point[k]);
                // }
            }else{
                vector<double> r_point = rute[j+1]; //retrival point para el resto de casos
                // std::cout << "punto+1" << "\n";
                // for (int k=0; k<r_point.size(); k++){
                //     printf("%.17g \n", r_point[k]);
                // }
            }
            vector<vector<double>> op = poblacion_inicial_c[i][j];
        }

    }
    return 0;
}

int main(int argc, char *argv[]){
    string passedValue;
    for(int i = 1; i < argc; i++){
        passedValue += argv[i];
    }
    //Primero leemos el archivo y generamos las variables
    leer_archivo(passedValue);
    generar_poblacion(2);
    f_evaluacion();
    //print_vv(lr_locations, "lr");
    //print_vv(c_locations, "c");
    // printf("%.17g \n", lr_locations[0][1]);
    // std::cout << passedValue << "\n";
    return 0;
}