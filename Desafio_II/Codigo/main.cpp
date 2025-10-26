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

int main() {
    cargar_favoritos_premium(); // Cargar las listas de favoritos al inicio para poder acceder a ellas
    int opcion;
    bool ejecutar = true;

    while (ejecutar) {
        cout << "\n=== MENU PRINCIPAL ===\n"; // -- Menú principal --
        cout << "[1] Registrarse\n";
        cout << "[2] Ingresar\n";
        cout << "[3] Cambiar membresia (en desarrollo)\n";
        cout << "[4] Salir\n";
        cout << "Ingrese una opcion: ";
        cin >> opcion;
        switch (opcion) {
        case 1: {
            string nick, pass, date, country;
            bool member, C_member;
            cout << "Ingrese su nickname: "; cin >> nick;
            cout << "Ingrese una contrasena: "; cin >> pass;
            cout << "Ingrese su fecha de nacimiento (DD/MM/AAAA): "; cin >> date;
            cout << "Ingrese su pais: "; cin >> country;
            cout << "¿Desea ser miembro premium? (1 para si, 0 para no): "; cin >> member; // -- Recibir datos de usuario para registro --
            C_member = false;
            usuario nuevo_usuario(nick, pass, date, country, member, C_member);
            nuevo_usuario.guardar_usuario(); // -- Guardar usuario usando la función --
            cout << "Usuario creado exitosamente." << endl;
            break;
        }
        case 2: {
            string nick, pass;
            cout << "Ingrese su nickname: "; cin >> nick;
            cout << "Ingrese su contrasena: "; cin >> pass; // -- Nuestro login --
            ifstream archivo("usuarios.txt");
            bool encontrado = false;
            string linea_usuario;
            if (archivo.is_open()) {
                string linea;
                while (getline(archivo, linea)) {
                    if (linea.find("------------------") == string::npos) {
                        size_t pos1 = linea.find(',');
                        size_t pos2 = linea.find(',', pos1 + 1);
                        if (pos1 != string::npos && pos2 != string::npos) {
                            string archivo_nick = linea.substr(0, pos1);
                            string archivo_pass = linea.substr(pos1 + 1, pos2 - pos1 - 1);
                            if (archivo_nick == nick && archivo_pass == pass) {
                                encontrado = true;
                                linea_usuario = linea; // Identificar datos del usuario logueado
                                break;
                            }
                        }
                    }
                }
                archivo.close();

                if (encontrado) {
                    cout << "Ingreso exitoso. Bienvenido, " << nick << "!" << endl;
                    string tipo_membresia = obtenerTipoMembresia(linea_usuario);
                    cout << "Tu tipo de membresia es: " << tipo_membresia << endl;

                    if (tipo_membresia == "premium") { // -- De a cuerdo a los datos del loguin, será el menú mostrado --
                        bool menu_premium_activo = true;
                        while (menu_premium_activo) {
                            mostrar_menu_premium();
                            int opcion_premium; cin >> opcion_premium;
                            cin.ignore();  // -- Limpiar buffer para getline --
                            switch (opcion_premium) {
                                case 1: { // -- Reproducir canción --
                                    string nombre_cancion;
                                    cout << "Ingrese el nombre de la cancion a reproducir: ";
                                    getline(cin, nombre_cancion);
                                    reproducir_cancion(nombre_cancion, true);
                                    break;
                                }
                                case 2: { // -- Agregar favoritos --
                                    string nombre_cancion;
                                    cout << "Ingrese el nombre de la cancion para agregar a favoritos: ";
                                    getline(cin, nombre_cancion);
                                    favoritos_premium[nick].insert(nombre_cancion);
                                    guardar_favoritos_premium();
                                    cout << "Canción agregada a favoritos!" << endl;
                                    break;
                                }
                                case 3: { // -- Ver favoritos propios --
                                    cout << "--- Tus favoritos ---" << endl;
                                    if (!favoritos_premium.count(nick) || favoritos_premium[nick].empty()) {
                                        cout << "<Vacio>" << endl;
                                    } else {
                                        for (const auto& cancion : favoritos_premium[nick]) cout << cancion << endl;
                                    }
                                    break;
                                }
                                case 4: { // -- Ver los favoritos de otro usuario premium --
                                    string nombre_otro;
                                    cout << "Ingrese el nickname del usuario: ";
                                    getline(cin, nombre_otro);
                                    if (!favoritos_premium.count(nombre_otro) || favoritos_premium[nombre_otro].empty()) {
                                        cout << "El usuario no tiene favoritos o no existe (en esta sesion)." << endl;
                                    } else {
                                        cout << "--- Favoritos de " << nombre_otro << " ---" << endl;
                                        for (const auto& cancion : favoritos_premium[nombre_otro]) cout << cancion << endl;
                                    }
                                    break;
                                }
                                case 5:
                                    menu_premium_activo = false;
                                    break;
                                default:
                                    cout << "Opción no implementada o inválida en menú premium." << endl;
                                    break;
                            }
                        }
                    } else if (tipo_membresia == "regular") { // -- En caso de que los datos de loguien sean de un usuario regular --
                        bool menu_estandar_activo = true;
                        while (menu_estandar_activo) {
                            mostrar_menu_estandar();
                            int opcion_estandar; cin >> opcion_estandar;
                            cin.ignore();
                            switch (opcion_estandar) {
                                case 1: {
                                    string nombre_cancion;
                                    cout << "Ingrese el nombre de la cancion a reproducir: ";
                                    getline(cin, nombre_cancion);
                                    reproducir_cancion(nombre_cancion, false);
                                    break;
                                }
                                case 2:
                                    menu_estandar_activo = false;
                                    break;
                                default:
                                    cout << "Opcion invalida en menu estandar." << endl;
                                    break;
                            }
                        }
                    } else {
                        cout << "Hubo un error identificando su membresía. Contacte soporte." << endl;
                    }
                } else {
                    cout << "Nickname o contrasena incorrectos." << endl;
                }
            } else {
                cerr << "No se pudo abrir el archivo de usuarios." << endl;
            }
            break;
        }
        case 3: {
            cout << "Funcionalidad para cambiar membresia en desarrollo." << endl;
            break;
        }
        case 4: {
            cout << "Saliendo del programa. ¡Hasta luego!" << endl;
            ejecutar = false;
            break;
        }
        default: {
            cout << "Opcion invalida. Intente de nuevo." << endl;
            break;
        }
        }
    }
    return 0;
}
