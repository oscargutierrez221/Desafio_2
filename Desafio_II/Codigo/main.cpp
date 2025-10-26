#include <iostream>
#include <fstream> // Para el manejo de archivos
#include <string> // Para el manejo de strings
#include <ctime> // Para medir el tiempo de reproducción
#include <windows.h> // Para la función Sleep
#include <unordered_map> // Para el mapa de favoritos
#include "clases.h"


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
            cout << "Ingrese su nickname: ";
            cin >> nick;
            cout << "Ingrese una contrasena: ";
            cin >> pass;
            cout << "Ingrese su fecha de nacimiento (DD/MM/AAAA): ";
            cin >> date;
            cout << "Ingrese su pais: "; cin >> country;
            cout << "¿Desea ser miembro premium? (1 para si, 0 para no): ";
            cin >> member; // -- Recibir datos de usuario para registro --
            C_member = false;
            usuario nuevo_usuario(nick, pass, date, country, member, C_member);
            nuevo_usuario.guardar_usuario(); // -- Guardar usuario usando la función --
            cout << "Usuario creado exitosamente." << endl;
            break;
        }
        case 2: {
            string nick, pass;
            cout << "Ingrese su nickname: ";
            cin >> nick;
            cout << "Ingrese su contrasena: ";
            cin >> pass; // -- Nuestro login --
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
                                cout << "\n¿Desea buscar esta cancion en un directorio? (s/n): ";
                                char respuesta;
                                cin >> respuesta;
                                cin.ignore();
                                if (respuesta == 's' || respuesta == 'S') {
                                    buscar_canciones_en_directorio(nombre_cancion, true);
                                }
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
                            int opcion_estandar;
                            cin >> opcion_estandar;
                            cin.ignore();
                            switch (opcion_estandar) {
                            case 1: {
                                string nombre_cancion;
                                cout << "Ingrese el nombre de la cancion a reproducir: ";
                                getline(cin, nombre_cancion);
                                reproducir_cancion(nombre_cancion, false);
                                cout << "\n¿Desea ver la informacion de esta cancion? (s/n): ";
                                char respuesta;
                                cin >> respuesta;
                                cin.ignore();
                                if (respuesta == 's' || respuesta == 'S') {
                                    buscar_canciones_en_directorio(nombre_cancion, false);
                                }
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
