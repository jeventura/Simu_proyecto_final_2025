#include <iostream>
#include <SFML/Graphics.hpp>
#include "model/PentagonType.hpp"
#include "model/Pentagono.hpp"
#include "graphics/PentagonDrawer.hpp"
#include "utils/JsonLoader.hpp"
#include "utils/GeometryUtils.hpp"
#include "utils/DialogBox.hpp"
#include "algorithm/bfs.hpp"
#include <string>

// Incluir filesystem solo si está disponible
#if defined(__cpp_lib_filesystem) || (defined(_MSC_VER) && _MSC_VER >= 1914) || \
    (defined(__GNUC__) && __GNUC__ >= 8) || (defined(__clang__) && __clang_major__ >= 7)
#include <filesystem>
#endif

// Includes para funciones del sistema operativo
#ifdef _WIN32
#include <windows.h>
#include <commdlg.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#include <cstdlib>
#endif

using namespace std;
using namespace sf;

Color getColorByType(PentagonType type) {
    switch(type) {
        case PentagonType::START:
            return Color::Green;
        case PentagonType::FINISH:
            return Color::Blue;
        case PentagonType::WALL:
            return Color::Magenta;
        case PentagonType::ELECTRIC_WALL:
            return Color::Red;
        case PentagonType::FREE:
            return Color(160, 160, 160);
        case PentagonType::SWITCH:
            return Color::Cyan;
        default:
            return Color(160, 160, 160);
    }
}

string showFileDialog() {
    string selectedFile = "";
    
    #ifdef _WIN32
    // Windows - usar GetOpenFileName
    char filename[MAX_PATH];
    
    OPENFILENAME ofn;
    ZeroMemory(&filename, sizeof(filename));
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = "Archivos JSON\0*.json\0Todos los archivos\0*.*\0";
    ofn.lpstrFile = filename;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(filename);
    ofn.lpstrFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    
    if (GetOpenFileName(&ofn)) {
        selectedFile = string(filename);
    }
    
    #else
    // Linux/macOS - usar zenity o kdialog
    // Primero intentamos zenity
    FILE* pipe = popen("zenity --file-selection --title='Seleccionar archivo JSON del laberinto' --file-filter='*.json' 2>/dev/null", "r");
    if (pipe) {
        char buffer[1024];
        if (fgets(buffer, sizeof(buffer), pipe) != NULL) {
            selectedFile = string(buffer);
            // Remover el salto de línea al final
            if (!selectedFile.empty() && selectedFile[selectedFile.length()-1] == '\n') {
                selectedFile.erase(selectedFile.length()-1);
            }
        }
        pclose(pipe);
    } else {
        // Si zenity no está disponible, intentar kdialog
        pipe = popen("kdialog --getopenfilename --title 'Seleccionar archivo JSON del laberinto' '*.json' 2>/dev/null", "r");
        if (pipe) {
            char buffer[1024];
            if (fgets(buffer, sizeof(buffer), pipe) != NULL) {
                selectedFile = string(buffer);
                // Remover el salto de línea al final
                if (!selectedFile.empty() && selectedFile[selectedFile.length()-1] == '\n') {
                    selectedFile.erase(selectedFile.length()-1);
                }
            }
            pclose(pipe);
        }
    }
    
    // Si no se pudo abrir el explorador, mostrar mensaje
    if (selectedFile.empty()) {
        cout << "No se pudo abrir el explorador de archivos." << endl;
        cout << "Por favor, instale zenity o kdialog en Linux, o use el archivo por defecto." << endl;
        cout << "Usando archivo por defecto: src/resources/map_creation.json" << endl;
        selectedFile = "src/resources/map_creation.json";
    }
    #endif
    
    return selectedFile;
}

string showFileSelector(RenderWindow& window, Font& font) {
    // Cerrar la ventana temporalmente para mostrar el diálogo de archivo
    window.setVisible(false);
    
    string selectedFile = showFileDialog();
    
    // Reabrir la ventana
    window.setVisible(true);
    
    return selectedFile;
}

