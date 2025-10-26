#include <iostream>
#include <fstream> // Para el manejo de archivos
#include <string> // Para el manejo de strings
#include <vector> // Para elmanejo de listas
#include <random> // Para la selección aleatoria de anuncios
#include <ctime> // Para medir el tiempo de reproducción
#include <windows.h> // Para la función Sleep
#include <unordered_map> // Para el mapa de favoritos
#include <unordered_set> // Para el conjunto de canciones favoritas
#include <iomanip> // Para la precisión en la salida de tiempo
#include <dirent.h>


using namespace std;

// --- Estructura para anuncios ---
struct Anuncio {
    string prioridad;
    string texto;
    int id;
};

// --- Variables globales que gestionan gestion anuncios/canciones ---
vector<Anuncio> anuncios_disponibles;
unordered_set<int> anuncios_mostrados;
bool anuncios_cargados = false;
int contador_canciones = 0;

// --- Mapa de favoritos por usuario premium ---
unordered_map<string, unordered_set<string>> favoritos_premium;

// --- Clase usuario ---
class usuario {
private:
    string nickname;
    string contrasena;
    string fecha;
    string pais;
    bool membresia;
    bool cambiar_membresia;
public:
    usuario(string nick, string pass, string date, string country, bool member, bool C_member)
        : nickname(nick), contrasena(pass), fecha(date), pais(country), membresia(member), cambiar_membresia(C_member) {}
    void guardar_usuario() {
        ofstream archivo("usuarios.txt", ios::app);
        if (archivo.is_open()) {
            archivo << "------------------nuevo usuario----------------------" << endl;
            archivo << nickname << "," << contrasena << "," << fecha << "," << pais << "," << (membresia ? "1" : "0") << "," << (cambiar_membresia ? "1" : "0") << endl;
            archivo << "------------------siguiente usuario------------------" << endl;
            archivo.close();
        } else {
            cerr << "No se pudo abrir el archivo para guardar el usuario." << endl;
        }
    }

    void setNickname(const string& nick) { nickname = nick; }
    string getNickname() const { return nickname; }
    void setContrasena(const string& pass) { contrasena = pass; }
    string getContrasena() const { return contrasena; }
    void setFecha(const string& date) { fecha = date; }
    string getFecha() const { return fecha; }
    void setPais(const string& country) { pais = country; }
    string getPais() const { return pais; }
    void setMembresia(bool member) { membresia = member; }
    bool getMembresia() const { return membresia; }
    void setCambiarMembresia(bool cmem) { cambiar_membresia = cmem; }
    bool getCambiarMembresia() const { return cambiar_membresia; }
};

// --- Determinar tipo membresía de usuario a partir de la línea leída del archivo ---
string obtenerTipoMembresia(const string& linea) {
    size_t pos = 0, last = 0;
    int campo = 0;
    string memb = "regular";
    while ((pos = linea.find(',', last)) != string::npos) {
        if (campo == 4) {
            if (linea.substr(last, pos-last) == "1")
                memb = "premium";
            break;
        }
        campo++;
        last = pos+1;
    }
    return memb;
}

// --- Cargar los anuncios desde el archivo ---
void cargar_anuncios() {
    if (anuncios_cargados) return;
    ifstream archivo("anuncios.txt");
    string linea;
    int contador_id = 0;
    anuncios_disponibles.clear();
    while (getline(archivo, linea)) {
        if (linea.empty()) continue;
        size_t coma = linea.find(',');
        if (coma == string::npos) continue;
        string prioridad = linea.substr(0, coma);
        string texto = linea.substr(coma + 1);
        if (texto.size() > 500) texto = texto.substr(0, 500);
        int repeticiones = 1;
        if (prioridad == "AAA") repeticiones = 3;
        else if (prioridad == "B") repeticiones = 2;
        for (int i = 0; i < repeticiones; ++i) {
            anuncios_disponibles.push_back({prioridad, texto, contador_id++});
        }
    }
    anuncios_cargados = true;
}

// Mostrar un anuncio aleatorio, evitando repeticiones
void anuncios() {
    cargar_anuncios();
    vector<Anuncio> restantes;
    for (const auto& anuncio : anuncios_disponibles)
        if (anuncios_mostrados.count(anuncio.id) == 0)
            restantes.push_back(anuncio);
    if (restantes.empty()) {
        anuncios_mostrados.clear();
        restantes = anuncios_disponibles;
    }
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, restantes.size() - 1);
    int idx = dis(gen);
    const Anuncio& anuncio_a_mostrar = restantes[idx];
    anuncios_mostrados.insert(anuncio_a_mostrar.id);
    cout << "\n--- ANUNCIO (" << anuncio_a_mostrar.prioridad << ") ---\n";
    cout << anuncio_a_mostrar.texto << "\n";
    cout << "----------------\n";
}

