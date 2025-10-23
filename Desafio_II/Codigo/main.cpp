#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

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
            archivo << nickname << "," << contrasena << "," << fecha << "," << pais << "," << membresia << "," << cambiar_membresia << endl;
            archivo << "------------------siguiente usuario------------------" << endl;
            archivo.close();
        } else {
            cerr << "No se pudo abrir el archivo para guardar el usuario." << endl;
        }
    }

    void setNickname(const string& nick) {
        nickname = nick;
    }
    string getNickname() const {
        return nickname;
    }
    void setContrasena(const string& pass) {
        contrasena = pass;
    }
    string getContrasena() const {
        return contrasena;
    }
    void setFecha(const string& date) {
        fecha = date;
    }
    string getFecha() const {
        return fecha;
    }
    void setPais(const string& country) {
        pais = country;
    }
    string getPais() const {
        return pais;
    }
    void setMembresia(bool member) {
        membresia = member;
    }
    bool getMembresia() const {
        return membresia;
    }
    void setCambiarMembresia(bool C_member) {
        cambiar_membresia = C_member;
    }
    bool getCambiarMembresia() const {
        return cambiar_membresia;
    }
};

string obtenerTipoMembresia(const string& linea) {
    stringstream ss(linea);
    string token;
    int campo = 0;
    
    // formato: nickname,contrasena,fecha,pais,membresia,cambiar_membresia
    while (getline(ss, token, ',')) {
        if (campo == 4) { // El campo de membresía es el quinto (índice 4)
            return (token == "1") ? "premium" : "regular";
        }
        campo++;
    }
    return "desconocido";
}

void mostrar_menu_principal() {
    cout << "\n=== MENU PRINCIPAL ===\n";
    cout << "[1] Crear usuario\n";
    cout << "[2] Ingresar con su usuario\n";
    cout << "[3] Cambiar membresia\n";
    cout << "[4] Salir\n";
    cout << "Ingrese una opcion: ";
}

void mostrar_menu_premium() {
    cout << "\n=== MENU PREMIUM ===\n";
    cout << "[1] Reproducir una cancion\n";
    cout << "[2] Agregar una cancion a lista de favoritos\n";
    cout << "[3] Ver su lista de favoritos\n";
    cout << "[4] Ver la lista de favoritos de alguien más\n";
    cout << "[5] Volver al menu principal\n";
    cout << "Ingrese una opcion: ";
}

int main() {
    int opcion;
    bool ejecutar = true;

    while (ejecutar) {
        mostrar_menu_principal();
        cin >> opcion;

        switch (opcion) {
        case 1: {
            string nick, pass, date, country;
            bool member, C_member;
            cout << "Ingrese su nickname: ";
            cin >> nick;
            cout << "Ingrese una contrasena: ";
            cin >> pass;
            cout << "Ingrese su fecha de nacimiento (DD/MM/AAAA): ";
            cin >> date;
            cout << "Ingrese su pais: ";
            cin >> country;
            cout << "¿Desea ser miembro premium? (1 para si, 0 para no): ";
            cin >> member;
            C_member = false;
            usuario nuevo_usuario(nick, pass, date, country, member, C_member);
            nuevo_usuario.guardar_usuario();
            cout << "Usuario creado exitosamente." << endl;
            break;
        }
        case 2: {
            string nick, pass;
            cout << "Ingrese su nickname: ";
            cin >> nick;
            cout << "Ingrese su contrasena: ";
            cin >> pass;
            ifstream archivo("usuarios.txt");
            bool encontrado = false;
            string linea_usuario;
            if (archivo.is_open()) {
                string linea;
                while (getline(archivo, linea)) {
                    size_t pos1 = linea.find(',');
                    size_t pos2 = linea.find(',', pos1 + 1);
                    if (pos1 != string::npos && pos2 != string::npos) {
                        string archivo_nick = linea.substr(0, pos1);
                        string archivo_pass = linea.substr(pos1 + 1, pos2 - pos1 - 1);
                        if (archivo_nick == nick && archivo_pass == pass) {
                            encontrado = true;
                            linea_usuario = linea;
                            break;
                        }
                    }
                }
                archivo.close();
                if (encontrado) {
                    cout << "Ingreso exitoso. Bienvenido, " << nick << "!" << endl;
                    string tipo_membresia = obtenerTipoMembresia(linea_usuario);
                    cout << "Tu tipo de membresía es: " << tipo_membresia << endl;
                    if (tipo_membresia == "premium") {
                        bool menu_premium_activo = true;
                        while (menu_premium_activo) {
                            mostrar_menu_premium();
                            int opcion_premium;
                            cin >> opcion_premium;
                            switch (opcion_premium) {
                                case 1: {
                                    cout << "Menu de opciones de reproduccion: " << endl;
                                    // Falta agreagar el codigo para la reproduccion de canciones
                                    break;
                                }
                                case 2: {
                                    cout << "Agregar cancion a favoritos: " << endl;
                                    // Falta el codigo para los favoritos
                                    break;
                                }
                                case 3: {
                                    cout << "Ver lista de favoritos: " << endl;
                                    // Así mismo para verlos
                                    break;
                                }
                                case 4: {
                                    cout << "Ver lista de favoritos de otro usuario: " << endl;
                                    // Y los de otros
                                    break;
                                }
                                case 5: {
                                    cout << "Saliendo del menu premium." << endl;
                                    menu_premium_activo = false;
                                    break;
                                }
                                default: {
                                    cout << "Opcion invalida. Intente de nuevo." << endl;
                                    break;
                                }
                            }
                        }
                    } else {
                        cout << "Usted es un miembro regular. Funcionalidades limitadas disponibles." << endl;
                        // Aquí lo de los miembros regulares y los anuncios
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
