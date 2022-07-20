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
vector<vector<vector<double>>> best;
vector<vector<double>> best_rute;
vector<vector<vector<vector<double>>>> poblacion; //guardaremos la poblacion inicial de clientes


float drone_speed;
float truck_speed;
int p = 20;
int maxIter = 5;
int n_drones = 2; //cantidad de drones en el sistema
float maxWeight = 3; // peso maximo de un dron
unsigned t0, t1;
double best_t;

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
        else if ( c >= 8 && helper){ //lineas para el cliente
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

double suma(vector<double> v){
    double suma = 0;
    for( int i = 0; i < v.size(); i++ ) {
        suma += v[i];
    }

    return suma;
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

void print_vv(vector<vector<double>> v){ //printear array de arrays
    for (int i = 0; i < v.size(); i++)
    {
        //std::cout << "xdlol" << '\n';
        for (int j = 0; j < v[i].size(); j++)
        {
            printf("%.17g \n", v[i][j]);
        }
    }
}

void print(std::vector<double> const &v){
    for (double i: v) {
        std::cout << i << ' ';
    }
}

void shuffle(vector<vector<double>> v){
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();//generar seed para que sea totalmente random
    std::default_random_engine e(seed);
    std::shuffle(v.begin(), v.end(), e);
    //print_vv(v, "copy");
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
        float capacidad_max = maxWeight * n_drones;
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
// vector<vector<vector<vector<double>>>> poblacion_inicial_c;
auto f_evaluacion(vector<vector<vector<vector<double>>>> poblacion_inicial){
    vector<vector<double>> tt_times;
    int cont = 0;
    for (int i=0; i<poblacion_inicial.size(); i++){
        //std::cout << "op " << "i\n";
        vector<vector<double>> rute = poblacion_inicial_t[i];
        vector<vector<double>> op;
        vector<double> times;
        //print_vv(poblacion_inicial_c[0][0],"ad");
        for (int j=0; j<poblacion_inicial[i].size(); j++){
            double total_time = 0;
            double temp_distancia = 0;
            vector<double> l_point = rute[j]; // launch point
            vector<double> r_point;
            vector<double> last = rute[rute.size()-1]; //last l/r point
            //std::cout << poblacion_inicial_c[i].size()<< "a\n";
            // for (int k=0; k<l_point.size(); k++){
            //     printf("%.17g \n", l_point[k]);
            // }
            if(l_point == last){
                r_point = rute[0]; //retrival point si es la ultima operacion
                // std::cout << "punto+1" << "\n";
                // for (int k=0; k<r_point.size(); k++){
                //     printf("%.17g \n", r_point[k]);
                // }
            }else{
                r_point = rute[j+1]; //retrival point para el resto de casos
                // std::cout << "punto+1" << "\n";
                // for (int k=0; k<r_point.size(); k++){
                //     printf("%.17g \n", r_point[k]);
                // }
            }
            double distancia_truck = euclidian(l_point[0],l_point[1],r_point[0],r_point[1]);
            double truck_time = (distancia_truck*1000)/truck_speed;
            total_time = total_time + truck_time;
            op = poblacion_inicial[i][j];
            if(op.size()==1){
                double d_ida = euclidian(l_point[0],l_point[1], op[0][0],op[0][1]);
                double d_vuelta = euclidian(op[0][0],op[0][1],r_point[0],r_point[1]);
                temp_distancia = temp_distancia + d_ida + d_vuelta;
            }else if (op.size() > 1){
                for (int l = 0; l<op.size(); l++){
                    vector<double>operation = op[l];
                    // std::cout << operation[0] << "\n";
                    // std::cout << operation[1] << "\n";
                    if(l == 0){
                        double dist = euclidian(operation[0],operation[1],l_point[0],l_point[1]);
                        temp_distancia = temp_distancia + dist;
                    }else if( l == op.size()-1){
                        double dist_1 = euclidian(operation[0],operation[1],r_point[0],r_point[1]);
                        double dist_2 = euclidian(op[l-1][0],op[l-1][1],operation[0],operation[1]);
                        temp_distancia = temp_distancia + dist_1 + dist_2;
                    }else {
                        double dist = euclidian(op[l-1][0],op[l-1][1],operation[0],operation[1]);
                        temp_distancia = temp_distancia + dist;
                    }
                }
            }
            double op_time = (temp_distancia*1000)/drone_speed;
            total_time = total_time + op_time;
            times.push_back(total_time);
            //std::cout << total_time << "\n";
            //print(times);
        }
        tt_times.push_back(times);
    }
    return tt_times;
}

template <typename T>
void swap(T &x, T &y)
{
    T temp = std::move(x);
    x = std::move(y);
    y = std::move(temp);
}

void evolutivo(int iter ){
    auto eval = f_evaluacion(poblacion_inicial_c);
    for (int v=0; v<poblacion_inicial_c.size(); v++){
        poblacion.push_back(poblacion_inicial_c[v]);}
    double min = 999999;
    for(int i = 0; i < iter; i++){
        vector<vector<vector<vector<double>>>> mutados;
        //print_vv(eval);
        vector<double> tiempos;
        for(int time = 0; time < eval.size(); time++){
            tiempos.push_back(suma(eval[time]));
        }
        int min_index = -1;
        for (int n = 0; n < tiempos.size(); n++)
        {
            if (tiempos[n] < min)
            {
                min = tiempos[n];
                min_index = n;
                best = poblacion[n];
                best_rute = poblacion_inicial_t[n];
            }
        }
        //mutados.push_back(poblacion_inicial_c[max_index]);
        //mutados.push_back(poblacion_inicial_c[min_index]);
        //std::cout << min << "\n";
        // srand(time(NULL));
        // int r_index_1;
        // int r_index_2;
        // srand (time(NULL));
        // r_index_1 = 1;
        // r_index_2 = 2;
        // if(r_index_2 == r_index_1){
        //     r_index_2 = 2;
        // }
        // for(int j = 0; j < poblacion_inicial_c.size(); j++){
        //     if(poblacion_inicial_c[j] == poblacion_inicial_c[max_index] || poblacion_inicial_c[j] == poblacion_inicial_c[min_index]){
        //         continue;
        //     }else{
        //         for(int k = 0; k < poblacion_inicial_c[j].size(); k++){
        //             std::swap(poblacion_inicial_c[j][0], poblacion_inicial_c[j][2]);
        //             auto itr_i = std::next(poblacion_inicial_c[j].begin(), 0);
        //             auto itr_j = std::next(poblacion_inicial_c[j].begin(), 1);
        //             std::iter_swap(itr_i, itr_j);
        //         }
        //         mutados.push_back(poblacion_inicial_c[j]);
        //     }
        // }

        //std::cout << "asd" << "\n";
        for(int j = 0; j < poblacion_inicial_c.size(); j++){
        auto copy = poblacion[j][0];
        poblacion[j].erase(poblacion[j].begin());
        //print_vv(best[0]);
        poblacion[j].push_back(copy);}
        auto new_eval = f_evaluacion(poblacion);
        //print_vv(new_eval);
        eval.clear();
        eval = new_eval;
    }
    best_t=min;
}


void removeLastN(std::string &str, int n) {            // no-const
    if (str.length() < n) {
        return ;
    }
 
    str.erase(str.length() - n);
}

void out(string passedValue, double time){
    string nombreArchivo = "ouput.txt";
    ofstream archivo;
    // Abrimos el archivo
    archivo.open(nombreArchivo.c_str(), fstream::out);
    // instacia
    removeLastN(passedValue, 4);
    archivo <<passedValue;
    archivo << " k="<<n_drones;
    archivo << " capacidad dron="<<maxWeight<<"kg";
    archivo << " tiempo de ejecución="<<time<<"s";
    archivo << " valor objetivo="<<best_t<<"s"<< endl;
    //recuperamos la ruta del camion
    vector<vector<double>> rute = best_rute;
    for(int i=0; i<rute.size()-1;i++){
        archivo << "camión viajó desde ("<<rute[i][0]<<","<<rute[i][1]<<")";
        archivo << " a ("<<rute[i+1][0]<<","<<rute[i+1][1]<<")";
        archivo << endl;
        auto operation = best[i];
        float div = (float)n_drones;
        int n_op = operation.size()/div;
        int count = 0;
        //std::cout<<operation.size() << " size"<<"\n";
        //std::cout<<lround(n_op) << " round(n_op)"<<"\n";
        for(int drones=0;drones<n_drones;drones++){
            archivo << "drone "<< drones+1;
            archivo << " atendió a cliente: ";
            for(int j=0; j<lround(n_op) && count<operation.size()-1; j++){
                count = count + 1;
                archivo << " ("<<operation[count][0]<<","<<operation[count][1]<<")";
            }
            archivo << endl;
        }
    }
    // Finalmente lo cerramos
    archivo.close();
    cout << "Escrito correctamente\n";
}
int main(int argc, char *argv[]){
    string passedValue;
    for(int i = 1; i < argc; i++){
        passedValue += argv[i];
    }
    //Primero leemos el archivo y generamos las variables
    leer_archivo(passedValue);
    generar_poblacion(p);
    //auto eval = f_evaluacion();
    t0=clock();
    evolutivo(maxIter);
    t1 = clock();
    double time = (double(t1-t0)/CLOCKS_PER_SEC);
    out(passedValue,time);
    //print_vv(eval);
    //print_vv(c_locations, "c");
    // printf("%.17g \n", lr_locations[0][1]);
    // std::cout << passedValue << "\n";
    return 0;
}