// Simular reproducción de canción
void reproducir_cancion(const string& nombre_cancion, bool premium = false) {
    cout << "La cancion: " << nombre_cancion << endl;
    cout << "Reproduciendo la cancion: " << nombre_cancion << endl;
    time_t start = time(NULL);
    Sleep(3000);
    time_t end = time(NULL);
    double duration = difftime(end, start);
    cout << fixed << setprecision(2) << duration << " segundos de reproduccion." << endl;
    contador_canciones++;
    if (!premium && contador_canciones % 2 == 0) {
        anuncios();
    }
}

void guardar_favoritos_premium() {
    ofstream archivo("favoritos.txt");
    if (archivo.is_open()) {
        for (const auto& par : favoritos_premium) {
            archivo << par.first << ",";
            bool primero = true;
            for (const auto& cancion : par.second) {
                if (!primero) archivo << "|";
                archivo << cancion;
                primero = false;
            }
            archivo << endl;
        }
        archivo.close();
    } else {
        cerr << "No se pudo guardar favoritos premium." << endl;
    }
}

// Cargar favoritos premium al inicio
void cargar_favoritos_premium() {
    ifstream archivo("favoritos.txt");
    if (!archivo.is_open()) return;
    favoritos_premium.clear();
    string linea;
    while (getline(archivo, linea)) {
        if (linea.empty()) continue;
        size_t coma = linea.find(',');
        if (coma == string::npos) continue;
        string nick = linea.substr(0, coma);
        string lista = linea.substr(coma+1);
        unordered_set<string> canciones;
        size_t pos = 0, last = 0;
        while ((pos = lista.find('|', last)) != string::npos) {
            canciones.insert(lista.substr(last, pos - last));
            last = pos + 1;
        }
        if (last < lista.size()) canciones.insert(lista.substr(last));
        favoritos_premium[nick] = canciones;
    }
    archivo.close();
}

// Menú premium para usuarios premium
void mostrar_menu_premium() {
    cout << "\n=== MENU PREMIUM ===\n";
    cout << "[1] Reproducir una cancion\n";
    cout << "[2] Agregar una cancion a lista de favoritos\n";
    cout << "[3] Ver su lista de favoritos\n";
    cout << "[4] Ver la lista de favoritos de alguien más\n";
    cout << "[5] Volver al menu principal\n";
    cout << "Ingrese una opcion: ";
}

// Menú estándar para los usuarios regulares
void mostrar_menu_estandar() {
    cout << "\n=== MENU ESTANDAR (REGULAR) ===\n";
    cout << "[1] Reproducir una cancion\n";
    cout << "[2] Volver al menu principal\n";
    cout << "Ingrese una opcion: ";
}

void buscar_canciones_en_directorio(const string& nombre_cancion, bool premium = false){
    string dir;
    DIR *direccion;
    struct dirent *elemento;
    
    // Asignar la ruta según el tipo de usuario
    if (premium) {
        dir = "C:\\Users\\dav98\\OneDrive\\Documentos\\Desafio_2\\Desafio_II\\Codigo\\build\\Desktop_Qt_6_10_0_MinGW_64_bit-Debug\\Canciones_premium";
    } else {
        dir = "C:\\Users\\dav98\\OneDrive\\Documentos\\Desafio_2\\Desafio_II\\Codigo\\build\\Desktop_Qt_6_10_0_MinGW_64_bit-Debug\\Canciones_regulares";
    }
    
    cout << "Buscando en: " << dir << "\n";
    
    if (direccion = opendir(dir.c_str())){
        cout << "\n=== Resultados de búsqueda para: " << nombre_cancion << " ===\n\n";
        bool encontrada = false;
        
        while (elemento = readdir(direccion)){
            string nombre_archivo = elemento->d_name;
            
            // Saltar directorios especiales
            if (nombre_archivo == "." || nombre_archivo == "..") continue;
            
            // Buscar si el nombre del archivo contiene la canción buscada
            if (nombre_archivo.find(nombre_cancion) != string::npos) {
                encontrada = true;
                string ruta_completa = dir + "\\" + nombre_archivo;
                
                cout << "Archivo encontrado: " << ruta_completa << "\n";
                cout << "--------------------------------------------------\n";
                
                // Si es un archivo de texto, mostrar su contenido
                if (nombre_archivo.length() >= 4 && nombre_archivo.substr(nombre_archivo.length() - 4) == ".txt") {
                    ifstream archivo_txt(ruta_completa);
                    if (archivo_txt.is_open()) {
                        string linea;
                        while (getline(archivo_txt, linea)) {
                            cout << linea << "\n";
                        }
                        archivo_txt.close();
                    } else {
                        cout << "No se pudo leer el contenido del archivo.\n";
                    }
                    cout << "--------------------------------------------------\n\n";
                } else {
                    cout << "No es un archivo de texto (.txt).\n";
                    cout << "--------------------------------------------------\n\n";
                }
            }
        }
        closedir(direccion);
        
        if (!encontrada) {
            cout << "No se encontraron archivos que coincidan con: " << nombre_cancion << "\n\n";
        }
    } else {
        cout << "Error: No se pudo abrir el directorio especificado.\n\n";
    }
}
