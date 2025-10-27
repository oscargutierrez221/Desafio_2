#include <iostream>
#include <fstream> // Para el manejo de archivos
#include <string> // Para el manejo de strings
#include <ctime> // Para medir el tiempo de reproducción
#include <dirent.h> // Para el manejo de directorios
#include <windows.h> // Para la función Sleep
#include <cstdlib> // Para rand y srand


using namespace std;

// --- Enum para créditos ---
enum Creditos {
    CREDITO_GRATIS = 0,
    CREDITO_PREMIUM = 1,
    CREDITO_PREMIUM_PLUS = 2
};

// --- Estructura para canciones ---
struct Cancion {
    string nombre;
    string artista;
    int id;
    Creditos credito;
    int reproducciones;
    string ruta_archivo;
};

// --- Estructura para anuncios ---
struct Anuncio {
    string prioridad;
    string texto;
    int id;
    int peso; // Para el sistema de pesos (AAA= 3 veces mas que c, B= 2 veces más que c, C= Menor peso)
    int memoria_usada; // Para medir la memoria
};

// --- Variables globales que gestionan anuncios/canciones ---
Anuncio anuncios_disponibles[1000];
bool anuncios_mostrados[1000];
int total_anuncios = 0;
bool anuncios_cargados = false;
int contador_canciones = 0;

// --- Estructura para favoritos ---
struct FavoritosUsuario {
    string nickname;
    string canciones[10000]; // Array para las canciones de favoritos
    int total_canciones;
};

// --- Usamos el Arratya para los usuarios premium ---
FavoritosUsuario favoritos_premium[1000];
int total_usuarios_favoritos = 0;

// --- Clase de usuario ---
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

// --- Cargar los anuncios desde el archivo anuncios.txt ---
void cargar_anuncios() {
    if (anuncios_cargados) return;
    ifstream archivo("anuncios.txt");
    string linea;
    int contador_id = 0;
    total_anuncios = 0;
    
    // Inicializar arrays
    for (int i = 0; i < 1000; i++) {
        anuncios_mostrados[i] = false;
    }
    
    while (getline(archivo, linea) && total_anuncios < 1000) {
        if (linea.empty()) continue;
        size_t coma = linea.find(',');
        if (coma == string::npos) continue;
        
        string prioridad = linea.substr(0, coma);
        string texto = linea.substr(coma + 1);
        if (texto.size() > 500) texto = texto.substr(0, 500);
        
        int peso = 1;
        if (prioridad == "AAA") peso = 3;
        else if (prioridad == "B") peso = 2;
        
        // Calculamos la memoria usada
        int memoria_usada = (int)(prioridad.size() + texto.size() + sizeof(int) * 2);
        
        // Agregar según el peso
        for (int i = 0; i < peso && total_anuncios < 1000; i++) {
            anuncios_disponibles[total_anuncios].prioridad = prioridad;
            anuncios_disponibles[total_anuncios].texto = texto;
            anuncios_disponibles[total_anuncios].id = contador_id++;
            anuncios_disponibles[total_anuncios].peso = peso;
            anuncios_disponibles[total_anuncios].memoria_usada = memoria_usada;
            total_anuncios++;
        }
    }
    anuncios_cargados = true;
    archivo.close();
}

// -- Mostrar un anuncio aleatorio, evitando repeticiones --
void anuncios() {
    cargar_anuncios();
    if (total_anuncios == 0) return;
    
    // Buscar anuncios no mostrados
    int indices_disponibles[1000];
    int disponibles = 0;
    
    for (int i = 0; i < total_anuncios; i++) {
        if (!anuncios_mostrados[i]) {
            indices_disponibles[disponibles] = i;
            disponibles++;
        }
    }
    
    // Si no hay anuncios disponibles, resetear
    if (disponibles == 0) {
        for (int i = 0; i < total_anuncios; i++) {
            anuncios_mostrados[i] = false;
        }
        disponibles = total_anuncios;
        for (int i = 0; i < total_anuncios; i++) {
            indices_disponibles[i] = i;
        }
    }
    
    // Selección aleatoria usando time
    srand((unsigned int)time(NULL));
    int idx = rand() % disponibles;
    int anuncio_idx = indices_disponibles[idx];
    
    anuncios_mostrados[anuncio_idx] = true;
    
    cout << "\n--- ANUNCIO (" << anuncios_disponibles[anuncio_idx].prioridad << ") ---\n";
    cout << anuncios_disponibles[anuncio_idx].texto << "\n";
    cout << "Memoria usada: " << anuncios_disponibles[anuncio_idx].memoria_usada << " bytes\n";
    cout << "----------------\n";
}

// -- Generador de ID --
int generar_id_cancion() {
    srand((unsigned int)time(NULL));
    return 100000000 + (rand() % 900000000); // La ID de 9 diguitos estara entre 100000000 y 900000000
}

// -- Reproducción de la canción --
void reproducir_cancion(const string& nombre_cancion, bool premium = false) {
    int id_cancion = generar_id_cancion();
    cout << "La cancion: " << nombre_cancion << endl;
    cout << "ID de la cancion: " << id_cancion << endl;
    cout << "Reproduciendo la cancion: " << nombre_cancion << endl;
    
    time_t start = time(NULL);
    Sleep(3000);
    time_t end = time(NULL);
    double duration = difftime(end, start);
    
    cout << "Duracion: " << duration << " segundos de reproduccion." << endl;
    contador_canciones++;
    cout << "Total de canciones reproducidas: " << contador_canciones << endl;
    
    if (!premium && contador_canciones % 2 == 0) {
        anuncios();
    }
}