int main() {
    RenderWindow window(VideoMode(800, 600), "Selector de Laberinto");
    window.setFramerateLimit(60);

    Font font;
    if (!font.loadFromFile("src/resources/roboto.ttf")) {
        cout << "Error: No se pudo cargar la fuente" << endl;
        return 1;
    }

    // Mostrar selector de archivo
    string filePath = showFileSelector(window, font);
    
    if (filePath.empty()) {
        return 0; // Usuario cerró la ventana
    }
    
    // Cerrar ventana de selección y crear ventana del juego
    window.close();
    
    RenderWindow gameWindow(VideoMode(1600, 1200), "Pentagon Grid");
    gameWindow.setFramerateLimit(60);

    int nodo_inicio;
    int nodo_fin;
    vector<int> solucion_bfs;

    DialogBox dialog(font, 600, 120, gameWindow);

    int selectedIndex = 0;

    map<Pentagono, bool> switches;

    vector<ConvexShape> pentagons;
    vector<vector<Vector2f>> verticesList;
    vector<Text> labels;

    vector<Vector2f> v1;
    auto p1 = PentagonDrawer::createInitialPentagon(725.f, 400.f, 0.f, v1);
    pentagons.push_back(p1);
    verticesList.push_back(v1);

    vector<Pentagono> pentagonos;
    int steps;
    if(!loadDataFromJson(filePath, pentagonos, steps)) {
        // Mostrar error en una ventana gráfica
        RenderWindow errorWindow(VideoMode(600, 200), "Error");
        errorWindow.setFramerateLimit(60);
        
        Text errorText("Error: No se pudo cargar el archivo del laberinto\n" + filePath, font, 20);
        errorText.setFillColor(Color::Red);
        errorText.setPosition(50, 50);
        
        Text instructionText("Presione cualquier tecla para salir", font, 16);
        instructionText.setFillColor(Color::Black);
        instructionText.setPosition(50, 120);
        
        while (errorWindow.isOpen()) {
            Event event;
            while (errorWindow.pollEvent(event)) {
                if (event.type == Event::Closed || 
                    (event.type == Event::KeyPressed)) {
                    errorWindow.close();
                }
            }
            
            errorWindow.clear(Color::White);
            errorWindow.draw(errorText);
            errorWindow.draw(instructionText);
            errorWindow.display();
        }
        return 1;
    }

    Vector2f center(0.f, 0.f);
    for (auto& p : v1) center += p;
    center /= 5.f;
    Text label("1", font, 24), stepsTitle("Pasos restantes: " + to_string(steps), font, 24);
    FloatRect bounds = stepsTitle.getLocalBounds();

    label.setFillColor(Color::Black);
    label.setPosition(center.x - 6, center.y - 12);
    labels.push_back(label);

    stepsTitle.setFillColor(Color::Black);
    stepsTitle.setOrigin(bounds.left + bounds.width, bounds.top);
    stepsTitle.setPosition(gameWindow.getSize().x - 10, 10);

    int startType = returnPentagonTypeAsInt(PentagonType::START);
    int endType = returnPentagonTypeAsInt(PentagonType::FINISH);
    int switchType = returnPentagonTypeAsInt(PentagonType::SWITCH);
    int wallType = returnPentagonTypeAsInt(PentagonType::WALL);
    int electricWallType = returnPentagonTypeAsInt(PentagonType::ELECTRIC_WALL);
    for (int i = 0; i < pentagonos.size(); i++) {
        const Pentagono& p = pentagonos[i];

        // Encontramos el pentagono inicial y lo coloreamos
        if(p.type == startType) {
            selectedIndex = i;
            pentagons[i].setFillColor(i == selectedIndex ? Color::Yellow : Color(160, 160, 160));
        } else if (p.type == switchType) {
            switches[p] = false;
        }
        PentagonDrawer::addConnectedPentagon(p.base_index, p.vertice1, p.vertice2, font, pentagons, verticesList, labels);
    }

    int totalPentagonos = pentagons.size();
    vector<vector<int>> adjacencyList(totalPentagonos);

    int currentIndex = 1; // el primer pentágono es el 0
    for (const auto& p : pentagonos) {
        int base = p.base_index;
        int connected = currentIndex++;

        // Asegúrate de no acceder fuera del rango
        if (base < adjacencyList.size() && connected < adjacencyList.size()) {
            if (p.type == startType) {
                nodo_inicio = connected;
            } else if (p.type == endType) {
                nodo_fin = connected;
            }

            adjacencyList[base].push_back(connected);
            adjacencyList[connected].push_back(base);
        }
    }

    for (size_t i = 0; i < verticesList.size(); ++i) {
        for (size_t j = i + 1; j < verticesList.size(); ++j) {
            if (countSharedVertices(verticesList[i], verticesList[j]) >= 2) {
                // Verifica si ya están conectados
                if (find(adjacencyList[i].begin(), adjacencyList[i].end(), j) == adjacencyList[i].end()) {
                    adjacencyList[i].push_back(j);
                    adjacencyList[j].push_back(i);
                }
            }
        }
    }

    while (gameWindow.isOpen()) {
        Event event;
        while (gameWindow.pollEvent(event)) {
            if (event.type == Event::Closed)
                gameWindow.close();

            // Enter o Espacio
            if (event.type == Event::KeyPressed &&
            (event.key.code == Keyboard::Enter || event.key.code == Keyboard::Space)) {
                if (dialog.isVisible()) {
                    dialog.dismiss();
                }
            }

            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                Vector2f mousePos = gameWindow.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});

                // Clic en botón
                if (dialog.isButtonClicked(mousePos)) {
                        dialog.dismiss();
                        return 1;
                    }

                if(!dialog.isVisible()) {
                    for (size_t i = 0; i < verticesList.size(); ++i) {
                        if (isPointInsidePolygon(verticesList[i], mousePos)) {

                            bool isWall = pentagonos[i].type == wallType;
                            bool isSwitch = pentagonos[i].type == switchType;
                            bool areSwitchesOpen, isElectricWall;

                            if(isSwitch) {
                                switches[pentagonos[i]] = !switches[pentagonos[i]];
                                areSwitchesOpen = all_of(
                                    switches.begin(),
                                    switches.end(),
                                    [](const pair<Pentagono, bool>& pair) {
                                        return pair.second;
                                    }
                                );
                            }

                            isElectricWall = pentagonos[i].type == electricWallType && !areSwitchesOpen;

                            // Solo permitir cambio si i es adyacente a selectedIndex o es el mismo pentágono y si no es un wall o electric wall
                            if (
                                (i == selectedIndex || 
                                find(adjacencyList[selectedIndex].begin(), adjacencyList[selectedIndex].end(), i) != adjacencyList[selectedIndex].end()) &&
                                !isWall && !isElectricWall
                            ) {
                                selectedIndex = i;
                                steps--;
                                if(pentagonos[selectedIndex].type == endType && steps >= 0) {
                                    dialog.setText("Ganaste!");
                                    dialog.show();
                                } else if(pentagonos[selectedIndex].type != endType && steps <= 0) {
                                    dialog.setText("Te quedaste sin movimiento!");
                                    dialog.show();
                                }
                                cout << "Steps: " << steps << endl;
                                break;
                            }
                        }
                    }
                }
                // Actualizar colores
                for (size_t i = 0; i < pentagons.size(); ++i) {
                    pentagons[i].setFillColor(i == selectedIndex ? Color::Yellow : Color(160, 160, 160));
                }
            }
        }

        gameWindow.clear(Color::White);
        for (size_t i = 0; i < pentagons.size(); ++i) {
            if(i != selectedIndex) {
                PentagonType type = static_cast<PentagonType>(pentagonos[i].type);
                Color color = getColorByType(type);
                pentagons[i].setFillColor(color);
            }
        
            gameWindow.draw(pentagons[i]);
            gameWindow.draw(labels[i]);

            for (int j = 0; j < 5; ++j) {
                Vector2f v = verticesList[i][j];
                Text vLabel(to_string(j), font, 14);
                vLabel.setFillColor(Color::Red);
                vLabel.setPosition(v.x - 5, v.y - 5);
                gameWindow.draw(vLabel);
            }
        }

        dialog.draw(gameWindow);

        // Actualziar contador de pasos
        stepsTitle.setString("Pasos restantes: " + to_string(steps));
        gameWindow.draw(stepsTitle);
        gameWindow.display();

        if (Keyboard::isKeyPressed(Keyboard::R)) {
            solucion_bfs = findShortestPath(nodo_inicio, nodo_fin,
                                                adjacencyList, pentagonos, switches);
        }

    }

    // Imprimir la lista de adyacencia

    /*
    for (int i = 0; i < adjacencyList.size(); ++i) {
        cout << "Pentágono " << i + 1 << " está conectado con: ";
        for (int j = 0; j < adjacencyList[i].size(); ++j) {
            cout << adjacencyList[i][j] + 1<< " ";
        }
        cout << endl;
    }
    */

    //Imprimir solucion

    for (int index : solucion_bfs) {
        cout << "Pentágono " << index << endl;
    }

    return 0;
}
