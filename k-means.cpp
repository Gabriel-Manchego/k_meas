#include <iostream>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <fstream>
#include <sstream>
#define nombre_archivo "10milcoordenadas.txt"
using namespace std;

class Punto{
private:
	int nom_pun, nom_cluster;
	vector<double> coords;
public:
	Punto(int nom_pun, vector<double>& coords){
		this->nom_pun = nom_pun;
		for(int i = 0; i < 2; i++)
			this->coords.push_back(coords[i]);
		nom_cluster = -1;
	}
	int get_nomcluster(){
		return nom_pun;
	}
	void setCluster(int nom_cluster){
		this->nom_cluster = nom_cluster;
	}
	int getnom_cluster(){
		return nom_cluster;
	}
	double getcoords(int index){
		return coords[index];
	}	
};

class Cluster{
private:
	int nom_cluster;
	vector<double> coords_centrales;
	vector<Punto> puntos;
	
public:
	Cluster(int nom_cluster, Punto pun){
		this->nom_cluster = nom_cluster;
		for(int i = 0; i < 2; i++)
			coords_centrales.push_back(pun.getcoords(i));
		puntos.push_back(pun);
	}
	void addpunto(Punto pun){
		puntos.push_back(pun);
	}
	
	bool removepunto(int nom_pun){
		int tot_pun = puntos.size();
		
		for(int i = 0; i < tot_pun; i++){
			if(puntos[i].get_nomcluster() == nom_pun){
				puntos.erase(puntos.begin() + i);
				return true;
			}
		}
		return false;
	}
	double getcoords_centrales(int index){
		return coords_centrales[index];
	}
	void setcoords_centrales(int index, double value){
		coords_centrales[index] = value;
	}
	Punto getpunt(int index){
		return puntos[index];
	}
	int getTotalPoints(){
		return puntos.size();
	}
	int get_nomcluster(){
		return nom_cluster;
	}
};

class KMeans
{
private:
	int K; // es k duh
	int tot_pun, max_iter;
	vector<Cluster> clusters;
	
	
	int getnom_punNearestCenter(Punto pun){
		double sum = 0.0, min_dist;
		int nom_cluster_central = 0;
		
		for(int i = 0; i < 2; i++){
			sum += pow(clusters[0].getcoords_centrales(i) -
					   pun.getcoords(i), 2.0);
		}
		min_dist = sqrt(sum);
		for(int i = 1; i < K; i++){
			double dist;
			sum = 0.0;
			
			for(int j = 0; j < 2; j++){
				sum += pow(clusters[i].getcoords_centrales(j) -
						   pun.getcoords(j), 2.0);
			}
			dist = sqrt(sum);
			if(dist < min_dist){
				min_dist = dist;
				nom_cluster_central = i;
			}
		}
		return nom_cluster_central;
	}
	
public:
	KMeans(int K, int tot_pun, int max_iter){
		this->K = K;
		this->tot_pun = tot_pun;
		this->max_iter = max_iter;
	}
	
	void run(vector<Punto> & puntos){
		if(K > tot_pun)
			return;
		vector<int> descartados;
		for(int i = 0; i < K; i++){
			while(true){
				int index_punto = rand() % tot_pun;
				
				if(find(descartados.begin(), descartados.end(),index_punto) == descartados.end()){
					descartados.push_back(index_punto);
					puntos[index_punto].setCluster(i);
					Cluster cluster(i, puntos[index_punto]);
					clusters.push_back(cluster);
					break;
				}
			}
		}
		int iter = 1;
		while(true){
			bool flag = true;
			for(int i = 0; i < tot_pun; i++){
				int nom_centro_v = puntos[i].getnom_cluster();
				int nom_centro = getnom_punNearestCenter(puntos[i]);
				
				if(nom_centro_v != nom_centro){
					if(nom_centro_v != -1)
						clusters[nom_centro_v].removepunto(puntos[i].get_nomcluster());
					
					puntos[i].setCluster(nom_centro);
					clusters[nom_centro].addpunto(puntos[i]);
					flag = false;
				}
			}
			for(int i = 0; i < K; i++){
				for(int j = 0; j < 2; j++){
					int tot_puntos_c = clusters[i].getTotalPoints();
					double sum = 0.0;
					
					if(tot_puntos_c > 0){
						for(int p = 0; p < tot_puntos_c; p++)
							sum += clusters[i].getpunt(p).getcoords(j);
						clusters[i].setcoords_centrales(j, sum / tot_puntos_c);
					}
				}
			}
			if(flag == true || iter >= max_iter){
				cout << "paro en " << iter << "\n\n";
				break;
			}
			iter++;
		}
		for(int i = 0; i < K; i++){
			int tot_puntos_c =  clusters[i].getTotalPoints();
			cout << "Cluster " << clusters[i].get_nomcluster() + 1 << endl;
			for(int j = 0; j < tot_puntos_c; j++){
				cout << "Punto " << clusters[i].getpunt(j).get_nomcluster() + 1 << ": ";
				for(int p = 0; p < 2; p++)
					cout << clusters[i].getpunt(j).getcoords(p) << " ";
				
				cout << endl;
			}
			cout << "Punto central : ";
			for(int j = 0; j < 2; j++)
				cout << clusters[i].getcoords_centrales(j) << " ";
			cout << endl << endl;
		}
	}
};

int main(int argc, char *argv[]){
	srand (time(NULL));
	
	vector<Punto> puntos;//vector de donde almacenare todos los puntos
	
	ifstream archivo(nombre_archivo);
	char delimitador='$';
	string linea, strx, stry;
	double x, y;
	int i=0;
	while(getline(archivo,linea)){
		vector<double> coords;
		stringstream stream(linea);
		getline(stream,strx,delimitador);
		x = stod(strx);
		coords.push_back(x);
		getline(stream,stry,delimitador);
		y = stod(stry);
		coords.push_back(y);
		
		Punto p(i, coords);
		puntos.push_back(p);
		i++;
	}
	
	KMeans kmeans(5, 10000, 100);//(numero de clusters, cantidad total de puntos, maximo numero de iteraciones)
	kmeans.run(puntos);
	cout<<"Estoy usando 10 mil puntos de cvs entregado, con un K=5";
	return 0;
}
	