// -- Para guardar los favoritos en el archivo de texto --
void guardar_favoritos_premium() {
    ofstream archivo("favoritos.txt");
    if (archivo.is_open()) {
        for (int i = 0; i < total_usuarios_favoritos; i++) {
            archivo << favoritos_premium[i].nickname << ",";
            for (int j = 0; j < favoritos_premium[i].total_canciones; j++) {
                if (j > 0) archivo << "|";
                archivo << favoritos_premium[i].canciones[j];
            }
            archivo << endl;
        }
        archivo.close();
    } else {
        cerr << "No se pudo guardar favoritos premium." << endl;
    }
}

// -- Cargar favoritos premium al inicio --
void cargar_favoritos_premium() {
    ifstream archivo("favoritos.txt");
    if (!archivo.is_open()) return;
    
    total_usuarios_favoritos = 0;
    string linea;
    
    while (getline(archivo, linea) && total_usuarios_favoritos < 1000) {
        if (linea.empty()) continue;
        size_t coma = linea.find(',');
        if (coma == string::npos) continue;
        
        string nick = linea.substr(0, coma);
        string lista = linea.substr(coma + 1);
        
        favoritos_premium[total_usuarios_favoritos].nickname = nick;
        favoritos_premium[total_usuarios_favoritos].total_canciones = 0;
        
        size_t pos = 0, last = 0;
        while ((pos = lista.find('|', last)) != string::npos && 
               favoritos_premium[total_usuarios_favoritos].total_canciones < 10000) {
            favoritos_premium[total_usuarios_favoritos].canciones[favoritos_premium[total_usuarios_favoritos].total_canciones] = 
                lista.substr(last, pos - last);
            favoritos_premium[total_usuarios_favoritos].total_canciones++;
            last = pos + 1;
        }
        
        if (last < lista.size() && favoritos_premium[total_usuarios_favoritos].total_canciones < 10000) {
            favoritos_premium[total_usuarios_favoritos].canciones[favoritos_premium[total_usuarios_favoritos].total_canciones] = 
                lista.substr(last);
            favoritos_premium[total_usuarios_favoritos].total_canciones++;
        }
        
        total_usuarios_favoritos++;
    }
    archivo.close();
}

// -- Buscar usuario en favoritos --
int buscar_usuario_favoritos(const string& nickname) {
    for (int i = 0; i < total_usuarios_favoritos; i++) {
        if (favoritos_premium[i].nickname == nickname) {
            return i;
        }
    }
    return -1; // En caso de no encontrarlo
}

// -- Agregar canción a favoritos --
void agregar_favorito(const string& nickname, const string& cancion) {
    int idx = buscar_usuario_favoritos(nickname);
    
    if (idx == -1) {
        // Usuario nuevo
        if (total_usuarios_favoritos < 1000) {
            favoritos_premium[total_usuarios_favoritos].nickname = nickname;
            favoritos_premium[total_usuarios_favoritos].total_canciones = 0;
            idx = total_usuarios_favoritos;
            total_usuarios_favoritos++;
        } else {
            cout << "No se pueden agregar más usuarios de favoritos." << endl;
            return;
        }
    }
    
    // Verificar si la canción ya existe
    for (int i = 0; i < favoritos_premium[idx].total_canciones; i++) {
        if (favoritos_premium[idx].canciones[i] == cancion) {
            cout << "La canción ya está en tus favoritos." << endl;
            return;
        }
    }
    
    // Agregar canción si hay espacio
    if (favoritos_premium[idx].total_canciones < 10000) {
        favoritos_premium[idx].canciones[favoritos_premium[idx].total_canciones] = cancion;
        favoritos_premium[idx].total_canciones++;
        cout << "Canción agregada a favoritos!" << endl;
    } else {
        cout << "Lista de favoritos llena (máximo 10,000 canciones)." << endl;
    }
}

// Mostrar favoritos de un usuario
void mostrar_favoritos_usuario(const string& nickname) {
    int idx = buscar_usuario_favoritos(nickname);
    
    if (idx == -1 || favoritos_premium[idx].total_canciones == 0) {
        cout << "El usuario no tiene favoritos o no existe." << endl;
        return;
    }
    
    cout << "--- Favoritos de " << nickname << " ---" << endl;
    for (int i = 0; i < favoritos_premium[idx].total_canciones; i++) {
        cout << (i + 1) << ". " << favoritos_premium[idx].canciones[i] << endl;
    }
}

// -- Menú premium para usuarios premium --
void mostrar_menu_premium() {
    cout << "\n=== MENU PREMIUM ===\n";
    cout << "[1] Reproducir una cancion\n";
    cout << "[2] Agregar una cancion a lista de favoritos\n";
    cout << "[3] Ver su lista de favoritos\n";
    cout << "[4] Ver la lista de favoritos de alguien más\n";
    cout << "[5] Volver al menu principal\n";
    cout << "Ingrese una opcion: ";
}

// -- Menú estándar para los usuarios regulares --
void mostrar_menu_estandar() {
    cout << "\n=== MENU ESTANDAR (REGULAR) ===\n";
    cout << "[1] Reproducir una cancion\n";
    cout << "[2] Volver al menu principal\n";
    cout << "Ingrese una opcion: ";
}


// -- Para buscar las canciones en el directorio --
void buscar_canciones_en_directorio(const string& nombre_cancion, bool premium = false){
    string dir;
    DIR *direccion;
    struct dirent *elemento;
    
    // Asignar la ruta según el tipo de usuario
    if (premium) {
        dir = "C:\\Users\\dav98\\OneDrive\\Documentos\\Desafio_2\\Desafio_II\\Codigo\\Canciones_premium";
    } else {
        dir = "C:\\Users\\dav98\\OneDrive\\Documentos\\Desafio_2\\Desafio_II\\Codigo\\Canciones_regulares";
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
